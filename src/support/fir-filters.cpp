#
/*
 *    Copyright (C) 2010, 2011, 2012
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

#include	"fir-filters.h"

//	FIR LowPass

	LowPassFIR::LowPassFIR (int16_t firsize,
	                        int32_t Fc, int32_t fs){
DABFLOAT	sum	= 0.0;
//DABFLOAT	temp [firsize];
auto	*temp 	= dynVec (DABFLOAT, firsize);

	this -> frequency	= (DABFLOAT)Fc / fs;
	this -> filterSize	= firsize;
	this -> ip		= 0;
	filterKernel.	resize (filterSize);
	Buffer.		resize (filterSize);

	for (int i = 0; i < filterSize; i ++) {
	   filterKernel [i]	= 0;
	   Buffer [i]		= Complex (0, 0);
	}

	for (int i = 0; i < filterSize; i ++) {
	   if (i == filterSize / 2)
	      temp [i] = 2 * M_PI * frequency;
	   else 
	      temp [i] =
	         sin (2 * M_PI * frequency * (i - filterSize/2))/ (i - filterSize/2);
//
//	Blackman window
	   temp [i]  *= (0.42 -
		    0.5 * cos (2 * M_PI * (float)i / filterSize) +
		    0.08 * cos (4 * M_PI * (float)i / filterSize));

	   sum += temp [i];
	}

	for (int i = 0; i < filterSize; i ++)
	   filterKernel [i] = temp [i] / sum;
}

	LowPassFIR::~LowPassFIR () {
}

int	LowPassFIR::theSize	() {
	return Buffer. size ();
}

void	LowPassFIR::resize (int newSize) {
//DABFLOAT	temp [newSize];
auto	*temp 	= dynVec (DABFLOAT, newSize);
float	sum = 0;

	filterSize	= newSize;
	filterKernel. resize (filterSize);
	Buffer. resize (filterSize);
	ip		= 0;

	for (int i = 0; i < filterSize; i ++) {
	   filterKernel [i]	= 0;
	}

	for (int i = 0; i < filterSize; i ++) {
	   if (i == filterSize / 2)
	      temp [i] = 2 * M_PI * frequency;
	   else 
	      temp [i] =
	         sin (2 * M_PI * frequency * (i - filterSize/2))/ (i - filterSize/2);
//
//	Blackman window
	   temp [i]  *= (0.42 -
		    0.5 * cos (2 * M_PI * (float)i / filterSize) +
		    0.08 * cos (4 * M_PI * (float)i / filterSize));

	   sum += temp [i];
	}

	for (int i = 0; i < filterSize; i ++)
	   filterKernel [i] = temp [i] / sum;
}

//	we process the samples backwards rather than reversing
//	the kernel
Complex	LowPassFIR::Pass (Complex z) {
int16_t	i;
Complex	tmp	= 0;

	Buffer [ip]	= z;
	for (i = 0; i < filterSize; i ++) {
	   int16_t index = ip - i;
	   if (index < 0)
	      index += filterSize;
	   tmp		+= Buffer [index] * filterKernel [i];
	}

	ip = (ip + 1) % filterSize;
	return tmp;
}

DABFLOAT	LowPassFIR::Pass (DABFLOAT v) {
	return real (Pass (Complex (v, 0)));
}


