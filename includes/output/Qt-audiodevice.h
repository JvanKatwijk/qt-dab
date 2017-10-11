
#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB (formerly SDR-J, JSDR).
 *    Many of the ideas as implemented in Qt-DAB are derived from
 *    other work, made available through the GNU general Public License.
 *    All copyrights of the original authors are acknowledged.
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
 *
 */
#ifndef	__QT_AUDIODEVICE__
#define	__QT_AUDIODEVICE__

#include	<QIODevice>
#include	<QObject>
#include	"dab-constants.h"
#include	"ringbuffer.h"

class Qt_AudioDevice : public QIODevice {
Q_OBJECT
public:
		Qt_AudioDevice	(RingBuffer<float> *, QObject *);
		~Qt_AudioDevice	(void);

	void	start		(void);
	void	stop		(void);

	qint64	readData	(char *data, qint64 maxlen);
	qint64	writeData	(const char *data, qint64 len);

private:
	RingBuffer<float> *Buffer;
};
#endif

