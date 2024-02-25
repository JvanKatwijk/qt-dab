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
#ifdef __MINGW32__
#define GETPROCADDRESS  GetProcAddress
#define LIB_NAME        "libtii-lib.dll"
#else
#define GETPROCADDRESS  dlsym
#define LIB_NAME        "libtii-lib.so"
#endif


#include	"tii-reader.h"
#include	<QDir>

        tiiReader::tiiReader () {
	QString path    = LIB_NAME;
        read_db_L      = nullptr;
        QLibrary *p_handle = new QLibrary (path);
        p_handle         -> load ();
	if (!p_handle -> isLoaded ())
	   fprintf (stderr, "cannot load %s\n", LIB_NAME);
        read_db_L      = (read_db_P) p_handle -> resolve ("read_db");
	if (read_db_L == nullptr)
	   fprintf (stderr, "cannot resolve read_db\n");
        delete p_handle;
}

	tiiReader::~tiiReader	() {}

std::vector<cacheElement> tiiReader::readFile (const QString &f) {
std::vector<cacheElement> res (0);

	fprintf (stderr, "We gaan laden met %s\n", f. toStdString (). c_str ());
        if (read_db_L != nullptr)
           res = read_db_L (f. toStdString ());
        return res;
}


