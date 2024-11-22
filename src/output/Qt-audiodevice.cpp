#
/*
 *    Copyright (C) 2014 .. 2017
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

#include	"Qt-audiodevice.h"
#include	<QtGlobal>
#include	"radio.h"
#if QT_VERSION >= 0x060000
//
//	Create a "device"
Qt_AudioDevice::Qt_AudioDevice (RadioInterface *mr,
	                        RingBuffer<char>* Buffer_i,
	                        QObject* parent)
	                               : QIODevice (parent),
	                                 Buffer (Buffer_i) {

	totalBytes	= 0;
	missedBytes     = 0;
	start ();
}

Qt_AudioDevice::~Qt_AudioDevice () {
}

void	Qt_AudioDevice::start () {
	open (QIODevice::ReadOnly);
}

void	Qt_AudioDevice::stop () {
	Buffer -> FlushRingBuffer();
	close ();
}

void	Qt_AudioDevice::samplesMissed (int &total, int &too_short) {
	total		= totalBytes / (2 * sizeof (float));
	too_short	= missedBytes / (2 * sizeof (float));

	totalBytes	= 0;
	missedBytes		= 0;
}

//
//	we always return "maxSize" bytes
qint64	Qt_AudioDevice::readData (char* buffer, qint64 maxSize) {
qint64	amount = 0;

//	"maxSize" is the requested size in bytes
//	"amount" is in floats
	amount = Buffer -> getDataFromBuffer (buffer, maxSize);

	if (amount < maxSize) {
	   for (int i = amount; i < maxSize; i ++) 
	      buffer [i] = 0;
	}

	totalBytes	+= amount;
	missedBytes	+= maxSize - amount;
	return maxSize;
}
//
//	usused here
qint64	Qt_AudioDevice::writeData (const char* data, qint64 len) {
	Q_UNUSED (data);
	Q_UNUSED (len);
	return 0;
}
#endif

