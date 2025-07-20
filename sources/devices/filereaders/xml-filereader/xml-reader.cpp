#
/*
 *    Copyright (C) 2014 .. 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB
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
#include	"xml-reader.h"
#include	"xml-descriptor.h"
#include	"xml-filereader.h"
#include	"dab-constants.h"
#include	<sys/time.h>
#include	<stdio.h>

#include	"device-handler.h"

static
int	shift (int a) {
int r	= 1;
	while (-- a > 0) {
	   r <<= 1;
	}
	return r;
}

static inline
std::complex<float> compmul (std::complex<float> a, float b) {
	return std::complex<float> (real (a) * b, imag (a) * b);
}

static inline
uint64_t	currentTime () {
struct timeval tv;

	gettimeofday (&tv, nullptr);
	return (uint64_t)(tv. tv_sec * 1000000 +
	                  (uint64_t)tv. tv_usec);
}

	xml_Reader::xml_Reader (xml_fileReader *mr,
	                        FILE	*f,
	                        xmlDescriptor *fd,
	                        uint64_t	filePointer,
	                        RingBuffer<std::complex<float>> *b) {
	this	-> parent	= mr;
	this	-> file		= f;
	this	-> fd		= fd;
	this	-> filePointer	= filePointer;
	sampleBuffer		= b;
//
//	convBufferSize is a little confusing since the actual 
//	buffer is one larger
	convBufferSize		= fd -> sampleRate / 1000;
	continuous. store (true);
	float samplesPerMsec	= SAMPLERATE / 1000.0;
	for (int i = 0; i < SAMPLERATE / 1000; i ++) {
	   float inVal = float (fd -> sampleRate / 1000);
	   mapTable_int [i] = (int)(floor (i * (inVal / samplesPerMsec )));
	   mapTable_float [i] = i * (inVal / samplesPerMsec) - mapTable_int [i];
	}

	convIndex	= 0;
	convBuffer. resize (convBufferSize + 1);
	nrElements	= fd -> blockList [0]. nrElements;

	connect (this, &xml_Reader::setProgress,
	         parent, &xml_fileReader::setProgress);

	fprintf (stderr, "reader task wordt gestart\n");
	start ();
}

	xml_Reader::~xml_Reader () {
	running. store (false);
	while (this -> isRunning ())
	   usleep (1000);
}

void	xml_Reader::stopReader	() {
	if (!running. load ())
	   return;
	running. store (false);
	while (isRunning ())
	   usleep (1000);
}

static	int cycleCount = 0;
void	xml_Reader::run () {
uint64_t	samplesRead	= 0;
uint64_t	nextStop;
int	startPoint	= filePointer;

	running. store (true);
	fseek (file, filePointer, SEEK_SET);
	nextStop = currentTime ();
	for (int blocks = 0; blocks < fd -> nrBlocks; blocks ++) {
	   samplesToRead	= compute_nrSamples (file, blocks);
	   fprintf (stderr, "samples to read %ld\n", samplesToRead);
	   samplesRead		= 0;
	   do {
	      while ((samplesRead <= samplesToRead) && running. load ()) {

	         if (fd -> iqOrder == "IQ") 
	            samplesRead += readSamples (file,
	                                        &xml_Reader::readElements_IQ);
	         else
	          if (fd -> iqOrder == "QI")
	            samplesRead += readSamples (file, 
	                                        &xml_Reader::readElements_QI);
	         else
	         if (fd -> iqOrder == "I_Only")
	            samplesRead += readSamples (file,
	                                        &xml_Reader::readElements_I);
	         else
	            samplesRead += readSamples (file,
	                                        &xml_Reader::readElements_Q);

	         if (++cycleCount >= 200) {
	            setProgress (samplesRead, samplesToRead);
	            cycleCount = 0;
	         }
//
//	the readSamples function returns 1 msec of data,
//	we assume taking this data does not take time
	         nextStop = nextStop + (uint64_t)1000;
	         if (nextStop > currentTime ())
	            usleep ( nextStop - currentTime ());
	      }
	      setProgress (0, samplesToRead);
	      filePointer = startPoint;
	      fseek (file, filePointer, SEEK_SET);
	      samplesRead		= 0;
	   } while (running. load () && continuous. load ());
	}
}

bool	xml_Reader::handle_continuousButton  () {
	continuous. store (!continuous. load ());
	return continuous. load ();
}

uint64_t	xml_Reader::compute_nrSamples (FILE *f, int blockNumber) {
uint64_t	nrElements	= fd -> blockList. at (blockNumber). nrElements;
uint64_t	samplesToRead	= 0;

	(void)f;
	if (fd -> blockList. at (blockNumber). typeofUnit == "Channel") {
	   if ((fd -> iqOrder == "IQ") ||
	       (fd -> iqOrder == "QI"))
	      samplesToRead = nrElements / 2;
	   else
	      samplesToRead = nrElements;
	}
	else	// typeofUnit = "sample"
	   samplesToRead = nrElements;

	fprintf (stderr, "%ld samples have to be read, order is %s\n",
	                 samplesToRead, fd -> iqOrder. toLatin1 (). data ());
	return samplesToRead;
}

uint64_t	xml_Reader::readSamples (FILE *theFile, 
	                         void(xml_Reader::*r)(FILE *theFile,
	                                    std::complex<float> *, int)) {
std::complex<float> temp [SAMPLERATE / 1000];

	(*this.*r) (theFile, &convBuffer [1], convBufferSize);
	for (int i = 0; i < SAMPLERATE / 1000; i ++) {
	   int16_t inpBase	= mapTable_int [i];
	   float   inpRatio	= mapTable_float [i];
	   temp [i] = compmul (convBuffer [inpBase + 1], inpRatio) +
	                       compmul (convBuffer [inpBase], 1 - inpRatio);
	}
	convBuffer [0] = convBuffer [convBufferSize];
	convIndex = 1;
	sampleBuffer -> putDataIntoBuffer (temp, SAMPLERATE / 1000);
	return SAMPLERATE / 1000;
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

//
//	the readers
void	xml_Reader::readElements_IQ (FILE *theFile, 
	                             std::complex<float> *buffer,
	                             int amount) {

int	nrBits	= fd -> bitsperChannel;
float	scaler	= float (shift (nrBits));

	if (fd -> container == "int8") {
	   auto *lbuf = dynVec (uint8_t, 2 * amount);
	   size_t objectsRead = fread (lbuf, 1, 2 * amount, theFile);
	   for (size_t i = 0; i < objectsRead / 2; i ++)
	      buffer [i] = std::complex<float> (((int8_t)lbuf [2 * i]) / 127.0,
	                                        ((int8_t)lbuf [2 * i + 1]) / 127.0);
	   return;
	}
	
	if (fd -> container == "uint8") {
	   auto *lbuf = dynVec (uint8_t, 2 * amount);
	   size_t objectsRead = fread (lbuf, 1, 2 * amount, theFile);
	   for (size_t i = 0; i < objectsRead / 2; i ++)
	      buffer [i] = std::complex<float> (mapTable [lbuf [2 * i]],
	                                        mapTable [lbuf [2 * i + 1]]);
	   return;
	}

	if (fd -> container == "int16") {
	   auto *lbuf = dynVec (uint8_t, 4 * amount);
	   size_t objectsRead = fread (lbuf, 2, 2 * amount, theFile);
	   if (fd -> byteOrder == "MSB") {
	      for (size_t i = 0; i < objectsRead / 2; i ++) {
	         int16_t temp_16_1 = (lbuf [4 * i] << 8) | lbuf [4 * i + 1];
	         int16_t temp_16_2 = (lbuf [4 * i + 2] << 8) | lbuf [4 * i + 3];
	         buffer [i] = std::complex<float> ((float)temp_16_1 / scaler,
	                                           (float)temp_16_2 / scaler);
	      }
	   }
	   else {
	      for (size_t i = 0; i < objectsRead / 2; i ++) {
	         int16_t temp_16_1 = (lbuf [4 * i + 1] << 8) | lbuf [4 * i];
	         int16_t temp_16_2 = (lbuf [4 * i + 3] << 8) | lbuf [4 * i + 2];
	         buffer [i] = std::complex<float> ((float)temp_16_1 / scaler,
	                                           (float)temp_16_2 / scaler);
	      }
	   }
	   return;
	}

	if (fd -> container == "int24") {
	   auto *lbuf = dynVec (uint8_t, 6 * amount);
           size_t objectsRead = fread (lbuf, 3, 2 * amount, theFile);
           if (fd -> byteOrder == "MSB") {
              for (size_t i = 0; i < objectsRead / 2; i ++) {
                 int32_t temp_32_1 = (lbuf [6 * i] << 16) |
	                             (lbuf [6 * i + 1] << 8) | lbuf [6 * i + 2];
                 int32_t temp_32_2 = (lbuf [6 * i + 3] << 16) |
	                             (lbuf [4 * i + 4] << 8) | lbuf [6 * i + 5];
	   	 if (temp_32_1 & 0x800000) 
	            temp_32_1 |= 0xFF000000;
	   	 if (temp_32_2 & 0x800000) 
	            temp_32_2 |= 0xFF000000;
	         buffer [i] = std::complex<float> ((float)temp_32_1 / scaler,
	                                           (float)temp_32_2 / scaler);
	      }
	   }
	   else {
              for (size_t i = 0; i < objectsRead / 2; i ++) {
                 int32_t temp_32_1 = (lbuf [6 * i + 2] << 16) |
	                             (lbuf [6 * i + 1] << 8) | lbuf [6 * i];
                 int32_t temp_32_2 = (lbuf [6 * i + 5] << 16) |
	                             (lbuf [6 * i + 4] << 8) | lbuf [6 * i + 3];
	   	 if (temp_32_1 & 0x800000) 
	            temp_32_1 |= 0xFF000000;
	   	 if (temp_32_2 & 0x800000) 
	            temp_32_2 |= 0xFF000000;
	         buffer [i] = std::complex<float> ((float)temp_32_1 / scaler,
	                                           (float)temp_32_2 / scaler);
	      }
	   }
	   return;
	}

	if (fd -> container == "int32") {
	   auto *lbuf = dynVec (uint8_t, 8 * amount);
           size_t objectsRead = fread (lbuf, 4, 2 * amount, theFile);
           if (fd -> byteOrder == "MSB") {
              for (size_t i = 0; i < objectsRead / 2; i ++) {
                 int32_t temp_32_1 = (lbuf [8 * i] << 24) |
	                             (lbuf [8 * i + 1] << 16) |
	                             (lbuf [8 * i + 2] << 8) | lbuf [8 * i + 3];
                 int32_t temp_32_2 = (lbuf [8 * i + 4] << 24) |
	                             (lbuf [8 * i + 5] << 16) |
	                             (lbuf [8 * i + 6] << 8) | lbuf [8 * i + 7];
	         buffer [i] = std::complex<float> ((float)temp_32_1 / scaler,
	                                           (float)temp_32_2 / scaler);
	      }
	   }
	   else {
              for (size_t i = 0; i < objectsRead / 2; i ++) {
                 int32_t temp_32_1 = (lbuf [8 * i + 3] << 24) |
	                             (lbuf [8 * i + 2] << 16) |
	                             (lbuf [8 * i + 1] << 8) | lbuf [8 * i];
                 int32_t temp_32_2 = (lbuf [8 * i + 7] << 24) |
	                             (lbuf [8 * i + 6] << 16) |
	                             (lbuf [8 * i + 5] << 8) | lbuf [8 * i + 4];
	         buffer [i] = std::complex<float> ((float)temp_32_1 / scaler,
	                                           (float)temp_32_2 / scaler);
	      }
	   }
	   return;
	}

	if (fd -> container == "float32") {
	   auto *lbuf = dynVec (uint8_t, 8 * amount);
           size_t objectsRead = fread (lbuf, 4, 2 * amount, theFile);
           if (fd -> byteOrder == "MSB") {
              for (size_t i = 0; i < objectsRead / 2; i ++) {
                 int32_t temp_32_1 = (lbuf [8 * i] << 24) |
	                             (lbuf [8 * i + 1] << 16) |
	                             (lbuf [8 * i + 2] << 8) | lbuf [8 * i + 3];
	         float t1	=*(float *)(&temp_32_1);
                 int32_t temp_32_2 = (lbuf [8 * i + 4] << 24) |
	                             (lbuf [8 * i + 5] << 16) |
	                             (lbuf [8 * i + 6] << 8) | lbuf [8 * i + 7];
	         float t2	=*(float *)(&temp_32_2);
	         buffer [i] = std::complex<float> (t1, t2);
	      }
	   }
	   else {
              for (size_t i = 0; i < objectsRead / 2; i ++) {
                 int32_t temp_32_1 = (lbuf [8 * i + 3] << 24) |
	                             (lbuf [8 * i + 2] << 16) |
	                             (lbuf [8 * i + 1] << 8) | lbuf [8 * i];
	         float t1	=*(float *)(&temp_32_1);
                 int32_t temp_32_2 = (lbuf [8 * i + 7] << 24) |
	                             (lbuf [8 * i + 6] << 16) |
	                             (lbuf [8 * i + 5] << 8) | lbuf [8 * i + 4];
	         float t2	=*(float *)(&temp_32_2);
	         buffer [i] = std::complex<float> (t1, t2);
	      }
	   }
	   return;
	}
}

void	xml_Reader::readElements_QI (FILE *theFile, 
	                             std::complex<float> *buffer,
	                             int amount) {

int	nrBits	= fd -> bitsperChannel;
float	scaler	= float (shift (nrBits));

	if (fd -> container == "int8") {
	   auto *lbuf = dynVec (uint8_t, 2 * amount);
	   size_t objectsRead = fread (lbuf, 1, 2 * amount, theFile);
	   for (size_t i = 0; i < objectsRead / 2; i ++)
	      buffer [i] = std::complex<float> (((int8_t)lbuf [2 * i + 1]) / 127.0,
	                                        ((int8_t)lbuf [2 * i]) / 127.0);
	   return;
	}
	
	if (fd -> container == "uint8") {
	   auto *lbuf = dynVec (uint8_t, 2 * amount);
	   size_t objectsRead = fread (lbuf, 1, 2 * amount, theFile);
	   for (size_t i = 0; i < objectsRead / 2; i ++)
	      buffer [i] = std::complex<float> (mapTable [2 * i + 1],
	                                        mapTable [2 * i]);
	   return;
	}

	if (fd -> container == "int16") {
	   auto *lbuf = dynVec (uint8_t, 4 * amount);
	   size_t objectsRead = fread (lbuf, 2, 2 * amount, theFile);
	   if (fd -> byteOrder == "MSB") {
	      for (size_t i = 0; i < objectsRead / 2; i ++) {
	         int16_t temp_16_1 = (lbuf [4 * i] << 8) | lbuf [4 * i + 1];
	         int16_t temp_16_2 = (lbuf [4 * i + 2] << 8) | lbuf [4 * i + 3];
	         buffer [i] = std::complex<float> ((float)temp_16_2 / scaler,
	                                           (float)temp_16_1 / scaler);
	      }
	   }
	   else {
	      for (size_t i = 0; i < objectsRead / 2; i ++) {
	         int16_t temp_16_1 = (lbuf [4 * i + 1] << 8) | lbuf [4 * i];
	         int16_t temp_16_2 = (lbuf [4 * i + 3] << 8) | lbuf [4 * i + 2];
	         buffer [i] = std::complex<float> ((float)temp_16_2 / scaler,
	                                           (float)temp_16_1 / scaler);
	      }
	   }
	   return;
	}

	if (fd -> container == "int24") {
	   auto *lbuf = dynVec (uint8_t, 6 * amount);
           size_t objectsRead = fread (lbuf, 3, 2 * amount, theFile);
           if (fd -> byteOrder == "MSB") {
              for (size_t i = 0; i < objectsRead / 2; i ++) {
                 int32_t temp_32_1 = (lbuf [6 * i] << 16) |
	                             (lbuf [6 * i + 1] << 8) | lbuf [6 * i + 2];
                 int32_t temp_32_2 = (lbuf [6 * i + 3] << 16) |
	                             (lbuf [4 * i + 4] << 8) | lbuf [6 * i + 5];
	   	 if (temp_32_1 & 0x800000) 
	            temp_32_1 |= 0xFF000000;
	   	 if (temp_32_2 & 0x800000) 
	            temp_32_2 |= 0xFF000000;
	         buffer [i] = std::complex<float> ((float)temp_32_2 / scaler,
	                                           (float)temp_32_1 / scaler);
	      }
	   }
	   else {
              for (size_t i = 0; i < objectsRead / 2; i ++) {
                 int32_t temp_32_1 = (lbuf [6 * i + 2] << 16) |
	                             (lbuf [6 * i + 1] << 8) | lbuf [6 * i];
                 int32_t temp_32_2 = (lbuf [6 * i + 5] << 16) |
	                             (lbuf [6 * i + 4] << 8) | lbuf [6 * i + 3];
	   	 if (temp_32_1 & 0x800000) 
	            temp_32_1 |= 0xFF000000;
	   	 if (temp_32_2 & 0x800000) 
	            temp_32_2 |= 0xFF000000;
	         buffer [i] = std::complex<float> ((float)temp_32_2 / scaler,
	                                           (float)temp_32_1 / scaler);
	      }
	   }
	   return;
	}

	if (fd -> container == "int32") {
	   auto *lbuf = dynVec (uint8_t, 8 * amount);
           size_t objectsRead = fread (lbuf, 4, 2 * amount, theFile);
           if (fd -> byteOrder == "MSB") {
              for (size_t i = 0; i < objectsRead / 2; i ++) {
                 int32_t temp_32_1 = (lbuf [8 * i] << 24) |
	                             (lbuf [8 * i + 1] << 16) |
	                             (lbuf [8 * i + 2] << 8) | lbuf [8 * i + 3];
                 int32_t temp_32_2 = (lbuf [8 * i + 4] << 24) |
	                             (lbuf [8 * i + 5] << 16) |
	                             (lbuf [8 * i + 6] << 8) | lbuf [8 * i + 7];
	         buffer [i] = std::complex<float> ((float)temp_32_2 / scaler,
	                                           (float)temp_32_1 / scaler);
	      }
	   }
	   else {
              for (size_t i = 0; i < objectsRead / 2; i ++) {
                 int32_t temp_32_1 = (lbuf [8 * i + 3] << 24) |
	                             (lbuf [8 * i + 2] << 16) |
	                             (lbuf [8 * i + 1] << 8) | lbuf [8 * i];
                 int32_t temp_32_2 = (lbuf [8 * i + 7] << 24) |
	                             (lbuf [8 * i + 6] << 16) |
	                             (lbuf [8 * i + 5] << 8) | lbuf [8 * i + 4];
	         buffer [i] = std::complex<float> ((float)temp_32_2 / scaler,
	                                           (float)temp_32_1 / scaler);
	      }
	   }
	   return;
	}

	if (fd -> container == "float32") {
	   auto *lbuf = dynVec (uint8_t, 6 * amount);
           size_t objectsRead = fread (lbuf, 4, 2 * amount, theFile);
           if (fd -> byteOrder == "MSB") {
              for (size_t i = 0; i < objectsRead / 2; i ++) {
                 int32_t temp_32_1 = (lbuf [8 * i] << 24) |
	                             (lbuf [8 * i + 1] << 16) |
	                             (lbuf [8 * i + 2] << 8) | lbuf [8 * i + 3];
	         float t1	=*(float *)(&temp_32_1);
                 int32_t temp_32_2 = (lbuf [8 * i + 4] << 24) |
	                             (lbuf [8 * i + 5] << 16) |
	                             (lbuf [8 * i + 6] << 8) | lbuf [8 * i + 7];
	         float t2	=*(float *)(&temp_32_2);
	         buffer [i] = std::complex<float> (t1, t2);
	      }
	   }
	   else {
              for (size_t i = 0; i < objectsRead / 2; i ++) {
                 int32_t temp_32_1 = (lbuf [8 * i + 3] << 24) |
	                             (lbuf [8 * i + 2] << 16) |
	                             (lbuf [8 * i + 1] << 8) | lbuf [8 * i];
	         float t1	=*(float *)(&temp_32_1);
                 int32_t temp_32_2 = (lbuf [8 * i + 7] << 24) |
	                             (lbuf [8 * i + 6] << 16) |
	                             (lbuf [8 * i + 5] << 8) | lbuf [8 * i + 4];
	         float t2	=*(float *)(&temp_32_2);
	         buffer [i] = std::complex<float> (t1, t2);
	      }
	   }
	   return;
	}
}

void	xml_Reader::readElements_I (FILE *theFile, 
	                            std::complex<float> *buffer,
	                            int amount) {

int	nrBits	= fd -> bitsperChannel;
float	scaler	= float (shift (nrBits));

	if (fd -> container == "int8") {
	   auto *lbuf = dynVec (uint8_t, 1 * amount);
	   size_t samplesRead = fread (lbuf, 1, amount, theFile);
	   for (size_t i = 0; i < samplesRead; i ++)
	      buffer [i] =
	           std::complex<float> ((int8_t)lbuf [i] / 127.0, 0);
	   return;
	}
	
	if (fd -> container == "uint8") {
	   auto *lbuf = dynVec (uint8_t, 1 * amount);
	   size_t samplesRead = fread (lbuf, 1, amount, theFile);
	   for (size_t i = 0; i < samplesRead; i ++)
	      buffer [i] = std::complex<float> (mapTable [lbuf [i]], 0);
	   return;
	}

	if (fd -> container == "int16") {
	   auto *lbuf = dynVec (uint8_t, 2 * amount);
	   size_t samplesRead = fread (lbuf, 2, amount, theFile);
	   if (fd -> byteOrder == "MSB") {
	      for (size_t i = 0; i < samplesRead; i ++) {
	         int16_t temp_16_1 = (lbuf [2 * i] << 8) | lbuf [2 * i + 1];
	         buffer [i] =
	               std::complex<float> ((float)temp_16_1 / scaler, 0);
	      }
	   }
	   else {
	      for (size_t i = 0; i < samplesRead; i ++) {
	         int16_t temp_16_1 = (lbuf [2 * i + 1] << 8) | lbuf [2 * i];
	         buffer [i] =
	              std::complex<float> ((float)temp_16_1 / scaler, 0);
	      }
	   }
	   return;
	}

	if (fd -> container == "int24") {
	   auto *lbuf = dynVec (uint8_t, 3 * amount);
           size_t samplesRead = fread (lbuf, 3, amount, theFile);
           if (fd -> byteOrder == "MSB") {
              for (size_t i = 0; i < samplesRead; i ++) {
                 int32_t temp_32_1 = (lbuf [3 * i] << 16) |
	                             (lbuf [3 * i + 1] << 8) | lbuf [3 * i + 2];
	   	 if (temp_32_1 & 0x800000) 
	            temp_32_1 |= 0xFF000000;
	         buffer [i] =
	              std::complex<float> ((float)temp_32_1 / scaler, 0);
	      }
	   }
	   else {
              for (size_t i = 0; i < samplesRead; i ++) {
                 int32_t temp_32_1 = (lbuf [3 * i + 2] << 16) |
	                             (lbuf [3 * i + 1] << 8) | lbuf [3 * i];
	   	 if (temp_32_1 & 0x800000) 
	            temp_32_1 |= 0xFF000000;
	         buffer [i] =
	              std::complex<float> ((float)temp_32_1 / scaler, 0);
	      }
	   }
	   return;
	}

	if (fd -> container == "int32") {
	   auto *lbuf = dynVec (uint8_t, 4 * amount);
           size_t samplesRead = fread (lbuf, 4, amount, theFile);
           if (fd -> byteOrder == "MSB") {
              for (size_t i = 0; i < samplesRead; i ++) {
                 int32_t temp_32_1 = (lbuf [4 * i] << 24) |
	                             (lbuf [4 * i + 1] << 16) |
	                             (lbuf [4 * i + 2] << 8) | lbuf [4 * i + 3];
	         buffer [i] =
	              std::complex<float> ((float)temp_32_1 / scaler, 0);
	      }
	   }
	   else {
              for (size_t i = 0; i < samplesRead; i ++) {
                 int32_t temp_32_1 = (lbuf [4 * i + 3] << 24) |
	                             (lbuf [4 * i + 2] << 16) |
	                             (lbuf [4 * i + 1] << 8) | lbuf [4 * i];
	         buffer [i] =
	               std::complex<float> ((float)temp_32_1 / scaler, 0);
	      }
	   }
	   return;
	}

	if (fd -> container == "float32") {
	   auto *lbuf = dynVec (uint8_t, 4 * amount);
           size_t samplesRead = fread (lbuf, 4, amount, theFile);
           if (fd -> byteOrder == "MSB") {
              for (size_t i = 0; i < samplesRead; i ++) {
                 int32_t temp_32_1 = (lbuf [4 * i] << 24) |
	                             (lbuf [4 * i + 1] << 16) |
	                             (lbuf [4 * i + 2] << 8) | lbuf [4 * i + 3];
	         float t1	=*(float *)(&temp_32_1);
	         buffer [i] = std::complex<float> (t1, 0);
	      }
	   }
	   else {
              for (size_t i = 0; i < samplesRead; i ++) {
                 int32_t temp_32_1 = (lbuf [4 * i + 3] << 24) |
	                             (lbuf [4 * i + 2] << 16) |
	                             (lbuf [4 * i + 1] << 8) | lbuf [4 * i];
	         float t1	=*(float *)(&temp_32_1);
	         buffer [i] = std::complex<float> (t1, 0);
	      }
	   }
	   return;
	}
}

void	xml_Reader::readElements_Q (FILE *theFile, 
	                            std::complex<float> *buffer,
	                            int amount) {

int	nrBits	= fd -> bitsperChannel;
float	scaler	= float (shift (nrBits));

	if (fd -> container == "int8") {
	   auto *lbuf = dynVec (uint8_t, 1 * amount);
	   size_t samplesRead = fread (lbuf, 1, amount, theFile);
	   for (size_t i = 0; i < samplesRead; i ++)
	      buffer [i] =
	           std::complex<float> (127.0, ((int8_t)lbuf [i] / 127.0));
	   return;
	}
	
	if (fd -> container == "uint8") {
	   auto *lbuf = dynVec (uint8_t, 1 * amount);
	   size_t samplesRead = fread (lbuf, 1, amount, theFile);
	   for (size_t i = 0; i < samplesRead; i ++)
	      buffer [i] = std::complex<float> (0, mapTable [lbuf [i]]);
	   return;
	}

	if (fd -> container == "int16") {
	   auto *lbuf = dynVec (uint8_t, 2 * amount);
	   size_t samplesRead = fread (lbuf, 2, amount, theFile);
	   if (fd -> byteOrder == "MSB") {
	      for (size_t i = 0; i < samplesRead; i ++) {
	         int16_t temp_16_1 = (lbuf [2 * i] << 8) | lbuf [2 * i + 1];
	         buffer [i] =
	               std::complex<float> (0, (float)temp_16_1 / scaler);
	      }
	   }
	   else {
	      for (size_t i = 0; i < samplesRead; i ++) {
	         int16_t temp_16_1 = (lbuf [2 * i + 1] << 8) | lbuf [2 * i];
	         buffer [i] =
	              std::complex<float> (0, (float)temp_16_1 / scaler);
	      }
	   }
	   return;
	}

	if (fd -> container == "int24") {
	   auto *lbuf = dynVec (uint8_t, 3 * amount);
           size_t samplesRead = fread (lbuf, 3, amount, theFile);
           if (fd -> byteOrder == "MSB") {
              for (size_t i = 0; i < samplesRead; i ++) {
                 int32_t temp_32_1 = (lbuf [3 * i] << 16) |
	                             (lbuf [3 * i + 1] << 8) | lbuf [3 * i + 2];
	   	 if (temp_32_1 & 0x800000) 
	            temp_32_1 |= 0xFF000000;
	         buffer [i] =
	              std::complex<float> (0, (float)temp_32_1 / scaler);
	      }
	   }
	   else {
              for (size_t i = 0; i < samplesRead; i ++) {
                 int32_t temp_32_1 = (lbuf [3 * i + 2] << 16) |
	                             (lbuf [3 * i + 1] << 8) | lbuf [3 * i];
	   	 if (temp_32_1 & 0x800000) 
	            temp_32_1 |= 0xFF000000;
	         buffer [i] =
	              std::complex<float> (0, (float)temp_32_1 / scaler);
	      }
	   }
	   return;
	}

	if (fd -> container == "int32") {
	   auto *lbuf = dynVec (uint8_t, 4 * amount);
           size_t samplesRead = fread (lbuf, 4, amount, theFile);
           if (fd -> byteOrder == "MSB") {
              for (size_t i = 0; i < samplesRead; i ++) {
                 int32_t temp_32_1 = (lbuf [4 * i] << 24) |
	                             (lbuf [4 * i + 1] << 16) |
	                             (lbuf [4 * i + 2] << 8) | lbuf [4 * i + 3];
	         buffer [i] =
	              std::complex<float> (0, (float)temp_32_1 / scaler);
	      }
	   }
	   else {
              for (size_t i = 0; i < samplesRead; i ++) {
                 int32_t temp_32_1 = (lbuf [4 * i + 3] << 24) |
	                             (lbuf [4 * i + 2] << 16) |
	                             (lbuf [4 * i + 1] << 8) | lbuf [4 * i];
	         buffer [i] =
	               std::complex<float> (0, (float)temp_32_1 / scaler);
	      }
	   }
	   return;
	}

	if (fd -> container == "float32") {
	   auto *lbuf = dynVec (uint8_t, 4 * amount);
           size_t samplesRead = fread (lbuf, 4, amount, theFile);
           if (fd -> byteOrder == "MSB") {
              for (size_t i = 0; i < samplesRead; i ++) {
                 int32_t temp_32_1 = (lbuf [4 * i] << 24) |
	                             (lbuf [4 * i + 1] << 16) |
	                             (lbuf [4 * i + 2] << 8) | lbuf [4 * i + 3];
	         float t1	=*(float *)(&temp_32_1);
	         buffer [i] = std::complex<float> (0, t1);
	      }
	   }
	   else {
              for (size_t i = 0; i < samplesRead; i ++) {
                 int32_t temp_32_1 = (lbuf [4 * i + 3] << 24) |
	                             (lbuf [4 * i + 2] << 16) |
	                             (lbuf [4 * i + 1] << 8) | lbuf [4 * i];
	         float t1	=*(float *)(&temp_32_1);
	         buffer [i] = std::complex<float> (0, t1);
	      }
	   }
	   return;
	}
}

