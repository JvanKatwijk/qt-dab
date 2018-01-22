#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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

#include	"dab-constants.h"	// some general definitions
#include	"elad-worker.h"		// our header
#include	"elad-handler.h"
#include	"elad-loader.h"		// the API definition
#include	"ringbuffer.h"		// the buffer

//	The elad-worker is a simple wrapper around the elad
//	interface. It is a pretty simple thread performing the
//	basic functions, it reads the bytes, converts them to
//	samples and converts the rate to 2048000
//	
	eladWorker::eladWorker (int32_t		defaultFreq,
	                        eladLoader	*f,
	                        eladHandler	*h,
	                        RingBuffer<std::complex<float>> *theBuffer,
	                        bool	*OK) {
int	i;
	fprintf (stderr, "creating a worker\n");
	_I_Buffer		= new RingBuffer<uint8_t> (16 * 32768);
	fprintf (stderr, "local buffer allocated\n");
	this	-> theRate	= 3072000;
	this	-> defaultFreq	= defaultFreq;
	this	-> functions	= f;
	this	-> theBuffer	= theBuffer;
	*OK			= false;	// just the default
	iqSwitch		= false;

	theFilter		= new  eladFilter (5, 1024000, 3072000);
	conversionNumber	= theRate == 192000 ? 1:
	                          theRate <= 3072000 ? 2 : 3;
//
//	we convert to complexes directly
	iqSize			= conversionNumber == 3 ? 4 : 8;
	convBufferSize          = theRate / 1000;
	convIndex		= 0;
	convBuffer 		= new std::complex<float> [convBufferSize + 1];

	fprintf (stderr, "iqSize = %d, conversion = %d\n",	
	                       iqSize, conversionNumber);
//      The sizes of the mapTable and the convTable are
//      predefined and follow from the input and output rate
//      (theRate / 1000) vs (2048000 / 1000)
        for (i = 0; i < 2048; i ++) {
           float inVal  = float (theRate / 1000);
           mapTable_int [i] =  int (floor (i * (inVal / 2048.0)));
           mapTable_float [i] = i * (inVal / 2048.0) - mapTable_int [i];
        }
	fprintf (stderr, "mapTables initialized\n");
//
	fprintf (stderr, "testing functions\n");
	if (!functions	-> OK ())
	   return;
	lastFrequency		= defaultFreq;	// the parameter!!!!
	runnable		= true;
	fprintf (stderr, "functions are OK\n");
	functions	-> StartFIFO (functions -> getHandle ());
	connect (this, SIGNAL (show_eladFrequeny (int)),
	         h, SLOT (show_eladFrequency (int)));
	connect (this, SIGNAL (show_iqSwitch (bool)),
	         h, SLOT (show_iqSwitch (bool)));
	start ();
	*OK			= true;
}

//	As usual, killing objects containing a thread need to
//	be done carefully.
void	eladWorker::stop	(void) {
	if (runnable)
	   functions	-> StopFIFO (functions -> getHandle ());
	runnable	= false;
}

	eladWorker::~eladWorker	(void) {
	stop ();
	while (isRunning ())
	   msleep (1);
	delete _I_Buffer;
	delete [] convBuffer;
}

std::complex<float>	makeSample_31bits (uint8_t *);
std::complex<float>	makeSample_30bits (uint8_t *, bool);
std::complex<float>	makeSample_15bits (uint8_t *);

typedef union {
	struct __attribute__((__packed__)) {
		float	i;
		float	q;
		} iqf;
	struct __attribute__((__packed__)) {
		int32_t	i;
		int32_t	q;
		} iq;
	struct __attribute__((__packed__)) {
		uint8_t		i1;
		uint8_t		i2;
		uint8_t		i3;
		uint8_t		i4;
		uint8_t		q1;
		uint8_t		q2;
		uint8_t		q3;
		uint8_t		q4;
		};
} iq_sample;

#define	SCALE_FACTOR_30 1073741824.000
#define	SCALE_FACTOR_29 536970912.000
#define	SCALE_FACTOR_14 16384.000


std::complex<float>	makeSample_31bits (uint8_t *buf) {
int ii = 0; int qq = 0;
int16_t	i = 0;

	uint8_t q0 = buf [i++];
	uint8_t q1 = buf [i++];
	uint8_t q2 = buf [i++];
	uint8_t q3 = buf [i++];

	uint8_t i0 = buf [i++];
	uint8_t i1 = buf [i++];
	uint8_t i2 = buf [i++];
	uint8_t i3 = buf [i++];

	ii = (i3 << 24) | (i2 << 16) | (i1 << 8) | i0;
	qq = (q3 << 24) | (q2 << 16) | (q1 << 8) | q0;
	return std::complex<float> ((float)qq / SCALE_FACTOR_30,
	                   (float)ii / SCALE_FACTOR_30);
	return std::complex<float> ((float)ii / SCALE_FACTOR_30,
	                   (float)qq / SCALE_FACTOR_30);
}


