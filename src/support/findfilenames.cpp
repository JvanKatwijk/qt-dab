#
/*
 *    Copyright (C)  2015, 2016, 2017, 2018, 2019, 2020
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
 */

#include	"findfilenames.h"
#include	"dab-constants.h"
#include	<QDebug>
#include	<QFileDialog>
static inline
bool    isValid (QChar c) {
	return c. isLetter () || c. isDigit () || (c == '-');
}

	findfileNames::findfileNames (QSettings *s) {
	dabSettings	= s;
}

	findfileNames::~findfileNames	() {}


FILE	*findfileNames::findContentDump_fileName (const QString &channel) {
QString	saveDir		= dabSettings -> value ("contentDir",
	                                        QDir::homePath ()). toString ();
QString theTime         = QDateTime::currentDateTime (). toString ();
QString suggestedFileName;

	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	for (int i = 0; i < theTime. length (); i ++)
	   if (!isValid (theTime. at (i)))
	      theTime. replace (i, 1, '-');
	suggestedFileName = saveDir + "Qt-DAB-" + channel +
	                                          "-" + theTime + ".csv";
	suggestedFileName        = QDir::toNativeSeparators (suggestedFileName);


	QString fileName = QFileDialog::
	                     getSaveFileName (nullptr,
	                                      "Save file ...",
	                                      suggestedFileName,
	                                      "csv (*.csv)",
	                                      Q_NULLPTR,
	                                      QFileDialog::DontUseNativeDialog);
	if (fileName == "")
	   return nullptr;

	fileName	= QDir::toNativeSeparators (fileName);
	FILE *fileP	= fopen (fileName. toUtf8(). data(), "w");

	if (fileP == nullptr) {
	   fprintf (stderr, "Could not open file %s\n",
	                              fileName. toUtf8(). data());
	   return nullptr;
	}

	QString	dumper	= QDir::fromNativeSeparators (fileName);
	int x           = dumper. lastIndexOf ("/");
	saveDir         = dumper. remove (x, dumper. count () - x);
	dabSettings     -> setValue ("contentDir", saveDir);
	return fileP;
}

//
FILE	*findfileNames::findFrameDump_fileName (const QString &service,
	                                                      bool flag) {
QString	saveDir	= dabSettings -> value ("saveDir_frameDump",
	                                QDir::homePath ()).  toString ();
QString theTime         = QDateTime::currentDateTime (). toString ();

	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	QString tailS	= service + "-" + theTime;
	for (int i = 0; i < tailS. length (); i ++)
	   if (!isValid (tailS. at (i)))
	      tailS. replace (i, 1, '-');

	QString suggestedFileName = saveDir + tailS + ".aac";
	QString fileName;
	if (flag)
	   fileName = QFileDialog::
	                      getSaveFileName (nullptr,
	                                      "Save file ...",
	                                      suggestedFileName,
	                                      "aac data (*.aac)",
	                                      Q_NULLPTR,
	                                      QFileDialog::DontUseNativeDialog);
	else
	   fileName = suggestedFileName;

	if (fileName == QString (""))       // apparently cancelled
	   return nullptr;

	if (!fileName.endsWith (".aac", Qt::CaseInsensitive))
	   fileName.append (".aac");
	fileName	= QDir::toNativeSeparators (fileName);
	FILE *theFile	= fopen (fileName. toUtf8 (). data (), "w+b");
	if (theFile == nullptr) {
	   QString s = QString ("cannot open ") + fileName;
	   QMessageBox::warning (nullptr, "Warning", s. toUtf8 (). data ());
	   return nullptr;
	}

	QString dumper	= QDir::fromNativeSeparators (fileName);
	int x		= dumper. lastIndexOf ("/");
	saveDir		= dumper. remove (x, dumper. count () - x);
	dabSettings	-> setValue ("saveDir_frameDump", saveDir);
	return theFile;
}

