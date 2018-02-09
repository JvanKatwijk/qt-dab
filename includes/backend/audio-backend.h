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
#ifndef	__AUDIO_BACKEND__
#define	__AUDIO_BACKEND__

#include	<QSemaphore>
#include	"virtual-backend.h"
#include	<vector>
#ifdef	__THREADED_BACKEND
#include	<QThread>
#include	<atomic>
#endif
#include	"ringbuffer.h"
#include	<stdio.h>

class	frameProcessor;
class	protection;
class	RadioInterface;

#ifdef	__THREADED_BACKEND
class	audioBackend:public QThread, public virtualBackend {
#else
class	audioBackend:public virtualBackend {
#endif
public:
	audioBackend	(RadioInterface	*mr,
	                 audiodata	*d,
	                 RingBuffer<int16_t> *,
	                 QString	picturesPath);
	~audioBackend	(void);
int32_t	process		(int16_t *, int16_t);
void	stopRunning	(void);
protected:
	RadioInterface	*myRadioInterface;
	RingBuffer<int16_t>	*audioBuffer;
private:
#ifdef	__THREADED_BACKEND
void	run		(void);
	atomic<bool>	running;
	QSemaphore	freeSlots;
	QSemaphore	usedSlots;
	int16_t		*theData [20];
	int16_t		nextIn;
	int16_t		nextOut;
#endif
void	processSegment	(int16_t *Data);

	uint8_t		dabModus;
	int16_t		fragmentSize;
	int16_t		bitRate;
	bool		shortForm;
	int16_t		protLevel;
	std::vector<uint8_t> outV;
	int16_t		**interleaveData;
	std::vector<int16_t> tempX;
	int16_t		countforInterleaver;
	int16_t		interleaverIndex;
	std::vector<uint8_t> disperseVector;

	protection	*protectionHandler;
	frameProcessor	*our_dabProcessor;
};

#endif

