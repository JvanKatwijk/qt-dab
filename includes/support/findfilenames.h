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
//
//	just a convenience class to pack a number of functions
//	dealing with getting a filename
#include	<QSettings>
#include	<QString>
#include	<QDateTime>
#include	<QDir>
#include	<QFileDialog>
#include	<QMessageBox>
#include	<stdio.h>
#include	<sndfile.h>

class	findfileNames {
	QSettings	*dabSettings;
public:
	findfileNames			(QSettings *);
	~findfileNames			();
FILE	*findContentDump_fileName	(const QString &channel);
FILE	*findFrameDump_fileName		(const QString &service, bool);
SNDFILE	*findAudioDump_fileName		(const QString &service);
SNDFILE *findRawDump_fileName		(const QString &deviceName,
	                                       const QString &channelName);
FILE	*findScanDump_fileName		();
FILE	*findSummary_fileName		();
const 
QString	findskipFile_fileName		();
QString	finddlText_fileName		();
};

