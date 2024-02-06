#
/*
 *    Copyright (C) 2014 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	<stdio.h>
#include	<QDir>
#include	<QString>
#include	<QStringList>
#include	<math.h>
#include	"dab-constants.h"
#include	"tii-mapper.h"
#include	"ITU_Region_1.h"
#include	<QSettings>

		tiiMapper::tiiMapper	():
	                                   theReader () {
}
		tiiMapper::~tiiMapper	() {
}
//
bool	tiiMapper::tiiFile	(const QString &s) {
theCache	= theReader. readFile (s);
	return theCache. size () > 10;
	   
}

bool	tiiMapper::loadTable	(const QString &s) {
	return theReader. loadTable (s);
}

QString tiiMapper::
	        get_transmitterName (const QString &channel,
	                             uint16_t Eid,
                                     uint8_t mainId, uint8_t subId) {
	for (int i = 0; i < (int)(theCache. size ()); i ++) {
	   if (((channel == "any") || (channel == theCache [i]. channel)) &&
	       (theCache [i]. Eid == Eid) && (theCache [i]. mainId == mainId) &&
	       (theCache [i]. subId == subId)) {
	      return theCache [i]. transmitterName;
	   }
	}
	return "";
}

void	tiiMapper::get_coordinates (position	&thePos,
	                             float	&power,
	                             const QString &channel,
	                             const QString &transmitter) {
	for (int i = 0; i < (int)(theCache. size ()); i++) {
	   if (((channel == "any") || (channel == theCache [i]. channel)) &&
	       (theCache [i]. transmitterName == transmitter)) {
	      thePos.  latitude = theCache [i]. latitude;
	      thePos.  longitude = theCache [i]. longitude;
	      power	= theCache [i]. power;
	      return;
	   }
	}
	thePos. latitude	= 0;
	thePos. longitude	= 0;
}

void	tiiMapper::get_coordinates (float *latitude, float * longitude,
	                             float *power,
	                             const QString &channel,
	                             const QString &transmitter) {
	for (int i = 0; i < (int)(theCache. size ()); i++) {
	   if (((channel == "any") || (channel == theCache [i]. channel)) &&
	       (theCache [i]. transmitterName == transmitter)) {
	      *latitude = theCache [i]. latitude;
	      *longitude = theCache [i]. longitude;
	      *power	= theCache [i]. power;
	      return;
	   }
	}
	*latitude	= 0;
	*longitude	= 0;
}
//
//	Great circle distance
//	(https://towardsdatascience.com/calculating-the-distance-between-two-locations-using-geocodes-1136d810e517)
//	en
//	https://www.movable-type.co.uk/scripts/latlong.html
//	Haversine formula applied
float	tiiMapper::distance_2 (float latitude1, float longitude1,
	                       float latitude2, float longitude2) {
double	R	= 6371;
double	Phi1	= latitude1 * M_PI / 180;
double	Phi2	= latitude2 * M_PI / 180;
double	dDelta	= (longitude2 - longitude1) * M_PI / 180;

	if ((latitude2 == 0) || (longitude2 == 0))
	   return -32768;

double 	x	= dDelta * cos ((Phi1 + Phi2) / 2);
double	y	= (Phi2 - Phi1);
double	d	= sqrt (x * x + y * y);
//
	return R * d + 0.5;
}
//
//	The distances on the y-axis are usually correct
float	tiiMapper::distance (position target,
	                      position home) {
bool	dy_sign	= target. latitude >  home. latitude;
double	dx;
double dy	= distance_2 (target. latitude,  home. longitude,	
	                      home. latitude, home. longitude);
	if (dy_sign)		// lat1 is "higher" than lat2
	   dx = distance_2 (target. latitude, target. longitude,
	                    target. latitude, home. longitude);
	else
	   dx = distance_2 (target. latitude, home. longitude,
	                    target. latitude, target. longitude);
	return sqrt (dx * dx + dy * dy);
}
	
float	tiiMapper::distance (float latitude1, float longitude1,
	                      float latitude2, float longitude2) {
bool	dy_sign	= latitude1 > latitude2;
double	dx;
double dy	= distance_2 (latitude1, longitude2,	
	                      latitude2, longitude2);
	if (dy_sign)		// lat1 is "higher" than lat2
	   dx = distance_2 (latitude1, longitude1,
	                    latitude1, longitude2);
	else
	   dx = distance_2 (latitude2, longitude1,
	                    latitude2, longitude2);
	return sqrt (dx * dx + dy * dy);
}
	
float	tiiMapper::corner (position targetPos, position homePos) {
bool dx_sign	= targetPos. longitude - homePos. longitude > 0;
bool dy_sign	= targetPos. latitude  - homePos. latitude > 0;
double dx;
double dy	= distance (homePos. latitude,  targetPos. longitude,	
	                    targetPos. latitude,  targetPos. longitude);
	if (dy_sign)		// lat1 is "higher" than lat2
	   dx = distance (homePos. latitude, homePos. longitude,
	                  homePos. latitude,  targetPos. longitude);
	else
	   dx = distance (targetPos. latitude,  homePos. longitude,
	                  targetPos. latitude,  targetPos. longitude);
	float azimuth = atan2 (dy, dx);
	   
	if (dx_sign && dy_sign)		// eerste kwadrant
	   return (M_PI / 2 - azimuth) / M_PI * 180;
	if (dx_sign && !dy_sign)	// tweede kwadrant
	   return (M_PI / 2 + azimuth) / M_PI * 180;
	if (!dx_sign && !dy_sign)	// derde kwadrant
	   return (3 * M_PI / 2 - azimuth) / M_PI * 180;
	return (3 * M_PI / 2 + azimuth) / M_PI * 180;
}

void	tiiMapper::set_black (uint16_t Eid, uint8_t mainId, uint8_t subId) {
black element;
	element. Eid = Eid;
	element. mainId	= mainId;
	element. subId	= subId;
	blackList. push_back (element);
}

bool	tiiMapper::is_black (uint16_t Eid, uint8_t mainId, uint8_t subId) {

	for (int i = 0; i < (int)(blackList. size ()); i ++)
	   if ((blackList [i]. Eid == Eid) &&
	       (blackList [i]. mainId == mainId) &&
	       (blackList [i]. subId == subId))
	      return true;
	return false;
}

