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
#include	"spectrum-scope.h"
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

static inline
int	minimum	(int x, int y) {
	if (y < x)
	   return y;
	return x;
}

	spectrumScope::spectrumScope (clickableChart *dabScope,
	                              int displaySize,
	                              QSettings	*dabSettings,
	                              const QString &scopeName):
	                                 basicScope (dabScope,
	                                             dabSettings, displaySize,
	                                             scopeName),
	                                 theFFT (4 * displaySize, false) {

	this		-> dabSettings		= dabSettings;
	this		-> displaySize		= displaySize;
//      create a blackman window 
	Window. resize (4 * displaySize);
        for (int i = 0; i < 4 * displaySize ; i ++)
           Window [i] =
                0.42 - 0.5 * cos ((2.0 * M_PI * i) / (4 * displaySize - 1)) +
                      0.08 * cos ((4.0 * M_PI * i) / (4 * displaySize - 1));
	normalizer	= valueFor (12);
	displayBuffer. resize (displaySize);
}

	spectrumScope::~spectrumScope	() {
}

void	spectrumScope::display		(std::vector<Complex> &v,
	                                 int low, int high, int Amp) {
float X_axis [displaySize];
float Y_values [displaySize];
Complex fftBuffer [4 * displaySize];
int amount 	= minimum (v. size (), 4 * displaySize);

	for (int i = 0; i < displaySize; i ++)
	   X_axis [i] = (i * (high - low) / displaySize + low);
	for (int i = 0; i < amount; i ++)
	   fftBuffer [i] = v [i] * Window [i];
	for (int i = amount; i < 4 * displaySize; i ++)
	   fftBuffer [i] = Complex (0, 0);
	theFFT. fft (fftBuffer);

	DABFLOAT min	= 100000;
	DABFLOAT max	= -100000;
	for (int i = 0; i < displaySize; i ++) {
	   Y_values [i] = 0;
	   for (int j = 0; j < 4; j ++) 
	      Y_values [i] +=
	               abs (fftBuffer [(2 * displaySize + 4 * i + j) % (4 * displaySize)]);
	   displayBuffer [i] = 0.8 * displayBuffer [i] +
	                              0.2 * get_db (Y_values [i]);
	   if (displayBuffer [i] < min)
	      min = displayBuffer [i];
	   else
	   if (displayBuffer [i] > max)
	      max = displayBuffer [i];
	}

	showSpectrum (displayBuffer. data (), displaySize,
	              low / 1000, high / 1000,
//	              X_axis [0], X_axis [displaySize - 1],
	              min, min + (float)Amp + 20);
}

float   spectrumScope::get_db (float x) {
        return 20 * log10 ((x + 1) / (float)(normalizer));
}

void	spectrumScope::set_bitDepth	(int d) {
	this	-> bitDepth	= d;
}

void	spectrumScope::clean		() {
}
