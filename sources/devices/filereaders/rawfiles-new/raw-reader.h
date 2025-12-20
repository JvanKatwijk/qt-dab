#
/*
 *    Copyright (C) 2013 .. 2024
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

#include	<QThread>
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	<atomic>
class	rawFiles;

#define RAW_BUFFERSIZE      32768

class	rawReader:public QThread {
Q_OBJECT
public:
			rawReader	(rawFiles *,
	                                 FILE *,
	                                 RingBuffer<std::complex<float>> *); 
			~rawReader	();
	void		startReader	();
	void		stopReader	();
	void		handle_progressSlider	(int);
private:
virtual void		run		();
	FILE		*filePointer;
	RingBuffer<std::complex<float>>	*_I_Buffer;
	uint64_t	period;
	std::atomic<bool>	running;
	std::atomic<int>	newPosition;
	uint8_t		rawDataBuffer [RAW_BUFFERSIZE];;
	rawFiles	*parent;
	int64_t		fileLength;
	float		mapTable [256];
signals:
	void		setProgress	(int, float);
};

