#
/*
 *    Copyright (C) 2014 .. 2019
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
#include	"backend.h"
#include	"dab-params.h"
//
//	Interface program for processing the MSC.
//	The ofdm processor assumes the existence of an msc-handler, whether
//	a service is selected or not. 

#define	CUSize	(4 * 16)
static int cifTable [] = {18, 72, 0, 36};

//	Note CIF counts from 0 .. 3
//
		mscHandler::mscHandler	(RadioInterface *mr,
	                                 uint8_t	dabMode,
	                                 QString	picturesPath,
	                                 RingBuffer<uint8_t> *frameBuffer) :
	                                       params (dabMode),
	                                       my_fftHandler (dabMode),
	                                       myMapper (dabMode),
	                                       freeSlots (params. get_L ()){
	myRadioInterface	= mr;
	this	-> frameBuffer	= frameBuffer;
	this	-> picturesPath	= picturesPath;
	cifVector. resize (55296);

	this	-> T_u		= params. get_T_u ();
	BitsperBlock		= 2 * params. get_carriers();
	nrBlocks		= params. get_L();

	command. resize (nrBlocks);
	for (int i = 0; i < nrBlocks; i ++)
	   command [i]. resize (T_u);
	blockTable. resize (nrBlocks);
	for (int i = 0; i < nrBlocks; i ++)
	   blockTable [i] = -1;

	fft_buffer                      = my_fftHandler. getVector();
	phaseReference                  .resize (T_u);

	numberofblocksperCIF = cifTable [(dabMode - 1) & 03];
	work_to_be_done. store (false);
	running. store (false);
	nextIn		= 0;
	nextOut		= 0;
}

		mscHandler::~mscHandler() {
	running. store (false);
	while (isRunning())
	   usleep (100);
	locker. lock();
	work_to_be_done. store (false);
	for (auto const &b : theBackends) {
	   b -> stopRunning();
	   delete b;
	}
	theBackends. resize (0);
	locker. unlock();
}

//
//	We know that the dabProcessor hands over the
//	blocks one by one, in order 0 .. nrBlocks - 1


void	mscHandler::processBlock_0 (DSPCOMPLEX *b) {
	if (!running. load ())
	   return;
	while (!freeSlots. tryAcquire (1, 200))
	   if (!running. load ())
	      return;
	for (int i = 0; i < T_u; i ++)
	   command [nextIn][i] = b [i];
	blockTable [nextIn]	= 0;
	nextIn = (nextIn + 1) % nrBlocks;
	usedSlots. release (1);
}

void	mscHandler::process_Msc	(DSPCOMPLEX *b, int blkno) {
	if (!running. load ())
	   return;
	while (!freeSlots. tryAcquire (1, 200))
	   if (!running)
	      return;
	for (int i = 0; i < T_u; i ++)
	   command [nextIn][i] = b [i];
	blockTable [nextIn] = blkno;
	nextIn = (nextIn + 1) % nrBlocks;
	usedSlots. release (1);
}

//	The thread will be started - and stopped - by its owner,
//	the dabProcessor. 
//	Reasons to stop are switch of channel (or related, switch
//	of device)
//	Changing a service is done while processing continues
//	Changing a device will cause the delete of the mscHandler
//	object and a re-create
//
void    mscHandler::run () {
std::atomic<int>	currentBlock;
std::vector<int16_t> ibits (BitsperBlock);

	if (running. load ()) {		// should not happen
	   fprintf (stderr, "we draaien al!!!\n");
	   return;
	}

	currentBlock. store (0);
	running. store (true);
        while (running. load()) {
	   while (!usedSlots. tryAcquire (1, 200))
	      if (!running. load ())
	         return;
//
//	waiting for blockNumber "currentBlock" to arrive
//	If the block to be handled does NOT
//	have the number we want, we just skip the whole
//	frame and start with the next one
	   if (blockTable [nextOut] != currentBlock. load ()) {
	      fprintf (stderr, "s");
	      currentBlock. store (0);
	      blockTable [nextOut] = -1;
	      nextOut = (nextOut + 1) % nrBlocks;
	      freeSlots. release (1);
	      continue;
	   }
	      
//	now we know that amount > 0 and we have block "currentBlock"
	   memcpy (fft_buffer,
	              command [nextOut]. data (),
	                               T_u * sizeof (DSPCOMPLEX));
	   blockTable [nextOut] = -1;
	   nextOut = (nextOut + 1) % nrBlocks;
//	block 3 and up are needed as basis for demodulation the "mext" block
//	"our" msc blocks start with blkno 4
	   my_fftHandler. do_FFT ();
           if (currentBlock. load () >= 4) {
              for (int i = 0; i < params. get_carriers(); i ++) {
                 int16_t      index   = myMapper. mapIn (i);
                 if (index < 0)
                    index += T_u;

                 DSPCOMPLEX  r1 = fft_buffer [index] *
                                       conj (phaseReference [index]);
                 float ab1    = abs (r1);
//      Recall:  the viterbi decoder wants 127 max pos, - 127 max neg
//      we make the bits into softbits in the range -127 .. 127
                 ibits [i]            =  - real (r1) / ab1 * 1024.0;
                 ibits [params. get_carriers() + i]
	                                 =  - imag (r1) / ab1 * 1024.0;
              }

	      process_mscBlock (ibits, currentBlock. load ());
	   }
	   memcpy (phaseReference. data (),
	           fft_buffer, T_u * sizeof (DSPCOMPLEX));
	   freeSlots. release (1);
           currentBlock. store ((currentBlock. load () + 1) % (nrBlocks));
        }
}

//
//	This function is called whenever the radio selects a different
//	channel. stopChannel will stop the mscHandler, startChannel
//	will restart it.
void	mscHandler::stop () {
	running. store (false);
	while (isRunning())
	   usleep (100);
	locker. lock();
	work_to_be_done. store (false);
	for (auto const &b : theBackends) {
	   b -> stopRunning();
	   delete b;
	}
	theBackends. resize (0);
//
//	Note that at this point, whatever the dabProcessor is
//	doing (nothing if everything is OK), no activity
//	around the buffr takes place, and basically we "reset"
//	the buffer state
	usedSlots. acquire (usedSlots. available ());
	freeSlots. acquire (freeSlots. available ());
	freeSlots. release (nrBlocks);
	fprintf (stderr, "resource available %d %d\n",
	                     freeSlots. available (), usedSlots. available ());
	nextIn		= 0;
	nextOut		= 0;
	locker. unlock();
}
//
//	new service arrives
//	
//	This function is not used
void	mscHandler::reset	() {
	stop ();
	start ();
}

//
//	Note, the set_Channel and unset_Channel functions 
//	are  called from within the thread executing the
//	"radio" program, a thread from the process_mscBlock method is in.
//	So, a little bit of locking seems wise while
//	the actual changing of the settings is done in the
//	thread executing process_mscBlock
void	mscHandler::set_Channel (descriptorType *d,
	                         RingBuffer<int16_t> *audioBuffer,
	                         RingBuffer<uint8_t> *dataBuffer) {
	locker. lock();
	theBackends. push_back (new Backend (myRadioInterface,
	                                     d,
	                                     audioBuffer,
	                                     dataBuffer,
	                                     frameBuffer,
	                                     picturesPath));
	work_to_be_done. store (true);
	locker. unlock();
}

void	mscHandler::unset_Channel (const QString &s) {
	for (int i = 0; i < theBackends. size (); i ++) {
	   if (s == theBackends. at (i) -> theDescriptor. serviceName) {
	      locker. lock ();
	      theBackends. at (i) -> stopRunning ();
	      delete theBackends. at (i);
	      theBackends. erase (theBackends. begin () + i);
	      locker. unlock ();
	      return;
	   }
	}
}
//
//	add blocks. First is (should be) block 4, last is (should be) 
//	nrBlocks -1.
//	Note that this method is called from within the ofdm-processor thread
//	while the set_xxx methods are called from within the 
//	gui thread, so some locking is added
//
//	Any change in the selected service will only be active
//	during te next process_mscBlock call.
void	mscHandler::process_mscBlock	(std::vector<int16_t> fbits,
	                                 int16_t blkno) { 
int16_t	currentblk;

	currentblk	= (blkno - 4) % numberofblocksperCIF;
//	and the normal operation is:
	memcpy (&cifVector [currentblk * BitsperBlock],
	                    fbits. data(), BitsperBlock * sizeof (int16_t));
	if (currentblk < numberofblocksperCIF - 1) 
	   return;

	if (!work_to_be_done. load())
	   return;

//	OK, now we have a full CIF and it seems there is some work to
//	be done.  We assume that the backend itself
//	does the work in a separate thread.
//	Note that there is no garantee that there is work
	locker. lock();
	for (auto const& b: theBackends) {
	   int16_t startAddr	= b -> startAddr;
	   int16_t Length	= b -> Length; 
	   if (Length > 0) {		// Length = 0? should not happen
	      int16_t temp [Length * CUSize];
	      memcpy (temp, &cifVector [startAddr * CUSize],
	                           Length * CUSize * sizeof (int16_t));
	      (void) b -> process (temp, Length * CUSize);
	   }
	}
	locker. unlock();
}

