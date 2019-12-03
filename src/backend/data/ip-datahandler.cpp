#
/*
 *    Copyright (C) 2015 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB
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
#include	"ip-datahandler.h"
#include	"radio.h"

	ip_dataHandler::ip_dataHandler (RadioInterface *mr,
	                                RingBuffer<uint8_t> *dataBuffer) {
	this	-> dataBuffer		= dataBuffer;
	this	-> handledPackets	= 0;
	connect (this, SIGNAL (writeDatagram (int)),
	         mr, SLOT (sendDatagram (int)));
}

	ip_dataHandler::~ip_dataHandler() {
}

void	ip_dataHandler::add_mscDatagroup (std::vector<uint8_t> msc) {
uint8_t *data		= (uint8_t *)(msc. data());
bool	extensionFlag	= getBits_1 (data, 0) != 0;
bool	crcFlag		= getBits_1 (data, 1) != 0;
bool	segmentFlag	= getBits_1 (data, 2) != 0;
bool	userAccessFlag	= getBits_1 (data, 3) != 0;
int16_t	next		= 16;		// bits
bool	lastSegment	= false;
uint16_t segmentNumber	= 0;
bool transportIdFlag	= false;
uint16_t transportId	= 0;
uint8_t	lengthInd;
int16_t	i;

	if (crcFlag && !check_CRC_bits (data, msc.size())) 
	   return;

	if (extensionFlag)
	   next += 16;

	if (segmentFlag) {
	   lastSegment	= getBits_1 (data, next) != 0;
	   segmentNumber = getBits (data, next + 1, 15);
	   next += 16;
	}

	if (userAccessFlag) {
	   transportIdFlag	= getBits_1 (data, next + 3);
	   lengthInd		= getBits_4 (data, next + 4);
	   next	+= 8;
	   if (transportIdFlag) {
	      transportId = getBits (data, next, 16);
	   }
	   next	+= lengthInd * 8;
	}

	uint16_t	ipLength	= 0;
	int16_t		sizeinBits	=
	              msc. size() - next - (crcFlag != 0 ? 16 : 0);
	ipLength = getBits (data, next + 16, 16);
	if (ipLength < msc. size() / 8) {	// just to be sure
	   std::vector<uint8_t> ipVector;
	   ipVector. resize (ipLength);
	   for (i = 0; i < ipLength; i ++)
	      ipVector [i] = getBits_8 (data, next + 8 * i);
	   if ((ipVector [0] >> 4) != 4)
	      return;	// should be version 4
	   process_ipVector (ipVector);
	}
}

void	ip_dataHandler::process_ipVector (std::vector<uint8_t> v) {
uint8_t	*data		= (uint8_t *)(v. data());
int16_t	headerSize	= data [0] & 0x0F;	// in 32 bits words
int16_t ipSize		= (data [2] << 8) | data [3];
uint8_t	protocol	= data [9];

uint32_t checkSum	= 0;
int16_t	i;

	for (i = 0; i < 2 * headerSize; i ++)
	   checkSum +=  ((data [2 * i] << 8) | data [2 * i + 1]);
	checkSum = (checkSum >> 16) + (checkSum & 0xFFFF);
	if ((~checkSum & 0xFFFF) != 0) {
	   return;
	}

	switch (protocol) {
	   case 17:			// UDP protocol
	      process_udpVector (&data [4 * headerSize], ipSize - 4 * headerSize);
	      return;
	   default:
	      return;
	}
}
//
//	We keep it simple now, just hand over the data from the
//	udp packet to port 8888
void	ip_dataHandler::process_udpVector (uint8_t *data, int16_t length) {
char *message = (char *)(&(data [8]));
	dataBuffer -> putDataIntoBuffer ((uint8_t *)message, length - 8);
	writeDatagram (length - 8);
}

