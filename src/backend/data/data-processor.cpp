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
 */
#
#include	"dab-constants.h"
#include	"radio.h"
#include	"data-processor.h"
#include	"virtual-datahandler.h"
#include	"ip-datahandler.h"
#include	"mot-handler.h"
#include	"journaline-datahandler.h"
#include	"tdc-datahandler.h"

//	\class dataProcessor
//	The main function of this class is to assemble the 
//	MSCdatagroups and dispatch to the appropriate handler
//
//	fragmentsize == Length * CUSize
	dataProcessor::dataProcessor	(RadioInterface *mr,
	                                 packetdata	*pd,
	                                 RingBuffer<uint8_t> *dataBuffer) {
	this	-> myRadioInterface	= mr;
	this	-> bitRate		= pd -> bitRate;
	this	-> DSCTy		= pd -> DSCTy;
	this	-> appType		= pd -> appType;
	this	-> packetAddress	= pd -> packetAddress;
	this	-> DGflag		= pd -> DGflag;
	this	-> FEC_scheme		= pd -> FEC_scheme;
	this	-> dataBuffer		= dataBuffer;
	this	-> expectedIndex	= 0;
	switch (DSCTy) {
	   default:
	      fprintf (stderr, "DSCTy %d not supported\n", DSCTy);
	      my_dataHandler	= new virtual_dataHandler();
	      break;

	   case 5:			
	      my_dataHandler	= new tdc_dataHandler (mr, dataBuffer, appType);
	      break;

	   case 44:
	      my_dataHandler	= new journaline_dataHandler();
	      break;

	   case 59:
	      my_dataHandler	= new ip_dataHandler (mr, dataBuffer);
	      break;

	   case 60:
	      my_dataHandler	= new motHandler (mr);
	      break;
	   
	}

	packetState	= 0;
}

	dataProcessor::~dataProcessor() {
	delete		my_dataHandler;
}


void	dataProcessor::addtoFrame (std::vector<uint8_t>  outV) {
//	There is - obviously - some exception, that is
//	when the DG flag is on and there are no datagroups for DSCTy5

	   if ((this -> DSCTy == 5) &&
	       (this -> DGflag))	// no datagroups
	      handleTDCAsyncstream (outV. data(), 24 * bitRate);
	   else
	      handlePackets (outV. data(), 24 * bitRate);
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
//	Handle a single DAB packet:
//	Note, although not yet encountered, the standard says that
//	there may be multiple streams, to be identified by
//	the address. For the time being we only handle a single
//	stream!!!!
void	dataProcessor::handlePacket (uint8_t *data) {
int32_t	packetLength	= (getBits_2 (data, 0) + 1) * 24;
int16_t	continuityIndex	= getBits_2 (data, 2);
int16_t	firstLast	= getBits_2 (data, 4);
int16_t	address		= getBits   (data, 6, 10);
uint16_t command	= getBits_1 (data, 16);
int32_t	usefulLength	= getBits_7 (data, 17);
//	if (usefulLength > 0)
//	   fprintf (stderr, "CI = %d, address = %d, usefulLength = %d\n",
//	                       continuityIndex, address, usefulLength);

	if (continuityIndex != expectedIndex) {
	   expectedIndex = 0;
	   return;
	}

	expectedIndex = (expectedIndex + 1 ) % 4;
	(void)command;

	if (!check_CRC_bits (data, packetLength * 8)) {
	   return;
	}

	if (address == 0)
	   return;		// padding packet
//
	if (packetAddress != address)	// sorry
	   return;
	
//	assemble the full MSC datagroup

	if (packetState == 0) {	// waiting for a start
	   if (firstLast == 02) {	// first packet
	      packetState = 1;
	      series. resize (usefulLength * 8);
	      for (uint16_t i = 0; i < series. size(); i ++)
	         series [i] = data [24 + i];
	   }
	   else
	   if (firstLast == 03) {	// single packet, mostly padding
	      series. resize (usefulLength * 8);
	      for (uint16_t i = 0; i < series. size(); i ++)
	         series [i] = data [24 + i];
	      my_dataHandler	-> add_mscDatagroup (series);
	   }
	   else 
	      series. resize (0);	// packetState remains 0
	}
	else
	if (packetState == 01) {	// within a series
	   if (firstLast == 0) {	// intermediate packet
	      int32_t currentLength = series. size();
	      series. resize (currentLength + 8 * usefulLength);
	      for (uint16_t i = 0; i < 8 * usefulLength; i ++)
	         series [currentLength + i] = data [24 + i];
	   }
	   else
	   if (firstLast == 01) {	// last packet
	      int32_t currentLength = series. size();
	      series. resize (currentLength + 8 * usefulLength);
	      for (uint16_t i = 0; i < 8 * usefulLength; i ++)
	         series [currentLength + i] = data [24 + i];

	      my_dataHandler	-> add_mscDatagroup (series);
	      packetState = 0;
	   }
	   else
	   if (firstLast == 02) {	// first packet, previous one erroneous
	      packetState = 1;
	      series. resize (usefulLength * 8);
	      for (uint16_t i = 0; i < series. size(); i ++)
	         series [i] = data [24 + i];
	   }
	   else {
	      packetState = 0;
	      series. resize (0);
	   }
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
