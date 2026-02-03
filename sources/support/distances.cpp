#
/*
 *    Copyright (C) 2020 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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

#include	"distances.h"
#include	"dab-constants.h"
//
//	Great circle distance
//	(https://towardsdatascience.com/calculating-the-distance-between-two-locations-using-geocodes-1136d810e517)
//	en
//	https://www.movable-type.co.uk/scripts/latlong.html
//	Haversine formula applied
float	distance_2 (float latitude1, float longitude1,
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
	return R * d + 0.5;
}
//
//	The distances on the y-axis are usually correct
float	distance (position target, position home) {
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

static
float	distance (float latitude1, float longitude1,
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

float	corner (position targetPos, position homePos) {
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
	   
	if (dx_sign && dy_sign)		// quadrant 1
	   return (M_PI / 2 - azimuth) / M_PI * 180;
	if (dx_sign && !dy_sign)	// quadrant 2
	   return (M_PI / 2 + azimuth) / M_PI * 180;
	if (!dx_sign && !dy_sign)	// quadrant 3
	   return (3 * M_PI / 2 - azimuth) / M_PI * 180;
	return (3 * M_PI / 2 + azimuth) / M_PI * 180;
}

