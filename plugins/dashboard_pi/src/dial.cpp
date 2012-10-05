/******************************************************************************
 * $Id: dial.cpp, v1.0 2010/08/05 SethDart Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Dashboard Plugin
 * Author:   Jean-Eudes Onfray
 *           (Inspired by original work from Andreas Heiming)
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register   *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
 ***************************************************************************
 */

#include "dial.h"

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

double rad2deg(double angle)
{
      return angle*180.0/M_PI;
}
double deg2rad(double angle)
{
      return angle/180.0*M_PI;
}

DashboardInstrument_Dial::DashboardInstrument_Dial( wxWindow *parent, wxWindowID id, wxString title, int cap_flag,
                  int s_angle, int r_angle, int s_value, int e_value) : DashboardInstrument(parent, id, title, cap_flag)
{
      m_AngleStart = s_angle;
      m_AngleRange = r_angle;
      m_MainValueMin = s_value;
      m_MainValueMax = e_value;
      m_MainValueCap = cap_flag;

      m_MainValue = s_value;
      m_ExtraValue = 0;
      m_MainValueFormat = _T("%d");
      m_MainValueOption = DIAL_POSITION_NONE;
      m_ExtraValueFormat = _T("%d");
      m_ExtraValueOption = DIAL_POSITION_NONE;
      m_MarkerOption = DIAL_MARKER_SIMPLE;
      m_MarkerStep = 1;
      m_LabelStep = 1;
      m_MarkerOffset = 1;
      m_LabelOption = DIAL_LABEL_HORIZONTAL;

      SetInstrumentWidth(200);
}

void DashboardInstrument_Dial::SetInstrumentWidth(int width)
{
      wxClientDC dc(this);
      int w;
      dc.GetTextExtent(m_title, &w, &m_TitleHeight, 0, 0, g_pFontTitle);
      m_width = width;
      m_height = m_TitleHeight+width;
      SetMinSize(wxSize(m_width, m_height));
      Refresh(false);
}

void DashboardInstrument_Dial::SetData(int st, double data, wxString unit)
{
      if (st == m_MainValueCap)
            m_MainValue = data;
      else if (st == m_ExtraValueCap)
            m_ExtraValue = data;
}

void DashboardInstrument_Dial::Draw(wxGCDC* bdc)
{
    DrawFrame(bdc);
    DrawMarkers(bdc);
    DrawLabels(bdc);
    DrawBackground(bdc);
    DrawData(bdc, m_MainValue, m_MainValueFormat, m_MainValueOption);
    DrawData(bdc, m_ExtraValue, m_ExtraValueFormat, m_ExtraValueOption);
    DrawForeground(bdc);
}

void DashboardInstrument_Dial::DrawFrame(wxGCDC* dc)
{
      wxRect rect = GetClientRect();
      wxColour cl;

      m_cx = rect.width / 2;
      int availableHeight = rect.height - m_TitleHeight - 6;
      int width, height;
      dc->GetTextExtent(_T("000"), &width, &height, 0, 0, g_pFontLabel);
      m_cy = m_TitleHeight + 2;
      m_cy += availableHeight / 2;
      m_radius = availableHeight / 2;

      GetGlobalColor(_T("DASHL"), &cl);
      dc->SetTextForeground(cl);
      dc->SetBrush(*wxTRANSPARENT_BRUSH);

      wxPen pen;
      pen.SetWidth(2);

      if (m_MarkerOption == DIAL_MARKER_REDGREENBAR)
      {
            pen.SetWidth(4);
            GetGlobalColor(_T("DASHR"), &cl);
            pen.SetColour(cl);
            dc->SetPen(pen);
            double angle1 = deg2rad(270); // 305-ANGLE_OFFSET
            double angle2 = deg2rad(90); // 55-ANGLE_OFFSET
            wxCoord x1 = m_cx + ((m_radius-3) * cos(angle1));
            wxCoord y1 = m_cy + ((m_radius-3) * sin(angle1));
            wxCoord x2 = m_cx + ((m_radius-3) * cos(angle2));
            wxCoord y2 = m_cy + ((m_radius-3) * sin(angle2));
            dc->DrawArc(x1, y1, x2, y2, m_cx, m_cy);
            GetGlobalColor(_T("DASHG"), &cl);
            pen.SetColour(cl);
            dc->SetPen(pen);
            angle1 = deg2rad(90); // 305-ANGLE_OFFSET
            angle2 = deg2rad(270); // 55-ANGLE_OFFSET
            x1 = m_cx + ((m_radius-3) * cos(angle1));
            y1 = m_cy + ((m_radius-3) * sin(angle1));
            x2 = m_cx + ((m_radius-3) * cos(angle2));
            y2 = m_cy + ((m_radius-3) * sin(angle2));
            dc->DrawArc(x1, y1, x2, y2, m_cx, m_cy);
            GetGlobalColor(_T("DASHF"), &cl);
            pen.SetWidth(2);
      }
      GetGlobalColor(_T("DASHF"), &cl);
      pen.SetStyle(wxSOLID);
      pen.SetColour(cl);
      dc->SetPen(pen);

      dc->DrawCircle(m_cx, m_cy, m_radius);
}

