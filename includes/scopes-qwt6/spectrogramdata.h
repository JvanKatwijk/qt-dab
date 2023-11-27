#
/*
 *    Copyright (C) 2016 .. 2022
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
 *
 */
#pragma once

#include	<cstdio>
#include	<cstdlib>
#include	<qwt_interval.h>
#include	<QPen>
//
//	Qwt 6.2 is different from the 6.1 version, these mods
//	seem to work
# include	<qwt_raster_data.h>

class	spectrogramData: public QwtRasterData {
public:
	double	*data;		// pointer to actual data
	int	left;		// index of left most element in raster
	int	width;		// raster width
	int	height;		// rasterheigth
	int	datawidth;	// width of matrix
	int	dataheight;	// for now == rasterheigth
	double	max;

	spectrogramData (double *data, int left,
	                 int width, int height,
	                 int datawidth, double max);

	~spectrogramData	();

void	initRaster (const QRectF &x, const QSize &raster);

QwtInterval interval (Qt::Axis x) const;

double value (double x, double y) const;
};

