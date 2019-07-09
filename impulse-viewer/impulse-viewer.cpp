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

#include	"impulse-viewer.h"
#include	<QSettings>
#include	<QColor>

	impulseViewer::impulseViewer	(RadioInterface	*mr,
	                                 RingBuffer<float> *b) {
int16_t	i;
QString	colorString	= "black";
QColor	displayColor;
QColor	gridColor;
QColor	curveColor;

	this	-> myRadioInterface	= mr;
	this	-> responseBuffer	= b;
	colorString			= "black";
	displayColor			= QColor (colorString);
	colorString			= "white";
	gridColor			= QColor (colorString);
	curveColor			= QColor ("red");
	myFrame				= new QFrame;
	setupUi (this -> myFrame);

	plotgrid			= impulseGrid;
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
	spectrumCurve	-> setBaseline	(0);
	ourBrush	= new QBrush (Qt::white);
	ourBrush	-> setStyle (Qt::Dense3Pattern);
	spectrumCurve	-> setBrush (*ourBrush);
	spectrumCurve	-> attach (plotgrid);
	plotgrid	-> enableAxis (QwtPlot::yLeft);
}

	impulseViewer::~impulseViewer() {
	myFrame		-> hide();
	delete		ourBrush;
	delete		spectrumCurve;
	delete		grid;
	delete		myFrame;
}

void	impulseViewer::showIndex	(int32_t v) {
int32_t	i;

QString theText;

	if (v == -1) {
	   indexVector. resize (0);
	   return;
	}
	if (v != 0) {
	   indexVector. push_back (v);
	   return;
	}

	if (indexVector. size() < 2)
	   theText	= QString (" ");
	else {
	   theText	= QString (" trans ");
	   for (int i = 1; i < indexVector. size(); i ++) {
	      char t [255];
	      sprintf (t, " (%d -> %d msec) ", i,
	                      (indexVector. at (i) - indexVector. at (0)) / 2);
              theText. append (t);
	   }
	}
	indexDisplay -> setText (theText);
}

void	impulseViewer::show() {
	myFrame		-> show();
}

void	impulseViewer::hide() {
	myFrame		-> hide();
}

bool	impulseViewer::isHidden() {
	return myFrame	-> isHidden();
}


static int lcount = 0;
void	impulseViewer::showImpulse (int32_t dots) {
uint16_t	i;
double X_axis [dots];
float data [dots];
double Y_values [dots];
float	mmax	= 0;

	responseBuffer	-> getDataFromBuffer (data, dots);
	if (myFrame -> isHidden())
	   return;

	for (i = 0; i < dots; i ++) 
	   X_axis [i] = i;

	for (i = 0; i < dots; i ++) {
	   Y_values [i] = get_db (data [i]);
	   if (Y_values [i] > mmax)
	      mmax = Y_values [i];
	}

	if (++lcount < 3)
	   return;
	lcount = 0;
	plotgrid	-> setAxisScale (QwtPlot::xBottom,
				         (double)0,
				         (double)dots);
	plotgrid	-> enableAxis (QwtPlot::xBottom);
	plotgrid	-> setAxisScale (QwtPlot::yLeft,
				         get_db (0), mmax);
	plotgrid	-> enableAxis (QwtPlot::yLeft);
	spectrumCurve   -> setBaseline  (get_db (0));

	Y_values [0]		= get_db (0);
	Y_values [dots - 1] 	= get_db (0);
	spectrumCurve	-> setSamples (X_axis, Y_values, dots);
	plotgrid	-> replot(); 
}

float	impulseViewer::get_db (float x) {
	return 20 * log10 ((x + 1) / (float)(512));
}

