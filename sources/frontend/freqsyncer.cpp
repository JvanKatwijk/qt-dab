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
	                                bool		speedUp,
	                                uint8_t		correlator):
	                                     params (p -> dabMode),
	                                     fft_forward (params. get_T_u (), false),
	                                     go_forward (TEST_SIZE, false),
	                                     go_backwards (TEST_SIZE, true) {
	(void)mr;
	this	-> speedUp	= speedUp;
	this	-> correlator	= correlator;
//	this	-> diff_length	= p -> diff_length;
	this	-> diff_length	= 128;
	this	-> theTable	= theTable;
	this	-> T_u		= params. get_T_u();
	this	-> T_g		= params. get_T_g();
	this	-> carriers	= params. get_carriers();

	phaseDifferences.       resize (diff_length);
//      prepare a table for the coarse frequency synchronization
//      can be a static one, actually, we are only interested in
//      the ones with a null
	for (int i = 1; i <= diff_length; i ++) 
	   phaseDifferences [i - 1] =
	        abs (arg (theTable -> refTable [(T_u + i) % T_u] *
	             conj (theTable -> refTable [(T_u + i + 1) % T_u])));
//	first of all: compute the phases
	for (int i = -TEST_SIZE / 2; i < TEST_SIZE / 2; i ++) {
	   t1 [TEST_SIZE / 2 + i] = theTable -> refTable [(T_u + i) % T_u] *
	                 conj (theTable -> refTable [(T_u + i + 1) % T_u]);
//	   t1 [TEST_SIZE / 2 + i] = Complex (arg (t1 [i]), 0);
	}
	go_forward. fft (t1);
}

	freqSyncer::~freqSyncer () {
}
//
//	an approach that works fine is to correlate the phasedifferences
//	between subsequent carriers
//	We start with an estimate of the offset by looking at the 
//	best fit of begin and end segment of the carrier sequence
//	from the input to the carrier sequence in the predefined
//	null symbol
#define	SEARCH_RANGE	(2 * 32)
//
//	Note: the vector v is being processed, its value is not constant
int16_t	freqSyncer::
	     estimate_CarrierOffset (std::vector<Complex> v) {

	fft_forward. fft (v);
//
//	The coarse frequency offset is computed by looking at the
//	phase differences of subsequenct carriers in the
//	predefined data vs the incoming symbol 0.
//	The current approach is pretty simple, we just look at the
//	low values (phase 0) and the higher values on the positions
//	defined by the predefined data.
//	A simple optimization was to just make a (not too wild) guess
//	of the position of the NULL carrier, and applying the
//	search on just a few carriers around this assumed null carrier.
//
//	Willem S suggested to apply a more "scientific" approach to
//	compute the correlation between the phases, required for
//	computing the coarse frequency offset.
//	Inspired by https://phys.uri.edu/nigh/NumRec/bookfpdf/f13-2.pdf
//	(and of course using the approach in the time syncing),
//	I experimented with different "test" sizes and, while testsize 128
//	works pretty well, it underperforms compared to the more basic
//	approach that consists of counting high and low values
//
	if (correlator == FFT_CORR) {
	   for (int i = -TEST_SIZE / 2; i < TEST_SIZE / 2; i ++) {
	      t2 [TEST_SIZE / 2 + i] = v [(T_u + i) % T_u] *
	                    conj (v [(T_u + i + 1) % T_u]);
//	   t2 [TEST_SIZE / 2 +i] = Complex (arg (t2 [i]), 0);
	   }
//	apply the FFT
	   go_forward. fft (t2);
	   for (int i = 0; i < TEST_SIZE; i ++)
	      t2 [i] = t1 [i] * conj (t2 [i]);
//	and go back
	   go_backwards. fft (t2);
	   int theCarrier	= -1000;
	   float mm	= -0;
	   for (int i = -SEARCH_RANGE / 2 ; i < SEARCH_RANGE / 2; i ++) {
	      if (abs (t2 [(TEST_SIZE + i) % TEST_SIZE]) > mm) {
	         mm = abs (t2 [(TEST_SIZE + i) % TEST_SIZE]);
	         theCarrier = i;
	      }
	   }
	   if ((theCarrier == -SEARCH_RANGE) || (theCarrier == SEARCH_RANGE))
	      return 100;
	   return theCarrier;
	}
	else {
	   int16_t index_1 = 100, index_2 = 100;
	   int starter_1	= - SEARCH_RANGE / 2;	// the default
	   float	computedDiffs [SEARCH_RANGE + diff_length + 1];
	   float	test [T_u];
	   for (int i = 0; i < T_u / 2; i ++) {
	      test [i]		= jan_abs (v [T_u / 2 + i]);
	      test [T_u / 2 + i]	= jan_abs (v [i]);
	   }
#define	SUM_SIZE	75
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
}

