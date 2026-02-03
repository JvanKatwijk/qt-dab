#
/*
 *    Copyright (C) 2016 .. 2024
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
#define	SCHEDULE	".qt-dab-schedule.sch"
#ifndef	GITHASH
#define	GITHASH	"      "
#endif

static
QString fullPathfor (const QString &v, const QString &ending) {
QString fileName;

	if (v == QString (""))
	   return QString ("/tmp/xxx");

	if (v. at (0) == QChar ('/'))           // full path specified
	   return v;

#ifdef	OSX_INIT_FILE
	char *PathFile;
	PathFile = getenv ("HOME");
	fileName = PathFile;
	fileName.append ("/");
	fileName. append (v);
	qDebug() << fileName;
#else
	fileName = QDir::homePath();
	fileName. append ("/");
	fileName. append (v);
	fileName = QDir::toNativeSeparators (fileName);
#endif
	if (!fileName. endsWith (ending))
	   fileName. append (ending);

	return fileName;
}

void    setTranslator (QTranslator *, QString Language);

int     main (int argc, char **argv) {
QString initFileName	= fullPathfor (QString (DEFAULT_INI), QString (".ini"));
QString	scheduleFile	= fullPathfor (QString (SCHEDULE), QString (".sch"));
QString presetFile	= fullPathfor (QString (".qt-dab-presets.xml"),
	                                                 QString (".xml"));
QString scanListFile	= fullPathfor (QString (".qt-scanList.xml"),
	                                                 QString (".xml"));
QString tiiFileName	= fullPathfor (QString (".txdata.tii"), 
	                                                 QString (".tii"));
RadioInterface	* myRadioInterface;

// Default values
int32_t		dataPort	= 8888;
int32_t		clockPort	= 8889;
int		opt;
QString		freqExtension	= "";
bool		error_report	= false;
int		fmFrequency	= 110000;

QTranslator	theTranslator;
	QCoreApplication::setOrganizationName ("Lazy Chair Computing");
	QCoreApplication::setOrganizationDomain ("Lazy Chair Computing");
	QCoreApplication::setApplicationName ("Qt-DAB");
	QCoreApplication::setApplicationVersion (QString (CURRENT_VERSION) + " Git: " + GITHASH);

	while ((opt = getopt (argc, argv, "C:i:P:Q:A:TM:F:s:")) != -1) {
	   switch (opt) {
	      case 'i':		// alternative for ini file
	         initFileName = fullPathfor (QString (optarg), QString (".ini"));
	         break;

	      case 't':		// alternative for unencoded tiifile
	         tiiFileName	= fullPathfor (QString (optarg), QString (".csv"));
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

	QSettings dabSettings (initFileName, QSettings::IniFormat);
	
/*
 *      Before we connect control to the gui, we have to
 *      instantiate
 */
#if QT_VERSION < QT_VERSION_CHECK (6, 0, 0)
	QGuiApplication::setAttribute (Qt::AA_EnableHighDpiScaling);
#endif

	dabSettings. beginGroup ("SKIN_HANDLING");
	QString skin    = dabSettings. value ("skin", "globstyle"). toString ();
	dabSettings. endGroup ();

	skin    = skin == "Combinear" ? ":res/skins/Combinear.qss" :
	          skin == "globstyle" ? ":res/skins/globstyle.qss":
	          skin == "Adaptic"   ? ":res/skins/Adaptic.qss" :
	          skin == "Darkeum"   ? ":res/skins/Darkeum.qss" :
	          skin == "EasyCode"  ? ":res/skins/EasyCode.qss":
	          skin == "Diffnes"   ? ":res/skins/Diffnes.qss" : "";

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
//
//	here we go for real!!
	qRegisterMetaType<tiiData> ("tiiData");
	qRegisterMetaType<uint32_t> ("uint32_t");
	qRegisterMetaType<QVector<int> >("QVector<int>");
	qRegisterMetaType<QVector<tiiData> >("QVector<tiiData>");
	myRadioInterface = new RadioInterface (&dabSettings,
	                                        scanListFile,
	                                        presetFile,
	                                        freqExtension,
	                                        scheduleFile,
	                                        tiiFileName,
	                                        error_report,
	                                        dataPort,
	                                        clockPort,
	                                        fmFrequency
	                                       );
	myRadioInterface -> show ();
	a. exec();
/*
 *      done:
 */
	fprintf (stderr, "back in main program\n");
	fflush (stdout);
	fflush (stderr);
	qDebug ("It is done\n");
	delete myRadioInterface;
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

	QLocale curLocale (QLocale (static_cast<const QString&>(Language)));
	QLocale::setDefault (curLocale);
}

