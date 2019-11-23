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
	                                       bufferSpace (params. get_L()){
	myRadioInterface	= mr;
	this	-> frameBuffer	= frameBuffer;
	this	-> picturesPath	= picturesPath;
	cifVector. resize (55296);

	fprintf (stderr, "begin met resource %d\n",
	                           bufferSpace. available ());
	BitsperBlock		= 2 * params. get_carriers();
	nrBlocks		= params. get_L();

	command. resize (nrBlocks);
	for (int i = 0; i < nrBlocks; i ++)
	   command [i]. resize (params. get_T_u());

	fft_buffer                      = my_fftHandler. getVector();
	phaseReference                  .resize (params. get_T_u());

	numberofblocksperCIF = cifTable [(dabMode - 1) & 03];
	work_to_be_done. store (false);
	running. store (false);
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
	bufferSpace. acquire (1);
	for (int i = 0; i < params. get_T_u (); i ++)
	   command [0][i] = b [i];
	helper. lock ();
        commandHandler. wakeOne ();
	helper. unlock();
}

void	mscHandler::process_Msc	(DSPCOMPLEX *b, int blkno) {
	bufferSpace. acquire (1);
	for (int i = 0; i < params. get_T_u (); i ++)
	   command [blkno][i] = b [i];
	helper. lock ();
        commandHandler. wakeOne ();
	helper. unlock ();
}
//
//	The thread will be started - and stopped - by its owner,
//	the dabProcessor. 
//	Reasons to stop are switch of channel (or related, switch
//	of device)
//	Changing a service is done while processing continues
//
void    mscHandler::run () {
std::atomic<int>	currentBlock;
std::vector<int16_t> ibits (BitsperBlock);
int	T_u		= params. get_T_u ();

	fprintf (stderr, "mscHandler starts\n");
	if (running. load ()) {		// should not happen
	   fprintf (stderr, "we draaien al!!!\n");
	   return;
	}

	currentBlock. store (0);
	running. store (true);
        while (running. load()) {
	   int amount = nrBlocks - bufferSpace. available ();
	   while ((amount == 0) && running. load ()) {
              helper. lock();
              commandHandler. wait (&helper, 100);
	      amount = nrBlocks - bufferSpace. available ();
              helper. unlock();
	   }

	   if (!running. load ())
	      break;
//
//	now we know that amount > 0
	   memcpy (fft_buffer,
	              command [currentBlock. load ()]. data (),
	                               T_u * sizeof (DSPCOMPLEX));
//	block 3 and up are needed as basis for demodulation the "mext" block
//	"our" msc blocks start with blkno 4
	   my_fftHandler. do_FFT();
           if (currentBlock. load () >= 4) {
              for (int i = 0; i < params. get_carriers(); i ++) {
                 int16_t      index   = myMapper. mapIn (i);
                 if (index < 0)
                    index += params. get_T_u();

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
           bufferSpace. release (1);
           currentBlock. store ((currentBlock. load () + 1) % (nrBlocks));
        }
}

//
//	This function is called whenever the radio selects a different
//	channel. stopChannel will stop the mscHandler, startChannel
//	will restart it.
void	mscHandler::stop () {
	fprintf (stderr, "mscHandler stopt\n");
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
	fprintf (stderr, "resource available %d\n",
	                     bufferSpace. available ());
	bufferSpace. release (nrBlocks - bufferSpace. available ());
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

