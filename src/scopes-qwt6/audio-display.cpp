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
	                              spectrumCurve (""),
	                              fft (2048, false) {
int16_t	i;
QString	colorString;

	this	-> myRadioInterface	= mr;
	this	-> plotGrid		= plotGrid;
	this	-> dabSettings		= dabSettings;

	displaySize			= 512;
	spectrumSize			= 2048;
	normalizer			= 16 * 2048;

	for (int i = 0; i < displaySize; i ++)
	   displayBuffer [i] = 0;
	spectrumBuffer		= new std::complex<float> [spectrumSize];
	dabSettings	-> beginGroup ("audioDisplay");
	colorString	= dabSettings -> value ("displayColor",
	                                                 "black"). toString();
	this	->	displayColor	= QColor (colorString);
	colorString	= dabSettings -> value ("gridColor",
	                                                 "black"). toString();
	this	->	gridColor	= QColor (colorString);
	colorString	= dabSettings -> value ("curveColor",
	                                                 "white"). toString();
	this	-> 	curveColor	= QColor (colorString);

	brush		= dabSettings -> value ("brush", 1). toInt () == 1;
	displaySize	= dabSettings -> value ("displaySize",
	                                                   512).toInt();
	dabSettings	-> endGroup ();

	this	-> plotGrid	-> setCanvasBackground (displayColor);
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
	
	for (i = 0; i < spectrumSize; i ++) 
	   Window [i] =
	        0.42 - 0.5 * cos ((2.0 * M_PI * i) / (spectrumSize - 1)) +
	              0.08 * cos ((4.0 * M_PI * i) / (spectrumSize - 1));
}

	audioDisplay::~audioDisplay () {
	delete	spectrumBuffer;
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

	fft. fft (spectrumBuffer);
//
//	first X axis labels
	for (i = 0; i < displaySize; i ++)
	   X_axis [i] = 
	          (double)((i) * (double) sampleRate / 2) / displaySize / 1000;
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

void	audioDisplay::rightMouseClick	(const QPointF &point) {
colorSelector *selector;
int	index;
	(void)point;
	selector		= new colorSelector ("display color");
	index			= selector -> QDialog::exec ();
	QString displayColor	= selector -> getColor (index);
	delete selector;
	if (index == 0)
	   return;
	selector		= new colorSelector ("grid color");
	index			= selector	-> QDialog::exec ();
	QString gridColor	= selector	-> getColor (index);
	delete selector;
	if (index == 0)
	   return;
	selector		= new colorSelector ("curve color");
	index			= selector	-> QDialog::exec ();
	QString curveColor	= selector	-> getColor (index);
	delete selector;
	if (index == 0)
	   return;

	dabSettings	-> beginGroup ("audioDisplay");
	dabSettings	-> setValue ("displayColor", displayColor);
	dabSettings	-> setValue ("gridColor", gridColor);
	dabSettings	-> setValue ("curveColor", curveColor);
	dabSettings	-> endGroup ();
	this		-> displayColor	= QColor (displayColor);
	this		-> gridColor	= QColor (gridColor);
	this		-> curveColor	= QColor (curveColor);
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