std::complex<float>	makeSample_30bits (uint8_t *buf, bool flag) {
int ii = 0; int qq = 0;
int16_t	i = 0;

	uint8_t q0 = buf [i++];
	uint8_t q1 = buf [i++];
	uint8_t q2 = buf [i++];
	uint8_t q3 = buf [i++];

	uint8_t i0 = buf [i++];
	uint8_t i1 = buf [i++];
	uint8_t i2 = buf [i++];
	uint8_t i3 = buf [i++];

	ii = (i3 << 24) | (i2 << 16) | (i1 << 8) | i0;
	qq = (q3 << 24) | (q2 << 16) | (q1 << 8) | q0;
	if (flag) 
	   return std::complex<float> ((float)qq / SCALE_FACTOR_29,
	                               (float)ii / SCALE_FACTOR_29);
	else
	   return std::complex<float> ((float)ii / SCALE_FACTOR_29,
	                               (float)qq / SCALE_FACTOR_29);
}
//
std::complex<float>	makeSample_15bits (uint8_t *buf) {
int ii	= 0; int qq = 0;
int16_t	i = 0;

              ii   = (int)((unsigned char)(buf[i++]));
              ii  += (int)((unsigned char)(buf[i++])) << 8;
              qq   = (int)((unsigned char)(buf[i++]));
              qq  += (int)((unsigned char)(buf[i++])) << 8;
              return std::complex<float> ((float)ii / SCALE_FACTOR_14,
	                                  (float)ii / SCALE_FACTOR_14);
              return std::complex<float> ((float)qq / SCALE_FACTOR_14,
	                                  (float)ii / SCALE_FACTOR_14);
}

#define	BUFFER_SIZE	(8 * 8192)
uint8_t buffer [BUFFER_SIZE];
//
//	To make life easy, we do all handling in this task,
//	its "output", i.e. the shared buffer contains the
//	samples, type complex, representing a samplerate 2048000
//	Every millisecond we add 2048 samples by converting
//	3072 samples
void	eladWorker:: run (void) {
int32_t	amount;
int	rc, i;

//	when (re)starting, clean up first
	_I_Buffer	-> FlushRingBuffer ();

	fprintf (stderr, "worker thread started\n");

	while (runnable) {
	   rc = libusb_bulk_transfer (functions -> getHandle (),
	                              (6 | LIBUSB_ENDPOINT_IN),
	                              (uint8_t *)buffer,
	                              BUFFER_SIZE * sizeof (uint8_t),
	                              &amount,
	                              2000);
	   if (rc) {
              fprintf (stderr,
	               "Error in libusb_bulk_transfer: [%d] %s\n",
	               rc,
	               libusb_error_name (rc));
	      if (rc != 7)
	         break;
	   }
//
//	Since we do not know whether the amount read is a multiple
//	of iqSize, we use an intermediate buffer
	   _I_Buffer	-> putDataIntoBuffer (buffer, amount);
	   while (_I_Buffer -> GetRingBufferReadAvailable () >= iqSize * 1024) {
	      uint8_t myBuffer [iqSize * 1024];
	      _I_Buffer -> getDataFromBuffer (myBuffer, iqSize * 1024);
//
//	Having read 1024 * iqSize bytes, we can make them into complex samples
//	and start converting the rate
	      for (i = 0; i < 1024; i ++) {
	         convBuffer [convIndex ++] =
//	                theFilter -> Pass (
//	                       makeSample_30bits (&myBuffer [iqSize * i],
//	                                          iqSwitch));
	                       makeSample_30bits (&myBuffer [iqSize * i],
	                                          iqSwitch);
	         if (convIndex > convBufferSize) {
	            std::complex<float> temp [2048];
	            int16_t j;
	            for (j = 0; j < 2048; j ++) {
	               int16_t  inpBase       = mapTable_int [j];
                       float    inpRatio      = mapTable_float [j];

                       temp [j]  = cmul (convBuffer [inpBase + 1], inpRatio) +
                                   cmul (convBuffer [inpBase], 1 - inpRatio);
                    }

                    theBuffer -> putDataIntoBuffer (temp, 2048);
//      shift the sample at the end to the beginning, it is needed
//      as the starting sample for the next time
	            convBuffer [0] = convBuffer [convBufferSize];
	            convIndex = 1;
	         }
	      }
	   }
	}
	fprintf (stderr, "eladWorker now stopped\n");
}

void	eladWorker::setVFOFrequency	(int32_t f) {
int	result;
int	realFreq;
	if (!runnable)
	   return;

	realFreq	= f % Khz (3072);
	iqSwitch	= ((f / Khz (3072)) & 01) == 01;
	lastFrequency	= f;
	result = functions -> SetHWLO (functions -> getHandle (),
	                                                 &lastFrequency);
	if (result == 1)
	   fprintf (stderr, "setting frequency to %d succeeded\n",
	                                              realFreq);
	else
	   fprintf (stderr, "setting frequency to %d failed\n",
	                                               realFreq);
	show_eladFrequency (realFreq);
	show_iqSwitch (iqSwitch);
}

int32_t	eladWorker::getVFOFrequency	(void) {
	return lastFrequency;
}

