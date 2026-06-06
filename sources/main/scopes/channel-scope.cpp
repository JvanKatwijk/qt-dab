#
/*
 *    Copyright (C)  2016 .. 2023
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
#include	"channel-scope.h"

	channelScope::channelScope (QwtPlot *plotArea, int displaySize,
	                            QSettings	*dabSettings,
	                            const QString &name):
	                                  basicScope (plotArea,
	                                              dabSettings,
	                                              displaySize,
	                                              name) {
	this	-> displaySize		= displaySize;
}

	channelScope::~channelScope	() {
}

void	channelScope::display		(const floatQwt *amplitudeValues,
	                                 int Amp) {
floatQwt ampVals [displaySize];
floatQwt min	= +10000;
floatQwt max	= -10000;

	for (int i = 0; i < displaySize; i ++) {
	   ampVals [i] = amplitudeValues [i];
	   if (ampVals [i] < min)
	      min = ampVals [i];
	   else
	   if (ampVals [i] > max)
	      max = ampVals [i];
	}
	showSpectrum (ampVals, displaySize,
	              0, displaySize,
	              min, max);
}

void	channelScope::clean		() {
}

