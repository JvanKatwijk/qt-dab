#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB.
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
#
#ifndef	__BACKEND__
#define	__BACKEND__

#include	<QSemaphore>
#include	<vector>
#ifdef	__THREADED_BACKEND
#include	<QThread>
#include	<atomic>
#endif
#include	"ringbuffer.h"
#include	<stdio.h>
#include        "backend-driver.h"
#include        "backend-deconvolver.h"

class	RadioInterface;

#ifdef	__THREADED_BACKEND
class	Backend:public QThread {
#else
class	Backend {
#endif
public:
	Backend	(RadioInterface	*mr,
	         descriptorType	*d,
	         RingBuffer<int16_t> *,
	         RingBuffer<uint8_t> *,
	         QString	picturesPath);
	~Backend	(void);
int32_t	process		(int16_t *, int16_t);
void	stopRunning	(void);
	int		startAddr;
	int		Length;
private:
#ifdef	__THREADED_BACKEND
void	run		(void);
	atomic<bool>	running;
	QSemaphore	freeSlots;
	QSemaphore	usedSlots;
	std::vector<int16_t>	theData [20];
	int16_t		nextIn;
	int16_t		nextOut;
#endif
void	processSegment	(int16_t *Data);

	int16_t		bitRate;
	int16_t		fragmentSize;
	std::vector<uint8_t> outV;
	std::vector<std::vector <int16_t>> interleaveData;
	std::vector<int16_t> tempX;
	int16_t		countforInterleaver;
	int16_t		interleaverIndex;
	std::vector<uint8_t> disperseVector;

	backendDriver		driver;
	backendDeconvolver	deconvolver;
};

#endif

