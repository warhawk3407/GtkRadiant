/*
Copyright (C) 1999-2007 id Software, Inc. and contributors.
For a list of contributors, see the accompanying CONTRIBUTORS file.

This file is part of GtkRadiant.

GtkRadiant is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

GtkRadiant is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GtkRadiant; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

//
// User preferences
//
// Leonardo Zide (leo@lokigames.com)
//

#include "stdafx.h"
#include <glib.h>
#include <assert.h>
#if defined (__linux__) || defined (__APPLE__)
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#endif
#include "missing.h"
#include "gtkmisc.h"

#ifdef _WIN32
#include <io.h>
#define X_OK 0
#include <sys/stat.h>
#endif

#define PREF_SECTION            "Prefs"
#define INTERNAL_SECTION        "Internals"
#define MOUSE_KEY               "MouseButtons"
#define WINDOW_KEY              "QE4StyleWindows"
#define LAYOUT_KEY              "WindowLayout"
#define RUNQ2_KEY               "RunQuake2Run"
#define TLOCK_KEY               "TextureLock"
#define RLOCK_KEY               "RotateLock"
#define LOADLAST_KEY            "LoadLast"
#define LOADLASTMAP_KEY         "LoadLastMap"
#define LASTPROJ_KEY            "LastProject"
#define LASTPROJVER_KEY         "LastProjectKey"
#define LASTMAP_KEY             "LastMap"
#define FACE_KEY                "NewFaceGrab"
#define BSP_KEY                 "InternalBSP"
#define RCLICK_KEY              "NewRightClick"
#define VERTEX_KEY              "NewVertex"
#define AUTOSAVE_KEY            "Autosave"
#define AUTOSAVETIME_KEY        "AutosaveMinutes"
#define PAK_KEY                 "UsePAK"
#define NEWAPPLY_KEY            "ApplyDismissesSurface"
#define HACK_KEY                "Gatewayescapehack"
#define TEXTURE_KEY             "NewTextureWindowStuff"
#define TINYBRUSH_KEY           "CleanTinyBrushes"
#define TINYSIZE_KEY            "CleanTinyBrusheSize"
#define SNAPSHOT_KEY            "Snapshots"
#define MOVESPEED_KEY           "MoveSpeed"
#define ANGLESPEED_KEY          "AngleSpeed"
#define SETGAME_KEY             "UseSetGame"
#define CAMXYUPDATE_KEY         "CamXYUpdate"
#define CAMDRAGMULTISELECT_KEY  "CamDragMultiSelect"
#define CAMFREELOOK_KEY         "CamFreeLook"
#define CAMINVERSEMOUSE_KEY	    "CamInverseMouse"
#define CAMDISCRETE_KEY         "CamDiscrete"
#define LIGHTDRAW_KEY           "NewLightStyle"
#define WHATGAME_KEY            "WhichGame"
#define CUBICCLIP_KEY           "CubicClipping"
#define CUBICSCALE_KEY          "CubicScale"
#define ALTEDGE_KEY             "ALTEdgeDrag"
#define FACECOLORS_KEY          "FaceColors"
#define SNAPT_KEY               "SnapT"
#define XZVIS_KEY               "XZVIS"
#define YZVIS_KEY               "YZVIS"
#define ZVIS_KEY                "ZVIS"
#define SIZEPAINT_KEY           "SizePainting"
#define DLLENTITIES_KEY         "DLLEntities"
#define DETACHABLEMENUS_KEY     "DetachableMenus"
#define PATCHTOOLBAR_KEY        "PatchToolBar"
#define WIDETOOLBAR_KEY         "WideToolBar"
#define PLUGINTOOLBAR_KEY "PluginToolBar"
#define NOCLAMP_KEY             "NoClamp"
#define PREFAB_KEY              "PrefabPath"
#define USERINI_KEY             "UserINIPath"
#define ROTATION_KEY            "Rotation"
#define BUGGYICD_KEY            "BuggyICD"
#define CHASEMOUSE_KEY          "ChaseMouse"
#define ENTITYSHOW_KEY          "EntityShow"
#define TEXTURESCALE_KEY        "TextureScale"
#define TEXTURESCROLLBAR_KEY    "TextureScrollbar"
#define DISPLAYLISTS_KEY        "UseDisplayLists"
#define ANTIALIASEDLINES_KEY    "UseAntialiasedPointsAndLines" // Fishman - Add antialiazed points and lines support. 09/03/00
#define NORMALIZECOLORS_KEY     "NormalizeColors"
#define SHADERS_KEY             "UseShaders"
#define SWITCHCLIP_KEY          "SwitchClipKey"
#define SELWHOLEENTS_KEY		    "SelectWholeEntitiesKey"
#define TEXTURESUBSET_KEY	  	  "UseTextureSubsetLoading"
#define TEXTUREQUALITY_KEY 	    "TextureQuality"
#define SHOWSHADERS_KEY 	      "ShowShaders"
#define SHADERTEST_KEY 	        "ShaderTest"
#define GLLIGHTING_KEY          "UseGLLighting"
#define LOADSHADERS_KEY         "LoadShaders"
#define NOSTIPPLE_KEY           "NoStipple"
#define UNDOLEVELS_KEY          "UndoLevels"
#define VERTEXMODE_KEY          "VertexSplit"
#define ENGINEPATH_KEY          "EnginePath"
#define ENGINE_KEY              "Engine"
#define LOGCONSOLE_KEY          "LogConsole"
#define SELECTCURVES_KEY        "SelectCurves"
#define SELECTMODELS_KEY        "SelectModels"
#define SHADERLISTONLY_KEY      "ShowShaderlistOnly"
#define WATCHBSP_KEY            "WatchBSP"
#define LEAKSTOP_KEY            "LeakStop"
#define DOSLEEP_KEY             "SleepMode"
#define SUBDIVISIONS_KEY        "Subdivisions"
#define CLIPCAULK_KEY           "ClipCaulk"
#define PATCHSHOWBOUNDS_KEY     "PatchShowBounds"
#define NATIVEGUI_KEY           "NativeGUI"
#define STARTONPRIMMON_KEY      "StartOnPrimMon"
#define NOSYSMENUPOPUPS_KEY     "NoSysMenuPopups"
#define SNAPTTOGRID_KEY         "SnapTToGrid"
#define FLOATINGZ_KEY           "FloatingZ"
#define TARGETFIX_KEY           "TargetFix"
#define GLPOINTWORKAROUND_KEY   "GlPointWorkaround"		// Gef: Workaround for broken Kyro * gl driver 25-aug-2001
#define WHEELINC_KEY            "WheelMouseInc"
#define PATCHBBOXSEL_KEY        "PatchBBoxSel"
#define LASTLIGHTINTENSITY_KEY  "LastLightIntensity"
#define CUSTOMSHADEREDITOR_KEY        "UseCustomShaderEditor"
#define CUSTOMSHADEREDITORCOMMAND_KEY "CustomShaderEditorCommand"
#define TEXTURECOMPRESSIONFORMAT_KEY "TextureCompressionFormat"
#define LIGHTRADIUS_KEY "LightRadiuses"
#define Q3MAP2TEX_KEY "Q3Map2Tex"
#define ATIHACK_KEY "ATIHack"

// window stuff
#define ENTITYSPLIT1_KEY  "EntitySplit1"
#define ENTITYSPLIT2_KEY  "EntitySplit2"
#define POSITIONX_KEY     "PositionX"
#define POSITIONY_KEY     "PositionY"
#define ENTITYWND_KEY     "EntityWnd"
#define MAPINFOWND_KEY    "MapInfoDlg"
#define CAMWND_KEY        "CamWnd"
#define ZWND_KEY          "ZWnd"
#define XYWND_KEY         "XYWnd"
#define XZWND_KEY         "XZWnd"
#define YZWND_KEY         "YZWnd"
#define PATCHWND_KEY      "PatchWnd"
#define SURFACEWND_KEY    "SurfaceWnd"
#define ENTITYINFOWND_KEY "EntityInfoDlg"
#define WIDTH_KEY         "Width"
#define HEIGHT_KEY        "Height"
#define ZWIDTH_KEY        "ZWidth"
#define XYHEIGHT_KEY      "XYHeight"
#define XYWIDTH_KEY       "XYWidth"
#define CAMWIDTH_KEY      "CamWidth"
#define CAMHEIGHT_KEY     "CamHeight"
#define ZFLOATWIDTH_KEY   "ZWidthFloating"
#define STATE_KEY         "State"

// menu stuff
#define COUNT_KEY         "Count"
#define FILE_KEY          "File"

//saved info
#define SI_TEXMENU_KEY          "SI_TexMenu"
#define SI_GAMMA_KEY            "SI_Gamma"
#define SI_COLORS_KEY           "SI_Colors"
#define SI_EXCLUDE_KEY          "SI_Exclude"
#define SI_INCLUDE_KEY          "SI_Include"
#define SI_SURFACE_TEXDEF_KEY   "SI_SurfaceTexdef"
#define SI_PATCH_TEXDEF_KEY     "SI_PatchTexdef"
#define SI_AXISCOLORS_KEY       "SI_AxisColors"
#define SI_SHOWNAMES_KEY        "SI_ShowNames"
#define SI_SHOWCOORDS_KEY       "SI_ShowCoords"
#define SI_SHOWANGLES_KEY       "SI_ShowAngles"
#define SI_SHOWOUTLINES_KEY     "SI_ShowOutlines"
#define SI_SHOWAXIS_KEY         "SI_ShowAxis"
#define SI_NOSELOUTLINES_KEY    "SI_NoSelectedOutlines"
#define SI_OUTLINESTYLE_KEY "SI_OutLineStyle"

//for texdefs
#define TD_SCALE1_KEY           "_Scale1"
#define TD_SCALE2_KEY           "_Scale2"
#define TD_SHIFT1_KEY           "_Shift1"
#define TD_SHIFT2_KEY           "_Shift2"
#define TD_ROTATE_KEY           "_Rotate"

#define MOUSE_DEF 1
#define WINDOW_DEF 0
#define RUNQ2_DEF 0
#define WATCHBSP_DEF 1
#define TLOCK_DEF 1
#define LOADLAST_DEF 1
#define RUN_DEF 0
#define SUBDIVISIONS_DEF 4

void WindowPosition_Parse(window_position_t& m_value, const CString& value)
{
  if(sscanf(value.GetBuffer(), "%d %d %d %d", &m_value.x, &m_value.y, &m_value.w, &m_value.h) != 4)
    m_value.x = m_value.y = m_value.w = m_value.h = -1;
}

void WindowPosition_Write(const window_position_t& m_value, CString& value)
{
  char buffer[64];
  sprintf(buffer, "%d %d %d %d", m_value.x, m_value.y, m_value.w, m_value.h);
  value = buffer;
}


CXMLPropertyBag::CXMLPropertyBag() {
  mStrFilename = "";
  mpDoc = NULL;
  mbEmpty = false;
}

// generic preference functions

void CXMLPropertyBag::PushAssignment(char *name, PrefTypes_t type, void *pV)
{
  list<CPrefAssignment>::iterator iAssign;
  for(iAssign=mPrefAssignments.begin(); iAssign!=mPrefAssignments.end(); iAssign++)
  {
    if ((*iAssign).mName == name)
    {
      // we have it already, check anyway
      if (pV != (*iAssign).mVal)
      {
        Sys_FPrintf(SYS_ERR, "PushAssignment, '%s' has different mVal\n", name);
        return;
      }
    }
  }
  // ok, it's not in our list yet
  mPrefAssignments.push_front(CPrefAssignment(name, type, pV));
}

xmlNodePtr CXMLPropertyBag::EpairForName(const char *name)
{
  xmlNodePtr ret = NULL;

  xmlNodePtr pNode = mpDocNode->children;
  while (pNode != NULL)
  {
    if(pNode->type == XML_ELEMENT_NODE)
    {
      xmlAttrPtr tmp_attr_ptr = xmlHasProp(pNode, (xmlChar *)"name");
      if (tmp_attr_ptr != NULL && !strcmp(name, (char *)tmp_attr_ptr->children->content))
      {
        if ( ret ) {
          Sys_FPrintf( SYS_WRN, "WARNING: dupe property in CXMLPropertyBag::EpairForName '%s'\n", name );
        } else {
          ret = pNode;
        }
      }
    }
    pNode = pNode->next;
  }
  return ret;
}

void CXMLPropertyBag::GetPref(char *name, Str *pV, char *V)
{
  xmlNodePtr pNode = EpairForName( name );
  if ( pNode )
  {
    if ( pNode->children && pNode->children->content ) {
      *pV = pNode->children->content;
    } else {
      // http://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=427
      // means the pref exists, and that the value is ""
      *pV = "";
    }
  }
  else
  {
    pNode = xmlNewChild(mpDocNode, NULL, (xmlChar *)"epair", (xmlChar *)V);
    xmlSetProp(pNode, (xmlChar *)"name", (xmlChar *)name);
  }
  // push the pref assignment if needed
  PushAssignment(name, PREF_STR, pV);
}

void CXMLPropertyBag::GetPref(char *name, int *pV, int V)
{
  xmlNodePtr pNode;  
  if ((pNode = EpairForName(name)) && pNode->children && pNode->children->content)
  {
    *pV = atoi((char *)pNode->children->content);
  }
  else
  {
    char s[10];
    sprintf(s, "%d", V);
    pNode = xmlNewChild(mpDocNode, NULL, (xmlChar *)"epair", (xmlChar *)s);
    xmlSetProp(pNode, (xmlChar *)"name", (xmlChar *)name);
    *pV=V;
  }
  // push the pref assignment if needed
  PushAssignment(name, PREF_INT, pV); 
}

void CXMLPropertyBag::GetPref(char *name, bool *pV, bool V)
{
  xmlNodePtr pNode;  
  if ((pNode = EpairForName(name)) && pNode->children && pNode->children->content)
  {
    if (!strcmp((char *)pNode->children->content, "true"))
    {
      *pV = true;
    }
    else
    {
      *pV = false;
    }
  }
  else
  {
    char s[10];
    V ? strcpy(s, "true") : strcpy(s, "false");
    pNode = xmlNewChild(mpDocNode, NULL, (xmlChar *)"epair", (xmlChar *)s);
    xmlSetProp(pNode, (xmlChar *)"name", (xmlChar *)name);
    *pV=V;
  }
  // push the pref assignment
  PushAssignment(name, PREF_BOOL, pV); 
}

void CXMLPropertyBag::GetPref(char *name, float *pV, float V)
{
  xmlNodePtr pNode;  
  if ((pNode = EpairForName(name)) && pNode->children && pNode->children->content)
  {
    *pV = atof((char *)pNode->children->content);
  }
  else
  {
    char s[10];
    sprintf(s, "%f", V);
    pNode = xmlNewChild(mpDocNode, NULL, (xmlChar *)"epair", (xmlChar *)s);
    xmlSetProp(pNode, (xmlChar *)"name", (xmlChar *)name);
    *pV=V;
  }
  // push the pref assignment if needed
  PushAssignment(name, PREF_FLOAT, pV); 
}

void CXMLPropertyBag::GetPref(char *name, float* pV, float* V)
{
  xmlNodePtr pNode;  
  if ((pNode = EpairForName(name)) && pNode->children && pNode->children->content)
  {
    sscanf((char *)pNode->children->content, "%f %f %f", &pV[0], &pV[1], &pV[2]);
  }
  else
  {
    char s[128];
    sprintf(s, "%f %f %f", V[0], V[1], V[2]);
    pNode = xmlNewChild(mpDocNode, NULL, (xmlChar *)"epair", (xmlChar *)s);
    xmlSetProp(pNode, (xmlChar *)"name", (xmlChar *)name);
    pV[0] = V[0];
    pV[1] = V[1];
    pV[2] = V[2];
  }
  // push the pref assignment if needed
  PushAssignment(name, PREF_VEC3, pV); 
}

void CXMLPropertyBag::GetPref(char *name, window_position_t* pV, window_position_t V)
{
  xmlNodePtr pNode;  
  if ((pNode = EpairForName(name)) && pNode->children && pNode->children->content)
  {
    WindowPosition_Parse(*pV, CString((xmlChar *)pNode->children->content));
  }
  else
  {
    CString str;
    WindowPosition_Write(V, str);
    pNode = xmlNewChild(mpDocNode, NULL, (xmlChar *)"epair", (xmlChar *)str.GetBuffer());
    xmlSetProp(pNode, (xmlChar *)"name", (xmlChar *)name);
    *pV = V;
  }
  // push the pref assignment if needed
  PushAssignment(name, PREF_WNDPOS, pV); 
}

void CXMLPropertyBag::UpdatePrefTree()
{
  // read the assignments and update the tree
  list<CPrefAssignment>::iterator iPref;
  for(iPref = mPrefAssignments.begin(); iPref != mPrefAssignments.end(); iPref++)
  {
    CPrefAssignment *pPref = &(*iPref);
    // look for the node
    xmlNodePtr pNode;
    char s[64];

    pNode = EpairForName(pPref->mName.GetBuffer());
    // we never expect that the node could not be found, because this is supposed to happen
    // after the tree was built with GetPref calls, never on a blank tree
    if (!pNode)
    {
      Sys_FPrintf(SYS_ERR, "Unexpected EpairForName '%s' not found in UpdatePrefTree\n", pPref->mName.GetBuffer());
      return;
    }
    switch ((*iPref).mType)
    {
    case PREF_STR:
      xmlNodeSetContent(pNode, (const xmlChar *)((Str *)pPref->mVal)->GetBuffer());
      break;
    case PREF_INT:
      sprintf(s, "%d", *(int *)pPref->mVal);
      xmlNodeSetContent(pNode, (xmlChar *)s);
      break;
    case PREF_FLOAT:
      sprintf(s, "%f", *(float *)pPref->mVal);
      xmlNodeSetContent(pNode, (xmlChar *)s);
      break;
    case PREF_BOOL:
      *(bool *)pPref->mVal ? strcpy(s, "true") : strcpy(s, "false");
      xmlNodeSetContent(pNode, (xmlChar *)s);
      break;
    case PREF_VEC3:
      {
        float* v = (float*)pPref->mVal;
        sprintf(s, "%f %f %f", v[0], v[1], v[2]);
        xmlNodeSetContent(pNode, (xmlChar *)s);
      }
      break;
    case PREF_WNDPOS:
      {
        CString str;
        WindowPosition_Write(*(window_position_t*)pPref->mVal, str);
        xmlNodeSetContent(pNode, (xmlChar*)str.GetBuffer());
      }
      break;
    }
  }
}

void CXMLPropertyBag::Clear()
{
  if(!InUse())
    return;

  xmlFreeDoc(mpDoc);
  mpDoc = NULL;
  mpDocNode = NULL;
  mbEmpty = false;
}

void CXMLPropertyBag::ReadXMLFile(const char* pFilename)
{
  mpDoc = xmlParseFile(pFilename);

  // basic checks
  if (mpDoc)
  {
    mpDocNode = mpDoc->children;
    xmlAttrPtr tmp_attr_ptr = xmlHasProp(mpDocNode, (xmlChar *)"version");
    if (strcmp((char *)mpDocNode->name, "qpref"))
    {
      Sys_FPrintf(SYS_ERR, "Unrecognized node '%s' in '%s'\n", mpDocNode->name, mpDoc->URL);
      xmlFreeDoc(mpDoc);
      mpDoc = NULL;
    }
    else if (tmp_attr_ptr != NULL && strcmp((char*)tmp_attr_ptr->children->content, "1"))
    {
      Sys_FPrintf(SYS_ERR, "Wrong version '%s' in <qpref> node for '%s'\n", (char*)tmp_attr_ptr->children->content, mpDoc->URL);
      xmlFreeDoc(mpDoc);
      mpDoc = NULL;
    }    
    Sys_Printf("Opened XML property file: '%s'\n", pFilename);
  }

  if (!mpDoc)
  {
    mbEmpty = true;
    // no document, create one
    mpDoc = xmlNewDoc((xmlChar *)"1.0");
    mpDocNode = xmlNewDocNode(mpDoc, NULL, (xmlChar *)"qpref", NULL);
    xmlDocSetRootElement(mpDoc, mpDocNode);
    xmlSetProp(mpDocNode, (xmlChar *)"version", (xmlChar *)"1");
    Sys_Printf("XML property file '%s' invalid/not found, creating blank properties tree\n", pFilename);
  }
}

qboolean CXMLPropertyBag::WriteXMLFile(const char* pFilename)
{
  int res = xmlSaveFormatFile(pFilename, mpDoc, 1);
    
  if(res == -1)
    return false;

  Sys_Printf("Wrote XML property file '%s'\n", pFilename);
  return true;
}

// =============================================================================
// Widget callbacks for PrefsDlg

#if !defined(WIN32)
// browse for custom editor executable
static void OnBtnBrowseEditor (GtkWidget *widget, gpointer data)
{
  PrefsDlg *dlg = (PrefsDlg*)data;
  
  const char *filename = file_dialog(g_PrefsDlg.GetWidget(), TRUE, "Executable for Custom Editor");
  
  if(filename != NULL)
  {
    dlg->m_strEditorCommand = filename;
    dlg->UpdateData(FALSE);
  }
}
#endif

static void OnBtnBrowseprefab (GtkWidget *widget, gpointer data)
{
  PrefsDlg *dlg = (PrefsDlg*)data;
  char *path = dlg->m_strPrefabPath;
  if (strlen (path) == 0)
    path = g_strGameToolsPath;
  char *dir = dir_dialog (g_PrefsDlg.GetWidget (), "Set prefab path", path);
  dlg->UpdateData(TRUE);

  if (dir != NULL)
  {
    CString strPath;
    strPath = dir;
    AddSlash(strPath);
    dlg->m_strPrefabPath = strPath;
    dlg->UpdateData(FALSE);
    free (dir);
  }
}

static void OnBtnBrowseuserini (GtkWidget *widget, gpointer data)
{
  PrefsDlg *dlg = (PrefsDlg*)data;
  char *path = dlg->m_strUserPath;
  if (strlen (path) == 0)
    path = g_strGameToolsPath;
  // TODO: INI filter?
  const char *filename = file_dialog (g_PrefsDlg.GetWidget(), TRUE, "Find INI file", path);

  if (filename != NULL)
  {
    dlg->UpdateData(TRUE);
    dlg->m_strUserPath = filename;
    dlg->UpdateData(FALSE);
  }
}

static void OnButtonClean (GtkWidget *widget, gpointer data) 
{
  // make sure this is what the user wants
  if (gtk_MessageBox (g_PrefsDlg.GetWidget (), "This will close Radiant and clean the corresponding registry entries.\n"
		  "Next time you start Radiant it will be good as new. Do you wish to continue?",
		  "Reset Registry", MB_YESNO) == IDYES)
  {
    PrefsDlg *dlg = (PrefsDlg*)data;
    dlg->EndModal (IDCANCEL);

    g_qeglobals.disable_ini = true;
    remove (dlg->m_inipath->str);
    char buf[PATH_MAX];
    sprintf(buf, "%sSavedInfo.bin", dlg->m_rc_path->str);
    remove(buf);
    HandleCommand (NULL, GINT_TO_POINTER (ID_FILE_EXIT));
    _exit (0);
  }
}

// =============================================================================
// PrefsDlg class

// IMPORTANT NOTE: the values here don't matter very much
// the actual intialization if you start with an empty .ini is done when loading the prefs for the first time
// profile_load_int takes an argument to use if the value is not found
PrefsDlg::PrefsDlg ()
{
  m_bWarn = TRUE;
  m_nMouse = 1;
  m_nView = MainFrame::eRegular;
  m_bLoadLast = FALSE;
  m_bInternalBSP = FALSE;
  m_bRightClick = FALSE;
  m_bSetGame = FALSE;
  m_bAutoSave = TRUE;
  m_nAutoSave = 5;
  m_bLoadLastMap = FALSE;
  m_bTextureWindow = FALSE;
  m_bSnapShots = FALSE;
  m_fTinySize = 0.5;
  m_bCleanTiny = FALSE;
  m_bCamXYUpdate = TRUE;
  m_bCamDragMultiSelect = FALSE;
  m_bCamFreeLook = TRUE;
  m_bCamFreeLookStrafe = FALSE;
  m_bCamInverseMouse = FALSE;
  m_bCamDiscrete = TRUE;
  m_bNewLightDraw = FALSE;
  m_strPrefabPath = "";
  m_nWhatGame = 0;
  m_bALTEdge = FALSE;
  m_bFaceColors = FALSE;
  m_bXZVis = FALSE;
  m_bYZVis = FALSE;
  m_bZVis = FALSE;
  m_bSizePaint = FALSE;
  m_bDLLEntities = FALSE;
#ifdef _WIN32
  m_bDetachableMenus = FALSE;   // Most win32 users will find detachable menus annoying
#else
  m_bDetachableMenus = TRUE;    // Linux/Apple users are used to them...
#endif
  m_bPatchToolbar = TRUE;
  m_bWideToolbar = TRUE;
  m_bPluginToolbar = TRUE;
  m_bNoClamp = FALSE;
  m_strUserPath = "";
  m_nRotation = 0;
  m_bChaseMouse = FALSE;
  m_bTextureScrollbar = TRUE;
  m_bDisplayLists = TRUE;
  m_bAntialiasedPointsAndLines = FALSE; // Fishman - Add antialiazed points and lines support. 09/03/00
  m_bShowShaders = FALSE;
  m_nShader = -1;
  m_bNoStipple = FALSE;
  m_bVertexSplit = FALSE;
  m_bSelectCurves = TRUE;
  m_bSelectModels = TRUE;
  m_nEntityShowState = ENTITY_SKINNED_BOXED;
  m_nTextureScale = 2;
  m_bSwitchClip = FALSE;
  m_bSelectWholeEntities = TRUE;
  m_nTextureQuality = 3;
  m_bShowShaders = TRUE;
  m_bGLLighting = FALSE;
  m_nShader = 0;
  m_nUndoLevels = 30;
  m_bTexturesShaderlistOnly = FALSE;
  // paths to ini files
  m_rc_path = NULL;
  m_inipath = NULL;
  m_bWatchBSP = TRUE;
  m_bLeakStop = TRUE;
  m_iTimeout = 15;
  m_bRunQuake = TRUE;
  m_bDoSleep = FALSE;
  m_nSubdivisions = 4;
  // not prefs
  m_bFloatingZ = FALSE;
  m_bGlPtWorkaround = FALSE;	// Gef: Kyro/GL_POINTS workaround 25-aug-2001
#ifdef _WIN32
  m_bNativeGUI = FALSE;
  m_bStartOnPrimMon = FALSE;
#endif
  m_global_rc_path = NULL;
#ifdef _WIN32
  m_bUseWin32Editor = TRUE;
#else
  // custom shader editor options
  m_bUseCustomEditor = FALSE;
  m_strEditorCommand = "";
#endif
  m_nLightRadiuses = 1;
  m_bQ3Map2Texturing = TRUE;
#ifdef ATIHACK_812
	m_bGlATIHack = FALSE;
#endif
}

/*!
=========================================================
Games selection dialog
=========================================================
*/

