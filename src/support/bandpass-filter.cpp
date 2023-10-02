#
/*
 *    Copyright (C) 2020
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
#include	"bandpass-filter.h"
/*
 *	The bandfilter is for the complex domain. 
 *	We create a lowpass filter, which stretches over the
 *	positive and negative half, then shift this filter
 *	to the right position to form a nice bandfilter.
 *	For the real domain, we use the Simple BandPass version.
 */
	BandPassFIR::BandPassFIR (int16_t filterSize,
	                          int32_t low, int32_t high,
	                          int32_t sampleRate):
	                                 kernel (filterSize),
	                                 buffer (filterSize) {
float	tmp [filterSize];
float	lo	= (float) ((high - low) / 2) / sampleRate;
float	shift	= (float) ((high + low) / 2) / sampleRate;
float	sum	= 0.0;


	this	-> sampleRate	= sampleRate;
	this	-> filterSize	= filterSize;
	this	-> ip		= 0;

	for (int i = 0; i < filterSize; i ++) {
	   kernel [i] = Complex (0, 0);
	   buffer [i] = Complex (0, 0);
	}

	for (int i = 0; i < filterSize; i ++) {
	   if (i == filterSize / 2)
	      tmp [i] = 2 * M_PI * lo;
	   else 
	      tmp [i] = sin (2 * M_PI * lo * (i - filterSize /2)) / (i - filterSize/2);
//
//	windowing
	   tmp [i]  *= (0.42 -
		    0.5 * cos (2 * M_PI * (float)i / (float)filterSize) +
		    0.08 * cos (4 * M_PI * (float)i / (float)filterSize));

	   sum += tmp [i];
	}

	for (int i = 0; i < filterSize; i ++) {	// shifting
	   float v = (i - filterSize / 2) * (2 * M_PI * shift);
	   kernel [i] = Complex (tmp [i] * cos (v) / sum, 
	                         tmp [i] * sin (v) / sum);
	}
}

	BandPassFIR::~BandPassFIR () {
}


//	we process the samples backwards rather than reversing
//	the kernel
std::complex<float> BandPassFIR::Pass (Complex z) {
int16_t	i;
Complex	tmp	= 0;

	buffer [ip]	= z;
	for (i = 0; i < filterSize; i ++) {
	   int16_t index = ip - i;
	   if (index < 0)
	      index += filterSize;
	   tmp		+= buffer [index] * kernel [i];
	}

	ip = (ip + 1) % filterSize;
	return tmp;
}

float	BandPassFIR::Pass (float v) {
int16_t		i;
float	tmp	= 0;

	buffer [ip] = Complex (v, 0);
	for (i = 0; i < filterSize; i ++) {
	   int16_t index = ip - i;
	   if (index < 0)
	      index += filterSize;
	   tmp += real (buffer [index]) * real (kernel [i]);
	}

	ip = (ip + 1) % filterSize;
	return tmp;
}

