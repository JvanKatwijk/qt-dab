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

#include	"tii-viewer.h"
#include	"iqdisplay.h"
#include	<QColor>
#include	"color-selector.h"

	tiiViewer::tiiViewer	(RadioInterface	*mr,
	                         QSettings	*dabSettings,
	                         RingBuffer<std::complex<float>> *sbuffer) {
int16_t	i;
QString	colorString	= "black";
bool	brush;

	this	-> myRadioInterface	= mr;
	this	-> dabSettings		= dabSettings;
	this	-> tiiBuffer		= sbuffer;

	dabSettings	-> beginGroup ("tiiViewer");
	colorString	= dabSettings -> value ("displayColor",
	                                                 "black"). toString();
	displayColor	= QColor (colorString);
	colorString	= dabSettings -> value ("gridColor",
	                                                 "black"). toString();
	gridColor	= QColor (colorString);
	colorString        = dabSettings -> value ("curveColor",
	                                                 "white"). toString();
	curveColor	= QColor (colorString);

	brush		= dabSettings -> value ("brush", 0). toInt () == 1;
	displaySize	= dabSettings -> value ("displaySize",
	                                                   1024).toInt();
	dabSettings	-> endGroup ();
	if ((displaySize & (displaySize - 1)) != 0)
	   displaySize = 1024;

	this	-> myFrame		= new QFrame (nullptr);
	setupUi (this -> myFrame);

	this	-> myFrame	-> hide();
	displayBuffer. resize (displaySize);
	memset (displayBuffer. data(), 0, displaySize * sizeof (double));
	this	-> spectrumSize	= 2 * displaySize;
	spectrum		= (std::complex<float> *)fftwf_malloc (sizeof (fftwf_complex) * spectrumSize);
        plan    = fftwf_plan_dft_1d (spectrumSize,
                                    reinterpret_cast <fftwf_complex *>(spectrum),
                                    reinterpret_cast <fftwf_complex *>(spectrum),
                                    FFTW_FORWARD, FFTW_ESTIMATE);
	
	plotgrid	= tiiGrid;
	plotgrid	-> setCanvasBackground (displayColor);
	grid		= new QwtPlotGrid;
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid		-> setMajPen (QPen(gridColor, 0, Qt::DotLine));
#else
	grid		-> setMajorPen (QPen(gridColor, 0, Qt::DotLine));
#endif
	grid		-> enableXMin (true);
	grid		-> enableYMin (true);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid		-> setMinPen (QPen(gridColor, 0, Qt::DotLine));
#else
	grid		-> setMinorPen (QPen(gridColor, 0, Qt::DotLine));
#endif
	grid		-> attach (plotgrid);

	lm_picker       = new QwtPlotPicker (plotgrid -> canvas ());
        QwtPickerMachine *lpickerMachine =
                             new QwtPickerClickPointMachine ();

        lm_picker       -> setStateMachine (lpickerMachine);
        lm_picker       -> setMousePattern (QwtPlotPicker::MouseSelect1,
                                            Qt::RightButton);
        connect (lm_picker, SIGNAL (selected (const QPointF&)),
                 this, SLOT (rightMouseClick (const QPointF &)));

	spectrumCurve	= new QwtPlotCurve ("");
   	spectrumCurve	-> setPen (QPen(curveColor, 2.0));
	spectrumCurve	-> setOrientation (Qt::Horizontal);
	spectrumCurve	-> setBaseline	(get_db (0));
	ourBrush	= new QBrush (curveColor);
	ourBrush	-> setStyle (Qt::Dense3Pattern);
	if (brush)
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
}

    tiiViewer::~tiiViewer() {
	fftwf_destroy_plan (plan);
	fftwf_free	(spectrum);
	myFrame		-> hide();
	delete		Marker;
	delete		ourBrush;
	delete		spectrumCurve;
	delete		grid;
	delete		myFrame;
}

void	tiiViewer::clear() {
	transmitterDisplay	-> setText (" ");
}

static
QString tiiNumber (int n) {
	if (n >= 10)
	   return QString::number (n);
	return QString ("0") + QString::number (n);
}

void	tiiViewer::showTransmitters	(QByteArray transmitters) {
	if (myFrame	-> isHidden())
	   return;

	if (transmitters. size () == 0) {
	   transmitterDisplay	-> setText (" ");
	   return;
	}

	QString t	= "transmitter IDs ";
	for (int i = 0; i < transmitters. size (); i += 2) {
	   uint8_t mainId	= transmitters. at (i);
	   uint8_t subId	= transmitters. at (i + 1);
	   t = t + " (" + tiiNumber (mainId) +
                    "+" + tiiNumber (subId) + ")";
        }
	transmitterDisplay	-> setText (t);
}

void	tiiViewer::showSpectrum	(int32_t amount) {
double	X_axis [displaySize];
double	Y_values [displaySize];
int16_t	i, j;
double	temp	= (double)INPUT_RATE / 2 / displaySize;
int16_t	averageCount	= 3;

	   
	(void)amount;
	if (tiiBuffer -> GetRingBufferReadAvailable() < spectrumSize) {
	   return;
	}
	tiiBuffer	-> getDataFromBuffer (spectrum, spectrumSize);
	tiiBuffer	-> FlushRingBuffer ();
	if (myFrame	-> isHidden ())
	   return;
//	and window it
//	first X axis labels
	for (i = 0; i < displaySize; i ++)
	   X_axis [i] = 
	         ((double)0 - (double)(INPUT_RATE / 2) +
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
	              AmplificationSlider -> value(),
	              0 / 1000);
}

void	tiiViewer::ViewSpectrum (double *X_axis,
		                       double *Y1_value,
	                               double amp,
	                               int32_t marker) {
uint16_t	i;
float	amp1	= amp / 100;

	amp		= amp / 50.0 * (-get_db (0));
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

float	tiiViewer::get_db (float x) {
	return 20 * log10 ((x + 1) / (float)(normalizer));
}

void	tiiViewer::setBitDepth	(int16_t d) {

	if (d < 0 || d > 32)
	   d = 24;

	normalizer	= 1;
	while (-- d > 0) 
	   normalizer <<= 1;
}

void	tiiViewer::show() {
	myFrame	-> show();
}

void	tiiViewer::hide() {
	myFrame	-> hide();
}

bool	tiiViewer::isHidden() {
	return myFrame -> isHidden();
}

void	tiiViewer::rightMouseClick	(const QPointF &point) {
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

	dabSettings	-> beginGroup ("tiiViewer");
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
	grid		-> setMajorPen (QPen(this -> gridColor, 0,
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

