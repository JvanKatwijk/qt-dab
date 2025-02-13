#
/*
 *    Copyright (C) 2016 .. 2024
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
//
//
#include	"time-table.h"
#include	"radio.h"

	timeTableHandler::timeTableHandler (RadioInterface *radio) {
	this	-> radio	= radio;
//
	myWidget        = new QScrollArea (nullptr);
        myWidget        -> setWidgetResizable (true);

        programDisplay	= new QTableWidget (0, 4);
        programDisplay	-> setColumnWidth (0, 250);
        programDisplay	-> setColumnWidth (3, 350);
        myWidget        -> setWidget (programDisplay);
        programDisplay	-> setHorizontalHeaderLabels (
                                  QStringList () << "program");
	addRow ();
}

	timeTableHandler::~timeTableHandler   () {
	clear ();
	delete	programDisplay;
	delete	myWidget;
}

void	timeTableHandler::display	(const scheduleDescriptor &schedule) {
	addHeader (schedule);
	for (auto &program : schedule. thePrograms)
	   addProgram (program);
	show ();
}

void	timeTableHandler::addHeader	(const scheduleDescriptor &schedule) {
int	row	= programDisplay -> rowCount ();
QString name	= schedule. name;
QString start	= schedule. startTime. toString ();
QString stop	= schedule. stopTime. toString ();
	programDisplay	-> insertRow (row);
	QTableWidgetItem *item0 = new QTableWidgetItem;
	item0	-> setTextAlignment (Qt::AlignLeft);
	item0	-> setText (name);
	programDisplay -> setItem (row, 0, item0);
	QTableWidgetItem *item1 = new QTableWidgetItem;
	item1	-> setTextAlignment (Qt::AlignLeft);
	item1	-> setText (QString::number (schedule. Sid, 16));
	programDisplay -> setItem (row, 1, item1);
	QTableWidgetItem *item2 = new QTableWidgetItem;
	item2	-> setTextAlignment (Qt::AlignLeft);
	item2	-> setText (start);
	programDisplay -> setItem (row, 2, item2);
	QTableWidgetItem *item3 = new QTableWidgetItem;
	item3	-> setTextAlignment (Qt::AlignLeft);
	item3	-> setText (stop);
	programDisplay -> setItem (row, 3, item3);
}

void	timeTableHandler::addProgram (const programDescriptor &program) {
int	row	= programDisplay -> rowCount ();
QString start		= program. startTime. toString ();
QString duration	= QString::number (program. duration) + "min";
	programDisplay	-> insertRow (row);
	QTableWidgetItem *item0 = new QTableWidgetItem;
	item0	-> setTextAlignment (Qt::AlignLeft);
	item0	-> setText (start);
	programDisplay -> setItem (row, 0, item0);
	QTableWidgetItem *item1 = new QTableWidgetItem;
	item1	-> setTextAlignment (Qt::AlignLeft);
	item1	-> setText (duration);
	programDisplay -> setItem (row, 1, item1);
	QTableWidgetItem *item2 = new QTableWidgetItem;
	item2	-> setTextAlignment (Qt::AlignLeft);
	item2	-> setText ("");
	programDisplay -> setItem (row, 2, item2);
	QTableWidgetItem *item3 = new QTableWidgetItem;
	item3	-> setTextAlignment (Qt::AlignLeft);
	item3	-> setText (program. program);
	programDisplay -> setItem (row, 3, item3);
}

void	timeTableHandler::addRow	() {	// add empty row
int	row	= programDisplay -> rowCount ();
	programDisplay	-> insertRow (row);
	for (int i = 0; i < 4; i ++) {
	   QTableWidgetItem *item = new QTableWidgetItem;
	   item	-> setText ("");
	   programDisplay -> setItem (row, i, item);
	}
}

void	timeTableHandler::clear () {
int	rows    = programDisplay -> rowCount ();
        for (int i = rows; i > 0; i --)
           programDisplay -> removeRow (i - 1);
}

void	timeTableHandler::show      () {
        myWidget        -> show ();
}

void	timeTableHandler::hide      () {
        myWidget        -> hide ();
}

bool	timeTableHandler::isVisible () {
        return !myWidget -> isHidden ();
}

