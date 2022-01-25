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
bool	res = false;
	if (s == "") {
	   return false;
	}
	blackList. resize (0);
	cache. resize (0);
	FILE	*f	= fopen (s. toLatin1 (). data (), "r");
	if (f != nullptr) {
	   res = true;
	   readFile (f);
	   fclose (f);
	}
	return res;
}
	
	QString tiiHandler::
	        get_transmitterName (const QString &channel,
	                             const QString &country, uint16_t Eid,
                                     uint8_t mainId, uint8_t subId) {
	for (int i = 0; i < (int)(cache. size ()); i ++) {
	   if (((channel == "any") || (channel == cache [i]. channel)) &&
	       (cache [i]. Eid == Eid) && (cache [i]. mainId == mainId) &&
	       (cache [i]. subId == subId)) {
	      return cache [i]. transmitterName;
	   }
	}
	return "";
}

void	tiiHandler::get_coordinates (float *latitude, float * longitude,
	                             const QString &channel,
	                             const QString &transmitter) {
	for (int i = 0; i < cache. size (); i++) {
	   if (((channel == "any") || (channel == cache [i]. channel)) &&
	       (cache [i]. transmitterName == transmitter)) {
	      *latitude = cache [i]. latitude;
	      *longitude = cache [i]. longitude;
	      return;
	   }
	}
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
//double	dz	= distance (latitude1, longitude1,
//	                    latitude2, longitude2);
float azimuth = atan2 (dy, dx);
//float azimuth_1	= asin (dy / dz);
//float azimuth_2	= acos (dx / dz);

	if (longitude1 == longitude2) {
	   if (latitude1 < latitude2) 
	      return 360;
	   else
	      return 0;
	}

	if (dx_sign && dy_sign)		// eerste kwadrant
	   return (int)((M_PI / 2 - azimuth) / M_PI * 180);
	if (dx_sign && !dy_sign)	// tweede kwadrant
	   return (int)((M_PI / 2 + azimuth) / M_PI * 180);
	if (!dx_sign && !dy_sign)	// derde kwadrant
	   return (int)((3 * M_PI / 2 - azimuth) / M_PI * 180);
	return (int)((3 * M_PI / 2 + azimuth) / M_PI * 180);
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
	   if (count >= cache. size ())
	      cache. resize (cache. size () + 500);
	   cache. at (count) = ed;
	   count ++;
	}
	cache. resize (count);
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
char	*bufferP;
	if (fgets (buffer, amount, f) == nullptr)
	   return nullptr;
	bufferP	= buffer;
	while (*bufferP != 0) {
	   *bufferP -= shift;
	   bufferP ++;
	}
	*bufferP = 0;
	return buffer;
}

static 
uint8_t entryLabel [] = {
0x69, 0x75, 0x75, 0x71, 0x74, 0x3b, 0x30, 0x30, 0x78, 0x78, 0x78, 0x2f, 0x65, 0x62, 0x63, 0x6d, 0x6a, 0x74, 0x75, 0x2f, 0x70, 0x73, 0x68, 0x30, 0x72, 0x75, 0x65, 0x62, 0x63, 0x30, 0x72, 0x75, 0x65, 0x62, 0x63, 0x60, 0x65, 0x62, 0x63, 0x75, 0x79, 0x60, 0x65, 0x62, 0x75, 0x62, 0x2f, 0x71, 0x69, 0x71, 0x0};

QString	tiiHandler::entry (const char *s) {
char test [120];
int	i;
	for (i = 0; entryLabel [i] != 0; i ++)
	   test[i] = entryLabel [i] - 1;
	test [i] = 0;
	fprintf (stderr, "%s\n", test);
	return QString (test);
}


#ifdef	__LOAD_TABLES__
static
size_t  writeCallBack (void *contents, size_t size,
                                          size_t nmemb, void *userP) {
        ((std::string *)userP) -> append ((char *)contents, size * nmemb);
        return size * nmemb;
}

void	tiiHandler::loadTable (const QString &tf) {
CURL	*curl;
CURLcode	res;
std::string readBuffer;

	curl_global_init (CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (curl) {
	   curl_f	= curl_fopen ();
	   curl_easy_setopt (curl, CURLOPT_URL, 
	                     entry ("jan"). toLatin1 (). data ());
	   curl_easy_setopt (curl, CURLOPT_SSL_VERIFYPEER, 0L);
	   curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION,  writeCallBack);
	   curl_easy_setopt (curl, CURLOPT_WRITEDATA, &readBuffer);
/*	   Perform the request, res will get the return code */
	   res = curl_easy_perform (curl);
/*	   Check for errors */
	   if (res != CURLE_OK)
	      fprintf (stderr, "curl_easy_perform() failed: %s\n",
                                              curl_easy_strerror(res));
/*	   always cleanup */
	   curl_easy_cleanup (curl);
	   curl_fclose (curl_f);
	}
	curl_global_cleanup();

	FILE *outp	= fopen (tf. toLatin1 (). data (), "w + b");
	if (outp == nullptr)
	   return;
#define	SHIFT 6

	fputc (SHIFT, outp);
	for (int i = 0; i < readBuffer. size (); i ++) {
	   if (readBuffer [i] == '\n')
	      fputc ('\n', outp);
	   else
	      fputc (readBuffer [i] + SHIFT, outp);
	}
	fclose (outp);
	return;
}

bool	tiiHandler::valid	() {
	return true;
}
#else
void	tiiHandler::loadTable	(const QString &ff) {
	(void)ff;
	fprintf (stderr, "No name support in this version\n");
}

bool	tiiHandler::valid	() {
	return false;
}

#endif
