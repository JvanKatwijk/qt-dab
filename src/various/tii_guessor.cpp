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

#include	"tii_guessor.h"
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
		TII_Guessor::TII_Guessor (uint8_t dabMode):
	                                          params (dabMode),
	                                          my_fftHandler (dabMode) {
int	i;
	this	-> T_u		= params. get_T_u ();
	this	-> carriers	= params. get_carriers ();
	this	-> theBuffer	= new std::complex<float> [T_u];
	fft_buffer		= my_fftHandler. getVector ();	
	window			= new float [T_u];
	for (i = 0; i < T_u; i ++)
	   window [i]  = (0.42 -
                    0.5 * cos (2 * M_PI * (float)i / T_u) +
                    0.08 * cos (4 * M_PI * (float)i / T_u));

}

		TII_Guessor::~TII_Guessor (void) {
	delete []	theBuffer;
	delete []	window;
}

void		TII_Guessor::reset (void) {
	memset (theBuffer, 0, T_u * sizeof (std::complex<float>));
}


void		TII_Guessor:: addBuffer (std::complex<float> *buffer) {
int	i;

	for (i = 0; i < T_u; i ++)
	   fft_buffer [i] = cmul (buffer [i], window [i]);
	my_fftHandler. do_FFT ();
	for (i = 0; i < T_u; i ++)
	   theBuffer [i] += fft_buffer [(T_u / 2 + i) % T_u];
}

bool		TII_Guessor::guess (int16_t *id) {
int i, j;
float	max	= 0;
float	avg	= 0;
int	startCarrier;

	for (i = - carriers / 2; i < - carriers / 4 - 1; i ++)
	   avg += abs (real (theBuffer [T_u / 2 + i] *
	                            conj (theBuffer [T_u / 2 + i + 1])));
	avg /= (carriers / 4);

	for (i = - carriers / 2; i < - carriers / 2 + 4 * 48; i += 2) {
	   int index = T_u / 2 + i;
	   float sum = 0;
	   if (abs (real (theBuffer [index] * conj (theBuffer [index + 1]))) < 5 * avg)
	      continue;
	   for (j = 0; j < 32; j ++) {
	      int ci = index + j * 48;
	      if (ci >= T_u / 2) ci ++;
	      sum += abs (real (theBuffer [ci]* conj (theBuffer [ci + i])));
	   }

	   if (sum > max) {
	      max = sum;
	      startCarrier = i;
	   }
	}

	fprintf (stderr, "estimated startcarrier %d\n", startCarrier);
}
