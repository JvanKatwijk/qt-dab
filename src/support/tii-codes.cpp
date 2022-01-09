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

		tiiHandler::tiiHandler	(const QString &tiiDir) {
	this	-> tiiDir	= tiiDir;
	if (!this -> tiiDir. endsWith ('/'))
	   this -> tiiDir. append ('/');
	this -> tiiDir += "tii-codes/";
}

		tiiHandler::~tiiHandler	() {}
//
QString	makeName (uint8_t x, uint8_t y) {
QString res;
uint8_t b [4];
	b [0]	= (x & 0xF0) >> 4;
	b [1]	= (x & 0x0F);
	b [2]	= (y & 0xF0) >> 4;
	b [3]	= (y & 0x0F);

	for (int i = 0; i < 4; i ++)	
	   if (b [i] < 10) 
	       res. append ((char)('0' + b [i]));
	   else
	      res. append ((char)('A' + b [i] - 10));
	return res;
}
//
//	Germany is particular
QString	tiiHandler::tiiFile	(uint8_t eccByte, uint16_t Eid) {
	if ((eccByte == 0xE0) && (Eid == 0x01))
	   Eid = 0x0D;
QString	fileName	= tiiDir + makeName (eccByte, Eid >> 12);;

	fileName.  append (".tii");
	fileName        = QDir::toNativeSeparators (fileName);

	fprintf (stderr, "looking for %s (%x %x)\n", 
	                       fileName. toLatin1 (). data (), eccByte, Eid);
	FILE	*file = fopen (fileName. toLatin1 (). data (), "r");
	fprintf (stderr, "file %s can %s be opened\n",
	                 fileName. toLatin1 (). data (),
	                 file == nullptr ? "not" : "good");
	if (file == nullptr)
	   return "";
	fclose (file);
	return fileName;
}
	
	QString tiiHandler::
	        get_transmitterName (const QString &fileName, uint16_t Eid,
                                     uint8_t mainId, uint8_t subId) {

	for (int i = 0; i < (int)(cache. size ()); i ++) {
	   if ((cache [i]. Eid == Eid) && (cache [i]. mainId == mainId) &&
	       (cache [i]. subId == subId)) {
	      return cache [i]. transmitterName;
	   }
	}
	   

	FILE *file = fopen (fileName. toLatin1 (). data (), "r");
	fprintf (stderr, "Bij openen van %s gaat het %s\n",
	                fileName. toLatin1 (). data (), 
	                file == nullptr ? "fout" : "goed");
	if (file == nullptr)
	   return "";

	char buffer [512];
	while (fgets (buffer, 512, file) != nullptr) {
	   if (feof (file))
	      break;
	   bool test;
	   uint16_t theID;
	   uint16_t the_tii;
	   QString transmitterName;
	   QStringList S = QString (buffer). split ('\t');
	   if (S. size () < 6)
	      continue;
	   theID = S. at (0). toUInt (&test, 16);
	   if (!test)
	      continue;
	   if (theID != Eid)
	      continue;
	   the_tii = S. at (1). toUInt (&test, 10);
	   if (!test)
	      continue;
	   if (((the_tii / 100) == (mainId)) &&
               (the_tii % 100) == subId) {
	      QString latitude 	= S. at (4);
	      QString longitude  = S. at (5);
	      cacheElement temp;
	      temp. Eid		= Eid;
	      temp. mainId	= mainId;
	      temp. subId	= subId;
	      temp. transmitterName = S. at (2);
	      temp. latitude	= convert (latitude);
	      temp. longitude	= convert (longitude);
	      cache. push_back (temp);
	      fclose (file);
	      return S. at (2);
	   }
	}
	fclose (file);
	return "";
}

#define	isDigit(x) (('0' <= x) && (x <= '9'))

float	tiiHandler::convert (const QString &s) {
bool	sign	= false;
int	teller	= 0;

QString	degreeString;
QString	minuteString;
QString secondString;

int degrees, seconds, minutes;
bool	success;

	while (isDigit (s. toUtf8 (). data () [teller])) {
	   degreeString. append (s. toUtf8 (). data () [teller]);
	   teller ++;
	}

	if (s. toUtf8 (). data () [teller] == 0)
	   return 0;
	
	teller += 2;
	while (isDigit (s. toUtf8 (). data () [teller])) {
           minuteString. append (s. toUtf8 (). data () [teller]);
	   teller ++;
	}

	if (s. toUtf8 (). data () [teller] == 0)
	   return 0;

	teller ++;
	while (isDigit (s. toUtf8 (). data () [teller])) {
           secondString. append (s. toUtf8 (). data () [teller]);
	   teller ++;
	}

	if (s. toUtf8 (). data () [teller] == 0)
	   return 0;

	teller ++;
	char quadrant	= s. toUtf8 (). data () [teller];

	teller ++;
	
	degrees	= degreeString. toUInt (&success, 10);
	if (!success || (degrees >= 180))
	   return 0;
	minutes = minuteString. toUInt (&success, 10);
	if (!success || (minutes >= 60))
	   return 0;
	seconds	= secondString. toUInt (&success, 10);
	if (!success || (seconds >= 60))
	   return 0;

	switch (quadrant) {
	   case 'N':	break;
	   case 'E':	break;
	   case 'W':	sign = true;
	                break;
	   case 'S':	sign = true;
	                break;
	}

	float result = degrees + (float)minutes / 60 + (float) seconds / 3600;
	fprintf (stderr, "converting %s to %f\n", s. toUtf8 (). data (),
	                                  result);
	return sign ? - result : result;
}

void	tiiHandler::get_coordinates (float *latitude, float * longitude,
	                              const QString &transmitter) {
	for (int i = 0; i < cache. size (); i++) {
	   if (cache [i]. transmitterName == transmitter) {
	      *latitude = cache [i]. latitude;
	      *longitude = cache [i]. longitude;
	      return;
	   }
	}
	*latitude	= 0;
	*longitude	= 0;
}

//
//	Great circle distance (https://towardsdatascience.com/calculating-the-distance-between-two-locations-using-geocodes-1136d810e517)
//
int	tiiHandler::distance (float latitude1, float longitude1,
	                      float latitude2, float longitude2) {
double	R	= 6371 * 1000;
double	Phi1	= latitude1 * M_PI / 180;
double	Phi2	= latitude2 * M_PI / 180;
double	dPhi	= (latitude2 - latitude1) * M_PI / 180;
double	dDelta	= (longitude2 - longitude1) * M_PI / 180;

float	a = sin (dPhi / 2) * sin (dPhi / 2) + cos (Phi1) * cos (Phi2) *
	     sin (dDelta / 2) * sin (dDelta / 2);
float	c = 2 * atan2 (sqrt (a), sqrt (1 - a));

	return (int)(R * c / 1000 + 0.5);
}



