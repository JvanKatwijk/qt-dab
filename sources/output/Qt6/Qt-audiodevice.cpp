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
	                        QObject* parent):
	                                 QIODevice (parent),
	                                 Buffer (32 * 32768) {

	(void)mr;
	totalBytes_l	= 0;
	missedBytes_l	= 0;
	connect (this, &QIODevice::readyRead,
	         this, &Qt_AudioDevice::print_readyRead);
	connect (this, &QIODevice::readChannelFinished,
	         this, &Qt_AudioDevice::print_readChannelFinished);
	connect (this, &QIODevice::channelReadyRead,
	         this, &Qt_AudioDevice::print_channelReadyRead);
	connect (this, &QIODevice::aboutToClose,
	         this, &Qt_AudioDevice::print_aboutToClose);
	running	= false;
	start ();
}

Qt_AudioDevice::~Qt_AudioDevice () {
//	fprintf (stderr, "Closing QIODevice\n");
	if (running)
	   close ();
}

void	Qt_AudioDevice::start () {
	fprintf (stderr, "restarting QIODevice with buffer %d\n",
	                               Buffer. GetRingBufferReadAvailable ());
	(void)open (QIODevice::ReadOnly);
	running	= true;
}

void	Qt_AudioDevice::stop () {
	fprintf (stderr, "Closing QIODevice\n");
	Buffer. FlushRingBuffer();
	close ();
	running = false;
}

void	Qt_AudioDevice::suspend	() {
	Buffer. FlushRingBuffer ();
}

void	Qt_AudioDevice::resume	() {
	Buffer. FlushRingBuffer ();
}

void	Qt_AudioDevice::putData	(float *fragment, int32_t size) {
	if (running)
	   Buffer. putDataIntoBuffer ((char *)fragment,
                                               sizeof (float) * size);
}

//	we always return "maxSize" bytes
qint64	Qt_AudioDevice::readData (char* buffer, qint64 maxSize) {
qint64	amount = 0;
//	"maxSize" is the requested size in bytes
//	"amount" is in uint8_t's
	amount = Buffer. getDataFromBuffer (buffer, maxSize);
	if (amount < maxSize) {
	   for (int i = amount; i < maxSize; i ++)
	      buffer [i] = (char)(0); 
	}

	totalBytes_l	+= maxSize;
	missedBytes_l	+= maxSize - amount;
	return maxSize;
}


bool	Qt_AudioDevice::isRunning () {
	return running;
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

void	Qt_AudioDevice::print_readyRead () {
}

void	Qt_AudioDevice::print_readChannelFinished () {
}

void	Qt_AudioDevice::print_channelReadyRead (int channel) {
	(void)channel;
}

void	Qt_AudioDevice::print_aboutToClose () {
}

qint64  Qt_AudioDevice::bytesAvailable  () const {
        return  32768;
}

qint64  Qt_AudioDevice::size            () const {
        return 32768;
}


