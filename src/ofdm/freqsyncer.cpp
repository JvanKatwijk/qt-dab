#
/*
 *    Copyright (C) 2014 .. 2023
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
#include	"freqsyncer.h" 
#include	<QVector>
#include	<cstring>
#include	"radio.h"
#include	<vector>

	freqSyncer::freqSyncer (RadioInterface *mr,
	                                processParams	*p,
	                                phaseTable	*theTable,
	                                bool		speedUp):
	                                     params (p -> dabMode),
	                                     fft_forward (params. get_T_u (), false) {
int32_t	i;
//float	Phi_k;

	(void)mr;
	this	-> speedUp	= speedUp;
	this	-> diff_length	= p -> diff_length;
	this	-> diff_length	= 128;
	this	-> T_u		= params. get_T_u();
	this	-> T_g		= params. get_T_g();
	this	-> carriers	= params. get_carriers();
	
	phaseDifferences.       resize (diff_length);

//      prepare a table for the coarse frequency synchronization
//      can be a static one, actually, we are only interested in
//      the ones with a null
	for (i = 1; i <= diff_length; i ++) 
	   phaseDifferences [i - 1] =
	        abs (arg (theTable -> refTable [(T_u + i) % T_u] *
	             conj (theTable -> refTable [(T_u + i + 1) % T_u])));
}

	freqSyncer::~freqSyncer () {
}
//
//	an approach that works fine is to correlate the phasedifferences
//	between subsequent carriers
#define	SEARCH_RANGE	(2 * 35)
int16_t	freqSyncer::
	     estimate_CarrierOffset (std::vector<Complex> v) {
int16_t index_1 = 100, index_2 = 100;
int starter_1	= - SEARCH_RANGE / 2;	// the default
float	computedDiffs [SEARCH_RANGE + diff_length + 1];
//
	fft_forward. fft (v);

float	test [T_u];
	for (int i = 0; i < T_u / 2; i ++) {
	   test [i]		= jan_abs (v [T_u / 2 + i]);
	   test [T_u / 2 + i]	= jan_abs (v [i]);
	}
#define	SUM_SIZE	50
	if (speedUp) {
//	   We look with a moving sum  of the first and the last
//	   SUM_SIZE carriers to a full sequence of "carrier" carriers
//	   for a maximum.
//	   if found, we have a rough estimate of where the NULL carrier is
	   double xx	= 0;
	   double max	= 0;
	   for (int j = - SEARCH_RANGE / 2;
	         j < -SEARCH_RANGE / 2 + SUM_SIZE; j ++) {
	      xx += test [T_u / 2 - carriers / 2 + j];
	      xx += test [T_u / 2 + carriers / 2 - SUM_SIZE + j];
	   }
	   for (int i = -SEARCH_RANGE / 2; i < SEARCH_RANGE / 2; i ++) {
	      xx -= test [T_u / 2 - carriers / 2  + i];
	      xx += test [T_u / 2 - carriers / 2  + i + SUM_SIZE];
	      xx -= test [T_u / 2 + (carriers / 2 - SUM_SIZE) + i];
	      xx += test [T_u / 2 + (carriers / 2 - SUM_SIZE) + i + SUM_SIZE];
	      if (xx > max) {
	         max = xx;
	         starter_1 = i;
	      }
	   }
	}

//	the actual search for the best fit of the incoming data wrt
//	the predefined data is done over a range of app 20 carriers
	if (starter_1 - 10 < - SEARCH_RANGE / 2)
	   starter_1 = - SEARCH_RANGE / 2;
	for (int i = T_u - SEARCH_RANGE / 2;
	     i < T_u + SEARCH_RANGE / 2 + diff_length; i ++) {
	   computedDiffs [i - (T_u - SEARCH_RANGE / 2)] =
	      jan_abs (arg (v [i % T_u] *
	                      conj (v [(i + 1) % T_u])));
	}

	float	Mmin	= 1000;
	float	Mmax	= 0;
	int	length	= speedUp ? 20 : SEARCH_RANGE;
	for (int i = T_u + starter_1;
	     i < T_u + starter_1 + length; i ++) {
	   float sum1	= 0;
	   float sum2	= 0;
//
//	Since correlation with lots of zeros in the reference
//	vector does not make much sense (the reference values
//	are 0, PI/ 2 and PI).
//	We therefore compute the sum of the values that should be 0,
//	and the sum of the values that should be either PI / 2 or PI
//	and compute the minimum resp max value (and index)
//	If the indices are the same, we believe we found a solution
	   for (int j = 1; j < diff_length; j ++) {
	      if (phaseDifferences [j - 1] < 0.1) {
	         sum1 += computedDiffs [i - (T_u - SEARCH_RANGE / 2) + j];
	      }
	      if (phaseDifferences [j - 1] > M_PI / 2 - 0.1) {
	         sum2 += computedDiffs [i - (T_u - SEARCH_RANGE / 2) + j];
	      }
	   }
	   if (sum1 < Mmin) {
	      Mmin = sum1;
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

