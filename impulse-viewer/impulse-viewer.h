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
//	Simple viewer for impulse responses
//
#ifndef		__IMPULSE_VIEWER__
#define		__IMPULSE_VIEWER__

#include        "dab-constants.h"
#include	<QFrame>
#include	"ringbuffer.h"
#include	<QObject>
#include	"ui_impulse-widget.h"
#include	<qwt.h>
#include	<qwt_plot.h>
#include	<qwt_plot_marker.h>
#include	<qwt_plot_grid.h>
#include	<qwt_plot_curve.h>
#include	<qwt_plot_marker.h>

class	RadioInterface;
class	impulseViewer: public QObject, Ui_impulseWidget {
Q_OBJECT
public:
			impulseViewer		(RadioInterface *,
	                                         RingBuffer<float> *);
			~impulseViewer();
	void		showImpulse		(int32_t);
	void		showIndex		(int32_t);
	void		show();
	void		hide();
	bool		isHidden();

private:
	std::vector<int> indexVector;
	float		get_db			(float);
	RadioInterface	*myRadioInterface;
	RingBuffer<float> *responseBuffer;
	int16_t		displaySize;
	QFrame		*myFrame;
	QwtPlot		*plotgrid;
	QwtPlotGrid	*grid;
	QwtPlotCurve	*spectrumCurve;
	QBrush		*ourBrush;
};

#endif

