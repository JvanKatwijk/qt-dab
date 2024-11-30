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
#include	<QHBoxLayout>
#include	<qwt_compass.h>
#include	<qlayout.h>
#include	<qwt_compass_rose.h>
#include	<qwt_dial_needle.h>
#include	<QMap>

	dxDisplay::dxDisplay (RadioInterface *mr, QSettings *s){
	dxSettings	= s;
	myWidget	= new QScrollArea (nullptr);
	myWidget	-> resize (220, 400);
	myWidget	-> setWidgetResizable(true);

	tableWidget 	= new QTableWidget (0, 12);
	tableWidget	-> setColumnWidth (0, 30);
	tableWidget	-> setColumnWidth (1, 70);
	tableWidget	-> setColumnWidth (2, 70);
	tableWidget	-> setColumnWidth (3, 100);
	tableWidget	-> setColumnWidth (4, 100);
	tableWidget	-> setColumnWidth (5, 200);
	tableWidget	-> setColumnWidth (6, 70);
	tableWidget	-> setColumnWidth (7, 70);
	tableWidget	-> setColumnWidth (8, 70);
	tableWidget	-> setColumnWidth (9, 70);
	tableWidget	-> setColumnWidth (10, 70);
	tableWidget	-> setColumnWidth (11, 70);
	QHeaderView *headerView = tableWidget -> horizontalHeader ();
	headerView	-> setSectionResizeMode (1, QHeaderView::Stretch);
//	headerView	-> resizeSection (0, 50);
	tableWidget 	-> setHorizontalHeaderLabels (
	                QStringList () << tr ("x") << tr ("mainId") <<
	                tr ("subId") << tr ("channel") <<tr ("ensemble") <<
	                tr ("transmiiter") << tr ("dist") <<
	                tr ("azimuth")  << tr ("power") <<
	                tr ("alt") << tr ("height") << tr ("direction"));

	theDial		= new QwtCompass ();

	QMap<double, QString> map;
	map.insert(0.0, "N");
	map.insert(90.0, "E");
	map.insert(180.0, "S");
	map.insert(270.0, "W");

	QwtSimpleCompassRose *rose = new QwtSimpleCompassRose( 4, 1 );
            theDial->setRose( rose );
	theDial -> setNeedle (
	       new QwtCompassMagnetNeedle(QwtCompassMagnetNeedle::ThinStyle));
	theDial -> setValue (220.0);

	QHBoxLayout *l	= new QHBoxLayout (myWidget);
	l -> addWidget (tableWidget);
	l -> addWidget (theDial);
	set_position_and_size (s, myWidget, "DX_DISPLAY");
	myWidget	-> setWindowTitle ("dx display");
	theChannel	= "";
}

	dxDisplay::~dxDisplay () {
	store_widget_position (dxSettings, myWidget, "DX_DISPLAY");
	int16_t	rows	= tableWidget -> rowCount ();
	for (int row = rows; row > 0; row --)
	   tableWidget -> removeRow (row);
	delete	tableWidget;
	delete	myWidget;
}

void	dxDisplay::setChannel (const QString &channel) {
	myWidget -> setWindowTitle (channel);
	theChannel	= channel;
}

void	dxDisplay::cleanUp	() {
	int16_t	rows	= tableWidget -> rowCount ();
	for (int row = rows; row >= 0; row --)
	   tableWidget -> removeRow (row);
	myWidget -> setWindowTitle ("dx display");
	theChannel	= "";
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

void	dxDisplay::addRow (cacheElement *theTransmitter, bool isStrongest) {
//void	dxDisplay::addRow (uint8_t mainId, uint8_t subId,
//	                   const QString &channel, const QString &ensemble, 
//	                   const QString &transmitterName,
//	                   float distance, int azimuth, float strength,
//	                   float power, int altitude, int height,
//	                   const QString &dir, bool b) {
int16_t	row	= tableWidget -> rowCount ();

	const QString &channel	= theTransmitter -> channel;
	const QString &ensemble	= theTransmitter -> ensemble;
	const QString &transmitterName = theTransmitter -> transmitterName;	
	int   mainId		= theTransmitter -> mainId;
	int   subId		= theTransmitter -> subId;
	float distance		= theTransmitter -> distance;
	float azimuth		= theTransmitter -> azimuth;
	float strength		= theTransmitter -> strength;
	float power		= theTransmitter -> power;
	int   altitude		= theTransmitter -> altitude;
	int  height		= theTransmitter -> height;
	const QString &dir	= theTransmitter -> direction;

	bool	b		= isStrongest;

	tableWidget	-> insertRow (row);
	QTableWidgetItem *item0	= new QTableWidgetItem;
	item0		-> setTextAlignment (Qt::AlignRight |Qt::AlignVCenter);
	tableWidget	-> setItem (row, 0, item0);

	QTableWidgetItem *item1 = new QTableWidgetItem;
	item1		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 1, item1);

	QTableWidgetItem *item2 = new QTableWidgetItem;
	item2		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 2, item2);

	QTableWidgetItem *item3 = new QTableWidgetItem;
	item3		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 3, item3);

	QTableWidgetItem *item4 = new QTableWidgetItem;
	item4		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 4, item4);

	QTableWidgetItem *item5 = new QTableWidgetItem;
	item5		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 5, item5);

	QTableWidgetItem *item6 = new QTableWidgetItem;
	item6		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 6, item6);

	QTableWidgetItem *item7 = new QTableWidgetItem;
	item7		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 7, item7);

	QTableWidgetItem *item8 = new QTableWidgetItem;
	item8		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 8, item8);

	QTableWidgetItem *item9 = new QTableWidgetItem;
	item9		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 9, item9);

	QTableWidgetItem *item10 = new QTableWidgetItem;
	item10		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 10, item10);

	QTableWidgetItem *item11 = new QTableWidgetItem;
	item11		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 11, item11);

	tableWidget	-> setCurrentItem (item0);
	tableWidget	-> item (row, 1) -> setText (QString::number (mainId));
	tableWidget	-> item (row, 2) -> setText (QString::number (subId));
	tableWidget	-> item (row, 3) -> setText (channel);
	tableWidget	-> item (row, 4) -> setText (ensemble);
	tableWidget	-> item (row, 5) -> setText (transmitterName);
	tableWidget	-> item (row, 6) -> setText (QString::number (distance, 'f', 1) + " km");
	tableWidget	-> item (row, 7) -> setText (QString::number (azimuth, 'f', 1) + QString::fromLatin1 (" \xb0 "));
	tableWidget	-> item (row, 8) -> setText (QString::number (power, 'f', 1) + " KW ");
	tableWidget	-> item (row, 9) -> setText (QString::number (altitude) +  " m");
	tableWidget	-> item (row, 10) -> setText (QString::number (height) +  " m");
	tableWidget	-> item (row, 11) -> setText (dir);
	tableWidget	-> item (row, 0) -> setText (b ? "***" : "");
	if (b)
	   theDial -> setValue (azimuth);
}

