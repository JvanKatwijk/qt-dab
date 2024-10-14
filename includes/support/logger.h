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

#pragma once
#include	<stdio.h>
#include	<stdint.h>
#include	<QString>
#include	<QSettings>
#include	"findfilenames.h"

class logger {
public:
	enum logType {LOG_RADIO_STARTS, LOG_RADIO_STOPS,
	      LOG_NEWDEVICE, LOG_NEW_CHANNEL, LOG_NEW_SERVICE,
	      LOG_SOURCEDUMP_STARTS, LOG_SOURCEDUMP_STOPS,
	      LOG_AUDIODUMP_STARTS, LOG_AUDIODUMP_STOPS,
	      LOG_FRAMEDUMP_STARTS, LOG_FRAMEDUMP_STOPS,
	      LOG_HIDDEN_SERVICE_STARTS, 
	      LOG_SCANNING_STARTS, LOG_SCANNING_STOPS,
	      LOG_ETI_STARTS, LOG_ETI_STOPS};
		logger	(QSettings *);
		~logger	();
	void	logging_starts	();
	void	logging_stops	();
	void	log	(logType);
	void	log	(logType, const QString &);
	void	log	(logType, const QString &, int);
	void	log	(logType, const QString &, const QString &);
private:
	findfileNames	fileNameFinder;
	QString	logFileName;
	FILE	*logFile;
	bool	logMode;
	QSettings	*logSettings;
};

	
