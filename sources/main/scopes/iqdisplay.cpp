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

	IQDisplay::IQDisplay (QwtPlot *plot):
	                                QwtPlotSpectrogram (), 
	                                 plotgrid (plot) {
	auto	*const colorMap = new QwtLinearColorMap (
	                           QColor (0, 0, 255, 20),
	                           QColor(255, 255, 178, 255));

	setRenderThreadCount	(1);
	setColorMap (colorMap);
	IQData		= nullptr;

	lm_picker       = new QwtPlotPicker (plot -> canvas ());
        QwtPickerMachine *lpickerMachine =
                             new QwtPickerClickPointMachine ();
 
        lm_picker       -> setStateMachine (lpickerMachine);
        lm_picker       -> setMousePattern (QwtPlotPicker::MouseSelect1,
                                            Qt::RightButton);
        connect (lm_picker, qOverload<const QPointF&>(&QwtPlotPicker::selected),
                 this, qOverload<const QPointF&>(&IQDisplay::rightMouseClick));
 

	CycleCount	= 0;

	lastCircleSize	= 0;
	plotDataBackgroundBuffer. resize (2 * RADIUS * 2 * RADIUS, 0.0);
	plotDataDrawBuffer.	  resize (2 * RADIUS * 2 * RADIUS, 0.0);
	memset (plotDataDrawBuffer. data (), 0,
	                  2 * 2 * RADIUS * RADIUS * sizeof (double));
	IQData		= new spectrogramData (plotDataDrawBuffer. data(),
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

void	IQDisplay::display_centerPoints (const std::vector<Complex> &z,
	                                              float scale) {
	cleanScreen	();
	Points. resize (0);

	drawCross ();
	Complex V [4];
	extract_centerPoints (z, V);

	for (int i = 0; i < 4; i ++) 
	   set_fatPoint (V [i], z. size () / 4, scale);
	
	memcpy (plotDataDrawBuffer. data (),
	        plotDataBackgroundBuffer. data (),
	        2 * 2 * RADIUS * RADIUS * sizeof (double));
	this		-> detach();
	this		-> setData	(IQData);
	this		-> setDisplayMode (QwtPlotSpectrogram::ImageMode, true);
	this		-> attach     (plotgrid);
	plotgrid	-> replot ();
}

void	IQDisplay::set_fatPoint (Complex V, int amount, float scale) {
int x	= (int)scale * real (V);
int y	= (int)scale * imag (V);
	(void)amount;
	for (int i = 0; i < 12; i ++) {
	   int yy = y - 6 + i;
           constrain (yy, RADIUS - 1);
	   for (int j = 0; j < 12; j ++) {
	      int xx = (int)(x - 6  + j);
	      constrain (xx, RADIUS - 1); 
	      Points. push_back (std::complex<int> (xx, yy));
	      setPoint (xx, yy, 1000);
	   }
	}
}
	      
void	IQDisplay::displayIQ (const std::vector<Complex> &z, float scale) {
	cleanScreen	();
	Points. resize (0);

	drawCross ();
	repaintCircle (scale);

	for (uint16_t i = 0; i < z. size () / 2; i ++) {
           int x = (int)(scale * real (z [i]));
           int y = (int)(scale * imag (z [i]));

	   constrain (x, RADIUS - 1);
	   constrain (y, RADIUS - 1);
	   int xx	= x + 1;
	   int yy	= y + 1;
	   constrain (xx, RADIUS - 1);
	   constrain (yy, RADIUS - 1);

	   Points. push_back (std::complex<int32_t> (x, y));
	   setPoint (x, y, 1000);
	   Points. push_back (std::complex<int32_t> (xx, yy));
	   setPoint (xx, yy, 1000);
	   Points. push_back (std::complex<int32_t> (xx, y));
	   setPoint (xx, y, 1000);
	   Points. push_back (std::complex<int32_t> (x, yy));
	   setPoint (x, yy, 1000);
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

//	clean the screen
void	IQDisplay::cleanScreen	() {
	for (uint16_t i = 0; i < Points. size (); i ++) {
	   int a	= real (Points [i]);
	   int b	= imag (Points [i]);
	   setPoint (a, b, 0);
	}
}

void	IQDisplay::drawCross () {
	for (int32_t i = -(RADIUS - 1); i < RADIUS; i++) {
	   setPoint (i, i, 30);
	   setPoint (-i, i, 30);
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
	   setPoint ( h,  v, val);
	   setPoint (+h,  v, val);
	   setPoint ( h, +v, val);
	}
}

void	IQDisplay::repaintCircle (float size) {

	if (size != lastCircleSize) {
	   drawCircle (lastCircleSize, 0); // clear old circle
	   lastCircleSize = size;
	}
	drawCircle (size, 20);
}

void	IQDisplay::extract_centerPoints (const std::vector<Complex> &V,
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

