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
	(void)s;
	return false;
}
	
	QString tiiHandler::
	        get_transmitterName (const QString &channel,
	                             const QString &country, uint16_t Eid,
                                     uint8_t mainId, uint8_t subId) {
	(void)channel; (void)country;
	(void)Eid; (void)mainId; (void)subId;
	return "";
}

void	tiiHandler::get_coordinates (float *latitude, float * longitude,
	                             const QString &channel,
	                             const QString &transmitter) {
	(void)channel; (void)transmitter;
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
	(void)v; (void)b; (void)N;
	return 0;
}

//
//	Great circle distance (https://towardsdatascience.com/calculating-the-distance-between-two-locations-using-geocodes-1136d810e517)
//	en
//	https://www.movable-type.co.uk/scripts/latlong.html
//	Haversine formula applied
int	tiiHandler::distance (float latitude, float longitude,
	                      float homeLatitude, float homeLongitude) {
	(void)latitude; (void)longitude;
	(void)homeLatitude; (void)homeLongitude;
	return 0;
}

int	tiiHandler::corner (float latitude, float longitude,
	                    float homeLatitude, float homeLongitude) {
	(void)latitude; (void)longitude;
	(void)homeLatitude; (void)homeLongitude;
	return 0;
}

void	tiiHandler::readFile (FILE *f) {
	(void)f;
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
	(void)buffer; (void)amount; (void)f;
	return buffer;
}

QString	tiiHandler::entry (const char *s) {
	(void)s;
	return QString ("");
}

static
size_t  writeCallBack (void *contents, size_t size,
                                          size_t nmemb, void *userP) {
        ((std::string *)userP) -> append ((char *)contents, size * nmemb);
        return size * nmemb;
}

void	tiiHandler::loadTable (const QString &tf) {
	(void)tf;
	QMessageBox::warning (nullptr, "Warning",
                              "Not supported in this version");
}

bool	tiiHandler::valid	() {
	return false;;
}

