#
/*
 *    Copyright (C) 2014 .. 2023
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

	riffReader::riffReader (const QString &fileName) {
uint32_t segmentSize;
char header [5];

	tunedFrequency	= -1;
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
	fprintf (stderr, "Size of RIFF %d\n", segmentSize);
//
//	We expect a "WAVE" header, enclosing an "fmt " header
	fread (header, 1, 4, filePointer);
//	fprintf (stderr, "Header -> %s\n", header);
	if (QString (header) != "WAVE") {
	   QString val =
                   QString ("File '%1' is no valid SDR file").arg(fileName);
           throw device_exception (val. toStdString ());
        }

	fread (header, 1, 4, filePointer);
	fprintf (stderr, "Header = %s\n", header);
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
	fprintf (stderr, "bytes per second %d\n", bytesperSecond);

	fread (&blockAlign, 1, 2, filePointer);
	if (blockAlign != 4) {
	   QString val =
                   QString ("File '%1' is no valid SDR file").arg(fileName);
           throw device_exception (val. toStdString ());
        }
	fprintf (stderr, "blockAlign %d\n", blockAlign);
	fsetpos (filePointer, &pos);
	fseek (filePointer, segmentSize, SEEK_CUR);

	fread (header, 1, 4, filePointer);
	while (QString (header) != "data") {
	   fread (&segmentSize, 1, 4, filePointer);
	   fprintf (stderr, "we read %s (%d)\n", header, segmentSize);
	   if (QString (header) == "freq")
	      fread (&tunedFrequency, 1, 4, filePointer);
	   else
	      fseek (filePointer, segmentSize, SEEK_CUR);
	   fread (header, 1, 4, filePointer);
	   fprintf (stderr, "Now we read %s\n", header);
	   if (feof (filePointer)) {
	      QString val =
                   QString ("File '%1' is no valid SDR file").arg(fileName);
              throw device_exception (val. toStdString ());
           }
	}

	if (QString (header) != "data") {	// should not happen
	   QString val =
                   QString ("File '%1' is no valid SDR file").arg(fileName);
           throw device_exception (val. toStdString ());
        }

	fread (&nrElements, 1, 4, filePointer);
	fprintf (stderr, "nrbytes in data %d\n", nrElements);
	nrElements /= blockAlign;
	remainingElements	= nrElements;
	fprintf (stderr, "nrElements %d\n", nrElements);
	std::fgetpos (filePointer, &baseofData);
}

	riffReader::~riffReader () {
	fclose (filePointer);}

void	riffReader::reset	() {
	fsetpos (filePointer, &baseofData);
	remainingElements = nrElements;
}

int	riffReader::read (std::complex<float> *buffer, int nrSamples) {
int16_t lBuf [2 * nrSamples];

	if (nrSamples > remainingElements) {
	   nrSamples = remainingElements;
	   remainingElements	= 0;
	}
	int n =  fread (lBuf, sizeof (int16_t), 2 * nrSamples, filePointer);
	for (int i = 0; i < nrSamples; i ++)
	   buffer [i] = std::complex<float> ((float)(lBuf [2 * i]) / 2048.0,
	                                     (float)(lBuf [2 * i + 1]) / 2048.0);
	if (remainingElements != 0)
	   remainingElements -= nrSamples;
	return nrSamples;
}

int	riffReader::elementCount	() {
	return nrElements;
}

int	riffReader::currentPos		() {
	return nrElements - remainingElements;
}

int	riffReader::getVFOFrequency	() {
	return tunedFrequency;
}

