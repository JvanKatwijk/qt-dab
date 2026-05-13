#
/*
 *    Copyright (C) 2016 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB 
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

#include	<QWidget>
#include	<QObject>
#include	<QScrollArea>
#include	<QTableWidget>
#include	<QStringList>
#include	<QTableWidgetItem>
#include	<QObject>
#include	<QString>

class	RadioInterface;
class	QSettings;

class   processMonitor:public QObject {
Q_OBJECT

public:
		processMonitor	(RadioInterface *);
		~processMonitor	();
	void	show		();
	void	hide		();
	bool	isVisible	();
	void	addProcess	(const QString &, uint32_t, 
	                         uint8_t SCIds, uint8_t subChId, int);
	void	removeProcess	(const QString &, uint32_t, uint8_t);
	void	clearTable	();
private:
        QString         channel;

        int             columns;
        RadioInterface  *theRadio;
        QScrollArea     *myWidget;
        QTableWidget    *contentWidget;
public slots:
	void		process_clickRequest	(int, int);
signals:
	void		handle_clickRequest	(const QString &);
};