CGameDescription::CGameDescription(xmlDocPtr pDoc, const Str &GameFile)
{
  char *p, *prop;
  mpDoc = pDoc;
  // read the user-friendly game name 
  xmlNodePtr pNode = mpDoc->children;

  while (strcmp((const char*)pNode->name, "game") && pNode != NULL) pNode=pNode->next;
  if (!pNode)
  {
    ///< \todo add the file name (this node and gametools should all be part of CGameDescription anyway)
    Error("Didn't find 'game' node in the game description file '%s'\n", pDoc->URL);
  }
  // on win32, game tools path can now be specified relative to the exe's cwd
  prop = (char*)xmlGetProp( pNode, (xmlChar*)"gametools" );
  if ( prop == NULL ) {
	  Error( "Didn't find 'gametools' node in the game description file '%s'\n", pDoc->URL );
  }
  {
	char full[PATH_MAX];
#ifdef _WIN32
	_fullpath( full, prop, PATH_MAX );
#else
    strncpy( full, prop, PATH_MAX );
#endif
    xmlFree( prop );
    prop = NULL;
    for ( p = full; *p != '\0'; p++ ) {
	  if ( *p == '\\' ) {
	    *p = '/';
	  }
	  mGameToolsPath = full;
	  if ( p != full && *(p-1) != '/' ) {
	    mGameToolsPath += "/";
	  }
    }
  }

  prop = (char*)xmlGetProp(pNode, (xmlChar*)"name");
  if (prop == NULL)
  {
    Sys_FPrintf(SYS_WRN, "Warning, 'name' attribute not found in '%s'\n", pDoc->URL);
    mGameName = pDoc->URL;
  }
  else
  {
    mGameName = prop;
    xmlFree(prop);
  }

  mGameFile = GameFile;

  prop = (char*)xmlGetProp(pNode, (xmlChar*)"basegame");
  if (prop == NULL)
  {
    // default
    mBaseGame = "baseq3";
  }
  else
  {
    mBaseGame = prop;
    xmlFree(prop);
  }

  // on win32, engine path can now be specified relative to the exe's cwd
  prop = (char*)xmlGetProp(pNode, (const xmlChar *)"enginepath");
  if ( prop != NULL ) {
    char full[PATH_MAX];
#ifdef _WIN32
	_fullpath( full, prop, PATH_MAX );
#else
	strncpy( full, prop, PATH_MAX );
#endif
    xmlFree( prop );
	prop = NULL;
    // process seperators
    for ( p = full; *p != '\0'; p++ ) {
	  if ( *p == '\\' ) {
	    *p = '/';
	  }
	}
    mEnginePath = full;
	if ( p != full && *(p-1) != '/' ) {
	  mEnginePath += "/";
	}
  }
  else
  {
    // if engine path was not specified in the .game, it implies we can guess it from the gametools path
    // on win32, and for most game package, the gametools are installed with the game
    char aux_path[PATH_MAX]; // aux
    strcpy( aux_path, mGameToolsPath.GetBuffer() );
	if ( ( aux_path[ strlen(aux_path)-1 ] == '/' ) || ( aux_path[ strlen(aux_path)-1 ] == '\\' ) ) {
      aux_path[strlen(aux_path)-1] = '\0'; // strip ending '/' if any
	}
    char up_path[PATH_MAX]; // up one level
    ExtractFilePath( aux_path, up_path );
    mEnginePath = up_path;
  }

  prop = (char*)xmlGetProp(pNode, (xmlChar*)"engine");
  if (prop == NULL)
  {
#ifdef _WIN32
    mEngine = "quake3.exe";
#elif __linux__
    mEngine = "quake3";
#elif __APPLE__
    mEngine = "Quake3.app";
#endif
  }
  else
  {
    mEngine = prop;
    xmlFree(prop);
  }

#if defined (__linux__) || defined (__APPLE__)
  // *nix specific
  prop = (char*)xmlGetProp(pNode, (const xmlChar *)"prefix");
  if(prop != NULL)
  {
    mUserPathPrefix = prop;
    xmlFree(prop);
  }
#endif
  mShaderPath = xmlGetProp(pNode, (const xmlChar *)"shaderpath");
  if (!mShaderPath.GetLength())
  {
    mShaderPath = "scripts/";
    mShaderlist = "scripts/shaderlist.txt";
  }
  else
  {
    AddSlash(mShaderPath);
    mShaderlist = mShaderPath;
    mShaderlist += "shaderlist.txt";
  }
  xmlChar* default_scale = xmlGetProp(pNode, (const xmlChar *)"default_scale");
  if (default_scale)
  {
    mTextureDefaultScale = atof((const char *)default_scale);
    xmlFree(default_scale);
  }
  else
    mTextureDefaultScale = 0.5f;
  xmlChar* eclass_singleload = xmlGetProp(pNode, (const xmlChar*)"eclass_singleload");
  if (eclass_singleload)
  {
    mEClassSingleLoad = true;
    xmlFree(eclass_singleload);
  }
  else
    mEClassSingleLoad = false;
  xmlChar* no_patch = xmlGetProp(pNode, (const xmlChar *)"no_patch");
  if (no_patch)
  {
    mNoPatch = true;
    xmlFree(no_patch);
  }
  else
    mNoPatch = false;
  xmlChar* caulk_shader = xmlGetProp(pNode, (const xmlChar *)"caulk_shader");
  if (caulk_shader)
  {
    mCaulkShader = caulk_shader;
    xmlFree(caulk_shader);
  }
  else
    mCaulkShader = "textures/common/caulk";
}

