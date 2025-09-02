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
#include	"estimator.h" 
#include	<QVector>
#include	<cstring>
#include	"radio.h"
#include	<vector>
/**
  */

	estimator::estimator (RadioInterface *mr,
	                      processParams	*p,
	                      phaseTable 	*theTable) :
	                             params (p -> dabMode),
	                             fft_forward (params. get_T_u (), false),
	                             fft_backwards (params. get_T_u (), true) {

	this	-> theTable	= theTable;
	this	-> T_u		= params. get_T_u();
	this	-> T_g		= params. get_T_g();
	this	-> carriers	= params. get_carriers();
}

	estimator::~estimator () {
}

/**
  */
//
void	estimator::estimate (std::vector <Complex> v,
	                       std::vector<Complex> &CI_Vector) {

	fft_forward. fft (v);
//
//	into the frequency domain, now correlate
	for (int i = 0; i < T_u; i ++) 
//	   v [i] = v [i] * conj (theTable -> refTable [i]);
	   v [i] = v [i] * Complex (real (theTable -> refTable [i]),
	                           -imag (theTable -> refTable [i]));

//	and, again, back into the time domain
	fft_backwards. fft (v);
	for (int i = 0; i < NR_TAPS; i ++)
	   CI_Vector [i] = v [i];
}

