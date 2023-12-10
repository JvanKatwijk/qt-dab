#
/*
 *    Copyright (C) 2020
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

#include	<QThread>
#include	<QSettings>
#include	<QHBoxLayout>
#include	<QLabel>
#include	<QMessageBox>
#include	<QDir>

#include	"device-exceptions.h"
#include	"elad-handler.h"	// our header
#include	"elad-worker.h"		// the thread, reading in the data
#include	"elad-loader.h"		// function loader
#include	<stdio.h>

//	Currently, we do not have lots of settings,
//	it just might change suddenly, but not today
		eladHandler::eladHandler (QSettings *s):
	                                     myFrame (nullptr),
	                                     _I_Buffer  (256 * 32768),
	                                     _O_Buffer  (16 * 32768) {
int16_t	success;

	this	-> eladSettings	= s;
	setupUi (&myFrame);
	myFrame. show ();
	theLoader		= nullptr;
	theWorker		= nullptr;
	Offset			= offsetSelector	-> value ();
	Nyquist			= NyquistWidth		-> value ();
//
//	sometimes problems with dynamic linkage of libusb, it is
//	loaded indirectly through the dll
	if (libusb_init (nullptr) < 0) {
	   throw (device_exception ("libusb problem"));
	}

	libusb_exit (nullptr);
	theLoader	= new eladLoader (ELAD_RATE, &success);
	if (success != 0) {
	   statusLabel -> setText ("not functioning");
	   delete theLoader;
	   theLoader	= nullptr;
	   switch (success) {
	      case -1:
	         throw (device_exception ("No success in loading libs\n"));

	      case -2:
	         throw (device_exception ("No success in setting up USB\n"));

	      case -3:
	         throw (device_exception ("No success in FPGA init\n"));
	   
	      case -4:
	         throw (device_exception ("No success in hardware init\n"));
	      
	      default:		// cannot happen
	         ;
	   }
	}
	statusLabel	-> setText ("Loaded");
//
	externalFrequency	= Khz (220000);
//
//	since localFilter and gainReduced are also used as
//	parameter for the API functions, they are int's rather
//	than bool.
	localFilter	= 1;
	filterText	-> setText ("no filter");
	gainReduced	= 1;
	gainLabel	-> setText ("0");

        for (int i = 0; i < 2048; i ++) {
           float inVal  = float (ELAD_RATE / 1000);
           mapTable_int [i] =  int (floor (i * (inVal / 2048.0)));
           mapTable_float [i] = i * (inVal / 2048.0) - mapTable_int [i];
        }
	convIndex	= 0;
	fprintf (stderr, "mapTables initialized\n");

	iqSize		= 8;
//
	connect (gainReduction, SIGNAL (clicked ()),
	         this, SLOT (setGainReduction ()));
	connect (filter, SIGNAL (clicked ()),
	         this, SLOT (setFilter ()));
	connect (NyquistWidth, SIGNAL (valueChanged (int)),
	         this, SLOT (set_NyquistWidth (int)));
	connect (offsetSelector, SIGNAL (valueChanged (int)),
	         this, SLOT (set_Offset (int)));
	connect (IQSwitch, SIGNAL (clicked ()),
	         this, SLOT (toggle_IQSwitch ()));
}
//
	eladHandler::~eladHandler	(void) {
	stopReader ();
//
//	theWorker refers to the loader, so clean up int his order
	if (theWorker != nullptr)
	   delete theWorker;
	if (theLoader != nullptr)
	   delete theLoader;
}

int32_t	eladHandler::defaultFrequency	(void) {
	return Khz (220000);
}

int32_t	eladHandler::getVFOFrequency	(void) {
	return externalFrequency;
}

bool	eladHandler::restartReader	(int32_t externalFrequency) {
bool	success;

	if (theWorker != nullptr) 
	   return true;

	this	-> externalFrequency	= externalFrequency;

	_O_Buffer. FlushRingBuffer ();
	_I_Buffer. FlushRingBuffer ();
//
//	first map the external frequency to an elad frequency

	int K = 1;
	iqSwitch. store (false);
	eladFrequency	= externalFrequency - MHz (Offset);
	while (eladFrequency > KHz (Nyquist)) {
	   eladFrequency -= KHz (Nyquist);
	   K ++;
	}
	if ((K & 01) == 0) {	// even
	   eladFrequency = KHz (Nyquist) - eladFrequency;
	   iqSwitch. store (true);
	}

	eladFrequencyDisplay	-> display (eladFrequency);
	show_iqSwitch	(iqSwitch. load ());
	fprintf (stderr, "external frequency %d, eladFreq %d\n",
	                     externalFrequency, eladFrequency);

//	Note that eladWorker now sends its output (_I_Buffer)
//	in the 3072000 samplerate
	theWorker	= new eladWorker (eladFrequency,
	                                  theLoader,
	                                  this,
	                                  &_I_Buffer,
	                                  &success);
	fprintf (stderr, "worker started, success = %d\n", success);
	return success;
}

void	eladHandler::stopReader	(void) {
	if (theWorker == nullptr) 
	   return;

	theWorker	-> stop ();
	while (theWorker -> isRunning ())
	   usleep (100);
	delete theWorker;
	theWorker = nullptr;
}

std::complex<float>	makeSample_31bits (uint8_t *, bool);

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

// ADC output unsigned 14 bit input to FPGA output signed 32 bit
// multiply output FPGA by SCALE_FACTOR to normalize 32bit signed values to ADC range signed 14 bit
#define SCALE_FACTOR_32to14    (0.000003814) //(8192/2147483648)
// ADC out unsigned 14 bit input to FPGA output signed 16 bit
#define SCALE_FACTOR_16to14    (0.250)       //(8192/32768)  

std::complex<float>	makeSample_31bits (uint8_t *buf, bool iqSwitch) {
int ii = 0; int qq = 0;
int16_t	i = 0;
uint32_t	uii = 0, uqq = 0;

	uint8_t i0 = buf [i++]; //i+0
        uint8_t i1 = buf [i++]; //i+1
        uint8_t i2 = buf [i++]; //i+2
        uint8_t i3 = buf [i++]; //i+3

        uint8_t q0 = buf [i++]; //i+4
        uint8_t q1 = buf [i++]; //i+5
        uint8_t q2 = buf [i++]; //i+6
        uint8_t q3 = buf [i++]; //i+7

// Andrea Montefusco recipe
// from four unsigned 8bit little endian order to unsigned 32bit (just move),
// then cast it to signed 32 bit
        uii = (i3 << 24) | (i2 << 16) | (i1 << 8) | i0;
        uqq = (q3 << 24) | (q2 << 16) | (q1 << 8) | q0;

        ii =(int)uii;
        qq =(int)uqq;

	if (iqSwitch)
	   return std::complex<float> ((float)qq * SCALE_FACTOR_32to14,
	                               (float)ii * SCALE_FACTOR_32to14);
	else
	   return std::complex<float> ((float)ii * SCALE_FACTOR_32to14,
	                               (float)qq * SCALE_FACTOR_32to14);
}

//	we are - in this context - certain that whenever getSamples
//	is called, there are sufficient samples available.
//
//	all conversion now has to be done while answering
//	a request for samples
//	a. if the _O_Buffer contains sufficient samples, then go
//	b. otherwise,
//	   1. we continuously read in BYTES from the _I_Buffer
//	   2. per group of iqSize BYTES we form a complex
//	   3. we fill the conversion buffer and
//	   4. from time to time, we convert the rate
//	   until the _O_Buffer is filled up with at least "size" samples
#define	SEGMENT_SIZE	(1024 * iqSize)
static
int	teller		= 0;
int32_t	eladHandler::getSamples (std::complex<float> *V, int32_t size) { 
uint8_t lBuf [SEGMENT_SIZE];
std::complex<float> temp [2048];
//
//	if we have sufficient samples in the buffer, go for it
	if (_O_Buffer. GetRingBufferReadAvailable () >= size) 
	   return _O_Buffer. getDataFromBuffer (V, size);
//
//	per cycle we read in SEGMENT_SIZE bytes, convert them
//	to 512 complex numbers and put these samples into the
//	rate converter
	while (Samples () < size)
	   usleep (500);

	while ((_I_Buffer. GetRingBufferReadAvailable () >= SEGMENT_SIZE) &&
	       (_O_Buffer. GetRingBufferReadAvailable () < size)) {
	   _I_Buffer. getDataFromBuffer (lBuf, SEGMENT_SIZE);

	   for (int i = 0; i < SEGMENT_SIZE / iqSize; i ++) {
	      convBuffer [convIndex ++] =
	                     makeSample_31bits (&lBuf [iqSize * i],
                                                iqSwitch. load ());
	      if (convIndex > ELAD_RATE / 1000) {
	         float sum = 0;
	         int16_t j;
	         for (j = 0; j < 2048; j ++) {
	            int16_t  inpBase		= mapTable_int [j];
	            float    inpRatio		= mapTable_float [j];
	            temp [j]  = convBuffer [inpBase + 1] * inpRatio +
                                convBuffer [inpBase] * (1 - inpRatio);
	            sum += abs (temp [j]);
                 }
	         if (++teller > 1000) {
	            //fprintf (stderr, "signal is %f dB\n",
//10 * log10 (sum / 2048 / 84));
	            teller = 0;
	         }
	
	         _O_Buffer. putDataIntoBuffer (temp, 2048);
//      shift the sample at the end to the beginning, it is needed
//      as the starting sample for the next time
                 convBuffer [0] = convBuffer [ELAD_RATE / 1000];
                 convIndex = 1;
              }
	   }
	}

	return _O_Buffer. getDataFromBuffer (V, size);
}
//
//	The amount of available samples available consists of two
//	parts
//	   a. the amount already in _O_Buffer
//	   b. the samples in the _I_Buffer, but these are bytes
//	      sampled at a rate of 3072000, so the number has to be adapted
//
//	Since the buffersize exceeds the 10^6, we need 64 bit ints
//	for the computation.
//	but the bottom line is that 3072 * iqSize input samples result in
//	2048 "output" samples
int32_t	eladHandler::Samples	(void) {
int64_t	bufferContent	= _I_Buffer. GetRingBufferReadAvailable ();
	return _O_Buffer. GetRingBufferReadAvailable () +
	       (int)(((int64_t)2048 * bufferContent / (int64_t)3072) / iqSize);
}

void	eladHandler::resetBuffer	(void) {
	_I_Buffer. FlushRingBuffer ();
}
//
//	Although we are getting 30-more bits in, the adc in the
//	elad gives us 14 bits. That + 20 db gain results in app 105 db
//	plus a marge it is app 120 a 130 db, so the bit depth for the scope
//	is 21
int16_t	eladHandler::bitDepth	(void) {
	return 14;
}

//
void	eladHandler::setGainReduction	(void) {
  	static int tempG;
	//fprintf(stderr, "\n--gainReduced=%2d tempG=%2d\n",gainReduced,tempG);
	gainReduced = gainReduced == 1 ? 0 : 1;
	tempG=gainReduced;
	theLoader -> set_en_ext_io_ATT20 (theLoader -> getHandle (),
	                                     &gainReduced);
	usleep(5000000);				     				     	gainReduced=tempG;
	gainLabel -> setText (gainReduced == 1 ? "0" :"-20");
	//fprintf(stderr, "  gainReduced=%2d tempG=%2d\n",gainReduced,tempG);
}

void	eladHandler::setFilter	(void) {
        static int tempF;
	localFilter = localFilter == 1 ? 0: 1;
	tempF=localFilter; 
	//fprintf(stderr,"\n--localFilter=%2d tempF=%2d\n",localFilter,tempF); 
	theLoader -> set_en_ext_io_LP30 (theLoader -> getHandle (),
	                                     &localFilter);
	usleep(500000);					     
        localFilter = tempF;
	
	filterText	-> setText (localFilter == 1 ? "no filter" :"30 Mhz");
	//fprintf(stderr,"  localFilter=%2d tempF=%2d\n",localFilter,tempF);
	
}

void	eladHandler::show_iqSwitch	(bool b) {
	if (b)
	   elad_iqSwitch -> setText ("reversed IQ");
	else
	   elad_iqSwitch -> setText ("normal IQ");
}

void	eladHandler::toggle_IQSwitch	() {
	iqSwitch. store (!iqSwitch. load ());
	show_iqSwitch (iqSwitch. load ());
}

void	eladHandler::set_NyquistWidth	(int w) {
	Nyquist		= w;
}

void	eladHandler::set_Offset		(int w) {
	Offset		= w;
}


