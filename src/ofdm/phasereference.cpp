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
 */
#include	"phasereference.h" 
#include	"string.h"
#include	"radio.h"
/**
  *	\class phaseReference
  *	Implements the correlation that is used to identify
  *	the "first" element (following the cyclic prefix) of
  *	the first non-null block of a frame
  *	The class inherits from the phaseTable.
  */

	phaseReference::phaseReference (RadioInterface *mr,
	                                uint8_t		dabMode,
#ifdef	IMPULSE_RESPONSE
	                                RingBuffer<float> *b,
#endif
	                                int16_t		threshold,
	                                int16_t		diff_length):
	                                     phaseTable (dabMode),
	                                     params (dabMode),
	                                     my_fftHandler (dabMode) {
int32_t	i;
float	Phi_k;

#ifdef	IMPULSE_RESPONSE
	this	-> response	= b;
#endif
	this	-> threshold	= threshold;
	this	-> diff_length	= diff_length;
	this	-> T_u		= params. get_T_u ();
	this	-> carriers	= params. get_carriers ();

	refTable.		resize (T_u);
	phaseDifferences.	resize (diff_length);
	fft_buffer		= my_fftHandler. getVector ();

	framesperSecond		= 2048000 / params. get_T_F ();
	displayCounter		= 0;

	memset (refTable. data (), 0, sizeof (std::complex<float>) * T_u);

	for (i = 1; i <= params. get_carriers () / 2; i ++) {
	   Phi_k =  get_Phi (i);
	   refTable [i] = std::complex<float> (cos (Phi_k), sin (Phi_k));
	   Phi_k = get_Phi (-i);
	   refTable [T_u - i] = std::complex<float> (cos (Phi_k), sin (Phi_k));
	}
//
//	prepare a table for the coarse frequency synchronization
//	can be a static one, actually, we are only interested in
//	the ones with a null
	for (i = 1; i <= diff_length; i ++) 
	   phaseDifferences [i - 1] = abs (arg (refTable [(T_u + i) % T_u] *
                                 conj (refTable [(T_u + i + 1) % T_u])));
#ifdef	IMPULSE_RESPONSE
	connect (this, SIGNAL (showImpulse (int)),
	         mr,   SLOT   (showImpulse (int)));
#endif
}

	phaseReference::~phaseReference (void) {
}

/**
  *	\brief findIndex
  *	the vector v contains "T_u" samples that are believed to
  *	belong to the first non-null block of a DAB frame.
  *	We correlate the data in this vector with the predefined
  *	data, and if the maximum exceeds a threshold value,
  *	we believe that that indicates the first sample we were
  *	looking for.
  */

int32_t	phaseReference::findIndex (std::vector <std::complex<float>> v) {
int32_t	i;
int32_t	maxIndex	= -1;
int32_t	oldIndex	= -1;
float	sum		= 0;
float	Max		= -1000;
float	lbuf [T_u];

	memcpy (fft_buffer, v. data (), T_u * sizeof (std::complex<float>));
	my_fftHandler. do_FFT ();
//
//	into the frequency domain, now correlate
	for (i = 0; i < T_u; i ++) 
	   fft_buffer [i] *= conj (refTable [i]);
//	and, again, back into the time domain
	my_fftHandler. do_IFFT ();
/**
  *	We compute the average and the max signal values
  */
	for (i = 0; i < T_u; i ++) {
	   lbuf [i] = jan_abs (fft_buffer [i]);
	   sum	+= lbuf [i];
	   if (lbuf [i] > Max) {
	      maxIndex = i;
	      Max = lbuf [i];
	   }
	}

#ifdef	IMPULSE_RESPONSE
	if (++displayCounter > framesperSecond / 4) {
	   response	-> putDataIntoBuffer (lbuf, T_u);
	   showImpulse (T_u);
	   displayCounter	= 0;
	}
#endif
/**
  *	that gives us a basis for defining the actual threshold value
  */
	if (Max < threshold * sum / T_u)
	   return  - abs (Max * T_u / sum) - 1;
	else {
	   return maxIndex;	
	}
}

//	We investigate a sequence of phasedifferences that
//	are known starting at real carrier 0.
//	Phase of the carriers of the "real" block 0 may be
//	quite different than the phase of the carriers of the "reference"
//	block, plain correlation (i.e. sum (x, y, i) does not work well.
//	What is a good measure though is looking at the phase differences
//	between successive carriers in both the "real" block and the
//	reference block. These should be more or less the same.
//	So we just compute the phasedifference between phasedifferences
//	as measured and as they should be.
//	To keep things simple, we just look at the locations where
//	the phasedifference with the successor should be 0
//	In previous versions we looked
//	at the "weight" of the positive and negative carriers in the
//	fft, but that did not work too well.
#define	SEARCH_RANGE	(2 * 35)
int16_t	phaseReference::estimate_CarrierOffset (std::vector<std::complex<float>> v) {
int16_t	i, j, index = 100;
float	diff;
float	computedDiffs [SEARCH_RANGE + diff_length + 1];
int	index_1	= 0;

	memcpy (fft_buffer, v. data (), T_u * sizeof (std::complex<float>));
	my_fftHandler. do_FFT ();

	for (i = T_u - SEARCH_RANGE / 2;
	     i < T_u + SEARCH_RANGE / 2 + diff_length; i ++) 
	   computedDiffs [i - (T_u - SEARCH_RANGE / 2)] =
	      abs (arg (fft_buffer [i % T_u] * conj (fft_buffer [(i + 1) % T_u])));

	float	Mmin = 1000;
	for (i = T_u - SEARCH_RANGE /2;
	     i < T_u + SEARCH_RANGE / 2; i ++) {
	   float sum = 0;

	   for (j = 1; j < diff_length; j ++)
	      if (phaseDifferences [j - 1] < 0.1)
	         sum += computedDiffs [i - (T_u - SEARCH_RANGE / 2) + j];
	   if (sum < Mmin) {
	      Mmin = sum;
	      index = i;
	   }
	}
	
	return index - T_u; 
}
//
//	NOT USED, just for some tests
//	An alternative way to compute the small frequency offset
//	is to look at the phase offset of subsequent carriers
//	in block 0, compared to the values as computed from the
//	reference block.
//	The values are reasonably close to the values computed
//	on the fly
#define	LLENGTH	100
float	phaseReference::estimate_FrequencyOffset (std::vector <std::complex<float>> v) {
int16_t	i, j;
float pd	= 0;

	for (i = - LLENGTH / 2 ; i < LLENGTH / 2; i ++) {
	   std::complex<float> a1 = refTable [(T_u + i) % T_u] * conj (refTable [(T_u + i + 1) % T_u]);
	   std::complex<float> a2 = fft_buffer [(T_u + i) % T_u] * conj (fft_buffer [(T_u + i + 1) % T_u]);
	   pd += arg (a2) - arg (a1);
	}
	return pd / LLENGTH;
}

