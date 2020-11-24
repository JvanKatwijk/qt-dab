#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the dumpViewer
 *
 *    dumpViewer is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    dumpViewer is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dumpViewer; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	<QCoreApplication>
#include	"dump-viewer.h"
//

	dumpViewer::dumpViewer (FILE *f, QWidget *parent):
	                                       QDialog (parent) {
int16_t	i;

	this	-> theFile	= f;
	setupUi (this);
	displayColor	= QColor	("black");
	gridColor	= QColor	("white");
	curveColor	= QColor	("white");
	plotgrid	= viewerWindow;
	plotgrid	-> setCanvasBackground (displayColor);
	grid		= new QwtPlotGrid;
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid		-> setMajPen (QPen(gridColor, 0, Qt::DotLine));
#else
	grid		-> setMajorPen (QPen(gridColor, 0, Qt::DotLine));
#endif
	grid		-> enableXMin (true);
	grid		-> enableYMin (true);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid		-> setMinPen (QPen(gridColor, 0, Qt::DotLine));
#else
	grid		-> setMinorPen (QPen(gridColor, 0, Qt::DotLine));
#endif
	grid		-> attach (plotgrid);

	lm_picker       = new QwtPlotPicker (plotgrid -> canvas ());
        QwtPickerMachine *lpickerMachine =
                             new QwtPickerClickPointMachine ();

        lm_picker       -> setStateMachine (lpickerMachine);
        lm_picker       -> setMousePattern (QwtPlotPicker::MouseSelect1,
                                            Qt::RightButton);
//	connect (lm_picker, SIGNAL (selected (const QPointF&)),
//	         this, SLOT (rightMouseClick (const QPointF &)));

	spectrumCurve	= new QwtPlotCurve ("");
   	spectrumCurve	-> setPen (QPen(curveColor, 2.0));
	spectrumCurve	-> setOrientation (Qt::Horizontal);
	spectrumCurve	-> setBaseline	(0);
	spectrumCurve	-> attach (plotgrid);
	plotgrid	-> enableAxis (QwtPlot::yLeft);
	connect (viewSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_viewSlider (int)));
//
//	Now looking at the file
	fseek (theFile, 0, SEEK_END);
	fileLength	= ftell (theFile);
	fseek (theFile, 0, SEEK_SET);

	show_segment (0);
}
//

	dumpViewer::~dumpViewer	(void) {
}

void	dumpViewer::handle_viewSlider	(int pos) {
int lengthF	= fileLength / sizeof (float);
int p		= pos * lengthF / 100;
	show_segment (p * sizeof (float));
}

void	dumpViewer::show_segment (int pos) {
double	X_axis [512];
double	Y_Values [512];
float	temp [512];

	for (int i = 0; i < 512; i ++)
	   X_axis [i] = pos + i;

	memset (Y_Values, 0, sizeof (double) * 512);
	fseek (theFile, pos, SEEK_SET);
	int length = fread (temp, sizeof (float), 512, theFile);
	for (int i = 0; i < length; i ++)
	   Y_Values [i] = temp [i];

	plotgrid        -> setAxisScale (QwtPlot::xBottom,
                                         double (pos),
                                         (double)(pos + 512));
        plotgrid        -> enableAxis (QwtPlot::xBottom);
        plotgrid        -> setAxisScale (QwtPlot::yLeft,
                                         0, 14);
        plotgrid        -> enableAxis (QwtPlot::yLeft);
        spectrumCurve   -> setBaseline  (0);
//	Y_values [0]	= 0;
//	Y_values [512 - 1]	= 0;
        spectrumCurve   -> setSamples (X_axis, Y_Values, 512);
        plotgrid        -> replot();
}

