/******************************************************************************
 * $Id: chart1.cpp,v 1.102 2010/06/25 02:02:04 bdbcat Exp $
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN Main wxWidgets Program
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register   *
 *   $EMAIL$   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 *
 * $Log: chart1.cpp,v $
 * Revision 1.102  2010/06/25 02:02:04  bdbcat
 * 624
 *
 * Revision 1.101  2010/06/24 02:02:04  bdbcat
 * 623
 *
 * Revision 1.100  2010/06/21 02:04:57  bdbcat
 * 620
 *
 * Revision 1.99  2010/06/16 03:52:44  bdbcat
 * 615a
 *
 * Revision 1.98  2010/06/13 21:04:48  bdbcat
 * 613a
 *
 * Revision 1.97  2010/06/12 02:21:19  bdbcat
 * 611b
 *
 * Revision 1.96  2010/06/12 00:48:56  bdbcat
 * 611b
 *
 * Revision 1.95  2010/06/11 16:24:50  bdbcat
 * 611a
 *
 * Revision 1.94  2010/06/07 15:29:12  bdbcat
 * 607a
 *
 * Revision 1.93  2010/06/06 20:50:27  bdbcat
 * 606a
 *
 * Revision 1.92  2010/06/04 22:32:43  bdbcat
 * 604
 *
 * Revision 1.91  2010/05/31 00:43:04  bdbcat
 * 530
 *
 * Revision 1.90  2010/05/29 17:33:11  bdbcat
 * 529a
 *
 * Revision 1.89  2010/05/27 18:57:03  bdbcat
 * 527a
 *
 * Revision 1.88  2010/05/26 21:59:31  bdbcat
 * 526a
 *
 * Revision 1.87  2010/05/23 23:07:42  bdbcat
 * Build 523a
 *
 * Revision 1.86  2010/05/20 19:03:51  bdbcat
 * Build 520
 *
 * Revision 1.85  2010/05/19 01:07:28  bdbcat
 * Build 518
 *
 * Revision 1.84  2010/05/15 03:57:04  bdbcat
 * Build 514
 *
 * Revision 1.83  2010/05/04 04:56:04  bdbcat
 * Build 504
 *
 * Revision 1.82  2010/05/04 04:44:57  bdbcat
 * Build 504
 *
 * Revision 1.81  2010/05/04 01:33:52  bdbcat
 * Build 503
 *
 * Revision 1.80  2010/05/02 20:04:50  bdbcat
 * Build 502b
 *
 * Revision 1.79  2010/05/02 03:03:04  bdbcat
 * Build 501
 *
 * Revision 1.78  2010/04/27 01:40:44  bdbcat
 * Build 426
 *
 * Revision 1.77  2010/04/15 15:49:17  bdbcat
 * Build 415.
 *
 * Revision 1.76  2010/04/01 20:17:37  bdbcat
 * 2.1.0 Build 331
 *
 * Revision 1.75  2010/03/29 18:39:01  bdbcat
 * Correct for Unicode build
 *
 * Revision 1.74  2010/03/29 03:28:25  bdbcat
 * 2.1.0 Beta Initial
 *
 * 2010-02-15 pjotrc
 * - implement anchor watch
 *
 * 2010-02-09 pjotrc
 * - implement logbook
 * - implement ships bells (the bells count code comes from wxTides by Michael Hopper)
 * - rename old log file on overfill
 * - implement ShowAIS button
 *
 *
 */
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "wx/print.h"
#include "wx/printdlg.h"
#include "wx/artprov.h"
#include "wx/stdpaths.h"
#include "scrollingdialog.h"
#include <wx/intl.h>
#include <wx/listctrl.h>



#include "dychart.h"

#ifdef __WXMSW__
  #include <stdlib.h>
  #include <math.h>
  #include <time.h>
#endif

#ifndef __WXMSW__
#include <signal.h>
#include <setjmp.h>
#endif

#include "chart1.h"
#include "chcanv.h"
#include "chartdb.h"
#include "navutil.h"
#include "routeman.h"
#include "statwin.h"
#include "concanv.h"
#include "nmea.h"
#include "options.h"
#include "about.h"
#include "thumbwin.h"
#include "tcmgr.h"
#include "cpl_error.h"
#include "ais.h"
#include "chartimg.h"               // for ChartBaseBSB
#include "routeprop.h"
#include "cm93.h"
#include "grib.h"
#include "cutil.h"
#include "routemanagerdialog.h"

#include <wx/image.h>
#include "wx/apptrait.h"

#ifdef __WXOSX__
#include "macutils.h"
#endif

#ifdef USE_S57
#include "s52plib.h"
#include "s57chart.h"
///#include "s57.h"
#include "cpl_csv.h"
#endif

#ifdef USE_WIFI_CLIENT
#include "wificlient.h"
#endif

#ifdef __WXMSW__
//#define __MSVC__LEAK
#ifdef __MSVC__LEAK
#include "Stackwalker.h"
#endif
#endif


#define ocpnUSE_OPNCTOOLBAR

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayOfCDI);

//------------------------------------------------------------------------------
//      Static variable definition
//------------------------------------------------------------------------------
CPL_CVSID("$Id: chart1.cpp,v 1.102 2010/06/25 02:02:04 bdbcat Exp $");


FILE            *flog;                  // log file
wxLog           *logger;
wxLog           *Oldlogger;

ComPortManager  *g_pCommMan;

MyFrame         *gFrame;

ChartCanvas     *cc1;
ConsoleCanvas   *console;
NMEAWindow      *g_pnmea;
StatWin         *stats;

//wxToolBar       *toolBar;

MyConfig        *pConfig;

ChartBase       *Current_Vector_Ch;
ChartBase       *Current_Ch;
ChartDB         *ChartData;
ChartStack      *pCurrentStack;
wxString        *pdir_list[20];
int             g_restore_stackindex;

RouteList       *pRouteList;

Select          *pSelect;
Select          *pSelectTC;
Select          *pSelectAIS;

Routeman        *g_pRouteMan;
WayPointman     *pWayPointMan;
MarkProp        *pMarkPropDialog;
RouteProp       *pRoutePropDialog;
MarkInfo        *pMarkInfoDialog;
RouteManagerDialog *pRouteManagerDialog;


double          gLat, gLon, gCog, gSog, gHdt, gHDg, gVar;
double          vLat, vLon;
double          initial_scale_ppm;

//wxArrayString   *pChartDirArray;
ArrayOfCDI      g_ChartDirArray;

bool            bDBUpdateInProgress;

ThumbWin        *pthumbwin;
TCMgr           *ptcmgr;

bool            bDrawCurrentValues;

wxString        g_SData_Locn;
wxString        *pChartListFileName;
wxString        *pTC_Dir;
wxString        *pHome_Locn;
wxString        *pWVS_Locn;
wxString        *pInit_Chart_Dir;
wxString        *g_pcsv_locn;
wxString        *g_pSENCPrefix;
wxString        g_PresLibData;

int             user_user_id;
int             file_user_id;

int             quitflag;
int             g_tick;
int             mem_total, mem_initial;

bool            s_bSetSystemTime;

wxString        *phost_name;

static unsigned int malloc_max;

OCP_NMEA_Thread   *pNMEA_Thread;
OCP_GARMIN_Thread *pGARMIN_Thread;
wxString        *pNMEADataSource;
wxString         g_NMEABaudRate;

wxString        *pNMEA_AP_Port;

wxDateTime      g_start_time;
wxDateTime      g_loglast_time;   // pjotrc 2010.02.09
wxSound         bells_sound[8];   // pjotrc 2010.02.09

RoutePoint      *pAnchorWatchPoint1;       // pjotrc 2010.02.15
RoutePoint      *pAnchorWatchPoint2;       // pjotrc 2010.02.15
double          AnchorPointMaxDist, AnchorPointMinDist;  // pjotrc 2010.02.15
bool            AnchorAlertOn1, AnchorAlertOn2; // pjotrc 2010.02.17 flags for alerting

bool            g_bCruising;

ChartDummy      *pDummyChart;

wxString        *pWIFIServerName;

AutoPilotWindow *pAPilot;

// Global print data, to remember settings during the session
wxPrintData *g_printData = (wxPrintData*) NULL ;

// Global page setup data
wxPageSetupData* g_pageSetupData = (wxPageSetupData*) NULL;

bool              g_bShowPrintIcon;
bool              g_bShowOutlines;
bool              g_bShowDepthUnits;
bool              g_bGarminPersistance;
int               g_nNMEADebug;
bool              g_bPlayShipsBells;   // pjotrc 2010.02.09


bool              g_bShowGPXIcons;  // toh, 2009.02.14
bool              g_bNavAidShowRadarRings;            // toh, 2009.02.24
int               g_iNavAidRadarRingsNumberVisible;   // toh, 2009.02.24
float             g_fNavAidRadarRingsStep;            // toh, 2009.02.24
int               g_pNavAidRadarRingsStepUnits;       // toh, 2009.02.24
bool              g_bWayPointPreventDragging;         // toh, 2009.02.24

FontMgr         *pFontMgr;

ColorScheme     global_color_scheme;
int             Usercolortable_index;
wxArrayPtrVoid  *UserColorTableArray;
wxArrayPtrVoid  *UserColourHashTableArray;
ColourHash      *pcurrent_user_color_hash;

int             gps_watchdog_timeout_ticks;
int             sat_watchdog_timeout_ticks;

int             gGPS_Watchdog;
bool            bGPSValid;

int             gHDx_Watchdog;
bool            g_bHDxValid;

int             gSAT_Watchdog;
int             g_SatsInView;
bool            g_bSatValid;


bool            g_bDebugCM93;
bool            g_bDebugS57;

#ifdef USE_S57
s52plib           *ps52plib;
S57ClassRegistrar *g_poRegistrar;
s57RegistrarMgr   *m_pRegistrarMan;

// begin rms
#elif defined __WXOSX__
s52plib         *ps52plib;
// end rms
#endif

#ifdef USE_WIFI_CLIENT
WIFIWindow      *pWIFI;
#endif

#ifdef __WXOSX__
#include "macutils.h"
#endif

// begin rms
#if defined( USE_S57) || defined ( __WXOSX__ )
#ifdef __WXMSW__
#ifdef USE_GLU_TESS
#ifdef USE_GLU_DLL
// end rms
extern bool           s_glu_dll_ready;
extern HINSTANCE      s_hGLU_DLL;                   // Handle to DLL
#endif
#endif
#endif
#endif

double           g_ownship_predictor_minutes;

OCP_AIS_Thread  *pAIS_Thread;
AIS_Decoder     *g_pAIS;
wxString        *pAIS_Port;
bool             g_bGPSAISMux;
bool             g_bAIS_CPA_Alert;
bool             g_bAIS_CPA_Alert_Audio;
AISTargetAlertDialog    *g_pais_alert_dialog_active;
AISTargetQueryDialog    *g_pais_query_dialog_active;

int               g_ais_alert_dialog_x, g_ais_alert_dialog_y;
int               g_ais_alert_dialog_sx, g_ais_alert_dialog_sy;
int               g_ais_query_dialog_x, g_ais_query_dialog_y;

bool            s_socket_test_running;
bool            s_socket_test_passed;
wxSocketClient  *s_t_sock;
wxSocketServer  *s_s_sock;


int              g_nframewin_x;
int              g_nframewin_y;
bool             g_bframemax;

bool             g_bAutoAnchorMark;

wxRect           g_blink_rect;
double           g_PlanSpeed;

wxArrayString    *pMessageOnceArray;

FILE             *s_fpdebug;
bool             bAutoOpen;
bool             bFirstAuto;

bool             g_bUseRMC;
bool             g_bUseGLL;

int              g_nCacheLimit;
bool             g_bGDAL_Debug;

double           g_VPRotate;                   // Viewport rotation angle, used on "Course Up" mode
bool             g_bCourseUp;
int              g_COGAvgSec;                   // COG average period (sec.) for Course Up Mode
double           g_COGAvg;
bool             g_bLookAhead;

int              g_nCOMPortCheck;

bool             g_bbigred;

//-----------------------------------------------------------------------------------------------------
//      OCP_NMEA_Thread Static data store
//-----------------------------------------------------------------------------------------------------
char                            rx_share_buffer[MAX_RX_MESSSAGE_SIZE];
unsigned int                    rx_share_buffer_length;
ENUM_BUFFER_STATE               rx_share_buffer_state;


#ifndef __WXMSW__
struct sigaction sa_all;
struct sigaction sa_all_old;
#endif




#ifdef __WXMSW__
//    System color control support

typedef DWORD (WINAPI *SetSysColors_t)(DWORD, DWORD *, DWORD *);
typedef DWORD (WINAPI *GetSysColor_t)(DWORD);

SetSysColors_t pSetSysColors;
GetSysColor_t  pGetSysColor;

void SaveSystemColors(void);
void RestoreSystemColors(void);

DWORD       color_3dface;
DWORD       color_3dhilite;
DWORD       color_3dshadow;
DWORD       color_3ddkshadow;
DWORD       color_3dlight;
DWORD       color_activecaption;
DWORD       color_gradientactivecaption;
DWORD       color_captiontext;
DWORD       color_windowframe;
DWORD       color_inactiveborder;

#endif

cm93manager       *s_pcm93mgr;

//    AIS Global configuration
bool             g_bShowAIS; // pjotrc 2010.02.09
bool             g_bCPAMax;
double           g_CPAMax_NM;
bool             g_bCPAWarn;
double           g_CPAWarn_NM;
bool             g_bTCPA_Max;
double           g_TCPA_Max;
bool             g_bMarkLost;
double           g_MarkLost_Mins;
bool             g_bRemoveLost;
double           g_RemoveLost_Mins;
bool             g_bShowCOG;
double           g_ShowCOG_Mins;
bool             g_bShowTracks;
double           g_ShowTracks_Mins;
bool             g_bShowMoored;
double           g_ShowMoored_Kts;
wxString         g_sAIS_Alert_Sound_File;
bool             g_bAIS_CPA_Alert_Suppress_Moored;
wxToolBarToolBase *m_pAISTool;

DummyTextCtrl    *g_pDummyTextCtrl;
bool             g_bEnableZoomToCursor;

wxString         g_CM93DictDir;

bool             g_bShowTrackIcon;
bool             g_bTrackActive;
bool             g_bTrackCarryOver;
Track            *g_pActiveTrack;
double           g_TrackIntervalSeconds;
double           g_TrackDeltaDistance;
bool             g_bTrackTime;
bool             g_bTrackDistance;

int              g_total_NMEAerror_messages;

int              g_cm93_zoom_factor;
CM93DSlide       *pCM93DetailSlider;
bool             g_bShowCM93DetailSlider;
int              g_cm93detail_dialog_x, g_cm93detail_dialog_y;

bool             g_bUseGreenShip;

wxString         g_AW1GUID;
wxString         g_AW2GUID;


//    GRIB Support
GRIBUIDialog     *g_pGribDialog;

int              g_grib_dialog_x, g_grib_dialog_y;
int              g_grib_dialog_sx, g_grib_dialog_sy;
wxString         g_grib_dir;
bool             g_bShowGRIBIcon;
bool             g_bGRIBUseHiDef;


bool             g_b_overzoom_x;                      // Allow high overzoom
bool             g_bshow_overzoom_emboss;

int              g_n_ownship_meters;

int              g_nautosave_interval_seconds;

bool             g_bPreserveScaleOnX;

about             *g_pAboutDlg;

wxPlatformInfo    *g_pPlatform;
wxLocale         locale_def_lang;
wxString          g_locale;

TTYWindow        *g_NMEALogWindow;
int              g_NMEALogWindow_x, g_NMEALogWindow_y;
int              g_NMEALogWindow_sx, g_NMEALogWindow_sy;

bool             g_bUseRaster;
bool             g_bUseVector;
bool             g_bUseCM93;

int              g_click_stop;

int              g_MemFootSec;
int              g_MemFootMB;

ArrayOfInts      g_quilt_noshow_index_array;

wxStaticBitmap   *g_pStatBoxTool;

bool              g_bquiting;
int               g_BSBImgDebug;

AISTargetListDialog   *g_pAISTargetList;

char bells_sound_file_name[8][12] =    // pjotrc 2010.02.09

{ "1bells.wav","2bells.wav","3bells.wav","4bells.wav","5bells.wav","6bells.wav","7bells.wav","8bells.wav"   // pjotrc 2010.02.09

};


static char nmea_tick_chars[] = {'|', '/', '-', '\\', '|', '/', '-', '\\'};
static int tick_idx;

extern char     OpenCPNVersion[];

int             n_NavMessageShown;
wxString        g_config_version_string;


#ifndef __WXMSW__
sigjmp_buf           env;                    // the context saved by sigsetjmp();
#endif

// {2C9C45C2-8E7D-4C08-A12D-816BBAE722C0}
#ifdef  __WXMSW__
DEFINE_GUID(GARMIN_DETECT_GUID, 0x2c9c45c2L, 0x8e7d, 0x4c08, 0xa1, 0x2d, 0x81, 0x6b, 0xba, 0xe7, 0x22, 0xc0);
#endif


#ifdef __MSVC__
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif


//    Some static helpers
void appendOSDirSlash(wxString* pString);
void InitializeUserColors(void);
void DeInitializeUserColors(void);
void SetSystemColors(ColorScheme cs);


//------------------------------------------------------------------------------
//    PNG Icon resources
//------------------------------------------------------------------------------
#ifdef OCPN_USE_PNGICONS

#include "bitmaps/icons.h"
#include "bitmaps/icons.cpp"

#include "bitmaps/polyconic.xpm"
wxBitmap *_img_polyprj;

#endif

// ----------------------------------------------------------------------------
// Icon resources
// ----------------------------------------------------------------------------
#ifndef OCPN_USE_PNGICONS
 #include "bitmaps/settings.xpm"
 #include "bitmaps/zoomin.xpm"
 #include "bitmaps/zoomout.xpm"
 #include "bitmaps/scin.xpm"
 #include "bitmaps/scout.xpm"
 #include "bitmaps/tide.xpm"
 #include "bitmaps/text.xpm"
 #include "bitmaps/route.xpm"
 #include "bitmaps/exitt.xpm"
 #include "bitmaps/follow.xpm"
 #include "bitmaps/current.xpm"
 #include "bitmaps/print.xpm"
 #include "bitmaps/help.xpm"
 #include "bitmaps/colscheme.xpm"
 #include "bitmaps/gpx_import.xpm"
 #include "bitmaps/gpx_export.xpm"
 #include "bitmaps/track.xpm"
#endif

#include "bitmaps/opencpn.xpm"

//------------------------------------------------------------------------------
//              Local constants
//------------------------------------------------------------------------------
enum
{
      ID_PIANO_DISABLE_QUILT_CHART  =  50000,
      ID_PIANO_ENABLE_QUILT_CHART
};

//------------------------------------------------------------------------------
//              Fwd Refs
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------
//      Signal Handlers
//-----------------------------------------------------------------------
#ifndef __WXMSW__

//These are the signals possibly expected
//      SIGUSR1
//      Raised externally to cause orderly termination of application
//      Intended to act just like pushing the "EXIT" button

//      SIGSEGV
//      Some undefined segfault......

void
catch_signals(int signo)
{
      switch(signo)
      {
            case SIGUSR1:
                  quitflag++;                             // signal to the timer loop
                  break;

            case SIGSEGV:
                  siglongjmp(env, 1);                       // jump back to the setjmp() point
                  break;

            default:
                  break;
      }

}
#endif

int ShowNavWarning()
{
      wxString msg0(_T("\n\
OpenCPN is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied\n\
warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.\n\
See the GNU General Public License for more details.\n\n\
OpenCPN must only be used in conjunction with approved\n\
paper charts and traditional methods of navigation.\n\n\
DO NOT rely upon OpenCPN for safety of life or property.\n\n\
Please click \"OK\" to agree and proceed, \"Cancel\" to quit.\n"));

      wxMessageDialog odlg(gFrame, msg0, _T("Welcome to OpenCPN"), wxCANCEL | wxOK );

      return(odlg.ShowModal());
}



// `Main program' equivalent, creating windows and returning main app frame
//------------------------------------------------------------------------------
// MyApp
//------------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

#include "wx/dynlib.h"

