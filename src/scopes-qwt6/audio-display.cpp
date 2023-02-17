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
#include	"color-selector.h"

	audioDisplay::audioDisplay  (RadioInterface	*mr,
	                             QwtPlot		*plotGrid,
	                             QSettings		*dabSettings):
	                              spectrumCurve ("") {
int16_t	i;
bool	brush;

	this	-> myRadioInterface	= mr;
	this	-> dabSettings		= dabSettings;

	displaySize	= 512;
	spectrumSize	= 2048;
	normalizer	= 16 * 2048;

	for (int i = 0; i < displaySize; i ++)
	   displayBuffer [i] = 0;
	spectrumBuffer	= (std::complex<float> *)fftwf_malloc (sizeof (fftwf_complex) * spectrumSize);
        plan    = fftwf_plan_dft_1d (spectrumSize,
                                    reinterpret_cast <fftwf_complex *>(spectrumBuffer),
                                    reinterpret_cast <fftwf_complex *>(spectrumBuffer),
                                    FFTW_FORWARD, FFTW_ESTIMATE);
	
	this	-> plotGrid	= plotGrid;
	this	-> plotGrid	-> setCanvasBackground (QColor ("black"));
	this	-> gridColor	= QColor ("white");
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid. setMajPen (QPen(gridColor, 0, Qt::DotLine));
#else
	grid. setMajorPen (QPen(gridColor, 0, Qt::DotLine));
#endif
	grid. enableXMin (true);
	grid. enableYMin (true);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
//	grid. setMinPen (QPen(gridColor, 0, Qt::DotLine));
#else
//	grid. setMinorPen (QPen(gridColor, 0, Qt::DotLine));
#endif
	grid. attach (plotGrid);
	curveColor	= QColor ("white");
   	spectrumCurve. setPen (QPen(curveColor, 2.0));
	spectrumCurve. setOrientation (Qt::Horizontal);
	spectrumCurve. setBaseline	(get_db (0));
	ourBrush	= new QBrush (curveColor);
	ourBrush        -> setStyle (Qt::Dense3Pattern);
	QBrush ourBrush (curveColor);
	ourBrush. setStyle (Qt::Dense3Pattern);
	spectrumCurve. setBrush (ourBrush);
        spectrumCurve. attach (plotGrid);
	
	for (i = 0; i < spectrumSize; i ++) 
	   Window [i] =
	        0.42 - 0.5 * cos ((2.0 * M_PI * i) / (spectrumSize - 1)) +
	              0.08 * cos ((4.0 * M_PI * i) / (spectrumSize - 1));
}

	audioDisplay::~audioDisplay () {
	fftwf_destroy_plan (plan);
	fftwf_free	(spectrumBuffer);
	delete		ourBrush;
}


void	audioDisplay::createSpectrum  (int16_t *data,
	                              int amount, int sampleRate) {
double	X_axis [displaySize];
double	Y_values [displaySize];
int16_t	i, j;
int16_t	averageCount	= 3;

	if (amount > spectrumSize)
	   amount = spectrumSize;
	for (int i = 0; i < amount / 2; i ++)
	   spectrumBuffer [i] = std::complex<float> (data [2 * i] / 8192.0,
	                                             data [2 * i + 1] / 8192.0);

	for (int i = amount / 2; i < spectrumSize;  i ++)
	   spectrumBuffer [i] = std::complex<float> (0, 0);
//	and window it

	for (i = 0; i < spectrumSize; i ++)
	   spectrumBuffer [i] = cmul (spectrumBuffer [i], Window [i]);

	fftwf_execute (plan);
//
//	first X axis labels
	for (i = 0; i < displaySize; i ++)
	   X_axis [i] = 
	          (double)((i) * (double) sampleRate) / displaySize / 1000;
//
//	and map the spectrumSize values onto displaySize elements
	for (i = 0; i < displaySize; i ++) {
	   double f	= 0;
	   for (j = 0; j < spectrumSize / 2 / displaySize; j ++)
	      f += abs (spectrumBuffer [i * spectrumSize / 2 / displaySize  + j]);

	   Y_values [i] = f / (spectrumSize / 2 / displaySize);
	}
//
//	average the image a little.
	for (i = 0; i < displaySize; i ++) {
	   if (std::isnan (Y_values [i]) || std::isinf (Y_values [i]))
	      continue;
	   displayBuffer [i] = 
	          (double)(averageCount - 1) /averageCount * displayBuffer [i] +
	           1.0f / averageCount * Y_values [i];
	}

	memcpy (Y_values,
	        displayBuffer, displaySize * sizeof (double));
	ViewSpectrum (X_axis, Y_values, 100, 0);
}

void	audioDisplay::ViewSpectrum (double *X_axis,
		                       double *Y1_value,
	                               double amp,
	                               int32_t marker) {
uint16_t	i;
float	amp1	= amp / 100;

	amp		= amp / 50.0 * (-get_db (0));
	plotGrid	-> setAxisScale (QwtPlot::xBottom,
				         (double)X_axis [0],
				         X_axis [displaySize - 1]);
	plotGrid	-> enableAxis (QwtPlot::xBottom);
	plotGrid	-> setAxisScale (QwtPlot::yLeft,
				         get_db (0), get_db (0) + amp1 * 40);

	for (i = 0; i < displaySize; i ++) 
	   Y1_value [i] = get_db (amp1 * Y1_value [i]); 

	spectrumCurve. setBaseline (get_db (0));
	Y1_value [0]		= get_db (0);
	Y1_value [displaySize - 1] = get_db (0);

	spectrumCurve. setSamples (X_axis, Y1_value, displaySize);
	plotGrid	-> replot(); 
}

float	audioDisplay::get_db (float x) {
	return 20 * log10 ((x + 1) / (float)(normalizer));
}

