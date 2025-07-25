#
/*
 *    Copyright (C) 2016 .. 2023
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
	                        processParams	*p,
	                        phaseTable 	*theTable) :
	                             params (p -> dabMode),
	                             fft_forward (params. get_T_u (), false),
	                             fft_backwards (params. get_T_u (), true),
	                             response (p -> responseBuffer) {
	                    
//float	Phi_k;

	this	-> theTable	= theTable;
	this	-> depth	= p -> echo_depth;
	this	-> T_u		= params. get_T_u();
	this	-> T_g		= params. get_T_g();
	this	-> carriers	= params. get_carriers();

	framesperSecond		= SAMPLERATE / params. get_T_F();
	displayCounter		= 0;
	connect (this, &correlator::showCorrelation,
	         mr, &RadioInterface::showCorrelation);
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
//
//	Pre v. size () >= T_u
//	Note that "v" is being modified by the fft function
//	that is why it is a value parameter
int32_t	correlator::findIndex (std::vector <Complex> v,
	                       bool firstFound, int threshold ) {
int32_t	i;
int32_t	maxIndex	= -1;
float	sum		= 0;
float	Max		= -1000;
float	lbuf [T_u / 2];

const	int SEARCH_GAP	= 10;
const	int SEARCH_OFFSET = T_g / 2;

	fft_forward. fft (v);
//
//	into the frequency domain, now correlate
	for (i = 0; i < T_u; i ++) 
//	   v [i] = v [i] * conj (theTable -> refTable [i]);
	   v [i] = v [i] * Complex (real (theTable -> refTable [i]),
	                           -imag (theTable -> refTable [i]));

//	and, again, back into the time domain
	fft_backwards. fft (v);
/**
  *	We compute the average and the max signal values
  */
	for (i = 0; i < T_u / 2; i ++) {
	   lbuf [i] = jan_abs (v [i]);
	   sum	+= lbuf [i];
	}

	typedef struct {
	   int index;
	   float Value;
	} corVal;
	std::vector<corVal> workList;
	sum /= T_u / 2;
	QVector<int> indices;

	for (i = T_g - SEARCH_OFFSET; i < T_g + SEARCH_OFFSET; i ++) {
	   if (lbuf [i] / sum > threshold)  {
	      bool foundOne = true;
	      for (int j = 2; (j < SEARCH_GAP) &&
	                              (i + j < T_g + SEARCH_OFFSET); j ++) {
	         if (lbuf [i + j] > lbuf [i]) {
	            foundOne = false;
	            break;
	         }
	      }
	      if (foundOne) {
	         corVal t;
	         t. index = i;
	         t. Value = lbuf [i];
	         workList. push_back (t);
	         if (lbuf [i] > Max){
	            Max = lbuf [i];
	            maxIndex = i;
	         }
	         i += SEARCH_GAP;
	      }
	   }
	}

	if (Max / sum < threshold) {
	   return (- abs (Max / sum) - 1);
	}

	for (uint16_t i = 0; i < workList. size (); i ++)
	   indices. push_back (workList [i]. index);

//	while (workList. size () > 0) {
//	   float Max = 0;
//	   int bestIndex = -1;
//	   for (int i = 0; i < workList. size (); i ++) {
//	      if (workList [i]. Value > Max) {
//	         bestIndex = i;
//	         Max = workList [i]. Value;
//	      }
//	   }
//	   if (bestIndex >= 0) {
//	      indices. push_back (workList [bestIndex]. index);
//	      workList. erase (workList. begin () + bestIndex);
//	   }
//	}

//	for (int i = 0; i < indices. size (); i ++) {
//	   if (T_g - 15 <= indices. at (i) &&
//	                    indices. at (i) <= T_g + 15) {
//	      std::vector<int> temp;
//	      temp. push_back (indices. at (i));
//	      for (int j = 0; j < i; j ++)
//	         temp. push_back (indices. at (j));
//	      for (int j = i + 1; j < indices. size (); j ++)
//	         temp. push_back (indices. at (j));
//	      indices. resize (0);
//	      for (int i = 0; i < (int)(temp. size ()); i ++)
//	         indices. push_back (temp. at (i));
//	      break;
//	   }
//	}
	if (response != nullptr) {
	   if (++displayCounter > framesperSecond / 2) {
	      response	-> putDataIntoBuffer (lbuf, T_u / 2);
	      showCorrelation (T_u / 2, T_g, indices);
	      displayCounter	= 0;
	   }
	}
	if (firstFound)
	   return indices [0];
	return maxIndex;
}

