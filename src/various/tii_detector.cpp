#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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

#include	"tii_detector.h"
//#include	"tii_verify.h"
#include	<stdio.h>
//
//	Transmitter Identification Info is carrier in the null period
//	of a DAB frame. In case the FIB's carry information on the
//	set of transmitters used in the SFN, an attempt is made
//	to identify the transmitter by inspecting the null period.
//	The information in the null-period is encoded in a "p"
//	a "pattern" and a "c", a "carrier"
//	value. The "p" value, derived from the FIB, defines the
//	pattern within the null-period as well as a set of
//	startcarriers, i.e. carrier numbers where the pattern
//	could start.
//	The start carrier itself determined the "c" value.
//	Basically, within an SFN the "p" is fixed for all transmitters,
//	while the latter show the pattern on different positions in
//	the carriers of the null-period.
//
//	Matching the position of the pattern is relatively easy, since
//	the standard defines the signals (i.e. phase and amplitude) of
//	the carriers in the pattern.
//
//	As it turns out, the pattern is represented by a sequence
//	consisting of elements with two subsequent bins with the same
//	value, followed by a "gap" of K * 48 (-1) bins.
//
//	The constructor of the class generates the patterns, according
//	to the algorithm in the standard.
		TII_Detector::TII_Detector (uint8_t dabMode):
	                                          params (dabMode),
	                                          my_fftHandler (dabMode) {
int16_t	p, c, k;

	this	-> T_u		= params. get_T_u ();
	this	-> carriers	= params. get_carriers ();
	this	-> theBuffer	= new std::complex<float> [T_u];
	fillCount		= 0;
	fft_buffer		= my_fftHandler. getVector ();	
//
//	create the patterns
	for (p = 0; p < 70; p ++) {
	   int16_t digits	= 0;
	   c = 0;		// patterns are equal for all c values
	   {
	      bool first = true;
	      int lastOne = 0;
	      for (k = -carriers / 2; k < -carriers / 4; k ++) {
	         if (A (c, p, k) > 0) {
	            if (first) {
	               first = false;
	               theTable [p]. carrier = k;
	               theTable [p]. pattern = 0;
	            }
	            else {
	               theTable [p]. pattern <<= 4;
	               theTable [p]. pattern |= (k - lastOne) / 48;
	               digits ++;
	            }
	            lastOne = k;
	         }
	      }

	      for (k = -carriers / 4; k < 0; k ++) {
	         if (A (c, p, k) > 0) {
	            theTable [p]. pattern <<= 4;
	            theTable [p]. pattern |= (k - lastOne) / 48;
	            lastOne = k;
	            digits ++;
	         }
	      }

	      for (k = 1; k <= carriers / 4; k ++) {
	         if (A (c, p, k) > 0) {
	            theTable [p]. pattern <<= 4;
	            theTable [p]. pattern |= (k - lastOne) / 48;
	            lastOne = k;
	            digits ++;
	         }
	      }

	      for (k = carriers / 4 + 1; k <= carriers / 2; k ++) {
	         if (A (c, p, k) > 0) {
	            theTable [p]. pattern <<= 4;
	            theTable [p]. pattern |= (k - lastOne) / 48;
	            lastOne = k;
	            digits ++;
	         }
	      }
	   }
//	   fprintf (stderr, "p = %d\tc = %d\tk=%d\tpatter=%llX (digits = %d)\n",
//	                     p, c, theTable [p]. carrier,
//	                        theTable [p]. pattern, digits);
	}
}

		TII_Detector::~TII_Detector (void) {
	delete []	theBuffer;
}

//	Zm (0, k) is a function of the P and the C, together forming the key to
//	the database where he transmitter locations are described.
//
//	p is in the range  0 .. 69
//	c is in the range 0 .. 23
//	The transmitted signal - for a given p and c -
//	   Zm (0, k) = A (c, p) (k) e^jPhi (k) + A (c, p) (k - 1) e ^jPhi (k - 1)
//
//	a (b, p) = getBit (table [p], b);

