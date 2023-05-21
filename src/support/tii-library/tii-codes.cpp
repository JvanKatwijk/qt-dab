#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation recorder 2 of the License.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	<stdio.h>
#include        <QDir>
#include        <QString>
#include        <QStringList>
#include        <math.h>
#include        "dab-constants.h"
#include        "tii-codes.h"
#include        "ITU_Region_1.h"
#include        <QSettings>
#include        <QMessageBox>

#define	SEPARATOR	';'
#define	COUNTRY		1
#define	CHANNEL		2
#define	LABEL		3
#define	EID		4
#define	TII		5
#define	LOCATION	6
#define	LATITUDE	7
#define	LONGITUDE	8
#define	POWER		13
#define	NR_COLUMNS	14

#ifdef __MINGW32__
#define GETPROCADDRESS  GetProcAddress
#else
#define GETPROCADDRESS  dlsym
#endif

	tiiHandler::tiiHandler	() {
	Handle		= dlopen ("libtii-lib.so", RTLD_NOW | RTLD_GLOBAL);
	fprintf (stderr, "%s\n", dlerror ());
	if (Handle == nullptr)
	   Handle	= dlopen ("/usr/local/lib/libtii-lib.so",
	                                           RTLD_NOW | RTLD_GLOBAL);
	if (Handle == nullptr)
	   Handle	= dlopen ("/usr/local/lib/tii-lib.so",
	                                           RTLD_NOW | RTLD_GLOBAL);
	fprintf (stderr, "%s\n", dlerror ());
	if (Handle == nullptr)
	   fprintf (stderr, "Library not loaded\n");
//	set the defaults
	init_tii_L	= nullptr;
	close_tii_L	= nullptr;
	loadTable_L	= nullptr;
	loadFunctions	();
	if (init_tii_L != nullptr)
	   handler	= init_tii_L ();
	else
	   handler	= nullptr;
}

	tiiHandler::~tiiHandler	() { 
	if (close_tii_L != nullptr)
	   close_tii_L (handler);
}

bool	tiiHandler::tiiFile         (const QString &s) {
bool	res = false;
	if (s == "") {
	   return false;
	}
	blackList. resize (0);
	cache. resize (0);
	FILE	*f	= fopen (s. toUtf8 (). data (), "r+b");
	if (f != nullptr) {
	   fprintf (stderr, "tiiFile is %s\n", s. toUtf8 (). data ());
	   res = true;
	   readFile (f);
	   fclose (f);
	}
	return res;
}

QString	tiiHandler::get_transmitterName     (const QString & channel,
	                                     uint16_t Eid,
	                                     uint8_t mainId, uint8_t subId) {
//	fprintf (stderr, "looking for %s %X %d %d\n",
//	                           channel. toLatin1 (). data (),
//	                           Eid, mainId, subId);
	for (int i = 0; i < (int)(cache. size ()); i ++) {
	   if (((channel == "any") || (channel == cache [i]. channel)) &&
	       (cache [i]. Eid == Eid) && (cache [i]. mainId == mainId) &&
	       (cache [i]. subId == subId)) {
	      return cache [i]. transmitterName;
	   }
	}
	return "";
}

