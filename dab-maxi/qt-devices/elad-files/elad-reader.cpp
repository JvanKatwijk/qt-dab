#
/*
 *    Copyright (C) 2013 .. 2017
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

#include	<sys/time.h>
#include	"elad-reader.h"
#include	"eladfiles.h"


#define	BUFFERSIZE	32768
static inline
int64_t         getMyTime() {
struct timeval  tv;

        gettimeofday (&tv, nullptr);
        return ((int64_t)tv. tv_sec * 1000000 + (int64_t)tv. tv_usec);
}

	eladReader::eladReader	(eladFiles	*mr,
	                         FILE		*filePointer,
	                         RingBuffer<uint8_t> *theBuffer) {
	this	-> parent	= mr;
	this	-> filePointer	= filePointer;
	this	-> theBuffer	= theBuffer;
	fileLength		= fseek (filePointer, 0, SEEK_END);
	fprintf (stderr, "fileLength = %d\n", (int)fileLength);
        sf_seek (filePointer, 0, SEEK_SET);
	period          = 1000;  // segments of 1 millisecond
        fprintf (stderr, "Period = %ld\n", period);
	running. store (false);
	start();
}

	eladReader::~eladReader	() {
	stopReader();
}

void	eladReader::stopReader () {
	if (running. load()) {
	   running. store (false);
	   while (isRunning())
	      usleep (200);
	}
}

void	eladReader::run	() {
int32_t	bufferSize	= 3072 * 8;
int64_t	nextStop;
int	teller		= 0;
uint8_t lBuffer [bufferSize];

	fseek (filePointer, 0, SEEK_SET);
	running. store (true);

	nextStop	= getMyTime ();
	try {
	   while (running. load()) {
	      while (theBuffer -> WriteSpace() < bufferSize) {
	         if (!running. load())
	            throw (33);
	         usleep (100);
	      }

	      nextStop += period;
	      int n = fread (lbuffer,8, 3072, filePointer);,
	      if (n < bufferSize) {
	         sf_seek (filePointer, 0, SEEK_SET);
	         for (int i = n; i < bufferSize; i ++)
	            bi [i] = std::complex <float> (0, 0);
	      }
	      theBuffer -> putDataIntoBuffer (lbuffer, bufferSize);
	      if (nextStop - getMyTime() > 0)
	         usleep (nextStop - getMyTime());
	   }
	} catch (int e) {}
	fprintf (stderr, "taak voor replay eindigt hier\n"); fflush (stderr);
}

