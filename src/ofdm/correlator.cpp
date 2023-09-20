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
#include	"correlator.h" 
#include	<QVector>
#include	<cstring>
#include	"radio.h"
#include	<vector>
/**
  *	\class correlator
  *	Implements the correlation that is used to identify
  *	the "first" element (following the cyclic prefix) of
  *	the first non-null block of a frame
  *	The class inherits from the phaseTable.
  */

	correlator::correlator (RadioInterface *mr,
	                                processParams	*p):
	                                     phaseTable (p -> dabMode),
	                                     params (p -> dabMode),
	                                     fft_forward (params. get_T_u (), false),
	                                     fft_backwards (params. get_T_u (), true) {
int32_t	i;
float	Phi_k;

	this	-> response	= p -> responseBuffer;
	this	-> depth	= p -> echo_depth;
	this	-> T_u		= params. get_T_u();
	this	-> T_g		= params. get_T_g();
	this	-> carriers	= params. get_carriers();

	refTable.		resize (T_u);

	framesperSecond		= 2048000 / params. get_T_F();
	displayCounter		= 0;
	
	for (i = 0; i < T_u; i ++)
	   refTable [i] = Complex (0, 0);

	for (i = 1; i <= params. get_carriers() / 2; i ++) {
	   Phi_k =  get_Phi (i);
	   refTable [i] = Complex (cos (Phi_k), sin (Phi_k));
	   Phi_k = get_Phi (-i);
	   refTable [T_u - i] = Complex (cos (Phi_k), sin (Phi_k));
	}

	connect (this, SIGNAL (showCorrelation (int, int, QVector<int>)),
	         mr,   SLOT   (showCorrelation (int, int, QVector<int>)));
}

	correlator::~correlator () {
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

int32_t	correlator::findIndex (std::vector <Complex> v,
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

