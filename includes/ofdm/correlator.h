#
/*
 *    Copyright (C) 2016 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
#
#pragma once

#include	<QObject>
#include	<cstdio>
#include	<cstdint>
#include	<vector>
#include	"fft-handler.h"
#include	"phasetable.h"
#include	"dab-constants.h"
#include	"dab-params.h"
#include	"process-params.h"
#include	"ringbuffer.h"
#include	"fft-handler.h"

class	RadioInterface;

class correlator : public QObject, public phaseTable {
Q_OBJECT
public:
			correlator 		(RadioInterface *,
	                                         processParams *);
			~correlator		();
	int32_t		findIndex		(std::vector<Complex>,
	                                         bool,  int);
//	This one is used in the ofdm decoder
private:
	dabParams	params;
	fftHandler	fft_forward;
	fftHandler	fft_backwards;
	RingBuffer<float> *response;

	std::vector<Complex> refTable;
	int16_t		depth;
	int32_t		T_u;
	int32_t		T_g;
	int16_t		carriers;

	int32_t		fft_counter;
	int32_t		framesperSecond;	
	int32_t		displayCounter;


signals:
	void		show_correlation	(int, int, QVector<int>);
};
//#endif

