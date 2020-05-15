#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of dab-2
 *
 *    dab-2 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    dab-2 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dab-2; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * 	File reader:
 *	For the (former) files with 8 bit raw data from the
 *	dabsticks 
 */
#include	<cstdio>
#include	<unistd.h>
#include	<cstdlib>
#include	<fcntl.h>
#include	<sys/time.h>
#include	<ctime>
#include	<QString>
#include	"rawfiles.h"
#include	"dab-processor.h"

static inline
int64_t         getMyTime       (void) {
struct timeval  tv;

        gettimeofday (&tv, NULL);
        return ((int64_t)tv. tv_sec * 1000000 + (int64_t)tv. tv_usec);
}

#define	INPUT_FRAMEBUFFERSIZE	8 * 32768
//
//
	rawFiles::rawFiles (QString f, dabProcessor *base) {
	fileName	= f;
	this	-> base	= base;
	myFrame		= new QFrame;
	setupUi	(myFrame);
	myFrame		-> show ();
	_I_Buffer	= new RingBuffer<std::complex<float>>(INPUT_FRAMEBUFFERSIZE);
	filePointer	= fopen (f. toUtf8(). data(), "rb");
	if (filePointer == nullptr) {
	   fprintf (stderr, "file %s cannot open\n",
	                                   f. toUtf8(). data());
	   perror ("file ?");
	   delete myFrame;
	   delete _I_Buffer;
	   throw (31);
	}

	nameofFile	-> setText (f);
	readerOK	= true;
	readerPausing	= true;
	oscillatorTable = new std::complex<float> [2048000];
	for (int i = 0; i < INPUT_RATE; i ++)
	   oscillatorTable [i] = std::complex<float>
	                              (cos (2.0 * M_PI * i / 2048000),
	                               sin (2.0 * M_PI * i / 2048000));
        currentPhase    = 0;
        totalOffset	= 0;

	fileProgress    -> setValue (0);
        currentTime     -> display (0);
}

	rawFiles::~rawFiles() {
	ExitCondition	= true;
	while (isRunning ())
	   usleep (1000);
	fclose (filePointer);
	delete _I_Buffer;
	delete myFrame;	
	delete oscillatorTable;
}

bool	rawFiles::restartReader	(int32_t freq) {
	(void)freq;
	if (readerOK)
	   readerPausing = false;
	start ();
	return readerOK;
}

void	rawFiles::stopReader() {
	if (readerOK)
	   readerPausing = true;
}

