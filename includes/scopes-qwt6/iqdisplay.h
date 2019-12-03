#
/*
 *    Copyright (C) 2008, 2009, 2010
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
#ifndef	__IQDISPLAY__
#define	__IQDISPLAY__

#include	"dab-constants.h"
#include	<vector>
#include	<qwt.h>
#include	<qwt_slider.h>
#include	<qwt_plot.h>
#include	<qwt_plot_curve.h>
#include	<qwt_plot_marker.h>
#include	<qwt_plot_grid.h>
#include	<qwt_dial.h>
#include	<qwt_dial_needle.h>
#include	<qwt_plot_spectrogram.h>
#include	<qwt_color_map.h>
#include	<qwt_plot_spectrogram.h>
#include	<qwt_scale_widget.h>
#include	<qwt_scale_draw.h>
#include	<qwt_plot_zoomer.h>
#include	<qwt_plot_panner.h>
#include	<qwt_plot_layout.h>
#include	"dab-constants.h"
/*
 *	for the waterfall display
 */

class IQDisplay: public QObject, public QwtPlotSpectrogram {
Q_OBJECT
public:
	IQDisplay		(QwtPlot *, int16_t);
	~IQDisplay();
void	DisplayIQ		(std::complex<float> *, float);
private:
	int32_t			x_amount;
	std::vector<double>	plotData;
	std::vector<double>	plot2;
	std::vector<complex<float> > Points;
	QwtPlot			*plotgrid;
	int			_OutputRate;
	int			Radius;
	int			CycleCount;
private slots:
};
#endif

