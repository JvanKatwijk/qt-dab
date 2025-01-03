#
/*
 *    Copyright (C)  2016 .. 2024
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

#include	"waterfall-scope.h"
#include        <qwt_plot.h>
#include	<qwt_color_map.h>
#include	<qwt_scale_widget.h>

	waterfallScope::waterfallScope (QwtPlot       *scope,
	                                int		displaySize_i,
	                                int		rasterSize_i):
	                                      QwtPlotSpectrogram (),
	                                      plotgrid (scope),
	                                      displaySize (displaySize_i),
	                                      rasterSize (rasterSize_i) {
	colorMap	= new QwtLinearColorMap (Qt::darkCyan, Qt::red);
	colorMap        -> addColorStop (0.0, Qt::cyan);
//	colorMap        -> addColorStop (0.1, Qt::cyan);
	colorMap        -> addColorStop (0.4, Qt::green);
	colorMap        -> addColorStop (0.6, Qt::yellow);
	colorMap        -> addColorStop (0.8, Qt::red);
	rightAxis	= plotgrid -> axisWidget (QwtPlot::yRight);
	axisMap		= new QwtLinearColorMap (Qt::darkCyan, Qt::red);
	axisMap		-> addColorStop (0.0, Qt::cyan);
//	axisMap		-> addColorStop (0.1, Qt::cyan);
	axisMap		-> addColorStop (0.4, Qt::green);
	axisMap		-> addColorStop (0.6, Qt::yellow);
	axisMap		-> addColorStop (0.8, Qt::red);
	this            -> setColorMap (colorMap);

	this -> setDisplayMode (QwtPlotSpectrogram::ImageMode, true);
	plotData. resize (displaySize * rasterSize);

	for (int i = 0; i < rasterSize; i ++)
	   for (int j = 0; j < displaySize; j ++)
	      plotData [i * displaySize + j] = (double)i / rasterSize;

	WaterfallData	= nullptr;
	started		= false;
	oldAmp		= 1000;		//  impossible value
}

	waterfallScope::~waterfallScope () {
	plotgrid        -> enableAxis (QwtPlot::yRight, false);
	plotgrid        -> enableAxis (QwtPlot::xBottom, false);
	plotgrid        -> enableAxis (QwtPlot::yLeft, false);
	this            -> detach ();
}

void	waterfallScope::cleanUp () {
	for (int i = 0; i < rasterSize; i ++)
	   for (int j = 0; j < displaySize; j ++)
	      plotData [i * displaySize + j] = (floatQwt)i / rasterSize;
	started = false;
}

void    waterfallScope::display (const floatQwt *X_axis,
	                         const floatQwt *Y1_value,
	                         floatQwt  amp,
	                         int32_t marker) {
int     orig    = (int)(X_axis [0]);
int     width   = (int)(X_axis [displaySize - 1] - orig);

/*
 *      shift one row, faster with memmove than writing out
 *      the loops. Note that source and destination overlap
 *      and we therefore use memmove rather than memcpy
 */

	memmove (&plotData [0], &plotData [displaySize],
	                     (rasterSize - 1) * displaySize * sizeof (double));
	for (int i = 0; i < displaySize; i ++)
	   plotData [(rasterSize - 1) * displaySize + i] =  Y1_value [i];

	invalidateCache ();

	if (!started || ((int)amp != oldAmp)) {
	   WaterfallData = new spectrogramData (plotData. data (),
	                                        orig,
	                                        width,
	                                        rasterSize,
	                                        displaySize,
	                                        amp);

	   this		-> setData (WaterfallData);
           plotgrid	-> setAxisScale (QwtPlot::xBottom,
                                         orig,
                                         orig + width);

           plotgrid	-> enableAxis (QwtPlot::xBottom);
           plotgrid	-> enableAxis (QwtPlot::yLeft);
           plotgrid	-> setAxisScale (QwtPlot::yLeft, 0, rasterSize);
	   rightAxis	-> setColorMap (this -> data () -> interval (Qt::YAxis),
	                                           axisMap);
           this 	-> attach     (plotgrid);
	   started	= true;
	   oldAmp	= (int)amp;
	}
	plotgrid        -> replot();
}

