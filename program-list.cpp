#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the  qt-dab program
 *
 *    qt-dab is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    qt-dab is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"preset-list.h"
#include	"radio.h"
#include	<QFile>
#include	<QDataStream>

	presetList::presetList (RadioInterface *mr,
	                          QString saveName) {
	this	-> saveName	= saveName;
	myWidget	= new QScrollArea (NULL);
	myWidget	-> resize (240, 200);
	myWidget	-> setWidgetResizable(true);

	tableWidget 	= new QTableWidget (0, 2);
	myWidget	-> setWidget(tableWidget);
	tableWidget 	-> setHorizontalHeaderLabels (
	            QStringList () << tr ("station") << tr ("frequency"));
	connect (tableWidget, SIGNAL (cellClicked (int, int)),
	         this, SLOT (tableSelect (int, int)));
	connect (tableWidget, SIGNAL (cellDoubleClicked (int, int)),
	         this, SLOT (removeRow (int, int)));
	connect (this, SIGNAL (newFrequency (int)),
	         mr, SLOT (newFrequency (int)));
	loadTable ();
}

	presetList::~presetList (void) {
int16_t	i;
int16_t	rows	= tableWidget -> rowCount ();

	for (i = rows; i > 0; i --)
	   tableWidget -> removeRow (i);
	delete	tableWidget;
	delete	myWidget;
}

void	presetList::show	(void) {
	myWidget	-> show ();
}

void	presetList::hide	(void) {
	myWidget	-> hide ();
}

void	presetList::addRow (const QString &name, const QString &channel) {
int16_t	row	= tableWidget -> rowCount ();

	tableWidget	-> insertRow (row);
	QTableWidgetItem *item0	= new QTableWidgetItem;
	item0		-> setTextAlignment (Qt::AlignRight |Qt::AlignVCenter);
	tableWidget	-> setItem (row, 0, item0);

	QTableWidgetItem *item1 = new QTableWidgetItem;
	item1		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 1, item1);

	tableWidget	-> setCurrentItem (item0);
	tableWidget	-> item (row, 0) -> setText (channel);
	tableWidget	-> item (row, 1) -> setText (name);
}
//
//	Locally we dispatch the "click" and "translate"
//	it into a frequency and a call to the main gui to change
//	the frequency

void	presetList::tableSelect (int row, int column) {
QTableWidgetItem* theItem_1 = tableWidget -> item (row, 1);
QTableWidgetItem* tgeItem_2 = tableWidget -> item (row, 2);
	(void)column;
	QString channel	= theItem_1 -> text ();
	QString service = theItem_2 -> text ();
	emit select_presetService (channel, service);
}

void	programList::removeRow (int row, int column) {
	tableWidget	-> removeRow (row);
	(void)column;
}

void	programList::saveTable (void) {
QFile	file (saveName);

	if (file. open (QIODevice::WriteOnly)) {
	   QDataStream stream (&file);
	   int32_t	n = tableWidget -> rowCount ();
	   int32_t	m = tableWidget -> columnCount ();
	   stream << n << m;

	   for (int i = 0; i < n; i ++) 
	      for (int j = 0; j < m; j ++) 
	         tableWidget -> item (i, j) -> write (stream);
	   file. close ();
	}
}

void	programList::loadTable (void) {
QFile	file (saveName);

	if (file. open (QIODevice::ReadOnly)) {
	   QDataStream stream (&file);
	   int32_t	n, m;
	   stream >> n >> m;
	   tableWidget	-> setRowCount (n);
	   tableWidget	-> setColumnCount	(m);

	   for (int i = 0; i < n; i ++) {
	      for (int j = 0; j < m; j ++) {
	         QTableWidgetItem *item = new QTableWidgetItem;
	         item -> read (stream);
	         tableWidget -> setItem (i, j, item);
	      }
	   }
	   file. close ();
	}
}

