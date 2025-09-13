#
/*
 *    Copyright (C) 2014 .. 2025
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation version 2 of the License.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


//	In an OFDM system such as DAB, IQ imbalance appears as a form of
//	intercarrier interference (ICI), where the signal from a mirror
//	subcarrier leaks into the desired subcarrier, and the signal
//	on the desired subcarrier is scaled and rotated.
//	This manifests as a distorted constellation diagram at the receiver,
//	with points appearing scaled and rotated, and also results in
//	a degraded bit error rate (BER). 
//
//	At some point, one of the configured input devices showed a 
//	large difference in size of the I and Q component.
//
//	The  IQ imbalance is blindly detected by correlating the I
//	and Q paths. The complex result of the complex conjugate product
//	of the I and Q data path is proportional to the IQ quadrature error.
//	Any DC offset is removed first and amplitude normalization is provided
//	by a computed gain component
//
//	especially 
//	https://dsp.stackexchange.com/questions/49136/detecting-i-q-imbalance
//	is worth reading

#pragma once
#include	"dab-constants.h"

class	equalizer {
private:
DABFLOAT	I_avg;
DABFLOAT	Q_avg;
DABFLOAT	IQ_avg;
DABFLOAT	Q_out;
DABFLOAT	Alpha_;
public:
	equalizer	() {
	I_avg		= 1.0f;		// meanII
	Q_avg		= 1.0;
	IQ_avg		= 1.0f;		// meanIQ
	Q_out		= 1.0f;		// meanQQ
	Alpha_		= 1.0 / 2048000.0;
}
	~equalizer	() {}

Complex	equalize	(Complex v) {
	DABFLOAT v_i	= real (v);
	DABFLOAT v_q	= imag (v);
	I_avg		= compute_avg (I_avg, square (v_i), Alpha_);
	Q_avg		= compute_avg (Q_avg, square (v_q), Alpha_);
	IQ_avg		= compute_avg (IQ_avg, v_i * v_q, Alpha_);
	DABFLOAT Beta	= IQ_avg / (sqrt (I_avg) * sqrt (Q_avg));
	DABFLOAT K	= v_q - Beta * v_i;
	Q_out		= compute_avg (Q_out, square (K), Alpha_);
	DABFLOAT Q_gain	= std::sqrt (I_avg / Q_out);
	return Complex (v_i, K * Q_gain);
}

};


