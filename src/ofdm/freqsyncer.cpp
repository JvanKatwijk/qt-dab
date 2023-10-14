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
#include	"freqsyncer.h" 
#include	<QVector>
#include	<cstring>
#include	"radio.h"
#include	<vector>

	freqSyncer::freqSyncer (RadioInterface *mr,
	                                processParams	*p):
	                                     phaseTable (p -> dabMode),
	                                     params (p -> dabMode),
	                                     fft_forward (params. get_T_u (), false) {
int32_t	i;
float	Phi_k;

	(void)mr;
	this	-> diff_length	= p -> diff_length;
	this	-> diff_length	= 128;
	this	-> T_u		= params. get_T_u();
	this	-> T_g		= params. get_T_g();
	this	-> carriers	= params. get_carriers();
	
	refTable.		resize (T_u);
	phaseDifferences.       resize (diff_length);

	for (i = 0; i < T_u; i ++)
	   refTable [i] = Complex (0, 0);

	for (i = 1; i <= params. get_carriers() / 2; i ++) {
	   Phi_k =  get_Phi (i);
	   refTable [i] = Complex (cos (Phi_k), sin (Phi_k));
	   Phi_k = get_Phi (-i);
	   refTable [T_u - i] = Complex (cos (Phi_k), sin (Phi_k));
	}

//
//      prepare a table for the coarse frequency synchronization
//      can be a static one, actually, we are only interested in
//      the ones with a null
	for (i = 1; i <= diff_length; i ++) 
	   phaseDifferences [i - 1] = abs (arg (refTable [(T_u + i) % T_u] *
	                         conj (refTable [(T_u + i + 1) % T_u])));
	
}

	freqSyncer::~freqSyncer () {
}
//
//
//	an approach that works fine is to correlate the phasedifferences
//	between subsequent carriers
#define	SEARCH_RANGE	(2 * 35)
int16_t	freqSyncer::
	     estimate_CarrierOffset (std::vector<Complex> v) {
int16_t index_1 = 100, index_2 = 100;
float	computedDiffs [SEARCH_RANGE + diff_length + 1];

	fft_forward. fft (v);

	for (int i = T_u - SEARCH_RANGE / 2;
	     i < T_u + SEARCH_RANGE / 2 + diff_length; i ++) {
	   computedDiffs [i - (T_u - SEARCH_RANGE / 2)] =
	      abs (arg (v [i % T_u] *
	                      conj (v [(i + 1) % T_u])));
	}

	float	Mmin	= 1000;
	float	Mmax	= 0;
	for (int i = T_u - SEARCH_RANGE / 2;
	     i < T_u + SEARCH_RANGE / 2; i ++) {
	   float sum	= 0;
	   float sum2	= 0;

	   for (int j = 1; j < diff_length; j ++) {
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