void CGameDescription::Dump()
{
#ifdef _WIN32
  if (CGameDialog::GetNetrun())
    Sys_Printf("Running in network mode, prefs path set to '%s'\n", g_strTempPath.GetBuffer());
#endif
  Sys_Printf("game name            : '%s'\n", mGameName.GetBuffer());
  Sys_Printf("game file            : '%s'\n", mGameFile.GetBuffer());
  Sys_Printf("game path            : '%s'\n", mGameToolsPath.GetBuffer());
  Sys_Printf("base game            : '%s'\n", mBaseGame.GetBuffer());
  Sys_Printf("engine path          : '%s'\n", mEnginePath.GetBuffer());
  Sys_Printf("engine               : '%s'\n", mEngine.GetBuffer());
  Sys_Printf("shaderlist           : '%s'\n", mShaderlist.GetBuffer());
  Sys_Printf("caulk shader: '%s'\n", mCaulkShader.GetBuffer());
#if defined (__linux__) || defined (__APPLE__)
  Sys_Printf("prefix               : '%s'\n", mUserPathPrefix.GetBuffer());
#endif
  Sys_Printf("default texture scale: %g\n", mTextureDefaultScale);
  Sys_Printf("single eclass load   : %s\n", mEClassSingleLoad ? "Yes" : "No");
  Sys_Printf("patches supported    : %s\n", mNoPatch ? "No" : "Yes");
}

CPrefAssignment& CPrefAssignment::operator = (const CPrefAssignment& ass)
{
  if (&ass != this)
  {
    mName = ass.mName;
    mType = ass.mType;
    mVal = ass.mVal;
  }
  return *this;
}

CPrefAssignment::CPrefAssignment(const CPrefAssignment& ass)
{
  *this = ass;
}

void CGameDialog::LoadPrefs()
{
  bool bEmpty = false;

  // if we already have a document loaded, we will free and reload from file
  if (mGlobalPrefs.InUse())
  {
    Sys_Printf("Reloading global prefs from file\n");
    mGlobalPrefs.Clear();
  }

  // load global .pref file
  CString strGlobalPref = g_PrefsDlg.m_global_rc_path->str;
  strGlobalPref += "global.pref";

  mGlobalPrefs.ReadXMLFile(strGlobalPref.GetBuffer());

  mGlobalPrefs.GetPref("gamefile", &m_sGameFile, ""); // NOTE: there's no default, user HAS to select something
  mGlobalPrefs.GetPref("autoload", &m_bAutoLoadGame, false);
  mGlobalPrefs.GetPref("log console", &m_bLogConsole, false);
  // in a very particular post-.pid startup
  // we may have the console turned on and want to keep it that way
  // so we use a latching system
  if (m_bForceLogConsole)
  {
    m_bLogConsole = true;
    Sys_Printf("console logging has been latched on, saving prefs\n");
    SavePrefs();
    m_bForceLogConsole = false;
  }

  // console logging: call Sys_LogConsole to check console logging status
  // it is important that we would log console as early as possible to make it useful
  Sys_LogFile();

  if (mGlobalPrefs.mbEmpty)
  {
    Sys_Printf("Saving global.pref with default pref values\n");
    SavePrefs();
  }
}

void CGameDialog::SavePrefs()
{
  // update the tree and save it
  mGlobalPrefs.UpdatePrefTree();
  
  CString strGlobalPref = g_PrefsDlg.m_global_rc_path->str;
  strGlobalPref += "global.pref";
  
  if (!mGlobalPrefs.WriteXMLFile(strGlobalPref.GetBuffer()))
    Sys_FPrintf(SYS_ERR, "Error occured while saving global prefs file '%s'\n", strGlobalPref.GetBuffer());
}

void CGameDialog::DoGameInstall() {
	mGameInstall.Run();
}

void CGameDialog::DoGameDialog() {
	// allow looping the game selection dialog with calls to the game configure dialog in between
	while ( m_bDoGameInstall ) {
		
		m_bDoGameInstall = false;

		DoModal();
		
		if ( m_bDoGameInstall ) {
			DoGameInstall();
			ScanForGames();
			// and we will loop to do another DoModal dialog
		}
	}

	// unhook so we can use in other places
	// we manually incref'ed it when creating, it won't be freed (destructor)
	gtk_container_remove( GTK_CONTAINER( mTopBox ), GetGlobalFrame() );

	// we save the prefs file
	SavePrefs();
}

GtkWidget* CGameDialog::GetGlobalFrame()
{
  GtkWidget *vbox, *text, *combo, *check;

  if ( mFrame != NULL ) {
	  return mFrame;
  }

  mFrame = gtk_frame_new( NULL );
  gtk_container_set_border_width( GTK_CONTAINER( mFrame ), 5 );
  gtk_widget_show( mFrame );

  vbox = gtk_vbox_new( FALSE, 6 );
  gtk_widget_show( vbox );
  gtk_container_add( GTK_CONTAINER( mFrame ), vbox );
  gtk_container_set_border_width( GTK_CONTAINER( vbox ), 5 );

  text = gtk_label_new( "Select the game:" );
  gtk_widget_show( text );
  gtk_box_pack_start( GTK_BOX( vbox ), text, FALSE, FALSE, 0 );

  combo = gtk_combo_new();
  gtk_widget_show( combo );
  gtk_box_pack_start( GTK_BOX( vbox ), combo, FALSE, FALSE, 0 );

  // fill in with the game descriptions
  GList *combo_list = (GList*)NULL;
  list<CGameDescription *>::iterator iGame;
  for( iGame = mGames.begin(); iGame != mGames.end(); iGame++ ) {
	  combo_list = g_list_append( combo_list, (void *)(*iGame)->mGameName.GetBuffer() );
  }
  gtk_combo_set_popdown_strings( GTK_COMBO( combo ), combo_list );
  g_list_free( combo_list );

  AddDialogData( combo, &m_nComboSelect, DLG_COMBO_INT );

  check = gtk_check_button_new_with_label( "Auto load selected game on startup" );
  gtk_widget_show(check);
  gtk_box_pack_start (GTK_BOX(vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bAutoLoadGame, DLG_CHECK_BOOL);
  
  text = gtk_label_new("(this frame is available in the prefs menu if you set auto-select)");
  gtk_widget_show(text);
  gtk_box_pack_start (GTK_BOX(vbox), text, FALSE, FALSE, 0);

#ifdef _WIN32
  check = gtk_check_button_new_with_label( "Networked install - per-user settings" );
  gtk_widget_show( check );
  gtk_box_pack_start( GTK_BOX( vbox ), check, FALSE, FALSE, 0 );
  AddDialogData( check, &m_bNetRun, DLG_CHECK_BOOL );
#endif

  check = gtk_check_button_new_with_label( "Log the console to radiant.log" );
  gtk_widget_show( check );
  gtk_box_pack_start( GTK_BOX( vbox ), check, FALSE, FALSE, 0 );
  AddDialogData( check, &m_bLogConsole, DLG_CHECK_BOOL );

  // incref it so we can pass it around
  gtk_widget_ref( GTK_WIDGET( mFrame ) );

  return mFrame;
}

void CGameDialog::UpdateData( bool retrieve ) {
  if (!retrieve)
  {
    // use m_sGameFile to set m_nComboSelect
    list<CGameDescription *>::iterator iGame;
    int i = 0;
    for(iGame=mGames.begin(); iGame!=mGames.end(); iGame++)
    {
      if ((*iGame)->mGameFile == m_sGameFile)
      {
        m_nComboSelect = i;
        break;
      }
      i++;
    }
#ifdef _WIN32
    UpdateNetrun(false);
#endif
  }
  Dialog::UpdateData(retrieve);
  if (retrieve)
  {
    // use m_nComboSelect to set m_sGameFile
    list<CGameDescription *>::iterator iGame = mGames.begin();
    int i;
    for(i=0; i<m_nComboSelect; i++)
    {
      iGame++;
    }
    m_sGameFile = (*iGame)->mGameFile;
#ifdef _WIN32
    UpdateNetrun(true);
#endif
  }
}

void CGameDialog::SInstallCallback( GtkWidget *widget, gpointer data ) {
	CGameDialog *d = static_cast< CGameDialog* >( data );
	d->m_bDoGameInstall = true;
	d->EndModal( 0 );
}

void CGameDialog::BuildDialog() {
	GtkWidget *dlg, *vbox1, *button, *setup_button;
	
	dlg = m_pWidget;
	gtk_window_set_title( GTK_WINDOW( dlg ), "Select Game" );

	vbox1 = gtk_vbox_new( FALSE, 0 );
	gtk_widget_show( vbox1 );
	gtk_container_add( GTK_CONTAINER( dlg ), vbox1 );

	gtk_container_add( GTK_CONTAINER( vbox1 ), GetGlobalFrame() );
	mTopBox = vbox1;

	setup_button = gtk_button_new_with_label( "Configure more games" );
	gtk_widget_show( setup_button );
	gtk_box_pack_start( GTK_BOX( vbox1 ), setup_button, FALSE, FALSE, 0 );
	gtk_signal_connect( GTK_OBJECT( setup_button ), "clicked",
						GTK_SIGNAL_FUNC( SInstallCallback ), this );

	button = gtk_button_new_with_label( "OK" );
	gtk_widget_show( button );
	gtk_box_pack_start( GTK_BOX( vbox1 ), button, FALSE, FALSE, 0 );
	AddModalButton( button, IDOK );
	
	gtk_widget_set_usize( button, 60, -2 );
}

void CGameDialog::ScanForGames()
{
  CString strPath;
  char *dirlist;
  GDir *dir;
  CString strGamesPath = g_strAppPath.GetBuffer();
  strGamesPath += "games";
  const char *path = strGamesPath.GetBuffer();

  if ( !mGames.empty() ) {
	  Sys_Printf( "Clearing game list\n" );
	  list<CGameDescription*>::iterator iGame;
	  for ( iGame = mGames.begin(); iGame != mGames.end(); iGame++ ) {
		  delete (*iGame);
	  }
	  mGames.clear();
  }

  Sys_Printf( "Scanning for game description files: %s\n", path );

  /*!
  \todo FIXME LINUX:
  do we put game description files below g_strAppPath, or in ~/.radiant
  i.e. read only or read/write?
  my guess .. readonly cause it's an install
  we will probably want to add ~/.radiant/<version>/games/ scanning on top of that for developers
  (if that's really needed)
  */

  // FIXME need to catch the 'no game description' situation and exit with a clean error

  dir = g_dir_open(path, 0, NULL);

  if (dir != NULL)
  {
    while (1)
    {
      const gchar* name = g_dir_read_name(dir);
      if(name == NULL)
        break;

      dirlist = g_strdup(name);
#ifdef _WIN32
      strlwr (dirlist);
#endif
      char *ext = strrchr (dirlist, '.');
      if ((ext == NULL) || (strcmp (ext, ".game") != 0))
        continue;
      strPath.Format("%s/%s", path, dirlist);
      Sys_Printf("%s\n", strPath.GetBuffer());
      // got one, load it
      xmlDocPtr pDoc = xmlParseFile(strPath.GetBuffer());
      if (pDoc)
      {
        mGames.push_front( new CGameDescription( pDoc, dirlist ) );
      }
      else
      {
        Sys_FPrintf(SYS_ERR, "XML parser failed on '%s'\n", strPath.GetBuffer());
      }

      g_free(dirlist);
    }
    g_dir_close (dir);
  }
}

CGameDescription* CGameDialog::GameDescriptionForComboItem()
{
  list<CGameDescription *>::iterator iGame;
  int i=0;
  for(iGame=mGames.begin(); iGame!=mGames.end(); iGame++,i++)
  {
    if (i == m_nComboSelect)
    {
      return (*iGame);
    }
  }
  return NULL; // not found
}

void CGameDialog::InitGlobalPrefPath()
{
  GString *global_rc_path;
  // configure m_global_rc_path
  // this is the g_strTempPath, and it has already been mkdir'ed
  global_rc_path = g_string_new(g_strTempPath.GetBuffer());
  g_PrefsDlg.m_global_rc_path = global_rc_path;
}

void CGameDialog::Reset()
{
  if (!g_PrefsDlg.m_global_rc_path)
    InitGlobalPrefPath();
  CString strGlobalPref = g_PrefsDlg.m_global_rc_path->str;
  strGlobalPref += "global.pref";
  remove(strGlobalPref.GetBuffer());
}

void CGameDialog::Init()
{
  InitGlobalPrefPath();
  ScanForGames();
  if ( mGames.empty() ) {
	  DoGameInstall();
	  ScanForGames();
	  if ( mGames.empty() ) {
		  Error( "No games setup, aborting\n" );
	  }
  }
  LoadPrefs();
  if ( m_bAutoLoadGame ) {
    // search by .game name
    list<CGameDescription *>::iterator iGame;
    for(iGame=mGames.begin(); iGame!=mGames.end(); iGame++)
    {
      if ((*iGame)->mGameFile == m_sGameFile)
      {
        m_pCurrentGameDescription = (*iGame);
        break;
      }
    }
  }
  if ( !m_bAutoLoadGame || !m_pCurrentGameDescription ) {
    DoGameDialog();
    // use m_nComboSelect to identify the game to run as and set the globals
    m_pCurrentGameDescription = GameDescriptionForComboItem();
    if ( !m_pCurrentGameDescription ) {
		Error("Lookup of game description object failed, can't continue\n");
	}
  }
  g_pGameDescription = m_pCurrentGameDescription;

  g_strGameToolsPath = g_pGameDescription->mGameToolsPath;

	// NOTE TTimo: this is moved from QE_LoadProject in 1.2
	// (probably broken)
  // NOTE Hydra: was broken for win32, we don't use m_strHomeGame or m_strFSBasePath
#if defined (__linux__) || defined (__APPLE__)
  g_qeglobals.m_strHomeGame = g_get_home_dir();
  g_qeglobals.m_strHomeGame += "/";
  g_qeglobals.m_strHomeGame += m_pCurrentGameDescription->mUserPathPrefix.GetBuffer();
  g_qeglobals.m_strHomeGame += "/";
#else
  g_qeglobals.m_strHomeGame = g_pGameDescription->mEnginePath.GetBuffer();
#endif
  
  g_pGameDescription->Dump();
}

CGameDialog::~CGameDialog()
{
  if (mFrame)
  {
    // NOTE I'm not too sure how reliable this is
    gtk_widget_unref(GTK_WIDGET(mFrame));
  }
  // free all the game descriptions
  list<CGameDescription *>::iterator iGame;
  for(iGame=mGames.begin(); iGame!=mGames.end(); iGame++)
  {
    delete (*iGame);
    *iGame = NULL;
  }
}

void CGameDialog::AddPacksURL(Str &URL)
{
  // add the URLs for the list of game packs installed
  // FIXME: this is kinda hardcoded for now..
  list<CGameDescription *>::iterator iGame;
  for(iGame=mGames.begin(); iGame!=mGames.end(); iGame++)
  {  
    if ((*iGame)->mGameFile == "q3.game")      
      URL += "&Games_dlup%5B%5D=1";
    else if ((*iGame)->mGameFile == "wolf.game")
      URL += "&Games_dlup%5B%5D=2";
    else if ((*iGame)->mGameFile == "wolf.game")
      URL += "&Games_dlup%5B%5D=3";
    else if ((*iGame)->mGameFile == "jk2.game")
      URL += "&Games_dlup%5B%5D=4";
    else if ((*iGame)->mGameFile == "stvef.game")
      URL += "&Games_dlup%5B%5D=5";
    else if ((*iGame)->mGameFile == "sof2.game")
      URL += "&Games_dlup%5B%5D=6";
    else if ((*iGame)->mGameFile == "ja.game")
      URL += "&Games_dlup%5B%5D=7";
  }
}

#ifdef _WIN32

#define NETRUN_FILENAME "netrun.conf"

bool CGameDialog::m_bNetRun;

void CGameDialog::UpdateNetrun(bool retrieve)
{
  FILE *f_netrun;
  CString strNetrun;
  strNetrun = g_strAppPath; strNetrun += NETRUN_FILENAME;
  if (!retrieve)
  {
    // http://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=639
    // now check if we are running from a network installation
    // use a dummy file as the flag
    f_netrun = fopen(strNetrun.GetBuffer(), "r");
    if (f_netrun)
    {
      fclose(f_netrun);
      m_bNetRun = true;
    }
    else
      m_bNetRun = false;
  }
  else
  {
    if (m_bNetRun)
    {
      f_netrun = fopen(strNetrun.GetBuffer(), "w");
      if (!f_netrun)
      {
        Sys_FPrintf(SYS_ERR, "ERROR: Failed to create netrun file '%s'\n", strNetrun.GetBuffer());
        m_bNetRun = false;
      }
      else
      {
        fclose(f_netrun);
        Sys_Printf("Created/Checked '%s'\n", strNetrun.GetBuffer());
      }
    }
    else
    {
      if (remove(strNetrun.GetBuffer()) == -1)
      {
        if (errno != ENOENT)
          Sys_FPrintf(SYS_ERR, "Failed to remove netrun file '%s'\n", strNetrun.GetBuffer());
        m_bNetRun = true;
      }
      else
      {
        Sys_Printf("Netrun mode is disabled\n");
      }
    }
  }
}

bool CGameDialog::GetNetrun()
{
  return m_bNetRun;
}
#endif

/*
========

very first prefs init deals with selecting the game and the game tools path
then we can load .ini stuff

using prefs / ini settings:
those are per-game

win32:
look in g_strGameToolsPath for .ini

linux:
look in ~/.radiant/<version>/gamename
========
*/

#define PREFS_LOCAL_FILENAME "local.pref"

void PrefsDlg::Init()
{
  mGamesDialog.Init();

  // m_global_rc_path has been set above, do m_rc_path with game specific stuff now
  // the win32 and linux versions have been unified for network mode
#ifdef _WIN32
  if (!CGameDialog::GetNetrun())
  {
    // legacy prefs settings, this goes where the game pack is installed
    m_rc_path = g_string_new (g_strGameToolsPath.GetBuffer() );
    m_inipath = g_string_new (m_rc_path->str);
    g_string_append (m_inipath, PREFS_LOCAL_FILENAME);
    return;
  }
#endif
  // this is common to win32 and Linux init now
  m_rc_path = g_string_new (m_global_rc_path->str);
  
  // game sub-dir
  g_string_append (m_rc_path, g_pGameDescription->mGameFile.GetBuffer());
  g_string_append (m_rc_path, "/");
  Q_mkdir (m_rc_path->str, 0775);
  
  // then the ini file
  m_inipath = g_string_new (m_rc_path->str);
  g_string_append (m_inipath, PREFS_LOCAL_FILENAME);
  
}

void PrefsDlg::UpdateData (bool retrieve)
{
  // leo: the "changed" signal confuses the update function
  if (m_pWidget == NULL)
    return;
  mGamesDialog.UpdateData (retrieve);
  Dialog::UpdateData (retrieve);
}

#ifdef _WIN32
#define PREFSHSPACE 5
#else
#define PREFSHSPACE 0
#endif

static void UpdateSensitivity( GtkWidget *widget, gpointer data )
{
  PrefsDlg *dlg = (PrefsDlg*)data;
  dlg->DoSensitivity();
}

static void UpdateEditorSensitivity(GtkWidget *widget, gpointer data)
{
  PrefsDlg *dlg = (PrefsDlg*)data;
  dlg->DoEditorSensitivity();
}

// start new prefs dialog

/*! Utility function for swapping notebook pages for tree list selections */
void PrefsDlg::showPrefPage(int prefpage)
{
  if(gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)) != prefpage)
    gtk_notebook_set_page(GTK_NOTEBOOK(notebook), prefpage);
  
  return;
}

