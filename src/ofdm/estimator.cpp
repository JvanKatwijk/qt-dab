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
#include	"estimator.h"
#include	<QVector>
#include	<cstring>
#include	"radio.h"
#include	<vector>

//
		estimator::estimator (RadioInterface *mr,
	                                processParams	*p):
	                                     phaseTable (p -> dabMode),
	                                     params (p -> dabMode),
	                                     fft_forward (params. get_T_u (),
	                                                  false),
	                                     fft_backwards (params. get_T_u (),
	                                                    true) {
float	Phi_k;
Complex temp [params. get_T_u ()];
	(void)mr;
	this	-> T_u		= params. get_T_u();
	this	-> T_g		= params. get_T_g();
	this	-> carriers	= params. get_carriers();
	fftSize			= T_u;
	refTable.		resize (T_u);

//
//	The reftanle takes the role of the pilots, i.e. we
//	know what there value should be
	for (int i = 0; i < T_u; i ++)
	   refTable [i] = std::complex<float> (0, 0);
//
//	The reference values
	for (int i = 1; i <= carriers / 2; i ++) {
	   Phi_k =  get_Phi (i);
	   temp [i] = Complex (cos (Phi_k), sin (Phi_k));
	   Phi_k = get_Phi (-i);
	   temp [T_u - i] = std::complex<float> (cos (Phi_k), sin (Phi_k));
	}
//
//	and organized as -inf ..0 .. inf
	for (int i = 0; i < T_u; i ++) {
	   refTable [i] = temp [(T_u / 2 + i) % T_u];
	}
}

	estimator::~estimator () {
}

void	estimator::estimate	(std::vector<Complex> v,
	                             std::vector <Complex> &resultRow) {

	resultRow. resize (NR_TAPS);
	fft_forward. fft (v);
	Complex hhh [T_u];
	for (int i = 0; i < T_u; i ++)
	   hhh [i] = Complex (0, 0);
	for (int i = - carriers / 2; i < carriers / 2; i ++) {
	   int index = (T_u  + i) % T_u;
	   if (index != 0)
	      hhh [T_u / 2 + i] =
	                     v [index] / refTable [T_u / 2 + i];
	}
	fft_backwards. fft (hhh);
//
	for (int i = 0; i < NR_TAPS - 4; i ++)
	   resultRow [i + 4 ] = hhh [i] * 10.0;
}

