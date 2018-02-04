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
#include	"virtual-backend.h"
#include	"audio-backend.h"
#include	"data-backend.h"
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
	                                 QString	picturesPath) :
	                                       params (mode) {
	myRadioInterface	= mr;
	this	-> picturesPath	= picturesPath;
	cifVector. resize (55296);
	theBackend		= new virtualBackend (0, 0);
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
	theBackend	-> stopRunning ();
	delete	theBackend;
}

//
//	Note, the set_xxx functions are called from within a
//	different thread than the process_mscBlock method,
//	so, a little bit of locking seems wise while
//	the actual changing of the settings is done in the
//	thread executing process_mscBlock
void	mscHandler::set_audioChannel (audiodata *d,
	                              RingBuffer<int16_t> *audioBuffer) {
	locker. lock ();
	theBackend -> stopRunning ();
	delete theBackend;
	theBackend = new audioBackend (myRadioInterface,
	                               d,
	                               audioBuffer,
	                               picturesPath);
	locker. unlock ();
}
//
void	mscHandler::set_dataChannel (packetdata	*d,
	                             RingBuffer<uint8_t> *dataBuffer) {
	locker. lock ();
	theBackend -> stopRunning ();
	delete theBackend;
//	fprintf (stderr, "bitRate = %d\n", d -> bitRate);
	theBackend = new dataBackend (myRadioInterface,
	                              d,
	                              dataBuffer,
	                              picturesPath);
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
void	mscHandler::process_mscBlock	(std::vector<int16_t> fbits,
	                                 int16_t blkno) { 
int16_t	currentblk;
std::vector<int16_t>myBegin;
int16_t	startAddr;
int16_t	Length;

	currentblk	= (blkno - 4) % numberofblocksperCIF;
//	and the normal operation is:
	memcpy (&cifVector [currentblk * BitsperBlock],
	                    fbits. data (), BitsperBlock * sizeof (int16_t));
	if (currentblk < numberofblocksperCIF - 1) 
	   return;


//	OK, now we have a full CIF. We assume that the backend itself
//	does the work in a separate thread.
	locker. lock ();
	startAddr       = theBackend -> startAddr ();
        Length          = theBackend -> Length    (); 
	if (Length > 0) {		// Length = 0? virtual Backend
	   myBegin. resize (Length * CUSize);
	   memcpy (myBegin. data (), &cifVector [startAddr * CUSize],
	                           Length * CUSize * sizeof (int16_t));
	   (void) theBackend -> process (myBegin. data (), Length * CUSize);
	}
	locker. unlock ();
}
//

void	mscHandler::stopProcessing	(void) {
	work_to_be_done	= false;
}

void	mscHandler::stop		(void) {
	work_to_be_done	= false;
	theBackend	-> stopRunning ();
}

void	mscHandler::reset		(void) {
	work_to_be_done	= false;
	locker. lock ();
	theBackend	-> stopRunning ();
	delete theBackend;
	theBackend	= new virtualBackend (0, 0);
	locker. unlock ();
}

