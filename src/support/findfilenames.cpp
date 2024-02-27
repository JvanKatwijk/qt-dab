#
/*
 *    Copyright (C)  2016 .. 2023
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

#include	"findfilenames.h"
#include	"dab-constants.h"
#include	<QDebug>
#include	<QFileDialog>
#include	"settingNames.h"

static inline
bool    isValid (QChar c) {
	return c. isLetter () || c. isDigit () || (c == '-');
}

	findfileNames::findfileNames (QSettings *s) {
	dabSettings	= s;
}

	findfileNames::~findfileNames	() {}


void	findfileNames::save_saveDir (const QString key, const QString path) {
	QString	dumper	= QDir::fromNativeSeparators (path);
	int x           = dumper. lastIndexOf ("/");
	QString saveDir	= dumper. remove (x, dumper. count () - x);
	dabSettings     -> setValue (key, saveDir);
}

QString	findfileNames::outputDialog (QString saveDir,
	                             const QString &channel,
	                             const QString &extension, bool flag) {
QString theTime         = QDateTime::currentDateTime (). toString ();
QString suggestedFileName;
	
	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	for (int i = 0; i < theTime. length (); i ++)
	   if (!isValid (theTime. at (i)))
	      theTime. replace (i, 1, '-');
	suggestedFileName = saveDir + "Qt-DAB-" + channel +
	                                          "-" + theTime + extension;
	suggestedFileName        = QDir::toNativeSeparators (suggestedFileName);

	if (!flag)
	   return suggestedFileName;
	bool	useNativeFileDialog = true;
	QString fileName = QFileDialog::
	                     getSaveFileName (nullptr,
	                                      "Save file ...",
	                                      suggestedFileName,
	                                      QString ("%1 (%2)").arg (extension, extension),
	                                      Q_NULLPTR,
	                                      useNativeFileDialog ?
	                                          QFileDialog::Options() :
	                                          QFileDialog::DontUseNativeDialog);
	if (fileName == "")
	   return "";

	return QDir::toNativeSeparators (fileName);
}

	                         
FILE	*findfileNames::findContentDump_fileName (const QString &channel) {
QString	saveDir		= dabSettings -> value (CONTENT_DIR,
	                                        QDir::homePath ()). toString ();
QString	fileName	= outputDialog (saveDir, channel, ".csv", true);

	if (fileName == "")
	   return nullptr;
	FILE *fileP	= fopen (fileName. toUtf8(). data(), "w");

	if (fileP == nullptr) {
	   fprintf (stderr, "Could not open file %s\n",
	                              fileName. toUtf8(). data());
	   return nullptr;
	}

	save_saveDir (CONTENT_DIR, fileName);
	return fileP;
}

//
FILE	*findfileNames::findFrameDump_fileName (const QString &service,
	                                                      bool flag) {
QString	saveDir	= dabSettings -> value ("saveDir_frameDump",
	                                QDir::homePath ()).  toString ();
QString	fileName	= outputDialog (saveDir, service, ".aac", flag);

	if (fileName == "")
	   return nullptr;
	FILE *theFile	= fopen (fileName. toUtf8 (). data (), "w+b");
	if (theFile == nullptr) {
	   QString s = QString ("cannot open ") + fileName;
	   QMessageBox::warning (nullptr, "Warning", s. toUtf8 (). data ());
	   return nullptr;
	}

	save_saveDir ("saveDir_frameDump", fileName);
	return theFile;
}

SNDFILE	*findfileNames::findAudioDump_fileName (const QString &service, 
	                                                      bool flag) {
SF_INFO	*sf_info	= (SF_INFO *)alloca (sizeof (SF_INFO));
QString theTime		= QDateTime::currentDateTime (). toString ();
QString	saveDir	 = dabSettings -> value ("saveDir_audioDump",
	                                 QDir::homePath ()).  toString ();

	QString fileName = outputDialog (saveDir, service, ".aac", flag);
	if (fileName == "")
	   return nullptr;
	sf_info		-> samplerate	= 48000;
	sf_info		-> channels	= 2;
	sf_info		-> format	= SF_FORMAT_WAV | SF_FORMAT_PCM_16;

	SNDFILE *theFile	= sf_open (fileName. toUtf8(). data(),
	                                   SFM_WRITE, sf_info);
	if (theFile == nullptr) {
	   qDebug() << "Cannot open " << fileName. toUtf8(). data();
	   return nullptr;
	}

	save_saveDir ("saveDir_audioDump", fileName);
	return theFile;
}

SNDFILE *findfileNames::findRawDump_fileName (const QString &deviceName,
	                                       const QString &channelName) {
QString	saveDir		= dabSettings -> value ("saveDir_rawDump",
	                                        QDir::homePath ()). toString ();
	QString fileName =
	              outputDialog (saveDir, 
	                            QString (deviceName + "-" + channelName),
	                            ".sdr", true);
	if (fileName == "")
	   return nullptr;

	SF_INFO	sf_info;
	sf_info. samplerate   = INPUT_RATE;
	sf_info. channels     = 2;
	sf_info. format       = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	SNDFILE *theFile	= sf_open (fileName. toUtf8 (). data(),
	                                   SFM_WRITE, &sf_info);
	if (theFile == nullptr) {
	   fprintf (stderr, "foute boel\n");
	   qDebug() << "cannot open " << fileName. toUtf8(). data();
	   return nullptr;
	}

	save_saveDir ("saveDir_rawDump", fileName);
	return theFile;
}

FILE	*findfileNames::findScanDump_fileName		() {
QString   saveDir = dabSettings -> value (CONTENT_DIR,
	                                        QDir::homePath ()). toString ();

	QString fileName	= outputDialog (saveDir,
	                                        "Qt_DAB-scan",
	                                        ".csv", 
	                                        true);

	if (fileName == "")
	   return nullptr;
	return  fopen (fileName. toUtf8 (). data (), "w");
}

FILE	*findfileNames::findSummary_fileName	() {
QString   saveDir = dabSettings -> value (CONTENT_DIR,
	                                        QDir::homePath ()). toString ();

	QString fileName = outputDialog (saveDir, 
	                                 "Qt_DAB-summary",
	                                 ".csv", true);
	if (fileName == "")
	   return nullptr;
	return  fopen (fileName. toUtf8 (). data (), "w");
}

const
QString	findfileNames::findskipFile_fileName	() {
QString   saveDir = dabSettings -> value (CONTENT_DIR,
	                                        QDir::homePath ()). toString ();

	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	QString suggestedFileName =
	                  saveDir + "Qt-DAB-skipFile" + ".xml";

	QString fileName =
	   QFileDialog::getSaveFileName (nullptr,
	                                 "Save File",
	                                 suggestedFileName,
	                                 "Xml (*.xml)", 
	                                 Q_NULLPTR,
	                                 QFileDialog::DontUseNativeDialog |
	                                 QFileDialog::DontConfirmOverwrite);
	if (fileName == "")	// canceled?
	   return nullptr;
	fileName	= QDir::toNativeSeparators (fileName);
	return  fileName;
}

QString findfileNames::finddlText_fileName	(bool flag) {
QString   saveDir = dabSettings -> value (CONTENT_DIR,
                                                QDir::homePath ()). toString ();

	QString fileName	= outputDialog (saveDir, 
        	                                "Qt-DAB-dlText",
	                                        ".text", flag);

	if (fileName == "")
	   return "";
        return  fileName;
}

FILE	* findfileNames::findLogFileName	() {
QString   saveDir = dabSettings -> value (CONTENT_DIR,
                                                QDir::homePath ()). toString ();

	QString fileName	= outputDialog (saveDir,
	                                        "Qt_DAB-log",
	                                        ".text", true);

	if (fileName == "")
	   return nullptr;	
	return fopen (fileName. toUtf8 (). data (), "w");
}

FILE	* findfileNames::find_ficDump_file	(const QString &channel) {
QString	saveDir = dabSettings -> value (CONTENT_DIR,
                                              QDir::homePath ()). toString ();
	QString fileName = outputDialog (saveDir, channel, ".fic", true);
	if (fileName == "")
	   return nullptr;
	return fopen (fileName. toUtf8 (). data (), "w+b");
}

QString	findfileNames::findMaps_fileName () {
QString	saveDir		= dabSettings -> value (CONTENT_DIR,
	                                        QDir::homePath ()). toString ();
	return outputDialog (saveDir, "Qt_DAB-transmitters", ".csv", true);
}

QString	findfileNames::find_eti_fileName (const QString &ensemble,
	                                  const QString &channelName) {
QString	saveDir	 = dabSettings -> value (CONTENT_DIR,
	                                   QDir::homePath ()). toString ();
QString theTime         = QDateTime::currentDateTime (). toString ();
QString suggestedFileName;

	return outputDialog (saveDir,
	                     QString (ensemble + "-" + channelName),
	                     ".eti", true);
}

