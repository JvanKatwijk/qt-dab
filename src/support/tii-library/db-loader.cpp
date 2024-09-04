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
#include	"db-loader.h"
#include	<QSettings>

#ifdef __MINGW32__
#define GETPROCADDRESS  GetProcAddress
#define LIB_NAME        "libtii-lib.dll"
#else
#define GETPROCADDRESS  dlsym
#define LIB_NAME        "libtii-lib.so"
#endif

		dbLoader::dbLoader	(QSettings *dabSettings) {
	tiiFileName = QDir::homePath () + "/.txdata.tii";
        tiiFileName = dabSettings -> value ("tiiFileName", tiiFileName). toString ();                                         
	if (tiiFileName == "")
	   tiiFileName = QDir::homePath () + "/.txdata.tii";
	QString	path	= LIB_NAME;
        this	-> load_db_L  = nullptr;
//	try to get a handle
	phandle		= new QLibrary (path);
	phandle		-> load ();
	if (!phandle -> isLoaded ()) {
	   fprintf (stderr, "Cannot load %s\n", LIB_NAME);
	   return;
	}

	load_db_L	= (load_db_P) phandle -> resolve ("load_db");
	delete phandle;
}

	dbLoader::~dbLoader	() {
	
}

bool	dbLoader::load_db () {
	if (load_db_L != nullptr) {
	   load_db_L (tiiFileName. toStdString ());
	   return true;
	}
	return false;
}
