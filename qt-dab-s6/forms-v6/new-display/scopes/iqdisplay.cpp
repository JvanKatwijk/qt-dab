#
/*
 *    Copyright (C) 2016 .. 2023
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
static inline
void	constrain (int32_t & testVal, const int32_t limit) {
	if (testVal > limit) 
	   testVal = limit;
	else
	if (testVal < -limit) {
	   testVal = -limit;
	}
}

	IQDisplay::IQDisplay (QwtPlot *plot, int16_t x):
	                                QwtPlotSpectrogram (), 
	                                 plotgrid (plot) {
	          
auto	*const colorMap = new QwtLinearColorMap (
	                           QColor(0, 0, 255, 20),
	                           QColor(255, 255, 178, 255));

	(void)x;
	setRenderThreadCount	(1);
	setColorMap (colorMap);
	IQData		= nullptr;

	lm_picker       = new QwtPlotPicker (plot -> canvas ());
        QwtPickerMachine *lpickerMachine =
                             new QwtPickerClickPointMachine ();
 
        lm_picker       -> setStateMachine (lpickerMachine);
        lm_picker       -> setMousePattern (QwtPlotPicker::MouseSelect1,
                                            Qt::RightButton);
        connect (lm_picker, SIGNAL (selected (const QPointF&)), 
                 this, SLOT (rightMouseClick (const QPointF &)));
 

	x_amount	= 4 * 512;
	CycleCount	= 0;

	lastCircleSize	= 0;
	plotDataBackgroundBuffer. resize (2 * RADIUS * 2 * RADIUS, 0.0);
	plotDataDrawBuffer.	  resize (2 * RADIUS * 2 * RADIUS, 0.0);
	memset (plotDataDrawBuffer. data (), 0,
	                  2 * 2 * RADIUS * RADIUS * sizeof (double));
	IQData		= new SpectrogramData (plotDataDrawBuffer. data(),
	                                       0,
	                                       2 * RADIUS,
	                                       2 * RADIUS,
	                                       2 * RADIUS,
	                                       50.0);
	this		-> setData (IQData);
	plot		-> enableAxis (QwtPlot::xBottom, false);
	plot		-> enableAxis (QwtPlot::yLeft, false);
	this		-> setDisplayMode (QwtPlotSpectrogram::ImageMode, true);
	plotgrid	-> replot();
}

	IQDisplay::~IQDisplay () {
	this		-> detach();
//	delete		IQData;
}

void	IQDisplay::setPoint (int x, int y, int val) {
	plotDataBackgroundBuffer [(x + RADIUS - 1) * 2 * RADIUS + y + RADIUS - 1] = val;
}

//	clean the screen
void	IQDisplay::cleanScreen	() {
	for (int i = 0; i < Points. size (); i ++) {
	   int a	= real (Points [i]);
	   int b	= imag (Points [i]);
	   setPoint (a, b, 0);
	}
}

void	IQDisplay::DisplayIQ (const std::vector<complex<float>> &z,
	                             int amount, float scale) {

	(void)amount;
	cleanScreen	();
	if (z. size () != Points. size ())
	   Points. resize (z. size (), {0, 0});

//	drawCross ();
	repaintCircle (scale);

	for (int i = 0; i < Points. size () / 2; i ++) {
           int x = (int)(scale * real (z [i]));
           int y = (int)(scale * imag (z [i]));

	   constrain (x, RADIUS - 1);
	   constrain (y, RADIUS - 1);
	   int xx	= x + 1;
	   int yy	= y + 1;
	   constrain (xx, RADIUS - 1);
	   constrain (yy, RADIUS - 1);

	   Points [2 * i] = std::complex<int32_t> (x, y);
	   setPoint (x, y, 1000);
	   Points [2 * i + 1] = std::complex<int32_t> (xx, yy);
	   setPoint (xx, yy, 1000);
	}

	memcpy (plotDataDrawBuffer. data (),
	        plotDataBackgroundBuffer. data (),
	        2 * 2 * RADIUS * RADIUS * sizeof (double));
	this		-> detach();
	this		-> setData	(IQData);
	this		-> setDisplayMode (QwtPlotSpectrogram::ImageMode, true);
	this		-> attach     (plotgrid);
	plotgrid	-> replot ();
}

void	IQDisplay::rightMouseClick (const QPointF &p) {
	(void)p;
	emit rightMouseClick ();
}

void	IQDisplay::drawCross () {
	for (int32_t i = -(RADIUS - 1); i < RADIUS; i++) {
	   setPoint (1, i, 30); // horizontal line
	   setPoint (i, 0, 30); // vertical line
	}
}

void	IQDisplay::drawCircle (float scale, int val) {
	const int32_t MAX_CIRCLE_POINTS =
	           static_cast<int32_t> (180 * scale); // per quarter

	for (int32_t i = 0; i < MAX_CIRCLE_POINTS; i ++) {
	   const float phase =
	              0.5f * (float)M_PI * (float)i / MAX_CIRCLE_POINTS;

	   auto h = (int32_t)(RADIUS * scale * cosf(phase));
	   auto v = (int32_t)(RADIUS * scale * sinf(phase));

	   constrain (h, RADIUS - 1);
	   constrain (v, RADIUS - 1);

//	as h and v covers only the top right segment, fill also other segments
	   setPoint (-h, -v, val);
	   setPoint (-h, +v, val);
	   setPoint (+h, -v, val);
	   setPoint (+h, +v, val);
	}
}

void	IQDisplay::repaintCircle (float size) {

	if (size != lastCircleSize) {
	   drawCircle (lastCircleSize, 0); // clear old circle
	   lastCircleSize = size;
	}
	drawCircle (size, 20);
}
