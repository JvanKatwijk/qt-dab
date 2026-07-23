#
/*
 *    Copyright (C)  2016 .. 2024
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

#include	"correlation-scope.h"
#include	<QSettings>

	correlationScope::correlationScope	(clickableChart	*plotArea,
	                                         int		displaySize,
	                                         QSettings	*s,
	                                         const QString	&name) :
	                                             basicScope (plotArea,
	                                                         s,
	                                                         displaySize,
	                                                         name) {
	this	-> dabSettings		= s;
	this	-> displaySize		= displaySize;
}

	correlationScope::~correlationScope	() {
}

void	correlationScope::display	(const std::vector<float> &v,
	                                 uint32_t T_g,
	                                 uint32_t amount,
	                                 int sliderValue,
	                                 const std::vector<corrElement> &ss) {
double	 *Y_values 	= dynVec (double, displaySize);
float	Max	= -1000;
float	Min	= 1000;
int	teller	= 0;

	for (uint32_t i = 1024 - 768; i < 1024; i ++) {
	   Y_values [teller] = get_db (v [i]);
	   if (Y_values [teller] > Max)
	      Max = Y_values [teller];
	   if (Y_values [teller] < Min)
	      Min = Y_values [teller];
	   teller ++;
	}

	markers. resize (0);
	int up_to_5	= 0;
	for (auto &x : ss) {
	   up_to_5 ++;
	   if (up_to_5 > 5)
	      break;
	   int x_coord = (int)(x. phase / 360 * 2048 + T_g - 96);
	   if ((x_coord < 1024 - 768) || (x_coord >= 1024))
	      continue;

	   markType t;
	   t. offset	= x_coord;
	   t. text	= QString::number (x. mainId) +
	                      " " + QString::number (x. subId);
	   markers. push_back (t);
	}
	showSpectrum (Y_values, displaySize,
	              1024 - 768, 1024,
	              Min, Min + 10 + sliderValue, markers);
}

float	correlationScope::get_db (float x) {
	return 20 * log10 ((x + 1) / (float)(512));
}

void	correlationScope::clean	() {
}

