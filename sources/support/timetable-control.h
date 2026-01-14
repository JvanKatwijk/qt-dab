#
/*
 *    Copyright (C) 2016 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
 *    along with dab-scanner; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#pragma once

#include	<QScrollArea>
#include	<QString>
#include	<QStringList>
#include	<stdint.h>
#include	<QDate>
#include	<QTableWidget>
#include	<QTableWidgetItem>
#include	"radio.h"
#include	"dab-constants.h"
#include	"findfilenames.h"
#include	"super-frame.h"

class	QSettings;
class	timeTableHandler;

class	timeTableControl: public superFrame {
Q_OBJECT
public:
		timeTableControl (QString &ensemble, int32_t EId,
	                          std::vector<basicService> &services,
	                          QDate today,
	                          QSettings	*);
		~timeTableControl ();

private:
	class element {
	  public:
	      int32_t	SId;
	      bool	hasTable;
	      QString	serviceName;
	};
	QScrollArea	*theScrollArea;
	QTableWidget	*contentWidget;
	std::vector<basicService> services;
	QString		ensemble;
	int32_t		EId;
	QDate		today;
	QSettings	*dabSettings;
	QString		path_for_files;
	std::vector<element> otherTable;
	timeTableHandler *serviceHandler;
	void		addRow  	(const QString &, int32_t, bool);
	bool		checkFor 	(const QStringList &, int32_t);
public	slots:
	void		selectService	(int, int);
	void		cleanup_serviceHandler	();
};

