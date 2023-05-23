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
	                                 myFrame (nullptr),
	                                 fft (SP_SPECTRUMSIZE, false) {
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
	for (i = 0; i < SP_SPECTRUMSIZE; i ++) 
	   Window [i] =
	        0.42 - 0.5 * cos ((2.0 * M_PI * i) / (SP_SPECTRUMSIZE - 1)) +
	              0.08 * cos ((4.0 * M_PI * i) / (SP_SPECTRUMSIZE - 1));

	mySpectrumScope	= new spectrumScope (dabScope,
	                                     SP_DISPLAYSIZE, dabSettings);
	myWaterfallScope	= new waterfallScope (dabWaterfall,
	                                     SP_DISPLAYSIZE, 50);
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
	myFrame. hide();
	delete		myIQDisplay;
	delete		mySpectrumScope;
	delete		myWaterfallScope;
	delete		myNullScope;
}

void	spectrumViewer::showSpectrum	(int32_t amount, int32_t vfoFrequency) {
double	X_axis [SP_DISPLAYSIZE];
double	Y_values [SP_DISPLAYSIZE];
double	Y2_values [SP_DISPLAYSIZE];
double	temp	= (double)INPUT_RATE / 2 / SP_DISPLAYSIZE;
int16_t	averageCount	= 5;
	   
	(void)amount;
	if (spectrumBuffer -> GetRingBufferReadAvailable() < SP_SPECTRUMSIZE)
	   return;

	spectrumBuffer	-> getDataFromBuffer (spectrum, SP_SPECTRUMSIZE);
	spectrumBuffer	-> FlushRingBuffer();
	if (myFrame. isHidden()) {
	   spectrumBuffer	-> FlushRingBuffer();
	   return;
	}

//	first X axis labels
	for (int i = 0; i < SP_DISPLAYSIZE; i ++)
	   X_axis [i] = 
	         ((double)vfoFrequency - (double)(INPUT_RATE / 2) +
	          (double)((i) * (double) 2 * temp)) / ((double)1000);
//
//	and window it
//	get the buffer data
	for (int i = 0; i < SP_SPECTRUMSIZE; i ++)
	   if (std::isnan (abs (spectrum [i])) ||
	                 std::isinf (abs (spectrum [i])))
	      spectrum [i] = std::complex<float> (0, 0);
	   else
	      spectrum [i] = cmul (spectrum [i], Window [i]);

	fft. fft (spectrum);
//
//	and map the SP_SPECTRUMSIZE values onto SP_DISPLAYSIZE elements
	for (int i = 0; i < SP_DISPLAYSIZE / 2; i ++) {
	   double f	= 0;
	   for (int j = 0; j < SP_SPECTRUMSIZE / SP_DISPLAYSIZE; j ++)
	      f += abs (spectrum [SP_SPECTRUMSIZE / SP_DISPLAYSIZE * i + j]);

	   Y_values [SP_DISPLAYSIZE / 2 + i] = 
                                 f / (SP_SPECTRUMSIZE / SP_DISPLAYSIZE);
	   f = 0;
	   for (int j = 0; j < SP_SPECTRUMSIZE / SP_DISPLAYSIZE; j ++)
	      f += abs (spectrum [SP_SPECTRUMSIZE / 2 +
	                             SP_SPECTRUMSIZE / SP_DISPLAYSIZE * i + j]);
	   Y_values [i] = f / (SP_SPECTRUMSIZE / SP_DISPLAYSIZE);
	}
//
//	average the image a little.
	for (int i = 0; i < SP_DISPLAYSIZE; i ++) {
	   if (std::isnan (Y_values [i]) || std::isinf (Y_values [i]))
	      continue;

	   displayBuffer [i] = 
	          (double)(averageCount - 1) /averageCount * displayBuffer [i] +
	           1.0f / averageCount * Y_values [i];
	}

	memcpy (Y_values,
	        displayBuffer, SP_DISPLAYSIZE * sizeof (double));
	memcpy (Y2_values,
	        displayBuffer, SP_DISPLAYSIZE * sizeof (double));
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

void	spectrumViewer::showFrequency (float f) {
	frequencyDisplay	-> display (f);
}

