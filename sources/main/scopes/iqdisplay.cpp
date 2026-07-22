#
/*
 *    Copyright (C)  2026
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

#include	<QSettings>
#include	<QPen>
#include	<QLabel>
#include	<QChart>
#include        <QColorDialog>
#include	<QGraphicsLayout>
#include	"iqdisplay.h"
#include	"dab-constants.h"
#include	"settings-handler.h"

	iqDisplay::iqDisplay	(clickableChart *plotArea,
	                         QSettings *dabSettings) {
QString	colorString;

	scopeSettings	= dabSettings;
	colorString     = value_s (scopeSettings, "iqDisplay",
                                               "displayColor", "black");
	theChart	= new QChart ();
	theChart	-> setBackgroundBrush (QBrush (QColor ("colorString")));
        theChart	 -> legend () -> hide ();
        theChart	 -> layout () -> setContentsMargins(0, 0, 0, 0);
        theChart	 -> setMargins (QMargins (2, 2, 2, 2));

	X_axis		= new QValueAxis();
	X_axis	-> 	setLabelsColor (Qt::lightGray);
	X_axis	->	setGridLineVisible (false);
	X_axis	->	setMinorGridLineVisible (false);
//	X_axis	->	setTickCount (5);
	X_axis	->	setRange	(-100, 100);

	Y_axis		= new QValueAxis();
	Y_axis	->	setLabelsColor (Qt::lightGray);
	Y_axis  ->      setGridLineVisible (false);
        Y_axis  ->      setMinorGridLineVisible (false);
//	Y_axis  ->      setTickCount (5);
	Y_axis	->	setRange	(-100, 100);

	theChart	-> addAxis (X_axis, Qt::AlignBottom);
	theChart	-> addAxis (Y_axis, Qt::AlignLeft);
	plotArea	-> setChart (theChart);
	connect (plotArea, &clickableChart::clicked_right,
	         this, &iqDisplay::rightMouseClick);

	ValueLine	= new QScatterSeries();
	ValueLine	-> setMarkerSize (2.0);
	colorString	= value_s (scopeSettings, "iqDisplay",
                                               "valueLineColor", "#f9f06b");
        ValueLine       -> setPen (QPen (QColor (colorString), 1.0));
	theChart        -> addSeries (ValueLine);
        ValueLine       -> attachAxis (X_axis);
        ValueLine       -> attachAxis (Y_axis);
}

	iqDisplay::~iqDisplay	() {}
	

void	iqDisplay::displayIQ	(const std::vector<Complex> &v, float scale) {
QList<QPointF> theValues;
	ValueLine	-> setMarkerSize (2.0);
        QString colorString =
	                value_s (scopeSettings, "iqDisplay",
                                               "valueLineColor", "#f9f06b");
        ValueLine       -> setPen (QPen (QColor (colorString), 1.0));
	for (uint32_t i = 0; i < v. size () / 2; i ++)
	   theValues. append (QPointF (real (v [i]) * scale,
	                               imag (v [i]) * scale));
	ValueLine -> replace (theValues);
}

void	iqDisplay::display_centerPoints	(const std::vector<Complex> &input,
	                                                float scale) {
QList<QPointF> theValues;
	Complex V [4];
	extract_centerPoints (input, V);
	for (int i = 0; i < 4; i ++)
	   theValues. append (QPointF (real (V [i]) * scale,
	                               imag (V [i]) * scale));

	ValueLine	-> setMarkerSize (6.0);
        QString colorString =
	                value_s (scopeSettings, "iqDisplay",
                                               "valueLineColor", "#f9f06b");
        ValueLine	-> setPen (QPen (QColor (colorString), 4.0));
	
	ValueLine -> replace (theValues);
}

void	iqDisplay::extract_centerPoints	(const std::vector<Complex> &V, 
	                                                    Complex *out) {
int amounts [4] = {0};

	for (int i = 0; i < 4; i ++) 
	   out [i] = Complex (0, 0);

	for (uint16_t i = 0; i < V. size (); i ++) {
	   Complex W = V [i];
	   if ((real (W) > 0) && (imag (W) > 0)) {
	      out [0] += W;
	      amounts [0] ++;
	   }
	   else
	   if ((real (W) > 0) && (imag (W) < 0)) {
	      out [1] += W;
	      amounts [1] ++;
	   }
	   else
	   if ((real (W) < 0) && (imag (W) > 0)) {
	      out [2] += W;
	      amounts [2] ++;
	   }
	   else 
	   if ((real (W) < 0) && (imag (W) < 0)) {
	      out [3] += W;
	      amounts [3] ++;
	   }
	}
	for (int i = 0; i < 4; i ++)
	   out [i] /= (DABFLOAT)amounts [i];
}

void	iqDisplay::rightMouseClick	() {
QColor	displayColor;
QColor	valueLineColor;

	displayColor =
	        QColorDialog::getColor (Qt::black, nullptr, "displayColor");
        if (!displayColor. isValid ())
           return;
        valueLineColor =
	        QColorDialog::getColor (Qt::yellow, nullptr, "valueLineColor");
        if (!valueLineColor. isValid ())
           return;
	store (scopeSettings, "iqDisplay",
	                    "displayColor", displayColor. name ());
	store (scopeSettings, "iqDisplay",
	                     "valueLineColor", valueLineColor. name ());

	theChart	-> setBackgroundBrush (QBrush (displayColor));
//	X_axis		-> setGridLineColor (gridColor);
//	Y_axis		-> setGridLineColor (gridColor);
        ValueLine       -> setPen (QPen (valueLineColor, 1.0));
}
