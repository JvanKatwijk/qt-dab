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
class	QSettings;

class	TII_Detector {
public:
			TII_Detector	(uint8_t dabMode, QSettings *);
			~TII_Detector	();
	void		reset		();
	void		addBuffer	(std::vector<Complex>);
	void		set_tiiThreshold	(int);
	QVector<tiiData>	processNULL	();

private:
	QSettings	*dabSettings;
	dabParams	params;
	phaseTable	theTable;
	std::vector<Complex> table_2;
	int16_t		T_u;
	int16_t		carriers;	
	fftHandler	my_fftHandler;
	void		collapse	(Complex *, 
	                                 Complex *, Complex *);

//	bool		collisions	= false;

	std::vector<Complex >	theBuffer;
	std::vector<DABFLOAT>	window;

	int		tiiThreshold;
};


