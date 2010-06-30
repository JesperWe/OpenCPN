/******************************************************************************
 * $Id: chcanv.cpp,v 1.107 2010/06/25 02:03:34 bdbcat Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Chart Canvas
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register                                      *
 *   $EMAIL$                                                               *
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
 * $Log: chcanv.cpp,v $
 * Revision 1.107  2010/06/25 02:03:34  bdbcat
 * 624
 *
 * Revision 1.106  2010/06/21 02:03:43  bdbcat
 * 620
 *
 * Revision 1.105  2010/06/16 03:52:55  bdbcat
 * 615a
 *
 * Revision 1.104  2010/06/13 21:05:15  bdbcat
 * 613a
 *
 * Revision 1.103  2010/06/12 00:49:06  bdbcat
 * 611b
 *
 * Revision 1.102  2010/06/11 16:29:57  bdbcat
 * 611a
 *
 * Revision 1.101  2010/06/07 15:29:27  bdbcat
 * 607a
 *
 * Revision 1.100  2010/06/06 20:53:23  bdbcat
 * 606a
 *
 * Revision 1.99  2010/06/04 22:35:23  bdbcat
 * 604
 *
 * Revision 1.98  2010/05/27 18:59:31  bdbcat
 * 527a
 *
 * Revision 1.97  2010/05/26 22:14:33  bdbcat
 * 526a
 *
 * Revision 1.96  2010/05/26 22:06:11  bdbcat
 * 526a
 *
 * Revision 1.95  2010/05/25 01:54:58  bdbcat
 * Build 524b
 *
 * Revision 1.94  2010/05/25 01:16:00  bdbcat
 * Build 524b
 *
 * Revision 1.93  2010/05/23 23:26:15  bdbcat
 * Build 523a
 *
 * Revision 1.91  2010/05/20 19:04:11  bdbcat
 * Build 520
 *
 * Revision 1.90  2010/05/19 01:06:53  bdbcat
 * Build 518
 *
 * Revision 1.89  2010/05/15 04:00:32  bdbcat
 * Build 514
 *
 * Revision 1.88  2010/05/04 04:44:57  bdbcat
 * Build 504
 *
 * Revision 1.87  2010/05/04 01:33:52  bdbcat
 * Build 503
 *
 * Revision 1.86  2010/05/02 20:04:50  bdbcat
 * Build 502b
 *
 * Revision 1.85  2010/05/02 15:44:51  bdbcat
 * Always rebuild ChartStack on set VP in quilt mode.
 *
 * Revision 1.84  2010/05/02 03:03:46  bdbcat
 * Build 501
 *
 * Revision 1.83  2010/04/27 01:40:44  bdbcat
 * Build 426
 *
 * Revision 1.82  2010/04/16 00:46:51  bdbcat
 * Correct logic for right click in quilt mode.
 *
 * Revision 1.81  2010/04/15 23:52:19  bdbcat
 * Correct logic for high speed limit
 *
 * Revision 1.80  2010/04/15 15:51:27  bdbcat
 * Build 415.
 *
 * Revision 1.79  2010/04/01 20:17:37  bdbcat
 * 2.1.0 Build 331
 *
 * Revision 1.78  2010/03/29 03:28:25  bdbcat
 * 2.1.0 Beta Initial
 *
 * 2010=02-16 pjotrc
 * - show measured distance in meters below 1 cable
 *
 * 2010-02-15 pjotrc
 * - implement anchor watch
 *
 * 2010/02/07 17:30 pjotrc
 * - implement switching offf/on of AIS display
 *
 * 2010/02/01 17:30 pjotrc
 * - implement drawing of AIS AtoN
 *
 * 2010/01/31 16:07:00 pjotrc
 * - change icon shape for Class B AIS targets
 * - mark moored/anchored targets with blue dot
 * - skip displaying moored/anchored only if NavStatus says so explicitly
 * - mark lost targets according to IMO NAV 48-16
 * - mark selected targets according to IMO NAV 48-16
 *
 */


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers
#include "wx/image.h"
#include <wx/graphics.h>
#include <wx/sound.h>

#include "dychart.h"

#include <wx/listimpl.cpp>

#include "chcanv.h"

#include "routeman.h"
#include "navutil.h"
#include "concanv.h"
#include "thumbwin.h"
#include "chartdb.h"
#include "wvschart.h"
#include "chartimg.h"
#include "chart1.h"
#include "cutil.h"
#include "routeprop.h"
#include "tcmgr.h"
#include "cm93.h"                   // for chart outline draw
#include "grib.h"
#include "routemanagerdialog.h"

#ifdef USE_S57
#include "s57chart.h"               // for ArrayOfS57Obj
#include "s52plib.h"
#endif

#include "ais.h"

#ifdef OCPN_USE_PNGICONS
  #include "bitmaps/icons.h"
#endif

#ifdef __MSVC__
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

#ifndef __WXMSW__
#include <signal.h>
#include <setjmp.h>


extern struct sigaction sa_all;
extern struct sigaction sa_all_old;

extern sigjmp_buf           env;                    // the context saved by sigsetjmp();
#endif

// ----------------------------------------------------------------------------
// Useful Prototypes
// ----------------------------------------------------------------------------
extern bool G_FloatPtInPolygon ( MyFlPoint *rgpts, int wnumpts, float x, float y ) ;
extern void catch_signals(int signo);

extern ChartBase        *Current_Vector_Ch;
extern ChartBase        *Current_Ch;
extern double           gLat, gLon, gCog, gSog, gHdt;
extern double           vLat, vLon;
extern ChartDB          *ChartData;
extern bool             bDBUpdateInProgress;
extern ColorScheme      global_color_scheme;
extern bool             g_bHDxValid;


extern ConsoleCanvas    *console;

extern RouteList        *pRouteList;
extern MyConfig         *pConfig;
extern Select           *pSelect;
extern Routeman         *g_pRouteMan;
extern ThumbWin         *pthumbwin;
extern TCMgr            *ptcmgr;
extern Select           *pSelectTC;
extern Select           *pSelectAIS;
extern WayPointman      *pWayPointMan;
extern MarkProp         *pMarkPropDialog;
extern RouteProp        *pRoutePropDialog;
extern MarkInfo         *pMarkInfoDialog;
extern Track            *g_pActiveTrack;

extern RoutePoint       *pAnchorWatchPoint1;   // pjotrc 2010.02.15
extern RoutePoint       *pAnchorWatchPoint2;   // pjotrc 2010.02.15
extern double           AnchorPointMaxDist, AnchorPointMinDist;  // pjotrc 2010.02.15
extern bool             AnchorAlertOn1;  // pjotrc 2010.02.17
extern bool             AnchorAlertOn2;  // pjotrc 2010.02.17
extern wxString         g_AW1GUID;
extern wxString         g_AW2GUID;


extern bool             bDrawCurrentValues;
extern wxString         *pWVS_Locn;

#ifdef USE_S57
extern s52plib          *ps52plib;
#endif

extern bool             bGPSValid;
extern bool             g_bShowOutlines;
extern bool             g_bShowDepthUnits;
extern AIS_Decoder      *g_pAIS;
extern FontMgr         *pFontMgr;

//    AIS Global configuration
extern bool             g_bShowAIS; //pjotrc 2010.02.09
extern bool             g_bCPAMax;
extern double           g_CPAMax_NM;
extern bool             g_bCPAWarn;
extern double           g_CPAWarn_NM;
extern bool             g_bTCPA_Max;
extern double           g_TCPA_Max;
extern bool             g_bMarkLost;
extern double           g_MarkLost_Mins;
extern bool             g_bRemoveLost;
extern double           g_RemoveLost_Mins;
extern bool             g_bShowCOG;
extern double           g_ShowCOG_Mins;
extern bool             g_bShowTracks;
extern double           g_ShowTracks_Mins;
extern bool             g_bShowMoored;
extern double           g_ShowMoored_Kts;

extern bool             g_bNavAidShowRadarRings;
extern int              g_iNavAidRadarRingsNumberVisible;
extern float            g_fNavAidRadarRingsStep;
extern int              g_pNavAidRadarRingsStepUnits;
extern bool             g_bWayPointPreventDragging;
extern bool             g_bEnableZoomToCursor;

extern AISTargetAlertDialog    *g_pais_alert_dialog_active;
extern AISTargetQueryDialog    *g_pais_query_dialog_active;
extern int              g_ais_query_dialog_x, g_ais_query_dialog_y;

extern CM93DSlide       *pCM93DetailSlider;
extern bool             g_bShowCM93DetailSlider;
extern int              g_cm93detail_dialog_x, g_cm93detail_dialog_y;
extern int              g_cm93_zoom_factor;

extern bool             g_b_overzoom_x;                      // Allow high overzoom

extern bool             g_bUseGreenShip;

extern ChartCanvas      *cc1;


extern GRIBUIDialog     *g_pGribDialog;

extern int              g_grib_dialog_x, g_grib_dialog_y;
extern int              g_grib_dialog_sx, g_grib_dialog_sy;
extern wxString         g_grib_dir;

extern bool             g_bshow_overzoom_emboss;
extern int              g_n_ownship_meters;

extern wxPlatformInfo   *g_pPlatform;

extern bool             g_bUseRaster;
extern bool             g_bUseVector;
extern bool             g_bUseCM93;

extern bool             g_bCourseUp;
extern double           g_COGAvg;               // only needed for debug....

extern int              g_click_stop;
extern double           g_ownship_predictor_minutes;

extern ArrayOfInts      g_quilt_noshow_index_array;
extern ChartStack       *pCurrentStack;
extern RouteManagerDialog *pRouteManagerDialog;
extern bool              g_bquiting;
extern AISTargetListDialog *g_pAISTargetList;
extern wxString         g_sAIS_Alert_Sound_File;




//  TODO why are these static?
static int mouse_x;
static int mouse_y;
static bool mouse_leftisdown;


CPL_CVSID ( "$Id: chcanv.cpp,v 1.107 2010/06/25 02:03:34 bdbcat Exp $" );


//  These are xpm images used to make cursors for this class.
//  The relevant static identifying label is the same as the file name
//  e.g. down.xpm contains a line .....static const char *down[]....

//#ifndef USE_PNG_CURSOR

 #include "bitmaps/down.xpm"
 #include "bitmaps/up.xpm"
 #include "bitmaps/left.xpm"
 #include "bitmaps/right.xpm"
 #include "bitmaps/pencil.xpm"
//#endif

#ifndef USE_PNG_TIDESML
 #include "bitmaps/tidesml.xpm"
#endif

//    Constants for right click menus
enum
{
        ID_DEF_MENU_MAX_DETAIL =1,
        ID_DEF_MENU_SCALE_IN,
        ID_DEF_MENU_SCALE_OUT,
        ID_DEF_MENU_DROP_WP,
        ID_DEF_MENU_QUERY,
        ID_DEF_MENU_MOVE_BOAT_HERE,
        ID_DEF_MENU_GOTO_HERE,
        ID_DEF_MENU_CM93ZOOM,
        ID_DEF_MENU_ROUTEMANAGER,

        ID_WP_MENU_DELPOINT,
        ID_WP_MENU_PROPERTIES,
        ID_WP_MENU_DELETEALL,
        ID_RT_MENU_ACTIVATE,
        ID_RT_MENU_DEACTIVATE,
        ID_RT_MENU_INSERT,
        ID_RT_MENU_APPEND,
        ID_RT_MENU_DELETE,
        ID_RT_MENU_REVERSE,
        ID_RT_MENU_DELPOINT,
        ID_RT_MENU_ACTPOINT,
        ID_RT_MENU_DEACTPOINT,
        ID_RT_MENU_ACTNXTPOINT,
        ID_RT_MENU_REMPOINT,
        ID_RT_MENU_PROPERTIES,
        ID_RT_MENU_SENDTOGPS,
        ID_RT_MENU_EXPORT,
        ID_RT_MENU_DELETEALL,
        ID_WP_MENU_SET_ANCHORWATCH,             // pjotrc 2010.02.15
        ID_WP_MENU_CLEAR_ANCHORWATCH,           // pjotrc 2010.02.15
        ID_DEF_MENU_AISTARGETLIST,

        ID_RC_MENU_SCALE_IN,
        ID_RC_MENU_SCALE_OUT,
        ID_RC_MENU_ZOOM_IN,
        ID_RC_MENU_ZOOM_OUT,
        ID_RC_MENU_FINISH,
        ID_DEF_MENU_AIS_QUERY,
        ID_DEF_MENU_ACTIVATE_MEASURE,
        ID_DEF_MENU_DEACTIVATE_MEASURE,
        ID_TK_MENU_PROPERTIES,
        ID_TK_MENU_EXPORT,
        ID_TK_MENU_DELETE,
        ID_WP_MENU_ADDITIONAL_INFO,                  // toh, 2009.02.08

        ID_DEF_MENU_QUILTREMOVE

};


//------------------------------------------------------------------------------
//    Quilt Candidate Definition
//------------------------------------------------------------------------------
class QuiltCandidate
{
      public:
            QuiltCandidate(){ b_include = false; b_eclipsed = false;}

            int         dbIndex;
            int         ChartScale;
            bool        b_include;
            bool        b_eclipsed;

};

WX_DEFINE_SORTED_ARRAY(QuiltCandidate *, ArrayOfSortedQuiltCandidates);


int CompareScales(QuiltCandidate *qc1, QuiltCandidate *qc2)
{
      if(!ChartData)
            return 0;

      const ChartTableEntry &cte1 = ChartData->GetChartTableEntry(qc1->dbIndex);
      const ChartTableEntry &cte2 = ChartData->GetChartTableEntry(qc2->dbIndex);

      return cte1.GetScale() - cte2.GetScale();
}

//------------------------------------------------------------------------------
//    Patch Definition
//------------------------------------------------------------------------------
class QuiltPatch
{
      public:
            QuiltPatch(){ b_Valid = false; b_eclipsed = false;}
            int         dbIndex;
            wxRegion    ActiveRegion;
            int         ProjType;
            bool        b_Valid;
            bool        b_eclipsed;
};


WX_DECLARE_LIST(QuiltPatch, PatchList);

//------------------------------------------------------------------------------
//    Quilt Definition
//------------------------------------------------------------------------------
class Quilt
{
      public:

            Quilt();
            ~Quilt();

            void EnableRasterCharts(bool f){m_bEnableRaster = f;}
            void EnableVectorCharts(bool f){m_bEnableVector = f;}
            void EnableCM93Charts(bool f){m_bEnableCM93 = f;}

            bool Compose(const ViewPort &vp);

            ChartBase *GetFirstChart();
            ChartBase *GetNextChart();
            ChartBase *GetLargestScaleChart();
            ArrayOfInts GetQuiltIndexArray(void);
            bool IsQuiltDelta(void);
            bool IsChartQuiltable(int db_index);

            wxString GetQuiltDepthUnit(){ return m_quilt_depth_unit; }

            int GetnCharts(){ return m_PatchList.GetCount();}
            bool RenderQuiltRegionViewOnDC ( wxMemoryDC &dc, ViewPort &vp, wxRegion &chart_region, ScaleTypeEnum scale_type );

            ChartBase *GetChartAtPix(wxPoint p);
            int GetChartdbIndexAtPix(wxPoint p);
            void InvalidateAllQuiltPatchs(void);
            void Invalidate(void){ m_bcomposed = false; }
            void AdjustQuiltVP(ViewPort &vp_last, ViewPort &vp_proposed);

            wxRegion &GetFullQuiltRegion(void){ return m_covered_region; }

            void SetHiliteIndex(int index){ m_nHiLiteIndex = index; }
            void SetReferenceChart(int dbIndex){ m_refchart_dbIndex = dbIndex; }
            int  GetRefChartdbIndex(void){ return m_refchart_dbIndex; }
            int GetQuiltProj(void){ return m_quilt_proj; }
            double GetMaxErrorFactor(){ return m_max_error_factor; }
            double GetRefScale(){ return m_reference_scale; }
            double GetRefNativeScale();
            bool GetbPolyconic(){ return m_bquilt_polyconic; }

            ArrayOfInts GetCandidatedbIndexArray(bool from_ref_chart, bool exclude_user_hidden);
            ArrayOfInts GetExtendedStackIndexArray() { return m_extended_stack_array; }
            ArrayOfInts GetEclipsedStackIndexArray() { return m_eclipsed_stack_array; }

      private:
            wxRegion GetChartQuiltRegion(const ChartTableEntry &cte, ViewPort &vp);
            QuiltPatch *GetCurrentPatch();
            void EmptyCandidateArray(void);
            void SubstituteClearDC ( wxMemoryDC &dc, ViewPort &vp );

            bool              m_bEnableRaster;
            bool              m_bEnableVector;
            bool              m_bEnableCM93;

            wxRegion          m_covered_region;
            PatchList         m_PatchList;
            wxBitmap          *m_pBM;

            bool              m_bcomposed;
            wxPatchListNode   *cnode;
            bool              m_bbusy;
            int               m_quilt_proj;

            ArrayOfSortedQuiltCandidates        *m_pcandidate_array;
            ArrayOfInts       m_last_index_array;
            ArrayOfInts       m_index_array;
            ArrayOfInts       m_extended_stack_array;
            ArrayOfInts       m_eclipsed_stack_array;

            int               m_nHiLiteIndex;
            int               m_refchart_dbIndex;
            int               m_reference_scale;
            int               m_reference_type;
            bool              m_bneed_clear;
            wxRegion          m_back_region;
            wxString          m_quilt_depth_unit;
            double            m_max_error_factor;
            bool              m_bquilt_polyconic;
};

WX_DEFINE_LIST(PatchList);

//------------------------------------------------------------------------------
//    Quilt Implementation
//------------------------------------------------------------------------------
Quilt::Quilt()
{
      m_bEnableRaster = true;
      m_bEnableVector = false;;
      m_bEnableCM93 = false;

      m_reference_scale = 1;
      m_refchart_dbIndex = -1;
      m_reference_type = CHART_TYPE_UNKNOWN;

      cnode = NULL;

      m_pBM = NULL;
      m_bcomposed = false;
      m_bbusy = false;

      m_pcandidate_array = new ArrayOfSortedQuiltCandidates(CompareScales);
      m_nHiLiteIndex = -1;
      m_bquilt_polyconic = true;
}

Quilt::~Quilt()
{
      m_PatchList.DeleteContents(true);
      m_PatchList.Clear();

      EmptyCandidateArray();
      delete m_pcandidate_array;

      m_extended_stack_array.Clear();

      delete m_pBM;
}

bool Quilt::IsChartQuiltable(int db_index)
{
      const ChartTableEntry &ctei = ChartData->GetChartTableEntry(db_index);

//      bool btype_match = (m_reference_type == ctei.GetChartType()) | (m_reference_type == CHART_TYPE_UNKNOWN);
      bool bproj_match = (m_quilt_proj == ctei.GetChartProjectionType()) |
                  (m_bquilt_polyconic && (ctei.GetChartProjectionType() == PROJECTION_POLYCONIC));

      double skew_norm = ctei.GetChartSkew();
      if(skew_norm > 180.)
            skew_norm -= 360.;

      bool skew_match = fabs(skew_norm) <  1.;

      return (bproj_match & skew_match);
}

ArrayOfInts Quilt::GetCandidatedbIndexArray(bool from_ref_chart, bool exclude_user_hidden)
{
      ArrayOfInts ret;
      for( unsigned int ir=0 ; ir<m_pcandidate_array->GetCount() ; ir++)
      {
            QuiltCandidate *pqc = m_pcandidate_array->Item(ir);
            if(from_ref_chart)                                    // only add entries of smaller scale than ref scale
            {
                  if(pqc->ChartScale >= m_reference_scale)
                  {
                        // Search the no-show array
                        if(exclude_user_hidden)
                        {
                              bool b_noshow = false;
                              for(unsigned int i=0 ; i < g_quilt_noshow_index_array.GetCount() ; i++)
                              {
                                    if(g_quilt_noshow_index_array.Item(i) == pqc->dbIndex)        // chart is in the noshow list
                                    {
                                          b_noshow = true;
                                          break;
                                    }
                              }
                              if(!b_noshow)
                                    ret.Add(pqc->dbIndex);
                        }
                        else
                        {
                              ret.Add(pqc->dbIndex);
                        }
                  }
            }
            else
                  ret.Add(pqc->dbIndex);

      }
      return ret;
}

QuiltPatch *Quilt::GetCurrentPatch()
{
      if(cnode)
            return (cnode->GetData());
      else
            return NULL;
}

void Quilt::EmptyCandidateArray(void)
{
      for(unsigned int i=0 ; i < m_pcandidate_array->GetCount() ; i++)
      {
            delete m_pcandidate_array->Item(i);
      }

      m_pcandidate_array->Clear();

}

ChartBase *Quilt::GetFirstChart()
{
      if(!ChartData)
            return NULL;

      if(!ChartData->IsValid())               // This could happen during yield recursion from progress dialog during databse update
            return NULL;

      if(!m_bcomposed)
            return NULL;

      if(m_bbusy)
            return NULL;

      m_bbusy = true;
      ChartBase *pret = NULL;
      cnode = m_PatchList.GetFirst();
      if(cnode)
            pret = ChartData->OpenChartFromDB(cnode->GetData()->dbIndex, FULL_INIT);

      m_bbusy = false;
      return pret;
}

ChartBase *Quilt::GetNextChart()
{
      if(!ChartData)
            return NULL;

      if(!ChartData->IsValid())
            return NULL;

      if(m_bbusy)
            return NULL;

      m_bbusy = true;
      ChartBase *pret = NULL;
      cnode = cnode->GetNext();
      if(cnode)
            pret = ChartData->OpenChartFromDB(cnode->GetData()->dbIndex, FULL_INIT);

      m_bbusy = false;
      return pret;
}

ChartBase *Quilt::GetLargestScaleChart()
{
      if(!ChartData)
            return NULL;

      if(m_bbusy)
            return NULL;

      m_bbusy = true;
      ChartBase *pret = NULL;
      cnode = m_PatchList.GetLast();
      if(cnode)
            pret = ChartData->OpenChartFromDB(cnode->GetData()->dbIndex, FULL_INIT);

      m_bbusy = false;
      return pret;
}


wxRegion Quilt::GetChartQuiltRegion(const ChartTableEntry &cte, ViewPort &vp)
{
      //    If the chart has exactly one aux ply table, use it for finer region precision
      wxRegion chart_region;

      int nAuxPlyEntries = cte.GetnAuxPlyEntries();
      if(nAuxPlyEntries >= 1)
      {
            for(int ip=0 ; ip < nAuxPlyEntries ; ip++)
            {
                  float *pfp = cte.GetpAuxPlyTableEntry(ip);
                  int nAuxPly = cte.GetAuxCntTableEntry(ip);

                  wxRegion t_region = vp.GetVPRegion(nAuxPly, pfp, cte.GetScale());
                  if(!t_region.Empty())
                      chart_region.Union(t_region);
            }
      }

      else
      {
            int n_ply_entries = cte.GetnPlyEntries();
            float *pfp = cte.GetpPlyTable();

            if(n_ply_entries >= 3)                          // could happen with old database and some charts, e.g. SHOM 2381.kap
            {
                  wxRegion t_region = vp.GetVPRegion(n_ply_entries, pfp, cte.GetScale());
                  if(!t_region.Empty())
                      chart_region.Union(t_region);

            }
            else
                  chart_region = wxRegion(0, 0, vp.pix_width, vp.pix_height);
      }

      //    This super bad hack needs to be fixed by changing the the plypoints on cm93 composite,
      //    or opening the chart and asking for the current coverage region....
      //    If we don't do this, cm93 reports empty (invalid) region due to +/- 360 degree coverage declared in chart table...
      if(cte.GetChartType() == CHART_TYPE_CM93COMP)
            chart_region = wxRegion(vp.rv_rect/*0, 0, vp.pix_width, vp.pix_height*/);

      //    Another superbad hack....
      //    Super small scale charts like bluemarble.kap usually cross the prime meridian
      //    and Plypoints georef is problematic......
      //    So, force full screen coverage in the quilt
      if(cte.GetScale() > 90000000)
            chart_region = wxRegion(vp.rv_rect/*0, 0, vp.pix_width, vp.pix_height*/);

      //    Clip the region to the current viewport
//      chart_region.Intersect(0, 0, vp.pix_width, vp.pix_height);
      chart_region.Intersect(vp.rv_rect);

      if(chart_region.IsOk())
            return chart_region;
      else
            return wxRegion(0,0,100,100);
}

int Quilt::GetChartdbIndexAtPix(wxPoint p)
{
      if(m_bbusy)
            return NULL;

      m_bbusy = true;

      int ret = -1;

      wxPatchListNode *cnode = m_PatchList.GetFirst();
      while(cnode)
      {
            if(cnode->GetData()->ActiveRegion.Contains(p) == wxInRegion)
            {
                  ret = cnode->GetData()->dbIndex;
                  break;
            }
            else
                  cnode = cnode->GetNext();
      }

      m_bbusy = false;
      return ret;
}
ChartBase *Quilt::GetChartAtPix(wxPoint p)
{
      if(m_bbusy)
            return NULL;

      m_bbusy = true;

      //    The patchlist is organized from small to large scale.
      //    We generally will want the largest scale chart at this point, so
      //    walk the whole list.  The result will be the last one found, i.e. the largest scale chart.
      ChartBase *pret = NULL;
      wxPatchListNode *cnode = m_PatchList.GetFirst();
      while(cnode)
      {
            if(cnode->GetData()->ActiveRegion.Contains(p) == wxInRegion)
                  pret = ChartData->OpenChartFromDB(cnode->GetData()->dbIndex, FULL_INIT);
            cnode = cnode->GetNext();
      }

      m_bbusy = false;
      return pret;
}

void Quilt::InvalidateAllQuiltPatchs(void)
{
      if(m_bbusy)
            return;

      m_bbusy = true;
      m_bbusy = false;
      return;
}

ArrayOfInts Quilt::GetQuiltIndexArray(void)
{
      return m_index_array;

      ArrayOfInts ret;

      if(m_bbusy)
            return ret;

      m_bbusy = true;

      wxPatchListNode *cnode = m_PatchList.GetFirst();
      while(cnode)
      {
            ret.Add(cnode->GetData()->dbIndex);
            cnode = cnode->GetNext();
      }

      m_bbusy = false;

      return ret;
}

bool Quilt::IsQuiltDelta(void)
{
      if(m_last_index_array.GetCount() != m_index_array.GetCount())
            return true;

      for(unsigned int i=0 ; i < m_index_array.GetCount() ; i++)
            if(m_last_index_array.Item(i) != m_index_array.Item(i))
                  return true;

      return false;
}


void Quilt::AdjustQuiltVP(ViewPort &vp_last, ViewPort &vp_proposed)
{
      if(m_bbusy)
            return;

      ChartBase *pRefChart = GetLargestScaleChart();
      if(pRefChart)
            pRefChart->AdjustVP(vp_last, vp_proposed);
}

double Quilt::GetRefNativeScale()
{
      double ret_val = 1.0;
      if(ChartData)
      {
            ChartBase *pc = ChartData->OpenChartFromDB(m_refchart_dbIndex, FULL_INIT);
            if(pc)
                  ret_val = pc->GetNativeScale();
      }

      return ret_val;
}



bool Quilt::Compose(const ViewPort &vp_in)
{
      if(!ChartData)
            return false;

      if(m_bbusy)
            return false;

      ViewPort vp_local = vp_in;                   // need a non-const copy

      //    Get Reference Chart parameters
      m_quilt_proj = PROJECTION_MERCATOR;                   // default
      if(m_refchart_dbIndex >= 0)
      {
            const ChartTableEntry &cte_ref = ChartData->GetChartTableEntry(m_refchart_dbIndex);
            m_reference_scale = cte_ref.GetScale();
            m_reference_type = cte_ref.GetChartType();
            m_quilt_proj = ChartData->GetDBChartProj(m_refchart_dbIndex);

            if((PROJECTION_POLYCONIC == m_quilt_proj) && m_bquilt_polyconic)
                  m_quilt_proj = PROJECTION_MERCATOR;   // switch quilt to Mercator if allowing polyconic charts

      }

      //    Set up the vieport projection type
      vp_local.SetProjectionType(m_quilt_proj);

      //    Build an array of chart database indices of all charts for which the ViewPort center
      //    is on the chart, and whose type matches the ReferenceChart,

      //    They will be added to an auto-sorted array based on scale, largest first

      EmptyCandidateArray();
      m_extended_stack_array.Clear();

      int n_charts = 0;
      if(pCurrentStack)
            n_charts = pCurrentStack->nEntry;

      //    Walk the current ChartStack...
      for(int ics=0 ; ics < n_charts ; ics++)
      {
            int i = pCurrentStack->GetDBIndex(ics);
            m_extended_stack_array.Add(i);

            double chart_skew = ChartData->GetDBChartSkew(i);
            if(chart_skew > 180.)
                  chart_skew -= 360.;

            // only unskewed charts of the proper projection and type may be quilted....
            if((m_reference_type == ChartData->GetDBChartType(i)) &&
               (fabs(chart_skew) < 1.0) &&
                ((ChartData->GetDBChartProj(i) == m_quilt_proj) || (m_bquilt_polyconic && (ChartData->GetDBChartProj(i) == PROJECTION_POLYCONIC))))
            {
                  QuiltCandidate *qcnew = new QuiltCandidate;
                  qcnew->dbIndex = i;
                  qcnew->ChartScale = ChartData->GetDBChartScale(i);
                  m_pcandidate_array->Add(qcnew);
           }
      }

      //    For S57ENCs
      if(CHART_TYPE_S57 == m_reference_type)
      {
            double quilt_area = vp_local.pix_width * vp_local.pix_height;
            int n_all_charts = ChartData->GetChartTableEntries();

            LLBBox viewbox = vp_local.vpBBox;
            for(int i=0 ; i < n_all_charts ; i++)
            {
                  if(ChartData->GetDBChartType(i) == m_reference_type)
                  {
                        wxBoundingBox chart_box;
                        ChartData->GetDBBoundingBox(i, &chart_box);

                        bool b_add = false;
                        if((viewbox.Intersect( chart_box) != _OUT))
                               b_add = true;

                        if(b_add)
                        {
                              // Check to see if this chart is already in the candidate array...
                              bool b_exists = false;
                              for( unsigned int ir=0 ; ir<m_pcandidate_array->GetCount() ; ir++)
                              {
                                    QuiltCandidate *pqc = m_pcandidate_array->Item(ir);
                                    if(i == pqc->dbIndex)
                                    {
                                          b_exists = true;
                                          break;
                                    }
                              }

                              //    Add the chart only if the area is more than 10 % of the ViewPort screen size
                              if(!b_exists)
                              {
                                    const ChartTableEntry &cte = ChartData->GetChartTableEntry(i);
                                    wxRegion chart_region = GetChartQuiltRegion(cte, vp_local);
                                    if(!chart_region.Empty())
                                    {
                                          wxRect rect_ch = chart_region.GetBox();
                                          double chart_fractional_area = (rect_ch.GetWidth() * rect_ch.GetHeight()) / quilt_area;

                                          double chart_skew = ChartData->GetDBChartSkew(i);
                                          if(chart_skew > 180.)
                                                chart_skew -= 360.;

                                          if((chart_fractional_area > .10) &&
                                                            (fabs(chart_skew) < 1.0) &&
                                                            (ChartData->GetDBChartProj(i) == m_quilt_proj))

                                          {
                                                QuiltCandidate *qcnew = new QuiltCandidate;
                                                qcnew->dbIndex = i;
                                                qcnew->ChartScale = ChartData->GetDBChartScale(i);
                                                m_pcandidate_array->Add(qcnew);

                                                m_extended_stack_array.Add(i);
                                          }
                                    }
                              }
                        }
                  }
            }

            // Re sort the extended stack array on scale
            if(m_extended_stack_array.GetCount() > 1)
            {
                  int swap = 1;
                  int ti;
                  while(swap == 1)
                  {
                        swap = 0;
                        for(unsigned int is=0 ; is<m_extended_stack_array.GetCount()-1 ; is++)
                        {
                              const ChartTableEntry &m = ChartData->GetChartTableEntry(m_extended_stack_array.Item(is));
                              const ChartTableEntry &n = ChartData->GetChartTableEntry(m_extended_stack_array.Item(is+1));


                              if(n.GetScale() < m.GetScale())
                              {
                                    ti = m_extended_stack_array.Item(is);
                                    m_extended_stack_array.RemoveAt(is);
                                    m_extended_stack_array.Insert(ti, is+1);
                                    swap = 1;
                              }
                        }
                  }
            }
      }




/*
      for(unsigned int i=0 ; i<m_pcandidate_array->GetCount() ; i++)
      {
            QuiltCandidate *qc = m_pcandidate_array->Item(i);
            const ChartTableEntry &m = ChartData->GetChartTableEntry(qc->dbIndex);
            printf("Sorted candidate array chart scale: %d\n", m.GetScale());
      }
*/


#if 0
      //    Slight of hand here....
      //    Change the reference chart to the largest scale chart in the candidate array
      int max_scale = 1e8;
      for(unsigned int i=0 ; i<m_pcandidate_array->GetCount() ; i++)
      {
            QuiltCandidate *qc = m_pcandidate_array->Item(i);
            if(qc->ChartScale <= max_scale)
            {
                  m_refchart_dbIndex = qc->dbIndex;;
                  m_reference_scale = qc->ChartScale;

                  max_scale = qc->ChartScale;
            }
      }
#endif


      //    Using Region logic, and starting from the largest scale chart
      //    figuratively "draw" charts until the ViewPort window is completely quilted over
      //    Add only those charts whose scale is smaller than the "reference scale"
      wxRegion vp_region(vp_local.rv_rect);
      unsigned int ir;

      //    As ChartdB data is always in rectilinear space, region calculations need to be done with no VP rotation
      double saved_vp_rotation = vp_local.rotation;                      // save a copy
      vp_local.SetRotationAngle(0.);

      for( ir=0 ; ir<m_pcandidate_array->GetCount() ; ir++)
      {

            QuiltCandidate *pqc = m_pcandidate_array->Item(ir);

            const ChartTableEntry &cte = ChartData->GetChartTableEntry(pqc->dbIndex);
            if(cte.GetScale() >= m_reference_scale)
            {
                  //  If this chart appears in the no-show array, then simply include it, but
                  //  don't subtract its region when determining the smaller scale charts to include.....
                  bool b_in_noshow = false;
                  for(unsigned int ins=0 ; ins < g_quilt_noshow_index_array.GetCount() ; ins++)
                  {
                        if(g_quilt_noshow_index_array.Item(ins) == pqc->dbIndex)        // chart is in the noshow list
                        {
                              b_in_noshow = true;
                              break;
                        }
                  }

                  if(!b_in_noshow)
                  {
                  //    Check intersection
                        wxRegion vpu_region(vp_local.rv_rect);

                        wxRegion chart_region = GetChartQuiltRegion(cte, vp_local);
                        if(!chart_region.Empty())
                              vpu_region.Intersect(chart_region);

                        if(vpu_region.IsEmpty())
                              pqc->b_include = false;                     // skip this chart, no true overlap
                        else
                        {
                              pqc->b_include = true;
                              vp_region.Subtract(chart_region);          // adding this chart
                        }
                  }
                  else
                  {
                        pqc->b_include = true;
                  }

            }
            else
            {
                  pqc->b_include = false;                         // skip this chart, scale is too large
            }

            //    Here is a special case for CM93Composite, which covers the world....
            //    Adding needs to be deferred........
            if(cte.GetChartType() == CHART_TYPE_CM93COMP)
                  pqc->b_include = false;                         // skip for now

/// Don't break early if the quilt is S57 ENC
/// This will allow the overlay cells found in Euro IENC to be included
            if(CHART_TYPE_S57 != m_reference_type)
            {
                  if(vp_region.IsEmpty())                         // normal stop condition, quilt is full
                        break;
            }
      }


      //    Walk the candidate list again, marking "eclipsed" charts
      //    which at this point are the ones with b_include == false .AND. whose scale is strictly smaller than the ref scale
      //    Also, maintain the member list of same

      m_eclipsed_stack_array.Clear();

      for( ir=0 ; ir<m_pcandidate_array->GetCount() ; ir++)
      {
            QuiltCandidate *pqc = m_pcandidate_array->Item(ir);

            if(!pqc->b_include)
            {
                  const ChartTableEntry &cte = ChartData->GetChartTableEntry(pqc->dbIndex);
                  if(cte.GetScale() > m_reference_scale)
                  {
                        m_eclipsed_stack_array.Add(pqc->dbIndex);
                        pqc->b_eclipsed = true;
                  }
            }
      }

      //    Potentially add cm93 to the candidate array if the region is not yet fully covered
      if(!vp_region.IsEmpty())
      {
            //    Walk the current ChartStack to find cm93
            for(int ics=0 ; ics < n_charts ; ics++)
            {
                  int i = pCurrentStack->GetDBIndex(ics);
                  if(CHART_TYPE_CM93COMP == ChartData->GetDBChartType(i))
                  {
                        QuiltCandidate *qcnew = new QuiltCandidate;
                        qcnew->dbIndex = i;
                        qcnew->ChartScale = ChartData->GetDBChartScale(i);
                        m_pcandidate_array->Add(qcnew);
                        break;
                  }
            }
      }


      //    Check the list...if no charts are visible due to all being smaller than reference_scale,
      //    then make sure the smallest scale chart which has any true region intersection is visible anyway
      //    Also enable any other charts which are the same scale as the first one added
      bool b_vis = false;
      for(unsigned int i=0 ; i < m_pcandidate_array->GetCount() ; i++)
      {
            QuiltCandidate *pqc = m_pcandidate_array->Item(i);
            if(pqc->b_include)
            {
                  b_vis = true;
                  break;
            }
      }

      if(!b_vis && m_pcandidate_array->GetCount())
      {
            int add_scale = 0;

            for( int i=m_pcandidate_array->GetCount() - 1 ; i>= 0 ; i--)
            {
                  QuiltCandidate *pqc = m_pcandidate_array->Item(i);
                  const ChartTableEntry &cte = ChartData->GetChartTableEntry(pqc->dbIndex);

                  //    Don't add cm93 yet, it is always covering the quilt...
                  if(cte.GetChartType() == CHART_TYPE_CM93COMP)
                        continue;

            //    Check intersection
                  wxRegion vpck_region(vp_local.rv_rect);

                  wxRegion chart_region = GetChartQuiltRegion(cte, vp_local);
                  if(!chart_region.Empty())
                        vpck_region.Intersect(chart_region);

                  if(!vpck_region.IsEmpty())
                  {
                        if(add_scale)
                        {
                              if(add_scale == cte.GetScale())
                                    pqc->b_include = true;;
                        }
                        else
                        {
                              pqc->b_include = true;
                              add_scale = cte.GetScale();
                        }
                  }
            }
      }

#ifdef QUILT_TYPE_1

      //    Iterate thru the candidate list again, from smallest scale currently included to largest candidate
      //    tentatively adding charts to the quilt until an added chart does not change the quilt coverage...
      //    Further, do not add any chart that covers less than 10 % of the viewport....

      wxRegion rgn_cover;
      rgn_cover.Clear();
      bool b_start_add = false;
      double quilt_area = vp_local.pix_width * vp_local.pix_height;

      if(m_pcandidate_array->GetCount())
      {
            for(  int ic = m_pcandidate_array->GetCount() - 1 ; ic >= 0 ; ic--)
            {
                  QuiltCandidate *pqc = m_pcandidate_array->Item(ic);

                  if( pqc->b_include)                  // start looking at the smallest included to this point
                        b_start_add = true;

                  if(b_start_add)
                  {
                        const ChartTableEntry &cte = ChartData->GetChartTableEntry(pqc->dbIndex);

                        wxRegion chart_region = GetChartQuiltRegion(cte, vp_local);
                        if(!chart_region.Empty())
                        {
                              wxRect rect_ch = chart_region.GetBox();
                              double chart_fractional_area = (rect_ch.GetWidth() * rect_ch.GetHeight()) / quilt_area;

                              if(chart_fractional_area > .10)
                              {
                                    wxRegion rgn_temp = rgn_cover;
                                    rgn_cover.Union(chart_region);


                                    if(rgn_temp != rgn_cover)
                                          pqc->b_include = true;               // add this chart
                              }
                        }
                  }
            }
      }

#endif

      //    Finally, build a list of "patches" for the quilt.
      //    Smallest scale first, as this will be the natural drawing order

      m_PatchList.DeleteContents(true);
      m_PatchList.Clear();

      if(m_pcandidate_array->GetCount())
      {
            for(int i=m_pcandidate_array->GetCount() - 1 ; i>= 0 ; i--)
            {
                  QuiltCandidate *pqc = m_pcandidate_array->Item(i);

                  //    cm93 add has been deferred until here
                  //    so that it would not displace possible raster or ENCs of larger scale
                  const ChartTableEntry &m = ChartData->GetChartTableEntry(pqc->dbIndex);

                  if(m.GetChartType() == CHART_TYPE_CM93COMP)
                        pqc->b_include = true;              // force acceptance of this chart in quilt
                                                            // would not be in candidate array if not elected

                  if(pqc->b_include)
                  {
//                       const ChartTableEntry &m = ChartData->GetChartTableEntry(pqc->dbIndex);
//                       printf("First list Build i:%d   Chart Scale: %d\n", i, m.GetScale());

                        QuiltPatch *pqp = new QuiltPatch;
                        pqp->dbIndex = pqc->dbIndex;
                        pqp->ProjType = m.GetChartProjectionType();
                        pqp->b_Valid = true;

                        m_PatchList.Append(pqp);
                  }
            }
      }
      //    From here on out, the PatchList is usable...

#ifdef QUILT_TYPE_1
      //    Establish the quilt projection type
      m_quilt_proj = PROJECTION_MERCATOR;                   // default
      ChartBase *ppc = GetLargestScaleChart();
      if(ppc)
            m_quilt_proj = ppc->GetChartProjectionType();
#endif


      //    Walk the PatchList, marking any entries whose projection does not match the determined quilt projection
      for(unsigned int i=0 ; i < m_PatchList.GetCount() ; i++)
      {
            wxPatchListNode *pcinode = m_PatchList.Item(i);
            QuiltPatch *piqp = pcinode->GetData();
            if(piqp->ProjType == PROJECTION_POLYCONIC)
            {
                if(!m_bquilt_polyconic)
                        piqp->b_Valid = false;
            }
            else if((piqp->ProjType != m_quilt_proj) && (piqp->ProjType != PROJECTION_UNKNOWN))
                  piqp->b_Valid = false;
      }

      //    Walk the PatchList, marking any entries which appear in the noshow array
      for(unsigned int i=0 ; i < m_PatchList.GetCount() ; i++)
      {
            wxPatchListNode *pcinode = m_PatchList.Item(i);
            QuiltPatch *piqp = pcinode->GetData();
            for(unsigned int ins=0 ; ins < g_quilt_noshow_index_array.GetCount() ; ins++)
            {
                  if(g_quilt_noshow_index_array.Item(ins) == piqp->dbIndex)        // chart is in the noshow list
                  {
                        piqp->b_Valid = false;
                        break;
                  }
            }
      }

      //    If using chart database < 1.6
      //    Walk the list again, removing any skewed chart entries
      if(ChartData->GetVersion() < 16)
      {
            unsigned int il = 0;
            while(il < m_PatchList.GetCount())
            {
                  wxPatchListNode *pcinode = m_PatchList.Item(il);
                  QuiltPatch *piqp = pcinode->GetData();
                  ChartBase *pc = ChartData->OpenChartFromDB(piqp->dbIndex, FULL_INIT);

                  if(pc)
                  {
                        if(pc->GetChartSkew() != 0.0)
                        {
                              m_PatchList.DeleteNode(pcinode);
                              il = 0;           // restart the list walk
                        }
                        else
                              il++;
                  }
                  else
                        il++;
            }
      }

      //    Generate the final render regions for the patches, one by one, smallest to largest scale
      wxRegion vpu_region(vp_local.rv_rect);

      m_covered_region.Clear();

      for(unsigned int i=0 ; i < m_PatchList.GetCount() ; i++)
      {
            wxPatchListNode *pcinode = m_PatchList.Item(i);
            QuiltPatch *piqp = pcinode->GetData();

            if(!piqp->b_Valid)                         // skip invalid entries
                  continue;

            wxRegion vpr_region = vpu_region;

            //    Start with the chart's full region coverage.
            const ChartTableEntry &ctei = ChartData->GetChartTableEntry(piqp->dbIndex);
            vpr_region = GetChartQuiltRegion(ctei, vp_local);

            //fetch and subtract regions for all larger scale charts
            for(unsigned int k = i+1 ; k < m_PatchList.GetCount() ; k++)
            {
                  wxPatchListNode *pnode = m_PatchList.Item(k);
                  QuiltPatch *pqp = pnode->GetData();

                  if(!pqp->b_Valid)                         // skip invalid entries
                        continue;

                  const ChartTableEntry &cte = ChartData->GetChartTableEntry(pqp->dbIndex);
                  wxRegion larger_scale_chart_region = GetChartQuiltRegion(cte, vp_local);

/// In S57ENC quilts, do not subtract larger scale regions from smaller,
/// This allows exactly co-incident chart regions to both be included
/// This covers the case found in layered Euro IENC cells

                  if(CHART_TYPE_S57 != ctei.GetChartType() /*m_reference_type*/)
                        if(!vpr_region.Empty())
                              vpr_region.Subtract(larger_scale_chart_region);
            }

            //    Whatever is left in the vpr region must belong to the current target chart
            wxPatchListNode *pinode = m_PatchList.Item(i);
            QuiltPatch *pqpi = pinode->GetData();
            pqpi->ActiveRegion = vpr_region;

            //    Move the active region so that upper left is 0,0 in final render region
            pqpi->ActiveRegion.Offset(-vp_local.rv_rect.x, -vp_local.rv_rect.y);

            //    Could happen that a larger scale chart covers completely a smaller scale chart
            if(pqpi->ActiveRegion.IsEmpty())
                  pqpi->b_eclipsed = true;

            //    Update the next pass full region to remove the region just allocated
            if(!vpr_region.Empty())
                  vpu_region.Subtract(vpr_region);

            //    Maintain the present full quilt coverage region
            m_covered_region.Union(pqpi->ActiveRegion);
      }

      //    Restore temporary VP Rotation
      vp_local.SetRotationAngle(saved_vp_rotation);


      //    Walk the list again, removing any entries marked as eclipsed....
      unsigned int il = 0;
      while(il < m_PatchList.GetCount())
      {
            wxPatchListNode *pcinode = m_PatchList.Item(il);
            QuiltPatch *piqp = pcinode->GetData();
            if(piqp->b_eclipsed)
            {
                  //    Make sure that this chart appears in the eclipsed list...
                  //    This can happen when....
                  bool b_noadd = false;
                  for( unsigned int ir=0 ; ir<m_eclipsed_stack_array.GetCount() ; ir++)
                  {
                        if(piqp->dbIndex == m_eclipsed_stack_array.Item(ir))
                        {
                              b_noadd = true;
                              break;
                        }
                  }
                  if(!b_noadd)
                        m_eclipsed_stack_array.Add(piqp->dbIndex);


                  m_PatchList.DeleteNode(pcinode);
                  il = 0;           // restart the list walk
            }
//            else if(!piqp->b_valid)
//            {
//            }

            else
                  il++;
      }

      //    Mark the quilt to indicate need for background clear if the region is not fully covered
      m_bneed_clear = !vpu_region.IsEmpty();
      m_back_region = vpu_region;

      //    Finally, iterate thru the quilt and preload all of the required charts.
      //    For dynamic S57 SENC creation, this is where SENC creation happens first.....
      for( ir=0 ; ir<m_pcandidate_array->GetCount() ; ir++)
      {
            QuiltCandidate *pqc = m_pcandidate_array->Item(ir);
            ChartData->OpenChartFromDB(pqc->dbIndex, FULL_INIT);
      }


      //    Build and maintain the array of indexes in this quilt

      m_last_index_array = m_index_array;       //save the last one for delta checks

      m_index_array.Clear();

      //    The index array is to be built in reverse, largest scale first
      unsigned int kl = m_PatchList.GetCount();
      for(unsigned int k=0 ; k < kl ; k++)
      {
            wxPatchListNode *cnode = m_PatchList.Item((kl -k) -1);
            m_index_array.Add(cnode->GetData()->dbIndex);
            cnode = cnode->GetNext();
      }

      //    Walk the patch list again, checking the depth units
      //    If they are all the same, then the value is usable

      m_quilt_depth_unit = _T("");
      ChartBase *pc = ChartData->OpenChartFromDB(m_refchart_dbIndex, FULL_INIT);
      if(pc)
      {
            m_quilt_depth_unit =  pc->GetDepthUnits();

            if(pc->m_ChartFamily == CHART_FAMILY_VECTOR)
            {
                  int units = ps52plib->m_nDepthUnitDisplay;
                  switch(units)
                  {
                        case 0 : m_quilt_depth_unit = _("Feet"); break;
                        case 1 : m_quilt_depth_unit = _("Meters"); break;
                        case 2 : m_quilt_depth_unit = _("Fathoms"); break;
                  }
            }
      }


      for(unsigned int k = 0 ; k < m_PatchList.GetCount() ; k++)
      {
            wxPatchListNode *pnode = m_PatchList.Item(k);
            QuiltPatch *pqp = pnode->GetData();

            if(!pqp->b_Valid)                         // skip invalid entries
                  continue;

            ChartBase *pc = ChartData->OpenChartFromDB(pqp->dbIndex, FULL_INIT);
            if(pc)
            {
                  wxString du = pc->GetDepthUnits();

                  if(pc->m_ChartFamily == CHART_FAMILY_VECTOR)
                  {
                        int units = ps52plib->m_nDepthUnitDisplay;
                        switch(units)
                        {
                        case 0 : du = _("Feet"); break;
                        case 1 : du = _("Meters"); break;
                        case 2 : du = _("Fathoms"); break;
                        }
                  }
                  wxString dul = du.Lower();
                  wxString ml = m_quilt_depth_unit.Lower();

                  if(dul != ml)
                  {
                        //    Try all the odd cases
                        if(dul.StartsWith(_T("meters")) && ml.StartsWith(_T("meters")))
                            continue;
                        else if(dul.StartsWith(_T("metres")) && ml.StartsWith(_T("metres")))
                            continue;
                        else if(dul.StartsWith(_T("fathoms")) && ml.StartsWith(_T("fathoms")))
                            continue;

                        //    They really are different
                        m_quilt_depth_unit = _T("");
                        break;
                  }
            }
      }

     //    Walk the patch list again, checking the error factor

      m_max_error_factor = 0.;
      for(unsigned int k = 0 ; k < m_PatchList.GetCount() ; k++)
      {
            wxPatchListNode *pnode = m_PatchList.Item(k);
            QuiltPatch *pqp = pnode->GetData();

            if(!pqp->b_Valid)                         // skip invalid entries
                  continue;

            ChartBase *pc = ChartData->OpenChartFromDB(pqp->dbIndex, FULL_INIT);
            if(pc)
            {
                  m_max_error_factor = wxMax(m_max_error_factor, pc->Chart_Error_Factor);
            }
      }

/*  This was some debug code looking for inconsistent quilt
      //   Find the index into the extended array corresponding to the current db index
      int current_ref_stack_index = -1;
      for(unsigned int im=0 ; im < m_extended_stack_array.GetCount() ; im++)
      {
            int dbindex = m_extended_stack_array.Item(im);
            if(m_refchart_dbIndex == m_extended_stack_array.Item(im))
            {
                  current_ref_stack_index = im;
                  break;
            }
      }

      if(-1 == current_ref_stack_index)
            int yyp = 9;
*/

      m_bcomposed = true;
      return true;
}


bool Quilt::RenderQuiltRegionViewOnDC ( wxMemoryDC &dc, ViewPort &vp, wxRegion &chart_region, ScaleTypeEnum scale_type )
{

#ifdef ocpnUSE_DIBSECTION
      ocpnMemDC tmp_dc;
#else
      wxMemoryDC tmp_dc;
#endif

      if(!m_bcomposed)
            return false;
      if(GetnCharts() && !m_bbusy)
      {

                  //  Walk the quilt, drawing each chart from smallest scale to largest

            int ip = 0;
                  // Render the smallest scale chart on the temp_dc
            ChartBase *pch = GetFirstChart();


            bool rendered_base = false;
            while(pch && !rendered_base)
            {
                       pch->SetVPParms(vp);

                        QuiltPatch *pqp = GetCurrentPatch();
                        if(pqp->b_Valid)
                        {

#if 0
                              printf("\n");
                              wxRegionIterator upd ( pqp->ActiveRegion );
                              while ( upd )
                              {
                                    wxRect rect = upd.GetRect();
                                    printf("   zero chart  pch: %d %d %d %d %d\n",(int)pch, rect.x, rect.y, rect.width, rect.height);
                                    upd ++ ;
                              }
#endif

                              if(!chart_region.IsEmpty())
                              {
                                    wxRegion get_region = pqp->ActiveRegion;
                                    get_region.Intersect(chart_region);

                                    pch->RenderRegionViewOnDC(dc, vp, get_region, scale_type);

                              }
                              rendered_base = true;
                        }

                        pch = GetNextChart();
                        ip++;
            }

            //    Funny case here....On MSW, the dc is Ok(), even if no bitmap is mapped.....
            //    So, resort to checking rendered_base
            if(dc.Ok())
            {
                  if(!rendered_base)            // nothing rendered, nothing mapped.
                        SubstituteClearDC(dc, vp);

                  else if(m_bneed_clear)        // base chart is partial coverage, and no other charts are going to be rendered
                  {
                        wxRegionIterator upd ( m_back_region );
                        while ( upd )
                        {
                              wxRect rect = upd.GetRect();
                              dc.Blit(rect.x, rect.y, rect.width, rect.height, &dc, rect.x, rect.y, wxCLEAR);
                              upd ++ ;
                        }

                  }
            }



                  //  Render the rest of the quilt's larger scale charts onto a temp dc
                  // and blit the active region rectangles to to target dc, one-by-one

            while(pch)
            {
                        pch->SetVPParms(vp);

                        QuiltPatch *pqp = GetCurrentPatch();
                        if(pqp->b_Valid)
                        {
                              if(!chart_region.IsEmpty())
                              {
                                    wxRegion get_region = pqp->ActiveRegion;
                                    get_region.Intersect(chart_region);


                                    pch->RenderRegionViewOnDC(tmp_dc, vp, get_region, scale_type);

                                    wxRegionIterator upd ( get_region );
                                    while ( upd )
                                    {
                                          wxRect rect = upd.GetRect();
                                          dc.Blit(rect.x, rect.y, rect.width, rect.height, &tmp_dc, rect.x, rect.y, wxCOPY, true);
                                          upd ++ ;
                                    }

                                    tmp_dc.SelectObject(wxNullBitmap);
                              }
                        }

                        pch = GetNextChart();
                        ip++;
            }

            //    Highlighting....
            if(m_nHiLiteIndex >= 0)
            {
                  if(m_pBM)
                  {
                        if((m_pBM->GetWidth() != vp.rv_rect.width) || (m_pBM->GetHeight() != vp.rv_rect.height))
                        {
                              delete m_pBM;
                              m_pBM = NULL;
                        }
                  }

                  if(NULL == m_pBM)
                        m_pBM = new wxBitmap(vp.rv_rect.width, vp.rv_rect.height);

                  //    Walk the PatchList, looking for the target hilite index
                  for(unsigned int i=0 ; i < m_PatchList.GetCount() ; i++)
                  {
                        wxPatchListNode *pcinode = m_PatchList.Item(i);
                        QuiltPatch *piqp = pcinode->GetData();
                        if((m_nHiLiteIndex == piqp->dbIndex) && (piqp->b_Valid))      // found it
                        {
                              //    Copy the entire quilt to my scratch bm
                              wxMemoryDC q_dc;
                              q_dc.SelectObject(*m_pBM);
                              q_dc.Blit(0, 0, vp.rv_rect.width, vp.rv_rect.height, &dc, 0,0);
                              q_dc.SelectObject(wxNullBitmap);

                              wxRect box = piqp->ActiveRegion.GetBox();

                              //    Create a "mask" bitmap from the chart's region
                              //    WxGTK has an error in this method....Creates a color bitmap, not usable for mask creation
                              //    So, I clone with correction
//                              wxBitmap hl_mask_bm = piqp->ActiveRegion.ConvertToBitmap();
                              wxBitmap hl_mask_bm;
                              {
                                    wxRect box = piqp->ActiveRegion.GetBox();
                                    wxBitmap bmp(box.GetRight(), box.GetBottom(), 1);
                                    wxMemoryDC dc;
                                    dc.SelectObject(bmp);
                                    dc.SetBackground(*wxBLACK_BRUSH);
                                    dc.Clear();
                                    dc.SetClippingRegion(*wx_static_cast(const wxRegion *, &piqp->ActiveRegion));
                                    dc.SetBackground(*wxWHITE_BRUSH);
                                    dc.Clear();
                                    dc.SelectObject(wxNullBitmap);

                                    hl_mask_bm = bmp;
                              }

                              if(hl_mask_bm.IsOk())
                              {
                                    wxMask *phl_mask = new wxMask(hl_mask_bm);
                                    m_pBM->SetMask(phl_mask);
                                    q_dc.SelectObject(*m_pBM);

                                    // Create another mask, dc and bitmap for red-out
                                    wxBitmap rbm(vp.rv_rect.width, vp.rv_rect.height);
                                    wxMask *pr_mask = new wxMask(hl_mask_bm);
                                    wxMemoryDC rdc;
                                    rbm.SetMask(pr_mask);
                                    rdc.SelectObject(rbm);
                                    unsigned char hlcolor = 255;
                                    switch(global_color_scheme)
                                    {
                                          case GLOBAL_COLOR_SCHEME_DAY:
                                                hlcolor = 255;
                                                break;
                                          case GLOBAL_COLOR_SCHEME_DUSK:
                                                hlcolor = 64;
                                                break;
                                          case GLOBAL_COLOR_SCHEME_NIGHT:
                                                hlcolor = 16;
                                                break;
                                          default:
                                                hlcolor = 255;
                                                break;
                                    }

                                    rdc.SetBackground(wxBrush(wxColour(hlcolor,0,0)));
                                    rdc.Clear();

                                    //  Blit the mask area of the quilt onto the red-out
                                    rdc.Blit(box.x, box.y, box.width, box.height, &q_dc, box.x, box.y, wxOR, true);

                                    //  And then blit the red-out onto the target
                                    q_dc.Blit(box.x, box.y, box.width, box.height, &rdc, box.x, box.y, wxCOPY, true);
                                    q_dc.SelectObject(wxNullBitmap);
                                    m_pBM->SetMask(NULL);

                                    //    Select the scratch BM as the return dc contents
                                    dc.SelectObject(*m_pBM);

                                    //    Clear the rdc
                                    rdc.SelectObject(wxNullBitmap);


                              }
                        }
                  }
            }


            if(!dc.Ok())                  // some error, probably bad charts, to be disabled on next compose
            {
                  SubstituteClearDC(dc, vp);
/*
                  dc.SelectObject(*m_pBM);
                  dc.SetBackground(*wxBLACK_BRUSH);
                  dc.Clear();
                  m_covered_region.Clear();
*/
            }

            return true;
      }
      else              // no charts yet, or busy....
      {
            SubstituteClearDC(dc, vp);
/*
            dc.SelectObject(*m_pBM);
            dc.SetBackground(*wxBLACK_BRUSH);
            dc.Clear();
            m_covered_region.Clear();
*/
            return true;                // no quilt yet...
      }
}


void Quilt::SubstituteClearDC ( wxMemoryDC &dc, ViewPort &vp )
{
      if(m_pBM)
      {
            if((m_pBM->GetWidth() != vp.rv_rect.width) || (m_pBM->GetHeight() != vp.rv_rect.height))
            {
                  delete m_pBM;
                  m_pBM = NULL;
            }
      }

      if(NULL == m_pBM)
      {
            m_pBM = new wxBitmap(vp.rv_rect.width, vp.rv_rect.height);
      }

      dc.SelectObject(wxNullBitmap);
      dc.SelectObject(*m_pBM);
      dc.SetBackground(*wxBLACK_BRUSH);
      dc.Clear();
      m_covered_region.Clear();

}


//------------------------------------------------------------------------------
//    ViewPort Implementation
//------------------------------------------------------------------------------
wxPoint ViewPort::GetMercatorPixFromLL(double lat, double lon) const
{
      double easting, northing;
      double xlon = lon;

     /*  Make sure lon and lon0 are same phase */
      if(xlon * clon < 0.)
      {
            if(xlon < 0.)
                 xlon += 360.;
            else
                 xlon -= 360.;
      }

      if(fabs(xlon - clon) > 180.)
      {
            if(xlon > clon)
                xlon -= 360.;
            else
                xlon += 360.;
      }

      if(PROJECTION_TRANSVERSE_MERCATOR == m_projection_type)
      {
            double tmeasting, tmnorthing;
            double tmceasting, tmcnorthing;
            toTM(clat, clon, clat, clon, &tmceasting, &tmcnorthing);
            toTM(lat, xlon, clat, clon, &tmeasting, &tmnorthing);

            tmeasting -= tmceasting;
            tmnorthing -= tmcnorthing;

            northing = tmnorthing;
            easting = tmeasting;
      }
      else
            toSM(lat, xlon, clat, clon, &easting, &northing);


      if(!wxFinite(easting) || !wxFinite(northing))
            return wxPoint(0,0);

      double epix = easting  * view_scale_ppm;
      double npix = northing * view_scale_ppm;

      double dx = epix * cos ( skew ) + npix * sin ( skew );
      double dy = npix * cos ( skew ) - epix * sin ( skew );

      //    Apply VP Rotation
      double dxr = dx;
      double dyr = dy;

      if(g_bCourseUp)
      {
            dxr = dx * cos ( rotation ) + dy * sin ( rotation );
            dyr = dy * cos ( rotation ) - dx * sin ( rotation );
      }

      double angle = skew;
      if(g_bCourseUp)
            angle += rotation;

      dxr = epix * cos ( angle ) + npix * sin ( angle );
      dyr = npix * cos ( angle ) - epix * sin ( angle );

      wxPoint r;
      //    We definitely need a round() function here
      r.x = ( int ) wxRound ( ( pix_width  / 2 ) + dxr );
      r.y = ( int ) wxRound ( ( pix_height / 2 ) - dyr );

      return r;
}

void ViewPort::GetMercatorLLFromPix(const wxPoint &p, double *lat, double *lon)
{
      //    Use SM estimator
      int dx = p.x - (pix_width  / 2 );
      int dy = ( pix_height / 2 ) - p.y;

      double xpr = dx;
      double ypr = dy;
      //    Apply VP Rotation
      if(g_bCourseUp)
      {
            xpr = ( dx * cos ( rotation ) ) - ( dy * sin ( rotation ) );
            ypr = ( dy * cos ( rotation ) ) + ( dx * sin ( rotation ) );
      }

      double xp = ( xpr * cos ( skew ) ) - ( ypr * sin ( skew ) );
      double yp = ( ypr * cos ( skew ) ) + ( xpr * sin ( skew ) );

      double d_east = xp / view_scale_ppm;
      double d_north = yp / view_scale_ppm;

      //TODO  This could be fromSM_ECC to better match some Raster charts
      //      However, it seems that cm93 (and S57) prefer no eccentricity correction
      //      Think about it....
      double slat, slon;
      if(PROJECTION_TRANSVERSE_MERCATOR == m_projection_type)
            fromTM ( d_east, d_north, clat, clon, &slat, &slon );
      else
            fromSM ( d_east, d_north, clat, clon, &slat, &slon );


      *lat = slat;

      if(slon < -180.)
            slon += 360.;
      else if(slon > 180.)
            slon -= 360.;
      *lon = slon;
}


wxRegion ViewPort::GetVPRegion( size_t n, float *llpoints, int chart_native_scale, wxPoint *ppoints )
{
      //    If the viewpoint is highly overzoomed wrt to chart native scale, the resulting region may be huge.
      //    This can be very expensive, and lead to crashes on some platforms (gtk in particular)
      //    So, look for this case and handle appropriately


      if(chart_scale < chart_native_scale / 10)
      {
            //    Make a positive definite vp
            ViewPort vp_positive = *this;
            while(vp_positive.vpBBox.GetMinX() < 0)
            {
                  vp_positive.clon += 360.;
                  wxPoint2DDouble t(360., 0.);
                  vp_positive.vpBBox.Translate(t);
            }


            //    Scan the points one-by-one, so that we can get min/max
            float *pfp = llpoints;
            float lon_max = -10000.;
            float lon_min =  10000.;
            float lat_max = -10000.;
            float lat_min =  10000.;

            for(unsigned int ip=0 ; ip < n ; ip++)
            {
                  lon_max = wxMax(lon_max, pfp[1]);
                  lon_min = wxMin(lon_min, pfp[1]);
                  lat_max = wxMax(lat_max, pfp[0]);
                  lat_min = wxMin(lat_min, pfp[0]);

                  pfp+=2;
            }

            wxBoundingBox chart_box(lon_min, lat_min, lon_max, lat_max);

            //    Case:  vpBBox is completely outside the chart box, or vice versa
            //    Return an empty region
            if(_OUT == chart_box.Intersect((wxBoundingBox&)vp_positive.vpBBox))
            {
                  if(_OUT == chart_box.Intersect((wxBoundingBox&)vpBBox))
                  {
                        return wxRegion();
                  }
            }

            //    Case:  vpBBox is completely inside the chart box
            if(_IN == chart_box.Intersect((wxBoundingBox&)vp_positive.vpBBox))
            {
//                  return wxRegion(0,0,pix_width, pix_height);
                  return wxRegion(rv_rect);
            }

            //    The ViewPort and the chart region overlap in some way....
            //    Simple solution is to create a full screen region and call it good enough.....
            //    And it will be good enough most of the time when severely overzoomed.

            //    TODO A better solution would involve converting and bounding the points, perhaps using some scaling,
            //    and then get the exact intersection
            return wxRegion(0,0,pix_width, pix_height);

      }

      //    More "normal" case

      wxPoint *pp;

      //    Use the passed point buffer if available
      if(ppoints == NULL)
            pp = new wxPoint[n];
      else
            pp = ppoints;

      float *pfp = llpoints;

      for(unsigned int ip=0 ; ip < n ; ip++)
      {
            wxPoint p = GetMercatorPixFromLL(pfp[0], pfp[1]);
            pp[ip] = p;
            pfp+=2;
      }


#ifdef __WXGTK__
      sigaction(SIGSEGV, NULL, &sa_all_old);             // save existing action for this signal

      struct sigaction temp;
      sigaction(SIGSEGV, NULL,  &temp);             // inspect existing action for this signal

      temp.sa_handler = catch_signals ;             // point to my handler
      sigemptyset(&temp.sa_mask);                  // make the blocking set
                                                        // empty, so that all
                                                        // other signals will be
                                                        // unblocked during my handler
      temp.sa_flags = 0;
      sigaction(SIGSEGV, &temp, NULL);

      if(sigsetjmp(env, 1))             //  Something in the below code block faulted....
      {
            sigaction(SIGSEGV, &sa_all_old, NULL);        // reset signal handler

            return wxRegion(0,0,pix_width, pix_height);

      }

      else
      {
            wxRegion r = wxRegion(n, pp);
            if(NULL == ppoints)
                  delete[] pp;

            sigaction(SIGSEGV, &sa_all_old, NULL);        // reset signal handler
            return r;
      }

#else
      wxRegion r = wxRegion(n, pp);

      if(NULL == ppoints)
            delete[] pp;
      return r;


#endif
}




//------------------------------------------------------------------------------
//    ChartCanvas Implementation
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE ( ChartCanvas, wxWindow )
        EVT_PAINT ( ChartCanvas::OnPaint )
        EVT_ACTIVATE ( ChartCanvas::OnActivate )
        EVT_SIZE ( ChartCanvas::OnSize )
        EVT_MOUSE_EVENTS ( ChartCanvas::MouseEvent )
        EVT_TIMER ( RESCALE_TIMER, ChartCanvas::RescaleTimerEvent )
        EVT_TIMER ( PAN_TIMER, ChartCanvas::PanTimerEvent )
        EVT_TIMER ( CURTRACK_TIMER, ChartCanvas::OnCursorTrackTimerEvent )
        EVT_TIMER ( ROT_TIMER, ChartCanvas::RotateTimerEvent )
        EVT_IDLE ( ChartCanvas::OnIdleEvent )
        EVT_CHAR(ChartCanvas::OnChar )
        EVT_MOUSE_CAPTURE_LOST(ChartCanvas::LostMouseCapture )


        EVT_MENU ( ID_DEF_MENU_MAX_DETAIL,         ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_DEF_MENU_SCALE_IN,           ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_DEF_MENU_SCALE_OUT,          ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_DEF_MENU_QUERY,              ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_DEF_MENU_DROP_WP,            ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_DEF_MENU_MOVE_BOAT_HERE,     ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_DEF_MENU_GOTO_HERE,          ChartCanvas::PopupMenuHandler )

        EVT_MENU ( ID_RT_MENU_ACTIVATE,     ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_RT_MENU_DEACTIVATE,   ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_RT_MENU_INSERT,       ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_RT_MENU_APPEND,       ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_RT_MENU_DELETE,       ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_RT_MENU_REVERSE,      ChartCanvas::PopupMenuHandler )

        EVT_MENU ( ID_RT_MENU_DELPOINT,     ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_RT_MENU_REMPOINT,     ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_RT_MENU_ACTPOINT,     ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_RT_MENU_DEACTPOINT,   ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_RT_MENU_ACTNXTPOINT,  ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_RT_MENU_PROPERTIES,   ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_RT_MENU_EXPORT,       ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_RT_MENU_DELETEALL,    ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_WP_MENU_SET_ANCHORWATCH,    ChartCanvas::PopupMenuHandler )    // pjotrc 2010.02.15
        EVT_MENU ( ID_WP_MENU_CLEAR_ANCHORWATCH,  ChartCanvas::PopupMenuHandler )    // pjotrc 2010.02.15
        EVT_MENU ( ID_RT_MENU_SENDTOGPS,    ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_DEF_MENU_AISTARGETLIST,     ChartCanvas::PopupMenuHandler )

        EVT_MENU ( ID_RC_MENU_SCALE_IN,     ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_RC_MENU_SCALE_OUT,    ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_RC_MENU_ZOOM_IN,      ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_RC_MENU_ZOOM_OUT,     ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_RC_MENU_FINISH,       ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_DEF_MENU_AIS_QUERY,   ChartCanvas::PopupMenuHandler )

        EVT_MENU ( ID_DEF_MENU_ACTIVATE_MEASURE,   ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_DEF_MENU_DEACTIVATE_MEASURE, ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_DEF_MENU_CM93ZOOM,           ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_DEF_MENU_ROUTEMANAGER,       ChartCanvas::PopupMenuHandler )

        EVT_MENU ( ID_WP_MENU_DELPOINT,           ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_WP_MENU_PROPERTIES,         ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_WP_MENU_DELETEALL,          ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_WP_MENU_ADDITIONAL_INFO,    ChartCanvas::PopupMenuHandler )   // toh, 2009.02.08

        EVT_MENU ( ID_TK_MENU_EXPORT,           ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_TK_MENU_PROPERTIES,       ChartCanvas::PopupMenuHandler )
        EVT_MENU ( ID_TK_MENU_DELETE,           ChartCanvas::PopupMenuHandler )

        EVT_MENU ( ID_DEF_MENU_QUILTREMOVE,     ChartCanvas::PopupMenuHandler )

END_EVENT_TABLE()


// Define a constructor for my canvas
ChartCanvas::ChartCanvas ( wxFrame *frame ) :
                wxWindow ( frame, wxID_ANY,    wxPoint ( 20,20 ), wxSize ( 5,5 ), wxSIMPLE_BORDER )
{
        parent_frame = ( MyFrame * ) frame;       // save a pointer to parent

        SetBackgroundColour ( GetGlobalColor ( _T ( "NODTA" ) ) );
        SetBackgroundStyle ( wxBG_STYLE_CUSTOM );  // on WXMSW, this prevents flashing on color scheme change

        m_bDrawingRoute = false;
        m_bRouteEditing = false;
        m_bMarkEditing = false;
        m_bFollow = false;
        m_bTCupdate = false;
        m_bAppendingRoute = false;          // was true in MSW, why??
        pThumbDIBShow = NULL;
        m_bShowCurrent = false;
        m_bShowTide = false;
        bShowingCurrent = false;
        pCwin = NULL;
        warp_flag = false;
        pss_overlay_bmp = NULL;
        pss_overlay_mask = NULL;
        m_bBackRender = false;
        m_bbr_paused = false;
        m_bChartDragging = false;
        m_bMeasure_Active = false;
        m_pMeasureRoute = NULL;
        m_pPopUpWin = NULL;
        m_pCIWin = NULL;
        m_RescaleCandidate = NULL;

        m_pSelectedRoute              = NULL;
        m_pSelectedTrack              = NULL;
        m_pRoutePointEditTarget       = NULL;
        m_pFoundPoint                 = NULL;
        m_pMouseRoute                 = NULL;
        m_prev_pMousePoint            = NULL;
        m_pEditRouteArray             = NULL;
        m_pFoundRoutePoint            = NULL;
        m_pFoundRoutePointSecond      = NULL;

        m_bShowSegInfo = false;

//    Build the cursors

#if defined( __WXGTK__) || defined(__WXOSX__) /* inline rms */


#ifdef USE_PNG_CURSOR_THIS_DOES_NOT_WORK
              wxImage ICursorLeft  = _img_left->ConvertToImage( );
              wxImage ICursorRight  = _img_right->ConvertToImage( );
              wxImage ICursorUp  = _img_up->ConvertToImage( );
              wxImage ICursorDown  = _img_down->ConvertToImage( );
              wxImage ICursorPencil  = _img_pencil->ConvertToImage( );

#else
                wxImage ICursorLeft ( left );
                wxImage ICursorRight ( right );
                wxImage ICursorUp ( up );
                wxImage ICursorDown ( down );
                wxImage ICursorPencil ( pencil );
#endif
                if ( ICursorLeft.Ok() )
                {
                        ICursorLeft.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_X, 0 );
                        ICursorLeft.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15 );
                        pCursorLeft =  new wxCursor ( ICursorLeft );
                }
                else
                        pCursorLeft =  new wxCursor ( wxCURSOR_ARROW );

                if ( ICursorRight.Ok() )
                {
                        ICursorRight.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_X, 31 );
                        ICursorRight.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15 );
                        pCursorRight =  new wxCursor ( ICursorRight );
                }
                else
                        pCursorRight =  new wxCursor ( wxCURSOR_ARROW );

                if ( ICursorUp.Ok() )
                {
                        ICursorUp.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_X, 15 );
                        ICursorUp.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 0 );
                        pCursorUp =  new wxCursor ( ICursorUp );
                }
                else
                        pCursorUp =  new wxCursor ( wxCURSOR_ARROW );

                if ( ICursorDown.Ok() )
                {
                        ICursorDown.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_X, 15 );
                        ICursorDown.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 31 );
                        pCursorDown =  new wxCursor ( ICursorDown );
                }
                else
                        pCursorDown =  new wxCursor ( wxCURSOR_ARROW );

                if ( ICursorPencil.Ok() )
                {
                        ICursorPencil.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_X, 0 );
                        ICursorPencil.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 20 );
                        pCursorPencil =  new wxCursor ( ICursorPencil );
                }
                else
                        pCursorPencil =  new wxCursor ( wxCURSOR_ARROW );

#else

                //    Windows and X11 need custom, private cursors, say sorry...
                //    unless PNG graphic cursors are available....

#ifndef USE_PNG_CURSOR
                pCursorLeft =    new ocpCursor ( left,  0, 00, 15 );
                pCursorRight =   new ocpCursor ( right, 0, 31, 15 );
                pCursorUp =      new ocpCursor ( up,    0, 15, 00 );
                pCursorDown =    new ocpCursor ( down,  0, 15, 31 );
                pCursorPencil =  new ocpCursor ( pencil, 0, 00, 20 );

#else
              wxImage ICursorLeft  = _img_left->ConvertToImage( );
              wxImage ICursorRight  = _img_right->ConvertToImage( );
              wxImage ICursorUp  = _img_up->ConvertToImage( );
              wxImage ICursorDown  = _img_down->ConvertToImage( );
              wxImage ICursorPencil  = _img_pencil->ConvertToImage( );

              if ( ICursorLeft.Ok() )
              {
                    ICursorLeft.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_X, 0 );
                    ICursorLeft.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15 );
                    pCursorLeft =  new wxCursor ( ICursorLeft );
              }
              else
                    pCursorLeft =  new wxCursor ( wxCURSOR_ARROW );

              if ( ICursorRight.Ok() )
              {
                    ICursorRight.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_X, 31 );
                    ICursorRight.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15 );
                    pCursorRight =  new wxCursor ( ICursorRight );
              }
              else
                    pCursorRight =  new wxCursor ( wxCURSOR_ARROW );

              if ( ICursorUp.Ok() )
              {
                    ICursorUp.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_X, 15 );
                    ICursorUp.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 0 );
                    pCursorUp =  new wxCursor ( ICursorUp );
              }
              else
                    pCursorUp =  new wxCursor ( wxCURSOR_ARROW );

              if ( ICursorDown.Ok() )
              {
                    ICursorDown.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_X, 15 );
                    ICursorDown.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 31 );
                    pCursorDown =  new wxCursor ( ICursorDown );
              }
              else
                    pCursorDown =  new wxCursor ( wxCURSOR_ARROW );

              if ( ICursorPencil.Ok() )
              {
                    ICursorPencil.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_X, 0 );
                    ICursorPencil.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15 );
                    pCursorPencil =  new wxCursor ( ICursorPencil );
              }
              else
                    pCursorPencil =  new wxCursor ( wxCURSOR_ARROW );

              //  Additionally, on W98, the PNG cursors don't work well due to transparency issues
              //  So, default to simple B/W cursors
              if(g_pPlatform->GetOperatingSystemId() == wxOS_WINDOWS_9X)
              {
                    pCursorLeft =    new ocpCursor ( left,  0, 00, 15 );
                    pCursorRight =   new ocpCursor ( right, 0, 31, 15 );
                    pCursorUp =      new ocpCursor ( up,    0, 15, 00 );
                    pCursorDown =    new ocpCursor ( down,  0, 15, 31 );
                    pCursorPencil =  new ocpCursor ( pencil, 0, 00, 20 );
              }

#endif

#endif      // MSW, X11

        pCursorArrow = new wxCursor ( wxCURSOR_ARROW );

        SetMyCursor ( pCursorArrow );

        current_scale_method = RENDER_LODEF;          // default
        m_bSubsamp = true;
        m_rescale_timer_msec = 1000;

        pRescaleTimer = new wxTimer ( this, RESCALE_TIMER );
        pRescaleTimer->Stop();

        pPanTimer = new wxTimer ( this, PAN_TIMER );
        pPanTimer->Stop();

        pRotDefTimer = new wxTimer ( this, ROT_TIMER );
        pRotDefTimer->Stop();

        pCurTrackTimer = new wxTimer ( this, CURTRACK_TIMER );
        pCurTrackTimer->Stop();
        m_curtrack_timer_msec = 2;

        m_MouseWheelTimer.SetOwner(this);

        m_b_rot_hidef = true;

        pBM = NULL;

//    Set up current arrow drawing factors
        int mmx, mmy;
        wxDisplaySizeMM ( &mmx, &mmy );

        int sx, sy;
        wxDisplaySize ( &sx, &sy );

        m_pix_per_mm = ( ( double ) sx ) / ( ( double ) mmx );

        int mm_per_knot = 25;
        current_draw_scaler =  mm_per_knot * m_pix_per_mm;

        pscratch_bm = NULL;
        proute_bm = NULL;

        m_prot_bm = NULL;

// Set some benign initial values

        m_cs = GLOBAL_COLOR_SCHEME_DAY;
        VPoint.clat = 0;
        VPoint.clon = 0;
        VPoint.view_scale_ppm = 1;
        VPoint.bValid = false;

        m_pPix = NULL;

        m_canvas_scale_factor = 1.;

        m_canvas_width = 1000;

//    Create the default wvs chart
        pwvs_chart = new WVSChart ( this, *pWVS_Locn );


        //    Create the default depth unit emboss maps
        m_pEM_Feet = NULL;                // maps for depth unit emboss pattern
        m_pEM_Meters = NULL;
        m_pEM_Fathoms = NULL;

        CreateDepthUnitEmbossMaps( GLOBAL_COLOR_SCHEME_DAY );

        m_pEM_OverZoom = NULL;
        CreateOZEmbossMapData(GLOBAL_COLOR_SCHEME_DAY);


//    Build icons for tide/current points
#ifdef USE_PNG_TIDESML
        m_bmTideDay = wxBitmap(*_img_tidesml);
#else
        m_bmTideDay = wxBitmap(tidesml);
#endif

//    Dusk
        m_bmTideDusk = CreateDimBitmap(m_bmTideDay, .50);

//    Night
        m_bmTideNight = CreateDimBitmap(m_bmTideDay, .20);

//    Build Dusk/Night  ownship icons
#ifdef USE_PNG_OWNSHIP
        double factor_dusk = 0.5;
        double factor_night = 0.25;

        //Green
        m_os_image_green_day   = _img_ship_green->ConvertToImage();

        int img_width = m_os_image_green_day.GetWidth();
        int img_height = m_os_image_green_day.GetHeight();

        m_os_image_green_dusk = m_os_image_green_day.Copy();
        m_os_image_green_night = m_os_image_green_day.Copy();

        for(int iy=0 ; iy < img_height ; iy++)
        {
              for(int ix=0 ; ix < img_width ; ix++)
              {
                    if(!m_os_image_green_day.IsTransparent(ix, iy))
                    {
                        wxImage::RGBValue rgb(m_os_image_green_day.GetRed(ix, iy), m_os_image_green_day.GetGreen(ix, iy), m_os_image_green_day.GetBlue(ix, iy));
                        wxImage::HSVValue hsv = wxImage::RGBtoHSV(rgb);
                        hsv.value = hsv.value * factor_dusk;
                        wxImage::RGBValue nrgb = wxImage::HSVtoRGB(hsv);
                        m_os_image_green_dusk.SetRGB(ix, iy, nrgb.red, nrgb.green, nrgb.blue);

                        hsv = wxImage::RGBtoHSV(rgb);
                        hsv.value = hsv.value * factor_night;
                        nrgb = wxImage::HSVtoRGB(hsv);
                        m_os_image_green_night.SetRGB(ix, iy, nrgb.red, nrgb.green, nrgb.blue);
                    }
              }
        }

       //Red
        m_os_image_red_day   = _img_ship_red->ConvertToImage();

        int rimg_width = m_os_image_red_day.GetWidth();
        int rimg_height = m_os_image_red_day.GetHeight();

        m_os_image_red_dusk = m_os_image_red_day.Copy();
        m_os_image_red_night = m_os_image_red_day.Copy();

        for(int iy=0 ; iy < rimg_height ; iy++)
        {
              for(int ix=0 ; ix < rimg_width ; ix++)
              {
                    if(!m_os_image_red_day.IsTransparent(ix, iy))
                    {
                        wxImage::RGBValue rgb(m_os_image_red_day.GetRed(ix, iy), m_os_image_red_day.GetGreen(ix, iy), m_os_image_red_day.GetBlue(ix, iy));
                        wxImage::HSVValue hsv = wxImage::RGBtoHSV(rgb);
                        hsv.value = hsv.value * factor_dusk;
                        wxImage::RGBValue nrgb = wxImage::HSVtoRGB(hsv);
                        m_os_image_red_dusk.SetRGB(ix, iy, nrgb.red, nrgb.green, nrgb.blue);

                        hsv = wxImage::RGBtoHSV(rgb);
                        hsv.value = hsv.value * factor_night;
                        nrgb = wxImage::HSVtoRGB(hsv);
                        m_os_image_red_night.SetRGB(ix, iy, nrgb.red, nrgb.green, nrgb.blue);
                    }
              }
        }

        //Grey
        m_os_image_grey_day   = (_img_ship_red->ConvertToImage()).ConvertToGreyscale();

        int gimg_width = m_os_image_grey_day.GetWidth();
        int gimg_height = m_os_image_grey_day.GetHeight();

        m_os_image_grey_dusk  = m_os_image_grey_day.Copy();
        m_os_image_grey_night = m_os_image_grey_day.Copy();

        for(int iy=0 ; iy < gimg_height ; iy++)
        {
              for(int ix=0 ; ix < gimg_width ; ix++)
              {
                    if(!m_os_image_grey_day.IsTransparent(ix, iy))
                    {
                          wxImage::RGBValue rgb(m_os_image_grey_day.GetRed(ix, iy), m_os_image_grey_day.GetGreen(ix, iy), m_os_image_grey_day.GetBlue(ix, iy));
                          wxImage::HSVValue hsv = wxImage::RGBtoHSV(rgb);
                          hsv.value = hsv.value * factor_dusk;
                          wxImage::RGBValue nrgb = wxImage::HSVtoRGB(hsv);
                          m_os_image_grey_dusk.SetRGB(ix, iy, nrgb.red, nrgb.green, nrgb.blue);

                          hsv = wxImage::RGBtoHSV(rgb);
                          hsv.value = hsv.value * factor_night;
                          nrgb = wxImage::HSVtoRGB(hsv);
                          m_os_image_grey_night.SetRGB(ix, iy, nrgb.red, nrgb.green, nrgb.blue);
                    }
              }
        }


#endif
        //  Set initial pointers to ownship images
        m_pos_image_green = &m_os_image_green_day;
        m_pos_image_red   = &m_os_image_red_day;

        m_pQuilt = new Quilt();
}

ChartCanvas::~ChartCanvas()
{

        delete pThumbDIBShow;

//    Delete Cursors
        delete pCursorLeft;
        delete pCursorRight;
        delete pCursorUp;
        delete pCursorDown;
        delete pCursorArrow;
        delete pCursorPencil;

        delete pRescaleTimer;
        delete pPanTimer;
        delete pCurTrackTimer;
        delete pRotDefTimer;
        delete m_pPopUpWin;
        delete m_pCIWin;

        delete pBM;
        delete m_pPix;

        delete pscratch_bm;

        m_dc_route.SelectObject ( wxNullBitmap );
        delete proute_bm;


        delete pwvs_chart;
        delete pss_overlay_bmp;

        delete m_pEM_Feet;
        delete m_pEM_Meters;
        delete m_pEM_Fathoms;

        delete m_pEM_OverZoom;

        delete m_pQuilt;

        delete m_prot_bm;

}

bool ChartCanvas::IsQuiltDelta(void)
{
      return m_pQuilt->IsQuiltDelta();
}

ArrayOfInts ChartCanvas::GetQuiltIndexArray(void)
{
      return m_pQuilt->GetQuiltIndexArray();;
}

void ChartCanvas::SetQuiltMode(bool b_quilt)
{
      VPoint.b_quilt = b_quilt;
}

bool ChartCanvas::GetQuiltMode(void)
{
      return VPoint.b_quilt;
}

void ChartCanvas::InvalidateAllQuiltPatchs(void)
{
      m_pQuilt->InvalidateAllQuiltPatchs();
}

ChartBase *ChartCanvas::GetLargestScaleQuiltChart()
{
      return m_pQuilt->GetLargestScaleChart();
}

ChartBase *ChartCanvas::GetFirstQuiltChart()
{
      return m_pQuilt->GetFirstChart();
}

ChartBase *ChartCanvas::GetNextQuiltChart()
{
      return m_pQuilt->GetNextChart();
}

int ChartCanvas::GetQuiltChartCount()
{
      return m_pQuilt->GetnCharts();
}

void ChartCanvas::SetQuiltChartHiLiteIndex(int dbIndex)
{
      m_pQuilt->SetHiliteIndex(dbIndex);
}

ArrayOfInts ChartCanvas::GetQuiltCandidatedbIndexArray(bool flag1, bool flag2)
{
       return m_pQuilt->GetCandidatedbIndexArray(flag1, flag2);
}

int ChartCanvas::GetQuiltRefChartdbIndex(void)
{
      return  m_pQuilt->GetRefChartdbIndex();
}

ArrayOfInts ChartCanvas::GetQuiltExtendedStackdbIndexArray()
{
      return  m_pQuilt->GetExtendedStackIndexArray();
}

ArrayOfInts ChartCanvas::GetQuiltEclipsedStackdbIndexArray()
{
      return  m_pQuilt->GetEclipsedStackIndexArray();
}

void ChartCanvas::InvalidateQuilt(void)
{
      return m_pQuilt->Invalidate();
}

double ChartCanvas::GetQuiltMaxErrorFactor()
{
      return m_pQuilt->GetMaxErrorFactor();
}

bool ChartCanvas::IsChartQuiltable(int db_index)
{
      return m_pQuilt->IsChartQuiltable(db_index);
}





void ChartCanvas::OnChar(wxKeyEvent &event)
{
      bool b_proc = Do_Hotkeys(event);

      if(!b_proc)
            event.Skip();
}



bool ChartCanvas::Do_Hotkeys(wxKeyEvent &event)
{
      // HOTKEYS
      bool b_proc = false;
      {
            int key_code = event.GetKeyCode();

            switch(key_code)
            {
                  case  WXK_LEFT:
                        if ( event.GetModifiers() == wxMOD_CONTROL )
                              parent_frame->DoStackDown();
                        else
                        {
///                              g_bCourseUp = true; VPoint.rotation -= 5.0 * PI / 180.;   ReloadVP();
                              if(event.GetModifiers() == wxMOD_ALT)
                                    PanCanvas(-2,0);
                              else
                                    PanCanvas(-100, 0);///  gCog -= 1; if(gCog < 0) gCog += 360.; ReloadVP();
                        }
                        b_proc = true;
                        break;

                  case  WXK_UP:
                        if(event.GetModifiers() == wxMOD_ALT)
                              PanCanvas(0, -2);
                        else
                              PanCanvas(0, -100);
                        b_proc = true;
                        break;

                  case  WXK_RIGHT:
                        if ( event.GetModifiers() == wxMOD_CONTROL )
                              parent_frame->DoStackUp();
                        else
                        {
//                              g_bCourseUp = true; VPoint.rotation += 5.0 * PI / 180.;   ReloadVP();
                              if(event.GetModifiers() == wxMOD_ALT)
                                    PanCanvas(2,0);
                              else
                                    PanCanvas(100, 0); ///  gCog += 1; if(gCog > 360.) gCog -= 360.; ReloadVP();
                        }
                        b_proc = true;
                        break;

                  case  WXK_DOWN:
                        if(event.GetModifiers() == wxMOD_ALT)
                              PanCanvas(0, 2);
                        else
                              PanCanvas(0, 100);
                        b_proc = true;
                        break;

                  case WXK_F2:
                        parent_frame->TogglebFollow();
                        b_proc = true;
                        break;

                  case WXK_F3:
                        parent_frame->ToggleENCText();
                        b_proc = true;

                        break;
                  case WXK_F4:
                        m_bMeasure_Active = true;
                        m_nMeasureState = 1;
                        SetMyCursor ( pCursorPencil );
                        Refresh();
                        b_proc = true;
                        break;

                  case WXK_F5:
                        parent_frame->ToggleColorScheme();
                        b_proc = true;
                        break;

                  case WXK_F7:
                        parent_frame->DoStackDown();
                        b_proc = true;
                        break;

                  case WXK_F8:
                        parent_frame->DoStackUp();
                        b_proc = true;
                        break;

                  case WXK_F9:
                  {
                        parent_frame->ToggleQuiltMode();
                        b_proc = true;
                        ReloadVP();
                        break;
                  }
                  case WXK_F11:
                        ShowGribDialog();
                        break;

                  case WXK_F12:
                      {
                        parent_frame->ToggleChartOutlines();
                        b_proc = true;
                        break;
                      }
                  default:
                        break;

            }

            if(key_code < 128)            //ascii
            {
                  char key_char = (char)key_code;
                  switch(key_char)
                  {
                        case '+':
                        case '=':
                        case 26:                     // Ctrl Z
                                    ZoomCanvasIn();
                                    break;
                                    b_proc = true;

                        case '-':
                        case 24:                     // Ctrl X
                                    ZoomCanvasOut();
                                    break;
                                    b_proc = true;

                        case 19:                     // Ctrl S
                                    parent_frame->ToggleENCText();
                                    break;
                                    b_proc = true;

                        case 1:                      // Ctrl A
                                    parent_frame->TogglebFollow();
                                    break;
                                    b_proc = true;

                  case 15:                     // Ctrl O
                                    parent_frame->ToggleChartOutlines();
                                    break;
                                    b_proc = true;

                  case 13:                     // Ctrl M                      //    Drop Marker;
                              {
                                    RoutePoint *pWP = new RoutePoint ( m_cursor_lat, m_cursor_lon, wxString ( _T ( "triangle" ) ), wxString ( _( "New Mark" ) ), NULL );
                                    pSelect->AddSelectableRoutePoint ( m_cursor_lat, m_cursor_lon, pWP );
                                    pConfig->AddNewWayPoint ( pWP, -1 );    // use auto next num
                                    Refresh ( false );
                                    break;
                                    b_proc = true;
                              }

                  case 32:                     // Ctrl Space            //    Drop MOB
                        {
                              if ( event.GetModifiers() == wxMOD_CONTROL )
                              {
                                          RoutePoint *pWP = new RoutePoint ( gLat, gLon, wxString ( _T ( "mob" ) ), wxString ( _( "MAN OVERBOARD" ) ), NULL );
                                          pSelect->AddSelectableRoutePoint ( gLat, gLon, pWP );
                                          pConfig->AddNewWayPoint ( pWP, -1 );    // use auto next num
                                          Refresh ( false );
                              }
                              b_proc = true;
                              break;
                        }

                  case 17:                       // Ctrl Q
                              parent_frame->Close();
                              b_proc = true;
                              break;

                  case 27:                       // Generic break
                        if(m_bMeasure_Active)
                        {
                              m_bMeasure_Active = false;
                              g_pRouteMan->DeleteRoute ( m_pMeasureRoute );
                              m_pMeasureRoute = NULL;
                              Refresh ( false );
                        }
                        b_proc = true;
                        break;

                        default:
                              break;

                  }           // switch
            }
      }

      return b_proc;
}

void ChartCanvas::ShowGribDialog(void)
{
      if(NULL == g_pGribDialog)
      {
            g_pGribDialog = new GRIBUIDialog();
            g_pGribDialog->Create ( this, -1, _("GRIB Display Control"), g_grib_dir,
                               wxPoint( g_grib_dialog_x, g_grib_dialog_y), wxSize( g_grib_dialog_sx, g_grib_dialog_sy));
      }

      g_pGribDialog->Show();                        // Show modeless, so it stays on the screen
}



void ChartCanvas::SetColorScheme(ColorScheme cs)
{
            //    Setup ownship image pointers
      switch(cs)
      {
            case GLOBAL_COLOR_SCHEME_DAY:
                  m_pos_image_green = &m_os_image_green_day;
                  m_pos_image_red   = &m_os_image_red_day;
                  m_pos_image_grey  = &m_os_image_grey_day;
                  break;
            case GLOBAL_COLOR_SCHEME_DUSK:
                  m_pos_image_green = &m_os_image_green_dusk;
                  m_pos_image_red   = &m_os_image_red_dusk;
                  m_pos_image_grey  = &m_os_image_grey_dusk;
                  break;
            case GLOBAL_COLOR_SCHEME_NIGHT:
                  m_pos_image_green = &m_os_image_green_night;
                  m_pos_image_red   = &m_os_image_red_night;
                  m_pos_image_grey  = &m_os_image_grey_night;
                  break;
            default:
                  m_pos_image_green = &m_os_image_green_day;
                  m_pos_image_red   = &m_os_image_red_day;
                  m_pos_image_grey  = &m_os_image_grey_day;
                  break;
      }

      CreateDepthUnitEmbossMaps( cs );
      CreateOZEmbossMapData( cs );

      SetbTCUpdate(true);                        // force re-render of tide/current locators
      FlushBackgroundRender();

      ReloadVP();

      m_cs = cs;
}

wxBitmap ChartCanvas::CreateDimBitmap(wxBitmap &Bitmap, double factor)
{
      wxImage img = Bitmap.ConvertToImage();
      int sx = img.GetWidth();
      int sy = img.GetHeight();

      wxImage new_img(img);

      for(int i = 0 ; i < sx ; i++)
      {
            for(int j = 0 ; j < sy ; j++)
            {
                  if(!img.IsTransparent(i,j))
                  {
                        new_img.SetRGB(i, j, (unsigned char)(img.GetRed(i, j) * factor),
                                       (unsigned char)(img.GetGreen(i, j) * factor),
                                        (unsigned char)(img.GetBlue(i, j) * factor));
                  }
            }
      }

      wxBitmap ret = wxBitmap(new_img);

      return ret;

}


void ChartCanvas::RotateTimerEvent( wxTimerEvent& event )
{
      m_b_rot_hidef = true;
      ReloadVP();
}


void ChartCanvas::RescaleTimerEvent ( wxTimerEvent& event )
{

      /*
        if ( m_bBackRender && !m_bbr_paused )
        {
                if ( br_Ch )
                {
                        br_Ch->Abort_BackgroundHiDefRender();
                        br_Ch = NULL;
                        m_bBackRender = false;
                }
        }

*/
        if ( ( RENDER_HIDEF != current_scale_method ) && !m_bBackRender )
        {
                //    Start a background render
                //    If the Initialize call fails, it simply means that the chart cannot do a background render
                //    and so no action is performed.
                br_Ch = m_RescaleCandidate;
                if ( br_Ch )
                {
                        {
//                              printf("Chcanv Rescale timer start bbr\n");
                              m_bBackRender = true;
                                m_bbr_paused = false;
                                ::wxWakeUpIdle();
                        }
                }
                return;
        }

        if ( ( RENDER_HIDEF != current_scale_method ) && m_bBackRender && m_bbr_paused )
        {
                m_bbr_paused = false;
                return;
        }
}

/*
void ChartCanvas::MouseWheelTimerEvent ( wxTimerEvent& event )
{
      m_bEnableWheelEvents = true;
}
*/

void ChartCanvas::OnIdleEvent ( wxIdleEvent& event )
{
        //  If a background render is in process, continue it along...
        if ( m_bBackRender )
        {

                if ( br_Ch != m_RescaleCandidate )              // a logical error, happens on chart change
                {
                        if ( br_Ch )
                        {
                                br_Ch = NULL;
                                m_bBackRender = false;
                                event.Skip();
                                return;
                        }
                }

                if ( !m_bbr_paused )
                {
                      int br_ret = br_Ch->Continue_BackgroundHiDefRender();
                      if(( BR_DONE_NOP == br_ret) || ( BR_DONE_REFRESH == br_ret))    //done?
                      {
//                              printf("Chcanv bbr finish reported\n");
                              current_scale_method = RENDER_HIDEF;
                              m_bBackRender = false;

                              if(BR_DONE_REFRESH == br_ret)
                                    Refresh ( false );
                        }
                        else
                                event.RequestMore();

                }
                event.Skip();
        }
        else
                event.Skip();
}


void ChartCanvas::OnCursorTrackTimerEvent ( wxTimerEvent& event )
{
//      This is here because GTK status window update is expensive.. Why??
//      Anyway, only update the status bar when this timer expires
#ifdef __WXGTK__
        {
//                char buf[30];
                if ( 1/*Current_Ch*/ )
                {
                        double cursor_lat, cursor_lon;
                        GetCanvasPixPoint ( mouse_x, mouse_y, cursor_lat, cursor_lon );

                        //    Check the absolute range of the cursor position
                        //    There could be a window wherein the chart geoereferencing is not valid....
                        if((fabs(cursor_lat) < 90.) && (fabs(cursor_lon) < 360.))
                        {
                              while(cursor_lon < -180.)
                                    cursor_lon += 360.;

                              while(cursor_lon > 180.)
                                    cursor_lon -= 360.;

                              if ( parent_frame->m_pStatusBar )
                              {
                                    wxString s1 = _("Cursor: ");
                                    s1 += toSDMM(1, cursor_lat);
                                    s1 += _T(" ");
                                    s1 += toSDMM(2, cursor_lon);
                                    parent_frame->SetStatusText ( s1, STAT_FIELD_CURSOR_LL );

                                    double brg, dist;
                                    DistanceBearingMercator(cursor_lat, cursor_lon, gLat, gLon, &brg, &dist);
                                    wxString s;
                                    s.Printf(_("From Ownship: %03d Deg  %6.2f NMi"), (int)brg, dist);
                                    parent_frame->SetStatusText ( s, STAT_FIELD_CURSOR_BRGRNG );
                              }
                       }
                }
        }
#endif
}

void ChartCanvas::GetCursorLatLon(double *lat, double *lon)
{
      double clat, clon;
      GetCanvasPixPoint ( mouse_x, mouse_y, clat, clon );
      *lat = clat;
      *lon = clon;
}

void ChartCanvas::GetCanvasPointPix ( double rlat, double rlon, wxPoint *r )
{
                // If the Current Chart is a raster chart, and the
                // requested lat/long is within the boundaries of the chart,
                // and the VP is not rotated,
                // then use the embedded BSB chart georeferencing algorithm
                // for greater accuracy
                // If for some reason the chart rejects the request by returning an error,
                // then fall back to mercator estimate from canvas parameters
                bool bUseMercator = true;

                if ( Current_Ch && ( fabs(VPoint.rotation) < .01) )
                {
                  if(Current_Ch->m_ChartFamily == CHART_FAMILY_RASTER)
                  {
                        ChartBaseBSB *Cur_BSB_Ch = dynamic_cast<ChartBaseBSB *> ( Current_Ch );
                        bool bInside = G_FloatPtInPolygon ( ( MyFlPoint * ) Cur_BSB_Ch->GetCOVRTableHead ( 0 ),
                                                            Cur_BSB_Ch->GetCOVRTablenPoints ( 0 ), rlon, rlat );
                        bInside = true;
                        if ( bInside )
                        {
                              int  rpixxd, rpixyd;
                              if ( 0 == Cur_BSB_Ch->latlong_to_pix_vp ( rlat, rlon, rpixxd, rpixyd, VPoint ) )
                              {
                                        r->x = rpixxd;
                                        r->y = rpixyd;
                                        bUseMercator = false;
                              }
                        }
                  }
                }

                //    if needed, use the Mercator scaling estimator,
                //    using VPoint center as reference point
                if ( bUseMercator )
                {
                      wxPoint p = VPoint.GetMercatorPixFromLL(rlat, rlon);
                      *r = p;
                }

}

void ChartCanvas::GetCanvasPixPoint ( int x, int y, double &lat, double &lon )
{
                // If the Current Chart is a raster chart, and the
                // requested x,y is within the boundaries of the chart,
                // and the VP is not rotated,
                // then use the embedded BSB chart georeferencing algorithm
                // for greater accuracy
                // If for some reason the chart rejects the request by returning an error,
                // then fall back to mercator estimate from canvas parameters
                bool bUseMercator = true;

                if ( Current_Ch && ( fabs(VPoint.rotation) < .01))
                {
                      if ( ( Current_Ch->m_ChartType == CHART_TYPE_GEO ) || ( Current_Ch->m_ChartType == CHART_TYPE_KAP ) )
                        {
                        ChartBaseBSB *Cur_BSB_Ch = dynamic_cast<ChartBaseBSB *> ( Current_Ch );

                        // TODO     maybe need iterative process to validate bInside
                        //          first pass is mercator, then check chart boundaries
                        bool bInside = true;
                        if ( bInside )
                        {
                                double slat, slon;
                                if ( 0 == Cur_BSB_Ch->vp_pix_to_latlong ( VPoint, x, y, &slat, &slon ) )
                                {
                                        lat = slat;

                                        if(slon < -180.)
                                              slon += 360.;
                                        else if(slon > 180.)
                                              slon -= 360.;

                                        lon = slon;
                                        bUseMercator = false;
                                }
                          }
                        }
                }

                //    if needed, use the Mercator scaling estimator
                if ( bUseMercator )
                {
                        VPoint.GetMercatorLLFromPix(wxPoint(x, y), &lat, &lon);
                }
}

void ChartCanvas::FlushBackgroundRender ( void )
{
        if ( m_bBackRender )
        {
                m_bBackRender = false;
                m_bbr_paused = false;
        }
}

bool ChartCanvas::ZoomCanvasIn(double lat, double lon)
{
      double zoom_factor = 2.0;
//      if(g_bQuilt)
//            zoom_factor = 1.5;

      double min_allowed_scale = 50.0;                // meters per meter

      ChartBase *reference_chart = NULL;
      if(VPoint.b_quilt)
            reference_chart = ChartData->OpenChartFromDB(m_pQuilt->GetRefChartdbIndex(), FULL_INIT);
      else
            reference_chart = Current_Ch;

      double proposed_scale_onscreen = GetCanvasScaleFactor() / (GetVPScale() * zoom_factor);

      if(reference_chart)
      {
            if(!VPoint.b_quilt)
                  min_allowed_scale = reference_chart->GetNormalScaleMin(GetCanvasScaleFactor(), g_b_overzoom_x);

            double target_scale_ppm = GetVPScale() * zoom_factor;
            double new_scale_ppm = reference_chart->GetNearestPreferredScalePPM(target_scale_ppm);

            proposed_scale_onscreen = GetCanvasScaleFactor() / new_scale_ppm;

            //  Query the chart to determine the appropriate zoom range
            if(proposed_scale_onscreen < min_allowed_scale)
            {
                  if(min_allowed_scale == GetCanvasScaleFactor() / (GetVPScale()))
                        return false;
                  else
                        proposed_scale_onscreen = min_allowed_scale;
            }


            //          In quilt mode, maybe we want to shift the reference chart to a larger scale on zoom in

            if(VPoint.b_quilt)
            {
                  double min_ref_scale = 2 * reference_chart->GetNormalScaleMin(GetCanvasScaleFactor(), false);
                  if(proposed_scale_onscreen < min_ref_scale)
                  {
                        ArrayOfInts piano_chart_index_array = GetQuiltExtendedStackdbIndexArray();
                        int current_db_index = m_pQuilt->GetRefChartdbIndex();
                        int current_type = ChartData->GetDBChartType(current_db_index);

                        //   Find the index into the extended array corresponding to the current db index
                        int current_ref_stack_index = -1;
                        for(unsigned int im=0 ; im < piano_chart_index_array.GetCount() ; im++)
                        {
                              if(current_db_index == piano_chart_index_array.Item(im))
                              {
                                    current_ref_stack_index = im;
                                    break;
                              }
                        }

                        if(current_ref_stack_index > 0)           //Must be at least 1
                        {
                              int target_stack_index = current_ref_stack_index-1;

                              //    Make sure the target is the same chart type
                              //    and acceptable projection, etc....
                              while(target_stack_index >= 0)
                              {
                                    int new_db_index_test = piano_chart_index_array.Item(target_stack_index);

                                    if(m_pQuilt->IsChartQuiltable(new_db_index_test) &&
                                       (current_type == ChartData->GetDBChartType(new_db_index_test)))
                                       break;

                                    target_stack_index--;
                              }

                              if(target_stack_index >= 0)
                              {
                                    int new_db_index = piano_chart_index_array.Item(target_stack_index);
                                    ChartBase *pc = ChartData->OpenChartFromDB(new_db_index, FULL_INIT);
                                    double max_scale = pc->GetNormalScaleMin(GetCanvasScaleFactor(), false);

                              //    Maybe jump one chart, if the original selection is too overzoomed
                                    if(max_scale < proposed_scale_onscreen)
                                    {
                                          if(target_stack_index)
                                          {
                                                int new_db_index_jump = piano_chart_index_array.Item(target_stack_index-1);

                                                if(m_pQuilt->IsChartQuiltable(new_db_index_jump) &&
                                                                  (current_type == ChartData->GetDBChartType(new_db_index_jump)))

                                                      new_db_index = piano_chart_index_array.Item(target_stack_index-1);
                                          }
                                    }


                                    pCurrentStack->SetCurrentEntryFromdbIndex(new_db_index);

                                    SetQuiltRefChart(new_db_index);
                              }

                        }
                  }
            }
      }

       if((lat == 0.) && (lon == 0.))            // this is a special secret code, means to change scale only
             SetVPScale(GetCanvasScaleFactor() / proposed_scale_onscreen);
       else
             SetViewPoint ( lat, lon, GetCanvasScaleFactor() / proposed_scale_onscreen, VPoint.skew, VPoint.rotation, FORCE_SUBSAMPLE );

       Refresh(false);

       return true;
}


bool ChartCanvas::ZoomCanvasOut(double lat, double lon)
{
      double zoom_factor = 2.0;
//      if(g_bQuilt)
//            zoom_factor = 1.5;


      ChartBase *reference_chart = NULL;
      if(VPoint.b_quilt)
            reference_chart = ChartData->OpenChartFromDB(m_pQuilt->GetRefChartdbIndex(), FULL_INIT);
      else
            reference_chart = Current_Ch;

      double max_allowed_scale = 1e8;

      double proposed_scale_onscreen = GetCanvasScaleFactor() / (GetVPScale() / zoom_factor);

      if(reference_chart)
      {
//            if(!VPoint.b_quilt)
                  max_allowed_scale = 1.01 * (reference_chart->GetNormalScaleMax(GetCanvasScaleFactor(), GetCanvasWidth()));

            double target_scale_ppm = GetVPScale() / zoom_factor;
            double new_scale_ppm = reference_chart->GetNearestPreferredScalePPM(target_scale_ppm);

            proposed_scale_onscreen = GetCanvasScaleFactor() / new_scale_ppm;

            //  Query the chart to determine the appropriate zoom range
            if(proposed_scale_onscreen > max_allowed_scale)
            {
                  if(max_allowed_scale == GetCanvasScaleFactor() / (GetVPScale()))
                        return false;
                  else
                        proposed_scale_onscreen = max_allowed_scale;
            }


//          In quilt mode, maybe we want to shift the reference chart to a smaller scale on zoom out
            if(VPoint.b_quilt)
            {
                  double min_ref_scale = reference_chart->GetNormalScaleMax(GetCanvasScaleFactor(), GetCanvasWidth());
                  if(proposed_scale_onscreen > min_ref_scale)
                  {
                        ArrayOfInts piano_chart_index_array = GetQuiltExtendedStackdbIndexArray();
                        int current_db_index = m_pQuilt->GetRefChartdbIndex();
                        int current_type = ChartData->GetDBChartType(current_db_index);

                        //   Find the index into the extended array corresponding to the current db index
                        unsigned int current_ref_stack_index = piano_chart_index_array.GetCount()-1;
                        for(unsigned int im=0 ; im < piano_chart_index_array.GetCount() ; im++)
                        {
                              if(current_db_index == piano_chart_index_array.Item(im))
                              {
                                    current_ref_stack_index = im;
                                    break;
                              }
                        }

                        if(current_ref_stack_index < (piano_chart_index_array.GetCount()-1))
                        {
                              unsigned int target_stack_index = current_ref_stack_index+1;

                              //    Make sure the target is the same chart type
                              while(target_stack_index < piano_chart_index_array.GetCount())
                              {
                                    int new_db_index_test = piano_chart_index_array.Item(target_stack_index);
                                    if(m_pQuilt->IsChartQuiltable(new_db_index_test) &&
                                       (current_type == ChartData->GetDBChartType(new_db_index_test)))

                                          break;
                                    target_stack_index++;
                              }

                              if(target_stack_index < piano_chart_index_array.GetCount())
                              {
                                    int new_db_index = piano_chart_index_array.Item(target_stack_index);

                                    pCurrentStack->SetCurrentEntryFromdbIndex(new_db_index);

                                    SetQuiltRefChart(new_db_index);
                              }
                        }
                  }
            }
      }


      if((lat == 0.) && (lon == 0.))            // this is a special secret code, means to change scale only
            SetVPScale(GetCanvasScaleFactor() / proposed_scale_onscreen);
      else
            SetViewPoint ( lat, lon, GetCanvasScaleFactor() / proposed_scale_onscreen, VPoint.skew, VPoint.rotation, FORCE_SUBSAMPLE );


      Refresh(false);
      return true;
}


void ChartCanvas::ClearbFollow(void)
{
      m_bFollow = false;      // update the follow flag
      parent_frame->SetToolbarItemState( ID_FOLLOW, false );
}

bool ChartCanvas::PanCanvas(int dx, int dy)
{
      double dlat, dlon;
      wxPoint p;

//      double saved_vp_rotation = VPoint.rotation;                      // save a copy
//      VPoint.SetRotationAngle(0.);
      GetCanvasPointPix ( VPoint.clat, VPoint.clon, &p );
      GetCanvasPixPoint ( p.x + dx, p.y + dy, dlat, dlon );
//      VPoint.SetRotationAngle(saved_vp_rotation);


      if(dlon > 360.) dlon -= 360.;
      if(dlon < -360.) dlon += 360.;

      //    This should not really be necessary, but round-trip georef on some charts is not perfect,
      //    So we can get creep on repeated unidimensional pans, and corrupt chart cacheing.......
      if(dx == 0)
            dlon = VPoint.clon;
      if(dy == 0)
            dlat = VPoint.clat;

      SetViewPoint ( dlat, dlon, VPoint.view_scale_ppm, VPoint.skew, VPoint.rotation, RENDER_LODEF/*FORCE_SUBSAMPLE*/ );
      vLat = dlat;
      vLon = dlon;

      ClearbFollow();      // update the follow flag

      Refresh(false);

      return true;
}

void ChartCanvas::ReloadVP ( void )
{
      m_cache_vp.Invalidate();

      VPoint.Invalidate();
      SetViewPoint ( VPoint.clat, VPoint.clon, VPoint.view_scale_ppm, VPoint.skew, VPoint.rotation, CURRENT_RENDER );

      Refresh(false);
}

void ChartCanvas::SetQuiltRefChart(int dbIndex)
{
      m_pQuilt->SetReferenceChart(dbIndex);
      VPoint.Invalidate();
}



void ChartCanvas::SetVPScale ( double scale )
{
//      if(VPoint.bValid)
      SetViewPoint ( VPoint.clat, VPoint.clon, scale, VPoint.skew, VPoint.rotation, FORCE_SUBSAMPLE );
}

bool ChartCanvas::SetViewPoint ( double lat, double lon)
{
      return SetViewPoint ( lat, lon, VPoint.view_scale_ppm, VPoint.skew, VPoint.rotation, CURRENT_RENDER );
}


bool ChartCanvas::SetViewPoint ( double lat, double lon, double scale_ppm, double skew, double rotation, int sample_mode )
{


        //  Any sensible change?
        if((fabs(VPoint.view_scale_ppm - scale_ppm) < 1e-9)
            && (fabs(VPoint.skew - skew) < 1e-9)
            && (fabs(VPoint.rotation - rotation) < 1e-9)
            && (fabs(VPoint.clat - lat) < 1e-9)
            && (fabs(VPoint.clon - lon) < 1e-9)
            && VPoint.IsValid())
              return false;

//        printf("New set viewpoint %g %g %g \n",VPoint.view_scale_ppm - scale_ppm, VPoint.clat - lat, VPoint.clon - lon);

        VPoint.SetProjectionType(PROJECTION_MERCATOR);            // deafault

        VPoint.bValid = true;                     // Mark this ViewPoint as OK

     //    Take a copy of the last viewport
        ViewPort last_vp = VPoint;


        VPoint.skew = skew;
        VPoint.clat = lat;
        VPoint.clon = lon;
        VPoint.view_scale_ppm = scale_ppm;
        VPoint.rotation = rotation;

        if((VPoint.pix_width < 0) || (VPoint.pix_height < 0))           // Canvas parameters not yet set
              return true;

#if 0
               //      Calculate and store some metrics
        if((fabs(skew) > .001) || (fabs(rotation) > .001))
            int ggp = 4;

        //  Compute Viewport reference points for co-ordinate hit testing
        double lat_ul, lat_ur, lat_lr, lat_ll;
        double lon_ul, lon_ur, lon_lr, lon_ll;


        VPoint.GetMercatorLLFromPix(wxPoint(0,                0),                 &lat_ul, &lon_ul);
        VPoint.GetMercatorLLFromPix(wxPoint(VPoint.pix_width, 0),                 &lat_ur, &lon_ur);
        VPoint.GetMercatorLLFromPix(wxPoint(VPoint.pix_width, VPoint.pix_height), &lat_lr, &lon_lr);
        VPoint.GetMercatorLLFromPix(wxPoint(0,                VPoint.pix_height), &lat_ll, &lon_ll);


        if(lon < 0.)
        {
              if((lon_ul > 0.)  &&  (lon_ur < 0.) ){ lon_ul -= 360.;  lon_ll -= 360.;}
        }
        else
        {
              if((lon_ul > 0.)  &&  (lon_ur < 0.) ){ lon_ur += 360.;  lon_lr += 360.;}
        }

        if(lon_ur < lon_ul)
        {
              lon_ur += 360.;
              lon_lr += 360.;
        }

        if(lon_ur > 360.)
        {
              lon_ur -= 360.;
              lon_lr -= 360.;
              lon_ul -= 360.;
              lon_ll -= 360.;
        }

        double dlat_min = lat_ul;
        dlat_min = fmin ( dlat_min, lat_ur );
        dlat_min = fmin ( dlat_min, lat_lr );
        dlat_min = fmin ( dlat_min, lat_ll );

        double dlon_min = lon_ul;
        dlon_min = fmin ( dlon_min, lon_ur );
        dlon_min = fmin ( dlon_min, lon_lr );
        dlon_min = fmin ( dlon_min, lon_ll );

        double dlat_max = lat_ul;
        dlat_max = fmax ( dlat_max, lat_ur );
        dlat_max = fmax ( dlat_max, lat_lr );
        dlat_max = fmax ( dlat_max, lat_ll );

        double dlon_max = lon_ur;
        dlon_max = fmax ( dlon_max, lon_ul );
        dlon_max = fmax ( dlon_max, lon_lr );
        dlon_max = fmax ( dlon_max, lon_ll );


        VPoint.vpBBox.SetMin ( dlon_min,  dlat_min );
        VPoint.vpBBox.SetMax ( dlon_max,  dlat_max );

//        printf("lon:%8g %8g %8g          %8g  %8g\n", lon, lon_min, lon_max, dlon_min, dlon_max);


        //  Calculate a "virtual" screen size rectangle which will encompass all of a potentially rotated canvas

        double rot_angle_save = VPoint.rotation;
        VPoint.rotation = 0.;
        wxPoint p1, p2;
        p1 = VPoint.GetMercatorPixFromLL( dlat_max, dlon_min);
        p2 = VPoint.GetMercatorPixFromLL( dlat_min, dlon_max);
        VPoint.rotation = rot_angle_save;

//        printf("virtual rect  p1.x:%d  p1.y: %d  width: %d  height: %d\n", p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);

        VPoint.rv_rect = wxRect(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);

        /*
        if(VPoint.rv_rect.height < 0)
              VPoint.rv_rect.height = VPoint.pix_height;

        if(VPoint.rv_rect.width < 0)
             VPoint.rv_rect.width = VPoint.pix_width;
*/

#endif
        VPoint.rv_rect = wxRect(0, 0, VPoint.pix_width, VPoint.pix_height);

        //  Specify the minimum required rectangle in unrotated screen space which will supply full screen data after specified rotation
        if((fabs(skew) > .001) || (fabs(rotation) > .001))
        {
              //  Get four reference "corner" points in rotated space

              //  First, get screen geometry factors
              double pw2 = VPoint.pix_width / 2;
              double ph2 = VPoint.pix_height / 2;
              double pix_l = sqrt ( ( pw2 * pw2 ) + ( ph2 * ph2 ) );
              double phi = atan2 ( ph2, pw2 );


              //Rotate the 4 corner points, and get the max rectangle enclosing it
              double rotator = VPoint.rotation;

              double a_east = pix_l * cos ( phi + rotator ) ;
              double a_north = pix_l * sin ( phi + rotator ) ;

              double b_east = pix_l * cos ( rotator - phi + PI ) ;
              double b_north = pix_l * sin ( rotator - phi + PI ) ;

              double c_east = pix_l * cos ( phi + rotator + PI ) ;
              double c_north = pix_l * sin ( phi + rotator + PI ) ;

              double d_east = pix_l * cos ( rotator - phi ) ;
              double d_north = pix_l * sin ( rotator - phi ) ;


              int xmin = (int)wxMin( wxMin(a_east, b_east), wxMin(c_east, d_east));
              int xmax = (int)wxMax( wxMax(a_east, b_east), wxMax(c_east, d_east));
              int ymin = (int)wxMin( wxMin(a_north, b_north), wxMin(c_north, d_north));
              int ymax = (int)wxMax( wxMax(a_north, b_north), wxMax(c_north, d_north));

              int dx = xmax - xmin;
              int dy = ymax - ymin;

              //  It is important for MSW build that viewport pixel dimensions be multiples of 4.....
              if(dy % 4)
                    dy+= 4 - (dy%4);
              if(dx % 4)
                    dx+= 4 - (dx%4);

              //  Grow the source rectangle appropriately
              if(fabs(rotator) > .001)
                    VPoint.rv_rect.Inflate((dx - VPoint.pix_width)/2, (dy - VPoint.pix_height)/2);
        }



        //  Compute Viewport lat/lon reference points for co-ordinate hit testing

        //  This must be done in unrotated space with respect to full unrotated screen space calculated above
        double rotation_save = VPoint.rotation;
        VPoint.SetRotationAngle(0.);

        double lat_ul, lat_ur, lat_lr, lat_ll;
        double lon_ul, lon_ur, lon_lr, lon_ll;


        VPoint.GetMercatorLLFromPix(wxPoint(VPoint.rv_rect.x,                        VPoint.rv_rect.y),                         &lat_ul, &lon_ul);
        VPoint.GetMercatorLLFromPix(wxPoint(VPoint.rv_rect.x + VPoint.rv_rect.width, VPoint.rv_rect.y),                         &lat_ur, &lon_ur);
        VPoint.GetMercatorLLFromPix(wxPoint(VPoint.rv_rect.x + VPoint.rv_rect.width, VPoint.rv_rect.y + VPoint.rv_rect.height), &lat_lr, &lon_lr);
        VPoint.GetMercatorLLFromPix(wxPoint(VPoint.rv_rect.x,                        VPoint.rv_rect.y + VPoint.rv_rect.height), &lat_ll, &lon_ll);


        if(lon < 0.)
        {
              if((lon_ul > 0.)  &&  (lon_ur < 0.) ){ lon_ul -= 360.;  lon_ll -= 360.;}
        }
        else
        {
              if((lon_ul > 0.)  &&  (lon_ur < 0.) ){ lon_ur += 360.;  lon_lr += 360.;}
        }

        if(lon_ur < lon_ul)
        {
              lon_ur += 360.;
              lon_lr += 360.;
        }

        if(lon_ur > 360.)
        {
              lon_ur -= 360.;
              lon_lr -= 360.;
              lon_ul -= 360.;
              lon_ll -= 360.;
        }

        double dlat_min = lat_ul;
        dlat_min = fmin ( dlat_min, lat_ur );
        dlat_min = fmin ( dlat_min, lat_lr );
        dlat_min = fmin ( dlat_min, lat_ll );

        double dlon_min = lon_ul;
        dlon_min = fmin ( dlon_min, lon_ur );
        dlon_min = fmin ( dlon_min, lon_lr );
        dlon_min = fmin ( dlon_min, lon_ll );

        double dlat_max = lat_ul;
        dlat_max = fmax ( dlat_max, lat_ur );
        dlat_max = fmax ( dlat_max, lat_lr );
        dlat_max = fmax ( dlat_max, lat_ll );

        double dlon_max = lon_ur;
        dlon_max = fmax ( dlon_max, lon_ul );
        dlon_max = fmax ( dlon_max, lon_lr );
        dlon_max = fmax ( dlon_max, lon_ll );


        //  Set the viewport lat/lon bounding box appropriately
        VPoint.vpBBox.SetMin ( dlon_min,  dlat_min );
        VPoint.vpBBox.SetMax ( dlon_max,  dlat_max );

        VPoint.SetRotationAngle(rotation_save);

        m_RescaleCandidate = NULL;


        if ( !VPoint.b_quilt && Current_Ch )
        {
              //  Allow the chart to adjust the new ViewPort for performance optimization
              //  This will normally be only a few pixels adjustment...
            Current_Ch->AdjustVP(last_vp, VPoint);

            // If there is a sensible change in the chart render, refresh the whole screen
            if(Current_Ch->IsRenderDelta(last_vp, VPoint))
                  Refresh(false);


             m_RescaleCandidate = Current_Ch;
        }



        //    Calculate the on-screen displayed actual scale
        //    by a simple traverse northward from the center point
        //    of roughly 10 % of the Viewport extent
        double tlat, tlon;
        wxPoint r, r1;
        double delta_y = (VPoint.vpBBox.GetMaxY() - VPoint.vpBBox.GetMinY()) * 60.0 * .10;              // roughly 10 % of lat range, in NM

        //  Make sure the two points are in phase longitudinally
        double lon_norm = VPoint.clon;
        if(lon_norm > 180.)
              lon_norm -= 360;
        else if(lon_norm < -180.)
              lon_norm += 360.;

        ll_gc_ll ( VPoint.clat, lon_norm, 0, delta_y, &tlat, &tlon );

        GetCanvasPointPix ( tlat, tlon, &r1 );
        GetCanvasPointPix ( VPoint.clat, lon_norm, &r );

        m_true_scale_ppm = sqrt(pow((double)(r.y - r1.y), 2) + pow((double)(r.x - r1.x), 2)) / (delta_y * 1852.);

        //        A fall back in case of very high zoom-out, giving delta_y == 0
        //        which can probably only happen with vector charts
        if(0.0 == m_true_scale_ppm)
              m_true_scale_ppm = scale_ppm;

        //        Another fallback, for highly zoomed out charts
        //        This adjustment makes the displayed TrueScale correspond to the
        //        same algorithm used to calculate the chart zoom-out limit for ChartDummy.
        if(scale_ppm < 1e-4)
              m_true_scale_ppm = scale_ppm;

        if(m_true_scale_ppm)
              VPoint.chart_scale = m_canvas_scale_factor / ( m_true_scale_ppm );
        else
              VPoint.chart_scale = 1.0;


        if ( parent_frame->m_pStatusBar )
        {
              double true_scale_display = floor(VPoint.chart_scale / 100.) * 100.;
              wxString text;

              if(Current_Ch)
              {
                    double chart_native_ppm = m_canvas_scale_factor / Current_Ch->GetNativeScale();
                    double scale_factor = scale_ppm / chart_native_ppm;
                    if(scale_factor > 1.0)
                          text.Printf(_("TrueScale: %8.0f  Zoom %4.1fx"), true_scale_display, scale_factor);
                    else
                          text.Printf(_("TrueScale: %8.0f  Zoom %4.2fx"), true_scale_display, scale_factor);
              }
              else
                    text.Printf(_("TrueScale: %8.0f             "), true_scale_display);

              parent_frame->SetStatusText ( text, STAT_FIELD_SCALE );
        }

        if(VPoint.b_quilt)
        {

            if ( last_vp.view_scale_ppm != scale_ppm )
                  m_pQuilt->InvalidateAllQuiltPatchs();


            //  Create the quilt
            if(ChartData && ChartData->IsValid())
            {
                  ChartData->BuildChartStack(pCurrentStack, lat, lon);

                  //   Check to see if the current quilt reference chart is in the new stack
                  int current_ref_stack_index = -1;
                  for(int i=0 ; i < pCurrentStack->nEntry ; i++)
                  {
                        if(m_pQuilt->GetRefChartdbIndex() == pCurrentStack->GetDBIndex(i))
                              current_ref_stack_index = i;
                  }

                  //    If the new stack does not contain the current ref chart, reset the ref chart
                  //    to a chart that is the same type, and has a scale of just larger than the current ref chart
                  if((-1 == current_ref_stack_index) && (m_pQuilt->GetRefChartdbIndex() >= 0))
                  {
                        const ChartTableEntry &cte_ref = ChartData->GetChartTableEntry(m_pQuilt->GetRefChartdbIndex());
                        int target_scale = cte_ref.GetScale();
                        int target_type = cte_ref.GetChartType();

                        int candidate_stack_index = pCurrentStack->nEntry-1;
                        while(candidate_stack_index  >= 0)
                        {
                              const ChartTableEntry &cte_candidate = ChartData->GetChartTableEntry(pCurrentStack->GetDBIndex(candidate_stack_index));
                              int candidate_scale = cte_candidate.GetScale();
                              int candidate_type = cte_candidate.GetChartType();

                              if((candidate_scale <= target_scale) && (candidate_type == target_type))
                                    break;

                              candidate_stack_index --;
                        }

                        //    If that did not work, look for a chart of just smaller scale and same type
                        if(candidate_stack_index < 0)
                        {
                              candidate_stack_index = 0;
                              while(candidate_stack_index  <= pCurrentStack->nEntry-1)
                              {
                                    const ChartTableEntry &cte_candidate = ChartData->GetChartTableEntry(pCurrentStack->GetDBIndex(candidate_stack_index));
                                    int candidate_scale = cte_candidate.GetScale();
                                    int candidate_type = cte_candidate.GetChartType();

                                    if((candidate_scale >= target_scale) && (candidate_type == target_type))
                                          break;

                                    candidate_stack_index ++;
                              }
                        }
                        // and if that did not work, chose stack entry 0

                        if(candidate_stack_index >= pCurrentStack->nEntry)
                              candidate_stack_index = 0;

                        int new_ref_index = pCurrentStack->GetDBIndex(candidate_stack_index);

                        m_pQuilt->SetReferenceChart(new_ref_index);
                  }



                  m_pQuilt->Compose(VPoint);
                  VPoint.SetProjectionType(m_pQuilt->GetQuiltProj());
                  parent_frame->UpdateControlBar();


              //  Allow the quilt to adjust the new ViewPort for performance optimization
              //  This will normally be only a few pixels adjustment...
                  m_pQuilt->AdjustQuiltVP(last_vp, VPoint);

                  m_RescaleCandidate = m_pQuilt->GetLargestScaleChart();
            }

            VPoint.skew = 0.;                                     // Quilting supports 0 Skew

        }


        //    If requested by sample_mode = FORCE_SUBSAMPLE, then
        //    force the scale method to LODEF, rescale enabled


        if( !m_bBackRender && !pRescaleTimer->IsRunning() &&  m_RescaleCandidate )
        {
              current_scale_method = RENDER_LODEF;
              pRescaleTimer->Start ( m_rescale_timer_msec, wxTIMER_ONE_SHOT );
        }

        //  Maintain global vLat/vLon
        vLat = VPoint.clat;
        vLon = VPoint.clon;

        return true;

}

//          Static Icon definitions for some symbols requiring scaling/rotation/translation
//          Very specific wxDC draw commands are necessary to properly render these icons...See the code in ShipDraw()

//      This icon was adapted and scaled from the S52 Presentation Library version 3_03.
//     Symbol VECGND02

#ifndef USE_PNG_OWNSHIP
static int s_pred_icon[] = {
                      0,0,
                      10,10,
                      0,0,
                      -10,10,
                      0,8,
                      7,15,
                      0,8,
                      -7,15
};

//      This ownship icon was adapted and scaled from the S52 Presentation Library version 3_03
//      Symbol OWNSHP05
static int s_ownship_icon[] = {
      5, -50,
      11, -36,
      11, 34,
      -11, 34,
      -11, -36,
      -5, -50,
      -11, 0,
      11, 0,
      0, 34,
      0, -50
};

#else
static int s_png_pred_icon[] = {
      -10,-10,
      -10,10,
      10,10,
      10,-10
};
#endif



wxPoint transrot(wxPoint pt, double theta, wxPoint offset)
{
      wxPoint ret;
      double px =  ( double ) (pt.x * sin ( theta )) + ( double ) (pt.y * cos ( theta ));
      double py =  ( double ) (pt.y * sin ( theta )) - ( double ) (pt.x * cos ( theta ));
      ret.x = (int) wxRound( px );
      ret.y = (int) wxRound( py );
      ret.x += offset.x;
      ret.y += offset.y;

      return ret;
}

void ChartCanvas::ShipDraw ( wxDC& dc )
{

        int drawit = 0;
        wxPoint lShipPoint, lPredPoint, lHeadPoint;

//    Is ship in Vpoint?
        if ( VPoint.vpBBox.PointInBox ( gLon, gLat, 0 ) )
                drawit++;                                 // yep


///debug
//        gCog = 60.;
//        gSog = 6.;
//        gHdt = 70.;
//        g_bHDxValid = true;

//    Calculate ownship Position Predictor

        double pred_lat, pred_lon;

        ll_gc_ll ( gLat, gLon, gCog, gSog * g_ownship_predictor_minutes / 60., &pred_lat, &pred_lon );

        GetCanvasPointPix ( gLat, gLon, &lShipPoint );
        GetCanvasPointPix ( pred_lat, pred_lon, &lPredPoint );

        double cog_rad = atan2 ( (double)( lPredPoint.y - lShipPoint.y ), (double)( lPredPoint.x - lShipPoint.x ) );
        cog_rad += PI;

        double lpp = sqrt(pow((double)(lPredPoint.x - lShipPoint.x), 2) + pow((double)(lPredPoint.y - lShipPoint.y), 2));


//    Is predicted point in the VPoint?
        if ( VPoint.vpBBox.PointInBox ( pred_lon, pred_lat, 0 ) )
                drawit++;                                 // yep

        //  Draw the icon rotated to the COG
        //  or to the Hdt if available
        double icon_hdt = gCog;
        if(g_bHDxValid)
             icon_hdt = gHdt;


//    Calculate ownship Heading pointer as a predictor
        double hdg_pred_lat, hdg_pred_lon;

        ll_gc_ll ( gLat, gLon, icon_hdt, gSog * g_ownship_predictor_minutes / 60., &hdg_pred_lat, &hdg_pred_lon );

        GetCanvasPointPix ( gLat, gLon, &lShipPoint );
        GetCanvasPointPix ( hdg_pred_lat, hdg_pred_lon, &lHeadPoint );

        double icon_rad = atan2 ( (double)( lHeadPoint.y - lShipPoint.y ), (double)( lHeadPoint.x - lShipPoint.x ) );
        icon_rad += PI;

        if(gSog < 0.2)
              icon_rad = ((icon_hdt + 90.) * PI / 180.) + VPoint.skew;

//    Compare the points lHeadPoint and lPredPoint
//    If they differ by more than n pixels, then render the head vector

        double ndelta_pix = 10.;
        bool b_render_hdt = false;
        double dist = sqrt(pow((double)(lHeadPoint.x - lPredPoint.x), 2) + pow((double)(lHeadPoint.y - lPredPoint.y), 2));
        if(dist > ndelta_pix)
              b_render_hdt = true;

//    Another draw test ,based on pixels, assuming the ship icon is a fixed nominal size
//    and is just barely outside the viewport        ....
        wxBoundingBox bb_screen(0, 0, VPoint.pix_width, VPoint.pix_height);
        if(bb_screen.PointInBox(lShipPoint, 20))
              drawit++;

        // And one more test to catch the case where COG line crosses the screen,
        // but ownship and pred point are both off

        if(VPoint.vpBBox.LineIntersect(wxPoint2DDouble(gLon, gLat),
                        wxPoint2DDouble( pred_lon, pred_lat)))
              drawit++;

//    Do the draw if either the ship or prediction is within the current VPoint
        if ( drawit )
        {

#ifdef USE_PNG_OWNSHIP
                wxImage *pos_image;
                wxColour pred_colour;

                if(g_bUseGreenShip)
                {
                        pos_image = m_pos_image_green;
                        pred_colour = GetGlobalColor ( _T ( "GREEN2" ) );

                        if(SHIP_NORMAL != m_ownship_state)
                        {
                            pos_image = m_pos_image_red;
                            pred_colour = GetGlobalColor ( _T ( "UDKRD" ) );
                        }
                }
                else
                {
                        pos_image = m_pos_image_red;
                        pred_colour = GetGlobalColor ( _T ( "URED" ) );

                        if(SHIP_NORMAL != m_ownship_state)
                        {
                              pos_image = m_pos_image_grey;
                              pred_colour = GetGlobalColor ( _T ( "GREY1" ) );
                        }
                }
                int img_width = pos_image->GetWidth();
                int img_height = pos_image->GetHeight();


                //      calculate true ship size in pixels
                int ship_pix = (int)/*rint*/(g_n_ownship_meters * VPoint.view_scale_ppm);

                //      Grow the ship icon if needed, will only happen on big overzoom
                if(ship_pix > img_height)
                {
                      //      Make a new member image under some conditions
                      if((m_cur_ship_pix != ship_pix) || ((SHIP_NORMAL == m_ownship_state) && m_cur_ship_pix_isgrey) || !m_ship_pix_image.IsOk())
                      {
                        int nh = ship_pix;
                        int nw = img_width * ship_pix / img_height;
                        m_ship_pix_image =   pos_image->Scale(nw, nh, wxIMAGE_QUALITY_HIGH);
                        m_cur_ship_pix_isgrey = (SHIP_NORMAL != m_ownship_state);
                        m_cur_ship_pix = ship_pix;
                      }
                      pos_image = &m_ship_pix_image;
                }

                //      Draw the ownship icon
                if(VPoint.chart_scale < 300000)             // According to S52, this should be 50,000
                {

                        wxPoint rot_ctr(1 + img_width/2, 1 + img_height/2);

                        wxImage rot_image = pos_image->Rotate(-(icon_rad - (PI / 2.)), rot_ctr);
                        wxBitmap os_bm(rot_image);
                        wxMemoryDC mdc(os_bm);

                        int w =  os_bm.GetWidth();
                        int h = os_bm.GetHeight();
                        dc.Blit(lShipPoint.x - w/2, (lShipPoint.y - h/2) , w, h, &mdc, 0, 0, wxCOPY, true);

                        dc.CalcBoundingBox( lShipPoint.x - w/2, lShipPoint.y - h/2 );
                        dc.CalcBoundingBox( lShipPoint.x - w/2 + w, lShipPoint.y - h/2 + h );        // Maintain dirty box,, missing in __WXMSW__ library
                }
                else
                {
                      dc.SetPen ( wxPen ( pred_colour , 2 ) );

                      if(SHIP_NORMAL == m_ownship_state)
                            dc.SetBrush ( wxBrush ( pred_colour, wxTRANSPARENT ) );
                      else
                            dc.SetBrush ( wxBrush ( GetGlobalColor ( _T ( "YELO1" )) ) );

                      dc.DrawEllipse ( lShipPoint.x - 10, lShipPoint.y - 10, 20, 20 );
                      dc.DrawEllipse ( lShipPoint.x -  6, lShipPoint.y -  6, 12, 12 );

                      dc.DrawLine ( lShipPoint.x - 12, lShipPoint.y, lShipPoint.x + 12, lShipPoint.y);
                      dc.DrawLine ( lShipPoint.x, lShipPoint.y - 12, lShipPoint.x, lShipPoint.y + 12);

                }



                bool b_render_cog = true;
                if(lpp < img_height/2)                  // don't draw predictors if they are shorter than the ship
                      b_render_cog = false;

#if wxUSE_GRAPHICS_CONTEXT
                wxGraphicsContext *pgc = NULL;

                wxMemoryDC *pmdc = wxDynamicCast(&dc, wxMemoryDC);
                if(pmdc)
                {
                      pgc = wxGraphicsContext::Create(*pmdc);
                }
                else
                {
                      wxClientDC *pcdc = wxDynamicCast(&dc, wxClientDC);
                      if(pcdc)
                            pgc = wxGraphicsContext::Create(*pcdc);
                }


                if(pgc)
                {
                      if(b_render_cog)
                      {
                              //      COG Predictor
                              wxDash dash_long[2];
                              dash_long[0] = ( int ) ( 3.0 * m_pix_per_mm );  //8// Long dash  <---------+
                              dash_long[1] = ( int ) ( 1.5 * m_pix_per_mm );  //2// Short gap            |

                              wxPen ppPen2 ( pred_colour, 3, wxUSER_DASH );
                              ppPen2.SetDashes( 2, dash_long );
                              pgc->SetPen(ppPen2);
                              pgc->StrokeLine(lShipPoint.x, lShipPoint.y, lPredPoint.x, lPredPoint.y);


                              wxDash dash_long3[2];
                              dash_long3[0] = 3 * dash_long[0];
                              dash_long3[1] = 3 * dash_long[1];

                              wxPen ppPen3 ( GetGlobalColor ( _T ( "UBLCK" ) ), 1, wxUSER_DASH );
                              ppPen3.SetDashes( 2, dash_long3 );
                              pgc->SetPen(ppPen3);
                              pgc->StrokeLine(lShipPoint.x, lShipPoint.y, lPredPoint.x, lPredPoint.y);


                              dc.CalcBoundingBox(lShipPoint.x, lShipPoint.y);               // keep dc dirty box up-to-date
                              dc.CalcBoundingBox(lPredPoint.x, lPredPoint.y);



                              double png_pred_icon_scale_factor = .40;

                              wxPoint icon[10];

                              for ( int i=0; i<4; i++ )
                              {
                                    int j = i * 2;
                                    double pxa = ( double ) (s_png_pred_icon[j]  );
                                    double pya = ( double ) (s_png_pred_icon[j+1]);

                                    pya *=  png_pred_icon_scale_factor;
                                    pxa *=  png_pred_icon_scale_factor;

                                    double px = ( pxa * sin ( cog_rad ) ) + ( pya * cos ( cog_rad ) );
                                    double py = ( pya * sin ( cog_rad ) ) - ( pxa * cos ( cog_rad ) );


                                    icon[i].x = ( int ) ( px ) + lPredPoint.x;
                                    icon[i].y = ( int ) ( py ) + lPredPoint.y;
                              }


                              wxGraphicsPath gpath = pgc->CreatePath();

                              gpath.MoveToPoint(icon[0].x,icon[0].y);
                              gpath.AddLineToPoint(icon[1].x,icon[1].y);
                              gpath.AddLineToPoint(icon[2].x,icon[2].y);
                              gpath.AddLineToPoint(icon[3].x,icon[3].y);
                              gpath.AddLineToPoint(icon[0].x,icon[0].y);

                              wxPen ppPen1 ( GetGlobalColor ( _T ( "UBLCK" ) ), 1, wxSOLID );
                              pgc->SetPen(ppPen1);

                              pgc->SetBrush(wxBrush(pred_colour)); //*wxWHITE_BRUSH);

                              pgc->FillPath(gpath);
                              pgc->StrokePath(gpath);

                              dc.CalcBoundingBox(icon[0].x,icon[0].y);              // keep dc dirty box up-to-date
                              dc.CalcBoundingBox(icon[1].x,icon[1].y);
                              dc.CalcBoundingBox(icon[2].x,icon[2].y);
                              dc.CalcBoundingBox(icon[3].x,icon[3].y);
                              dc.CalcBoundingBox(icon[0].x,icon[0].y);

                      }

                      //      HDT Predictor
                      if(b_render_hdt)
                      {
                            wxDash dash_short[2];
                            dash_short[0] = ( int ) ( 1.5 * m_pix_per_mm );  // Short dash  <---------+
                            dash_short[1] = ( int ) ( 1.8 * m_pix_per_mm );  // Short gap            |

                            wxPen ppPen2 ( pred_colour, 1, wxUSER_DASH );
                            ppPen2.SetDashes( 2, dash_short );

                            pgc->SetPen(ppPen2);

                            pgc->StrokeLine(lShipPoint.x, lShipPoint.y, lHeadPoint.x, lHeadPoint.y);
                            dc.CalcBoundingBox(lHeadPoint.x, lHeadPoint.y);               // keep dc dirty box up-to-date

                            wxGraphicsPath gpath = pgc->CreatePath();

                            wxPen ppPen1 ( pred_colour, 2, wxSOLID );
                            pgc->SetPen(ppPen1);
                            pgc->SetBrush(wxBrush(GetGlobalColor ( _T ( "GREY2" ) )));

                            gpath.AddCircle(lHeadPoint.x, lHeadPoint.y, 4);
                            pgc->FillPath(gpath);
                            pgc->StrokePath(gpath);

                            dc.CalcBoundingBox(lHeadPoint.x+6, lHeadPoint.y+6);               // keep dc dirty box up-to-date
                            dc.CalcBoundingBox(lHeadPoint.x-6, lHeadPoint.y-6);               // keep dc dirty box up-to-date

                      }


                      delete pgc;
                }
#else       //wxGraphicsContext

                if(b_render_cog)
                {
                        wxPen ppPen2 ( pred_colour, 2, wxSOLID );
                        dc.SetPen ( ppPen2 );
                        dc.DrawLine ( lShipPoint.x, lShipPoint.y, lPredPoint.x, lPredPoint.y );

                        double png_pred_icon_scale_factor = 0.5;

                        wxPoint icon[10];

                        for ( int i=0; i<4; i++ )
                        {
                              int j = i * 2;
                              double pxa = ( double ) (s_png_pred_icon[j]  );
                              double pya = ( double ) (s_png_pred_icon[j+1]);

                              pya *=  png_pred_icon_scale_factor;
                              pxa *=  png_pred_icon_scale_factor;

                              double px = ( pxa * sin ( cog_rad ) ) + ( pya * cos ( cog_rad ) );
                              double py = ( pya * sin ( cog_rad ) ) - ( pxa * cos ( cog_rad ) );


                              icon[i].x = ( int ) ( px ) + lPredPoint.x;
                              icon[i].y = ( int ) ( py ) + lPredPoint.y;
                        }

                        wxPen ppPen1 ( pred_colour, 2, wxSOLID );
                        dc.SetPen ( ppPen1 );

                        dc.DrawLine ( icon[0].x,icon[0].y, icon[1].x ,icon[1].y );
                        dc.DrawLine ( icon[1].x,icon[1].y, icon[2].x ,icon[2].y  );
                        dc.DrawLine ( icon[2].x,icon[2].y, icon[3].x ,icon[3].y );
                        dc.DrawLine ( icon[3].x,icon[3].y, icon[0].x ,icon[0].y  );

                }

                //      HDT Predictor
                if(b_render_hdt)
                {
                      wxPen ppPen2 ( pred_colour, 1, wxSOLID );
                      dc.SetPen ( ppPen2 );
                      dc.SetBrush(wxBrush(GetGlobalColor ( _T ( "GREY2" ) )));

                      dc.DrawLine ( lShipPoint.x, lShipPoint.y, lHeadPoint.x, lHeadPoint.y );
                      dc.DrawCircle(lHeadPoint.x, lHeadPoint.y, 4);
                }
#endif

#else  //USE_PNG_OWNSHIP
                //      Establish ship color
                 //     It changes color based on GPS and Chart accuracy/availability
                wxColour ship_color(GetGlobalColor ( _T ( "URED" )));         // default is OK

                if(SHIP_LOWACCURACY == m_ownship_state)
                {
                      ship_color = GetGlobalColor ( _T ( "YELO1" ) );
                }

                if(SHIP_INVALID == m_ownship_state)
                {
                      ship_color = GetGlobalColor ( _T ( "YELO1" ) );
                }

                dc.SetBrush ( wxBrush ( ship_color ) );

                //      Draw the COG predictor

                //      First, however, scale the predictor icon by an empirical factor
                //      If the predictor length is less than 10 mm, scale further
                GetClientSize ( &canvas_width, &canvas_height );
                double x_mm = wxGetDisplaySizeMM().GetWidth();         // gives client width in mm
                double pix_per_mm = canvas_width / x_mm;

                double llmm = lpp / pix_per_mm;

                double pred_icon_scale_factor = 1.25;
                if(llmm < 10)
                      pred_icon_scale_factor *= llmm / 10;



                wxPen ppPen2 ( GetGlobalColor ( _T ( "UBLCK" ) ), 3, wxSOLID );
                dc.SetPen ( ppPen2 );
                dc.DrawLine ( lShipPoint.x, lShipPoint.y, lPredPoint.x, lPredPoint.y );

                wxPoint icon[10];

                for ( int i=0; i<8; i++ )
                {
                      int j = i * 2;
                      double pxa = ( double ) (s_pred_icon[j]  );
                      double pya = ( double ) (s_pred_icon[j+1]);

                      pya *=  pred_icon_scale_factor;
                      pxa *=  pred_icon_scale_factor;

                      double px = ( pxa * sin ( cog_rad ) ) + ( pya * cos ( cog_rad ) );
                      double py = ( pya * sin ( cog_rad ) ) - ( pxa * cos ( cog_rad ) );


                      icon[i].x = ( int ) ( px ) + lPredPoint.x;
                      icon[i].y = ( int ) ( py ) + lPredPoint.y;
                }

                wxPen ppPen1 ( GetGlobalColor ( _T ( "UBLCK" ) ), 3, wxSOLID );
                dc.SetPen ( ppPen1 );

                dc.DrawLine ( icon[0].x,icon[0].y, icon[1].x ,icon[1].y );
                dc.DrawLine ( icon[2].x,icon[2].y, icon[3].x ,icon[3].y  );
                dc.DrawLine ( icon[4].x,icon[4].y, icon[5].x ,icon[5].y );
                dc.DrawLine ( icon[6].x,icon[6].y, icon[7].x ,icon[7].y  );

                //      Now draw the ownship icon

                if(VPoint.chart_scale < 50000)
                {
                        wxPoint ownship_icon[10];
                        for ( int i=0; i<10 ; i++ )
                        {
                              int j = i * 2;
                              double pxa = ( double ) (s_ownship_icon[j]  );
                              double pya = ( double ) (s_ownship_icon[j+1]);
                                pya /= 1.5;
                                pxa /= 1.5;

                                double px = ( pxa * sin ( icon_rad ) ) + ( pya * cos ( icon_rad ) );
                                double py = ( pya * sin ( icon_rad ) ) - ( pxa * cos ( icon_rad ) );

                                ownship_icon[i].x = ( int ) ( px ) + lShipPoint.x;
                                ownship_icon[i].y = ( int ) ( py ) + lShipPoint.y;
                        }

                        wxPen ppPen1 ( GetGlobalColor ( _T ( "UBLCK" ) ), 1, wxSOLID );
                        dc.SetPen ( ppPen1 );
                        dc.DrawPolygon ( 6, &ownship_icon[0], 0, 0 );

                        //                 //     draw reference point cross
                        dc.DrawLine ( ownship_icon[6].x ,ownship_icon[6].y, ownship_icon[7].x,ownship_icon[7].y );
                        dc.DrawLine ( ownship_icon[8].x ,ownship_icon[8].y, ownship_icon[9].x,ownship_icon[9].y );
                }
                else
                {
                      dc.SetPen ( wxPen ( GetGlobalColor ( _T ( "URED" ) ) , 2 ) );

                      if(SHIP_NORMAL == m_ownship_state)
                            dc.SetBrush ( wxBrush ( ship_color, wxTRANSPARENT ) );
                      else
                            dc.SetBrush ( wxBrush ( GetGlobalColor ( _T ( "YELO1" )) ) );

                      dc.DrawEllipse ( lShipPoint.x - 10, lShipPoint.y - 10, 20, 20 );
                      dc.DrawEllipse ( lShipPoint.x -  6, lShipPoint.y -  6, 12, 12 );
                }

#endif      //USE_PNG_OWNSHIP


//    Test code to draw CEP circle based on chart scale
/*
                double radius = 25;
                double radius_meters = Current_Ch->GetNativeScale() * .0015;         // 1.5 mm at original scale
                radius = radius_meters * VPoint.view_scale_ppm;
                dc.DrawCircle(lShipPoint.x, lShipPoint.y, (int)radius);
*/
                        // Draw radar rings if activated
                if (g_bNavAidShowRadarRings)
                {
                      double factor = 1.00;
                      if (g_pNavAidRadarRingsStepUnits == 1)          // nautical miles
                            factor = 1 / 1.852;

                      factor *= g_fNavAidRadarRingsStep;

                      double tlat, tlon;
                      wxPoint r;
                      ll_gc_ll ( gLat, gLon, 0, factor, &tlat, &tlon );
                      GetCanvasPointPix ( tlat, tlon, &r );

                      double lpp = sqrt(pow((double)(lShipPoint.x - r.x), 2) + pow((double)(lShipPoint.y - r.y), 2));
                      int pix_radius = (int)lpp;

#if wxUSE_GRAPHICS_CONTEXT

                      wxGraphicsContext *pgc = NULL;

                      wxMemoryDC *pmdc = wxDynamicCast(&dc, wxMemoryDC);
                      if(pmdc)
                      {
                            pgc = wxGraphicsContext::Create(*pmdc);
                      }
                      else
                      {
                            wxClientDC *pcdc = wxDynamicCast(&dc, wxClientDC);
                            if(pcdc)
                                  pgc = wxGraphicsContext::Create(*pcdc);
                      }


                      if(pgc)
                      {
                            wxPen ppPen1 ( GetGlobalColor ( _T ( "URED" ) ) , 2 );
                            pgc->SetPen(ppPen1);

                            for (int i=1; i<=g_iNavAidRadarRingsNumberVisible;i++)
                            {
                                  int RadiusRing;
                                  RadiusRing = i * pix_radius;

                                  wxGraphicsPath gpath = pgc->CreatePath();

                                  gpath.AddCircle(lShipPoint.x, lShipPoint.y, RadiusRing);
                                  pgc->StrokePath(gpath);

                                  dc.CalcBoundingBox(lShipPoint.x + RadiusRing , lShipPoint.y + RadiusRing);               // keep dc dirty box up-to-date
                                  dc.CalcBoundingBox(lShipPoint.x - RadiusRing , lShipPoint.y - RadiusRing);
                            }
                      }

#else
                      dc.SetPen ( wxPen ( GetGlobalColor ( _T ( "URED" ) ) , 2 ) );

                      wxBrush CurrentBrush = dc.GetBrush();
                      wxBrush RingBrush(CurrentBrush.GetColour(),wxTRANSPARENT);
                      dc.SetBrush(RingBrush);
                      for (int i=1; i<=g_iNavAidRadarRingsNumberVisible;i++)
                      {
                            int RadiusRing;
                            RadiusRing = i * pix_radius;
                            dc.DrawCircle(lShipPoint.x, lShipPoint.y, RadiusRing);
                      }
                      dc.SetBrush(CurrentBrush);
#endif
                }
        }         // if drawit

        //  Test code to validate the dc drawing rectangle....
/*
               //  Retrieve the drawing extents
        wxRect ship_rect ( dc.MinX(),
                           dc.MinY(),
                           dc.MaxX() - dc.MinX(),
                           dc.MaxY() - dc.MinY() );

        dc.SetPen(wxPen(*wxRED));
        dc.SetBrush(wxBrush(*wxRED, wxTRANSPARENT));
        dc.DrawRectangle(ship_rect);
*/
}


void ChartCanvas::ScaleBarDraw( wxDC& dc, int x_origin, int y_origin )
{
      double blat, blon, tlat, tlon;
      wxPoint r;

      if(VPoint.chart_scale > 80000)        // Draw 10 mile scale as SCALEB11
      {
            GetCanvasPixPoint ( x_origin, y_origin, blat, blon );
            ll_gc_ll ( blat, blon, 0, 10.0, &tlat, &tlon );
            GetCanvasPointPix ( tlat, tlon, &r );

            int l1 = (y_origin - r.y) / 5;

            wxPen pen1 ( GetGlobalColor ( _T ( "SNDG2" ) ), 3, wxSOLID );
            wxPen pen2 ( GetGlobalColor ( _T ( "SNDG1" ) ), 3, wxSOLID );

            for(int i=0 ; i < 5 ; i++)
            {
                  int y = l1 * i;
                  if( i & 1)
                        dc.SetPen(pen1);
                  else
                        dc.SetPen(pen2);

                  dc.DrawLine(x_origin, y_origin - y, x_origin, y_origin - (y + l1));
            }
      }
      else                                // Draw 1 mile scale as SCALEB10
      {
            GetCanvasPixPoint ( x_origin, y_origin, blat, blon );
            ll_gc_ll ( blat, blon, 0, 1.0, &tlat, &tlon );
            GetCanvasPointPix ( tlat, tlon, &r );

            int l1 = (y_origin - r.y) / 10;

            wxPen pen1 ( GetGlobalColor ( _T ( "SCLBR" ) ), 3, wxSOLID );
            wxPen pen2 ( GetGlobalColor ( _T ( "CHDRD" ) ), 3, wxSOLID );

            for(int i=0 ; i < 10 ; i++)
            {
                  int y = l1 * i;
                  if( i & 1)
                        dc.SetPen(pen1);
                  else
                        dc.SetPen(pen2);

                  dc.DrawLine(x_origin, y_origin - y, x_origin, y_origin - (y + l1));
            }
      }

}

void ChartCanvas::AISDraw ( wxDC& dc )
{
      if ( !g_pAIS )
            return;

// Toggling AIS display on and off

      if ( !g_bShowAIS )                   // pjotrc 2010.02.09
            return;                      //

      //      Iterate over the AIS Target Hashmap
      AIS_Target_Hash::iterator it;

      AIS_Target_Hash *current_targets = g_pAIS->GetTargetList();

      //    Draw all targets in two pass loop, sorted on SOG
      //    This way, fast targets are not obscured by slow/stationary targets
      for ( it = ( *current_targets ).begin(); it != ( *current_targets ).end(); ++it )
      {
            AIS_Target_Data *td = it->second;
            if(td->SOG < g_ShowMoored_Kts)
                  AISDrawTarget(td, dc);
      }

      for ( it = ( *current_targets ).begin(); it != ( *current_targets ).end(); ++it )
      {
            AIS_Target_Data *td = it->second;
            if(td->SOG >= g_ShowMoored_Kts)
                  AISDrawTarget(td, dc);
      }
}




void ChartCanvas::AISDrawTarget (AIS_Target_Data *td, wxDC& dc )
{

                //      Target data must be valid
            if(NULL == td)
                  return;

                //      Skip anchored/moored (interpreted as low speed) targets if requested
            if ( (!g_bShowMoored) && (td->SOG <= g_ShowMoored_Kts))  // dsr
                        return;

                //      Target data position must be valid
            if(!td->b_positionValid)
                  return;

                 //      Target data speed must be valid
            if(td->SOG > 102.2)
                  return;

            int drawit = 0;
            wxPoint TargetPoint, PredPoint;

                //    Is target in Vpoint?
            if ( VPoint.vpBBox.PointInBox ( td->Lon, td->Lat, 0 ) )
                  drawit++;                                 // yep

                //   Always draw alert targets, even if they are off the screen
            if((td->n_alarm_state == AIS_ALARM_SET) || (td->n_alarm_state == AIS_ALARM_ACKNOWLEDGED))
                  drawit++;

                //    Calculate AIS target Position Predictor, using global static variable for length of vector

            double pred_lat, pred_lon;

            ll_gc_ll ( td->Lat, td->Lon, td->COG, td->SOG * g_ShowCOG_Mins / 60., &pred_lat, &pred_lon );

                //    Is predicted point in the VPoint?
            if ( VPoint.vpBBox.PointInBox ( pred_lon, pred_lat, 0 ) )
                  drawit++;                                 // yep

        // And one more test to catch the case where target COG line crosses the screen,
        // but the target itself and its pred point are both off-screen
            if(VPoint.vpBBox.LineIntersect(wxPoint2DDouble(td->Lon, td->Lat), wxPoint2DDouble( pred_lon, pred_lat)))
                  drawit++;

                //    Do the draw if conditions indicate
            if ( drawit )
            {
                  GetCanvasPointPix ( td->Lat, td->Lon, &TargetPoint );
                  GetCanvasPointPix ( pred_lat, pred_lon, &PredPoint );

                        //  Calculate the relative angle for this chart orientation
                        //  Exception:  if speed is very low, force the target symbol to be rendered at COG 000 (North)
                        //  Another exception:  if g_ShowCOG_Mins is zero, we'll need to use a dummy value to get the
                        //  angle for symbolization.  Say 5 minutes.

                  double theta;
                  wxPoint PredPointAngleCalc;

                  if(g_ShowCOG_Mins > 0)
                  {
                        PredPointAngleCalc = PredPoint;
                  }
                  else
                  {
                        double pred_lat_dummy, pred_lon_dummy;
                        ll_gc_ll ( td->Lat, td->Lon, td->COG, td->SOG * 5.0 / 60., &pred_lat_dummy, &pred_lon_dummy );
                        GetCanvasPointPix ( pred_lat_dummy, pred_lon_dummy, &PredPointAngleCalc );
                  }


                  if( abs( PredPointAngleCalc.x - TargetPoint.x ) > 0 )
                  {
                        if(td->SOG > g_ShowMoored_Kts)
                              theta = atan2 ( (double)( PredPointAngleCalc.y - TargetPoint.y ), (double)( PredPointAngleCalc.x - TargetPoint.x ) );
                        else
                              theta = -PI / 2;
                  }
                  else
                  {
                        if( PredPointAngleCalc.y > TargetPoint.y)
                              theta =  PI / 2.;             // valid COG 180
                        else
                              theta = -PI / 2.;            //  valid COG 000 or speed is too low to resolve course
                  }


                        //    Of course, if the target reported a valid HDG, then use it for icon
                  if((int)(td->HDG) != 511)
                        theta = ((td->HDG - 90 ) * PI / 180.) + VPoint.skew + VPoint.rotation;

                                //  Draw the icon rotated to the COG
                  wxPoint ais_quad_icon[4];   // pjotrc 2010.01.31
                  ais_quad_icon[0].x = -8;
                  ais_quad_icon[0].y = -6;
                  ais_quad_icon[1].x =  0;
                  ais_quad_icon[1].y =  24;
                  ais_quad_icon[2].x =  8;
                  ais_quad_icon[2].y = -6;
                  ais_quad_icon[3].x =  0;
                  ais_quad_icon[3].y = -6;

                           //   If this is an AIS Class B target, so symbolize it differently // pjotrc 2010.01.31
                  if(td->Class == AIS_CLASS_B) ais_quad_icon[3].y = 0;               // pjotrc 2010.01.31

                  for ( int i=0; i<4 ; i++ )                                           // pjotrc 2010.01.31
                  {
                        double px = ( ( double ) ais_quad_icon[i].x ) * sin ( theta ) + ( ( double ) ais_quad_icon[i].y ) * cos ( theta );
                        double py = ( ( double ) ais_quad_icon[i].y ) * sin ( theta ) - ( ( double ) ais_quad_icon[i].x ) * cos ( theta );
                        ais_quad_icon[i].x = (int) round( px );
                        ais_quad_icon[i].y = (int) round( py );
                  }

                  dc.SetPen ( wxPen ( GetGlobalColor ( _T ( "UBLCK" ) ) ) );

                                // Default color is green
                  wxBrush target_brush =  wxBrush ( GetGlobalColor ( _T ( "UINFG" ) ) ) ;

                         //and....
                  if(!td->b_nameValid)
                        target_brush =  wxBrush ( GetGlobalColor ( _T ( "CHYLW" ) ) ) ;

//    Check for alarms here, maintained by AIS class timer tick
                  if((td->n_alarm_state == AIS_ALARM_SET) || (td->n_alarm_state == AIS_ALARM_ACKNOWLEDGED))
                  {
                        target_brush = wxBrush ( GetGlobalColor ( _T ( "URED" ) ) ) ;

                                      //  Calculate the point of CPA for target
                        double tcpa_lat,tcpa_lon;
                        ll_gc_ll ( td->Lat, td->Lon, td->COG, td->SOG * td->TCPA / 60., &tcpa_lat, &tcpa_lon );
                        wxPoint tCPAPoint;
                        wxPoint TPoint = TargetPoint;
                        GetCanvasPointPix ( tcpa_lat, tcpa_lon, &tCPAPoint );


                                      //  Draw the intercept line from target
                        dc.SetPen ( wxPen ( GetGlobalColor ( _T ( "URED" )), 2, wxSHORT_DASH) );
                        ClipResult res = cohen_sutherland_line_clip_i ( &TPoint.x, &TPoint.y, &tCPAPoint.x, &tCPAPoint.y,
                                    0, VPoint.pix_width, 0, VPoint.pix_height );
                        if ( res != Invisible )
                              dc.DrawLine (  TPoint.x, TPoint.y, tCPAPoint.x, tCPAPoint.y );


                                      //  Calculate the point of CPA for ownship
                        double ocpa_lat, ocpa_lon;
                        ll_gc_ll ( gLat, gLon, gCog, gSog * td->TCPA / 60., &ocpa_lat, &ocpa_lon );
                        wxPoint oCPAPoint;

                        GetCanvasPointPix ( ocpa_lat, ocpa_lon, &oCPAPoint );
                        GetCanvasPointPix ( tcpa_lat, tcpa_lon, &tCPAPoint );

                                      //        Save a copy of these
                        wxPoint oCPAPoint_sav = oCPAPoint;
                        wxPoint tCPAPoint_sav = tCPAPoint;

                                      //  Draw a line from target CPA point to ownship CPA point
                        dc.SetPen ( wxPen ( GetGlobalColor ( _T ( "URED" )), 2, wxSHORT_DASH) );
                        ClipResult ores = cohen_sutherland_line_clip_i ( &tCPAPoint.x, &tCPAPoint.y, &oCPAPoint.x, &oCPAPoint.y,
                                    0, VPoint.pix_width, 0, VPoint.pix_height );
                        if ( ores != Invisible )
                        {
                              wxColour yellow = GetGlobalColor ( _T ( "YELO1" ));
                              wxColour yh(yellow.Red(), yellow.Green(), yellow.Blue(), 32);
                              dc.SetPen ( wxPen ( yh, 4) );

                              dc.DrawLine (  tCPAPoint.x, tCPAPoint.y, oCPAPoint.x, oCPAPoint.y );

                              dc.SetPen ( wxPen ( GetGlobalColor ( _T ( "URED" )), 2, wxSHORT_DASH) );
                              dc.DrawLine (  tCPAPoint.x, tCPAPoint.y, oCPAPoint.x, oCPAPoint.y );

                                            //        Draw little circles at the ends of the CPA alert line
                              dc.SetBrush ( wxBrush ( GetGlobalColor ( _T ( "BLUE3" ) ) ) );
                              dc.SetPen ( wxPen ( GetGlobalColor ( _T ( "UBLK" ))) );

                                            //  Using the true ends, not the clipped ends
                              dc.DrawCircle( tCPAPoint_sav, 5);
                              dc.DrawCircle( oCPAPoint_sav, 5);

                        }

                        dc.SetPen ( wxPen ( GetGlobalColor ( _T ( "UBLCK" ) ) ) );
                        dc.SetBrush ( wxBrush ( GetGlobalColor ( _T ( "URED" ) ) ) );
                  }


                          //  Highlight the AIS target symbol if an alert dialog is currently open for it
                  if(g_pais_alert_dialog_active)
                  {
                        if(g_pais_alert_dialog_active->Get_Dialog_MMSI() == td->MMSI)
                              JaggyCircle(dc,  wxPen ( GetGlobalColor ( _T ( "URED" ) ) , 2 ), TargetPoint.x, TargetPoint.y, 100);
                  }


                          //  Highlight the AIS target symbol if a query dialog is currently open for it      // pjotrc 2010.01.31
                  if(g_pais_query_dialog_active)
                  {
                        if(g_pais_query_dialog_active->GetMMSI() == td->MMSI)
                              TargetFrame(dc,  wxPen ( GetGlobalColor ( _T ( "UBLCK" ) ) , 2 ), TargetPoint.x, TargetPoint.y, 25);
                  }


                           //       Render the COG line if the speed is greater than moored speed defined by ais options dialog
                  if((g_bShowCOG) && (td->SOG > g_ShowMoored_Kts))
                  {
                        int pixx =  TargetPoint.x;  // + ais_quad_icon[1].x;
                        int pixy =  TargetPoint.y;  // + ais_quad_icon[1].y;
                        int pixx1 = PredPoint.x;
                        int pixy1 = PredPoint.y;

                                      //  Don't draw the COG line  and predictor point if zoomed far out.... or if target lost/inactive
                        double l = pow(pow((double)(PredPoint.x - TargetPoint.x), 2) + pow((double)(PredPoint.y - TargetPoint.y), 2), 0.5);

                        if(l > 24)
                        {
                              ClipResult res = cohen_sutherland_line_clip_i ( &pixx, &pixy, &pixx1, &pixy1,
                                          0, VPoint.pix_width, 0, VPoint.pix_height );
                              if (( res != Invisible ) && (td->b_active))
                              {
                                    //    Draw a 3 pixel wide line
                                    wxColour cb = target_brush.GetColour();
                                    dc.SetPen ( wxPen ( cb, 3 ));
                                    dc.DrawLine ( pixx, pixy, pixx1, pixy1 );

                                    //    Draw a 1 pixel wide black line
                                    dc.SetPen ( wxPen ( GetGlobalColor ( _T ( "UBLCK" ) ),1 ) );
                                    dc.DrawLine ( pixx, pixy, pixx1, pixy1 );
                              }

                              dc.SetBrush ( target_brush );
                              dc.SetPen ( wxPen ( GetGlobalColor ( _T ( "UBLCK" )), 1) );
                              if (( res != Invisible ) && (td->b_active)) dc.DrawCircle ( PredPoint.x, PredPoint.y, 5 );  // pjotrc 2010.02.01

                                //      Draw RateOfTurn Vector
                              if ( td->ROTAIS && td->b_active)   // pjotrc 2010.02.01
                              {
                                    double nv = 10;
                                    double theta2 = theta;
                                    if ( td->ROTAIS > 0 )
                                          theta2 += PI/2.;
                                    else
                                          theta2 -= PI/2.;

                                    int xrot = ( int ) round ( pixx1 + ( nv * cos ( theta2 ) ) );
                                    int yrot = ( int ) round ( pixy1 + ( nv * sin ( theta2 ) ) );
                                    dc.DrawLine ( pixx1, pixy1, xrot, yrot );
                              }
                        }
                  }

                           //        Actually Draw the target
                  if (td->Class == AIS_ATON) {                       // Aid to Navigation    // pjotrc 2010.02.01
                        AtoN_Diamond(dc,  wxPen ( GetGlobalColor ( _T ( "UBLCK" )) , 2 ), TargetPoint.x, TargetPoint.y, 8);
                  }
                  else {         // ship class A or B
                        dc.SetBrush ( target_brush );
                        dc.SetPen ( wxPen ( GetGlobalColor ( _T ( "UBLCK" )), 1) );

                        dc.DrawPolygon ( 4, ais_quad_icon, TargetPoint.x, TargetPoint.y );         // pjotrc 2010.01.31

                           //        If this is a moored/anchored target, so symbolize it
                        if (((td->NavStatus == MOORED) || (td->NavStatus == AT_ANCHOR)) &&(td->SOG < g_ShowMoored_Kts))   // pjotrc 2010.01.31
                        {
                              dc.SetBrush ( wxBrush ( GetGlobalColor ( _T ( "UBLCK" ) ) ) );
                              dc.DrawCircle ( TargetPoint.x, TargetPoint.y, 5 );
                        }

                                //        Draw the inactive cross-out line                                                   // pjotrc 2010.01.31
                        if(!td->b_active)
                        {
                              dc.SetPen ( wxPen ( GetGlobalColor ( _T ( "UBLCK" )), 2) );

                              wxPoint p1 = transrot(wxPoint(-14, 0), theta, TargetPoint);
                              wxPoint p2 = transrot(wxPoint(14, 0), theta, TargetPoint);
                              dc.DrawLine ( p1.x, p1.y, p2.x, p2.y );

                              dc.SetPen ( wxPen ( GetGlobalColor ( _T ( "UBLCK" )), 1) );
                        }
                  }  // ship class A or B


            }       // drawit
}


void ChartCanvas::JaggyCircle(wxDC &dc, wxPen pen, int x, int y, int radius)
{
      //    Constants?
      double da_min = 2.;
      double da_max = 6.;
      double ra_min = 0.;
      double ra_max = 40.;

      wxPen pen_save = dc.GetPen();

      wxDateTime now = wxDateTime::Now();

//      srand( now.GetTicks());
      srand( 1);
      dc.SetPen(pen);

      int x0, y0, x1, y1;

      x0 = x + radius;                    // Start point
      y0 = y;
      double angle = 0.;
     int i = 0;

      while(angle < 360.)
      {
            double da =  da_min + (((double)rand() / RAND_MAX) * (da_max - da_min));
            angle += da;

            if(angle > 360.)
                  angle = 360.;

            double ra =  ra_min + (((double)rand() / RAND_MAX) * (ra_max - ra_min));

            double r;
            if(i % 1)
                  r = radius + ra;
            else
                  r = radius - ra;


            x1 = (int)(x + cos(angle * PI / 180.)*r);
            y1 = (int)(y + sin(angle * PI / 180.)*r);

            dc.DrawLine(x0, y0, x1, y1);

            x0 = x1;
            y0 = y1;

            i++;

      }

      dc.DrawLine(x + radius, y, x1, y1);             // closure

      dc.SetPen(pen_save);
}

void ChartCanvas::TargetFrame(wxDC &dc, wxPen pen, int x, int y, int radius)   // pjotrc 2010.02.01
{
      //    Constants?
      int gap2 = 2*radius/6;

      wxPen pen_save = dc.GetPen();

      dc.SetPen(pen);

      dc.DrawLine(x-radius, y+gap2, x-radius, y+radius);
      dc.DrawLine(x-radius, y+radius, x-gap2, y+radius);
      dc.DrawLine(x+gap2, y+radius, x+radius, y+radius);
      dc.DrawLine(x+radius, y+radius, x+radius, y+gap2);
      dc.DrawLine(x+radius, y-gap2, x+radius, y-radius);
      dc.DrawLine(x+radius, y-radius, x+gap2, y-radius);
      dc.DrawLine(x-gap2, y-radius, x-radius, y-radius);
      dc.DrawLine(x-radius, y-radius, x-radius, y-gap2);

      dc.SetPen(pen_save);
}

void ChartCanvas::AtoN_Diamond(wxDC &dc, wxPen pen, int x, int y, int radius)  // pjotrc 2010.02.01
{
      //    Constants?
      int gap2 = 2*radius/6;
      int pen_width = pen.GetWidth();

      wxPen pen_save = dc.GetPen();

      dc.SetPen(pen);   // draw diamond

      dc.DrawLine(x-radius, y, x, y+radius);
      dc.DrawLine(x, y+radius, x+radius, y);
      dc.DrawLine(x+radius, y, x, y-radius);
      dc.DrawLine(x, y-radius, x-radius, y);

      if (pen_width > 1)
      { pen_width -= 1; pen.SetWidth(pen_width); }    // draw cross inside

      dc.DrawLine(x-gap2, y, x+gap2, y);
      dc.DrawLine(x, y-gap2, x, y+gap2);

      dc.SetPen(pen_save);
}

void ChartCanvas::AlertDraw ( wxDC& dc )                     // pjotrc  2010.02.22
{
// Just for prototyping, visual alert for anchorwatch goes here          2010.02.17 pjotrc
      if (pAnchorWatchPoint1 && AnchorAlertOn1) {
            if (AnchorAlertOn1) {
                  wxPoint TargetPoint;
                  GetCanvasPointPix ( pAnchorWatchPoint1->m_lat, pAnchorWatchPoint1->m_lon, &TargetPoint );
                  JaggyCircle(dc, wxPen(GetGlobalColor(_T("URED")),2), TargetPoint.x, TargetPoint.y, 100);
                  wxSound AIS_Sound(g_sAIS_Alert_Sound_File);
                  if(AIS_Sound.IsOk())
                        AIS_Sound.Play();

            }
      } else AnchorAlertOn1 = false;

      if (pAnchorWatchPoint2 && AnchorAlertOn2) {
            if (AnchorAlertOn2) {
                  wxPoint TargetPoint;
                  GetCanvasPointPix ( pAnchorWatchPoint2->m_lat, pAnchorWatchPoint2->m_lon, &TargetPoint );
                  JaggyCircle(dc, wxPen(GetGlobalColor(_T("URED")),2), TargetPoint.x, TargetPoint.y, 100);
                  wxSound AIS_Sound(g_sAIS_Alert_Sound_File);
                  if(AIS_Sound.IsOk())
                        AIS_Sound.Play();
            }
      } else AnchorAlertOn2 = false;

}
// End of prototype anchor watch alerting-----------------------

void ChartCanvas::UpdateShips()
{
        //  Get the rectangle in the current dc which bounds the "ownship" symbol

        //  Use this dc
        wxClientDC dc ( this );

        // Get dc boundary
        int sx, sy;
        dc.GetSize ( &sx, &sy );

        //  Need a bitmap
        wxBitmap test_bitmap ( sx, sy,  -1 );

        // Create a memory DC
        wxMemoryDC temp_dc;
        temp_dc.SelectObject ( test_bitmap );

        temp_dc.ResetBoundingBox();
        temp_dc.DestroyClippingRegion();
        temp_dc.SetClippingRegion ( wxRect ( 0,0,sx,sy ) );

        // Draw the ownship on the temp_dc
        ShipDraw ( temp_dc );

        //  Retrieve the drawing extents
        wxRect ship_rect ( temp_dc.MinX(),
                           temp_dc.MinY(),
                           temp_dc.MaxX() - temp_dc.MinX(),
                           temp_dc.MaxY() - temp_dc.MinY() );


        wxRect own_ship_update_rect = ship_draw_rect;

        if ( !ship_rect.IsEmpty() )
        {
                ship_rect.Inflate ( 2 );                // clear all drawing artifacts

                //  The required invalidate rectangle is the union of the last drawn rectangle
                //  and this drawn rectangle
                own_ship_update_rect.Union ( ship_rect );
        }

        RefreshRect ( own_ship_update_rect, false );

        //  Save this rectangle for next time
        ship_draw_rect = ship_rect;

        temp_dc.SelectObject ( wxNullBitmap );      // clean up

}

void ChartCanvas::UpdateAlerts()    // pjotrc 2010.02.22
{
        //  Get the rectangle in the current dc which bounds the detected Alert targets

        //  Use this dc
      wxClientDC dc ( this );

        // Get dc boundary
      int sx, sy;
      dc.GetSize ( &sx, &sy );

        //  Need a bitmap
      wxBitmap test_bitmap ( sx, sy,  -1 );

        // Create a memory DC
      wxMemoryDC temp_dc;
      temp_dc.SelectObject ( test_bitmap );

      temp_dc.ResetBoundingBox();
      temp_dc.DestroyClippingRegion();
      temp_dc.SetClippingRegion ( wxRect ( 0,0,sx,sy ) );

        // Draw the Alert Targets on the temp_dc
      AlertDraw ( temp_dc );

        //  Retrieve the drawing extents
      wxRect alert_rect ( temp_dc.MinX(),
                          temp_dc.MinY(),
                                       temp_dc.MaxX() - temp_dc.MinX(),
                                                   temp_dc.MaxY() - temp_dc.MinY() );

      if ( !alert_rect.IsEmpty() )
            alert_rect.Inflate ( 2 );                // clear all drawing artifacts


      if ( !alert_rect.IsEmpty() || !alert_draw_rect.IsEmpty() )
      {
                //  The required invalidate rectangle is the union of the last drawn rectangle
                //  and this drawn rectangle
            wxRect alert_update_rect = alert_draw_rect;
            alert_update_rect.Union ( alert_rect );

                //  Invalidate the rectangular region
            RefreshRect ( alert_update_rect, false );
      }

        //  Save this rectangle for next time
      alert_draw_rect = alert_rect;

      temp_dc.SelectObject ( wxNullBitmap );      // clean up

}


void ChartCanvas::UpdateAIS()
{
        //  Get the rectangle in the current dc which bounds the detected AIS targets

        //  Use this dc
        wxClientDC dc ( this );

        // Get dc boundary
        int sx, sy;
        dc.GetSize ( &sx, &sy );

        //  Need a bitmap
        wxBitmap test_bitmap ( sx, sy,  -1 );

        // Create a memory DC
        wxMemoryDC temp_dc;
        temp_dc.SelectObject ( test_bitmap );

        temp_dc.ResetBoundingBox();
        temp_dc.DestroyClippingRegion();
        temp_dc.SetClippingRegion ( wxRect ( 0,0,sx,sy ) );

        // Draw the AIS Targets on the temp_dc
        AISDraw ( temp_dc );

        //  Retrieve the drawing extents
        wxRect ais_rect ( temp_dc.MinX(),
                          temp_dc.MinY(),
                          temp_dc.MaxX() - temp_dc.MinX(),
                          temp_dc.MaxY() - temp_dc.MinY() );

        if ( !ais_rect.IsEmpty() )
                ais_rect.Inflate ( 2 );                // clear all drawing artifacts


        if ( !ais_rect.IsEmpty() || !ais_draw_rect.IsEmpty() )
        {
                //  The required invalidate rectangle is the union of the last drawn rectangle
                //  and this drawn rectangle
                wxRect ais_update_rect = ais_draw_rect;
                ais_update_rect.Union ( ais_rect );

                //  Invalidate the rectangular region
                RefreshRect ( ais_update_rect, false );
        }

        //  Save this rectangle for next time
        ais_draw_rect = ais_rect;

        temp_dc.SelectObject ( wxNullBitmap );      // clean up

}

void ChartCanvas::OnActivate ( wxActivateEvent& event )
{
        ReloadVP();
}

void ChartCanvas::OnSize ( wxSizeEvent& event )
{
//    Stop any background rendering unilaterally
        FlushBackgroundRender();

        GetClientSize ( &m_canvas_width, &m_canvas_height );

//    Get some canvas metrics

//          Rescale to current value, in order to rebuild VPoint data structures
//          for new canvas size
        SetVPScale ( GetVPScale() );

        double display_size_meters =  wxGetDisplaySizeMM().GetWidth() / 1000.;         // gives client width in meters
        m_canvas_scale_factor = m_canvas_width / display_size_meters;

#ifdef USE_S57
        float pix_per_mm = m_canvas_width / (display_size_meters * 1000.);
        if ( ps52plib )
                ps52plib->SetPPMM ( pix_per_mm );
#endif


//    Set up the scroll margins
        xr_margin = m_canvas_width  * 95/100;
        xl_margin = m_canvas_width  * 5/100;
        yt_margin = m_canvas_height * 5/100;
        yb_margin = m_canvas_height * 95/100;


//    Resize the current viewport

        VPoint.pix_width = m_canvas_width;
        VPoint.pix_height = m_canvas_height;

        // Resize the scratch BM
        delete pscratch_bm;
        pscratch_bm = new wxBitmap ( VPoint.pix_width, VPoint.pix_height,  -1 );

        // Resize the Route Calculation BM
        m_dc_route.SelectObject ( wxNullBitmap );
        delete proute_bm;
        proute_bm = new wxBitmap ( VPoint.pix_width, VPoint.pix_height,  -1 );
        m_dc_route.SelectObject ( *proute_bm );

        //  Resize the PixelCache
        delete m_pPix;
        m_pPix = new PixelCache(VPoint.pix_width, VPoint.pix_height,  BPP );

//  Rescale again, to capture all the changes for new canvas size
        SetVPScale ( GetVPScale() );

        m_cache_vp.Invalidate();

        //  Invalidate the whole window
        ReloadVP();
}


void ChartCanvas::ShowChartInfoWindow(int x, int y, int dbIndex)
{
      if(dbIndex >= 0)
      {
            if(NULL == m_pCIWin)
            {
                  m_pCIWin = new ChInfoWin(this);
                  m_pCIWin->Hide();
            }


            if(!m_pCIWin->IsShown())
            {
                  wxString s;
                  ChartBase *pc = NULL;

                  if((ChartData->IsChartInCache(dbIndex)) && ChartData->IsValid())
                        pc = ChartData->OpenChartFromDB(dbIndex, FULL_INIT);   // this must come from cache

                  s = ChartData->GetFullChartInfo(pc, dbIndex, 50);

                  m_pCIWin->SetString(s);

                  m_pCIWin->SetWinSize(wxSize(400, 250));
                  m_pCIWin->SetPosition(wxPoint(x, y));
                  m_pCIWin->SetBitmap();
                  m_pCIWin->Refresh();
                  m_pCIWin->Show();

            }
      }
      else
            HideChartInfoWindow();
}

void ChartCanvas::HideChartInfoWindow(void)
{
      if(m_pCIWin && m_pCIWin->IsShown())
            m_pCIWin->Hide();
}


void ChartCanvas::PanTimerEvent ( wxTimerEvent& event )
{
        wxMouseEvent ev ( wxEVT_MOTION );
        ev.m_x = mouse_x;
        ev.m_y = mouse_y;
        ev.m_leftDown = mouse_leftisdown;

        wxEvtHandler *evthp = GetEventHandler();

        ::wxPostEvent ( evthp, ev );


}

bool ChartCanvas::CheckEdgePan ( int x, int y )
{
      bool bft = false;
      int pan_margin = 20;
      int pan_timer_set = 100;
      double pan_delta = VPoint.pix_width / 50;

      wxPoint np(VPoint.pix_width  / 2, VPoint.pix_height / 2 );

      if ( x > m_canvas_width - pan_margin )
      {
            np.x += (int)pan_delta;
            bft = true;
      }

      else if ( x < pan_margin )
      {
            np.x -= (int)pan_delta;
            bft = true;
      }

      if ( y < pan_margin )
      {
            np.y -= (int)pan_delta;
            bft = true;
      }

      else if ( y > m_canvas_height - pan_margin )
      {
            np.y += (int)pan_delta;
            bft = true;
      }

      double new_lat, new_lon;
      GetCanvasPixPoint ( np.x, np.y, new_lat, new_lon );

        if ( ( bft ) && !pPanTimer->IsRunning() )
        {
              if(new_lon > 360.) new_lon -= 360.;
              if(new_lon < -360.) new_lon += 360.;

                SetViewPoint ( new_lat, new_lon, VPoint.view_scale_ppm, VPoint.skew, VPoint.rotation, FORCE_SUBSAMPLE );
                Refresh ( false );

                vLat = new_lat;
                vLon = new_lon;

                ClearbFollow();      // update the follow flag

                pPanTimer->Start ( pan_timer_set, wxTIMER_ONE_SHOT );
                return true;
        }

        //    This mouse event must not be due to pan timer event injector
        //    Mouse is out of the pan zone, so prevent any orphan event injection
        if ( ( !bft ) && pPanTimer->IsRunning() )
        {
                pPanTimer->Stop();
        }

        return ( false );
}

void ChartCanvas::MouseEvent ( wxMouseEvent& event )
{
        int x,y;
        int mx, my;

        event.GetPosition ( &x, &y );

        //  This logic is necessary on MSW to handle the case where
        //  a context (right-click) menu is dismissed without action
        //  by clicking on the chart surface.
        //  We need to avoid an unintentional pan by eating some clicks...
#ifdef __WXMSW__
        if(event.IsButton() || event.Dragging())
        {
            if(g_click_stop > 0)
            {
              g_click_stop--;
              return;
            }
        }
#endif

        //    pause any underway background rendering operation
        //    and re-trigger the one-shot timer to be sure

        if ( m_bBackRender )
        {
                m_bbr_paused = true;
                pRescaleTimer->Start ( m_rescale_timer_msec, wxTIMER_ONE_SHOT );
        }

        //  Kick off the Rotation control timer
        if(g_bCourseUp)
        {
            m_b_rot_hidef = false;
            pRotDefTimer->Start ( 500, wxTIMER_ONE_SHOT );
        }


        mouse_x = x;
        mouse_y = y;
        mouse_leftisdown = event.LeftIsDown();

//    Retrigger the rescale timer
        if ( pRescaleTimer->IsRunning() )
                pRescaleTimer->Start ( m_rescale_timer_msec, wxTIMER_ONE_SHOT );

//  Retrigger the cursor tracking timer
        pCurTrackTimer->Start ( m_curtrack_timer_msec, wxTIMER_ONE_SHOT );

        mx = x;
        my = y;
        GetCanvasPixPoint ( x, y, m_cursor_lat, m_cursor_lon );

        //    Calculate meaningful SelectRadius
        float SelectRadius;
        int sel_rad_pix = 8;
        SelectRadius = sel_rad_pix/ ( m_true_scale_ppm * 1852 * 60 );  // Degrees, approximately

#ifdef __WXMSW__
        if ( console->IsShown() )
        {
                wxRegion rgn_console ( console->GetRect() );
                if ( rgn_console.Contains ( x,y )  == wxInRegion )
                {
//                wxLogMessage(_T("chcanv::MouseEvent invoking concanv::CaptureMouse"));
                        console->CaptureMouse();
                        return;
                }
        }
#endif

//      Show cursor position on Status Bar, if present

//      except for GTK, under which status bar updates are very slow
//      due to Update() call.
//      In this case, as a workaround, update the status window
//      after an interval timer (pCurTrackTimer) pops, which will happen
//      whenever the mouse has stopped moving for specified interval.
//      See the method OnCursorTrackTimerEvent()

#ifndef __WXGTK__
        {
                if ( parent_frame->m_pStatusBar )
                {
                      double show_cursor_lon = m_cursor_lon;
                      double show_cursor_lat = m_cursor_lat;

                      //    Check the absolute range of the cursor position
                      //    There could be a window wherein the chart geoereferencing is not valid....
                      if((fabs(show_cursor_lat) < 90.) && (fabs(show_cursor_lon) < 360.))
                      {
                        while(show_cursor_lon < -180.)
                              show_cursor_lon += 360.;

                        while(show_cursor_lon > 180.)
                              show_cursor_lon -= 360.;

                        wxString s1 = _("Cursor: ");
                        s1 += toSDMM(1, show_cursor_lat);
                        s1 += _T(" ");
                        s1 += toSDMM(2, show_cursor_lon);
                        parent_frame->SetStatusText ( s1, STAT_FIELD_CURSOR_LL );

                        double brg, dist;
                        DistanceBearingMercator(m_cursor_lat, m_cursor_lon, gLat, gLon, &brg, &dist);
                        wxString s;
                        s.Printf(_("From Ownship: %03d Deg  %6.2f NMi"), (int)brg, dist);
                        parent_frame->SetStatusText ( s, STAT_FIELD_CURSOR_BRGRNG );
                      }
                }
        }
#endif


        if(g_pGribDialog)
              g_pGribDialog->SetCursorLatLon(m_cursor_lat, m_cursor_lon);

        //        Check for wheel rotation
        m_mouse_wheel_oneshot = 50;                  //msec
                                                      // ideally, should be just longer than the time between
                                                      // processing accumulated mouse events from the event queue
                                                      // as would happen during screen redraws.
        int wheel_dir = event.GetWheelRotation();

        if(m_MouseWheelTimer.IsRunning())
        {
              if(wheel_dir != m_last_wheel_dir)
                    m_MouseWheelTimer.Stop();
              else
                    m_MouseWheelTimer.Start(m_mouse_wheel_oneshot, true);           // restart timer
        }

        m_last_wheel_dir = wheel_dir;

        if(wheel_dir)
        {
            if(!m_MouseWheelTimer.IsRunning())
            {
                  if(g_bEnableZoomToCursor)
                  {
                        bool b_zoom_moved = false;
                        if(wheel_dir > 0)
                              b_zoom_moved = ZoomCanvasIn(m_cursor_lat, m_cursor_lon);
                        else if(wheel_dir < 0)
                              b_zoom_moved = ZoomCanvasOut(m_cursor_lat, m_cursor_lon);

                        if(b_zoom_moved)
                        {
                              WarpPointerDeferred(m_canvas_width/2, m_canvas_height/2);          // move the mouse pointer to zoomed location
                              vLat = m_cursor_lat;
                              vLon = m_cursor_lon;
                        }

                        ClearbFollow();      // update the follow flag

                  }
                  else
                  {
                        if(wheel_dir > 0)
                              ZoomCanvasIn();
                        else if(wheel_dir < 0)
                              ZoomCanvasOut();
                  }

                  m_MouseWheelTimer.Start(m_mouse_wheel_oneshot, true);           // start timer
            }
        }


//    Route Creation Rubber Banding
        if ( parent_frame->nRoute_State >= 2 )
        {
                r_rband.x = x;
                r_rband.y = y;
                m_bDrawingRoute = true;

                CheckEdgePan ( x, y );
                Refresh ( false );
        }


//    Measure Tool Rubber Banding
        if ( m_bMeasure_Active && (m_nMeasureState >= 2 ))
        {
              r_rband.x = x;
              r_rband.y = y;

              CheckEdgePan ( x, y );
              Refresh ( false );
        }


//    AIS Target Rollover
        if(g_pAIS && g_pAIS->GetNumTargets() && g_bShowAIS )     // pjotrc 2010.02.09
        {
              SelectItem *pFind = pSelectAIS->FindSelection ( m_cursor_lat, m_cursor_lon, SELTYPE_AISTARGET, SelectRadius );
              if ( pFind )
              {
                    int FoundAIS_MMSI = ( long ) pFind->m_pData1;       // cast to long avoids problems with 64bit compilers
                    AIS_Target_Data *ptarget = g_pAIS->Get_Target_Data_From_MMSI(FoundAIS_MMSI);

                    if(ptarget)
                    {
                        if(NULL == m_pPopUpWin)
                        {
                              m_pPopUpWin = new AISroWin(this);
                              m_pPopUpWin->Hide();
                        }


                        if(!m_pPopUpWin->IsShown())
                        {
                              wxString s;


                              if(ptarget->b_nameValid)
                              {
                                    char *tp = ptarget->ShipName;
                                    while((*tp) && (*tp != '@'))
                                          s.Append(*tp++);
                                    s.Trim();

                                    wxString t(ptarget->Get_vessel_type_string(true), wxConvUTF8);
                                    if(t.Len())
                                    {
                                          s.Prepend(_T(" "));
                                          s.Prepend(t);
                                    }
                              }
                              else
                              {
                                    wxString t;
                                    t.Printf(_T("%d"), ptarget->MMSI);
                                    s.Prepend(t);
                              }


                              m_pPopUpWin->SetString(s);

                              m_pPopUpWin->SetPosition(wxPoint(x+16, y+16));
                              m_pPopUpWin->SetBitmap();
                              m_pPopUpWin->Refresh();
                              m_pPopUpWin->Show();

                        }
                    }
              }
              else
              {
                    if(m_pPopUpWin && m_pPopUpWin->IsShown())
                          m_pPopUpWin->Hide();

              }
        }

//          Mouse Clicks

//    Manage canvas 2
/*
        if ( event.LeftDClick() )
        {
                ClearbFollow();      // update the follow flag


                vLat = cursor_lat;
                vLon = cursor_lon;

                SetViewPoint ( ( double ) vLat, ( double ) vLon, VPoint.view_scale_ppm, VPoint.skew, FORCE_SUBSAMPLE );

                Refresh ( FALSE );
        }
*/
        if ( event.LeftDown() )
        {
                last_drag.x = mx;
                last_drag.y = my;

                if ( parent_frame->nRoute_State )                     // creating route?
                {
                        double rlat, rlon;


                        SetMyCursor ( pCursorPencil );
                        rlat = m_cursor_lat;
                        rlon = m_cursor_lon;

                        if ( parent_frame->nRoute_State == 1 )
                        {
                                m_pMouseRoute = new Route();
                                pRouteList->Append ( m_pMouseRoute );
                                r_rband.x = x;
                                r_rband.y = y;
                        }

                        //    Check to see if there is a nearby point which may be reused
                        RoutePoint *pMousePoint = NULL;

                        //    Calculate meaningful SelectRadius
                        int nearby_sel_rad_pix = 8;
                        double nearby_radius_meters = nearby_sel_rad_pix / m_true_scale_ppm;

                        RoutePoint *pNearbyPoint = pWayPointMan->GetNearbyWaypoint(rlat, rlon, nearby_radius_meters);
                        if(pNearbyPoint && (pNearbyPoint != m_prev_pMousePoint) && !pNearbyPoint->m_bIsInTrack)     // pjotrc 2010.02.13
                        {
                              wxMessageDialog near_point_dlg(this, _("Use nearby waypoint?"), _("OpenCPN Route Create"), (long)wxYES_NO | wxCANCEL | wxYES_DEFAULT);
                              int dlg_return = near_point_dlg.ShowModal();

                              if(dlg_return == wxID_YES)
                                    pMousePoint = pNearbyPoint;
                        }

                        if(NULL == pMousePoint)                   // need a new point
                        {
                              pMousePoint = new RoutePoint ( rlat, rlon, wxString ( _T ( "diamond" ) ), wxString ( _T ( "" ) ), NULL );
                              pConfig->AddNewWayPoint ( pMousePoint, -1 );    // use auto next num
                              pSelect->AddSelectableRoutePoint ( rlat, rlon, pMousePoint );
                        }

                        m_pMouseRoute->AddPoint ( pMousePoint );
                        if ( parent_frame->nRoute_State > 1 )
                                pSelect->AddSelectableRouteSegment ( m_prev_rlat, m_prev_rlon, rlat, rlon,
                                                                     m_prev_pMousePoint, pMousePoint, m_pMouseRoute );

                        m_prev_rlat = rlat;
                        m_prev_rlon = rlon;
                        m_prev_pMousePoint = pMousePoint;

                        parent_frame->nRoute_State++;

                        Refresh ( false );
                }

                else if ( m_bMeasure_Active )                     // measure tool on
                {
                      SetMyCursor ( pCursorPencil );

                      if(NULL != m_pMeasureRoute)
                            g_pRouteMan->DeleteRoute ( m_pMeasureRoute );

                      m_pMeasureRoute = new Route();
                      r_rband.x = x;
                      r_rband.y = y;

                      RoutePoint *pMousePoint = new RoutePoint ( m_cursor_lat, m_cursor_lon, wxString ( _T ( "circle" ) ), wxString ( _T ( "" ) ), NULL );
                      pMousePoint->m_bShowName = false;

                      m_pMeasureRoute->AddPoint ( pMousePoint );

                      m_prev_rlat = m_cursor_lat;
                      m_prev_rlon = m_cursor_lon;

                      m_nMeasureState++;

                      Refresh ( false );
                }

                else                                // Not creating Route
                {
                        // So look for selectable route point

                        SelectItem *pFind = pSelect->FindSelection ( m_cursor_lat, m_cursor_lon, SELTYPE_ROUTEPOINT, SelectRadius );

                        if ( pFind )
                        {
                                RoutePoint *frp = ( RoutePoint * ) pFind->m_pData1;
                                m_pRoutePointEditTarget = frp;
                                m_pFoundPoint = pFind;


                         //    Get an array of all routes using this point and use it to rubberband all affected routes
                                m_pEditRouteArray = g_pRouteMan->GetRouteArrayContaining(frp);

                                if ( m_pEditRouteArray )                       // Editing Waypoint as part of route
                                {
                                      for(unsigned int ir=0 ; ir < m_pEditRouteArray->GetCount() ; ir++)
                                      {
                                            Route *pr = (Route *)m_pEditRouteArray->Item(ir);
                                            pr->m_bIsBeingEdited = true;
                                      }
                                      m_bRouteEditing = true;
                                }
                                else                                      // editing Mark
                                {
                                        frp->m_bIsBeingEdited = true;
                                        m_bMarkEditing = true;
                                }
                        }

                }                                   // else
        }



        if ( event.Dragging() )
        {
                if ( m_bRouteEditing )
                {

                   bool DraggingAllowed = true;

                   if ( NULL == pMarkPropDialog )
                   {
                         if( g_bWayPointPreventDragging)
                              DraggingAllowed = false;
                   }
                   else if ( !pMarkPropDialog->IsShown() && g_bWayPointPreventDragging)
                         DraggingAllowed = false;

                   if (DraggingAllowed)
                   {

                        // Get the update rectangle for the union of the un-edited routes
                      wxRect pre_rect;

                      if ( m_pEditRouteArray )
                      {
                            for(unsigned int ir=0 ; ir < m_pEditRouteArray->GetCount() ; ir++)
                            {
                                  Route *pr = (Route *)m_pEditRouteArray->Item(ir);
                                  wxRect route_rect;
                                  pr->CalculateDCRect ( m_dc_route, &route_rect, VPoint );
                                  pre_rect.Union ( route_rect );
                            }
                      }

                        m_pRoutePointEditTarget->m_lat = m_cursor_lat;     // update the RoutePoint entry
                        m_pRoutePointEditTarget->m_lon = m_cursor_lon;
                        m_pFoundPoint->m_slat = m_cursor_lat;             // update the SelectList entry
                        m_pFoundPoint->m_slon = m_cursor_lon;

                        if ( CheckEdgePan ( x, y ) )
                        {
                                double new_cursor_lat, new_cursor_lon;
                                GetCanvasPixPoint ( x, y, new_cursor_lat, new_cursor_lon );
                                m_pRoutePointEditTarget->m_lat = new_cursor_lat;     // update the RoutePoint entry
                                m_pRoutePointEditTarget->m_lon = new_cursor_lon;
                                m_pFoundPoint->m_slat = new_cursor_lat;             // update the SelectList entry
                                m_pFoundPoint->m_slon = new_cursor_lon;
                        }


                        //    Update the MarkProperties Dialog, if currently shown
                        if ( ( NULL != pMarkPropDialog ) && ( pMarkPropDialog->IsShown() ) )
                        {
                                if ( m_pRoutePointEditTarget == pMarkPropDialog->GetRoutePoint() )
                                        pMarkPropDialog->UpdateProperties();
                        }

                        // Get the update rectangle for the edited route
                        wxRect post_rect;

                        if ( m_pEditRouteArray )
                        {
                              for(unsigned int ir=0 ; ir < m_pEditRouteArray->GetCount() ; ir++)
                              {
                                    Route *pr = (Route *)m_pEditRouteArray->Item(ir);
                                    wxRect route_rect;
                                    pr->CalculateDCRect ( m_dc_route, &route_rect, VPoint );
                                    post_rect.Union ( route_rect );
                              }
                        }

                        //    Invalidate the union region
                        pre_rect.Union ( post_rect );
                        RefreshRect ( pre_rect, false );
                   }
                }     // if Route Editing


                else if ( m_bMarkEditing )
                {
                                    // toh, 2009.02.24
                      bool DraggingAllowed = true;

                      if ( NULL == pMarkPropDialog )
                      {
                            if( g_bWayPointPreventDragging)
                                  DraggingAllowed = false;
                      }
                      else if ( !pMarkPropDialog->IsShown() && g_bWayPointPreventDragging)
                            DraggingAllowed = false;

                      if(m_pRoutePointEditTarget && (m_pRoutePointEditTarget->m_IconName == _T("mob")))
                            DraggingAllowed = false;

                      if (DraggingAllowed)
                      {
                            //      The mark may be an anchorwatch
                            double lpp1 = 0.;
                            double lpp2 = 0.;
                            double lppmax;

                            if (pAnchorWatchPoint1 == m_pRoutePointEditTarget)
                            {
                                  lpp1 = fabs(GetAnchorWatchRadiusPixels(pAnchorWatchPoint1));

                            }
                            if (pAnchorWatchPoint2 == m_pRoutePointEditTarget)
                            {
                                  lpp2 = fabs(GetAnchorWatchRadiusPixels(pAnchorWatchPoint2));
                            }
                            lppmax = wxMax(lpp1 + 10, lpp2 + 10);         // allow for cruft

                        // Get the update rectangle for the un-edited mark
                        wxRect pre_rect;
                        m_pRoutePointEditTarget->CalculateDCRect ( m_dc_route, &pre_rect );
                        if((lppmax > pre_rect.width/2) || (lppmax > pre_rect.height/2))
                              pre_rect.Inflate(lppmax - (pre_rect.width/2), lppmax - (pre_rect.height/2));

                        m_pRoutePointEditTarget->m_lat = m_cursor_lat;     // update the RoutePoint entry
                        m_pRoutePointEditTarget->m_lon = m_cursor_lon;
                        m_pFoundPoint->m_slat = m_cursor_lat;             // update the SelectList entry
                        m_pFoundPoint->m_slon = m_cursor_lon;

                        //    Update the MarkProperties Dialog, if currently shown
                        if ( ( NULL != pMarkPropDialog ) && ( pMarkPropDialog->IsShown() ) )
                        {
                                if ( m_pRoutePointEditTarget == pMarkPropDialog->GetRoutePoint() )
                                        pMarkPropDialog->UpdateProperties();
                        }

                        // Get the update rectangle for the edited mark
                        wxRect post_rect;
                        m_pRoutePointEditTarget->CalculateDCRect ( m_dc_route, &post_rect );
                        if((lppmax > post_rect.width/2) || (lppmax > post_rect.height/2))
                              post_rect.Inflate(lppmax - (post_rect.width/2), lppmax - (post_rect.height/2));

                        //    Invalidate the union region
                        pre_rect.Union ( post_rect );
                        RefreshRect ( pre_rect, false );
                      }
                }

                else                                            // must be chart dragging...
                {
                        if ( ( last_drag.x != mx ) || ( last_drag.y != my ) )
                        {
                                m_bChartDragging = true;
                                PanCanvas(last_drag.x - mx, last_drag.y - my);

                                last_drag.x = mx;
                                last_drag.y = my;

//    if LODEF->HIDEF panning is enabled....
                                if ( m_bSubsamp )
                                {
                                      current_scale_method = RENDER_LODEF;
//    Retrigger the rescale timer
                                      pRescaleTimer->Start ( m_rescale_timer_msec, wxTIMER_ONE_SHOT );
                                }

                                Refresh ( false );
                        }
                }
        }

        if ( event.LeftUp() )
        {
                if ( m_bRouteEditing )
                {

                        pSelect->UpdateSelectableRouteSegments ( m_pRoutePointEditTarget );

                        if ( m_pEditRouteArray )
                        {
                              for(unsigned int ir=0 ; ir < m_pEditRouteArray->GetCount() ; ir++)
                              {
                                    Route *pr = (Route *)m_pEditRouteArray->Item(ir);
                                    pr->CalculateBBox();
                                    pr->UpdateSegmentDistances();
                                    pr->m_bIsBeingEdited = false;

                                    pConfig->UpdateRoute ( pr );
                              }
                        }

                        //    Update the RouteProperties Dialog, if currently shown
                        if ( ( NULL != pRoutePropDialog ) && ( pRoutePropDialog->IsShown() ) )
                        {
                              if ( m_pEditRouteArray )
                              {
                                    for(unsigned int ir=0 ; ir < m_pEditRouteArray->GetCount() ; ir++)
                                    {
                                          Route *pr = (Route *)m_pEditRouteArray->Item(ir);
                                          if(pRoutePropDialog->m_pRoute == pr)
                                          {
                                                pRoutePropDialog->SetRouteAndUpdate ( pr );
                                                pRoutePropDialog->UpdateProperties();
                                          }
                                    }
                              }
                        }


                        m_bRouteEditing = false;
                        delete m_pEditRouteArray;
                        m_pEditRouteArray = NULL;

                        m_pRoutePointEditTarget->m_bPtIsSelected = false;
                        m_pRoutePointEditTarget = NULL;
                }

                else if ( m_bMarkEditing )
                {
                        pConfig->UpdateWayPoint ( m_pRoutePointEditTarget );

                        m_pRoutePointEditTarget->m_bIsBeingEdited = false;
                        m_pRoutePointEditTarget->m_bPtIsSelected = false;
                        m_pRoutePointEditTarget = NULL;
                        m_bMarkEditing = false;

                }

                else if ( parent_frame->nRoute_State )             // creating route?
                {
                }

                else if ( m_bMeasure_Active )                     // Measure Tool in use?
                {
                }

                else                      // left click for chart center
                {
                      if(!m_bChartDragging)
                      {
//          Chart Panning

                                  switch ( cursor_region )
                                  {
                                        case MID_RIGHT:
                                        {
                                              PanCanvas(100, 0);
                                              break;
                                        }

                                        case MID_LEFT:
                                        {
                                              PanCanvas(-100, 0);
                                              break;
                                        }

                                        case MID_TOP:
                                        {
                                              PanCanvas(0, 100);
                                              break;
                                        }

                                        case MID_BOT:
                                        {
                                              PanCanvas(0, -100);
                                              break;
                                        }

                                        case CENTER:
                                        {
                                              double dlat, dlon;
                                              wxPoint p;
                                              GetCanvasPixPoint ( x, y, dlat, dlon );

                                              if(dlon > 180.) dlon -= 360.;
                                              if(dlon < -180.) dlon += 360.;

                                              SetViewPoint ( dlat, dlon, VPoint.view_scale_ppm, VPoint.skew, VPoint.rotation, FORCE_SUBSAMPLE );
                                              vLat = dlat;
                                              vLon = dlon;

                                              ClearbFollow();      // update the follow flag

                                              Refresh ( false );
                                              PanCanvas(0, 0);
                                              break;
                                        }
                                  }                             // switch
                      }
                      else
                        m_bChartDragging = false;
                }

                m_bShowSegInfo = false;;
        }

        if ( event.RightDown() )
        {
                last_drag.x = mx;
                last_drag.y = my;

                if ( parent_frame->nRoute_State )                     // creating route?
                        CanvasPopupMenu ( x,y, SELTYPE_ROUTECREATE );
                else                                                  // General Right Click
                {
                                              // Look for selectable objects
                      float slat, slon;
                      slat = m_cursor_lat;
                      slon = m_cursor_lon;
//                      SelectItem *pFind;
                      wxClientDC dc ( this );

                      SelectItem *pFindAIS;
                      SelectItem *pFindRP;
                      SelectItem *pFindRouteSeg;
                      SelectItem *pFindTrackSeg;
                      SelectItem *pFindCurrent = NULL;
                      SelectItem *pFindTide = NULL;

                        //    Deselect any current objects
                      if ( m_pSelectedRoute )
                      {
                            m_pSelectedRoute->m_bRtIsSelected = false;        // Only one selection at a time
                            m_pSelectedRoute->DeSelectRoute();
                            m_pSelectedRoute->Draw ( dc, VPoint );
                      }

                      if ( m_pFoundRoutePoint )
                      {
                            m_pFoundRoutePoint->m_bPtIsSelected = false;
                            m_pFoundRoutePoint->Draw( dc );
                            RefreshRect ( m_pFoundRoutePoint->CurrentRect_in_DC );
                      }

                      //      Get all the selectable things at the cursor
                      pFindAIS =      pSelectAIS->FindSelection ( slat, slon, SELTYPE_AISTARGET, SelectRadius );
                      pFindRP =       pSelect->FindSelection ( slat, slon, SELTYPE_ROUTEPOINT,SelectRadius );
                      pFindRouteSeg = pSelect->FindSelection ( slat, slon,SELTYPE_ROUTESEGMENT,SelectRadius );
                      pFindTrackSeg = pSelect->FindSelection ( slat, slon,SELTYPE_TRACKSEGMENT,SelectRadius );
                      if ( m_bShowCurrent )
                             pFindCurrent = pSelectTC->FindSelection ( slat, slon,SELTYPE_CURRENTPOINT,SelectRadius );

                      if ( m_bShowTide )                                // look for tide stations
                            pFindTide = pSelectTC->FindSelection ( slat, slon,SELTYPE_TIDEPOINT,SelectRadius );

                      int seltype = 0;

                      //    Try for AIS targets first
                      if ( pFindAIS )
                      {
                            m_FoundAIS_MMSI = pFindAIS->GetUserData();
                            seltype |= SELTYPE_AISTARGET;
                      }


                        //    Now the various Route Parts

                      m_pFoundRoutePoint = NULL;
                      if ( pFindRP )
                      {
                            m_pFoundRoutePoint = ( RoutePoint * ) pFindRP->m_pData1;
                            if(m_pFoundRoutePoint->IsVisible())
                            {
                              m_pFoundRoutePoint->m_bPtIsSelected = true;
                              m_pFoundRoutePoint->Draw( dc );

                              //    Get an array of all routes using this point and use it to choose the appropriate route
                              //    Give preference to any active route, otherwise select the first visible route in the array
                              wxArrayPtrVoid *proute_array = g_pRouteMan->GetRouteArrayContaining(m_pFoundRoutePoint);
                              m_pSelectedRoute = NULL;
                              if(proute_array)
                              {
                                    for(unsigned int ir=0 ; ir < proute_array->GetCount() ; ir++)
                                    {
                                          Route *pr = (Route *)proute_array->Item(ir);
                                          if(pr->m_bRtIsActive)
                                          {
                                                m_pSelectedRoute = pr;
                                                break;
                                          }
                                    }

                                    if(NULL == m_pSelectedRoute)
                                    {
                                          for(unsigned int ir=0 ; ir < proute_array->GetCount() ; ir++)
                                          {
                                                Route *pr = (Route *)proute_array->Item(ir);
                                                if(pr->IsVisible())
                                                {
                                                      m_pSelectedRoute = pr;
                                                      break;
                                                }
                                          }
                                    }

                                    //    Final fallback is to select the first in the array
                                    if(NULL == m_pSelectedRoute)
                                          m_pSelectedRoute = (Route *)proute_array->Item(0);

                                    delete proute_array;
                              }

                              if(NULL == m_pSelectedRoute)
                                    m_pSelectedRoute = g_pRouteMan->FindRouteContainingWaypoint ( m_pFoundRoutePoint );

                              if ( m_pSelectedRoute )
                              {
                                    if(m_pSelectedRoute->IsVisible())
                                          seltype |= SELTYPE_ROUTEPOINT;
                              }
                              else
                                    seltype |= SELTYPE_MARKPOINT;
                            }
                      }


                        // Note here that we use SELTYPE_ROUTESEGMENT to select tracks as well as routes
                        // But call the popup handler with identifier appropriate to the type
                      if ( pFindRouteSeg )                  // there is at least one select item
                      {
                            SelectableItemList SelList = pSelect->FindSelectionList(slat, slon,SELTYPE_ROUTESEGMENT,SelectRadius );



                            if(NULL == m_pSelectedRoute)  // the case where a segment only is selected
                            {
                                  //  Choose the first visible route containing segment in the list
                                  wxSelectableItemListNode *node = SelList.GetFirst();
                                  while ( node )
                                  {
                                        SelectItem *pFindSel = node->GetData();

                                        Route *pr = (Route *)pFindSel->m_pData3;
                                        if(pr->IsVisible())
                                        {
                                              m_pSelectedRoute = pr;
                                              break;
                                        }
                                        node = node->GetNext();
                                  }
                            }




                            if ( m_pSelectedRoute )
                            {
                                  if(NULL == m_pFoundRoutePoint)
                                          m_pFoundRoutePoint = ( RoutePoint * ) pFindRouteSeg->m_pData1;
                                  m_pFoundRoutePointSecond = ( RoutePoint * ) pFindRouteSeg->m_pData2;

                                  m_pSelectedRoute->m_bRtIsSelected = true;
                                  m_pSelectedRoute->Draw ( dc, VPoint );
                                  seltype |= SELTYPE_ROUTESEGMENT;
                            }

                            //      Show the route segment info
                            m_bShowSegInfo = true;
                            m_segShow_point_a = ( RoutePoint * ) pFindRouteSeg->m_pData1;
                            m_segShow_point_b = ( RoutePoint * ) pFindRouteSeg->m_pData2;
                            Refresh(false);

                      }

                      if ( pFindTrackSeg )
                      {
                            m_pSelectedTrack = (Route *)pFindTrackSeg->m_pData3;
                            if ( m_pSelectedTrack   &&  m_pSelectedTrack->IsVisible())
                            {
                                  seltype |= SELTYPE_TRACKSEGMENT;
                            }
                      }


                      bool bseltc = false;
                      if(0 == seltype)
                      {
                                  if (  pFindCurrent )
                                  {
                                        DrawTCWindow ( x, y, ( void * ) pFindCurrent->m_pData1 );
                                        Refresh ( false );
                                        bseltc = true;
                                  }

                                  else if ( pFindTide )
                                  {
                                        DrawTCWindow ( x, y, ( void * ) pFindTide->m_pData1 );
                                        Refresh ( false );
                                        bseltc = true;
                                  }

                      }

                      if(0 == seltype)
                            seltype |= SELTYPE_UNKNOWN;

                      if ( !bseltc )
                            CanvasPopupMenu ( x,y, seltype );

                      m_bShowSegInfo = false;
                      Refresh ( false );            // needed for MSW, not GTK  Why??

                }

        }


//    Switch to the appropriate cursor on mouse movement

        wxCursor *ptarget_cursor;

        if (( !parent_frame->nRoute_State ) && ( !m_bMeasure_Active ))
        {


                if ( x > xr_margin )
                {
                        ptarget_cursor = pCursorRight;
                        cursor_region = MID_RIGHT;
                }
                else if ( x < xl_margin )
                {
                        ptarget_cursor =  pCursorLeft;
                        cursor_region = MID_LEFT;
                }
                else if ( y > yb_margin )
                {
                        ptarget_cursor = pCursorDown;
                        cursor_region = MID_TOP;
                }
                else if ( y < yt_margin )
                {
                        ptarget_cursor =  pCursorUp;
                        cursor_region = MID_BOT;
                }
                else
                {
                        ptarget_cursor =  pCursorArrow;
                        cursor_region = CENTER;
                }
        }
        else
              ptarget_cursor = pCursorPencil;

        SetMyCursor ( ptarget_cursor );


}

void ChartCanvas::LostMouseCapture(wxMouseCaptureLostEvent& event)
{
      SetMyCursor ( pCursorArrow );
}




//-------------------------------------------------------------------------------
//          Popup Menu Handling
//-------------------------------------------------------------------------------
void ChartCanvas::DoCanvasPopupMenu ( int x, int y, wxMenu *pMenu )
{
      wxColour back_color = GetGlobalColor(_T("UIBCK"));
      wxColour text_color = GetGlobalColor ( _T ( "UITX1" ) );
      wxString col;
      col.Printf(_T("#%02X%02X%02X"), text_color.Red(), text_color.Green(), text_color.Blue());
//      wxLogMessage(col);

      for(unsigned int i=0 ; i < pMenu->GetMenuItemCount() ; i++)
      {
            wxMenuItem *pitem = pMenu->FindItemByPosition(i);

#ifdef __WXGTK__
#ifdef ocpnUSE_GTK_OPTIMIZE
            //    This works for at least some versions of GTK+ with <<some>> window managers and themes....
            wxString text = pitem->GetText();
//            ::wxSnprintf(tmp, 99, (const wxChar*)("<span color=\"%s\">%s</span>"), col, text);
            wxString tmp = _T("<span color=\"");
            tmp += col;
            tmp += _T("\">");
            tmp += text;
            tmp += _T("</span>");

            gtk_label_set_markup( GTK_LABEL( GTK_BIN(pitem->GetMenuItem())->child ), tmp.mb_str());
#endif
#endif

#ifdef __WXMSW__
            //    This does not work......
            pitem->SetTextColour(text_color);
            pitem->SetBackgroundColour( back_color);

#endif

      }

      PopupMenu ( pMenu, x, y );

}



void ChartCanvas::CanvasPopupMenu ( int x, int y, int seltype )
{
        wxMenu *pdef_menu = new wxMenu();
        popx = x;
        popy = y;

#ifdef __WXGTK__
#ifdef ocpnUSE_GTK_OPTIMIZE
    //  This code changes the background color on the popup context menu
        wxColour back_color = GetGlobalColor(_T("UIBCK"));
        GdkColor color;

        color.red   = back_color.Red() << 8;
        color.green = back_color.Green() << 8;
        color.blue  = back_color.Blue() << 8;

        gtk_widget_modify_bg (GTK_WIDGET(pdef_menu->m_menu), GTK_STATE_NORMAL, &color);
#endif
#endif

        if(seltype & SELTYPE_TRACKSEGMENT)
        {
              pdef_menu->Append ( ID_TK_MENU_PROPERTIES,        _( "Track Properties" ) );
              pdef_menu->Append ( ID_TK_MENU_DELETE,            _( "Delete Track" ) );
              pdef_menu->Append ( ID_TK_MENU_EXPORT,            _( "Export Track...." ) );
              pdef_menu->AppendSeparator();

        }

        if(seltype & SELTYPE_ROUTESEGMENT)
        {
              pdef_menu->Append ( ID_RT_MENU_ACTIVATE,          _( "Activate Route" ) );
              pdef_menu->Append ( ID_RT_MENU_DEACTIVATE,        _( "Deactivate Route" ) );
              pdef_menu->Append ( ID_RT_MENU_INSERT,            _( "Insert Waypoint" ) );
              pdef_menu->Append ( ID_RT_MENU_APPEND,            _( "Append Waypoint" ) );
              pdef_menu->Append ( ID_RT_MENU_DELETE,            _( "Delete Route" ) );
              pdef_menu->Append ( ID_RT_MENU_REVERSE,           _( "Reverse Route" ) );
              pdef_menu->Append ( ID_RT_MENU_PROPERTIES,        _( "Route Properties" ) );
              pdef_menu->Append ( ID_RT_MENU_EXPORT,            _( "Export Route...." ) );
              pdef_menu->Append ( ID_RT_MENU_SENDTOGPS,         _( "Send Route To GPS..." ) );
//              pdef_menu->Append ( ID_RT_MENU_DELETEALL ,        _( "Delete All Routes..." ) );

              if ( m_pSelectedRoute )
              {
                    pdef_menu->Enable ( ID_RT_MENU_ACTIVATE, !m_pSelectedRoute->m_bRtIsActive );
                    pdef_menu->Enable ( ID_RT_MENU_DEACTIVATE, m_pSelectedRoute->m_bRtIsActive );
              }
              pdef_menu->AppendSeparator();

        }

        if(seltype & SELTYPE_ROUTEPOINT)
        {
            pdef_menu->Append ( ID_RT_MENU_ACTPOINT,    _( "Activate Waypoint" ) );
            pdef_menu->Append ( ID_RT_MENU_ACTNXTPOINT, _( "Activate Next in Route" ) );
            pdef_menu->Append ( ID_RT_MENU_REMPOINT,    _( "Remove Waypoint from Route" ) );
            pdef_menu->Append ( ID_RT_MENU_DELPOINT,    _( "Delete Waypoint" ) );
            pdef_menu->Append ( ID_WP_MENU_PROPERTIES,  _( "Mark/WP Properties" ) );

            int NbrLinks = m_pFoundRoutePoint->m_HyperlinkList->GetCount();
            if (NbrLinks > 0)
            {
                  pdef_menu->Append ( ID_WP_MENU_ADDITIONAL_INFO,   _( "Additional information" ) );
            }

            pdef_menu->AppendSeparator();

            if(m_pSelectedRoute)
            {
                  pdef_menu->Enable ( ID_RT_MENU_ACTPOINT, m_pSelectedRoute->m_bRtIsActive );
                  pdef_menu->Enable ( ID_RT_MENU_ACTNXTPOINT, m_pSelectedRoute->m_bRtIsActive );
            }
        }

        if(seltype & SELTYPE_MARKPOINT)
        {
              pdef_menu->Append ( ID_WP_MENU_DELPOINT,    _( "Delete Mark" ) );
              pdef_menu->Append ( ID_WP_MENU_PROPERTIES,  _( "Mark/WP Properties" ) );
              pdef_menu->Append ( ID_WP_MENU_DELETEALL,   _( "Delete All Waypoints..." ) );

              if ((m_pFoundRoutePoint == pAnchorWatchPoint1) || (m_pFoundRoutePoint == pAnchorWatchPoint2))       //pjotrc 2010.02.15
                    pdef_menu->Append ( ID_WP_MENU_CLEAR_ANCHORWATCH,   _( "Clear Anchor Watch" ) );                //pjotrc 2010.02.15
              else                                                                                                //pjotrc 2010.02.15
                    if ((NULL == pAnchorWatchPoint1) || (NULL == pAnchorWatchPoint2)) {                             //pjotrc 2010.02.15

                    double dist;
                    double brg;                                                                                               //pjotrc 2010.02.15
                    DistanceBearingMercator(m_pFoundRoutePoint->m_lat, m_pFoundRoutePoint->m_lon, gLat, gLon, &brg, &dist);         //pjotrc 2010.02.15
                    if (dist*1852. <= AnchorPointMaxDist /*&& dist*1852. >= AnchorPointMinDist*/)
                          pdef_menu->Append ( ID_WP_MENU_SET_ANCHORWATCH,   _( "Set Anchor Watch" ) );      //pjotrc 2010.02.15
                    }

              if (m_pFoundRoutePoint->m_HyperlinkList->GetCount() > 0)
                   pdef_menu->Append ( ID_WP_MENU_ADDITIONAL_INFO,   _( "Additional information" ) );

              pdef_menu->AppendSeparator();

        }

        if(seltype & SELTYPE_UNKNOWN)
        {
              if(!VPoint.b_quilt)
              {
                  if(parent_frame->GetnChartStack() > 1)
                  {
                        pdef_menu->Append ( ID_DEF_MENU_MAX_DETAIL, _( "Max Detail Here" ) );
                        pdef_menu->Append ( ID_DEF_MENU_SCALE_IN,   _( "Scale In" ) );
                        pdef_menu->Append ( ID_DEF_MENU_SCALE_OUT,  _( "Scale Out" ) );
                        pdef_menu->AppendSeparator();
                  }

                  if (Current_Ch && ( Current_Ch->m_ChartType == CHART_TYPE_CM93COMP ))
                  {
                        pdef_menu->AppendCheckItem(ID_DEF_MENU_CM93ZOOM, _("Enable CM93 Detail Slider"));
                        if(pCM93DetailSlider)
                              pdef_menu->Check(ID_DEF_MENU_CM93ZOOM, pCM93DetailSlider->IsShown());
                  }

                  if (Current_Ch && ( Current_Ch->m_ChartFamily == CHART_FAMILY_VECTOR ))
                        pdef_menu->Append ( ID_DEF_MENU_QUERY,  _( "Object Query" ) );

              }
              else
              {
                    ChartBase *pChartTest = m_pQuilt->GetChartAtPix(wxPoint(x, y));
                    if(pChartTest && (pChartTest->m_ChartFamily == CHART_FAMILY_VECTOR ))
                        pdef_menu->Append ( ID_DEF_MENU_QUERY,  _( "Object Query" ) );

                    pdef_menu->Append ( ID_DEF_MENU_QUILTREMOVE,  _( "Remove this chart from quilt." ) );

              }


        }

        if(seltype & SELTYPE_AISTARGET)
        {
              pdef_menu->Append ( ID_DEF_MENU_AIS_QUERY,  _( "AIS Target Query" ) );
        }


        //        Following Select type are exclusive
        if(seltype == SELTYPE_ROUTECREATE)
        {
              if(!VPoint.b_quilt)
              {
                  pdef_menu->Append ( ID_DEF_MENU_MAX_DETAIL, _( "Max Detail Here" ) );
                  pdef_menu->Append ( ID_RC_MENU_SCALE_IN,    _( "Scale In" ) );
                  pdef_menu->Append ( ID_RC_MENU_SCALE_OUT,   _( "Scale Out" ) );
              }
              pdef_menu->Append ( ID_RC_MENU_ZOOM_IN,     _( "Zoom In" ) );
              pdef_menu->Append ( ID_RC_MENU_ZOOM_OUT,    _( "Zoom Out" ) );
              pdef_menu->Append ( ID_RC_MENU_FINISH,      _( "End Route" ) );
        }


        //        Add invariant items
        pdef_menu->Append ( ID_DEF_MENU_DROP_WP,    _( "Drop Mark Here" ) );
        pdef_menu->Append ( ID_DEF_MENU_MOVE_BOAT_HERE, _( "Move Boat Here" ) );

        if ( !g_pRouteMan->GetpActiveRoute() )
              pdef_menu->Append ( ID_DEF_MENU_GOTO_HERE, _( "Go To Here" ) );

        if(!m_bMeasure_Active)
              pdef_menu->Append ( ID_DEF_MENU_ACTIVATE_MEASURE,    _( "Measure....." ) );
        else
              pdef_menu->Append ( ID_DEF_MENU_DEACTIVATE_MEASURE,    _( "Measure Off" ) );

        pdef_menu->Append(ID_DEF_MENU_ROUTEMANAGER, _("Route Manager..."));

        if ( g_pAIS )
              pdef_menu->Append(ID_DEF_MENU_AISTARGETLIST, _("AIS target list"));


#ifdef __WXMSW__
        //  If we dismiss the context menu without action, we need to discard some mouse events....
        //  Eat the next 3 button events, which happen as down-down-up on MSW XP
        g_click_stop = 3;
#endif

        //        Invoke the drop-down menu
        DoCanvasPopupMenu ( x, y, pdef_menu );

            // Cleanup
        if (( m_pSelectedRoute ))
        {
                   m_pSelectedRoute->m_bRtIsSelected = false;
        }

        m_pSelectedRoute = NULL;

        if ( m_pFoundRoutePoint )
        {
              m_pFoundRoutePoint->m_bPtIsSelected = false;
//              m_pFoundRoutePoint->Draw( dc );
        }
        m_pFoundRoutePoint = NULL;

        m_pFoundRoutePointSecond = NULL;
        m_bShowSegInfo = false;


        delete pdef_menu;

}

void ChartCanvas::PopupMenuHandler ( wxCommandEvent& event )
{
        RoutePoint *pLast;

        wxPoint r;
        double zlat, zlon;

        wxString *QueryResult;

#ifdef USE_S57
        float SelectRadius;
        int sel_rad_pix;
        S57QueryDialog *pdialog;
        ListOfObjRazRules *rule_list;
        s57chart *Chs57;
        S57ObjectDesc *pdescription;
#endif

        GetCanvasPixPoint ( popx, popy, zlat, zlon );

        switch ( event.GetId() )
        {
                case ID_DEF_MENU_MAX_DETAIL:
                        vLat = zlat;
                        vLon = zlon;
                        ClearbFollow();

                        parent_frame->DoChartUpdate();

                        parent_frame->SelectChartFromStack (0, false, CHART_TYPE_UNKNOWN, CHART_FAMILY_RASTER);
                        break;

                case ID_DEF_MENU_SCALE_IN:
                        parent_frame-> DoStackDown();
                        break;

                case ID_DEF_MENU_SCALE_OUT:
                        parent_frame-> DoStackUp();
                        break;

                case ID_DEF_MENU_MOVE_BOAT_HERE:
                        gLat = zlat;
                        gLon = zlon;
                        break;

              case ID_DEF_MENU_GOTO_HERE:
              {
                    RoutePoint *pWP_dest = new RoutePoint ( zlat, zlon, wxString ( _T ( "triangle" ) ), wxString ( _T ( "" ) ), NULL );
                    pSelect->AddSelectableRoutePoint ( zlat, zlon, pWP_dest );

                    RoutePoint *pWP_src = new RoutePoint ( gLat, gLon, wxString ( _T ( "triangle" ) ), wxString ( _T ( "" ) ), NULL );
                    pSelect->AddSelectableRoutePoint ( gLat, gLon, pWP_src );

                    Route *temp_route = new Route();
                    pRouteList->Append ( temp_route );

                    temp_route->AddPoint(pWP_src);
                    temp_route->AddPoint(pWP_dest);

                    pSelect->AddSelectableRouteSegment ( gLat, gLon, zlat, zlon, pWP_src, pWP_dest, temp_route );

                    temp_route->m_RouteNameString = _("Temporary GOTO Route");
                    temp_route->m_RouteStartString = _("Here");;
                    temp_route->m_RouteEndString = _("There");

                    temp_route->m_bDeleteOnArrival = true;



                    if ( g_pRouteMan->GetpActiveRoute() )
                          g_pRouteMan->DeactivateRoute();

                    g_pRouteMan->ActivateRoute ( temp_route, pWP_dest );

                    break;
              }

              case ID_DEF_MENU_DROP_WP:
                {
                        RoutePoint *pWP = new RoutePoint ( zlat, zlon, wxString ( _T ( "triangle" ) ), wxString ( _T ( "" ) ), NULL );
                        pWP->m_bIsolatedMark = true;                      // This is an isolated mark
                        pWP->m_bKeepXRoute = true;                        // This mark should be kept
                        pSelect->AddSelectableRoutePoint ( zlat, zlon, pWP );
                        pConfig->AddNewWayPoint ( pWP, -1 );    // use auto next num
                        Refresh ( false );      // Needed for MSW, why not GTK??
                        break;
                }

                case ID_DEF_MENU_ROUTEMANAGER:
                        ShowRouteManager();
                        break;

                case ID_DEF_MENU_AISTARGETLIST:
                        ShowAISTargetList();
                        break;


                case ID_WP_MENU_DELPOINT:
                {
                       if (m_pFoundRoutePoint == pAnchorWatchPoint1) pAnchorWatchPoint1 = NULL;       // pjotrc 2010.02.15
                       else if (m_pFoundRoutePoint == pAnchorWatchPoint2) pAnchorWatchPoint2 = NULL;  // pjotrc 2010.02.15

                        pConfig->DeleteWayPoint ( m_pFoundRoutePoint );
                        pSelect->DeleteSelectablePoint ( m_pFoundRoutePoint, SELTYPE_ROUTEPOINT );
                        delete m_pFoundRoutePoint;
                        m_pFoundRoutePoint = NULL;

                        if(pMarkPropDialog)
                        {
                              pMarkPropDialog->SetRoutePoint ( NULL );
                              pMarkPropDialog->UpdateProperties();
                        }
                        break;
                }
                case ID_WP_MENU_PROPERTIES:
                        if ( NULL == pMarkPropDialog )          // There is one global instance of the MarkProp Dialog
                                pMarkPropDialog = new MarkProp ( this );

                        pMarkPropDialog->SetRoutePoint ( m_pFoundRoutePoint );
                        pMarkPropDialog->UpdateProperties();

                        pMarkPropDialog->Show();
                        break;

                case ID_WP_MENU_DELETEALL:
                {
                        wxMessageDialog mdlg(this, _("Are you sure you want to delete <ALL> waypoints?"), wxString(_("OpenCPN Alert")),wxYES_NO  );
                        if(mdlg.ShowModal() == wxID_YES)
                        {
                            pWayPointMan->DeleteAllWaypoints(false);          // only delete unused waypoints
                            m_pFoundRoutePoint = NULL;
                        }

                        break;
                }
                case ID_WP_MENU_CLEAR_ANCHORWATCH:             // pjotrc 2010.02.15
                       if (pAnchorWatchPoint1 == m_pFoundRoutePoint)
                       {
                             pAnchorWatchPoint1 = NULL;
                             g_AW1GUID.Clear();
                       }
                       else if (pAnchorWatchPoint2 == m_pFoundRoutePoint)
                       {
                             pAnchorWatchPoint2 = NULL;
                             g_AW2GUID.Clear();
                       }
                       break;

                case ID_WP_MENU_SET_ANCHORWATCH:              // pjotrc 2010.02.15
                       if (pAnchorWatchPoint1 == NULL)
                       {
                             pAnchorWatchPoint1 = m_pFoundRoutePoint;
                             g_AW1GUID = pAnchorWatchPoint1->m_GUID;
                       }
                       else if (pAnchorWatchPoint2 == NULL)
                       {
                             pAnchorWatchPoint2 = m_pFoundRoutePoint;
                             g_AW2GUID = pAnchorWatchPoint2->m_GUID;
                       }
                       break;


                case ID_WP_MENU_ADDITIONAL_INFO:             // toh, 2009.02.08
                        if ( NULL == pMarkInfoDialog )          // There is one global instance of the MarkInfo Dialog
                              pMarkInfoDialog = new MarkInfo ( this );

                        pMarkInfoDialog->SetRoutePoint ( m_pFoundRoutePoint );
                        pMarkInfoDialog->UpdateProperties();

                        pMarkInfoDialog->Show();
                        break;


                case ID_DEF_MENU_ACTIVATE_MEASURE:
//                        WarpPointer(popx,popy);
                        m_bMeasure_Active = true;
                        m_nMeasureState = 1;
                        break;

                case ID_DEF_MENU_DEACTIVATE_MEASURE:
                        m_bMeasure_Active = false;
                        g_pRouteMan->DeleteRoute ( m_pMeasureRoute );
                        m_pMeasureRoute = NULL;
                        Refresh ( false );
                        break;

                case ID_DEF_MENU_CM93ZOOM:
                        g_bShowCM93DetailSlider = event.IsChecked();

                        if(g_bShowCM93DetailSlider)
                        {
                              if(!pCM93DetailSlider)
                              {
                                    pCM93DetailSlider = new CM93DSlide(this, -1 , 0, -CM93_ZOOM_FACTOR_MAX_RANGE, CM93_ZOOM_FACTOR_MAX_RANGE,
                                                wxPoint(g_cm93detail_dialog_x, g_cm93detail_dialog_y), wxDefaultSize, wxSIMPLE_BORDER , _("cm93 Detail") );
                              }

            //    Here is an ugly piece of code which prevents the slider from taking the keyboard focus
            //    Only seems to work for Windows.....
                              pCM93DetailSlider->Disable();
                              pCM93DetailSlider->Show();
                              pCM93DetailSlider->Enable();

                        }
                        else
                        {
                              if(pCM93DetailSlider)
                                    pCM93DetailSlider->Close();
                        }

                        Refresh();
                        break;

#ifdef USE_S57
                case ID_DEF_MENU_QUERY:
                {
                      ChartBase *target_chart;
                      if (Current_Ch && ( Current_Ch->m_ChartFamily == CHART_FAMILY_VECTOR ))
                            target_chart = Current_Ch;
                      else if(VPoint.b_quilt)
                            target_chart = m_pQuilt->GetChartAtPix(wxPoint(popx, popy));
                      else
                            target_chart = NULL;

                      Chs57 = dynamic_cast<s57chart*> ( target_chart );

                      if(Chs57)
                      {
//    Go get the array of all objects at the cursor lat/lon
                                sel_rad_pix = 5;
                                SelectRadius = sel_rad_pix/ ( VPoint.view_scale_ppm * 1852 * 60 );

                                QueryResult = new wxString;

                                SetCursor(wxCURSOR_WAIT);
                                rule_list = Chs57->GetObjRuleListAtLatLon ( zlat, zlon, SelectRadius, &VPoint );

                                S57ObjectDesc **ppOD = (S57ObjectDesc **)malloc(rule_list->GetCount() * sizeof(S57ObjectDesc *));


                                int ndescriptions = 0;
                                if ( !rule_list->IsEmpty() )
                                {
                                        for ( ListOfObjRazRules::Node *node = rule_list->GetFirst(); node; node = node->GetNext() )
                                        {
                                                ObjRazRules *current = node->GetData();

                                                pdescription = Chs57->CreateObjDescription ( current );
                                                QueryResult->Append ( pdescription->Attributes );

                                                ppOD[ndescriptions] = pdescription;
                                                ndescriptions++;
                                        }
                                }


                                pdialog = new S57QueryDialog();

                                pdialog->SetObjectTree((void **)ppOD, rule_list->GetCount());
                                pdialog->SetText ( *QueryResult );

                                pdialog->Create ( this, -1, _( "Object Query" ) );
//                                pdialog->SetSize ( 800, -1 );
                                pdialog->Centre();

                                SetCursor(wxCURSOR_ARROW);
                                pdialog->ShowModal();

                                delete rule_list;
                                delete pdialog;
                                delete QueryResult;

                                for(int j=0 ; j < ndescriptions ; j++)
                                      delete ppOD[j];

                                free (ppOD);

                        }
                        break;
                }
#endif
                case ID_DEF_MENU_AIS_QUERY:

                      if(NULL == g_pais_query_dialog_active)
                      {
                            g_pais_query_dialog_active = new AISTargetQueryDialog();
                            g_pais_query_dialog_active->Create ( this, -1, _( "AIS Target Query" ), wxPoint(g_ais_query_dialog_x, g_ais_query_dialog_y));
                      }

                      g_pais_query_dialog_active->SetMMSI(m_FoundAIS_MMSI);
                      g_pais_query_dialog_active->UpdateText();
                      g_pais_query_dialog_active->Show();

                      break;

              case ID_DEF_MENU_QUILTREMOVE:
              {
                    if(VPoint.b_quilt)
                    {
                          int dbIndex = m_pQuilt->GetChartdbIndexAtPix(wxPoint(popx, popy));
                          parent_frame->RemoveChartFromQuilt(dbIndex);

                          ReloadVP();

                    }

                    break;
              }

                case ID_RT_MENU_REVERSE:

                        pSelect->DeleteAllSelectableRouteSegments ( m_pSelectedRoute );
                        m_pSelectedRoute->Reverse();
                        pSelect->AddAllSelectableRouteSegments ( m_pSelectedRoute );

                        pConfig->UpdateRoute ( m_pSelectedRoute );

                        if ( pRoutePropDialog )
                        {
                              pRoutePropDialog->SetRouteAndUpdate ( m_pSelectedRoute );
                              pRoutePropDialog->UpdateProperties();
                        }

                        break;


                case ID_RT_MENU_DELETE:
                      if ( g_pRouteMan->GetpActiveRoute() == m_pSelectedRoute )
                            g_pRouteMan->DeactivateRoute();

                        pConfig->DeleteConfigRoute ( m_pSelectedRoute );
                        g_pRouteMan->DeleteRoute ( m_pSelectedRoute );
                        m_pSelectedRoute = NULL;
                        m_pFoundRoutePoint = NULL;
                        m_pFoundRoutePointSecond = NULL;
                        if ( pRoutePropDialog )
                        {
                              pRoutePropDialog->SetRouteAndUpdate ( m_pSelectedRoute );
                              pRoutePropDialog->UpdateProperties();
                        }

                        break;

                case ID_RT_MENU_ACTIVATE:
                {
                        if ( g_pRouteMan->GetpActiveRoute() )
                            g_pRouteMan->DeactivateRoute();

                        RoutePoint *best_point = g_pRouteMan->FindBestActivatePoint(m_pSelectedRoute, gLat, gLon, gCog, gSog);

                        g_pRouteMan->ActivateRoute ( m_pSelectedRoute, best_point );
                        m_pSelectedRoute->m_bRtIsSelected = false;

                        break;
                }

                case ID_RT_MENU_DEACTIVATE:
                        g_pRouteMan->DeactivateRoute();
                        m_pSelectedRoute->m_bRtIsSelected = false;

                        break;

                case ID_RT_MENU_INSERT:

                         m_pSelectedRoute->InsertPointBefore ( m_pFoundRoutePointSecond, zlat, zlon );

                        pSelect->DeleteAllSelectableRoutePoints ( m_pSelectedRoute );
                        pSelect->DeleteAllSelectableRouteSegments ( m_pSelectedRoute );

                        pSelect->AddAllSelectableRouteSegments ( m_pSelectedRoute );
                        pSelect->AddAllSelectableRoutePoints ( m_pSelectedRoute );

                        //    As a special case (which comes up often)...
                        //    If the inserted waypoint is on the active leg of an active route
                        /*            if(m_pSelectedRoute->m_bRtIsActive)
                                    {
                                          if(m_pSelectedRoute->m_nRouteActivePoint == np + 1)
                                          {
                                                pNew_Point = m_pSelectedRoute->GetPoint(np + 2);
                                                pRouteMan->ActivateRoutePoint(m_pSelectedRoute, pNew_Point);
                                          }
                                    }
                        */
                        m_pSelectedRoute->RebuildGUIDList();                  // ensure the GUID list is intact and good
                        pConfig->UpdateRoute ( m_pSelectedRoute );

                        if ( pRoutePropDialog )
                        {
                              pRoutePropDialog->SetRouteAndUpdate ( m_pSelectedRoute );
                              pRoutePropDialog->UpdateProperties();
                        }

                        break;

                case ID_RT_MENU_APPEND:
                        m_pMouseRoute = m_pSelectedRoute;
                        parent_frame->nRoute_State = m_pSelectedRoute->GetnPoints() + 1;

                        pLast = m_pSelectedRoute->GetLastPoint();

                        m_prev_rlat = pLast->m_lat;
                        m_prev_rlon = pLast->m_lon;
                        m_prev_pMousePoint = pLast;

                        m_bAppendingRoute = true;

                        SetMyCursor ( pCursorPencil );

                        break;

                case ID_RT_MENU_DELPOINT:
                        if ( m_pSelectedRoute )
                        {
                             // Get a list of all routes containing this point
                             // and remove the point from them all
                              wxArrayPtrVoid *proute_array = g_pRouteMan->GetRouteArrayContaining(m_pFoundRoutePoint);
                              if(proute_array)
                              {
                                    for(unsigned int ir=0 ; ir < proute_array->GetCount() ; ir++)
                                    {
                                          Route *pr = (Route *)proute_array->Item(ir);
                                          pr->RemovePoint ( m_pFoundRoutePoint );

                                    }

                              //    Scrub the routes, looking for one-point routes
                                    for(unsigned int ir=0 ; ir < proute_array->GetCount() ; ir++)
                                    {
                                          Route *pr = (Route *)proute_array->Item(ir);
                                          if(pr->GetnPoints() < 2)
                                          {
                                                pConfig->DeleteConfigRoute ( pr );
                                                g_pRouteMan->DeleteRoute ( pr );
                                                m_pSelectedRoute = NULL;
                                          }
                                    }

                                    delete proute_array;
                              }

                              // Now it is safe to delete the point
                              pConfig->DeleteWayPoint ( m_pFoundRoutePoint );
                              pSelect->DeleteSelectablePoint ( m_pFoundRoutePoint, SELTYPE_ROUTEPOINT );
                              delete m_pFoundRoutePoint;
                              m_pFoundRoutePoint = NULL;

                              if ( pRoutePropDialog )
                              {
                                      pRoutePropDialog->SetRouteAndUpdate ( m_pSelectedRoute );
                                      pRoutePropDialog->UpdateProperties();
                              }
                        }

                        break;

              case ID_RT_MENU_REMPOINT:
                    if ( m_pSelectedRoute )
                    {
                          //  Rebuild the route selectables
                          pSelect->DeleteAllSelectableRoutePoints ( m_pSelectedRoute );
                          pSelect->DeleteAllSelectableRouteSegments ( m_pSelectedRoute );

                          m_pSelectedRoute->RemovePoint ( m_pFoundRoutePoint );

                          //  Check for 1 point routes
                          if ( m_pSelectedRoute->GetnPoints() > 1 )
                          {
                                pSelect->AddAllSelectableRouteSegments ( m_pSelectedRoute );
                                pSelect->AddAllSelectableRoutePoints ( m_pSelectedRoute );

                                pConfig->UpdateRoute ( m_pSelectedRoute );
                                m_pSelectedRoute->RebuildGUIDList();                  // ensure the GUID list is intact and good

                          }
                          else
                          {
                                pConfig->DeleteConfigRoute ( m_pSelectedRoute );
                                g_pRouteMan->DeleteRoute ( m_pSelectedRoute );
                                m_pSelectedRoute = NULL;
                          }
                          //  Add this point back into the selectables
                          pSelect->AddSelectableRoutePoint(m_pFoundRoutePoint->m_lat, m_pFoundRoutePoint->m_lon, m_pFoundRoutePoint);

                          if ( pRoutePropDialog )
                          {
                                pRoutePropDialog->SetRouteAndUpdate ( m_pSelectedRoute );
                                pRoutePropDialog->UpdateProperties();
                          }


                    }
                    break;

              case ID_RT_MENU_ACTPOINT:
                        if ( g_pRouteMan->GetpActiveRoute() == m_pSelectedRoute )
                        {
                                g_pRouteMan->ActivateRoutePoint ( m_pSelectedRoute, m_pFoundRoutePoint );
                                m_pSelectedRoute->m_bRtIsSelected = false;
                        }

                        break;

                case ID_RT_MENU_DEACTPOINT:
                        break;

                case ID_RT_MENU_ACTNXTPOINT:
                        if ( g_pRouteMan->GetpActiveRoute() == m_pSelectedRoute )
                        {
                                g_pRouteMan->ActivateNextPoint ( m_pSelectedRoute );
                                m_pSelectedRoute->m_bRtIsSelected = false;
                        }

                        break;

                case ID_RT_MENU_PROPERTIES:
                {
                        if ( NULL == pRoutePropDialog )          // There is one global instance of the RouteProp Dialog
                                pRoutePropDialog  = new RouteProp ( this );

                        pRoutePropDialog->SetRouteAndUpdate ( m_pSelectedRoute );
                        pRoutePropDialog->UpdateProperties();
                        pRoutePropDialog->SetDialogTitle(_("Route Properties"));

                        pRoutePropDialog->Show();

                        break;
                }

                case ID_RT_MENU_DELETEALL:
                {
                       wxMessageDialog mdlg(this, _("Are you sure you want to delete <ALL> routes?"), wxString(_("OpenCPN Alert")),wxYES_NO  );
                       if(mdlg.ShowModal() == wxID_YES)
                       {
                             if ( g_pRouteMan->GetpActiveRoute() )
                                   g_pRouteMan->DeactivateRoute();

                            g_pRouteMan->DeleteAllRoutes();
                            m_pSelectedRoute = NULL;
                            m_pFoundRoutePoint = NULL;
                            m_pFoundRoutePointSecond = NULL;
                       }

                        break;
                }
                case ID_RT_MENU_SENDTOGPS:
                {
                        SendToGpsDlg *pdlg = new SendToGpsDlg();
                        pdlg->SetRoute(m_pSelectedRoute);

                        pdlg->Create ( NULL, -1, _( "Send To GPS..." ) );
                        pdlg->ShowModal();

                        delete pdlg;

                        break;

                }
                case ID_RT_MENU_EXPORT:
                {
                        pConfig->ExportGPXRoute(this, m_pSelectedRoute);
                        break;
                }


              case ID_TK_MENU_PROPERTIES:
              {
                    if ( NULL == pRoutePropDialog )          // There is one global instance of the RouteProp Dialog
                          pRoutePropDialog  = new RouteProp ( this );

                    pRoutePropDialog->SetRouteAndUpdate ( m_pSelectedTrack );
                    pRoutePropDialog->UpdateProperties();
                    pRoutePropDialog->SetDialogTitle(_("Track Properties"));

                    pRoutePropDialog->Show();

                    Refresh ( false );
                    break;
              }

             case ID_TK_MENU_EXPORT:
              {
                    pConfig->ExportGPXRoute(this, m_pSelectedTrack);
                    break;
              }

              case ID_TK_MENU_DELETE:
              {
                    if((Track *)(m_pSelectedTrack) == g_pActiveTrack)
                        parent_frame->TrackOff();

                    g_pRouteMan->DeleteTrack ( m_pSelectedTrack );
                    m_pSelectedRoute = NULL;
                    m_pSelectedTrack = NULL;
                    m_pFoundRoutePoint = NULL;
                    m_pFoundRoutePointSecond = NULL;

                    if ( pRoutePropDialog )
                    {
                          pRoutePropDialog->SetRouteAndUpdate ( m_pSelectedTrack );
                          pRoutePropDialog->UpdateProperties();
                    }

                    break;
              }


                case ID_RC_MENU_SCALE_IN:
                        parent_frame-> DoStackDown();
                        GetCanvasPointPix ( zlat, zlon, &r );
                        WarpPointer ( r.x, r.y );
                        break;

                case ID_RC_MENU_SCALE_OUT:
                        parent_frame-> DoStackUp();
                        GetCanvasPointPix ( zlat, zlon, &r );
                        WarpPointer ( r.x, r.y );
                        break;

                case ID_RC_MENU_ZOOM_IN:
                        SetVPScale ( GetVPScale() * 2 );
                        GetCanvasPointPix ( zlat, zlon, &r );
                        WarpPointer ( r.x, r.y );
                        break;

                case ID_RC_MENU_ZOOM_OUT:
                        SetVPScale ( GetVPScale() / 2 );
                        GetCanvasPointPix ( zlat, zlon, &r );
                        WarpPointer ( r.x, r.y );
                        break;

                case ID_RC_MENU_FINISH:
                        parent_frame->nRoute_State = 0;
                        parent_frame->SetToolbarItemState ( ID_ROUTE, false );
                        SetMyCursor ( pCursorArrow );
                        m_bDrawingRoute = false;

                        if ( m_pMouseRoute )
                        {
                                if ( m_bAppendingRoute )
                                        pConfig->UpdateRoute ( m_pMouseRoute );
                                else
                                {
                                        if ( m_pMouseRoute->GetnPoints() > 1 )
                                        {
                                                pConfig->AddNewRoute ( m_pMouseRoute, -1 );    // use auto next num
                                        }
                                        else
                                        {
                                              g_pRouteMan->DeleteRoute ( m_pMouseRoute );
                                                m_pMouseRoute = NULL;
                                        }

                                        if ( m_pMouseRoute )
                                                m_pMouseRoute->RebuildGUIDList();         // ensure the GUID list is intact and good
                                }
                                if ( m_pMouseRoute )
                                        m_pMouseRoute->RebuildGUIDList();                  // ensure the GUID list is intact and good

                                if ( pRoutePropDialog )
                                {
                                      pRoutePropDialog->SetRouteAndUpdate ( m_pMouseRoute );
                                      pRoutePropDialog->UpdateProperties();
                                }


                        }
                        m_bAppendingRoute = false;
                        m_pMouseRoute = NULL;

                        m_pSelectedRoute = NULL;
//                        m_pFoundRoutePoint = NULL;
                        m_pFoundRoutePointSecond = NULL;

                        Refresh ( false );
                        break;

        }           // switch


        g_click_stop = 0;    // Context menu was processed, all is well

}

void ChartCanvas::ShowRouteManager(void)
{
      if ( NULL == pRouteManagerDialog )          // There is one global instance of the Dialog
            pRouteManagerDialog = new RouteManagerDialog ( this );


      pRouteManagerDialog->UpdateRouteListCtrl();
      pRouteManagerDialog->Show();
}

void ChartCanvas::ShowAISTargetList(void)
{
      if ( NULL == g_pAISTargetList ) {         // There is one global instance of the Dialog
            g_pAISTargetList = new AISTargetListDialog ( );
            g_pAISTargetList->Create ( this, g_pAIS, wxID_ANY,
                                       wxString(_("AIS target list")),
                                        wxDefaultPosition, wxSize( -1, -1 ),
                                        wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
     }

      g_pAISTargetList->UpdateAISTargetList();
      g_pAISTargetList->Show();
}


void ChartCanvas::RenderAllChartOutlines ( wxDC *pdc, ViewPort& vp, bool bdraw_mono )
{
        int nEntry = ChartData->GetChartTableEntries();

        for ( int i=0 ; i < nEntry ; i++ )
        {
                RenderChartOutline ( pdc, i, vp, bdraw_mono );
        }

//      Could render in different color/width if thumbnail is selected
//    if(NULL !=  pthumbwin->pThumbChart)
//        int ggl = 4;


 //        On CM93 Composite Charts, draw the outlines of the next smaller scale cell
        if(Current_Ch && (Current_Ch->m_ChartType == CHART_TYPE_CM93COMP))
        {
              cm93compchart *pch = (cm93compchart *)Current_Ch;
              if(pch)
              {
                    wxPen mPen(GetGlobalColor(_T("UINFM")), 1, wxSOLID);
                    pdc->SetPen(mPen);
                    pch->RenderNextSmallerCellOutlines(pdc,vp, bdraw_mono);
              }
        }
}



void ChartCanvas::RenderChartOutline ( wxDC *pdc, int dbIndex, ViewPort& vp, bool bdraw_mono_for_mask )
{
        float plylat, plylon;
        float plylat1, plylon1;

        int pixx, pixy, pixx1, pixy1;
        bool b_draw = false;
        double lon_bias = 0.;

        wxBoundingBox box;
        ChartData->GetDBBoundingBox ( dbIndex, &box );

        if ( vp.vpBBox.Intersect ( box, 0 ) != _OUT )               // chart is not outside of viewport
              b_draw = true;

        //  Does simple test fail, and current vp cross international dateline?
        if(!b_draw && ((vp.vpBBox.GetMinX() < -180.) || (vp.vpBBox.GetMaxX() > 180.)))
        {
              //  If so, do an explicit test with alternate phasing
              if(vp.vpBBox.GetMinX() < -180.)
              {
                    wxPoint2DDouble p(-360., 0);
                    box.Translate(p);
                    if ( vp.vpBBox.Intersect ( box, 0 ) != _OUT )               // chart is not outside of viewport
                    {
                          b_draw = true;
                          lon_bias = -360.;
                    }
              }
              else
              {
                    wxPoint2DDouble p(360., 0);
                    box.Translate(p);
                    if ( vp.vpBBox.Intersect ( box, 0 ) != _OUT )               // chart is not outside of viewport
                    {
                          b_draw = true;
                          lon_bias = 360.;
                    }
            }

        }

        //  Does simple test fail, and chart box cross international dateline?
        if(!b_draw && (box.GetMinX() < 180.) && (box.GetMaxX() > 180.))
        {
              wxPoint2DDouble p(-360., 0);
              box.Translate(p);
              if ( vp.vpBBox.Intersect ( box, 0 ) != _OUT )               // chart is not outside of viewport
              {
                    b_draw = true;
                    lon_bias = -360.;
              }
        }



        if(!b_draw)
              return;


        int nPly = ChartData->GetDBPlyPoint ( dbIndex, 0, &plylat, &plylon );

        if ( ChartData->GetDBChartType ( dbIndex ) == CHART_TYPE_S57 )
              pdc->SetPen ( wxPen( GetGlobalColor ( _T ( "UINFG" ) ), 1, wxSOLID ) );

        else if ( ChartData->GetDBChartType ( dbIndex ) == CHART_TYPE_CM93 )
              pdc->SetPen (  wxPen(GetGlobalColor ( _T ( "YELO1" ) ), 1, wxSOLID  ) );

        else
              pdc->SetPen ( wxPen( GetGlobalColor ( _T ( "UINFR" ) ), 1, wxSOLID ) );

        if ( bdraw_mono_for_mask )
        {
                wxPen pp ( *wxWHITE, 2, wxSOLID );
                pdc->SetPen ( pp );
        }


        //        Are there any aux ply entries?
        int nAuxPlyEntries = ChartData->GetnAuxPlyEntries(dbIndex);
        if(0 == nAuxPlyEntries)                 // There are no aux Ply Point entries
        {
            wxPoint r, r1;

            ChartData->GetDBPlyPoint ( dbIndex, 0, &plylat, &plylon );
            plylon += lon_bias;

            GetCanvasPointPix ( plylat, plylon, &r );
            pixx = r.x;
            pixy = r.y;

            for ( int i=0 ; i<nPly-1 ; i++ )
            {
                  ChartData->GetDBPlyPoint ( dbIndex, i+1, &plylat1, &plylon1 );
                  plylon1 += lon_bias;

                  GetCanvasPointPix ( plylat1, plylon1, &r1 );
                  pixx1 = r1.x;
                  pixy1 = r1.y;

                  int pixxs1 = pixx1;
                  int pixys1 = pixy1;

                  bool b_skip = false;

                  if(vp.chart_scale > 5e7)
                  {
                        //    calculate projected distance between these two points in meters
                        double dist = sqrt((double)((pixx1-pixx) * (pixx1 - pixx)) + ((pixy1-pixy) * (pixy1 - pixy))) / vp.view_scale_ppm;
                        //    calculate GC distance between these two points in meters
                        double distgc = DistGreatCircle(plylat, plylon, plylat1, plylon1) * 1852.;

                        //    If the distances are nonsense, it means that the scale is very small and the segment wrapped the world
                        //    So skip it....
                        //    TODO improve this to draw two segments
                        if(fabs(dist - distgc) > 10000. * 1852.)          //lotsa miles
                              b_skip = true;
                  }

                  ClipResult res = cohen_sutherland_line_clip_i ( &pixx, &pixy, &pixx1, &pixy1,
                                    0, vp.pix_width, 0, vp.pix_height );
                  if ( res != Invisible  && !b_skip)
                              pdc->DrawLine ( pixx, pixy, pixx1, pixy1 );

                  plylat = plylat1;
                  plylon = plylon1;
                  pixx = pixxs1;
                  pixy = pixys1;
            }

            ChartData->GetDBPlyPoint ( dbIndex, 0, &plylat1, &plylon1 );
            plylon1 += lon_bias;

            GetCanvasPointPix ( plylat1, plylon1, &r1 );
            pixx1 = r1.x;
            pixy1 = r1.y;

            ClipResult res = cohen_sutherland_line_clip_i ( &pixx, &pixy, &pixx1, &pixy1,
                              0, vp.pix_width, 0, vp.pix_height );
            if ( res != Invisible )
                  pdc->DrawLine ( pixx, pixy, pixx1, pixy1 );
        }

        else                              // Use Aux PlyPoints
        {
                      wxPoint r, r1;

                      int nAuxPlyEntries = ChartData->GetnAuxPlyEntries(dbIndex);
                      for(int j=0 ; j<nAuxPlyEntries ; j++)
                      {

                          int nAuxPly =  ChartData->GetDBAuxPlyPoint(dbIndex, 0, j, &plylat, &plylon);
                          GetCanvasPointPix(plylat, plylon, &r);
                          pixx = r.x;
                          pixy = r.y;

                          for( int i=0 ; i<nAuxPly-1 ; i++)
                          {
                            ChartData->GetDBAuxPlyPoint(dbIndex, i+1, j, &plylat1, &plylon1);

                            GetCanvasPointPix(plylat1, plylon1, &r1);
                            pixx1 = r1.x;
                            pixy1 = r1.y;

                            int pixxs1 = pixx1;
                            int pixys1 = pixy1;

                            bool b_skip = false;

                            if(vp.chart_scale > 5e7)
                            {
                              //    calculate projected distance between these two points in meters
                                    double dist = sqrt((double)((pixx1-pixx) * (pixx1 - pixx)) + ((pixy1-pixy) * (pixy1 - pixy))) / vp.view_scale_ppm;
                              //    calculate GC distance between these two points in meters
                                    double distgc = DistGreatCircle(plylat, plylon, plylat1, plylon1) * 1852.;

                              //    If the distances are nonsense, it means that the scale is very small and the segment wrapped the world
                              //    So skip it....
                              //    TODO improve this to draw two segments
                                    if(fabs(dist - distgc) > 10000. * 1852.)          //lotsa miles
                                          b_skip = true;
                            }

                            ClipResult res = cohen_sutherland_line_clip_i (&pixx, &pixy, &pixx1, &pixy1,
                                        0, vp.pix_width, 0, vp.pix_height);
                            if(res != Invisible && !b_skip)
                                  pdc->DrawLine(pixx, pixy, pixx1, pixy1);

                            plylat = plylat1;
                            plylon = plylon1;
                            pixx = pixxs1;
                            pixy = pixys1;
                          }

                          ChartData->GetDBAuxPlyPoint(dbIndex, 0, j, &plylat1, &plylon1);
                          GetCanvasPointPix(plylat1, plylon1, &r1);
                          pixx1 = r1.x;
                          pixy1 = r1.y;

                          ClipResult res = cohen_sutherland_line_clip_i (&pixx, &pixy, &pixx1, &pixy1,
                                  0, vp.pix_width, 0, vp.pix_height);
                          if(res != Invisible)
                            pdc->DrawLine(pixx, pixy, pixx1, pixy1);
                      }
        }

}

void ChartCanvas::WarpPointerDeferred ( int x, int y )
{
        warp_x = x;
        warp_y = y;
        warp_flag = true;
}


void RenderRouteLegInfo(wxMemoryDC *dc, double lata, double lona, double latb, double lonb, wxPoint ref_point, wxString prefix)
{

      double brg, dist;
      DistanceBearingMercator(latb, lonb, lata, lona, &brg, &dist);   // for brg only

      if((lata == latb) && (lona ==  lonb))               // special optimization
            brg = 90.;

      wxString s;
      if ( dist > 0.1 )                                                                 //pjotrc 2010.02.16
            s.Printf(_T("%03d Deg %6.2f NMi"), (int)brg, dist);
      else                                                                                    //pjotrc 2010.02.16
            s.Printf(_T("%03d Deg %4.1f (m)"), (int)brg, dist*1852.); //pjotrc 2010.02.16

      s.Prepend(prefix);

      int w, h;
      double angle;
      int xp, yp, xp1, yp1;
      int hilite_offset = 3;
      dc->GetTextExtent(s, &w, &h);

      if(true/*(brg >= 0.) && (brg < 180.)*/)
      {
            angle = 0; //90. - brg;

            xp = ref_point.x  - (int)(w * cos((angle) * PI / 180.));
            yp = ref_point.y  + (int)(w * sin((angle) * PI / 180.));

            xp += (int)(hilite_offset * sin((angle) * PI / 180.));
            yp += (int)(hilite_offset * cos((angle) * PI / 180.));

            xp1 = ref_point.x;
            yp1 = ref_point.y;
            xp1 += (int)(hilite_offset * sin((angle) * PI / 180.));
            yp1 += (int)(hilite_offset * cos((angle) * PI / 180.));

      }
      else
      {
            angle = 0; //270. - brg;

            xp = ref_point.x;
            yp = ref_point.y;
            xp += (int)(hilite_offset * sin((angle) * PI / 180.));
            yp += (int)(hilite_offset * cos((angle) * PI / 180.));

            xp1 = ref_point.x  + (int)(w * cos((angle) * PI / 180.));
            yp1 = ref_point.y  - (int)(w * sin((angle) * PI / 180.));
            xp1 += (int)(hilite_offset * sin((angle) * PI / 180.));
            yp1 += (int)(hilite_offset * cos((angle) * PI / 180.));

      }

      wxPoint hilite_array[4];
      hilite_array[0].x = xp;
      hilite_array[0].y = yp;

      hilite_array[1].x = xp + (int)((h) * sin((angle) * PI / 180.));
      hilite_array[1].y = yp + (int)((h) * cos((angle) * PI / 180.));

      hilite_array[2].x = xp1 + (int)((h) * sin((angle) * PI / 180.));
      hilite_array[2].y = yp1 + (int)((h) * cos((angle) * PI / 180.));

      hilite_array[3].x = xp1;
      hilite_array[3].y = yp1;

      dc->SetPen ( wxPen ( GetGlobalColor ( _T ( "YELO1" ) ) ) );
      dc->SetBrush ( wxBrush ( GetGlobalColor ( _T ( "YELO1" ) ) ) );
      dc->DrawPolygon(4, hilite_array);

      dc->SetPen ( wxPen ( GetGlobalColor ( _T ( "UBLCK" ) ) ) );
      dc->DrawRotatedText(s, xp, yp, angle);
}



void ChartCanvas::OnPaint ( wxPaintEvent& event )
{
        int rx, ry, rwidth, rheight;

        wxPaintDC dc ( this );

        wxRegion ru = GetUpdateRegion();
        ru.GetBox ( rx, ry, rwidth, rheight );
/*
        if ( !VPoint.IsValid() )
        {
//              printf("Invalid vp\n");
                return;
        }
*/


        wxBoundingBox BltBBox;

#ifdef ocpnUSE_DIBSECTION
        ocpnMemDC temp_dc;
#else
        wxMemoryDC temp_dc;
#endif

//    In case Console is shown, set up dc clipper and blt iterator regions

        wxRegion rgn_chart ( 0,0,VPoint.pix_width, VPoint.pix_height );
        int conx, cony, consx, consy;
        console->GetPosition ( &conx, &cony );
        console->GetSize ( &consx, &consy );
        wxRegion rgn_console ( conx, cony, consx - 1, consy - 1 );

        if ( console->IsShown() )
        {
                rgn_chart.Subtract ( rgn_console );               // For dc Drawing clipping
                ru.Subtract ( rgn_console );                      // for Blit updating
        }

//    Same for Thumbnail window
        if ( pthumbwin )
        {
                int thumbx, thumby, thumbsx, thumbsy;
                pthumbwin->GetPosition ( &thumbx, &thumby );
                pthumbwin->GetSize ( &thumbsx, &thumbsy );
                wxRegion rgn_thumbwin ( thumbx, thumby, thumbsx - 1, thumbsy - 1 );

                if ( pthumbwin->IsShown() )
                {
                        rgn_chart.Subtract ( rgn_thumbwin );
                        ru.Subtract ( rgn_thumbwin );
                }
        }

        //  Is this viewpoint the same as the previously painted one?
        bool b_newview = true;;
        if((m_cache_vp.view_scale_ppm == VPoint.view_scale_ppm)
                  && (m_cache_vp.rotation == VPoint.rotation)
                  && (m_cache_vp.clat == VPoint.clat)
                  && (m_cache_vp.clon == VPoint.clon)
                  &&  m_cache_vp.IsValid()
                )
              {
                    b_newview = false;
              }


        //  If in COG UP Mode, we may be able to use the cached rotated bitmap
        bool b_rcache_ok = false;
        if(g_bCourseUp && (fabs(VPoint.rotation) > 0.01))
              b_rcache_ok = !b_newview;


        //  Make a special VP
        ViewPort svp = VPoint;

        svp.pix_width = svp.rv_rect.width;
        svp.pix_height = svp.rv_rect.height;

        wxRegion chart_get_region(wxRect(0,0,svp.pix_width, svp.pix_height));

        //  If we are going to use the cached rotated image, there is no need to fetch any chart data
        //  and this will do it...
        if(b_rcache_ok)
            chart_get_region.Clear();

        if(VPoint.b_quilt)          // quilted
        {
            if(!m_pQuilt->RenderQuiltRegionViewOnDC ( temp_dc, svp, chart_get_region, current_scale_method ))
            { return;}
        }
        else                  // not quilted
        {
              if(!Current_Ch)
              {
                    dc.SetBackground(wxBrush(*wxLIGHT_GREY));
                    dc.Clear();
                    return;
              }

              Current_Ch->SetVPParms(svp);
              Current_Ch->RenderViewOnDC(temp_dc, svp, current_scale_method);
        }

        if(!temp_dc.Ok())
            { return;}


//    Arrange to render the WVSChart vector data ..BEHIND.. the rendered current chart
//    So that uncovered canvas areas show at least the WVS chart
        wxRegion CValidRegion;
        if(!VPoint.b_quilt)
              Current_Ch->GetValidCanvasRegion ( VPoint, &CValidRegion );     // Make a region covering the current chart on the canvas
        else
              CValidRegion = m_pQuilt->GetFullQuiltRegion();


      //      CValidRegion.Clear(); CValidRegion.Union(0,  0, 1, 1);          // This line for debug, to show all of WVS chart

      //    Copy current chart region
        wxRegion WVSRegion ( rgn_chart );

      //    Remove the valid chart area
        if(CValidRegion.IsOk())
              WVSRegion.Subtract ( CValidRegion );

      //    Associate with temp_dc
        temp_dc.DestroyClippingRegion();
        temp_dc.SetClippingRegion ( WVSRegion );

      //    Draw the WVSChart only in the areas NOT covered by the current chart view
      //    And, only if the region is ..not.. empty
      //    (exp.) only draw WVS if scale is sufficiently large, since it is so slow for large windows
        if ( !WVSRegion.IsEmpty() && ( fabs (VPoint.skew) < .01 ) && (VPoint.view_scale_ppm > 5e-05) )
                  pwvs_chart->RenderViewOnDC ( temp_dc, VPoint );


        wxMemoryDC *pChartDC = &temp_dc;
        wxMemoryDC rotd_dc;


            if(g_bCourseUp && (fabs(VPoint.rotation) > 0.01))
            {

                  //  Can we use the current rotated image cache?
                  if(!b_rcache_ok)
                  {
#ifdef __WXMSW__
                        wxMemoryDC tbase_dc;
                        wxBitmap bm_base(svp.pix_width, svp.pix_height, -1);
                        tbase_dc.SelectObject(bm_base);
                        tbase_dc.Blit(0,0,svp.pix_width, svp.pix_height, &temp_dc, 0, 0);
                        tbase_dc.SelectObject ( wxNullBitmap );
#else
                        const wxBitmap &bm_base = temp_dc.GetSelectedBitmap();
#endif

                        wxImage base_image;
                        if(bm_base.IsOk())
                              base_image = bm_base.ConvertToImage();

                        //    Use a local static image rotator to improve wxWidgets code profile
                        //    Especially, on GTK the wxRound and wxRealPoint functions are very expensive.....
                        double angle;
                        angle = -VPoint.rotation;

                        wxImage ri;
                        bool b_rot_ok = false;
                        if(base_image.IsOk())
                        {
                              ViewPort rot_vp = VPoint;
                              m_b_rot_hidef = false;
                              ri = Image_Rotate(base_image, angle, wxPoint(VPoint.rv_rect.width/2, VPoint.rv_rect.height/2), m_b_rot_hidef, &m_roffset);

                              if((rot_vp.view_scale_ppm == VPoint.view_scale_ppm)
                                  && (rot_vp.rotation == VPoint.rotation)
                                  && (rot_vp.clat == VPoint.clat)
                                  && (rot_vp.clon == VPoint.clon)
                                  &&  rot_vp.IsValid()
                                  && (ri.IsOk())
                                )
                              {
                                    b_rot_ok = true;
                              }
                        }

                        if(b_rot_ok)
                        {
                              delete m_prot_bm;
                              m_prot_bm = new wxBitmap(ri);

                        }

                        m_roffset.x += VPoint.rv_rect.x;
                        m_roffset.y += VPoint.rv_rect.y;

                  }

                  if(m_prot_bm && m_prot_bm->IsOk())
                  {
                        rotd_dc.SelectObject(*m_prot_bm);
                        pChartDC = &rotd_dc;
                  }
                  else
                  {
                        pChartDC = &temp_dc;
                        m_roffset = wxPoint(0,0);
                  }


            }
            else
            {
                  pChartDC = &temp_dc;
                  m_roffset = wxPoint(0,0);

            }

        wxPoint offset = m_roffset;

        //        Save the PixelCache viewpoint for next time
        m_cache_vp = VPoint;


//    Set up a scratch DC for overlay objects
        wxRegion rgn_blit;
        wxMemoryDC scratch_dc;
        scratch_dc.SelectObject ( *pscratch_bm );

        scratch_dc.ResetBoundingBox();
        scratch_dc.DestroyClippingRegion();
        scratch_dc.SetClippingRegion ( rgn_chart );


        //    Blit the externally invalidated areas of the chart onto the scratch dc
        rgn_blit = ru;
        wxRegionIterator upd ( rgn_blit ); // get the update rect list
        while ( upd )
        {
                wxRect rect = upd.GetRect();

                scratch_dc.Blit ( rect.x, rect.y, rect.width, rect.height,
                                  pChartDC, rect.x - offset.x, rect.y - offset.y  );
//                                  pChartDC, rect.x, rect.y );
///          printf("   Scratch Blit %d %d %d %d\n",rect.x, rect.y, rect.width, rect.height);
                upd ++ ;
        }

//    Process the array of registered overlay providers
        for(unsigned int i=0 ; i < m_OverlaySpecArray.GetCount() ; i++)
        {
              OverlaySpec *olspec = (OverlaySpec *)m_OverlaySpecArray.Item(i);

              //  Do the callback
              (*olspec->m_render_callback)(&scratch_dc, &VPoint);            //(*pcallback)()
        }


        //      If Depth Unit Display is selected, emboss it
        if ( g_bShowDepthUnits )
        {
              int depth_unit_type = DEPTH_UNIT_UNKNOWN;

              if(GetQuiltMode())
              {
                    wxString s =m_pQuilt->GetQuiltDepthUnit();
                    s.MakeUpper();
                    if(s == _T("FEET"))
                       depth_unit_type = DEPTH_UNIT_FEET;
                    else if(s.StartsWith(_T("FATHOMS")))
                       depth_unit_type = DEPTH_UNIT_FATHOMS;
                    else if(s.StartsWith(_T("METERS")))
                          depth_unit_type = DEPTH_UNIT_METERS;
                    else if(s.StartsWith(_T("METRES")))
                          depth_unit_type = DEPTH_UNIT_METERS;

              }
              else
              {
                    if(Current_Ch)
                    {
                          depth_unit_type = Current_Ch->GetDepthUnitType();

                          if(Current_Ch->m_ChartFamily == CHART_FAMILY_VECTOR)
                                depth_unit_type =  ps52plib->m_nDepthUnitDisplay + 1;
                    }
              }

              EmbossDepthScale ( &scratch_dc, &scratch_dc, depth_unit_type );
        }



            //        If extremely overzoomed, show the embossed informational symbol
            if(g_bshow_overzoom_emboss)
            {

                  if(GetQuiltMode())
                  {
                        double chart_native_ppm;
                        chart_native_ppm = m_canvas_scale_factor / m_pQuilt->GetRefNativeScale();

//                        chart_native_ppm = m_true_scale_ppm;

                        double zoom_factor = VPoint.view_scale_ppm / chart_native_ppm;

                        if (zoom_factor > 4.0)
                              EmbossOverzoomIndicator ( &scratch_dc, &scratch_dc);
                  }
                  else
                  {
                        double chart_native_ppm;
                        if(Current_Ch)
                              chart_native_ppm = m_canvas_scale_factor / Current_Ch->GetNativeScale();
                        else
                              chart_native_ppm = m_true_scale_ppm;

                        double zoom_factor = VPoint.view_scale_ppm / chart_native_ppm;

                        if(Current_Ch)
                        {
                  //    Special case for cm93
                              if(Current_Ch->m_ChartType == CHART_TYPE_CM93COMP)
                              {
                                    if(zoom_factor > 8.0)
                                    {

                                          cm93compchart *pch = (cm93compchart *)Current_Ch;
                                          if(pch)
                                          {
                                                wxPen mPen(GetGlobalColor(_T("UINFM")), 2, wxSHORT_DASH);
                                                scratch_dc.SetPen(mPen);
                                                pch->RenderNextSmallerCellOutlines(&scratch_dc, VPoint, false);
                                          }

                                          EmbossOverzoomIndicator ( &scratch_dc, &scratch_dc);
                                    }
                              }

                              else if(zoom_factor > 4.0)
                              {
                                    EmbossOverzoomIndicator ( &scratch_dc, &scratch_dc);
                              }
                        }
                  }
            }


//    Draw the rest of the overlay objects directly on the scratch dc

        scratch_dc.SetClippingRegion ( rgn_chart );

        DrawAllRoutesInBBox ( scratch_dc, VPoint.vpBBox, ru );
        DrawAllWaypointsInBBox ( scratch_dc, VPoint.vpBBox, ru, true ); // true draws only isolated marks

        ShipDraw ( scratch_dc );
        AISDraw ( scratch_dc );

        AlertDraw( scratch_dc );               // pjotrc 2010.02.22

        if ( g_bShowOutlines )
                RenderAllChartOutlines ( &scratch_dc, VPoint ) ;

        if ( parent_frame->nRoute_State >= 2 )
        {
                wxPoint rpt;
                m_pMouseRoute->DrawPointWhich ( scratch_dc, parent_frame->nRoute_State - 1,  &rpt );
                m_pMouseRoute->DrawSegment ( scratch_dc, &rpt, &r_rband, VPoint, false );
        }


        wxFont *pFont = wxTheFontList->FindOrCreateFont(12, wxFONTFAMILY_MODERN, wxNORMAL, wxNORMAL);
        if(pFont)
              scratch_dc.SetFont(*pFont);

        if(m_bShowSegInfo)
        {
              wxPoint r;
              GetCanvasPointPix (m_cursor_lat, m_cursor_lon, &r );

              wxString s = _T("1");
              int h;
              scratch_dc.GetTextExtent(s, NULL, &h);

              r.y -= h;

              if(m_segShow_point_a && m_segShow_point_b)
                  RenderRouteLegInfo(&scratch_dc, m_segShow_point_a->m_lat, m_segShow_point_a->m_lon,
                                 m_segShow_point_b->m_lat, m_segShow_point_b->m_lon, r, _("Leg:  "));
        }


        if ( m_pMeasureRoute && m_bMeasure_Active && ( m_nMeasureState > 1) )
        {
              wxPoint rpt;
              m_pMeasureRoute->DrawPointWhich ( scratch_dc, 1,  &rpt );
              m_pMeasureRoute->DrawSegment ( scratch_dc, &rpt, &r_rband, VPoint, false );

              RenderRouteLegInfo(&scratch_dc, m_prev_rlat, m_prev_rlon,
                                  m_cursor_lat, m_cursor_lon, r_rband, _T(""));


///
#if 0
              double brg, dist;
              DistanceBearingMercator(m_cursor_lat, m_cursor_lon, m_prev_rlat, m_prev_rlon, &brg, &dist);   // for brg only
//              dist = DistGreatCircle(m_cursor_lat, m_cursor_lon, m_prev_rlat, m_prev_rlon);

              if((m_cursor_lat == m_prev_rlat) && (m_cursor_lon ==  m_prev_rlon))               // special optimization
                    brg = 90.;

              wxString s;
              if ( dist > 0.1 )                                                                 //pjotrc 2010.02.16
                    s.Printf(_T("%03d Deg %6.2f NMi"), (int)brg, dist);
              else                                                                                    //pjotrc 2010.02.16
                    s.Printf(_T("%03d Deg %4.1f (m)"), (int)brg, dist*1852.); //pjotrc 2010.02.16

              int w, h;
              double angle;
              int xp, yp, xp1, yp1;
              int hilite_offset = 3;
              scratch_dc.GetTextExtent(s, &w, &h);

              if(true/*(brg >= 0.) && (brg < 180.)*/)
              {
                    angle = 0; //90. - brg;

                    xp = r_rband.x  - (int)(w * cos((angle) * PI / 180.));
                    yp = r_rband.y  + (int)(w * sin((angle) * PI / 180.));

                    xp += (int)(hilite_offset * sin((angle) * PI / 180.));
                    yp += (int)(hilite_offset * cos((angle) * PI / 180.));

                    xp1 = r_rband.x;
                    yp1 = r_rband.y;
                    xp1 += (int)(hilite_offset * sin((angle) * PI / 180.));
                    yp1 += (int)(hilite_offset * cos((angle) * PI / 180.));

              }
              else
              {
                    angle = 0; //270. - brg;

                    xp = r_rband.x;
                    yp = r_rband.y;
                    xp += (int)(hilite_offset * sin((angle) * PI / 180.));
                    yp += (int)(hilite_offset * cos((angle) * PI / 180.));

                    xp1 = r_rband.x  + (int)(w * cos((angle) * PI / 180.));
                    yp1 = r_rband.y  - (int)(w * sin((angle) * PI / 180.));
                    xp1 += (int)(hilite_offset * sin((angle) * PI / 180.));
                    yp1 += (int)(hilite_offset * cos((angle) * PI / 180.));

             }

              wxPoint hilite_array[4];
              hilite_array[0].x = xp;
              hilite_array[0].y = yp;

              hilite_array[1].x = xp + (int)((h) * sin((angle) * PI / 180.));
              hilite_array[1].y = yp + (int)((h) * cos((angle) * PI / 180.));

              hilite_array[2].x = xp1 + (int)((h) * sin((angle) * PI / 180.));
              hilite_array[2].y = yp1 + (int)((h) * cos((angle) * PI / 180.));

              hilite_array[3].x = xp1;
              hilite_array[3].y = yp1;

              scratch_dc.SetPen ( wxPen ( GetGlobalColor ( _T ( "YELO1" ) ) ) );
              scratch_dc.SetBrush ( wxBrush ( GetGlobalColor ( _T ( "YELO1" ) ) ) );
              scratch_dc.DrawPolygon(4, hilite_array);

              scratch_dc.SetPen ( wxPen ( GetGlobalColor ( _T ( "UBLCK" ) ) ) );
              scratch_dc.DrawRotatedText(s, xp, yp, angle);
///
#endif
        }

        //  Draw S52 compatible Scale Bar
        wxCoord w, h;
        scratch_dc.GetSize(&w, &h);
        ScaleBarDraw( scratch_dc, 20, h - 20 );

//  Using yet another bitmap and DC, draw semi-static overlay objects if necessary

        /*    Why go to all this trouble?
        Answer:  Calculating and rendering tides and currents is expensive,
        and the data only change every 15 minutes or so.  So, keep a "mask blit-able"
        copy in persistent storage, and use as necessary.
        */
        if ( m_bShowTide || m_bShowCurrent )         // Showing T/C?
        {
                if ( 1/*b_newview*/ || m_bTCupdate )         // need to update the overlay
                {
                        delete pss_overlay_bmp;
                        pss_overlay_bmp = DrawTCCBitmap(&scratch_dc);
                }

                //    blit the semi-static overlay onto the scratch DC if it is needed
                if ( NULL != pss_overlay_bmp )
                {
                        wxMemoryDC ssdc_r;
                        ssdc_r.SelectObject ( *pss_overlay_bmp );

                        wxRegionIterator upd_final ( rgn_blit );
                        while ( upd_final )
                        {
                                wxRect rect = upd_final.GetRect();
                                scratch_dc.Blit ( rect.x, rect.y, rect.width, rect.height,
                                                  &ssdc_r, rect.x, rect.y, wxCOPY, true );      // Blit with mask
                                upd_final ++ ;
                        }

                        ssdc_r.SelectObject ( wxNullBitmap );
                }
        }


        //quiting?
        if(g_bquiting)
        {
#ifdef ocpnUSE_DIBSECTION
              ocpnMemDC q_dc;
#else
              wxMemoryDC q_dc;
#endif
              wxBitmap qbm(VPoint.pix_width, VPoint.pix_height);
              q_dc.SelectObject(qbm);

              // Get a copy of the screen
              q_dc.Blit(0, 0, VPoint.pix_width, VPoint.pix_height, &scratch_dc, 0, 0);

              //  Draw a rectangle over the screen with a stipple brush
              wxBrush qbr(*wxBLACK, wxFDIAGONAL_HATCH);
              q_dc.SetBrush(qbr);
              q_dc.DrawRectangle(0, 0, VPoint.pix_width, VPoint.pix_height);

              // Blit back into source
              scratch_dc.Blit(0, 0, VPoint.pix_width, VPoint.pix_height, &q_dc, 0, 0, wxCOPY  );

              q_dc.SelectObject ( wxNullBitmap );

        }


//    And finally, blit the scratch dc onto the physical dc
        wxRegionIterator upd_final ( rgn_blit );
        while ( upd_final )
        {
                wxRect rect = upd_final.GetRect();
                dc.Blit ( rect.x, rect.y, rect.width, rect.height,
                          &scratch_dc, rect.x, rect.y );
                upd_final ++ ;
        }

//    Deselect the chart bitmap from the temp_dc, so that it will not be destroyed in the temp_dc dtor
        temp_dc.SelectObject ( wxNullBitmap );
//    And for the scratch bitmap
        scratch_dc.SelectObject ( wxNullBitmap );


//    Draw a crosshair at the viewport center for debugging
//        dc.DrawLine((VPoint.pix_width/2)-10, VPoint.pix_height/2,(VPoint.pix_width/2)+10, VPoint.pix_height/2);
//        dc.DrawLine((VPoint.pix_width/2), (VPoint.pix_height/2)-10,(VPoint.pix_width/2), (VPoint.pix_height/2) +10);


//    Handle the current graphic window, if present

        if ( pCwin )
        {
                pCwin->Show();
                if ( m_bTCupdate )
                {
                        pCwin->m_bForceTCRedraw = true;
                        pCwin->Refresh();
                        pCwin->Update();
                }
        }

//    And set flags for next time
        m_bTCupdate = false;

        dc.DestroyClippingRegion();


//    Handle deferred WarpPointer
        if ( warp_flag )
        {
                WarpPointer ( warp_x, warp_y );
                warp_flag = false;
        }

}


#if 0
wxColour GetErrorGraphicColor(double val)
{
/*
      double valm = wxMin(val_max, val);

      unsigned char green = (unsigned char)(255 * (1 - (valm/val_max)));
      unsigned char red   = (unsigned char)(255 * (valm/val_max));

      wxImage::HSVValue hv = wxImage::RGBtoHSV(wxImage::RGBValue(red, green, 0));

      hv.saturation = 1.0;
      hv.value = 1.0;

      wxImage::RGBValue rv = wxImage::HSVtoRGB(hv);
      return wxColour(rv.red, rv.green, rv.blue);
*/

      //    HTML colors taken from NOAA WW3 Web representation

      wxColour c;
      if((val > 0) && (val < 1))         c.Set(_T("#002ad9"));
      else if((val >= 1)  && (val < 2))   c.Set(_T("#006ed9"));
      else if((val >= 2)  && (val < 3))   c.Set(_T("#00b2d9"));
      else if((val >= 3)  && (val < 4))   c.Set(_T("#00d4d4"));
      else if((val >= 4)  && (val < 5))   c.Set(_T("#00d9a6"));
      else if((val >= 5)  && (val < 7))   c.Set(_T("#00d900"));
      else if((val >= 7)  && (val < 9))   c.Set(_T("#95d900"));
      else if((val >= 9)  && (val < 12))  c.Set(_T("#d9d900"));
      else if((val >= 12) && (val < 15))  c.Set(_T("#d9ae00"));
      else if((val >= 15) && (val < 18))  c.Set(_T("#d98300"));
      else if((val >= 18) && (val < 21))  c.Set(_T("#d95700"));
      else if((val >= 21) && (val < 24))  c.Set(_T("#d90000"));
      else if((val >= 24) && (val < 27))  c.Set(_T("#ae0000"));
      else if((val >= 27) && (val < 30))  c.Set(_T("#8c0000"));
      else if((val >= 30) && (val < 36))  c.Set(_T("#870000"));
      else if((val >= 36) && (val < 42))  c.Set(_T("#690000"));
      else if((val >= 42) && (val < 48))  c.Set(_T("#550000"));
      else if( val >= 48)                 c.Set(_T("#410000"));

      return c;
}

void ChartCanvas::RenderGeorefErrorMap( wxMemoryDC *pmdc, ViewPort *vp)
{
      wxImage gr_image(vp->pix_width, vp->pix_height);
      gr_image.InitAlpha();

      double maxval = -10000;
      double minval =  10000;

      double rlat, rlon;
      double glat, glon;

      GetCanvasPixPoint(0, 0, rlat, rlon);

      for(int i=1 ; i < vp->pix_height-1  ; i++)
      {
            for(int j=0 ; j < vp->pix_width ; j++)
            {
                  // Reference mercator value
//                  vp->GetMercatorLLFromPix(wxPoint(j, i), &rlat, &rlon);

                  // Georef value
                  GetCanvasPixPoint(j, i, glat, glon);

                  maxval = wxMax(maxval, (glat - rlat));
                  minval = wxMin(minval, (glat - rlat));

            }
            rlat = glat;
      }

      GetCanvasPixPoint(0, 0, rlat, rlon);
      for(int i=1 ; i < vp->pix_height-1 ; i++)
      {
            for(int j=0 ; j < vp->pix_width ; j++)
            {
                  // Reference mercator value
//                  vp->GetMercatorLLFromPix(wxPoint(j, i), &rlat, &rlon);

                  // Georef value
                  GetCanvasPixPoint(j, i, glat, glon);

                  double f = ((glat - rlat)-minval)/(maxval - minval);

                  double dy = (f * 40);

                  wxColour c = GetErrorGraphicColor(dy);
                  unsigned char r = c.Red();
                  unsigned char g = c.Green();
                  unsigned char b = c.Blue();

                  gr_image.SetRGB(j, i, r,g,b);
                  if((glat - rlat )!= 0)
                        gr_image.SetAlpha(j, i, 128);
                  else
                        gr_image.SetAlpha(j, i, 255);


            }
            rlat = glat;
      }




      //    Create a Bitmap
      wxBitmap *pbm = new wxBitmap(gr_image);
      wxMask *gr_mask = new wxMask(*pbm, wxColour(0,0,0));
      pbm->SetMask(gr_mask);

      pmdc->DrawBitmap(*pbm, 0,0);

      delete pbm;

}


#endif



 bool ChartCanvas::RegisterOverlayProvider(int sequence, RenderOverlayCallBackFunction callback)
{
      OverlaySpec *spec = new OverlaySpec;
      spec->m_sequence = sequence;
      spec->m_render_callback = callback;

      m_OverlaySpecArray.Add(spec);

      return true;
}

bool ChartCanvas::UnRegisterOverlayProvider(int sequence, RenderOverlayCallBackFunction callback)
{
      for(unsigned int i=0 ; i < m_OverlaySpecArray.GetCount() ; i++)
      {
            OverlaySpec *olspec = (OverlaySpec *)m_OverlaySpecArray.Item(i);

            if((olspec->m_render_callback = callback) && (olspec->m_sequence == sequence))
            {
                  m_OverlaySpecArray.Remove(olspec);
                  delete olspec;
                  break;
            }
      }

      return true;
}






void ChartCanvas::SetMyCursor ( wxCursor *c )
{
        pPriorCursor = c;
        SetCursor ( *c );
}



void ChartCanvas::EmbossCanvas ( wxMemoryDC *psource_dc, wxMemoryDC *pdest_dc, emboss_data *pemboss, int x, int y)
{
      if(!pemboss)
            return;

        //Grab a snipped image out of the chart
      wxMemoryDC snip_dc;
      wxBitmap snip_bmp ( pemboss->width, pemboss->height, -1 );
      snip_dc.SelectObject ( snip_bmp );

      snip_dc.Blit ( 0,0, pemboss->width, pemboss->height, psource_dc, x, y );

      wxImage snip_img = snip_bmp.ConvertToImage();

      double factor = 200;


        //  Apply emboss map to the snip image
      {
            unsigned char* pdata = snip_img.GetData();

            for ( int y=0 ; y < pemboss->height ; y++ )
            {
                  int map_index = ( y * pemboss->width );
                  for ( int x=0 ; x < pemboss->width ; x++ )
                  {
                        double val = ( pemboss->pmap[map_index] * factor ) / 256.;

                        int nred = ( int ) ( ( *pdata ) +  val );
                        nred = nred > 255 ? 255 : ( nred < 0 ? 0 : nred );
                        *pdata++ = ( unsigned char ) nred;

                        int ngreen = ( int ) ( ( *pdata ) +  val );
                        ngreen = ngreen > 255 ? 255 : ( ngreen < 0 ? 0 : ngreen );
                        *pdata++ = ( unsigned char ) ngreen;

                        int nblue = ( int ) ( ( *pdata ) +  val );
                        nblue = nblue > 255 ? 255 : ( nblue < 0 ? 0 : nblue );
                        *pdata++ = ( unsigned char ) nblue;

                        map_index++;
                  }
            }
      }


        //  Convert embossed snip to a bitmap
      wxBitmap emb_bmp ( snip_img );

        //  Map to another memoryDC
      wxMemoryDC result_dc;
      result_dc.SelectObject ( emb_bmp );

        //  Blit to target
      pdest_dc->Blit ( x, y, pemboss->width, pemboss->height, &result_dc, 0, 0 );

      result_dc.SelectObject ( wxNullBitmap );
      snip_dc.SelectObject ( wxNullBitmap );

}


void ChartCanvas::EmbossOverzoomIndicator ( wxMemoryDC *psource_dc, wxMemoryDC *pdest_dc)
{
      EmbossCanvas ( psource_dc, pdest_dc, m_pEM_OverZoom, 0,0);
}


void ChartCanvas::EmbossDepthScale ( wxMemoryDC *psource_dc, wxMemoryDC *pdest_dc, int emboss_ident )
{
        if ( emboss_ident == DEPTH_UNIT_UNKNOWN )
                return;

        emboss_data *ped = NULL;
        switch ( emboss_ident )
        {
              case DEPTH_UNIT_FEET:
                    ped = m_pEM_Feet;
                    break;
              case DEPTH_UNIT_METERS:
                    ped = m_pEM_Meters;
                    break;
              case DEPTH_UNIT_FATHOMS:
                    ped = m_pEM_Fathoms;
                    break;
              default:
                    ped = NULL;
                    break;
        }

        if(!ped)
              return;


        EmbossCanvas ( psource_dc, pdest_dc, ped, (VPoint.pix_width - ped->width), 0);
/*
        int chart_width = VPoint.pix_width;

        //Grab an snipped image out of the chart
        wxMemoryDC snip_dc;
        wxBitmap snip_bmp ( ped->width, ped->height, -1 );
        snip_dc.SelectObject ( snip_bmp );

        snip_dc.Blit ( 0,0, ped->width, ped->height, psource_dc, (chart_width - ped->width), 0 );

        wxImage snip_img = snip_bmp.ConvertToImage();

        double factor = 200;


        //  Apply emboss map to the snip image
        if ( ped != NULL )
        {
                unsigned char* pdata = snip_img.GetData();

                for ( int y=0 ; y < ped->height ; y++ )
                {
                        int map_index = ( y * ped->width );
                        for ( int x=0 ; x < ped->width ; x++ )
                        {
                                double val = ( ped->pmap[map_index] * factor ) / 256.;

                                int nred = ( int ) ( ( *pdata ) +  val );
                                nred = nred > 255 ? 255 : ( nred < 0 ? 0 : nred );
                                *pdata++ = ( unsigned char ) nred;

                                int ngreen = ( int ) ( ( *pdata ) +  val );
                                ngreen = ngreen > 255 ? 255 : ( ngreen < 0 ? 0 : ngreen );
                                *pdata++ = ( unsigned char ) ngreen;

                                int nblue = ( int ) ( ( *pdata ) +  val );
                                nblue = nblue > 255 ? 255 : ( nblue < 0 ? 0 : nblue );
                                *pdata++ = ( unsigned char ) nblue;

                                map_index++;
                        }
                }
        }


        //  Convert embossed snip to a bitmap
        wxBitmap emb_bmp ( snip_img );

        //  Map to another memoryDC
        wxMemoryDC result_dc;
        result_dc.SelectObject ( emb_bmp );

        //  Blit to target
        pdest_dc->Blit ( chart_width - ped->width, 0, ped->width, ped->height, &result_dc, 0, 0 );

        result_dc.SelectObject ( wxNullBitmap );
        snip_dc.SelectObject ( wxNullBitmap );
*/
}



void ChartCanvas::CreateDepthUnitEmbossMaps ( ColorScheme cs )
{
        wxFont font ( 60, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );

        int emboss_width = 400;
        int emboss_height = 100;

// Free any existing emboss maps
        delete m_pEM_Feet;
        delete m_pEM_Meters;
        delete m_pEM_Fathoms;

// Create the 3 DepthUnit emboss map structures
        m_pEM_Feet    = CreateEmbossMapData ( font, emboss_width, emboss_height, _("Feet"), cs);
        m_pEM_Meters  = CreateEmbossMapData ( font, emboss_width, emboss_height, _("Meters"), cs );
        m_pEM_Fathoms = CreateEmbossMapData ( font, emboss_width, emboss_height, _("Fathoms"), cs );
}

void ChartCanvas::CreateOZEmbossMapData(ColorScheme cs)
{
      delete m_pEM_OverZoom;

      int w, h;
      wxFont font ( 40, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
      wxClientDC dc(this);
      dc.SetFont(font);
      dc.GetTextExtent(_("OverZoom"), &w, &h);

      m_pEM_OverZoom    = CreateEmbossMapData ( font, w + 10, h + 10, _("OverZoom"), cs);
}



emboss_data *ChartCanvas::CreateEmbossMapData ( wxFont &font, int width, int height, const wxChar *str, ColorScheme cs )
{
        int *pmap;

        //  Create a temporary bitmap
        wxBitmap bmp ( width, height, -1 );

        // Create a memory DC
        wxMemoryDC temp_dc;
        temp_dc.SelectObject ( bmp );

        //  Paint on it
        temp_dc.SetBackground ( *wxWHITE_BRUSH );
        temp_dc.SetTextBackground ( *wxWHITE );
        temp_dc.SetTextForeground ( *wxBLACK );

        temp_dc.Clear();

        temp_dc.SetFont ( font );

        int str_w, str_h;
        temp_dc.GetTextExtent ( wxString ( str, wxConvUTF8 ), &str_w, &str_h );
        temp_dc.DrawText ( wxString ( str, wxConvUTF8 ), width - str_w - 10, 10 );

        //  Deselect the bitmap
        temp_dc.SelectObject ( wxNullBitmap );

        //  Convert bitmap the wxImage for manipulation
        wxImage img = bmp.ConvertToImage();

        double val_factor;
        switch (cs)
        {
            case GLOBAL_COLOR_SCHEME_DAY:
            default:
                  val_factor = 1;
                  break;
            case GLOBAL_COLOR_SCHEME_DUSK:
                  val_factor = .5;
                  break;
            case GLOBAL_COLOR_SCHEME_NIGHT:
                  val_factor = .25;
                  break;
        }

        int val;
        int index;
        pmap = ( int * ) calloc ( width * height * sizeof ( int ), 1 );
        //  Create emboss map by differentiating the emboss image
        //  and storing integer results in pmap
        //  n.b. since the image is B/W, it is sufficient to check
        //  one channel (i.e. red) only
        for ( int y=1 ; y < height-1 ; y++ )
        {
                for ( int x=1 ; x < width-1 ; x++ )
                {
                        val = img.GetRed ( x+1,y+1 ) - img.GetRed ( x-1, y-1 );  // range +/- 256
                        val = (int)(val * val_factor);
                        index = ( y * width ) + x;
                        pmap[index] = val;

                }
        }

        emboss_data *pret = new emboss_data;
        pret->pmap = pmap;
        pret->width = width;
        pret->height = height;

        return pret;
}

//----------------------------------------------------------------------------
//  Get a wxBitmap with wxMask associated containing the semi-static overlays
//----------------------------------------------------------------------------

wxBitmap *ChartCanvas::DrawTCCBitmap ( wxDC *pbackground_dc, bool bAddNewSelpoints )
{
        wxBitmap *p_bmp = new wxBitmap ( VPoint.pix_width, VPoint.pix_height, -1 );

        //      Here is the new drawing DC
        wxMemoryDC ssdc;
        ssdc.SelectObject ( *p_bmp );
        ssdc.SetBackground ( *wxWHITE_BRUSH );

        //  if a background dc is provided, use it as wallpaper
        if(pbackground_dc)
            ssdc.Blit(0, 0, VPoint.pix_width, VPoint.pix_height, pbackground_dc, 0, 0);
        else
              ssdc.Clear();


        //      Believe it or not, it is faster to REDRAW the overlay objects
        //      onto a mono bitmap, and then convert it into a mask bitmap
        //      than it is to create a mask from a colour bmp.
        //      Look at the wx code.  It goes through wxImage conversion, etc...
        //      So, create a mono DC, drawing white-on-black
        wxMemoryDC ssdc_mask;
        wxBitmap mask_bmp ( VPoint.pix_width, VPoint.pix_height, 1 );
        ssdc_mask.SelectObject ( mask_bmp );

        //      On X11, the drawing is Black on White, and the mask bitmap is inverted before
        //      making into a mask.
        //      On MSW and GTK, the drawing is White on Black, and no inversion is required
        //      Todo....  Some wxWidgets problem with this....
#ifndef __WXX11__
        ssdc_mask.SetBackground ( *wxBLACK_BRUSH );
#endif

        ssdc_mask.Clear();

//    Maybe draw the Tide Points

        if ( m_bShowTide )
        {
                // Rebuild Selpoints list on new map
                DrawAllTidesInBBox ( ssdc,      VPoint.vpBBox, bAddNewSelpoints );
                DrawAllTidesInBBox ( ssdc_mask, VPoint.vpBBox, false, true );    // onto the mask
                bShowingTide = true;
        }
        else
                bShowingTide = false;

//    Maybe draw the current arrows
        if ( m_bShowCurrent )
        {
                if ( bShowingCurrent )
                {
                        // Rebuild Selpoints list on new map
                        // and force redraw
                        DrawAllCurrentsInBBox ( ssdc,      VPoint.vpBBox, VPoint.skew, bAddNewSelpoints, true );
                        DrawAllCurrentsInBBox ( ssdc_mask, VPoint.vpBBox, VPoint.skew, false,            true, true );  // onto the mask
                }
                else
                {
                        DrawAllCurrentsInBBox ( ssdc,      VPoint.vpBBox, VPoint.skew, true, true ); // Force Selpoints add first time after
                        DrawAllCurrentsInBBox ( ssdc_mask, VPoint.vpBBox, VPoint.skew, false, true, true );    // onto the mask
                }
                bShowingCurrent = true;
        }
        else
                bShowingCurrent = false;

        ssdc.SelectObject ( wxNullBitmap );

#ifdef __WXX11__
        //      Invert the mono bmp, to make a useable mask bmp
        wxMemoryDC ssdc_mask_invert;
        wxBitmap mask_bmp_invert ( VPoint.pix_width, VPoint.pix_height, 1 );
        ssdc_mask_invert.SelectObject ( mask_bmp_invert );
        ssdc_mask_invert.Blit ( 0, 0, VPoint.pix_width, VPoint.pix_height,
                                &ssdc_mask, 0, 0, wxSRC_INVERT );

        ssdc_mask_invert.SelectObject ( wxNullBitmap );
        pss_overlay_mask = new wxMask ( mask_bmp_invert );
        ssdc_mask.SelectObject ( wxNullBitmap );
#else
        ssdc_mask.SelectObject ( wxNullBitmap );
        pss_overlay_mask = new wxMask ( mask_bmp );
#endif

        //      Create and associate the mask
        p_bmp->SetMask ( pss_overlay_mask );

        return p_bmp;
}






void ChartCanvas::DrawAllRoutesInBBox ( wxDC& dc, wxBoundingBox& BltBBox, const wxRegion& clipregion )
{
        Route *active_route = NULL;

        dc.DestroyClippingRegion();
        wxDCClipper(dc, clipregion);
        wxRouteListNode *node = pRouteList->GetFirst();
        while ( node )
        {
                Route *pRouteDraw = node->GetData();
                if ( pRouteDraw )
                {
                      if ( BltBBox.Intersect ( pRouteDraw->RBBox, 0 ) != _OUT ) // Route is not wholly outside window
                            if(pRouteDraw->IsActive() || pRouteDraw->IsSelected())
                                  active_route = pRouteDraw;
                            else
                                  pRouteDraw->Draw ( dc, VPoint );
                }

                node = node->GetNext();
        }

        //  Draw any active or selected route last, so that is is always on top
        if(active_route)
              active_route->Draw ( dc, VPoint );

}

void ChartCanvas::DrawAllWaypointsInBBox ( wxDC& dc, wxBoundingBox& BltBBox, const wxRegion& clipregion, bool bDrawMarksOnly )
{
//        wxBoundingBox bbx;
        wxDCClipper(dc, clipregion);
        wxRoutePointListNode *node = pWayPointMan->m_pWayPointList->GetFirst();

        while ( node )
        {
                RoutePoint *pWP = node->GetData();
                if ( pWP )
                {
                        if ( ( bDrawMarksOnly ) && ( pWP->m_bIsInRoute ) )
                        {
                                node = node->GetNext();
                                continue;
                        }
                        else
                        {
                                if ( BltBBox.GetValid() )
                                {
                                       if ( BltBBox.PointInBox ( pWP->m_lon, pWP->m_lat, 0 ) )
                                                pWP->Draw( dc, NULL );
                                }
                        }
                }

                node = node->GetNext();
        }

        // draw anchor watch rings, if activated                          // pjotrc 2010.02.22

        if (pAnchorWatchPoint1 || pAnchorWatchPoint2)
        {
              wxPoint r1, r2;
              wxPoint lAnchorPoint1, lAnchorPoint2;
              double lpp1 = 0.0;
              double lpp2 = 0.0;
              if (pAnchorWatchPoint1) {
                    lpp1 = GetAnchorWatchRadiusPixels(pAnchorWatchPoint1);
                    GetCanvasPointPix ( pAnchorWatchPoint1->m_lat, pAnchorWatchPoint1->m_lon, &lAnchorPoint1 );

              }
              if (pAnchorWatchPoint2) {
                   lpp2 = GetAnchorWatchRadiusPixels(pAnchorWatchPoint2);
                   GetCanvasPointPix ( pAnchorWatchPoint2->m_lat, pAnchorWatchPoint2->m_lon, &lAnchorPoint2 );

              }

#if wxUSE_GRAPHICS_CONTEXT

              wxGraphicsContext *pgc = NULL;

              wxMemoryDC *pmdc = wxDynamicCast(&dc, wxMemoryDC);
              if(pmdc)
              {
                    pgc = wxGraphicsContext::Create(*pmdc);
              }
              else
              {
                    wxClientDC *pcdc = wxDynamicCast(&dc, wxClientDC);
                    if(pcdc)
                          pgc = wxGraphicsContext::Create(*pcdc);
              }


              if(pgc)
              {
                    wxPen ppPeng ( GetGlobalColor ( _T ( "UGREN" ) ) , 2 );
                    wxPen ppPenr ( GetGlobalColor ( _T ( "URED" ) ) , 2 );

                    if(lpp1 > 0)
                    {
                        pgc->SetPen(ppPeng);
                        wxGraphicsPath gpath = pgc->CreatePath();
                        gpath.AddCircle(lAnchorPoint1.x, lAnchorPoint1.y, fabs(lpp1));
                        pgc->StrokePath(gpath);
                    }

                    if(lpp2 > 0)
                    {
                          pgc->SetPen(ppPeng);
                          wxGraphicsPath gpath = pgc->CreatePath();
                          gpath.AddCircle(lAnchorPoint2.x, lAnchorPoint2.y, fabs(lpp2));
                          pgc->StrokePath(gpath);
                    }

                    if(lpp1 < 0)
                    {
                          pgc->SetPen(ppPenr);
                          wxGraphicsPath gpath = pgc->CreatePath();
                          gpath.AddCircle(lAnchorPoint1.x, lAnchorPoint1.y, fabs(lpp1));
                          pgc->StrokePath(gpath);
                    }

                    if(lpp2 < 0)
                    {
                          pgc->SetPen(ppPenr);
                          wxGraphicsPath gpath = pgc->CreatePath();
                          gpath.AddCircle(lAnchorPoint2.x, lAnchorPoint2.y, fabs(lpp2));
                          pgc->StrokePath(gpath);
                    }


                    dc.CalcBoundingBox((int)wxRound(lAnchorPoint1.x + fabs(lpp1)) , (int)wxRound(lAnchorPoint1.y + fabs(lpp1)));  // keep dc dirty box up-to-date
                    dc.CalcBoundingBox((int)wxRound(lAnchorPoint1.x - fabs(lpp1)) , (int)wxRound(lAnchorPoint1.y - fabs(lpp1)));
                    dc.CalcBoundingBox((int)wxRound(lAnchorPoint2.x + fabs(lpp2)) , (int)wxRound(lAnchorPoint2.y + fabs(lpp2)));  // keep dc dirty box up-to-date
                    dc.CalcBoundingBox((int)wxRound(lAnchorPoint2.x - fabs(lpp2)) , (int)wxRound(lAnchorPoint2.y - fabs(lpp2)));

              }


#else
              dc.SetPen ( wxPen ( GetGlobalColor ( _T ( "UGREN" ) ) , 2 ) );

              wxBrush CurrentBrush1 = dc.GetBrush();
              wxBrush RingBrush1(CurrentBrush1.GetColour(),wxTRANSPARENT);
              dc.SetBrush(RingBrush1);

              if (lpp1 > 0) dc.DrawCircle(lAnchorPoint1.x, lAnchorPoint1.y, lpp1);
              if (lpp2 > 0) dc.DrawCircle(lAnchorPoint2.x, lAnchorPoint2.y, lpp2);

              dc.SetBrush(CurrentBrush1);
              dc.SetPen ( wxPen ( GetGlobalColor ( _T ( "URED" ) ) , 2 ) );

              wxBrush CurrentBrush2 = dc.GetBrush();
              wxBrush RingBrush2(CurrentBrush2.GetColour(),wxTRANSPARENT);
              dc.SetBrush(RingBrush2);

              if (lpp1 < 0) dc.DrawCircle(lAnchorPoint1.x, lAnchorPoint1.y, fabs(lpp1));
              if (lpp2 < 0) dc.DrawCircle(lAnchorPoint2.x, lAnchorPoint2.y, fabs(lpp2));

              dc.SetBrush(CurrentBrush1);

#endif
        }

}



double ChartCanvas::GetAnchorWatchRadiusPixels(RoutePoint *pAnchorWatchPoint)
{
   double lpp = 0.;
   wxPoint r1;
   wxPoint lAnchorPoint;
   double d1 = 0.0;
   double dabs;
   double tlat1, tlon1;

   if (pAnchorWatchPoint)
   {
      (pAnchorWatchPoint->m_MarkName).ToDouble(&d1);
      d1 = AnchorDistFix(d1, AnchorPointMinDist, AnchorPointMaxDist);
      dabs = fabs(d1/1852.);
      ll_gc_ll ( pAnchorWatchPoint->m_lat, pAnchorWatchPoint->m_lon, 0, dabs, &tlat1, &tlon1 );
      GetCanvasPointPix ( tlat1, tlon1, &r1 );
      GetCanvasPointPix ( pAnchorWatchPoint->m_lat, pAnchorWatchPoint->m_lon, &lAnchorPoint );
      lpp = sqrt(pow((double)(lAnchorPoint.x - r1.x), 2) + pow((double)(lAnchorPoint.y - r1.y), 2));

      //    This is an entry watch
      if(d1 < 0)
            lpp = -lpp;
   }
   return lpp;
}




//------------------------------------------------------------------------------------------
//    Tides and Current Chart Canvas Interface
//------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------
//    Tides Support
//------------------------------------------------------------------------------------------



void ChartCanvas::DrawAllTidesInBBox ( wxDC& dc, wxBoundingBox& BBox,
                                       bool bRebuildSelList, bool bdraw_mono_for_mask )
{

        wxPen *pblack_pen = wxThePenList->FindOrCreatePen ( GetGlobalColor ( _T ( "UINFD" ) ), 1, wxSOLID );
        wxBrush *pgreen_brush = wxTheBrushList->FindOrCreateBrush ( GetGlobalColor ( _T ( "GREEN1" ) ), wxSOLID );
//        wxBrush *pblack_brush = wxTheBrushList->FindOrCreateBrush ( GetGlobalColor ( _T ( "UINFD" ) ), wxSOLID );


        if ( bdraw_mono_for_mask )
        {
#ifdef __WXX11__
                const wxPen *pmono_pen = wxBLACK_PEN;
                const wxBrush *pmono_brush = wxBLACK_BRUSH;
#else
                const wxPen *pmono_pen = wxWHITE_PEN;
                const wxBrush *pmono_brush = wxWHITE_BRUSH;
#endif

                pblack_pen = ( wxPen * ) pmono_pen;
                pgreen_brush = ( wxBrush * ) pmono_brush;

        }

        dc.SetPen ( *pblack_pen );
        dc.SetBrush ( *pgreen_brush );

        if ( bRebuildSelList )
                pSelectTC->DeleteAllSelectableTypePoints ( SELTYPE_TIDEPOINT );

        wxBitmap bm;
        switch(m_cs)
        {
              case GLOBAL_COLOR_SCHEME_DAY:
                    bm = m_bmTideDay;
                    break;
              case GLOBAL_COLOR_SCHEME_DUSK:
                    bm = m_bmTideDusk;
                    break;
              case GLOBAL_COLOR_SCHEME_NIGHT:
                    bm = m_bmTideNight;
                    break;
              default:
                    bm = m_bmTideDay;
                    break;
        }

        int bmw = bm.GetWidth();
        int bmh = bm.GetHeight();

//      if(1/*BBox.GetValid()*/)
        {

                for ( int i=1 ; i<ptcmgr->Get_max_IDX() +1 ; i++ )
                {
                        IDX_entry *pIDX = ptcmgr->GetIDX_entry ( i );

                        char type = pIDX->IDX_type;             // Entry "TCtcIUu" identifier
                        if ( ( type == 't' ) || ( type == 'T' ) )  // only Tides
                        {
                                float lon = pIDX->IDX_lon;
                                float lat = pIDX->IDX_lat;
                                bool b_inbox = false;
                                float nlon;

                                if ( BBox.PointInBox ( lon, lat, 0 ) )
                                    {nlon = lon ; b_inbox = true;}
                                else if ( BBox.PointInBox ( lon + 360., lat, 0 ) )
                                    {nlon = lon + 360. ; b_inbox = true;}
                                else if ( BBox.PointInBox ( lon - 360., lat, 0 ) )
                                    {nlon = lon - 360. ; b_inbox = true;}

                                if ( b_inbox )
                                {

//    Manage the point selection list
                                        if ( bRebuildSelList )
                                                pSelectTC->AddSelectablePoint ( lat, lon, pIDX, SELTYPE_TIDEPOINT );

                                        wxPoint r;
                                        GetCanvasPointPix ( lat, nlon, &r );

                                        if(bdraw_mono_for_mask)
                                              dc.DrawRectangle(r.x - bmw/2, r.y - bmh/2, bmw, bmh);
                                        else
                                              dc.DrawBitmap(bm, r.x - bmw/2, r.y - bmh/2, true);

                                }
                        }
                }
        }
}







//------------------------------------------------------------------------------------------
//    Currents Support
//------------------------------------------------------------------------------------------


void ChartCanvas::DrawAllCurrentsInBBox ( wxDC& dc, wxBoundingBox& BBox, double skew_angle,
        bool bRebuildSelList,   bool bforce_redraw_currents, bool bdraw_mono_for_mask )
{
        float tcvalue, dir;
        bool bnew_val;
        char sbuf[20];
        wxFont *pTCFont;
        float lon_last = 0.;
        float lat_last = 0.;

        wxPen *pblack_pen = wxThePenList->FindOrCreatePen ( GetGlobalColor ( _T ( "UINFD" ) ), 1, wxSOLID );
        wxPen *porange_pen = wxThePenList->FindOrCreatePen ( GetGlobalColor ( _T ( "UINFO" ) ), 1, wxSOLID );
        wxBrush *porange_brush = wxTheBrushList->FindOrCreateBrush ( GetGlobalColor ( _T ( "UINFO" ) ), wxSOLID );
        wxBrush *pgray_brush = wxTheBrushList->FindOrCreateBrush ( GetGlobalColor ( _T ( "UIBDR" ) ), wxSOLID );

        if ( bdraw_mono_for_mask )
        {
#ifdef __WXX11__
                const wxPen *pmono_pen = wxBLACK_PEN;
                const wxBrush *pmono_brush = wxBLACK_BRUSH;
#else
                const wxPen *pmono_pen = wxWHITE_PEN;
                const wxBrush *pmono_brush = wxWHITE_BRUSH;
#endif

                pblack_pen = ( wxPen * ) pmono_pen;
                porange_pen = ( wxPen * ) pmono_pen;
                porange_brush = ( wxBrush * ) pmono_brush;
                pgray_brush = ( wxBrush * ) pmono_brush;
        }


        pTCFont = wxTheFontList->FindOrCreateFont ( 12, wxDEFAULT,wxNORMAL, wxBOLD,
                  FALSE, wxString ( _T ( "Eurostile Extended" ) ) );
        int now = time ( NULL );

        if ( bRebuildSelList )
                pSelectTC->DeleteAllSelectableTypePoints ( SELTYPE_CURRENTPOINT );



//     if(1/*BBox.GetValid()*/)
        {

                for ( int i=1 ; i<ptcmgr->Get_max_IDX() +1 ; i++ )
                {
                        IDX_entry *pIDX = ptcmgr->GetIDX_entry ( i );
                        float lon = pIDX->IDX_lon;
                        float lat = pIDX->IDX_lat;

                        char type = pIDX->IDX_type;             // Entry "TCtcIUu" identifier
                        if ( ( type == 'c' ) && (pIDX->IDX_Useable) )        // only subordinate currents are useful
                        {
                                // with directions known

//  TODO This is a ---HACK---
//  try to avoid double current arrows.  Select the first in the list only
//  Proper fix is to correct the TCDATA index file for depth indication

                                if ( ( BBox.PointInBox ( lon, lat, 0 ) ) && ( lat != lat_last ) && ( lon != lon_last ) )
                                {


//    Manage the point selection list
                                        if ( bRebuildSelList )
                                                pSelectTC->AddSelectablePoint ( lat, lon, pIDX, SELTYPE_CURRENTPOINT );

                                        wxPoint r;
                                        GetCanvasPointPix ( lat, lon, &r );

                                        wxPoint d[4];
                                        int dd = 6;
                                        d[0].x = r.x; d[0].y = r.y+dd;
                                        d[1].x = r.x+dd; d[1].y = r.y;
                                        d[2].x = r.x; d[2].y = r.y-dd;
                                        d[3].x = r.x-dd; d[3].y = r.y;


                                        if ( ptcmgr->GetTideOrCurrent15 ( now, i, tcvalue, dir, bnew_val ) )
                                        {
                                                porange_pen->SetWidth ( 1 );
                                                dc.SetPen ( *pblack_pen );
                                                dc.SetBrush ( *porange_brush );
                                                dc.DrawPolygon ( 4, d );

                                                if ( VPoint.chart_scale < 1000000 )
                                                {
                                                        if ( bnew_val || bforce_redraw_currents )
                                                        {

//    Get the display pixel location of the current station
                                                                int pixxc, pixyc;
                                                                wxPoint cpoint;
//                                                Current_Ch->latlong_to_pix_vp(lat, lon, pixxc, pixyc, VPoint);
                                                                GetCanvasPointPix ( lat, lon, &cpoint );
                                                                pixxc = cpoint.x;
                                                                pixyc = cpoint.y;

//    Draw arrow using preset parameters, see mm_per_knot variable
                                                                float scale = fabs ( tcvalue ) * current_draw_scaler;

                                                                porange_pen->SetWidth ( 2 );
                                                                dc.SetPen ( *porange_pen );
                                                                DrawArrow ( dc, pixxc, pixyc, dir - 90 + ( skew_angle * 180. / PI ), scale/100 );



// Draw text, if enabled

                                                                if ( bDrawCurrentValues )
                                                                {
                                                                        dc.SetFont ( *pTCFont );
                                                                        snprintf ( sbuf, 19, "%3.1f", fabs ( tcvalue ) );
                                                                        dc.DrawText ( wxString ( sbuf, wxConvUTF8 ), pixxc, pixyc );
                                                                }
                                                        }
                                                }           // scale
                                        }
/*          This is useful for debugging the TC database
                                        else
                                        {
                                                dc.SetPen ( *porange_pen );
                                                dc.SetBrush ( *pgray_brush );
                                                dc.DrawPolygon ( 4, d );
                                        }
*/

                                }
                                lon_last = lon;
                                lat_last = lat;

                        }
                }
        }
}


void ChartCanvas::DrawTCWindow ( int x, int y, void *pvIDX )
{
        pCwin = new TCWin ( this, x, y, pvIDX );

}

#define NUM_CURRENT_ARROW_POINTS 9
static wxPoint CurrentArrowArray[NUM_CURRENT_ARROW_POINTS] =
    {wxPoint ( 0, 0 ),
     wxPoint ( 0, -10 ),
     wxPoint ( 55, -10 ),
     wxPoint ( 55, -25 ),
     wxPoint ( 100, 0 ),
     wxPoint ( 55, 25 ),
     wxPoint ( 55, 10 ),
     wxPoint ( 0, 10 ),
     wxPoint ( 0,0 )
    };

void ChartCanvas::DrawArrow ( wxDC& dc, int x, int y, float rot_angle, float scale )
{

        float sin_rot = sin ( rot_angle * PI / 180. );
        float cos_rot = cos ( rot_angle * PI / 180. );

        // Move to the first point

        float xt = CurrentArrowArray[0].x;
        float yt = CurrentArrowArray[0].y;

        float xp = ( xt * cos_rot ) - ( yt * sin_rot );
        float yp = ( xt * sin_rot ) + ( yt * cos_rot );
        int x1 = ( int ) ( xp * scale );
        int y1 = ( int ) ( yp * scale );

        // Walk thru the point list
        for ( int ip=1 ; ip < NUM_CURRENT_ARROW_POINTS ; ip++ )
        {
                xt = CurrentArrowArray[ip].x;
                yt = CurrentArrowArray[ip].y;

                float xp = ( xt * cos_rot ) - ( yt * sin_rot );
                float yp = ( xt * sin_rot ) + ( yt * cos_rot );
                int x2 = ( int ) ( xp * scale );
                int y2 = ( int ) ( yp * scale );

                dc.DrawLine ( x1 + x, y1 + y, x2 + x, y2 + y );

                x1 = x2;
                y1 = y2;
        }
}





//------------------------------------------------------------------------------
//    TCwin Implementation
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE ( TCWin, wxWindow )
        EVT_PAINT ( TCWin::OnPaint )
        EVT_SIZE ( TCWin::OnSize )
        EVT_MOUSE_EVENTS ( TCWin::MouseEvent )
        EVT_BUTTON ( wxID_OK, TCWin::OKEvent )
        EVT_BUTTON ( ID_TCWIN_NX, TCWin::NXEvent )
        EVT_BUTTON ( ID_TCWIN_PR, TCWin::PREvent )
        EVT_CLOSE ( TCWin::OnCloseWindow )
END_EVENT_TABLE()

#include <wx/listimpl.cpp>
WX_DEFINE_LIST ( SplineList );



// Define a constructor
TCWin::TCWin ( ChartCanvas *parent, int x, int y, void *pvIDX )
{

        //    As a display optimization....
        //    if current color scheme is other than DAY,
        //    Then create the dialog ..WITHOUT.. borders and title bar.
        //    This way, any window decorations set by external themes, etc
        //    will not detract from night-vision

        long wstyle = wxCLIP_CHILDREN | wxDEFAULT_DIALOG_STYLE;
        if ( global_color_scheme != GLOBAL_COLOR_SCHEME_DAY )
                wstyle |= ( wxNO_BORDER );

        wxDialog::Create ( parent, wxID_ANY,   wxString ( _T ( "test" ) ), wxPoint ( x,y ), wxSize ( 500,400 ), wstyle );

        pParent = parent;

        pIDX = ( IDX_entry * ) pvIDX;

//    Set up plot type
        if ( strchr ( "Tt", pIDX->IDX_type ) )
        {
                plot_type = TIDE_PLOT;
                SetTitle ( wxString ( _( "Tide" ) ) );
        }
        else
        {
                plot_type = CURRENT_PLOT;
                SetTitle ( wxString ( _( "Current" ) ) );
        }

        int sx,sy;
        GetClientSize ( &sx, &sy );
        int swx,swy;
        GetSize ( &swx, &swy );
        int parent_sx, parent_sy;
        pParent->GetClientSize ( &parent_sx, &parent_sy );

        int xc=x;
        int yc=y;

//  Arrange for tcWindow to be always totally visible
        if ( ( x + swx ) > parent_sx )
                xc = xc-swx;
        if ( ( y + swy ) > parent_sy )
                yc = yc-swy;

//  Don't let the window origin move out of client area
        if( yc < 0)
              yc = 0;
        if(xc < 0)
              xc = 0;

        pParent->ClientToScreen ( &xc, &yc );
        wxPoint r ( xc,yc );
        Move ( r );

        SetBackgroundColour ( GetGlobalColor ( _T ( "DILG1" ) ) );
        SetForegroundColour ( GetGlobalColor ( _T ( "DILG3" ) ) );

//    Figure out this computer timezone minute offset
        wxDateTime this_now = wxDateTime::Now();
        wxDateTime this_gmt = this_now.ToGMT();

#if wxCHECK_VERSION(2, 6, 2)
        wxTimeSpan diff = this_now.Subtract ( this_gmt );
#else
        wxTimeSpan diff = this_gmt.Subtract ( this_now );
#endif

        int diff_mins = diff.GetMinutes();

        int station_offset = pIDX->IDX_time_zone;
        if ( this_now.IsDST() )
                station_offset += 60;
        corr_mins = station_offset - diff_mins;

        int h = pIDX->IDX_time_zone / 60;
        int m = pIDX->IDX_time_zone - (h * 60);
        if ( this_now.IsDST() )
            h += 1;
        m_stz.Printf(_T("Z %+03d:%02d"), h, m);


//    Make the "nice" (for the US) station time-zone string, brutally by hand
        wxString mtz;
        switch ( pIDX->IDX_time_zone )
        {
                case -240:
                        mtz = _T( "AST" );
                        break;
                case -300:
                        mtz = _T( "EST" );
                        break;
                case -360:
                        mtz = _T( "CST" );
                        break;
        }

        if(mtz.Len())
        {
              if ( this_now.IsDST() )
                    mtz[1] = 'D';

              m_stz = mtz;
        }


//    Establish the inital drawing day as today
        graphday = wxDateTime::Now();
        wxDateTime graphday_00 = wxDateTime::Today();
        int t_graphday_00 = graphday_00.GetTicks();

        //    Correct a Bug in wxWidgets time support
        if ( !graphday_00.IsDST() && graphday.IsDST() )
                t_graphday_00 -= 3600;

        t_graphday_00_at_station = t_graphday_00 - ( corr_mins * 60 );

        m_bForceTCRedraw = true;
        btc_valid = false;

        psList = NULL;

        OK_button = new wxButton ( this, wxID_OK, _( "OK" ),
                                   wxPoint ( sx - 100, sy - 32 ), wxDefaultSize );
        OK_button->SetBackgroundColour ( GetGlobalColor ( _T ( "UIBCK" ) ) );


        PR_button = new wxButton ( this, ID_TCWIN_PR, _( "Prev" ),
                                   wxPoint ( 10 , sy - 32 ), wxSize ( 40, -1 ) );
        PR_button->SetBackgroundColour ( GetGlobalColor ( _T ( "UIBCK" ) ) );

        int bsx, bsy, bpx, bpy;
        PR_button->GetSize ( &bsx, &bsy );
        PR_button->GetPosition ( &bpx, &bpy );

        NX_button = new wxButton ( this, ID_TCWIN_NX, _( "Next" ),
                                   wxPoint ( bpx + bsx + 5, bpy ), wxSize ( 40, -1 ) );
        NX_button->SetBackgroundColour ( GetGlobalColor ( _T ( "UIBCK" ) ) );

}


TCWin::~TCWin()
{
        if ( psList )
        {
                psList->DeleteContents ( TRUE );
                psList->Clear();
                delete psList;
        }

//      pParent->m_bForceReDraw = true;
        pParent->Refresh ( false );

}


void TCWin::OKEvent ( wxCommandEvent& event )
{
        Hide();
        pParent->pCwin = NULL;
//      pParent->m_bForceReDraw = true;
        pParent->Refresh ( false );
        Destroy();                          // that hurts
}

void TCWin::OnCloseWindow ( wxCloseEvent& event )
{
        Hide();
        pParent->pCwin = NULL;
//      pParent->m_bForceReDraw = true;
        Destroy();                          // that hurts
}

void TCWin::NXEvent ( wxCommandEvent& event )
{
        wxTimeSpan dm;
        wxTimeSpan dt ( 24,0,0,0 );
        dm = dt;

        graphday += dm;

        wxDateTime graphday_00 = graphday.ResetTime();
        int t_graphday_00 = graphday_00.GetTicks();
        t_graphday_00_at_station = t_graphday_00 - ( corr_mins * 60 );


        m_bForceTCRedraw = true;
        btc_valid = false;
        Refresh();

}

void TCWin::PREvent ( wxCommandEvent& event )
{
        wxTimeSpan dm;
        wxTimeSpan dt ( -24,0,0,0 );
        dm = dt;

        graphday += dm;

        wxDateTime graphday_00 = graphday.ResetTime();
        int t_graphday_00 = graphday_00.GetTicks();
        t_graphday_00_at_station = t_graphday_00 - ( corr_mins * 60 );


        m_bForceTCRedraw = true;
        btc_valid = false;
        Refresh();
}


void TCWin::Resize ( void )
{
}

void TCWin::RePosition ( void )
{
//    Position the window
        float lon = pIDX->IDX_lon;
        float lat = pIDX->IDX_lat;

        wxPoint r;
        pParent->GetCanvasPointPix ( lat, lon, &r );
        pParent->ClientToScreen ( &r.x, &r.y );
        Move ( r );
}

void TCWin::OnPaint ( wxPaintEvent& event )
{
        int x,y;
        int i;
        char sbuf[100];
        int w, h;
        float tcmax, tcmin;

        GetClientSize ( &x, &y );

        wxPaintDC dc ( this );

        wxString tlocn ( pIDX->IDX_station_name, wxConvUTF8 );

//     if(1/*bForceRedraw*/)
        {

                //    Make pens, etc...
                wxPen *pblack_1 = wxThePenList->FindOrCreatePen ( GetGlobalColor ( _T ( "UINFD" ) ), 1, wxSOLID );
                wxPen *pblack_2 = wxThePenList->FindOrCreatePen ( GetGlobalColor ( _T ( "UINFD" ) ), 2, wxSOLID );
                wxPen *pred_2   = wxThePenList->FindOrCreatePen ( GetGlobalColor ( _T ( "UINFR" ) ), 2, wxSOLID );

                wxBrush *pltgray = wxTheBrushList->FindOrCreateBrush ( GetGlobalColor ( _T ( "UIBCK" ) ), wxSOLID );

                wxFont *pSFont = wxTheFontList->FindOrCreateFont ( 8, wxFONTFAMILY_SWISS,wxNORMAL,  wxFONTWEIGHT_NORMAL,
                                 FALSE, wxString ( _T ( "Arial" ) ) );
                wxFont *pMFont = wxTheFontList->FindOrCreateFont ( 14, wxFONTFAMILY_SWISS,wxNORMAL,  wxFONTWEIGHT_NORMAL,
                                 FALSE, wxString ( _T ( "Arial" ) ) );
                wxFont *pLFont = wxTheFontList->FindOrCreateFont ( 18, wxFONTFAMILY_SWISS,wxNORMAL, wxBOLD,
                                 FALSE, wxString ( _T ( "Arial" ) ) );


                int x_graph = x * 1/10;
                int y_graph = y * 2/10;
                int x_graph_w = x * 8/10;
                int y_graph_h = y * 6/10;

                //    Box the graph
                dc.SetPen ( *pblack_1 );
                dc.SetBrush ( *pltgray );
                dc.DrawRectangle ( x_graph, y_graph, x_graph_w, y_graph_h );


                //    Horizontal axis
                dc.SetFont ( *pSFont );
                for ( i=0 ; i<25 ; i++ )
                {
                        int xd = x_graph + ( ( i ) * x_graph_w / 25 );
                        dc.DrawLine ( xd, y_graph, xd, y_graph + y_graph_h + 5 );

                        char sbuf[5];
                        sprintf ( sbuf, "%02d", i );
#ifdef __WXMSW__
                        dc.DrawRotatedText ( wxString ( sbuf ), xd + ( x_graph_w/25 ) /2, y_graph + y_graph_h + 8, 270. );
#else
                        int x_shim = -12;
                        dc.DrawText ( wxString ( sbuf, wxConvUTF8 ), xd + x_shim + ( x_graph_w/25 ) /2, y_graph + y_graph_h + 8 );
#endif
                }

                //    Make a line for "right now"
                int t_now = wxDateTime::Now().GetTicks();       // now, in ticks

                float t_ratio = x_graph_w * ( t_now - t_graphday_00_at_station ) / ( 25 * 3600 );

                int xnow = x_graph + ( int ) t_ratio;
                dc.SetPen ( *pred_2 );
                dc.DrawLine ( xnow, y_graph, xnow, y_graph + y_graph_h );
                dc.SetPen ( *pblack_1 );


                //    Build the array of values, capturing max and min

                if ( !btc_valid )
                {

                        float dir;
                        tcmax = -10;
                        tcmin = 10;

                        for ( i=0 ; i<25 ; i++ )
                        {
                                int tt = t_graphday_00_at_station + ( i * 3600 );
                                ptcmgr->GetTideOrCurrent ( tt, pIDX->IDX_rec_num, tcv[i], dir );
                                if ( tcv[i] > tcmax )
                                        tcmax = tcv[i];
                                if ( tcv[i] < tcmin )
                                        tcmin = tcv[i];
                        }



//    Set up the vertical parameters based on Tide or Current plot
                        if ( CURRENT_PLOT == plot_type )
                        {
                                ib = ( int ) tcmin -  1;
                                it = ( int ) tcmax  + 1;

                                im = 2 * __max ( abs ( ib ), abs ( it ) );
                                plot_y_offset = y_graph_h/2;
                                val_off = 0;
                        }
                        else
                        {
                                ib = ( int ) tcmin;
                                if(tcmin < 0)
                                      ib -= 1;
                                it = ( int ) tcmax  + 1;

                                im =  it - ib; //abs ( ib ) + abs ( it );
                                plot_y_offset = ( y_graph_h * (it - ib) ) /im;
                                val_off = ib;
                        }


//    Build spline list of points
                        if ( psList )                 // destroy any current list
                        {
                                psList->DeleteContents ( TRUE );
                                psList->Clear();
                                delete psList;
                        }

                        psList = new SplineList;;

                        for ( i = 0 ; i<25 ; i++ )
                        {
                                wxPoint *pp = new wxPoint;
                                pp->x =  x_graph + ( ( i ) * x_graph_w / 25 );
                                pp->y = y_graph + ( plot_y_offset ) - ( int ) ( (tcv[i]- val_off) * y_graph_h / im );

                                psList->Append ( pp );
                        }

                        btc_valid = true;
                }


                dc.SetTextForeground ( GetGlobalColor ( _T ( "DILG3" ) ) );

                //    Vertical Axis

                //      Maybe skip some lines and legends if the range is too high
                int height_stext;;
                dc.GetTextExtent(_T("1"), NULL, &height_stext);

                int i_skip = 1;
                if(height_stext > y_graph_h / im)
                      i_skip = 2;

                i = ib;
                while ( i < it + 1 )
                {
                        int yd = y_graph + ( plot_y_offset ) - (( i- val_off) * y_graph_h / im );

                        if ( ( plot_y_offset + y_graph ) == yd )
                                dc.SetPen ( *pblack_2 );
                        else
                                dc.SetPen ( *pblack_1 );

                        dc.DrawLine ( x_graph, yd, x_graph+x_graph_w, yd );
                        snprintf ( sbuf, 99, "%d", i );
                        dc.DrawText ( wxString ( sbuf, wxConvUTF8 ), x_graph - 20, yd - 10 );
                        i += i_skip;

                }
                //    Units
/*
                if ( pIDX->pref_sta_data )
                {
                        wxString units ( pIDX->pref_sta_data->units_abbrv, wxConvUTF8 );
                        dc.DrawText ( units, x_graph - 40, y_graph + y_graph_h/2 );
                }
*/
                //  Location text
                wxString locn ( pIDX->IDX_station_name, wxConvUTF8 );
                wxString locna, locnb;
                if ( locn.Contains ( wxString ( _T ( "," ) ) ) )
                {
                        locna = locn.BeforeFirst ( ',' );
                        locnb = locn.AfterFirst ( ',' );
                }
                else
                {
                        locna = locn;
                        locnb.Empty();
                }



                int y_first_line = y * 2 / 100;
                dc.SetFont ( *pLFont );
                dc.GetTextExtent ( locna, &w, &h );
                if ( w > x )
                {
                        dc.SetFont ( *pMFont );
                        dc.GetTextExtent ( locna, &w, &h );
                }

                dc.DrawText ( locna, ( ( x/4 - w/2 ) < 0 ) ? 12: ( x/4 - w/2 ) , y_first_line );

                int y_second_line = y_first_line + h + 2;
                dc.SetFont ( *pMFont );
                dc.GetTextExtent ( locnb, &w, &h );
                dc.DrawText ( locnb, x/2 - w/2, y_second_line );

//    Reference to the master station
                if ( 't' == pIDX->IDX_type )
                {

                        dc.SetFont ( *pMFont );
                        wxString mref ( pIDX->IDX_reference_name, wxConvUTF8 );
                        dc.GetTextExtent ( mref, &w, &h );
                        int y_master_reference = y_graph - h - 2;
                        dc.DrawText ( mref, x - w, y_master_reference );

                        wxString ref_legend ( _( "Reference Station:" ) );
                        int wl;
                        dc.GetTextExtent ( ref_legend, &wl, &h );
                        dc.DrawText ( ref_legend, x - w/2 - wl/2, y_master_reference - h + 4 );
                }



                //    Draw the Value curve

                dc.SetPen ( *pblack_2 );
#if wxUSE_SPLINES
                dc.DrawSpline ( ( wxList * ) psList );
#else
                dc.DrawLines ( ( wxList * ) psList );
#endif
                //  More Info

                dc.SetFont ( *pSFont );
                dc.GetTextExtent ( m_stz, &w, &h );
                dc.DrawText ( m_stz, x/2 - w/2, y * 88/100 );


                // There seems to be some confusion about format specifiers
                //  Hack this.....
                //  Find and use the longest "sprintf" result......
                wxString sdate;
                wxString s1 = graphday.Format ( _T ( "%#x" ) );
                wxString s2 = graphday.Format ( _T ( "%x" ) );

                if ( s2.Len() > s1.Len() )
                        sdate = s2;
                else
                        sdate = s1;
                dc.SetFont ( *pMFont );
                dc.GetTextExtent ( sdate, &w, &h );
                dc.DrawText ( sdate, x/2 - w/2, y * 92/100 );

                ///
                Station_Data *pmsd = pIDX->pref_sta_data;
                if ( pmsd )
                {
                        dc.GetTextExtent ( wxString ( pmsd->units_conv, wxConvUTF8 ), &w, &h );
                        dc.DrawRotatedText ( wxString ( pmsd->units_conv, wxConvUTF8 ),
                                             5, y_graph + y_graph_h/2 + w/2, 90. );
                }

//    Today or tomorrow
                wxString sday;
                wxDateTime this_now = wxDateTime::Now();

                int day = graphday.GetDayOfYear();
                if ( graphday.GetYear() == this_now.GetYear() )
                {
                       if ( day ==  this_now.GetDayOfYear() )
                               sday.Append ( _( "Today" ) );
                       else if ( day == this_now.GetDayOfYear() + 1 )
                               sday.Append ( _( "Tomorrow" ) );
                 }
                 else if ( graphday.GetYear() == this_now.GetYear() + 1 && day == this_now.Add( wxTimeSpan::Day() ).GetDayOfYear() )
                       sday.Append ( _( "Tomorrow" ) );




                dc.SetFont ( *pSFont );
                dc.GetTextExtent ( wxString ( sday, wxConvUTF8 ), &w, &h );
                dc.DrawText ( wxString ( sday, wxConvUTF8 ), 55 - w/2, y * 88/100 );

                m_bForceTCRedraw = false;
        }
}


void TCWin::OnSize ( wxSizeEvent& event )
{
        int width,height;
        GetClientSize ( &width, &height );
        int x,y;
        GetPosition ( &x, &y );
}


void TCWin::MouseEvent ( wxMouseEvent& event )
{

        int x,y;
        event.GetPosition ( &x, &y );

}


#ifdef __WXX11__
//----------------------------------------------------------------------------------------------------------
//    ocpCursor Implementation
//----------------------------------------------------------------------------------------------------------
#include "wx/x11/private.h"


//----------------------------------------------------------------------------------------------
//      ocpCursorRefData Definition/Implementation
//----------------------------------------------------------------------------------------------

class ocpCursorRefData: public wxObjectRefData
{
        public:

                ocpCursorRefData();
                ~ocpCursorRefData();

                WXCursor     m_cursor;
                WXDisplay   *m_display;
};


ocpCursorRefData::ocpCursorRefData()
{
        m_cursor = NULL;
        m_display = NULL;
}


ocpCursorRefData::~ocpCursorRefData()
{
        if ( m_cursor )
                XFreeCursor ( ( Display* ) m_display, ( Cursor ) m_cursor );
}


//-----------------------------------------------------------------------------

#define M_CURSORDATA ((ocpCursorRefData *)m_refData)


/*
IMPLEMENT_DYNAMIC_CLASS(ocpCursor,wxObject)

ocpCursor::ocpCursor()
{

}
*/

//----------------------------------------------------------------------------------------------
//      ocpCursor Implementation
//
//----------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------
//      A new constructor taking a file name to load and assign as a cursor
//----------------------------------------------------------------------------------------------
ocpCursor::ocpCursor ( const wxString& cursorName, long type,
                       int hotSpotX, int hotSpotY ) : wxCursor ( wxCURSOR_CROSS )
{
        wxImage cImage;

        if ( !cImage.CanRead ( cursorName ) )
                ::wxInitAllImageHandlers();

        cImage.LoadFile ( cursorName );

        int width = cImage.GetWidth();
        int height = cImage.GetHeight();

//    m_refData = new wxCursorRefData();

        //    Get some X parameters
        int xscreen = DefaultScreen ( ( Display* ) wxGlobalDisplay() );
        Window xroot = RootWindow ( ( Display* ) wxGlobalDisplay(), xscreen );
        Visual* xvisual = DefaultVisual ( ( Display* ) wxGlobalDisplay(), xscreen );

        M_CURSORDATA->m_display = wxGlobalDisplay();
        wxASSERT_MSG ( M_CURSORDATA->m_display, wxT ( "No display" ) );

        //    Make a pixmap
        Pixmap cpixmap = XCreatePixmap ( ( Display* ) wxGlobalDisplay(),
                                         xroot, width, height, 1 );

        //    Make an Ximage
        XImage *data_image = XCreateImage ( ( Display* ) wxGlobalDisplay(), xvisual,
                                            1, ZPixmap, 0, 0, width, height, 32, 0 );
        data_image->data = ( char* ) malloc ( data_image->bytes_per_line * data_image->height );


        int index = 0;
        int pixel = 0;
        unsigned char* data = cImage.GetData();

        // Create mask

        Pixmap cmask;
        unsigned char mr, mg, mb;

        if ( cImage.HasMask() )
        {
                XImage *mask_image = XCreateImage ( ( Display* ) wxGlobalDisplay(), xvisual,
                                                    1, ZPixmap, 0, 0, width, height, 32, 0 );
                mask_image->data = ( char* ) malloc ( mask_image->bytes_per_line * mask_image->height );

                cImage.GetOrFindMaskColour ( &mr, &mg, &mb );

                int rit = ( mr << 16 ) + ( mg << 8 ) + mb;
                for ( int y = 0; y < height; y++ )
                {
                        for ( int x = 0; x < width; x++ )
                        {
                                int ri = ( int ) data[index++];
                                ri += data[index++] << 8;
                                ri += data[index++] << 16;

                                /*
                                                        int ri = *(int *)(&data[index]);
                                                        ri &= 0x00ffffff;
                                                        index++;
                                                        index++;
                                                        index++;
                                */
                                pixel = 1;
                                if ( ri == rit )        // if data is mask value, mask pixel gets 0
                                        pixel = 0;


                                XPutPixel ( mask_image, x, y, pixel );


                        }
                }

                cmask = XCreatePixmap ( ( Display* ) wxGlobalDisplay(), xroot, width, height, 1 );

                GC gc = XCreateGC ( ( Display* ) wxGlobalDisplay(), cmask, 0, NULL );
                XPutImage ( ( Display* ) wxGlobalDisplay(), cmask, gc, mask_image,
                            0, 0, 0, 0, width, height );

                XDestroyImage ( mask_image );
                XFreeGC ( ( Display* ) wxGlobalDisplay(), gc );

        }

        //    Render the wxImage cImage onto the Ximage
        //    Simple black/white cursors only, please

        index = 0;

        for ( int y = 0; y < height; y++ )
        {
                for ( int x = 0; x < width; x++ )
                {
                        int ri = ( int ) data[index++];
                        ri += data[index++] << 8;
                        ri += data[index++] << 16;

                        /*
                        int ri = *(int *)(&data[index]);
                        ri &= 0x00ffffff;
                        index++;
                        index++;
                        index++;
                        */

                        pixel = 0;
                        if ( ri )
                                pixel = 1;


                        XPutPixel ( data_image, x, y, pixel );


                }
        }



        //    Put the Ximage into the pixmap

        GC gc = XCreateGC ( ( Display* ) wxGlobalDisplay(), cpixmap, 0, NULL );
        XPutImage ( ( Display* ) wxGlobalDisplay(), cpixmap, gc, data_image,
                    0, 0, 0, 0, width, height );

        //    Free the Ximage stuff
        XDestroyImage ( data_image );
        XFreeGC ( ( Display* ) wxGlobalDisplay(), gc );

        //    Make a X cursor from the pixmap



        XColor fg, bg;
        fg.red = fg.blue = fg.green = 0xffff;
        bg.red = bg.blue = bg.green = 0;


        M_CURSORDATA->m_cursor = ( WXCursor ) XCreatePixmapCursor ( ( Display* ) wxGlobalDisplay(), cpixmap, cmask,
                                 &fg, &bg, hotSpotX, hotSpotY );




}

//----------------------------------------------------------------------------------------------
//      A new constructor taking a static char ** of XPM data and assign as a cursor
//----------------------------------------------------------------------------------------------

ocpCursor::ocpCursor ( const char **xpm_data, long type,
                       int hotSpotX, int hotSpotY ) : wxCursor ( wxCURSOR_CROSS )
{
        wxImage cImage ( xpm_data );

        int width = cImage.GetWidth();
        int height = cImage.GetHeight();

//    m_refData = new wxCursorRefData();

        //    Get some X parameters
        int xscreen = DefaultScreen ( ( Display* ) wxGlobalDisplay() );
        Window xroot = RootWindow ( ( Display* ) wxGlobalDisplay(), xscreen );
        Visual* xvisual = DefaultVisual ( ( Display* ) wxGlobalDisplay(), xscreen );

        M_CURSORDATA->m_display = wxGlobalDisplay();
        wxASSERT_MSG ( M_CURSORDATA->m_display, wxT ( "No display" ) );

        //    Make a pixmap
        Pixmap cpixmap = XCreatePixmap ( ( Display* ) wxGlobalDisplay(),
                                         xroot, width, height, 1 );

        //    Make an Ximage
        XImage *data_image = XCreateImage ( ( Display* ) wxGlobalDisplay(), xvisual,
                                            1, ZPixmap, 0, 0, width, height, 32, 0 );
        data_image->data = ( char* ) malloc ( data_image->bytes_per_line * data_image->height );


        int index = 0;
        int pixel = 0;
        unsigned char* data = cImage.GetData();

        // Create mask

        Pixmap cmask;
        unsigned char mr, mg, mb;

        if ( cImage.HasMask() )
        {
                XImage *mask_image = XCreateImage ( ( Display* ) wxGlobalDisplay(), xvisual,
                                                    1, ZPixmap, 0, 0, width, height, 32, 0 );
                mask_image->data = ( char* ) malloc ( mask_image->bytes_per_line * mask_image->height );

                cImage.GetOrFindMaskColour ( &mr, &mg, &mb );

                int rit = ( mr << 16 ) + ( mg << 8 ) + mb;
                for ( int y = 0; y < height; y++ )
                {
                        for ( int x = 0; x < width; x++ )
                        {
                                int ri = ( int ) data[index++];
                                ri += data[index++] << 8;
                                ri += data[index++] << 16;

                                /*
                                                int ri = *(int *)(&data[index]);
                                                ri &= 0x00ffffff;
                                                index++;
                                                index++;
                                                index++;
                                */
                                pixel = 1;
                                if ( ri == rit )        // if data is mask value, mask pixel gets 0
                                        pixel = 0;


                                XPutPixel ( mask_image, x, y, pixel );


                        }
                }

                cmask = XCreatePixmap ( ( Display* ) wxGlobalDisplay(), xroot, width, height, 1 );

                GC gc = XCreateGC ( ( Display* ) wxGlobalDisplay(), cmask, 0, NULL );
                XPutImage ( ( Display* ) wxGlobalDisplay(), cmask, gc, mask_image,
                            0, 0, 0, 0, width, height );

                XDestroyImage ( mask_image );
                XFreeGC ( ( Display* ) wxGlobalDisplay(), gc );

        }

        //    Render the wxImage cImage onto the Ximage
        //    Simple black/white cursors only, please

        index = 0;

        for ( int y = 0; y < height; y++ )
        {
                for ( int x = 0; x < width; x++ )
                {
                        int ri = ( int ) data[index++];
                        ri += data[index++] << 8;
                        ri += data[index++] << 16;

                        /*
                        int ri = *(int *)(&data[index]);
                        ri &= 0x00ffffff;
                        index++;
                        index++;
                        index++;
                        */

                        pixel = 0;
                        if ( ri )
                                pixel = 1;


                        XPutPixel ( data_image, x, y, pixel );


                }
        }



        //    Put the Ximage into the pixmap

        GC gc = XCreateGC ( ( Display* ) wxGlobalDisplay(), cpixmap, 0, NULL );
        XPutImage ( ( Display* ) wxGlobalDisplay(), cpixmap, gc, data_image,
                    0, 0, 0, 0, width, height );

        //    Free the Ximage stuff
        XDestroyImage ( data_image );
        XFreeGC ( ( Display* ) wxGlobalDisplay(), gc );

        //    Make a X cursor from the pixmap



        XColor fg, bg;
        fg.red = fg.blue = fg.green = 0xffff;
        bg.red = bg.blue = bg.green = 0;


        M_CURSORDATA->m_cursor = ( WXCursor ) XCreatePixmapCursor ( ( Display* ) wxGlobalDisplay(), cpixmap, cmask,
                                 &fg, &bg, hotSpotX, hotSpotY );




}

#endif      // __WXX11__



//      We derive a class from wxCursor to create ocpCursor
//      Specifically to fix a bug in wxImage-wxBitmap conversions

#ifdef __WXMSW__

/*
//----------------------------------------------------------------------------------------------
//      ocpCursorRefData Definition/Implementation
//----------------------------------------------------------------------------------------------
class ocpCursorRefData: public wxObjectRefData
{
     public:
           ocpCursorRefData();
           ocpCursorRefData(HCURSOR);
           ~ocpCursorRefData();
};

ocpCursorRefData::ocpCursorRefData()
{

}

ocpCursorRefData::ocpCursorRefData(HCURSOR hcursor)
{
}


ocpCursorRefData::~ocpCursorRefData()
{
}
*/

//----------------------------------------------------------------------------------------------
//      ocpCursor Implementation
//
//----------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------
//      A new constructor taking a file name to load and assign as a cursor
//----------------------------------------------------------------------------------------------


ocpCursor::ocpCursor ( const wxString& cursorName, long type,
                       int hotSpotX, int hotSpotY ) : wxCursor ( wxCURSOR_ARROW )

{
        wxImage cImage;

        if ( !cImage.CanRead ( cursorName ) )
                ::wxInitAllImageHandlers();

        cImage.LoadFile ( cursorName );



//      wxMSW Bug???
//      On Windows XP, conversion from wxImage to wxBitmap fails at the ::CreateDIBitmap() call
//      unless a "compatible" dc is provided.  Why??
//      As a workaround, just make a simple wxDC for temporary use

        wxBitmap tbmp ( cImage.GetWidth(),cImage.GetHeight(),-1 );
        wxMemoryDC dwxdc;
        dwxdc.SelectObject ( tbmp );

        HCURSOR hcursor = wxBitmapToHCURSOR ( wxBitmap ( cImage, ( wxDC & ) dwxdc ),
                                              hotSpotX, hotSpotY );
        if ( !hcursor )
        {
                wxLogWarning ( _T( "Failed to create ocpCursor." ) );
                return;
        }


//  Replace the HANDLE created in the base class constructor
//  Probably leaks....
        GetGDIImageData()->m_handle = hcursor;
}


//----------------------------------------------------------------------------------------------
//      A new constructor taking a static char ** of XPM data and assign as a cursor
//----------------------------------------------------------------------------------------------


ocpCursor::ocpCursor ( const char **xpm_data, long type,
                       int hotSpotX, int hotSpotY ) : wxCursor ( wxCURSOR_ARROW )

{
        wxImage cImage ( xpm_data );

//      wxMSW Bug???
//      On Windows XP, conversion from wxImage to wxBitmap fails at the ::CreateDIBitmap() call
//      unless a "compatible" dc is provided.  Why??
//      As a workaround, just make a simple wxDC for temporary use

        wxBitmap tbmp ( cImage.GetWidth(),cImage.GetHeight(),-1 );
        wxMemoryDC dwxdc;
        dwxdc.SelectObject ( tbmp );

        HCURSOR hcursor = wxBitmapToHCURSOR ( wxBitmap ( cImage, ( wxDC & ) dwxdc ),
                                              hotSpotX, hotSpotY );
        if ( !hcursor )
        {
                wxLogWarning ( _T( "Failed to create ocpCursor." ) );
                return;
        }


//  Replace the HANDLE created in the base class constructor
//  Probably leaks....
        GetGDIImageData()->m_handle = hcursor;
}




#endif   // __MSW






#ifdef __WXOSX__  // begin rms

/*
//----------------------------------------------------------------------------------------------
//      ocpCursorRefData Definition/Implementation
//----------------------------------------------------------------------------------------------
class ocpCursorRefData: public wxObjectRefData
{
     public:
           ocpCursorRefData();
           ocpCursorRefData(HCURSOR);
           ~ocpCursorRefData();
};

ocpCursorRefData::ocpCursorRefData()
{

}

ocpCursorRefData::ocpCursorRefData(HCURSOR hcursor)
{
}


ocpCursorRefData::~ocpCursorRefData()
{
}
*/

//----------------------------------------------------------------------------------------------
//      ocpCursor Implementation
//
//----------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------
//      A new constructor taking a file name to load and assign as a cursor
//----------------------------------------------------------------------------------------------


ocpCursor::ocpCursor ( const wxString& cursorName, long type,
                       int hotSpotX, int hotSpotY ) : wxCursor ( wxCURSOR_ARROW )

{
        wxImage cImage;

        if ( !cImage.CanRead ( cursorName ) )
                ::wxInitAllImageHandlers();

        cImage.LoadFile ( cursorName );



//      wxMSW Bug???
//      On Windows XP, conversion from wxImage to wxBitmap fails at the ::CreateDIBitmap() call
//      unless a "compatible" dc is provided.  Why??
//      As a workaround, just make a simple wxDC for temporary use

        wxBitmap tbmp ( cImage.GetWidth(),cImage.GetHeight(),-1 );
}


//----------------------------------------------------------------------------------------------
//      A new constructor taking a static char ** of XPM data and assign as a cursor
//----------------------------------------------------------------------------------------------


ocpCursor::ocpCursor ( const char **xpm_data, long type,
                       int hotSpotX, int hotSpotY ) : wxCursor ( wxCURSOR_ARROW )

{
        wxImage cImage ( xpm_data );

        wxBitmap tbmp ( cImage.GetWidth(),cImage.GetHeight(),-1 );
}




#endif   // __WXOSX__ end rms





//------------------------------------------------------------------------------
//    AISInfoWin Implementation
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE ( AISInfoWin, wxWindow )
            EVT_PAINT ( AISInfoWin::OnPaint )
            END_EVENT_TABLE()


// constructor
AISInfoWin::AISInfoWin ( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name):
            wxWindow(parent, id, pos, size, wxSIMPLE_BORDER)
{
      m_offsetx = 0;
      m_offsety = 0;
}


AISInfoWin::~AISInfoWin(void)
{
}

void AISInfoWin::OnPaint(wxPaintEvent& event)
{
      int width, height;
      GetClientSize(&width, &height );
      wxPaintDC dc(this);

      dc.SetFont(GetFont());
      dc.SetBackground(wxBrush(GetBackgroundColour()));
      dc.SetTextForeground(GetForegroundColour());
      dc.SetBackgroundMode(wxTRANSPARENT);

      //dc.SetTextForeground(pFontMgr->GetFontColor(_("AISTargetQuery")));

      dc.Clear();

      if(m_text.Len())
      {
            int font_size_x, font_size_y, font_descent, font_lead;
            dc.GetTextExtent(_T("1"), &font_size_x, &font_size_y, &font_descent, &font_lead);
            int dy = font_size_y;

            int y = m_offsety;
            unsigned int i = 0;
            wxString line;

            while(i < m_text.Len() - 1)
            {
                  while(m_text[i] != '\n' && i < m_text.Len() - 1)
                        line.Append(m_text[i++]);

                  dc.DrawText(line, m_offsetx, y);
//                  dc.DrawLine(m_offsetx, y, m_maxtl + m_offsetx, y);
                  i++;
                  y += dy;
                  line.Clear();
            }
      }
}

void AISInfoWin::AppendText(wxString &text)
{
      m_text = text;
      Refresh(false);
}


wxSize AISInfoWin::GetOptimumSize(int *pn_nl, int *pn_cmax)
{
      wxSize pix_size;

            //    Count lines and characters
      wxString max_line;
      int nl = 0;
      unsigned int max_len = 0;
      int max_pix = 0;

      wxClientDC dc(this);
      dc.SetFont(GetFont());

      unsigned int i = 0;
      wxString rline;

      while(i < m_text.Len())
      {
            while(m_text.GetChar(i) != '\n')
                  rline.Append(m_text.GetChar(i++));

            if(rline.Len() > max_len)
            {
                  max_line = rline;
                  max_len = rline.Len();
            }

            int w, h;
            dc.GetTextExtent(rline.Trim(), &w, &h);
            if(w > max_pix)
                 max_pix = w;

            i++;              // skip nl
            nl++;
            rline.Clear();
       }

       if(pn_cmax)
              *pn_cmax = max_len;

       if(pn_nl)
             *pn_nl = nl;

      int w, h;
      dc.GetTextExtent(max_line, &w, &h);

      pix_size.x = max_pix + (m_offsetx * 2);       // x comes from above
      pix_size.y = h * nl  + (m_offsety * 2);       // y is the same for all

      m_maxtl = max_pix;

      return pix_size;

}




//---------------------------------------------------------------------------------------
//          AISTargetQueryDialog Implementation
//---------------------------------------------------------------------------------------

#define xID_OK 10009

IMPLEMENT_CLASS ( AISTargetQueryDialog, wxDialog )


// AISTargetQueryDialog event table definition

            BEGIN_EVENT_TABLE ( AISTargetQueryDialog, wxDialog )
            EVT_BUTTON( xID_OK, AISTargetQueryDialog::OnIdOKClick )
            EVT_CLOSE(AISTargetQueryDialog::OnClose)
            EVT_MOVE( AISTargetQueryDialog::OnMove )
            END_EVENT_TABLE()

 AISTargetQueryDialog::AISTargetQueryDialog( )
{
      Init();
}

AISTargetQueryDialog::AISTargetQueryDialog ( wxWindow* parent,
                                             wxWindowID id, const wxString& caption,
                                             const wxPoint& pos, const wxSize& size, long style )
{
      Init();
      Create ( parent, id, caption, pos, size, style );
}

AISTargetQueryDialog::~AISTargetQueryDialog( )
{
      delete m_pQueryTextCtl;
}


void AISTargetQueryDialog::Init( )
{
      m_MMSI = -1;
      m_pQueryTextCtl = NULL;
      m_nl = 0;
      m_colorscheme = (ColorScheme)(-1);
      m_okButton = NULL;

}
void AISTargetQueryDialog::OnClose(wxCloseEvent& event)
{
      Destroy();
      g_pais_query_dialog_active = NULL;
}

void AISTargetQueryDialog::OnIdOKClick( wxCommandEvent& event )
{
      Close();
}

bool AISTargetQueryDialog::Create ( wxWindow* parent,
                                    wxWindowID id, const wxString& caption,
                                    const wxPoint& pos, const wxSize& size, long style )
{

        //    As a display optimization....
        //    if current color scheme is other than DAY,
        //    Then create the dialog ..WITHOUT.. borders and title bar.
        //    This way, any window decorations set by external themes, etc
        //    will not detract from night-vision


      long wstyle = wxDEFAULT_FRAME_STYLE;
      if (( global_color_scheme != GLOBAL_COLOR_SCHEME_DAY ) && ( global_color_scheme != GLOBAL_COLOR_SCHEME_RGB ))
            wstyle |= ( wxNO_BORDER );

 //     m_colorscheme = global_color_scheme;

      if ( !wxDialog::Create ( parent, id, caption, pos, size, wstyle ) )
            return false;

//      wxColour back_color = GetGlobalColor ( _T ( "UIBDR" ) );
//      SetBackgroundColour ( back_color );

      wxFont *dFont = pFontMgr->GetFont(_("AISTargetQuery"), 12);
      SetFont ( *dFont );

//      SetForegroundColour(pFontMgr->GetFontColor(_("AISTargetQuery")));


//      wxColour back_color =GetGlobalColor ( _T ( "UIBCK" ) );
//      SetBackgroundColour ( back_color );

//      wxColour text_color = GetGlobalColor ( _T ( "UINFF" ) );          // or UINFD
//      SetForegroundColour ( text_color );

      CreateControls();

      SetColorScheme(global_color_scheme);

// This fits the dialog to the minimum size dictated by
// the sizers
      GetSizer()->Fit ( this );

// This ensures that the dialog cannot be sized smaller
// than the minimum size
      GetSizer()->SetSizeHints ( this );

      return true;
}

void AISTargetQueryDialog::SetColorScheme(ColorScheme cs)
{
      if(cs != m_colorscheme)
      {
            long wstyle = wxDEFAULT_FRAME_STYLE;
            if (( global_color_scheme != GLOBAL_COLOR_SCHEME_DAY ) && ( global_color_scheme != GLOBAL_COLOR_SCHEME_RGB ))
                  wstyle |= ( wxNO_BORDER );

            SetWindowStyle(wstyle);

            m_colorscheme = cs;

            wxColour back_color = GetGlobalColor ( _T ( "UIBCK" ) );
            wxColour text_color = GetGlobalColor ( _T ( "UITX1" ) );          // or UINFD

            SetBackgroundColour ( back_color );
            SetForegroundColour ( text_color );

            if(m_pQueryTextCtl)
            {
                  m_pQueryTextCtl->SetBackgroundColour ( back_color );
                  m_pQueryTextCtl->SetForegroundColour ( text_color );
            }

            if(m_okButton)
            {
                  m_okButton->SetBackgroundColour ( back_color );
                  m_okButton->SetForegroundColour ( text_color );
            }


            Refresh();
      }
}




void AISTargetQueryDialog::CreateControls()
{

// A top-level sizer
      wxBoxSizer* topSizer = new wxBoxSizer ( wxVERTICAL );
      SetSizer ( topSizer );

// A second box sizer to give more space around the controls
      m_pboxSizer = new wxBoxSizer ( wxVERTICAL );
      topSizer->Add ( m_pboxSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0 );

// Here is the query result
      m_pQueryTextCtl = new AISInfoWin ( this );
      m_pQueryTextCtl->SetHPad(8);
      m_pQueryTextCtl->SetVPad(2);

 //     wxColour back_color =GetGlobalColor ( _T ( "UIBCK" ) );
 //     m_pQueryTextCtl->SetBackgroundColour ( back_color );

 //    wxColour text_color = GetGlobalColor ( _T ( "UINFF" ) );          // or UINFD
 //     m_pQueryTextCtl->SetForegroundColour ( text_color );

      m_pboxSizer->Add ( m_pQueryTextCtl, 0, wxALIGN_LEFT|wxALL, 0 );


// A horizontal box sizer to contain Reset, OK, Cancel and Help
      wxBoxSizer* okCancelBox = new wxBoxSizer ( wxHORIZONTAL );
      topSizer->Add ( okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

//    Button color
//      wxColour button_color = GetGlobalColor ( _T ( "UIBCK" ) );;

// The OK button

      m_okButton = new wxButton( this, xID_OK, _("Ok"), wxDefaultPosition, wxDefaultSize, 0 );

      okCancelBox->Add ( m_okButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
//      m_okButton->SetBackgroundColour ( button_color );
//      m_okButton->SetForegroundColour ( text_color );

}

void AISTargetQueryDialog::UpdateText()
{
      wxString query_text;
      int n_nl;

      if(m_pQueryTextCtl)
             m_pQueryTextCtl->Clear();

      if(m_MMSI >= 0)                        //  Faulty MMSI could be reported as 0
      {
            AIS_Target_Data *td = g_pAIS->Get_Target_Data_From_MMSI(m_MMSI);
            if(td)
            {
                  query_text = td->BuildQueryResult();

                  if(m_pQueryTextCtl)
                  {
                        m_pQueryTextCtl->AppendText ( query_text );
                        m_pQueryTextCtl->SetInsertionPoint ( 0 );

                        wxSize osize = m_pQueryTextCtl->GetOptimumSize(&n_nl);

                        m_pQueryTextCtl->SetSize(osize);
                        m_pboxSizer->SetMinSize(osize);
                        m_pboxSizer->FitInside(m_pQueryTextCtl);


                        //    Grow/Shrink "this" to fit the contents
                        if(n_nl != m_nl)
                        {
                              m_nl = n_nl;
                              Fit();
                        }
                  }
            }
      }
}
void AISTargetQueryDialog::OnMove( wxMoveEvent& event )
{
      //    Record the dialog position
      wxPoint p = event.GetPosition();
      g_ais_query_dialog_x = p.x;
      g_ais_query_dialog_y = p.y;
      event.Skip();
}


//---------------------------------------------------------------------------------------
//          S57QueryDialog Implementation
//---------------------------------------------------------------------------------------
IMPLEMENT_CLASS ( S57QueryDialog, wxDialog )  // was wxDialog


// S57QueryDialog event table definition

BEGIN_EVENT_TABLE ( S57QueryDialog, wxDialog )  //ws wxDialog
END_EVENT_TABLE()


S57QueryDialog::S57QueryDialog( )
{
        Init();
}


S57QueryDialog::S57QueryDialog ( wxWindow* parent,
                                 wxWindowID id, const wxString& caption,
                                 const wxPoint& pos, const wxSize& size, long style )
{
      Init();
        Create ( parent, id, caption, pos, size, style );
}

S57QueryDialog::~S57QueryDialog( )
{
      m_pTree->DeleteAllItems();
      delete m_pTree;
      delete[] m_id_array;

}


/// Initialisation
void S57QueryDialog::Init( )
{
        m_pTree = NULL;
        m_n_items = 0;
        m_ppOD = NULL;
        m_id_array = NULL;

}

void S57QueryDialog::SetText ( wxString &text_string )
{
        QueryResult = text_string;
}

void S57QueryDialog::SetObjectTree(void **ppOD, int n_items)
{
    m_n_items = n_items;
    m_ppOD = ppOD;
}


/*
* S57QueryDialog creator
*/

bool S57QueryDialog::Create ( wxWindow* parent,
                              wxWindowID id, const wxString& caption,
                              const wxPoint& pos, const wxSize& size, long style )
{
// We have to set extra styles before creating the
// dialog
//      SetExtraStyle(wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP);

        //    As a display optimization....
        //    if current color scheme is other than DAY,
        //    Then create the dialog ..WITHOUT.. borders and title bar.
        //    This way, any window decorations set by external themes, etc
        //    will not detract from night-vision

        long wstyle = wxDEFAULT_FRAME_STYLE;
//        if ( global_color_scheme != GLOBAL_COLOR_SCHEME_DAY )
//                wstyle |= ( wxNO_BORDER );

        if ( !wxDialog::Create ( parent, id, caption, pos, size, wstyle ) )
                return false;

        wxColour back_color = GetGlobalColor ( _T ( "UIBDR" ) );
        SetBackgroundColour ( back_color );

        wxFont *dFont = wxTheFontList->FindOrCreateFont ( 10, wxFONTFAMILY_TELETYPE,
                        wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );

        SetFont ( *dFont );
        CreateControls();

// This fits the dialog to the minimum size dictated by
// the sizers
        GetSizer()->Fit ( this );

// This ensures that the dialog cannot be sized smaller
// than the minimum size
        GetSizer()->SetSizeHints ( this );

// Centre the dialog on the parent or (if none) screen
        Centre();
        return true;
}




void S57QueryDialog::CreateControls()
{
      wxFont *qFont = wxTheFontList->FindOrCreateFont ( 14, wxFONTFAMILY_TELETYPE,
                  wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
      wxColour back_color = GetGlobalColor ( _T ( "UIBCK" ) );
      wxColour text_color = GetGlobalColor ( _T ( "UINFF" ) );

// A top-level sizer
        wxBoxSizer* topSizer = new wxBoxSizer ( wxVERTICAL );
        SetSizer ( topSizer );

// A second box sizer to give more space around the controls
        wxBoxSizer* boxSizer = new wxBoxSizer ( wxHORIZONTAL );
        topSizer->Add ( boxSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND|wxALL, 5 );

//  The Tree control
        m_pTree = new S57ObjectTree(this, ID_S57QUERYTREECTRL, wxDefaultPosition, wxSize ( -1, 500 ), wxTR_HAS_BUTTONS);
        m_root_id = m_pTree->AddRoot(_("Chart"));

        m_id_array = new wxTreeItemId[m_n_items];

        for(int i=0 ; i < m_n_items ; i++)
        {
              S57ObjectDesc *pOD = (S57ObjectDesc *) m_ppOD[i];
              MyTreeItemData *pmtid = new MyTreeItemData(pOD);
              m_id_array[i] = m_pTree->AppendItem(m_root_id, pOD->S57ClassDesc, -1, -1, pmtid);
        }
        m_pTree->Expand(m_root_id);

        m_pTree->SetBackgroundColour ( back_color );
        m_pTree->SetForegroundColour ( text_color );
        m_pTree->SetFont ( *qFont );

        m_pTree->SetQuickBestSize(false);
        wxSize sz = m_pTree->GetBestSize();
        m_pTree->SetSize(sz.x, -1);
        boxSizer->Add ( m_pTree, 0, wxALL, 5 );


// Here is the query result as a Text Control

//    Set the Text Control style
        int tcstyle = wxTE_MULTILINE | wxTE_READONLY;

//    wxX11 TextCtrl is broken in many ways.
//    Here, the wxTE_DONTWRAP flag creates a horizontal scroll bar
//    which fails in wxX11 2.8.2....
#ifndef __WXX11__
        tcstyle |= wxTE_DONTWRAP;
#endif

        m_pQueryTextCtl = new wxTextCtrl ( this, -1, _T ( "" ), wxDefaultPosition, wxSize ( 500, 500 ), tcstyle );

        m_pQueryTextCtl->SetBackgroundColour ( back_color );
        m_pQueryTextCtl->SetForegroundColour ( text_color );

        boxSizer->Add ( m_pQueryTextCtl, 0, wxALL|wxEXPAND, 5 );

        m_pQueryTextCtl->SetFont ( *qFont );

        //  Get the pixel width of the largest character for future reference
        int w, h, descent;
        GetTextExtent(_T("W"), &w, &h, &descent, NULL, qFont);
        m_char_width = w;

        m_pQueryTextCtl->SetSelection ( 0,0 );
        m_pQueryTextCtl->SetInsertionPoint ( 0 );


// A horizontal box sizer to contain Reset, OK, Cancel and Help
//        wxBoxSizer* okCancelBox = new wxBoxSizer ( wxHORIZONTAL );
//        topSizer->Add ( okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND|wxALL, 5 );

//    Button color
        wxColour button_color = GetGlobalColor ( _T ( "UIBCK" ) );

// The OK button
        wxButton* ok = new wxButton ( this, wxID_OK, _( "OK" ), wxDefaultPosition, wxDefaultSize, 0 );
        topSizer->Add ( ok, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
        ok->SetBackgroundColour ( button_color );
        ok->SetForegroundColour ( text_color );

/*
// The Cancel button
        wxButton* cancel = new wxButton ( this, wxID_CANCEL, _ ( "&Cancel" ), wxDefaultPosition, wxDefaultSize, 0 );
        okCancelBox->Add ( cancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
        cancel->SetBackgroundColour ( button_color );
        cancel->SetForegroundColour ( text_color );

// The Help button
        wxButton* help = new wxButton ( this, wxID_HELP, _T ( "&Help" ), wxDefaultPosition, wxDefaultSize, 0 );
        okCancelBox->Add ( help, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
        help->SetBackgroundColour ( button_color );
        help->SetForegroundColour ( text_color );
*/
}

//    Process a "notification" from Tree control

void S57QueryDialog::SetSelectedItem(wxTreeItemId item_id)
{
      MyTreeItemData *pmtid = (MyTreeItemData *)m_pTree->GetItemData(item_id);

      m_pQueryTextCtl->Clear();

      if(pmtid)
      {
            //    Calculate the column constants based on the text control size....
            int width, height;
            m_pQueryTextCtl->GetSize(&width, &height);

            //    Format the string
            int rcol = width / m_char_width;
            wxString fs = format_attributes(pmtid->m_pOD->Attributes, 15, rcol-1);
            SetText ( fs );

            m_pQueryTextCtl->AppendText ( QueryResult );
      }

      m_pQueryTextCtl->SetInsertionPoint(0);

      m_pQueryTextCtl->Refresh();

}

void S57QueryDialog::OnPaint ( wxPaintEvent& event )
{
      m_pQueryTextCtl->Clear();
      m_pQueryTextCtl->AppendText ( QueryResult );
      m_pQueryTextCtl->SetInsertionPoint(0);

}

wxString S57QueryDialog::format_attributes(wxString &attr, int lcol, int rcol)
{
      wxString result;

      //    First, emit verbatim everything up to and including the string "Attributes\n"
      int index = attr.Find(_T("Attributes\n"));
      if(index != wxNOT_FOUND)
      {
            index += strlen("Attributes\n");
            result << attr.Mid(0, index);
      }
      else
            index = 0;

      wxString remains = attr.Mid(index);
      while(remains.Len())
      {
            // emit verbatim up to <atval>
            int rindex = remains.Find(_T("<atval>"));
            if(rindex != wxNOT_FOUND)
            {
                  result << remains.Mid(0, rindex);

                  //    Skip the keyword
                  rindex += 7;
                  //    Find the ending keyword
                  int rtindex = remains.Find(_T("<\\atval>"));
                  if(rtindex != wxNOT_FOUND)
                  {
                        //    extract the denoted string
                        wxString attval = remains.Mid(rindex, rtindex - rindex);

                        //    Do a gross format of attval
                        unsigned int icol = lcol;
                        unsigned int icol_max = rcol;

                        unsigned int iline=0;
                        wxString tkr;

                        wxStringTokenizer tk(attval, _T(" |,"));
                        while ( tk.HasMoreTokens() )
                        {
                              wxString token = tk.GetNextToken();
                              if(iline + token.Len() < icol_max - icol)
                              {
                                    result << token;
                                    result << wxChar(' ');
                                    tkr.Clear();
                              }
                              else
                              {
                                    tkr = token;
                                    tkr << wxChar(' ');
                              }

                              iline += token.Len() + 1;

                              wxChar dlim = tk.GetLastDelimiter();

                              if((iline > icol_max - icol) || (dlim == '|'))
                              {
                                    result << wxChar('\n');

                                    for(unsigned int k=0 ; k < icol ; k++)
                                          result << wxChar(' ');

                                    result << tkr;

                                    iline = tkr.Len();
                              }
                        }

                        //    Adjust the remains string
                        wxString tmp = remains.Mid(rtindex+8);
                        remains = tmp;


                  }
                  else
                        result << _T("Format Error (missing <\\atval>)");
            }
            else
            {
                  result << remains;
                  remains.Clear();
            }

      }


      return result;
}



//---------------------------------------------------------------------------------------
//          S57 Object Query Tree Control Implementation
//---------------------------------------------------------------------------------------
IMPLEMENT_CLASS ( S57ObjectTree, wxTreeCtrl )


// S57ObjectTree event table definition

BEGIN_EVENT_TABLE ( S57ObjectTree, wxTreeCtrl )
            EVT_TREE_ITEM_EXPANDING( ID_S57QUERYTREECTRL, S57ObjectTree::OnItemExpanding)
            EVT_TREE_SEL_CHANGED( ID_S57QUERYTREECTRL, S57ObjectTree::OnItemSelectChange)
END_EVENT_TABLE()


S57ObjectTree::S57ObjectTree( )
{
      Init();
}

S57ObjectTree::S57ObjectTree ( S57QueryDialog* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
      Init();
      m_parent = parent;
      Create ( parent, id, pos, size, style );
}

S57ObjectTree::~S57ObjectTree( )
{
}

void S57ObjectTree::Init( )
{
}

void S57ObjectTree::OnItemExpanding( wxTreeEvent& event)
{
}

void S57ObjectTree::OnItemSelectChange( wxTreeEvent& event)
{
      wxTreeItemId item_id = event.GetItem();
      m_parent->SetSelectedItem(item_id);
}


//-----------------------------------------------------------------------
//
//    AIS Rollover window implementation
//
//-----------------------------------------------------------------------
BEGIN_EVENT_TABLE(AISroWin, wxWindow)
            EVT_PAINT(AISroWin::OnPaint)

            END_EVENT_TABLE()

// Define a constructor
AISroWin::AISroWin(wxWindow *parent):
            wxWindow(parent, wxID_ANY, wxPoint(0,0), wxSize(1,1), wxNO_BORDER)
{
      m_pbm = NULL;

      Hide();
}

AISroWin::~AISroWin()
{
      delete m_pbm;
}

void AISroWin::SetBitmap()
{
      int h, w;

      wxClientDC cdc(GetParent());


      wxFont *plabelFont = pFontMgr->GetFont(_T("AISRollover"));
      cdc.GetTextExtent(m_string, &w, &h,  NULL, NULL, plabelFont);

      m_size.x = w + 4;
      m_size.y = h + 10;

      wxMemoryDC mdc;

      delete m_pbm;
      m_pbm = new wxBitmap( m_size.x, m_size.y, -1);
      mdc.SelectObject(*m_pbm);

      mdc.Blit(0, 0, m_size.x, m_size.y, &cdc, m_position.x, m_position.y);

      //    Draw the text
      mdc.SetFont(*plabelFont);
      mdc.SetTextForeground(pFontMgr->GetFontColor(_T("AISRollover")));

      mdc.DrawText(m_string,0,0);

      SetSize(m_position.x, m_position.y, m_size.x, m_size.y);           // Assumes a nominal 32 x 32 cursor


}

void AISroWin::OnPaint(wxPaintEvent& event)
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

//------------------------------------------------------------------------------
//    CM93 Detail Slider Implementation
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CM93DSlide, wxDialog)
            EVT_MOVE( CM93DSlide::OnMove )
            EVT_COMMAND_SCROLL_THUMBRELEASE(-1, CM93DSlide::OnChangeValue)
            EVT_COMMAND_SCROLL_LINEUP(-1, CM93DSlide::OnChangeValue)
            EVT_COMMAND_SCROLL_LINEDOWN(-1, CM93DSlide::OnChangeValue)
            EVT_COMMAND_SCROLL_PAGEUP(-1, CM93DSlide::OnChangeValue)
            EVT_COMMAND_SCROLL_PAGEDOWN(-1, CM93DSlide::OnChangeValue)
            EVT_COMMAND_SCROLL_BOTTOM(-1, CM93DSlide::OnChangeValue)
            EVT_COMMAND_SCROLL_TOP(-1, CM93DSlide::OnChangeValue)
            EVT_CLOSE(CM93DSlide::OnClose)
            END_EVENT_TABLE()

CM93DSlide::CM93DSlide ( wxWindow *parent, wxWindowID id, int value, int minValue, int maxValue,
                         const wxPoint& pos, const wxSize& size, long style, const wxString& title)
{
      Init();
      Create(parent, ID_CM93ZOOMG, value, minValue, maxValue, pos, size, style, title );
}


CM93DSlide::~CM93DSlide()
{
      delete m_pCM93DetailSlider;
}

void CM93DSlide::Init(void)
{
      m_pCM93DetailSlider = NULL;
}


bool CM93DSlide::Create( wxWindow *parent, wxWindowID id, int value, int minValue, int maxValue,
                    const wxPoint& pos, const wxSize& size, long style, const wxString& title)
{
      if(!wxDialog::Create(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE))
            return false;

      m_pparent = parent;

      m_pCM93DetailSlider = new wxSlider(this, id , value, minValue, maxValue,
                                         wxPoint(0,0), wxDefaultSize, wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS, wxDefaultValidator, title);

      m_pCM93DetailSlider->SetSize(wxSize(200, -1));

      m_pCM93DetailSlider->InvalidateBestSize();
      wxSize bs = m_pCM93DetailSlider->GetBestSize();

      m_pCM93DetailSlider->SetSize(wxSize(200, bs.y));
      Fit();

      m_pCM93DetailSlider->SetValue(g_cm93_zoom_factor);

      Hide();

      return true;
}

void CM93DSlide::OnCancelClick( wxCommandEvent& event )
{
      g_bShowCM93DetailSlider = false;
      Close();
}


void CM93DSlide::OnClose(wxCloseEvent& event)
{
      g_bShowCM93DetailSlider = false;

      Destroy();
      pCM93DetailSlider = NULL;
}


void CM93DSlide::OnMove( wxMoveEvent& event )
{
      //    Record the dialog position
      wxPoint p = event.GetPosition();
      g_cm93detail_dialog_x = p.x;
      g_cm93detail_dialog_y = p.y;

      event.Skip();
}

void CM93DSlide::OnChangeValue( wxScrollEvent& event)
{
      g_cm93_zoom_factor = m_pCM93DetailSlider->GetValue();

      ::wxBeginBusyCursor();

      cc1->ReloadVP();
      cc1->Refresh(false);

      ::wxEndBusyCursor();
}


//-----------------------------------------------------------------------
//
//    Chart Info Rollover window implementation
//
//-----------------------------------------------------------------------
BEGIN_EVENT_TABLE(ChInfoWin, wxWindow)
            END_EVENT_TABLE()

// Define a constructor
ChInfoWin::ChInfoWin(wxWindow *parent):
            wxWindow(parent, wxID_ANY, wxPoint(0,0), wxSize(1,1), wxNO_BORDER)
{

      int tcstyle = wxTE_MULTILINE | wxTE_READONLY | wxNO_BORDER;
      m_pInfoTextCtl = new wxTextCtrl ( this, -1, _T ( "" ), wxDefaultPosition, wxDefaultSize, tcstyle );

      Hide();
}

ChInfoWin::~ChInfoWin()
{
      delete m_pInfoTextCtl;
}
void ChInfoWin::SetBitmap()
{


      SetBackgroundColour(GetGlobalColor ( _T ( "UBLCK" ) ));

      m_pInfoTextCtl->SetBackgroundColour(GetGlobalColor ( _T ( "UIBCK" ) ));
      m_pInfoTextCtl->SetForegroundColour(GetGlobalColor ( _T ( "UITX1" ) ));

      m_pInfoTextCtl->SetSize(4, 4, m_size.x-8, m_size.y-8);
      m_pInfoTextCtl->Clear();
      m_pInfoTextCtl->AppendText(m_string);


      SetSize(m_position.x, m_position.y, m_size.x, m_size.y);
}


