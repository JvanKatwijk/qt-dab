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
#ifndef	__DAB_AUDIO__
#define	__DAB_AUDIO__

#include	<QSemaphore>
#include	"dab-virtual.h"
#include	<QThread>
#include	"ringbuffer.h"
#include	<stdio.h>

class	dabProcessor;
class	protection;
class	RadioInterface;

class	dabAudio:public QThread, public dabVirtual {
public:
	dabAudio	(RadioInterface	*mr,
	                 uint8_t dabModus,
	                 int16_t fragmentSize,
	                 int16_t bitRate,
	                 bool	shortForm,
	                 int16_t protLevel,
	                 RingBuffer<int16_t> *,
	                 QString	picturesPath);
	~dabAudio	(void);
int32_t	process		(int16_t *, int16_t);
void	stopRunning	(void);
protected:
	RadioInterface	*myRadioInterface;
	RingBuffer<int16_t>	*audioBuffer;
private:
void	run		(void);
volatile bool		running;
	uint8_t		dabModus;
	int16_t		fragmentSize;
	int16_t		bitRate;
	bool		shortForm;
	int16_t		protLevel;
	uint8_t		*outV;
	int16_t		**interleaveData;
	int16_t		*Data;
	int16_t		*tempX;

	protection	*protectionHandler;
	dabProcessor	*our_dabProcessor;
	QSemaphore	freeSlots;
	QSemaphore	usedSlots;
	int16_t		*theData [20];
	int16_t		nextIn;
	int16_t		nextOut;
};

#endif

