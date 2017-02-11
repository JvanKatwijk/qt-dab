#
/*
 *    Copyright (C) 2013, 2014, 2015, 2015, 2016
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the Qt-DAB (formerly sdr-j-dab and DAB-rpi)
 *    Many of the ideas as implemented in Qt-DAB are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
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
 *    along with Qt-DAB-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	Main program
 */
#include	<QApplication>
#include	<QSettings>
#include	<QDir>
#include	<unistd.h>
#include	"dab-constants.h"
#include	"radio.h"

QString	fullPathfor (QString v) {
QString	fileName;

	if (v == QString ("")) 
	   return QString ("/tmp/xxx");

	if (v. at (0) == QChar ('/')) 		// full path specified
	   return v;

	fileName = QDir::homePath ();
	fileName. append ("/");
	fileName. append (v);
	fileName = QDir::toNativeSeparators (fileName);

	if (!fileName. endsWith (".ini"))
	   fileName. append (".ini");

	return fileName;
}

#define	DEFAULT_INI	".qt-dab.ini"

int	main (int argc, char **argv) {
QString	initFileName;
RadioInterface	*MyRadioInterface;

// Default values
uint8_t		syncMethod	= 2;
QSettings	*dabSettings;		// ini file
#ifdef	GUI_2
uint8_t		dabMode		= 127;	// illegal value
QString		dabDevice	= QString ("");
QString		dabBand		= QString ("");
QString		ipAddress	= QString ("");
#endif
//
	int	opt;
#ifdef	GUI_2
	QString	channel		= "11C";
	QString	programName	= "Classic FM";
	int	gain		= 20;
	QString	dabChannel	= QString ("");
	QString	dabProgramName	= QString ("");
#ifndef	TCP_STREAMER
	QString	soundChannel	= "default";
	QString	dabSoundchannel	= QString ("");
#endif
#endif
	while ((opt = getopt (argc, argv, "i:D:S:M:B:C:P:G:A:")) != -1) {
	   switch (opt) {
	      case 'i':
	         initFileName = fullPathfor (QString (optarg));
	         break;

	      case 'S':
	         syncMethod	= atoi (optarg);
	         break;

#if defined (GUI_2) 
	      case 'D':
	         dabDevice = optarg;
	         break;

	      case 'M':
	         dabMode	= atoi (optarg);
	         if (!(dabMode == 1) || (dabMode == 2) || (dabMode == 4))
	            dabMode = 1; 
	         break;

	      case 'B':
	         dabBand 	= optarg;
	         break;

	      case 'I':
	         ipAddress	= optarg;
	         break;

	      case 'C':
	         dabChannel	= QString (optarg);
	         break;

	      case 'P':
	         dabProgramName	= QString (optarg);
	         break;

	      case 'G':
	         dabGain	= atoi (optarg);
	         break;
#ifndef	TCP_STREAMER
	      case 'A':
	         dabSoundchannel	= QString (optarg);
	         break;
#endif
#endif
	      default:
	         break;
	   }
	}

	if (initFileName == QString (""))
	   initFileName	= fullPathfor (QString (DEFAULT_INI));
	dabSettings =  new QSettings (initFileName, QSettings::IniFormat);

#if defined (GUI_2) 
//	Since we do not have the possibility in GUI_2 to select
//	Mode, Band or Device, we create the possibility for
//	passing appropriate parameters to the command
//	Selections - if any - will be default for the next session

	if (dabMode == 127)
	   dabMode = dabSettings -> value ("dabMode", 1). toInt ();
	if (dabDevice == QString (""))
	   dabDevice = dabSettings -> value ("device", "dabstick"). toString ();
	if (dabBand == QString (""))
	   dabBand = dabSettings -> value ("band", "BAND III"). toString ();
#endif 
/*
 *	Before we connect control to the gui, we have to
 *	instantiate
 */
	QApplication a (argc, argv);
#ifdef GUI_2
	(void)syncMethod;
	dabSettings -> setValue ("dabMode",	dabMode);
	dabSettings -> setValue ("device",	dabDevice);
	dabSettings -> setValue ("band",	dabBand);
	MyRadioInterface = new RadioInterface (dabSettings, 
	                                       dabDevice, dabMode, dabBand);
	dabSettings	-> sync ();
#else
	MyRadioInterface = new RadioInterface (dabSettings, syncMethod);
	MyRadioInterface -> show ();
#endif

#if QT_VERSION >= 0x050600
	QGuiApplication::setAttribute (Qt::AA_EnableHighDpiScaling);
#endif
	a. exec ();
/*
 *	done:
 */
	fflush (stdout);
	fflush (stderr);
	qDebug ("It is done\n");
	MyRadioInterface	-> ~RadioInterface ();
	dabSettings		-> ~QSettings ();
	exit (1);
}

