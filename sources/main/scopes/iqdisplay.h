#
/*
 *    Copyright (C) 2026
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

#include	"dab-constants.h"
#include	<QObject>
#include	<QString>
#include	<QSettings>
#include	<QValueAxis>
#include	<QLineSeries>
#include	<QScatterSeries>
#include	<QGraphicsSimpleTextItem>
#include	<QChartView>
#include	<QChart>
#include	"clickable-chart.h"
#include	<vector>

class	iqDisplay: public QObject {
Q_OBJECT
public:
		iqDisplay	(clickableChart *plotArea,
	                         QSettings *dabSettings);
		~iqDisplay	();
	void	displayIQ	(const std::vector<Complex> &, float);
	void	display_centerPoints	(const std::vector<Complex> &, float);
private:
	void	set_fatPoint (Complex, int, float);
        void	extract_centerPoints (const std::vector<Complex> &, Complex *);

	QChart		*theChart;
	QValueAxis	*X_axis;
	QValueAxis	*Y_axis;
	QScatterSeries	*ValueLine;

	QSettings	*scopeSettings;
};
