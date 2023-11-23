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
#include	"estimator.h"
#include	<QVector>
#include	<cstring>
#include	"radio.h"
#include	<vector>

#include        <Eigen/QR>
#include        <Eigen/Dense>

static inline
Complex createExp (float s) {
        return Complex (cos (s), - sin (s));
}
//
//	For DAB there is no need to equalize, however, it is
//	(well for me at least) interesting to see what the
//	equalization factor should be. After all, that is
//	another quality measure
//
//	"pilot" range is fftSize / 2 - carriers / 2 .. fftSize / 2 + carriers / 2
bool	estimator::isPilot (int n) {
int low		=  -200;
int high	=  +200;
	return (n != 0) && (low < n) && (n < high) && (n % 6 == 0);
}
		estimator::estimator (RadioInterface *mr,
	                                processParams	*p):
	                                     phaseTable (p -> dabMode),
	                                     params (p -> dabMode),
	                                     fft_forward (params. get_T_u (),
	                                                  false) {
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
//
//	compute a stack of pilots
        numberofPilots		= 0;
	for (int i = -carriers / 2; i < carriers / 2; i ++) {
	   if (isPilot (i)) {
	      numberofPilots ++;
	      pilotTable. push_back (i);
	   }
	}

	numberofTaps		= T_g;
        F_p                     = MatrixXd (numberofPilots, numberofTaps);
        S_p                     = MatrixXd (numberofPilots,
                                                  numberofPilots);
        A_p                     = MatrixXd (numberofPilots, numberofTaps);
        A_p_inv                 = MatrixXd (numberofTaps, numberofPilots);

//
//	S_p is a diagonal matrix with the pilot values as they should be
	for (int row = 0; row < numberofPilots; row ++)
           for (int col = 0; col < numberofPilots; col ++)
              S_p (row, col) = std::complex<float> (0, 0);

	int index	= 0;
        for (int carrier = - carriers / 2; carrier < carriers / 2; carrier ++)
	   if (isPilot (carrier)) {
	      S_p (index, index) = refTable [T_u / 2 + carrier];
	      index ++;
	   }
//
//      F_p is initialized with the precomputed values and is
//      matrix filled with the (pilot, tap) combinations, where for the
//      pilots, their carrier values (relative to 0) are relevant

	for (int pilotIndex = 0; pilotIndex < numberofPilots; pilotIndex ++) {
	   for (int tap = 0; tap < numberofTaps; tap ++)
	      F_p (pilotIndex, tap) =
	         createExp (2 * M_PI *
	             (fftSize / 2 + pilotTable [pilotIndex]) * tap / fftSize) /
	                                         (FLOAT)(sqrt (fftSize));
	}
	A_p	= S_p * F_p;
	A_p_inv = A_p. transpose () * (A_p * A_p. transpose ()). inverse ();
}

	estimator::~estimator () {
}

void	estimator::estimate	(std::vector<Complex> v,
	                             std::vector <Complex> &resultRow) {
Vector  h_td (numberofTaps);
Vector  H_fd (numberofPilots);
Vector  X_p  (numberofPilots);

	resultRow. resize (numberofPilots);
	fft_forward. fft (v);
//
//	Note that the fft result is in the "wrong" order
        for (int index = 0; index < numberofPilots; index ++) {
           X_p (index) = v [(T_u / 2 + pilotTable [index]) % T_u];
	}

//
////    Ok, the matrices are filled, now computing the channelvalues
        h_td	= A_p_inv * X_p;
        H_fd    = F_p * h_td;
//
        for (int index = 0; index < numberofPilots; index ++)
           resultRow [index] = Complex (real (H_fd (index)),
	                                imag (H_fd (index)));
}

