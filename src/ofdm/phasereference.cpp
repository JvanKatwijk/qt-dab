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
/**
  *	\class phaseReference
  *	Implements the correlation that is used to identify
  *	the "first" element (following the cyclic prefix) of
  *	the first non-null block of a frame
  *	The class inherits from the phaseTable.
  */

	phaseReference::phaseReference (uint8_t	dabMode,
	                                int16_t	threshold,
	                                int16_t	diff_length):
	                                     phaseTable (dabMode),
	                                     params (dabMode) {
int32_t	i;
DSPFLOAT	Phi_k;

	this	-> threshold	= threshold;
	this	-> diff_length	= diff_length;
	phasedifferences	= new DSPCOMPLEX [diff_length];
	this	-> T_u		= params. get_T_u ();
	this	-> carriers	= params. get_carriers ();

	refTable		= new DSPCOMPLEX 	[T_u];	//
	fft_processor		= new common_fft 	(T_u);
	fft_buffer		= fft_processor		-> getVector ();
	res_processor		= new common_ifft 	(T_u);
	res_buffer		= res_processor		-> getVector ();
	
	memset (refTable, 0, sizeof (DSPCOMPLEX) * T_u);

	for (i = 1; i <= params. get_carriers () / 2; i ++) {
	   Phi_k =  get_Phi (i);
	   refTable [i] = DSPCOMPLEX (cos (Phi_k), sin (Phi_k));
	   Phi_k = get_Phi (-i);
	   refTable [T_u - i] = DSPCOMPLEX (cos (Phi_k), sin (Phi_k));
	}
//
//	prepare a table for the coarse frequency synchronization
//	can be a static one
	for (i = 1; i <= diff_length; i ++) 
	   phasedifferences [i - 1] = refTable [(T_u + i) % T_u] *
	                              conj (refTable [(T_u + i + 1) % T_u]);

//	for (i = 0; i < diff_length; i ++)
//	   fprintf (stderr, "%f ", abs (arg (phasedifferences [i])));
//	fprintf (stderr, "\n");
}

	phaseReference::~phaseReference (void) {
	delete		fft_processor;
	delete []	refTable;
	delete []	phasedifferences;
}

/**
  *	\brief findIndex
  *	the vector v contains "Tu" samples that are believed to
  *	belong to the first non-null block of a DAB frame.
  *	We correlate the data in this vector with the predefined
  *	data, and if the maximum exceeds a threshold value,
  *	we believe that that indicates the first sample we were
  *	looking for.
  */

int32_t	phaseReference::findIndex (DSPCOMPLEX *v) {
int32_t	i;
int32_t	maxIndex	= -1;
float	sum		= 0;
float	Max		= -1000;

	memcpy (fft_buffer, v, T_u * sizeof (DSPCOMPLEX));
	fft_processor -> do_FFT ();
//
//	into the frequency domain, now correlate
	for (i = 0; i < T_u; i ++) 
	   res_buffer [i] = fft_buffer [i] * conj (refTable [i]);
//	and, again, back into the time domain
	res_processor	-> do_IFFT ();
/**
  *	We compute the average signal value ...
  */
	for (i = 0; i < T_u; i ++)
	   sum	+= abs (res_buffer [i]);
	for (i = 0; i < T_u; i ++)
	   if (abs (res_buffer [i]) > Max) {
	      maxIndex = i;
	      Max = abs (res_buffer [i]);
	   }
/**
  *	that gives us a basis for defining the threshold
  */
	if (Max < threshold * sum / T_u)
	   return  - abs (Max * T_u / sum) - 1;
	else {
	   return maxIndex;	
	}
}

//	We investigate a sequence of phasedifferences that
//	are known starting at real carrier 0.
//	Note that due to phases being in a modulo system,
//	plain correlation (i.e. sum (x, y, i) does not work well,
//	so we just compute the phasedifference between phasedifferences
//	as measured and as they should be. To be on the safe side
//	the difference is the difference of the absolute phasedifference
//	values.
//	In previous versions we looked
//	at the "weight" of the positive and negative carriers in the
//	fft, but that did not work too well.
#define	SEARCH_RANGE	(2 * 35)
int16_t	phaseReference::estimateOffset (DSPCOMPLEX *v) {
int16_t	i, j, index = 100;

	memcpy (fft_buffer, v, T_u * sizeof (DSPCOMPLEX));
	fft_processor	-> do_FFT ();

	int Mmin	= 1000;
	for (i = T_u - SEARCH_RANGE / 2; i < T_u + SEARCH_RANGE / 2; i ++) {
	   float diff = 0;
	   for (j = 0; j < diff_length; j ++) {
	      int16_t ind1 = (i + j + 1) % T_u;
	      int16_t ind2 = (i + j + 2) % T_u;
	      DSPCOMPLEX pd = fft_buffer [ind1] * conj (fft_buffer [ind2]);
	      diff += abs (arg (pd * conj (phasedifferences [j])));
	   }
	   if (diff < Mmin) {
	      Mmin = diff;
	      index = i;
	   }
	}
	return index - T_u;
}

