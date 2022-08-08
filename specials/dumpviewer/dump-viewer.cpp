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
	displayColor	= QColor	("white");
	gridColor	= QColor	("red");
	curveColor	= QColor	("red");
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
	connect (amplitudeSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_amplitudeSlider (int)));
	connect (compressor, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_compressor (int)));
//
//	Now looking at the file
	seconds_per_frame	= 12.0 / 125;
	seconds_per_sample	= 2 * seconds_per_frame;
	fseek (theFile, 0, SEEK_END);
	fileLength	= ftell (theFile);
	fseek (theFile, 0, SEEK_SET);
	fprintf (stderr, "samples per minute: %d\n",
	                          (int)(60 / seconds_per_sample));
	fprintf (stderr, "duration of record: %d seconds\n", 
	                     (int)(fileLength / sizeof (float) * seconds_per_sample));
	show_segment (0, 1);
}
//

	dumpViewer::~dumpViewer	(void) {
}

void	dumpViewer::handle_viewSlider	(int pos) {
	show_segment (pos, compressor -> value ());
}

void	dumpViewer::handle_amplitudeSlider (int h) {
	(void)h;
	show_segment (viewSlider -> value (), 
	              compressor -> value ());
}

void	dumpViewer::handle_compressor	(int h) {
	show_segment (viewSlider -> value (), h);
}

//
//	pos is the pos of the slider, so a value between 0 .. 100
void	dumpViewer::show_segment (int pos, int compression) {
double	X_axis [512];
double	Y_Values [512];
float	temp [512 * compression];
int	lengthF	=  fileLength / sizeof (float);
int	p	= pos * lengthF / 100;
	for (int i = 0; i < 512; i ++)
	   X_axis [i] = (p + i) * compression * seconds_per_sample;

	memset (Y_Values, 0, sizeof (double) * 512);
	fseek (theFile, p * sizeof (float), SEEK_SET);
	int length = fread (temp, sizeof (float), 512 * compression, theFile);
	for (int i = 0; i < length / compression; i ++)
	   Y_Values [i] = temp [i * compression];

	plotgrid        -> setAxisScale (QwtPlot::xBottom,
                                         double (p * compression * seconds_per_sample),
                                         (double)((p + 512) * compression * seconds_per_sample));
        plotgrid        -> enableAxis (QwtPlot::xBottom);
        plotgrid        -> setAxisScale (QwtPlot::yLeft,
                                         0, amplitudeSlider -> value ());
        plotgrid        -> enableAxis (QwtPlot::yLeft);
        spectrumCurve   -> setBaseline  (0);
        spectrumCurve   -> setSamples (X_axis, Y_Values, 512);
        plotgrid        -> replot();
}

