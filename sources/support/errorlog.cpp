#
/*
 *    Copyright (C)  2015, 2023
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

#include	"errorlog.h"
#include	"findfilenames.h"
#include	<QDir>

		errorLogger::errorLogger	(QSettings *settings) {
findfileNames fileNameFinder (settings);

	this	-> settings	= settings;
	this	-> logFile	= nullptr;
	logFileName		= fileNameFinder. basicPath ();
	if (!logFileName. endsWith ('/'))
	   logFileName += '/';
	logFileName	+= "errorlog";
	logFileName	= QDir::toNativeSeparators (logFileName);
	logFile			= fopen (logFileName. toLatin1(). data (), "a");
}
		errorLogger::~errorLogger	() {
	if (logFile != nullptr)
	   fclose (logFile);
}
void	errorLogger::add		(const QString deviceName,
	                                  const QString theError) {
	if (logFile == nullptr)
	   return;
	QDateTime dateTime	= QDateTime::currentDateTime ();
	QString date		= dateTime. toString (Qt::ISODate);
	QString theText		= deviceName + " at " + date +
	                                        " -> " + theError;
	fprintf (logFile, "%s \n", theText. toLatin1 (). data ());
}