static void treeSelection(GtkTreeSelection* selection, gpointer data)
{
  PrefsDlg *dlg = (PrefsDlg*)data;

  GtkTreeModel* model;
  GtkTreeIter selected;
  if(gtk_tree_selection_get_selected(selection, &model, &selected))
  {
    int prefpage;
    gtk_tree_model_get(model, &selected, 1, (gpointer*)&prefpage, -1);
    dlg->showPrefPage(prefpage);
  }
}

void PrefsDlg::BuildDialog ()
{
  // Main Preferences dialog
  GtkWidget *dialog, *mainvbox, *hbox, *sc_win, *preflabel;

  // Widgets on notebook pages
  GtkWidget *check, *label, *scale, *hbox2, *combo,
            *table, *spin,  *entry, *pixmap,
            *radio, *button, *pageframe, *vbox;
  
  GList *combo_list = (GList*)NULL;
  
  GtkObject *adj;
    
  dialog = m_pWidget;
  gtk_window_set_title(GTK_WINDOW(dialog), "GtkRadiant Preferences");
  gtk_widget_realize(dialog);
    
  mainvbox = gtk_vbox_new(FALSE, 5);
  gtk_container_add(GTK_CONTAINER(dialog), mainvbox);
  gtk_container_set_border_width(GTK_CONTAINER(mainvbox), 5);
  gtk_widget_show(mainvbox);
  
  hbox = gtk_hbox_new(FALSE, 5);
  gtk_widget_show(hbox);
  gtk_box_pack_end(GTK_BOX(mainvbox), hbox, FALSE, TRUE, 0);

  button = gtk_button_new_with_label("OK");
  gtk_widget_show(button);
  gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 0);
  gtk_widget_set_usize(button, 60, -2);
  AddModalButton(button, IDOK);

  button = gtk_button_new_with_label("Cancel");
  gtk_widget_show(button);
  gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 0);
  gtk_widget_set_usize(button, 60, -2);
  AddModalButton(button, IDCANCEL);

  button = gtk_button_new_with_label ("Clean");
  gtk_widget_show(button);
  gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(OnButtonClean), this);
  gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
  gtk_widget_set_usize (button, 60, -2);
  
  hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(mainvbox), hbox, TRUE, TRUE, 0);
  gtk_widget_show(hbox);
  
  sc_win = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sc_win), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start(GTK_BOX(hbox), sc_win, FALSE, FALSE, 0);
  gtk_widget_show(sc_win);

  // prefs pages notebook
  notebook = gtk_notebook_new();
  // hide the notebook tabs since its not supposed to look like a notebook
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), FALSE);
  gtk_box_pack_start(GTK_BOX(hbox), notebook, TRUE, TRUE, 0);
  gtk_widget_show(notebook);

  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sc_win), GTK_SHADOW_IN);

  {
    GtkTreeStore* store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);

    GtkWidget* view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(view), FALSE);

    {
      GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
      GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes("Preferences", renderer, "text", 0, NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);
    }

    {
      GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
      g_signal_connect(G_OBJECT(selection), "changed", G_CALLBACK(treeSelection), this);
    }

    gtk_widget_show(view);

    gtk_container_add(GTK_CONTAINER (sc_win), view);

    {
      /********************************************************************/
      /* Add preference tree options                                      */
      /********************************************************************/
      {
        GtkTreeIter group;
        gtk_tree_store_append(store, &group, NULL);
        gtk_tree_store_set(store, &group, 0, "Globals", 1, PTAB_FRONT, -1);
        {
          GtkTreeIter tab;
          gtk_tree_store_append(store, &tab, &group);
          gtk_tree_store_set(store, &tab, 0, "Game settings", 1, (gpointer)PTAB_GAME_SETTINGS, -1);
        }
      }

      {
        GtkTreeIter group;
        gtk_tree_store_append(store, &group, NULL);
        gtk_tree_store_set(store, &group, 0, "Display", 1, PTAB_FRONT, -1);
        {
          GtkTreeIter tab;
          gtk_tree_store_append(store, &tab, &group);
          gtk_tree_store_set(store, &tab, 0, "2D Display/Rendering", 1, (gpointer)PTAB_2D, -1);
        }
        {
          GtkTreeIter tab;
          gtk_tree_store_append(store, &tab, &group);
          gtk_tree_store_set(store, &tab, 0, "3D View", 1, (gpointer)PTAB_CAMERA, -1);
        }
        {
          GtkTreeIter tab;
          gtk_tree_store_append(store, &tab, &group);
          gtk_tree_store_set(store, &tab, 0, "Texture Settings", 1, (gpointer)PTAB_TEXTURE, -1);
        }
      }
  
      {
        GtkTreeIter group;
        gtk_tree_store_append(store, &group, NULL);
        gtk_tree_store_set(store, &group, 0, "Interface", 1, PTAB_FRONT, -1);
        {
          GtkTreeIter tab;
          gtk_tree_store_append(store, &tab, &group);
          gtk_tree_store_set(store, &tab, 0, "Layout", 1, (gpointer)PTAB_LAYOUT, -1);
        }
        {
          GtkTreeIter tab;
          gtk_tree_store_append(store, &tab, &group);
          gtk_tree_store_set(store, &tab, 0, "Mouse", 1, (gpointer)PTAB_MOUSE, -1);
        }
        {
          GtkTreeIter tab;
          gtk_tree_store_append(store, &tab, &group);
          gtk_tree_store_set(store, &tab, 0, "Editing", 1, (gpointer)PTAB_EDITING, -1);
        }
      }
  
      {
        GtkTreeIter group;
        gtk_tree_store_append(store, &group, NULL);
        gtk_tree_store_set(store, &group, 0, "Other", 1, PTAB_FRONT, -1);
        {
          GtkTreeIter tab;
          gtk_tree_store_append(store, &tab, &group);
          gtk_tree_store_set(store, &tab, 0, "Startup/Auto save", 1, (gpointer)PTAB_STARTUP, -1);
        }
        {
          GtkTreeIter tab;
          gtk_tree_store_append(store, &tab, &group);
          gtk_tree_store_set(store, &tab, 0, "Paths", 1, (gpointer)PTAB_PATHS, -1);
        }
        {
          GtkTreeIter tab;
          gtk_tree_store_append(store, &tab, &group);
          gtk_tree_store_set(store, &tab, 0, "Misc", 1, (gpointer)PTAB_MISC, -1);
        }
        if (!g_qeglobals.bBSPFrontendPlugin)
        {
          GtkTreeIter tab;
          gtk_tree_store_append(store, &tab, &group);
          gtk_tree_store_set(store, &tab, 0, "BSP Monitoring", 1, (gpointer)PTAB_BSPMONITOR, -1);
        }
      }
    }

    gtk_tree_view_expand_all(GTK_TREE_VIEW(view));
    
    g_object_unref(G_OBJECT(store));
  }
  
  /**********************************************************************/
  /* build the prefs pages                                              */
  /**********************************************************************/
  
  // Front page...
  // todo : add something interesting here
  // NOTE TTimo: tip of the day? or a logo?
  preflabel = gtk_label_new("Front Page");
  gtk_widget_show(preflabel);
  pageframe = gtk_frame_new(NULL);
  gtk_container_set_border_width(GTK_CONTAINER(pageframe), 5);
  gtk_widget_show(pageframe);
  vbox = gtk_vbox_new(FALSE, 5);
  gtk_widget_show(vbox);
  gtk_widget_set_usize(GTK_WIDGET(vbox), 350, -2);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
  gtk_container_add(GTK_CONTAINER(pageframe), vbox);
  
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageframe, preflabel);

  /******** global preferences group ****************************/
  preflabel = gtk_label_new("Globals");
  gtk_widget_show(preflabel);

  pageframe = mGamesDialog.GetGlobalFrame();
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageframe, preflabel);
  
  /******** 2D prefs group (xy views/rendering options) *********/
  preflabel = gtk_label_new("2D Display");
  gtk_widget_show(preflabel);
  pageframe = gtk_frame_new("2D Display");
  gtk_container_set_border_width(GTK_CONTAINER(pageframe), 5);
  gtk_widget_show(pageframe);
  vbox = gtk_vbox_new(FALSE, 5);
  gtk_widget_show(vbox);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
  gtk_container_add(GTK_CONTAINER(pageframe), vbox);
  
  // OpenGL Display Lists
  check = gtk_check_button_new_with_label("OpenGL Display Lists");
  gtk_widget_show(check);
  gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
  AddDialogData(check, &m_bDisplayLists, DLG_CHECK_BOOL);
  
  // Antialiased points & lines
  // Fishman - Add antialiazed points and lines support. 09/03/00
  check = gtk_check_button_new_with_label ("OpenGL antialiased points and lines");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bAntialiasedPointsAndLines, DLG_CHECK_BOOL);
  
  // Solid selection boxes
  check = gtk_check_button_new_with_label ("Solid selection boxes");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bNoStipple, DLG_CHECK_BOOL);
  
  // Display size info
  check = gtk_check_button_new_with_label ("Display size info");
  gtk_widget_show (check);
  gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bSizePaint, DLG_CHECK_BOOL);

  // Alternate vertex/edge handles
  // Gef: Kyro GL_POINT work around 25-aug-2001
  check = gtk_check_button_new_with_label ("Alternate vertex/edge handles");
  gtk_widget_show(check);
  gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
  AddDialogData(check, &m_bGlPtWorkaround, DLG_CHECK_BOOL);

  g_list_free (combo_list);

#ifdef ATIHACK_812
	// ATI bugs
	// http://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=812
	check = gtk_check_button_new_with_label ("ATI cards with broken drivers - bug #802");
	gtk_widget_show(check);
	gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
	AddDialogData(check, &m_bGlATIHack, DLG_CHECK_BOOL);