static
uint8_t table [] = {
	0017,		// 0 0 0 0 1 1 1 1		0
	0027,		// 0 0 0 1 0 1 1 1		1
	0033,		// 0 0 0 1 1 0 1 1		2
	0035,		// 0 0 0 1 1 1 0 1		3
	0036,		// 0 0 0 1 1 1 1 0		4
	0047,		// 0 0 1 0 0 1 1 1		5
	0053,		// 0 0 1 0 1 0 1 1		6
	0055,		// 0 0 1 0 1 1 0 1		7
	0056,		// 0 0 1 0 1 1 1 0		8
	0063,		// 0 0 1 1 0 0 1 1		9

	0065,		// 0 0 1 1 0 1 0 1		10
	0066,		// 0 0 1 1 0 1 1 0		11
	0071,		// 0 0 1 1 1 0 0 1		12
	0072,		// 0 0 1 1 1 0 1 0		13
	0074,		// 0 0 1 1 1 1 0 0		14
	0107,		// 0 1 0 0 0 1 1 1		15
	0113,		// 0 1 0 0 1 0 1 1		16
	0115,		// 0 1 0 0 1 1 0 1		17
	0116,		// 0 1 0 0 1 1 1 0		18
	0123,		// 0 1 0 1 0 0 1 1		19

	0125,		// 0 1 0 1 0 1 0 1		20
	0126,		// 0 1 0 1 0 1 1 0		21
	0131,		// 0 1 0 1 1 0 0 1		22
	0132,		// 0 1 0 1 1 0 1 0		23
	0134,		// 0 1 0 1 1 1 0 0		24
	0143,		// 0 1 1 0 0 0 1 1		25
	0145,		// 0 1 1 0 0 1 0 1		26
	0146,		// 0 1 1 0 0 1 1 0		27
	0151,		// 0 1 1 0 1 0 0 1		28	
	0152,		// 0 1 1 0 1 0 1 0		29

	0154,		// 0 1 1 0 1 1 0 0		30
	0161,		// 0 1 1 1 0 0 0 1		31
	0162,		// 0 1 1 1 0 0 1 0		32
	0164,		// 0 1 1 1 0 1 0 0		33
	0170,		// 0 1 1 1 1 0 0 0		34
	0207,		// 1 0 0 0 0 1 1 1		35
	0213,		// 1 0 0 0 1 0 1 1		36
	0215,		// 1 0 0 0 1 1 0 1		37
	0216,		// 1 0 0 0 1 1 1 0		38
	0223,		// 1 0 0 1 0 0 1 1		39

	0225,		// 1 0 0 1 0 1 0 1		40
	0226,		// 1 0 0 1 0 1 1 0		41
	0231,		// 1 0 0 1 1 0 0 1		42
	0232,		// 1 0 0 1 1 0 1 0		43
	0234,		// 1 0 0 1 1 1 0 0		44
	0243,		// 1 0 1 0 0 0 1 1		45
	0245,		// 1 0 1 0 0 1 0 1		46
	0246,		// 1 0 1 0 0 1 1 0		47
	0251,		// 1 0 1 0 1 0 0 1		48
	0252,		// 1 0 1 0 1 0 1 0		49

	0254,		// 1 0 1 0 1 1 0 0		50
	0261,		// 1 0 1 1 0 0 0 1		51
	0262,		// 1 0 1 1 0 0 1 0		52
	0264,		// 1 0 1 1 0 1 0 0		53
	0270,		// 1 0 1 1 1 0 0 0		54
	0303,		// 1 1 0 0 0 0 1 1		55
	0305,		// 1 1 0 0 0 1 0 1		56
	0306,		// 1 1 0 0 0 1 1 0		57
	0311,		// 1 1 0 0 1 0 0 1		58
	0312,		// 1 1 0 0 1 0 1 0		59

	0314,		// 1 1 0 0 1 1 0 0		60
	0321,		// 1 1 0 1 0 0 0 1		61
	0322,		// 1 1 0 1 0 0 1 0		62
	0324,		// 1 1 0 1 0 1 0 0		63
	0330,		// 1 1 0 1 1 0 0 0		64
	0341,		// 1 1 1 0 0 0 0 1		65
	0342,		// 1 1 1 0 0 0 1 0		66
	0344,		// 1 1 1 0 0 1 0 0		67
	0350,		// 1 1 1 0 1 0 0 0		68
	0360		// 1 1 1 1 0 0 0 0		69
};

static
uint8_t bits [] = {128, 64, 32, 16, 8, 4, 2, 1};
static inline
uint8_t getbit (uint8_t value, int16_t bitPos) {
	return value & bits [bitPos] ? 1 : 0;
}

