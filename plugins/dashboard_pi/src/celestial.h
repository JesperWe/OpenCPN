/******************************************************************************
 * $Id: celestial.h, v1.0 2010/08/30 SethDart Exp $
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

#ifndef _CELESTIAL_H_
#define _CELESTIAL_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "instrument.h"

// Zeniths for sunset/sunrise calculation
#define ZENITH_OFFICIAL (90.0 + 50.0 / 60.0)
#define ZENITH_CIVIL 96.0
#define ZENITH_NAUTICAL 102.0
#define ZENITH_ASTRONOMICAL 108.0

class DashboardInstrument_Sun : public DashboardInstrument_Position
{
public:
      DashboardInstrument_Sun(wxWindow *pparent, wxWindowID id, int cap_flag1=OCPN_DBP_STC_LAT, int cap_flag2 = OCPN_DBP_STC_LON, int cap_flag= OCPN_DBP_STC_CLK) : DashboardInstrument_Position(pparent, id, cap_flag1, cap_flag2) { m_lat = m_lon = 999.9; m_dt = wxDateTime::Now().ToUTC(); m_cap_flag = m_cap_flag | cap_flag; }
      ~DashboardInstrument_Sun(){}

      void SetData(int st, double data, wxString unit);
      void SetUtcTime(int st, wxDateTime value);

protected:
      double m_lat;
      double m_lon;
      wxDateTime m_dt;

private:
      void calculateSun(double latit, double longit, wxDateTime &sunrise, wxDateTime &sunset);
};

#endif