void    tiiHandler::get_coordinates (float *latitude, float * longitude,
	                             float *power,
	                             const QString &channel,
	                             const QString &transmitter) {
	for (int i = 0; i < (int)(cache. size ()); i++) {
	   if (((channel == "any") || (channel == cache [i]. channel)) &&
	       (cache [i]. transmitterName == transmitter)) {
	      *latitude = cache [i]. latitude;
	      *longitude = cache [i]. longitude;
	      *power	= cache [i]. power;
	      return;
	   }
	}
	*latitude       = 0;
	*longitude      = 0;
}
//
//	Great circle distance (https://towardsdatascience.com/calculating-the-distance-between-two-locations-using-geocodes-1136d810e517)
//	en
//	https://www.movable-type.co.uk/scripts/latlong.html
//	Haversine formula applied
int	tiiHandler::distance_2 (float latitude1, float longitude1,
	                        float latitude2, float longitude2) {
double	R	= 6371;
double	Phi1	= latitude1 * M_PI / 180;
double	Phi2	= latitude2 * M_PI / 180;
double	dPhi	= (latitude2 - latitude1) * M_PI / 180;
double	dDelta	= (longitude2 - longitude1) * M_PI / 180;

	if ((latitude2 == 0) || (longitude2 == 0))
	   return -32768;
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

int	tiiHandler::distance (float latitude1, float longitude1,
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
	
int	tiiHandler::corner (float latitude1, float longitude1,
	                    float latitude2, float longitude2) {
bool dx_sign	= longitude1 - longitude2 > 0;
bool dy_sign	= latitude1  - latitude2 > 0;
double dx;
double dy	= distance (latitude1, longitude2,	
	                    latitude2, longitude2);
	if (dy_sign)		// lat1 is "higher" than lat2
	   dx = distance (latitude1, longitude1,
	                  latitude1, longitude2);
	else
	   dx = distance (latitude2, longitude1,
	                  latitude2, longitude2);
	float azimuth = atan2 (dy, dx);
	   
	if (dx_sign && dy_sign)		// eerste kwadrant
	   return (int)((M_PI / 2 - azimuth) / M_PI * 180);
	if (dx_sign && !dy_sign)	// tweede kwadrant
	   return (int)((M_PI / 2 + azimuth) / M_PI * 180);
	if (!dx_sign && !dy_sign)	// derde kwadrant
	   return (int)((3 * M_PI / 2 - azimuth) / M_PI * 180);
	return (int)((3 * M_PI / 2 + azimuth) / M_PI * 180);
}

bool	tiiHandler::is_black		(uint16_t Eid,
	                                 uint8_t mainId, uint8_t subId) {
	for (int i = 0; i < blackList. size (); i ++)
	   if ((blackList [i]. Eid == Eid) &&
	       (blackList [i]. mainId == mainId) &&
	       (blackList [i]. subId == subId))
	      return true;
	return false;
}

void	tiiHandler::set_black		(uint16_t Eid,
	                                 uint8_t mainId, uint8_t subId) {
black element;
	element. Eid = Eid;
	element. mainId = mainId;
	element. subId  = subId;
	blackList. push_back (element);
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

void	tiiHandler::readFile (FILE *f) {
int	count = 0; 
char	buffer [1024];
std::vector<QString> columnVector;

	this	-> shift	= fgetc (f);
	while (eread  (buffer, 1024, f) != nullptr) {
	   cacheElement ed;
	   if (feof (f))
	      break;
	   columnVector. resize (0);
	   int columns = readColumns (columnVector, buffer, NR_COLUMNS);
	   if (columns < NR_COLUMNS)
	      continue;
	   ed. country		= columnVector [COUNTRY]. trimmed  ();
	   ed. Eid		= get_Eid (columnVector [EID]);
	   ed. mainId		= get_mainId (columnVector [TII]);
	   ed. subId		= get_subId (columnVector [TII]);
	   ed. channel		= columnVector [CHANNEL]. trimmed ();
	   ed. ensemble 	= columnVector [LABEL]. trimmed ();
	   ed. transmitterName	= columnVector [LOCATION];
	   ed. latitude		= convert (columnVector [LATITUDE]);
	   ed. longitude	= convert (columnVector [LONGITUDE]);
	   ed. power		= convert (columnVector [POWER]);
	   if (count >= cache. size ())
	      cache. resize (cache. size () + 500);
	   cache. at (count) = ed;
	   count ++;
	}
	cache. resize (count);
}

int	tiiHandler::readColumns (std::vector<QString> &v, char *b, int N) {
int charp	= 0;
char	tb [256];
int elementCount = 0;
QString element;
	v. resize (0);
	while ((*b != 0) && (*b != '\n')) {
	   if (*b == SEPARATOR) {
	      tb [charp] = 0;
	      QString ss = QString::fromUtf8 (tb);
	      v. push_back (ss);
	      charp = 0;
	      elementCount ++;
	      if (elementCount >= N)
	         return N;
	   }
	   else
	      tb [charp ++] = *b;
	   b ++;
	}
	return elementCount;
}

char    *tiiHandler::eread (char * buffer, int amount, FILE *f) {
char    *bufferP;
	if (fgets (buffer, amount, f) == nullptr)
	   return nullptr;
	bufferP = buffer;
	while (*bufferP != 0) {
	   if (shift != 0xAA)
	      *bufferP -= shift;
	   else
	      *bufferP ^= 0xAA;
	   bufferP ++;
	}
	*bufferP = 0;
	return buffer;
}

bool	tiiHandler::valid		() {
	return handler != nullptr;
}

bool	tiiHandler::loadFunctions	() {
	init_tii_L	= (init_tii_P)
	                    GETPROCADDRESS (this -> Handle,
	                                    "init_tii_L");
	if (init_tii_L == nullptr)
	   fprintf (stderr, "init_tii_L not loaded\n");

	close_tii_L	= (close_tii_P)
	                    GETPROCADDRESS (this -> Handle,
	                                    "close_tii_L");
	loadTable_L	= (loadTable_P)
	                    GETPROCADDRESS (this -> Handle,
	                                    "loadTableL");
	return true;
}

void	tiiHandler::loadTable		(const QString &tf) {
	if (loadTable_L != nullptr)
	   loadTable_L (handler, tf. toStdString ());
}