static inline
uint8_t delta (int16_t a, int16_t b) {
	return a == b ? 1 : 0;
}
//
//	Litterally copied from the standard. There is no doubt that
//	creating the patterns above can be done more efficient, however
//	this approach feels more readable, while initialization, i.e.
//	executing the constructor, is done once
int16_t	TII_Detector::A (uint8_t c, uint8_t p, int16_t k) {
int16_t b;
int16_t res	= 0;

	if ((-768 <= k) && (k < - 384))
	   for (b = 0; b <= 7; b ++)
	      res += delta (k, -768 + 2 * c + 48 * b) * getbit (table [p], b);
	else
	if ((-384 <= k) && (k < 0))
	   for (b = 0; b <= 7; b ++)
	      res += delta (k, -384 + 2 * c + 48 * b) * getbit (table [p], b);
	else
	if ((0 < k) && (k <= 384))
	   for (b = 0; b <= 7; b ++)
	      res += delta (k, 1 + 2 * c + 48 * b) * getbit (table [p], b);
	else
	if ((384 < k) && (k <= 768))
	   for (b = 0; b <= 7; b ++)
	      res += delta (k, 385 + 2 * c + 48 * b) * getbit (table [p], b);
	else
	   return 0;

	return res;
}

//
//	The current, simplistic, approach is to compute the
//	pattern as appearing in the null symbol,
//
//	For a "wild guess", i.e. if we do not have the FIB information
//	available, we first look for the likely startcarrier in the
//	carriers of the null period, and then just try patterns.
//
//	If we know the "mainId" from the FIG0/22, we can try to locate
//	the pattern and compute the C
//
//	To eliminate (reduce?) noise in the input signal, we
//	add a few spectra before computing.
//	We compute two ways, one by just computing the product
//	of the "pattern" with relevant elements from the input,
//	which we then obviously do for all 48 elements to consider,
//	the other way is the FFT approach were taking an FFT/iFFT
//	will give us the "best" guess for the start position
static int cnt	= 0;
int16_t	TII_Detector::find_C (std::complex<float> *v,
	                      int16_t mainId) {
int16_t	i;
int16_t	startCarrier	= theTable [mainId]. carrier;
uint64_t pattern	= theTable [mainId]. pattern;
float	maxCorr		= -1;
int	maxIndex	= -1;
float	avg		= 0;
float	corrTable [48];

	if (mainId < 0)
	   return - 1;

	memcpy (fft_buffer, v, T_u * sizeof (std::complex<float>));
	my_fftHandler. do_FFT ();

	if (cnt == 0)
	   memcpy (theBuffer, fft_buffer, T_u * sizeof (std::complex<float>));
	else
	   for (i = 0; i < T_u; i ++)
	      theBuffer [i] += fft_buffer [i];
	if (++cnt < 2)
	   return -1;
	cnt = 0;

//	We collected two "spectra', and start correlating the 
//	combined spectrum with the pattern.

	for (i = 1; i < 24; i ++)  
	   corrTable [i] = correlate (theBuffer,
	                              startCarrier + 2 * i,
	                              pattern);

	for (i = 1; i < 24; i ++) {
	   avg += corrTable [i];
	   if (corrTable [i] > maxCorr) {
	      maxCorr = corrTable [i];
	      maxIndex = i;
	   }
	}

	avg /= 23;
	if (maxCorr < 2 * avg)
	   maxIndex = -1;

	return maxIndex;
}
//
//
//	It turns out that the location "startIndex + k * 48"
//	and "startIndex + k * 48 + 1" both contain the refTable
//	value from "startIndex + k * 48" (where k is 1 .. 5, determined
//	by the pattern)
//
float	TII_Detector::correlate (std::complex<float> 	*v,
	                         int16_t	startCarrier,
	                         uint64_t	pattern) {
int16_t	realIndex;
int16_t	i;
int16_t	carrier;
float	s1	= 0;

	if (pattern == 0)
	   return 0;

	carrier		= startCarrier;
	realIndex	= T_u - 1 + carrier;
	s1		= abs (real (v [realIndex] *
	                              conj (v [realIndex + 1])));
	for (i = 0; i < 15; i ++) {
	   carrier	+= ((pattern >> 56) & 0xF) * 48;
	   realIndex	= carrier < 0 ? T_u - 1 + carrier : carrier + 1;
	   s1		+= abs (real (v [realIndex] * conj (v [realIndex + 1])));
	   pattern <<= 4;
	}
	
	return s1;
}
//

