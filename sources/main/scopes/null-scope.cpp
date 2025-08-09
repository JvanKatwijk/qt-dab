#
/*
 *    Copyright (C)  2016 .. 2024
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

#include	"null-scope.h"
#include	<QSettings>
#include        <QColor>
#include        <QPen>
#include	<QColorDialog>

	nullScope::nullScope (QwtPlot *nullScope,
	                              int displaySize,
	                              QSettings	*dabSettings):
	                                  spectrumCurve ("") {
QString	colorString	= "black";

	(void)displaySize;
	this	-> dabSettings		= dabSettings;
	dabSettings	-> beginGroup ("nullScope");
	colorString	= dabSettings -> value ("displayColor",
	                                           "white"). toString();
	displayColor	= QColor (colorString);
	colorString	= dabSettings -> value ("gridColor",
	                                           "black"). toString();
	gridColor	= QColor (colorString);
	colorString	= dabSettings -> value ("curveColor",
	                                            "magenta"). toString();
	curveColor	= QColor (colorString);

	colorString	= dabSettings -> value ("labelColor",
	                                             "yellow"). toString ();
	labelColor	= QColor (colorString);

//	brush		= dabSettings -> value ("brush", 0). toInt () == 1;
	dabSettings	-> endGroup ();
	plotgrid		= nullScope;
	plotgrid		-> setCanvasBackground (displayColor);
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

	lm_picker       = new QwtPlotPicker (nullScope -> canvas ());
        QwtPickerMachine *lpickerMachine =
                             new QwtPickerClickPointMachine ();
 
        lm_picker       -> setStateMachine (lpickerMachine);
        lm_picker       -> setMousePattern (QwtPlotPicker::MouseSelect1,
                                            Qt::RightButton);
        connect (lm_picker, qOverload<const QPointF&>(&QwtPlotPicker::selected),
                 this, &nullScope::rightMouseClick);

	spectrumCurve. setPen (QPen(curveColor, 2.0));
	spectrumCurve. setOrientation (Qt::Horizontal);
	spectrumCurve. setBaseline	(0);
	spectrumCurve. attach (plotgrid);

	Marker		= new QwtPlotMarker ();
}

	nullScope::~nullScope	() {
	delete		grid;
	delete	Marker;
}

void	nullScope::display	(const Complex *V, int amount, int startIndex) {
float	max	= 0;
floatQwt X_axis [512];
floatQwt Y_values [512];
	(void)amount;
	for (int i = 0; i < 512; i ++) {
	   X_axis [i] = 2 * i;
	   Y_values [i] = abs (V [i]);
	   if (abs (V [i]) > max)
	      max = abs (V [i]);
	}

	if (startIndex < 0)
	   Marker -> hide ();
	else {
//	   fprintf (stderr, "Hoera, we gaan de marker zetten %d\n", startIndex); 
	   Marker	-> setXValue (startIndex);
	   Marker	-> setYValue (200);
           Marker	-> setLineStyle (QwtPlotMarker::VLine);
           QwtText theText = "***" + QString::number (startIndex);
           QFont zz = theText. font ();
           int pp = zz. pointSize ();
           zz . setPointSize (pp + 3);
           theText. setFont (zz);
           Marker	-> setLinePen (labelColor, 1.0);
           Marker	-> setLabelOrientation (Qt::Orientation::Vertical);
           Marker	-> setLabelAlignment (Qt::AlignLeft);
           Marker	-> setLabel  (theText);
           Marker	-> attach (plotgrid);
	}

	plotgrid	-> setAxisScale (QwtPlot::xBottom,
				         (floatQwt)X_axis [0],
				         X_axis [512 - 1]);
	plotgrid	-> enableAxis (QwtPlot::xBottom);
	plotgrid	-> setAxisScale (QwtPlot::yLeft,
				         0, 1.5 * max );
	spectrumCurve. setBaseline (0);
	Y_values [0]		= 0;
	Y_values [512 - 1]	= 0;

	spectrumCurve. setSamples (X_axis, Y_values, 512);
	plotgrid	-> replot (); 
}


void	nullScope::rightMouseClick	(const QPointF &point) {
QColor color;

	(void) point;
	color	= QColorDialog::getColor (displayColor,
	                                        nullptr, "display color");
	if (!color. isValid ())
	   return;
	this	-> displayColor	= color;
	color	= QColorDialog::getColor (gridColor,
	                                        nullptr, "grid color");
	if (!color. isValid ())
	   return;
	this		-> gridColor	= color;
	color	= QColorDialog::getColor (curveColor,
	                                        nullptr, "curve color");
	if (!color. isValid ())
	   return;
	this		-> curveColor	= color;

	color	= QColorDialog::getColor (labelColor,
	                                        nullptr, "label color");
	if (!color. isValid ())
	   return;

	this		-> labelColor	= color;

	dabSettings	-> beginGroup ("nullScope");
	dabSettings	-> setValue ("displayColor", displayColor. name ());
	dabSettings	-> setValue ("gridColor", gridColor. name ());
	dabSettings	-> setValue ("curveColor", curveColor. name ());
	dabSettings	-> setValue ("labelColor", labelColor. name ());
	dabSettings	-> endGroup ();

	spectrumCurve. setPen (QPen (this -> curveColor, 2.0));
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

void	nullScope::clean		() {
}

