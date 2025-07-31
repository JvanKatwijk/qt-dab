#
/*
 *    Copyright (C) 2015 .. 2024
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
#include	"adv-datahandler.h"
#include	"radio.h"
#include	"crc-handlers.h"
#include	"bit-extractors.h"

//
//	The adv data handler is meant to be for processing PPP type data,
//	the format is unknown to me, so I leave it here to
//	assembling the MSC datagroup
	adv_dataHandler::adv_dataHandler (RadioInterface *mr,
	                                  RingBuffer<uint8_t> *dataBuffer,
	                                  int appType) {
	(void)mr;
	(void)appType;
	this	-> dataBuffer		= dataBuffer;
}

	adv_dataHandler::~adv_dataHandler () {
}

const uint8_t syncWord [] = {0x01, 0x41, 0x0f, 0xf7, 0xcf, 0x78, 0x9c};

void	adv_dataHandler::add_mscDatagroup (const std::vector<uint8_t> msc) {
uint8_t *data		= (uint8_t *)(msc. data());
bool	extensionFlag	= getBits_1 (data, 0) != 0;
bool	crcFlag		= getBits_1 (data, 1) != 0;
bool	segmentFlag	= getBits_1 (data, 2) != 0;
bool	userAccessFlag	= getBits_1 (data, 3) != 0;
//uint8_t dataGroupType	= getBits_1 (data, 4);
uint8_t cntIdx		= getBits_4 (data, 8);
//uint8_t repInd		= getBits_4 (data, 12);
//uint16_t extField	=
	 extensionFlag ? getBits   (data, 16, 16) : 0;
int	next		= extensionFlag ? 32 : 16; 

//bool	LastSegment	= 0;
uint16_t segmentNumber	= 0;

	if (segmentFlag)  {
//	   LastSegment = getBits   (data, next, 15);
	   segmentNumber = getBits_1 (data, next ++);
//	   fprintf (stderr, "segment %d\n", segmentNumber);
	   next = next + 15;
	}

	(void)segmentNumber;
	if (userAccessFlag) {
	   int lengthInd = getBits (data, next + 4, 4);
	   uint8_t transportFlag = getBits (data,  next + 3, 1);
	   if (transportFlag)
	      fprintf (stderr, "transportId %d\n",
	                            getBits (data, next + 8, 16));
	   next		= 16 + (extensionFlag ? 1 : 0) * 16 + (lengthInd + 1) * 8;
	}
	if (crcFlag && !check_CRC_bits (data, msc.size())) {	
	   fprintf (stderr, "cntIdx %d fails\n", cntIdx);
	   return;
	}

//	int dataLength	= msc. size () / 8 - 2 - next / 8;
//	fprintf (stderr, "%d datalength %d\n",
//	         cntIdx, msc. size () / 8 - 2 - next / 8);
	uint8_t x0, x1, x2;
	x0	= getBits (data, next + 8 * 0, 8);	
	x1	= getBits (data, next + 8 * 1, 8);	
	x2	= getBits (data, next + 8 * 2, 8);	
	(void)x2;
	if (x0 == 0xd3 && x1 == 0) {
	   for (int i = 0; i < 25; i ++) {
	      uint8_t xx = getBits (data, next + 8 * i, 8);
	      fprintf (stderr, "%x%x ", xx >> 4, xx & 0xF);
	   }
	   fprintf (stderr, "\n");
	}
	//
	//	the data can be found at next / 8 
	//	and is still one bit per byte, ;ength
	//	dataLength
}