bool MyApp::OnInit()
{
      if (!wxApp::OnInit())
            return false;

      g_pPlatform = new wxPlatformInfo;

      //    On MSW, force the entire process to run on one CPU core only
      //    This resolves some difficulty with wxThread syncronization
#ifdef __WXMSW__
      //Gets the current process handle
      HANDLE hProc = GetCurrentProcess();
      DWORD procMask;
      DWORD sysMask;
      HANDLE hDup;
      DuplicateHandle(hProc,
                      hProc,
                      hProc,
                      &hDup,
                      0,
                      FALSE,
                      DUPLICATE_SAME_ACCESS);

//Gets the current process affinity mask
      GetProcessAffinityMask(hDup,&procMask,&sysMask);

// Take a simple approach, and assume up to 4 processors
      DWORD newMask;
      if((procMask & 1) == 1) newMask = 1;
      else if((procMask & 2) == 2) newMask = 2;
      else if((procMask & 4) == 4) newMask = 4;
      else if((procMask & 8) == 8) newMask = 8;

//Set te affinity mask for the process
      BOOL res = SetProcessAffinityMask(hDup,(DWORD_PTR)newMask);

      if(res == 0 )
      {
         //Error setting affinity mask!!
      }
#endif




//Fulup: force floating point to use dot as separation.
// This needs to be set early to catch numerics in config file.
//#ifdef __POSIX__
         setlocale(LC_NUMERIC,"C");
//#endif






//      CALLGRIND_STOP_INSTRUMENTATION


      g_start_time = wxDateTime::Now();

      g_loglast_time = g_start_time;   // pjotrc 2010.02.09
	g_loglast_time.MakeGMT();        // pjotrc 2010.02.09
      g_loglast_time.Subtract(wxTimeSpan(0,29,0,0)); // give 1 minute for GPS to get a fix   // pjotrc 2010.02.09

      AnchorPointMaxDist = 1852.0*.5;  // pjotrc 2010.02.15 really should be an .init param
      AnchorPointMinDist = 10.0;       // pjotrc 2010.02.15 really should be an .init param

#ifdef __WXMSW__

      //    Handle any Floating Point Exceptions which may leak thru from other
      //    processes.  The exception filter is in cutil.c
      //    Seems to only happen for W98

      if(g_pPlatform->GetOperatingSystemId() == wxOS_WINDOWS_9X)
            SetUnhandledExceptionFilter( &MyUnhandledExceptionFilter );
#endif

#ifdef __WXMSW__
//     _CrtSetBreakAlloc(78287);
#endif


#ifndef __WXMSW__
//      Setup Linux SIGNAL handling, for external program control

//      Build the sigaction structure
        sa_all.sa_handler = catch_signals ;             // point to my handler
        sigemptyset(&sa_all.sa_mask);                  // make the blocking set
                                                        // empty, so that all
                                                        // other signals will be
                                                        // unblocked during my handler
        sa_all.sa_flags = 0;

        sigaction(SIGUSR1, NULL, &sa_all_old);             // save existing action for this signal

//      Register my request for some signals
        sigaction(SIGUSR1, &sa_all, NULL);

        sigaction(SIGUSR1, NULL, &sa_all_old);             // inspect existing action for this signal
#endif

//      Initialize memory tracer
#ifndef __WXMSW__
//        mtrace();
#endif

//      Here is some experimental code for wxTheme support
//      Not also these lines included above....
//      They are necessary to ensure that the themes are statically loaded

//      #ifdef __WXUNIVERSAL__
//      WX_USE_THEME(gtk);
//      WX_USE_THEME(Metal);
//      #endif

#ifdef __WXUNIVERSAL__
//        wxTheme* theme = wxTheme::Create("gtk");
//        wxTheme::Set(theme);
#endif


//      Init the private memory manager
        malloc_max = 0;

//      wxHandleFatalExceptions(true);

// Set up default FONT encoding, which should have been done by wxWidgets some time before this......
        wxFont temp_font(10, wxDEFAULT ,wxNORMAL, wxNORMAL, FALSE, wxString(_T("")), wxFONTENCODING_SYSTEM );
        temp_font.SetDefaultEncoding(wxFONTENCODING_SYSTEM );

//  Init my private font manager
        pFontMgr = new FontMgr();

//    Init the XPM image handler
        wxImage t;
        wxXPMHandler *th = new wxXPMHandler;
        t.AddHandler(th);

//      Establish a "home" location
        wxStandardPathsBase& std_path = wxApp::GetTraits()->GetStandardPaths();
        std_path.Get();

        pHome_Locn= new wxString;
        pHome_Locn->Append(std_path.GetUserConfigDir());          // on w98, produces "/windows/Application Data"
        appendOSDirSlash(pHome_Locn) ;

//        wxString loc = std_path.GetLocalizedResourcesDir(loc_lang_canonical, wxStandardPaths::ResourceCat_Messages);
//        wxLogMessage(loc);

#if defined( __WXMAC__) || defined ( __WXMSW__ )
        pHome_Locn->Append(_T("opencpn"));
        appendOSDirSlash(pHome_Locn) ;
#endif

        // create the opencpn "home" directory if we need to
        wxFileName wxHomeFiledir(*pHome_Locn) ;
        if(true != wxHomeFiledir.DirExists(wxHomeFiledir.GetPath()))
            if(!wxHomeFiledir.Mkdir(wxHomeFiledir.GetPath()))
                  {
                        wxASSERT_MSG(false,_T("Cannot create opencpn home directory"));
                        return false ;
                  }

//      Establish Log File location
        wxString log;
        log.Append(std_path.GetUserConfigDir());
        appendOSDirSlash(&log);

#ifdef __WXMAC__
        log.Append(_T("Logs/"));
#elif defined __WXMSW__
        log.Append(_T("opencpn\\"));
#endif

        // create the opencpn "log" directory if we need to
        wxFileName wxLogFiledir(log) ;
        if(true != wxLogFiledir.DirExists(wxLogFiledir.GetPath()))
              if(!wxLogFiledir.Mkdir(wxLogFiledir.GetPath()))
        {
              wxASSERT_MSG(false,_T("Cannot create opencpn log directory"));
              return false ;
        }

        log.Append(_T("opencpn.log"));

        //  Constrain the size of the log file
        if(::wxFileExists(log))
        {
            if(wxFileName::GetSize(log) > 1000000)
            {
                  wxString oldlog = log;                      // pjotrc 2010.02.09
                  oldlog.Append(_T(".log"));
                  wxString msg1 (_T("Old log will be moved to opencpn.log.log"));
                  wxMessageDialog mdlg(gFrame, msg1, wxString(_("OpenCPN Info")), wxICON_INFORMATION | wxOK );
                        int dlg_ret;
                        dlg_ret = mdlg.ShowModal();
                  ::wxRenameFile(log, oldlog);
            }
        }
        const char *mode = "a";
        flog = fopen(log.mb_str(), mode);
        logger=new wxLogStderr(flog);

        Oldlogger = wxLog::SetActiveTarget(logger);

//        wxLog::AddTraceMask("timer");               // verbose message traces to log output

//      Send init message
        wxLogMessage(_T("\n\n"));


        wxDateTime now = wxDateTime::Now();
        now.MakeGMT(true);                    // no DST

        wxString imsg = now.FormatISODate();
        wxLogMessage(imsg);

        imsg = _T(" -------Starting opencpn-------");
        wxLogMessage(imsg);

        wxString version(OpenCPNVersion,  wxConvUTF8);
        wxString vs = version.Trim(true);
        vs = vs.Trim(false);
        wxLogMessage(vs);



#ifdef USE_CPL
//      Set up a useable CPL library error handler
        CPLSetErrorHandler( MyCPLErrorHandler );
#endif


//    Initialize embedded PNG icon graphics
#ifdef OCPN_USE_PNGICONS
        ::wxInitAllImageHandlers();
//        wxImage::AddHandler(wxPNGHandler);
        initialize_images();
#endif


//      Create some static strings
        pNMEADataSource = new wxString();
        pNMEA_AP_Port = new wxString();
        pWIFIServerName = new wxString();
        pAIS_Port = new wxString();
        g_pcsv_locn = new wxString();
        pInit_Chart_Dir = new wxString();
        g_pSENCPrefix = new wxString();

//      Create an array string to hold repeating messages, so they don't
//      overwhelm the log
        pMessageOnceArray = new wxArrayString;

//      Init the Route Manager
        g_pRouteMan = new Routeman(this);

//      Init the WayPoint Manager
        pWayPointMan = new WayPointman();

//      Init the Selectable Route Items List
        pSelect = new Select();

//      Init the Selectable Tide/Current Items List
        pSelectTC = new Select();

//      Init the Selectable AIS Target List
        pSelectAIS = new Select();

//      Initially AIS display is always on       // pjotrc 2010.02.09
	  g_bShowAIS = true;                       // pjotrc 2010.02.09

//      Who am I?
        phost_name = new wxString(::wxGetHostName());


//      (Optionally) Capture the user and file(effective) ids
//  Some build environments may need root privileges for hardware
//  port I/O, as in the NMEA data input class.  Set that up here.


#ifndef __WXMSW__
#ifdef PROBE_PORTS__WITH_HELPER
        user_user_id = getuid ();
        file_user_id = geteuid ();
#endif
#endif


//      Establish a "shared data" location
/*  From the wxWidgets documentation...

        wxStandardPaths::GetDataDir
        wxString GetDataDir() const
        Return the location of the applications global, i.e. not user-specific, data files.
            * Unix: prefix/share/appname
            * Windows: the directory where the executable file is located
            * Mac: appname.app/Contents/SharedSupport bundle subdirectory
*/
        g_SData_Locn = std_path.GetDataDir();         // where the application is located
        appendOSDirSlash(&g_SData_Locn) ;

        imsg = _T("SData_Locn is ");
        imsg += g_SData_Locn;
        wxLogMessage(imsg);



//      Establish the location of the config file
#ifdef __WXMSW__
        wxString Config_File(_T("opencpn.ini"));
        Config_File.Prepend(*pHome_Locn);


#elif defined __WXMAC__
        wxString Config_File = std_path.GetUserConfigDir(); // should be ~/Library/Preferences
        appendOSDirSlash(&Config_File) ;
        Config_File.Append(_T("opencpn.ini"));
#else
        wxString Config_File = std_path.GetUserDataDir(); // should be ~/.opencpn
        appendOSDirSlash(&Config_File) ;
        Config_File.Append(_T("opencpn.conf"));
#endif

        bool b_novicemode = false;

        wxFileName config_test_file_name(Config_File);
        if(config_test_file_name.FileExists())
            wxLogMessage(_T("Using existing Config_File: ") + Config_File);
        else
        {
              {
                  wxLogMessage(_T("Creating new Config_File: ") + Config_File);

                  //    Flag to preset some options for initial config file creation
                  b_novicemode = true;

                  if(true != config_test_file_name.DirExists(config_test_file_name.GetPath()))
                       if(!config_test_file_name.Mkdir(config_test_file_name.GetPath()))
                               wxLogMessage(_T("Cannot create config file directory for ") + Config_File);
              }
        }

//      Open/Create the Config Object
        MyConfig *pCF = new MyConfig(wxString(_T("")), wxString(_T("")), Config_File);
        pConfig = (MyConfig *)pCF;
        pConfig->LoadMyConfig(0);

        //  Send the warning message if it has never been sent before, or if the version string has changed at all
        if(!n_NavMessageShown || (vs != g_config_version_string))
        {
              if(wxID_CANCEL == ShowNavWarning())
                    return false;
              n_NavMessageShown = 1;
        }

        g_config_version_string = vs;

        //  Now we can set the locale

        //    Manage internationalization of embedded messages
        //    using wxWidgets/gettext methodology....

        if(lang_list[0]){};                 // silly way to avoid compiler warnings

      // Add a new prefix for search order. New prefix = .\lang,
      // where '.' refers to the opencpn.exe directory
        wxLocale::AddCatalogLookupPathPrefix(wxT("./lang"));

        //  Get the default for info
        wxString def_lang_canonical = wxLocale::GetLanguageInfo(wxLANGUAGE_DEFAULT)->CanonicalName;
        imsg = _T("System default Language:  ");
        imsg += def_lang_canonical;
        wxLogMessage(imsg);


        //  Find the language specified by the config file
        const wxLanguageInfo *pli = wxLocale::FindLanguageInfo(g_locale);
        wxString loc_lang_canonical;
        bool b_initok;

        if(pli)
        {
              b_initok = locale_def_lang.Init( pli->Language, wxLOCALE_CONV_ENCODING );
              loc_lang_canonical = pli->CanonicalName;
        }

        if(!pli || !b_initok)
        {
              locale_def_lang.Init( wxLANGUAGE_ENGLISH_US, wxLOCALE_CONV_ENCODING );
              loc_lang_canonical = wxLocale::GetLanguageInfo(wxLANGUAGE_ENGLISH_US)->CanonicalName;
        }

        imsg = _T("Opencpn language set to:  ");
        imsg += loc_lang_canonical;
        wxLogMessage(imsg);

      // Set filename without extension (example : opencpn_fr_FR)
      // i.e. : Set-up the filename needed for translation
//        wxString loc_lang_filename = _T("opencpn_") + loc_lang_canonical;
        wxString loc_lang_filename = _T("opencpn");

      // Get translation file (example : opencpn_fr_FR.mo)
      // No problem if the file doesn't exist
      // as this case is handled by wxWidgets
        locale_def_lang.AddCatalog(loc_lang_filename);

      //    Always use dot as decimal
        setlocale(LC_NUMERIC,"C");

        //        A special case for windows, which has some trouble finding the data files....
#ifdef __WXMSW__
        wxString sfd(g_SData_Locn);
        appendOSDirSlash(&sfd);
        sfd.Append(_T("s57data"));

        if(!wxDir::Exists(sfd))                 // not found
        {
              wxString s_exe = std_path.GetExecutablePath();
              wxFileName fn_look(s_exe);
              while(fn_look.GetDirCount())
              {
                          wxString cpath = fn_look.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

                          // look for  path/s57data/S52RAZDS.RLE
                          wxString cp1 = cpath;
                          wxString cp1d = cp1;
                          cp1 += _T("s57data");
                          appendOSDirSlash(&cp1);
                          cp1 += _T("S52RAZDS.RLE");
                          if(wxFileName::FileExists(cp1))
                          {
                                g_SData_Locn = cp1d;
                                break;
                          }

                          // look for  path/data/s57data/S52RAZDS.RLE
                          cp1 = cpath;
                          cp1 += _T("data");
                          appendOSDirSlash(&cp1);
                          cp1d = cp1;
                          cp1 += _T("s57data");
                          appendOSDirSlash(&cp1);
                          cp1 += _T("S52RAZDS.RLE");
                          if(wxFileName::FileExists(cp1))
                          {
                                g_SData_Locn = cp1d;
                                break;
                          }

                          fn_look.RemoveLastDir();
              }
        }

#endif


#ifdef USE_S57
//      Init the s57 chart object, specifying the location of the required csv files

//      If the config file contains an entry for s57 .csv files,
//      confirm that the required files are available, and use them.
//      Otherwise, default to [shared data dir]/s57_data
        if(!g_pcsv_locn->IsEmpty())
        {
              wxString fd(*g_pcsv_locn);
              appendOSDirSlash(&fd);

              bool tflag = true;          // assume true
              wxFileName f1;
/*
              f1 = wxFileName(fd); fd.Append(_T("s57attributes.csv"));
              tflag &= f1.FileExists();
              f1 = wxFileName(fd); fd.Append(_T("attdecode.csv"));
              tflag &= f1.FileExists();
              f1 = wxFileName(fd); fd.Append(_T("s57expectedinput.csv"));
              tflag &= f1.FileExists();
              f1 = wxFileName(fd); fd.Append(_T("s57objectclasses.csv"));
              tflag &= f1.FileExists();
*/
              fd = *g_pcsv_locn; //PL
              appendOSDirSlash(&fd); //PL
              fd.Append(_T("s57attributes.csv"));
              f1 = wxFileName(fd);
              tflag &= f1.FileExists();

              fd = *g_pcsv_locn; //PL
              appendOSDirSlash(&fd); //PL
              fd.Append(_T("attdecode.csv"));
              f1 = wxFileName(fd);
              tflag &= f1.FileExists();

              fd = *g_pcsv_locn; //PL
              appendOSDirSlash(&fd); //PL
              fd.Append(_T("s57expectedinput.csv"));
              f1 = wxFileName(fd);
              tflag &= f1.FileExists();

              fd = *g_pcsv_locn; //PL
              appendOSDirSlash(&fd); //PL
              fd.Append(_T("s57objectclasses.csv"));
              f1 = wxFileName(fd);
              tflag &= f1.FileExists();

              if(!tflag)
              {
                    g_pcsv_locn->Clear();
                    g_pcsv_locn->Append(g_SData_Locn);
                    g_pcsv_locn->Append(_T("s57data"));
              }

        }
        else
        {
              g_pcsv_locn->Append(g_SData_Locn);
              g_pcsv_locn->Append(_T("s57data"));
        }

//      If the config file contains an entry for SENC file prefix, use it.
//      Otherwise, default to std_path.GetUserDataDir()
        if(g_pSENCPrefix->IsEmpty())
        {
              g_pSENCPrefix->Append(std_path.GetUserDataDir());
              appendOSDirSlash(g_pSENCPrefix);
              g_pSENCPrefix->Append(_T("SENC"));
        }

        // S52RAZDS.RLE

        wxString plib_data;

//      If the config file contains an entry for PresentationLibraryData, use it.
//      Otherwise, default to conditionally set spot under g_pcsv_locn
        if(g_PresLibData.IsEmpty())
        {
              plib_data = *g_pcsv_locn;
              appendOSDirSlash(&plib_data);
              plib_data.Append(_T("S52RAZDS.RLE"));
        }
        else
        {
              plib_data = g_PresLibData;
        }



        ps52plib = new s52plib(plib_data);

        //  If the library load failed, try looking for the s57 data elsewhere

        //  First, look in UserDataDir
/*    From wxWidgets documentation

        wxStandardPaths::GetUserDataDir
                    wxString GetUserDataDir() const
        Return the directory for the user-dependent application data files:
                    * Unix: ~/.appname
                    * Windows: C:\Documents and Settings\username\Application Data\appname
                    * Mac: ~/Library/Application Support/appname
*/

        if(!ps52plib->m_bOK)
        {
            delete ps52plib;

            wxString look_data_dir;
            look_data_dir.Append(std_path.GetUserDataDir());
            appendOSDirSlash(&look_data_dir) ;
            wxString tentative_SData_Locn = look_data_dir;
            look_data_dir.Append(_T("s57data"));

            plib_data = look_data_dir;
            appendOSDirSlash(&plib_data) ;
            plib_data.Append(_T("S52RAZDS.RLE"));

            wxLogMessage(_T("Looking for s57data in ") + look_data_dir);
            ps52plib = new s52plib(plib_data);

            if(ps52plib->m_bOK)
            {
                  *g_pcsv_locn = look_data_dir;
                  g_SData_Locn = tentative_SData_Locn;
            }
        }

        //  And if that doesn't work, look again in the original SData Location
        //  This will cover the case in which the .ini file entry is corrupted or moved

        if(!ps52plib->m_bOK)
        {
              delete ps52plib;

              wxString look_data_dir;
              look_data_dir = g_SData_Locn;
              look_data_dir.Append(_T("s57data"));

              plib_data = look_data_dir;
              appendOSDirSlash(&plib_data) ;
              plib_data.Append(_T("S52RAZDS.RLE"));

              wxLogMessage(_T("Looking for s57data in ") + look_data_dir);
              ps52plib = new s52plib(plib_data);

              if(ps52plib->m_bOK)
              {
                    *g_pcsv_locn = look_data_dir;
              }
        }



        if(ps52plib->m_bOK)
            wxLogMessage(_T("Using s57data in ") + *g_pcsv_locn);
        else
            wxLogMessage(_T("   S52PLIB Initialization failed, disabling S57 charts."));


// Todo Maybe initialize only when an s57 chart is actually opened???
        if(ps52plib->m_bOK)
              m_pRegistrarMan = new s57RegistrarMgr(*g_pcsv_locn, flog);


#endif  // S57


// Set default color scheme
        global_color_scheme = GLOBAL_COLOR_SCHEME_DAY;


//      Establish location and name of chart database
#ifdef __WXMSW__
        pChartListFileName = new wxString(_T("CHRTLIST.DAT"));
        pChartListFileName->Prepend(*pHome_Locn);

#else
        pChartListFileName = new wxString(_T(""));
        pChartListFileName->Append(std_path.GetUserDataDir());
        appendOSDirSlash(pChartListFileName) ;
        pChartListFileName->Append(_T("chartlist.dat"));
#endif


//      Establish location of Tide and Current data
        pTC_Dir = new wxString(_T("tcdata"));
        pTC_Dir->Prepend(g_SData_Locn);
        pTC_Dir->Append(wxFileName::GetPathSeparator());

        wxLogMessage(_T("Using Tide/Current data from:  ") + *pTC_Dir);




//      Establish guessed location of chart tree
        if(pInit_Chart_Dir->IsEmpty())
        {
            pInit_Chart_Dir->Append(_T("charts"));
            pInit_Chart_Dir->Prepend(g_SData_Locn);
        }


//      Establish the WorldVectorShoreline Dataset location
        pWVS_Locn = new wxString(_T("wvsdata"));
        pWVS_Locn->Prepend(g_SData_Locn);
        pWVS_Locn->Append(wxFileName::GetPathSeparator());

//      Reload the config data, to pick up any missing data class configuration info
//      e.g. s52plib, which could not be created until first config load completes
//      Think catch-22
        pConfig->LoadMyConfig(1);

        //  Override some config options for initial user startup with empty config file
        if(b_novicemode)
        {
              g_bShowOutlines = true;

              g_CPAMax_NM = 20.;
              g_CPAWarn_NM = 2.;
              g_TCPA_Max = 30.;
              g_bMarkLost = true;;
              g_MarkLost_Mins = 8;
              g_bRemoveLost = true;
              g_RemoveLost_Mins = 10;
              g_bShowCOG = true;
              g_ShowCOG_Mins = 6;
              g_bShowMoored = true;
              g_ShowMoored_Kts = 0.2;

              g_PlanSpeed = 6.;
        }


//  Set up the frame initial visual parameters
//      Default size, resized later
        wxSize new_frame_size(-1, -1);
        int cx, cy, cw, ch;
        ::wxClientDisplayRect(&cx, &cy, &cw, &ch);

        InitializeUserColors();

//  Create the global instance of the CommPortManager
        g_pCommMan = new ComPortManager;

//  For Windows and GTK, provide the expected application Minimize/Close bar
       long app_style = wxDEFAULT_FRAME_STYLE;

#ifdef __WXX11__
        app_style = (wxSIMPLE_BORDER | wxCLIP_CHILDREN | wxRESIZE_BORDER);

        //      Here is a specific size set for my (dsr) specific imbedded X11 environment
        new_frame_size.Set(cw, 735);
///        new_frame_size.Set(cw, ch);
#else
        if((g_nframewin_x > 100) && (g_nframewin_y > 100) && (g_nframewin_x <= cw) && (g_nframewin_y <=ch ))
            new_frame_size.Set(g_nframewin_x, g_nframewin_y);
        else
            new_frame_size.Set(cw * 7/10, ch * 7/10);
#endif


        app_style |= wxWANTS_CHARS;

// Create the main frame window
        gFrame = new MyFrame(NULL, _T("OpenCPN"), wxPoint(0, 0), new_frame_size, app_style );


//      Create Children of Frame
//              n.b.  if only one child exists, wxWindows expands the child
//                        to the parent client area automatically, (as a favor?)
//                        Here, we'll do explicit sizing on SIZE events

        cc1 =  new ChartCanvas(gFrame);                         // the chart display canvas

        cc1->SetQuiltMode(pConfig->m_bQuilt);                     // set initial quilt mode

        if(cc1)
        {
            cc1->m_bFollow = pConfig->st_bFollow;               // set initial state
            cc1->SetViewPoint ( vLat, vLon, initial_scale_ppm, 0., 0., CURRENT_RENDER );
        }


        cc1->SetFocus();


        console = new ConsoleCanvas(gFrame);                    // the console

        stats = new StatWin(gFrame);

        if(cc1->GetQuiltMode())
        {
            stats->pPiano->SetVizIcon(_img_viz);
            stats->pPiano->SetInVizIcon(_img_redX);

            stats->pPiano->SetRoundedRectangles(true);
        }
        stats->pPiano->SetTMercIcon(_img_tmercprj);
        stats->pPiano->SetPolyIcon(_img_polyprj);
        stats->pPiano->SetSkewIcon(_img_skewprj);

        stats->Show(true);

        pAPilot = new AutoPilotWindow(gFrame, *pNMEA_AP_Port);

#ifdef USE_WIFI_CLIENT
        pWIFI = new WIFIWindow(gFrame, *pWIFIServerName );
#endif

        pthumbwin = new ThumbWin(gFrame);

        gFrame->ApplyGlobalSettings(1, true);               // done once on init with resize

        gFrame->SetAndApplyColorScheme(global_color_scheme);

// Show the frame
        gFrame->ClearBackground();
        gFrame->Show(TRUE);

        if(g_bframemax)
            gFrame->Maximize(true);


//      Try to load the current chart list Data file
        ChartData = new ChartDB(gFrame);
        if(!ChartData->LoadBinary(pChartListFileName))
        {
                bDBUpdateInProgress = true;

                if(g_ChartDirArray.GetCount())
                {
//              Create and Save a new Chart Database based on the hints given in the config file

                        wxString msg1(_("OpenCPN needs to update the chart database from config file entries...."));

                        wxMessageDialog mdlg(gFrame, msg1, wxString(_("OpenCPN Info")),wxICON_INFORMATION | wxOK );
                        int dlg_ret;
                        dlg_ret = mdlg.ShowModal();

                        delete ChartData;
                        ChartData = new ChartDB(gFrame);

                        wxString line(_T("Example of a long line which will make the dialog big enough to show chart paths"));
                        wxProgressDialog *pprog = new wxProgressDialog (  _("OpenCPN Chart Update"), line, 100, NULL,
                                    wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME);

                        ChartData->Create(g_ChartDirArray, pprog);
                        ChartData->SaveBinary(pChartListFileName);

                        delete pprog;
                }

                else                                            // No chart database, no config hints, so bail....
                {
                  wxLogMessage(_T("Chartlist file not found, config chart dir array is empty.  Chartlist target file is:") +
                              *pChartListFileName);

                  wxString msg1(_("           No Charts Installed.\nPlease select chart folders in ToolBox->Charts."));

                  wxMessageDialog mdlg(gFrame, msg1, wxString(_("OpenCPN Info")),wxICON_INFORMATION | wxOK );
                  mdlg.CentreOnParent();
                  int dlg_ret;
                  dlg_ret = mdlg.ShowModal();

                  gFrame->DoOptionsDialog();

                  //    As a favor to new users, poll the database and move the inital viewport so that a chart will come up.
                  double clat, clon;
                  if(ChartData->GetCentroidOfLargestScaleChart(&clat, &clon, CHART_FAMILY_RASTER))
                  {
                        gLat = clat;
                        gLon = clon;
                        gFrame->ClearbFollow();
                  }
                  else
                  {
                        if(ChartData->GetCentroidOfLargestScaleChart(&clat, &clon, CHART_FAMILY_VECTOR))
                        {
                              gLat = clat;
                              gLon = clon;
                              gFrame->ClearbFollow();
                        }
                  }

                }

                bDBUpdateInProgress = false;
        }

        pCurrentStack = new ChartStack;


//      All set to go.....

#ifndef __WXMSW__
//      Record initial memory status
        gFrame->GetMemoryStatus(mem_total, mem_initial);
        wxLogMessage(_T("MemoryStatus:  mem_total: %d kb,  mem_initial: %d kb"),
                                        mem_total, mem_initial);
#endif

//      establish GPS timeout value as multiple of frame timer
//      This will override any nonsense or unset value from the config file
        if((gps_watchdog_timeout_ticks > 60) || (gps_watchdog_timeout_ticks <= 0))
              gps_watchdog_timeout_ticks = (GPS_TIMEOUT_SECONDS * 1000) / TIMER_GFRAME_1;

        wxString dogmsg;
        dogmsg.Printf(_T("GPS Watchdog Timeout is: %d sec."), gps_watchdog_timeout_ticks);
        wxLogMessage(dogmsg);

        sat_watchdog_timeout_ticks = 12;

        gGPS_Watchdog = 2;
        gHDx_Watchdog = 2;
        gSAT_Watchdog = 2;


//  Start up a new track if enabled in config file
//        test this
        if(g_bTrackCarryOver)
              gFrame->TrackOn();

//    Re-enable anchor watches if set in config file
        if(!g_AW1GUID.IsEmpty())
        {
              pAnchorWatchPoint1 = pWayPointMan->FindRoutePointByGUID(g_AW1GUID);
        }
        if(!g_AW2GUID.IsEmpty())
        {
              pAnchorWatchPoint2 = pWayPointMan->FindRoutePointByGUID(g_AW2GUID);
        }


        gFrame->DoChartUpdate();

//      Start up the ticker....
        gFrame->FrameTimer1.Start(TIMER_GFRAME_1, wxTIMER_CONTINUOUS);

//      Start up the ViewPort Rotation angle Averaging Timer....
        gFrame->FrameCOGTimer.Start(10, wxTIMER_CONTINUOUS);

//        gFrame->MemFootTimer.Start(wxMax(g_MemFootSec * 1000, 60 * 1000), wxTIMER_CONTINUOUS);
//        gFrame->MemFootTimer.Start(1000, wxTIMER_CONTINUOUS);

//debug
//        g_COGAvg = 45.0;


        cc1->ReloadVP();                  // once more, and good to go
        return TRUE;
}



int MyApp::OnExit()
{
      //  Send current nav status data to log file   // pjotrc 2010.02.09

      wxDateTime lognow = wxDateTime::Now();
      lognow.MakeGMT();
      wxString day = lognow.FormatISODate();
      wxString utc = lognow.FormatISOTime();
      wxString navmsg = _T("LOGBOOK:  ");
      navmsg += day;
      navmsg += _T(" ");
      navmsg += utc;
      navmsg += _T(" UTC ");

      if (bGPSValid)
      {
            wxString data;
            data.Printf(_T("OFF: Lat %10.5f Lon %10.5f COG %10.5f SOG %6.2f"), gLat, gLon, gCog, gSog);
            navmsg += data;
      }
      else
      {
            wxString data;
            data.Printf(_T("OFF: Lat %10.5f Lon %10.5f"), gLat, gLon);
            navmsg += data;
      }
      wxLogMessage(navmsg);
      g_loglast_time = lognow;

      wxLogMessage(_T("opencpn::MyApp exiting cleanly...\n"));
        delete pConfig;
        delete pSelect;
        delete pSelectTC;
        delete pSelectAIS;


#ifdef USE_S57
        delete ps52plib;
#endif

        delete pDummyChart;

        if(ptcmgr)
                delete ptcmgr;

        if(logger)
        {
                wxLog::SetActiveTarget(Oldlogger);
                delete logger;
        }

        delete pChartListFileName;
        delete pHome_Locn;
        delete g_pcsv_locn;
        delete g_pSENCPrefix;
        delete pTC_Dir;
        delete phost_name;
        delete pInit_Chart_Dir;
        delete pWVS_Locn;

        delete pNMEADataSource;
        delete pNMEA_AP_Port;
        delete pAIS_Port;

        delete pWIFIServerName;

        delete pFontMgr;

        delete g_pRouteMan;
        delete pWayPointMan;

        delete pMessageOnceArray;

        delete s_pcm93mgr;

        DeInitializeUserColors();

        delete g_pCommMan;

#ifdef USE_S57
        delete m_pRegistrarMan;
        CSVDeaccess(NULL);
#endif

#ifdef USE_S57
#ifdef __WXMSW__
#ifdef USE_GLU_TESS
#ifdef USE_GLU_DLL
        if(s_glu_dll_ready)
            FreeLibrary(s_hGLU_DLL);           // free the glu32.dll
#endif
#endif
#endif
#endif

//        _CrtDumpMemoryLeaks( );

        //      Restore any changed system colors
#ifdef __WXMSW__
    RestoreSystemColors();
#endif

#ifdef __MSVC__LEAK
    DeInitAllocCheck();
#endif

    delete g_pPlatform;

    return TRUE;
}


void MyApp::TestSockets(void)
{
//      wxWidgets for X11, starting with version 2.6 and extending however
//      long, has a problem handling TCP/IP socket events.
//      The problem relates to wxHashTable::Next()
//      There is a workaround:  configure wxWidgets with --enable-compat24
//      Anyway, we'll test here to be sure sockets work HERE.
//      If they don't, we shall have to disable GPSD data input support


//  Create a dynamic event handler in MyApp for wxSocketServer events
        Connect( wxID_ANY, wxEVT_SOCKET, wxSocketEventHandler(MyApp::OnSocketEvent) );

//      Make a localhost server
        wxIPV4address serv_addr;
        serv_addr.Service(3001);

        s_s_sock = new wxSocketServer(serv_addr);
        s_s_sock->SetEventHandler(*this, wxID_ANY);

        s_s_sock->SetNotify(wxSOCKET_CONNECTION_FLAG);
        s_s_sock->Notify(TRUE);

// Create the test client socket
        s_t_sock = new wxSocketClient();

//      Connect() the test socket

        wxIPV4address addr;
        addr.AnyAddress();
        addr.Service(3001);

        s_socket_test_running = true;
        s_socket_test_passed = false;

        s_t_sock->Connect(addr, FALSE);       // Non-blocking connect

//    Sleep and loop for N seconds
#define SLEEP_TEST_SEC  1
        for(int is=0 ; is<SLEEP_TEST_SEC * 10 ; is++)
{
    wxMilliSleep(100);
}

// Test requires at least one pass thru event loop.  Test will finish in first MyFrame::OnTimer

}



void MyApp::OnSocketEvent(wxSocketEvent& event)
//  Used for testing wxSocketClient event handling
{

    switch(event.GetSocketEvent())
    {
        case wxSOCKET_CONNECTION :
        case wxSOCKET_INPUT:
        case wxSOCKET_OUTPUT:
        case wxSOCKET_LOST:

            s_socket_test_passed = true;
            break;
    }
}


void MyApp::TrackOff(void)
{
      if(gFrame)
            gFrame->TrackOff();
}

/*
int MyApp::FilterEvent(wxEvent& event)
{
      if ( event.GetEventType() == wxEVT_CHAR )
      {
            if(cc1->Do_Hotkeys((wxKeyEvent&)event))
            return true;
      }
      return -1;
}
*/

//------------------------------------------------------------------------------
// MyFrame
//------------------------------------------------------------------------------


//      Frame implementation
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_CLOSE(MyFrame::OnCloseWindow)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_SIZE(MyFrame::OnSize)
  EVT_MENU(-1, MyFrame::OnToolLeftClick)
  EVT_TIMER(FRAME_TIMER_1, MyFrame::OnFrameTimer1)
  EVT_TIMER(FRAME_TC_TIMER, MyFrame::OnFrameTCTimer)
  EVT_TIMER(FRAME_COG_TIMER, MyFrame::OnFrameCOGTimer)
  EVT_TIMER(MEMORY_FOOTPRINT_TIMER, MyFrame::OnMemFootTimer)
  EVT_ACTIVATE(MyFrame::OnActivate)
  EVT_MAXIMIZE(MyFrame::OnMaximize)
  EVT_COMMAND(wxID_ANY, EVT_NMEA, MyFrame::OnEvtNMEA)
  EVT_COMMAND(wxID_ANY, EVT_THREADMSG, MyFrame::OnEvtTHREADMSG)
  EVT_ERASE_BACKGROUND(MyFrame::OnEraseBackground)
END_EVENT_TABLE()


// My frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
  wxFrame(frame, -1, title, pos, size, style)  //wxSIMPLE_BORDER | wxCLIP_CHILDREN | wxRESIZE_BORDER)
      //wxCAPTION | wxSYSTEM_MENU | wxRESIZE_BORDER
{
        m_ulLastNEMATicktime = 0;
        m_pStatusBar = NULL;

        m_toolBar = NULL;
        m_bneedtoolbar = false;

        PrepareToolbarBitmaps();

//        m_pStatBmp = NULL;

        //      Redirect the global heartbeat timer to this frame
        FrameTimer1.SetOwner(this, FRAME_TIMER_1);

        //      Redirect the Tide/Current update timer to this frame
        FrameTCTimer.SetOwner(this, FRAME_TC_TIMER);

        //      Redirect the COG Averager timer to this frame
        FrameCOGTimer.SetOwner(this, FRAME_COG_TIMER);

        //      Redirect the Memory Footprint Management timer to this frame
        MemFootTimer.SetOwner(this, MEMORY_FOOTPRINT_TIMER);

        //      Set up some assorted member variables
        nRoute_State = 0;
        m_bTimeIsSet = false;

        m_phash = NULL;

        m_pStatBoxToolStaticBmp = NULL;
        m_pStatBoxTool = NULL;

        m_ptool_ct_dummyStaticBmp = NULL;
        m_pStatDummyTool = NULL;



//    Establish my children
#ifdef __WXOSX__
        if (false == ValidateSerialPortName(pNMEADataSource->mb_str(),MAX_SERIAL_PORTS))
              *pNMEADataSource = _T("NONE") ;
        if (false == ValidateSerialPortName(pAIS_Port->mb_str(),MAX_SERIAL_PORTS))
              *pAIS_Port = _T("NONE") ;
        if (false == ValidateSerialPortName(pNMEA_AP_Port->mb_str(), MAX_SERIAL_PORTS))
              *pNMEA_AP_Port = _T("NONE") ;
#endif

        //    If the selected port is the same as AIS port, override the name to force the
       //    NMEA class to expect muxed data from AIS decoder
        if(pNMEADataSource->IsSameAs(*pAIS_Port))
              g_pnmea = new NMEAWindow(ID_NMEA_WINDOW, this, _T("AIS Port (Shared)"), g_NMEABaudRate, &m_mutexNMEAEvent );
        else
              g_pnmea = new NMEAWindow(ID_NMEA_WINDOW, this, *pNMEADataSource, g_NMEABaudRate, &m_mutexNMEAEvent );


//        pAIS = new AIS_Decoder(ID_AIS_WINDOW, gFrame, wxString("TCP/IP:66.235.48.168"));  // a test
        g_pAIS = new AIS_Decoder(ID_AIS_WINDOW, this, *pAIS_Port, &m_mutexNMEAEvent);


        //  Create/connect a dynamic event handler slot for OCPN_NMEAEvent(s) coming from NMEA or AIS threads
        Connect(wxEVT_OCPN_NMEA, (wxObjectEventFunction)(wxEventFunction)&MyFrame::OnEvtOCPN_NMEA);

//  Initialize the Printer data structures

//wxGTK 2.8.0 enables gnomeprint by default.  No workee under KDevelop....

#ifdef __WXDEBUG
        if( wxPORT_GTK != g_pPlatform->GetPortId())
        {
            g_printData = new wxPrintData;
            g_printData->SetOrientation(wxLANDSCAPE);
            g_pageSetupData = new wxPageSetupDialogData;
        }
#else
        g_printData = new wxPrintData;
        g_printData->SetOrientation(wxLANDSCAPE);
        g_pageSetupData = new wxPageSetupDialogData;

#endif

        bFirstAuto = true;

 //        Establish the system icons for the frame.

#ifdef __WXMSW__
       SetIcon(wxICON(0));                // this grabs the first icon in the integrated MSW resource file
#endif

#ifdef __WXGTK__
       wxIcon app_icon(opencpn);          // This comes from opencpn.xpm inclusion above
       SetIcon(app_icon);
#endif



#ifdef __WXMSW__

//    Establish the entry points in USER32.DLL for system color control

        wxDynamicLibrary dllUser32(_T("user32.dll"));

        pSetSysColors = (SetSysColors_t)dllUser32.GetSymbol(wxT("SetSysColors"));
        pGetSysColor = (GetSysColor_t)dllUser32.GetSymbol(wxT("GetSysColor"));

        SaveSystemColors();
#endif



}

MyFrame::~MyFrame()
{
        FrameTimer1.Stop();
        delete ChartData;
        delete pCurrentStack;

//      Free the Route List
        wxRouteListNode *node = pRouteList->GetFirst();

        while(node)
        {
            Route *pRouteDelete = node->GetData();
            delete pRouteDelete;

            node = node->GetNext();
        }
        delete pRouteList;

//        delete m_pStatBmp;
}


void MyFrame::OnEraseBackground(wxEraseEvent& event)
{
}

void MyFrame::OnMaximize(wxMaximizeEvent& event)
{
      g_click_stop = 0;
}

void MyFrame::OnActivate(wxActivateEvent& event)
{
//    Code carefully in this method.
//    It is called in some unexpected places,
//    such as on closure of dialogs, etc.

//      Activating?

      if(!event.GetActive())
      {
//            g_click_stop = 2;
      }

      if(cc1)
        cc1->SetFocus();            // This seems to be needed for MSW, to get key and wheel events
                                    // after minimize/maximize.
      event.Skip();
}


ColorScheme MyFrame::GetColorScheme()
{
       return global_color_scheme;
}


void MyFrame::SetAndApplyColorScheme(ColorScheme cs)
{
      global_color_scheme = cs;

      wxString SchemeName;
      switch(cs)
      {
            case GLOBAL_COLOR_SCHEME_DAY:
                  SchemeName = _T("DAY");
                  break;
            case GLOBAL_COLOR_SCHEME_DUSK:
                  SchemeName = _T("DUSK");
                  break;
            case GLOBAL_COLOR_SCHEME_NIGHT:
                  SchemeName = _T("NIGHT");
                  break;
            default:
                  SchemeName = _T("DAY");
                  break;
      }

      if(ps52plib)
             ps52plib->SetPLIBColorScheme(SchemeName);


        //Search the user color table array to find the proper hash table
      Usercolortable_index = 0;
      for ( unsigned int i=0 ; i< UserColorTableArray->GetCount() ; i++ )
      {
            colTable *ct = ( colTable * ) UserColorTableArray->Item ( i );
            if ( SchemeName.IsSameAs(*ct->tableName ))
            {
                  Usercolortable_index = i;
                  break;
            }
      }

      //    Set up a pointer to the proper hash table
      pcurrent_user_color_hash = ( ColourHash * ) UserColourHashTableArray->Item ( Usercolortable_index );


      SetSystemColors(cs);

      if(cc1)
            cc1->SetColorScheme(cs);

      if(pWayPointMan)
            pWayPointMan->SetColorScheme(cs);

      if(ChartData)
            ChartData->ApplyColorSchemeToCachedCharts(cs);

      if(stats)
            stats->SetColorScheme(cs);

      if(console)
            console->SetColorScheme(cs);

      if(g_pRouteMan)
            g_pRouteMan->SetColorScheme(cs);

      if(pMarkPropDialog)
            pMarkPropDialog->SetColorScheme(cs);

      if(pRoutePropDialog)
            pRoutePropDialog->SetColorScheme(cs);

            // toh, 2009.02.08
      if(pMarkInfoDialog)
            pMarkInfoDialog->SetColorScheme(cs);

      //    For the AIS target query dialog, we must rebuild it to incorporate the style desired for the colorscheme selected
      if(g_pais_query_dialog_active)
      {
            bool b_isshown = g_pais_query_dialog_active->IsShown();
            int n_mmsi = g_pais_query_dialog_active->GetMMSI();
            if(b_isshown)
                  g_pais_query_dialog_active->Show(false);              // dismiss it


            g_pais_query_dialog_active->Close();

            g_pais_query_dialog_active = new AISTargetQueryDialog();
            g_pais_query_dialog_active->Create ( this, -1, _( "AIS Target Query" ), wxPoint(g_ais_query_dialog_x, g_ais_query_dialog_y));
            g_pais_query_dialog_active->SetMMSI(n_mmsi);
            g_pais_query_dialog_active->UpdateText();
            if(b_isshown)
                  g_pais_query_dialog_active->Show();
      }

      ApplyGlobalColorSchemetoStatusBar();

      UpdateToolbar(cs);
//      UpdateToolbarStatusWindow(Current_Ch, true);
}


void MyFrame::ApplyGlobalColorSchemetoStatusBar(void)
{
      if(m_pStatusBar != NULL)
      {
            m_pStatusBar->SetBackgroundColour(GetGlobalColor(_T("UIBDR")));    //UINFF
            m_pStatusBar->ClearBackground();

          //    As an optimization, if color scheme is anything other than GLOBAL_COLOR_SCHEME_DAY,
          //    adjust the status bar field styles to be simple flat fields, with no unmanageable 3-D
          //    effects.

            int sb_style;
            if((global_color_scheme == GLOBAL_COLOR_SCHEME_DAY) ||
                (global_color_scheme == GLOBAL_COLOR_SCHEME_RGB))
                  sb_style = wxSB_NORMAL;
            else
                  sb_style = wxSB_FLAT;

            int sb_styles[N_STATUS_BAR_FIELDS_MAX];
            for(int i=0 ; i<m_StatusBarFieldCount ; i++)
            {
                  if(i < N_STATUS_BAR_FIELDS_MAX)
                        sb_styles[i] = sb_style;
            }

            m_pStatusBar->SetStatusStyles(m_StatusBarFieldCount, (const int *)&sb_styles[0]);

//          m_pStatusBar->Refresh(false);
      }

}

void MyFrame::DestroyMyToolbar()
{
//    delete m_ptool_ct_dummy;

    if(m_toolBar)
    {
          m_toolBar->ClearTools();
          m_toolBar->Destroy();
          m_toolBar = NULL;
    }

}


