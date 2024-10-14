#
/*
 *    Copyright (C)  2015, 2024
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

#include	"logger.h"
#include	<QDateTime>
#include	<QDir>

	logger::logger (QSettings *s):
	                                     fileNameFinder (s) {
	this	-> logSettings	= s;
	QString tempPath	= fileNameFinder. basicPath ();
	tempPath		= s -> value ("logFile", tempPath). toString ();
	this	-> logFileName	= tempPath + "logFile.txt";
	this	-> logMode	= s -> value ("logMode", 1). toInt () != 0;
	if (logMode)
	   logFile 	= fopen (logFileName. toLatin1 (). data (), "a");
	else
	   logFile	= nullptr;
	fprintf (stderr, "file is %s open\n", logFile == nullptr ? "niet" : "");
}

	logger::~logger	() {
	if (logFile != nullptr)
	   fclose (logFile);
}

void	logger::logging_starts	() {
	if (logFile != nullptr)		// cannot happen
	   return;

	this -> logSettings  -> setValue ("logMode", 1);
	logFile	= fopen (logFileName. toLatin1 (). data (), "a");
}

void	logger::logging_stops	() {
	if (logFile != nullptr)
	   fclose (logFile);
	logFile	= nullptr;
	this -> logSettings -> setValue ("logMode", 0);
}

void	logger::log	(logType t) {
	if (logFile == nullptr)
	   return;

	QString theTime = QDateTime::currentDateTime (). toString ();
	switch (t) {
	   case LOG_RADIO_STOPS:
	      fprintf (logFile, "%s\t: %s\n", theTime. toLatin1 (). data (),
	                                                "Radio stops");
	      break;
	   case LOG_FRAMEDUMP_STOPS:
	      fprintf (logFile, "%s\t: %s\n", theTime. toLatin1 (). data (),
	                                                 "framedump stops");
	      break;
	   case LOG_AUDIODUMP_STOPS:
	      fprintf (logFile, "%s\t: %s\n", theTime. toLatin1 (). data (),
	                                                "Audiodump stops");
	      break;
	   case LOG_SOURCEDUMP_STOPS:
	      fprintf (logFile, "%s\t: %s\n", theTime. toLatin1 (). data (),
	                                                 "sourcedump stops");
	      break;
	   case LOG_SCANNING_STOPS:
	      fprintf (logFile, "%s\t: %s\n", theTime. toLatin1 (). data (),
	                                                 "scanningdump stops");
	      break;
	   case LOG_ETI_STOPS:
	      fprintf (logFile, "%s\t: %s\n", theTime. toLatin1 (). data (),
	                                                 "ETI processing stops");
	      break;

	   default:;
	}
}
	
void	logger::log	(logType, const QString &) {}
void	logger::log	(logType t, const QString &channel, int snr) {
	if (logFile == nullptr)
	   return;
	QString theTime = QDateTime::currentDateTime (). toString ();
	switch (t) {
	   case LOG_NEW_CHANNEL:
	      fprintf (logFile, "%s\t: %s %s %d\n",
	                 theTime. toLatin1 (). data (), "new channel active",
	                  channel. toLatin1 (). data (), snr);
	   default:;
	}
	
}
void	logger::log	(logType t, const QString & s1,
	                            const QString & s2) {
	if (logFile == nullptr)
	   return;
	QString theTime = QDateTime::currentDateTime (). toString ();
	switch (t) {
	   case LOG_NEW_SERVICE:	//channel and service
	      fprintf (logFile, "%s\t: %s %s %s\n",
	                 theTime. toLatin1 (). data (), "service starts",
	                  s1. toLatin1 (). data (),
	                  s2. toLatin1 (). data ());
	      break;

	   case LOG_RADIO_STARTS:	// device and channel
	      fprintf (logFile, "%s\t: %s %s %s\n",
	                 theTime. toLatin1 (). data (), "Radio starts",
	                  s1. toLatin1 (). data (),
	                  s2. toLatin1 (). data ());
	      break;

	   case LOG_SOURCEDUMP_STARTS:	// device and channel
	      fprintf (logFile, "%s\t: %s %s %s\n",
	                 theTime. toLatin1 (). data (), "sourcedump starts",
	                  s1. toLatin1 (). data (),
	                  s2. toLatin1 (). data ());
	      break;

	   case LOG_AUDIODUMP_STARTS:	// channel and service
	      fprintf (logFile, "%s\t: %s %s %s\n",
	                 theTime. toLatin1 (). data (), "audiodump starts",
	                  s1. toLatin1 (). data (),
	                  s2. toLatin1 (). data ());
	      break;

	   case LOG_FRAMEDUMP_STARTS:	// channel and service
	      fprintf (logFile, "%s\t: %s %s %s\n",
	                 theTime. toLatin1 (). data (), "framedump starts",
	                  s1. toLatin1 (). data (),
	                  s2. toLatin1 (). data ());
	      break;

	   default:;
	}
}

