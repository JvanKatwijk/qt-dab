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
#include	"estimator-2.h" 
#include	<QVector>
#include	<cstring>
#include	"radio.h"
#include	<vector>
#include	"dab-params.h"
#include	"dab-constants.h"
/**
  */
	estimator_2::estimator_2 () :
	                             phaseTable (),
	                             fft_forward (get_T_u (), false),
	                             fft_backwards (get_T_u (), true) {
	this	-> T_u		= get_T_u ();
}

	estimator_2::~estimator_2 () {
}
//
std::vector<Complex> local (2048 * 97);
void	estimator_2::estimate (RingBuffer<Complex> *cirBuffer,
	                       std::vector<Complex> &CI_Vector) {
std::vector<Complex> w (T_u);

	if (cirBuffer -> GetRingBufferReadAvailable () < 2048 * 97)
	   return;

	cirBuffer	-> getDataFromBuffer (local. data (), 2048 * 97);
	for (int i = 0; i < 385; i ++) {
	   memcpy (w. data (), 
	              &local [i * 512], sizeof (Complex) * T_u);
	   fft_forward. fft (w);
	   for (int j = 0; j < T_u; j ++)
	      w [j] = w [j] * conj (refTable [j]);
	   fft_backwards. fft (w);
	   float max = 0;
	   for (int j = 0; j < T_u; j ++) {
	      float x = std::abs (w [j]);
	      if (x > max)
	         max = x;
	   }
	      
	   CI_Vector .push_back (Complex ((float)i / 4, max / 30000.0));
	}
}

