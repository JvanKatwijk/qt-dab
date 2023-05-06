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

#include	"spectrum-viewer.h"
#include	<QSettings>
#include	"iqdisplay.h"
#include	<QColor>
#include	<QPen>
#include	"color-selector.h"

	spectrumViewer::spectrumViewer	(RadioInterface	*mr,
	                                 QSettings	*dabSettings,
	                                 RingBuffer<std::complex<float>> *sbuffer,
	                                 RingBuffer<std::complex<float>>* ibuffer):
	                                 myFrame (nullptr) {
int16_t	i;

	this	-> myRadioInterface	= mr;
	this	-> dabSettings		= dabSettings;
	this	-> spectrumBuffer	= sbuffer;
	this	-> iqBuffer		= ibuffer;

	dabSettings	-> beginGroup ("spectrumViewer");
	int x   = dabSettings -> value ("position-x", 100). toInt ();
        int y   = dabSettings -> value ("position-y", 100). toInt ();
	int w	= dabSettings -> value ("width", 150). toInt ();
	int h	= dabSettings -> value ("height", 120). toInt ();
        dabSettings -> endGroup ();
        setupUi (&myFrame);
	myFrame. resize (QSize (w, h));
        myFrame. move (QPoint (x, y));
	
	QPoint  pos     = myFrame. mapToGlobal (QPoint (0, 0));
	fprintf (stderr, "spectrumViewer gezet op %d %d, staat op %d %d\n",
	                                x, y, pos. x (), pos. y ());
	myFrame. hide();
	displayBuffer. resize (256);
	displaySize	= 256;
	spectrumSize	= 4 * displaySize;
	spectrum		= (std::complex<float> *)
	               fftwf_malloc (sizeof (fftwf_complex) * spectrumSize);
        plan    = fftwf_plan_dft_1d (spectrumSize,
                                  reinterpret_cast <fftwf_complex *>(spectrum),
                                  reinterpret_cast <fftwf_complex *>(spectrum),
                                  FFTW_FORWARD, FFTW_ESTIMATE);
	
	Window. resize (spectrumSize);
	for (i = 0; i < spectrumSize; i ++) 
	   Window [i] =
	        0.42 - 0.5 * cos ((2.0 * M_PI * i) / (spectrumSize - 1)) +
	              0.08 * cos ((4.0 * M_PI * i) / (spectrumSize - 1));
	mySpectrumScope	= new spectrumScope (dabScope, 256, dabSettings);
	myWaterfallScope	= new waterfallScope (dabWaterfall, 256, 50);
	myIQDisplay	= new IQDisplay (iqDisplay, 512);
	myNullScope	= new nullScope	(nullDisplay, 256, dabSettings);
	setBitDepth	(12);
}

	spectrumViewer::~spectrumViewer () {
	dabSettings	-> beginGroup ("spectrumViewer");
        dabSettings	-> setValue ("position-x", myFrame. pos (). x ());
        dabSettings	-> setValue ("position-y", myFrame. pos (). y ());

	QSize size	= myFrame. size ();
	dabSettings	-> setValue ("width", size. width ());
	dabSettings	-> setValue ("height", size. height ());
	dabSettings	-> endGroup ();
	fftwf_destroy_plan (plan);
	fftwf_free	(spectrum);
	myFrame. hide();
	delete		myIQDisplay;
	delete		mySpectrumScope;
	delete		myWaterfallScope;
	delete		myNullScope;
}

