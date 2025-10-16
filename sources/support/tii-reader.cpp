#
/*
 *    Copyright (C) 2016 .. 2023
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

//#include	"scan-handler.h"

#define	SEPARATOR	';'
#define	COUNTRY		1
#define	CHANNEL		2
#define	ENSEMBLE	3
#define	EID		4
#define	TII		5
#define	TRANSMITTERNAME	6
#define	LATITUDE	7
#define	LONGITUDE	8
#define	ALTITUDE	9
#define	HEIGHT		10
#define	POLARIZATION	11
#define	FREQUENCY	12
#define	POWER		13
#define DIRECTION	14
#define	NR_COLUMNS	15

		tiiReader::tiiReader	() {
}
		tiiReader::~tiiReader	() {
}
//
std::vector<dbElement> tiiReader::readFile (const QString &s) {
std::vector<dbElement> res;
	if (s == "") {
	   return res;
	}

	res. resize (0);
	Rflag =  s. endsWith (".txdata.tii");
	FILE	*f	= fopen (s. toUtf8 (). data (), "r+b");
#ifdef	__TRACE__
	if (f == nullptr)
	   fprintf (stderr, "Cannot open %s\n", s. toUtf8 (). data ());
#endif
	if (f == nullptr) 
	   return res;
	if (!Rflag)
	   fprintf (stderr, "YOU OPENED A NON STANDARD TII DATABASE\n");
#ifdef	__TRACE__
	fprintf (stderr, "File %s is opened\n", s. toUtf8 (). data ());
#endif
	dbElement ed;
	res. push_back (ed);	// the dummy one
	int	count = 1; 
	char	buffer [1024];
	std::vector<QString> columnVector;
	int	shift	= 0;
	if (Rflag)
	   shift	= fgetc (f);
	while (eread  (buffer, 1024, f, shift) != nullptr) {
	   dbElement ed;
	   if (feof (f))
	      break;
	   columnVector. resize (0);
	   int columns = readColumns (columnVector, buffer, NR_COLUMNS);
	   if (columns < NR_COLUMNS)
	      continue;
	   ed. valid		= true;
	   ed. country		= columnVector [COUNTRY]. trimmed ();
	   ed. Eid		= get_Eid (columnVector [EID]);
	   ed. mainId		= get_mainId (columnVector [TII]);
	   ed. subId		= get_subId (columnVector [TII]);
	   ed. channel		= columnVector [CHANNEL]. trimmed ();
	   ed. ensemble 	= columnVector [ENSEMBLE]. trimmed ();
	   ed. transmitterName	= columnVector [TRANSMITTERNAME];
	   ed. latitude		= convert (columnVector [LATITUDE]);
	   ed. longitude	= convert (columnVector [LONGITUDE]);
	   ed. power		= convert (columnVector [POWER]);
	   ed. altitude		= convert (columnVector [ALTITUDE]);
	   ed. height		= convert (columnVector [HEIGHT]);
	   ed. polarization	= columnVector [POLARIZATION].trimmed();
	   ed. frequency	= convert (columnVector[FREQUENCY]);
	   ed. direction	= columnVector [DIRECTION]. trimmed();

	   if ((ed. mainId == 255) ||(ed. subId == 255))
	      ed. valid = false;
	   if ((ed. mainId == 0) || (ed. subId == 0))
	      ed. valid = false;
	   if (ed. ensemble == "")
	      ed. valid = false;
	   if (count >= (int) res. size ())
	      res. resize (res. size () + 100);
	   ed. key_1	= ((ed. Eid << 16) | (ed. mainId << 8)) +  ed. subId; 
	   bool ok;
	   uint16_t cc	= ed. channel. toInt (&ok, 16);
	   ed. key_2	= ok ? cc : 0;
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
	if (Rflag) {
	   while (*bufferP != 0) {
	      if (shift != 0xAA)
	         *bufferP -= shift;
	      else
	         *bufferP ^= 0xAA;
	      bufferP ++;
	   }
	   *bufferP = 0;
	}
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

