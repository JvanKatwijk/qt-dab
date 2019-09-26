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


#define	BUFFERSIZE	32768
static inline
int64_t         getMyTime() {
struct timeval  tv;

        gettimeofday (&tv, nullptr);
        return ((int64_t)tv. tv_sec * 1000000 + (int64_t)tv. tv_usec);
}

	wavReader::wavReader	(wavFiles	*mr,
	                         SNDFILE	*filePointer,
	                         RingBuffer<std::complex<float> > *theBuffer) {
	this	-> parent	= mr;
	this	-> filePointer	= filePointer;
	this	-> theBuffer	= theBuffer;
	fileLength		= sf_seek (filePointer, 0, SEEK_END);
	fprintf (stderr, "fileLength = %d\n", fileLength);
        sf_seek (filePointer, 0, SEEK_SET);
	period          = (32768 * 1000) / (2048);  // full IQś read
        fprintf (stderr, "Period = %ld\n", period);
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

void	wavReader::run() {
int32_t	t, i;
std::complex<float>	*bi;
int32_t	bufferSize	= 32768;
int64_t	nextStop;
int	teller		= 0;

	connect (this, SIGNAL (setProgress (int, float)),
                 parent,   SLOT (setProgress (int, float)));
	sf_seek (filePointer, 0, SEEK_SET);

	running. store (true);

	bi		= new std::complex<float> [bufferSize];
	nextStop	= getMyTime();
	try {
	   while (running. load()) {
	      while (theBuffer -> WriteSpace() < bufferSize) {
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
	         for (i = n; i < bufferSize; i ++)
	            bi [i] = std::complex <float> (0, 0);
	      }
	      theBuffer -> putDataIntoBuffer (bi, bufferSize);
	      if (nextStop - getMyTime() > 0)
	         usleep (nextStop - getMyTime());
	   }
	} catch (int e) {}
	fprintf (stderr, "taak voor replay eindigt hier\n"); fflush (stderr);
	delete[] bi;
}

