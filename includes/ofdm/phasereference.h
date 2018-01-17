#
/*
 *    Copyright (C) 2013 .. 2017
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
#
#ifndef	__PHASEREFERENCE__
#define	__PHASEREFERENCE__
#include	<QObject>
#include	<stdio.h>
#include	<stdint.h>
#include	<vector>
#include	"fft-handler.h"
#include	"phasetable.h"
#include	"dab-constants.h"
#include	"dab-params.h"
#include	"ringbuffer.h"
class	RadioInterface;

class phaseReference : public QObject, public phaseTable {
Q_OBJECT
public:
			phaseReference 		(RadioInterface *,
	                                         uint8_t,
#ifdef	IMPULSE_RESPONSE
						 RingBuffer<float> *b,
#endif
	                                         int16_t, int16_t);
			~phaseReference		(void);
	int32_t		findIndex		(std::vector<std::complex<float>>);
	int16_t		estimate_CarrierOffset	(std::vector<std::complex<float>>);
	float		estimate_FrequencyOffset (std::vector<std::complex<float>>);
//
//	This one is used in the ofdm decoder
	std::vector<std::complex<float>> refTable;
private:
	fftHandler	my_fftHandler;
	dabParams	params;
#ifdef	IMPULSE_RESPONSE
	RingBuffer<float> *response;
#endif
	std::vector<float> phaseDifferences;
	int16_t		threshold;
	int16_t		diff_length;
	int32_t		T_u;
	int16_t		carriers;

	std::complex<float>	*fft_buffer;
	int32_t		fft_counter;
	int32_t		framesperSecond;	
	int32_t		displayCounter;
signals:
	void		showImpulse (int);
};
#endif