#endif
  
  // Add the page to the notebook
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageframe, preflabel);
  
  /******** 3D Camera view group *********/
  preflabel = gtk_label_new("3D View");
  gtk_widget_show(preflabel);
  pageframe = gtk_frame_new("3D View");
  gtk_container_set_border_width(GTK_CONTAINER(pageframe), 5);
  gtk_widget_show(pageframe);
  vbox = gtk_vbox_new(FALSE, 5);
  gtk_widget_show(vbox);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
  gtk_container_add(GTK_CONTAINER(pageframe), vbox);

  // Directional velocity (Movement Velocity)
  // label container
  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox2, FALSE, FALSE, 0);
  
  // label
  label = gtk_label_new("Movement Velocity");
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_widget_show(label);
  gtk_box_pack_start(GTK_BOX(hbox2), label, FALSE, FALSE, 0);
  
  // adjustment
  adj = gtk_adjustment_new(100, 50, 300, 1, 10, 10);
  AddDialogData(adj, &m_nMoveSpeed, DLG_ADJ_INT);
  
  // scale
  scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
  gtk_widget_show(scale);
  gtk_box_pack_start(GTK_BOX (vbox), scale, FALSE, TRUE, 2);
  
  gtk_scale_set_draw_value (GTK_SCALE (scale), TRUE);
  
  // Angular velocity (Rotational Velocity)
  // label container
  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox2, FALSE, FALSE, 0);
  
  // label
  label = gtk_label_new ("Rotational Velocity");
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_widget_show (label);
  gtk_box_pack_start(GTK_BOX(hbox2), label, FALSE, FALSE, 0);
  
  // adjustment
  adj = gtk_adjustment_new (3, 1, 180, 1, 10, 10); // value, low, high, step, page_step, page_size
  AddDialogData (adj, &m_nAngleSpeed, DLG_ADJ_INT);
  
  // scale
  scale = gtk_hscale_new (GTK_ADJUSTMENT (adj));
  gtk_widget_show (scale);
  gtk_box_pack_start (GTK_BOX (vbox), scale, FALSE, TRUE, 2);
  gtk_scale_set_draw_value (GTK_SCALE (scale), TRUE);  

  // Text under the velocity sliders
  // container
  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox2, FALSE, FALSE, 0);
  
  // label
  label = gtk_label_new ("slow");
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
  
  // label
  label = gtk_label_new ("fast");
  gtk_widget_show (label);
  gtk_box_pack_end (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
  
  // Allow drag to select multiple faces/brushes
  // container
  table = gtk_table_new(2, 1, FALSE);
  gtk_widget_show(table);
  gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table), 5);

  label = gtk_label_new ("Use paint-select in camera view:");
  gtk_widget_show (label);
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
                  (GtkAttachOptions) (0),
                  (GtkAttachOptions) (0), 0, 0);

  combo_list = NULL;
  combo_list = g_list_append (combo_list, (void *)"No");
  combo_list = g_list_append (combo_list, (void *)"Yes");
  combo_list = g_list_append (combo_list, (void *)"Yes (Classic Key Setup)");

  combo = gtk_combo_new ();
  gtk_combo_set_popdown_strings (GTK_COMBO (combo), combo_list);
  gtk_widget_show (combo);
  gtk_table_attach(GTK_TABLE(table), combo, 1, 2, 0, 1,
                  (GtkAttachOptions) (GTK_FILL),
                  (GtkAttachOptions) (0), 0, 0);
  gtk_entry_set_editable (GTK_ENTRY (GTK_COMBO (combo)->entry), FALSE);
  AddDialogData (combo, &m_nCamDragMultiSelect, DLG_COMBO_INT);  

  // Freelook in Camera view
  check = gtk_check_button_new_with_label ("Freelook in Camera view");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (GTK_BIN (check)->child), GTK_JUSTIFY_LEFT);
  AddDialogData (check, &m_bCamFreeLook, DLG_CHECK_BOOL);

  // Freelook in Camera view w/ forward & back strafing instead of up and down looking
  check = gtk_check_button_new_with_label ("Freelook strafes Forward and Back");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (GTK_BIN (check)->child), GTK_JUSTIFY_LEFT);
  AddDialogData (check, &m_bCamFreeLookStrafe, DLG_CHECK_BOOL);

  // Invert mouse in freelook
  check = gtk_check_button_new_with_label ("Invert mouse in freelook");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (GTK_BIN (check)->child), GTK_JUSTIFY_LEFT);
  AddDialogData (check, &m_bCamInverseMouse, DLG_CHECK_BOOL);

  // Discrete movement
  check = gtk_check_button_new_with_label ("Discrete movement");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (GTK_BIN (check)->child), GTK_JUSTIFY_LEFT);
  AddDialogData (check, &m_bCamDiscrete, DLG_CHECK_BOOL);

  // Update XY views on camera move
  check = gtk_check_button_new_with_label ("Update XY views on camera move");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (GTK_BIN (check)->child), GTK_JUSTIFY_LEFT);
  AddDialogData (check, &m_bCamXYUpdate, DLG_CHECK_BOOL);

  // Add the page to the notebook
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageframe, preflabel);

  /******** Texture group *********/
  preflabel = gtk_label_new("Textures");
  gtk_widget_show(preflabel);
  pageframe = gtk_frame_new("Textures");
  gtk_container_set_border_width(GTK_CONTAINER(pageframe), 5);
  gtk_widget_show(pageframe);
  vbox = gtk_vbox_new(FALSE, 6);
  gtk_widget_show(vbox);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
  gtk_container_add(GTK_CONTAINER(pageframe), vbox);
  
  // Texture quality slider
  // label
  label = gtk_label_new ("Texture quality");
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  
  // adjustment
  adj = gtk_adjustment_new (0, 0, 4, 1, 1, 1);
  AddDialogData (adj, &m_nLatchedTextureQuality, DLG_ADJ_INT);
  
  // scale
  scale = gtk_hscale_new (GTK_ADJUSTMENT (adj));
  gtk_widget_show (scale);
  gtk_box_pack_start (GTK_BOX (vbox), scale, FALSE, FALSE, 0);
  gtk_scale_set_draw_value (GTK_SCALE (scale), FALSE);

  // text under the texture slider
  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox2, FALSE, FALSE, 0);
  label = gtk_label_new ("low");
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
  label = gtk_label_new ("high");
  gtk_widget_show (label);
  gtk_box_pack_end (GTK_BOX (hbox2), label, FALSE, FALSE, 0);

  // texture subsets
  check = gtk_check_button_new_with_label ("Texture subsets");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bTextureWindow, DLG_CHECK_BOOL);
  
  // texture scrollbar
  check = gtk_check_button_new_with_label ("Texture scrollbar");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bTextureScrollbar, DLG_CHECK_BOOL);

  // texture increment matches grid
  check = gtk_check_button_new_with_label ("Tex increment matches grid");
  gtk_widget_show (check);
  gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bSnapTToGrid, DLG_CHECK_BOOL);

  // RIANT
  // Texture compression choice label
  // container
  table = gtk_table_new(2, 1, FALSE);
  gtk_widget_show(table);
  gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table), 5);

  label = gtk_label_new ("Texture Compression (if available):");
  gtk_widget_show (label);
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
                  (GtkAttachOptions) (0),
                  (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);

  // Texture compression choice label
  combo_list = NULL;
  // NONE will always be in pos 0
  combo_list = g_list_append (combo_list, (void *)"None");

  // if OpenGL compression is enabled it will always be
  // in pos 1
  if (g_qeglobals.m_bOpenGLCompressionSupported)
  {
    combo_list = g_list_append (combo_list, (void *)"OpenGL ARB");
  }

  // If S3 is enabled offer all 3 valid compression schemes in RGBA
  if (g_qeglobals.m_bS3CompressionSupported)
  {
    combo_list = g_list_append (combo_list, (void *)"S3TC DXT1");
    combo_list = g_list_append (combo_list, (void *)"S3TC DXT3");
    combo_list = g_list_append (combo_list, (void *)"S3TC DXT5");
  }

  combo = gtk_combo_new ();
  gtk_combo_set_popdown_strings (GTK_COMBO (combo), combo_list);
  gtk_widget_show (combo);
  gtk_table_attach(GTK_TABLE(table), combo, 1, 2, 0, 1,
                  (GtkAttachOptions) (GTK_FILL),
                  (GtkAttachOptions) (0), 0, 0);
  gtk_entry_set_editable (GTK_ENTRY (GTK_COMBO (combo)->entry), FALSE);
  AddDialogData (combo, &m_nTextureCompressionFormat, DLG_COMBO_INT);
  g_list_free (combo_list);

  // container
  table = gtk_table_new(2, 1, FALSE);
  gtk_widget_show(table);
  gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table), 5);

  // Startup shaders
  // label
  label = gtk_label_new ("Startup Shaders:");
  gtk_widget_show (label);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
  gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
                  (GtkAttachOptions) (0),
                  (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);
  
  // combo list
  combo_list = NULL;
  combo_list = g_list_append (combo_list, (void *)"None");
  if (g_pGameDescription->mGameFile == "jk2.game" || g_pGameDescription->mGameFile == "ja.game")
    combo_list = g_list_append (combo_list, (void *)"System");
  else if (g_pGameDescription->mGameFile == "sof2.game")
    combo_list = g_list_append (combo_list, (void *)"Tools");
  else
    combo_list = g_list_append (combo_list, (void *)"Common");
  combo_list = g_list_append (combo_list, (void *)"All");
  combo = gtk_combo_new ();
  gtk_combo_set_popdown_strings (GTK_COMBO (combo), combo_list);
  gtk_widget_show (combo);
  gtk_table_attach(GTK_TABLE(table), combo, 1, 2, 0, 1,
                  (GtkAttachOptions) (GTK_FILL),
                  (GtkAttachOptions) (0), 0, 0);
  gtk_entry_set_editable (GTK_ENTRY (GTK_COMBO (combo)->entry), FALSE);
  AddDialogData (combo, &m_nLatchedShader, DLG_COMBO_INT);
  g_list_free (combo_list);
  
  // Add the page to the notebook
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageframe, preflabel);
  
  /******** Layout group *********/
  preflabel = gtk_label_new("Layout");
  gtk_widget_show(preflabel);
  pageframe = gtk_frame_new("Layout");
  gtk_container_set_border_width(GTK_CONTAINER(pageframe), 5);
  gtk_widget_show(pageframe);
  vbox = gtk_vbox_new(FALSE, 5);
  gtk_widget_show(vbox);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
  gtk_container_add(GTK_CONTAINER(pageframe), vbox);

  // View types
  // table
  table = gtk_table_new (2, 4, FALSE);
  gtk_widget_show (table);
  gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table), 5);

  // view type 1
  pixmap = new_pixmap (g_pParentWnd->m_pWidget, "window1.bmp");
  gtk_widget_show (pixmap);
  gtk_table_attach (GTK_TABLE (table), pixmap, 0, 1, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  // view type 2
  pixmap = new_pixmap (g_pParentWnd->m_pWidget, "window2.bmp");
  gtk_widget_show (pixmap);
  gtk_table_attach (GTK_TABLE (table), pixmap, 1, 2, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  // view type 3
  pixmap = new_pixmap (g_pParentWnd->m_pWidget, "window3.bmp");
  gtk_widget_show (pixmap);
  gtk_table_attach (GTK_TABLE (table), pixmap, 2, 3, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  // view type 4
  pixmap = new_pixmap (g_pParentWnd->m_pWidget, "window4.bmp");
  gtk_widget_show (pixmap);
  gtk_table_attach (GTK_TABLE (table), pixmap, 3, 4, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  // view type 1 selector
  radio = gtk_radio_button_new (NULL);
  gtk_widget_show (radio);
  gtk_table_attach (GTK_TABLE (table), radio, 0, 1, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  // view type 2 selector
  radio = gtk_radio_button_new_from_widget (GTK_RADIO_BUTTON (radio));
  gtk_widget_show (radio);
  gtk_table_attach (GTK_TABLE (table), radio, 1, 2, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  // view type 3 selector
  radio = gtk_radio_button_new_from_widget (GTK_RADIO_BUTTON (radio));
  gtk_widget_show (radio);
  gtk_table_attach (GTK_TABLE (table), radio, 2, 3, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  // view type 4 selector
  radio = gtk_radio_button_new_from_widget (GTK_RADIO_BUTTON (radio));
  gtk_widget_show (radio);
  gtk_table_attach (GTK_TABLE (table), radio, 3, 4, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  AddDialogData (radio, &m_nLatchedView, DLG_RADIO_INT);
  
  // Floating Z window
  check = gtk_check_button_new_with_label ("Floating Z Window");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bLatchedFloatingZ, DLG_CHECK_BOOL);
  
  // show menu tear-off seperators
  check = gtk_check_button_new_with_label ("Detachable Menus");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bLatchedDetachableMenus, DLG_CHECK_BOOL);

  if (!g_pGameDescription->mNoPatch)
  {
    // show patch toolbar
    check = gtk_check_button_new_with_label ("Patch Toolbar");
    gtk_widget_show (check);
    gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
    g_object_set_data (G_OBJECT (dialog), "check_patchtoolbar", check); // Allow to be disabled for Q1/Q2
    AddDialogData (check, &m_bLatchedPatchToolbar, DLG_CHECK_BOOL);
  }

  // use wide toolbar
  check = gtk_check_button_new_with_label ("Wide Toolbar");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bLatchedWideToolbar, DLG_CHECK_BOOL);

  // use plugin toolbar
  check = gtk_check_button_new_with_label ("Plugin Toolbar");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bLatchedPluginToolbar, DLG_CHECK_BOOL);

#ifdef _WIN32
  // win32 file dialog
  check = gtk_check_button_new_with_label ("Use win32 file load dialog");
  gtk_widget_show (check);
  // gtk_container_add (GTK_CONTAINER (vbox), check);
  gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bNativeGUI, DLG_CHECK_BOOL);

  // position on primary monitor
  check = gtk_check_button_new_with_label ("Start on Primary Monitor");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  g_object_set_data (G_OBJECT (dialog), "check_startonprimary", check);
  gtk_signal_connect( GTK_OBJECT (check), "clicked", GTK_SIGNAL_FUNC(UpdateSensitivity), this );
  AddDialogData (check, &m_bStartOnPrimMon, DLG_CHECK_BOOL);
#endif  

  // Add the page to the notebook
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageframe, preflabel);
  
  /******** Mouse group *********/
  preflabel = gtk_label_new("Mouse");
  gtk_widget_show(preflabel);
  pageframe = gtk_frame_new("Mouse");
  gtk_container_set_border_width(GTK_CONTAINER(pageframe), 5);
  gtk_widget_show(pageframe);
  vbox = gtk_vbox_new(FALSE, 5);
  gtk_widget_show(vbox);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
  gtk_container_add(GTK_CONTAINER(pageframe), vbox);
  
  // Buttons
  // container
  hbox2 = gtk_hbox_new (FALSE, 5);
  gtk_widget_show (hbox2);
  gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 0);

  // 2 button radio
  radio = gtk_radio_button_new_with_label (NULL, "2 button");
  gtk_widget_show (radio);
  gtk_box_pack_start (GTK_BOX (hbox2), radio, FALSE, FALSE, 0);

  // 3 button radio
  radio = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (radio), "3 button");
  gtk_widget_show (radio);
  gtk_box_pack_start (GTK_BOX (hbox2), radio, FALSE, FALSE, 0);
  AddDialogData (radio, &m_nMouse, DLG_RADIO_INT);

  // right click to drop entity
  check = gtk_check_button_new_with_label ("Right click to drop entities");
  gtk_widget_show (check);
  gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bRightClick, DLG_CHECK_BOOL);

  // Mouse chaser (and this does what?)
  check = gtk_check_button_new_with_label ("Mouse chaser");
  gtk_widget_show (check);
  gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bChaseMouse, DLG_CHECK_BOOL);

  // Alt + multi-drag
  check = gtk_check_button_new_with_label ("ALT + multi-drag");
  gtk_widget_show (check);
  gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bALTEdge, DLG_CHECK_BOOL);

  // Mouse wheel increments
  // container
  hbox2 = gtk_hbox_new (FALSE, 5);
  gtk_widget_show (hbox2);
  gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 0);
  
  // label
  label = gtk_label_new ("Wheel Mouse inc:");
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
  
  // entry
  entry = gtk_entry_new ();
  gtk_widget_show (entry);
  gtk_widget_set_usize (entry, 40, -2);
  gtk_box_pack_start (GTK_BOX (hbox2), entry, FALSE, FALSE, 0);
  AddDialogData (entry, &m_nWheelInc, DLG_ENTRY_INT);  

  // Add the page to the notebook
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageframe, preflabel);
  
  /******** Editing group *********/
  preflabel = gtk_label_new("Editing");
  gtk_widget_show(preflabel);
  pageframe = gtk_frame_new("Editing");
  gtk_container_set_border_width(GTK_CONTAINER(pageframe), 5);
  gtk_widget_show(pageframe);
  vbox = gtk_vbox_new(FALSE, 5);
  gtk_widget_show(vbox);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
  gtk_container_add(GTK_CONTAINER(pageframe), vbox);
  
  // Vertex editing splits faces
  check = gtk_check_button_new_with_label ("Vertex editing splits face");
  gtk_widget_show (check);
  gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bVertexSplit, DLG_CHECK_BOOL);

  // Fix target/targetname collisions
  check = gtk_check_button_new_with_label ("Fix target/targetname collisions");
  gtk_widget_show (check);
  gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bDoTargetFix, DLG_CHECK_BOOL);
  
  // Clipper tool uses caulk
  check = gtk_check_button_new_with_label ("Clipper tool uses caulk");
  gtk_widget_show (check);
  gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bClipCaulk, DLG_CHECK_BOOL);

  // Don't clamp plane points
  check = gtk_check_button_new_with_label ("Don't clamp plane points");
  gtk_widget_show (check);
  gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bNoClamp, DLG_CHECK_BOOL);

  // Select patch by bounding box
  check = gtk_check_button_new_with_label ("Select patches by bounding box");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bPatchBBoxSelect, DLG_CHECK_BOOL);  
  
  // Rotation increment
  // container
  table = gtk_table_new (2, 3, FALSE);
  gtk_widget_show (table);
  gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table), 5);
  
  // label
  label = gtk_label_new ("Rotation increment:");
  gtk_widget_show (label);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1,
                   (GtkAttachOptions) (0),
                   (GtkAttachOptions) (0), 0, 0);
                   
  // entry
  entry = gtk_entry_new ();
  gtk_widget_show (entry);
  gtk_widget_set_usize (entry, 60, -2);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1,
                   (GtkAttachOptions) (GTK_FILL),
                   (GtkAttachOptions) (0), 0, 0);
  AddDialogData (entry, &m_nRotation, DLG_ENTRY_INT);
  
  // Undo levels
  // label
  label = gtk_label_new ("Undo Levels:");
  gtk_widget_show (label);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2,
                   (GtkAttachOptions) (0),
                   (GtkAttachOptions) (0), 0, 0);
                   
  // spinner (allows undo levels to be set to zero)
  spin = gtk_spin_button_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 0, 64, 1, 10, 10)), 1, 0); 
  gtk_widget_show (spin);
  gtk_table_attach (GTK_TABLE (table), spin, 1, 2, 1, 2,
                   (GtkAttachOptions) (GTK_FILL),
                   (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_usize (spin, 60, -2);
  AddDialogData (spin, &m_nUndoLevels, DLG_SPIN_INT);

  // Patch subdivisions
  // label
  label = gtk_label_new ("Patch subdivisions:");
  gtk_widget_show (label);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3,
                   (GtkAttachOptions) (0),
                   (GtkAttachOptions) (0), 0, 0);
                   
  // entry (spinner perhaps? [2-16])
  entry = gtk_entry_new ();
  gtk_widget_show (entry);
  gtk_widget_set_usize (entry, 60, -2);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 2, 3,
                   (GtkAttachOptions) (GTK_FILL),
                   (GtkAttachOptions) (0), 0, 0);
  AddDialogData (entry, &m_nSubdivisions, DLG_ENTRY_INT);

  // Add the page to the notebook
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageframe, preflabel);
  
  /******** Save/Load group *********/
  preflabel = gtk_label_new("Startup/Auto save");
  gtk_widget_show(preflabel);
  pageframe = gtk_frame_new("Startup/Auto save");
  gtk_container_set_border_width(GTK_CONTAINER(pageframe), 5);
  gtk_widget_show(pageframe);
  vbox = gtk_vbox_new(FALSE, 5);
  gtk_widget_show(vbox);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
  gtk_container_add(GTK_CONTAINER(pageframe), vbox);

  // Snapshots
  check = gtk_check_button_new_with_label ("Snapshots");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bSnapShots, DLG_CHECK_BOOL);
  
  // load last project on open
  check = gtk_check_button_new_with_label ("Load last project on open");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bLoadLast, DLG_CHECK_BOOL);

  // load last map on open
  check = gtk_check_button_new_with_label ("Load last map on open");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bLoadLastMap, DLG_CHECK_BOOL);

  // Auto save..
  // container
  hbox2 = gtk_hbox_new (FALSE, 5);
  gtk_widget_show (hbox2);
  gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox2), 0);
  
  // label
  check = gtk_check_button_new_with_label ("Auto save every");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (hbox2), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bAutoSave, DLG_CHECK_BOOL);
  
  // spinner
  spin = gtk_spin_button_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 1, 60, 1, 10, 10)), 1, 0);
  gtk_widget_show (spin);
  gtk_box_pack_start (GTK_BOX (hbox2), spin, FALSE, FALSE, 0);
  gtk_widget_set_usize (spin, 60, -2);
  AddDialogData (spin, &m_nAutoSave, DLG_SPIN_INT);
  
  // label
  label = gtk_label_new ("minutes");
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
  
  // Add the page to the notebook
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageframe, preflabel);
  
  /******** Paths group *********/
  preflabel = gtk_label_new("Paths");
  gtk_widget_show(preflabel);
  pageframe = gtk_frame_new("Paths");
  gtk_container_set_border_width(GTK_CONTAINER(pageframe), 5);
  gtk_widget_show(pageframe);
  vbox = gtk_vbox_new(FALSE, 5);
  gtk_widget_show(vbox);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
  gtk_container_add(GTK_CONTAINER(pageframe), vbox);

  // prefab path
  // table
  table = gtk_table_new (3, 3, FALSE);
  gtk_widget_show (table);
  gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table), 5);
  
  // label
  label = gtk_label_new ("Prefab path:");
  gtk_widget_show (label);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1,
                   (GtkAttachOptions) (0),
                   (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);
  
  // path entry
  entry = gtk_entry_new ();
  gtk_widget_show (entry);
  gtk_widget_set_usize(GTK_WIDGET(entry), 240, -2);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1,
                   (GtkAttachOptions) (GTK_FILL),
                   (GtkAttachOptions) (0), 1, 0);
  AddDialogData (entry, &m_strPrefabPath, DLG_ENTRY_TEXT);

  // http://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=805
