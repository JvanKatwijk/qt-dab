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
	                                phaseTable	*theTable):
	                                     params (p -> dabMode),
	                                     fft_forward (params. get_T_u (), false),
	                                     go_forward (TEST_SIZE, false),
	                                     go_backwards (TEST_SIZE, true) {
	(void)mr;
	this	-> theTable	= theTable;
	this	-> T_u		= params. get_T_u();
	this	-> T_g		= params. get_T_g();
	this	-> carriers	= params. get_carriers();

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
#define	SEARCH_RANGE	(2 * 32)
//
//	Note: the vector v is being processed, its value is not constant
int16_t	freqSyncer::
	     estimateCarrierOffset (std::vector<Complex> v) {

	fft_forward. fft (v);
//
//	Inspired by https://phys.uri.edu/nigh/NumRec/bookfpdf/f13-2.pdf
//	(and of course using the approach in the time syncing),
//
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

