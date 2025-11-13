#
/*
 *    Copyright (C) 2015 .. 2024
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
#include	"bit-extractors.h"
#include	"crc-handlers.h"
#include	"data-processor.h"
#include	"virtual-datahandler.h"
#include	"ip-datahandler.h"
#include	"mot-handler.h"
#include	"journaline-controller.h"
//#include	"journaline-datahandler.h"
#include	"tdc-datahandler.h"
#include	"adv-datahandler.h"

//	\class dataProcessor
//	The main function of this class is to ASSEMBLE the 
//	MSCdatagroups from the incoming packets
//	 and dispatch to the appropriate handler
#define	RSDIMS	12
#define	FRAMESIZE 188
//	fragmentsize == Length * CUSize
	dataProcessor::dataProcessor	(RadioInterface *mr,
	                                 packetdata	*pd,
	                                 RingBuffer<uint8_t> *dataBuffer,
	                                 bool	backgroundFlag):
	                                     my_rsDecoder (8, 0435, 0, 1, 16) {
	this	-> myRadioInterface	= mr;
	this	-> bitRate		= pd -> bitRate;
	this	-> DSCTy		= pd -> DSCTy;
	this	-> appType		= pd -> appType;
	this	-> packetAddress	= pd -> packetAddress;
	this	-> DGflag		= pd -> DGflag;
	this	-> FEC_scheme		= pd -> FEC_scheme;
	this	-> dataBuffer		= dataBuffer;

	AppVector. resize (RSDIMS * FRAMESIZE + 48);
	FECVector. resize (9 * 22);
	for (int i = 0; i < 9; i ++)
	   FEC_table [i] = false;

	fillPointer	= 0;
	switch (DSCTy) {
	   default:
	      fprintf (stderr, "DSCTy %d not supported\n", DSCTy);
	      my_dataHandler. reset (new virtual_dataHandler ());
	      break;

	   case 5:
	      if (appType == 0x44a)
//	         my_dataHandler. reset (new journaline_dataHandler ());
	         my_dataHandler. reset (new journalineController (mr));
	      else
	      if (appType == 1500)
	         my_dataHandler. reset (new adv_dataHandler (mr, dataBuffer, appType));
	      else
	      if (appType == 4)
	         my_dataHandler. reset (new tdc_dataHandler (mr, dataBuffer, appType));
	      else {
	         fprintf (stderr, "DSCTy 5 with appType %d not supported\n",
	                                                           appType);
	         my_dataHandler. reset (new virtual_dataHandler ());
	      }
	      break;

	   case 44:
	      my_dataHandler. reset (new journalineController (mr));
//	      my_dataHandler. reset (new journaline_dataHandler ());
	      break;

	   case 59:
	      my_dataHandler. reset (new ip_dataHandler (mr, dataBuffer));
	      break;

	   case 60:
	      my_dataHandler. reset (new motHandler (mr, backgroundFlag));
	      break;
	   
	}
	assembling	= false;
	last_cntIdx	= 0;
}

	dataProcessor::~dataProcessor() {
}

void	dataProcessor::addtoFrame (const std::vector<uint8_t>  &outV) {
//	There is - obviously - some exception, that is
//	when the DG flag is on and there are no datagroups for DSCTy5
std::vector<uint8_t> VV = outV;
	if ((this -> DSCTy == 5) &&
	    (this -> DGflag))	// no datagroups
	      handleTDCAsyncstream (VV. data (), 24 * bitRate);
	   else
	      handlePackets (VV. data (), 24 * bitRate);
}
//
void	dataProcessor::handlePackets (const uint8_t *data, int16_t length) {
	while (true) {
//	pLength is in bits
	   int32_t pLength = (getBits_2 (data, 0) + 1) * 24 * 8;
	   if (length < pLength)	// be on the safe side
	      return;

	   if (!FEC_scheme) 
	      handlePacket (data);
	   else
	      handleRSPacket (data);
//
//	prepare for the next round
	   length -= pLength;
	   if (length < 24) {
	      return;
	   }
	   data = &(data [pLength]);
	}
}

void	dataProcessor::handlePacket (const uint8_t *vec) {
	uint8_t Length	= (getBits (vec, 0, 2) + 1) * 24;
	if (!check_CRC_bits (vec, Length * 8)) {
//	   fprintf (stderr, "crc fails %d\n", Length);
	   return;
	}
//	fprintf (stderr, "packet crc OK %d\n", Length);

//	Continuity index:
	const uint8_t cntIdx	= getBits (vec, 2, 2);
//	First/Last flag:
	const uint8_t flflg	= getBits (vec, 4, 2);
//	Packet address
	const uint16_t paddr	= getBits (vec, 6, 10);
//	Useful data length
	const uint8_t udlen	= getBits (vec, 17,7); 
//	fprintf (stderr, "udlen = %d\n", udlen);
	if (udlen == 0)
	   return;

	if (paddr != packetAddress)
	   return;

	if (cntIdx != (last_cntIdx + 1) % 4) {
//	   fprintf (stderr, "packet cntIdx %d expected %d address %d\n",
//	                                cntIdx, last_cntIdx, paddr);
	   if (cntIdx == last_cntIdx)
	      return;
	   last_cntIdx = 0;
	   assembling = false;
	   return;
	}
	last_cntIdx = cntIdx;

	if (udlen > Length - 5) {
	   fprintf (stderr, "packet udlen %d is larger than max payload %d\n",
	               udlen, Length - 5);
	   assembling - false;
	}

	switch (flflg) {
	   case 2:  // First data group packet
	      series. resize (udlen * 8);
	      for (uint16_t i = 0; i < udlen * 8; i ++)
	         series [i] = vec [3 * 8 + i];
	      assembling	= true;
	      return;

	   case 0:    // Intermediate data group packet
	      if (assembling) {
	         int currentLength = series. size ();
	         if (currentLength + udlen * 8 > 4 * 8192) {
	            assembling = false;
//	            fprintf (stderr, "too large???\n");
	            return;
	         }
	         series. resize (currentLength + udlen * 8);
	         for (int i = 0; i < udlen * 8; i ++) 
	            series [currentLength + i] = vec [3 * 8 + i];
	      }
	      return;

	   case 1:  // Last data group packet
	      if (assembling) {
	         int currentLength = series. size ();
	         if (currentLength + udlen * 8 > 4 * 8192) {
	            assembling = false;
//	            fprintf (stderr, "too large???\n");
	            return;
	         }
	         series. resize (currentLength + udlen * 8);
	         for (int i = 0; i < udlen * 8; i ++)
	            series [currentLength + i] = vec [3 * 8 + i];
	         assembling = false;
//
//	Note, we are sending the UNPROCESSED mscdatagroup to the
//	appropriate handler
	         my_dataHandler	-> add_mscDatagroup (series);
	         series. resize (0);
	      }
	      return;

	      case 3: { // Single packet, mostly padding
	         if (Length > 3 * 8 + udlen * 8) {
	            series. resize (udlen * 8);
	            for (uint8_t i = 0; i < udlen * 8; i ++)
	               series [i] = vec [3 * 8 + i];
	            my_dataHandler -> add_mscDatagroup (series);
	         }
	         series. resize (0);
              }
	      assembling = true;
	      return;

	      default:	// cannot happen
	         return;
	   }
}
//
//	we try to ensure that when the RS packages are readin, we
//	have exactly RSDIMS * FRAMESIZE uint's read
void	dataProcessor::handleRSPacket (const uint8_t  *vec) {
int32_t pLength		= (getBits_2 (vec, 0) + 1) * 24;
uint16_t address	= getBits (vec, 6, 10);
//
//	we differentiate between the "data" packets and the "RS" packets
//
//	the "order" is first RSDIMS * FRAMESIZE packet elements
//	with data, bext 9 * 22 bytes RS data
	if ((pLength == 24) && (address == 1022)) {	// RS packet
	   uint8_t counter = getBits (vec, 2, 4);
	   if (counter < 9) {	// zo hoort het
	      registerFEC (vec, counter);
	      if (FEC_complete ()) {
	         processRS (AppVector, FECVector);
	         handle_RSpackets (AppVector);
	         clear_FECtable ();
	         fillPointer = 0;
	      }
	   }
	   else
	      fprintf (stderr, "Foute counter: %d\n", counter);
	}
	else {
//	addPacket checks the size and sets fillPointer to 0 id erroneous
	   fillPointer = addPacket (vec, AppVector, fillPointer);
	}
}

void	dataProcessor::clear_FECtable () {
	for (int i = 0; i < 9; i ++)
	   FEC_table [i] = false;
}
//
//	addPacket basically packs the sequence of bits into a sequence
//	of bytes, for processing by the RS decoder
//	of course, we check for overflow
int	dataProcessor::addPacket (const uint8_t *vec,
	                          std::vector<uint8_t> &theBuffer,
	                          int fillPointer) {
	int16_t	packetLength	= (getBits_2 (vec, 0) + 1) * 24;
//
//	Assert theBuffer. size () == RSDIMS * FRAMESIZE
	if (fillPointer + packetLength > (int)theBuffer. size ()) {
	   clear_FECtable ();
	   return 0;
	}
	for (int i = 0; i < packetLength; i ++) {
	   uint8_t temp = 0;
	   for (int j = 0; j < 8; j ++)
	      temp = (temp << 1) | (vec [i * 8 + j] == 0 ? 0 : 1);
	   theBuffer [fillPointer + i] = temp;
	}
	return fillPointer + packetLength;
}
//
//	The output of the RS decoding is a vector with a sequence
//	of packets, first dispatch and separate the packet sequence
//	into its elements
//
void	dataProcessor::handle_RSpackets (const std::vector<uint8_t> &vec) {
	for (int baseP = 0; baseP < RSDIMS * FRAMESIZE; ) {
	   int16_t packetLength = (((vec [baseP] & 0xc0) >> 6) + 1) * 24;
	   handle_RSpacket (&(vec. data ()) [baseP], packetLength);
	   baseP += packetLength;
	}
}

static
uint8_t bitList [] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
//
//	The RS data is with packed bytes, while the basis infrastructure
//	is with bit sequences, so to keep things simple, we just
//	transform the byte sequence into a bit sequence
void	dataProcessor::handle_RSpacket (const uint8_t *packet,
	                                        int16_t packetLength) {
std::vector<uint8_t> bitData (packetLength * 8);
	for (int i = 0; i < packetLength; i ++) {
	   uint8_t temp = packet [i];
	   for (int j = 0; j < 8; j ++) {
	      uint8_t theBit = (temp & bitList [j]) == 0 ? 0 : 1;
	      bitData [8 * i + j] = theBit;
	   }
	}
	handlePacket (bitData. data ());
}
//
//	as it tuns out, the FEC data packages are arriving in order,
//	so it would have been sufficient just to wait until the
//	package with counter '8' was seen
void	dataProcessor::registerFEC (const uint8_t *vec, int cnt) {
	for (int i = 0; i < 22; i ++) {
	   uint8_t temp = 0;
	   for (int j = 0; j < 8; j ++)
	      temp = (temp << 1) | vec [16 + 8 * i + j];
	   FECVector [cnt * 22 + i] = temp;
	}
	FEC_table [cnt] = true;
}

bool	dataProcessor::FEC_complete () {
	for (int i = 0; i < 9; i ++)
	   if (!FEC_table [i])
	      return false;
	return true;
}
//
//	Really no idea what to do here
void	dataProcessor::handleTDCAsyncstream (const uint8_t *data,
	                                              int32_t length) {
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
//	To keep things simple, we abstraxct from the rs decoding
//	by providing - as separate vectors - the RSDIMS * FRAMESIZE
//	app data values and the 9 * 22 RS data values
//	The appData vector is overwritten with the corrected data
//
void	dataProcessor::processRS (std::vector<uint8_t> &appData,
	                          const std::vector<uint8_t> &RSdata) {
static
uint8_t table [RSDIMS][FRAMESIZE + 16];
uint8_t rsOut	[FRAMESIZE];
//	Assert appdata . size () == RSDIMS * FRAMESIZE
//	Assert RSdata. size () == 9 * 22;
	for (int i = 0; i < RSDIMS * FRAMESIZE; i ++)
	   table [i % RSDIMS][i / RSDIMS] = appData [i];
	for (int i = 0; i <  (int)(RSdata. size ()); i ++)
	   table [i % RSDIMS] [FRAMESIZE + i / RSDIMS] = RSdata [i];

	for (int i = 0; i < RSDIMS; i ++) {
	   (void) my_rsDecoder. dec (table [i], rsOut, 51);
//	   fprintf (stderr, "rs decoder says %d\n", xx);
	   for (int j = 0; j < FRAMESIZE; j ++)
	      table [i][j] = rsOut [j];
	}
//
//	copy the table back to the vector
	for (int i = 0; i < RSDIMS * FRAMESIZE; i ++)
	   appData [i] = table [i % RSDIMS][i / RSDIMS];
}

void	dataProcessor::stop	() {}

