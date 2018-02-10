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
 */
#
#include	"dab-constants.h"
#include	"radio.h"
#include	"data-processor.h"
#include	"virtual-datahandler.h"
#include	"ip-datahandler.h"
#include	"mot-databuilder.h"
#include	"journaline-datahandler.h"
#include	"tdc-datahandler.h"

//	\class dataProcessor
//	The main function of this class is to assemble the 
//	MSCdatagroups and dispatch to the appropriate handler
//
//	fragmentsize == Length * CUSize
	dataProcessor::dataProcessor	(RadioInterface *mr,
	                                 int16_t	bitRate,
	                         	 uint8_t	DSCTy,
	                                 int16_t	appType,
	                                 uint8_t	DGflag,
	                         	 int16_t	FEC_scheme,
	                                 RingBuffer<uint8_t> *dataBuffer,
	                                 QString	picturesPath) {
	this	-> myRadioInterface	= mr;
	this	-> bitRate		= bitRate;
	this	-> DSCTy		= DSCTy;
	this	-> appType		= appType;
	this	-> DGflag		= DGflag;
	this	-> FEC_scheme		= FEC_scheme;
	this	-> dataBuffer		= dataBuffer;
	this	-> expectedIndex	= 0;
	switch (DSCTy) {
	   default:
	      my_dataHandler	= new virtual_dataHandler ();
	      break;

	   case 5:			
	      my_dataHandler	= new tdc_dataHandler (mr, dataBuffer, appType);
	      break;

	   case 44:
	      my_dataHandler	= new journaline_dataHandler ();
	      break;

	   case 59:
	      my_dataHandler	= new ip_dataHandler (mr, dataBuffer);
	      break;

	   case 60:
	      my_dataHandler	= new mot_databuilder (mr, picturesPath);
	      break;
	}
}

	dataProcessor::~dataProcessor	(void) {
	delete		my_dataHandler;
}


void	dataProcessor::addtoFrame (uint8_t *outV) {
//	There is - obviously - some exception, that is
//	when the DG flag is on and there are no datagroups for DSCTy5
	   if ((this -> DSCTy == 5) &&
	       (this -> DGflag))	// no datagroups
	      handleTDCAsyncstream (outV, 24 * bitRate);
	   else
	      handlePackets (outV, 24 * bitRate);
}
//
//	While for a full mix data and audio there will be a single packet in a
//	data compartment, for an empty mix, there may be many more
void	dataProcessor::handlePackets (uint8_t *data, int32_t length) {
	while (true) {
	   int32_t pLength = (getBits_2 (data, 0) + 1) * 24 * 8;
	   if (length < pLength)	// be on the safe side
	      return;
	   handlePacket (data);
	   length -= pLength;
	   if (length < 2)
	      return;
	   data	= &(data [pLength]);
	}
}
//
//	Handle multiple DAB packet based on address:
//
void	dataProcessor::handlePacket (uint8_t *data) {
int32_t	packetLength	= (getBits_2 (data, 0) + 1) * 24;
int16_t	continuityIndex	= getBits_2 (data, 2);
int16_t	firstLast	= getBits_2 (data, 4);
int16_t	address		= getBits   (data, 6, 10);
uint16_t command	= getBits_1 (data, 16);
int32_t	usefulLength	= getBits_7 (data, 17);
int32_t	i;

//	fprintf(stderr,"packetLength[%d] continuityIndex[%d] firstLast[%d] address[%d] command[%d] usefulLength[%d]\n", \
//		packetLength, continuityIndex, firstLast, address, command, usefulLength);
	if (continuityIndex != expectedIndex) {
	   expectedIndex = 0;
	   return;
	}
//
	expectedIndex = (expectedIndex + 1 ) % 4;
	(void)command;

	if (!check_CRC_bits (data, packetLength * 8)) {
	   return;
	}

	if (address == 0)
	   return;		// padding packet
	
//	assemble the full MSC datagroup
// Single packet is a special case and should be treated properly.
	if (firstLast == 03) {	// single packet, mostly padding
	      series[address]. resize (usefulLength * 8);
	      for (i = 0; i < series[address]. size (); i ++)
	         series[address] [i] = data [24 + i];
	      my_dataHandler	-> add_mscDatagroup (series[address]);
	      series[address]. resize (0); // reset buffer
	}
	 if (firstLast == 02) {	// first packet
	      series[address]. resize (0); // reset buffer, just in case a packet is missed and new one is received.
	      series[address]. resize (usefulLength * 8);
	      for (i = 0; i < series[address]. size (); i ++)
	         series[address] [i] = data [24 + i];
	}
	else
	   if (firstLast == 00) {	// intermediate packet
	      int32_t currentLength = series[address]. size ();
	      series[address]. resize (currentLength + 8 * usefulLength);
	      for (i = 0; i < 8 * usefulLength; i ++)
	         series[address] [currentLength + i] = data [24 + i];
	   }
	   else
	   if (firstLast == 01) {	// last packet
	      int32_t currentLength = series[address]. size ();
	      series[address]. resize (currentLength + 8 * usefulLength);
	      for (i = 0; i < 8 * usefulLength; i ++)
	         series[address] [currentLength + i] = data [24 + i];

	      my_dataHandler	-> add_mscDatagroup (series[address]);
	      series[address]. resize (0); // reset buffer
	   }
}
//
//
//	Really no idea what to do here
void	dataProcessor::handleTDCAsyncstream (uint8_t *data, int32_t length) {
int16_t	packetLength	= (getBits_2 (data, 0) + 1) * 24;
int16_t	continuityIndex	= getBits_2 (data, 2);
int16_t	firstLast	= getBits_2 (data, 4);
int16_t	address		= getBits   (data, 6, 10);
uint16_t command	= getBits_1 (data, 16);
int16_t	usefulLength	= getBits_7 (data, 17);

	(void)	length;
	(void)	packetLength;
	(void)	continuityIndex;
	(void)	firstLast;
	(void)	address;
	(void)	command;
	(void)	usefulLength;
	if (!check_CRC_bits (data, packetLength * 8))
	   return;
}
//
