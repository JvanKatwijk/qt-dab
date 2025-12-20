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

#include	"raw-reader.h"
#include	"rawfiles.h"

#ifdef _WIN32
#else
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

	rawReader::rawReader	(rawFiles *mr,
	                         FILE	*filePointer,
	                         RingBuffer<std::complex<float>> *_I_Buffer) {
	this	-> parent	= mr;
	this	-> filePointer	= filePointer;
	this	-> _I_Buffer	= _I_Buffer;

	fseek (filePointer, 0, SEEK_END);
	fileLength	= ftell (filePointer);
//	fprintf (stderr, "fileLength = %d\n", (int)fileLength);
	fseek (filePointer, 0, SEEK_SET);
	period          = (RAW_BUFFERSIZE * 1000) / (2 * SAMPLERATE / 1000);  // full IQś read
//	fprintf (stderr, "Period = %d\n", (int)period);
	for (int i = 0; i < 256; i ++)
//	the offset 127.38f is due to the input data comes usually from 
//	an  SDR stick which has its DC offset a bit shifted from ideal
//	(from old-dab)
	   mapTable [i] = ((float)i - 127.38) / 128.0;
	running. store (false);
	connect (this, &rawReader::setProgress,
	         parent,   &rawFiles::setProgress);
	newPosition. store (0);
	fseek (filePointer, 0, SEEK_SET);
	start	();
}

		rawReader::~rawReader () {
	stopReader ();
}

void	rawReader::stopReader() {
	if (running) {
	   running = false;
	   while (isRunning ())
	      usleep (200);
	}
}
//
void	rawReader::run () {
uint64_t nextStop;
int	teller	= 0;
std::complex<float> localBuffer [RAW_BUFFERSIZE / 2];

	running. store (true);
	nextStop        = getMyTime();
	try {
	   while (running. load()) {
	      while (_I_Buffer -> WriteSpace () < RAW_BUFFERSIZE + 10) {
	         if (!running. load())
	            throw (32);
	         usleep (100);
	      }

	      if (newPosition. load () > 0) {
	         int newPos =
	             (int)(newPosition. load () / 100.0 * fileLength / 2);
	         int sampleSize = 2;
	         fseek (filePointer, newPos * sampleSize, SEEK_SET);
	         newPosition. store (0);
	         teller = 20;
	      }

	      if (++teller >= 20) {
	         int xx = ftell (filePointer);
	         float progress = (float)xx / fileLength;
	         setProgress ((int)(progress * 100),
	                           (float)xx / (2 * SAMPLERATE / 1000));
	         teller = 0;
	      }

	      nextStop += period;
	      int n = fread (rawDataBuffer, sizeof (uint8_t),
	                                     RAW_BUFFERSIZE, filePointer);
	      if (n < RAW_BUFFERSIZE) {
	         fprintf (stderr, "eof gehad\n");
	         fseek (filePointer, 0, SEEK_SET);
	         for (int i =  n; i < RAW_BUFFERSIZE; i ++)
	            rawDataBuffer [i] = 0;
	      }

	      for (int i = 0; i < RAW_BUFFERSIZE / 2; i ++)
	         localBuffer [i] = std::complex<float> (
	                             4 * mapTable [rawDataBuffer [2 * i]],
	                             4 * mapTable [rawDataBuffer [2 * i + 1]]);
	      _I_Buffer -> putDataIntoBuffer (localBuffer, 
	                                      RAW_BUFFERSIZE / 2);
	      if (nextStop - getMyTime() > 0)
	         usleep (nextStop - getMyTime());
	   }
	} catch (int e) {}
	fprintf (stderr, "taak voor replay eindigt hier\n");
}

void	rawReader::handle_progressSlider (int v) {
	newPosition. store (v);
}