static 
float mapTable [] = {
 -128 / 128.0 , -127 / 128.0 , -126 / 128.0 , -125 / 128.0 , -124 / 128.0 , -123 / 128.0 , -122 / 128.0 , -121 / 128.0 , -120 / 128.0 , -119 / 128.0 , -118 / 128.0 , -117 / 128.0 , -116 / 128.0 , -115 / 128.0 , -114 / 128.0 , -113 / 128.0 
, -112 / 128.0 , -111 / 128.0 , -110 / 128.0 , -109 / 128.0 , -108 / 128.0 , -107 / 128.0 , -106 / 128.0 , -105 / 128.0 , -104 / 128.0 , -103 / 128.0 , -102 / 128.0 , -101 / 128.0 , -100 / 128.0 , -99 / 128.0 , -98 / 128.0 , -97 / 128.0 
, -96 / 128.0 , -95 / 128.0 , -94 / 128.0 , -93 / 128.0 , -92 / 128.0 , -91 / 128.0 , -90 / 128.0 , -89 / 128.0 , -88 / 128.0 , -87 / 128.0 , -86 / 128.0 , -85 / 128.0 , -84 / 128.0 , -83 / 128.0 , -82 / 128.0 , -81 / 128.0 
, -80 / 128.0 , -79 / 128.0 , -78 / 128.0 , -77 / 128.0 , -76 / 128.0 , -75 / 128.0 , -74 / 128.0 , -73 / 128.0 , -72 / 128.0 , -71 / 128.0 , -70 / 128.0 , -69 / 128.0 , -68 / 128.0 , -67 / 128.0 , -66 / 128.0 , -65 / 128.0 
, -64 / 128.0 , -63 / 128.0 , -62 / 128.0 , -61 / 128.0 , -60 / 128.0 , -59 / 128.0 , -58 / 128.0 , -57 / 128.0 , -56 / 128.0 , -55 / 128.0 , -54 / 128.0 , -53 / 128.0 , -52 / 128.0 , -51 / 128.0 , -50 / 128.0 , -49 / 128.0 
, -48 / 128.0 , -47 / 128.0 , -46 / 128.0 , -45 / 128.0 , -44 / 128.0 , -43 / 128.0 , -42 / 128.0 , -41 / 128.0 , -40 / 128.0 , -39 / 128.0 , -38 / 128.0 , -37 / 128.0 , -36 / 128.0 , -35 / 128.0 , -34 / 128.0 , -33 / 128.0 
, -32 / 128.0 , -31 / 128.0 , -30 / 128.0 , -29 / 128.0 , -28 / 128.0 , -27 / 128.0 , -26 / 128.0 , -25 / 128.0 , -24 / 128.0 , -23 / 128.0 , -22 / 128.0 , -21 / 128.0 , -20 / 128.0 , -19 / 128.0 , -18 / 128.0 , -17 / 128.0 
, -16 / 128.0 , -15 / 128.0 , -14 / 128.0 , -13 / 128.0 , -12 / 128.0 , -11 / 128.0 , -10 / 128.0 , -9 / 128.0 , -8 / 128.0 , -7 / 128.0 , -6 / 128.0 , -5 / 128.0 , -4 / 128.0 , -3 / 128.0 , -2 / 128.0 , -1 / 128.0 
, 0 / 128.0 , 1 / 128.0 , 2 / 128.0 , 3 / 128.0 , 4 / 128.0 , 5 / 128.0 , 6 / 128.0 , 7 / 128.0 , 8 / 128.0 , 9 / 128.0 , 10 / 128.0 , 11 / 128.0 , 12 / 128.0 , 13 / 128.0 , 14 / 128.0 , 15 / 128.0 
, 16 / 128.0 , 17 / 128.0 , 18 / 128.0 , 19 / 128.0 , 20 / 128.0 , 21 / 128.0 , 22 / 128.0 , 23 / 128.0 , 24 / 128.0 , 25 / 128.0 , 26 / 128.0 , 27 / 128.0 , 28 / 128.0 , 29 / 128.0 , 30 / 128.0 , 31 / 128.0 
, 32 / 128.0 , 33 / 128.0 , 34 / 128.0 , 35 / 128.0 , 36 / 128.0 , 37 / 128.0 , 38 / 128.0 , 39 / 128.0 , 40 / 128.0 , 41 / 128.0 , 42 / 128.0 , 43 / 128.0 , 44 / 128.0 , 45 / 128.0 , 46 / 128.0 , 47 / 128.0 
, 48 / 128.0 , 49 / 128.0 , 50 / 128.0 , 51 / 128.0 , 52 / 128.0 , 53 / 128.0 , 54 / 128.0 , 55 / 128.0 , 56 / 128.0 , 57 / 128.0 , 58 / 128.0 , 59 / 128.0 , 60 / 128.0 , 61 / 128.0 , 62 / 128.0 , 63 / 128.0 
, 64 / 128.0 , 65 / 128.0 , 66 / 128.0 , 67 / 128.0 , 68 / 128.0 , 69 / 128.0 , 70 / 128.0 , 71 / 128.0 , 72 / 128.0 , 73 / 128.0 , 74 / 128.0 , 75 / 128.0 , 76 / 128.0 , 77 / 128.0 , 78 / 128.0 , 79 / 128.0 
, 80 / 128.0 , 81 / 128.0 , 82 / 128.0 , 83 / 128.0 , 84 / 128.0 , 85 / 128.0 , 86 / 128.0 , 87 / 128.0 , 88 / 128.0 , 89 / 128.0 , 90 / 128.0 , 91 / 128.0 , 92 / 128.0 , 93 / 128.0 , 94 / 128.0 , 95 / 128.0 
, 96 / 128.0 , 97 / 128.0 , 98 / 128.0 , 99 / 128.0 , 100 / 128.0 , 101 / 128.0 , 102 / 128.0 , 103 / 128.0 , 104 / 128.0 , 105 / 128.0 , 106 / 128.0 , 107 / 128.0 , 108 / 128.0 , 109 / 128.0 , 110 / 128.0 , 111 / 128.0 
, 112 / 128.0 , 113 / 128.0 , 114 / 128.0 , 115 / 128.0 , 116 / 128.0 , 117 / 128.0 , 118 / 128.0 , 119 / 128.0 , 120 / 128.0 , 121 / 128.0 , 122 / 128.0 , 123 / 128.0 , 124 / 128.0 , 125 / 128.0 , 126 / 128.0 , 127 / 128.0 };

