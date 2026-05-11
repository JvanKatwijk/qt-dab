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
#include	"settingNames.h"
#include	"settings-handler.h"

	logger::logger (QSettings *s):
	                  filenameFinder (s) {
	this	-> logSettings	= s;

	QString tempPath	= filenameFinder. basicPath ();
	value_s (logSettings, "LOG_FILE", "logFile", tempPath);
	this	-> logFileName	= tempPath + "logFile.txt";
	logFile 	= fopen (logFileName. toLatin1 (). data (), "a");
}

	logger::~logger	() {
	if (logFile != nullptr)
	   fclose (logFile);
}

void	logger::log	(const QString & s1, const QString & s2) {
	QString theTime = QDateTime::currentDateTime (). toString ();
	fprintf (logFile, "%s\t: %s %s %s\n",
	                 theTime. toLatin1 (). data (), "service starts",
	                  s1. toLatin1 (). data (),
	                  s2. toLatin1 (). data ());
}