ocpnToolBarSimple *MyFrame::CreateAToolbar()
{
    ocpnToolBarSimple *tb;

    long style = 0;
    style |= wxTB_HORIZONTAL ;
    m_ptool_ct_dummyStaticBmp = NULL;

    tb = new ocpnToolBarSimple(this, -1, wxPoint(-1, -1), wxSize(-1, -1), wxTB_HORIZONTAL | wxNO_BORDER | wxTB_FLAT);

    tb->SetToolBitmapSize(wxSize(32,32));
    tb->SetRows(1);


//Set up the ToolBar


//      Set up the margins, etc...
//      All this is poorly documented, and very clumsy
//      see the wx src files

//  On MSW, ToolPacking does nothing
    tb->SetToolPacking(1);
    int tool_packing = tb->GetToolPacking();


//  On MSW, ToolMargins does nothing
    wxSize defMargins = tb->GetMargins();
    tb->SetMargins(6, defMargins.y);
    wxSize tool_margin = tb->GetMargins();


    tb->SetToolSeparation(5);                  // width of separator
    int tool_sep = tb->GetToolSeparation();

//      Calculate the tool and separator pitches
    wxSize toolsize = tb->GetToolSize();

    int x = 0;                                          // running position
    int pitch_tool, pitch_sep;

    pitch_tool = toolsize.x + tool_packing + tool_margin.x;
    pitch_sep =  tool_sep   + tool_packing + tool_margin.x;
    x =tool_packing + tool_margin.x;

      // Some platform specific fixups needed here
#ifdef __WXMSW__
      pitch_tool = toolsize.x;
      pitch_sep =  tool_sep + 1;
      x = 3;
#endif

#ifdef __WXGTK__
    {
        pitch_tool += 5;  //2

      //    More hacks to this sorry class.
        if((wxMAJOR_VERSION == 2) && (wxMINOR_VERSION >= 8))
            x = 0;
    }
#endif

//    printf("toolsize.x: %d    tool_packing: %d    tool_margin.x %d  tool_sep: %d x0: %d\n", toolsize.x, tool_packing, tool_margin.x, tool_sep, x);

#ifdef ocpnUSE_OPNCTOOLBAR
    tb->SetMargins(6, 8);
    tb->SetToolPacking(8);
    pitch_tool = toolsize.x + tb->GetToolPacking();
    tool_margin = tb->GetMargins();
    x = tool_margin.x;
#endif


    //  Fetch a sample bitmap for a placeholder
    string_to_pbitmap_hash *phash;
    phash = m_phash;


    tb->AddTool( ID_ZOOMIN, _T(""), *(*phash)[wxString(_T("zoomin"))], _T(""), wxITEM_NORMAL);
    x += pitch_tool;
    tb->AddTool( ID_ZOOMOUT, _T(""), *(*phash)[wxString(_T("zoomout"))], _T(""), wxITEM_NORMAL);
    x += pitch_tool;


    {
      tb->AddTool( ID_STKDN, _T(""), *(*phash)[wxString(_T("scin"))], _("Shift to Larger Scale Chart"), wxITEM_NORMAL);
      x += pitch_tool;
      tb->AddTool( ID_STKUP, _T(""),*(*phash)[wxString(_T("scout"))], _("Shift to Smaller Scale Chart"), wxITEM_NORMAL);
      x += pitch_tool;
    }


    tb->AddTool( ID_ROUTE, _T(""), *(*phash)[wxString(_T("route"))], _("Create Route"), wxITEM_NORMAL);
    x += pitch_tool;
    tb->AddTool( ID_FOLLOW, _T(""), *(*phash)[wxString(_T("follow"))], _("Auto Follow"), wxITEM_CHECK);
    x += pitch_tool;


    tb->AddTool( ID_SETTINGS, _T(""), *(*phash)[wxString(_T("settings"))], _("ToolBox"), wxITEM_NORMAL);
    x += pitch_tool;
    tb->AddTool( ID_TEXT, _T(""), *(*phash)[wxString(_T("text"))], _("Show ENC Text"), wxITEM_CHECK);
    x += pitch_tool;

    m_pAISTool = NULL;
    if(!pAIS_Port->IsSameAs(_T("None"), false))
    {
//          m_pAISTool = tb->AddTool( ID_AIS, _T(""), *(*phash)[wxString(_T("AIS_Normal"))], _("Show AIS Targets"), wxITEM_CHECK);   // pjotrc 2010.02.09
          m_pAISTool = tb->AddTool( ID_AIS, _T(""), *(*phash)[wxString(_T("AIS_Normal"))], *(*phash)[wxString(_T("AIS_Disabled"))],
                                    wxITEM_CHECK, _("Show AIS Targets"));
          x += pitch_tool;
    }



    tb->AddTool( ID_CURRENT, _T(""), *(*phash)[wxString(_T("current"))], _("Show Currents"), wxITEM_CHECK);
    x += pitch_tool;
    tb->AddTool( ID_TIDE, _T(""), *(*phash)[wxString(_T("tide"))], _("Show Tides"), wxITEM_CHECK);
    x += pitch_tool;


    if(g_bShowPrintIcon)
    {
        tb->AddTool( ID_PRINT, _T(""), *(*phash)[wxString(_T("print"))], _("Print Chart"), wxITEM_NORMAL);
        x += pitch_tool;
        x += 1;                     // now why in the heck is this necessary?????? Grrrrrrrr.
    }

    if (g_bShowGPXIcons)
    {
          tb->AddTool( ID_GPXIMPORT, _T(""), *(*phash)[wxString(_T("gpx_import"))], _("Import GPX file"), wxITEM_NORMAL);
          x += pitch_tool;
          x += 1;                     // now why in the heck is this necessary?????? Grrrrrrrr.

          tb->AddTool( ID_GPXEXPORT, _T(""), *(*phash)[wxString(_T("gpx_export"))], _("Export GPX file"), wxITEM_NORMAL);
          x += pitch_tool;
          x += 1;                     // now why in the heck is this necessary?????? Grrrrrrrr.
    }

    if (g_bShowTrackIcon)
    {
          tb->AddTool( ID_TRACK, _T(""), *(*phash)[wxString(_T("track"))], _("Toggle Tracking"), wxITEM_CHECK);
          x += pitch_tool;
          x += 1;                     // now why in the heck is this necessary?????? Grrrrrrrr.
    }

    if (g_bShowGRIBIcon)
    {
          if((*phash)[wxString(_T("grib"))])
          {
            tb->AddTool( ID_GRIB, _T(""), *(*phash)[wxString(_T("grib"))], _("Show GRIB dialog"), wxITEM_NORMAL);
            x += pitch_tool;
            x += 1;                     // now why in the heck is this necessary?????? Grrrrrrrr.
          }
    }



    tb->AddTool( ID_COLSCHEME, _T(""), *(*phash)[wxString(_T("colorscheme"))], _("Change Color Scheme"), wxITEM_NORMAL);
    x += pitch_tool;

    tb->AddTool( ID_MOB, _T(""), *(*phash)[wxString(_T("mob_btn"))], _("Drop MOB Marker"), wxITEM_NORMAL);
    x += pitch_tool;

    tb->AddTool( ID_HELP, _T(""), *(*phash)[wxString(_T("help"))], _("About OpenCPN"), wxITEM_NORMAL);
    x += pitch_tool;



    // The status filler tool will be here
    m_statTool_pos = tb->GetToolPos(ID_HELP) + 1;

//      Create and add a dummy control, to arrange for the "exit" tool to be
//      at right margin

    int tx,ty;
    GetClientSize(&tx, &ty);                    // of the frame

    int filler_width = tx - x;
    int filler_pad = tool_margin.x + tool_packing;
    toolbar_width_without_static = x;     // used by Resize()

    if(g_bbigred)
    {
          filler_width -= (pitch_tool + filler_pad);
          toolbar_width_without_static += (pitch_tool + filler_pad);
    }

    //      Account for the StatBox
    //      Build the (bare) StatBox bitmap
//    needs to be deleted in destroymytoolbar or in frame dtor, or amke it copy(not ref) from automatic....
//    delete m_pStatBmp;
//    wxBitmap *m_pStatBmp = new wxBitmap((_img_compass->GetWidth() + _img_gpsRed->GetWidth()) + 8, _img_compass->GetHeight() + 8);
    m_StatBmp.Create((_img_compass->GetWidth() + _img_gpsRed->GetWidth()) + 8, _img_compass->GetHeight() + 8);


    //    Clear the bitmap
    wxMemoryDC mdc;
    mdc.SelectObject(m_StatBmp);

    mdc.SetBackground(wxBrush(GetGlobalColor(_T("GREY2")), wxSOLID));
    mdc.Clear();
    mdc.SelectObject(wxNullBitmap);

    filler_width -= (m_StatBmp.GetWidth() + filler_pad);

    //      And 2 more pixels for good looks
    filler_width -= 2;

#define DUMMY_HEIGHT    40
    m_tool_dummy_size_x = filler_width;
    m_tool_dummy_size_y = DUMMY_HEIGHT;
//    printf("x: %d   filler_pad: %d  filler_width: %d  m_tool_dummy_size_x: %d \n", x, filler_pad, filler_width, m_tool_dummy_size_x);

    //  Make sure to always create a reasonable dummy
    if(m_tool_dummy_size_x <= 0)
        m_tool_dummy_size_x = 1;

    wxImage tool_image_dummy(m_tool_dummy_size_x,32);
    wxBitmap tool_bm_dummy(tool_image_dummy);

    wxMemoryDC bmdc;
    bmdc.SelectObject(tool_bm_dummy);

    bmdc.SetBackground(wxBrush(GetGlobalColor(_T("GREY2")), wxSOLID));
    bmdc.Clear();
    bmdc.SelectObject(wxNullBitmap);


    m_ptool_ct_dummyStaticBmp = new wxStaticBitmap(tb, ID_TBSTAT, tool_bm_dummy, wxPoint(2000,0),
                                        wxSize(m_tool_dummy_size_x,DUMMY_HEIGHT),
                                        wxSIMPLE_BORDER, _T("staticBitmap"));

    m_pStatDummyTool = tb->AddControl(m_ptool_ct_dummyStaticBmp);


    //      Add the StatBox tool
    m_pStatBoxToolStaticBmp = new wxStaticBitmap(tb, ID_TBSTATBOX, m_StatBmp, wxPoint(3000,0),
                                        wxSize(m_StatBmp.GetWidth(),DUMMY_HEIGHT),
                                                      wxSIMPLE_BORDER, _T("staticBitmap"));

    m_pStatBoxTool = tb->AddControl(m_pStatBoxToolStaticBmp);

    toolbar_width_without_static += m_StatBmp.GetWidth() + filler_pad + 2;


//      And add the "Exit" tool if called for
    if(g_bbigred)
      tb->AddTool( ID_TBEXIT, _T(""), *(*phash)[wxString(_T("exitt"))], _("Exit OpenCPN"), wxITEM_NORMAL);

    tb->SetRows(1);

// Realize() the toolbar
    tb->Realize();


//      Set up the toggle states
    if(pConfig)
        tb->ToggleTool(ID_FOLLOW, pConfig->st_bFollow);

#ifdef USE_S57
    if((pConfig) && (ps52plib))
        if(ps52plib->m_bOK)
            tb->ToggleTool(ID_TEXT, ps52plib->GetShowS57Text());
#endif
          tb->ToggleTool(ID_AIS, g_bShowAIS);           // pjotrc 2010.02.10

    if (g_bShowTrackIcon)
          tb->ToggleTool(ID_TRACK, g_bTrackActive);


    //      Induce re-rendering of the statusbox
    //      TODO not very elegant....
    m_rose_angle = -999;
    m_last_gps_bmp_hash_index.Clear();

    SetStatusBarPane(-1);                   // don't show help on status bar

    return tb;
}



wxBitmap *ConvertRedToBlue(wxBitmap *pbmp )
{
      wxImage *pimg = new wxImage(pbmp->ConvertToImage());

      wxImage img_dup = pimg->Copy();


#if wxCHECK_VERSION(2, 8, 0)
            wxImage img_dupG = img_dup;       //.ConvertToGreyscale();
#else // replacement code for old version
            wxImage img_dupG = img_dup;
#endif
            img_dup = img_dupG;

            int gimg_width = img_dup.GetWidth();
            int gimg_height = img_dup.GetHeight();

            for(int iy=0 ; iy < gimg_height ; iy++)
            {
                  for(int ix=0 ; ix < gimg_width ; ix++)
                  {
                        if(!img_dup.IsTransparent(ix, iy))
                        {
                              wxImage::RGBValue rgb(img_dup.GetRed(ix, iy), img_dup.GetGreen(ix, iy), img_dup.GetBlue(ix, iy));
                              wxImage::HSVValue hsv = wxImage::RGBtoHSV(rgb);

                              double angle = .6;
                              hsv.hue = hsv.hue + angle;
                              if (hsv.hue > 1.0)
                                    hsv.hue = hsv.hue - 1.0;
                              else if (hsv.hue < 0.0)
                                    hsv.hue = hsv.hue + 1.0;

                              wxImage::RGBValue nrgb = wxImage::HSVtoRGB(hsv);
                              img_dup.SetRGB(ix, iy, nrgb.red, nrgb.green, nrgb.blue);
                        }
                  }
            }



       //  Make a bitmap
      wxBitmap tbmp(img_dup.GetWidth(),img_dup.GetHeight(),-1);
      wxMemoryDC dwxdc;
      dwxdc.SelectObject(tbmp);
      wxBitmap *ptoolBarBitmap;

#ifdef __WXMSW__
      ptoolBarBitmap = new wxBitmap(img_dup, (wxDC &)dwxdc);
#else
      ptoolBarBitmap = new wxBitmap(img_dup);
#endif

      delete pimg;

      return ptoolBarBitmap;
}














//      Some helpers functions for Toolbar support
void MyFrame::PrepareToolbarBitmaps(void)
{

#ifdef OCPN_USE_PNGICONS
    // Load up all the toolbar bitmap xpm data pointers into a hash map
    tool_xpm_hash.clear();
    tool_xpm_hash[_T("settings")]     = (char *)_img_settings;
    tool_xpm_hash[_T("zoomin")]       = (char *)_img_zoomin;
    tool_xpm_hash[_T("zoomout")]                = (char *)_img_zoomout;
    tool_xpm_hash[_T("scin")]                   = (char *)_img_scin;
    tool_xpm_hash[_T("scout")]                  = (char *)_img_scout;
    tool_xpm_hash[_T("tide")]                   = (char *)_img_tide;
    tool_xpm_hash[_T("route")]                  = (char *)_img_route;
    tool_xpm_hash[_T("current")]                = (char *)_img_current;
    tool_xpm_hash[_T("text")]                   = (char *)_img_text;
    tool_xpm_hash[_T("print")]                  = (char *)_img_print;
    tool_xpm_hash[_T("exitt")]                  = (char *)_img_exitt;
    tool_xpm_hash[_T("follow")]                 = (char *)_img_follow;
    tool_xpm_hash[_T("help")]                   = (char *)_img_help;
    tool_xpm_hash[_T("colorscheme")]            = (char *)_img_colscheme;
    tool_xpm_hash[_T("gpx_import")]             = (char *)_img_gpx_import;
    tool_xpm_hash[_T("gpx_export")]             = (char *)_img_gpx_export;
    tool_xpm_hash[_T("track")]                  = (char *)_img_track;
    tool_xpm_hash[_T("grib")]                   = (char *)_img_grib;
    tool_xpm_hash[_T("mob")]                    = (char *)_img_mob;
    tool_xpm_hash[_T("mob_btn")]                = (char *)_img_mob_btn;

    tool_xpm_hash[_T("AIS_Normal")]             = (char *)_img_ais_alive;
    tool_xpm_hash[_T("AIS_Suppressed")]         = (char *)_img_ais_supressed;
    tool_xpm_hash[_T("AIS_AlertGeneral")]       = (char *)_img_ais_alarm;
    tool_xpm_hash[_T("AIS_Disabled")]           = (char *)_img_ais_disabled;

    tool_xpm_hash[_T("gps1Bar")]                = (char *)_img_gps1;
    tool_xpm_hash[_T("gps2Bar")]                = (char *)_img_gps2;
    tool_xpm_hash[_T("gps3Bar")]                = (char *)_img_gps3;
    tool_xpm_hash[_T("gpsGrn")]                 = (char *)_img_gpsGrn;
    tool_xpm_hash[_T("gpsGry")]                 = (char *)_img_gpsGry;
    tool_xpm_hash[_T("gpsRed")]                 = (char *)_img_gpsRed;
    tool_xpm_hash[_T("CompassRose")]            = (char *)_img_compass;


    wxBitmap *pBlueCompass = ConvertRedToBlue(_img_compass );
    tool_xpm_hash[_T("CompassRoseBlue")]        = (char *)pBlueCompass;


    wxImage pimg((char **)polyconic);
    _img_polyprj = new wxBitmap(pimg);


        //  Process all members of the XPM hashmap

    string_to_pchar_hash::iterator it;
    for( it = tool_xpm_hash.begin(); it != tool_xpm_hash.end(); ++it )
    {
          wxImage *pimg;

          wxString index = it->first;
          wxBitmap *px1 = (wxBitmap*)tool_xpm_hash[index];

//  Build Day Bitmap
          pimg = new wxImage(px1->ConvertToImage());
          BuildToolBitmap(pimg, 230, index, tool_bitmap_hash_day);
          delete pimg;

//  Build Dusk Bitmap
          pimg = new wxImage(px1->ConvertToImage());
          BuildToolBitmap(pimg, 128, index, tool_bitmap_hash_dusk);
          delete pimg;

//  Build Night Bitmap
          pimg = new wxImage(px1->ConvertToImage());
          BuildToolBitmap(pimg,  32, index, tool_bitmap_hash_night);
          delete pimg;
    }


#else
    // Load up all the toolbar bitmap xpm data pointers into a hash map
    tool_xpm_hash.clear();
    tool_xpm_hash[_T("settings")]     = (char *)settings;
    tool_xpm_hash[_T("zoomin")]       = (char *)zoomin;
    tool_xpm_hash[_T("zoomout")]      = (char *)zoomout;
    tool_xpm_hash[_T("scin")]         = (char *)scin;
    tool_xpm_hash[_T("scout")]        = (char *)scout;
    tool_xpm_hash[_T("tide")]         = (char *)tide;
    tool_xpm_hash[_T("route")]        = (char *)route;
    tool_xpm_hash[_T("current")]      = (char *)current;
    tool_xpm_hash[_T("text")]         = (char *)text;
    tool_xpm_hash[_T("print")]        = (char *)print;
    tool_xpm_hash[_T("exitt")]        = (char *)exitt;
    tool_xpm_hash[_T("follow")]       = (char *)follow;
    tool_xpm_hash[_T("help")]         = (char *)help;
    tool_xpm_hash[_T("colorscheme")]  = (char *)colscheme;
    tool_xpm_hash[_T("gpx_import")]   = (char *)gpx_import; // toh, 2009.02.14
    tool_xpm_hash[_T("gpx_export")]   = (char *)gpx_export; // toh, 2009.02.14
    tool_xpm_hash[_T("track")]        = (char *)track;



    //  Process all members of the XPM hashmap

    string_to_pchar_hash::iterator it;
    for( it = tool_xpm_hash.begin(); it != tool_xpm_hash.end(); ++it )
    {
          wxImage *pimg;

          wxString index = it->first;
          char **px1 = (char **)tool_xpm_hash[index];

//  Build Day Bitmap
          pimg = new wxImage(px1);
          BuildToolBitmap(pimg, 230, index, tool_bitmap_hash_day);
          delete pimg;

//  Build Dusk Bitmap
          pimg = new wxImage(px1);
          BuildToolBitmap(pimg, 128, index, tool_bitmap_hash_dusk);
          delete pimg;

//  Build Night Bitmap
          pimg = new wxImage(px1);
          BuildToolBitmap(pimg,  32, index, tool_bitmap_hash_night);
          delete pimg;
    }
#endif

}

void MyFrame::BuildToolBitmap(wxImage *pimg, unsigned char back_color, wxString &index, string_to_pbitmap_hash &hash)
{
        wxImage img_dup = pimg->Copy();

        if(back_color < 200)
        {
#if wxCHECK_VERSION(2, 8, 0)
              wxImage img_dupG = img_dup;       //.ConvertToGreyscale();
#else // replacement code for old version
              wxImage img_dupG = img_dup;
#endif
              img_dup = img_dupG;

              //  Create a dimmed version of the image/bitmap
              int gimg_width = img_dup.GetWidth();
              int gimg_height = img_dup.GetHeight();

              double factor = (double)(back_color) / 256.0;

              for(int iy=0 ; iy < gimg_height ; iy++)
              {
                    for(int ix=0 ; ix < gimg_width ; ix++)
                    {
                          if(!img_dup.IsTransparent(ix, iy))
                          {
                                wxImage::RGBValue rgb(img_dup.GetRed(ix, iy), img_dup.GetGreen(ix, iy), img_dup.GetBlue(ix, iy));
                                wxImage::HSVValue hsv = wxImage::RGBtoHSV(rgb);
                                hsv.value = hsv.value * factor;
                                wxImage::RGBValue nrgb = wxImage::HSVtoRGB(hsv);
                                img_dup.SetRGB(ix, iy, nrgb.red, nrgb.green, nrgb.blue);
                          }
                    }
              }
        }

        // Substitute the mask color with a fixed background color
/*
        unsigned char mask_r, mask_g, mask_b;
        img_dup.GetOrFindMaskColour(&mask_r, &mask_g, &mask_b);

        img_dup.Replace(mask_r, mask_g, mask_b, back_color,back_color,back_color);

        //Remove the mask from the image
        img_dup.SetMask(false);

*/
        //  Make a bitmap
        wxBitmap tbmp(img_dup.GetWidth(),img_dup.GetHeight(),-1);
        wxMemoryDC dwxdc;
        dwxdc.SelectObject(tbmp);
        wxBitmap *ptoolBarBitmap;

#ifdef __WXMSW__
        ptoolBarBitmap = new wxBitmap(img_dup, (wxDC &)dwxdc);
#else
        ptoolBarBitmap = new wxBitmap(img_dup);
#endif

        // store it
        hash[index] = ptoolBarBitmap;
}

void MyFrame::ReSizeToolbar(void)
{
    int tx, ty;
    GetClientSize(&tx, &ty);                    // of the frame

    int dummy_size_last = m_tool_dummy_size_x;
    m_tool_dummy_size_x = tx - toolbar_width_without_static;
    if(dummy_size_last != m_tool_dummy_size_x)
          m_bneedtoolbar = true;

}



//      Update inplace the current toolbar with bitmaps corresponding to the current color scheme
void MyFrame::UpdateToolbar(ColorScheme cs)
{

    //  Select the correct bitmap hash table and background color
    switch(cs)
    {
        case GLOBAL_COLOR_SCHEME_DAY:
            m_phash = &tool_bitmap_hash_day;
            break;
        case GLOBAL_COLOR_SCHEME_DUSK:
            m_phash = &tool_bitmap_hash_dusk;
            break;
        case GLOBAL_COLOR_SCHEME_NIGHT:
            m_phash = &tool_bitmap_hash_night;
            break;
        default:
            m_phash = &tool_bitmap_hash_day;
            break;
    }

#ifdef __WXOSX__
    // RMS Problems with 2.8.3 on the mac and Destroy toolbar
    //DestroyMyToolbar();
    if (0 == m_toolBar)
    {
      m_toolBar = CreateAToolbar();
      SetToolBar((wxToolBar *)m_toolBar);
    }
#else
    DestroyMyToolbar();
    m_toolBar = CreateAToolbar();
    SetToolBar((wxToolBar *)m_toolBar);
#endif

    wxColour back_color = GetGlobalColor(_T("GREY2"));            // Was GREY1, switched on v 1.3.4 transparent icons

    //  Set background
    m_toolBar->SetBackgroundColour(back_color);
    m_toolBar->ClearBackground();

#ifdef ocpnUSE_OPNCTOOLBAR
    m_toolBar->SetToggledBackgroundColour(GetGlobalColor(_T("GREY1")));

    m_toolBar->SetColorScheme(cs);
#endif

//    UpdateToolbarDynamics();
//    UpdateToolbarStatusBox(false);
//    UpdateToolbarStatusWindow(Current_Ch, false);

    //  Re-establish toggle states
    m_toolBar->ToggleTool(ID_FOLLOW, cc1->m_bFollow);
    m_toolBar->ToggleTool(ID_CURRENT, cc1->GetbShowCurrent());
    m_toolBar->ToggleTool(ID_TIDE, cc1->GetbShowTide());

    return;
}

void MyFrame::UpdateToolbarDynamics(void)
{
      if(m_bneedtoolbar)
      {
            UpdateToolbar(global_color_scheme);
            m_bneedtoolbar = false;
      }

      bool b_need_refresh = false;

      wxString bmp_hash_index;

      if(m_pAISTool)
      {
            bmp_hash_index = _T("AIS_Normal");
            if(g_pAIS->IsAISSuppressed())
                  bmp_hash_index = _T("AIS_Suppressed");
            if(g_pAIS->IsAISAlertGeneral())
                  bmp_hash_index = _T("AIS_AlertGeneral");

            if(m_AIS_bmp_hash_index_last != bmp_hash_index)
            {
                  m_pAISTool->SetNormalBitmap(*(*m_phash)[bmp_hash_index]);
                  b_need_refresh = true;
            }

      }

      if(b_need_refresh)
      {
            m_toolBar->Refresh();
            m_AIS_bmp_hash_index_last = bmp_hash_index;
      }
}

void MyFrame::DeleteToolbarBitmaps()
{
#ifdef OCPN_USE_PNGICONS

      string_to_pchar_hash::iterator it;
      for( it = tool_xpm_hash.begin(); it != tool_xpm_hash.end(); ++it )
      {
            wxString index = it->first;
            wxBitmap *px1 = (wxBitmap*)tool_xpm_hash[index];

            delete px1;
      }

      //    Delete everything no in the hash

    delete _img_left;
    delete _img_right;
    delete _img_up;
    delete _img_down;
    delete _img_pencil;

    delete _img_opencpn;

    delete _img_ship_green;
    delete _img_ship_red;

    delete _img_redX;
    delete _img_viz;
    delete _img_tmercprj;
    delete _img_mercprj;
    delete _img_skewprj;
    delete _img_inviz;
    delete _img_tidesml;

    delete _img_polyprj;
    delete _img_ais;

#endif
}

void MyFrame::EnableToolbar(bool newstate)
{
      if(m_toolBar)
      {
            m_toolBar-> EnableTool(ID_ZOOMIN, newstate);
            m_toolBar-> EnableTool(ID_ZOOMOUT, newstate);
            m_toolBar-> EnableTool(ID_STKUP, newstate);
            m_toolBar-> EnableTool(ID_STKDN, newstate);
            m_toolBar-> EnableTool(ID_ROUTE, newstate);
            m_toolBar-> EnableTool(ID_FOLLOW, newstate);
            m_toolBar-> EnableTool(ID_SETTINGS, newstate);
            m_toolBar-> EnableTool(ID_TEXT, newstate);
            m_toolBar-> EnableTool(ID_CURRENT, newstate);
            m_toolBar-> EnableTool(ID_TIDE, newstate);
            m_toolBar-> EnableTool(ID_HELP, newstate);
            m_toolBar-> EnableTool(ID_TBEXIT, newstate);
            m_toolBar-> EnableTool(ID_TBSTAT, newstate);
            m_toolBar-> EnableTool(ID_PRINT, newstate);
            m_toolBar-> EnableTool(ID_COLSCHEME, newstate);
            m_toolBar-> EnableTool(ID_GPXIMPORT, newstate);
            m_toolBar-> EnableTool(ID_GPXEXPORT, newstate);
            m_toolBar-> EnableTool(ID_TRACK, newstate);
            m_toolBar-> EnableTool(ID_GRIB, newstate);
            m_toolBar-> EnableTool(ID_AIS, newstate);
      }
}



// Intercept menu commands
void MyFrame::OnExit(wxCommandEvent& event)
{
        quitflag++;                             // signal to the timer loop

}

void MyFrame::OnCloseWindow(wxCloseEvent& event)
{
      //    It is possible that double clicks on application exit box could cause re-entrance here
      //    Not good, and don't need it anyway, so simply return.
      if(quitflag)
      {
//            wxLogMessage(_T("opencpn::MyFrame re-entering OnCloseWindow"));
            return;
      }

      ::wxSetCursor(wxCURSOR_WAIT);

      g_bquiting = true;
      if(cc1)
      {
            cc1->SetCursor(wxCURSOR_WAIT);

            cc1->Refresh(false);
            cc1->Update();
      }

      wxLogMessage(_T("opencpn::MyFrame exiting cleanly."));

      quitflag++ ;

      FrameTimer1.Stop();

    /*
          Automatically drop an anchorage waypoint, if enabled
          On following conditions:
          1.  In "Cruising" mode, meaning that speed has at some point exceeded 3.0 kts.
          2.  Current speed is less than 0.5 kts.
          3.  Opencpn has been up at least 30 minutes
          4.  And, of course, opencpn is going down now.
          5.  And if there is no anchor watch set on "anchor..." icon mark           // pjotrc 2010.02.15
    */
      if(g_bAutoAnchorMark)
      {
            bool watching_anchor = false;                                                              // pjotrc 2010.02.15
            if (pAnchorWatchPoint1)                                                                    // pjotrc 2010.02.15
                 watching_anchor = (pAnchorWatchPoint1->m_IconName.StartsWith(_T("anchor")));           // pjotrc 2010.02.15
            if (pAnchorWatchPoint2)                                                                    // pjotrc 2010.02.15
                 watching_anchor |= (pAnchorWatchPoint2->m_IconName.StartsWith(_T("anchor")));          // pjotrc 2010.02.15

            wxDateTime now = wxDateTime::Now();
            wxTimeSpan uptime = now.Subtract(g_start_time);

            if(!watching_anchor && (g_bCruising) && (gSog < 0.5) && (uptime.IsLongerThan(wxTimeSpan(0,30,0,0))))     // pjotrc 2010.02.15
            {
                  //    First, delete any single anchorage waypoint closer than 0.25 NM from this point
                  //    This will prevent clutter and database congestion....

                  wxRoutePointListNode *node = pWayPointMan->m_pWayPointList->GetFirst();
                  while(node)
                  {
                        RoutePoint *pr = node->GetData();
                        if(pr->m_MarkName.StartsWith(_T("Anchorage")))
                        {
                              double a = gLat - pr->m_lat;
                              double b = gLon - pr->m_lon;
                              double l = sqrt((a*a) + (b*b));

                  // caveat: this is accurate only on the Equator
                              if((l * 60. * 1852.) < (.25 *1852.))
                              {
                                    pConfig->DeleteWayPoint ( pr );
                                    pSelect->DeleteSelectablePoint ( pr, SELTYPE_ROUTEPOINT );
                                    delete pr;
                                   break;
                              }
                        }

                        node = node->GetNext();
                  }

                  wxString name = now.Format();
                  name.Prepend(_T("Anchorage created "));
                  RoutePoint *pWP = new RoutePoint(gLat, gLon, wxString(_T("anchorage")), name, NULL);
                  pWP->m_bShowName = false;

                  pConfig->AddNewWayPoint(pWP, -1);       // use auto next num
            }
      }

      FrameTimer1.Stop();

      g_bframemax = IsMaximized();

      //    Record the current state of tracking
      g_bTrackCarryOver = g_bTrackActive;

      TrackOff();

      if(pCurrentStack)
            g_restore_stackindex = pCurrentStack->CurrentStackEntry;

      pConfig->UpdateSettings();

      delete g_printData;
      delete g_pageSetupData;

      if(g_pAboutDlg)
            g_pAboutDlg->Destroy();

//      Explicitely Close some children, especially the ones with event handlers
//      or that call GUI methods

    cc1->Destroy();
    cc1 = NULL;

    if(g_pnmea)
    {
          g_pnmea->Close();
          g_pnmea = NULL;                        // This will be a signal to TCP/IP socket event handler
                                            // that any remaining events in queue are to be ignored
    }

#ifdef USE_WIFI_CLIENT
    if(pWIFI)
    {
        pWIFI->Close();
        pWIFI = NULL;
    }
#endif

    if(g_pAIS)
    {
        delete g_pAIS;        //g_pAIS->Close();
        g_pAIS = NULL;
    }

    if(NULL != console)
    {
          console->Destroy();
          console = NULL;
    }

    SetStatusBar(NULL);
    stats->Destroy();
    stats = NULL;

    //      This stuff causes crash SEGV on MSW, and should not really be necessary anyway,
    //      since the toolbar is a child of the frame....
//    m_toolBar->Hide();
//    SetToolBar(NULL);
//    m_toolBar->Destroy(); //DestroyMyToolbar();

//    pthumbwin->Destroy();
    delete pthumbwin;
    pthumbwin = NULL;

    if(pAPilot)
    {
      pAPilot->Close();
      pAPilot->Destroy();
      pAPilot = NULL;
    }

    // Delete the toolbar bitmaps

    string_to_pbitmap_hash::iterator it;

    for( it = tool_bitmap_hash_day.begin(); it != tool_bitmap_hash_day.end(); ++it )
    {
        wxBitmap *pbm = it->second;
        delete pbm;
    }

    for( it = tool_bitmap_hash_dusk.begin(); it != tool_bitmap_hash_dusk.end(); ++it )
    {
        wxBitmap *pbm = it->second;
        delete pbm;
    }

    for( it = tool_bitmap_hash_night.begin(); it != tool_bitmap_hash_night.end(); ++it )
    {
        wxBitmap *pbm = it->second;
        delete pbm;
    }

    DeleteToolbarBitmaps();               // delete the originals, too


    this->Destroy();

}


void MyFrame::OnSize(wxSizeEvent& event)
{
      DoSetSize();
}

void MyFrame::DoSetSize(void)
{
        int x,y;
        GetClientSize(&x, &y);

//      Resize the children

        if(m_pStatusBar)
        {
              //  Maybe resize the font
              wxRect stat_box;
              m_pStatusBar->GetFieldRect(0, stat_box);
              int font_size = stat_box.width / 28;                // 30 for linux
              wxFont *pstat_font = wxTheFontList->FindOrCreateFont(font_size,
                                    wxFONTFAMILY_DEFAULT,
                                    wxFONTSTYLE_NORMAL,
                                    wxFONTWEIGHT_BOLD);
              m_pStatusBar->SetFont(*pstat_font);
        }

        int stat_height = 0;
        if(stats)
        {
//              int yt = stats->GetFontHeight();
//              stat_height = (yt + 6) * stats->GetRows();
              stat_height = 22 * stats->GetRows();
        }


        int cccw = x;
        int ccch = y;

        if(cc1)
        {
                cccw = x * 10/10;               // constrain to mod 4
                int wr = cccw/4;
                cccw = wr*4;
                cccw += 2;                              // account for simple border

                if(stats->IsShown())
                      ccch = y - stat_height;

                int cur_width, cur_height;
                cc1->GetSize(&cur_width, &cur_height);
                if((cur_width != cccw) || (cur_height != ccch))
                    cc1->SetSize(0,0,cccw, ccch);

        }

        if(console)
              PositionConsole();


        if(stats)
        {
                stats->Size_X = cccw;
                stats->Size_Y = stat_height;
                stats->Pos_X = 0;
                stats->Pos_Y = ccch;
                stats->SetSize(stats->Pos_X,stats->Pos_Y,stats->Size_X, stats->Size_Y);
                if(stats->IsShown())
                {
                   stats->FormatStat();
                   stats->Refresh(true);
                }
        }

        if(pthumbwin)
        {
              pthumbwin->SetMaxSize(cc1->GetSize());
        }


//  Rebuild the Toolbar
    if(m_toolBar)
    {
          ReSizeToolbar();
    }

//  Update the stored window size
    GetSize(&x, &y);
    g_nframewin_x = x;
    g_nframewin_y = y;

}

void MyFrame::PositionConsole(void)
{
      //    Reposition console based on its size and chartcanvas size
      int ccx, ccy, consx, consy;
      cc1->GetSize(&ccx, &ccy);
      console->GetSize(&consx, &consy);
      console->SetSize(ccx - consx, 0, -1, -1);
}


void MyFrame::UpdateAllFonts()
{
      if(console)
      {
            console->UpdateFonts();

            //    Reposition console
            PositionConsole();
            cc1->Refresh();
      }

      if(g_pais_query_dialog_active)
      {
            g_pais_query_dialog_active->Destroy();
            g_pais_query_dialog_active = NULL;
      }
}

