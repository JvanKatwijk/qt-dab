#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB.
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

//
//	Simple spectrum scope object
//	Shows the spectrum of the incoming data stream 
//	If made invisible, it is a "do nothing"
//
#ifndef		__SPECTRUM_VIEWER__
#define		__SPECTRUM_VIEWER__

#include        "dab-constants.h"
#include	<QFrame>
#include	<QObject>
#include	<fftw3.h>
#include	"ui_scopewidget.h"
#include	"ringbuffer.h"
#include	<qwt.h>
#include	<qwt_plot.h>
#include	<qwt_plot_marker.h>
#include	<qwt_plot_grid.h>
#include	<qwt_plot_curve.h>
#include	<qwt_plot_marker.h>

class	RadioInterface;
class	QSettings;
class	IQDisplay;
class	spectrumViewer: public QObject, Ui_scopeWidget {
Q_OBJECT
public:
			spectrumViewer	(RadioInterface *,
	                                 QSettings	*,
	                                 RingBuffer<std::complex<float>> *,
	                                 RingBuffer<std::complex<float>> *);
			~spectrumViewer();
	void		showSpectrum	(int32_t, int32_t);
	void		showIQ		(int32_t);
	void		showQuality	(float);
	void		setBitDepth	(int16_t);
	void		show();
	void		hide();
	bool		isHidden();
private:
	RadioInterface	*myRadioInterface;
	QSettings	*dabSettings;
	RingBuffer<std::complex<float>>	*spectrumBuffer;
	RingBuffer<std::complex<float>>	*iqBuffer;
	int16_t		displaySize;
	int16_t		spectrumSize;
	std::complex<float>	*spectrum;
	std::vector<double>	displayBuffer;
	std::vector<float>	Window;
	fftwf_plan	plan;
	QFrame		*myFrame;
	QwtPlotMarker	*Marker;
	QwtPlot		*plotgrid;
	QwtPlotGrid	*grid;
	QwtPlotCurve	*spectrumCurve;
	QBrush		*ourBrush;
	int32_t		indexforMarker;
	void		ViewSpectrum		(double *, double *, double, int);
	float		get_db 			(float);
	int32_t		normalizer;

	IQDisplay	*myIQDisplay;
};

#endif

