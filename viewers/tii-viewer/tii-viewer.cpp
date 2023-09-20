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
#include	<QPen>
#include	"color-selector.h"
#include	"fft-complex.h"

	tiiViewer::tiiViewer	(RadioInterface	*mr,
	                         QSettings	*dabSettings,
	                         RingBuffer<std::complex<float>> *sbuffer):
	                              myFrame (nullptr),
	                              spectrumCurve (""),
	                              fft (TII_SPECTRUMSIZE, false) {
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
	colorString	= dabSettings -> value ("curveColor",
	                                                 "white"). toString();
	curveColor	= QColor (colorString);

	brush		= dabSettings -> value ("brush", 0). toInt () == 1;

	int x   = dabSettings -> value ("position-x", 100). toInt ();
        int y   = dabSettings -> value ("position-y", 100). toInt ();
	int w	= dabSettings -> value ("width", 150). toInt ();
	int h	= dabSettings -> value ("height", 120). toInt ();
        dabSettings -> endGroup ();
        setupUi (&myFrame);
	myFrame. resize (QSize (w, h));
        myFrame. move (QPoint (x, y));

	myFrame. hide();
	memset (displayBuffer, 0, TII_DISPLAYSIZE * sizeof (double));
	plotgrid	= tiiGrid;
	plotgrid	-> setCanvasBackground (displayColor);
//	grid		= new QwtPlotGrid;
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
	grid. attach (plotgrid);

	lm_picker       = new QwtPlotPicker (plotgrid -> canvas ());
        QwtPickerMachine *lpickerMachine =
                             new QwtPickerClickPointMachine ();

        lm_picker       -> setStateMachine (lpickerMachine);
        lm_picker       -> setMousePattern (QwtPlotPicker::MouseSelect1,
                                            Qt::RightButton);
        connect (lm_picker, SIGNAL (selected (const QPointF&)),
                 this, SLOT (rightMouseClick (const QPointF &)));

//	spectrumCurve	= new QwtPlotCurve ("");
   	spectrumCurve. setPen (QPen(curveColor, 2.0));
	spectrumCurve. setOrientation (Qt::Horizontal);
	spectrumCurve.  setBaseline	(get_db (0));
//	ourBrush	= new QBrush (curveColor);
//	ourBrush	-> setStyle (Qt::Dense3Pattern);
	if (brush) {
	   QBrush ourBrush (curveColor);
	   ourBrush. setStyle (Qt::Dense3Pattern);
	   spectrumCurve. setBrush (ourBrush);
	}
	spectrumCurve. attach (plotgrid);
	
	Marker		= new QwtPlotMarker();
	Marker		-> setLineStyle (QwtPlotMarker::VLine);
	Marker		-> setLinePen (QPen (Qt::red));
	Marker		-> attach (plotgrid);
	plotgrid	-> enableAxis (QwtPlot::yLeft);

	for (i = 0; i < TII_SPECTRUMSIZE; i ++) 
	   Window [i] =
	        0.42 - 0.5 * cos ((2.0 * M_PI * i) / (TII_SPECTRUMSIZE - 1)) +
	              0.08 * cos ((4.0 * M_PI * i) / (TII_SPECTRUMSIZE - 1));
	setBitDepth	(12);
}

	tiiViewer::~tiiViewer() {
	dabSettings	-> beginGroup ("tiiViewer");
        dabSettings	-> setValue ("position-x", myFrame. pos (). x ());
        dabSettings	-> setValue ("position-y", myFrame. pos (). y ());
	QSize size	= myFrame. size ();
	dabSettings	-> setValue ("width", size. width ());
	dabSettings	-> setValue ("height", size. height ());
	dabSettings	-> endGroup ();
	myFrame. hide();
	delete		Marker;
//	delete		ourBrush;
}

void	tiiViewer::clear	() {
double	X_axis [TII_DISPLAYSIZE];
double	Y_values [TII_DISPLAYSIZE];
double	temp	= (double)INPUT_RATE / 2 / TII_DISPLAYSIZE;

	if  (isHidden ())
	   return;

	for (int i = 0; i < TII_DISPLAYSIZE; i ++)
	   X_axis [i] = 
	         ((double)0 - (double)(INPUT_RATE / 2) +
	          (double)((i) * (double) 2 * temp)) / ((double)1000);
	for (int i = 0; i < TII_DISPLAYSIZE; i ++)
	   Y_values [i] = 0;

	ViewSpectrum (X_axis, Y_values,
	              AmplificationSlider -> value(),
	              0 / 1000);
	transmitterDisplay	-> setText ("");
}

static
QString tiiNumber (int n) {
	if (n >= 10)
	   return QString::number (n);
	return QString ("0") + QString::number (n);
}

