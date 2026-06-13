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
#include	<QGraphicsSimpleTextItem>
#include	<QChartView>
#include	<QChart>
#include	"clickable-chart.h"
#include	<vector>

typedef struct {
        int     offset;
        QLineSeries     *pLine;
        QGraphicsSimpleTextItem *text;
} plotMarker;


class basicScope : public QObject {
Q_OBJECT

public:
	basicScope	(clickableChart *, QSettings *, int, const QString &);
	~basicScope	();

void	showSpectrum	(double *, int, float, float, 
	                                float, float);

void	showSpectrum	(double *, int, float, float, 
	                                float, float,
	                                std::vector<markType> &);

private:
	clickableChart	*plotArea;
	QSettings	*scopeSettings;
	int		displaySize;
	QString		scopeName;
	QChart		*theChart;
	QValueAxis	*X_axis;
	QValueAxis	*Y_axis;
	QLineSeries	*ValueLine;

	std::vector<plotMarker>markerStack;
public slots:
	void		rightMouseClick	();
};
