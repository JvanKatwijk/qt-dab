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
#include	<QColorDialog>

	audioDisplay::audioDisplay  (RadioInterface	*mr,
	                             QwtPlot		*plotGrid_i,
	                             QSettings		*dabSettings_i):
	                              myRadioInterface (mr),
	                              dabSettings (dabSettings_i),
	                              plotGrid (plotGrid_i),
	                              spectrumCurve (""),
	                              fft (2048, false) {

int16_t	i;
QString	colorString;

	displaySize			= 512;
	spectrumSize			= 2048;
	normalizer			= 16 * 2048;

	for (int i = 0; i < displaySize; i ++)
	   displayBuffer [i] = 0;
	spectrumBuffer		= new Complex [spectrumSize];

	dabSettings	-> beginGroup ("audioDisplay");
	colorString = dabSettings -> value ("gridColor",
	                                            "#5e5c64").toString();
	this	-> gridColor = QColor (colorString);
	colorString = dabSettings -> value ("curveColor",
	                                            "#f9f06b").toString();
	this	-> curveColor = QColor (colorString);
	brush	= dabSettings -> value ("brush", 1).toInt() == 1;
	displaySize = dabSettings->value("displaySize", displaySize).toInt();
	dabSettings	-> endGroup ();

//	this	-> plotGrid	-> setCanvasBackground (displayColor);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid. setMajPen (QPen(gridColor, 0, Qt::DotLine));
#else
	grid. setMajorPen (QPen(gridColor, 0, Qt::DotLine));
#endif
	grid. enableXMin (true);
	grid. enableYMin (true);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid. setMinPen (QPen(gridColor, 0, Qt::DotLine));
#else
	grid. setMinorPen (QPen(gridColor, 0, Qt::DotLine));
#endif
	grid. attach (plotGrid);
	lm_picker       = new QwtPlotPicker (plotGrid -> canvas ());
        QwtPickerMachine *lpickerMachine =
                             new QwtPickerClickPointMachine ();

        lm_picker       -> setStateMachine (lpickerMachine);
        lm_picker       -> setMousePattern (QwtPlotPicker::MouseSelect1,
                                            Qt::RightButton);
        connect (lm_picker, SIGNAL (selected (const QPointF&)),
                 this, SLOT (rightMouseClick (const QPointF &)));

   	spectrumCurve. setPen (QPen (curveColor, 2.0));
	spectrumCurve. setOrientation (Qt::Horizontal);
	spectrumCurve. setBaseline	(get_db (0));
	if (brush) {
	   QBrush ourBrush (curveColor);
	   ourBrush. setStyle (Qt::Dense3Pattern);
	   spectrumCurve. setBrush (ourBrush);
	}
        spectrumCurve. attach (plotGrid);
//	create a blackman window
	for (i = 0; i < spectrumSize; i ++) 
	   Window [i] =
	        0.42 - 0.5 * cos ((2.0 * M_PI * i) / (spectrumSize - 1)) +
	              0.08 * cos ((4.0 * M_PI * i) / (spectrumSize - 1));
}

	audioDisplay::~audioDisplay () {
	delete []	spectrumBuffer;
}


void	audioDisplay::createSpectrum  (std::complex<int16_t> *data,
	                              int amount, int sampleRate) {
auto X_axis	= dynVec (floatQwt, displaySize);
auto Y_values	= dynVec (floatQwt, displaySize);
//floatQwt	X_axis [displaySize];
//floatQwt	Y_values [displaySize];
int16_t	averageCount	= 3;

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
//	first X axis labels
	for (int i = 0; i < displaySize; i ++)
	   X_axis [i] = 
	          (floatQwt)((i) * (floatQwt) sampleRate / 2) / displaySize / 1000;
//
//	and map the spectrumSize values onto displaySize elements
	for (int i = 0; i < displaySize; i ++) {
	   floatQwt f	= 0;
	   for (int j = 0; j < spectrumSize / 2 / displaySize; j ++)
	      f += abs (spectrumBuffer [i * spectrumSize / 2 / displaySize  + j]);

	   Y_values [i] = f / (spectrumSize / 2 / displaySize);
	}
//
//	average the image a little.
	for (int i = 0; i < displaySize; i ++) {
	   if (std::isnan (Y_values [i]) || std::isinf (Y_values [i]))
	      continue;
	   displayBuffer [i] = 
	          (floatQwt)(averageCount - 1) /averageCount * displayBuffer [i] +
	           1.0f / averageCount * Y_values [i];
	}

	memcpy (Y_values,
	        displayBuffer, displaySize * sizeof (floatQwt));
	ViewSpectrum (X_axis, Y_values, 100, 0);
}

void	audioDisplay::ViewSpectrum (floatQwt *X_axis,
		                    floatQwt *Y1_value,
	                            floatQwt amp, int32_t marker) {
float	amp1	= amp / 100;

	(void)marker;
	amp		= amp / 50.0 * (-get_db (0));
	plotGrid	-> setAxisScale (QwtPlot::xBottom,
				         (floatQwt)X_axis [0],
				         X_axis [displaySize - 1]);
	plotGrid	-> enableAxis (QwtPlot::xBottom);
	plotGrid	-> setAxisScale (QwtPlot::yLeft,
				         get_db (0), get_db (0) + amp1 * 40);

	for (int i = 0; i < displaySize; i ++) 
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

void	audioDisplay::rightMouseClick	(const QPointF &point) {
QColor	color;
	(void)point;

	color = QColorDialog::getColor (color, nullptr, "displayColor");
        if (!color. isValid ())
           return;
	this	-> displayColor	= color;
        color = QColorDialog::getColor (gridColor, nullptr, "gridColor");
        if (!color. isValid ())
           return;
	this	-> gridColor	= color;
        color = QColorDialog::getColor (curveColor, nullptr, "curveColor");
        if (!color. isValid ())
           return;
	this	-> curveColor	= color;
	dabSettings	-> beginGroup ("audioDisplay");
	dabSettings	-> setValue ("displayColor", displayColor. name ());
	dabSettings	-> setValue ("gridColor", gridColor. name ());
	dabSettings	-> setValue ("curveColor", curveColor. name ());
	dabSettings	-> endGroup ();
	spectrumCurve. setPen (QPen (this -> curveColor, 2.0));
	if (brush) {
           QBrush ourBrush (this -> curveColor); 
           ourBrush. setStyle (Qt::Dense3Pattern);         
           spectrumCurve. setBrush (ourBrush);
        }

#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid. setMajPen (QPen(this -> gridColor, 0, Qt::DotLine));
#else
	grid. setMajorPen (QPen(this -> gridColor, 0, Qt::DotLine));
#endif
	grid. enableXMin (true);
	grid. enableYMin (true);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid. setMinPen (QPen(this -> gridColor, 0, Qt::DotLine));
#else
	grid. setMinorPen (QPen(this -> gridColor, 0, Qt::DotLine));
#endif
	plotGrid	-> setCanvasBackground (this -> displayColor);
}
