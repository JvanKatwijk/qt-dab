#
/*
 *    Copyright (C) 2014 .. 2023
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
#pragma once

#include	<QSettings>
#include        "dab-constants.h"
#include	<QFrame>
#include	<QObject>

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

#include	"fft-handler.h"
class	RadioInterface;

class	audioDisplay: public QObject {
Q_OBJECT
public:
			audioDisplay		(RadioInterface *,
	                                         QwtPlot	*, 	
	                                         QSettings *);
			~audioDisplay		();
	void		createSpectrum		(int16_t *, int, int);
private:
	RadioInterface	*myRadioInterface;
	QSettings	*dabSettings;
	QwtPlot		*plotGrid;
	QwtPlotCurve	spectrumCurve;
	QBrush		*ourBrush;
	QwtPlotGrid	grid;
	int16_t		displaySize;
	int16_t		spectrumSize;
	double		displayBuffer [512];
	Complex		*spectrumBuffer;
	float		Window [4 * 512];
	fftHandler	fft;
	QwtPlotPicker   *lm_picker;
	QColor		displayColor;
	QColor		gridColor;
	QColor		curveColor;
	bool		brush;
	void		ViewSpectrum		(double *, double *, double, int);
	float		get_db 			(float);
	int32_t		normalizer;
private slots:
        void            rightMouseClick		(const QPointF &);
};