void DashboardInstrument_Dial::DrawMarkers(wxGCDC* dc)
{
      if (m_MarkerOption == DIAL_MARKER_NONE)
            return;

      wxColour cl;
      GetGlobalColor(_T("DASHF"), &cl);
      wxPen pen;
      pen.SetStyle(wxSOLID);
      pen.SetColour(cl);
      dc->SetPen(pen);

      int diff_angle = m_AngleStart + m_AngleRange - ANGLE_OFFSET;
      // angle between markers
      double abm = m_AngleRange * m_MarkerStep / (m_MainValueMax - m_MainValueMin);
      // don't draw last value, it's already done as first
      if (m_AngleRange == 360) diff_angle -= abm;

      int offset = 0;
      for(double angle = m_AngleStart - ANGLE_OFFSET; angle <= diff_angle; angle += abm)
      {
            if (m_MarkerOption == DIAL_MARKER_REDGREEN)
            {
                  int a = int(angle + ANGLE_OFFSET) % 360;
                  if (a > 180)
                        GetGlobalColor(_T("DASHR"), &cl);
                  else if ((a > 0) && (a < 180))
                        GetGlobalColor(_T("DASHG"), &cl);
                  else
                        GetGlobalColor(_T("DASHF"), &cl);

                  pen.SetColour(cl);
                  dc->SetPen(pen);
            }

            double size = 0.92;
            if(offset % m_MarkerOffset)
            {
                  size = 0.96;
            }
            offset++;

            dc->DrawLine(m_cx + (m_radius * size * cos(deg2rad(angle))),
                        m_cy + (m_radius * size * sin(deg2rad(angle))),
                        m_cx + (m_radius * cos(deg2rad(angle))),
                        m_cy + (m_radius * sin(deg2rad(angle))));
      }
      // We must reset pen color so following drawings are fine
      if (m_MarkerOption == DIAL_MARKER_REDGREEN)
      {
            GetGlobalColor(_T("DASHF"), &cl);
            pen.SetStyle(wxSOLID);
            pen.SetColour(cl);
            dc->SetPen(pen);
      }
}

void DashboardInstrument_Dial::DrawLabels(wxGCDC* dc)
{
      if (m_LabelOption == DIAL_LABEL_NONE)
            return;

      wxPoint TextPoint;
      wxPen pen;

      dc->SetFont(*g_pFontSmall);

      wxColor cl;
      GetGlobalColor(_T("DASHF"), &cl);
      dc->SetTextForeground(cl);

      int diff_angle = m_AngleStart + m_AngleRange - ANGLE_OFFSET;
      // angle between markers
      double abm = m_AngleRange * m_LabelStep / (m_MainValueMax - m_MainValueMin);
      // don't draw last value, it's already done as first
      if (m_AngleRange == 360) diff_angle -= abm;

      int offset = 0;
      int value = m_MainValueMin;
      int width, height;
      for(double angle = m_AngleStart - ANGLE_OFFSET; angle <= diff_angle; angle += abm)
      {
            wxString label = (m_LabelArray.GetCount() ? m_LabelArray.Item(offset) : wxString::Format(_T("%d"), value));
            dc->GetTextExtent(label, &width, &height, 0, 0, g_pFontSmall);

            double halfW = width / 2;
            if (m_LabelOption == DIAL_LABEL_HORIZONTAL)
            {
                  double halfH = height / 2;
                  //double delta = sqrt(width*width+height*height);
                  double delta = sqrt(halfW*halfW+halfH*halfH);
                  TextPoint.x = m_cx + ((m_radius * 0.90) - delta) * cos(deg2rad(angle)) - halfW;
                  TextPoint.y = m_cy + ((m_radius * 0.90) - delta) * sin(deg2rad(angle)) - halfH;
                  dc->DrawText(label, TextPoint);
            }
            else if (m_LabelOption == DIAL_LABEL_ROTATED)
            {
                  // The coordinates of dc->DrawRotatedText refer to the top-left corner
                  // of the rectangle bounding the string. So we must calculate the
                  // right coordinates depending of the angle.
                  // Move left from the Marker so that the position is in the Middle of Text
                  long double tmpangle = angle - rad2deg(asin(halfW / (0.90 * m_radius)));
                  TextPoint.x = m_cx + m_radius * 0.90 * cos(deg2rad(tmpangle));
                  TextPoint.y = m_cy + m_radius * 0.90 * sin(deg2rad(tmpangle));

                  dc->DrawRotatedText(label, TextPoint, -90 - angle);
            }
            offset++;
            value += m_LabelStep;
      }
}

