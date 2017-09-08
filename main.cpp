#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB (formerly SDR-J, JSDR).
 *    Many of the ideas as implemented in Qt-DAB are derived from
 *    other work, made available through the GNU general Public License.
 *    All copyrights of the original authors are acknowledged.
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
 *
 *      Main program
 */
#include        <QApplication>
#include        <QSettings>
#include        <QDir>
#include        <unistd.h>
#include        "dab-constants.h"
#include        "radio.h"

QString fullPathfor (QString v) {
QString fileName;

	if (v == QString (""))
	   return QString ("/tmp/xxx");

	if (v. at (0) == QChar ('/'))           // full path specified
	   return v;

	fileName = QDir::homePath ();
	fileName. append ("/");
	fileName. append (v);
	fileName = QDir::toNativeSeparators (fileName);

	if (!fileName. endsWith (".ini"))
	   fileName. append (".ini");

	return fileName;
}

#define DEFAULT_INI     ".qt-dab.ini"

int     main (int argc, char **argv) {
QString initFileName;
RadioInterface  *MyRadioInterface;

// Default values
bool            tracing         = false;
int16_t         tii_delay       = 60;
QSettings       *dabSettings;           // ini file
int32_t		dataPort	= 8888;
	int     opt;

	while ((opt = getopt (argc, argv, "i:D:TP:")) != -1) {
	   switch (opt) {
	      case 'i':
	         initFileName = fullPathfor (QString (optarg));
	         break;

	      case 'D':
	         tii_delay      = atoi (optarg);
	         break;

	      case 'T':
	         tracing        = true;
	         break;

	      case 'P':
	         dataPort	= atoi (optarg);
	         break;

	     default:
	         break;
	   }
	}

	if (initFileName == QString (""))
	   initFileName = fullPathfor (QString (DEFAULT_INI));
	dabSettings =  new QSettings (initFileName, QSettings::IniFormat);

/*
 *      Before we connect control to the gui, we have to
 *      instantiate
 */
	QApplication a (argc, argv);
	MyRadioInterface = new RadioInterface (dabSettings,
                                               tii_delay,
	                                       dataPort,
                                               tracing);
	MyRadioInterface -> show ();

#if QT_VERSION >= 0x050600
	QGuiApplication::setAttribute (Qt::AA_EnableHighDpiScaling);
#endif
        a. exec ();
/*
 *      done:
 */
	fprintf (stderr, "back in main program\n");
	fflush (stdout);
	fflush (stderr);
	qDebug ("It is done\n");
//	delete MyRadioInterface;
	delete dabSettings;
}

