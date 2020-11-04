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
#include	<cstdio>
#include	<unistd.h>
#include	<cstdlib>
#include	<fcntl.h>
#include	<sys/time.h>
#include	<ctime>
#include	<QString>
#include	"wavfiles.h"

#define	__BUFFERSIZE__	32 * 32768

	eladFiles::eladFiles (QString f):
	                           myFrame (nullptr),
	                           _I_Buffer (4 * __BUFFERSIZE__),
	                           _O_Buffer (__BUFFERSIZE__) {
SF_INFO *sf_info;

	fileName	= f;
	setupUi (&myFrame);
	myFrame. show	();

	filePointer	= fopen (f. toUtf8(). data(), "r+b");
	if (filePointer == nullptr) {
	   fprintf (stderr, "file %s no legitimate sound file\n", 
	                                f. toUtf8().data());
	   throw (24);
	}

        for (int i = 0; i < DAB_RATE / 1000; i ++) {
           float inVal  = float (ELAD_RATE / 1000);
           mapTable_int [i] =  int (floor (i * (inVal / (INPUT_RATE / 1000))));
           mapTable_float [i] = i * (inVal / (INPUT_RATE / 1000)) - mapTable_int [i];
        }
        convIndex       = 0;
        convBufferSize  = ELAD_RATE / 1000 + 1;
        fprintf (stderr, "mapTables initialized\n");

        iqSize          = 8;

	nameofFile	-> setText (f);
	fileProgress	-> setValue (0);
	currentTime	-> display (0);
	int64_t fileLength	= fseek (filePointer, 0, SEEK_END);
	totalTime	-> display ((float)fileLength / 2048000);
	running. store (false);
}
//
//	Note that running == true <==> readerTask has value assigned

	eladFiles::~eladFiles() {
	if (running. load()) {
	   readerTask	-> stopReader();
	   while (readerTask -> isRunning())
	      usleep (500);
	   delete readerTask;
	}
	if (filePointer != nullptr)
	   fclose (filePointer);
}

bool	eladFiles::restartReader	(int32_t freq) {
	(void)freq;
	if (running. load())
           return true;
        readerTask      = new wavReader (this, filePointer, &_I_Buffer);
        running. store (true);
        return true;
}

void	eladFiles::stopReader() {
	if (running. load()) {
           readerTask   -> stopReader();
           while (readerTask -> isRunning())
              usleep (100);
	   delete readerTask;
        }
        running. store (false);
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

std::complex<float>	makeSample (uint8_t *buf, bool iqSwitch) {
//std::complex<float>	makeSample_31bits (uint8_t *buf, bool iqSwitch) {
int ii = 0; int qq = 0;
int16_t	i = 0;
uint32_t	uii = 0, uqq = 0;

	uint8_t i0 = buf [i++];
	uint8_t i1 = buf [i++];
	uint8_t i2 = buf [i++];
	uint8_t i3 = buf [i++];

	uint8_t q0 = buf [i++];
	uint8_t q1 = buf [i++];
	uint8_t q2 = buf [i++];
	uint8_t q3 = buf [i++];


	uii = (i3 << 24) | (i2 << 16) | (i1 << 8) | i0;
	uqq = (q3 << 24) | (q2 << 16) | (q1 << 8) | q0;

	ii	= (int) uii;
	qq	= (int) uqq;
	if (iqSwitch)
	   return std::complex<float> ((float)qq * SCALE_FACTOR_32to14,
	                               (float)ii * SCALE_FACTOR_32to14);
	else
	   return std::complex<float> ((float)ii * SCALE_FACTOR_32to14,
	                               (float)qq * SCALE_FACTOR_32to14);
}


//	size is in I/Q pairs
int32_t	eladFiles::getSamples	(std::complex<float> *V, int32_t size) {
int32_t	amount;
	
	if (filePointer == nullptr)
	   return 0;

	if (_O_Buffer. GetRingBufferReadAvailable () >= size)
	   return _O_Buffer. getDataFromBuffer (V, size);

	while (Samples () < size)
	   usleep (500);

	while ((_O_Buffer. GetRingBufferReadAvailable () < size) &&
	       (_I_Buffer. GETRingBufferReadAvailable () > SEGMENT_SIZE)) {

	   _I_Buffer. getDataFromBuffer (lbuffer, SEGMENT_SIZE);
	   for (int i = 0; i < SEGMENT_SIZE / iqSize; i ++) {
	      convBuffer [convIndex] = makeSample (&(lbuffer [iqSize * i]),
	                                                iqSwitch. loasd ());
	      convIndex ++;
	      if (convIndex > ELAD_RATE / 1000 + 1) {
	         float sum = 0;
	         int16_t j;
	         for (j = 0; j < INPUT_RATE / 1000; j ++) {
	            int16_t  inpBase		= mapTable_int [j];
	            float    inpRatio		= mapTable_float [j];
	            temp [j]  = cmul (convBuffer [inpBase + 1], inpRatio) +
                                cmul (convBuffer [inpBase], 1 - inpRatio);
	            sum += abs (temp [j]);
                 }
	         if (++teller > 1000) {
	            fprintf (stderr, "signal is %f dB\n",
	                        10 * log10 (sum / (INPUT_RATE / 1000 / 84)));
	            teller = 0;
	         }
	
	         _O_Buffer. putDataIntoBuffer (temp, INPUT_RATE / 1000);
//      shift the sample at the end to the beginning, it is needed
//      as the starting sample for the next time
                 convBuffer [0] = convBuffer [convBufferSize];
                 convIndex = 1;
              }
	   }
	}
	return _O_Buffer. getDataFromBuffer (V, size);
}

int32_t	eladHandler::Samples	(void) {
int64_t	bufferContent	= _I_Buffer. GetRingBufferReadAvailable ();
	return _O_Buffer. GetRingBufferReadAvailable () +
	       (int)(((int64_t)2048 * bufferContent / (int64_t)3072) / iqSize);
}

void    wavFiles::setProgress (int progress, float timelength) {
}

void	wavFiles::show		() {
	myFrame. show ();
}

void	wavFiles::hide		() {
	myFrame. hide	();
}

bool	wavFiles::isHidden	() {
	return myFrame. isHidden ();
}

