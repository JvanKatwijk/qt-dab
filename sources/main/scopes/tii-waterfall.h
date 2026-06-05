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
#pragma once

#include	<QwtPlot>
#include	<complex>
#include	<vector>
#include	"waterfall-scope.h"
#include	"fft-handler.h"


class tiiWaterfall: public waterfallScope {
public:
		tiiWaterfall		(QwtPlot *, int, int	rasterSize);
	        ~tiiWaterfall		();
	void	display			(std::vector<Complex> &v,
	                                 int low_x, int high_x,
	                                 int sliderValue);
private:
	fftHandler	theFFT;
	int		displaySize;
	std::vector<DABFLOAT> Window;
};
