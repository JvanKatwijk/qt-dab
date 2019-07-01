#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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

#ifndef	__LIME_READER__
#define	__LIME_READER__

#include	<QThread>
#include	"ringbuffer.h"
#include	<lime/LimeSuite.h>
#include	<complex>
#include	<atomic>

class	limeHandler;

class	limeReader:public QThread {
Q_OBJECT
public:
		limeReader	(lms_device_t *,
	                         RingBuffer<std::complex<float>> *,
	                         limeHandler *);
		~limeReader	(void);
private:
	lms_stream_meta_t meta;
	lms_stream_t    stream;
	lms_device_t	*device;
	limeHandler	*theBoss;
	void	run		(void);
	RingBuffer<std::complex<float>> *buffer;
	std::atomic<bool>	running;
signals:
	void	showErrors	(int, int);
};

#endif

