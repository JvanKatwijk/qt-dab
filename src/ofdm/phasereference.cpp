#
/*
 *    Copyright (C) 2014 .. 2017
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
#include	"phasereference.h" 
#include	<QVector>
#include	<cstring>
#include	"radio.h"
#include	<vector>
#ifdef	__WITH_JAN__
#include	"channel.h"
#endif
/**
  *	\class phaseReference
  *	Implements the correlation that is used to identify
  *	the "first" element (following the cyclic prefix) of
  *	the first non-null block of a frame
  *	The class inherits from the phaseTable.
  */

#define	PILOTS	100
#define	TAPS	100

	phaseReference::phaseReference (RadioInterface *mr,
	                                processParams	*p):
	                                     phaseTable (p -> dabMode),
	                                     params (p -> dabMode),
	                                     fft_forward (params. get_T_u (), false),
	                                     fft_backwards (params. get_T_u (), true) {
int32_t	i;
float	Phi_k;

	this	-> response	= p -> responseBuffer;
	this	-> diff_length	= p -> diff_length;
	this	-> diff_length	= 128;
	this	-> depth	= p -> echo_depth;
	this	-> T_u		= params. get_T_u();
	this	-> T_g		= params. get_T_g();
	this	-> carriers	= params. get_carriers();
	
	refTable.		resize (T_u);
	phaseDifferences.       resize (diff_length);

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
	
	connect (this, SIGNAL (showCorrelation (int, int, QVector<int>)),
	         mr,   SLOT   (showCorrelation (int, int, QVector<int>)));

#ifdef	__WITH_JAN__
	theEstimator	= new channel (refTable, PILOTS, TAPS);
#endif
}

	phaseReference::~phaseReference () {
#ifdef	__WITH_JAN__
	delete theEstimator;
#endif
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

	fft_forward. fft (v);
//
//	into the frequency domain, now correlate
	for (i = 0; i < T_u; i ++) 
	   v [i] = v [i] * conj (refTable [i]);

//	and, again, back into the time domain
	fft_backwards. fft (v);
/**
  *	We compute the average and the max signal values
  */
	for (i = 0; i < T_u / 2; i ++) {
	   lbuf [i] = jan_abs (v[i]);
	   sum	+= lbuf [i];
	}

	sum /= T_u / 2;
	QVector<int> indices;

	for (i = T_g - 250; i < T_g + 250; i ++) {
	   if (lbuf [i] / sum > threshold)  {
	      bool foundOne = true;
	      for (int j = 1; (j < 10) && (i + j < T_g + 250); j ++) {
	         if (lbuf [i + j] > lbuf [i]) {
	            foundOne = false;
	            break;
	         }
	      }
	      if (foundOne) {
	         indices. push_back (i);
	         if (lbuf [i]> Max){
	            Max = lbuf [i];
	            maxIndex = i;
	         }
	         i += 10;
	      }
	   }
	}

	if (Max / sum < threshold) {
	   return (- abs (Max / sum) - 1);
	}

	if (response != nullptr) {
	   if (++displayCounter > framesperSecond / 2) {
	      response	-> putDataIntoBuffer (lbuf, T_u / 2);
	      showCorrelation (T_u / 2, T_g, indices);
	      displayCounter	= 0;
	   }
	}
	
	return maxIndex;
}
//
//
//	an approach that works fine is to correlate the phasedifferences
//	between subsequent carriers
#define	SEARCH_RANGE	(2 * 35)
int16_t	phaseReference::
	     estimate_CarrierOffset (std::vector<std::complex<float>> v) {
int16_t	i, j, index_1 = 100, index_2 = 100;
float	computedDiffs [SEARCH_RANGE + diff_length + 1];

	fft_forward. fft (v);

	for (i = T_u - SEARCH_RANGE / 2;
	     i < T_u + SEARCH_RANGE / 2 + diff_length; i ++) {
	   computedDiffs [i - (T_u - SEARCH_RANGE / 2)] =
	      abs (arg (v [i % T_u] *
	                      conj (v [(i + 1) % T_u])));
	}

	float	Mmin	= 1000;
	float	Mmax	= 0;
	for (i = T_u - SEARCH_RANGE / 2;
	     i < T_u + SEARCH_RANGE / 2; i ++) {
	   float sum	= 0;
	   float sum2	= 0;

	   for (j = 1; j < diff_length; j ++) {
	      if (phaseDifferences [j - 1] < 0.1) {
	         sum += computedDiffs [i - (T_u - SEARCH_RANGE / 2) + j];
	      }
	      if (phaseDifferences [j - 1] > M_PI - 0.1) {
	         sum2 += computedDiffs [i - (T_u - SEARCH_RANGE / 2) + j];
	      }
	   }
	   if (sum < Mmin) {
	      Mmin = sum;
	      index_1 = i;
	   }
	   if (sum2 > Mmax) {
	      Mmax = sum2;
	      index_2 = i;
	   }
	}

	if (index_1 != index_2)
	   return 100;
	return index_1 - T_u; 
}

float	phaseReference::phase (std::vector<complex<float>> &v, int Ts) {
std::complex<float> sum = std::complex<float> (0, 0);

	for (int i = 0; i < Ts; i ++)
	   sum += v [i];

	return arg (sum);
}

#ifdef	__WITH_JAN__
void	phaseReference::estimate	(std::vector<std::complex<float>> v) {
std::complex<float> h_td [TAPS];

	fft_forward. fft (v);
	theEstimator -> estimate (v, h_td);

//	float	Tau		= 0;
//	float	teller		= 0;
//	float	noemer		= 0;
//	for (int i = - TAPS / 2; i < TAPS / 2; i ++) {
//	   float h_ts = abs (h_td [TAPS / 2 + i]) * abs (h_td [TAPS / 2 + i]);
//	   teller += i * h_ts;
//	   noemer += h_ts;
//	}
//	Tau	= teller / noemer;
//	teller	= 0;
//	noemer	= 0;
//
//	for (int i = -TAPS / 2; i < TAPS / 2; i ++) {
//	   float h_ts = abs (h_td [TAPS / 2 + i]) * abs (h_td [TAPS / 2 + i]);
//	   teller += (i - Tau) * (i - Tau) * h_ts;
//	   noemer += h_ts;
//	}
//	
//	fprintf (stderr, "Tau = %f, rms delay spread %f\n", Tau,
//	                                                teller / noemer);
}

#endif
