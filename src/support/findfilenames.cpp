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
	return c. isLetter () || c. isDigit () || (c == '-') || (c == '/');
}

	findfileNames::findfileNames (QSettings *s):
	                                      dabSettings (s) {
}

	findfileNames::~findfileNames	() {}


void	findfileNames::save_dirName (const QString key, const QString path) {
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

	suggestedFileName = saveDir + "Qt-DAB-" + channel +
	                                          "-" + theTime + extension;
	for (int i = 0; i < suggestedFileName. length (); i ++)
	   if (!isValid (suggestedFileName. at (i)))
	      suggestedFileName. replace (i, 1, '-');
	
	suggestedFileName        = QDir::toNativeSeparators (suggestedFileName);

	fprintf (stderr, "suggested filename %s\n", suggestedFileName. toLatin1 (). data ());
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
QString	saveDir		= dabSettings -> value (CONTENT_DIR, basicPath ()).
	                                                           toString ();
QString	fileName	= outputDialog (saveDir, channel, ".csv", true);

	if (fileName == "")
	   return nullptr;
	FILE *fileP	= fopen (fileName. toUtf8(). data(), "w");

	if (fileP == nullptr) {
	   fprintf (stderr, "Could not open file %s\n",
	                              fileName. toUtf8(). data());
	   return nullptr;
	}

	save_dirName (CONTENT_DIR, fileName);
	return fileP;
}

//
FILE	*findfileNames::findFrameDump_fileName (const QString &service,
	                                                      bool flag) {
QString	saveDir	= dabSettings -> value ("saveDir_frameDump", basicPath ()).
	                                                       toString ();
QString	fileName	= outputDialog (saveDir, service, ".aac", flag);

	if (fileName == "")
	   return nullptr;
	FILE *theFile	= fopen (fileName. toUtf8 (). data (), "w+b");
	if (theFile == nullptr) {
	   QString s = QString ("cannot open ") + fileName;
	   QMessageBox::warning (nullptr, "Warning", s. toUtf8 (). data ());
	   return nullptr;
	}

	save_dirName ("saveDir_frameDump", fileName);
	return theFile;
}

QString	findfileNames::findAudioDump_fileName (const QString &service, 
	                                                      bool flag) {
QString	saveDir		= dabSettings -> value ("saveDir_audioDump",
	                                       basicPath ()).  toString ();

	fprintf (stderr, "savedir = %s\n", saveDir. toLatin1 (). data ());
	QString fileName = outputDialog (saveDir, service, ".wav", flag);
	
	if (fileName == "")
	   return nullptr;

	save_dirName ("saveDir_audioDump", fileName);
	return fileName;
}

QString findfileNames::findRawDump_fileName (const QString &deviceName,
	                                       const QString &channelName) {
QString	saveDir		= dabSettings -> value ("saveDir_rawDump",
	                                        basicPath ()). toString ();
	QString fileName =
	              outputDialog (saveDir, 
	                            QString (deviceName + "-" + channelName),
	                            ".sdr", true);
	if (fileName == "")
	   return nullptr;

	save_dirName ("saveDir_rawDump", fileName);
	return fileName;
}

FILE	*findfileNames::findScanDump_fileName		() {
QString   saveDir = dabSettings -> value (CONTENT_DIR, basicPath ()).
	                                                         toString ();

	QString fileName	= outputDialog (saveDir,
	                                        "Qt_DAB-scan",
	                                        ".csv", 
	                                        true);

	if (fileName == "")
	   return nullptr;
	return  fopen (fileName. toUtf8 (). data (), "w");
}

FILE	*findfileNames::findSummary_fileName	() {
QString   saveDir = dabSettings -> value (CONTENT_DIR, basicPath ()).
	                                                       toString ();

	QString fileName = outputDialog (saveDir, 
	                                 "Qt_DAB-summary",
	                                 ".csv", true);
	if (fileName == "")
	   return nullptr;
	return  fopen (fileName. toUtf8 (). data (), "w");
}

const
QString	findfileNames::findskipFile_fileName	() {
QString   saveDir = dabSettings -> value (CONTENT_DIR, basicPath ()).
	                                                    toString ();

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
QString   saveDir = dabSettings -> value (CONTENT_DIR, basicPath ()).
                                                                toString ();

	QString fileName	= outputDialog (saveDir, 
        	                                "Qt-DAB-dlText",
	                                        ".text", flag);

	if (fileName == "")
	   return "";
        return  fileName;
}

FILE	* findfileNames::findLogFileName	() {
QString   saveDir = dabSettings -> value (CONTENT_DIR, basicPath ()).
                                                 toString ();

	QString fileName	= outputDialog (saveDir,
	                                        "Qt_DAB-log",
	                                        ".text", true);

	if (fileName == "")
	   return nullptr;	
	return fopen (fileName. toUtf8 (). data (), "w");
}

FILE	* findfileNames::find_ficDump_file	(const QString &channel) {
QString	saveDir = dabSettings -> value (CONTENT_DIR, basicPath ()).
                                               toString ();
	QString fileName = outputDialog (saveDir, channel, ".fic", true);
	if (fileName == "")
	   return nullptr;
	return fopen (fileName. toUtf8 (). data (), "w+b");
}

QString	findfileNames::findMaps_fileName () {
QString	saveDir		= dabSettings -> value (CONTENT_DIR, basicPath ()).
	                                                     toString ();
	return outputDialog (saveDir, "Qt_DAB-transmitters", ".csv", true);
}

QString	findfileNames::find_eti_fileName (const QString &ensemble,
	                                  const QString &channelName) {
QString	saveDir	 = dabSettings -> value (CONTENT_DIR, basicPath ()).
	                                                     toString ();
QString theTime         = QDateTime::currentDateTime (). toString ();
QString suggestedFileName;

	return outputDialog (saveDir,
	                     QString (ensemble + "-" + channelName),
	                     ".eti", true);
}

QString	findfileNames::basicPath	() {
#ifndef __MINGW32__
        QString tempPath	= QDir::tempPath ();
#else
        QString tempPath	= QDir::homePath ();
#endif
	if (!tempPath. endsWith ('/'))
	   tempPath		+= '/';
        tempPath        +=  "Qt-DAB-files/";
	tempPath	= checkDir (tempPath);

	return QDir::fromNativeSeparators (tempPath);
}

QString	findfileNames::checkDir		(const QString &s) {
QString dir = s;        

        if (!dir. endsWith (QChar ('/')))
           dir += QChar ('/');  
        
        if (QDir (dir). exists())
           return dir;
        QDir (). mkpath (dir);
        return dir;
}

