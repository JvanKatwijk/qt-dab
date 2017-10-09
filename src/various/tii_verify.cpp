#
/*
 *    Copyright (C) 2014 .. 2017
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
 *
 *	To locate the position of the pattern, we use correlation
 *	the correlation here is implemented as an FFT based
 *	correlation. Experience shows that the result is reasonably
 *	reliable when the value of the "peak" found is 3 or more times
 *	the average value in the spectrum.
 */

#include	"tii_verify.h"

	tii_verify::tii_verify (int16_t length,
	                        int16_t startCarrier,
	                        uint64_t pattern,
	                        int16_t	carriers,
	                        DSPCOMPLEX *refBase) {
int16_t i;
	this -> length		= length;
	(void)carriers;
	fft_up			= new common_fft (length);
	fft_buffer_1		= fft_up -> getVector ();
	refVector		= new DSPCOMPLEX [length];
	memset (fft_buffer_1, 0, length * sizeof (DSPCOMPLEX));

//	we create a reference vector, using the known startposition
//	of the pattern and the pattern elements themselves.
	int16_t st	= startCarrier;	// note that startCarrier is negative

	fft_buffer_1 [length - 1 + st] = refBase [length - 1 + st];
	uint64_t p	= pattern;
	for (i = 0; i < 15; i ++) {
	   int16_t offset = ((p >> 56) & 0xF) * 48;
	   st	+= offset;
	   int16_t realIndex = st < 0 ? length - 1 + st : st + 1;
	   fft_buffer_1 [realIndex] = refBase [realIndex];
	   if (realIndex + 1 != 0)
	      fft_buffer_1 [realIndex + 1] = refBase [realIndex];
	   p <<= 4;
	}
//
//	Then we take the fft and store the result in refVector
	fft_up	-> do_FFT ();
	memcpy (refVector, fft_buffer_1, length * sizeof (DSPCOMPLEX));
	fft_down		= new common_ifft (length);
	fft_buffer_2		= fft_down -> getVector ();
}

	tii_verify::~tii_verify (void) {
	delete	fft_up;
	delete	fft_down;
	delete[] refVector;
}

#define	SHIFT	16
//	We know the segment in the input where the pattern can be
//	detected, it is a range of 48 input values.
int16_t	tii_verify::correlate (DSPCOMPLEX *v) {
int	i;
int16_t maxIndex	= -1;
float	Max		= 0;
float	sum		= 0;
//
//	Correlation is by taking the fft, multiplying by the conj
//	of the reference vector, ifft and find a maximum.
//	We match the SHIFTED input with the reference, shifting
//	allows offsets both positive and negative
	memset (fft_buffer_1, 0, length * sizeof (DSPCOMPLEX));
	for (i = 0; i < length - SHIFT; i ++)
	   fft_buffer_1 [i + SHIFT] = v [i];
	
	fft_up -> do_FFT ();
	for (i = 0; i < length; i ++)
	   fft_buffer_2 [i] = fft_buffer_1 [i] * conj (refVector [i]);
	fft_down -> do_IFFT ();
	
	for (i = 1; i < 24; i ++) {
	   sum += abs (fft_buffer_2 [2 * i]);
	   if (abs (fft_buffer_2 [2 * i]) > Max) {
	      maxIndex = i;
	      Max	= abs (fft_buffer_2 [2 * i]);
	   }
	}
	
	sum /= 23;
//
//	unshift the result
	return Max > 2.5 * sum ? maxIndex - SHIFT / 2 : -100;
}

