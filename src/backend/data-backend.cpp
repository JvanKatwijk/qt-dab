#
/*
 *    Copyright (C) 2015
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB
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
#include	"frame-processor.h"
#include	"data-backend.h"
#include	"eep-protection.h"
#include	"uep-protection.h"
#include	"data-processor.h"

//	Interleaving is - for reasons of simplicity - done
//	inline rather than through a special class-object
//	We could make a single handler for interleaving
//	and deconvolution, but it is a pretty simple operation
//	so for now keep it in-line
//
//	The main function of this class is to assemble the 
//	MSCdatagroups and dispatch to the appropriate handler
//
//	fragmentsize == Length * CUSize
	dataBackend::dataBackend(RadioInterface *mr,
	                         packetdata	*d,
	                         RingBuffer<uint8_t> *dataBuffer,
	                         QString	picturesPath) :
	                             virtualBackend (d -> startAddr,
	                                             d -> length),
	                            freeSlots (20) {
int32_t i, j;

	this	-> myRadioInterface	= mr;
	this	-> fragmentSize		= d -> length * CUSize;
	this	-> bitRate		= d -> bitRate;
	this	-> protLevel		= d -> protLevel;
	our_frameProcessor	= new dataProcessor (mr,
	                                             d,
	                                             dataBuffer,
	                                             picturesPath);
	nextIn                          = 0;
        nextOut                         = 0;
        for (i = 0; i < 20; i ++)
           theData [i] = new int16_t [fragmentSize];

	outV			= new uint8_t [24 * bitRate];
	interleaveData		= new int16_t *[16]; // the size
	for (i = 0; i < 16; i ++) {
	   interleaveData [i] = new int16_t [fragmentSize];
	   memset (interleaveData [i], 0, fragmentSize * sizeof (int16_t));
	}
	countforInterleaver	= 0;
//
//	The handling of the depuncturing and deconvolution is
//	shared with that of the audio
	if (d -> shortForm)
	   protectionHandler	= new uep_protection (bitRate,
	                                              d -> protLevel);
	else
	   protectionHandler	= new eep_protection (bitRate,
	                                              d -> protLevel);
//
	uint8_t shiftRegister [9];
	disperseVector. resize (24 * bitRate);
//	and the energy dispersal
	memset (shiftRegister, 1, 9);
	for (i = 0; i < bitRate * 24; i ++) {
	   uint8_t b = shiftRegister [8] ^ shiftRegister [4];
	   for (j = 8; j > 0; j--)
	      shiftRegister [j] = shiftRegister [j - 1];
	   shiftRegister [0] = b;
	   disperseVector[i] = b;
	}

	start ();
}

	dataBackend::~dataBackend (void) {
int16_t	i;
	running = false;
	while (this -> isRunning ())
	   usleep (1);
	delete protectionHandler;
	for (i = 0; i < 16; i ++)
	   delete[] interleaveData [i];
        for (i = 0; i < 20; i ++)
           delete [] theData [i];
	delete[]	interleaveData;
	delete		outV;
}

int32_t	dataBackend::process	(int16_t *v, int16_t cnt) {
	(void)cnt;
	while (!freeSlots. tryAcquire (1, 200))
           if (!running)
              return 0;
        memcpy (theData [nextIn], v, fragmentSize * sizeof (int16_t));
        nextIn = (nextIn + 1) % 20;
        usedSlots. release ();
        return 1;
}

const   int16_t interleaveMap[] = {0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15};

void	dataBackend::run	(void) {
int16_t	countforInterleaver	= 0;
int16_t interleaverIndex	= 0;
int16_t	tempX [fragmentSize];
int16_t	i, j;

	running	= true;
	while (running) {
	   while (!usedSlots. tryAcquire (1, 200))
              if (!running)
                 return;

//	   memcpy (Data, theData [nextOut], fragmentSize * sizeof (int16_t));

	   for (i = 0; i < fragmentSize; i ++) {
	      tempX [i] = interleaveData [(interleaverIndex + 
	                                 interleaveMap [i & 017]) & 017][i];
	      interleaveData [interleaverIndex][i] = theData [nextOut] [i];
	   }
           nextOut = (nextOut + 1) % 20;
	   freeSlots. release ();

	   interleaverIndex = (interleaverIndex + 1) & 0x0F;

//	only continue when de-interleaver is filled
	   if (countforInterleaver <= 15) {
	      countforInterleaver ++;
	      continue;
	   }
//
	   protectionHandler -> deconvolve (tempX, fragmentSize, outV);

	   for (i = 0; i < bitRate * 24; i ++)
	      outV [i] ^= disperseVector [i];
//	What we get here is a long sequence (24 * bitrate) of bits, not packed
//	but forming a DAB packet
//	we hand it over to make an MSC data group
	   our_frameProcessor -> addtoFrame (outV);
	}
}

//	It might take a msec for the task to stop
void	dataBackend::stopRunning (void) {
	running = false;
	while (this -> isRunning ())
	   usleep (100);
}
//
