#
/*
 *    Copyright (C) 2016 .. 2024
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#pragma once

#include        <QWidget>
#include        <QObject>
#include        <QScrollArea>
#include        <QTableWidget>
#include        <QStringList>
#include        <QTableWidgetItem>
#include	<QPushButton>
#include	<QPixmap>
#include	<QLabel>
#include	"xml-extractor.h"

class	QSettings;
class	RadioInterface;

class	timeTableHandler: public QObject {
Q_OBJECT
public:
		timeTableHandler	(RadioInterface *radio,
	                                 QSettings *);
		~timeTableHandler	();
	void	display			(const scheduleDescriptor &);
	void	clear			();
	void    show            ();
        void    hide            ();
        bool    isVisible       ();
	void	setUp		(const QDate &, uint32_t,
	                                uint16_t, const QString &);
	void	addLogo		(const QPixmap &);
private:
	void		addRow		();
	void		addHeader	(const scheduleDescriptor &);
	void		addProgram	(const programDescriptor &);
	RadioInterface	*radio;
	QScrollArea     *myWidget;
        QTableWidget    *programDisplay;
	QPushButton	*left;
	QPushButton	*right;
	QLabel		*dateLabel;
	QLabel		*serviceLabel;
	QLabel		*serviceLogo;
	QString		serviceName;
	QDate		startDate;
	uint32_t	ensembleId;
	uint32_t	serviceId;
	int		dateOffset;
	void		start		(int);
	scheduleDescriptor	process_schedule (const QDomElement &,
	                                          QDate, 
	                                          uint32_t &ensembleId,
	                                          uint32_t &serviceId);

	QString		find_xmlFile (QDate& theDate,
                                      uint32_t Eid, uint32_t Sid);
QString fileName;

	QString		path_for_files;
private slots:
	void		handleLeft	();
	void		handleRight	();
};

