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
	                                       frameBuffer (frameBuffer_i)
#ifdef	__MSC_THREAD__
	                                       ,fft (params. get_T_u (), false)
	                                       ,bufferSpace (params. get_L())
#endif		                            
{
	this	-> theLogger	= theLogger;
	this	-> cpuSupport	= cpuSupport;
	cifVector. resize (55296);
	BitsperBlock		= 2 * params. get_carriers();
	softBits. resize (BitsperBlock);
	nrBlocks		= params. get_L();
	connect (this, &mscHandler::nrServices,
	         mr, &RadioInterface::nrActiveServices);
	

	numberofblocksperCIF = cifTable [(dabMode - 1) & 03];
#ifdef	__MSC_THREAD__
	phaseReference	.resize (params. get_T_u());
	command. resize (nrBlocks);
	for (int i = 0; i < nrBlocks; i ++)
	   command [i]. resize (params. get_T_u());
	amount          = 0;
	running. store (false);
	start ();
#endif
}

		mscHandler::~mscHandler () {
#ifdef	__MSC_THREAD__
	running. store (false);
	while (isRunning())
	   usleep (100);
#endif
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
#ifdef	__MSC_THREAD__
	bufferSpace. acquire (1);
	memcpy (command [0]. data(), b,
	            params. get_T_u() * sizeof (Complex));
	helper. lock();
	amount ++;
        commandHandler. wakeOne();
        helper. unlock();
#else
	(void)b;
	fprintf (stderr, "Why am I called?\n");
#endif
}

#ifdef	__MSC_THREAD__
void	mscHandler::processMsc	(std::vector<Complex> &b,
	                                   int offset,  int blkno) {
	bufferSpace. acquire (1);
        memcpy (command [blkno]. data (), &(b. data ())[offset],
	            params. get_T_u() * sizeof (Complex));
        helper. lock();
        amount ++;
        commandHandler. wakeOne();
        helper. unlock();
}

void    mscHandler::run () {
int	currentBlock	= 0;
Complex fft_buffer [params. get_T_u ()];
Complex conjVector [params. get_T_u ()];
int	carriers	= params. get_carriers ();

	if (running. load ()) {
	   fprintf (stderr, "already running\n");
	   return;
	}

	running. store (true);
        while (running. load()) {
           helper. lock();
           commandHandler. wait (&helper, 100);
           helper. unlock();
           while ((amount > 0) && running. load()) {
	      memcpy (fft_buffer, command [currentBlock]. data(),
	                 params. get_T_u() * sizeof (Complex));
//
//	block 3 and up are needed as basis for demodulation the "mext" block
//	"our" msc blocks start with blkno 4
	      fft. fft (fft_buffer);
              if (currentBlock >= 4) {
                 for (int i = 0; i < carriers; i ++) {
                    int16_t      index   = myMapper. mapIn (i);
                    if (index < 0)
                       index += params. get_T_u();

                    Complex  r1 = fft_buffer [index] *
                                       conj (phaseReference [index]);
	            conjVector [index] = r1;

	            float ab1	= jan_abs (r1);
                    softBbits [i]	=  (int16_t) (- real (r1) * MAX_VITERBI / ab1);
                    softBits [carriers + i] =
	                           (int16_t) (- imag (r1) * MAX_VITERBI / ab1);
                 }

	         processMscBlock (softBits, currentBlock);
	      }
	      memcpy (phaseReference. data (), fft_buffer,
	                 params. get_T_u() * sizeof (Complex));
              bufferSpace. release (1);
              helper. lock();
              currentBlock = (currentBlock + 1) % (nrBlocks);
              amount -= 1;
              helper. unlock();
           }
        }
}
#else
void	mscHandler::processMsc	(std::vector<Complex> &b,
	                                      int offset, int blkno) {
	(void)b;
	(void)offset;
	(void)blkno;
	fprintf (stderr, "I should not be called\n");
}
#endif
//
//	Note, the set_Channel function is called from within a
//	different thread than the process_mscBlock method is,
//	so, a little bit of locking seems wise while
//	the actual changing of the settings is done in the
//	thread executing process_mscBlock
void	mscHandler::resetBuffers	() {
	resetChannel ();
#ifdef	__MSC_THREAD__
	running. store (false);
	while (isRunning ())
	   wait (100);
	bufferSpace. release (params. get_L () - bufferSpace. available ());
	start ();
#endif
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
	nrServices ((int)(theBackends. size ()));
}

void	mscHandler::stopBackend	(int subchId, int flag) {
	locker. lock ();
	for (int i = 0; i < (int)(theBackends. size ());  i ++) {
	   Backend *b = theBackends. at (i);
	   if ((b -> subChId == subchId) && (b -> borf == flag)) {
//	      fprintf (stderr, "stopping subchannel %d\n", subchId);
	      b -> stopRunning ();
	      delete b;
	      theBackends. erase (theBackends. begin () + i);
	      break;
	   }
	}
	locker. unlock ();
	nrServices ((int)(theBackends. size ()));
}
//
//	Note that - in general - the backens run in their own thread
bool	mscHandler::startBackend (descriptorType &d,
	                          RingBuffer<std::complex<int16_t>> *audioBuffer,
	                          RingBuffer<uint8_t> *dataBuffer,
	                          FILE *dump, int flag) {
//	fprintf (stderr, "going to open %s\n",
//	                d. serviceName. toLatin1 (). data ());

	theBackends. push_back (new Backend (myRadioInterface,
	                                     theLogger,
	                                     &d,
	                                     audioBuffer,
	                                     dataBuffer,
	                                     frameBuffer,
	                                     dump,
	                                     flag,
	                                     cpuSupport)); 
	nrServices ((int)(theBackends. size ()));
	return true;
}

bool	mscHandler::serviceRuns	(uint32_t SId, uint16_t subChId) {
	for (auto &backend : theBackends)
	   if ((backend -> serviceId == SId) && (backend -> subChId == subChId))
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
