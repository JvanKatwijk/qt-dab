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
#include	"spectrogramdata.h"
//
	spectrogramData::spectrogramData (double *data, int left,
	                                  int width, int height,
	                                  int datawidth, double max):
	                                                 QwtRasterData () {
	this	-> data		= data;
	this	-> left		= left;
	this	-> width	= width;
	this	-> height	= height;
	this	-> datawidth	= datawidth;
	this	-> dataheight	= height;
	this	-> max		= max;
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0602)
	setInterval (Qt::XAxis, QwtInterval (left, left + width));
	setInterval (Qt::YAxis, QwtInterval (0, height));
	setInterval (Qt::ZAxis, QwtInterval (0, max));
#endif
}

	spectrogramData::~spectrogramData() {
}

void	spectrogramData::initRaster (const QRectF &x, const QSize &raster) {
	(void)x;
	(void)raster;
}

QwtInterval spectrogramData::interval (Qt::Axis x) const {
	if (x == Qt::XAxis)
	   return QwtInterval (left, left + width);
	if (x == Qt::YAxis)
	   return QwtInterval (0, height);
	return QwtInterval (0, max);
}


double	spectrogramData::value (double x, double y) const {
	   x = x - left;
	   x = x / width  * (datawidth  - 1);
	   y = y / height * (dataheight - 1);
	   return data [(int)y * datawidth + (int)x];
}
