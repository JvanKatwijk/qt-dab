#
/*
 *    Copyright (C)  2016 .. 2022
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

#include	"dev-scope.h"
#include	<QSettings>
#include        <QColor>
#include        <QPen>
#include	<QColorDialog>

	devScope::devScope (QwtPlot *devPlot,
	                              int displaySize,
	                              QSettings	*dabSettings):
	                                  spectrumCurve ("") {
QString	colorString	= "black";

	(void)displaySize;
	this	-> dabSettings		= dabSettings;
	dabSettings	-> beginGroup ("devScope");
	colorString	= dabSettings -> value ("displayColor",
	                                           "white"). toString();
	displayColor	= QColor (colorString);
	colorString	= dabSettings -> value ("gridColor",
	                                           "black"). toString();
	gridColor	= QColor (colorString);
	colorString	= dabSettings -> value ("curveColor",
	                                            "magenta"). toString();
	curveColor	= QColor (colorString);
//	brush		= dabSettings -> value ("brush", 0). toInt () == 1;
	dabSettings	-> endGroup ();
	plotgrid		= devPlot;
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

	lm_picker       = new QwtPlotPicker (devPlot -> canvas ());
        QwtPickerMachine *lpickerMachine =
                             new QwtPickerClickPointMachine ();
 
        lm_picker       -> setStateMachine (lpickerMachine);
        lm_picker       -> setMousePattern (QwtPlotPicker::MouseSelect1,
                                            Qt::RightButton);
        connect (lm_picker, qOverload<const QPointF&>(&QwtPlotPicker::selected),
                 this, &devScope::rightMouseClick);

	spectrumCurve. setPen (QPen(curveColor, 2.0));
	spectrumCurve. setOrientation (Qt::Horizontal);
	spectrumCurve. setBaseline	(0);
	spectrumCurve. attach (plotgrid);
}

	devScope::~devScope	() {
	delete		grid;
}

void	devScope::display	(std::vector<float> V) {
float	max	= 0;
float	min	= 100;
auto	*X_axis	= dynVec (floatQwt, V. size ());
auto	*Y_values	= dynVec (floatQwt, V. size ());
//double X_axis [V. size ()];
//double Y_values [V. size ()];
int	VSize	= V. size ();

	for (int i = 0; i < VSize; i ++) {
	   X_axis [i] = (float)(-VSize / 2 + i);
	   Y_values [i] = V [i];
	   if (V [i] > max)
	      max = V [i];
	   if (V [i] < min)
	      min = V [i];
	}

	plotgrid	-> setAxisScale (QwtPlot::xBottom,
				         (floatQwt)X_axis [0],
				         (floatQwt)X_axis [VSize - 1]);
	plotgrid	-> enableAxis (QwtPlot::xBottom);
	plotgrid	-> setAxisScale (QwtPlot::yLeft,
				         -20, 20);
	spectrumCurve. setBaseline (0);
	Y_values [0]		= 0;
	Y_values [VSize - 1]	= 0;

	spectrumCurve. setSamples (X_axis, Y_values, VSize);
	plotgrid	-> replot (); 
}

void	devScope::rightMouseClick	(const QPointF &point) {
QColor color;

	(void) point;
	color	= QColorDialog::getColor (displayColor,
	                                  nullptr, "displayVolor");
	if (!color. isValid ())
	   return;
	this	-> displayColor	= color;
	color	= QColorDialog::getColor (gridColor, nullptr, "gridColor");
	if (!color. isValid ())
	   return;
	this	-> gridColor	= color;
	color	= QColorDialog::getColor (curveColor, nullptr, "curveColor");
	if (!color. isValid ())
	   return;
	this	-> curveColor	= color;
	dabSettings	-> beginGroup ("devScope");
	dabSettings	-> setValue ("displayColor", displayColor. name ());
	dabSettings	-> setValue ("gridColor", gridColor. name ());
	dabSettings	-> setValue ("curveColor", curveColor. name ());
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

void	devScope::clean		() {
}

