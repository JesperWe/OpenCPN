/******************************************************************************
 * $Id: instrument.cpp, v1.0 2010/08/30 SethDart Exp $
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


#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "instrument.h"

//----------------------------------------------------------------
//
//    Generic DashboardInstrument Implementation
//
//----------------------------------------------------------------

// wxWS_EX_VALIDATE_RECURSIVELY required to push events to parents
DashboardInstrument::DashboardInstrument(wxWindow *pparent, wxWindowID id, int cap_flag )
      :wxWindow(pparent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
{
    m_cap_flag = cap_flag;

    SetBackgroundStyle( wxBG_STYLE_CUSTOM );

    Connect( wxEVT_CONTEXT_MENU, wxContextMenuEventHandler( DashboardInstrument::OnContextMenu ) );
    //Connect( wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( dashboard_pi::OnContextMenuSelect ) );
    Connect( wxEVT_SIZE, wxSizeEventHandler(DashboardInstrument::OnSize) );
    Connect( wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(DashboardInstrument::OnEraseBackground) );
    Connect( wxEVT_PAINT, wxPaintEventHandler(DashboardInstrument::OnPaint) );
}

DashboardInstrument::~DashboardInstrument()
{
}

int DashboardInstrument::GetCapacity()
{
      return m_cap_flag;
}

void DashboardInstrument::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
        // intentionally empty
}

void DashboardInstrument::OnSize(wxSizeEvent& evt)
{
    evt.Skip();

//wxSize esz = GetClientSize();
wxSize esz = evt.GetSize();
wxSize sz = GetSize( esz );
wxLogMessage(wxString::Format(_T("GetSize %dx%d => %dx%d "), esz.x, esz.y, sz.x, sz.y));
//    Layout();
    SetSize( sz );
//    Fit();
//    GetParent()->Layout();
    Refresh();
}

void DashboardInstrument::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxBufferedPaintDC pdc( this );
    if( !pdc.IsOk() ) return;

    wxSize size = GetSize( GetClientSize() );
    if( size.x == 0 || size.y == 0 ) return;

    wxBitmap bm( pdc.GetSize().x, pdc.GetSize().y, 32 );
    bm.UseAlpha();
    wxMemoryDC mdc( bm );
    wxGCDC dc( mdc );
    wxColour cl;
    GetGlobalColor( _T("DASHB"), &cl );
    dc.SetBackground( cl );
    dc.Clear();

    Draw( &dc );

    mdc.SelectObject( wxNullBitmap );
    pdc.DrawBitmap( bm, 0, 0, false );
}

void DashboardInstrument::OnContextMenu( wxContextMenuEvent& event )
{
    wxMenu* contextMenu = new wxMenu();

/*TODO: list instruments */

    contextMenu->AppendSeparator();
    wxMenuItem* btnPrefs = new wxMenuItem( contextMenu, ID_DASH_PREFS, _("Preferences...") );
    contextMenu->Append( btnPrefs );

    PopupMenu( contextMenu );
    delete contextMenu;
}

//----------------------------------------------------------------
//
//    DashboardInstrument_Simple Implementation
//
//----------------------------------------------------------------

DashboardInstrument_Single::DashboardInstrument_Single( wxWindow *parent, wxWindowID id, int cap_flag, wxString format )
      :DashboardInstrument( parent, id, cap_flag )
{
      m_format = format;
      m_data = _T("---");
}

wxSize DashboardInstrument_Single::GetSize( wxSize hint )
{
      wxClientDC dc(this);
      int w;
      dc.GetTextExtent(_T("000"), &w, &m_DataHeight, 0, 0, g_pFontData);

      return wxSize( DefaultWidth, m_DataHeight );
}

void DashboardInstrument_Single::Draw(wxGCDC* dc)
{
      wxColour cl;

      dc->SetFont(*g_pFontData);
      GetGlobalColor(_T("DASHF"), &cl);
      dc->SetTextForeground(cl);

      dc->DrawText(m_data, 10, 0);
}

