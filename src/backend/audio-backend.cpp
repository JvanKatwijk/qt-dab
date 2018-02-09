#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
#include	"audio-backend.h"
#include	"mp2processor.h"
#include	"mp4processor.h"
#include	"eep-protection.h"
#include	"uep-protection.h"
#include	"radio.h"
//
//	Interleaving is - for reasons of simplicity - done
//	inline rather than through a special class-object
//
//	fragmentsize == Length * CUSize
	audioBackend::audioBackend	(RadioInterface *mr,
	                                 audiodata	*d,
	                                 RingBuffer<int16_t> *buffer,
	                                 QString	picturesPath):
	                                    virtualBackend (d -> startAddr,
	                                                    d -> length),
	                                    outV (d -> bitRate * 24)
#ifdef	__THREADED_BACKEND
	                             ,freeSlots (20) 
#endif 
	                                          {
int32_t i, j;
	this	-> myRadioInterface	= mr;
	this    -> dabModus             = d -> ASCTy == 077 ? DAB_PLUS : DAB;
        this    -> fragmentSize         = d -> length * CUSize;
        this    -> bitRate              = d -> bitRate;
        this    -> shortForm            = d -> shortForm;
        this    -> protLevel            = d -> protLevel;

	this	-> audioBuffer		= buffer;

	interleaveData		= new int16_t *[16]; // max size
	for (i = 0; i < 16; i ++) {
	   interleaveData [i] = new int16_t [fragmentSize];
	   memset (interleaveData [i], 0, fragmentSize * sizeof (int16_t));
	}

	countforInterleaver	= 0;
	interleaverIndex	= 0;

	if (shortForm)
	   protectionHandler	= new uep_protection (bitRate,
	                                              protLevel);
	else
	   protectionHandler	= new eep_protection (bitRate,
	                                              protLevel);
//
	if (dabModus == DAB) 
	   our_dabProcessor = new mp2Processor (myRadioInterface,
	                                        bitRate,
	                                        audioBuffer,
	                                        picturesPath);
	else
	if (dabModus == DAB_PLUS) 
	   our_dabProcessor = new mp4Processor (myRadioInterface,
	                                        bitRate,
	                                        audioBuffer,
	                                        picturesPath);
	else		// cannot happen
	   our_dabProcessor = new frameProcessor ();

	fprintf (stderr, "we now have %s\n", dabModus == DAB_PLUS ? "DAB+" : "DAB");
	tempX. resize (fragmentSize);
	
	uint8_t shiftRegister [9];
	disperseVector. resize (24 * bitRate);
	memset (shiftRegister, 1, 9);
	for (i = 0; i < bitRate * 24; i ++) {
	   uint8_t b = shiftRegister [8] ^ shiftRegister [4];
	   for (j = 8; j > 0; j--)
	      shiftRegister [j] = shiftRegister [j - 1];
	   shiftRegister [0] = b;
	   disperseVector [i] = b;
	}
#ifdef	__THREADED_BACKEND
//	for local buffering the input, we have
	nextIn				= 0;
	nextOut				= 0;
	for (i = 0; i < 20; i ++)
	   theData [i] = new int16_t [fragmentSize];
	running. store (true);
	start ();
#endif
}

	audioBackend::~audioBackend (void) {
int16_t	i;
#ifdef	__THREADED_BACKEND
	running. store (false);
	while (this -> isRunning ())
	   usleep (1);
#endif
	delete protectionHandler;
	delete our_dabProcessor;
	for (i = 0; i < 16; i ++) 
	   delete[]  interleaveData [i];
	delete [] interleaveData;
#ifdef	__THREADED_BACKEND
	for (i = 0; i < 20; i ++)
	   delete [] theData [i];
#endif
}

int32_t	audioBackend::process	(int16_t *v, int16_t cnt) {

#ifdef	__THREADED_BACKEND
	while (!freeSlots. tryAcquire (1, 200))
	   if (!running)
	      return 0;
	memcpy (theData [nextIn], v, fragmentSize * sizeof (int16_t));
	nextIn = (nextIn + 1) % 20;
	usedSlots. release ();
#else
	processSegment (v);
#endif
	return 1;
}


const	int16_t interleaveMap [] = {0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15};
void	audioBackend::processSegment (int16_t *Data) {
int16_t	i, j;

	for (i = 0; i < fragmentSize; i ++) {
	   tempX [i] = interleaveData [(interleaverIndex + 
	                                interleaveMap [i & 017]) & 017][i];
	   interleaveData [interleaverIndex][i] = Data [i];
	}

	interleaverIndex = (interleaverIndex + 1) & 0x0F;
#ifdef	__THREADED_BACKEND
	nextOut = (nextOut + 1) % 20;
	freeSlots. release ();
#endif

//	only continue when de-interleaver is filled
	if (countforInterleaver <= 15) {
	   countforInterleaver ++;
	   return;
	}

	protectionHandler -> deconvolve (tempX. data (),
	                                 fragmentSize,
	                                 outV. data ());
//
//	and the energy dispersal
	for (i = 0; i < bitRate * 24; i ++)
	   outV [i] ^= disperseVector [i];

	our_dabProcessor -> addtoFrame (outV);
}

#ifdef	__THREADED_BACKEND
void	audioBackend::run	(void) {

	while (running. load ()) {
	   while (!usedSlots. tryAcquire (1, 200)) 
	      if (!running)
	         return;
	   processSegment (theData [nextOut]);
	}
}
#endif

//	It might take a msec for the task to stop
void	audioBackend::stopRunning (void) {
#ifdef	__THREADED_BACKEND
	running = false;
	while (this -> isRunning ())
	   usleep (1);
//	myAudioSink	-> stop ();
#endif
}