void DashboardInstrument_Dial::DrawBackground(wxGCDC* dc)
{
      // Nothing to do here right now, will be overwritten
      // by child classes if required
}

void DashboardInstrument_Dial::DrawData(wxGCDC* dc, double value,
            wxString format, DialPositionOption position)
{
      if (position == DIAL_POSITION_NONE)
            return;

      dc->SetFont(*g_pFontLabel);
      wxColour cl;
      GetGlobalColor(_T("DASHF"), &cl);
      dc->SetTextForeground(cl);

      wxRect rect = GetClientRect();

      wxString text;
      if(!wxIsNaN(value))
           text = wxString::Format(format, value);
      else
           text = _T("---");

      int width, height;
      dc->GetMultiLineTextExtent(text, &width, &height, NULL, g_pFontLabel);
      wxRect TextPoint;
      TextPoint.width = width;
      TextPoint.height = height;
      switch (position)
      {
            case DIAL_POSITION_NONE:
                  // This case was already handled before, it's here just
                  // to avoid compiler warning.
                  return;
            case DIAL_POSITION_INSIDE:
            {
                  TextPoint.x = m_cx - (width / 2) - 1;
                  TextPoint.y = (rect.height * .75) - height;
                  wxPen pen;
                  pen.SetStyle(wxSOLID);
                  GetGlobalColor(_T("DASHL"), &cl);
                  pen.SetColour(cl);
                  pen.SetWidth(1);
                  dc->SetPen(pen);
                  GetGlobalColor(_T("DASHB"), &cl);
                  dc->SetBrush(cl);
                  // There might be a background drawn below
                  // so we must clear it first.
                  dc->DrawRoundedRectangle(TextPoint.x-2, TextPoint.y-2, width+4, height+4, 3);
                  break;
            }
            case DIAL_POSITION_TOPLEFT:
                  TextPoint.x = 0;
                  TextPoint.y = m_TitleHeight;
                  break;
            case DIAL_POSITION_TOPRIGHT:
                  TextPoint.x = rect.width-width-1;
                  TextPoint.y = m_TitleHeight;
                  break;
            case DIAL_POSITION_BOTTOMLEFT:
                  TextPoint.x = 0;
                  TextPoint.y = rect.height-height;
                  break;
            case DIAL_POSITION_BOTTOMRIGHT:
                  TextPoint.x = rect.width-width-1;
                  TextPoint.y = rect.height-height;
                  break;
      }

      dc->DrawLabel(text, TextPoint);
}

void DashboardInstrument_Dial::DrawForeground(wxGCDC* dc)
{
      // The default foreground is the arrow used in most dials
      wxColour cl;
      GetGlobalColor(_T("GREY1"), &cl);
      wxPen pen1;
      pen1.SetStyle(wxSOLID);
      pen1.SetColour(cl);
      pen1.SetWidth(2);
      dc->SetPen(pen1);
      GetGlobalColor(_T("GREY2"), &cl);
      wxBrush brush1;
      brush1.SetStyle(wxSOLID);
      brush1.SetColour(cl);
      dc->SetBrush(brush1);
      dc->DrawCircle(m_cx, m_cy, m_radius / 8);

      dc->SetPen(*wxTRANSPARENT_PEN);

      GetGlobalColor(_T("DASHN"), &cl);
      wxBrush brush;
      brush.SetStyle(wxSOLID);
      brush.SetColour(cl);
      dc->SetBrush(brush);

      // The arrow should stay inside fixed limits
      double val;
      if (m_MainValue < m_MainValueMin) val = m_MainValueMin;
      else if (m_MainValue > m_MainValueMax) val = m_MainValueMax;
      else val = m_MainValue;

      double value = deg2rad((val - m_MainValueMin) * m_AngleRange / (m_MainValueMax - m_MainValueMin)) + deg2rad(m_AngleStart - ANGLE_OFFSET);

      wxPoint points[4];
      points[0].x = m_cx + (m_radius * 0.95 * cos(value - .010));
      points[0].y = m_cy + (m_radius * 0.95 * sin(value - .010));
      points[1].x = m_cx + (m_radius * 0.95 * cos(value + .015));
      points[1].y = m_cy + (m_radius * 0.95 * sin(value + .015));
      points[2].x = m_cx + (m_radius * 0.22 * cos(value + 2.8));
      points[2].y = m_cy + (m_radius * 0.22 * sin(value + 2.8));
      points[3].x = m_cx + (m_radius * 0.22 * cos(value - 2.8));
      points[3].y = m_cy + (m_radius * 0.22 * sin(value - 2.8));
      dc->DrawPolygon(4, points, 0, 0);
}

