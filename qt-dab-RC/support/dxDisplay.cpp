#
/*
 *    Copyright (C) 2024
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
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"dxDisplay.h"
#include	"radio.h"
#include	"position-handler.h"
#include	<QHeaderView>

	dxDisplay::dxDisplay (RadioInterface *mr, QSettings *s){
	dxSettings	= s;
	myWidget	= new QScrollArea (nullptr);
	myWidget	-> resize (220, 400);
	myWidget	-> setWidgetResizable(true);

	tableWidget 	= new QTableWidget (0, 3);
	tableWidget	-> setColumnWidth (0, 60);
	tableWidget	-> setColumnWidth (1, 200);
	tableWidget	-> setColumnWidth (2, 200);
	QHeaderView *headerView = tableWidget -> horizontalHeader ();
	headerView	-> setSectionResizeMode (1, QHeaderView::Stretch);
	headerView	-> resizeSection (0, 50);
	myWidget	-> setWidget(tableWidget);
	tableWidget 	-> setHorizontalHeaderLabels (
	            QStringList () << tr ("r") << tr ("transmitter"));
	set_position_and_size (s, myWidget, "DX_DISPLAY");
	myWidget	-> setWindowTitle ("dx display");
}

	dxDisplay::~dxDisplay () {
	store_widget_position (dxSettings, myWidget, "DX_DISPLAY");
	int16_t	rows	= tableWidget -> rowCount ();
	for (int row = rows; row > 0; row --)
	   tableWidget -> removeRow (row);
	delete	tableWidget;
	delete	myWidget;
}

void	dxDisplay::cleanUp	() {
	int16_t	rows	= tableWidget -> rowCount ();
	for (int row = rows; row >= 0; row --)
	   tableWidget -> removeRow (row);
}

int	dxDisplay::nrRows	() {
	return tableWidget -> rowCount ();
}

void	dxDisplay::show	() {
	myWidget	-> show ();
}

void	dxDisplay::hide	() {
	myWidget	-> hide ();
}

void	dxDisplay::addRow (const QString &tr, const QString &ds, bool b) {
int16_t	row	= tableWidget -> rowCount ();

	tableWidget	-> insertRow (row);
	QTableWidgetItem *item0	= new QTableWidgetItem;
	item0		-> setTextAlignment (Qt::AlignRight |Qt::AlignVCenter);
	tableWidget	-> setItem (row, 0, item0);

	QTableWidgetItem *item1 = new QTableWidgetItem;
	item1		-> setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 1, item1);

	QTableWidgetItem *item2 = new QTableWidgetItem;
	item2		-> setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 2, item2);

	tableWidget	-> setCurrentItem (item0);
	tableWidget	-> item (row, 1) -> setText (tr);
	tableWidget	-> item (row, 2) -> setText (ds);
	tableWidget	-> item (row, 0) -> setText (b ? "***" : "");
}