void DashboardInstrument_Single::SetData(int st, double data, wxString unit)
{
      if (m_cap_flag & st){
            if(!wxIsNaN(data)){
                if (unit == _T("C"))
                  m_data = wxString::Format(m_format, data)+DEGREE_SIGN+_T("C");
                else if (unit == _T("Deg"))
                  m_data = wxString::Format(m_format, data)+DEGREE_SIGN;
                else if (unit == _T("DegT"))
                  m_data = wxString::Format(m_format, data)+DEGREE_SIGN+_(" true");
                else if (unit == _T("DegM"))
                  m_data = wxString::Format(m_format, data)+DEGREE_SIGN+_(" mag");
                else if (unit == _T("DegL"))
                  m_data = _T(">")+ wxString::Format(m_format, data)+DEGREE_SIGN;
                else if (unit == _T("DegR"))
                  m_data = wxString::Format(m_format, data)+DEGREE_SIGN+_T("<");
                else if (unit == _T("N")) //Knots
                  m_data = wxString::Format(m_format, data)+_T(" Kts");
/* maybe in the future ...
                else if (unit == _T("M")) // m/s
                  m_data = wxString::Format(m_format, data)+_T(" m/s");
                else if (unit == _T("K")) // km/h
                  m_data = wxString::Format(m_format, data)+_T(" km/h");
 ... to be completed
 */
                else
                  m_data = wxString::Format(m_format, data)+_T(" ")+unit;
            }
            else
                m_data = _T("---");
      }
}

//----------------------------------------------------------------
//
//    DashboardInstrument_Position Implementation
//
//----------------------------------------------------------------

DashboardInstrument_Position::DashboardInstrument_Position(wxWindow *pparent, wxWindowID id, int cap_flag1, int cap_flag2)
      :DashboardInstrument(pparent, id, cap_flag1 | cap_flag2)
{

      m_data1 = _T("---");
      m_data2 = _T("---");
      m_cap_flag1 = cap_flag1;
      m_cap_flag2 = cap_flag2;
}

wxSize DashboardInstrument_Position::GetSize( wxSize hint )
{
      wxClientDC dc(this);
      int w;
      dc.GetTextExtent(_T("000  00.0000 W"), &w, &m_DataHeight, 0, 0, g_pFontData);

      return wxSize( w+10, m_DataHeight*2 );
}

void DashboardInstrument_Position::Draw(wxGCDC* dc)
{
      wxColour cl;

      dc->SetFont(*g_pFontData);
      GetGlobalColor(_T("DASHF"), &cl);
      dc->SetTextForeground(cl);

      dc->DrawText(m_data1, 10, 0);
      dc->DrawText(m_data2, 10, m_DataHeight);
}

void DashboardInstrument_Position::SetData(int st, double data, wxString unit)
{
      if (st == m_cap_flag1)
      {
            m_data1 = toSDMM(1, data);
      }
      else if (st == m_cap_flag2)
      {
            m_data2 = toSDMM(2, data);
      }
      else return;
      Refresh();
}

/**************************************************************************/
/*          Some assorted utilities                                       */
/**************************************************************************/

wxString toSDMM ( int NEflag, double a )
{
      short neg = 0;
      int d;
      long m;

      if ( a < 0.0 )
      {
            a = -a;
            neg = 1;
      }
      d = ( int ) a;
      m = ( long ) ( ( a - ( double ) d ) * 60000.0 );

      if ( neg )
            d = -d;

      wxString s;

      if ( !NEflag )
            s.Printf ( _T ( "%d %02ld.%03ld'" ), d, m / 1000, m % 1000 );
      else
      {
            if ( NEflag == 1 )
            {
                  char c = 'N';

                  if ( neg )
                  {
                        d = -d;
                        c = 'S';
                  }

                  s.Printf ( _T ( "%03d %02ld.%03ld %c" ), d, m / 1000, ( m % 1000 ), c );
            }
            else if ( NEflag == 2 )
            {
                  char c = 'E';

                  if ( neg )
                  {
                        d = -d;
                        c = 'W';
                  }
                  s.Printf ( _T ( "%03d %02ld.%03ld %c" ), d, m / 1000, ( m % 1000 ), c );
            }
      }
      return s;
}

