/******************************************************************************
 * $Id: rudder_angle.cpp, v1.0 2010/08/26 SethDart Exp $
 *
 * Project:  OpenCPN
 * Purpose:  DashBoard Plugin
 * Author:   Jean-Eudes Onfray
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
 */

#include "rudder_angle.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

DashboardInstrument_RudderAngle::DashboardInstrument_RudderAngle( wxWindow *parent, wxWindowID id, wxString title) :
      DashboardInstrument_Dial( parent, id, title, 100, 160, -40, +40)
{
      // Default Rudder position is centered
      m_MainValue = 0;

      //SetOptionMainValue(_T("%3.0f Deg"), DIAL_POSITION_BOTTOMLEFT);
      SetOptionMarker(5, DIAL_MARKER_REDGREEN, 2);
      // Labels are set static because we've no logic to display them this way
      wxString labels[] = {_T("40"), _("30"), _("20"), _("10"), _T("0"), _("10"), _("20"), _("30"), _("40")};
      SetOptionLabel(10, DIAL_LABEL_HORIZONTAL, wxArrayString(9, labels));
//      SetOptionExtraValue(_T("%02.0f"), DIAL_POSITION_INSIDE);
      SetMinSize(wxSize(200, 150));
}

void DashboardInstrument_RudderAngle::SetMainValue(double value)
{
      // Dial works clockwise but Rudder has negative values for left
      // and positive for right so we must inverse it.
      value = -value;

      if (value < m_MainValueMin) m_MainValue = m_MainValueMin;
      else if (value > m_MainValueMax) m_MainValue = m_MainValueMax;
      else m_MainValue = value;

      Refresh(false);
}

void DashboardInstrument_RudderAngle::DrawFrame(wxBufferedDC* dc)
{
      // We don't need the upper part
      // Move center up
      wxRect rect = GetClientRect();
      wxColour cl;

      m_cx = rect.width / 2;
      m_cy = m_TitleHeight + (rect.height - m_TitleHeight) * 0.38;
      m_radius = (rect.height - m_TitleHeight)*.6;

      dc->SetBrush(*wxTRANSPARENT_BRUSH);

      wxPen pen;
      pen.SetStyle(wxSOLID);
      GetGlobalColor(_T("BLUE2"), &cl);
      pen.SetColour(cl);
      dc->SetPen(pen);

      double angle1 = deg2rad(215); // 305-ANGLE_OFFSET
      double angle2 = deg2rad(-35); // 55-ANGLE_OFFSET
      wxCoord x1 = m_cx + (m_radius * cos(angle1));
      wxCoord y1 = m_cy + (m_radius * sin(angle1));
      wxCoord x2 = m_cx + (m_radius * cos(angle2));
      wxCoord y2 = m_cy + (m_radius * sin(angle2));
      dc->DrawArc(x1, y1, x2, y2, m_cx, m_cy);
      dc->DrawLine(x1, y1, x2, y2);
}

void DashboardInstrument_RudderAngle::DrawBackground(wxBufferedDC* dc)
{
      wxCoord x = m_cx - (m_radius * 0.3);
      wxCoord y = m_cy - (m_radius * 0.5);
      dc->DrawEllipticArc(x, y, m_radius * 0.6, m_radius * 1.4, 0, 180);
}
