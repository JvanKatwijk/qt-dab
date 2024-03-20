#
/*
 *    Copyright (C) 2014 .. 2023
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
#include	<stdio.h>
#include	<QDir>
#include	<QString>
#include	<QStringList>
#include	<math.h>
#include	"dab-constants.h"
#include	"tii-reader.h"
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
#define	HEIGHT		9
#define	POWER		13
#define	NR_COLUMNS	14

#include        <curl/curl.h>
		tiiReader::tiiReader	() {
}
		tiiReader::~tiiReader	() {
}
//
std::vector<cacheElement> tiiReader::readFile (const QString &s) {
std::vector<cacheElement> res;
	if (s == "") {
	   return res;
	}

	res. resize (0);
	FILE	*f	= fopen (s. toUtf8 (). data (), "r+b");
	if (f == nullptr) 
	   return res;
	int	count = 0; 
	char	buffer [1024];
	std::vector<QString> columnVector;
	int	shift	= fgetc (f);
//	this	-> shift	= fgetc (f);
	while (eread  (buffer, 1024, f, shift) != nullptr) {
	   cacheElement ed;
	   if (feof (f))
	      break;
	   columnVector. resize (0);
	   int columns = readColumns (columnVector, buffer, NR_COLUMNS);
	   if (columns < NR_COLUMNS)
	      continue;
	   ed. country		= columnVector [COUNTRY]. trimmed ();
	   ed. Eid		= get_Eid (columnVector [EID]);
	   ed. mainId		= get_mainId (columnVector [TII]);
	   ed. subId		= get_subId (columnVector [TII]);
	   ed. channel		= columnVector [CHANNEL]. trimmed ();
	   ed. ensemble 	= columnVector [LABEL]. trimmed ();
	   ed. transmitterName	= columnVector [LOCATION];
	   ed. latitude		= convert (columnVector [LATITUDE]);
	   ed. longitude	= convert (columnVector [LONGITUDE]);
	   ed. power		= convert (columnVector [POWER]);
	   ed. height		= convert (columnVector [HEIGHT]);
	   if (count >= (int) res. size ())
	      res. resize (res. size () + 100);
	   res. at (count) = ed;
	   count ++;
	}
	fclose (f);
	return res;
}
	
int	tiiReader::readColumns (std::vector<QString> &v, char *b, int N) {
int charp	= 0;
char	tb [256];
int elementCount = 0;
QString element;

	v. resize (0);
	while ((*b != 0) && (*b != '\n')) {
	   if (*b == SEPARATOR) {
	      tb [charp] = 0;
	      QString  ss = QString::fromUtf8 (tb);
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
char	*tiiReader::eread (char *buffer, int amount, FILE *f, uint8_t shift) {
char	*bufferP;

	if (fgets (buffer, amount, f) == nullptr)
	   return nullptr;
	bufferP	= buffer;
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

float	tiiReader::convert (const QString &s) {
bool	flag;
float	v;
	v = s. trimmed (). toFloat (&flag);
	if (!flag)
	   v = 0;
	return v;
}

uint16_t tiiReader::get_Eid (const QString &s) {
bool	flag;
uint16_t res;
	res = s. trimmed (). toInt (&flag, 16);
	if (!flag)
	   res = 0;
	return res;
}

uint8_t	tiiReader::get_mainId (const QString &s) {
bool flag;
uint16_t res;
	res = s. trimmed (). toInt (&flag);
	if (!flag)
	   res = 0;
	return res / 100;
}

uint8_t tiiReader::get_subId (const QString &s) {
bool flag;
uint16_t res;
	res = s. trimmed (). toInt (&flag);
	if (!flag)
	   res = 0;
	return res % 100;
}

