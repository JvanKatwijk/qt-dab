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
#include	"table-loader.h"
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
#define	POWER		13
#define	NR_COLUMNS	14

#ifdef __MINGW32__
#define GETPROCADDRESS  GetProcAddress
#else
#define GETPROCADDRESS  dlsym
#endif

		tableLoader::tableLoader	() {
	this	-> init_tii_L   = nullptr;
	this	-> close_tii_L  = nullptr;
        this	-> loadTable_L  = nullptr;

//	try to get a handle
	Handle 		= getLibraryHandle ();
//	check whether or not there IS a library
	if ((Handle != nullptr) && loadFunctions ()) 
	   handler	= init_tii_L ();
	else
	   handler	= nullptr;
}

	tableLoader::~tableLoader	() {
	if ((close_tii_L != nullptr) && (handler != nullptr))
	   close_tii_L (handler);
}

bool	tableLoader::loadFunctions () {
	init_tii_L	= (init_tii_P)
	                    GETPROCADDRESS (this -> Handle,
	                                    "init_tii_L");
	if (init_tii_L == nullptr) {
	   fprintf (stderr, "init_tii_L not loaded\n");
	   return false;
	}

	close_tii_L	= (close_tii_P)
	                    GETPROCADDRESS (this -> Handle,
	                                    "close_tii_L");
	if (close_tii_L == nullptr) {
	   fprintf (stderr, "close_tii_L not loaded\n");
	   return false;
	}

	loadTable_L	= (loadTable_P)
	                    GETPROCADDRESS (this -> Handle,
	                                    "loadTableL");
	if (loadTable_L == nullptr) {
	   fprintf (stderr, "loadTable_L not loaded\n");
	   return false;
	}
	return true;
}

#ifndef	__MINGW32__
#define	LIB_NAME	"libtii-lib.so"
#else
#define	LIB_NAME	"libtii-lib.dll"
#endif

HINSTANCE	tableLoader::getLibraryHandle	() {
HINSTANCE	theHandle	= nullptr;

	theHandle	= (HINSTANCE)dlopen (LIB_NAME,
	                                       RTLD_NOW | RTLD_GLOBAL);
	if (theHandle != nullptr)
	   return theHandle;
	QString pathName	= QDir::homePath () + "/" + LIB_NAME;
	theHandle	= (HINSTANCE) dlopen (pathName. toLatin1 (). data (),
	                                       RTLD_NOW | RTLD_GLOBAL);
	return theHandle;
}
//
std::vector<cacheElement> tableLoader::readFile (const QString &s) {
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
	   if (count >= (int) res. size ())
	      res. resize (res. size () + 100);
	   res. at (count) = ed;
	   count ++;
	}
	fclose (f);
	return res;
}
	
int	tableLoader::readColumns (std::vector<QString> &v, char *b, int N) {
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
char	*tableLoader::eread (char *buffer, int amount, FILE *f, uint8_t shift) {
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

bool	tableLoader::loadTable (const QString &tf) {
	if (loadTable_L != nullptr) {
	   loadTable_L (handler, tf. toStdString ());
	   return true;
	}
	return false;
}

float	tableLoader::convert (const QString &s) {
bool	flag;
float	v;
	v = s. trimmed (). toFloat (&flag);
	if (!flag)
	   v = 0;
	return v;
}

uint16_t tableLoader::get_Eid (const QString &s) {
bool	flag;
uint16_t res;
	res = s. trimmed (). toInt (&flag, 16);
	if (!flag)
	   res = 0;
	return res;
}

uint8_t	tableLoader::get_mainId (const QString &s) {
bool flag;
uint16_t res;
	res = s. trimmed (). toInt (&flag);
	if (!flag)
	   res = 0;
	return res / 100;
}

uint8_t tableLoader::get_subId (const QString &s) {
bool flag;
uint16_t res;
	res = s. trimmed (). toInt (&flag);
	if (!flag)
	   res = 0;
	return res % 100;
}

