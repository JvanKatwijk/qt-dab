#
/*
 *    Copyright (C) 2017 .. 2024
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

//
//	Note: this class is only referenced to in the Qt6 version
//	NOT in the Qt5 version
//
//	Create a "device"
Qt_AudioDevice::Qt_AudioDevice (RadioInterface *mr,
	                        RingBuffer<char>* Buffer_i,
	                        QObject* parent)
	                               : QIODevice (parent),
	                                 Buffer (Buffer_i) {

	totalBytes_l	= 0;
	missedBytes_l	= 0;
	running. store (false);
//	start ();
}

Qt_AudioDevice::~Qt_AudioDevice () {
}

void	Qt_AudioDevice::start () {
	if (running. load ())
	   return;
	bool b = open (QIODevice::ReadOnly);
	fprintf (stderr, "Opening QIODevice %s\n", b ? "ok" : "error");
	fprintf (stderr, "current channel %d %d\n",
	                        currentReadChannel (), currentWriteChannel ());
	running. store (true);
}

//	we always return "maxSize" bytes
qint64	Qt_AudioDevice::readData (char* buffer, qint64 maxSize) {
qint64	amount = 0;
static int64_t teller = 0;
//	"maxSize" is the requested size in bytes
//	"amount" is in uint8_t's
	amount = Buffer -> getDataFromBuffer (buffer, maxSize);
	if (amount < maxSize) {
	   for (int i = amount; i < maxSize; i ++)
	      buffer [i] = (char)(0); 
	}

	totalBytes_l	+= amount;
	missedBytes_l	+= maxSize - amount;
	return maxSize;
}

void	Qt_AudioDevice::stop () {
	Buffer -> FlushRingBuffer();
	close ();
	running. store (false);
}
void	Qt_AudioDevice::samplesMissed (int &total, int &too_short) {
	total		= totalBytes_l / (2 * sizeof (float));
	too_short	= missedBytes_l / (2 * sizeof (float));
	totalBytes_l	= 0;
	missedBytes_l	= 0;
}

//
//
//	usused here
qint64	Qt_AudioDevice::writeData (const char* data, qint64 len) {
	Q_UNUSED (data);
	Q_UNUSED (len);
	return 0;
}