SNDFILE	*findfileNames::findAudioDump_fileName (const QString &service, 
	                                                      bool flag) {
SF_INFO	*sf_info	= (SF_INFO *)alloca (sizeof (SF_INFO));
QString theTime		= QDateTime::currentDateTime (). toString ();
QString	saveDir	 = dabSettings -> value ("saveDir_audioDump",
	                                 QDir::homePath ()).  toString ();

	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	QString tailS = service + "-" + theTime;
	for (int i = 0; i < tailS. length (); i ++)
	   if (!isValid (tailS. at (i))) 
	      tailS. replace (i, 1, '-');

	QString suggestedFileName = saveDir + tailS + ".wav";
	QString fileName;
	if (flag)
	   fileName = QFileDialog::
	                     getSaveFileName (nullptr,
	                                      "Save file ...",
	                                      suggestedFileName,
	                                      "PCM wave file (*.wav)",
	                                      Q_NULLPTR,
	                                      QFileDialog::DontUseNativeDialog);
	else
	   fileName = suggestedFileName;
	if (fileName == QString (""))
	   return nullptr;

	if (!fileName. endsWith (".wav", Qt::CaseInsensitive))
	   fileName.append (".wav");
	fileName	= QDir::toNativeSeparators (fileName);
	sf_info		-> samplerate	= 48000;
	sf_info		-> channels	= 2;
	sf_info		-> format	= SF_FORMAT_WAV | SF_FORMAT_PCM_16;

	SNDFILE *theFile	= sf_open (fileName. toUtf8(). data(),
	                                   SFM_WRITE, sf_info);
	if (theFile == nullptr) {
	   qDebug() << "Cannot open " << fileName. toUtf8(). data();
	   return nullptr;
	}

	QString	dumper	= QDir::fromNativeSeparators (fileName);
	int x		= dumper. lastIndexOf ("/");
	saveDir		= dumper. remove (x, dumper. count () - x);
	dabSettings	-> setValue ("saveDir_audioDump", saveDir);

	return theFile;
}

SNDFILE *findfileNames::findRawDump_fileName (const QString &deviceName,
	                                       const QString &channelName) {
SF_INFO *sf_info        = (SF_INFO *)alloca (sizeof (SF_INFO));
QString theTime		= QDateTime::currentDateTime (). toString ();
QString	saveDir		= dabSettings -> value ("saveDir_rawDump",
	                                        QDir::homePath ()). toString ();
SNDFILE	*theFile;
	for (int i = 0; i < theTime. length (); i ++)
	   if (!isValid (theTime. at (i)))
	      theTime. replace (i, 1, '-');


	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';
	QString suggestedFileName = saveDir +
		                    deviceName + "-" +
	                            channelName + "-" + theTime + ".sdr";
	QString fileName = QFileDialog::
	                     getSaveFileName (nullptr,
	                                      "Save File",
	                                      suggestedFileName,
	                                      "raw data (*.sdr)",
	                                      Q_NULLPTR,
	                                      QFileDialog::DontUseNativeDialog);

	if (fileName == QString (""))       // apparently cancelled
	   return nullptr;

	if (!fileName.endsWith (".sdr", Qt::CaseInsensitive))
	   fileName.append (".sdr");

	fileName	= QDir::toNativeSeparators (fileName);
	sf_info -> samplerate   = INPUT_RATE;
	sf_info -> channels     = 2;
	sf_info -> format       = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	theFile = sf_open (fileName. toUtf8 (). data(),
	                                   SFM_WRITE, sf_info);
	fprintf (stderr, "the file %s is open?\n", 
	                              fileName. toUtf8 (). data ());
	if (theFile == nullptr) {
	   fprintf (stderr, "foute boel\n");
	   qDebug() << "cannot open " << fileName. toUtf8(). data();
	   return nullptr;
	}

	QString dumper	= QDir::fromNativeSeparators (fileName);
	int x		= dumper. lastIndexOf ("/");
	saveDir		= dumper. remove (x, dumper. count () - x);
	dabSettings	-> setValue ("saveDir_rawDump", saveDir);
	return theFile;
}

