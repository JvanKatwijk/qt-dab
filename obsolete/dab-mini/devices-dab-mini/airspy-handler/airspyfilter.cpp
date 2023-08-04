#
/*
 *    Copyright (C) 2008, 2009, 2010
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of qt-dab
 *
 *    qt-dab is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation as version 2 of the License.
 *
 *    qt-dab is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with qt-dab; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	"airspyfilter.h"
#include	<math.h>
#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif

	airspyFilter::airspyFilter (int16_t firSize,
	                            int32_t cutoffFreq,
	                            int32_t sampleRate) {
float	tmp [firSize];
float	lo	= (float)(cutoffFreq) / sampleRate;
float	sum	= 0.0;
int16_t	i;

	this	-> buffer_re	= new float [firSize];
	this	-> buffer_im	= new float [firSize];
	this	-> kernel	= new float [firSize];
	this	-> firSize	= firSize;
	ip			= 0;
//
//	create a low pass filter for lo;
	for (i = 0; i < firSize; i ++) {
	   if (i == firSize / 2)
	      tmp [i] = 2 * M_PI * lo;
	   else 
	      tmp [i] = sin (2 * M_PI * lo * (i - firSize /2)) / (i - firSize/2);
//
//	windowing with Blackman
	   tmp [i]  *= (0.42 -
		    0.5 * cos (2 * M_PI * (float)i / (float)firSize) +
		    0.08 * cos (4 * M_PI * (float)i / (float)firSize));

	   sum += tmp [i];
	}
//
//	and the kernel
	for (i = 0; i < firSize; i ++) 
	   kernel [i] = tmp [i] / sum;
}

	airspyFilter::~airspyFilter (void) {
	delete [] kernel;
	delete [] buffer_re;
	delete [] buffer_im;
}

//      we process the samples backwards rather than reversing
//      the kernel
std::complex<float>	airspyFilter::Pass (float re, float im) {
int16_t i;
float	tmp_1	= 0;
float	tmp_2	= 0;

        buffer_re [ip]     = re;
        buffer_im [ip]     = im;
        for (i = 0; i < firSize; i ++) {
           int16_t index = ip - i;
           if (index < 0)
              index += firSize;
           tmp_1	+= buffer_re [index] * kernel [i];
           tmp_2	+= buffer_im [index] * kernel [i];
        }

        ip = (ip + 1) % firSize;
        return std::complex<float> (tmp_1, tmp_2);;
}

