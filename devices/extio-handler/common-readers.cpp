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
//
//	For the different formats for input, we have
//	different readers, with one "mother" reader.
//	Note that the cardreader is quite different
//	and coded elsewhere
//
#include	"common-readers.h"
//
//	The reader for 16 bit int values
//
	reader_16::reader_16 (RingBuffer<std::complex<float>> *p,
	                      int32_t base_16,
	                      int32_t rate):virtualReader (p, rate) {
	this	-> base = base_16;
}

	reader_16::~reader_16 (void) {
}
//
//	apparently bytes are read in from low byte to high byte
void	reader_16::processData	(float IQoffs, void *data, int cnt) {
int32_t	i;
std::complex<float> IQData [blockSize];
uint8_t	*p	= (uint8_t *)data;
	(void)IQoffs;
	(void)cnt;

	for (i = 0; i < blockSize; i ++) {
	   uint8_t r0	= p [4 * i];
	   uint8_t r1	= p [4 * i + 1];
	   uint8_t i0	= p [4 * i + 2];
	   uint8_t i1	= p [4 * i + 3];
	   int16_t re	= (r1 << 8) | r0;
	   int16_t im	= (i1 << 8) | i0;
	   IQData [i]	= std::complex<float> ((float)re / base,
	                                       (float)im / base);
	}
	
	convertandStore (IQData, blockSize);
}

int16_t reader_16::bitDepth	(void) {
	return 16;
}
//
//	The reader for 24 bit integer values
//
	reader_24::reader_24 (RingBuffer<std::complex<float>> *p,
	                      int32_t base_24, int32_t rate):
	                                       virtualReader (p, rate) {
	this	-> base	= base_24;
}

	reader_24::~reader_24 (void) {
}

void	reader_24::processData	(float IQoffs, void *data, int cnt) {
int32_t	i;
std::complex<float>	IQData [blockSize];
uint8_t	*p	= (uint8_t *)data;
	(void)IQoffs;
	(void)cnt;

	for (i = 0; i < blockSize; i ++) {
	   uint8_t r0	= p [6 * i];
	   uint8_t r1	= p [6 * i + 1];
	   uint8_t r2	= p [6 * i + 2];
	   uint8_t i0	= p [6 * i + 3];
	   uint8_t i1	= p [6 * i + 4];
	   uint8_t i2	= p [6 * i + 5];
	   int32_t re	= int32_t (uint32_t (r2 << 16 | r1 << 8 | r0));
	   int32_t im	= int32_t (uint32_t (i2 << 16 | i1 << 8 | i0));
	   IQData [i]	= std::complex<float> ((float)re / base,
	                                       (float)im / base);
	}
	
	convertandStore (IQData, blockSize);
}

int16_t reader_24::bitDepth	(void) {
	return 24;
}
//
//	The reader for 32 bit integer values
//
	reader_32::reader_32 (RingBuffer<std::complex<float>> *p,
	                      int32_t base_32, int32_t rate):
	                                         virtualReader (p, rate) {
	this	-> base = base_32;
}

	reader_32::~reader_32 (void) {
}

void	reader_32::processData	(float IQoffs, void *data, int cnt) {
int32_t	i;
std::complex<float> IQData [blockSize];
uint8_t	*p	= (uint8_t *)data;
	(void)IQoffs;
	(void)cnt;

	for (i = 0; i < blockSize; i ++) {
	   uint8_t r0	= p [8 * i];
	   uint8_t r1	= p [8 * i + 1];
	   uint8_t r2	= p [8 * i + 2];
	   uint8_t r3	= p [8 * i + 3];
	   uint8_t i0	= p [8 * i + 4];
	   uint8_t i1	= p [8 * i + 5];
	   uint8_t i2	= p [8 * i + 6];
	   uint8_t i3	= p [8 * i + 7];
	   int32_t re	= int32_t (uint32_t (r3 << 24 | r2 << 16 |
	                                             r1 << 8 | r0));
	   int32_t im	= int32_t (uint32_t (i3 << 24 | i2 << 16 |
	                                             i1 << 8 | i0));
	   IQData [i]	= std::complex<float> ((float)re / base,
	                                       (float)im / base);
	}
	
	convertandStore (IQData, blockSize);
}

int16_t	reader_32::bitDepth	(void) {
	return 32;
}
//
//	The reader for 32 bit float values
//
	reader_float::reader_float (RingBuffer<std::complex<float>> *p,
	                                                     int32_t rate):
	                                             virtualReader (p, rate) {
int16_t	i;
}

	reader_float::~reader_float (void) {
}
//
void	reader_float::processData	(float IQoffs, void *data, int cnt) {
int32_t	i, j;
std::complex<float> IQData [blockSize];
float	*p	= (float *)data;
	(void)IQoffs;
	(void)cnt;

	for (i = 0; i < blockSize; i ++) 
	   IQData [i]	= std::complex<float> (p [2 * i], p [2 * i + 1]);

	convertandStore (IQData, blockSize);
}

int16_t reader_float::bitDepth	(void) {
	return 24;
}

