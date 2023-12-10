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
#include	"spectrum-scope.h"
#include	<QSettings>
#include        <QColor>
#include        <QPen>
#include        "color-selector.h"

static inline
int	valueFor (int bd) {
int result	= 1;
	while (bd > 1) {
	   result <<= 1;
	   bd --;
	}
	return result;
}


	spectrumScope::spectrumScope (QwtPlot *dabScope,
	                              int displaySize,
	                              QSettings	*dabSettings):
	                                  spectrumCurve ("") {
QString	colorString	= "black";
bool	brush;

	this	-> dabSettings		= dabSettings;
	this	-> displaySize		= displaySize;
	dabSettings	-> beginGroup ("spectrumScope");
	colorString	= dabSettings -> value ("displayColor",
	                                           "white"). toString();
	displayColor	= QColor (colorString);
	colorString	= dabSettings -> value ("gridColor",
	                                           "black"). toString();
	gridColor	= QColor (colorString);
	colorString	= dabSettings -> value ("curveColor",
	                                            "cyan"). toString();
	curveColor	= QColor (colorString);
	brush		= dabSettings -> value ("brush", 0). toInt () == 1;
	dabSettings	-> endGroup ();
	plotgrid		= dabScope;
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

	lm_picker	= new QwtPlotPicker (dabScope -> canvas ());
	QwtPickerMachine *lpickerMachine =
                             new QwtPickerClickPointMachine ();

	lm_picker       -> setStateMachine (lpickerMachine);
        lm_picker       -> setMousePattern (QwtPlotPicker::MouseSelect1,
                                            Qt::RightButton);
        connect (lm_picker, SIGNAL (selected (const QPointF&)),
                 this, SLOT (rightMouseClick (const QPointF &)));

	spectrumCurve. setPen (QPen(curveColor, 2.0));
	spectrumCurve. setOrientation (Qt::Horizontal);
	spectrumCurve. setBaseline	(get_db (0));

	if (brush) {
	   QBrush ourBrush (curveColor);
           ourBrush. setStyle (Qt::Dense3Pattern);
           spectrumCurve. setBrush (ourBrush);
	}
	spectrumCurve. attach (plotgrid);
	
	Marker		= new QwtPlotMarker();
	Marker		-> setLineStyle (QwtPlotMarker::VLine);
	Marker		-> setLinePen (QPen (Qt::red));
	Marker		-> attach (plotgrid);
	plotgrid	-> enableAxis (QwtPlot::yLeft);
	bitDepth	= 12;
	normalizer	= valueFor (bitDepth);
}

	spectrumScope::~spectrumScope	() {

	delete		Marker;
	delete		grid;
}

void	spectrumScope::display		(double *X_axis, double *Y_value,
	                                 int freq, int Amp) {
	(void)freq;
	float Max	= Amp / 100.0 * (-get_db (0));
	plotgrid	-> setAxisScale (QwtPlot::xBottom,
				         (double)X_axis [0],
				         X_axis [displaySize - 1]);
	plotgrid	-> enableAxis (QwtPlot::xBottom);
	plotgrid	-> setAxisScale (QwtPlot::yLeft,
				         get_db (0), get_db (0) + 2.5 * Max);

	spectrumCurve. setBaseline (get_db (0));
	Y_value [0]		= get_db (0);
	Y_value [displaySize - 1] = get_db (0);

	spectrumCurve. setSamples (X_axis, Y_value, 512);
	Marker		-> setXValue (0);
	plotgrid	-> replot (); 
}

void	spectrumScope::rightMouseClick	(const QPointF &point) {
int index;
	(void) point;
QString	displayColor;
QString	gridColor;
QString	curveColor;

	{  colorSelector displaySelector ("displayColor");
	   int index		= displaySelector. QDialog::exec ();
	   if (index == 0)
	      return;
	   displayColor	= displaySelector. getColor (index);
	}
	{  colorSelector gridSelector ("gridColor");
	   int index		= gridSelector. QDialog::exec ();
	   if (index == 0)
	      return;
	   gridColor		= gridSelector . getColor (index);
	}
	{  colorSelector curveSelector ("curveColor");
	   int index		= curveSelector.  QDialog::exec ();
	   if (index == 0)
	      return;
	   QString curveColor	= curveSelector. getColor (index);
	}

	dabSettings	-> beginGroup ("spectrumScope");
	dabSettings	-> setValue ("displayColor", displayColor);
	dabSettings	-> setValue ("gridColor", gridColor);
	dabSettings	-> setValue ("curveColor", curveColor);
	dabSettings	-> endGroup ();

	this		-> displayColor	= QColor (displayColor);
	this		-> gridColor	= QColor (gridColor);
	this		-> curveColor	= QColor (curveColor);
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
	grid		-> setMinPen (QPen (this -> gridColor, 0,
	                                                   Qt::DotLine));
#else
	grid		-> setMinorPen (QPen (this -> gridColor, 0,
	                                                   Qt::DotLine));
#endif
	plotgrid	-> setCanvasBackground (this -> displayColor);
}

float   spectrumScope::get_db (float x) {
        return 20 * log10 ((x + 1) / (float)(normalizer));
}

void	spectrumScope::set_bitDepth	(int d) {
	this	-> bitDepth	= d;
}