FILE	*findfileNames::findScanDump_fileName		() {
	QMessageBox::StandardButton resultButton =
	             QMessageBox::question (nullptr, "Qt-DAB",
	                                    "save the scan?\n",
	                                    QMessageBox::No | QMessageBox::Yes,
	                                    QMessageBox::Yes);
	if (resultButton != QMessageBox::Yes)
	   return nullptr;

	QString   saveDir = dabSettings -> value ("contentDir",
	                                        QDir::homePath ()). toString ();

	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	QString theTime = QDateTime::currentDateTime (). toString ();
	for (int i = 0; i < theTime. length (); i ++)
	   if (!isValid (theTime. at (i)))
	      theTime. replace (i, 1, '-');
	QString suggestedFileName =
	                       saveDir + "Qt-DAB-scan" + "-" + theTime + ".csv";

	QString fileName = QFileDialog::
	                     getSaveFileName (nullptr,
	                                      "Save file ...",
	                                      suggestedFileName,
	                                      "csv (*.csv)",
	                                      Q_NULLPTR,
	                                      QFileDialog::DontUseNativeDialog);
	if (fileName == nullptr) // canceled?
	   return nullptr;
	fileName	= QDir::toNativeSeparators (fileName);
	return  fopen (fileName. toUtf8 (). data (), "w");
}

FILE	*findfileNames::findSummary_fileName	() {
	QMessageBox::StandardButton resultButton =
	             QMessageBox::question (nullptr, "Qt-DAB",
	                                    "save summary?\n",
	                                    QMessageBox::No | QMessageBox::Yes,
	                                    QMessageBox::Yes);
	if (resultButton != QMessageBox::Yes)
	   return nullptr;

	QString   saveDir = dabSettings -> value ("contentDir",
	                                        QDir::homePath ()). toString ();

	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	QString theTime = QDateTime::currentDateTime (). toString ();
	for (int i = 0; i < theTime. length (); i ++)
	   if (!isValid (theTime. at (i)))
	      theTime. replace (i, 1, '-');
	QString suggestedFileName =
	                  saveDir + "Qt-DAB-summary" + "-" + theTime + ".csv";

	QString fileName = QFileDialog::
	                     getSaveFileName (nullptr,
	                                      "Save file ...",
	                                      suggestedFileName,
	                                      "csv (*.csv)",
	                                      Q_NULLPTR,
	                                      QFileDialog::DontUseNativeDialog);
	if (fileName == nullptr)	// canceled ?
	   return nullptr;
	fileName	= QDir::toNativeSeparators (fileName);
	return  fopen (fileName. toUtf8 (). data (), "w");
}

