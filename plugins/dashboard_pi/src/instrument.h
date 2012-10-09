/******************************************************************************
 * $Id: instrument.h, v1.0 2010/08/30 SethDart Exp $
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

#ifndef _INSTRUMENT_H_
#define _INSTRUMENT_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// Required GetGlobalColor
#include "../../../include/ocpn_plugin.h"
#include <wx/dcbuffer.h>

const wxString DEGREE_SIGN = wxString::Format(_T("%c"), 0x00B0); // This is the degree sign in UTF8. It should be correctly handled on both Win & Unix
#define DefaultWidth 150

extern wxFont *g_pFontData;
extern wxFont *g_pFontLabel;
extern wxFont *g_pFontSmall;

class DashboardInstrument;
wxString toSDMM ( int NEflag, double a );

enum
{
    OCPN_DBP_STC_LAT = 1 << 0,
    OCPN_DBP_STC_LON = 1 << 1,
    OCPN_DBP_STC_SOG = 1 << 2,
    OCPN_DBP_STC_COG = 1 << 3,
    OCPN_DBP_STC_STW = 1 << 4,
    OCPN_DBP_STC_HDM = 1 << 5,
    OCPN_DBP_STC_HDT = 1 << 6,
    OCPN_DBP_STC_HMV = 1 << 7, // Magnetic variation
    OCPN_DBP_STC_BRG = 1 << 8,
    OCPN_DBP_STC_AWA = 1 << 9,
    OCPN_DBP_STC_AWS = 1 << 10,
    OCPN_DBP_STC_TWA = 1 << 11,
    OCPN_DBP_STC_TWS = 1 << 12,
    OCPN_DBP_STC_DPT = 1 << 13,
    OCPN_DBP_STC_TMP = 1 << 14,
    OCPN_DBP_STC_VMG = 1 << 15,
    OCPN_DBP_STC_RSA = 1 << 16,
    OCPN_DBP_STC_SAT = 1 << 17,
    OCPN_DBP_STC_GPS = 1 << 18,
    OCPN_DBP_STC_PLA = 1 << 19, // Cursor latitude
    OCPN_DBP_STC_PLO = 1 << 20, // Cursor longitude
    OCPN_DBP_STC_CLK = 1 << 21,
    OCPN_DBP_STC_MON = 1 << 22,
    OCPN_DBP_STC_ATMP = 1 << 23, //AirTemp
	OCPN_DBP_STC_MWD = 1 << 24,  //True Wind direction & Speed
	OCPN_DBP_STC_VWT = 1 << 25,  //True Wind angle & Speed
	OCPN_DBP_STC_AWA2 = 1 << 26  //App Wind angle, sends unconverted Spd. value
};

enum
{
      ID_DASH_PREFS = 999,
      ID_DASH_VERTICAL,
      ID_DASH_HORIZONTAL
};

class DashboardInstrument : public wxWindow
{
public:
      DashboardInstrument( wxWindow *parent, wxWindowID id, int cap_flag );
      ~DashboardInstrument();

      int GetCapacity();
      void OnEraseBackground(wxEraseEvent& WXUNUSED(evt));
      void OnSize(wxSizeEvent& evt);
      virtual wxSize GetSize( wxSize hint ) = 0;
      void OnPaint(wxPaintEvent& WXUNUSED(event));
      virtual void SetData(int st, double data, wxString unit) = 0;
/*TODO: OnKeyPress pass event to main window or disable focus*/

protected:
      int               m_cap_flag;

      virtual void Draw(wxGCDC* dc) = 0;

private:
    void OnContextMenu( wxContextMenuEvent& evt );
};

class DashboardInstrument_Single : public DashboardInstrument
{
public:
      DashboardInstrument_Single(wxWindow *pparent, wxWindowID id, int cap, wxString format);
      ~DashboardInstrument_Single(){}

      wxSize GetSize( wxSize hint );
      void SetData(int st, double data, wxString unit);

protected:
      wxString          m_data;
      wxString          m_format;
      int               m_DataHeight;

      void Draw(wxGCDC* dc);
};

class DashboardInstrument_Position : public DashboardInstrument
{
public:
      DashboardInstrument_Position(wxWindow *pparent, wxWindowID id, int cap_flag1=OCPN_DBP_STC_LAT, int cap_flag2=OCPN_DBP_STC_LON);
      ~DashboardInstrument_Position(){}

      wxSize GetSize( wxSize hint );
      void SetData(int st, double data, wxString unit);

protected:
      wxString          m_data1;
      wxString          m_data2;
      int               m_cap_flag1;
      int               m_cap_flag2;
      int               m_DataHeight;

      void Draw(wxGCDC* dc);
};

class DashboardInstrumentContainer
{
      public:
            DashboardInstrumentContainer( int id, DashboardInstrument *instrument, int capa, wxString name, wxString caption ) {
                  m_ID = id; m_pInstrument = instrument; m_cap_flag = capa;
                  m_sName = name; m_sCaption = caption; m_bIsVisible = false; m_bIsDeleted = false; }
            ~DashboardInstrumentContainer(){}

            DashboardInstrument    *m_pInstrument;
            int                     m_ID;
            int                     m_cap_flag;
            bool                    m_bIsVisible; // Only used for config
            bool                    m_bIsDeleted; // Only used for config
            wxString                m_sName;
            wxString                m_sCaption;
};

//    Dynamic arrays of pointers need explicit macros in wx261
#ifdef __WX261
WX_DEFINE_ARRAY_PTR(DashboardInstrumentContainer *, wxArrayOfInstrument);
#else
WX_DEFINE_ARRAY(DashboardInstrumentContainer *, wxArrayOfInstrument);
#endif

#endif
