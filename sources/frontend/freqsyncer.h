#
/*
 *    Copyright (C) 2016 .. 2025
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

class	RadioInterface;

#define	FFT_CORR	0100
class freqSyncer : public QObject {
Q_OBJECT
public:
			freqSyncer 		(RadioInterface *,
	                                         processParams *,
	                                         phaseTable *,
	                                         bool speedUp = false,
	                                         uint8_t correlator = 0);
			~freqSyncer		();
	int16_t		estimate_CarrierOffset	(std::vector<Complex>);
private:
	dabParams	params;
	phaseTable	*theTable;
	std::vector<float> phaseDifferences;
	int16_t		diff_length;
	int32_t		T_u;
	int32_t		T_g;
	int16_t		carriers;
	fftHandler	fft_forward;
#define	TEST_SIZE	128
	fftHandler	go_forward;
	fftHandler	go_backwards;
	Complex		t1 [TEST_SIZE];
	Complex		t2 [TEST_SIZE];
	bool		speedUp;
	uint8_t		correlator;
};

