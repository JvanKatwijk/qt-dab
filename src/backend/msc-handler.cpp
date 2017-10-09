#
/*
 *    Copyright (C) 2014 .. 2017
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
#include	"dab-constants.h"
#include	"radio.h"
#include	"msc-handler.h"
#include	"dab-virtual.h"
#include	"dab-audio.h"
#include	"dab-data.h"
#include	"dab-params.h"
//
//	Interface program for processing the MSC.
//	The ofdm processor assumes the existence of an msc-handler, whether
//	a service is selected or not. 

#define	CUSize	(4 * 16)
//	Note CIF counts from 0 .. 3
//
		mscHandler::mscHandler	(RadioInterface *mr,
	                                 uint8_t	mode,
	                                 RingBuffer<int16_t> *audioBuffer,
	                                 RingBuffer<uint8_t> *dataBuffer,
	                                 QString	picturesPath) :
	                                       params (mode) {
	myRadioInterface	= mr;
	this	-> audioBuffer	= audioBuffer;
	this	-> dataBuffer	= dataBuffer;
	this	-> picturesPath	= picturesPath;
	cifVector		= new int16_t [55296];
	cifCount		= 0;	// msc blocks in CIF
	blkCount		= 0;
	dabHandler		= new dabVirtual;
	work_to_be_done		= false;
	dabModus		= 0;
	BitsperBlock		= 2 * params. get_carriers ();
	switch (mode) {
	   case 4:	// 2 CIFS per 76 blocks
	      numberofblocksperCIF	= 36;
	      break;

	   case 1:	// 4 CIFS per 76 blocks
	      numberofblocksperCIF	= 18;
	      break;

	   case 2:	// 1 CIF per 76 blocks
	      numberofblocksperCIF	= 72;
	      break;

	   default:
	      numberofblocksperCIF	= 18;
	      break;
	}
}

		mscHandler::~mscHandler	(void) {
	delete[]  cifVector;
	dabHandler	-> stopRunning ();
	delete	dabHandler;
}

//
//	Note, the set_xxx functions are called from within a
//	different thread than the process_mscBlock method,
//	so, a little bit of locking seems wise while
//	the actual changing of the settings is done in the
//	thread executing process_mscBlock
void	mscHandler::set_audioChannel (audiodata *d) {
	locker. lock ();
	shortForm	= d	-> shortForm;
	startAddr	= d	-> startAddr;
	Length		= d	-> length;
	protLevel	= d	-> protLevel;
	bitRate		= d	-> bitRate;
	language	= d	-> language;
	type		= d	-> programType;
	ASCTy		= d	-> ASCTy;
	dabModus	= ASCTy == 077 ? DAB_PLUS : DAB;
	dabHandler -> stopRunning ();
	delete dabHandler;

	dabHandler = new dabAudio (myRadioInterface,
	                           dabModus,
	                           Length * CUSize,
	                           bitRate,
	                           shortForm,
	                           protLevel,
	                           audioBuffer,
	                           picturesPath);
	work_to_be_done		= true;
	locker. unlock ();
}
//
void	mscHandler::set_dataChannel (packetdata	*d) {
	locker. lock ();
	shortForm	= d	-> shortForm;
	startAddr	= d	-> startAddr;
	Length		= d	-> length;
	protLevel	= d	-> protLevel;
	DGflag		= d	-> DGflag;
	bitRate		= d	-> bitRate;
	FEC_scheme	= d	-> FEC_scheme;
	DSCTy		= d	-> DSCTy;
	packetAddress	= d	-> packetAddress;
	appType		= d	-> appType;
	dabHandler -> stopRunning ();
	delete dabHandler;
	dabHandler = new dabData (myRadioInterface,
	                          DSCTy,
	                          appType,
	                          packetAddress,
	                          Length * CUSize,
	                          bitRate,
	                          shortForm,
	                          protLevel,
	                          DGflag,
	                          FEC_scheme,
	                          dataBuffer,
	                          picturesPath);
//	these we need for actual processing
//	and this one to get started
	work_to_be_done	= true;
	locker. unlock ();
}

//
//	add blocks. First is (should be) block 5, last is (should be) 76
//	Note that this method is called from within the ofdm-processor thread
//	while the set_xxx methods are called from within the 
//	gui thread
//
//	Any change in the selected service will only be active
//	during te next process_mscBlock call.
void	mscHandler::process_mscBlock	(int16_t *fbits,
	                                 int16_t blkno) { 
int16_t	currentblk;
int16_t	*myBegin;

	if (!work_to_be_done)
	   return;

	currentblk	= (blkno - 4) % numberofblocksperCIF;
//	and the normal operation is:
	memcpy (&cifVector [currentblk * BitsperBlock],
	                    fbits, BitsperBlock * sizeof (int16_t));
	if (currentblk < numberofblocksperCIF - 1) 
	   return;

	locker. lock ();
//
//	OK, now we have a full CIF
	blkCount	= 0;
	cifCount	= (cifCount + 1) & 03;
	myBegin		= &cifVector [startAddr * CUSize];
//	Here we move the vector to be processed to a
//	separate function executed by a separate thread
	(void) dabHandler -> process (myBegin, Length * CUSize);
	locker. unlock ();
}
//

void	mscHandler::stopProcessing	(void) {
	work_to_be_done	= false;
}

void	mscHandler::stop		(void) {
	work_to_be_done	= false;
	dabHandler	-> stopRunning ();
}

void	mscHandler::reset		(void) {
	work_to_be_done	= false;
	dabHandler	-> stopRunning ();
}

