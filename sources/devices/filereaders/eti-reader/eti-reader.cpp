#
/*
 *    Copyright (C) 2013 .. 2024
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

#include	"eti-reader.h"
#include	"eti-files.h"

#ifndef _WIN32
#include	<unistd.h>
#include	<sys/time.h>
#endif

#include	"device-handler.h"

static inline
int64_t         getMyTime() {
struct timeval  tv;
#ifdef	__MINGW32__
	mingw_gettimeofday (&tv, nullptr);
#else
	gettimeofday (&tv, nullptr);
#endif
	return ((int64_t)tv. tv_sec * 1000000 + (int64_t)tv. tv_usec);
}

	etiReader::etiReader	(etiFiles *mr,
	                         FILE	*filePointer,
	                         RingBuffer<uint8_t> *_I_Buffer) {
	this	-> parent	= mr;
	this	-> filePointer	= filePointer;
	this	-> _I_Buffer	= _I_Buffer;

	fseek (filePointer, 0, SEEK_END);
	fileLength	= ftell (filePointer);
	fseek (filePointer, 0, SEEK_SET);
	period          = 24000;	// 24 milliseconds

	running. store (false);
	connect (this, &etiReader::setProgress,
	         parent,   &etiFiles::setProgress);
	newPosition. store (0);
	fseek (filePointer, 0, SEEK_SET);
	start	();
}

		etiReader::~etiReader () {
	stopReader ();
}

void	etiReader::stopReader() {
	if (running) {
	   running = false;
	   while (isRunning ())
	      usleep (200);
	}
}

#define	FRAME_SIZE	6144
void	etiReader::run () {
uint64_t nextStop;
int	teller	= 0;

	running. store (true);
	nextStop        = getMyTime ();
	fprintf (stderr, "Reader gestart\n");
	try {
	   while (running. load ()) {
	      while (_I_Buffer -> WriteSpace () < FRAME_SIZE) {
	         if (!running. load())
	            throw (32);
	         usleep (10);
	      }

	      if (++teller >= 20) {
	         int xx = ftell (filePointer);
	         float progress = (float)xx / fileLength;
	         setProgress ((int)(progress * 100),
	                           (float)xx / (fileLength / FRAME_SIZE * 0.024));
	         teller = 0;
	      }

	      int n = fread (rawDataBuffer, sizeof (uint8_t),
	                                     FRAME_SIZE, filePointer);
	      if (n < FRAME_SIZE) {
	         fprintf (stderr, "eof gehad\n");
	         fseek (filePointer, 0, SEEK_SET);
	         for (int i =  n; i < RAW_BUFFERSIZE; i ++)
	            rawDataBuffer [i] = 0;
	      }

	      _I_Buffer -> putDataIntoBuffer (rawDataBuffer, FRAME_SIZE);
	      nextStop	+= period;
	      if ((int)(nextStop - getMyTime ()) > 0)
	         usleep (nextStop - getMyTime());
	   }
	} catch (int e) {}
	fprintf (stderr, "taak voor replay eindigt hier\n");
}

void	etiReader::handle_progressSlider (int v) {
	newPosition. store (v);
}

