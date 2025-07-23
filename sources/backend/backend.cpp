#
/*
 *    Copyright (C) 2016 .. 2024
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
#include	"backend.h"
#include	"logger.h"
//
//	Interleaving is - for reasons of simplicity - done
//	inline rather than through a special class-object
#define CUSize  (4 * 16)

//	fragmentsize == Length * CUSize
	Backend::Backend	(RadioInterface *mr,
	                         logger		*theLogger,
	                         descriptorType	*d,
	                         RingBuffer<std::complex<int16_t>> *audiobuffer,
	                         RingBuffer<uint8_t> *databuffer,	
	                         RingBuffer<uint8_t> *frameBuffer,
	                         FILE *dump, int flag,
	                         uint8_t	cpuSupport):
	                                    deconvolver (d, cpuSupport),
	                                    hardBits (d -> bitRate * 24),
	                                    driver (mr,
	                                            theLogger, 
	                                            d,
	                                            flag == BACK_GROUND,
	                                            audiobuffer,
	                                            databuffer,
	                                            frameBuffer, dump) 
#ifdef	__THREADED_BACKEND__
	                                    ,freeSlots (NUMBER_SLOTS) 
#endif 
	                                          {
	this	-> radioInterface	= mr;
	this	-> startAddr		= d -> startAddr;
	this	-> Length		= d -> length;
        this    -> fragmentSize         = d -> length * CUSize;
	this	-> bitRate		= d -> bitRate;
	this	-> serviceId		= d -> SId;
	this	-> serviceName		= d -> serviceName;
	this	-> shortForm		= d -> shortForm;
	this	-> protLevel		= d -> protLevel;
	this	-> subChId		= d -> subchId;
	this	-> borf			= flag;

//	fprintf (stderr, "starting a backend for %s (%X) %d\n",
//	                  serviceName. toUtf8 (). data (),
//	                                    serviceId, startAddr);
	interleaveData. resize (16);
	for (int i = 0; i < 16; i ++) {
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
	for (int i = 0; i < bitRate * 24; i ++) {
	   uint8_t b = shiftRegister [8] ^ shiftRegister [4];
	   for (int j = 8; j > 0; j--)
	      shiftRegister [j] = shiftRegister [j - 1];
	   shiftRegister [0] = b;
	   disperseVector [i] = b;
	}
#ifdef	__THREADED_BACKEND__
//	for local buffering the input, we have
	nextIn				= 0;
	nextOut				= 0;
	for (int i = 0; i < NUMBER_SLOTS; i ++)
	   theData [i]. resize (fragmentSize);
	running. store (true);
	start ();
#endif
}

	Backend::~Backend () {
#ifdef	__THREADED_BACKEND__
	running. store (false);
	while (this -> isRunning())
	   usleep (1000);
#endif
}

int32_t	Backend::process	(int16_t *softBits, int16_t cnt) {
	(void)cnt;
#ifdef	__THREADED_BACKEND__
	while (!freeSlots. tryAcquire (1, 200))
	   if (!running)
	      return 0;
	memcpy (theData [nextIn]. data (), softBits,
	                           fragmentSize * sizeof (int16_t));
	nextIn = (nextIn + 1) % NUMBER_SLOTS;
	usedSlots. release (1);
#else
	processSegment (softBits);
#endif
	return 1;
}

const	int16_t interleaveMap [] = {0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15};
void	Backend::processSegment (int16_t *softBits_in) {

	for (uint16_t i = 0; i < fragmentSize; i ++) {
	   tempX [i] = interleaveData [(interleaverIndex + 
	                                interleaveMap [i & 017]) & 017][i];
	   interleaveData [interleaverIndex][i] = softBits_in [i];
	}

	interleaverIndex = (interleaverIndex + 1) & 0x0F;
#ifdef	__THREADED_BACKEND__
	nextOut = (nextOut + 1) % NUMBER_SLOTS;
	freeSlots. release (1);
#endif

//	only continue when de-interleaver is filled
	if (countforInterleaver <= 15) {
	   countforInterleaver ++;
	   return;
	}

	deconvolver. deconvolve (tempX. data(), fragmentSize, hardBits. data());
//	and the energy dispersal
	for (uint16_t i = 0; i < bitRate * 24; i ++)
	   hardBits [i] ^= disperseVector [i];

	driver. addtoFrame (hardBits);
}

#ifdef	__THREADED_BACKEND__
void	Backend::run() {

	while (running. load()) {
	   while (!usedSlots. tryAcquire (1, 200)) 
	      if (!running)
	         return;
	   processSegment (theData [nextOut]. data());
	}
}
#endif

//	It might take a msec for the task to stop
void	Backend::stopRunning() {
#ifdef	__THREADED_BACKEND__
	running = false;
	while (this -> isRunning())
	   usleep (1);
//	myAudioSink	-> stop();
#endif
}