void MyFrame::OnToolLeftClick(wxCommandEvent& event)
{
  switch(event.GetId())
  {
    case ID_STKUP:
            DoStackUp();
            DoChartUpdate();
            break;


    case ID_STKDN:
            DoStackDown();
            DoChartUpdate();
            break;

    case ID_ZOOMIN:
    {
            cc1->ZoomCanvasIn();
            DoChartUpdate();
            break;
    }

    case ID_ZOOMOUT:
    {
            cc1->ZoomCanvasOut();
            DoChartUpdate();
            break;
    }

    case ID_ROUTE:
        {
            nRoute_State = 1;
            cc1->SetMyCursor(cc1->pCursorPencil);
            break;
        }

    case ID_FOLLOW:
        {
            TogglebFollow();
            break;
        }

#ifdef USE_S57
    case ID_TEXT:
        {
            ps52plib->SetShowS57Text(!ps52plib->GetShowS57Text());
            m_toolBar->ToggleTool(ID_TEXT, ps52plib->GetShowS57Text());
            cc1->ReloadVP();
            break;
        }
#endif

    case ID_AIS:
        {
            g_bShowAIS = !g_bShowAIS;
            m_toolBar->ToggleTool(ID_AIS, g_bShowAIS );
            cc1->ReloadVP();
            break;
        }

    case ID_SETTINGS:
        {

            bool bnewtoolbar = !(DoOptionsDialog() == 0);

//              Apply various system settings
            ApplyGlobalSettings(true, bnewtoolbar);                 // flying update

//  The chart display options may have changed, especially on S57 ENC,
//  So, flush the cache and redraw

            cc1->ReloadVP();
            break;
        }


    case ID_CURRENT:
        {
            if(!ptcmgr)                                                     // First time, init the manager
                ptcmgr = new TCMgr(*pTC_Dir, *pHome_Locn);

            if(ptcmgr->IsReady())
            {
                  cc1->SetbShowCurrent(!cc1->GetbShowCurrent());
                  m_toolBar->ToggleTool(ID_CURRENT, cc1->GetbShowCurrent());
                  cc1->ReloadVP();
            }
            else
            {
                wxLogMessage(_T("Chart1::Event...TCMgr Not Available"));
                cc1->SetbShowCurrent(false);
                m_toolBar->ToggleTool(ID_CURRENT, false);
            }

            if(cc1->GetbShowCurrent())
            {
                  FrameTCTimer.Start(TIMER_TC_VALUE_SECONDS * 1000, wxTIMER_CONTINUOUS);
                  cc1->SetbTCUpdate(true);                        // force immediate update
            }
            else
                  FrameTCTimer.Stop();

            cc1->Refresh(false);

            break;

        }

    case ID_TIDE:
        {
                if(!ptcmgr)                                                     // First time, init the manager
                      ptcmgr = new TCMgr(*pTC_Dir, *pHome_Locn);

                if(ptcmgr->IsReady())
                {
                      cc1->SetbShowTide(!cc1->GetbShowTide());
                      m_toolBar->ToggleTool(ID_TIDE, cc1->GetbShowTide());
                      cc1->ReloadVP();
                }
                else
                {
                    wxLogMessage(_("Chart1::Event...TCMgr Not Available"));
                    cc1->SetbShowTide(false);
                    m_toolBar->ToggleTool(ID_TIDE, false);
                }

                if(cc1->GetbShowTide())
                {
                      FrameTCTimer.Start(TIMER_TC_VALUE_SECONDS * 1000, wxTIMER_CONTINUOUS);
                      cc1->SetbTCUpdate(true);                        // force immediate update
                }
                else
                      FrameTCTimer.Stop();

                cc1->Refresh(false);

                break;

        }

    case ID_HELP:
      {
            if(!g_pAboutDlg)
                  g_pAboutDlg = new about(this, &g_SData_Locn);

            g_pAboutDlg->Show();

            break;
      }

    case ID_PRINT:
      {
        DoPrint();
        break;
      }

     case ID_COLSCHEME:
        {
              ToggleColorScheme();
              break;
        }

    case ID_TBEXIT:
       {
         Close();
         break;
       }

    case ID_GPXIMPORT:       // toh, 2009.02.14
       {
             DoImportGPX();
             break;
       }
    case ID_GPXEXPORT:        // toh, 2009.02.14
       {
             DoExportGPX();
             break;
       }

    case ID_TRACK:
    {
          if(!g_bTrackActive)
                TrackOn();
          else
                TrackOff();
          break;
    }
    case ID_GRIB:
    {
          if(NULL == g_pGribDialog)
               cc1->ShowGribDialog();
          else
                g_pGribDialog->Close();

          break;
    }

    case ID_TBSTATBOX:
    {
          ToggleCourseUp();
          break;
    }

    case ID_MOB:
    {
          RoutePoint *pWP = new RoutePoint ( gLat, gLon, wxString ( _T ( "mob" ) ), wxString ( _( "MAN OVERBOARD" ) ), NULL );
          pSelect->AddSelectableRoutePoint ( gLat, gLon, pWP );
          pConfig->AddNewWayPoint ( pWP, -1 );    // use auto next num
         break;
    }


  }         // switch
}

void MyFrame::ToggleColorScheme()
{
      ColorScheme s = GetColorScheme();
      int is = (int)s;
      is++;
      s = (ColorScheme)is;
      if(s == N_COLOR_SCHEMES)
            s = GLOBAL_COLOR_SCHEME_RGB;

      SetAndApplyColorScheme(s);
}

void MyFrame::TrackOn(void)
{
      g_bTrackActive = true;
      g_pActiveTrack = new Track();

      g_pActiveTrack->SetTrackTimer(g_TrackIntervalSeconds);
      g_pActiveTrack->SetTrackDeltaDistance(g_TrackDeltaDistance);
      g_pActiveTrack->SetTPTime(g_bTrackTime);
      g_pActiveTrack->SetTPDist(g_bTrackDistance);

      pRouteList->Append ( g_pActiveTrack );
      g_pActiveTrack->Start();

      m_toolBar->ToggleTool(ID_TRACK, g_bTrackActive);

}

void MyFrame::TrackOff(void)
 {
      if(g_pActiveTrack)
      {
            g_pActiveTrack->Stop();

            if ( g_pActiveTrack->GetnPoints() < 2 )
                  g_pRouteMan->DeleteRoute ( g_pActiveTrack );
      }

      g_pActiveTrack = NULL;

      g_bTrackActive = false;

      if(m_toolBar)
            m_toolBar->ToggleTool(ID_TRACK, g_bTrackActive);
}






void MyFrame::ToggleCourseUp(void)
{
      g_bCourseUp = !g_bCourseUp;
      DoCOGSet();
      m_rose_angle = -1000.;               // force new render
      UpdateToolbarStatusBox();
      DoChartUpdate();
      cc1->ReloadVP();
}



void MyFrame::ToggleENCText(void)
{
#ifdef USE_S57
      if(ps52plib)
      {
            ps52plib->SetShowS57Text(!ps52plib->GetShowS57Text());
            m_toolBar->ToggleTool(ID_TEXT, ps52plib->GetShowS57Text());
            cc1->ReloadVP();
      }

#endif
}

void MyFrame::TogglebFollow(void)
{
      if(!cc1->m_bFollow)
            SetbFollow();
      else
            ClearbFollow();
}

void MyFrame::SetbFollow(void)
{
      cc1->m_bFollow = true;
      m_toolBar->ToggleTool(ID_FOLLOW, cc1->m_bFollow);

      DoChartUpdate();
      cc1->ReloadVP();

}

void MyFrame::ClearbFollow(void)
{
      //    Center the screen on the GPS position, for lack of a better place
      vLat = gLat;
      vLon = gLon;
      cc1->m_bFollow = false;
      m_toolBar->ToggleTool(ID_FOLLOW, cc1->m_bFollow);
      DoChartUpdate();
      cc1->ReloadVP();

}

void MyFrame::ToggleChartOutlines(void)
{
      if(!g_bShowOutlines)
            g_bShowOutlines = true;
      else
            g_bShowOutlines = false;

      cc1->Refresh(false);

}

void MyFrame::SetToolbarItemState ( int tool_id, bool state )
{
      if(m_toolBar)
            m_toolBar->ToggleTool(tool_id, state);
}

void MyFrame::ApplyGlobalSettings(bool bFlyingUpdate, bool bnewtoolbar)
{
 //             ShowDebugWindow as a wxStatusBar
        m_StatusBarFieldCount = 5;

#ifdef __WXMSW__
        UseNativeStatusBar(false);              // better for MSW, undocumented in frame.cpp
#endif

        if(pConfig->m_bShowDebugWindows)
        {
                if(!m_pStatusBar)
                {
                    m_pStatusBar = CreateStatusBar(m_StatusBarFieldCount, 0);       // No wxST_SIZEGRIP needed
                    ApplyGlobalColorSchemetoStatusBar();
                }

        }
        else
        {
                if(m_pStatusBar)
                {
                    m_pStatusBar->Destroy();
                    m_pStatusBar = NULL;
                    SetStatusBar(NULL);

//    Since the chart canvas will need to be resized, we need
//    to refresh the entire frame.
                    Refresh();
                }
        }

        SendSizeEvent();

//      if(bnewtoolbar)
          UpdateToolbar(global_color_scheme);



}

int MyFrame::DoOptionsDialog()
{
      bool b_refresh_after_options = false;

      ::wxBeginBusyCursor();
      options *pSetDlg = new options(this, -1, _("ToolBox") );
      ::wxEndBusyCursor();

//    Set initial Chart Dir
      pSetDlg->SetInitChartDir(*pInit_Chart_Dir);

//      Pass two working pointers for Chart Dir Dialog
      pSetDlg->SetCurrentDirListPtr(&g_ChartDirArray);
      ArrayOfCDI *pWorkDirArray = new ArrayOfCDI;
      pSetDlg->SetWorkDirListPtr(pWorkDirArray);

//  Grab a copy of the current NMEA source and AP Port and AIS Port
      wxString previous_NMEA_source(*pNMEADataSource);
      wxString previous_NMEA_APPort(*pNMEA_AP_Port);
      wxString previous_AIS_Port(*pAIS_Port);

//      Pass a ptr to MyConfig, for updates
      pSetDlg->SetConfigPtr(pConfig);

      pSetDlg->SetInitialSettings();

      bDBUpdateInProgress = true;

      bool bPrevPrintIcon = g_bShowPrintIcon;

      bool bPrevGPXIcon = g_bShowGPXIcons;
      bool bPrevTrackIcon = g_bShowTrackIcon;
      bool bPrevGRIBIcon = g_bShowGRIBIcon;
      bool bPrevQuilt = cc1->GetQuiltMode();

      wxString prev_locale = g_locale;

//    Pause all of the async classes
#ifdef USE_WIFI_CLIENT
      if(pWIFI)
          pWIFI->Pause();
#endif

      if(g_pAIS)
          g_pAIS->Pause();
      if(g_pnmea)
            g_pnmea->Pause();


// And here goes the (modal) dialog
      int rr = pSetDlg->ShowModal();

      if(rr)
      {
            if(rr & VISIT_CHARTS)
            {
                  FrameTimer1.Stop();                  // stop other asynchronous activity

                  cc1->FlushBackgroundRender();

                  cc1->InvalidateQuilt();
                  cc1->SetQuiltRefChart(-1);

                  Current_Ch = NULL;

                  delete pCurrentStack;
                  pCurrentStack = NULL;


                  g_ChartDirArray = *pWorkDirArray;

                  bool b_force = false;
                  if(rr & FORCE_UPDATE)
                        b_force = true;

                  //    Disable Toolbar
//                  EnableToolbar(false);

                  ::wxBeginBusyCursor();

                  pSetDlg->Hide();

                  wxProgressDialog *pprog = new wxProgressDialog (  _("OpenCPN Chart Update"),
                              _T(""), 100, this,
                              wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME);


                  //    Make sure the dialog is big enough to be readable
                  pprog->Hide();
                  wxSize sz = pprog->GetSize();
                  wxSize csz = GetClientSize();
                  sz.x = csz.x * 7 / 10;
                  pprog->SetSize(sz);
                  pprog->Centre();
                  pprog->Update(1, _T(""));
                  pprog->Show();


                  ChartData->Update(g_ChartDirArray, b_force, pprog );           // with progress dialog
                  ChartData->SaveBinary(pChartListFileName);

                  delete pprog;

                  ::wxEndBusyCursor();

                  //    Re Enable Toolbar
//                  EnableToolbar(true);

                  pConfig->UpdateChartDirs(g_ChartDirArray);

                  ChartData->PurgeCache();

                  //    Build a new ChartStack
                  pCurrentStack = new ChartStack;
                  ChartData->BuildChartStack(pCurrentStack, vLat, vLon);

                  //    Select reference chart from the stack, as though clicked by user
                  //    Make it the smallest scale chart on the stack
                  pCurrentStack->CurrentStackEntry = pCurrentStack->nEntry-1;
                  int selected_index = pCurrentStack->GetCurrentEntrydbIndex();
                  cc1->SetQuiltRefChart(selected_index);


                  //    Choose the correct single chart, or set the quilt mode as appropriate
                  SetupQuiltMode();

                  cc1->ReloadVP();

                  UpdateControlBar();

                  UpdateToolbarStatusBox(true);

                  cc1->SetCursor(wxCURSOR_ARROW);
                  FrameTimer1.Start(TIMER_GFRAME_1,wxTIMER_CONTINUOUS);
            }

            if(*pNMEADataSource != previous_NMEA_source)
            {
                  if(g_pnmea)
                        g_pnmea->Close();
                  delete g_pnmea;

                  //    If the selected port is the same as AIS port, override the name to force the
                  //    NMEA class to expect muxed data from AIS decoder
                  if(pNMEADataSource->IsSameAs(*pAIS_Port))
                        g_pnmea = new NMEAWindow(ID_NMEA_WINDOW, gFrame, _T("AIS Port (Shared)"), g_NMEABaudRate, &m_mutexNMEAEvent );
                  else
                        g_pnmea = new NMEAWindow(ID_NMEA_WINDOW, gFrame, *pNMEADataSource, g_NMEABaudRate, &m_mutexNMEAEvent );

                  SetbFollow();
           }


            if(*pNMEA_AP_Port != previous_NMEA_APPort)
            {
                if(pAPilot)
                    pAPilot->Close();
                delete pAPilot;
                pAPilot = new AutoPilotWindow(gFrame, *pNMEA_AP_Port );
            }

            if(*pAIS_Port != previous_AIS_Port)
            {
                delete g_pAIS;
                g_pAIS = new AIS_Decoder(ID_AIS_WINDOW, gFrame, *pAIS_Port, &m_mutexNMEAEvent );
                b_refresh_after_options = true;
            }

#ifdef USE_S57
            if(rr & S52_CHANGED)
            {

/*
                //      All this is handled by the chart object now based on the plib state hash

                // Traverse the database of open charts.
                // Finding S57 chart, execute UpdateLUPs to link objects
                // to possibly new symbology style.
                unsigned int nCache = ChartData->GetChartCache()->GetCount();
                for(unsigned int i=0 ; i<nCache ; i++)
                {
                    CacheEntry *pce = (CacheEntry *)(ChartData->GetChartCache()->Item(i));
                    ChartBase *Ch = (ChartBase *)pce->pChart;
                    if(Ch->m_ChartFamily == CHART_FAMILY_VECTOR)
                    {
                        s57chart *S57_Ch = dynamic_cast<s57chart *>(Ch);
                        S57_Ch->UpdateLUPs(S57_Ch);
                        S57_Ch->ForceEdgePriorityEvaluate();             // force re-evaluation of line segment (edge) priorities
                        b_refresh_after_options = true;
                    }
                }
*/
                b_refresh_after_options = true;

            }
#endif

            if(rr & LOCALE_CHANGED)
            {
                  if(prev_locale != g_locale)
                  {
                        ::wxMessageBox(_("Please restart OpenCPN to activate new language selection."), _("OpenCPN Info"), wxOK | wxICON_INFORMATION);

                  }
            }


            pConfig->UpdateSettings();

            if(g_pActiveTrack)
            {
                  g_pActiveTrack->SetTrackTimer(g_TrackIntervalSeconds);
                  g_pActiveTrack->SetTrackDeltaDistance(g_TrackDeltaDistance);
                  g_pActiveTrack->SetTPTime(g_bTrackTime);
                  g_pActiveTrack->SetTPDist(g_bTrackDistance);
            }

            if(bPrevQuilt != cc1->GetQuiltMode())
                  SetupQuiltMode();

            if(g_bCourseUp)
            {
            //    Stuff the COGAvg table in case COGUp is selected

                  for(int i = 0 ; i < g_COGAvgSec ; i++)
                        COGTable[i] = gCog;

                  g_COGAvg = gCog;

            //    Short circuit the COG timer to force immediate refresh of canvas in case COGUp is selected
                  FrameCOGTimer.Stop();
                  FrameCOGTimer.Start(100, wxTIMER_CONTINUOUS);

                  b_refresh_after_options = true;
            }



      }

//    Restart the async classes
#ifdef USE_WIFI_CLIENT
      if(pWIFI)
          pWIFI->UnPause();
#endif

      if(g_pAIS)
          g_pAIS->UnPause();
      if(g_pnmea)
            g_pnmea->UnPause();

      delete pWorkDirArray;

      bDBUpdateInProgress = false;

      delete pSetDlg;

      if((bPrevPrintIcon       != g_bShowPrintIcon)     ||
         (bPrevGPXIcon         != g_bShowGPXIcons)      ||
         (bPrevTrackIcon       != g_bShowTrackIcon)     ||
         (bPrevGRIBIcon        != g_bShowGRIBIcon)      ||
          b_refresh_after_options
        )
            return true;    // indicate a refresh is necessary;

      return false;
}


void MyFrame::ToggleQuiltMode(void)
{
      if(cc1)
            cc1->SetQuiltMode(!cc1->GetQuiltMode());
      SetupQuiltMode();
}

void MyFrame::SetQuiltMode(bool bquilt)
{
      if(cc1)
            cc1->SetQuiltMode(bquilt);
}

bool MyFrame::GetQuiltMode(void)
{
      if(cc1)
            return cc1->GetQuiltMode();
      else
            return false;
}

void MyFrame::SetupQuiltMode(void)
{

//      stats->Show(!cc1->GetQuiltMode());                   // no status/piano widow on quilting....
      stats->Show(true);
      DoSetSize();

      if(cc1->GetQuiltMode())                               // going to quilt mode
      {
            stats->pPiano->SetNoshowIndexArray(g_quilt_noshow_index_array);
            stats->pPiano->SetVizIcon(_img_viz);
            stats->pPiano->SetInVizIcon(_img_redX);
            stats->pPiano->SetTMercIcon(_img_tmercprj);
            stats->pPiano->SetSkewIcon(_img_skewprj);

            stats->pPiano->SetRoundedRectangles(true);

            //    Select the proper Ref chart
            int target_new_dbindex = -1;
            if(pCurrentStack)
            {
                  target_new_dbindex = pCurrentStack->GetCurrentEntrydbIndex();

                  if(-1 != target_new_dbindex)
                  {
                  //    Check to see if the target new chart is Merc
                        int proj = ChartData->GetDBChartProj(target_new_dbindex);
                        int type = ChartData->GetDBChartType(target_new_dbindex);

                        if(PROJECTION_MERCATOR != proj)
                        {
                        // If it is not Merc, cannot use it for quilting
                        // walk the stack up looking for a satisfactory chart
                              int stack_index = pCurrentStack->CurrentStackEntry;

                              while((stack_index < pCurrentStack->nEntry-1) && (stack_index >= 0))
                              {
                                    int proj_tent = ChartData->GetDBChartProj( pCurrentStack->GetDBIndex(stack_index));
                                    int type_tent = ChartData->GetDBChartType( pCurrentStack->GetDBIndex(stack_index));

                                    if((PROJECTION_MERCATOR ==proj_tent) && (type_tent == type))
                                    {
                                          target_new_dbindex = pCurrentStack->GetDBIndex(stack_index);
                                          break;
                                    }
                                    stack_index++;
                              }
                        }
                  }
            }

            cc1->SetQuiltRefChart(target_new_dbindex);

            Current_Ch = NULL;                  // Bye....
      }
      else                                                  // going to SC Mode
      {
            ArrayOfInts empty_array;
            stats->pPiano->SetActiveKeyArray(empty_array);
            stats->pPiano->SetNoshowIndexArray(empty_array);
            stats->pPiano->SetSubliteIndexArray(empty_array);
            stats->pPiano->SetVizIcon(NULL);
            stats->pPiano->SetInVizIcon(NULL);
            stats->pPiano->SetTMercIcon(_img_tmercprj);
            stats->pPiano->SetSkewIcon(_img_skewprj);

            stats->pPiano->SetRoundedRectangles(false);

      }



            //    When shifting from quilt to single chart mode, select the "best" single chart to show
      if(!cc1->GetQuiltMode())
      {
            if(ChartData && ChartData->IsValid())
            {
                  double tLat, tLon;
                  if(cc1->m_bFollow == true)
                  {
                        tLat = gLat;
                        tLon = gLon;
                  }
                  else
                  {
                        tLat = vLat;
                        tLon = vLon;
                  }

                        // Build a temporary chart stack based on tLat, tLon
                  ChartStack TempStack;
                  ChartData->BuildChartStack(&TempStack, tLat, tLon);


                  //    Iterate over the quilt charts actaully shown, looking for the largest scale chart that will be in the new chartstack....
                  //    This will (almost?) always be the reference chart....

                  ChartBase *Candidate_Chart = NULL;
                  int cur_max_scale = (int)1e8;

                  ChartBase *pChart = cc1->GetFirstQuiltChart();
                  while(pChart)
                  {
                         //  Is this pChart in new stack?
                        int tEntry = ChartData->GetStackEntry(&TempStack, pChart->GetFullPath());
                        if(tEntry != -1)
                        {
                              if(pChart->GetNativeScale() < cur_max_scale)
                              {
                                    Candidate_Chart = pChart;
                                    cur_max_scale = pChart->GetNativeScale();
                              }
                        }
                        pChart = cc1->GetNextQuiltChart();
                  }

                  Current_Ch = Candidate_Chart;

                  //    If the quilt is empty, there is no "best" chart.
                  //    So, open the smallest scale chart in the current stack
                  if(NULL == Current_Ch)
                  {
                       Current_Ch = ChartData->OpenStackChartConditional(&TempStack,
                                    TempStack.nEntry - 1, true, CHART_TYPE_DONTCARE, CHART_FAMILY_DONTCARE);
                  }



                  //  Invalidate all the charts in the quilt, as any cached data may be region based and not have fullscreen coverage
                  cc1->InvalidateAllQuiltPatchs();

                  if(Current_Ch)
                  {
                        int dbi = ChartData->FinddbIndex(Current_Ch->GetFullPath());
                        ArrayOfInts one_array;
                        one_array.Add(dbi);
                        stats->pPiano->SetActiveKeyArray(one_array);
                  }

            }
            //    Invalidate the current stack so that it will be rebuilt on next tick
            if(pCurrentStack)
                  pCurrentStack->b_valid = false;
       }

//       UpdateToolbar(global_color_scheme);            // Reformat the toolbar
//       UpdateToolbarStatusWindow(Current_Ch, false);
}




void MyFrame::ClearRouteTool()
{
      m_toolBar->ToggleTool(ID_ROUTE, false);
}


void MyFrame::DoStackDown(void)
{
      int current_stack_index = pCurrentStack->CurrentStackEntry;

      if(0 == current_stack_index)
            return;

      if(!cc1->GetQuiltMode())
             SelectChartFromStack(current_stack_index - 1);
      else
      {
            int new_dbIndex = pCurrentStack->GetDBIndex(current_stack_index - 1);

            if(!cc1->IsChartQuiltable(new_dbIndex))
            {
                  ToggleQuiltMode();
                  SelectChartFromStack(current_stack_index - 1);
            }
            else
                   SelectQuiltRefChart(current_stack_index - 1);

      }

      cc1->SetQuiltChartHiLiteIndex(-1);

      cc1->ReloadVP();
}


void MyFrame::DoStackUp(void)
{
      int current_stack_index = pCurrentStack->CurrentStackEntry;

      if(current_stack_index >= pCurrentStack->nEntry-1)
            return;

      if(!cc1->GetQuiltMode())
      {
            SelectChartFromStack(current_stack_index + 1);
      }
      else
      {
            int new_dbIndex = pCurrentStack->GetDBIndex(current_stack_index + 1);

            if(!cc1->IsChartQuiltable(new_dbIndex))
            {
                  ToggleQuiltMode();
                  SelectChartFromStack(current_stack_index + 1);
            }
            else
                  SelectQuiltRefChart(current_stack_index + 1);
      }

      cc1->SetQuiltChartHiLiteIndex(-1);

      cc1->ReloadVP();
}


//    Manage the application memory footprint on a periodic schedule
void MyFrame::OnMemFootTimer(wxTimerEvent& event)
{
      MemFootTimer.Stop();

      int memsize = GetApplicationMemoryUse();

      g_MemFootMB = 100;
      printf("Memsize: %d  \n", memsize);
      // The application memory usage has exceeded the target, so try to manage it down....
      if(memsize > (g_MemFootMB * 1000))
      {
            if(ChartData && cc1)
            {
                  //    Get a local copy of the cache info
                  wxArrayPtrVoid *pCache = ChartData->GetChartCache();
                  unsigned int nCache = pCache->GetCount();
                  CacheEntry *pcea = new CacheEntry[nCache];

                  for(unsigned int i=0 ; i<nCache ; i++)
                  {
                        CacheEntry *pce = (CacheEntry *)(pCache->Item(i));
                        pcea[i] = *pce;//ChartBase *Ch = (ChartBase *)pce->pChart;
                  }

                  if(nCache > 1)
                  {
                  //    Bubble Sort the local cache entry array
                        bool b_cont = true;
                        while(b_cont)
                        {
                              b_cont = false;
                              for(unsigned int i=0 ; i<nCache-1 ; i++)
                              {
                                    if(pcea[i].RecentTime > pcea[i+1].RecentTime)
                                    {
                                          CacheEntry tmp = pcea[i];
                                          pcea[i] = pcea[i+1];
                                          pcea[i+1] = tmp;
                                          b_cont = true;
                                          break;
                                    }
                              }
                        }

                        //    Free up some chart cache entries until the memory footprint target is realized

                        unsigned int idelete = 0;                 // starting at top. which is oldest
                        unsigned int idelete_max = pCache->GetCount();

                        //    How many can be deleted?
                        unsigned int minimum_cache = 1;
                        if(cc1->GetQuiltMode())
                              minimum_cache = cc1->GetQuiltChartCount();

                        while((memsize > (g_MemFootMB * 1000)) && (pCache->GetCount() > minimum_cache) && (idelete < idelete_max))
                        {
                              int memsizeb = memsize;
                              cc1->FlushBackgroundRender();

                              ChartData->DeleteCacheChart((ChartBase *)pcea[idelete].pChart);
                              idelete++;
                              memsize = GetApplicationMemoryUse();
                              printf("delete, before: %d  after: %d\n", memsizeb, memsize);
                        }

                  }



                  delete[] pcea;
            }

      }

//      MemFootTimer.Start(wxMax(g_MemFootSec * 1000, 60 * 1000), wxTIMER_CONTINUOUS);
      MemFootTimer.Start(9000, wxTIMER_CONTINUOUS);
}


void MyFrame::OnFrameTimer1(wxTimerEvent& event)
{

    g_tick++;


//      Listen for quitflag to be set, requesting application close
      if(quitflag)
      {
          wxLogMessage(_T("Got quitflag from SIGUSR1"));
          FrameTimer1.Stop();
          Close();
          return;
      }

      if(bDBUpdateInProgress)
            return;

      FrameTimer1.Stop();

//  Update and check watchdog timer for GPS data source
      gGPS_Watchdog--;
      if(gGPS_Watchdog <= 0)
      {
          bGPSValid = false;
          if(g_nNMEADebug && (gGPS_Watchdog == 0))
                wxLogMessage(_T("   ***GPS Watchdog timeout..."));
      }

//  Update and check watchdog timer for Heading data source
      gHDx_Watchdog--;
      if(gHDx_Watchdog <= 0)
      {
            g_bHDxValid = false;
            if(g_nNMEADebug && (gHDx_Watchdog == 0))
                  wxLogMessage(_T("   ***HDx Watchdog timeout..."));
      }

//  Update and check watchdog timer for GSV (Satellite data)
      gSAT_Watchdog--;
      if(gSAT_Watchdog <= 0)
      {
            g_bSatValid = false;
            if(g_nNMEADebug && (gSAT_Watchdog == 0))
                  wxLogMessage(_T("   ***SAT Watchdog timeout..."));
      }


      //   Check for anchorwatch alarms                                 // pjotrc 2010.02.15
      if(pAnchorWatchPoint1)
      {
            double dist;
            double brg;
            DistanceBearingMercator(pAnchorWatchPoint1->m_lat, pAnchorWatchPoint1->m_lon, gLat, gLon, &brg, &dist);
            double d = AnchorPointMaxDist;
            (pAnchorWatchPoint1->m_MarkName).ToDouble(&d);
            d = AnchorDistFix(d, AnchorPointMinDist, AnchorPointMaxDist);
            bool toofar = false; bool tooclose = false;
            if (d >= 0.0) toofar = (dist*1852. > d);
            if (d < 0.0) tooclose = (dist*1852 < -d);

            if (tooclose || toofar) AnchorAlertOn1 = true;
            else AnchorAlertOn1 = false;
      }
      else
            AnchorAlertOn1 = false;

      if(pAnchorWatchPoint2)
      {
             double dist;
             double brg;
             DistanceBearingMercator(pAnchorWatchPoint2->m_lat, pAnchorWatchPoint2->m_lon, gLat, gLon, &brg, &dist);

             double d = AnchorPointMaxDist;
             (pAnchorWatchPoint2->m_MarkName).ToDouble(&d);
             d = AnchorDistFix(d, AnchorPointMinDist, AnchorPointMaxDist);
             bool toofar = false; bool tooclose = false;
             if (d >= 0) toofar = (dist*1852. > d);
             if (d < 0) tooclose = (dist*1852 < -d);

             if (tooclose || toofar) AnchorAlertOn2 = true;
             else AnchorAlertOn2 = false;
        }
        else AnchorAlertOn2 = false;

//  Send current nav status data to log file on every half hour   // pjotrc 2010.02.09

      wxDateTime lognow = wxDateTime::Now();   // pjotrc 2010.02.09
      int hour = lognow.GetHour();
      lognow.MakeGMT();
      int minute = lognow.GetMinute();
      wxTimeSpan logspan = lognow.Subtract(g_loglast_time);
      if ((logspan.IsLongerThan(wxTimeSpan(0,30,0,0))) || ( minute == 0 ) || ( minute == 30 ) )
      {
         if (logspan.IsLongerThan(wxTimeSpan(0,1,0,0)))
         {
            wxString day = lognow.FormatISODate();
            wxString utc = lognow.FormatISOTime();
            wxString navmsg = _T("LOGBOOK:  ");
            navmsg += day;
            navmsg += _T(" ");
            navmsg += utc;
            navmsg += _T(" UTC ");

            if (bGPSValid)
            {
                  wxString data;
                  data.Printf(_T(" GPS Lat %10.5f Lon %10.5f COG %10.5f SOG %6.2f"), gLat, gLon, gCog, gSog);
                  navmsg += data;
            }
            else
            {
                  wxString data;
                  data.Printf(_T(" DR Lat %10.5f Lon %10.5f"), gLat, gLon);
                  navmsg += data;
            }
            wxLogMessage(navmsg);
            g_loglast_time = lognow;

            int bells = (hour % 4)*2;     // 2 bells each hour
            if (minute!=0) bells++;       // + 1 bell on 30 minutes
            if (!bells)    bells = 8;     // 0 is 8 bells

            if (g_bPlayShipsBells && (( minute == 0 ) || ( minute == 30 )))
            {
                  if(!bells_sound[bells-1].IsOk())            // load the bells sound
                  {
                        wxString soundfile = _T("sounds");
                        appendOSDirSlash(&soundfile);
                        soundfile += wxString(bells_sound_file_name[bells-1], wxConvUTF8);
                        soundfile.Prepend(g_SData_Locn);
                        bells_sound[bells-1].Create(soundfile);
                        wxLogMessage(_T("Using bells sound file: ") + soundfile);

                  }

                  if(bells_sound[bells-1].IsOk())
                        bells_sound[bells-1].Play();
            }
         }
      }

//      Update the Toolbar Status windows and lower status bar the first time watchdog times out
      if((gGPS_Watchdog == 0) || (gSAT_Watchdog == 0))
      {
//          UpdateToolbarStatusWindow(Current_Ch, false);
          wxString sogcog(_T("SOG: ----- kts  COG: ----- Deg"));
          SetStatusText ( sogcog, STAT_FIELD_SOGCOG );

          gCog = 0.0;                                 // say speed is zero to kill ownship predictor
      }

      if(cc1)
      {
            double cursor_lat, cursor_lon;
            cc1->GetCursorLatLon(&cursor_lat, &cursor_lon);

            wxString s1 = _("Cursor: ");
            s1 += toSDMM(1, cursor_lat);
            s1 += _T(" ");
            s1 += toSDMM(2, cursor_lon);
            SetStatusText ( s1, STAT_FIELD_CURSOR_LL );

            double brg, dist;
            DistanceBearingMercator(cursor_lat, cursor_lon, gLat, gLon, &brg, &dist);
            wxString s;
            s.Printf(_("From Ownship: %03d Deg  %6.2f NMi"), (int)brg, dist);
            SetStatusText ( s, STAT_FIELD_CURSOR_BRGRNG );
      }


//    Poll for updates to Toolbar status window
//    in order to handle especially the CM93Composite chart
/*
      if(Current_Ch)
      {
            wxString name = Current_Ch->GetName();
            wxString pub_date = Current_Ch->GetPubDate();

            if((!name.IsSameAs(m_last_reported_chart_name)) || (!pub_date.IsSameAs(m_last_reported_chart_pubdate)))
            {
                  UpdateToolbarStatusWindow(Current_Ch, false);
                  m_last_reported_chart_name = name;
                  m_last_reported_chart_pubdate = pub_date;
            }
      }
*/

//      Update the chart database and displayed chart
      bool bnew_chart = false;

//    If in COG UP mode, the chart update is handled by COG Update timer
      if(!g_bCourseUp)
            bnew_chart = DoChartUpdate();

//      Update the active route, if any
      if(g_pRouteMan->UpdateProgress())
      {
          nBlinkerTick++;
          //    This RefreshRect will cause any active routepoint to blink
          if(g_pRouteMan->GetpActiveRoute())
            cc1->RefreshRect(g_blink_rect, false);
      }

//  Possibly save the current configuration
      if(0 == (g_tick % (g_nautosave_interval_seconds)))
             pConfig->UpdateSettings();


//  Force own-ship drawing parameters
      cc1->SetOwnShipState(SHIP_NORMAL);

      if(cc1->GetQuiltMode())
      {
            double erf = cc1->GetQuiltMaxErrorFactor();
            if(erf > 0.02)
                  cc1->SetOwnShipState(SHIP_LOWACCURACY);
      }
      else
      {
        if(Current_Ch)
        {
           if(Current_Ch->Chart_Error_Factor > 0.02)
               cc1->SetOwnShipState(SHIP_LOWACCURACY);
        }
      }

      if(!bGPSValid)
            cc1->SetOwnShipState(SHIP_INVALID);



        FrameTimer1.Start(TIMER_GFRAME_1, wxTIMER_CONTINUOUS);

//  Invalidate the ChartCanvas window appropriately
        cc1->UpdateShips();

        cc1->UpdateAIS();

        cc1->UpdateAlerts();                // pjotrc 2010.02.22

        if(g_pais_query_dialog_active && g_pais_query_dialog_active->IsShown())
              g_pais_query_dialog_active->UpdateText();

        // Refresh AIS target list every 5 seconds to avoid blinking
        if (g_pAISTargetList && (0 == (g_tick % (5))))
              g_pAISTargetList->UpdateAISTargetList();


        //  Pick up any change Toolbar status displays
        UpdateToolbarStatusBox();
        UpdateToolbarDynamics();


        if(bnew_chart)
            cc1->Refresh(false);

        if(NULL != console)
            if(console->IsShown())
                 console->Refresh(false);

}

