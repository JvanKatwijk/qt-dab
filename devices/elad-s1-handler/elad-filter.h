#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of qt-dab
 *
 *    qt-dab is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    qt-dab is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with qt-dab; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __ELAD_FILTER__
#define __ELAD_FILTER__

#include	<stdlib.h>
#include	<math.h>
#include	<complex>

class	eladFilter {
public:
			eladFilter	(int16_t, int32_t, int32_t);
	                ~eladFilter	(void);
	std::complex<float> Pass	(std::complex<float>);
private:
	int16_t		firSize;
	float		*kernel;
	float		*buffer_re;
	float		*buffer_im;
	int16_t		ip;
	int32_t		sampleRate;
};

#endif
