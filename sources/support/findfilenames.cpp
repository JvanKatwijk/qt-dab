#
/*
 *    Copyright (C)  2016 .. 2024
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
#include	"settings-handler.h"

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
	QString saveDir	= dumper. remove (x, dumper. size () - x);
	store (dabSettings, DAB_GENERAL, key, saveDir);
}

QString	findfileNames::outputDialog (QString saveDir,
	                             const QString &channel,
	                             const QString &extension, bool flag) {
QString theTime         = QDateTime::currentDateTime (). toString ();
QString suggestedFileName;
	
	suggestedFileName = saveDir + channel + "-" + theTime;
	for (int i = 4; i < suggestedFileName. length (); i ++)
	   if (!isValid (suggestedFileName. at (i)))
	      suggestedFileName. replace (i, 1, '-');

	suggestedFileName	+= extension;
	suggestedFileName        = QDir::toNativeSeparators (suggestedFileName);

	if (!flag)
	   return suggestedFileName;
//	bool	useNativeFileDialog = true;
	QString fileName = QFileDialog::
	                     getSaveFileName (nullptr,
	                                      "Save file ...",
	                                      suggestedFileName,
	                                      QString ("%1 (%2)").arg (extension, extension),
	                                      Q_NULLPTR);
//	                                      useNativeFileDialog ?
//	                                          QFileDialog::Options() :
//	                                          QFileDialog::DontUseNativeDialog);
	if (fileName. isEmpty ())
	   return "";
	return QDir::toNativeSeparators (fileName);
}

FILE	*findfileNames::find_contentDump_fileName (const QString &channel) {
QString	fileName	= outputDialog (basicPath (), channel, ".csv", true);

	if (fileName. isEmpty ())
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
//	framedumps and audio dump
FILE	*findfileNames::find_frameDump_fileName (const QString &service,
	                                        uint8_t ASCTy, bool flag) {
	QString ending;
	ending = ASCTy == DAB_PLUS ? ".aac" : ".mp2";
	QString subName	= basicPath () +
	                (ASCTy == DAB_PLUS ? "aacDump/" : "mp2Dump");
	checkDir (subName);
	QString	fileName	= outputDialog (subName, service,
	                                                 ending, flag);
	if (fileName. isEmpty ())
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

QString	findfileNames::find_audioDump_fileName (const QString &service, 
	                                                      bool flag) {
QString	saveDir		= basicPath () + "audiodumps/";

	checkDir (saveDir);
	QString fileName = outputDialog (saveDir, service, ".wav", flag);
	
	if (fileName. isEmpty ())
	   return nullptr;

	save_dirName ("saveDir_audioDump", fileName);
	return fileName;
}
//
//	Filename for an ".sdr dump"
QString findfileNames::find_rawDump_fileName (const QString &deviceName,
	                                       const QString &channelName) {
QString	saveDir		= basicPath () + "rawdump/";
	checkDir (saveDir);
	QString fileName =
	              outputDialog (saveDir, 
	                            QString (deviceName + "-" + channelName),
	                            ".sdr", true);
	if (fileName. isEmpty ())
	   return nullptr;
	save_dirName ("saveDir_rawDump", fileName);
	return fileName;
}
//
//	filename for an xml dump
QString	findfileNames::find_xmlName	(const QString &deviceName,
	                                 const QString &channelName,
	                                 bool direct) {
QString	saveDir	=  basicPath () + "xmlDump/";
	checkDir (saveDir);
	QString fileName =
	              outputDialog (saveDir, 
	                            QString (deviceName + "-" + channelName),
	                            ".uff", direct);
	if (fileName. isEmpty ())
	   return nullptr;

	save_dirName ("saveDir_xmlDump", fileName);
	return fileName;
}

FILE	*findfileNames::findScanDump_fileName		() {
QString   saveDir =  basicPath ();

	QString fileName	= outputDialog (saveDir,
	                                        "Qt_DAB-scan",
	                                        ".csv", 
	                                        true);

	if (fileName. isEmpty ())
	   return nullptr;
	return  fopen (fileName. toUtf8 (). data (), "w");
}

FILE	*findfileNames::findSummary_fileName	() {
QString   saveDir =  basicPath ();

	QString fileName = outputDialog (saveDir, 
	                                 "Qt_DAB-summary",
	                                 ".csv", true);
	if (fileName. isEmpty ())
	   return nullptr;
	return  fopen (fileName. toUtf8 (). data (), "w");
}

const
QString	findfileNames::findskipFile_fileName	() {
QString   saveDir = basicPath ();

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
	if (fileName. isEmpty ())	// canceled?
	   return nullptr;
	fileName	= QDir::toNativeSeparators (fileName);
	return  fileName;
}

QString findfileNames::finddlText_fileName	(bool flag) {
QString   saveDir = basicPath ();

	QString fileName	= outputDialog (saveDir, 
        	                                "Qt-DAB-dlText",
	                                        ".text", flag);

	if (fileName. isEmpty ())
	   return "";
        return  fileName;
}

FILE	* findfileNames::findLogFileName	() {
QString   saveDir =  basicPath ();

	QString fileName	= outputDialog (saveDir,
	                                        "Qt_DAB-log",
	                                        ".text", true);

	if (fileName. isEmpty ())
	   return nullptr;	
	return fopen (fileName. toUtf8 (). data (), "w");
}

QString	findfileNames::find_ficDump_file	(const QString &channel) {
QString	saveDir = basicPath ();
	QString fileName = outputDialog (saveDir, channel, ".fic", true);
	return fileName;
}

QString	findfileNames::find_eti_fileName (const QString &ensemble,
	                                  const QString &channel) {
QString	saveDir	 = basicPath ();
QString theTime         = QDateTime::currentDateTime (). toString ();
QString suggestedFileName =
	           basicPath () + theTime + ensemble + "-" + channel + ".eti";

	return outputDialog (saveDir, suggestedFileName, ".eti", true);
}

QString	findfileNames::find_mapdumpName	(const QString &deviceName) {
QString result	= basicPath () + "mapdumps/";
QDate	theDate;
QTime	theTime;
	QString timeString	= theDate. currentDate (). toString () + "-" +		                          theTime. currentTime (). toString ();
	checkDir (result);
	result	+= deviceName + "-" + timeString;
	for (int i = 4; i < result. length (); i ++)
	   if (!isValid (result. at (i)))
	      result. replace (i, 1, "-");
	result	= QDir::toNativeSeparators (result + ".xml");
	return result;
}

QString	findfileNames::basicPath	() {

        QString tempPath	= QDir::homePath () + "/Qt-DAB-files-7/";
	tempPath		=
	          value_s (dabSettings, "CONFIG_HANDLER", "filePath", tempPath);
	if (!tempPath. endsWith ('/'))
	   tempPath		+= '/';
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

