#
/*
 *    Copyright (C) 2014 .. 2024
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
#include	<stdio.h>
#include	<string>
#include	<stdint.h>
#include	"riff-reader.h"

#define	READ4BYTES	0
#define	READ6BYTES	1
#define	READ8BYTES	2

static inline
float	value_for (int bitDepth) {
int res	= 1;
	while (--bitDepth > 0)
	   res <<= 1;
	return (float)res;
}

	riffReader::riffReader (const QString &fileName) {
uint32_t segmentSize;
char header [5];

	bitDepth	= 15;
	tunedFrequency	= -1;
	denominator	= 0;
        header [4] = 0;
	filePointer     = fopen (fileName. toLatin1 (). data (), "rb");
        if (filePointer == nullptr) {
	   QString val =
                   QString ("File '%1' is no valid SDR file").arg(fileName);
           throw device_exception (val. toStdString ());
        }

        fread (header, 1, 4, filePointer);
        if (QString (header) != "RIFF") {
	   QString val =
                   QString ("File '%1' is no valid SDR file").arg(fileName);
           throw device_exception (val. toStdString ());
        }
	
        fread (&segmentSize, 1, 4, filePointer);
//	fprintf (stderr, "Size of RIFF %d\n", segmentSize);

//	We expect a "WAVE" header, enclosing an "fmt " header
	fread (header, 1, 4, filePointer);
//	fprintf (stderr, "Header -> %s\n", header);
	if (QString (header) != "WAVE") {
	   QString val =
                   QString ("File '%1' is no valid SDR file").arg(fileName);
           throw device_exception (val. toStdString ());
        }

	fread (header, 1, 4, filePointer);
//	fprintf (stderr, "Header = %s\n", header);
	if (QString (header) != "fmt ") {
	   QString val =
                   QString ("File '%1' is no valid SDR file").arg(fileName);
           throw device_exception (val. toStdString ());
        }
//
//	The format we expect is limited to genuine DAB files
	uint16_t formatTag;
	uint16_t nrChannels;
	fread (&segmentSize, 1, 4, filePointer);
//
//	save position of the fp
	std::fpos_t pos;
	std::fgetpos (filePointer, &pos);
	uint32_t samplingRate;
	fread (&formatTag, 1, sizeof (uint16_t), filePointer);
	fread (&nrChannels, 1, sizeof (uint16_t), filePointer);
	fread (&samplingRate, 1, 4, filePointer);
	fprintf (stderr, "%d %d %d\n", formatTag, nrChannels, samplingRate);
	if ((formatTag != 01) ||
	    (nrChannels != 02) || (samplingRate != 2048000)) {
	   QString val =
                   QString ("File '%1' is no valid SDR file").arg(fileName);
           throw device_exception (val. toStdString ());
        }

	uint32_t bytesperSecond;
	fread (&bytesperSecond, 1, 4, filePointer);
//	fprintf (stderr, "bytes per second %d\n", bytesperSecond);

	fread (&blockAlign, 1, 2, filePointer);
	if (blockAlign == 4) 
	   readBytes	= READ4BYTES;
	else
	if (blockAlign == 6)
	   readBytes	= READ6BYTES;
	else
	if (blockAlign == 8) {
	   readBytes	= READ8BYTES;
	   QString val =
	           QString ("File '%1' has unsupported 32 bit elements"). arg (fileName);
	   throw device_exception (val. toStdString ());
	}
	else {
	   QString val =
                   QString ("File '%1' has unsupported elements").arg(fileName);
           throw device_exception (val. toStdString ());
        }

	fsetpos (filePointer, &pos);
	fseek (filePointer, segmentSize, SEEK_CUR);

	fread (header, 1, 4, filePointer);
	while (QString (header) != "data") {
	   fread (&segmentSize, 1, 4, filePointer);
//	   fprintf (stderr, "we read %s (%d)\n", header, segmentSize);
	   if (QString (header) == "freq")
	      fread (&tunedFrequency, 1, 4, filePointer);
	   else
	   if (QString (header) == "bits")
	      fread (&bitDepth, 1, 4, filePointer);
	   else
	      fseek (filePointer, segmentSize, SEEK_CUR);
	   fread (header, 1, 4, filePointer);
//	   fprintf (stderr, "Now we read %s\n", header);
	   if (feof (filePointer)) {
	      QString val =
                   QString ("File '%1' is no valid SDR file").arg(fileName);
              throw device_exception (val. toStdString ());
           }
	}

	denominator	= value_for (bitDepth);
	if (QString (header) != "data") {	// should not happen
	   QString val =
                   QString ("File '%1' is no valid SDR file").arg(fileName);
           throw device_exception (val. toStdString ());
        }

	uint32_t xxx;
	fread (&xxx, 1, 4, filePointer);
//	fprintf (stderr, "nrbytes in data %d\n", nrElements);
	nrElements = xxx / blockAlign;
	remainingElements	= nrElements;
	std::fgetpos (filePointer, &baseofData);
}

	riffReader::~riffReader () {
	fclose (filePointer);}

void	riffReader::reset	() {
	fsetpos (filePointer, &baseofData);
	remainingElements = nrElements;
}

int	riffReader::read (std::complex<float> *buffer, uint64_t nrSamples) {

	switch (readBytes) {
	   case READ4BYTES:
	      return read4Bytes (buffer, nrSamples);

	   case READ6BYTES:
	      return read6Bytes (buffer, nrSamples);

	   case READ8BYTES:
	      return read8Bytes (buffer, nrSamples);

	   default:	// cannot happen
	      return 0;
	}
}

int	riffReader::read4Bytes (std::complex<float> *buffer,
	                                    uint64_t nrSamples) {
int16_t lBuf [2 * nrSamples];

	if (nrSamples > remainingElements) {
	   nrSamples = remainingElements;
	   remainingElements	= 0;
	}
	int n =  fread (lBuf, sizeof (int16_t), 2 * nrSamples, filePointer);
	for (int i = 0; i < n / 2; i ++)
	   buffer [i] =
	      std::complex<float> ((float)(lBuf [2 * i]) / denominator,
	                           (float)(lBuf [2 * i + 1]) /denominator);
	if (remainingElements != 0)
	   remainingElements -= nrSamples;
	return nrSamples;
}

int	riffReader::read6Bytes (std::complex<float> *buffer,
	                                    uint64_t nrSamples) {
int8_t lBuf [3 * 2 * nrSamples];
int	next	= 0;

	if (nrSamples > remainingElements) {
	   nrSamples = remainingElements;
	   remainingElements	= 0;
	}
	int n =  fread (lBuf, 1, 6 * nrSamples, filePointer);
	float scaler = 8388607.0 / 512;
	for (int i = 0; i < n; i += 6) {
	   int32_t re = 
	          lBuf [i] << 24 | lBuf [i + 1] << 16 | lBuf [i + 2] << 8;
	   int32_t im =
	          lBuf [i + 3] << 24 | lBuf [i + 4] << 16 | lBuf [i + 5] << 8;
	   buffer [next ++] = std::complex<float> (re / scaler, im / scaler);
	}
	if (remainingElements != 0)
           remainingElements -= nrSamples;

	return nrSamples;
}

int	riffReader::read8Bytes (std::complex<float> *Buffer,
	                                    uint64_t nrSamples) {
	(void)Buffer;
	(void)nrSamples;
	return 0;
}

uint64_t	riffReader::elementCount	() {
	return nrElements;
}

uint64_t	riffReader::currentPos		() {
	return nrElements - remainingElements;
}

int	riffReader::getVFOFrequency	() {
	return tunedFrequency;
}

