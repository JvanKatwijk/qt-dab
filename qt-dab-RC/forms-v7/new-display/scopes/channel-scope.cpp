#
/*
 *    Copyright (C)  2016 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB 
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
#include	"channel-scope.h"
#include	<QSettings>
#include        <QColor>
#include        <QPen>
#include        "color-selector.h"

	channelScope::channelScope (QwtPlot *channelDisplay,
	                            int displaySize,
	                            QSettings	*dabSettings):
	                                  amplitudeCurve (""),
	                                  phaseCurve ("") {
QString	colorString	= "black";

	(void)displaySize;
	this	-> dabSettings		= dabSettings;
	this	-> displaySize		= displaySize;
	dabSettings	-> beginGroup ("channelScope");
	colorString	= dabSettings -> value ("displayColor",
	                                           "white"). toString();
	displayColor	= QColor (colorString);
	colorString	= dabSettings -> value ("gridColor",
	                                           "vlack"). toString();
	gridColor	= QColor (colorString);
	colorString	= dabSettings -> value ("curveColor",
	                                            "cyan"). toString();
	curveColor	= QColor (colorString);
	dabSettings	-> endGroup ();
	plotgrid	= channelDisplay;
	plotgrid	-> setCanvasBackground (displayColor);
	grid		= new QwtPlotGrid;
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

	lm_picker	= new QwtPlotPicker (plotgrid -> canvas ());
	QwtPickerMachine *lpickerMachine =
                             new QwtPickerClickPointMachine ();

	lm_picker       -> setStateMachine (lpickerMachine);
        lm_picker       -> setMousePattern (QwtPlotPicker::MouseSelect1,
                                            Qt::RightButton);
        connect (lm_picker, SIGNAL (selected (const QPointF&)),
                 this, SLOT (rightMouseClick (const QPointF &)));

	amplitudeCurve. setPen (QPen(curveColor, 2.0));
	amplitudeCurve. setOrientation (Qt::Horizontal);
	amplitudeCurve. setBaseline	(get_db (0));

	phaseCurve. setPen (QPen (QColor ("red"), 2.0));
	phaseCurve. setOrientation (Qt::Horizontal);
	phaseCurve. setBaseline	(get_db (0));

	amplitudeCurve. attach (plotgrid);
	phaseCurve. attach (plotgrid);
	
	Marker		= new QwtPlotMarker();
	Marker		-> setLineStyle (QwtPlotMarker::VLine);
	Marker		-> setLinePen (QPen (Qt::red));
	Marker		-> attach (plotgrid);
	plotgrid	-> enableAxis (QwtPlot::yLeft);
	normalizer	= 512;
}

	channelScope::~channelScope	() {

	delete		Marker;
	delete		grid;
}

void	channelScope::display		(const double *X_axis,
	                                 const double *amplitudeValues,
	                                 const double *phaseValues,
	                                 int Amp) {
double ampVals [displaySize];
	(void)Amp;
	float Max	= 0;
	plotgrid	-> setAxisScale (QwtPlot::xBottom,
				         (double)X_axis [0],
				         X_axis [displaySize - 1]);
	plotgrid	-> enableAxis (QwtPlot::xBottom);
	plotgrid	-> setAxisScale (QwtPlot::yLeft,
				         0, Amp);

	for (int i = 0; i < displaySize; i ++)
	   if (amplitudeValues [i] > Max)
	      Max = amplitudeValues [i];

	if (Max > 15)
	   for (int i = 0; i < displaySize; i ++)
	      ampVals [i] = amplitudeValues [i] * 15.0 / Max;
	amplitudeCurve. setBaseline (0);
	ampVals [0]	= 0;
	ampVals [displaySize - 1] = 0;

	amplitudeCurve. setSamples (X_axis, ampVals, displaySize);
	phaseCurve. setSamples  (X_axis, phaseValues, displaySize);
	Marker		-> setXValue (0);
	plotgrid	-> replot (); 
}

void	channelScope::rightMouseClick	(const QPointF &point) {
colorSelector *selector;
int	index;
	(void) point;
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

	dabSettings	-> beginGroup ("channelScope");
	dabSettings	-> setValue ("displayColor", displayColor);
	dabSettings	-> setValue ("gridColor", gridColor);
	dabSettings	-> setValue ("curveColor", curveColor);
	dabSettings	-> endGroup ();

	this		-> displayColor	= QColor (displayColor);
	this		-> gridColor	= QColor (gridColor);
	this		-> curveColor	= QColor (curveColor);
	phaseCurve. setPen (QPen (QColor ("red"), 2.0));
	amplitudeCurve. setPen (QPen (this -> curveColor, 2.0));
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

float   channelScope::get_db (float x) {
        return 20 * log10 ((x + 1) / (float)(normalizer));
}

