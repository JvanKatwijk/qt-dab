#
/*
 *    Copyright (C) 2013 .. 2017
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

#include	<sys/time.h>
#include	"wav-reader.h"
#include	"wavfiles.h"
#include	"device-exceptions.h"

#define	BUFFERSIZE	32768
static inline
int64_t         getMyTime() {
struct timeval  tv;

	gettimeofday (&tv, nullptr);
	return ((int64_t)tv. tv_sec * 1000000 + (int64_t)tv. tv_usec);
}

	wavReader::wavReader	(wavFiles	*mr,
	                         SNDFILE	*filePointer,
	                         int32_t	samplerate,
	                         RingBuffer<std::complex<float> > *theBuffer) {
	this	-> parent	= mr;
	this	-> filePointer	= filePointer;
	this	-> sampleRate	= samplerate;
	this	-> theBuffer	= theBuffer;
	fileLength		= sf_seek (filePointer, 0, SEEK_END);
	fprintf (stderr, "fileLength = %d\n", (int)fileLength);
	sf_seek (filePointer, 0, SEEK_SET);
	period          = (32768 * 1000) / (2048);  // full IQś read
	fprintf (stderr, "Period = %ld\n", period);
	if (sampleRate != INPUT_RATE) {
//  	set up for interpolator
	   convBuffer. resize (sampleRate / 1000 + 1);
           float denominator	= float (INPUT_RATE) / DIVIDER;
           float inVal		= float (sampleRate) / DIVIDER;
           for (int i = 0; i < INPUT_RATE / DIVIDER; i ++) {
              mapTable_int [i]     = int (floor (i * (inVal / denominator)));
              mapTable_float [i] =
                             i * (inVal / denominator) - mapTable_int [i];
           } 
	}
        convIndex       = 0;
	running. store (false);
	start();
}

	wavReader::~wavReader() {
	stopReader();
}

void	wavReader::stopReader() {
	if (running. load()) {
	   running. store (false);
	   while (isRunning())
	      usleep (200);
	}
}

void	wavReader::run	() {
int32_t	bufferSize	= 32768;
int64_t	nextStop;
int	teller		= 0;
std::complex<float> bi [bufferSize];

	connect (this, &wavReader::setProgress,
	         parent,  &wavFiles::setProgress);
	sf_seek (filePointer, 0, SEEK_SET);

	running. store (true);

	nextStop	= getMyTime();
	try {
	   while (running. load()) {
	      while (theBuffer -> WriteSpace () < bufferSize) {
	         if (!running. load())
	            throw (33);
	         usleep (100);
	      }

	      if (++teller >= 20) {
	         int xx = sf_seek (filePointer, 0, SEEK_CUR);
	         float progress = (float)xx / fileLength;
	         setProgress ((int)(progress * 100), (float)xx / 2048000);
	         teller = 0;
	      }

	      nextStop += period;
	      int n = sf_readf_float (filePointer,
		                             (float *)bi, bufferSize);
	      if (n < bufferSize) {
	         sf_seek (filePointer, 0, SEEK_SET);
	         for (int i = n; i < bufferSize; i ++)
	            bi [i] = std::complex <float> (0, 0);
	      }
	      if (sampleRate == INPUT_RATE)
	         theBuffer -> putDataIntoBuffer (bi, bufferSize);
	      else {
	         std::complex<float> localBuf [SAMPLERATE / DIVIDER];
	         for (int i = 0; i < bufferSize; i ++) {
	            convBuffer [convIndex ++] = bi [i];
	            if (convIndex > (sampleRate / DIVIDER + 1)) {
	              for (int j = 0; j < INPUT_RATE / DIVIDER; j ++) {
	                  int16_t inpBase     = mapTable_int [j];
	                  float   inpRatio    = mapTable_float [j];
	                   localBuf [j]        = 
                                     convBuffer [inpBase + 1] * inpRatio +
                                     convBuffer [inpBase] * (1 - inpRatio);
	               
	               }
	               _I_Buffer. putDataIntoBuffer (localBuf,
                                                  INPUT_RATE / DIVIDER);
	               convBuffer [0] = convBuffer [CONV_SIZE];
	               convIndex = 1;
	            }
	         }
	      }
	      if (nextStop - getMyTime() > 0)
	         usleep (nextStop - getMyTime());
	   }
	} catch (int e) {}
	fprintf (stderr, "taak voor replay eindigt hier\n"); fflush (stderr);
}

