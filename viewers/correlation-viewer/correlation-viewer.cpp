#
/*
 *    Copyright (C)  2014 .. 2020
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

#include	"correlation-viewer.h"
#include	<QSettings>
#include	<QColor>
#include	<QPen>
#include	"color-selector.h"

	correlationViewer::correlationViewer	(RadioInterface	*mr,
	                                         QSettings	*s,
	                                         RingBuffer<float> *b) :
	                                           myFrame (nullptr),
	                                           spectrumCurve ("") {
QString	colorString	= "black";
bool	brush;
	this	-> myRadioInterface	= mr;
	this	-> dabSettings		= s;
	this	-> responseBuffer	= b;

	dabSettings	-> beginGroup ("correlationViewer");
	colorString	= dabSettings -> value ("displayColor",
	                                              "black"). toString();
	displayColor	= QColor (colorString);
	colorString	= dabSettings -> value ("gridColor",
	                                               "white"). toString();
	gridColor	= QColor (colorString);
	colorString	= dabSettings -> value ("curveColor",
	                                                "white"). toString();
	curveColor	= QColor (colorString);
	brush		= dabSettings -> value ("brush", 0). toInt () == 1;
	int lengthSetting	= dabSettings -> value ("plotLength", 3).
		                                              toInt ();

        int x   = dabSettings -> value ("position-x", 100). toInt ();
        int y   = dabSettings -> value ("position-y", 100). toInt ();
        int w   = dabSettings -> value ("width", 50). toInt ();
        int h   = dabSettings -> value ("height", 30). toInt ();
	dabSettings	-> endGroup ();
	setupUi (&myFrame);

	myFrame. resize (QSize (w, h));
	myFrame. move (QPoint (x, y));

	plotgrid			= impulseGrid;
	plotgrid	-> setCanvasBackground (displayColor);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid. setMajPen (QPen(gridColor, 0, Qt::DotLine));
#else
	grid. setMajorPen (QPen(gridColor, 0, Qt::DotLine));
#endif
	grid. enableXMin (true);
	grid. enableYMin (true);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid. setMinPen (QPen(gridColor, 0, Qt::DotLine));
#else
	grid. setMinorPen (QPen(gridColor, 0, Qt::DotLine));
#endif
	grid. attach (plotgrid);

	lm_picker       = new QwtPlotPicker (plotgrid -> canvas ());
        QwtPickerMachine *lpickerMachine =
                             new QwtPickerClickPointMachine ();

        lm_picker       -> setStateMachine (lpickerMachine);
        lm_picker       -> setMousePattern (QwtPlotPicker::MouseSelect1,
                                            Qt::RightButton);
        connect (lm_picker, SIGNAL (selected (const QPointF&)),
                 this, SLOT (rightMouseClick (const QPointF &)));
//
//	set the length of the display
	correlationLength	-> setValue (lengthSetting);
	connect (correlationLength, SIGNAL (valueChanged (int)),	
	         this, SLOT (handle_correlationLength (int)));
   	spectrumCurve. setPen (QPen(curveColor, 2.0));
	spectrumCurve. setOrientation (Qt::Horizontal);
	spectrumCurve. setBaseline	(0);
	if (brush) {
	   QBrush ourBrush (curveColor);
	   ourBrush. setStyle (Qt::Dense3Pattern);
	   spectrumCurve. setBrush (ourBrush);
	}
	spectrumCurve. attach (plotgrid);
	plotgrid	-> enableAxis (QwtPlot::yLeft);
}

	correlationViewer::~correlationViewer() {
	dabSettings	-> beginGroup ("CorrelationViewer");
	QPoint  pos     = myFrame. mapToGlobal (QPoint (0, 0));
        dabSettings	-> setValue ("position-x", pos. x ());
        dabSettings	-> setValue ("position-y", pos. y ());
	QSize size	= myFrame. frameSize ();
	dabSettings	-> setValue ("width", size. width ());
	dabSettings	-> setValue ("height", size. height ());
	dabSettings	-> endGroup ();
	myFrame. hide();
}

void	correlationViewer::showIndex	(int32_t v) {
QString theText;

	if (v == -1) {
	   indexVector. resize (0);
	   return;
	}
	if (v != 0) {
	   indexVector. push_back (v);
	   return;
	}

	if (indexVector. size() < 2)
	   theText	= QString (" ");
	else {
	   theText	= QString (" trans ");
	   for (uint16_t i = 1; i < indexVector. size(); i ++) {
	      char t [255];
	      sprintf (t, " (%d -> %d usec) ", i,
	                      (indexVector. at (i) - indexVector. at (0)) / 2);
              theText. append (t);
	   }
	}
	indexDisplay -> setText (theText);
}

void	correlationViewer::show () {
	myFrame. show ();
}

void	correlationViewer::hide () {
	myFrame. hide ();
}

bool	correlationViewer::isHidden() {
	return myFrame. isHidden();
}

static int lcount = 0;
void	correlationViewer::showCorrelation (int32_t dots,
	                                          int marker, QVector<int> &v) {
uint16_t	i;
float data	[dots];
float	mmax	= 0;

	responseBuffer	-> getDataFromBuffer (data, dots);
	if (myFrame. isHidden())
	   return;

	dabSettings	-> beginGroup ("correlationViewer");
	int plotLength	= dabSettings -> value ("plotLength",
	                                             5). toInt () * 100;
	dabSettings	-> endGroup ();
	if (plotLength > 1024)
	   plotLength = 1024;
	double X_axis	[plotLength];
	double Y_values [plotLength];	
	for (i = 0; i < plotLength; i ++) 
	   X_axis [i] = marker - plotLength / 2 + i;

	for (i = 0; i < plotLength; i ++) {
	   Y_values [i] = get_db (data [marker - plotLength / 2 + i]);
	   if (Y_values [i] > mmax)
	      mmax = Y_values [i];
	}

	if (++lcount < 2)
	   return;
	lcount = 0;
	plotgrid	-> setAxisScale (QwtPlot::xBottom,
				         (double)marker - plotLength / 2,
				         (double)marker + plotLength / 2 - 1);
	plotgrid	-> enableAxis (QwtPlot::xBottom);
	plotgrid	-> setAxisScale (QwtPlot::yLeft,
				         get_db (0), mmax);
	plotgrid	-> enableAxis (QwtPlot::yLeft);
	spectrumCurve. setBaseline  (get_db (0));

	Y_values [0]		= get_db (0);
	Y_values [plotLength - 1] 	= get_db (0);
	spectrumCurve. setSamples (X_axis, Y_values, plotLength);
	plotgrid	-> replot(); 
	QString theText;
	if (v. size () > 0) {
	   theText = "Best matches at ";
	   for (int i = 0; i < v. size (); i ++)
	      theText += QString::number (v. at (i)) + " ";
	}
	indexDisplay -> setText (theText);
}

float	correlationViewer::get_db (float x) {
	return 20 * log10 ((x + 1) / (float)(4 * 512));
}

void	correlationViewer::rightMouseClick	(const QPointF &point) {
colorSelector *selector;
int	index;
	(void)point;
	selector		= new colorSelector ("display color");
	index			= selector -> QDialog::exec ();
	QString displayColor	= selector -> getColor (index);
	delete selector;
	if (index == 0)
	   return;
	selector		= new colorSelector ("grid color");
	index			= selector	-> QDialog::exec ();
	QString gridColor	= selector	-> getColor (index);
	delete selector;
	if (index == 0)
	   return;
	selector		= new colorSelector ("curve color");
	index			= selector	-> QDialog::exec ();
	QString curveColor	= selector	-> getColor (index);
	delete selector;
	if (index == 0)
	   return;

	dabSettings	-> beginGroup ("correlationViewer");
	dabSettings	-> setValue ("displayColor", displayColor);
	dabSettings	-> setValue ("gridColor", gridColor);
	dabSettings	-> setValue ("curveColor", curveColor);
	dabSettings	-> endGroup ();

	this		-> displayColor	= QColor (displayColor);
	this		-> gridColor	= QColor (gridColor);
	this		-> curveColor	= QColor (curveColor);
	spectrumCurve. setPen (QPen(this -> curveColor, 2.0));
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
	plotgrid	-> setCanvasBackground (this -> displayColor);
}

void	correlationViewer::handle_correlationLength	(int l) {
	dabSettings	-> beginGroup ("correlationViewer");
	dabSettings	-> setValue ("plotLength", l);
	dabSettings	-> endGroup ();
}

