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

#pragma once

#include	<cstdint>
#include	"dab-params.h"
#include	<complex>
#include	<vector>
#include	"fft-handler.h"

class	TII_Detector {
public:
			TII_Detector	(uint8_t dabMode, int16_t);
			~TII_Detector	();
	void		reset		();
	void		setMode		(bool);
	void		addBuffer	(std::vector<Complex>);
	uint16_t	processNULL	();

private:
	fftHandler	*my_fftHandler;
	void		collapse	(Complex *,
	                                         float *);
	bool		detectMode_new;
	int16_t		depth;
	uint8_t		invTable [256];
	dabParams	params;
	int16_t		T_u;
	int16_t		carriers;
	std::vector<Complex >	theBuffer;
#if USE_DOUBLE
	std::vector<double>	window;
#else
	std::vector<float>	window;
#endif
};


