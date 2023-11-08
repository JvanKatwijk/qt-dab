#
/*
 *    Copyright (C)  2016 .. 2022
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
#include        <qwt_text.h>
 
	waterfallScope::waterfallScope (QwtPlot       *scope,
	                                int		displaySize_i,
	                                int		rasterSize_i):
	                                      QwtPlotSpectrogram (),
	                                      plotgrid (scope),
	                                      displaySize (displaySize_i),
	                                      rasterSize (rasterSize_i) {
	colorMap  = new QwtLinearColorMap (Qt::darkCyan, Qt::red);
	QwtLinearColorMap *c2 = new QwtLinearColorMap (Qt::darkCyan, Qt::red);
	colorMap        -> addColorStop (0.1, Qt::cyan);
	colorMap        -> addColorStop (0.4, Qt::green);
	colorMap        -> addColorStop (0.7, Qt::yellow);
	c2              -> addColorStop (0.1, Qt::cyan);
	c2              -> addColorStop (0.4, Qt::green);
	c2              -> addColorStop (0.7, Qt::yellow);
	this            -> setColorMap (colorMap);
	rightAxis	= plotgrid -> axisWidget (QwtPlot::yRight);

	plotData	= new double [2 * displaySize * rasterSize];

	for (int i = 0; i < rasterSize; i ++)
	   for (int j = 0; j < displaySize; j ++)
	      plotData [i * displaySize + j] = (double)i / rasterSize;

	WaterfallData   = new SpectrogramData (plotData,
	                                       10000,
	                                       1000,
	                                       rasterSize,
	                                       displaySize,
	                                       50.0);
	this -> setData (WaterfallData);
	this -> setDisplayMode (QwtPlotSpectrogram::ImageMode, true);
	rightAxis -> setColorMap (this -> data () -> interval (Qt::YAxis),
	                          c2);
//	plotgrid	-> setAxisScale (QwtPlot::yRight, 0, 50.0);
//	plotgrid	-> enableAxis   (QwtPlot::yRight);
	plotgrid	-> setAxisScale (QwtPlot::xBottom,
	                                 10000,
	                                 11000);

	plotgrid	-> enableAxis (QwtPlot::xBottom);
	plotgrid	-> enableAxis (QwtPlot::yLeft);
	plotgrid	-> setAxisScale (QwtPlot::yLeft, 0, rasterSize);

//	Marker          = new QwtPlotMarker ();
//	Marker          -> setLineStyle (QwtPlotMarker::VLine);
//	Marker          -> setLinePen (QPen (Qt::black, 2.0));
//	Marker          -> attach (plotgrid);
//	indexforMarker  = 0;
	plotgrid        -> replot ();
}

	waterfallScope::~waterfallScope () {
	plotgrid        -> enableAxis (QwtPlot::yRight, false);
	plotgrid        -> enableAxis (QwtPlot::xBottom, false);
	plotgrid        -> enableAxis (QwtPlot::yLeft, false);
	this            -> detach ();
	delete[]	plotData;
//	delete	colorMap;
//	delete	WaterfallData;
}

void	waterfallScope::cleanUp () {
	for (int i = 0; i < rasterSize; i ++)
	   for (int j = 0; j < displaySize; j ++)
	      plotData [i * displaySize + j] = (double)i / rasterSize;
}

void    waterfallScope::display (double *X_axis,
	                         double *Y1_value,
	                         double  amp,
	                         int32_t marker) {
int     orig    = (int)(X_axis [0]);
int     width   = (int)(X_axis [displaySize - 1] - orig);

//	indexforMarker  = marker;
/*
 *      shift one row, faster with memmove than writing out
 *      the loops. Note that source and destination overlap
 *      and we therefore use memmove rather than memcpy
 */
	memmove (&plotData [0],
	         &plotData [displaySize],
	         (rasterSize - 1) * displaySize * sizeof (double));
/*
 *      ... and insert the new line
 */
	memcpy (&plotData [(rasterSize - 1) * displaySize],
	        &Y1_value [0],
	        displaySize * sizeof (double));

	invalidateCache ();
	WaterfallData = new SpectrogramData (plotData,
	                                     orig,
	                                     width,
	                                     rasterSize,
	                                     displaySize,
	                                     amp / 2);

	this            -> detach       ();
	this            -> setData      (WaterfallData);
	this            -> setDisplayMode (QwtPlotSpectrogram::ImageMode,
	                                                               true);

	plotgrid        -> setAxisScale (QwtPlot::xBottom,
	                                 orig,
	                                 orig + width);
	plotgrid        -> enableAxis (QwtPlot::xBottom);
//	Marker          -> setXValue (marker);
	this            -> attach     (plotgrid);
	plotgrid        -> replot();
}