#if 0  
  // browse button
  button = gtk_button_new_with_label ("...");
  gtk_widget_show (button);
  gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (OnBtnBrowseprefab), this);
  gtk_table_attach (GTK_TABLE (table), button, 2, 3, 0, 1,
                   (GtkAttachOptions) (0),
                   (GtkAttachOptions) (0), 0, 0);
#endif

  // User ini path
  // label
  label = gtk_label_new ("User INI path:");
  gtk_widget_show (label);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2,
                   (GtkAttachOptions) (0),
                   (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);
  
  // user ini path entry
  entry = gtk_entry_new ();
  gtk_widget_show (entry);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2,
                   (GtkAttachOptions) (GTK_FILL),
                   (GtkAttachOptions) (0), 1, 0);
  AddDialogData (entry, &m_strUserPath, DLG_ENTRY_TEXT);
  
  // user ini browse button
  button = gtk_button_new_with_label ("...");
  gtk_widget_show (button);
  gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (OnBtnBrowseuserini), this);
  gtk_table_attach (GTK_TABLE (table), button, 2, 3, 1, 2,
                   (GtkAttachOptions) (0),
                   (GtkAttachOptions) (0), 0, 0);

  // Add the page to the notebook
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageframe, preflabel);
  
  /******** Misc group *********/
  preflabel = gtk_label_new("Misc");
  gtk_widget_show(preflabel);
  pageframe = gtk_frame_new("Misc");
  gtk_container_set_border_width(GTK_CONTAINER(pageframe), 5);
  gtk_widget_show(pageframe);
  vbox = gtk_vbox_new(FALSE, 5);
  gtk_widget_show(vbox);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
  gtk_container_add(GTK_CONTAINER(pageframe), vbox);
  
  // Light drawing
  check = gtk_check_button_new_with_label ("Light drawing");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &m_bNewLightDraw, DLG_CHECK_BOOL);

  // Light radiuses
  // container
  table = gtk_table_new(2, 1, FALSE);
  gtk_widget_show(table);
  gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table), 5);

  label = gtk_label_new ("Light radiuses:");
  gtk_widget_show (label);
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
                  (GtkAttachOptions) (0),
                  (GtkAttachOptions) (0), 0, 0);

  combo_list = NULL;
  combo_list = g_list_append (combo_list, (void *)"Disabled");
  combo_list = g_list_append (combo_list, (void *)"True Q3Map2 Style");
  combo_list = g_list_append (combo_list, (void *)"Classic Style");

  combo = gtk_combo_new ();
  gtk_combo_set_popdown_strings (GTK_COMBO (combo), combo_list);
  gtk_widget_show (combo);
  gtk_table_attach(GTK_TABLE(table), combo, 1, 2, 0, 1,
                  (GtkAttachOptions) (GTK_FILL),
                  (GtkAttachOptions) (0), 0, 0);
  gtk_entry_set_editable (GTK_ENTRY (GTK_COMBO (combo)->entry), FALSE);
  AddDialogData (combo, &m_nLightRadiuses, DLG_COMBO_INT);

#ifdef _WIN32
  check = gtk_check_button_new_with_label ("Use win32 file associations to open text files instead of builtin editor");
  gtk_widget_show(check);
  gtk_box_pack_start(GTK_BOX (vbox), check, FALSE, FALSE, 0);
  AddDialogData (check, &g_PrefsDlg.m_bUseWin32Editor, DLG_CHECK_BOOL);
#else
  // use custom shader editor
  check = gtk_check_button_new_with_label ("Use Custom Shader Editor");
  gtk_widget_show(check);
  gtk_box_pack_start(GTK_BOX (vbox), check, FALSE, FALSE, 0);
  gtk_signal_connect( GTK_OBJECT (check), "clicked", GTK_SIGNAL_FUNC(UpdateEditorSensitivity), this);
  g_object_set_data (G_OBJECT(dialog), "check_customeditor", check);
  AddDialogData (check, &g_PrefsDlg.m_bUseCustomEditor, DLG_CHECK_BOOL);
  
  // custom shader editor executable
  // table
  table = gtk_table_new (3, 1, FALSE);
  gtk_widget_show (table);
  gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table), 5);
  
  // label
  label = gtk_label_new("Custom Editor Command");
  gtk_widget_show(label);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1,
                   (GtkAttachOptions) (0),
                   (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);
  g_object_set_data (G_OBJECT(dialog), "label_customeditor", label);
  gtk_widget_set_sensitive (label, g_PrefsDlg.m_bUseCustomEditor);
  
  // custom editor command entry
  entry = gtk_entry_new ();
  gtk_widget_show (entry);
  gtk_widget_set_usize(GTK_WIDGET(entry), 240, -2);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1,
                   (GtkAttachOptions) (GTK_FILL),
                   (GtkAttachOptions) (0), 1, 0);
  AddDialogData (entry, &m_strEditorCommand, DLG_ENTRY_TEXT);
  gtk_widget_set_sensitive (entry, g_PrefsDlg.m_bUseCustomEditor);
  g_object_set_data (G_OBJECT(dialog), "entry_customeditor", entry);
  
  // browse button
  button = gtk_button_new_with_label ("...");
  gtk_widget_show (button);
  gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (OnBtnBrowseEditor), this);
  gtk_table_attach (GTK_TABLE (table), button, 2, 3, 0, 1,
                   (GtkAttachOptions) (0),
                   (GtkAttachOptions) (0), 0, 0);
  g_object_set_data (G_OBJECT(dialog), "button_customeditor", button);
  gtk_widget_set_sensitive (button, g_PrefsDlg.m_bUseCustomEditor);
#endif

  // Add the page to the notebook
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageframe, preflabel);

  /******** BSP Monitoring group *********/
  // this is never displayed if the plugin isn't available
  preflabel = gtk_label_new("BSP Monitoring");
  gtk_widget_show(preflabel);
  pageframe = gtk_frame_new("BSP Monitoring");
  gtk_container_set_border_width(GTK_CONTAINER(pageframe), 5);
  gtk_widget_show(pageframe);
  vbox = gtk_vbox_new(FALSE, 5);
  gtk_widget_show(vbox);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
  gtk_container_add(GTK_CONTAINER(pageframe), vbox);

  // Enable BSP process monitoring
  check = gtk_check_button_new_with_label ("Enable BSP process monitoring");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  g_object_set_data (G_OBJECT (dialog), "check_monitorbsp", check);
  gtk_signal_connect( GTK_OBJECT (check), "clicked", GTK_SIGNAL_FUNC(UpdateSensitivity), this );
  AddDialogData (check, &g_PrefsDlg.m_bWatchBSP, DLG_CHECK_BOOL);

  // Stop on leak
  check = gtk_check_button_new_with_label ("Stop compilation on leak");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  g_object_set_data (G_OBJECT (dialog), "check_leakstop", check);
  AddDialogData (check, &g_PrefsDlg.m_bLeakStop, DLG_CHECK_BOOL);

  // engine after compile
  check = gtk_check_button_new_with_label ("Run engine after compile");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  g_object_set_data (G_OBJECT (dialog), "check_runengine", check);
  gtk_signal_connect( GTK_OBJECT (check), "clicked", GTK_SIGNAL_FUNC(UpdateSensitivity), this );
  AddDialogData( check, &g_PrefsDlg.m_bRunQuake, DLG_CHECK_BOOL );  
  
  // sleep mode when running engine
  check = gtk_check_button_new_with_label ("Activate sleep mode when running the engine");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  g_object_set_data (G_OBJECT (dialog), "check_sleep", check);
  AddDialogData( check, &g_PrefsDlg.m_bDoSleep, DLG_CHECK_BOOL );
  
  // use q3map2's texture projection
  check = gtk_check_button_new_with_label ("Texturing compatible with q3map2");
  gtk_widget_show (check);
  gtk_box_pack_start (GTK_BOX (vbox), check, FALSE, FALSE, 0);
  g_object_set_data (G_OBJECT (dialog), "check_q3map2", check);
  AddDialogData( check, &g_PrefsDlg.m_bQ3Map2Texturing, DLG_CHECK_BOOL );

  // Add the page to the notebook
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageframe, preflabel);

  gtk_notebook_set_page(GTK_NOTEBOOK(notebook), PTAB_FRONT);

  return;
}

// end new prefs dialog

void PrefsDlg::LoadTexdefPref(texdef_t* pTexdef, char* pName)
{
  char buffer[256];

  memset(pTexdef, 0, sizeof(texdef_t));

  sprintf(buffer, "%s%s", pName, TD_SCALE1_KEY);
  mLocalPrefs.GetPref(buffer, &pTexdef->scale[0],   0.5f);      

  sprintf(buffer, "%s%s", pName, TD_SCALE2_KEY);
  mLocalPrefs.GetPref(buffer, &pTexdef->scale[1],   0.5f);

  sprintf(buffer, "%s%s", pName, TD_SHIFT1_KEY);
  mLocalPrefs.GetPref(buffer, &pTexdef->shift[0],   8.f);

  sprintf(buffer, "%s%s", pName, TD_SHIFT2_KEY);
  mLocalPrefs.GetPref(buffer, &pTexdef->shift[1],   8.f);

  sprintf(buffer, "%s%s", pName, TD_ROTATE_KEY);
  mLocalPrefs.GetPref(buffer, &pTexdef->rotate,     45);
}

void PrefsDlg::UpdateTextureCompression()
{
  // if OpenGL is not ready yet, don't do anything
  if (!g_qeglobals.m_bOpenGLReady) {
    Sys_Printf("OpenGL not ready - postpone texture compression capability check\n");
    return;
  }
  
  if (g_qeglobals.bTextureCompressionSupported)
  {
    if (m_nTextureCompressionFormat >= 2 && !g_qeglobals.m_bS3CompressionSupported)
    {
      Sys_Printf("Inconsistant pref setting for texture compression (%d), rolling back\n", m_nTextureCompressionFormat);
      m_nTextureCompressionFormat = 1; // if this is not supported either, see below
    }
    if (m_nTextureCompressionFormat == 1 && !g_qeglobals.m_bOpenGLCompressionSupported)
    {
      Sys_Printf("Inconsistant pref setting for texture compression (GL_COMPRESSED_RGBA), rolling back\n");
      m_nTextureCompressionFormat = 0;
    }
    switch (m_nTextureCompressionFormat)
    {
    case (0):
      {
        g_qeglobals.texture_components = GL_RGBA;
        Sys_Printf("texture compression disabled by preferences settings\n");
        break;
      }
    case (1):
      {
        g_qeglobals.texture_components = GL_COMPRESSED_RGBA;
        Sys_Printf("OpenGL texture compression enabled\n");
        break;
      }
    case (2):
      {
        g_qeglobals.texture_components = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        Sys_Printf("S3TC DXT1 texture compression enabled\n");
        break;
      }
    case (3):
      {
        g_qeglobals.texture_components = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        Sys_Printf("S3TC DXT3 texture compression enabled\n");
        break;
      }
    case (4):
      {
        g_qeglobals.texture_components = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        Sys_Printf("S3TC DXT5 texture compression enabled\n");
        break;
      }
    }
  }
  else
  {
    Sys_Printf("texture compression is not supported by your current graphic card/drivers\n");
    g_qeglobals.texture_components = GL_RGBA;
    m_nTextureCompressionFormat = 0;
  }
}