void	tiiViewer::showTransmitters	(QByteArray transmitters) {
	if (myFrame. isHidden())
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
double	X_axis [TII_DISPLAYSIZE];
double	Y_values [TII_DISPLAYSIZE];
int16_t	i, j;
double	temp	= (double)INPUT_RATE / 2 / TII_DISPLAYSIZE;
int16_t	averageCount	= 3;

	(void)amount;
	if (tiiBuffer -> GetRingBufferReadAvailable() < TII_SPECTRUMSIZE) {
	   return;
	}

	tiiBuffer	-> getDataFromBuffer (spectrumBuffer, TII_SPECTRUMSIZE);
	tiiBuffer	-> FlushRingBuffer ();
	if (myFrame. isHidden ())
	   return;
//	and window it
//	first X axis labels
	for (i = 0; i < TII_DISPLAYSIZE; i ++)
	   X_axis [i] = 
	         ((double)0 - (double)(INPUT_RATE / 2) +
	          (double)((i) * (double) 2 * temp)) / ((double)1000);
//
//	get the buffer data
	for (i = 0; i < TII_SPECTRUMSIZE; i ++)
	   spectrumBuffer [i] = cmul (spectrumBuffer [i], Window [i]);

	fft. fft (spectrumBuffer);
//
//	and map the TII_SPECTRUMSIZE values onto TII_DISPLAYSIZE elements
	for (i = 0; i < TII_DISPLAYSIZE / 2; i ++) {
	   double f	= 0;
	   for (j = 0; j < TII_SPECTRUMSIZE / TII_DISPLAYSIZE; j ++)
	      f += abs (spectrumBuffer [TII_SPECTRUMSIZE / TII_DISPLAYSIZE * i + j]);

	   Y_values [TII_DISPLAYSIZE / 2 + i] = 
                                 f / (TII_SPECTRUMSIZE / TII_DISPLAYSIZE);
	   f = 0;
	   for (j = 0; j < TII_SPECTRUMSIZE / TII_DISPLAYSIZE; j ++)
	      f += abs (spectrumBuffer [TII_SPECTRUMSIZE / 2 +
	                             TII_SPECTRUMSIZE / TII_DISPLAYSIZE * i + j]);
	   Y_values [i] = f / (TII_SPECTRUMSIZE / TII_DISPLAYSIZE);
	}
//
//	average the image a little.
	for (i = 0; i < TII_DISPLAYSIZE; i ++) {
	   if (std::isnan (Y_values [i]) || std::isinf (Y_values [i]))
	      continue;
	   displayBuffer [i] = 
	          (double)(averageCount - 1) /averageCount * displayBuffer [i] +
	           1.0f / averageCount * Y_values [i];
	}

	memcpy (Y_values, displayBuffer, TII_DISPLAYSIZE * sizeof (double));
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
				         X_axis [TII_DISPLAYSIZE - 1]);
	plotgrid	-> enableAxis (QwtPlot::xBottom);
	plotgrid	-> setAxisScale (QwtPlot::yLeft,
				         get_db (0), get_db (0) + amp1 * 40);
//				         get_db (0), get_db (0) + amp);
//				         get_db (0), 0);

	for (i = 0; i < TII_DISPLAYSIZE; i ++) 
	   Y1_value [i] = get_db (amp1 * Y1_value [i]); 

	spectrumCurve. setBaseline (get_db (0));
	Y1_value [0]		= get_db (0);
	Y1_value [TII_DISPLAYSIZE - 1] = get_db (0);

	spectrumCurve. setSamples (X_axis, Y1_value, TII_DISPLAYSIZE);
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

void	tiiViewer::show () {
	myFrame. show ();
}

void	tiiViewer::hide () {
	myFrame. hide ();
}

bool	tiiViewer::isHidden () {
	return myFrame. isHidden ();
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
	spectrumCurve. setPen (QPen (this -> curveColor, 2.0));
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
	plotgrid	-> setCanvasBackground (this -> displayColor);
}

static
double	Y_values [2000] = {0};
void	tiiViewer::show_nullPeriod (const QVector<float> &v, double  amp2) {
double	X_axis [TII_DISPLAYSIZE];
//double	amp	= AmplificationSlider -> value();


	for (int i = 0; i < TII_DISPLAYSIZE; i ++) {
	   X_axis [i] = i * v. size () / TII_DISPLAYSIZE;;
	   double x = 0;
	   for (int j = 0; j < v. size () / TII_DISPLAYSIZE; j ++)
	      x = abs (v [i * v. size () / TII_DISPLAYSIZE + j]);
	   Y_values [i] = x;
	}

	float Max	= 0;
	for (int i = 0; i < TII_DISPLAYSIZE; i ++)
	   if (Y_values [i] > Max)
	      Max = Y_values [i];

	plotgrid        -> setAxisScale (QwtPlot::xBottom,
                                         (double)X_axis [0],
                                         X_axis [TII_DISPLAYSIZE - 1]);
        plotgrid        -> enableAxis (QwtPlot::xBottom);
        plotgrid        -> setAxisScale (QwtPlot::yLeft,
                                         0, Max * 2);
//                                       get_db (0), 0);
//	for (int i = 0; i < TII_DISPLAYSIZE; i ++) 
//	   Y_values [i] = get_db (amp * Y_values [i]); 

	spectrumCurve. setBaseline (0);
	Y_values [0]		= 0;
	Y_values [TII_DISPLAYSIZE - 1] = 0;

	spectrumCurve. setSamples (X_axis, Y_values, TII_DISPLAYSIZE);
	plotgrid	-> replot(); 
}