void	rawFiles::run (void) {
int32_t	t, i;
std::complex<float>	*bi;
int32_t	bufferSize	= 32768;
int64_t	period;
int64_t	nextStop;
int64_t	fileLength	= 0;
int	teller		= 0;
int	teller_u	= 0;
uint64_t currentPos	= 0;

	if (!readerOK)
	   return;

	fileProgress	-> setValue (0);
	currentTime	-> display (0);
	fseek (filePointer, 0, SEEK_END);
	fileLength	= ftell (filePointer);
	fseek (filePointer, 0, SEEK_SET);
	totalTime	-> display ((float)fileLength / (2 * 2048000));
	fprintf (stderr, "fileLength = %d\n", (int)fileLength);
	ExitCondition = false;

	period		= (32768 * 1000) / 2048;	// full IQś read
	fprintf (stderr, "Period = %ld\n", period);
	bi		= new std::complex<float> [bufferSize];
	nextStop	= getMyTime ();

	while (!ExitCondition) {
	   if (readerPausing) {
	      usleep (1000);
	      nextStop = getMyTime ();
	      continue;
	   }

	   nextStop += period;
	   t = readBuffer (bi, bufferSize);
	   if (t < bufferSize) {
	      for (i = t; i < bufferSize; i ++)
	          bi [i] = 0;
	      t = bufferSize;
	      currentPos	= 0;
	   }

	   (void)base -> addSymbol (bi, bufferSize);
	   currentPos += 2 * t;

	   if (++teller >= 10) {
	      float progress = (float)currentPos / fileLength;
	      fileProgress	-> setValue ((int) (progress * 100));
	      currentTime	-> display ((float)currentPos /(2 * 2048000));
	      teller = 0;
	   }

	   if (nextStop - getMyTime () > 0)
	      usleep (nextStop - getMyTime ());
	}
	fprintf (stderr, "taak voor replay eindigt hier\n"); fflush (stderr);
}
/*
 *	length is number of uints that we read.
 */
int32_t	rawFiles::readBuffer (std::complex<float> *data, int32_t length) {
int32_t	i, n;
uint8_t	temp [2 * length];

	n = fread (temp, 2, length, filePointer);
	if (n < length) {
	   fseek (filePointer, 0, SEEK_SET);
	   fprintf (stderr, "End of file, restarting\n");
	}

	for (i = 0; i < n; i ++) {
	   data [i] = std::complex<float> (
	                              mapTable [temp [2 * i]],
	                              mapTable [temp [2 * i + 1]]);
	   data [i] *= oscillatorTable [currentPhase];
	   currentPhase -= totalOffset;
	   currentPhase = (currentPhase + 2048000) % 2048000;
	}
	return	n & ~01;
}

void	rawFiles::show		(void) {
	myFrame		-> show ();
}

void	rawFiles::hide		(void) {
	myFrame		-> hide ();
}

bool	rawFiles::isHidden	(void) {
	return !myFrame	-> isVisible ();
}

void	rawFiles::handle_Value	(int offset, float lowVal, float highVal) {
	if (offset != 0) 
	   totalOffset += offset;
	freq_errorDisplay	-> display (totalOffset);
}