#ifdef ATIHACK_812
void PrefsDlg::UpdateATIHack() {
  // if OpenGL is not ready yet, don't do anything
  if (!g_qeglobals.m_bOpenGLReady) {
    Sys_Printf("OpenGL not ready - postpone ATI bug workaround setup\n");
    return;
  }

  if (m_bGlATIHack) {
    qglCullFace = &qglCullFace_ATIHack;
    qglDisable = &qglDisable_ATIHack;
    qglEnable = &qglEnable_ATIHack;
    qglPolygonMode = &qglPolygonMode_ATIHack;
    Sys_Printf("ATI bug workaround enabled\n");
  } else {
    qglCullFace = qglCullFace_real;
    qglDisable = qglDisable_real;
    qglEnable = qglEnable_real;
    qglPolygonMode = qglPolygonMode_real;
    Sys_Printf("ATI bug workaround disabled\n");
  }
}
#endif

// TTimo: m_strEnginePath has a special status, if not found in registry we need to 
// initiliaze it for sure. It is not totally failsafe but we can use the same
// code than in q3map, expecting to find some "quake" above us. If not, we prompt
// for the engine executable path
void PrefsDlg::LoadPrefs ()
{
  int i;

  // first things first, load prefs from global prefs
  mGamesDialog.LoadPrefs();

  // if we already have a document loaded, we will free and reload from file
  if (mLocalPrefs.InUse()) {
    mLocalPrefs.Clear();
  }

  // load local.pref file
  mLocalPrefs.ReadXMLFile(m_inipath->str);

  mLocalPrefs.GetPref(PATCHSHOWBOUNDS_KEY,  &g_bPatchShowBounds,  FALSE);
  mLocalPrefs.GetPref(MOUSE_KEY,            &m_nMouse,            MOUSE_DEF);
  m_nMouseButtons = m_nMouse ? 3 : 2;
  
	// project file
	// if it's not found here, mainframe.cpp will take care of finding one
  mLocalPrefs.GetPref(LASTPROJ_KEY, &m_strLastProject, "");
  mLocalPrefs.GetPref(LASTPROJVER_KEY, &m_nLastProjectVer, -1);

	// prefab path
  // NOTE TTimo: I'm not sure why this is in prefs
  // should probably be a project setting
  // and I'm not sure that we really have a way to set this reliably either
  CString strPrefab;
  strPrefab = g_qeglobals.m_strHomeGame.GetBuffer();
  strPrefab += g_pGameDescription->mBaseGame.GetBuffer();
  strPrefab += "/prefabs/";
  mLocalPrefs.GetPref(PREFAB_KEY, &m_strPrefabPath, strPrefab);

  mLocalPrefs.GetPref(LASTLIGHTINTENSITY_KEY, &m_iLastLightIntensity, 300);
  mLocalPrefs.GetPref(TLOCK_KEY,              &m_bTextureLock,        TLOCK_DEF);
  mLocalPrefs.GetPref(RLOCK_KEY,              &m_bRotateLock,         TLOCK_DEF);
  mLocalPrefs.GetPref(LASTMAP_KEY,            &m_strLastMap,          "");
  mLocalPrefs.GetPref(LOADLAST_KEY,           &m_bLoadLast,           LOADLAST_DEF);
  mLocalPrefs.GetPref(BSP_KEY,                &m_bInternalBSP,        FALSE);
  mLocalPrefs.GetPref(RCLICK_KEY,             &m_bRightClick,         TRUE);
  mLocalPrefs.GetPref(AUTOSAVE_KEY,           &m_bAutoSave,           TRUE);
  mLocalPrefs.GetPref(LOADLASTMAP_KEY,        &m_bLoadLastMap,        FALSE);
  mLocalPrefs.GetPref(TINYBRUSH_KEY,          &m_bCleanTiny,          FALSE);
  mLocalPrefs.GetPref(TINYSIZE_KEY,           &m_fTinySize,           0.5f);
  mLocalPrefs.GetPref(AUTOSAVETIME_KEY,       &m_nAutoSave,           5);
  mLocalPrefs.GetPref(SNAPSHOT_KEY,           &m_bSnapShots,          FALSE);
  mLocalPrefs.GetPref(MOVESPEED_KEY,          &m_nMoveSpeed,          100);
  mLocalPrefs.GetPref(ANGLESPEED_KEY,         &m_nAngleSpeed,         3);
  mLocalPrefs.GetPref(SETGAME_KEY,            &m_bSetGame,            FALSE);
  mLocalPrefs.GetPref(CAMXYUPDATE_KEY,        &m_bCamXYUpdate,        TRUE);
  mLocalPrefs.GetPref(CAMDRAGMULTISELECT_KEY, &m_nCamDragMultiSelect, TRUE);
  mLocalPrefs.GetPref(CAMFREELOOK_KEY,        &m_bCamFreeLook,        TRUE);
  mLocalPrefs.GetPref(CAMINVERSEMOUSE_KEY,    &m_bCamInverseMouse,    FALSE);
  mLocalPrefs.GetPref(CAMDISCRETE_KEY,        &m_bCamDiscrete,        TRUE);
  mLocalPrefs.GetPref(LIGHTDRAW_KEY,          &m_bNewLightDraw,       TRUE);
  mLocalPrefs.GetPref(CUBICCLIP_KEY,          &m_bCubicClipping,      TRUE);
  mLocalPrefs.GetPref(CUBICSCALE_KEY,         &m_nCubicScale,         13);
  mLocalPrefs.GetPref(ALTEDGE_KEY,            &m_bALTEdge,            FALSE);
  mLocalPrefs.GetPref(FACECOLORS_KEY,         &m_bFaceColors,         FALSE);
  mLocalPrefs.GetPref(XZVIS_KEY,              &m_bXZVis,              FALSE);
  mLocalPrefs.GetPref(YZVIS_KEY,              &m_bYZVis,              FALSE);
  mLocalPrefs.GetPref(ZVIS_KEY,               &m_bZVis,               FALSE);
  mLocalPrefs.GetPref(SIZEPAINT_KEY,          &m_bSizePaint,                  FALSE);
  mLocalPrefs.GetPref(DLLENTITIES_KEY,        &m_bDLLEntities,                FALSE);

  mLocalPrefs.GetPref(DETACHABLEMENUS_KEY,    &m_bLatchedDetachableMenus,            TRUE);
  m_bDetachableMenus = m_bLatchedDetachableMenus;
  
  if (g_pGameDescription->mNoPatch)
  {
    m_bPatchToolbar = false;
  }
  else
  {
    mLocalPrefs.GetPref(PATCHTOOLBAR_KEY,       &m_bLatchedPatchToolbar,               TRUE);
    m_bPatchToolbar = m_bLatchedPatchToolbar;
  }

  mLocalPrefs.GetPref(WIDETOOLBAR_KEY,        &m_bLatchedWideToolbar,                TRUE);
  m_bWideToolbar = m_bLatchedWideToolbar;
 
  mLocalPrefs.GetPref(PLUGINTOOLBAR_KEY, &m_bLatchedPluginToolbar, TRUE);
  m_bPluginToolbar = m_bLatchedPluginToolbar;

  mLocalPrefs.GetPref(WINDOW_KEY,             (int*)&m_nLatchedView,  WINDOW_DEF);
  m_nView = m_nLatchedView;

  mLocalPrefs.GetPref(FLOATINGZ_KEY,          &m_bLatchedFloatingZ,           FALSE);
  m_bFloatingZ = m_bLatchedFloatingZ;

  mLocalPrefs.GetPref(TEXTUREQUALITY_KEY,     &m_nLatchedTextureQuality,             3);
  m_nTextureQuality = m_nLatchedTextureQuality;

  mLocalPrefs.GetPref(LOADSHADERS_KEY,        &m_nLatchedShader,                     0);
  m_nShader = m_nLatchedShader;

  mLocalPrefs.GetPref(NOCLAMP_KEY,            &m_bNoClamp,                    FALSE);
  mLocalPrefs.GetPref(USERINI_KEY,            &m_strUserPath,                 "");
  mLocalPrefs.GetPref(ROTATION_KEY,           &m_nRotation,                   45);
  mLocalPrefs.GetPref(CHASEMOUSE_KEY,         &m_bChaseMouse,                 TRUE);
  mLocalPrefs.GetPref(ENTITYSHOW_KEY,         &m_nEntityShowState,            ENTITY_SKINNED_BOXED);

  // this will probably need to be 75 or 100 for Q1.
  mLocalPrefs.GetPref(TEXTURESCALE_KEY,       &m_nTextureScale,               50);

  // FIXME: Hydra - actually, this stuff is Q1,Q2 and HL specific.
  if ( (g_pGameDescription->mGameFile == "hl.game") )
  {
    // No BSP monitoring in the default compiler tools for Half-life (yet)
    mLocalPrefs.GetPref(WATCHBSP_KEY,           &m_bWatchBSP,                   FALSE);

    // Texture subset on by default (HL specific really, because of halflife.wad's size)
    mLocalPrefs.GetPref(TEXTURE_KEY,            &m_bTextureWindow,              TRUE);
  }
  else if ( ( g_pGameDescription->mGameFile == "q2.game" ) || ( g_pGameDescription->mGameFile == "heretic2.game" ) )
  {
    // BSP monitoring is implemented in Quake2 and Heretic2 tools
    mLocalPrefs.GetPref(WATCHBSP_KEY,           &m_bWatchBSP,                   TRUE);

    // Texture subset on by default (HL specific really, because of halflife.wad's size)
    mLocalPrefs.GetPref(TEXTURE_KEY,            &m_bTextureWindow,              TRUE);
  }
  else
  {
    mLocalPrefs.GetPref(WATCHBSP_KEY,           &m_bWatchBSP,                   WATCHBSP_DEF);
    mLocalPrefs.GetPref(TEXTURE_KEY,            &m_bTextureWindow,              FALSE);
  }


  mLocalPrefs.GetPref(TEXTURESCROLLBAR_KEY,   &m_bTextureScrollbar,           TRUE);
  mLocalPrefs.GetPref(DISPLAYLISTS_KEY,       &m_bDisplayLists,               TRUE);
  mLocalPrefs.GetPref(ANTIALIASEDLINES_KEY,   &m_bAntialiasedPointsAndLines,  FALSE);
  mLocalPrefs.GetPref(SWITCHCLIP_KEY,         &m_bSwitchClip,                 TRUE);
  mLocalPrefs.GetPref(SELWHOLEENTS_KEY,       &m_bSelectWholeEntities,        TRUE);
  mLocalPrefs.GetPref(SHOWSHADERS_KEY,        &m_bShowShaders,                TRUE);
  mLocalPrefs.GetPref(GLLIGHTING_KEY,         &m_bGLLighting,                 FALSE);
  mLocalPrefs.GetPref(NOSTIPPLE_KEY,          &m_bNoStipple,                  FALSE);
  mLocalPrefs.GetPref(UNDOLEVELS_KEY,         &m_nUndoLevels,                 30);
  mLocalPrefs.GetPref(VERTEXMODE_KEY,         &m_bVertexSplit,                TRUE);
  mLocalPrefs.GetPref(RUNQ2_KEY,              &m_bRunQuake,                   RUNQ2_DEF);
  mLocalPrefs.GetPref(LEAKSTOP_KEY,           &m_bLeakStop,                   TRUE);
  mLocalPrefs.GetPref(DOSLEEP_KEY,            &m_bDoSleep,                    FALSE);
  mLocalPrefs.GetPref(SELECTCURVES_KEY,       &m_bSelectCurves,               TRUE);
  mLocalPrefs.GetPref(SELECTMODELS_KEY,       &m_bSelectModels,               TRUE);
  mLocalPrefs.GetPref(SHADERLISTONLY_KEY,     &m_bTexturesShaderlistOnly,     FALSE);
  mLocalPrefs.GetPref(SUBDIVISIONS_KEY,       &m_nSubdivisions,               SUBDIVISIONS_DEF);
  mLocalPrefs.GetPref(CLIPCAULK_KEY,          &m_bClipCaulk,                  FALSE);
  mLocalPrefs.GetPref(SNAPTTOGRID_KEY,        &m_bSnapTToGrid,                FALSE);
  mLocalPrefs.GetPref(TARGETFIX_KEY,          &m_bDoTargetFix,                TRUE);
  mLocalPrefs.GetPref(WHEELINC_KEY,           &m_nWheelInc,                   64);
  mLocalPrefs.GetPref(PATCHBBOXSEL_KEY,       &m_bPatchBBoxSelect,            FALSE);
  
  // Gef: Kyro GL_POINT workaround
  mLocalPrefs.GetPref(GLPOINTWORKAROUND_KEY,  &m_bGlPtWorkaround,             FALSE);

  // window positioning
  mLocalPrefs.GetPref(ENTITYSPLIT1_KEY,       &mWindowInfo.nEntitySplit1,     -1);
  mLocalPrefs.GetPref(ENTITYSPLIT2_KEY,       &mWindowInfo.nEntitySplit2,     -1);

  mLocalPrefs.GetPref(POSITIONX_KEY,          &mWindowInfo.position.x,        -1);
  mLocalPrefs.GetPref(POSITIONY_KEY,          &mWindowInfo.position.y,        -1);
  mLocalPrefs.GetPref(WIDTH_KEY,              &mWindowInfo.position.w,        -1);
  mLocalPrefs.GetPref(HEIGHT_KEY,             &mWindowInfo.position.h,        450);

  const window_position_t default_window_pos = { 0, 0, 200, 200, };

  mLocalPrefs.GetPref(ENTITYWND_KEY,          &mWindowInfo.posEntityWnd,      default_window_pos);
  mLocalPrefs.GetPref(MAPINFOWND_KEY,         &mWindowInfo.posMapInfoWnd,     default_window_pos);
  mLocalPrefs.GetPref(CAMWND_KEY,             &mWindowInfo.posCamWnd,         default_window_pos);
  mLocalPrefs.GetPref(ZWND_KEY,               &mWindowInfo.posZWnd,           default_window_pos);
  mLocalPrefs.GetPref(XYWND_KEY,              &mWindowInfo.posXYWnd,          default_window_pos);
  mLocalPrefs.GetPref(YZWND_KEY,              &mWindowInfo.posYZWnd,          default_window_pos);
  mLocalPrefs.GetPref(XZWND_KEY,              &mWindowInfo.posXZWnd,          default_window_pos);
  mLocalPrefs.GetPref(PATCHWND_KEY,           &mWindowInfo.posPatchWnd,       default_window_pos);
  mLocalPrefs.GetPref(SURFACEWND_KEY,         &mWindowInfo.posSurfaceWnd,     default_window_pos);
  mLocalPrefs.GetPref(ENTITYINFOWND_KEY,      &mWindowInfo.posEntityInfoWnd,  default_window_pos);

  mLocalPrefs.GetPref(ZWIDTH_KEY,             &mWindowInfo.nZWidth,           30);
  mLocalPrefs.GetPref(XYHEIGHT_KEY,           &mWindowInfo.nXYHeight,         300);
  mLocalPrefs.GetPref(XYWIDTH_KEY,            &mWindowInfo.nXYWidth,          300);
  mLocalPrefs.GetPref(CAMWIDTH_KEY,           &mWindowInfo.nCamWidth,         200);
  mLocalPrefs.GetPref(CAMHEIGHT_KEY,          &mWindowInfo.nCamHeight,        200);
  mLocalPrefs.GetPref(ZFLOATWIDTH_KEY,        &mWindowInfo.nZFloatWidth,      300);
#ifdef _WIN32
  mLocalPrefs.GetPref(STATE_KEY,              &mWindowInfo.nState,            SW_SHOW);
#endif

  // menu stuff
  mLocalPrefs.GetPref(COUNT_KEY,              &m_nMRUCount,                   0);
  for(i = 0; i < 4; i++) 
  {
    char buf[64];
    sprintf (buf, "%s%d", FILE_KEY, i);
    mLocalPrefs.GetPref(buf,                  &m_strMRUFiles[i],              "");    
  }

  // some platform specific prefs
#ifdef _WIN32
  mLocalPrefs.GetPref(NATIVEGUI_KEY,          &m_bNativeGUI,                  TRUE);
  mLocalPrefs.GetPref(STARTONPRIMMON_KEY,     &m_bStartOnPrimMon,             FALSE);
#endif

  mLocalPrefs.GetPref(SI_TEXMENU_KEY,         &g_qeglobals.d_savedinfo.iTexMenu,                ID_VIEW_BILINEARMIPMAP);
  mLocalPrefs.GetPref(SI_GAMMA_KEY,           &g_qeglobals.d_savedinfo.fGamma,                  1.0f);
  mLocalPrefs.GetPref(SI_EXCLUDE_KEY,         &g_qeglobals.d_savedinfo.exclude,                 0); // nothing filtered by default
  mLocalPrefs.GetPref(SI_INCLUDE_KEY,         &g_qeglobals.d_savedinfo.include,                 INCLUDE_NAMES | INCLUDE_COORDS | INCLUDE_ANGLES | INCLUDE_CAMERATINT);
  mLocalPrefs.GetPref(SI_SHOWNAMES_KEY,       &g_qeglobals.d_savedinfo.show_names,              FALSE);
  mLocalPrefs.GetPref(SI_SHOWCOORDS_KEY,      &g_qeglobals.d_savedinfo.show_coordinates,        TRUE);
  mLocalPrefs.GetPref(SI_SHOWANGLES_KEY,      &g_qeglobals.d_savedinfo.show_angles,             TRUE);
  mLocalPrefs.GetPref(SI_SHOWOUTLINES_KEY,    &g_qeglobals.d_savedinfo.show_outline,            FALSE);
  mLocalPrefs.GetPref(SI_SHOWAXIS_KEY,        &g_qeglobals.d_savedinfo.show_axis,               TRUE);
  mLocalPrefs.GetPref(SI_NOSELOUTLINES_KEY,   &g_qeglobals.d_savedinfo.bNoSelectedOutlines,     FALSE);

  mLocalPrefs.GetPref(SI_OUTLINESTYLE_KEY,   &g_qeglobals.d_savedinfo.iSelectedOutlinesStyle,  OUTLINE_ZBUF|OUTLINE_BSEL);

  LoadTexdefPref(&g_qeglobals.d_savedinfo.m_SIIncrement, SI_SURFACE_TEXDEF_KEY);
  LoadTexdefPref(&g_qeglobals.d_savedinfo.m_PIIncrement, SI_PATCH_TEXDEF_KEY);

  // text editor binding
#ifdef _WIN32
  mLocalPrefs.GetPref(CUSTOMSHADEREDITOR_KEY, &m_bUseWin32Editor, TRUE);
#else
  mLocalPrefs.GetPref(CUSTOMSHADEREDITOR_KEY, &m_bUseCustomEditor, FALSE);
  mLocalPrefs.GetPref(CUSTOMSHADEREDITORCOMMAND_KEY, &m_strEditorCommand, "");
#endif


  vec3_t vDefaultAxisColours[3] = {
    {0.f, 0.5f, 0.f},
    {0.f, 0.f, 1.f},
    {1.f, 0.f, 0.f},
  };

  for(i = 0; i < 3; i++) {
    char buf[64];
    sprintf(buf, "%s%d", SI_AXISCOLORS_KEY, i);
    mLocalPrefs.GetPref(buf,   g_qeglobals.d_savedinfo.AxisColors[i], vDefaultAxisColours[i]);
  }

  vec3_t vDefaultColours[COLOR_LAST] = {
    {0.25f, 0.25f,  0.25f},
    {1.f,   1.f,    1.f},
    {0.75f, 0.75f,  0.75f},
    {0.5f,  0.5f,   0.5f},
    {0.25f, 0.25f,  0.25f},
    {0.0f,  0.0f,   0.0f},
    {0.f,   0.f,    1.f},
    {0.f,   0.f,    0.f},
    {0.f,   0.f,    0.f},
    {1.f,   0.f,    0.f},
    {0.f,   0.f,    1.f},
    {0.5f,  0.f,    0.75f},
    {1.0f,  0.f,    0.f},
    {0.f,   0.f,    0.f},
    {0.f,   0.f,    0.f},
  };

  for(i = 0; i < COLOR_LAST; i++) {
    char buf[64];
    sprintf(buf, "%s%d", SI_COLORS_KEY, i);
    mLocalPrefs.GetPref(buf,   g_qeglobals.d_savedinfo.colors[i], vDefaultColours[i]);
  }

  mLocalPrefs.GetPref(TEXTURECOMPRESSIONFORMAT_KEY, &m_nTextureCompressionFormat, 1);

  mLocalPrefs.GetPref(LIGHTRADIUS_KEY, &m_nLightRadiuses, TRUE);

  mLocalPrefs.GetPref(Q3MAP2TEX_KEY, &m_bQ3Map2Texturing, TRUE);

#ifdef ATIHACK_812
	mLocalPrefs.GetPref(ATIHACK_KEY, &m_bGlATIHack, FALSE);
#endif

  Undo_SetMaxSize(m_nUndoLevels); // set it internally as well / FIXME: why not just have one global value?

  UpdateTextureCompression();

#ifdef ATIHACK_812
  UpdateATIHack();
#endif

  if (mLocalPrefs.mbEmpty)
  {
    mLocalPrefs.mbEmpty = false;
    Sys_Printf("Saving local.pref with default pref values\n");
    SavePrefs();
  }
}

