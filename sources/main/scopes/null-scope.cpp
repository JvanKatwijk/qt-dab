#
/*
 *    Copyright (C)  2016 .. 2024
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

#include	"null-scope.h"
#include	<QSettings>

	nullScope::nullScope (clickableChart *plotGrid,
	                      int displaySize,
	                      QSettings	*dabSettings,
	                      const QString &name):
	                         basicScope (plotGrid,
	                                     dabSettings,
	                                     displaySize,
	                                     name) {
	this	-> displaySize	= displaySize;
}

	nullScope::~nullScope	() {
}

void	nullScope::display	(const  std::vector<Complex> &V,
	                                   int amount, int startIndex) {
float	max	= 0;
double Y_values [512];

	(void)amount;
	for (int i = 0; i < 512; i ++) {
	   Y_values [i] = abs (V [i]);
	   if (abs (V [i]) > max)
	      max = abs (V [i]);
	}

	Y_values [0]		= 0;
	Y_values [512 - 1]	= 0;
	markType mark;
	mark. offset	= startIndex;
	mark. text	= QString ("startPoint");
	std::vector<markType> markers;
	markers. push_back (mark);
	showSpectrum (Y_values, displaySize,
	              0, 2 * displaySize, 0, max, markers);
}

void	nullScope::clean	() {}
