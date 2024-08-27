#
/*
 *    Copyright (C) 2013 .. 2017
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

#include	<sys/time.h>
#include	"new-reader.h"
#include	"newfiles.h"
//#include	"device-exceptions.h"

#define	BUFFERSIZE	32768
static inline
int64_t         getMyTime() {
struct timeval  tv;

	gettimeofday (&tv, nullptr);
	return ((int64_t)tv. tv_sec * 1000000 + (int64_t)tv. tv_usec);
}

	newReader::newReader	(newFiles	*mr,
	                         riffReader	*theReader,
	                         RingBuffer<std::complex<float> > *theBuffer) {
	this	-> parent	= mr;
	this	-> theReader	= theReader;
	this	-> theBuffer	= theBuffer;
	fileLength		= theReader -> elementCount ();
	fprintf (stderr, "fileLength = %lld\n", fileLength);
	theReader	-> reset ();
	period          = (32768 * 1000) / (2048);  // full IQś read
	fprintf (stderr, "Period = %ld\n", period);
	running. store (false);
	start ();
}

	newReader::~newReader	() {
	stopReader();
}

void	newReader::stopReader	() {
	if (running. load ()) {
	   running. store (false);
	   while (isRunning())
	      usleep (200);
	}
}

void	newReader::run	() {
int32_t	bufferSize	= 32768;
int64_t	nextStop;
int	teller		= 0;
std::complex<float> bi [bufferSize];

	connect (this, &newReader::setProgress,
	         parent,  &newFiles::setProgress);
	theReader -> reset ();

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
	         int xx = theReader -> currentPos ();
	         float progress = (float)xx / fileLength;
	         setProgress ((int)(progress * 100), (float)xx / 2048000);
	         teller = 0;
	      }

	      nextStop += period;
	      int n = theReader -> read (bi, bufferSize);
	      if (n < bufferSize) {
	         theReader -> reset ();
	         for (int i = n; i < bufferSize; i ++)
	            bi [i] = std::complex <float> (0, 0);
	      }
	      theBuffer -> putDataIntoBuffer (bi, bufferSize);
	      if (nextStop - getMyTime() > 0)
	         usleep (nextStop - getMyTime());
	   }
	} catch (int e) {}
	fprintf (stderr, "taak voor replay eindigt hier\n"); fflush (stderr);
}

