#
/*
 *    Copyright (C) 2016 .. 2023
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

//
//	For the different formats for input, we have
//	different readers, with one "mother" reader.
//	Note that the cardreader is quite different here
//	and its code is elsewhere

#pragma once

#include	"virtual-reader.h"

class	reader_16: public virtualReader {
public:
	reader_16	(RingBuffer<std::complex<float>> *p, int32_t, int32_t);
	~reader_16	(void);
void	processData	(float IQoffs, void *data, int cnt);
int16_t bitDepth	(void);
};

class	reader_24: public virtualReader {
public:
	reader_24	(RingBuffer<std::complex<float>> *p, int32_t, int32_t);
	~reader_24	(void);
void	processData	(float IQoffs, void *data, int cnt);
int16_t bitDepth	(void);
};

class	reader_32: public virtualReader {
public:
	reader_32	(RingBuffer<std::complex<float>> *p, int32_t, int32_t);
	~reader_32	(void);
void	processData	(float IQoffs, void *data, int cnt);
int16_t	bitDepth	(void);
};

//
//	This is the only one we actually need for
//	elad s2 as input device for DAB
class	reader_float: public virtualReader {
public:
	reader_float	(RingBuffer<std::complex<float>> *p, int32_t);
	~reader_float	(void);
void	processData	(float IQoffs, void *data, int cnt);
int16_t	bitDepth	(void);
private:
	int16_t		mapTable_int	[SAMPLERATE / 1000];
	float		mapTable_float	[SAMPLERATE / 1000];
	std::complex<float>	convBuffer	[3072 + 1];
	int16_t		convIndex;
};


