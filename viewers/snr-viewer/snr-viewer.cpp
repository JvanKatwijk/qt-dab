#
/*
 *    Copyright (C)  2014 .. 2020
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

#include	<QFileDialog>
#include	<QMessageBox>
#include	"snr-viewer.h"
#include	<QSettings>
#include	<QColor>
#include	<QPen>
#include	<string.h>
#include	"color-selector.h"

	snrViewer::snrViewer	(RadioInterface	*mr,
	                         QSettings	*s):
	                             myFrame (nullptr),
	                             spectrum_curve ("") {
QString	colorString	= "black";
	this		-> myRadioInterface	= mr;
	this		-> dabSettings		= s;

	dabSettings	-> beginGroup ("snrViewer");
	plotLength	= dabSettings -> value ("snrLength", 312). toInt ();
	plotHeight	= dabSettings -> value ("snrHeight", 15). toInt ();
	delayCount	= dabSettings	-> value ("snrDelay", 5). toInt ();
	colorString	= dabSettings -> value ("displayColor", "black").
	                                                       toString ();
	displayColor	= QColor (colorString);
	colorString	= dabSettings -> value ("gridColor",
	                                               "white"). toString();
	gridColor	= QColor (colorString);
	colorString	= dabSettings -> value ("curveColor",
	                                                "white"). toString();
	curveColor	= QColor (colorString);
	dabSettings	-> endGroup ();
	setupUi (&myFrame);

	snrLengthSelector -> setValue (plotLength);
	snrSlider	-> setValue (plotHeight);
	snrCompressionSelector -> setValue (delayCount);
	snrDumpFile. store (nullptr);
	connect (snrDumpButton, SIGNAL (clicked ()),
	         this, SLOT (handle_snrDumpButton ()));
	connect (snrSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_snrHeight (int)));
	connect (snrCompressionSelector, SIGNAL (valueChanged (int)),
	         this, SLOT (set_snrDelay (int)));
	connect (snrLengthSelector, SIGNAL (valueChanged (int)),
	         this, SLOT (set_snrLength (int)));
	plotgrid	= snrPlot;
	plotgrid	-> setCanvasBackground (displayColor);
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

	lm_picker	= new QwtPlotPicker (plotgrid -> canvas ());
	lpickerMachine =
	                  new QwtPickerClickPointMachine ();
	lm_picker	-> setStateMachine (lpickerMachine);
	lm_picker	-> setMousePattern (QwtPlotPicker::MouseSelect1,
	                                            Qt::RightButton);
	connect (lm_picker, SIGNAL (selected (const QPointF &)),
	         this, SLOT (rightMouseClick (const QPointF &)));

   	spectrum_curve. setPen (QPen(curveColor, 2.0));
	spectrum_curve. setOrientation (Qt::Horizontal);
	spectrum_curve. setBaseline	(0);
	spectrum_curve. attach (plotgrid);
	plotgrid	-> enableAxis (QwtPlot::yLeft);
	plotgrid	-> setAxisScale (QwtPlot::yLeft,
				         0, plotHeight);
	plotgrid	-> setAxisScale (QwtPlot::xBottom,
	                                 0, plotLength - 1);
	plotgrid	-> enableAxis (QwtPlot::xBottom);
	X_axis.   resize (plotLength);
	Y_Buffer. resize (plotLength);
	for (int i = 0; i < plotLength; i ++) {
	   X_axis  [i] = i;
	   Y_Buffer [i] = 0;
	}
	delayBufferP	= 0;
}

	snrViewer::~snrViewer () {
	stopDumping 	();
//	delete lm_picker;
//	delete lpickerMachine;
}


void	snrViewer::show () {
	myFrame. show();
}

void	snrViewer::hide	() {
	myFrame. hide();
	stopDumping ();
}

bool	snrViewer::isHidden () {
	return myFrame. isHidden();
}

void	snrViewer::add_snr	(float snr) {
static float delayBuffer [10];
float	sum	= 0;
	delayBuffer [delayBufferP] = snr;
	delayBufferP = (delayBufferP + 1) % delayCount;
	if (delayBufferP == 0) {
	   for (int i = 0; i < delayCount; i ++)
	      sum += delayBuffer [i];
	   sum /= delayCount;
	   addtoView (sum);
	}
}

#define	VIEWBUFFER_SIZE 5
void	snrViewer::addtoView (float v) {
static float displayBuffer [VIEWBUFFER_SIZE];
static int displayPointer = 0;

	displayBuffer [displayPointer] = v;
	displayPointer = (displayPointer + 1) % VIEWBUFFER_SIZE;
	if (displayPointer == 0) {
	   memmove (&(Y_Buffer. data () [VIEWBUFFER_SIZE]),
	                   &(Y_Buffer. data () [0]),
	                   (plotLength - VIEWBUFFER_SIZE) * sizeof (double));
	   for (int i = 0; i < VIEWBUFFER_SIZE; i ++)
	      Y_Buffer [i] = displayBuffer [i];
	   if (snrDumpFile. load () != nullptr)
	      fwrite (displayBuffer, sizeof (float), 
	                             VIEWBUFFER_SIZE, snrDumpFile. load ());
	}
}

void	snrViewer::add_snr	(float sig, float noise) {
float snr = 20 * log10 ((sig + 0.001) / (noise + 0.001));
	memmove (&(Y_Buffer. data () [1]), &(Y_Buffer. data () [0]),
	                               (plotLength - 1) * sizeof (double));
	Y_Buffer [0]	= snr;
	if (snrDumpFile. load () != nullptr)
	   fwrite (&snr, sizeof (float), 1, snrDumpFile. load ());
}

void	snrViewer::show_snr () {
	spectrum_curve. setSamples (X_axis. data (),
	                           Y_Buffer. data (), plotLength);
	plotgrid	-> replot (); 
}

float	snrViewer::get_db (float x) {
	return 20 * log10 ((x + 1) / (float)(4 * 512));
}

void	snrViewer::rightMouseClick	(const QPointF &point) {
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

	dabSettings	-> beginGroup ("snrViewer");
	dabSettings	-> setValue ("displayColor", displayColor);
	dabSettings	-> setValue ("gridColor", gridColor);
	dabSettings	-> setValue ("curveColor", curveColor);
	dabSettings	-> endGroup ();

	this		-> displayColor	= QColor (displayColor);
	this		-> gridColor	= QColor (gridColor);
	this		-> curveColor	= QColor (curveColor);
	spectrum_curve. setPen (QPen(this -> curveColor, 2.0));
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid. setMajPen (QPen(this -> gridColor, 0,
	                                                   Qt::DotLine));
#else
	grid. setMajorPen (QPen(this -> gridColor, 0,
	                                                   Qt::DotLine));
#endif
	grid. enableXMin (true);
	grid. enableYMin (true);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid. setMinPen (QPen(this -> gridColor, 0,
	                                                   Qt::DotLine));
#else
	grid. setMinorPen (QPen(this -> gridColor, 0,
	                                                   Qt::DotLine));
#endif
	plotgrid	->  setCanvasBackground (this -> displayColor);
}

void	snrViewer::handle_snrDumpButton () {
	if (snrDumpFile. load () != nullptr) {
	   stopDumping ();
	   return;
	}
	startDumping ();
}

void	snrViewer::stopDumping () {
	if (snrDumpFile. load () != nullptr) {
	   fclose (snrDumpFile. load ());
	   snrDumpFile. store (nullptr);
	   snrDumpButton	-> setText ("dump");
	}
}

void	snrViewer::startDumping () {
	QString fileName = QFileDialog::
	                     getSaveFileName (&myFrame,
                                              tr ("Open file ..."),
                                              QDir::homePath(),
                                              tr ("snr (*.snr)"),
	                                      Q_NULLPTR,
	                                      QFileDialog::DontUseNativeDialog);

	if (fileName == QString (""))  // canceled
	   return;

	fileName = QDir::toNativeSeparators (fileName);

	FILE *file = fopen (fileName. toUtf8 (). data (), "w+b");
	if (file == nullptr) {
	   QMessageBox::warning (&myFrame, tr ("Warning"),
	                            tr ("could not open file"));
	   return;
	}
	snrDumpFile. store (file);
	snrDumpButton -> setText ("dumping");
}


void	snrViewer::set_snrDelay	(int d) {
	if ((0 <= d) && (d <= 10))
	   delayCount = d;
	else
	   delayCount = 5;
	delayBufferP	= 0;
	dabSettings	-> beginGroup ("snrViewer");
	dabSettings     -> setValue ("snrDelay", d);
	dabSettings	-> endGroup ();

}

void	snrViewer::set_snrHeight	(int n) {
	plotHeight	= n;
	dabSettings	-> beginGroup ("snrViewer");
	dabSettings	-> setValue ("snrHeight", n);
	dabSettings	-> endGroup ();
	plotgrid	-> setAxisScale (QwtPlot::yLeft,
				              0, plotHeight);
	plotgrid	-> enableAxis (QwtPlot::yLeft);
//	spectrum_curve. setBaseline  (0);
}

void	snrViewer::set_snrLength	(int n) {
	Y_Buffer. resize (n);
	X_axis. resize (n);
	if (n > plotLength) {
	   for (int i = plotLength; i < n; i ++) {
	      Y_Buffer [i] = 0;
	      X_axis [i] = i;
	   }
	}
	plotLength = n;
	dabSettings	-> beginGroup ("snrViewer");
	dabSettings	-> setValue ("snrLength", plotLength);
	dabSettings	-> endGroup ();
	plotgrid	-> setAxisScale (QwtPlot::xBottom,
	                                           0, plotLength - 1);
	plotgrid	-> enableAxis (QwtPlot::xBottom);
}
