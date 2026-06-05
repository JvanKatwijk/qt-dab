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
#include	"tii-scope.h"
#include	"dab-constants.h"
#include	<QSettings>

static inline
int	valueFor (int bd) {
int result	= 1;
	while (bd > 1) {
	   result <<= 1;
	   bd --;
	}
	return result;
}

#define	T_u	2048
#define	carriers	1536

	tiiScope::tiiScope (QwtPlot *dabScope,
	                    int displaySize,
	                    QSettings	*dabSettings,
	                    const QString &scopeName):
	                          basicScope (dabScope, dabSettings,
	                                      displaySize, scopeName),
	                          theFFT (T_u, false) {

	this		-> dabSettings		= dabSettings;
	this		-> displaySize		= displaySize;
	displayBuffer. resize (displaySize);
	for (int i = 0; i < displaySize; i ++)
	   displayBuffer [i] = 0;
	bitDepth	= 12;
	normalizer	= valueFor (bitDepth);
}

	tiiScope::~tiiScope	() {
}

void	tiiScope::display		(std::vector<Complex> &v,
	                                 int lowFreq, int highFreq,
	                                 int Amp, int markOffset) {
floatQwt X_axis [displaySize];
floatQwt Y_values [displaySize];
Complex spectrumBuffer [T_u];

	if (v. size () != T_u)
	   return;

	for (int i = 0; i < displaySize; i ++)
	   X_axis [i] = i * (highFreq - lowFreq) / displaySize + lowFreq;

	floatQwt resVec [carriers / 8];
	for (int i = 0; i < v. size (); i ++)
	   spectrumBuffer [i] = v [i];

	theFFT. fft (spectrumBuffer);

	floatQwt min	= 100000;
	floatQwt max	= -100000;
	for (int i = 0; i < carriers / 8; i ++) {
	   Y_values [i] = 0;
	   for (int j = 0; j < 4; j ++) {
	      int index = (T_u - carriers / 2 + i * 2 + j * carriers / 4) % T_u;
	      Y_values [i] += abs (spectrumBuffer [index]) +
	                          abs (spectrumBuffer [index + 1]);
	   }
	   displayBuffer [i] = 0.8 * displayBuffer [i] + 0.2 * Y_values [i];
	
	   if (displayBuffer [i] < min)
	      min = displayBuffer [i];
	   else
	   if (displayBuffer [i] > max)
	      max = displayBuffer [i];
	}
	markType marker;
	marker. offset	= markOffset;
	marker. text	= QString (">>> (") + QString::number (markOffset) + ")";
	std::vector<markType> markers;
	markers. push_back (marker);
	showSpectrum (displayBuffer. data (), displaySize,
	              X_axis [0], X_axis [displaySize - 1],
	              min, min + (float)Amp + 20, markers);
}

float   tiiScope::get_db (float x) {
        return 20 * log10 ((x + 1) / (float)(normalizer));
}

void	tiiScope::set_bitDepth	(int d) {
	this	-> bitDepth	= d;
}

void	tiiScope::clean		() {
}

