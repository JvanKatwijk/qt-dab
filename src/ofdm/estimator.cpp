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

	estimator::estimator (RadioInterface *mr,
	                                processParams	*p):
	                                     phaseTable (p -> dabMode),
	                                     params (p -> dabMode) {
int32_t	i;
float	Phi_k;

	this	-> T_u		= params. get_T_u();
	this	-> T_g		= params. get_T_g();
	this	-> carriers	= params. get_carriers();
	fftSize			= T_u;
	fft_forward		= new fftHandler (T_u, false);
	refTable.		resize (T_u);
	
	for (i = 0; i < T_u; i ++)
	   refTable [i] = std::complex<float> (0, 0);

	for (i = 1; i <= params. get_carriers() / 2; i ++) {
	   Phi_k =  get_Phi (i);
	   refTable [i] = std::complex<float> (cos (Phi_k), sin (Phi_k));
	   Phi_k = get_Phi (-i);
	   refTable [T_u - i] = std::complex<float> (cos (Phi_k), sin (Phi_k));
	}

        numberofPilots		= T_g;
        numberofTaps		= T_g;
        F_p                     = MatrixXd (numberofPilots, numberofTaps);
        S_p                     = MatrixXd (numberofPilots,
                                                  numberofPilots);
        S_pxF_p                 = MatrixXd (numberofPilots, numberofTaps);
//
        pilotTable. resize (numberofPilots);

	for (int i = 0; i < numberofPilots; i ++)
	   pilotTable [i] = i;

	for (int row = 0; row < numberofPilots; row ++)
           for (int col = 0; col < numberofPilots; col ++)
              S_p (row, col) = std::complex<float> (0, 0);
        for (int index = 0; index < numberofPilots; index ++)
	   S_p (index, index) = refTable [pilotTable [index]];

	for (int pilotIndex = 0; pilotIndex < numberofPilots; pilotIndex ++) {
	   for (int tap = 0; tap < numberofTaps; tap ++)
	      F_p (pilotIndex, tap) =
	         cdiv (createExp (2 * M_PI *
	                        (fftSize / 2 + pilotTable [pilotIndex]) *
	                                          tap / fftSize), sqrt (fftSize));
	}
	S_pxF_p                 = S_p * F_p;
}

	estimator::~estimator () {
	delete fft_forward;
}

void	estimator::estimate	(std::vector<Complex> v,
	                             std::vector <Complex> &resultRow) {
Vector  h_td (numberofTaps);
Vector  H_fd (numberofPilots);
Vector  X_p  (numberofPilots);

	resultRow. resize (numberofPilots);
	fft_forward -> fft (v);
//
        for (int index = 0; index < numberofPilots; index ++)
           X_p (index) = v [pilotTable [index]];
//
////    Ok, the matrices are filled, now computing the channelvalues
        h_td    = S_pxF_p. bdcSvd (ComputeThinU | ComputeThinV). solve (X_p);
        H_fd    = F_p * h_td;
//
        for (int index = 0; index < numberofPilots; index ++)
           resultRow [index] = h_td (index);
}

