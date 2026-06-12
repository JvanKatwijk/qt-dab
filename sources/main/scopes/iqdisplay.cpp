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
	theChart	= new QChart ();
	theChart	-> setBackgroundBrush (QBrush (QColor ("black")));
        theChart	 -> legend () -> hide ();
        theChart	 -> layout () -> setContentsMargins(0, 0, 0, 0);
        theChart	 -> setMargins (QMargins (2, 2, 2, 2));

	X_axis		= new QValueAxis();
	X_axis	-> 	setLabelsColor (Qt::lightGray);
	X_axis	->	setGridLineColor (QColor ("black"));
	X_axis	->	setGridLineVisible (true);
	X_axis	->	setMinorGridLineVisible (false);
	X_axis	->	setTickCount (5);
	X_axis	->	setRange	(-100, 100);

	Y_axis		= new QValueAxis();
	Y_axis	->	setLabelsColor (Qt::lightGray);
	Y_axis	->	setGridLineColor (QColor ("black"));
	Y_axis  ->      setGridLineVisible (true);
        Y_axis  ->      setMinorGridLineVisible (false);
        Y_axis  ->      setTickCount (5);
	Y_axis	->	setRange	(-100, 100);

	theChart	-> addAxis (X_axis, Qt::AlignBottom);
	theChart	-> addAxis (Y_axis, Qt::AlignLeft);
	plotArea	-> setChart (theChart);

	ValueLine	= new QScatterSeries();
	ValueLine	-> setMarkerSize (2.0);
	theChart        -> addSeries (ValueLine);
        ValueLine       -> attachAxis (X_axis);
        ValueLine       -> attachAxis (Y_axis);
}

	iqDisplay::~iqDisplay	() {}

void	iqDisplay::displayIQ	(const std::vector<Complex> &v, float scale) {
QList<QPointF> theValues;
	for (int i = 0; i < v. size () / 2; i ++)
	   theValues. append (QPointF (real (v [i]) * scale,
	                               imag (v [i]) * scale));
	ValueLine -> replace (theValues);
}

void	iqDisplay::display_centerPoints	(const std::vector<Complex> &v,
	                                                float scale) {
	return displayIQ (v, scale);
}

void	iqDisplay::set_fatPoint	(Complex v, int x, float xx) {
	(void) v;
	(void) x;
	(void) xx;
}

void	iqDisplay::extract_centerPoints	(const std::vector<Complex> &v, 
	                                                    Complex *out) {
	(void) v;
	(void) out;
}


