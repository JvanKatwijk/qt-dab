#
/*
 *    Copyright (C)  2015 .. 2025
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
//	code based on a proposal made by AI

#include	<complex>
#include	"up-converter.h"

//
//	about the parameters
//	upFactor is the scaling factor for upsampling, i.e. outRate / inrate
//	firSize is the size of the filter,
//	fs is the input sample frequency, i.e. outrate = fs * upFactor
	upConverter::upConverter (int upFactor, int firSize, int fs) {
float	sum = 0;

	int realSize = ((firSize + upFactor - 1) / upFactor) * upFactor;
	std::vector<float> kernel (realSize);
	std::vector<float> temp   (realSize);
	float	 frequency	= 0.49 * (float)fs / fs;

	this	-> upFactor	= upFactor;
	this	-> ip		= 0;
	for (int i = 0; i < realSize; i ++)
	   kernel [i] = 0;

	for (int i = 0; i < realSize; i ++) {
	   if (i == firSize / 2)
	      temp [i] = 2 * M_PI * frequency;
	   else
	      temp [i] =
	        sin (2 * M_PI * frequency * (i - realSize / 2)) / (i - realSize / 2);
	   //      Blackman window
	   temp [i]  *= (0.42 -
	                0.5 * cos (2 * M_PI * (float)i / realSize) +
	                0.08 * cos (4 * M_PI * (float)i / realSize));
	   sum += temp [i];
        }
	

	subfilterSize	= realSize / upFactor;
	subfilters. resize (upFactor, 
	                    std::vector<float> (subfilterSize, 0.0));
//
//	partition the  coefficients over the polyfase subfilters
	for (int i = 0; i < realSize; i ++) {
	   int subfilter	= i % upFactor;
	   int coeff_index	= i / upFactor;
	   subfilters [subfilter][coeff_index] = temp [i] / sum;
	}

	buffer. resize (subfilterSize, 0.0);
}

	upConverter::~upConverter	() {}
//
//	Input a single sample on rate fs,
//	output a vector with size upFactor with samples on the output rate
std::vector<std::complex<float>>
	                 upConverter::process (std::complex<float> sample) {
std::vector<std::complex<float>> outputSamples (upFactor, 0.0);

	buffer [ip] = sample;

	for (int i = 0; i < upFactor; i ++) {
	   std::complex<float> sum = std::complex<float> (0, 0);
	   int current_ip = ip;

	   for (int k = 0; k < subfilterSize; k ++) {
	      sum += buffer [current_ip] * subfilters [i][k];
	      if (current_ip == 0)
	         current_ip = subfilterSize - 1;
	      else
	         current_ip --;
	   }
	   outputSamples [i] = sum * (float)upFactor;

	   ip = (ip + 1) % subfilterSize;
	}
	return outputSamples;
}

