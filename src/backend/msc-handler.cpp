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
static int cifTable [] = {18, 72, 0, 36};

//	Note CIF counts from 0 .. 3
//
		mscHandler::mscHandler	(RadioInterface *mr,
	                                 uint8_t	dabMode,
	                                 QString	picturesPath) :
	                                       params (dabMode),
	                                       my_fftHandler (dabMode),
	                                       myMapper (dabMode),
	                                       bufferSpace (params. get_L ()){
	myRadioInterface	= mr;
	this	-> picturesPath	= picturesPath;
	cifVector. resize (55296);
	theBackends. push_back (new virtualBackend (0, 0));
	BitsperBlock		= 2 * params. get_carriers ();
	nrBlocks		= params. get_L ();

	command. resize (nrBlocks);
	for (int i = 0; i < nrBlocks; i ++)
	   command [i]. resize (params. get_T_u ());
	amount          = 0;
	fft_buffer                      = my_fftHandler. getVector ();
	phaseReference                  .resize (params. get_T_u ());

	numberofblocksperCIF = cifTable [(dabMode - 1) & 03];
	work_to_be_done. store (false);
}

		mscHandler::~mscHandler	(void) {
	running. store (false);
	while (isRunning ())
	   usleep (100);
	locker. lock ();
	work_to_be_done. store (false);
	for (auto const &b : theBackends) {
	   b -> stopRunning ();
	   delete b;
	}
	locker. unlock ();
	theBackends. resize (0);
}

//
//	Input is put into a buffer, a the code in a separate thread
//	will handle the data from the buffer
void	mscHandler::processBlock_0 (std::complex<float> *b) {
	bufferSpace. acquire (1);
	memcpy (command [0]. data (), b,
	            params. get_T_u () * sizeof (std::complex<float>));
	helper. lock ();
	amount ++;
        commandHandler. wakeOne ();
        helper. unlock ();
}

void	mscHandler::process_Msc	(std::complex<float> *b, int blkno) {
	bufferSpace. acquire (1);
        memcpy (command [blkno]. data (), b,
	            params. get_T_u () * sizeof (std::complex<float>));
        helper. lock ();
        amount ++;
        commandHandler. wakeOne ();
        helper. unlock ();
}

void    mscHandler::run        (void) {
int	currentBlock	= 0;
std::vector<int16_t> ibits;

	running. store (true);
	ibits. resize (BitsperBlock);
        while (running. load ()) {
           helper. lock ();
           commandHandler. wait (&helper, 100);
           helper. unlock ();
           while ((amount > 0) && running. load ()) {
	      memcpy (fft_buffer, command [currentBlock]. data (),
	                 params. get_T_u () * sizeof (std::complex<float>));
//
//	block 3 and up are needed as basis for demodulation the "mext" block
//	"our" msc blocks start with blkno 4
	      my_fftHandler. do_FFT ();
              if (currentBlock >= 4) {
                 for (int i = 0; i < params. get_carriers (); i ++) {
                    int16_t      index   = myMapper. mapIn (i);
                    if (index < 0)
                       index += params. get_T_u ();

                    std::complex<float>  r1 = fft_buffer [index] *
                                       conj (phaseReference [index]);
                    float ab1    = jan_abs (r1);
//      Recall:  the viterbi decoder wants 127 max pos, - 127 max neg
//      we make the bits into softbits in the range -127 .. 127
                    ibits [i]            =  - real (r1) / ab1 * 127.0;
                    ibits [params. get_carriers () + i]
	                                 =  - imag (r1) / ab1 * 127.0;
                 }

	         process_mscBlock (ibits, currentBlock);
	      }
	      memcpy (phaseReference. data (), fft_buffer,
	                 params. get_T_u () * sizeof (std::complex<float>));
              bufferSpace. release (1);
              helper. lock ();
              currentBlock = (currentBlock + 1) % (nrBlocks);
              amount -= 1;
              helper. unlock ();
           }
        }
}
//	This function is to be called between invocations of
//	services
//	It might be called several times, so ...
void	mscHandler::reset	(void) {
int i;
	running. store (false);
	while (isRunning ())
	   usleep (100);
	locker. lock ();
	work_to_be_done. store (false);
	for (auto const &b : theBackends) {
	   b -> stopRunning ();
	   delete b;
	}
	theBackends. resize (0);

	bufferSpace. release (nrBlocks - bufferSpace. available ());
	locker. unlock ();
	start ();
}

void	mscHandler::stop	(void) {
	reset ();
}
//
//	Note, the set_xxx functions are called from within a
//	different thread than the process_mscBlock method is,
//	so, a little bit of locking seems wise while
//	the actual changing of the settings is done in the
//	thread executing process_mscBlock
void	mscHandler::set_audioChannel (audiodata *d,
	                              RingBuffer<int16_t> *audioBuffer) {
	locker. lock ();
//
//	we could assert here that theBackend == nullptr
	theBackends. push_back (new audioBackend (myRadioInterface,
	                                          d,
	                                          audioBuffer,
	                                          picturesPath));
	work_to_be_done. store (true);
	locker. unlock ();
}
//
void	mscHandler::set_dataChannel (packetdata	*d,
	                             RingBuffer<uint8_t> *dataBuffer) {
	locker. lock ();
	theBackends. push_back (new dataBackend (myRadioInterface,
	                                         d,
	                                         dataBuffer,
	                                         picturesPath));
	work_to_be_done. store (true);
	locker. unlock ();
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
int16_t	i;

	currentblk	= (blkno - 4) % numberofblocksperCIF;
//	and the normal operation is:
	memcpy (&cifVector [currentblk * BitsperBlock],
	                    fbits. data (), BitsperBlock * sizeof (int16_t));
	if (currentblk < numberofblocksperCIF - 1) 
	   return;

	if (!work_to_be_done. load ())
	   return;

//	OK, now we have a full CIF and it seems there is some work to
//	be done.  We assume that the backend itself
//	does the work in a separate thread.
	locker. lock ();
	for (auto const& b: theBackends) {
	   int16_t startAddr	= b -> startAddr ();
	   int16_t Length	= b -> Length    (); 
	   if (Length > 0) {		// Length = 0? virtual Backend
	      int16_t temp [Length * CUSize];
	      memcpy (temp, &cifVector [startAddr * CUSize],
	                           Length * CUSize * sizeof (int16_t));
	      (void) b -> process (temp, Length * CUSize);
	   }
	}
	locker. unlock ();
}
//