const
QString	findfileNames::findskipFile_fileName	() {
	QString   saveDir = dabSettings -> value ("contentDir",
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
	if (fileName == nullptr)	// canceled?
	   return nullptr;
	fileName	= QDir::toNativeSeparators (fileName);
	return  fileName;
}

QString findfileNames::finddlText_fileName	(bool flag) {
QString   saveDir = dabSettings -> value ("contentDir",
                                                QDir::homePath ()). toString ();
QString theTime         = QDateTime::currentDateTime (). toString ();

        if ((saveDir != "") && (!saveDir. endsWith ('/')))
           saveDir = saveDir + '/';

	for (int i = 0; i < theTime. length (); i ++)
	   if (!isValid (theTime. at (i)))
	      theTime. replace (i, 1, '-');

        QString suggestedFileName = saveDir + "Qt-DAB-dlText"  +
                                                  "-" + theTime + ".txt";
        QString fileName;

	if (flag)
	   fileName = 
              QFileDialog::getSaveFileName (nullptr,
                                            "Save File",
                                            suggestedFileName. toUtf8 (). data (),
                                            "Text (*.txt)",
	                                    Q_NULLPTR,
	                                    QFileDialog::DontUseNativeDialog);
	else
	   fileName = suggestedFileName;
	if (fileName == nullptr)	// canceled
	   return nullptr;
        fileName        = QDir::toNativeSeparators (fileName);
        return  fileName;
}

FILE	* findfileNames::findLogFileName	() {
QString   saveDir = dabSettings -> value ("contentDir",
                                                QDir::homePath ()). toString ();
QString theTime         = QDateTime::currentDateTime (). toString ();

        if ((saveDir != "") && (!saveDir. endsWith ('/')))
           saveDir = saveDir + '/';

	for (int i = 0; i < theTime. length (); i ++)
	   if (!isValid (theTime. at (i)))
	      theTime. replace (i, 1, '-');

        QString suggestedFileName = saveDir + "Qt-DAB-LOG"  +
                                                  "-" + theTime + ".txt";
	QString fileName = 
              QFileDialog::getSaveFileName (nullptr,
                                            "Save File",
                                            suggestedFileName. toUtf8 (). data (),
                                            "Text (*.txt)",
	                                    Q_NULLPTR,
	                                    QFileDialog::DontUseNativeDialog);
	if (fileName == "")
	   return nullptr;	
	return fopen (fileName. toUtf8 (). data (), "w");
}

FILE	* findfileNames::find_ficDump_file	(const QString &channel) {
QString	saveDir = dabSettings -> value ("contentDir",
                                              QDir::homePath ()). toString ();
QString theTime	= QDateTime::currentDateTime (). toString ();

	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	for (int i = 0; i < theTime. length (); i ++)
	   if (!isValid (theTime. at (i)))
	      theTime. replace (i, 1, '-');

	QString fileName = saveDir + channel  + "-" + theTime + ".fic";
	return fopen (fileName. toUtf8 (). data (), "w+b");
}

QString	findfileNames::findMaps_fileName () {
QString	saveDir		= dabSettings -> value ("contentDir",
	                                        QDir::homePath ()). toString ();
QString theTime         = QDateTime::currentDateTime (). toString ();
QString suggestedFileName;

	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	for (int i = 0; i < theTime. length (); i ++)
	   if (!isValid (theTime. at (i)))
	      theTime. replace (i, 1, '-');
	suggestedFileName = saveDir + "Qt-DAB-Transmitters" +
	                                          "-" + theTime + ".csv";
	suggestedFileName        = QDir::toNativeSeparators (suggestedFileName);

	QString fileName = QFileDialog::
	                     getSaveFileName (nullptr,
	                                      "Save file ...",
	                                      suggestedFileName,
	                                      "txt (*.txt)",
	                                      Q_NULLPTR,
	                                      QFileDialog::DontUseNativeDialog);
	if (fileName == "")
	   return "";

	return QDir::toNativeSeparators (fileName);
}

QString	findfileNames::find_eti_fileName (const QString &ensemble,
	                                  const QString &channelName) {
QString	saveDir	 = dabSettings -> value ("contentDir",
	                                   QDir::homePath ()). toString ();
QString theTime         = QDateTime::currentDateTime (). toString ();
QString suggestedFileName;

	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	suggestedFileName	=  channelName + "-" +
	                                 ensemble. trimmed () + "-" + theTime;
	for (int i = 0; i < suggestedFileName. length (); i ++) 
	   if (!isValid (suggestedFileName. at (i)))
	      suggestedFileName. replace (i, 1, '-');
	suggestedFileName	= saveDir + suggestedFileName + ".eti";
	suggestedFileName	= QDir::toNativeSeparators (suggestedFileName);

	QString fileName = QFileDialog::
	                     getSaveFileName (nullptr,
	                                      "Save file ...",
	                                      suggestedFileName,
	                                      "eti (*.eti)",
	                                      Q_NULLPTR,
	                                      QFileDialog::DontUseNativeDialog);
	if (fileName == "")
	   return "";

	return QDir::toNativeSeparators (fileName);
}
