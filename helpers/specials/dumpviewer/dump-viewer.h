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

#ifndef	__DUMP_VIEWER__
#define	__DUMP_VIEWER__
#include	<QDialog>
#include        <qwt.h>
#include        <qwt_plot.h>
#include        <qwt_plot_marker.h>
#include        <qwt_plot_grid.h>
#include        <qwt_plot_curve.h>
#include        <qwt_color_map.h>
#include        <qwt_plot_zoomer.h>
#include        <qwt_plot_textlabel.h>
#include        <qwt_plot_panner.h>
#include        <qwt_plot_layout.h>
#include        <qwt_picker_machine.h>
#include        <qwt_scale_widget.h>

#include	<QLabel>
#include	<QMessageBox>
#include	"ui_dumpwidget.h"
//

class	dumpViewer:public QDialog,
	            public Ui_dumpViewer {
Q_OBJECT
public:
		dumpViewer	(FILE *, QWidget *parent = NULL);
		~dumpViewer	(void);

private	slots:
	void	handle_viewSlider	(int);
	void	handle_amplitudeSlider	(int);
	void	handle_compressor	(int);
private:
	FILE		*theFile;
	float		seconds_per_frame;
	float		seconds_per_sample;
	void		show_segment	(int, int);
	int		fileLength;
	QwtPlot         *plotgrid;
        QwtPlotGrid     *grid;
        QwtPlotCurve    *spectrumCurve;
        QwtPlotPicker   *lm_picker;
        QColor          displayColor;
        QColor          gridColor;
        QColor          curveColor;
};
#endif