void	spectrumViewer::showSpectrum	(int32_t amount, int32_t vfoFrequency) {
double	X_axis [displaySize];
double	Y_values [displaySize];
double	Y2_values [displaySize];
int16_t	i, j;
double	temp	= (double)INPUT_RATE / 2 / displaySize;
int16_t	averageCount	= 5;
	   
	(void)amount;
	if (spectrumBuffer -> GetRingBufferReadAvailable() < spectrumSize)
	   return;

	spectrumBuffer	-> getDataFromBuffer (spectrum, spectrumSize);
	spectrumBuffer	-> FlushRingBuffer();
	if (myFrame. isHidden()) {
	   spectrumBuffer	-> FlushRingBuffer();
	   return;
	}

//	first X axis labels
	for (i = 0; i < displaySize; i ++)
	   X_axis [i] = 
	         ((double)vfoFrequency - (double)(INPUT_RATE / 2) +
	          (double)((i) * (double) 2 * temp)) / ((double)1000);
//
//	and window it
//	get the buffer data
	for (i = 0; i < spectrumSize; i ++)
	   if (std::isnan (abs (spectrum [i])) ||
	                 std::isinf (abs (spectrum [i])))
	      spectrum [i] = std::complex<float> (0, 0);
	   else
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
	        displayBuffer. data (), displaySize * sizeof (double));
	memcpy (Y2_values,
	        displayBuffer. data (), displaySize * sizeof (double));
	mySpectrumScope	-> showSpectrum (X_axis, Y_values,
	              scopeAmplification -> value (),
	              vfoFrequency / 1000);
	myWaterfallScope -> display (X_axis, Y2_values, 
	              dabWaterfallAmplitude -> value (), 
	              vfoFrequency / 1000);
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

	mySpectrumScope	-> setBitDepth (normalizer);
}

void	spectrumViewer::show () {
	myFrame. show ();
}

void	spectrumViewer::hide () {
	myFrame. hide ();
}

bool	spectrumViewer::isHidden () {
	return myFrame. isHidden ();
}

void	spectrumViewer::showIQ	(int amount) {
std::complex<float> Values [amount];
int16_t	i;
int16_t	t;
double	avg	= 0;
int	scopeWidth	= scopeSlider -> value();

	t = iqBuffer -> getDataFromBuffer (Values, amount);
	if (myFrame. isHidden())
	   return;

	for (i = 0; i < t; i ++) {
	   float x = abs (Values [i]);
	   if (!std::isnan (x) && !std::isinf (x))
	      avg += abs (Values [i]);
	}

	avg	/= t;
	myIQDisplay -> DisplayIQ (Values, 512, scopeWidth / avg);
//	myIQDisplay -> DisplayIQ (Values, amount, scopeWidth / avg);
}

void	spectrumViewer:: showQuality (float q, float timeOffset,	
	                              float freqOffset) {
	if (myFrame. isHidden ())
	   return;

	quality_display -> display (q);
	timeOffsetDisplay	-> display (timeOffset);
	frequencyOffsetDisplay	-> display (freqOffset);
}

void	spectrumViewer::show_snr	(float snr) {
	if (myFrame. isHidden ())
	   return;
	snrDisplay		-> display (snr);
}

void	spectrumViewer::show_correction	(int c) {
	if (myFrame. isHidden ())
	   return;
	correctorDisplay	-> display (c);
}

void	spectrumViewer::show_clockErr	(int e) {
	if (!myFrame. isHidden ())
	   clockError -> display (e);
}

void	spectrumViewer::show_nullPeriod	(float *v, int amount) {
	myNullScope	-> show_nullPeriod (v, amount);
}

void	spectrumViewer::rightMouseClick	(const QPointF &point) {
colorSelector *selector;
int	index;
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

	dabSettings	-> beginGroup ("spectrumViewer");
	dabSettings	-> setValue ("displayColor", displayColor);
	dabSettings	-> setValue ("gridColor", gridColor);
	dabSettings	-> setValue ("curveColor", curveColor);
	dabSettings	-> endGroup ();

	this		-> displayColor	= QColor (displayColor);
	this		-> gridColor	= QColor (gridColor);
	this		-> curveColor	= QColor (curveColor);
	spectrumCurve	-> setPen (QPen (this -> curveColor, 2.0));
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid		-> setMajPen (QPen(this -> gridColor, 0,
	                                                   Qt::DotLine));
#else
	grid		-> setMajorPen (QPen (this -> gridColor, 0,
	                                                   Qt::DotLine));
#endif
	grid		-> enableXMin (true);
	grid		-> enableYMin (true);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid		-> setMinPen (QPen(this -> gridColor, 0,
	                                                   Qt::DotLine));
#else
	grid		-> setMinorPen (QPen(this -> gridColor, 0,
	                                                   Qt::DotLine));
#endif
	plotgrid	-> setCanvasBackground (this -> displayColor);
}

