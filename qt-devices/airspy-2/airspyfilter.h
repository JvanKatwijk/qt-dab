#
/*
 *    Copyright (C) 2013 .. 2020
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

#ifndef __AIRSPY_FILTER__
#define __AIRSPY_FILTER__

#include	<cstdlib>
#include	<cmath>
#include	<complex>
class		logger;

class	airspyFilter {
public:
			airspyFilter	(int16_t, int32_t, int32_t, logger *);
	                ~airspyFilter	();
	std::complex<float> Pass	(float re, float im);
	int16_t		firSize;
	int16_t		ip;
	int32_t		sampleRate;
	float		buffer_re [];
	float		buffer_im [];
	float		kernel[];
};

#endif
