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

//
//	Simple viewer for correlation
//
#ifndef		__SNR_VIEWER__
#define		__SNR_VIEWER__

#include        "dab-constants.h"
#include	<QFrame>
#include	<QSettings>
#include	<QObject>
#include	<vector>
#include	<atomic>
#include	<stdio.h>
#include	"ui_snr-widget.h"
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
#include        <qwt_picker_machine.h>
#include        <qwt_scale_widget.h>
#include        <QBrush>


class	RadioInterface;
class	snrViewer: public QObject, Ui_snrWidget {
Q_OBJECT
public:
			snrViewer	(RadioInterface *,
	                                 QSettings	*);
			~snrViewer	();
	void		show_snr	();
	void		add_snr		(float);
	void		add_snr		(float, float);
	void		show		();
	void		hide		();
	bool		isHidden	();

private:
	RadioInterface	*myRadioInterface;
	QSettings	*dabSettings;
	QFrame		myFrame;
	QwtPlotCurve	spectrum_curve;
	QwtPlotCurve	baseLine_curve;
	QwtPlotGrid	grid;
	QwtPlotPicker	*lm_picker;
	QwtPickerMachine	*lpickerMachine;
	std::vector<double>	X_axis;
	std::vector<double>	Y_Buffer;
	float		get_db		(float);
	int16_t		displaySize;
	QwtPlot		*plotgrid;
        QColor          displayColor;
        QColor          gridColor;
        QColor          curveColor;
	int		plotLength;
	int		plotHeight;
	int		delayCount;
	int		delayBufferP;
	void		addtoView		(float);
	std::atomic<FILE *>	snrDumpFile;
	void		startDumping		();
	void		stopDumping		();
private slots:
        void            rightMouseClick (const QPointF &);
	void		handle_snrDumpButton	();
	void		set_snrHeight		(int);
	void		set_snrLength		(int);
	void		set_snrDelay		(int);
};

#endif

