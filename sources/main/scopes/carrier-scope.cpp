#
/*
 *    Copyright (C)  2016 .. 2026
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
#include	"carrier-scope.h"
#include	"dab-constants.h"
#include	<QSettings>

#define	T_u	2048
#define	carriers	1536

	carrierScope::carrierScope (clickableChart *dabScope,
	                           int displaySize,
	                           QSettings	*dabSettings,
	                           const QString &scopeName):
	                           basicScope (dabScope,
	                                       dabSettings,
	                                       displaySize,
	                                       scopeName) {

	this		-> dabSettings		= dabSettings;
	this		-> displaySize		= displaySize;
	bitDepth	= 12;
	normalizer	= 2048;
}

	carrierScope::~carrierScope	() {
}

void	carrierScope::display		(const std::vector<DABFLOAT> &V,
	                                 int lowFreq, int highFreq) {
double *Y_values = dynVec (double, displaySize);
	int max = 0;
	int	min	= 1000000;
	for (uint16_t i = 0; i < V. size (); i ++) {
           Y_values [i] = V [i];
	   if (V [i] > max)
	      max = V [i];
	   if (V [i] < min)
	      min = V [i];
	}
        showSpectrum (Y_values, displaySize,
                      -738, 738,
                      min - max / 5, max  + max / 5);
}

void	carrierScope::clean		() {
}

