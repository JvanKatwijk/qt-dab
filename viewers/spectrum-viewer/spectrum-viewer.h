#
/*
 *    Copyright (C) 2014 .. 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB.
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
#include	"ui_scopewidget.h"
#include	"ringbuffer.h"
#include	<qwt.h>
#include	<qwt_plot.h>
#include	<qwt_plot_marker.h>
#include	<qwt_plot_grid.h>
#include	<qwt_plot_curve.h>
#include        <qwt_color_map.h>
#include        <qwt_plot_zoomer.h>
#include        <qwt_plot_textlabel.h>
#include        <qwt_plot_panner.h>
#include        <qwt_plot_layout.h>
#include	<qwt_picker_machine.h>
#include        <qwt_scale_widget.h>
#include        <QBrush>
#include        <QTimer>

#include	"spectrum-scope.h"
#include	"waterfall-scope.h"
#include	"null-scope.h"

#define	SP_DISPLAYSIZE	512
#define	SP_SPECTRUMSIZE	2048

#include	"fft-handler.h"
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
	void		showFrequency	(float);
	void		showIQ		(int32_t);
	void		show_nullPeriod	(float *, int);
	void		showQuality	(float, float, float);
	void		show_snr	(float);
	void		show_clockErr	(int);
	void		show_correction	(int);
	void		setBitDepth	(int16_t);
	void		show		();
	void		hide		();
	bool		isHidden	();
private:
	QFrame		myFrame;
	RadioInterface	*myRadioInterface;
	QSettings	*dabSettings;
	RingBuffer<std::complex<float>>	*spectrumBuffer;
	RingBuffer<std::complex<float>>	*iqBuffer;
	QwtPlotPicker	*lm_picker;
	QColor		displayColor;
	QColor		gridColor;
	QColor		curveColor;

	fftHandler	fft;
	std::complex<float>	spectrum [SP_SPECTRUMSIZE];
	double			displayBuffer [SP_DISPLAYSIZE];
	float		Window [SP_SPECTRUMSIZE];;
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
	spectrumScope	*mySpectrumScope;
	waterfallScope	*myWaterfallScope;
	nullScope	*myNullScope;
private slots:
	void		rightMouseClick		(const QPointF &);
};

#endif

