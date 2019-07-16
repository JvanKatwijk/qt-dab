#
/*
 *    Copyright (C)  2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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

#include	"spectrum-viewer.h"
#include	<QSettings>
#include	"iqdisplay.h"
#include	<QColor>

	spectrumViewer::spectrumViewer	(RadioInterface	*mr,
	                                 QSettings	*dabSettings,
	                                 RingBuffer<std::complex<float>> *sbuffer,
	                                 RingBuffer<std::complex<float>>* ibuffer) {
int16_t	i;
QString	colorString	= "black";
QColor	displayColor;
QColor	gridColor;
QColor	curveColor;

	this	-> myRadioInterface	= mr;
	this	-> dabSettings		= dabSettings;
	this	-> spectrumBuffer	= sbuffer;
	this	-> iqBuffer		= ibuffer;

	colorString			= dabSettings -> value ("displaycolor", "black"). toString();
	displayColor			= QColor (colorString);
	colorString			= dabSettings -> value ("gridcolor", "white"). toString();
	gridColor			= QColor (colorString);
	colorString			= dabSettings -> value ("gridcolor", "white"). toString();
	curveColor			= QColor (colorString);

	displaySize			= dabSettings -> value ("displaySize", 1024).toInt();
	if ((displaySize & (displaySize - 1)) != 0)
	   displaySize = 1024;
	this	-> myFrame		= new QFrame (nullptr);
	setupUi (this -> myFrame);

	this	-> myFrame	-> hide();
	displayBuffer. resize (displaySize);
	memset (displayBuffer. data(), 0, displaySize * sizeof (double));
	this	-> spectrumSize	= 4 * displaySize;
	spectrum		= (std::complex<float> *)fftwf_malloc (sizeof (fftwf_complex) * spectrumSize);
        plan    = fftwf_plan_dft_1d (spectrumSize,
                                    reinterpret_cast <fftwf_complex *>(spectrum),
                                    reinterpret_cast <fftwf_complex *>(spectrum),
                                    FFTW_FORWARD, FFTW_ESTIMATE);
	
	plotgrid		= dabScope;
	plotgrid	-> setCanvasBackground (displayColor);
	grid			= new QwtPlotGrid;
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid	-> setMajPen (QPen(gridColor, 0, Qt::DotLine));
#else
	grid	-> setMajorPen (QPen(gridColor, 0, Qt::DotLine));
#endif
	grid	-> enableXMin (true);
	grid	-> enableYMin (true);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid	-> setMinPen (QPen(gridColor, 0, Qt::DotLine));
#else
	grid	-> setMinorPen (QPen(gridColor, 0, Qt::DotLine));
#endif
	grid	-> attach (plotgrid);

	spectrumCurve	= new QwtPlotCurve ("");
   	spectrumCurve	-> setPen (QPen(Qt::white));
	spectrumCurve	-> setOrientation (Qt::Horizontal);
	spectrumCurve	-> setBaseline	(get_db (0));
	ourBrush	= new QBrush (Qt::white);
	ourBrush	-> setStyle (Qt::Dense3Pattern);
	spectrumCurve	-> setBrush (*ourBrush);
	spectrumCurve	-> attach (plotgrid);
	
	Marker		= new QwtPlotMarker();
	Marker		-> setLineStyle (QwtPlotMarker::VLine);
	Marker		-> setLinePen (QPen (Qt::red));
	Marker		-> attach (plotgrid);
	plotgrid	-> enableAxis (QwtPlot::yLeft);

	Window. resize (spectrumSize);
	for (i = 0; i < spectrumSize; i ++) 
	   Window [i] =
	        0.42 - 0.5 * cos ((2.0 * M_PI * i) / (spectrumSize - 1)) +
	              0.08 * cos ((4.0 * M_PI * i) / (spectrumSize - 1));
	setBitDepth	(12);

	myIQDisplay	= new IQDisplay (iqDisplay, 256);
}

	spectrumViewer::~spectrumViewer() {
	fftwf_destroy_plan (plan);
	fftwf_free	(spectrum);
	myFrame		-> hide();
	delete		Marker;
	delete		ourBrush;
	delete		spectrumCurve;
	delete		grid;
	delete		myFrame;
}

void	spectrumViewer::showSpectrum	(int32_t amount, int32_t vfoFrequency) {
double	X_axis [displaySize];
double	Y_values [displaySize];
int16_t	i, j;
double	temp	= (double)INPUT_RATE / 2 / displaySize;
int16_t	averageCount	= 5;

	   
	(void)amount;
	if (spectrumBuffer -> GetRingBufferReadAvailable() < spectrumSize)
	   return;

	spectrumBuffer	-> getDataFromBuffer (spectrum, spectrumSize);
	spectrumBuffer	-> FlushRingBuffer();
	if (myFrame	-> isHidden()) {
	   spectrumBuffer	-> FlushRingBuffer();
	   return;
	}

//	and window it
//	first X axis labels
	for (i = 0; i < displaySize; i ++)
	   X_axis [i] = 
	         ((double)vfoFrequency - (double)(INPUT_RATE / 2) +
	          (double)((i) * (double) 2 * temp)) / ((double)1000);
//
//	get the buffer data
	for (i = 0; i < spectrumSize; i ++)
	   spectrum [i] = cmul (spectrum [i], Window [i]);

	fftwf_execute (plan);
//
//	and map the spectrumSize values onto displaySize elements
	for (i = 0; i < displaySize / 2; i ++) {
	   double f	= 0;
	   for (j = 0; j < spectrumSize / displaySize; j ++)
	      f += abs (spectrum [spectrumSize / displaySize * i + j]);

	   Y_values [displaySize / 2 + i] = 
                                 f / (spectrumSize / displaySize);
	   f = 0;
	   for (j = 0; j < spectrumSize / displaySize; j ++)
	      f += abs (spectrum [spectrumSize / 2 +
	                             spectrumSize / displaySize * i + j]);
	   Y_values [i] = f / (spectrumSize / displaySize);
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
	        displayBuffer. data(), displaySize * sizeof (double));
	ViewSpectrum (X_axis, Y_values,
	              scopeAmplification -> value(),
	              vfoFrequency / 1000);
}

void	spectrumViewer::ViewSpectrum (double *X_axis,
		                       double *Y1_value,
	                               double amp,
	                               int32_t marker) {
uint16_t	i;
float	amp1	= amp / 100;

	amp		= amp / 100.0 * (- get_db (0));
	plotgrid	-> setAxisScale (QwtPlot::xBottom,
				         (double)X_axis [0],
				         X_axis [displaySize - 1]);
	plotgrid	-> enableAxis (QwtPlot::xBottom);
	plotgrid	-> setAxisScale (QwtPlot::yLeft,
				         get_db (0), get_db (0) + amp);
//				         get_db (0), 0);

	for (i = 0; i < displaySize; i ++) 
	   Y1_value [i] = get_db (amp1 * Y1_value [i]); 

	spectrumCurve	-> setBaseline (get_db (0));
	Y1_value [0]		= get_db (0);
	Y1_value [displaySize - 1] = get_db (0);

	spectrumCurve	-> setSamples (X_axis, Y1_value, displaySize);
	Marker		-> setXValue (marker);
	plotgrid	-> replot(); 
}

float	spectrumViewer::get_db (float x) {
	return 20 * log10 ((x + 1) / (float)(normalizer));
}

void	spectrumViewer::setBitDepth	(int16_t d) {

	if (d < 0 || d > 32)
	   d = 24;

	normalizer	= 1;
	while (-- d > 0) 
	   normalizer <<= 1;
}

void	spectrumViewer::show() {
	myFrame	-> show();
}

void	spectrumViewer::hide() {
	myFrame	-> hide();
}

bool	spectrumViewer::isHidden() {
	return myFrame -> isHidden();
}

void	spectrumViewer::showIQ	(int amount) {
std::complex<float> Values [amount];
int16_t	i;
int16_t	t;
double	avg	= 0;
int	scopeWidth	= scopeSlider -> value();

	t = iqBuffer -> getDataFromBuffer (Values, amount);
	if (myFrame -> isHidden())
	   return;

	for (i = 0; i < t; i ++) {
	   float x = abs (Values [i]);
	   if (!std::isnan (x) && !std::isinf (x))
	      avg += abs (Values [i]);
	}

	avg	/= t;
	myIQDisplay -> DisplayIQ (Values, scopeWidth / avg);
}

void	spectrumViewer:: showQuality (float q) {
	if (!myFrame -> isHidden())
	   quality_display -> display (q);
}

