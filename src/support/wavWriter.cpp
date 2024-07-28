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

//
#include	"wavWriter.h"
static
const char	* riff	= "RIFF";
static
const char	* wave	= "WAVE";
static
const char	* fmt	= "fmt ";
static
const char	* data	= "data";

	wavWriter::wavWriter	() {
	isValid = false;
}

	wavWriter::~wavWriter	() {}

bool	wavWriter::init		(const QString &fileName) {
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
	uint32_t	samplingRate	= 48000;
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
	int nrBytes	= nrElements * 2 * sizeof (int16_t);
//
//	reset the fp to the location where the nr bytes in the
//	data chunk shouold be written
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
}

void	wavWriter::write (const int16_t *buf, int samples) {
	if (!isValid)
	   return;

	fwrite (buf, 2 * sizeof (int16_t), samples, filePointer);
	nrElements	+= samples;
}

bool	wavWriter::isActive	() {
	return isValid;
}

