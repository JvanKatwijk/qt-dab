#
/*
 *    Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013
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
#include	"iqdisplay.h"
#include	"spectrogramdata.h"
/*
 *	iq circle plotter
 */
SpectrogramData	*IQData	= NULL;

	IQDisplay::IQDisplay (QwtPlot *plot, int16_t x):
	                                QwtPlotSpectrogram () {
QwtLinearColorMap *colorMap  = new QwtLinearColorMap (Qt::black, Qt::white);

	setRenderThreadCount	(1);
	Radius		= 100;
	plotgrid	= plot;
	x_amount	= x;
	CycleCount	= 0;
	Points. resize (x_amount);
	memset (Points. data (), 0, x_amount * sizeof (std::complex<float>));
	this		-> setColorMap (colorMap);
	plotData. resize (2 * Radius * 2 * Radius);
	plot2.	  resize (2 * Radius * 2 * Radius);
	memset (plotData. data (), 0,
	                  2 * 2 * Radius * Radius * sizeof (double));
	IQData		= new SpectrogramData (plot2. data (),
	                                       0,
	                                       2 * Radius,
	                                       2 * Radius,
	                                       2 * Radius,
	                                       50.0);
	this		-> setData (IQData);
	plot		-> enableAxis (QwtPlot::xBottom, 0);
	plot		-> enableAxis (QwtPlot::yLeft, 0);
	this		-> setDisplayMode (QwtPlotSpectrogram::ImageMode, true);
	plotgrid	-> replot();
}

	IQDisplay::~IQDisplay () {
	this		-> detach ();
//	delete		IQData;
}

void	IQDisplay::DisplayIQ (std::complex<float> *z, float scale) {
int16_t	i;

	for (i = 0; i < x_amount; i ++) {
	   int a	= real (Points [i]);
	   int b	= imag (Points [i]);
	   plotData [(a + Radius - 1) * 2 * Radius + b + Radius - 1] = 0;
	}
	for (i = 0; i < x_amount; i ++) {
           int x = (int)(scale * real (z [i]));
           int y = (int)(scale * imag (z [i]));

	   if (x >= Radius)
	      x = Radius - 1;
	   if (y >= Radius)
	      y = Radius - 1;

	   if (x <= - Radius)
	      x = -(Radius - 1);
	   if (y <= - Radius)
	      y = -(Radius - 1);

	   Points [i] = std::complex<float> (x, y);
	   plotData [(x + Radius - 1) * 2 * Radius + y + Radius - 1] = 100;
	}

	memcpy (plot2. data (), plotData. data (),
	        2 * 2 * Radius * Radius * sizeof (double));
	this		-> detach	();
	this		-> setData	(IQData);
	this		-> setDisplayMode (QwtPlotSpectrogram::ImageMode, true);
	this		-> attach     (plotgrid);
	plotgrid	-> replot();
}