//    Cause refresh of active Tide/Current data, if displayed
void MyFrame::OnFrameTCTimer(wxTimerEvent& event)
{
      if(cc1)
      {
            cc1->SetbTCUpdate(true);
            cc1->Refresh(false);
      }
}


//    Keep and update the Viewport rotation angle according to average COG for COGUP mode
void MyFrame::OnFrameCOGTimer(wxTimerEvent& event)
{
//      return;
      FrameCOGTimer.Stop();

      DoCOGSet();

      //    Restart the timer
      FrameCOGTimer.Start(g_COGAvgSec * 1000, wxTIMER_CONTINUOUS);
}


void MyFrame::DoCOGSet(void)
{
      double old_VPRotate = g_VPRotate;

      if(g_bCourseUp)
      {
            g_VPRotate = -g_COGAvg * PI / 180.;

            if(Current_Ch)
                  g_VPRotate -= Current_Ch->GetChartSkew() * PI / 180.;
      }
      else
            g_VPRotate = 0.;

      if(cc1)
            cc1->SetVPRotation(g_VPRotate);

      if(g_bCourseUp)
      {
            bool bnew_chart = DoChartUpdate();

            if((bnew_chart) || (old_VPRotate != g_VPRotate))
                  if(cc1)
                        cc1->ReloadVP();
      }
}



void RenderShadowText(wxDC *pdc, wxFont *pFont, wxString& str, int x, int y)
{
#ifdef DrawText
#undef DrawText
#define FIXIT
#endif

        wxFont oldfont = pdc->GetFont(); // save current font

        pdc->SetFont(*pFont);
        pdc->SetTextForeground(GetGlobalColor(_T("CHGRF")));
        pdc->SetBackgroundMode(wxTRANSPARENT);

        pdc->DrawText(str, x, y+1);
        pdc->DrawText(str, x, y-1);
        pdc->DrawText(str, x+1, y);
        pdc->DrawText(str, x-1, y);

        pdc->SetTextForeground(GetGlobalColor(_T("CHBLK")));

        pdc->DrawText(str, x, y);

        pdc->SetFont(oldfont);                  // restore last font

#ifdef FIXIT
#undef FIXIT
#define DrawText DrawTextA
#endif


}

void MyFrame::UpdateToolbarStatusBox(bool b_update_toolbar)
{

       bool b_need_refresh = false;

//      delete  m_pStatBmp;
//      m_pStatBmp = new wxBitmap((_img_compass->GetWidth() + _img_gpsRed->GetWidth()) + 8, _img_compass->GetHeight() + 8);
      m_StatBmp.Create((_img_compass->GetWidth() + _img_gpsRed->GetWidth()) + 8, _img_compass->GetHeight() + 8);

      if(m_StatBmp.IsOk())
      {
            //    Clear the bitmap
            wxMemoryDC mdc;
            mdc.SelectObject(m_StatBmp);

            mdc.SetBackground(wxBrush(GetGlobalColor(_T("GREY2")), wxSOLID));
            mdc.Clear();

            //    Draw the frame
            mdc.SetPen(wxPen(GetGlobalColor(_T("UITX1")), 1));
            mdc.SetBrush(wxBrush(GetGlobalColor(_T("UITX1")), wxTRANSPARENT));

            int offset_x = 2;
            int offset_y = 4;

            mdc.DrawRoundedRectangle(0, 0, m_StatBmp.GetWidth(),  m_StatBmp.GetHeight(), 3);

            //    Build Compass Rose, rotated...
            wxBitmap BMPRose;

            if(g_bCourseUp)
                  BMPRose = *(*m_phash)[wxString(_T("CompassRose"))];
            else
                  BMPRose = *(*m_phash)[wxString(_T("CompassRoseBlue"))];

            if(fabs(cc1->GetVPSkew()) > .01)
                  BMPRose = *(*m_phash)[wxString(_T("CompassRose"))];

            double rose_angle = -999.0;
            if((fabs(cc1->GetVPRotation()) > .01) || (fabs(cc1->GetVPSkew()) > .01))
            {
                  wxPoint rot_ctr(BMPRose.GetWidth()/2, BMPRose.GetHeight()/2);
                  wxImage rose_img = BMPRose.ConvertToImage();

                  rose_angle = -(cc1->GetVPSkew() + cc1->GetVPRotation());
                  wxPoint after_rotate;
                  wxImage rot_image = rose_img.Rotate(rose_angle, rot_ctr, true, &after_rotate);
                  wxBitmap rot_bm(rot_image);

                  mdc.DrawBitmap(rot_bm, offset_x + after_rotate.x, offset_y + after_rotate.y, true);
                  b_need_refresh = true;

            }
            else
            {
                  rose_angle = 0.;
                  mdc.DrawBitmap(BMPRose, offset_x, offset_y, true);
            }
            if(fabs(m_rose_angle - rose_angle) > .001)
                  b_need_refresh = true;

            m_rose_angle = rose_angle;


            //    GPS Status
            wxBitmap *pBMP;
            wxString bmp_hash_index;
            if(bGPSValid)
            {
                  if(g_bSatValid)
                  {
                        bmp_hash_index = _T("gps3Bar");
                        if(g_SatsInView <= 8)
                              bmp_hash_index = _T("gps2Bar");
                        if(g_SatsInView <= 4)
                              bmp_hash_index = _T("gps1Bar");

                  }
                  else
                        bmp_hash_index = _T("gpsGrn");
            }
            else
                  bmp_hash_index = _T("gpsRed");

            if((m_last_gps_bmp_hash_index != bmp_hash_index) || b_need_refresh)
            {
                  pBMP = (*m_phash)[bmp_hash_index];
                  mdc.DrawBitmap(*pBMP, BMPRose.GetWidth() + offset_x + 2, offset_y, true);
                  b_need_refresh = true;
            }

            mdc.SelectObject(wxNullBitmap);


            // Update the tool in the toolbar
            if(m_toolBar && b_update_toolbar && b_need_refresh)
            {
///
                  //   Delete the current status tool, if present
                  int ct_pos = m_toolBar->GetToolPos(ID_TBSTATBOX);
                  if(ct_pos != -1)
                  {
      //      Delete the EXIT tool
                        if(g_bbigred)
                              m_toolBar->DeleteTool(ID_TBEXIT);

      //      Delete the current status tool
      #ifndef __WXOSX__
                        m_toolBar->RemoveTool(ID_TBSTATBOX);
      #endif
      // end rms
                  }

      //      Create the new control tool
      //#ifdef __WXMSW__
                  delete m_pStatBoxToolStaticBmp;        // Gets deleted by DeleteTool? No....
      //#endif


                  m_pStatBoxToolStaticBmp = new wxStaticBitmap(m_toolBar, ID_TBSTATBOX, m_StatBmp,
                              wxPoint(2000,10), wxSize(m_StatBmp.GetWidth(),DUMMY_HEIGHT),wxSIMPLE_BORDER, _T("staticBitmap"));
      //      Insert the new control
                  delete m_pStatBoxTool;        // needs to be deleted since it was only removed above
                  m_pStatBoxTool = m_toolBar->InsertControl(ct_pos, m_pStatBoxToolStaticBmp);

            //      Re-insert the EXIT tool
                  if(g_bbigred)
                        m_toolBar->AddTool( ID_TBEXIT, _T(""), *(*m_phash)[wxString(_T("exitt"))], _("Exit OpenCPN"), wxITEM_NORMAL);

      //     Realize the toolbar to reflect changes
                  m_toolBar->Realize();

                  m_last_gps_bmp_hash_index = bmp_hash_index;

            }


/*
            m_pStatBoxTool->SetBitmap(*m_pStatBmp);
            if(m_toolBar && b_update_toolbar && b_need_refresh)
            {
                  m_toolBar->Refresh();
                  m_last_gps_bmp_hash_index = bmp_hash_index;
            }
*/
      }
}




#include "wx/encconv.h"

void MyFrame::UpdateToolbarStatusWindow(ChartBase *pchart, bool bUpdate)
{
      return;

    if(m_tool_dummy_size_x <= 0)
        return;

    int iSysDescent = 0 ;
#ifdef __WXMSW__
      int font2_size = 10;
      int font3_size = 14;
#elif defined(__WXOSX__)
      int font2_size = 10;
      int font3_size = 12;
#else
      int font2_size = 12;
      int font3_size = 14;
#endif


//      Create a wxBitmap for the window
      wxBitmap tool_bm_dummy(m_tool_dummy_size_x, m_tool_dummy_size_y);
      tool_dummy_size_x_last = m_tool_dummy_size_x;       // record for change tracking during resize

      int size_x = m_tool_dummy_size_x;
      int size_y = m_tool_dummy_size_y;

//      Draw the graphics
      wxMemoryDC dc;
      dc.SelectObject(tool_bm_dummy);

// First, clear background
// Using a color depending on the state of GPS
      wxBrush brush(GetGlobalColor(_T("GREEN3")), wxSOLID);   // quiet green
      if(!bGPSValid)
         brush = wxBrush(GetGlobalColor(_T("RED1")), wxSOLID);  // soft red


// Adjust the color depending on the state of Chart_Error_Factor

      if(pchart)
      {
            if(pchart->Chart_Error_Factor > .02)                                       // X percent error
            {
                  brush = wxBrush(GetGlobalColor(_T("CHYLW")), wxSOLID);   // loud yellow

                  if(!bGPSValid)
                        brush = wxBrush(GetGlobalColor(_T("UINFO")), wxSOLID);  // orange
            }
      }

      dc.SetBackground(brush);
      dc.Clear();

      if(pchart)
      {

      // Show Pub date
      // Get a Font
            wxFontFamily family = wxFONTFAMILY_ROMAN;
            wxString font_name(_T("Roman"));
      #ifdef __WXMSW__
            family = (enum wxFontFamily)wxDEFAULT;
            font_name = _T("Arial");
      #endif

            wxFont *pSWFont1;
            pSWFont1 = wxTheFontList->FindOrCreateFont(font3_size, family, wxNORMAL, wxNORMAL,
            FALSE, font_name, wxFONTENCODING_SYSTEM );

            dc.SetFont(*pSWFont1);

      //      Get and show the Chart Publish Date
            wxString pub_date = pchart->GetPubDate();

            int w, h, descent;
            GetTextExtent(pub_date, &w, &h, &descent, NULL, pSWFont1);

            int date_locn_x = size_x - w - 2;
            int date_locn_y = size_y - h;

            //    GTK appears to return height as total height, including descenders.
            //    Other platforms return height as height above baseline...
      #ifndef __WXGTK__
            iSysDescent = descent ;
      #endif

      //      RenderShadowText(&dc, pSWFont1, pub_date, date_locn_x, date_locn_y - iSysDescent);

            dc.SetFont(*pSWFont1);
            dc.DrawText(pub_date, date_locn_x, date_locn_y - iSysDescent);


      //    Show File Name
            wxFont *pSWFont2;

            pSWFont2 = wxTheFontList->FindOrCreateFont(font2_size, wxDEFAULT ,wxNORMAL, wxNORMAL,
                  FALSE, wxString(_T("")), wxFONTENCODING_SYSTEM );
            dc.SetFont(*pSWFont2);

      //   Get and show the Chart Description String
            wxString desc = pchart->GetDescription();

            int height_font_2;
            GetTextExtent(desc, &w, &height_font_2, NULL, NULL, pSWFont2);

            dc.DrawText(desc, 4, 0);


      //   Get and show the Chart Nice Name
            wxFont *pSWFont3;
            pSWFont3 = wxTheFontList->FindOrCreateFont(font3_size, wxDEFAULT,wxNORMAL, wxBOLD,
                  FALSE, wxString(_T("Arial")), wxFONTENCODING_SYSTEM );
            dc.SetFont(*pSWFont3);


            /*************************************************************************************/
            /*        All of this name encoding has been rendered redundant with conversion      */
            /*        of opencpn to Unicode internally.  The chart name is reported as a         */
            /*        properly converted wxString, printable directly with the appropriate font. */
            /*************************************************************************************/

            /*
            //    The Chart Nice Name may be encoded with 8-bit ascii encoding set,
            //    especially some French chart names in NDI data sets.
            //    Use an Encoding Converter to ensure that the name string can be
            //    rendered in the selected font.  A simple brute force way is to
            //    convert the string to ISO8859_1, which will be renderable by
            //    all (??) installed system fonts.  There may be a more elegant way,
            //    for example, try to figure out the string encoding BEFORE specifying
            //    the font, and then FindOrCreate an appropriate font...  Todo Later.

            wxString name_possibly_intl;
            pchart->GetName(name_possibly_intl);

            wxEncodingConverter ec;
            ec.Init( wxFONTENCODING_ISO8859_2,  wxFONTENCODING_ISO8859_1, wxCONVERT_SUBSTITUTE);
            wxString name = ec.Convert(name_possibly_intl);
            */

            wxString name = pchart->GetName();

            int x_offset = 4;

            if(name.Len())
            {
            //    Possibly adjust the font?
                  GetTextExtent(name, &w, &h, NULL, NULL, pSWFont3);
                  if(w + x_offset > date_locn_x)
                  {
                        dc.SetFont(*pSWFont2);
                        GetTextExtent(name, &w, &h, NULL, NULL, pSWFont2);
                        if(w + x_offset > date_locn_x)                   // still too long, so shorten it
                        {
                              wxString nameshort;
                              int el = name.Len();
                              el -= 3;
                              while(el > 0)              //ell
                              {
                                    nameshort = name.Mid(0, el);
                                    nameshort.Append(_T("..."));
                                    GetTextExtent(nameshort, &w, &h, NULL, NULL, pSWFont2);
                                    if(w + x_offset < date_locn_x)
                                          break;
                                    el -= 1;
                              }
                              dc.DrawText(nameshort, x_offset, size_y - h - iSysDescent);    // properly placed
                        }
                        else
                              dc.DrawText(name, x_offset, size_y - h - iSysDescent);
                  }
                  else
                  dc.DrawText(name, x_offset, size_y - h - iSysDescent);
            }

      }

      dc.SelectObject(wxNullBitmap);

//   Delete the current status tool, if present
      int ct_pos = m_toolBar->GetToolPos(ID_TBSTAT);
      if(ct_pos != -1)
      {
//      Delete the EXIT tool
            if(g_bbigred)
                  m_toolBar->DeleteTool(ID_TBEXIT);

//      Delete the current status tool
//  begin rms
#ifndef __WXOSX__
            m_toolBar->DeleteTool(ID_TBSTAT);
#endif
// end rms
       }

//      Create the new control tool
//#ifdef __WXMSW__
       delete m_ptool_ct_dummyStaticBmp;        // Gets deleted by DeleteTool? No....
//#endif
       m_ptool_ct_dummyStaticBmp = new wxStaticBitmap(m_toolBar, ID_TBSTAT, tool_bm_dummy,
                                            wxPoint(2000,10), wxSize(size_x, size_y),wxSIMPLE_BORDER, _T("staticBitmap"));
//      Insert the new control
       m_toolBar->InsertControl(m_statTool_pos, m_ptool_ct_dummyStaticBmp);

       //      Re-insert the EXIT tool
       if(g_bbigred)
             m_toolBar->AddTool( ID_TBEXIT, _T(""), *(*m_phash)[wxString(_T("exitt"))], _("Exit OpenCPN"), wxITEM_NORMAL);

 //     Realize the toolbar to reflect changes
       m_toolBar->Realize();

//      if(bUpdate)
//      SendSizeEvent();
}

int MyFrame::GetnChartStack(void)
{
       return pCurrentStack->nEntry;
}

void MyFrame::HandlePianoClick(int selected_index, int selected_dbIndex)
{
      if( !pCurrentStack )
            return;

      if(!cc1->GetQuiltMode())
            SelectChartFromStack(selected_index);
      else
      {
            if(cc1->IsChartQuiltable(selected_dbIndex))
                  SelectQuiltRefChart(selected_index);
            else
            {
                  ToggleQuiltMode();
                  SelectChartFromStack(selected_index);
            }
      }

      cc1->SetQuiltChartHiLiteIndex(-1);

      DoChartUpdate();
      cc1->ReloadVP();                  // Pick up the new selections
}

void MyFrame::HandlePianoRClick(int x, int y, int selected_index, int selected_dbIndex)
{
      if( !pCurrentStack )
            return;

      PianoPopupMenu ( x, y, selected_index, selected_dbIndex );
      UpdateControlBar();

      cc1->SetQuiltChartHiLiteIndex(-1);

}





void MyFrame::HandlePianoRollover(int selected_index, int selected_dbIndex)
{
      if(!cc1)
            return;
      if( !pCurrentStack )
            return;



      if(!cc1->GetQuiltMode())
      {
            SetChartThumbnail(selected_index);
            int sx, sy;
            stats->GetPosition(&sx, &sy);
            wxPoint key_location = stats->pPiano->GetKeyOrigin(selected_index);
            cc1->ShowChartInfoWindow(key_location.x, sy + key_location.y-250, selected_dbIndex);
      }
      else
      {
            int sx, sy;
            stats->GetPosition(&sx, &sy);
            wxPoint key_location = stats->pPiano->GetKeyOrigin(selected_index);

            if(pCurrentStack->nEntry > 1)
            {
                  cc1->ShowChartInfoWindow(key_location.x, sy + key_location.y-250, selected_dbIndex);
                  cc1->SetQuiltChartHiLiteIndex(selected_dbIndex);

                  cc1->ReloadVP();
            }
            else if(pCurrentStack->nEntry == 1)
            {
                  const ChartTableEntry &cte = ChartData->GetChartTableEntry(pCurrentStack->GetDBIndex(0));
                  if(CHART_TYPE_CM93COMP != cte.GetChartType())
                  {
                        cc1->ShowChartInfoWindow(key_location.x, sy + key_location.y-250, selected_dbIndex);

                        cc1->ReloadVP();
                  }
            }
      }
}

void MyFrame::HandlePianoRolloverIcon(int selected_index, int selected_dbIndex)
{
      if(!cc1)
            return;

      if(!cc1->GetQuiltMode())
            SetChartThumbnail(selected_index);
      else
      {
            cc1->SetQuiltChartHiLiteIndex(selected_dbIndex);
      }
}


double MyFrame::GetBestVPScale(ChartBase *pchart)
{
      if(pchart)
      {
            double proposed_scale_onscreen = cc1->GetCanvasScaleFactor() / cc1->GetVPScale();

            if((g_bPreserveScaleOnX) || (CHART_TYPE_CM93COMP == pchart->GetChartType()))
            {
                  double new_scale_ppm = pchart->GetNearestPreferredScalePPM(cc1->GetVPScale());
                  proposed_scale_onscreen = cc1->GetCanvasScaleFactor() / new_scale_ppm;
            }
            else
            {
                        //  This logic will bring the new chart onscreen at roughly twice the true paper scale equivalent.
                  proposed_scale_onscreen = pchart->GetNativeScale() / 2;
                  double equivalent_vp_scale = cc1->GetCanvasScaleFactor() / proposed_scale_onscreen;
                  double new_scale_ppm = pchart->GetNearestPreferredScalePPM(equivalent_vp_scale);
                  proposed_scale_onscreen = cc1->GetCanvasScaleFactor() / new_scale_ppm;
            }


            proposed_scale_onscreen = wxMin(proposed_scale_onscreen, pchart->GetNormalScaleMax(cc1->GetCanvasScaleFactor(), cc1->GetCanvasWidth()));
            proposed_scale_onscreen = wxMax(proposed_scale_onscreen, pchart->GetNormalScaleMin(cc1->GetCanvasScaleFactor(), g_b_overzoom_x));

            return cc1->GetCanvasScaleFactor() / proposed_scale_onscreen;
      }
      else
            return 1.0;
}

void MyFrame::SelectQuiltRefChart(int selected_index)
{
      ArrayOfInts piano_chart_index_array = cc1->GetQuiltExtendedStackdbIndexArray();

      int current_db_index = piano_chart_index_array.Item(selected_index);

      if(pCurrentStack)
            pCurrentStack->SetCurrentEntryFromdbIndex(current_db_index);

      cc1->SetQuiltRefChart(current_db_index);

      ChartBase *pc = ChartData->OpenChartFromDB(current_db_index, FULL_INIT);
      double best_scale = GetBestVPScale(pc);
      cc1->SetVPScale ( best_scale );

//      UpdateToolbarStatusWindow(pc, false);

//      UpdateControlBar();           // Probably redundant
}


void MyFrame::SelectChartFromStack(int index, bool bDir, ChartTypeEnum New_Type, ChartFamilyEnum New_Family)
{
        if(index < pCurrentStack->nEntry)
        {
//      Open the new chart
            ChartBase *pTentative_Chart;
            pTentative_Chart = ChartData->OpenStackChartConditional(pCurrentStack, index, bDir, New_Type, New_Family);

            if(pTentative_Chart)
            {
                  if(Current_Ch)
                        Current_Ch->Deactivate();

                  Current_Ch = pTentative_Chart;
                  Current_Ch->Activate();

                  pCurrentStack->CurrentStackEntry = ChartData->GetStackEntry(pCurrentStack, Current_Ch->GetFullPath());
            }
            else
                SetChartThumbnail(index);       // need to reset thumbnail on failed chart open



//      Setup the view
            float zLat, zLon;
            if(cc1->m_bFollow)
                { zLat = gLat; zLon = gLon;}
            else
                { zLat = vLat; zLon = vLon;}


//  If the cache is invalid, as in new chart load, force a fast LODEF render
            int new_sample_mode;
            if(Current_Ch->IsCacheValid())
                new_sample_mode = CURRENT_RENDER;
            else
                new_sample_mode = FORCE_SUBSAMPLE;

            double best_scale = GetBestVPScale(Current_Ch);
            cc1->SetViewPoint(zLat, zLon, best_scale/*cc1->GetCanvasScaleFactor() / proposed_scale_onscreen*/,
                              Current_Ch->GetChartSkew() * PI / 180., cc1->GetVPRotation(), new_sample_mode);

            UpdateToolbarStatusBox();           // Pick up the rotation

        }

        //          Refresh the Piano Bar
        if(stats)
        {
              ArrayOfInts piano_active_chart_index_array;
              piano_active_chart_index_array.Add(pCurrentStack->GetCurrentEntrydbIndex());
              stats->pPiano->SetActiveKeyArray(piano_active_chart_index_array);

              stats->Refresh(true);
        }
}

void MyFrame::SetChartThumbnail(int index)
{
        if(bDBUpdateInProgress)
            return;

        if(NULL == pCurrentStack)
            return;

        if(NULL == pthumbwin)
            return;

        if(NULL == cc1)
            return;

        if(index == -1)
        {
            wxRect thumb_rect_in_parent = pthumbwin->GetRect();;
            pthumbwin->pThumbChart = NULL;
            pthumbwin->Show(false);
            cc1->RefreshRect(thumb_rect_in_parent, FALSE);
        }

        else if(index < pCurrentStack->nEntry)
        {
                if((ChartData->GetCSChartType(pCurrentStack, index) == CHART_TYPE_KAP) ||
                                  (ChartData->GetCSChartType(pCurrentStack, index) == CHART_TYPE_GEO))
                {
                        ChartBase *new_pThumbChart = ChartData->OpenChartFromStack(pCurrentStack, index);
                        if(new_pThumbChart)         // chart opened ok
                        {

                                ThumbData *pTD = new_pThumbChart->GetThumbData(200, 200, gLat, gLon);
                                if(pTD)
                                {
                                        pthumbwin->pThumbChart = new_pThumbChart;

                                        pthumbwin->Resize();
                                        pthumbwin->Show(true);
                                        pthumbwin->Refresh(FALSE);
                                }



                                else
                                {
                                        wxString fp = ChartData->GetFullPath(pCurrentStack, index);
                                        wxString msg(_("Chart file corrupt.. disabling this chart \n"));
                                        msg.Append(fp);
                                        wxLogMessage(msg);
                                        wxMessageDialog dlg(gFrame, msg, _("OpenCPN Message"), wxOK);
                                        dlg.ShowModal();
                                        ChartData->DisableChart(fp);

                                        wxLogMessage(_T("chart1.cpp:SetChartThumbnail...Could not create thumbnail"));
                                        pthumbwin->pThumbChart = NULL;
                                        pthumbwin->Show(false);
                                        cc1->Refresh(FALSE);
                                }

                        }
                        else                            // some problem opening chart
                        {
                                wxString fp = ChartData->GetFullPath(pCurrentStack, index);
                                fp.Prepend(_T("chart1.cpp:SetChartThumbnail...Could not open chart "));
                                wxLogMessage(fp);
                                pthumbwin->pThumbChart = NULL;
                                pthumbwin->Show(false);
                                cc1->Refresh(FALSE);
                        }

                }
                else
                {
                        ChartBase *new_pThumbChart = ChartData->OpenChartFromStack(pCurrentStack, index,
                                        THUMB_ONLY);

                        pthumbwin->pThumbChart = new_pThumbChart;

                        if(new_pThumbChart)
                        {
                            ThumbData *pTD = new_pThumbChart->GetThumbData(200, 200, gLat, gLon);
                            if(pTD)
                            {
                                pthumbwin->Resize();
                                pthumbwin->Show(true);
                                pthumbwin->Refresh(true);
                            }
                            else
                                pthumbwin->Show(false);

                            cc1->Refresh(FALSE);
                        }
                }
        }

}


void MyFrame::UpdateControlBar(void)
{
      if(!cc1)
            return;

      if(!stats)
            return;

      if(!pCurrentStack)
            return;

      ArrayOfInts piano_chart_index_array;

      if (cc1->GetQuiltMode())
      {
            piano_chart_index_array = cc1->GetQuiltExtendedStackdbIndexArray();
            stats->pPiano->SetKeyArray(piano_chart_index_array);

            ArrayOfInts piano_active_chart_index_array = cc1->GetQuiltCandidatedbIndexArray();
            stats->pPiano->SetActiveKeyArray(piano_active_chart_index_array);

            ArrayOfInts piano_eclipsed_chart_index_array = cc1->GetQuiltEclipsedStackdbIndexArray();
            stats->pPiano->SetSubliteIndexArray(piano_eclipsed_chart_index_array);

            stats->pPiano->SetNoshowIndexArray(g_quilt_noshow_index_array);

      }
      else
      {
            piano_chart_index_array = ChartData->GetCSArray(pCurrentStack);
            stats->pPiano->SetKeyArray(piano_chart_index_array);

            ArrayOfInts piano_active_chart_index_array;
            piano_active_chart_index_array.Add(pCurrentStack->GetCurrentEntrydbIndex());
            stats->pPiano->SetActiveKeyArray(piano_active_chart_index_array);

      }

      //    Set up the TMerc and Skew arrays
      ArrayOfInts piano_skew_chart_index_array;
      ArrayOfInts piano_tmerc_chart_index_array;
      ArrayOfInts piano_poly_chart_index_array;

      for(unsigned int ino=0 ; ino < piano_chart_index_array.GetCount() ; ino++)
      {
            const ChartTableEntry &ctei = ChartData->GetChartTableEntry(piano_chart_index_array.Item(ino));
            double skew_norm = ctei.GetChartSkew();
            if(skew_norm > 180.)
                  skew_norm -= 360.;

            if(ctei.GetChartProjectionType() == PROJECTION_TRANSVERSE_MERCATOR)
                  piano_tmerc_chart_index_array.Add(piano_chart_index_array.Item(ino));

            //    Polyconic skewed charts should show as skewed
            else if(ctei.GetChartProjectionType() == PROJECTION_POLYCONIC)
            {
                  if(fabs(skew_norm) > 1.)
                        piano_skew_chart_index_array.Add(piano_chart_index_array.Item(ino));
                  else
                        piano_poly_chart_index_array.Add(piano_chart_index_array.Item(ino));
            }
            else if(fabs(skew_norm) > 1.)
                  piano_skew_chart_index_array.Add(piano_chart_index_array.Item(ino));

      }
      stats->pPiano->SetSkewIndexArray(piano_skew_chart_index_array);
      stats->pPiano->SetTmercIndexArray(piano_tmerc_chart_index_array);
      stats->pPiano->SetPolyIndexArray(piano_poly_chart_index_array);


      stats->FormatStat();
      stats->Refresh(true);

}

