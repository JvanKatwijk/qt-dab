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
#ifndef		__TII_VIEWER__
#define		__TII_VIEWER__

#include	<QSettings>
#include        "dab-constants.h"
#include	<QFrame>
#include	<QObject>
#include	<QByteArray>
#include	<fftw3.h>
#include	"ui_tii-widget.h"
#include	"ringbuffer.h"
#include	<qwt.h>
#include	<qwt_plot.h>
#include	<qwt_plot_marker.h>
#include	<qwt_plot_grid.h>
#include	<qwt_plot_curve.h>
#include	<qwt_plot_marker.h>
#include        <qwt_color_map.h>
#include        <qwt_plot_zoomer.h>
#include        <qwt_plot_textlabel.h>
#include        <qwt_plot_panner.h>
#include        <qwt_plot_layout.h>
#include        <qwt_picker_machine.h>
#include        <qwt_scale_widget.h>
#include        <QBrush>

class	RadioInterface;

class	tiiViewer: public QObject, Ui_tiiWidget {
Q_OBJECT
public:
			tiiViewer		(RadioInterface *,
	                                         QSettings *,
	                                         RingBuffer<std::complex<float>> *);
			~tiiViewer		();
	void		showSpectrum		(int32_t);
	void		showTransmitters	(QByteArray);
	void		setBitDepth		(int16_t);
	void		show			();
	void		hide			();
	bool		isHidden		();
	void		clear			();
private:
	RadioInterface	*myRadioInterface;
	QSettings	*dabSettings;
	RingBuffer<std::complex<float>>	*tiiBuffer;
	int16_t		displaySize;
	int16_t		spectrumSize;
	std::complex<float>	*spectrum;
	std::vector<double>	displayBuffer;
	std::vector<float>	Window;
	QwtPlotPicker	*lm_picker;
	QColor		displayColor;
	QColor		gridColor;
	QColor		curveColor;

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
private slots:
	void		rightMouseClick	(const QPointF &);
};
#endif

