#
/*
 *    Copyright (C)   2015, 2016
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J.
 *    Many of the ideas as implemented in SDR-J are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
 *
 *    SDR-J is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    SDR-J is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with SDR-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

//
//	Simple spectrum scope object
//	can be in- or excluded from the set of sources to build
//	an dabreceiver.
//	Shows the spectrum of the incoming data stream and the
//	constellation of the bits in the signal of the first data
//	block of a dabframe
//
#ifndef		__SPECTRUM_HANDLER__
#define		__SPECTRUM_HANDLER__

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
class	spectrumhandler: public QObject, Ui_scopeWidget {
Q_OBJECT
public:
			spectrumhandler		(RadioInterface *,
	                                         QSettings	*,
	                                         RingBuffer<DSPCOMPLEX> *,
	                                         RingBuffer<DSPCOMPLEX> *);
			~spectrumhandler	(void);
	void		showSpectrum		(int32_t, int32_t);
	void		showIQ			(int32_t);
	void		showQuality		(float);
	void		setBitDepth		(int16_t);
	void		show			(void);
	void		hide			(void);
private:
	RadioInterface	*myRadioInterface;
	QSettings	*dabSettings;
	RingBuffer<DSPCOMPLEX>	*scopeBuffer;
	RingBuffer<DSPCOMPLEX>	*iqBuffer;
	int16_t		displaySize;
	int16_t		spectrumSize;
	DSPCOMPLEX	*spectrum;
	double		*displayBuffer;
	float		*Window;
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

