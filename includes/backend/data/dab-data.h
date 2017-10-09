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
#ifndef	__DAB_DATA__
#define	__DAB_DATA__

#include	<QSemaphore>
#include	"dab-virtual.h"
#include	<QThread>
#include	"ringbuffer.h"
#include	<stdio.h>
#include	<string.h>


class	frameProcessor;
class	RadioInterface;
class	protection;

class	dabData:public QThread, public dabVirtual {
Q_OBJECT
public:
	dabData		(RadioInterface *mr,
	                 uint8_t	DSCTy,
	                 int16_t	packetAddress,
	                 int16_t	appType,
	                 int16_t	fragmentSize,
	                 int16_t	bitRate,
	                 bool		shortForm,
	                 int16_t	protLevel,
	                 uint8_t	DGflag,
	                 int16_t	FEC_scheme,
	                 RingBuffer<uint8_t> *dataBuffer,
	                 QString	picturesPath);
	~dabData	(void);
int32_t	process		(int16_t *, int16_t);
void	stopRunning	(void);
private:
	RadioInterface	*myRadioInterface;
	uint8_t		DSCTy;
	int16_t		packetAddress;
	int16_t		fragmentSize;
	int16_t		bitRate;
	bool		shortForm;
	int16_t		protLevel;
	uint8_t		DGflag;
	int16_t		FEC_scheme;
	RingBuffer<uint8_t>	*dataBuffer;
void	run		(void);
	volatile bool	running;
	int32_t		countforInterleaver;
	uint8_t		*outV;
	int16_t		**interleaveData;
	int16_t		*Data;
	protection	*protectionHandler;
	RingBuffer<int16_t>	*Buffer;
	frameProcessor	*our_frameProcessor;
	QSemaphore      freeSlots;
	QSemaphore      usedSlots;
	int16_t         *theData [20];
	int16_t         nextIn;
	int16_t         nextOut;

//
signals:
	void		show_mscErrors		(int);
};

#endif

