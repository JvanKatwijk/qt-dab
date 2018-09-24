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
#include	"radio.h"
#include	"backend.h"
//
//	Interleaving is - for reasons of simplicity - done
//	inline rather than through a special class-object
#define CUSize  (4 * 16)

//	fragmentsize == Length * CUSize
	Backend::Backend	(RadioInterface *mr,
	                         descriptorType	*d,
	                         RingBuffer<int16_t> *audiobuffer,
	                         RingBuffer<uint8_t> *databuffer,
	                         QString	picturesPath):
	                                    outV (d -> bitRate * 24),
	                                    driver (mr, 
	                                            d,
	                                            audiobuffer,
	                                            databuffer,
	                                            picturesPath),
	                                    deconvolver (d)
#ifdef	__THREADED_BACKEND
	                             ,freeSlots (20) 
#endif 
	                                          {
int32_t i, j;
	this	-> startAddr		= d -> startAddr;
	this	-> Length		= d -> length;
        this    -> fragmentSize         = d -> length * CUSize;
	this	-> bitRate		= d -> bitRate;
	interleaveData. resize (16);
	for (i = 0; i < 16; i ++) {
	   interleaveData [i]. resize (fragmentSize);
	   memset (interleaveData [i]. data (), 0,
	                               fragmentSize * sizeof (int16_t));
	}

	countforInterleaver	= 0;
	interleaverIndex	= 0;

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
	   theData [i]. resize (fragmentSize);
	running. store (true);
	start ();
#endif
}

	Backend::~Backend (void) {
#ifdef	__THREADED_BACKEND
	running. store (false);
	while (this -> isRunning ())
	   usleep (1000);
#endif
}

int32_t	Backend::process	(int16_t *v, int16_t cnt) {
	(void)cnt;
#ifdef	__THREADED_BACKEND
	while (!freeSlots. tryAcquire (1, 200))
	   if (!running)
	      return 0;
	memcpy (theData [nextIn]. data (), v, fragmentSize * sizeof (int16_t));
	nextIn = (nextIn + 1) % 20;
	usedSlots. release ();
#else
	processSegment (v);
#endif
	return 1;
}

const	int16_t interleaveMap [] = {0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15};
void	Backend::processSegment (int16_t *Data) {
int16_t	i;

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

	deconvolver. deconvolve (tempX. data (), fragmentSize, outV. data ());
//	and the energy dispersal
	for (i = 0; i < bitRate * 24; i ++)
	   outV [i] ^= disperseVector [i];

	driver. addtoFrame (outV);
}

#ifdef	__THREADED_BACKEND
void	Backend::run	(void) {

	while (running. load ()) {
	   while (!usedSlots. tryAcquire (1, 200)) 
	      if (!running)
	         return;
	   processSegment (theData [nextOut]. data ());
	}
}
#endif

//	It might take a msec for the task to stop
void	Backend::stopRunning (void) {
#ifdef	__THREADED_BACKEND
	running = false;
	while (this -> isRunning ())
	   usleep (1);
//	myAudioSink	-> stop ();
#endif
}