/* Shared functions */
void DrawCompassRose(wxGCDC* dc, int cx, int cy, int radius, int startangle, bool showlabels)
{
      wxPoint TextPoint, points[3];
      wxString Value;
      int width, height;
      wxString CompassArray[] = {_("N"),_("NE"),_("E"),_("SE"),_("S"),_("SW"),_("W"),_("NW"),_("N")};

      dc->SetFont(*g_pFontSmall);

      wxColour cl;
      wxPen* pen;
      GetGlobalColor(_T("DASH2"), &cl);
      pen = wxThePenList->FindOrCreatePen( cl, 1, wxSOLID );
      wxBrush* b2 = wxTheBrushList->FindOrCreateBrush( cl );

      GetGlobalColor(_T("DASH1"), &cl);
      wxBrush* b1 = wxTheBrushList->FindOrCreateBrush( cl );

      dc->SetPen(*pen);
      dc->SetTextForeground(cl);

      int offset = 0;
      for(double tmpangle = startangle - ANGLE_OFFSET;
                        tmpangle <= startangle + 360 - ANGLE_OFFSET; tmpangle+=45)
      {
            if (showlabels)
            {
                Value = CompassArray[offset];
                dc->GetTextExtent(Value, &width, &height, 0, 0, g_pFontSmall);
                double x = width/2;
                long double anglefortext = asin((x/radius));
                anglefortext = tmpangle - rad2deg(anglefortext);
                TextPoint.x = cx + radius * cos(deg2rad(anglefortext));
                TextPoint.y = cy + radius * sin(deg2rad(anglefortext));
                dc->DrawRotatedText(Value, TextPoint.x,
                                                    TextPoint.y, -90 - tmpangle);
            }
            dc->SetBrush(*b1);
            points[0].x = cx;
            points[0].y = cy;
            points[1].x = cx + radius * 0.1 * cos(deg2rad(tmpangle-45));
            points[1].y = cy + radius * 0.1 * sin(deg2rad(tmpangle-45));
            double size = (offset % 2 ? 0.50 : 0.80);
            points[2].x = cx + radius * size * cos(deg2rad(tmpangle));
            points[2].y = cy + radius * size * sin(deg2rad(tmpangle));
            dc->DrawPolygon(3, points, 0, 0);

            points[1].x = cx + radius * 0.1 * cos(deg2rad(tmpangle+45));
            points[1].y = cy + radius * 0.1 * sin(deg2rad(tmpangle+45));
            dc->SetBrush(*b2);
            dc->DrawPolygon(3, points, 0, 0);
            offset++;
      }
}

void DrawBoat( wxGCDC* dc, int cx, int cy, int radius )
{
    // Now draw the boat
    wxColour cl;
    GetGlobalColor(_T("DASH2"), &cl);
    wxPen* pen = wxThePenList->FindOrCreatePen( cl, 1, wxSOLID );
    dc->SetPen( *pen );
    GetGlobalColor(_T("DASH1"), &cl);
    dc->SetBrush(cl);
    wxPoint points[7];

/*
 *           0
 *          /\
 *         /  \
 *        /    \
 *     6 /      \ 1
 *      |        |
 *      |    X   |
 *    5 |        | 2
 *       \      /
 *        \__ _/
 *        4    3
 */
    points[0].x = cx;
    points[0].y = cy - radius * .60; // a little bit longer than compass rose
    points[1].x = cx + radius * .15;
    points[1].y = cy - radius * .08;
    points[2].x = cx + radius * .15;
    points[2].y = cy + radius * .12;
    points[3].x = cx + radius * .10;
    points[3].y = cy + radius * .40;
    points[4].x = cx - radius * .10;
    points[4].y = cy + radius * .40;
    points[5].x = cx - radius * .15;
    points[5].y = cy + radius * .12;
    points[6].x = cx - radius * .15;
    points[6].y = cy - radius * .08;

    dc->DrawPolygon(7, points, 0, 0);
}

