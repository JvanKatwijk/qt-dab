#
/*
 *    Copyright (C) 2014 .. 2025
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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
#include	"dab-constants.h"
#include	"radio.h"
#include	"msc-handler.h"
#include	"ofdm-decoder.h"
#include	"backend.h"
#include	"dab-params.h"
#include	"logger.h"
//
//	Interface program for processing the MSC.
//	The dabProcessor assumes the existence of an msc-handler, whether
//	a service is selected or not. 
//
//	For "slower" computers (i.e. an RPI 3), the ofdm decoding is
//	split up, the FIC blocks are handled in the thread of the
//	ofdmHandler, the "payload" blocks in a separate thread here

#define	CUSize	(4 * 16)
static int cifTable [] = {18, 72, 0, 36};

//	Note CIF counts from 0 .. 3
//
		mscHandler::mscHandler	(RadioInterface *mr,
	                                 uint8_t	dabMode,
	                                 RingBuffer<uint8_t> *frameBuffer_i,
	                                 logger		*theLogger,
	                                 uint8_t 	cpuSupport):
	                                       params (dabMode),
	                                       myMapper (dabMode),
	                                       myRadioInterface (mr),
	                                       frameBuffer (frameBuffer_i) {
	this	-> theLogger	= theLogger;
	this	-> cpuSupport	= cpuSupport;
	cifVector. resize (55296);
	BitsperBlock		= 2 * params. get_carriers();
	softBits. resize (BitsperBlock);
	nrBlocks		= params. get_L();
	connect (this, &mscHandler::activeServices,
	         mr, &RadioInterface::nrActiveServices);
	

	numberofblocksperCIF = cifTable [(dabMode - 1) & 03];
}

		mscHandler::~mscHandler () {
	locker. lock();
	for (auto &b : theBackends) {
	   b -> stopRunning();
	   delete b;
	}
	theBackends. resize (0);
	locker. unlock();
}

//
//	Input is put into a buffer, a the code in a separate thread
//	will handle the data from the buffer
void	mscHandler::processBlock_0 (Complex *b) {
	fprintf (stderr, "Why am I called?\n");
}

void	mscHandler::processMsc	(std::vector<Complex> &b,
	                                      int offset, int blkno) {
	(void)b;
	(void)offset;
	(void)blkno;
	fprintf (stderr, "I should not be called\n");
}
//
//	Note, the set_Channel function is called from within a
//	different thread than the process_mscBlock method is,
//	so, a little bit of locking seems wise while
//	the actual changing of the settings is done in the
//	thread executing process_mscBlock
void	mscHandler::resetBuffers	() {
	resetChannel ();
}

void	mscHandler::resetChannel () {
//	fprintf (stderr, "channel reset: all services will be stopped\n");
	locker. lock ();
	for (auto &b : theBackends) {
	   b -> stopRunning();
	   delete b;
	}
	theBackends. resize (0);
	locker. unlock ();
	activeServices ((int)(theBackends. size ()));
}

void	mscHandler::stopBackend	(const QString &serviceName,
	                         int subchId, int flag) {
	locker. lock ();
	for (int i = 0; i < (int)(theBackends. size ());  i ++) {
	   Backend *b = theBackends. at (i);
	   if ((b -> serviceName == serviceName) &&
	                  (b -> subChId == subchId) && (b -> borf == flag)) {
	      b -> stopRunning ();
	      usleep (1000);
	      delete b;
	      theBackends. erase (theBackends. begin () + i);
	      break;
	   }
	}
	locker. unlock ();
	activeServices ((int)(theBackends. size ()));
}
//
//	Note that - in general - the backens run in their own thread
bool	mscHandler::startBackend (descriptorType &d,
	                          RingBuffer<std::complex<int16_t>> *audioBuffer,
	                          RingBuffer<uint8_t> *dataBuffer,
	                          FILE *dump, int flag) {

	theBackends. push_back (new Backend (myRadioInterface,
	                                     theLogger,
	                                     &d,
	                                     audioBuffer,
	                                     dataBuffer,
	                                     frameBuffer,
	                                     dump,
	                                     flag,
	                                     cpuSupport));
	activeServices ((int)(theBackends. size ()));
	return true;
}

bool	mscHandler::serviceRuns	(uint32_t SId, uint16_t subChId) {
	for (auto &backend : theBackends)
	   if ((backend -> serviceId == (int) SId) && (backend -> subChId == subChId))
	      return true;
	return false;
}

//
//	add blocks. First is (should be) block 4, last is (should be) 
//	nrBlocks -1.
//	Note that this method is called from within the ofdm-processor thread
//	while the set_xxx methods are called from within the 
//	gui thread, so some locking is added
//

void	mscHandler::processMscBlock	(std::vector<int16_t> &softBits,
	                                 int16_t blkno) { 
int16_t	currentblk	= (blkno - 4) % numberofblocksperCIF;

//	and the normal operation is:
	memcpy (&cifVector [currentblk * BitsperBlock],
	                    softBits. data(), BitsperBlock * sizeof (int16_t));
	if (currentblk < numberofblocksperCIF - 1) 
	   return;

//	OK, now we have a full CIF and it seems there is some work to
//	be done.  We assume that the backend itself
//	does the work in a separate thread.
	locker. lock ();
	for (auto & b: theBackends) {
	   int16_t startAddr	= b -> startAddr;
	   int16_t Length	= b -> Length; 
	   if (Length > 0) 		// Length = 0? should not happen
	      (void) b -> process (&cifVector [startAddr * CUSize],
	                                      Length * CUSize);
	}
	locker. unlock();
}

void	mscHandler::set_dataTracer	(bool v) {
	for (auto &be: theBackends) 
	   if (be -> is_dataBackend ()) 
	      be -> set_dataTracer (v);
}

