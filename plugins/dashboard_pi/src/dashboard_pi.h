/******************************************************************************
 * $Id: dashboard_pi.h, v1.0 2010/08/05 SethDart Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Dashboard Plugin
 * Author:   Jean-Eudes Onfray
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register                               *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 */

#ifndef _DASHBOARDPI_H_
#define _DASHBOARDPI_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#define     PLUGIN_VERSION_MAJOR    1
#define     PLUGIN_VERSION_MINOR    2

#define     MY_API_VERSION_MAJOR    1
#define     MY_API_VERSION_MINOR    6

#include <wx/notebook.h>
#include <wx/fileconf.h>
#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include <wx/spinctrl.h>
#include <wx/aui/aui.h>
#include <wx/fontpicker.h>
//wx2.9 #include <wx/wrapsizer.h>
#include "../../../include/ocpn_plugin.h"

#include "nmea0183/nmea0183.h"
#include "instrument.h"
#include "speedometer.h"
#include "compass.h"
#include "wind.h"
#include "rudder_angle.h"
#include "gps.h"
#include "depth.h"
#include "clock.h"
#include "celestial.h"

#define DASHBOARD_TOOL_POSITION -1          // Request default positioning of toolbar tool

//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------


class dashboard_pi : public wxTimer, opencpn_plugin_16
{
public:
      dashboard_pi(void *ppimgr);
      ~dashboard_pi(void);

//    The required PlugIn Methods
      int Init(void);
      bool DeInit(void);

      void Notify();

      int GetAPIVersionMajor();
      int GetAPIVersionMinor();
      int GetPlugInVersionMajor();
      int GetPlugInVersionMinor();
      wxBitmap *GetPlugInBitmap();
      wxString GetCommonName();
      wxString GetShortDescription();
      wxString GetLongDescription();

//    The optional method overrides
      void SetNMEASentence(wxString &sentence);
      void SetPositionFix(PlugIn_Position_Fix &pfix);
      void SetCursorLatLon(double lat, double lon);
      int GetToolbarToolCount(void);
      void OnToolbarToolCallback(int id);
      void ShowPreferencesDialog( wxWindow* parent );
      void SetColorScheme(PI_ColorScheme cs);
      void OnPaneClose( wxAuiManagerEvent& event );
      void UpdateAuiStatus(void);
      bool SaveConfig(void);

private:
      bool LoadConfig(void);
      void ApplyConfig(void);
      void SendSentenceToAllInstruments(int st, double value, wxString unit);
      void SendSatInfoToAllInstruments(int cnt, int seq, SAT_INFO sats[4]);
      void SendUtcTimeToAllInstruments(int st, wxDateTime value);
      int GetInstrumentShownCount();
      DashboardInstrument *GetInstrument( int id, wxWindow *parent );
      void OnContextMenuSelect( wxCommandEvent& evt );

      wxFileConfig     *m_pconfig;
      wxAuiManager     *m_pauimgr;
      int              m_toolbar_item_id;

      wxArrayOfInstrument       m_ArrayOfInstrument;
      int               m_show_id;
      int               m_hide_id;

      NMEA0183             m_NMEA0183;                 // Used to parse NMEA Sentences
      short                mPriPosition, mPriCOGSOG, mPriHeadingM, mPriHeadingT, mPriVar, mPriDateTime, mPriWindR, mPriWindT, mPriDepth;
      double               mVar;
      // FFU
      double               mSatsInView;
      double               mHdm;
      wxDateTime           mUTCDateTime;

//protected:
//      DECLARE_EVENT_TABLE();
};

class DashboardPreferencesDialog : public wxDialog
{
public:
      DashboardPreferencesDialog( wxWindow *pparent, wxWindowID id, wxArrayOfInstrument config );
      ~DashboardPreferencesDialog() {}

      void OnCloseDialog(wxCloseEvent& event);
      void OnDashboardSelected(wxListEvent& event);
      void OnDashboardAdd(wxCommandEvent& event);
      void OnDashboardDelete(wxCommandEvent& event);
      void OnInstrumentSelected(wxListEvent& event);
      void OnInstrumentAdd(wxCommandEvent& event);
      void OnInstrumentEdit(wxCommandEvent& event);
      void OnInstrumentDelete(wxCommandEvent& event);
      void OnInstrumentUp(wxCommandEvent& event);
      void OnInstrumentDown(wxCommandEvent& event);
      void SaveDashboardConfig();

      wxArrayOfInstrument            m_Config;
      wxFontPickerCtrl             *m_pFontPickerData;
      wxFontPickerCtrl             *m_pFontPickerLabel;
      wxFontPickerCtrl             *m_pFontPickerSmall;

private:
      void UpdateDashboardButtonsState(void);
      void UpdateButtonsState(void);
      int                           curSel;
      wxListCtrl                   *m_pListCtrlDashboards;
      wxBitmapButton               *m_pButtonAddDashboard;
      wxBitmapButton               *m_pButtonDeleteDashboard;
      wxPanel                      *m_pPanelDashboard;
      wxTextCtrl                   *m_pTextCtrlCaption;
      wxCheckBox                   *m_pCheckBoxIsVisible;
      wxListCtrl                   *m_pListCtrlInstruments;
      wxButton                     *m_pButtonAdd;
      wxButton                     *m_pButtonEdit;
      wxButton                     *m_pButtonDelete;
      wxButton                     *m_pButtonUp;
      wxButton                     *m_pButtonDown;
};

class AddInstrumentDlg : public wxDialog
{
public:
      AddInstrumentDlg(wxWindow *pparent, wxWindowID id);
      ~AddInstrumentDlg() {}

      unsigned int GetInstrumentAdded();

private:
      wxListCtrl*                   m_pListCtrlInstruments;
};

enum
{
      ID_DASHBOARD_WINDOW
};

#endif

