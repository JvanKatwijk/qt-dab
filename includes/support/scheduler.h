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
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#pragma once

#include	<QWidget>
#include	<QScrollArea>
#include	<QTableWidget>
#include	<QStringList>
#include	<QTableWidgetItem>
#include	<QObject>
#include	<QString>
#include	<QTimer>
#include	<QDate>

class	RadioInterface;

class Scheduler:public QObject {
Q_OBJECT
public:
		Scheduler	(RadioInterface *, const QString &);
		~Scheduler	();
	void	addRow		(const QString &, int, int, int);
	void	show		();
	void	hide		();
	void	dump		(const QString &);
	void	read		(const QString &);
public slots:
	void	removeRow	(int, int);
	void	handle_timeOut	();
signals:
	void	timeOut		(const QString &);
private:
	void		clear	();
	QTimer		wakeupTimer;
	QScrollArea	myWidget;
	QTableWidget	*tableWidget;
	int		wakeupTime;
	int		wakeupIndex;
	QDate		referenceDate;
	QString		fileName;
};

