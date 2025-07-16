#
/*
 *    Copyright (C) 2016 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB 
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
#pragma once

#include	<stdint.h>
#include	<complex>
#include	"ringbuffer.h"

class	processParams {
public:
	uint8_t	dabMode;
	int16_t	threshold;
	int16_t	diff_length;
	int16_t	tii_delay;
	int16_t	tii_depth;
	int16_t	echo_depth;
	int16_t	bitDepth;
	RingBuffer<float>	* responseBuffer;
	RingBuffer<Complex>	* spectrumBuffer;
	RingBuffer<Complex>	* iqBuffer;
	RingBuffer<Complex>	* tiiBuffer;
	RingBuffer<Complex>	* nullBuffer;
	RingBuffer<float>	* snrBuffer;
	RingBuffer<Complex>	* channelBuffer;
	RingBuffer<uint8_t>	* frameBuffer;
	RingBuffer<float>	* stdDevBuffer;

	processParams () {
	   responseBuffer	= nullptr;
	   spectrumBuffer	= nullptr;
	   iqBuffer	= nullptr;
	   nullBuffer	= nullptr;
	   snrBuffer	= nullptr;
	   channelBuffer	= nullptr;
	   stdDevBuffer	= nullptr;
	}
};


