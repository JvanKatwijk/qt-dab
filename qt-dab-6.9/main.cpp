#
/*
 *    Copyright (C) 2014 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB 
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
#include	<QTranslator>
#include	<QString>
#include        <QDir>
#include	<QFile>
#include	<QDebug>
#include        <unistd.h>
#include        "dab-constants.h"
#include        "radio.h"
#define DEFAULT_INI     ".qt-dab-6.8.ini"
#define	SCHEDULE	".qt-dab-schedule"
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
	fileName.append ("/.qt-dab.ini");
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

void    setTranslator (QTranslator *, QString Language);

int     main (int argc, char **argv) {
QString initFileName = fullPathfor (QString (DEFAULT_INI));
RadioInterface  *myRadioInterface;

// Default values
QSettings       *dabSettings;           // ini file
int32_t		dataPort	= 8888;
int32_t		clockPort	= 8889;
int		opt;
QString		freqExtension	= "";
bool		error_report	= false;
int		fmFrequency	= 110000;
QString	scheduleFile		= fullPathfor (SCHEDULE);

QTranslator	theTranslator;
	QCoreApplication::setOrganizationName ("Lazy Chair Computing");
	QCoreApplication::setOrganizationDomain ("Lazy Chair Computing");
	QCoreApplication::setApplicationName ("Qt-DAB");
	QCoreApplication::setApplicationVersion (QString (CURRENT_VERSION) + " Git: " + GITHASH);

	while ((opt = getopt (argc, argv, "C:i:P:Q:A:TM:F:s:")) != -1) {
	   switch (opt) {
	      case 'i':
	         initFileName = fullPathfor (QString (optarg));
	         break;

	      case 'P':
	         dataPort	= atoi (optarg);
	         break;

	      case 'C':
	         clockPort	= atoi (optarg);
	         break;

	      case 'A':
	         freqExtension	= optarg;
	         break;
	
	      case 'T':
	         error_report	= true;
	         break;

	      case 'F':
	         fmFrequency	= atoi (optarg);
	         break;

	      case 's':
	         break;

	      default:
	         break;
	   }
	}

	dabSettings =  new QSettings (initFileName, QSettings::IniFormat);
	
	QString presetFile = QDir::homePath () + "/" + ".qt-dab-presets.xml";
	presetFile	= QDir::toNativeSeparators (presetFile);
	QString scanListFile = QDir::homePath () + "/.qt-scanList.xml";
	scanListFile	= QDir::toNativeSeparators (scanListFile);
/*
 *      Before we connect control to the gui, we have to
 *      instantiate
 */
#if QT_VERSION >= 0x050600
	QGuiApplication::setAttribute (Qt::AA_EnableHighDpiScaling);
#endif

	dabSettings	-> beginGroup ("SKIN_HANDLING");
	QString skin    = dabSettings -> value ("skin", "Combinear"). toString ();
	dabSettings	-> endGroup ();

	skin    = skin == "Combinear" ? ":res/Combinear.qss" :
	          skin == "Adaptic"   ? ":res/Adaptic.qss" :
	          skin == "Darkeum"   ? ":res/Darkeum.qss" :
	          skin == "EasyCode"  ? ":res/EasyCode.qss":
	          skin == "globstyle" ? ":res/globstyle.qss" :
	          skin == "Diffnes"   ? ":res/Diffnes.qss" : "";

	QApplication a (argc, argv);
	if (skin != "") {
	   QFile file (skin);
	   if (file .open (QFile::ReadOnly | QFile::Text)) {   
	      a. setStyleSheet (file.readAll ());
	      file.close ();
	   }
	}
//	setting the language
	QString locale = QLocale::system (). name ();
	qDebug() << "main:" <<  "Detected system language" << locale;
	setTranslator (&theTranslator, locale);
	a. setWindowIcon (QIcon (":/res/qt-dab-6-128x128.png"));
	myRadioInterface = new RadioInterface (dabSettings,
	                                       scanListFile,
	                                       presetFile,
	                                       freqExtension,
	                                       scheduleFile,
	                                       error_report,
	                                       dataPort,
	                                       clockPort,
	                                       fmFrequency
	                                       );
	myRadioInterface -> show ();
	qRegisterMetaType<tiiData> ("tiiData");
	qRegisterMetaType<QVector<int> >("QVector<int>");
	qRegisterMetaType<QVector<tiiData> >("QVector<tiiData>");
	

	a. exec();
/*
 *      done:
 */
	fprintf (stderr, "back in main program\n");
	fflush (stdout);
	fflush (stderr);
	qDebug ("It is done\n");
	delete myRadioInterface;
	delete dabSettings;
	return 1;
}

void	setTranslator (QTranslator *theTranslator, QString Language) {

//	German is special (as always)
	if ((Language == "de_AT") || (Language ==  "de_CH"))
	   Language = "de_DE";
//
//	what about Dutch?
	bool translatorLoaded =
	             theTranslator -> load (QString(":/i18n/") + Language);
	qDebug () << "main:" <<  "Set language" << Language;
	QCoreApplication::installTranslator (theTranslator);

	if (!translatorLoaded) {
	   qDebug() << "main:" <<  "Error while loading language specifics" << Language << "use English \"en_GB\" instead";
	   Language = "en_GB";
	}

	QLocale curLocale (QLocale ((const QString&)Language));
	QLocale::setDefault (curLocale);
}

