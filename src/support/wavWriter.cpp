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
//
//	The RIFF writer writes the input samples into a "wav" file
//	with the samplerate of 2048000, as 16 bit int's, and a "chunk"
//	(chunk Id "freq") is added that contains the frequency
#include	"wavWriter.h"

static
const char	* riff	= "RIFF";
static
const char	* junk	= "JUNK";
static
const char	* bw64	= "BW64";
static
const char	* ds64	= "ds64";
static
const char	* wave	= "WAVE";
static
const char	* fmt	= "fmt ";
static
const char	* aux1	= "freq";
static
const char	* aux2	= "bits";
static
const char	* data	= "data";

	wavWriter::wavWriter	() {
	isValid = false;
}

	wavWriter::~wavWriter	() {}

bool	wavWriter::init	(const QString &fileName, const int sampleRate, 
	                 const int frequency, const int bitDepth) {
	isValid			= false;
	filePointer		= fopen (fileName. toUtf8 (). data (), "wb");
	if (filePointer == nullptr)
	   return false;

	fwrite (riff, 1, 4, filePointer);

//	the ultimate filesize is written at location 4
	fseek (filePointer, 4, SEEK_CUR);

//	The default header:
	locationCounter		= 8;
	fwrite (wave, 1, 4, filePointer);
	locationCounter		+= 4;
//
//	junk block
	fwrite (junk, 1, 4, filePointer);
	locationCounter		+= 4;
	uint32_t junkSize	= 32;
	fwrite (&junkSize, 1, 4, filePointer);
	locationCounter		+= 4;
	char junkNix [32]	= {0};
	fwrite (&junkNix, 1, 32, filePointer);
	locationCounter		+= 32;

	fwrite (fmt,  1, 4, filePointer);
	locationCounter		+= 4;
	uint32_t fmtSize	= 16;
	fwrite (&fmtSize, 1, 4, filePointer);
	locationCounter		+= 4;
	uint16_t	formatTag	= 01;
	fwrite (&formatTag, 1, sizeof (uint16_t), filePointer);
	locationCounter		+= 2;
	uint16_t	nrChannels	= 2;
	fwrite (&nrChannels, 1, sizeof (uint16_t), filePointer);
	locationCounter		+= 2;
	uint32_t	samplingRate	= sampleRate;
	fwrite (&samplingRate, 1, sizeof (uint32_t), filePointer);
	locationCounter		+= 4;
	uint32_t	bytesperSecond	= 4 * samplingRate;
	fwrite (&bytesperSecond, 1, sizeof (uint32_t), filePointer);
	locationCounter		+= 4;
	uint16_t	bytesperBlock	= 4;
	fwrite (&bytesperBlock, 1, sizeof (uint16_t), filePointer);
	locationCounter		+= 2;
	uint16_t bitsperSample		= 16;
	fwrite (&bitsperSample, 1, sizeof (uint16_t), filePointer);
	locationCounter		+= 2;
//
//	the "freq" chunk
	if (frequency > 0) {
	   fwrite (aux1, 1, 4, filePointer);
	   locationCounter	+= 4;
	   int freqLen	= sizeof (int32_t);
	   fwrite (&freqLen, 1, 4, filePointer);
	   locationCounter	+= 4;
	   fwrite (&frequency, 1, 4, filePointer);
	   locationCounter	+= 4;
	}
//
//	the "bitdepth" chunk
	if (bitDepth > 0) {
	   fwrite (aux2, 1, 4, filePointer);
	   locationCounter	+= 4;
	   int bitDepthLen	= 4;
	   fwrite (&bitDepthLen, 1, 4, filePointer);
	   locationCounter	+= 4;
	   fwrite (&bitDepth, 1, 4, filePointer);
	   locationCounter	+= 4;
	}
//
//	start of the "data" chunk
	fwrite (data, 1, 4, filePointer);
	locationCounter		+= 4;

	nrElements	= 0;
	isValid		= true;
	return true;
}

void	wavWriter::close	() {
	if (!isValid)
	   return;
	isValid		= false;
	uint64_t nrBytes	= nrElements * 2 * sizeof (int16_t);

//	reset the fp to the location where the nr bytes in the
//	data chunk should be written
	fseek (filePointer, 0, SEEK_SET);
	if (nrBytes < (uint64_t)0xFFFFFFFF) {
//	   fwrite (bw64, 1, 4, filePointer);
	   fseek (filePointer, locationCounter, SEEK_SET);
	   fwrite (&nrBytes, 1, 4, filePointer);
//
//	compute the number if of to be recorded in the RIFF count
	   fseek (filePointer, 0, SEEK_END);
	   int riffSize	= ftell (filePointer) - 8;
//
//	and record the value at loc 4
	   fseek (filePointer, 4, SEEK_SET);
	   fwrite (&riffSize, 1, 4, filePointer);
	   fseek (filePointer, 0, SEEK_END);
	   fclose (filePointer);
	   return;
	}
//	a large file, make it BW64
//
	uint32_t minusOne = 0xFFFFFFFF;
	fwrite (bw64, 1, 4, filePointer);
	fwrite (&minusOne, 1, 4, filePointer);
	fseek (filePointer, locationCounter, SEEK_SET);
	fwrite (&minusOne, 1, 4, filePointer);
//	compute the number if of to be recorded in the RIFF count
	fseek (filePointer, 0, SEEK_END);
	int riffSize	= ftell (filePointer) - 8;
//	just fill the ds64 struct
	fseek (filePointer, 12, SEEK_SET);
	fwrite (ds64, 1, 4, filePointer);
	fseek (filePointer, 20, SEEK_SET);
	uint64_t theSize	= riffSize;
	fwrite (&theSize, 1, 8, filePointer);
	fwrite (&nrBytes, 1, 8, filePointer);
	fseek (filePointer, 0, SEEK_END);
	fclose (filePointer);
}

void	wavWriter::write (int16_t *buff, uint64_t samples) {
	if (!isValid)
	   return;
	fwrite (buff, 2 * sizeof (int16_t), (size_t)samples, filePointer);
	nrElements	+= samples;
}

bool	wavWriter::isActive	() {
	return isValid;
}

