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
#include	"basic-scope.h"
#include	"dab-constants.h"
#include	"settings-handler.h"

	basicScope::basicScope (clickableChart *plotArea,
	                        QSettings *dabSettings,
	                        int	displaySize,
	                        const QString &scopeName) {
QString	colorString;

	this	-> plotArea		= plotArea;
	this	-> scopeSettings	= dabSettings;
	this	-> displaySize		= displaySize;
	this	-> scopeName		= scopeName;

	colorString     = value_s (scopeSettings, scopeName,
                                               "displayColor", "black");

	theChart	= new QChart ();
	theChart	-> setBackgroundBrush (QBrush (QColor ("black")));
        theChart	 -> legend () -> hide ();
        theChart	 -> layout () -> setContentsMargins(0, 0, 0, 0);
        theChart	 -> setMargins (QMargins (2, 2, 2, 2));

	X_axis		= new QValueAxis();
	X_axis	-> 	setLabelsColor (Qt::lightGray);
        colorString     = value_s (scopeSettings, scopeName,
                                               "gridColor", "5e5c64");
	X_axis	->	setGridLineColor (QColor (colorString));
	X_axis	->	setGridLineVisible (true);
	X_axis	->	setMinorGridLineVisible (false);
	X_axis	->	setTickCount (5);

	Y_axis		= new QValueAxis();
	Y_axis	->	setLabelsColor (Qt::lightGray);
	Y_axis	->	setGridLineColor (QColor (colorString));
	Y_axis  ->      setGridLineVisible (true);
        Y_axis  ->      setMinorGridLineVisible (false);
        Y_axis  ->      setTickCount (5);
//	Y_axis  ->      setMinorTickCount (1);

	theChart	-> addAxis (X_axis, Qt::AlignBottom);
	theChart	-> addAxis (Y_axis, Qt::AlignLeft);
	plotArea	-> setChart (theChart);

	connect (plotArea, &clickableChart::clicked_right,
	         this, &basicScope::rightMouseClick);
	ValueLine	= new QLineSeries ();
	colorString     = value_s (scopeSettings, scopeName,
                                               "curveColor", "#f9f06b");
	ValueLine	-> setPen (QPen (QColor (colorString), 2.0));
	theChart	-> addSeries (ValueLine);
	ValueLine	-> attachAxis (X_axis);
	ValueLine	-> attachAxis (Y_axis);
}

	basicScope::~basicScope	() {
	for (auto &m : markerStack)
	   delete m. pLine;
	markerStack. resize (0);
}

void	basicScope::showSpectrum (double *data, int amount,
	                            float x_min, float x_max,
	                            float y_min, float y_max) {
	for (auto &m: markerStack) {
	   delete m. pLine;
	   delete m. text;
	}
	markerStack. resize (0);
	X_axis	-> setRange	(x_min, x_max);
	Y_axis	-> setRange	(y_min, y_max);

	QList<QPointF> Y_Values;
        Y_Values. reserve (displaySize);
        for (uint16_t i = 0; i < displaySize; i++) 
	   Y_Values. append (QPointF (i * (x_max - x_min) / displaySize + x_min,
	                                                      data [i]));
	ValueLine -> replace (Y_Values);
}

void	basicScope::showSpectrum (double *data, int amount,
	                          float x_min, float x_max,
	                          float y_min, float y_max,
	                          std::vector<markType> &markers) {
	for (auto &m: markerStack) {
	   delete m. pLine;
	   delete m. text;
	}
	markerStack. resize (0);
	X_axis	-> setRange	(x_min, x_max);
	Y_axis  -> setRange     (y_min, y_max);

	QList<QPointF> Y_Values;
        Y_Values. reserve (displaySize);
        for (uint16_t i = 0; i < displaySize; i++) 
	   Y_Values. append (QPointF (i * (x_max - x_min) / displaySize + x_min,
	                                                       data [i]));
	ValueLine -> replace (Y_Values);

//	Add marker for TII
	for (auto & m : markers) {
	   plotMarker p;
	   p. offset	= m. offset;
	   p. pLine = new QLineSeries ();
	   p. pLine -> setPen (QPen(Qt::white, 1, Qt::DashDotLine));
	   p. pLine -> append (m. offset, -1e9);
	   p. pLine -> append (m. offset, +1e9);
	   theChart -> addSeries (p. pLine);
	   p. pLine -> attachAxis (X_axis);
	   p. pLine -> attachAxis (Y_axis);

	   p. text = new QGraphicsSimpleTextItem (m. text, theChart);
	   p. text -> setBrush (Qt::white);
	   p. text -> setRotation (-90);
	   p. text -> setZValue (10);

    // make the font smaller
	   QFont font = p. text -> font ();
	   font. setPointSize (12);
	   p. text -> setFont (font);
	   markerStack. push_back (std::move (p));
	   double yTop = Y_axis -> max ();
	   QPointF pos =
	        theChart -> mapToPosition (QPointF (p. offset, yTop));
	   double width = p.text -> boundingRect (). width ();
	   p. text -> setPos (pos. x () + 2 , pos.y () + width + 20);
	}
}

void	basicScope::rightMouseClick	() {
QColor	displayColor;
QColor	gridColor;
QColor	curveColor;

	displayColor =
	        QColorDialog::getColor (Qt::black, nullptr, "displayColor");
        if (!displayColor. isValid ())
           return;
        gridColor = QColorDialog::getColor (Qt::black, nullptr, "gridColor");
        if (!gridColor. isValid ())
           return;
        curveColor = QColorDialog::getColor (Qt::yellow, nullptr, "curveColor");
        if (!curveColor. isValid ())
           return;
	store (scopeSettings, scopeName,
	                    "displayColor", displayColor. name ());
	store (scopeSettings, scopeName, "gridColor", gridColor. name ());
	store (scopeSettings, scopeName, "curveColor", curveColor. name ());

	theChart	-> setBackgroundBrush (QBrush (displayColor));
	X_axis		-> setGridLineColor (gridColor);
	Y_axis		-> setGridLineColor (gridColor);
        ValueLine       -> setPen (QPen (curveColor, 2.0));
}
