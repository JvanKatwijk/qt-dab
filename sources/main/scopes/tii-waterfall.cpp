#
/*
 *    Copyright (C) 2016 .. 2025
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB 
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
#include	"tii-waterfall.h"

#define	T_u	2048
#define	carriers	1536

	tiiWaterfall::tiiWaterfall (QwtPlot	*plotArea,
	                            int	displaySize,
	                            int	rasterSize) :
	                                         waterfallScope (plotArea,
	                                                         displaySize,
	                                                         rasterSize),
	                                         theFFT (T_u, false) {
	this	-> displaySize	= displaySize;
	Window. resize (T_u);
        for (int i = 0; i < T_u ; i ++)
           Window [i] =
                0.42 - 0.5 * cos ((2.0 * M_PI * i) / (4 * displaySize - 1)) +
                      0.08 * cos ((4.0 * M_PI * i) / (4 * displaySize - 1));
}

	tiiWaterfall::~tiiWaterfall	() {}

void	tiiWaterfall::display	(std::vector<Complex> &v,
	                                 int low_x, int high_x,
	                                 int sliderValue) {
Complex spectrumBuffer [T_u];

floatQwt X_axis [displaySize];
floatQwt Y_values [displaySize];

	for (int i = 0; i < T_u; i ++) 
	   spectrumBuffer [i] = v [i] * Window [i];
	theFFT. fft (spectrumBuffer);

	for (int i = 0; i < displaySize; i ++) {
	   X_axis [i] = i * (high_x - low_x) / displaySize + low_x;
	   Y_values [i] = 0;
	   for (int j = 0; j < 4; j ++)
	      Y_values [i] +=
	          abs (spectrumBuffer [(T_u - carriers / 2 + i + j * carriers / 4) % T_u]);
	}

	waterfallScope::display (X_axis, Y_values, sliderValue, 0);
}