//----------------------------------------------------------------------------------
//      DoChartUpdate
//      Create a chartstack based on current lat/lon.
//      Update Current_Ch, using either current chart, if still in stack, or
//      smallest scale new chart in stack if not.
//      Return true if need a full canvas redraw
//----------------------------------------------------------------------------------
bool MyFrame::DoChartUpdate(void)
{

      double tLat, tLon;           // Chart Stack location
      double vpLat, vpLon;         // ViewPort location

      if(!cc1)
            return false;
      if(bDBUpdateInProgress)
            return false;


      //      If in auto-follow mode, use the current glat,glon to build chart stack.
      //      Otherwise, use vLat, vLon gotten from click on chart canvas, or other means

      if(cc1->m_bFollow == true)
      {
            tLat = gLat;
            tLon = gLon;
            vpLat = gLat;
            vpLon = gLon;

            // on lookahead mode, adjust the vp center point
            if(cc1  && g_bLookAhead)
            {
                  double angle = g_COGAvg;

                  double pixel_deltay = fabs(cos(angle * PI / 180.)) * cc1->GetCanvasHeight() / 4;
                  double pixel_deltax = fabs(sin(angle * PI / 180.)) * cc1->GetCanvasWidth() / 4;
                  double pixel_delta = sqrt((pixel_deltay * pixel_deltay) + ( pixel_deltax * pixel_deltax));

                  double meters_to_shift = cos(gLat * PI / 180.) * pixel_delta / cc1->GetVPScale();

                  double dir_to_shift = angle;

                  ll_gc_ll(gLat, gLon, dir_to_shift, meters_to_shift / 1852., &vpLat, &vpLon);
            }

      }
      else
      {
            tLat = vLat;
            tLon = vLon;
            vpLat = vLat;
            vpLon = vLon;

      }

      if (cc1->GetQuiltMode())
      {
            int current_db_index = -1;
            if(pCurrentStack)
                  current_db_index = pCurrentStack->GetCurrentEntrydbIndex();         // capture the currently selected Ref chart dbIndex
            else
                  pCurrentStack = new ChartStack;

            ChartData->BuildChartStack(pCurrentStack, tLat, tLon);
            pCurrentStack->SetCurrentEntryFromdbIndex(current_db_index);

            if(bFirstAuto)
            {
                  double proposed_scale_onscreen = cc1->GetCanvasScaleFactor() / cc1->GetVPScale(); // as set from config load

                  int initial_db_index = -1;
                  if(pCurrentStack->nEntry)
                  {
                        if((g_restore_stackindex < pCurrentStack->nEntry) && (g_restore_stackindex >= 0))
                              initial_db_index = pCurrentStack->GetDBIndex(g_restore_stackindex);
                        else
                              initial_db_index = pCurrentStack->GetDBIndex(pCurrentStack->nEntry-1) ;

                        int initial_type = ChartData->GetDBChartType(initial_db_index);

                  //    Check to see if the target new chart is quiltable

                        if(!cc1->IsChartQuiltable(initial_db_index))
                        {
                  // If it is not quiltable, then walk the stack up looking for a satisfactory chart
                  // i.e. one that is quiltable and of the same type
                              int stack_index = g_restore_stackindex;

                              while((stack_index < pCurrentStack->nEntry-1) && (stack_index >= 0))
                              {
                                    int test_db_index = pCurrentStack->GetDBIndex(stack_index);
                                    if(cc1->IsChartQuiltable(test_db_index) &&
                                       ( initial_type == ChartData->GetDBChartType(initial_db_index)))
                                    {
                                          initial_db_index = test_db_index;
                                          break;
                                    }
                                    stack_index++;
                              }
                        }

                        cc1->SetQuiltRefChart(initial_db_index);
                        pCurrentStack->SetCurrentEntryFromdbIndex(initial_db_index);

                        // Try to bound the inital Viewport scale to something reasonable for the selected reference chart
                        if(ChartData)
                        {
                              ChartBase *pc = ChartData->OpenChartFromDB(initial_db_index, FULL_INIT);
                              if(pc)
                              {
                                    proposed_scale_onscreen = wxMin(proposed_scale_onscreen, pc->GetNormalScaleMax(cc1->GetCanvasScaleFactor(), cc1->GetCanvasWidth()));
                                    proposed_scale_onscreen = wxMax(proposed_scale_onscreen, pc->GetNormalScaleMin(cc1->GetCanvasScaleFactor(), g_b_overzoom_x));
                              }
                        }

                  }

                  cc1->SetViewPoint(vpLat, vpLon, cc1->GetCanvasScaleFactor() / proposed_scale_onscreen, 0, cc1->GetVPRotation(), CURRENT_RENDER);

                  bFirstAuto = false;                           // Auto open on program start
            }

            if(cc1->SetViewPoint(vpLat, vpLon, cc1->GetVPScale(), 0, cc1->GetVPRotation(), CURRENT_RENDER))
                  return true;
            else
                  return false;

        }

        //  Single Chart Mode from here....
        ChartStack LastStack;
        bool bNewChart = false;
        bool bNewPiano = false;
        ChartBase *pLast_Ch = Current_Ch;
        ChartTypeEnum new_open_type;
        ChartFamilyEnum new_open_family;
        if(pLast_Ch)
        {
              new_open_type = pLast_Ch->m_ChartType;
              new_open_family = pLast_Ch->m_ChartFamily;
        }
        else
        {
              new_open_type = CHART_TYPE_KAP;
              new_open_family = CHART_FAMILY_RASTER;
        }


        bool bOpenSpecified =  bFirstAuto;
        bFirstAuto = false;                           // Auto open specified index once, on program start
        bAutoOpen = true;                             // debugging


        //  Make sure the target stack is valid
        if(NULL == pCurrentStack)
               pCurrentStack = new ChartStack;

        // Build a chart stack based on tLat, tLon
        ChartStack *pWorkStack = new ChartStack;
        int l = ChartData->BuildChartStack(pWorkStack, tLat, tLon);

        if(l == 0)                                    // Bogus Lat, Lon?
        {
                if(NULL == pDummyChart)
                {
                    pDummyChart = new ChartDummy;
                    bNewChart = true;
                }

                if(Current_Ch)
                    if(Current_Ch->m_ChartType != CHART_TYPE_DUMMY)
                        bNewChart = true;

                Current_Ch = pDummyChart;

//    If the current viewpoint is invalid, set the default scale to something reasonable.
                double set_scale = cc1->GetVPScale();
                if(!cc1->VPoint.bValid)
                    set_scale = 1./200000.;

                cc1->SetViewPoint(tLat, tLon, set_scale, 0, cc1->GetVPRotation(), CURRENT_RENDER);

        //      If the chart stack has just changed, there is new status
                if(!ChartData->EqualStacks(pWorkStack, pCurrentStack))
                {
                  bNewPiano = true;
                  bNewChart = true;
                }

        //      Copy the new (by definition empty) stack into the target stack
                ChartData->CopyStack(pCurrentStack, pWorkStack);

                goto update_finish;
        }


        //              Check to see if Chart Stack has changed
        if(!ChartData->EqualStacks(pWorkStack, pCurrentStack))
        {
        //      New chart stack, so...
                bNewPiano = true;

        //      Save a copy of the current stack
                ChartData->CopyStack(&LastStack, pCurrentStack);

        //      Copy the new stack into the target stack
                ChartData->CopyStack(pCurrentStack, pWorkStack);

        //  Is Current Chart in new stack?

                int tEntry = -1;
                if(NULL != Current_Ch)                                  // this handles startup case
                        tEntry = ChartData->GetStackEntry(pCurrentStack, Current_Ch->GetFullPath());

                if(tEntry != -1)                // Current_Ch is in the new stack
                {
                        pCurrentStack->CurrentStackEntry = tEntry;
                        bNewChart = false;
                }

                else                           // Current_Ch is NOT in new stack
                {                                       // So, need to open a new chart
                                                        //      Find the largest scale raster chart that opens OK

                  ChartBase *pProposed = NULL;

                  if(bAutoOpen)
                  {
                        bool search_direction = false;            // default is to search from lowest to highest
                        int start_index = 0;

                        //    A special case:  If panning at high scale, open largest scale chart first
                        if((LastStack.CurrentStackEntry == LastStack.nEntry - 1) || (LastStack.nEntry == 0))
                        {
                              search_direction = true;
                              start_index = pCurrentStack->nEntry - 1;
                        }

                        //    Another special case, open specified index on program start
                        if(bOpenSpecified)
                        {
                              search_direction = false;
                              start_index = g_restore_stackindex;
                              if((start_index < 0) | (start_index >= pCurrentStack->nEntry))
                                    start_index = 0;
                              new_open_type = CHART_TYPE_DONTCARE;
                        }

                        pProposed = ChartData->OpenStackChartConditional(pCurrentStack, start_index, search_direction, new_open_type, new_open_family);

//    Try to open other types/families of chart in some priority
                        if(NULL == pProposed)
                              pProposed = ChartData->OpenStackChartConditional(pCurrentStack, start_index, search_direction,
                                    CHART_TYPE_CM93COMP, CHART_FAMILY_VECTOR);

                        if(NULL == pProposed)
                              pProposed = ChartData->OpenStackChartConditional(pCurrentStack, start_index, search_direction,
                                    CHART_TYPE_CM93COMP, CHART_FAMILY_RASTER);

                        bNewChart = true;

                  }     // bAutoOpen

                  else
                        pProposed = NULL;



//  If no go, then
//  Open a Dummy Chart
                  if(NULL == pProposed)
                  {
                      if(NULL == pDummyChart)
                      {
                          pDummyChart = new ChartDummy;
                          bNewChart = true;
                      }

                      if(pLast_Ch)
                         if(pLast_Ch->m_ChartType != CHART_TYPE_DUMMY)
                               bNewChart = true;

                      pProposed = pDummyChart;
                   }

// Arriving here, pProposed points to an opened chart, or NULL.
                   if(Current_Ch)
                         Current_Ch->Deactivate();
                   Current_Ch = pProposed;

                   if(Current_Ch)
                   {
                         Current_Ch->Activate();
                         pCurrentStack->CurrentStackEntry = ChartData->GetStackEntry(pCurrentStack, Current_Ch->GetFullPath());
                   }
                }   // need new chart


// Arriving here, Current_Ch is opened and OK, or NULL
                if(NULL != Current_Ch)
                {

//      Setup the view using the current scale
                    double set_scale = cc1->GetVPScale();

//    If the current viewpoint is invalid, set the default scale to something reasonable.
                    if(!cc1->VPoint.bValid)
                        set_scale = 1./200000.;
                    else                                    // otherwise, match scale if elected.
                    {
                        double proposed_scale_onscreen;

                        double new_scale_ppm = Current_Ch->GetNearestPreferredScalePPM(cc1->GetVPScale());
                        proposed_scale_onscreen = cc1->GetCanvasScaleFactor() / new_scale_ppm;

                        //  This logic will bring a new chart onscreen at roughly twice the true paper scale equivalent.
                        //  Note that first chart opened on application startup (bOpenSpecified = true) will open at the config saved scale
                        if(bNewChart && !g_bPreserveScaleOnX && !bOpenSpecified)
                        {
                              proposed_scale_onscreen = Current_Ch->GetNativeScale() / 2;
                              double equivalent_vp_scale = cc1->GetCanvasScaleFactor() / proposed_scale_onscreen;
                              double new_scale_ppm = Current_Ch->GetNearestPreferredScalePPM(equivalent_vp_scale);
                              proposed_scale_onscreen = cc1->GetCanvasScaleFactor() / new_scale_ppm;
                        }

                        proposed_scale_onscreen = wxMin(proposed_scale_onscreen, Current_Ch->GetNormalScaleMax(cc1->GetCanvasScaleFactor(), cc1->GetCanvasWidth()));
                        proposed_scale_onscreen = wxMax(proposed_scale_onscreen, Current_Ch->GetNormalScaleMin(cc1->GetCanvasScaleFactor(), g_b_overzoom_x));

                        set_scale = cc1->GetCanvasScaleFactor() / proposed_scale_onscreen;
                    }

//  If the chart cache is invalid, as in new chart load, force a fast sub-sample render
                    int new_sample_mode = CURRENT_RENDER;
                    if(!Current_Ch->IsCacheValid())
                           new_sample_mode = FORCE_SUBSAMPLE;

                    cc1->SetViewPoint(vpLat, vpLon, set_scale, Current_Ch->GetChartSkew() * PI / 180., cc1->GetVPRotation(), new_sample_mode);

                }
        }         // new stack

        else                                                                    // No change in Chart Stack
        {
                if(cc1->m_bFollow)
                      cc1->SetViewPoint(vpLat, vpLon, cc1->GetVPScale(), Current_Ch->GetChartSkew() * PI / 180., cc1->GetVPRotation(),CURRENT_RENDER);
        }



update_finish:
        if(bNewChart)
            UpdateToolbarStatusWindow(Current_Ch, false);

        if(bNewPiano)
        {
              UpdateControlBar();
        }

        //  Update the ownship position on thumbnail chart, if shown
        if(pthumbwin->IsShown())
        {
              if(pthumbwin->pThumbChart)
                  if(pthumbwin->pThumbChart->UpdateThumbData(gLat, gLon))
                        pthumbwin->Refresh(TRUE);
        }

        delete pWorkStack;
        return bNewChart;
}



void MyFrame::MouseEvent(wxMouseEvent& event)
{
        int x,y;
        event.GetPosition(&x, &y);


}

void MyFrame::RemoveChartFromQuilt(int dbIndex)
{
      //    Remove the item from the list (if it appears) to avoid multiple addition
      for(unsigned int i=0 ; i < g_quilt_noshow_index_array.GetCount() ; i++)
      {
            if(g_quilt_noshow_index_array.Item(i) == dbIndex)        // chart is already in the noshow list
            {
                  g_quilt_noshow_index_array.RemoveAt(i);
                  break;
            }
      }

      g_quilt_noshow_index_array.Add(dbIndex);

}

//          Piano window Popup Menu Handlers and friends

static int menu_selected_dbIndex;
static int menu_selected_index;

void MyFrame::PianoPopupMenu ( int x, int y, int selected_index, int selected_dbIndex )
{
      menu_selected_dbIndex = selected_dbIndex;
      menu_selected_index = selected_index;

      wxMenu *pctx_menu = new wxMenu();

      //    Search the no-show array
      bool b_is_in_noshow = false;
      for(unsigned int i=0 ; i < g_quilt_noshow_index_array.GetCount() ; i++)
      {
            if(g_quilt_noshow_index_array.Item(i) == selected_dbIndex)        // chart is in the noshow list
            {
                  b_is_in_noshow = true;
                  break;
            }
      }


      if(b_is_in_noshow)
      {
            pctx_menu->Append(ID_PIANO_ENABLE_QUILT_CHART, _("Add this chart to quilt."));
            Connect( ID_PIANO_ENABLE_QUILT_CHART, wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(MyFrame::OnPianoMenuEnableChart) );
      }
      else
      {
            pctx_menu->Append(ID_PIANO_DISABLE_QUILT_CHART, _("Remove this chart from quilt."));
            Connect( ID_PIANO_DISABLE_QUILT_CHART, wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(MyFrame::OnPianoMenuDisableChart) );
      }


      int sx, sy;
      stats->GetPosition(&sx, &sy);
      wxPoint key_location = stats->pPiano->GetKeyOrigin(selected_index);


//        Invoke the drop-down menu
      PopupMenu ( pctx_menu, sx + key_location.x, sy + key_location.y - 50 );

      cc1->HideChartInfoWindow();
      stats->pPiano->ResetRollover();

      cc1->SetQuiltChartHiLiteIndex(-1);

      cc1->ReloadVP();

      delete pctx_menu;
}

void MyFrame::OnPianoMenuEnableChart(wxCommandEvent& event)
{
      for(unsigned int i=0 ; i < g_quilt_noshow_index_array.GetCount() ; i++)
      {
            if(g_quilt_noshow_index_array.Item(i) == menu_selected_dbIndex)        // chart is in the noshow list
            {
                  g_quilt_noshow_index_array.RemoveAt(i);
                  break;
            }
      }
}

void MyFrame::OnPianoMenuDisableChart(wxCommandEvent& event)
{
/*
       //    Remove the item from the list to avoid multiple addition
      for(unsigned int i=0 ; i < g_quilt_noshow_index_array.GetCount() ; i++)
      {
            if(g_quilt_noshow_index_array.Item(i) == menu_selected_dbIndex)        // chart is in the noshow list
            {
                  g_quilt_noshow_index_array.RemoveAt(i);
                  break;
            }
      }

      g_quilt_noshow_index_array.Add(menu_selected_dbIndex);
*/

      RemoveChartFromQuilt(menu_selected_dbIndex);

//      It could happen that the chart being disabled is the reference chart....
      if(menu_selected_dbIndex == cc1->GetQuiltRefChartdbIndex())
      {
            int type = ChartData->GetDBChartType(menu_selected_dbIndex);

            int i = menu_selected_index + 1;          // select next smaller scale chart
            while(i < pCurrentStack->nEntry - 1)
            {
                  int dbIndex = pCurrentStack->GetDBIndex(i);
                  if(type == ChartData->GetDBChartType(dbIndex))
                  {
                        SelectQuiltRefChart(i);
                        break;
                  }
                  i++;
            }
      }
}


//      Linux memory monitor support

bool MyFrame::GetMemoryStatus(int& mem_total, int& mem_used)
{

//      Use filesystem /proc/meminfo to determine memory status

    char *p;
    char buf[2000];
    int len;

//      Open and read the file
    int     fd = open("/proc/meminfo", O_RDONLY);

    if (fd == -1)
    {
        mem_total = 0;
        mem_used = 0;
        return false;
    }

    len = read(fd, buf, sizeof(buf) - 1);
    if (len <= 0) {
        close(fd);
        return false;
    }
    close(fd);
    buf[len] = 0;

    p = buf;

//      Parse the file contents
//      Here is an example....at least vaild for kernel 2.6
/*
MemTotal:       255684 kB
MemFree:          5996 kB
Buffers:         15260 kB
Cached:          99840 kB
SwapCached:         48 kB
Active:         176736 kB
Inactive:        45696 kB
HighTotal:           0 kB
HighFree:            0 kB
LowTotal:       255684 kB
LowFree:          5996 kB
SwapTotal:      610460 kB
SwapFree:       610412 kB
.
.
.
Hugepagesize:     4096 kB
*/

        char *s;
        char sbuf[100];
        char stoken[20];
        char skb[20];
        int sval, val_cnt;

        int m_total, m_active, m_inactive, m_buffers, m_cached, m_free;

        m_free = 0;
        m_total = 0;
        m_buffers = 0;
        m_cached = 0;

        val_cnt = 0;

//      Loop on the whole file
        while(*p)
        {
//      Extract line by line
                s=sbuf;
                while ((*p != '\n') && *p)
                        *s++ = *p++;
                *s = 0;
                p++;                    // skip \n

//      Parse  the sbuf line
                sscanf(sbuf, "%s %d %s\n", stoken, &sval, skb);

//      Look for and extract the required numbers

                if(!strcmp(stoken, "MemTotal:"))
                {
                        m_total = sval;
                        val_cnt++;
                }
                else if(!strcmp(stoken, "Active:"))
                {
                        m_active = sval;
                        val_cnt++;
                }
                else if(!strcmp(stoken, "Inactive:"))
                {
                        m_inactive = sval;
                        val_cnt++;
                }
                else if(!strcmp(stoken, "Buffers:"))
                {
                        m_buffers = sval;
                        val_cnt++;
                }
                else if(!strcmp(stoken, "Cached:"))
                {
                        m_cached = sval;
                        val_cnt++;
                }

                else if(!strcmp(stoken, "MemFree:"))
                {
                        m_free = sval;
                        val_cnt++;
                }

                if(val_cnt == 6)
                        break;
        }

//      Do the math....

//      int m_used = m_active + m_inactive - (m_buffers + m_cached);
//      mem_used = m_used;

        //      Maybe a good estimate of actual RAM available
        mem_used = (m_total - m_free) - (m_cached + m_buffers);

        mem_total = m_total;

        return true;

}

void MyFrame::DoPrint(void)
{

    wxPrintDialogData printDialogData(* g_printData);
    printDialogData.EnablePageNumbers(false);

    wxPrinter printer(& printDialogData);

    MyPrintout printout(_("Chart Print"));
    if (!printer.Print(this, &printout, true ))
    {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
            wxMessageBox(_("There was a problem printing.\nPerhaps your current printer is not set correctly?"), _T("OpenCPN"), wxOK);
//        else
//            wxMessageBox(_T("Print Cancelled"), _T("OpenCPN"), wxOK);
    }
    else
    {
        (*g_printData) = printer.GetPrintDialogData().GetPrintData();
    }


// Pass two printout objects: for preview, and possible printing.
/*
    wxPrintDialogData printDialogData(* g_printData);
    wxPrintPreview *preview = new wxPrintPreview(new MyPrintout, new MyPrintout, & printDialogData);
    if (!preview->Ok())
    {
        delete preview;
        wxMessageBox(_T("There was a problem previewing.\nPerhaps your current printer is not set correctly?"), _T("Previewing"), wxOK);
        return;
    }

    wxPreviewFrame *frame = new wxPreviewFrame(preview, this, _T("Demo Print Preview"), wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show();
*/

}

// toh, 2009.02.15
void MyFrame::DoExportGPX(void)
{
      if (pConfig)
            pConfig->ExportGPX(this);
}

// toh, 2009.02.16
void MyFrame::DoImportGPX(void)
{
      if (pConfig)
      {
            pConfig->ImportGPX(this);
            Refresh();
      }
}


//---------------------------------------------------------------------------------------------------------
//
//              Private Memory Management
//
//---------------------------------------------------------------------------------------------------------


void *x_malloc(size_t t)
{
        void *pr = malloc(t);

        //      malloc fails
        if(NULL == pr)
        {
                wxLogMessage(_T("x_malloc...malloc fails with request of %d bytes."), t);

                // Cat the /proc/meminfo file

                char *p;
                char buf[2000];
                int len;

                int     fd = open("/proc/meminfo", O_RDONLY);

                if (fd == -1)
                        exit(1);

                len = read(fd, buf, sizeof(buf) - 1);
                if (len <= 0) {
                        close(fd);
                        exit(1);
                }
                close(fd);
                buf[len] = 0;

                p=buf;
                while(*p)
                {
//                        printf("%c", *p++);
                }


                exit(0);
                return NULL;                            // for MSVC
        }

        else
        {
                if(t > malloc_max)
                {
                        malloc_max = t;
//                      wxLogMessage(_T("New malloc_max: %d", malloc_max));
                }

                return pr;                                      // good return
        }

}

void MyFrame::OnEvtTHREADMSG(wxCommandEvent & event)
{
      wxLogMessage(event.GetString());
}


void MyFrame::OnEvtOCPN_NMEA(OCPN_NMEAEvent & event)
{
      wxString sfixtime;
      bool bshow_tick = false;
      bool bis_recognized_sentence = true; //PL

      wxString str_buf = event.GetNMEAString();

      if( g_nNMEADebug && (g_total_NMEAerror_messages < g_nNMEADebug) )
      {
            g_total_NMEAerror_messages++;
            wxString msg(_T("MEH.NMEA Sentence received..."));
            msg.Append(str_buf);
            wxLogMessage(msg);
      }

      if(g_NMEALogWindow)
      {
            wxDateTime now = wxDateTime::Now();
            wxString ss = now.FormatISOTime();
            ss.Append(_T("  "));
            ss.Append(str_buf);
            g_NMEALogWindow->Add(ss);
            g_NMEALogWindow->Refresh(false);
      }

      m_NMEA0183 << str_buf;
      if(m_NMEA0183.PreParse())
      {
            if(m_NMEA0183.LastSentenceIDReceived == _T("RMC"))
            {
                  if(m_NMEA0183.Parse())
                  {
                              if(m_NMEA0183.Rmc.IsDataValid == NTrue)
                              {
                                    float llt = m_NMEA0183.Rmc.Position.Latitude.Latitude;
                                    int lat_deg_int = (int)(llt / 100);
                                    float lat_deg = lat_deg_int;
                                    float lat_min = llt - (lat_deg * 100);
                                    gLat = lat_deg + (lat_min/60.);
                                    if(m_NMEA0183.Rmc.Position.Latitude.Northing == South)
                                          gLat = -gLat;

                                    float lln = m_NMEA0183.Rmc.Position.Longitude.Longitude;
                                    int lon_deg_int = (int)(lln / 100);
                                    float lon_deg = lon_deg_int;
                                    float lon_min = lln - (lon_deg * 100);
                                    gLon = lon_deg + (lon_min/60.);
                                    if(m_NMEA0183.Rmc.Position.Longitude.Easting == West)
                                          gLon = -gLon;

                                    gSog = m_NMEA0183.Rmc.SpeedOverGroundKnots;
                                    gCog = m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue;

                                    if(m_NMEA0183.Rmc.MagneticVariationDirection == East)
                                          gVar =  m_NMEA0183.Rmc.MagneticVariation;
                                    else if(m_NMEA0183.Rmc.MagneticVariationDirection == West)
                                          gVar = -m_NMEA0183.Rmc.MagneticVariation;


                                    sfixtime = m_NMEA0183.Rmc.UTCTime;

                                    gGPS_Watchdog = gps_watchdog_timeout_ticks;

                                    bshow_tick = true;
                              }
                        }
                        else if(g_nNMEADebug)
                        {
                              wxString msg(_T("   "));
                              msg.Append(m_NMEA0183.ErrorMessage);
                              msg.Append(_T(" : "));
                              msg.Append(str_buf);
                              wxLogMessage(msg);
                        }

                  }

                  else if(m_NMEA0183.LastSentenceIDReceived == _T("HDT"))
                  {
                        if(m_NMEA0183.Parse())
                        {
                              gHdt = m_NMEA0183.Hdt.DegreesTrue;
                              g_bHDxValid = true;
                              gHDx_Watchdog = gps_watchdog_timeout_ticks;
                        }
                        else if(g_nNMEADebug)
                        {
                              wxString msg(_T("   "));
                              msg.Append(m_NMEA0183.ErrorMessage);
                              msg.Append(_T(" : "));
                              msg.Append(str_buf);
                              wxLogMessage(msg);
                        }

                  }


                  else if(m_NMEA0183.LastSentenceIDReceived == _T("HDG"))
                  {
                        if(m_NMEA0183.Parse())
                        {
                              gHdt = m_NMEA0183.Hdg.MagneticSensorHeadingDegrees;
                              if(m_NMEA0183.Hdg.MagneticVariationDirection == East)
                                    gHdt += m_NMEA0183.Hdg.MagneticVariationDegrees;
                              else if(m_NMEA0183.Hdg.MagneticVariationDirection == West)
                                    gHdt -= m_NMEA0183.Hdg.MagneticVariationDegrees;

                              g_bHDxValid = true;
                              gHDx_Watchdog = gps_watchdog_timeout_ticks;
                        }
                        else if(g_nNMEADebug)
                        {
                              wxString msg(_T("   "));
                              msg.Append(m_NMEA0183.ErrorMessage);
                              msg.Append(_T(" : "));
                              msg.Append(str_buf);
                              wxLogMessage(msg);
                        }

                  }

                  else if(m_NMEA0183.LastSentenceIDReceived == _T("HDM"))
                  {
                        if(m_NMEA0183.Parse())
                        {
                              gHdt = m_NMEA0183.Hdm.DegreesMagnetic;
                              gHdt += gVar;

                              g_bHDxValid = true;
                              gHDx_Watchdog = gps_watchdog_timeout_ticks;
                        }
                        else if(g_nNMEADebug)
                        {
                              wxString msg(_T("   "));
                              msg.Append(m_NMEA0183.ErrorMessage);
                              msg.Append(_T(" : "));
                              msg.Append(str_buf);
                              wxLogMessage(msg);
                        }

                  }

                  else if(m_NMEA0183.LastSentenceIDReceived == _T("VTG"))
                  {
                        if(m_NMEA0183.Parse())
                        {
                              //    Special check for unintialized values, as opposed to zero values
                              if(m_NMEA0183.Vtg.SpeedKnots < 999.)
                                    gSog = m_NMEA0183.Vtg.SpeedKnots;
                              if(m_NMEA0183.Vtg.TrackDegreesTrue < 999.)
                                    gCog = m_NMEA0183.Vtg.TrackDegreesTrue;
                        }
                        else if(g_nNMEADebug)
                        {
                              wxString msg(_T("   "));
                              msg.Append(m_NMEA0183.ErrorMessage);
                              msg.Append(_T(" : "));
                              msg.Append(str_buf);
                              wxLogMessage(msg);
                        }
                  }

                  else if(m_NMEA0183.LastSentenceIDReceived == _T("GSV"))
                  {
                        if(m_NMEA0183.Parse())
                        {
                              g_SatsInView = m_NMEA0183.Gsv.SatsInView;
                              gSAT_Watchdog = sat_watchdog_timeout_ticks;
                              g_bSatValid = true;

                        }
                        else if(g_nNMEADebug)
                        {
                              wxString msg(_T("   "));
                              msg.Append(m_NMEA0183.ErrorMessage);
                              msg.Append(_T(" : "));
                              msg.Append(str_buf);
                              wxLogMessage(msg);
                        }
                  }

                  else if(g_bUseGLL && m_NMEA0183.LastSentenceIDReceived == _T("GLL"))
                  {
                        if(m_NMEA0183.Parse())
                        {
                              if(m_NMEA0183.Gll.IsDataValid == NTrue)
                              {
                                    float llt = m_NMEA0183.Gll.Position.Latitude.Latitude;
                                    int lat_deg_int = (int)(llt / 100);
                                    float lat_deg = lat_deg_int;
                                    float lat_min = llt - (lat_deg * 100);
                                    gLat = lat_deg + (lat_min/60.);
                                    if(m_NMEA0183.Gll.Position.Latitude.Northing == South)
                                          gLat = -gLat;

                                    float lln = m_NMEA0183.Gll.Position.Longitude.Longitude;
                                    int lon_deg_int = (int)(lln / 100);
                                    float lon_deg = lon_deg_int;
                                    float lon_min = lln - (lon_deg * 100);
                                    gLon = lon_deg + (lon_min/60.);
                                    if(m_NMEA0183.Gll.Position.Longitude.Easting == West)
                                          gLon = -gLon;

                                    sfixtime = m_NMEA0183.Gll.UTCTime;

                                    gGPS_Watchdog = gps_watchdog_timeout_ticks;

                                    bshow_tick = true;
                              }
                        }
                        else if(g_nNMEADebug)
                        {
                              wxString msg(_T("   "));
                              msg.Append(m_NMEA0183.ErrorMessage);
                              msg.Append(_T(" : "));
                              msg.Append(str_buf);
                              wxLogMessage(msg);
                        }
                  }

                  else if(m_NMEA0183.LastSentenceIDReceived == _T("GGA"))
                  {
                        if(m_NMEA0183.Parse())
                        {
                              if(m_NMEA0183.Gga.GPSQuality > 0)
                              {
                                    float llt = m_NMEA0183.Gga.Position.Latitude.Latitude;
                                    int lat_deg_int = (int)(llt / 100);
                                    float lat_deg = lat_deg_int;
                                    float lat_min = llt - (lat_deg * 100);
                                    gLat = lat_deg + (lat_min/60.);
                                    if(m_NMEA0183.Gga.Position.Latitude.Northing == South)
                                          gLat = -gLat;

                                    float lln = m_NMEA0183.Gga.Position.Longitude.Longitude;
                                    int lon_deg_int = (int)(lln / 100);
                                    float lon_deg = lon_deg_int;
                                    float lon_min = lln - (lon_deg * 100);
                                    gLon = lon_deg + (lon_min/60.);
                                    if(m_NMEA0183.Gga.Position.Longitude.Easting == West)
                                          gLon = -gLon;

                                    sfixtime = m_NMEA0183.Gga.UTCTime;

                                    gGPS_Watchdog = gps_watchdog_timeout_ticks;

                                    g_SatsInView = m_NMEA0183.Gga.NumberOfSatellitesInUse;
                                    gSAT_Watchdog = sat_watchdog_timeout_ticks;
                                    g_bSatValid = true;

                                    bshow_tick = true;
                              }
                        }
                        else if(g_nNMEADebug)
                        {
                              wxString msg(_T("   "));
                              msg.Append(m_NMEA0183.ErrorMessage);
                              msg.Append(_T(" : "));
                              msg.Append(str_buf);
                              wxLogMessage(msg);
                        }
                  }
      }
      else
      {
            bis_recognized_sentence = false;
            if( g_nNMEADebug && (g_total_NMEAerror_messages < g_nNMEADebug) )
            {
                  g_total_NMEAerror_messages++;
                  wxString msg(_T("   Unrecognized NMEA Sentence..."));
                  msg.Append(str_buf);
                  wxLogMessage(msg);
            }
      }

      if(bis_recognized_sentence)
            PostProcessNNEA(bshow_tick, sfixtime);
}





void MyFrame::OnEvtNMEA(wxCommandEvent & event)
{
    bool bshow_tick = false;
    time_t fixtime;

    switch(event.GetExtraLong())
    {
        case EVT_NMEA_DIRECT:
        {
                wxMutexLocker stateLocker(m_mutexNMEAEvent);          // scope is this case

                GenericPosDat *pGPSData = (GenericPosDat *)event.GetClientData();
                gLat = pGPSData->kLat;
                gLon = pGPSData->kLon;
                gCog = pGPSData->kCog;
                gSog = pGPSData->kSog;
                gVar = pGPSData->kVar;

                fixtime = pGPSData->FixTime;

                bool last_bGPSValid = bGPSValid;
                bGPSValid = true;

                gGPS_Watchdog = gps_watchdog_timeout_ticks;

                g_SatsInView = pGPSData->nSats;
                if(g_SatsInView)
                {
                  gSAT_Watchdog = sat_watchdog_timeout_ticks;
                  g_bSatValid = true;
                }

                if(!last_bGPSValid)
                {
                      UpdateToolbarStatusBox();
                      cc1->Refresh(false);            // cause own-ship icon to redraw
                }

                bshow_tick = true;

                break;
        }

    }           // switch

    wxString sfixtime(_T(""));
    PostProcessNNEA(bshow_tick, sfixtime);

}

void MyFrame::PostProcessNNEA(bool brx_rmc, wxString &sfixtime)
{
      //    If gSog is greater than some threshold, we determine that we are "cruising"
      if(gSog > 3.0)
            g_bCruising = true;

      if(brx_rmc)
      {
            if(g_nNMEADebug)
            {
                  wxString msg(_T("PostProcess NMEA with valid position"));
                  wxLogMessage(msg);
            }

            //      Maintain the validity flags
            bool last_bGPSValid = bGPSValid;
            bGPSValid = true;
            if(!last_bGPSValid)
            {
//                  UpdateToolbarStatusWindow(Current_Ch, false);
                  UpdateToolbarStatusBox();
            }


            //      Show a little heartbeat tick in StatusWindow0 on NMEA events
            //      But no faster than 10 hz.
            unsigned long uiCurrentTickCount ;
            m_MMEAeventTime.SetToCurrent() ;
            uiCurrentTickCount = m_MMEAeventTime.GetMillisecond() / 100 ;           // tenths of a second
            uiCurrentTickCount += m_MMEAeventTime.GetTicks() * 10 ;
            if(uiCurrentTickCount > m_ulLastNEMATicktime + 1)
            {
                  m_ulLastNEMATicktime = uiCurrentTickCount ;

                  if(tick_idx++ > 6)
                        tick_idx = 0;
            }
      }

//    Show gLat/gLon in StatusWindow0

      if(NULL != GetStatusBar())
      {
            char tick_buf[2];
            tick_buf[0] = nmea_tick_chars[tick_idx];
            tick_buf[1] = 0;

            wxString s1(tick_buf, wxConvUTF8);
            s1 += _(" Ship: ");
            s1 += toSDMM(1, gLat);
            s1 += _T("   ");
            s1 += toSDMM(2, gLon);
            SetStatusText ( s1, STAT_FIELD_TICK );

            wxString sogcog;
            sogcog.Printf(_T("SOG: %5.2f kts  COG: %5.0f Deg"), gSog, gCog);
            SetStatusText ( sogcog, STAT_FIELD_SOGCOG );
      }

//    Maintain average COG for Course Up Mode

      //    Make a hole
      for(int i = g_COGAvgSec-1 ; i > 0 ; i--)
            COGTable[i] = COGTable[i-1];
      COGTable[0] = gCog;

      //
      double sum = 0.;
      for(int i = 0 ; i < g_COGAvgSec ; i++)
      {
            double adder = COGTable[i];

            if(abs(adder - g_COGAvg) > 180.)
            {
                  if((adder - g_COGAvg) > 0.)
                        adder -= 360.;
                  else
                        adder += 360.;
            }

            sum += adder;
      }
      sum /= g_COGAvgSec;


      if(sum < 0.)
            sum += 360.;
      else if(sum >= 360.)
            sum -= 360.;

      g_COGAvg = sum;


#ifdef ocpnUPDATE_SYSTEM_TIME
//      Use the fix time to update the local system clock, only once per session
      if((sfixtime.Len()) && s_bSetSystemTime && (m_bTimeIsSet == false))
      {
            wxDateTime Fix_Time;

            if(6 == sfixtime.Len())                   // perfectly recognised format?
            {
                  wxString a;
                  long b;
                  int hr = 0;
                  int min = 0;
                  int sec = 0;

                  a = sfixtime.Mid(0,2);
                  if(a.ToLong(&b))
                        hr = b;
                  a = sfixtime.Mid(2,2);
                  if(a.ToLong(&b))
                        min = b;
                  a = sfixtime.Mid(4,2);
                  if(a.ToLong(&b))
                        sec = b;

                  Fix_Time.Set(hr, min, sec);
            }
            wxString fix_time_format = Fix_Time.Format(_T("%Y-%m-%dT%H:%M:%S"));  // this should show as LOCAL


      //          Compare the server (fix) time to the current system time
            wxDateTime sdt;
            sdt.SetToCurrent();
            wxDateTime cwxft = Fix_Time;                  // take a copy
            wxTimeSpan ts;
            ts = cwxft.Subtract(sdt);

            int b = (ts.GetSeconds()).ToLong();

      //          Correct system time if necessary
      //      Only set the time if wrong by more than 1 minute, and less than 2 hours
      //      This should eliminate bogus times which may come from faulty GPS units

            if((abs(b) > 60) && (abs(b) < (2 * 60 * 60)))
            {

      #ifdef __WXMSW__
      //      Fix up the fix_time to convert to GMT
                  Fix_Time = Fix_Time.ToGMT();

      //    Code snippet following borrowed from wxDateCtrl, MSW

                  const wxDateTime::Tm tm(Fix_Time.GetTm());


                  SYSTEMTIME stm;
                  stm.wYear = (WXWORD)tm.year;
                  stm.wMonth = (WXWORD)(tm.mon - wxDateTime::Jan + 1);
                  stm.wDay = tm.mday;

                  stm.wDayOfWeek = 0;
                  stm.wHour = Fix_Time.GetHour();
                  stm.wMinute = tm.min;
                  stm.wSecond = tm.sec;
                  stm.wMilliseconds = 0;

                        ::SetSystemTime(&stm);            // in GMT


      #else


      //      This contortion sets the system date/time on POSIX host
      //      Requires the following line in /etc/sudoers
      //          nav ALL=NOPASSWD:/bin/date -s *

                        wxString msg;
                        msg.Printf(_T("Setting system time, delta t is %d seconds"), b);
                        wxLogMessage(msg);

                        wxString sdate(Fix_Time.Format(_T("%D")));
                        sdate.Prepend(_T("sudo /bin/date -s \""));

                        wxString stime(Fix_Time.Format(_T("%T")));
                        stime.Prepend(_T(" "));
                        sdate.Append(stime);
                        sdate.Append(_T("\""));

                        msg.Printf(_T("Linux command is:"));
                        msg += sdate;
                        wxLogMessage(msg);
                        wxExecute(sdate, wxEXEC_ASYNC);

      #endif      //__WXMSW__
                        m_bTimeIsSet = true;

            }           // if needs correction
      }               // if valid time

#endif            //ocpnUPDATE_SYSTEM_TIME
}



