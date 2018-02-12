#
/*
 *    Copyright (C) 2015
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
#
#ifndef	__DATA_BACKEND__
#define	__DATA_BACKEND__

#include	<QSemaphore>
#include	<vector>
#include	<QThread>
#include	"ringbuffer.h"
#include	<stdio.h>
#include	<string.h>
#include	"dab-constants.h"
#include	"virtual-backend.h"

class	dataProcessor;
class	RadioInterface;
class	protection;

class	dataBackend:public QThread, public virtualBackend {
Q_OBJECT
public:
	dataBackend	(RadioInterface *mr,
	                 packetdata	*d,
	                 RingBuffer<uint8_t> *dataBuffer,
	                 QString	picturesPath);
	~dataBackend	(void);
int32_t	process		(int16_t *, int16_t);
void	stopRunning	(void);
private:
	RadioInterface	*myRadioInterface;
	bool		shortForm;
	int16_t		protLevel;
	uint8_t		DGflag;
	int16_t		FEC_scheme;
	std::vector<uint8_t> disperseVector;
void	run		(void);
	volatile bool	running;
	int32_t		countforInterleaver;
	uint8_t		* outV;
	int16_t		**interleaveData;
	int16_t		*Data;
	protection	*protectionHandler;
	RingBuffer<int16_t>	*Buffer;
	dataProcessor	*our_frameProcessor;
	QSemaphore      freeSlots;
	QSemaphore      usedSlots;
	int16_t         *theData [20];
	int16_t         nextIn;
	int16_t         nextOut;
        int		fragmentSize;
        int16_t		bitRate;
};

#endif

