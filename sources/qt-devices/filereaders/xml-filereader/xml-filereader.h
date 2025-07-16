#
/*
 *    Copyright (C) 2013 .. 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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

#include	<QObject>
#include	<QString>
#include	<QFrame>
#include	<QSettings>
#include	<QScopedPointer>
#include	<atomic>
#include	"dab-constants.h"
#include	"device-handler.h"
#include	"ringbuffer.h"
#include	"ui_xmlfiles.h"

class	QSettings;
class	xmlDescriptor;
class	xml_Reader;
/*
 */
class	xml_fileReader: public deviceHandler, public Ui_xmlfile_widget {
Q_OBJECT
public:
				xml_fileReader	(QSettings *, const QString &);
                		~xml_fileReader	();
	bool			restartReader	(int32_t, int skipped = 0);
	void			stopReader	();
	int32_t			getSamples	(std::complex<float> *,
	                                                         int32_t);
	int32_t			Samples		();
	QString			deviceName	();
	bool			isFileInput	();
	int			getVFOFrequency	();
private:
	QSettings		*xmlFilesSettings;
	QString			fileName;
	std::atomic<bool>	running;
	RingBuffer<std::complex<float>>	_I_Buffer;
	FILE			*theFile;
	uint64_t		filePointer;
	QScopedPointer<xmlDescriptor>	theDescriptor;
	QScopedPointer<xml_Reader> theReader;
public slots:
	void			setProgress	(int, int);
	void			handle_continuousButton ();
};

