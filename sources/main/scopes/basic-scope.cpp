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

#include	<QColor>
#include	<QPen>
#include	<QColorDialog>
#include	"basic-scope.h"
#include	"settings-handler.h"

	basicScope::basicScope  (QwtPlot	*plotGrid_i,
	                         QSettings	*dabSettings,
	                         int		displaySize,
	                         const QString	&scopeName):
	                              scopeSettings (dabSettings),
	                              plotGrid (plotGrid_i),
	                              spectrumCurve ("") {

QString	colorString;
	this	-> displaySize		= displaySize;

	colorString	= value_s (scopeSettings, scopeName,
	                                       "gridColor", "5e5c64");
	this	-> gridColor = QColor (colorString);
	colorString	= value_s (scopeSettings, scopeName,
	                                       "curveColor", "#f9f06b");
	this	-> curveColor = QColor (colorString);
	colorString	= value_s (scopeSettings, scopeName,
	                                       "labelColor", "yellow");
	labelColor	= QColor (colorString);
	brush		= value_i (scopeSettings, scopeName, "brush", 0) != 0;

	this	-> plotGrid	-> setCanvasBackground (displayColor);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid. setMajPen (QPen(gridColor, 0, Qt::DotLine));
#else
	grid. setMajorPen (QPen(gridColor, 0, Qt::DotLine));
#endif
	grid. enableXMin (true);
	grid. enableYMin (true);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
//	grid. setMinPen (QPen(gridColor, 0, Qt::DotLine));
#else
//	grid. setMinorPen (QPen(gridColor, 0, Qt::DotLine));
#endif
	grid. attach (plotGrid);
	lm_picker       = new QwtPlotPicker (plotGrid -> canvas ());
        QwtPickerMachine *lpickerMachine =
                             new QwtPickerClickPointMachine ();

        lm_picker       -> setStateMachine (lpickerMachine);
        lm_picker       -> setMousePattern (QwtPlotPicker::MouseSelect1,
                                            Qt::RightButton);
        connect (lm_picker, qOverload<const QPointF&>(&QwtPlotPicker::selected),
                 this, &basicScope::rightMouseClick);

   	spectrumCurve. setPen (QPen (curveColor, 3.0));
	spectrumCurve. setOrientation (Qt::Horizontal);
	if (brush) {
	   QBrush ourBrush (curveColor);
	   ourBrush. setStyle (Qt::Dense3Pattern);
	   spectrumCurve. setBrush (ourBrush);
	}
        spectrumCurve. attach (plotGrid);
}

	basicScope::~basicScope () {
	for (auto &m : markerStack)
	   delete m;
	markerStack. resize (0);
}

void	basicScope::showSpectrum  (floatQwt	*data,
	                           int amount,
	                           floatQwt x_min, floatQwt x_max,
	                           floatQwt y_min, floatQwt y_max) {
floatQwt X_axis [displaySize];

	if (amount != displaySize)
	   return;

	for (auto &m : markerStack)
	   delete m;
	markerStack. resize (0);
//	first X axis labels
	for (int i = 0; i < displaySize; i ++)
	   X_axis [i] = 
	          (floatQwt(i)) * (x_max - x_min) / displaySize + x_min;

	plotGrid	-> setAxisScale (QwtPlot::xBottom,
				         (floatQwt)X_axis [0],
				         X_axis [displaySize - 1]);
	plotGrid	-> enableAxis (QwtPlot::xBottom);
	plotGrid	-> setAxisScale (QwtPlot::yLeft, y_min, y_max);
	spectrumCurve. setBaseline (data [0]);
	spectrumCurve. setSamples (X_axis, data, displaySize);
	plotGrid	-> replot(); 
}

void	basicScope::showSpectrum  (floatQwt	*data,
	                           int amount,
	                           floatQwt x_min, floatQwt x_max,
	                           floatQwt y_min, floatQwt y_max,
	                           std::vector<markType> &markers) {
floatQwt X_axis [displaySize];

	if (amount != displaySize)
	   return;
//	first X axis labels
	for (int i = 0; i < displaySize; i ++)
	   X_axis [i] = 
	          (floatQwt(i)) * (x_max - x_min) / displaySize + x_min;

	for (auto& m: markerStack)
	   delete m;
	markerStack. resize (0);
	for (auto &mark : markers) {
	   QwtPlotMarker *Marker = new QwtPlotMarker;
	   Marker	-> setXValue (mark. offset);
	   Marker       -> setYValue (y_max);
           Marker       -> setLineStyle (QwtPlotMarker::VLine);
           QwtText theText = mark. text;
           QFont zz = theText. font ();
           int pp = zz. pointSize ();
           zz . setPointSize (pp + 3);
           theText. setFont (zz);
           Marker       -> setLinePen (labelColor, 1.0);
           Marker       -> setLabelOrientation (Qt::Orientation::Vertical);
           Marker       -> setLabelAlignment (Qt::AlignLeft);
           Marker       -> setLabel  (theText);
           Marker       -> attach (plotGrid);
	   markerStack. push_back (Marker);
        }

	plotGrid	-> setAxisScale (QwtPlot::xBottom,
				         (floatQwt)X_axis [0],
				         X_axis [displaySize - 1]);
	plotGrid	-> enableAxis (QwtPlot::xBottom);
	plotGrid	-> setAxisScale (QwtPlot::yLeft, y_min, y_max);
	spectrumCurve. setBaseline (data [0]);
	spectrumCurve. setSamples (X_axis, data, displaySize);
	plotGrid	-> replot(); 
}

void	basicScope::rightMouseClick	(const QPointF &point) {
QColor	color;
	(void)point;

	color = QColorDialog::getColor (color, nullptr, "displayColor");
        if (!color. isValid ())
           return;
	this	-> displayColor	= color;
        color = QColorDialog::getColor (gridColor, nullptr, "gridColor");
        if (!color. isValid ())
           return;
	this	-> gridColor	= color;
        color = QColorDialog::getColor (curveColor, nullptr, "curveColor");
        if (!color. isValid ())
           return;
	this	-> curveColor	= color;
	store (scopeSettings, scopeName,
	                    "displayColor", displayColor. name ());
	store (scopeSettings, scopeName, "gridColor", gridColor. name ());
	store (scopeSettings, scopeName, "curveColor", curveColor. name ());
	spectrumCurve. setPen (QPen (this -> curveColor, 2.0));
	if (brush) {
           QBrush ourBrush (this -> curveColor); 
           ourBrush. setStyle (Qt::Dense3Pattern);         
           spectrumCurve. setBrush (ourBrush);
        }

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
	plotGrid	-> setCanvasBackground (this -> displayColor);
}
