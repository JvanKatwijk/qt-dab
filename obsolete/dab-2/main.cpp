#
/*
 *    Copyright (C) 2014 .. 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of dab-2
 *
 *    dab-2 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    dab-2 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dab-2; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *      Main program
 */
#include        <QApplication>
#include        <QSettings>
#include	<QTranslator>
#include	<QString>
#include        <QDir>
#include	<QDebug>
#include        <unistd.h>
#include        "dab-constants.h"
#include        "radio.h"

#define DEFAULT_INI     ".dab-2.ini"
#define	PRESETS		".qt-dab-presets.xml"
#ifndef	GITHASH
#define	GITHASH	"      "
#endif

QString fullPathfor (QString v) {
QString fileName;

	if (v == QString (""))
	   return QString ("/tmp/xxx");

	if (v. at (0) == QChar ('/'))           // full path specified
	   return v;

#ifdef	OSX_INIT_FILE
	char *PathFile;
	PathFile = getenv ("HOME");
	fileName = PathFile;
	fileName.append("/.qt-dab.ini");
	qDebug() << fileName;
#else

	fileName = QDir::homePath();
	fileName. append ("/");
	fileName. append (v);
	fileName = QDir::toNativeSeparators (fileName);
#endif
	if (!fileName. endsWith (".ini"))
	   fileName. append (".ini");

	return fileName;
}

void    setTranslator (QString Language);

int     main (int argc, char **argv) {
QString initFileName = fullPathfor (QString (DEFAULT_INI));
RadioInterface  *MyRadioInterface;

// Default values
QSettings       *dabSettings;           // ini file
QString		presetName	= PRESETS;
int32_t		dataPort	= 8888;
int     opt;
QString freqExtension		= "";
bool	error_report		= false;

	QCoreApplication::setOrganizationName ("Lazy Chair Computing");
	QCoreApplication::setOrganizationDomain ("Lazy Chair Computing");
	QCoreApplication::setApplicationName ("qt-dab");
	QCoreApplication::setApplicationVersion (QString (CURRENT_VERSION) + " Git: " + GITHASH);

	while ((opt = getopt (argc, argv, "i:P:Q:A:T")) != -1) {
	   switch (opt) {
	      case 'i':
	         initFileName = fullPathfor (QString (optarg));
	         break;

	      case 'P':
	         dataPort	= atoi (optarg);
	         break;

	      case 'A':
	         freqExtension	= optarg;
	         break;
	
	      case 'T':
	         error_report	= true;
	         break;

	      default:
	         break;
	   }
	}

	dabSettings =  new QSettings (initFileName, QSettings::IniFormat);

	QString presets = QDir::homePath();
	presets. append ("/");
	presets. append (presetName);
	presets = QDir::toNativeSeparators (presets);

/*
 *      Before we connect control to the gui, we have to
 *      instantiate
 */
#if QT_VERSION >= 0x050600
	QGuiApplication::setAttribute (Qt::AA_EnableHighDpiScaling);
#endif

	QApplication a (argc, argv);
//	setting the language
	QString locale = QLocale::system(). name();
	qDebug() << "main:" <<  "Detected system language" << locale;
	setTranslator (locale);

	a. setWindowIcon (QIcon (":/qt-dab.ico"));

	MyRadioInterface = new RadioInterface (dabSettings,
	                                       presets,
	                                       freqExtension,
	                                       dataPort
                                               );
	MyRadioInterface -> show();
        a. exec();
/*
 *      done:
 */
	fprintf (stderr, "back in main program\n");
	fflush (stdout);
	fflush (stderr);
	qDebug ("It is done\n");
//	delete MyRadioInterface;
	delete dabSettings;
	return 1;
}

void	setTranslator (QString Language) {
QTranslator *Translator = new QTranslator;

//	German is special (as always)
	if ((Language == "de_AT") || (Language ==  "de_CH"))
	   Language = "de_DE";
//
//	what about Dutch?
	bool TranslatorLoaded =
	             Translator -> load (QString(":/i18n/") + Language);
	qDebug() << "main:" <<  "Set language" << Language;
	QCoreApplication::installTranslator (Translator);

	if (!TranslatorLoaded) {
	   qDebug() << "main:" <<  "Error while loading language specifics" << Language << "use English \"en_GB\" instead";
	   Language = "en_GB";
	}

	QLocale curLocale (QLocale ((const QString&)Language));
	QLocale::setDefault (curLocale);
}