void MyFrame::StopSockets(void)
{
#ifdef USE_WIFI_CLIENT
    if(pWIFI)
       pWIFI->Pause();
#endif

    if(g_pnmea)
          g_pnmea->Pause();
}

void MyFrame::ResumeSockets(void)
{
#ifdef USE_WIFI_CLIENT
    if(pWIFI)
       pWIFI->UnPause();
#endif

    if(g_pnmea)
          g_pnmea->UnPause();
}

//----------------------------------------------------------------------------------------------------------
//      Application-wide CPL Error handler
//----------------------------------------------------------------------------------------------------------

void MyCPLErrorHandler( CPLErr eErrClass, int nError,
                             const char * pszErrorMsg )

{
    char msg[256];

    if( eErrClass == CE_Debug )
        snprintf(msg, 255,"CPL: %s", pszErrorMsg);
    else if( eErrClass == CE_Warning )
        snprintf(msg, 255,"CPL Warning %d: %s", nError, pszErrorMsg);
    else
        snprintf(msg, 255,"CPL ERROR %d: %s", nError, pszErrorMsg);

    wxString str(msg, wxConvUTF8);
    wxLogMessage(str);
}

//----------------------------------------------------------------------------------------------------------
//      Printing Framework Support
//----------------------------------------------------------------------------------------------------------

bool MyPrintout::OnPrintPage(int page)
{
    wxDC *dc = GetDC();
    if (dc)
    {
        if (page == 1)
            DrawPageOne(dc);

        return true;
    }
    else
        return false;
}

bool MyPrintout::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;

    return true;
}

void MyPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
    *minPage = 0;
    *maxPage = 1;
    *selPageFrom = 1;
    *selPageTo = 1;
}

bool MyPrintout::HasPage(int pageNum)
{
//    return (pageNum == 1);
    return true;
}

void MyPrintout::DrawPageOne(wxDC *dc)
{

    // Get the Size of the Chart Canvas
    int sx, sy;
    cc1->GetClientSize(&sx, &sy);                       // of the canvas

    float maxX = sx;
    float maxY = sy;

    // Let's have at least 50 device units margin
    float marginX = 50;
    float marginY = 50;

    // Add the margin to the graphic size
    maxX += (2*marginX);
    maxY += (2*marginY);

    // Get the size of the DC in pixels
    int w, h;
    dc->GetSize(&w, &h);

    // Calculate a suitable scaling factor
    float scaleX=(float)(w/maxX);
    float scaleY=(float)(h/maxY);

    // Use x or y scaling factor, whichever fits on the DC
    float actualScale = wxMin(scaleX,scaleY);

    // Calculate the position on the DC for centring the graphic
    float posX = (float)((w - (maxX*actualScale))/2.0);
    float posY = (float)((h - (maxY*actualScale))/2.0);

    // Set the scale and origin
    dc->SetUserScale(actualScale, actualScale);
    dc->SetDeviceOrigin( (long)posX, (long)posY );

//  Get the latest bitmap as rendered by the ChartCanvas
//  And Blit/scale it onto the Printer DC
    wxMemoryDC mdc;
    mdc.SelectObject(*(cc1->pscratch_bm));

    dc->Blit(0,0,cc1->pscratch_bm->GetWidth(),cc1->pscratch_bm->GetHeight(),&mdc, 0, 0);
    mdc.SelectObject(wxNullBitmap);

}

//---------------------------------------------------------------------------------------
//
//		GPS Positioning Device Detection
//
//---------------------------------------------------------------------------------------

/*
*     Enumerate all the serial ports on the system
*
*     wxArrayString *EnumerateSerialPorts(void)

*     Very system specific, unavoidably.
*/

#ifdef __WXGTK__
extern "C" int wait(int *);                     // POSIX wait() for process
#endif

wxArrayString *EnumerateSerialPorts(void)
{
      wxArrayString *preturn = new wxArrayString;

#ifdef __WXGTK__

      //    Looking for user privilege openable devices in /dev

      wxString sdev;

      for(int idev=0 ; idev < 8 ; idev++)
      {
            sdev.Printf(_T("/dev/ttyS%1d"), idev);

            int fd = open(sdev.mb_str(), O_RDWR|O_NDELAY|O_NOCTTY);
            if(fd > 0)
            {
                  /*  add to the output array  */
                  preturn->Add(wxString(sdev));
                  close(fd);
            }
      }

      for(int idev=0 ; idev < 8 ; idev++)
      {
            sdev.Printf(_T("/dev/ttyUSB%1d"), idev);

            int fd = open(sdev.mb_str(), O_RDWR|O_NDELAY|O_NOCTTY);
            if(fd > 0)
            {
                  /*  add to the output array  */
                  preturn->Add(wxString(sdev));
                  close(fd);
            }
      }

      //    Looking for BlueTooth GPS devices
      for(int idev=0 ; idev < 8 ; idev++)
      {
             sdev.Printf(_T("/dev/rfcomm%1d"), idev);

             int fd = open(sdev.mb_str(), O_RDWR|O_NDELAY|O_NOCTTY);
             if(fd > 0)
             {
                    /*  add to the output array  */
                    preturn->Add(wxString(sdev));
                    close(fd);
             }
      }



      //    A Fallback position, in case udev has failed or something.....
      if(preturn->IsEmpty())
      {
            preturn->Add( _T("/dev/ttyS0"));
            preturn->Add( _T("/dev/ttyS1"));
            preturn->Add( _T("/dev/ttyUSB0"));
            preturn->Add( _T("/dev/ttyUSB1"));
      }


#endif




#ifdef PROBE_PORTS__WITH_HELPER

/*
*     For modern Linux/(Posix??) systems, we may use
*     the system files /proc/tty/driver/serial
*     and /proc/tty/driver/usbserial to identify
*     available serial ports.
*     A complicating factor is that most (all??) linux
*     systems require root privileges to access these files.
*     We will use a helper program method here, despite implied vulnerability.
*/

char buf[256]; // enough to hold one line from serial devices list
char left_digit;
char right_digit;
int port_num;
FILE *f;

      pid_t pID = vfork();

      if (pID == 0)                // child
      {
//    Temporarily gain root privileges
            seteuid(file_user_id);

//  Execute the helper program
            execlp("ocpnhelper", "ocpnhelper", "-SB", NULL);

//  Return to user privileges
            seteuid(user_user_id);

            wxLogMessage(_T("Warning: ocpnhelper failed...."));
            _exit(0); // If exec fails then exit forked process.
       }


       wait(NULL);                  // for the child to quit

//    Read and parse the files

/*
      * see if we have any traditional ttySx ports available
*/
      f = fopen("/var/tmp/serial", "r");

      if (f != NULL)
      {
            wxLogMessage(_T("Parsing copy of /proc/tty/driver/serial..."));

            /* read in each line of the file */
            while(fgets(buf, sizeof(buf), f) != NULL)
            {
                  wxString sm(buf, wxConvUTF8);
                  sm.Prepend(_T("   "));
                  sm.Replace(_T("\n"), _T(" "));
                  wxLogMessage(sm);

                  /* if the line doesn't start with a number get the next line */
                  if (buf[0] < '0' || buf[0] > '9')
                        continue;

      /*
                  * convert digits to an int
      */
                  left_digit = buf[0];
                  right_digit = buf[1];
                  if (right_digit < '0' || right_digit > '9')
                        port_num = left_digit - '0';
                  else
                        port_num = (left_digit - '0') * 10 + right_digit - '0';

                  /* skip if "unknown" in the string */
                  if (strstr(buf, "unknown") != NULL)
                        continue;

                  /* upper limit of 15 */
                  if (port_num > 15)
                        continue;

                  /* create string from port_num  */

                  wxString s;
                  s.Printf(_T("/dev/ttyS%d"), port_num);

                  /*  add to the output array  */
                  preturn->Add(wxString(s));

            }

            fclose(f);
      }


/*
      * Same for USB ports
*/
      f = fopen("/var/tmp/usbserial", "r");

      if (f != NULL)
      {
            wxLogMessage(_T("Parsing copy of /proc/tty/driver/usbserial..."));

            /* read in each line of the file */
            while(fgets(buf, sizeof(buf), f) != NULL)
            {

                  wxString sm(buf, wxConvUTF8);
                  sm.Prepend(_T("   "));
                  sm.Replace(_T("\n"), _T(" "));
                  wxLogMessage(sm);

                  /* if the line doesn't start with a number get the next line */
                  if (buf[0] < '0' || buf[0] > '9')
                        continue;

      /*
                  * convert digits to an int
      */
                  left_digit = buf[0];
                  right_digit = buf[1];
                  if (right_digit < '0' || right_digit > '9')
                        port_num = left_digit - '0';
                  else
                        port_num = (left_digit - '0') * 10 + right_digit - '0';

                  /* skip if "unknown" in the string */
                  if (strstr(buf, "unknown") != NULL)
                        continue;

                  /* upper limit of 15 */
                  if (port_num > 15)
                        continue;

                  /* create string from port_num  */

                  wxString s;
                  s.Printf(_T("/dev/ttyUSB%d"), port_num);

                  /*  add to the output array  */
                  preturn->Add(wxString(s));

            }

            fclose(f);
      }


      //    As a fallback, in case seteuid doesn't work....
      //    provide some defaults
      //    This is currently the case for GTK+, which
      //    refuses to run suid.  sigh...

      if(preturn->IsEmpty())
      {
            preturn->Add( _T("/dev/ttyS0"));
            preturn->Add( _T("/dev/ttyS1"));
            preturn->Add( _T("/dev/ttyUSB0"));
            preturn->Add( _T("/dev/ttyUSB1"));
      }

//    Clean up the temporary files created by helper.
      pid_t cpID = vfork();

      if (cpID == 0)                // child
      {
//    Temporarily gain root privileges
            seteuid(file_user_id);

//  Execute the helper program
            execlp("ocpnhelper", "ocpnhelper", "-U", NULL);

//  Return to user privileges
            seteuid(user_user_id);
            _exit(0); // If exec fails then exit forked process.
      }

#endif      // __WXGTK__

#ifdef __WXOSX__
#include "macutils.h"
      char* paPortNames[MAX_SERIAL_PORTS] ;
      int iPortNameCount ;

      memset(paPortNames,0x00,sizeof(paPortNames)) ;
      iPortNameCount = FindSerialPortNames(&paPortNames[0],MAX_SERIAL_PORTS) ;
      for (int iPortIndex=0 ; iPortIndex<iPortNameCount ; iPortIndex++)
      {
            wxString sm(paPortNames[iPortIndex], wxConvUTF8);
            preturn->Add(sm);
            free(paPortNames[iPortIndex]) ;
      }
#endif      //__WXOSX__


#ifdef __WXMSW__
/*************************************************************************
 * Windows provides no system level enumeration of available serial ports
 * There are several ways of doing this.
 *
 *************************************************************************/

#include <windows.h>



      //    Method 1:  Use GetDefaultCommConfig()
      // Try first 16 possible COM ports, check for a default configuration
      for (int i=1; i<g_nCOMPortCheck; i++)
      {
            char s[20];
            sprintf(s, "COM%d", i);

            COMMCONFIG cc;
            DWORD dwSize = sizeof(COMMCONFIG);
            if (GetDefaultCommConfig(s, &cc, &dwSize))
                  preturn->Add(wxString(s));
      }


#if 0
      // Method 2:  Use FileOpen()
      // Try all 255 possible COM ports, check to see if it can be opened, or if
      // not, that an expected error is returned.

      BOOL bFound;
      for (int j=1; j<256; j++)
{
      char s[20];
      sprintf(s, "\\\\.\\COM%d", j);

          // Open the port tentatively
      BOOL bSuccess = FALSE;
      HANDLE hComm = ::CreateFile(s, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);

            //  Check for the error returns that indicate a port is there, but not currently useable
      if (hComm == INVALID_HANDLE_VALUE)
      {
            DWORD dwError = GetLastError();

            if (dwError == ERROR_ACCESS_DENIED ||
                dwError == ERROR_GEN_FAILURE ||
                dwError == ERROR_SHARING_VIOLATION ||
                dwError == ERROR_SEM_TIMEOUT)
                  bFound = TRUE;
      }
      else
      {
            bFound = TRUE;
            CloseHandle(hComm);
      }

      if (bFound)
            preturn->Add(wxString(s));
}
#endif

//    Search for (any?) Garmin device on Windows platforms

      HDEVINFO hdeviceinfo = INVALID_HANDLE_VALUE;
      SP_DEVICE_INTERFACE_DATA deviceinterface;

 //     wxLogMessage(_T("In EnumerateSerialPorts(), searching for Garmin DeviceInterface..."));

      hdeviceinfo = SetupDiGetClassDevs( (GUID *) &GARMIN_DETECT_GUID,
                                                      NULL, NULL,
                                                      DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);

      if (hdeviceinfo != INVALID_HANDLE_VALUE)
            wxLogMessage(_T("Found Garmin USB Driver."));


    deviceinterface.cbSize = sizeof(deviceinterface);

    if (SetupDiEnumDeviceInterfaces(hdeviceinfo,
                                                      NULL,
                                                      (GUID *) &GARMIN_DETECT_GUID,
                                                      0,
                                                      &deviceinterface))
      {
            wxLogMessage(_T("Found Garmin Device."));

            preturn->Add(_T("GARMIN"));         // Add generic Garmin selectable device
            g_bGarminPersistance = true;        // And record the existance

      }

#endif      //__WXMSW__

      return preturn;
}

 void appendOSDirSlash(wxString* pString)
 {
       wxChar sep = wxFileName::GetPathSeparator();
       if (pString->Last() != sep)
         pString->Append(sep);
 }


/*************************************************************************
 * Global color management routines
 *
 *************************************************************************/


wxColour GetGlobalColor(wxString colorName)
{
      wxColour ret_color;
      //    Use the S52 Presentation library if present
      if(ps52plib)
      {
            ret_color = ps52plib->S52_getwxColour(colorName);

            if(!ret_color.Ok())           //261 likes Ok(), 283 likes IsOk()...
            {
                  if(NULL != pcurrent_user_color_hash)
                        ret_color = ( *pcurrent_user_color_hash ) [colorName];
            }
      }

      else
      {
            if(NULL != pcurrent_user_color_hash)
                  ret_color = ( *pcurrent_user_color_hash ) [colorName];
      }

      //    Default
      if(!ret_color.Ok())
            ret_color.Set(128,128,128);  // Simple Grey

      return ret_color;
}


static const char *usercolors[] = {
"Table:DAY",
"GREEN1;120;255;120;",
"GREEN2; 45;150; 45;",
"GREEN3;200;220;200;",
"GREEN4;  0;255;  0;",
"BLUE1; 170;170;255;",
"BLUE2;  45; 45;170;",
"BLUE3;   0;  0;255;",
"GREY1; 200;200;200;",
"GREY2; 230;230;230;",
"RED1;  220;200;200;",
"UBLCK;   0;  0;  0;",
"UWHIT; 255;255;255;",
"URED;  255;  0;  0;",
"UGREN;   0;255;  0;",
"YELO1; 243;229; 47;",
"YELO2; 128; 80;  0;",
"DILG0; 238;239;242;",              // Dialog Background white
"DILG1; 212;208;200;",              // Dialog Background
"DILG2; 255;255;255;",              // Control Background
"DILG3;   0;  0;  0;",              // Text
"UITX1;   0;  0;  0;",              // Menu Text, derived from UINFF
"UDKRD; 124; 16;  0;",

"Table:DUSK",
"GREEN1; 60;128; 60;",
"GREEN2; 22; 75; 22;",
"GREEN3; 80;100; 80;",
"GREEN4;  0;128;  0;",
"BLUE1;  80; 80;160;",
"BLUE2;  30; 30;120;",
"BLUE3;   0;  0;128;",
"GREY1; 100;100;100;",
"GREY2; 128;128;128;",
"RED1;  150;100;100;",
"UBLCK;   0;  0;  0;",
"UWHIT; 255;255;255;",
"URED;  120; 54; 11;",
"UGREN;  35;110; 20;",
"YELO1; 120;115; 24;",
"YELO2;  64; 40;  0;",
"DILG0; 110;110;110;",              // Dialog Background
"DILG1; 110;110;110;",              // Dialog Background
"DILG2; 100;100;100;",              // Control Background
"DILG3; 130;130;130;",              // Text
"UITX1;  41; 46; 46;",              // Menu Text, derived from UINFF
"UDKRD;  80;  0;  0;",

"Table:NIGHT",
"GREEN1; 30; 80; 30;",
"GREEN2; 15; 60; 15;",
"GREEN3; 12; 23;  9;",
"GREEN4;  0; 64;  0;",
"BLUE1;  60; 60;100;",
"BLUE2;  22; 22; 85;",
"BLUE3;   0;  0; 40;",
"GREY1;  48; 48; 48;",
"GREY2;  64; 64; 64;",
"RED1;  100; 50; 50;",
"UWHIT; 255;255;255;",
"UBLCK;   0;  0;  0;",
"URED;   60; 27;  5;",
"UGREN;  17; 55; 10;",
"YELO1;  60; 65; 12;",
"YELO2;  32; 20;  0;",
"DILG0;  80; 80; 80;",              // Dialog Background
"DILG1;  80; 80; 80;",              // Dialog Background
"DILG2;  52; 52; 52;",              // Control Background
"DILG3;  65; 65; 65;",              // Text
"UITX1;  31; 34; 35;",              // Menu Text, derived from UINFF
"UDKRD;  50;  0;  0;",

"*****"
};


int get_static_line(char *d, const char **p, int index, int n)
{
      if(!strcmp(p[index], "*****"))
            return 0;

      strncpy(d, p[index], n);
      return strlen(d);
}

void InitializeUserColors(void)
{
      const char **p = usercolors;
      char buf[80];
      int index = 0;
      char TableName[20];
      colTable *ctp;
      colTable *ct;
      int colIdx = 0;
      int R,G,B;

      UserColorTableArray = new wxArrayPtrVoid;
      UserColourHashTableArray = new wxArrayPtrVoid;

      //    Create 3 color table entries
      ct = new colTable;
      ct->tableName = new wxString ( _T("DAY"));
      ct->color     = new wxArrayPtrVoid;
      UserColorTableArray->Add ( ( void * ) ct );

      ct = new colTable;
      ct->tableName = new wxString ( _T("DUSK"));
      ct->color     = new wxArrayPtrVoid;
      UserColorTableArray->Add ( ( void * ) ct );

      ct = new colTable;
      ct->tableName = new wxString ( _T("NIGHT"));
      ct->color     = new wxArrayPtrVoid;
      UserColorTableArray->Add ( ( void * ) ct );


      while((get_static_line(buf, p, index,80)))
      {
            if(!strncmp(buf, "Table", 5))
            {
                  sscanf(buf, "Table:%s", TableName);

                  for(unsigned int it=0 ; it < UserColorTableArray->GetCount() ; it++)
                  {
                        ctp = (colTable *)(UserColorTableArray->Item(it));
                        if(!strcmp(TableName, ctp->tableName->mb_str()))
                        {
                              ct = ctp;
                              colIdx = 0;
                              break;
                        }
                  }

            }
            else
            {
                  char name[80];
                  int j=0;
                  while(buf[j] != ';')
                  {
                        name[j] = buf[j];
                        j++;
                  }
                  name[j] = 0;

                  S52color *c = new S52color;
                  strcpy(c->colName, name);


                  sscanf(&buf[j], ";%i;%i;%i", &R, &G, &B);
                  c->R = (char)R;
                  c->G = (char)G;
                  c->B = (char)B;

                  ct->color->Add ( c );

            }

           index ++;
      }

      //    Now create the Hash tables

            for ( unsigned int its=0 ; its < UserColorTableArray->GetCount() ; its++ )
            {
                  ColourHash *phash = new ColourHash;
                  UserColourHashTableArray->Add ( ( void * ) phash );

                  colTable *ctp = ( colTable * ) ( UserColorTableArray->Item ( its ) );

                  for ( unsigned int ic=0 ; ic < ctp->color->GetCount() ; ic++ )
                  {
                        S52color *c2 = ( S52color * ) ( ctp->color->Item ( ic ) );

                        wxColour c ( c2->R, c2->G, c2->B );
                        wxString key ( c2->colName, wxConvUTF8 );
                        ( *phash ) [key] = c;

                  }
            }

            //    Establish a default hash table pointer
            //    in case a color is needed before ColorScheme is set
            pcurrent_user_color_hash = (ColourHash *)UserColourHashTableArray->Item(0);
}

void DeInitializeUserColors(void)
{
      unsigned int i;
      for( i = 0 ; i< UserColorTableArray->GetCount() ; i++)
      {
            colTable *ct = (colTable *)UserColorTableArray->Item(i);

            for(unsigned int j = 0 ; j<ct->color->GetCount() ; j++)
            {
                  S52color *c = (S52color *)ct->color->Item(j);
                  delete c;                     //color
            }

            delete ct->tableName;               // wxString
            delete ct->color;                   // wxArrayPtrVoid

            delete ct;                          // colTable
      }

      delete UserColorTableArray;

      for( i = 0 ; i< UserColourHashTableArray->GetCount() ; i++)
      {
            ColourHash *phash = (ColourHash *)UserColourHashTableArray->Item(i);
            delete phash;
      }

      delete UserColourHashTableArray;

}

#ifdef __WXMSW__

#define NCOLORS 40

typedef struct _MSW_COLOR_SPEC
{
      int         COLOR_NAME;
      wxString    S52_RGB_COLOR;
      int         SysRGB_COLOR;
}MSW_COLOR_SPEC;

MSW_COLOR_SPEC color_spec[] ={
      {COLOR_MENU,                  _T("UIBCK"), 0},
      {COLOR_MENUTEXT,              _T("UITX1"), 0},
      {COLOR_BTNSHADOW,             _T("UIBCK"), 0},                        // Menu Frame
      {-1, _T(""), 0}
};

void SaveSystemColors()
{
/*
      color_3dface = pGetSysColor(COLOR_3DFACE);
      color_3dhilite = pGetSysColor(COLOR_3DHILIGHT);
      color_3dshadow = pGetSysColor(COLOR_3DSHADOW);
      color_3ddkshadow = pGetSysColor(COLOR_3DDKSHADOW);
      color_3dlight = pGetSysColor(COLOR_3DLIGHT);
	  color_activecaption = pGetSysColor(COLOR_ACTIVECAPTION);
      color_gradientactivecaption = pGetSysColor(27); //COLOR_3DLIGHT);
      color_captiontext = pGetSysColor(COLOR_CAPTIONTEXT);
      color_windowframe = pGetSysColor(COLOR_WINDOWFRAME);
      color_inactiveborder = pGetSysColor(COLOR_INACTIVEBORDER);
*/
      //    Record the default system color in my substitution structure
      MSW_COLOR_SPEC *pcspec = &color_spec[0];
      while(pcspec->COLOR_NAME != -1)
      {
            pcspec->SysRGB_COLOR = pGetSysColor(pcspec->COLOR_NAME);
            pcspec++;
      }
}

void RestoreSystemColors()
{
      int element[NCOLORS];
      int rgbcolor[NCOLORS];
      int i=0;

      MSW_COLOR_SPEC *pcspec = &color_spec[0];
      while(pcspec->COLOR_NAME != -1)
      {
            element[i] = pcspec->COLOR_NAME;
            rgbcolor[i] = pcspec->SysRGB_COLOR;

            pcspec++;
            i++;
      }

      pSetSysColors(i, (unsigned long *)&element[0], (unsigned long *)&rgbcolor[0]);

}



#endif


void SetSystemColors ( ColorScheme cs )
{
//---------------
#if 0
      //    This is the list of Color Types from winuser.h
      /*
      * Color Types
      */
#define CTLCOLOR_MSGBOX         0
#define CTLCOLOR_EDIT           1
#define CTLCOLOR_LISTBOX        2
#define CTLCOLOR_BTN            3
#define CTLCOLOR_DLG            4
#define CTLCOLOR_SCROLLBAR      5
#define CTLCOLOR_STATIC         6
#define CTLCOLOR_MAX            7

#define COLOR_SCROLLBAR         0         //??
#define COLOR_BACKGROUND        1         //??
#define COLOR_ACTIVECAPTION     2       //??
#define COLOR_INACTIVECAPTION   3         //??
#define COLOR_MENU              4         // Menu background
#define COLOR_WINDOW            5         // default window background
#define COLOR_WINDOWFRAME       6         // Sub-Window frames, like status bar, etc..
#define COLOR_MENUTEXT          7         // Menu text
#define COLOR_WINDOWTEXT        8         //??
#define COLOR_CAPTIONTEXT       9         //??
#define COLOR_ACTIVEBORDER      10        //??
#define COLOR_INACTIVEBORDER    11       //??
#define COLOR_APPWORKSPACE      12       //??
#define COLOR_HIGHLIGHT         13       //Highlited text background  in query box tree
#define COLOR_HIGHLIGHTTEXT     14        //??
#define COLOR_BTNFACE           15        //??
#define COLOR_BTNSHADOW         16        // Menu Frame
#define COLOR_GRAYTEXT          17        // Greyed out text in menu
#define COLOR_BTNTEXT           18        //??
#define COLOR_INACTIVECAPTIONTEXT 19      //??
#define COLOR_BTNHIGHLIGHT      20        //??

#if(WINVER >= 0x0400)
#define COLOR_3DDKSHADOW        21        //??
#define COLOR_3DLIGHT           22        // Grid rule lines in list control
#define COLOR_INFOTEXT          23        //??
#define COLOR_INFOBK            24
#endif /* WINVER >= 0x0400 */

#if(WINVER >= 0x0500)
#define COLOR_HOTLIGHT          26              //??
#define COLOR_GRADIENTACTIVECAPTION 27        //??
#define COLOR_GRADIENTINACTIVECAPTION 28        //??
#if(WINVER >= 0x0501)
#define COLOR_MENUHILIGHT       29              // Selected item in menu, maybe needs reset on popup menu?
#define COLOR_MENUBAR           30              //??
#endif /* WINVER >= 0x0501 */
#endif /* WINVER >= 0x0500 */

#if(WINVER >= 0x0400)
#define COLOR_DESKTOP           COLOR_BACKGROUND
#define COLOR_3DFACE            COLOR_BTNFACE
#define COLOR_3DSHADOW          COLOR_BTNSHADOW
#define COLOR_3DHIGHLIGHT       COLOR_BTNHIGHLIGHT
#define COLOR_3DHILIGHT         COLOR_BTNHIGHLIGHT
#define COLOR_BTNHILIGHT        COLOR_BTNHIGHLIGHT
#endif /* WINVER >= 0x0400 */
#endif

#ifdef __WXMSW__
      int element[NCOLORS];
      int rgbcolor[NCOLORS];
        int i=0;
        if ( ( GLOBAL_COLOR_SCHEME_DUSK == cs ) || ( GLOBAL_COLOR_SCHEME_NIGHT == cs ) )
        {
              MSW_COLOR_SPEC *pcspec = &color_spec[0];
              while(pcspec->COLOR_NAME != -1)
              {
                    wxColour color = GetGlobalColor(pcspec->S52_RGB_COLOR);
                    rgbcolor[i] = (color.Red() << 16) + (color.Green() << 8) + color.Blue();
                    element[i] = pcspec->COLOR_NAME;

                    i++;
                    pcspec++;
              }
                pSetSysColors ( i, ( unsigned long * ) &element[0], ( unsigned long * ) &rgbcolor[0] );


        }
        else
        {         // for daylight colors, use default windows colors as saved....

              RestoreSystemColors();
        }
#endif
}


/////////////////////////////////////////////////////////////////////////////
//        OpenCPN ToolBar and friends
/////////////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class ocpnToolBarTool : public wxToolBarToolBase
{
      public:
            ocpnToolBarTool(ocpnToolBarSimple *tbar,
                                int id,
                                const wxString& label,
                                const wxBitmap& bmpNormal,
                                const wxBitmap& bmpDisabled,
                                wxItemKind kind,
                                wxObject *clientData,
                                const wxString& shortHelp,
                                const wxString& longHelp)
      : wxToolBarToolBase(tbar, id, label, bmpNormal, bmpDisabled, kind,
                          clientData, shortHelp, longHelp)
                          {
                          }

                          ocpnToolBarTool(ocpnToolBarSimple *tbar, wxControl *control)
      : wxToolBarToolBase(tbar, control)
                          {
                          }

                          void SetSize(const wxSize& size)
                          {
                                m_width = size.x;
                                m_height = size.y;
                          }

                          wxCoord GetWidth() const { return m_width; }
                          wxCoord GetHeight() const { return m_height; }

                          wxCoord m_x;
                          wxCoord m_y;
                          wxCoord m_width;
                          wxCoord m_height;
                          wxRect  trect;

                          bool b_hilite;

                          DECLARE_NO_COPY_CLASS(ocpnToolBarTool)
};

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(ocpnToolBarSimple, wxControl)

#if !wxUSE_TOOLBAR_NATIVE && !defined(__WXUNIVERSAL__)
            IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxToolBarSimple)
#endif

            BEGIN_EVENT_TABLE(ocpnToolBarSimple, wxToolBarBase)
            EVT_SIZE(ocpnToolBarSimple::OnSize)
            EVT_SCROLL(ocpnToolBarSimple::OnScroll)
            EVT_PAINT(ocpnToolBarSimple::OnPaint)
            EVT_KILL_FOCUS(ocpnToolBarSimple::OnKillFocus)
            EVT_MOUSE_EVENTS(ocpnToolBarSimple::OnMouseEvent)
            EVT_TIMER(TOOLTIPON_TIMER, ocpnToolBarSimple::OnToolTipTimerEvent)

            END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// tool bar tools creation
// ----------------------------------------------------------------------------

wxToolBarToolBase *ocpnToolBarSimple::CreateTool(int id,
                        const wxString& label,
                        const wxBitmap& bmpNormal,
                        const wxBitmap& bmpDisabled,
                        wxItemKind kind,
                        wxObject *clientData,
                        const wxString& shortHelp,
                        const wxString& longHelp)
{
      return new ocpnToolBarTool(this, id, label, bmpNormal, bmpDisabled,
                                     kind, clientData, shortHelp, longHelp);
}

wxToolBarToolBase *ocpnToolBarSimple::CreateTool(wxControl *control)
{
      return new ocpnToolBarTool(this, control);
}

// ----------------------------------------------------------------------------
// ocpnToolBarSimple creation
// ----------------------------------------------------------------------------

void ocpnToolBarSimple::Init()
{
      m_currentRowsOrColumns = 0;

      m_lastX =
                  m_lastY = 0;

      m_maxWidth =
                  m_maxHeight = 0;

      m_pressedTool =
                  m_currentTool = -1;

      m_xPos =
                  m_yPos = wxDefaultCoord;

      m_toolPacking = 1;
      m_toolSeparation = 5;
      m_xMargin = 4;
      m_yMargin = 4;

      m_defaultWidth = 16;
      m_defaultHeight = 15;

      m_xScrollPixelsPerLine = 1;
      m_yScrollPixelsPerLine = 1;
      m_xScrollingEnabled = false;
      m_yScrollingEnabled = false;
      m_xScrollPosition = 0;
      m_yScrollPosition = 0;
      m_xScrollLines = 0;
      m_yScrollLines = 0;
      m_xScrollLinesPerPage = 0;
      m_yScrollLinesPerPage = 0;

      m_toggle_bg_color = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
      m_toolOutlineColour.Set(_T("BLACK"));
      m_pToolTipWin = NULL;
      m_last_ro_tool = NULL;

}

