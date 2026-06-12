#
/*
 *    Copyright (C)  2014 .. 2017
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

#include	"audio-display.h"
#include	<QColor>
#include	<QPen>

	audioDisplay::audioDisplay  (RadioInterface	*mr,
	                             clickableChart	*plotGrid,
	                             QSettings		*dabSettings_i):
	                              basicScope (plotGrid,
	                                          dabSettings_i, 512,
	                                          "audioScope"),
	                              myRadioInterface (mr),
	                              dabSettings (dabSettings_i),
	                              fft (1024, false) {

	displaySize			= 512;
	spectrumSize			= 1024;
	normalizer			= 16 * 2048;

	displayBuffer	= new double [displaySize];
	for (int i = 0; i < displaySize; i ++)
	   displayBuffer [i] = 0;
	spectrumBuffer		= new Complex [spectrumSize];

//	create a blackman window
	for (int i = 0; i < spectrumSize; i ++) 
	   Window [i] =
	        0.42 - 0.5 * cos ((2.0 * M_PI * i) / (spectrumSize - 1)) +
	              0.08 * cos ((4.0 * M_PI * i) / (spectrumSize - 1));
}

	audioDisplay::~audioDisplay () {
	delete []	displayBuffer;
	delete []	spectrumBuffer;
}

void	audioDisplay::createSpectrum  (std::complex<int16_t> *data,
	                              int amount, int sampleRate) {
auto Y_values	= dynVec (double, displaySize);

	if (amount > spectrumSize)
	   amount = spectrumSize;
	for (int i = 0; i < amount; i ++)
	   spectrumBuffer [i] = 
	             Complex (real (data [i]) / 32768.0f,
	                                  imag (data [i]) / 32768.0);

	for (int i = amount; i < spectrumSize;  i ++)
	   spectrumBuffer [i] = Complex (0, 0);
//	and window it

	for (int i = 0; i < spectrumSize; i ++)
	   spectrumBuffer [i] = spectrumBuffer [i] * Window [i];

	fft. fft (spectrumBuffer);
//
//	and map the spectrumSize values onto displaySize elements
	for (int i = 0; i < displaySize; i ++) 
	   Y_values [i] = get_db (abs (spectrumBuffer [i]));
//
//	average the image a little.
	float	min	= 100000;
	float	max	= -100000;
	for (int i = 0; i < displaySize; i ++) {
	   if (std::isnan (Y_values [i]) || std::isinf (Y_values [i]))
	      continue;
	   displayBuffer [i] = 
	          0.9 * displayBuffer [i] + 0.1 * Y_values [i];
	   if (displayBuffer [i] < min)
	      min = displayBuffer [i];
	   else
	   if (displayBuffer [i] > max)
	      max = displayBuffer [i];
	}
	displayBuffer [0] = get_db (0);
	displayBuffer [displaySize - 1] = get_db (0);

	showSpectrum (displayBuffer, displaySize, 0,
	                            sampleRate / 2000, get_db (0),
	                                               get_db (0) + 40);
}

float	audioDisplay::get_db (float x) {
	return 20 * log10 ((x + 1) / (float)(normalizer));
}

