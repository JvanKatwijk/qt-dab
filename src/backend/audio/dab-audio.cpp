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
#include	"dab-audio.h"
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
	dabAudio::dabAudio	(RadioInterface *mr,
	                         uint8_t dabModus,
	                         int16_t fragmentSize,
	                         int16_t bitRate,
	                         bool	shortForm,
	                         int16_t protLevel,
	                         RingBuffer<int16_t> *buffer,
	                         QString	picturesPath):
	                             outV (bitRate * 24)
#ifdef	__THREADED_DECODING
	                             ,freeSlots (20) 
#endif 
	                                          {
int32_t i, j;
	this	-> dabModus		= dabModus;
	this	-> fragmentSize		= fragmentSize;
	this	-> bitRate		= bitRate;
	this	-> shortForm		= shortForm;
	this	-> protLevel		= protLevel;
	this	-> myRadioInterface	= mr;
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
	Data		= new int16_t [fragmentSize];
	tempX		= new int16_t [fragmentSize];

#ifdef	__THREADED_DECODING
//	for local buffering the input, we have
	nextIn				= 0;
	nextOut				= 0;
	for (i = 0; i < 20; i ++)
	   theData [i] = new int16_t [fragmentSize];
	running. store (true);
	start ();
#endif
}

	dabAudio::~dabAudio	(void) {
int16_t	i;
#ifdef	__THREADED_DECODING
	running. store (false);
	while (this -> isRunning ())
	   usleep (1);
#endif
	delete protectionHandler;
	delete our_dabProcessor;
	for (i = 0; i < 16; i ++) 
	   delete[]  interleaveData [i];
	delete [] interleaveData;
#ifdef	__THREADED_DECODING
	for (i = 0; i < 20; i ++)
	   delete [] theData [i];
#endif
	delete [] tempX;
}

int32_t	dabAudio::process	(int16_t *v, int16_t cnt) {

#ifdef	__THREADED_DECODING
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
void	dabAudio::processSegment (int16_t *Data) {
uint8_t shiftRegister [9];
int16_t	i, j;

	for (i = 0; i < fragmentSize; i ++) {
	   tempX [i] = interleaveData [(interleaverIndex + 
	                                interleaveMap [i & 017]) & 017][i];
	   interleaveData [interleaverIndex][i] = Data [i];
	}

	interleaverIndex = (interleaverIndex + 1) & 0x0F;
#ifdef	__THREADED_DECODING
	nextOut = (nextOut + 1) % 20;
	freeSlots. release ();
#endif

//	only continue when de-interleaver is filled
	if (countforInterleaver <= 15) {
	   countforInterleaver ++;
	   return;
	}

	protectionHandler -> deconvolve (tempX, fragmentSize, outV. data ());
//
//	and the inline energy dispersal
	memset (shiftRegister, 1, 9);
	for (i = 0; i < bitRate * 24; i ++) {
	   uint8_t b = shiftRegister [8] ^ shiftRegister [4];
	   for (j = 8; j > 0; j--)
	      shiftRegister [j] = shiftRegister [j - 1];
	   shiftRegister [0] = b;
	   outV [i] ^= b;
	}

	our_dabProcessor -> addtoFrame (outV);
}

#ifdef	__THREADED_DECODING
void	dabAudio::run	(void) {

	while (running. load ()) {
	   while (!usedSlots. tryAcquire (1, 200)) 
	      if (!running)
	         return;
	   processSegment (theData [nextOut]);
	}
}
#endif

//	It might take a msec for the task to stop
void	dabAudio::stopRunning (void) {
#ifdef	__THREADED_DECODING
	running = false;
	while (this -> isRunning ())
	   usleep (1);
//	myAudioSink	-> stop ();
#endif
}