wxToolBarToolBase *ocpnToolBarSimple::DoAddTool(int id,
                                              const wxString& label,
                                              const wxBitmap& bitmap,
                                              const wxBitmap& bmpDisabled,
                                              wxItemKind kind,
                                              const wxString& shortHelp,
                                              const wxString& longHelp,
                                              wxObject *clientData,
                                              wxCoord xPos,
                                              wxCoord yPos)
{
    // rememeber the position for DoInsertTool()
      m_xPos = xPos;
      m_yPos = yPos;

      return wxToolBarBase::DoAddTool(id, label, bitmap, bmpDisabled, kind,
                                      shortHelp, longHelp,
                                      clientData, xPos, yPos);
}

bool ocpnToolBarSimple::DoInsertTool(size_t WXUNUSED(pos),
                                   wxToolBarToolBase *toolBase)
{
      ocpnToolBarTool *tool = (ocpnToolBarTool *)toolBase;

//      if(tool->IsControl())
//            return false;

//      wxCHECK_MSG( !tool->IsControl(), false,
//                    _T("generic wxToolBarSimple doesn't support controls") );

      tool->m_x = m_xPos;
      if ( tool->m_x == wxDefaultCoord )
            tool->m_x = m_xMargin;

      tool->m_y = m_yPos;
      if ( tool->m_y == wxDefaultCoord )
            tool->m_y = m_yMargin;


      if ( tool->IsButton() )
      {
            tool->SetSize(GetToolSize());

        // Calculate reasonable max size in case Layout() not called
            if ((tool->m_x + tool->GetNormalBitmap().GetWidth() + m_xMargin) > m_maxWidth)
                  m_maxWidth = (wxCoord)((tool->m_x + tool->GetWidth() + m_xMargin));

            if ((tool->m_y + tool->GetNormalBitmap().GetHeight() + m_yMargin) > m_maxHeight)
                  m_maxHeight = (wxCoord)((tool->m_y + tool->GetHeight() + m_yMargin));
      }

      else if ( tool->IsControl() )
      {
            tool->SetSize(tool->GetControl()->GetSize());
      }

      tool->b_hilite = false;

      return true;
}

bool ocpnToolBarSimple::DoDeleteTool(size_t WXUNUSED(pos),
                                   wxToolBarToolBase *tool)
{
    // VZ: didn't test whether it works, but why not...
      tool->Detach();

      if(m_last_ro_tool == tool)
            m_last_ro_tool = NULL;

      Refresh(false);

      return true;
}

bool ocpnToolBarSimple::Create(wxWindow *parent,
                             wxWindowID id,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name)
{
      if ( !wxWindow::Create(parent, id, pos, size, style, name) )
            return false;

    // Set it to grey (or other 3D face colour)
      SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

      if ( GetWindowStyleFlag() & wxTB_VERTICAL )
      {
            m_lastX = 7;
            m_lastY = 3;

            m_maxRows = 32000;      // a lot
            m_maxCols = 1;
      }
      else
      {
            m_lastX = 3;
            m_lastY = 7;

            m_maxRows = 1;
            m_maxCols = 32000;      // a lot
      }

      SetCursor(*wxSTANDARD_CURSOR);

      m_tooltip_timer.SetOwner(this, TOOLTIPON_TIMER);

      return true;
}

ocpnToolBarSimple::~ocpnToolBarSimple()
{
      if(m_pToolTipWin)
      {
            m_pToolTipWin->Destroy();
            m_pToolTipWin = NULL;
      }

}

void ocpnToolBarSimple::SetColorScheme(ColorScheme cs)
{
      if(m_pToolTipWin)
      {
            m_pToolTipWin->Destroy();
            m_pToolTipWin = NULL;
      }

      m_toolOutlineColour = GetGlobalColor(_T("UIBDR"));

      m_currentColorScheme = cs;
}


bool ocpnToolBarSimple::Realize()
{
      m_currentRowsOrColumns = 0;
      m_lastX = m_xMargin;
      m_lastY = m_yMargin;
      m_maxWidth = 0;
      m_maxHeight = 0;

      int maxToolWidth = 0;
      int maxToolHeight = 0;

    // Find the maximum tool width and height
      wxToolBarToolsList::compatibility_iterator node = m_tools.GetFirst();
      while ( node )
      {
            ocpnToolBarTool *tool = (ocpnToolBarTool *)node->GetData();
            if(tool->IsButton())
            {
                  if ( tool->GetWidth() > maxToolWidth )
                        maxToolWidth = tool->GetWidth();
                  if (tool->GetHeight() > maxToolHeight)
                        maxToolHeight = tool->GetHeight();
            }
            node = node->GetNext();
      }

      int separatorSize = m_toolSeparation;

      node = m_tools.GetFirst();
      while ( node )
      {
            ocpnToolBarTool *tool = (ocpnToolBarTool *)node->GetData();
            if ( tool->IsSeparator() )
            {
                  if ( GetWindowStyleFlag() & wxTB_HORIZONTAL )
                  {
                        if (m_currentRowsOrColumns >= m_maxCols)
                              m_lastY += separatorSize;
                        else
                              m_lastX += separatorSize;
                  }
                  else
                  {
                        if (m_currentRowsOrColumns >= m_maxRows)
                              m_lastX += separatorSize;
                        else
                              m_lastY += separatorSize;
                  }
            }
            else if ( tool->IsButton() )
            {
                  if ( GetWindowStyleFlag() & wxTB_HORIZONTAL )
                  {
                        if (m_currentRowsOrColumns >= m_maxCols)
                        {
                              m_currentRowsOrColumns = 0;
                              m_lastX = m_xMargin;
                              m_lastY += maxToolHeight + m_toolPacking;
                        }
                        tool->m_x = (wxCoord)(m_lastX + (maxToolWidth - tool->GetWidth())/2.0);
                        tool->m_y = (wxCoord)(m_lastY );

                        tool->trect = wxRect(tool->m_x, tool->m_y, tool->GetWidth(), tool->GetHeight());
                        tool->trect.Inflate(m_toolPacking/2, m_yMargin);             ;
                        m_lastX += maxToolWidth + m_toolPacking;
                  }
                  else
                  {
                        if (m_currentRowsOrColumns >= m_maxRows)
                        {
                              m_currentRowsOrColumns = 0;
                              m_lastX += (maxToolWidth + m_toolPacking);
                              m_lastY = m_yMargin;
                        }
                        tool->m_x = (wxCoord)(m_lastX + (maxToolWidth - tool->GetWidth())/2.0);
                        tool->m_y = (wxCoord)(m_lastY + (maxToolHeight - tool->GetHeight())/2.0);

                        tool->trect = wxRect(tool->m_x, tool->m_y, tool->GetWidth(), tool->GetHeight());
                        tool->trect.Inflate(m_toolPacking/2, m_yMargin);             ;

                        m_lastY += maxToolHeight + m_toolPacking;
                  }
                  m_currentRowsOrColumns ++;
            }
            else if(tool->IsControl())
            {
                  tool->m_x = (wxCoord)(m_lastX );
                  tool->m_y = (wxCoord)(m_lastY - (m_yMargin / 2));           //TODO this is not exactly right, but it works.....

                  tool->trect = wxRect(tool->m_x, tool->m_y, tool->GetWidth(), tool->GetHeight());
                  tool->trect.Inflate(m_toolPacking/2, m_yMargin);             ;

                  wxSize s = tool->GetControl()->GetSize();
                  m_lastX += s.x + m_toolPacking;

           }

            if (m_lastX > m_maxWidth)
                  m_maxWidth = m_lastX;
            if (m_lastY > m_maxHeight)
                  m_maxHeight = m_lastY;

            node = node->GetNext();
      }

      if ( GetWindowStyleFlag() & wxTB_HORIZONTAL )
            m_maxHeight += maxToolHeight;
      else
            m_maxWidth += maxToolWidth;

      m_maxWidth += m_xMargin;
      m_maxHeight += m_yMargin;

      SetSize(m_maxWidth, m_maxHeight);

      return true;
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void ocpnToolBarSimple::OnPaint (wxPaintEvent& WXUNUSED(event))
{
      wxPaintDC dc(this);
      PrepareDC(dc);

      wxRegion ru = GetUpdateRegion();
      wxRect upRect = ru.GetBox();
//      printf("upRect:  %d %d %d %d\n", upRect.GetX(), upRect.GetY(), upRect.GetWidth(), upRect.GetHeight());
      static int count = 0;
    // Prevent reentry of OnPaint which would cause wxMemoryDC errors.
      if ( count > 0 )
            return;
      count++;

      for ( wxToolBarToolsList::compatibility_iterator node = m_tools.GetFirst();
            node;
            node = node->GetNext() )
      {
            wxToolBarToolBase *tool = node->GetData();
            ocpnToolBarTool *tools = (ocpnToolBarTool *) tool;
            wxRect toolRect = tools->trect;
//            printf("    toolRect:  %d %d %d %d\n", toolRect.GetX(), toolRect.GetY(), toolRect.GetWidth(), toolRect.GetHeight());

            if(toolRect.Intersects(upRect))
            {
//                  printf("draw\n");
                  if ( tool->IsButton() )
                  {
                        DrawTool(dc, tool);
                  }
                  else if ( tool->IsControl() )
                  {
                        if(tool->GetControl()->IsKindOf(CLASSINFO(wxStaticBitmap)))
                        {
                              wxStaticBitmap *psbm = (wxStaticBitmap *)tool->GetControl();
                              ocpnToolBarTool *toolsimp = (ocpnToolBarTool *)tool;
                              dc.DrawBitmap(psbm->GetBitmap(), toolsimp->m_x, toolsimp->m_y, false);
                        }
                  }
            }
      }

      count--;
}

void ocpnToolBarSimple::OnSize (wxSizeEvent& WXUNUSED(event))
{
      if (GetAutoLayout())
            Layout();

      AdjustScrollbars();
}

void ocpnToolBarSimple::OnKillFocus(wxFocusEvent& WXUNUSED(event))
{
      OnMouseEnter(m_pressedTool = m_currentTool = -1);
}

void ocpnToolBarSimple::OnToolTipTimerEvent(wxTimerEvent& event)
{
      if(m_pToolTipWin && (!m_pToolTipWin->IsShown()))
      {
            if(m_last_ro_tool)
            {
                  wxString s = m_last_ro_tool->GetShortHelp();

                  if(s.Len())
                  {
                        m_pToolTipWin->SetString(s);

                        m_pToolTipWin->SetPosition(wxPoint(m_last_ro_tool->m_x, 0));
                        m_pToolTipWin->SetBitmap();
//                        m_pToolTipWin->Refresh(false);
                        m_pToolTipWin->Show();
                  }
            }

            m_one_shot = 10;
      }
}


void ocpnToolBarSimple::OnMouseEvent(wxMouseEvent & event)
{
      if(event.Leaving())
      {
            if(m_pToolTipWin)
                  m_pToolTipWin->Hide();

            m_one_shot = 500;                   // inital value
      }

      if(event.Entering())
      {
            m_one_shot = 500;
      }


      wxCoord x, y;
      event.GetPosition(&x, &y);
      ocpnToolBarTool *tool = (ocpnToolBarTool *)FindToolForPosition(x, y);

      if (event.LeftDown())
      {
            CaptureMouse();
      }
      if (event.LeftUp())
      {
            if(HasCapture())
                  ReleaseMouse();
      }

      if(tool && tool->IsButton())
      {
            //    ToolTips
            if(NULL == m_pToolTipWin)
            {
                  m_pToolTipWin = new ToolTipWin(this->GetParent());
                  m_pToolTipWin->SetColorScheme(m_currentColorScheme);
                  m_pToolTipWin->Hide();
            }

            if(tool != m_last_ro_tool)
                  m_pToolTipWin->Hide();

            if(!m_pToolTipWin->IsShown())
            {
                  m_tooltip_timer.Start(m_one_shot, wxTIMER_ONE_SHOT);
            }

            //    Tool highlighting
            if(1)
            {
                  if(tool != m_last_ro_tool)
                  {
                        tool->b_hilite = true;
                        RefreshRect(tool->trect, false);
                        if(m_last_ro_tool)
                        {
                              m_last_ro_tool->b_hilite = false;
                              RefreshRect(m_last_ro_tool->trect, false);
                        }
                  }
            }



            m_last_ro_tool = tool;
      }
      else
      {
            //    Tooltips
            if(m_pToolTipWin && m_pToolTipWin->IsShown())
                  m_pToolTipWin->Hide();

            //    Highliting
            if(m_last_ro_tool)
            {
                  m_last_ro_tool->b_hilite = false;
                  RefreshRect(m_last_ro_tool->trect, false);

                  m_last_ro_tool = tool;
            }
      }






      if (!tool)
      {
            if (m_currentTool > -1)
            {
                  if (event.LeftIsDown())
                        SpringUpButton(m_currentTool);
                  m_currentTool = -1;
                  OnMouseEnter(-1);
            }
            return;
      }

      if (!event.IsButton())
      {
            if ( tool->GetId() != m_currentTool )
            {
            // If the left button is kept down and moved over buttons,
            // press those buttons.
                  if ( event.LeftIsDown() && tool->IsEnabled() )
                  {
                        SpringUpButton(m_currentTool);

                        if ( tool->CanBeToggled() )
                        {
                              tool->Toggle();
                        }

                        DrawTool(tool);
                  }

                  m_currentTool = tool->GetId();
                  OnMouseEnter(m_currentTool);
            }
            return;
      }

    // Left button pressed.
      if ( event.LeftDown() && tool->IsEnabled() )
      {
            if ( tool->CanBeToggled() )
            {
                  tool->Toggle();
            }

            DrawTool(tool);
      }
      else if (event.RightDown())
      {
            OnRightClick(tool->GetId(), x, y);
      }

    // Left Button Released.  Only this action confirms selection.
    // If the button is enabled and it is not a toggle tool and it is
    // in the pressed state, then raise the button and call OnLeftClick.
      //
      if ( event.LeftUp() && tool->IsEnabled() )
      {
        // Pass the OnLeftClick event to tool
            if ( !OnLeftClick(tool->GetId(), tool->IsToggled()) &&
                  tool->CanBeToggled() )
            {
            // If it was a toggle, and OnLeftClick says No Toggle allowed,
            // then change it back
                  tool->Toggle();
            }

//            DrawTool(tool);
      }
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

void ocpnToolBarSimple::DrawTool(wxToolBarToolBase *tool)
{
      wxClientDC dc(this);
      DrawTool(dc, tool);
}

void ocpnToolBarSimple::DrawTool(wxDC& dc, wxToolBarToolBase *toolBase)
{
      ocpnToolBarTool *tool = (ocpnToolBarTool *)toolBase;

      PrepareDC(dc);


      wxBitmap bitmap = tool->GetNormalBitmap();
      if (!bitmap.Ok())
            return;

      if ( !tool->IsToggled() )
      {
                                  {
                                        wxPen bg_pen;
                                        wxBrush bg_brush;

                                        if(tool->b_hilite)
                                        {
                                              bg_pen.SetColour(m_toolOutlineColour);
                                              bg_brush.SetColour(GetBackgroundColour());
                                        }
                                        else
                                        {
                                              bg_pen.SetColour(GetBackgroundColour());
                                              bg_brush.SetColour(GetBackgroundColour());
                                        }
                                        dc.SetPen(bg_pen);
                                        dc.SetBrush(bg_brush);
                                        dc.DrawRoundedRectangle(tool->m_x - (m_toolPacking/2), tool->m_y - (m_yMargin/2),
                                                    bitmap.GetWidth() + (m_toolPacking), bitmap.GetHeight() + (m_yMargin),
                                                               2);

                                        wxBitmap bmp = tool->GetDisabledBitmap();
                                        if(!bmp.IsOk())
                                              bmp = tool->GetNormalBitmap();
                                        dc.DrawBitmap(bmp, tool->m_x, tool->m_y, true);

                                  }

      }
      else if ( tool->IsToggled() )
      {
                  wxPen bg_pen;
                  wxBrush bg_brush;

                  if(tool->b_hilite)
                  {
                        bg_pen.SetColour(m_toolOutlineColour);
                        bg_brush.SetColour(GetBackgroundColour());
                  }
                  else
                  {
                        bg_pen.SetColour(m_toolOutlineColour);
                        bg_brush.SetColour(m_toggle_bg_color);

                        //    If the tool has a disabled state bitmap, then assum the enabled and disabled bitmaps
                        //    should be shown verbatim without decorations
                        if(tool->GetDisabledBitmap().IsOk())
                        {
                              bg_brush.SetColour(GetBackgroundColour());
                              bg_pen.SetColour(GetBackgroundColour());
                        }


                  }

                  dc.SetPen(bg_pen);
                  dc.SetBrush(bg_brush);
                  dc.DrawRoundedRectangle(tool->m_x - (m_toolPacking/2), tool->m_y - (m_yMargin/2),
                                          bitmap.GetWidth() + (m_toolPacking), bitmap.GetHeight() + (m_yMargin),
                                                      2);

                  dc.DrawBitmap(tool->GetNormalBitmap(), tool->m_x, tool->m_y, true);
      }
}

// ----------------------------------------------------------------------------
// toolbar geometry
// ----------------------------------------------------------------------------

void ocpnToolBarSimple::SetRows(int nRows)
{
      wxCHECK_RET( nRows != 0, _T("max number of rows must be > 0") );

      m_maxCols = (GetToolsCount() + nRows - 1) / nRows;

      AdjustScrollbars();
      Refresh();
}

wxToolBarToolBase *ocpnToolBarSimple::FindToolForPosition(wxCoord x,
            wxCoord y) const
{
      wxToolBarToolsList::compatibility_iterator node = m_tools.GetFirst();
      while (node)
      {
            ocpnToolBarTool *tool = (ocpnToolBarTool *)node->GetData();
//            printf("%d %d\n", tool->m_x, x);
            if ((x >= tool->m_x) && (y >= tool->m_y) &&
                 (x < (tool->m_x + tool->GetWidth())) &&
                 (y < (tool->m_y + tool->GetHeight())))
            {
                  return tool;
            }

            node = node->GetNext();
      }

      return (wxToolBarToolBase *)NULL;
}

// ----------------------------------------------------------------------------
// tool state change handlers
// ----------------------------------------------------------------------------

void ocpnToolBarSimple::DoEnableTool(wxToolBarToolBase *tool,
                                   bool WXUNUSED(enable))
{
      DrawTool(tool);
}

void ocpnToolBarSimple::DoToggleTool(wxToolBarToolBase *tool,
                                   bool WXUNUSED(toggle))
{
      DrawTool(tool);
}

void ocpnToolBarSimple::DoSetToggle(wxToolBarToolBase * WXUNUSED(tool),
                                  bool WXUNUSED(toggle))
{
    // nothing to do
}

// Okay, so we've left the tool we're in ... we must check if the tool we're
// leaving was a 'sprung push button' and if so, spring it back to the up
// state.
void ocpnToolBarSimple::SpringUpButton(int id)
{
      wxToolBarToolBase *tool = FindById(id);

      if ( tool && tool->CanBeToggled() )
      {
            if (tool->IsToggled())
                  tool->Toggle();

            DrawTool(tool);
      }
}

// ----------------------------------------------------------------------------
// scrolling implementation
// ----------------------------------------------------------------------------

/*
 * pixelsPerUnitX/pixelsPerUnitY: number of pixels per unit (e.g. pixels per text line)
 * noUnitsX/noUnitsY:        : no. units per scrollbar
 */
void ocpnToolBarSimple::SetScrollbars (int pixelsPerUnitX, int pixelsPerUnitY,
                                     int noUnitsX, int noUnitsY,
                                     int xPos, int yPos)
{
      m_xScrollPixelsPerLine = pixelsPerUnitX;
      m_yScrollPixelsPerLine = pixelsPerUnitY;
      m_xScrollLines = noUnitsX;
      m_yScrollLines = noUnitsY;

      int w, h;
      GetSize(&w, &h);

    // Recalculate scroll bar range and position
      if (m_xScrollLines > 0)
      {
            m_xScrollPosition = xPos;
            SetScrollPos (wxHORIZONTAL, m_xScrollPosition, true);
      }
      else
      {
            SetScrollbar(wxHORIZONTAL, 0, 0, 0, false);
            m_xScrollPosition = 0;
      }

      if (m_yScrollLines > 0)
      {
            m_yScrollPosition = yPos;
            SetScrollPos (wxVERTICAL, m_yScrollPosition, true);
      }
      else
      {
            SetScrollbar(wxVERTICAL, 0, 0, 0, false);
            m_yScrollPosition = 0;
      }
      AdjustScrollbars();
      Refresh();

#if 0 //def __WXMSW__
      ::UpdateWindow ((HWND) GetHWND());
#endif
}

void ocpnToolBarSimple::OnScroll(wxScrollEvent& event)
{
      int orient = event.GetOrientation();

      int nScrollInc = CalcScrollInc(event);
      if (nScrollInc == 0)
            return;

      if (orient == wxHORIZONTAL)
      {
            int newPos = m_xScrollPosition + nScrollInc;
            SetScrollPos(wxHORIZONTAL, newPos, true );
      }
      else
      {
            int newPos = m_yScrollPosition + nScrollInc;
            SetScrollPos(wxVERTICAL, newPos, true );
      }

      if (orient == wxHORIZONTAL)
      {
            if (m_xScrollingEnabled)
                  ScrollWindow(-m_xScrollPixelsPerLine * nScrollInc, 0, NULL);
            else
                  Refresh();
      }
      else
      {
            if (m_yScrollingEnabled)
                  ScrollWindow(0, -m_yScrollPixelsPerLine * nScrollInc, NULL);
            else
                  Refresh();
      }

      if (orient == wxHORIZONTAL)
      {
            m_xScrollPosition += nScrollInc;
      }
      else
      {
            m_yScrollPosition += nScrollInc;
      }

}

int ocpnToolBarSimple::CalcScrollInc(wxScrollEvent& event)
{
      int pos = event.GetPosition();
      int orient = event.GetOrientation();

      int nScrollInc = 0;
      if (event.GetEventType() == wxEVT_SCROLL_TOP)
      {
            if (orient == wxHORIZONTAL)
                  nScrollInc = - m_xScrollPosition;
            else
                  nScrollInc = - m_yScrollPosition;
      } else
            if (event.GetEventType() == wxEVT_SCROLL_BOTTOM)
            {
                  if (orient == wxHORIZONTAL)
                        nScrollInc = m_xScrollLines - m_xScrollPosition;
                  else
                        nScrollInc = m_yScrollLines - m_yScrollPosition;
            } else
                  if (event.GetEventType() == wxEVT_SCROLL_LINEUP)
                  {
                        nScrollInc = -1;
                  } else
                        if (event.GetEventType() == wxEVT_SCROLL_LINEDOWN)
                        {
                              nScrollInc = 1;
                        } else
                              if (event.GetEventType() == wxEVT_SCROLL_PAGEUP)
                              {
                                    if (orient == wxHORIZONTAL)
                                          nScrollInc = -GetScrollPageSize(wxHORIZONTAL);
                                    else
                                          nScrollInc = -GetScrollPageSize(wxVERTICAL);
                              } else
                                    if (event.GetEventType() == wxEVT_SCROLL_PAGEDOWN)
                                    {
                                          if (orient == wxHORIZONTAL)
                                                nScrollInc = GetScrollPageSize(wxHORIZONTAL);
                                          else
                                                nScrollInc = GetScrollPageSize(wxVERTICAL);
                                    } else
                                          if ((event.GetEventType() == wxEVT_SCROLL_THUMBTRACK) ||
                                               (event.GetEventType() == wxEVT_SCROLL_THUMBRELEASE))
                                          {
                                                if (orient == wxHORIZONTAL)
                                                      nScrollInc = pos - m_xScrollPosition;
                                                else
                                                      nScrollInc = pos - m_yScrollPosition;
                                          }

                                          if (orient == wxHORIZONTAL)
                                          {
                                                int w, h;
                                                GetClientSize(&w, &h);

                                                int nMaxWidth = m_xScrollLines*m_xScrollPixelsPerLine;
                                                int noPositions = (int) ( ((nMaxWidth - w)/(float)m_xScrollPixelsPerLine) + 0.5 );
                                                if (noPositions < 0)
                                                      noPositions = 0;

                                                if ( (m_xScrollPosition + nScrollInc) < 0 )
                                                      nScrollInc = -m_xScrollPosition; // As -ve as we can go
                                                else if ( (m_xScrollPosition + nScrollInc) > noPositions )
                                                      nScrollInc = noPositions - m_xScrollPosition; // As +ve as we can go

                                                return nScrollInc;
                                          }
                                          else
                                          {
                                                int w, h;
                                                GetClientSize(&w, &h);

                                                int nMaxHeight = m_yScrollLines*m_yScrollPixelsPerLine;
                                                int noPositions = (int) ( ((nMaxHeight - h)/(float)m_yScrollPixelsPerLine) + 0.5 );
                                                if (noPositions < 0)
                                                      noPositions = 0;

                                                if ( (m_yScrollPosition + nScrollInc) < 0 )
                                                      nScrollInc = -m_yScrollPosition; // As -ve as we can go
                                                else if ( (m_yScrollPosition + nScrollInc) > noPositions )
                                                      nScrollInc = noPositions - m_yScrollPosition; // As +ve as we can go

                                                return nScrollInc;
                                          }
}

// Adjust the scrollbars - new version.
void ocpnToolBarSimple::AdjustScrollbars()
{
      int w, h;
      GetClientSize(&w, &h);

    // Recalculate scroll bar range and position
      if (m_xScrollLines > 0)
      {
            int nMaxWidth = m_xScrollLines*m_xScrollPixelsPerLine;
            int newRange = (int) ( ((nMaxWidth)/(float)m_xScrollPixelsPerLine) + 0.5 );
            if (newRange < 0)
                  newRange = 0;

            m_xScrollPosition = wxMin(newRange, m_xScrollPosition);

        // Calculate page size i.e. number of scroll units you get on the
        // current client window
            int noPagePositions = (int) ( (w/(float)m_xScrollPixelsPerLine) + 0.5 );
            if (noPagePositions < 1)
                  noPagePositions = 1;

            SetScrollbar(wxHORIZONTAL, m_xScrollPosition, noPagePositions, newRange);
            SetScrollPageSize(wxHORIZONTAL, noPagePositions);
      }
      if (m_yScrollLines > 0)
      {
            int nMaxHeight = m_yScrollLines*m_yScrollPixelsPerLine;
            int newRange = (int) ( ((nMaxHeight)/(float)m_yScrollPixelsPerLine) + 0.5 );
            if (newRange < 0)
                  newRange = 0;

            m_yScrollPosition = wxMin(newRange, m_yScrollPosition);

        // Calculate page size i.e. number of scroll units you get on the
        // current client window
            int noPagePositions = (int) ( (h/(float)m_yScrollPixelsPerLine) + 0.5 );
            if (noPagePositions < 1)
                  noPagePositions = 1;

            SetScrollbar(wxVERTICAL, m_yScrollPosition, noPagePositions, newRange);
            SetScrollPageSize(wxVERTICAL, noPagePositions);
      }
}

// Prepare the DC by translating it according to the current scroll position
void ocpnToolBarSimple::PrepareDC(wxDC& dc)
{
      dc.SetDeviceOrigin(- m_xScrollPosition * m_xScrollPixelsPerLine, - m_yScrollPosition * m_yScrollPixelsPerLine);
}

void ocpnToolBarSimple::GetScrollPixelsPerUnit (int *x_unit, int *y_unit) const
{
      *x_unit = m_xScrollPixelsPerLine;
      *y_unit = m_yScrollPixelsPerLine;
}

int ocpnToolBarSimple::GetScrollPageSize(int orient) const
{
      if ( orient == wxHORIZONTAL )
            return m_xScrollLinesPerPage;
      else
            return m_yScrollLinesPerPage;
}

void ocpnToolBarSimple::SetScrollPageSize(int orient, int pageSize)
{
      if ( orient == wxHORIZONTAL )
            m_xScrollLinesPerPage = pageSize;
      else
            m_yScrollLinesPerPage = pageSize;
}

/*
 * Scroll to given position (scroll position, not pixel position)
 */
void ocpnToolBarSimple::Scroll (int x_pos, int y_pos)
{
      int old_x, old_y;
      ViewStart (&old_x, &old_y);
      if (((x_pos == wxDefaultCoord) || (x_pos == old_x)) && ((y_pos == wxDefaultCoord) || (y_pos == old_y)))
            return;

      if (x_pos != wxDefaultCoord)
      {
            m_xScrollPosition = x_pos;
            SetScrollPos (wxHORIZONTAL, x_pos, true);
      }
      if (y_pos != wxDefaultCoord)
      {
            m_yScrollPosition = y_pos;
            SetScrollPos (wxVERTICAL, y_pos, true);
      }
      Refresh();

#if 0 //def __WXMSW__
      UpdateWindow ((HWND) GetHWND());
#endif
}

void ocpnToolBarSimple::EnableScrolling (bool x_scroll, bool y_scroll)
{
      m_xScrollingEnabled = x_scroll;
      m_yScrollingEnabled = y_scroll;
}

void ocpnToolBarSimple::GetVirtualSize (int *x, int *y) const
{
      *x = m_xScrollPixelsPerLine * m_xScrollLines;
      *y = m_yScrollPixelsPerLine * m_yScrollLines;
}

// Where the current view starts from
void ocpnToolBarSimple::ViewStart (int *x, int *y) const
{
      *x = m_xScrollPosition;
      *y = m_yScrollPosition;
}


//-----------------------------------------------------------------------
//
//    Toolbar Tooltip window implementation
//
//-----------------------------------------------------------------------
BEGIN_EVENT_TABLE(ToolTipWin, wxWindow)
            EVT_PAINT(ToolTipWin::OnPaint)

            END_EVENT_TABLE()

// Define a constructor
ToolTipWin::ToolTipWin(wxWindow *parent):
            wxWindow(parent, wxID_ANY, wxPoint(0,0), wxSize(1,1), wxNO_BORDER)
{
      m_pbm = NULL;

      m_back_color = GetGlobalColor ( _T ( "UIBCK" ) );
      m_text_color = GetGlobalColor ( _T ( "UITX1" ) );

      SetBackgroundStyle(wxBG_STYLE_CUSTOM);
      SetBackgroundColour(m_back_color);
      Hide();
}

ToolTipWin::~ToolTipWin()
{
      delete m_pbm;
}

void ToolTipWin::SetColorScheme(ColorScheme cs)
{
      m_back_color = GetGlobalColor ( _T ( "UIBCK" ) );
      m_text_color = GetGlobalColor ( _T ( "UITX1" ) );
}

void ToolTipWin::SetBitmap()
{
      int h, w;

      wxClientDC cdc(GetParent());


      wxFont *plabelFont = pFontMgr->GetFont(_T("ToolTips"));
      cdc.GetTextExtent(m_string, &w, &h,  NULL, NULL, plabelFont);

      m_size.x = w + 8;
      m_size.y = h + 4;

      wxMemoryDC mdc;

      delete m_pbm;
      m_pbm = new wxBitmap( m_size.x, m_size.y, -1);
      mdc.SelectObject(*m_pbm);

      wxPen pborder(m_text_color);
      wxBrush bback(m_back_color);
      mdc.SetPen(pborder);
      mdc.SetBrush(bback);

      mdc.DrawRectangle( 0, 0, m_size.x, m_size.y);

      //    Draw the text
      mdc.SetFont(*plabelFont);
      mdc.SetTextForeground(m_text_color);
      mdc.SetTextBackground(m_back_color);

      mdc.DrawText(m_string, 4, 2);

      int parent_width;
      cdc.GetSize(&parent_width, NULL);
      if((m_position.x + m_size.x) > parent_width)
            SetSize(parent_width - m_size.x, 2, m_size.x, m_size.y);         // Dont overlap Right Hand Side
      else
            SetSize(m_position.x, 2, m_size.x, m_size.y);


}

void ToolTipWin::OnPaint(wxPaintEvent& event)
{
      int width, height;
      GetClientSize(&width, &height );
      wxPaintDC dc(this);

      if(m_string.Len())
      {
            wxMemoryDC mdc;
            mdc.SelectObject(*m_pbm);
            dc.Blit(0, 0, width, height, &mdc, 0,0);
      }
}


//    Application memory footprint management

int GetApplicationMemoryUse(void)
{
      int memsize = -1;
#ifdef __LINUX__

      //    Use a contrived ps command to get the virtual memory size associated with this process
#if 0
      wxExecute(_T("ps --no-headers -o vsize -C opencpn >/tmp/ocpn.mem"), wxEXEC_SYNC);
      wxTextFile f(_T("/tmp/ocpn.mem"));

      if(f.Open())
      {
            wxString s = f.GetFirstLine();
            long vtmp;
            if(s.ToLong(&vtmp))
                  memsize = vtmp;

            f.Close();
      }
#endif
      wxWindow *fWin = wxWindow::FindFocus();

      wxArrayString outputArray;
      wxString cmd(_T("ps --no-headers -o vsize "));
      unsigned long pid = wxGetProcessId();
      wxString cmd1;
      cmd1.Printf(_T("%ld"), pid);
      cmd += cmd1;
      wxExecute(cmd, outputArray);

      if(outputArray.GetCount())
      {
            wxString s = outputArray.Item(0);
            long vtmp;
            if(s.ToLong(&vtmp))
                  memsize = vtmp;
      }

      if(fWin)
            fWin->SetFocus();

#endif

      return memsize;
}


//               A helper function to check for proper parameters of anchor watch
//
double AnchorDistFix( double const d, double const AnchorPointMinDist, double const AnchorPointMaxDist)   //  pjotrc 2010.02.22
{
     if ( d >= 0.0 )
           if ( d < AnchorPointMinDist ) return AnchorPointMinDist;
           else if ( d > AnchorPointMaxDist ) return AnchorPointMaxDist;
           else return d;

     else         //if ( d < 0.0 )
           if ( d > -AnchorPointMinDist ) return -AnchorPointMinDist;
           else if ( d < -AnchorPointMaxDist ) return -AnchorPointMaxDist;
           else return d;
}
