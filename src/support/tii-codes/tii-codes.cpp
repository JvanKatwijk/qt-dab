#
/*
 *    Copyright (C) 2014 .. 2021
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
#include	"tii-codes.h"
#include	"ITU_Region_1.h"
#include	<QSettings>
#include	<QMessageBox>

#define	SEPARATOR	';'
#define	COUNTRY		1
#define	CHANNEL		2
#define	LABEL		3
#define	EID		4
#define	TII		5
#define	LOCATION	6
#define	LATITUDE	7
#define	LONGITUDE	8
#define	NR_COLUMNS	10

#ifdef  __LOAD_TABLES__
#include        <curl/curl.h>
#endif
		tiiHandler::tiiHandler	() {
}
		tiiHandler::~tiiHandler	() {
}
//
bool	tiiHandler::tiiFile	(const QString &s) {
	return false;
}
	
	QString tiiHandler::
	        get_transmitterName (const QString &channel,
	                             const QString &country, uint16_t Eid,
                                     uint8_t mainId, uint8_t subId) {
	return "";
}

void	tiiHandler::get_coordinates (float *latitude, float * longitude,
	                             const QString &channel,
	                             const QString &transmitter) {
	*latitude	= 0;
	*longitude	= 0;
}

float	tiiHandler::convert (const QString &s) {
bool	flag;
float	v;
	v = s. trimmed (). toFloat (&flag);
	if (!flag)
	   v = 0;
	return v;
}

uint16_t tiiHandler::get_Eid (const QString &s) {
bool	flag;
uint16_t res;
	res = s. trimmed (). toInt (&flag, 16);
	if (!flag)
	   res = 0;
	return res;
}

uint8_t	tiiHandler::get_mainId (const QString &s) {
bool flag;
uint16_t res;
	res = s. trimmed (). toInt (&flag);
	if (!flag)
	   res = 0;
	return res / 100;
}

uint8_t tiiHandler::get_subId (const QString &s) {
bool flag;
uint16_t res;
	res = s. trimmed (). toInt (&flag);
	if (!flag)
	   res = 0;
	return res % 100;
}

int	tiiHandler::readColumns (std::vector<QString> &v, char *b, int N) {
	return 0;
}

//
//	Great circle distance (https://towardsdatascience.com/calculating-the-distance-between-two-locations-using-geocodes-1136d810e517)
//	en
//	https://www.movable-type.co.uk/scripts/latlong.html
//	Haversine formula applied
int	tiiHandler::distance (float latitude1, float longitude1,
	                      float latitude2, float longitude2) {
double	R	= 6371;
double	Phi1	= latitude1 * M_PI / 180;
double	Phi2	= latitude2 * M_PI / 180;
double	dPhi	= (latitude2 - latitude1) * M_PI / 180;
double	dDelta	= (longitude2 - longitude1) * M_PI / 180;

//double	a	= sin (dPhi / 2) * sin (dPhi / 2) + cos (Phi1) * cos (Phi2) *
//	          sin (dDelta / 2) * sin (dDelta / 2);
//double	c	= 2 * atan2 (sqrt (a), sqrt (1 - a));

double 	x	= dDelta * cos ((Phi1 + Phi2) / 2);
double	y	= (Phi2 - Phi1);
double	d	= sqrt (x * x + y * y);
//
//	return (int)(R * c + 0.5);
	return (int)(R * d + 0.5);
}

int	tiiHandler::corner (float latitude1, float longitude1,
	                    float latitude2, float longitude2) {
bool dx_sign	= longitude1 - longitude2 > 0;
bool dy_sign	= latitude1  - latitude2 > 0;
double dy	= distance (latitude1, longitude2,	
	                    latitude2, longitude2);
double dx;
//	if (dy_sign)
	   dx = distance (latitude2, longitude1,
	                  latitude2, longitude2);
//	else
//	   dx = distance (latitude1, longitude1,
//	                  latitude1, longitude2);
double	dz	= distance (latitude1, longitude1,
	                    latitude2, longitude2);
float azimuth = atan2 (dy, dx);
float azimuth_1	= asin (dy / dz);
float azimuth_2	= acos (dx / dz);

	if (longitude1 == longitude2) {
	   if (latitude1 < latitude2) 
	      return 360;
	   else
	      return 0;
	}

	if (dx > dy)
	   azimuth = azimuth_1;
	if (dy > dx)
	   azimuth = azimuth_2;
	   
	if (dx_sign && dy_sign)		// eerste kwadrant
	   return (int)((M_PI / 2 - azimuth) / M_PI * 180);
	if (dx_sign && !dy_sign)	// tweede kwadrant
	   return (int)((M_PI / 2 + azimuth) / M_PI * 180);
	if (!dx_sign && !dy_sign)	// derde kwadrant
	   return (int)((3 * M_PI / 2 - azimuth) / M_PI * 180);
	return (int)((3 * M_PI / 2 + azimuth) / M_PI * 180);
}

void	tiiHandler::readFile (FILE *f) {
}

void	tiiHandler::set_black (uint16_t Eid, uint8_t mainId, uint8_t subId) {
black element;
	element. Eid = Eid;
	element. mainId	= mainId;
	element. subId	= subId;
	blackList. push_back (element);
}

bool	tiiHandler::is_black (uint16_t Eid, uint8_t mainId, uint8_t subId) {

	for (int i = 0; i < blackList. size (); i ++)
	   if ((blackList [i]. Eid == Eid) &&
	       (blackList [i]. mainId == mainId) &&
	       (blackList [i]. subId == subId))
	      return true;
	return false;
}

char	*tiiHandler::eread (char * buffer, int amount, FILE *f) {
	return buffer;
}

QString	tiiHandler::entry (const char *s) {
	return QString ("");
}

static
size_t  writeCallBack (void *contents, size_t size,
                                          size_t nmemb, void *userP) {
        ((std::string *)userP) -> append ((char *)contents, size * nmemb);
        return size * nmemb;
}

void	tiiHandler::loadTable (const QString &tf) {
	QMessageBox::warning (this, tr ("Warning"),
                             tr ("Not implemented in this version"));
}

bool	tiiHandler::valid	() {
	return false;;
}

