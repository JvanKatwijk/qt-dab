#
/*
 *    Copyright (C) 2015
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB
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
#
#pragma once

#include	<vector>
#include	<cstdio>
#include	<cstring>
#include	<QObject>
#include	"frame-processor.h"
#include	"ringbuffer.h"

class	RadioInterface;
class	virtual_dataHandler;
class	packetdata;

class	dataProcessor final :public QObject, public frameProcessor {
Q_OBJECT
public:
	dataProcessor	(RadioInterface *mr,
	                 packetdata	*pd,
	                 RingBuffer<uint8_t>	*dataBuffer,
	                 bool backendFlag);
	~dataProcessor	();
void	addtoFrame	(const std::vector<uint8_t>);
private:
	RadioInterface	*myRadioInterface;
	int16_t		bitRate;
	uint8_t		DSCTy;
	int16_t		appType;
	int16_t		packetAddress;
	uint8_t		DGflag;
	int16_t		FEC_scheme;
	RingBuffer<uint8_t>* dataBuffer;
	int16_t		expectedIndex;
	std::vector<uint8_t>	series;
	uint8_t		packetState;
	int32_t		streamAddress;		// int since we init with -1
//
//	result handlers
	void		handleTDCAsyncstream 	(const uint8_t *, int32_t);
	void		handlePackets		(const uint8_t *, int32_t);
	void		handlePacket		(const uint8_t *);
	virtual_dataHandler *my_dataHandler;
//
signals:
	void		show_mscErrors		(int);
};


