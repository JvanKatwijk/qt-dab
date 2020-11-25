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

#include	<QFileDialog>
#include	<QMessageBox>
#include	"snr-viewer.h"
#include	<QSettings>
#include	<QColor>
#include	"color-selector.h"

	snrViewer::snrViewer	(RadioInterface	*mr,
	                         QSettings	*s) {
QString	colorString	= "black";
bool	brush;
	this	-> myRadioInterface	= mr;
	this	-> dabSettings		= s;

	dabSettings	-> beginGroup ("snrViewer");
	plotLength	= dabSettings -> value ("snrLength", 312). toInt ();
	plotHeight	= dabSettings -> value ("snrHeight", 15). toInt ();
	         
	colorString	= dabSettings -> value ("displayColor",
	                                              "black"). toString();
	displayColor	= QColor (colorString);
	colorString	= dabSettings -> value ("gridColor",
	                                               "white"). toString();
	gridColor	= QColor (colorString);
	colorString	= dabSettings -> value ("curveColor",
	                                                "white"). toString();
	curveColor	= QColor (colorString);
	brush		= dabSettings -> value ("brush", 0). toInt () == 1;
	dabSettings	-> endGroup ();
	myFrame				= new QFrame;
	setupUi (this -> myFrame);
#ifdef	__DUMP_SNR__
	snrDumpFile. store (nullptr);
	connect (snrDumpButton, SIGNAL (clicked ()),
	         this, SLOT (handle_snrDumpButton ()));
#else
	snrDumpButton 	-> hide ();
#endif
	plotgrid	= snrPlot;
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
	spectrumCurve	-> setBaseline	(0);
	ourBrush	= new QBrush (curveColor);
	ourBrush	-> setStyle (Qt::Dense3Pattern);
	if (brush)
	   spectrumCurve	-> setBrush (*ourBrush);
	spectrumCurve	-> attach (plotgrid);
	plotgrid	-> enableAxis (QwtPlot::yLeft);
	Y_Buffer. resize (plotLength);
	for (int i = 0; i < plotLength; i ++)
	   Y_Buffer [i] = 0;
}

	snrViewer::~snrViewer() {
#ifdef	__DUMP_SNR__
	stopDumping 	();
#endif
	myFrame		-> hide();
	delete		ourBrush;
	delete		spectrumCurve;
	delete		grid;
	delete		myFrame;
}

void	snrViewer::setHeight	(int n) {
	plotHeight	= n;
	dabSettings	-> beginGroup ("snrViewer");
	dabSettings	-> setValue ("snrHeight", n);
	dabSettings	-> endGroup ();
}

void	snrViewer::setLength	(int n) {
	if (n < plotLength) {
	   plotLength = n;
	   Y_Buffer. resize (n);
	}
	else
	if (n > plotLength) {
	   Y_Buffer. resize (n);
	   for (int i = plotLength; i < n; i ++)
	      Y_Buffer [i] = 0;
	   plotLength = n;
	}
	dabSettings	-> beginGroup ("snrViewer");
	dabSettings	-> setValue ("snrLength", plotLength);
	dabSettings	-> endGroup ();
}

void	snrViewer::show () {
	myFrame		-> show();
}

void	snrViewer::hide	() {
	myFrame		-> hide();
#ifdef	__DUMP_SNR__
	stopDumping ();
#endif
}

bool	snrViewer::isHidden() {
	return myFrame	-> isHidden();
}

void	snrViewer::add_snr	(float snr) {
	for (int i = plotLength - 1; i > 0; i --)
	   Y_Buffer [i] = Y_Buffer [i - 1];
	Y_Buffer [0]	= snr;
#ifdef	__DUMP_SNR__
	if (snrDumpFile. load () != nullptr)
	   fwrite (&snr, sizeof (float), 1, snrDumpFile. load ());
#endif
}

void	snrViewer::show_snr () {
double X_axis	[plotLength];
	for (int i = 0; i < plotLength; i ++) 
	   X_axis [i] = i;

	plotgrid	-> setAxisScale (QwtPlot::xBottom,
	                                 0, plotLength - 1);
	plotgrid	-> enableAxis (QwtPlot::xBottom);
	plotgrid	-> setAxisScale (QwtPlot::yLeft,
				         0, plotHeight);
	plotgrid	-> enableAxis (QwtPlot::yLeft);
	spectrumCurve   -> setBaseline  (0);

	spectrumCurve	-> setSamples (X_axis, Y_Buffer. data (), plotLength);
	plotgrid	-> replot(); 
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
	spectrumCurve	-> setPen (QPen(this -> curveColor, 2.0));
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

#ifdef	__DUMP_SNR__
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
	QString fileName = QFileDialog::getSaveFileName (myFrame,
                                                       tr ("Open file ..."),
                                                       QDir::homePath(),
                                                       tr ("snr (*.snr)"));
	if (fileName == QString ("")) {
	   QMessageBox::warning (myFrame, tr ("Warning"),
	                            tr ("no file selected"));
	   return;
	}
	fileName = QDir::toNativeSeparators (fileName);

	FILE *file = fopen (fileName. toLatin1 (). data (), "w+b");
	if (file == nullptr) {
	   QMessageBox::warning (myFrame, tr ("Warning"),
	                            tr ("could not open file"));
	   return;
	}
	snrDumpFile. store (file);
	snrDumpButton -> setText ("dumping");
}
#endif