void PrefsDlg::SavePrefs ()
{
  if (g_qeglobals.disable_ini)
    return;

#ifdef _DEBUG
  Sys_Printf("PrefsDlg::SavePrefs\n");
#endif

  // this will take care of copying back from the dialog to the variables
  // NOTE: it may be overkill to call systematically before a SavePrefs, but it's safer
  // this will also cause an UpdateData for the mGamesDialog
  UpdateData(TRUE);

  mGamesDialog.SavePrefs();

  // update the tree and save it
  mLocalPrefs.UpdatePrefTree();
  if (!mLocalPrefs.WriteXMLFile(m_inipath->str))
    Sys_FPrintf(SYS_ERR, "Error occured while saving local prefs file '%s'\n", m_inipath->str);

  if ( m_nMouse == 0 ) {
	  m_nMouseButtons = 2;
  } else {
	  m_nMouseButtons = 3;
  }

}

void PrefsDlg::PostModal (int code)
{
  if (code == IDOK)
  {
    SavePrefs();
    // make sure the logfile is ok
    Sys_LogFile();
    #ifdef ATIHACK_812
      UpdateATIHack();
    #endif
    if (g_pParentWnd)
      g_pParentWnd->SetGridStatus();
    Sys_UpdateWindows(W_ALL);
    if (m_nUndoLevels != 0)
		Undo_SetMaxSize(m_nUndoLevels);
  }
}

void PrefsDlg::DoEditorSensitivity()
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(g_object_get_data (G_OBJECT(m_pWidget), "check_customeditor"))))
  {
    gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data (G_OBJECT(m_pWidget), "label_customeditor")), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data (G_OBJECT(m_pWidget), "entry_customeditor")), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data (G_OBJECT(m_pWidget), "button_customeditor")), TRUE);
  }
  else
  {
    gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data (G_OBJECT(m_pWidget), "label_customeditor")), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data (G_OBJECT(m_pWidget), "entry_customeditor")), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data (G_OBJECT(m_pWidget), "button_customeditor")), FALSE);
  }
}

void PrefsDlg::DoSensitivity()
{
#if 0
  // first, look at the project file version ... will monitoring work?
  // project files now XML, guaranteed to be at least version 2
  if (0)//IntForKey( g_qeglobals.d_project_entity, "version" ) < 2)
  {
    if (m_bWarn)
    {
      Str Msg;
      Msg = "The current project file (";
      Msg += g_PrefsDlg.m_strLastProject;
      Msg += ") is not at least version 2.\nI need version 2 or above to setup BSP monitoring correctly.";
      gtk_MessageBox(m_pWidget, Msg.GetBuffer(), MB_OK );
      
      m_bWarn = false;
    }

    // go ahead, disable everybuddy
    gtk_widget_set_sensitive( GTK_WIDGET(g_object_get_data( G_OBJECT(m_pWidget), "check_leakstop" )), FALSE );
    gtk_widget_set_sensitive( GTK_WIDGET(g_object_get_data( G_OBJECT(m_pWidget), "check_monitorbsp" )), FALSE );
    gtk_widget_set_sensitive( GTK_WIDGET(g_object_get_data( G_OBJECT(m_pWidget), "check_runengine" )), FALSE );
    gtk_widget_set_sensitive( GTK_WIDGET(g_object_get_data( G_OBJECT(m_pWidget), "check_sleep" )), FALSE );
  }
  else
  {
#endif
//    m_bWarn = true;

    gtk_widget_set_sensitive( GTK_WIDGET(g_object_get_data( G_OBJECT(m_pWidget), "check_leakstop" )), TRUE );
    gtk_widget_set_sensitive( GTK_WIDGET(g_object_get_data( G_OBJECT(m_pWidget), "check_monitorbsp" )), TRUE );
    gtk_widget_set_sensitive( GTK_WIDGET(g_object_get_data( G_OBJECT(m_pWidget), "check_runengine" )), TRUE );
    gtk_widget_set_sensitive( GTK_WIDGET(g_object_get_data( G_OBJECT(m_pWidget), "check_sleep" )), TRUE );

    if ( ! gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( g_object_get_data( G_OBJECT(m_pWidget), "check_monitorbsp" ) ) ) )
    {
      gtk_widget_set_sensitive( GTK_WIDGET(g_object_get_data( G_OBJECT(m_pWidget), "check_leakstop" )), FALSE );
      gtk_widget_set_sensitive( GTK_WIDGET(g_object_get_data( G_OBJECT(m_pWidget), "check_runengine" )), FALSE );
      gtk_widget_set_sensitive( GTK_WIDGET(g_object_get_data( G_OBJECT(m_pWidget), "check_sleep" )), FALSE );
    } else if (! gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( g_object_get_data( G_OBJECT(m_pWidget), "check_runengine" ) ) ) )
    {
      gtk_widget_set_sensitive( GTK_WIDGET(g_object_get_data( G_OBJECT(m_pWidget), "check_sleep" )), FALSE );
    }
}

/*
============================================================
CGameInstall
============================================================
*/

CGameInstall::CGameInstall() {
	memset( m_availGames, 0, sizeof( m_availGames ) );
}

void CGameInstall::BuildDialog() {
	GtkWidget *dlg, *vbox1, *button, *text, *combo, *entry;
	
	dlg = m_pWidget;
	gtk_window_set_title( GTK_WINDOW( dlg ), "Configure games" );

	vbox1 = gtk_vbox_new( FALSE, 0 );
	gtk_widget_show( vbox1 );
	gtk_container_add( GTK_CONTAINER( dlg ), vbox1 );

	text = gtk_label_new( "Select the game to configure" );
	gtk_widget_show( text );
	gtk_box_pack_start( GTK_BOX( vbox1 ), text, FALSE, FALSE, 0 );

	combo = gtk_combo_new();
	gtk_widget_show( combo );
	gtk_box_pack_start( GTK_BOX( vbox1 ), combo, FALSE, FALSE, 0 );

	GList *combo_list = NULL;
	int iGame = 0;
	while ( m_availGames[ iGame ] != GAME_NONE ) {
		switch ( m_availGames[ iGame ] ) {
		case GAME_Q3:
			combo_list = g_list_append( combo_list, "Quake III Arena (including mods)" );
			break;
		case GAME_URT:
			combo_list = g_list_append( combo_list, "Urban Terror (standalone)" );
			break;
		case GAME_WARSOW:
			combo_list = g_list_append( combo_list, "Warsow" );
			break;
		}
		iGame++;
	}
	gtk_combo_set_popdown_strings( GTK_COMBO( combo ), combo_list );
	g_list_free( combo_list );
	AddDialogData( combo, &m_nComboSelect, DLG_COMBO_INT );

	text = gtk_label_new( "Name:" );
	gtk_widget_show( text );
	gtk_box_pack_start( GTK_BOX( vbox1 ), text, FALSE, FALSE, 0 );
	
	entry = gtk_entry_new();
	gtk_widget_show( entry );
	gtk_box_pack_start( GTK_BOX( vbox1 ), entry, FALSE, FALSE, 0 );
	AddDialogData( entry, &m_strName, DLG_ENTRY_TEXT );

	text = gtk_label_new( "Engine directory:" );
	gtk_widget_show( text );
	gtk_box_pack_start( GTK_BOX( vbox1 ), text, FALSE, FALSE, 0 );
	
	entry = gtk_entry_new();
	gtk_widget_show( entry );
	gtk_box_pack_start( GTK_BOX( vbox1 ), entry, FALSE, FALSE, 0 );
	AddDialogData( entry, &m_strEngine, DLG_ENTRY_TEXT );

	// this gets done in the project stuff atm
#if 0
	text = gtk_label_new( "Mod subdirectory:" );
	gtk_widget_show( text );
	gtk_box_pack_start( GTK_BOX( vbox1 ), text, FALSE, FALSE, 0 );

	entry = gtk_entry_new();
	gtk_widget_show( entry );
	gtk_box_pack_start( GTK_BOX( vbox1 ), entry, FALSE, FALSE, 0 );
	AddDialogData( entry, &m_strMod, DLG_ENTRY_TEXT );
#endif

	button = gtk_button_new_with_label( "OK" );
	gtk_widget_show( button );
	gtk_box_pack_start( GTK_BOX( vbox1 ), button, FALSE, FALSE, 0 );
	AddModalButton( button, IDOK );

	gtk_widget_set_usize( button, 60, -2 );
}

void CGameInstall::Run() {
	ScanGames();
	DoModal();
	Sys_Printf( "combo: %d name: %s engine: %s mod: %s\n", m_nComboSelect, m_strName.GetBuffer(), m_strEngine.GetBuffer(), m_strMod.GetBuffer() );

	// write out the game file
	Str gameFilePath = g_strAppPath.GetBuffer();
	gameFilePath += "games/";
	gameFilePath += m_strName.GetBuffer();
	gameFilePath += ".game";
	Sys_Printf( "game file: %s\n", gameFilePath.GetBuffer() );

	FILE *fg = fopen( gameFilePath.GetBuffer(), "w" );
	if ( fg == NULL ) {
		Error( "Failed to open %s for writing\n", gameFilePath.GetBuffer() );
	}
	fprintf( fg, "<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"yes\"?>\n<game\n" );
	fprintf( fg, "  name=\"%s\"\n", m_strName.GetBuffer() );
	fprintf( fg, "  gametools=\"%sgames\"\n", g_strAppPath.GetBuffer() );
	fprintf( fg, "  enginepath=\"%s\"\n", m_strEngine.GetBuffer() );
	switch ( m_availGames[ m_nComboSelect ] ) {
	case GAME_Q3: {
		Str source = g_strAppPath.GetBuffer();
		source += "installs/";
		source += Q3_PACK;
		Str dest = m_strEngine.GetBuffer();
		CopyTree( source.GetBuffer(), dest.GetBuffer() );
		fprintf( fg, "  basegame=\"baseq3\"\n" );
		break;
	}
	case GAME_URT: {
		Str source = g_strAppPath.GetBuffer();
		source += "installs/";
		source += URT_PACK;
		Str dest = m_strEngine.GetBuffer();
		CopyTree( source.GetBuffer(), dest.GetBuffer() );
		fprintf( fg, "  basegame=\"q3ut4\"\n" );
		break;
	}
	case GAME_WARSOW:
		fprintf( fg, "  basegame=\"basewsw\"\n" );
		break;
	}
	fprintf( fg, "/>\n" );
	fclose( fg );
}

/*
===============
CGameInstall::ScanGames
scan for active games that can be installed, based on the presence 
===============
*/
void CGameInstall::ScanGames() {
	Str				pakPaths = g_strAppPath.GetBuffer();
	DIR				*dir;
	struct dirent	*dirlist;
	int				iGame = 0;

	pakPaths +=	"installs/";
	dir = opendir( pakPaths.GetBuffer() );
	if ( dir != NULL ) {
		while ( ( dirlist = readdir( dir ) ) != NULL ) {
			if ( stricmp( dirlist->d_name, Q3_PACK ) == 0 ) {
				m_availGames[ iGame++ ] = GAME_Q3;
			}
			if ( stricmp( dirlist->d_name, URT_PACK ) == 0 ) {
				m_availGames[ iGame++ ] = GAME_URT;
			}
		}
		closedir( dir );
	}
}