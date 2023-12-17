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
#include        <QString>
#include        "dab-constants.h"
#include        <vector>

#include	"fft-handler.h"
#include	"phasetable.h"
#include	"dab-constants.h"
#include	"dab-params.h"
#include	"process-params.h"
#include	"ringbuffer.h"
class	RadioInterface;

class estimator : public QObject, public phaseTable {
Q_OBJECT
public:
			estimator 		(RadioInterface *,
	                                         processParams *);
			~estimator		();
	void		estimate		(std::vector<Complex>,
	                                         std::vector<Complex> &);
//	This one is used in the ofdm decoder
private:
	dabParams	params;
	fftHandler	fft_forward;
	fftHandler	fft_backwards;
	std::vector<Complex> refTable;
	int32_t		T_u;
	int32_t		T_g;
	int16_t		carriers;

        int16_t         numberofTaps;
        int16_t         fftSize;
signals:
	void		showCorrelation	(int, int, QVector<int>);
};

