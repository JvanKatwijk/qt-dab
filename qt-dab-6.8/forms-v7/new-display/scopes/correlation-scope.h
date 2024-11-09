#
/*
 *    Copyright (C) 2014 .. 2017
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

#pragma once

#include        "dab-constants.h"
#include	<QFrame>
#include	<QSettings>
#include	<QObject>
#include	"qwt-2.h"
#include	<qwt_plot.h>
#include	<qwt_plot_marker.h>
#include	<qwt_plot_grid.h>
#include	<qwt_plot_curve.h>
#include        <qwt_color_map.h>
#include        <qwt_plot_zoomer.h>
#include        <qwt_plot_textlabel.h>
#include        <qwt_plot_panner.h>
#include        <qwt_plot_layout.h>
#include        <qwt_picker_machine.h>
#include        <qwt_scale_widget.h>
#include        <QBrush>
#include	<QVector>


class	RadioInterface;
class	correlationScope: public QObject {
Q_OBJECT
public:
			correlationScope	(QwtPlot *rig,
	                                         int	displaySize,
	                                         QSettings	*);
			~correlationScope	();
	void		display			(const std::vector<float> &v,
	                                         int T_g,
	                                         int length,
		                                 int sliderValue);

private:
	QwtPlotCurve	spectrumCurve;
	QSettings	*dabSettings;
	QwtPlotPicker   *lm_picker;
        QColor          displayColor;
        QColor          gridColor;
        QColor          curveColor;
	int		plotLength;
	QwtPlot         *plotgrid;
        QwtPlotGrid     *grid;

	int		displaySize;

	float		get_db		(float);
private slots:
        void            rightMouseClick			(const QPointF &);
};

