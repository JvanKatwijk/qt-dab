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
#include	"raw-reader.h"
#include	"rawfiles.h"


#define	BUFFERSIZE	32768
static inline
int64_t         getMyTime() {
struct timeval  tv;

        gettimeofday (&tv, nullptr);
        return ((int64_t)tv. tv_sec * 1000000 + (int64_t)tv. tv_usec);
}

	rawReader::rawReader	(rawFiles *mr,
	                         FILE	*filePointer,
	                         RingBuffer<uint8_t> *theBuffer) {
	this	-> parent	= mr;
	this	-> filePointer	= filePointer;
	this	-> theBuffer	= theBuffer;

	fseek (filePointer, 0, SEEK_END);
	fileLength		= ftell (filePointer);
	fprintf (stderr, "fileLength = %d\n", fileLength);
        fseek (filePointer, 0, SEEK_SET);
	period          = (32768 * 1000) / (2 * 2048);  // full IQś read
        fprintf (stderr, "Period = %ld\n", period);
        bi              = new uint8_t [BUFFERSIZE];
	running. store (false);
	start();
}

		rawReader::~rawReader() {
	stopReader();
	delete[] bi;
}

void	rawReader::stopReader() {
	if (running) {
	   running = false;
	   while (isRunning())
	      usleep (200);
	}
}

void	rawReader::run() {
int64_t	nextStop;
int	i;
int	teller	= 0;

	connect (this, SIGNAL (setProgress (int, float)),
	         parent,   SLOT   (setProgress (int, float)));
	fseek (filePointer, 0, SEEK_SET);
	running. store (true);
	nextStop        = getMyTime();
	try {
	   while (running. load()) {
	      while (theBuffer -> WriteSpace() < BUFFERSIZE + 10) {
	         if (!running. load())
	            throw (32);
	         usleep (100);
              }
	
	      if (++teller >= 20) {
	         int xx = ftell (filePointer);
	         float progress = (float)xx / fileLength;
	         setProgress ((int)(progress * 100), (float)xx / (2 * 2048000));
                 teller = 0;
	      }

	      nextStop += period;
	      int n = fread (bi, sizeof (uint8_t), BUFFERSIZE, filePointer);
	      if (n < BUFFERSIZE) {
	         fprintf (stderr, "eof gehad\n");
	         fseek (filePointer, 0, SEEK_SET);
                 for (i =  n; i < BUFFERSIZE; i ++)
                    bi [i] = 0;
              }

              theBuffer -> putDataIntoBuffer (bi, BUFFERSIZE);
              if (nextStop - getMyTime() > 0)
                 usleep (nextStop - getMyTime());
           }
	} catch (int e) {}
        fprintf (stderr, "taak voor replay eindigt hier\n");
}


