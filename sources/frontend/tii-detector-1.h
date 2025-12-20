#
/*
 *    Copyright (C) 2016 .. 2024
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

#pragma once

#include	<cstdint>
#include	"dab-params.h"
#include	<complex>
#include	<vector>
#include	"dab-constants.h"
#include	"fft-handler.h"
#include	"phasetable.h"
#include	<QVector>

#define NUM_GROUPS      8
#define GROUPSIZE       24
class	TII_Detector {
public:
			TII_Detector	(uint8_t dabMode, phaseTable *theTable);
			~TII_Detector	();
	void		reset		();
	void		addBuffer	(const std::vector<Complex> &);
	QVector<tiiData>	processNULL	(int16_t, uint8_t);

private:
	dabParams	params;
	int16_t		T_u;
	int16_t		T_g;
	int16_t		carriers;
	fftHandler	my_fftHandler;
	bool		carrierDelete;
	Complex		decodedBuffer [768];
	std::vector<Complex> nullSymbolBuffer;
	void		resetBuffer	();
	void		collapse	(const Complex *, 
	                                 Complex *, Complex *);
	int		tiiThreshold;
	std::vector<uint8_t>	rotationTable;
	Complex		rotate		(Complex, uint8_t);
};


