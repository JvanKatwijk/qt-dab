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
#include	<cstring>
#include	"radio.h"
#include	<vector>
/**
  *	\class phaseReference
  *	Implements the correlation that is used to identify
  *	the "first" element (following the cyclic prefix) of
  *	the first non-null block of a frame
  *	The class inherits from the phaseTable.
  */

	phaseReference::phaseReference (RadioInterface *mr,
	                                uint8_t		dabMode,
	                                int16_t		threshold,
	                                int16_t		diff_length,
	                                int16_t		depth,
	                                RingBuffer<float> *b):
	                                     phaseTable (dabMode),
	                                     params (dabMode),
	                                     my_fftHandler (dabMode) {
int32_t	i;
float	Phi_k;

	this	-> response	= b;
	this	-> threshold	= threshold;
	this	-> diff_length	= diff_length;
	this	-> depth	= depth;
	this	-> T_u		= params. get_T_u();
	this	-> T_g		= params. get_T_g();
	this	-> carriers	= params. get_carriers();
	
	refTable.		resize (T_u);
	phaseDifferences.       resize (diff_length);
	fft_buffer		= my_fftHandler. getVector();

	framesperSecond		= 2048000 / params. get_T_F();
	displayCounter		= 0;
	
	for (i = 0; i < T_u; i ++)
	   refTable [i] = std::complex<float> (0, 0);

	for (i = 1; i <= params. get_carriers() / 2; i ++) {
	   Phi_k =  get_Phi (i);
	   refTable [i] = std::complex<float> (cos (Phi_k), sin (Phi_k));
	   Phi_k = get_Phi (-i);
	   refTable [T_u - i] = std::complex<float> (cos (Phi_k), sin (Phi_k));
	}

//
//      prepare a table for the coarse frequency synchronization
//      can be a static one, actually, we are only interested in
//      the ones with a null
	for (i = 1; i <= diff_length; i ++)
	   phaseDifferences [i - 1] = abs (arg (refTable [(T_u + i) % T_u] *
	                         conj (refTable [(T_u + i + 1) % T_u])));

	connect (this, SIGNAL (showImpulse (int)),
	         mr,   SLOT   (showImpulse (int)));
	connect (this, SIGNAL (showIndex   (int)),
	         mr,   SLOT   (showIndex   (int)));
}

	phaseReference::~phaseReference() {
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

int32_t	phaseReference::findIndex (std::vector <std::complex<float>> v,
	                           int threshold ) {
int32_t	i;
int32_t	maxIndex	= -1;
float	sum		= 0;
float	Max		= -1000;
float	lbuf [T_u / 2];
float	mbuf [T_u / 2];
std::vector<int> resultVector;

	memcpy (fft_buffer, v. data(), T_u * sizeof (std::complex<float>));
	my_fftHandler. do_FFT();
//
//	into the frequency domain, now correlate
	for (i = 0; i < T_u; i ++) 
	   fft_buffer [i] *= conj (refTable [i]);
//	and, again, back into the time domain
	my_fftHandler. do_IFFT();
/**
  *	We compute the average and the max signal values
  */
	for (i = 0; i < T_u / 2; i ++) {
	   lbuf [i] = jan_abs (fft_buffer [i]);
	   mbuf [i] = lbuf [i];
	   sum	+= lbuf [i];
	}
	

	sum /= T_u / 2;
//	
	for (i = 0; i < 100; i ++) {
	   if (lbuf [T_g - 80 + i] > Max) {
	      maxIndex = T_g - 80 + i;
	      Max = lbuf [T_g - 80 + i];
	   }
	}

	if (Max / sum < threshold) {
	   return (- abs (Max / sum) - 1);
	}
	else 
	   resultVector. push_back (maxIndex);	

	if (threshold <= 5)	//	
	   return maxIndex;
	for (int k = 0; k < depth; k ++) {
	   float MMax = 0;
	   int	lIndex = -1;
	   for (i = T_g - 100; i < T_u / 2 - 200; i ++) {
	      if (lbuf [i] > MMax) {
	         MMax = lbuf [i];
	         lIndex = i;
	      }
	   }
	   if (MMax < Max / 1.4)
	      break;
	   if (lIndex > 0) {
	      resultVector . push_back (lIndex);
	      for (i = lIndex - 15; i < lIndex + 15; i ++)
	         lbuf [i] = 0;
	   }
	   else
	      break;
	}

	if (response != nullptr) {
	   if (++displayCounter > framesperSecond / 4) {
	      response	-> putDataIntoBuffer (mbuf, T_u / 2);
	      showImpulse (T_u / 2);
	      displayCounter	= 0;
	      if (resultVector. at (0) > 0) {
	         showIndex (-1);
	         for (i = 1; i < (int)(resultVector. size()); i ++)
	            showIndex (resultVector. at (i));
	         showIndex (0);
	      }
	   }
	}
	return resultVector. at (0);
}
//
//
//	an approach that works fine is to correlate the phasedifferences
//	between subsequent carriers
#define	SEARCH_RANGE	(2 * 35)
int16_t	phaseReference::estimate_CarrierOffset (std::vector<std::complex<float>> v) {
int16_t	i, j, index = 100;
float	computedDiffs [SEARCH_RANGE + diff_length + 1];

	memcpy (fft_buffer, v. data(), T_u * sizeof (std::complex<float>));
	my_fftHandler. do_FFT();

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
//	NOT USED, just for some tests
//	An alternative way to compute the small frequency offset
//	is to look at the phase offset of subsequent carriers
//	in block 0, compared to the values as computed from the
//	reference block.
//	The values are reasonably close to the values computed
//	on the fly
#define	LLENGTH	100
float	phaseReference::estimate_FrequencyOffset (std::vector <std::complex<float>> v) {
int16_t	i;
float pd	= 0;

	for (i = - LLENGTH / 2 ; i < LLENGTH / 2; i ++) {
	   std::complex<float> a1 = refTable [(T_u + i) % T_u] * conj (refTable [(T_u + i + 1) % T_u]);
	   std::complex<float> a2 = fft_buffer [(T_u + i) % T_u] * conj (fft_buffer [(T_u + i + 1) % T_u]);
	   pd += arg (a2) - arg (a1);
	}
	return pd / LLENGTH;
}

