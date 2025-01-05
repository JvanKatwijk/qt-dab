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

static inline
QString	convertNumber (uint16_t v) {
uint16_t	firstDigit (v / 10);
uint16_t	secondDigit (v % 10);
	return QString::number (firstDigit) +
	                  QString::number (secondDigit);
}

	dxDisplay::dxDisplay (RadioInterface *mr, QSettings *s){
	(void)mr;
	dxSettings	= s;
	myWidget	= new QScrollArea (nullptr);
	myWidget	-> resize (220, 400);
	myWidget	-> setWidgetResizable (true);

	tableWidget 	= new QTableWidget (0, 14);
	tableWidget	-> setColumnWidth (0, 30);
	tableWidget	-> setColumnWidth (1, 50);
	tableWidget	-> setColumnWidth (2, 50);
	tableWidget	-> setColumnWidth (3, 50);
	tableWidget	-> setColumnWidth (4, 60);
	tableWidget	-> setColumnWidth (5, 80);
	tableWidget	-> setColumnWidth (6, 100);
	tableWidget	-> setColumnWidth (7, 200);
	tableWidget	-> setColumnWidth (8, 70);
	tableWidget	-> setColumnWidth (9, 70);
	tableWidget	-> setColumnWidth (10, 70);
	tableWidget	-> setColumnWidth (11, 70);
	tableWidget	-> setColumnWidth (12, 70);
	tableWidget	-> setColumnWidth (13, 70);

	QHeaderView *headerView = tableWidget -> horizontalHeader ();
	headerView	-> setSectionResizeMode (1, QHeaderView::Stretch);
//	headerView	-> resizeSection (0, 50);
	tableWidget 	-> setHorizontalHeaderLabels (
	                QStringList () << tr ("x") << tr ("pattern") <<
	                tr ("tii") << tr ("phase") << tr ("strength") <<
	                tr ("channel") <<tr ("ensemble") <<
	                tr ("transmitter") << tr ("dist") <<
	                tr ("azimuth")  << tr ("power") <<
	                tr ("alt") << tr ("height") << tr ("direction"));

	theDial		= new QwtCompass ();
	theDial		-> setLineWidth (8);

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

	QHBoxLayout *l	= new QHBoxLayout ();
	l -> addWidget (tableWidget);
	l -> addWidget (theDial);
	myWidget	-> setLayout (l);
	myWidget	-> setWindowTitle ("dx display");
	set_position_and_size (s, myWidget, "DX_DISPLAY");
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
int16_t	row	= tableWidget -> rowCount ();

	const QString &channel	= theTransmitter -> channel;
	const QString &ensemble	= theTransmitter -> ensemble;
	const QString &transmitterName = theTransmitter -> transmitterName;	
	uint16_t pattern	= theTransmitter -> pattern;
	int   mainId		= theTransmitter -> mainId;
	int   subId		= theTransmitter -> subId;
	bool	etsi		= theTransmitter -> norm;
	float	phase		= theTransmitter -> phase;
	float	strength	= theTransmitter -> strength;
	float distance		= theTransmitter -> distance;
	float azimuth		= theTransmitter -> azimuth;
	float power		= theTransmitter -> power;
	int   altitude		= theTransmitter -> altitude;
	int  height		= theTransmitter -> height;
	const QString &dir	= theTransmitter -> direction;
	bool	b		= isStrongest;

	tableWidget	-> insertRow (row);	// the mark
	QTableWidgetItem *item0	= new QTableWidgetItem;
	item0		-> setTextAlignment (Qt::AlignRight |Qt::AlignVCenter);
	tableWidget	-> setItem (row, 0, item0);

	QTableWidgetItem *item1 = new QTableWidgetItem;	// pattern
	item1		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 1, item1);

	QTableWidgetItem *item2 = new QTableWidgetItem;	// tii data
	item2		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 2, item2);

	QTableWidgetItem *item3 = new QTableWidgetItem;	 // etsi
	item3		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 3, item3);

	QTableWidgetItem *item4 = new QTableWidgetItem;	 // strength
	item4		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 4, item4);

	QTableWidgetItem *item5 = new QTableWidgetItem;	// channel
	item5		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 5, item5);

	QTableWidgetItem *item6 = new QTableWidgetItem;	// ensemble
	item6		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 6, item6);

	QTableWidgetItem *item7 = new QTableWidgetItem;	// transmitter
	item7		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 7, item7);

	QTableWidgetItem *item8 = new QTableWidgetItem;	// dist
	item8		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 8, item8);

	QTableWidgetItem *item9 = new QTableWidgetItem;	// azimuth
	item9		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 9, item9);

	QTableWidgetItem *item10 = new QTableWidgetItem;	// power
	item10		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 10, item10);

	QTableWidgetItem *item11 = new QTableWidgetItem;	// alt
	item11		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 11, item11);

	QTableWidgetItem *item12 = new QTableWidgetItem;	// height
	item12		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 12, item12);

	QTableWidgetItem *item13 = new QTableWidgetItem;	// direction
	item13		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 13, item13);


	tableWidget	-> setCurrentItem (item0);
	QString aa	= QString ("0x") + QString::number (pattern, 16);
	tableWidget	-> item (row, 1) -> setText (aa);
	QString tii	= convertNumber (mainId) + "-" +
	                          convertNumber (subId);
	if (theTransmitter -> collision)
	   tii += "!";
	tableWidget	-> item (row, 2) -> setText (tii);
	QString ss	= QString::number (phase, 'f', 1) + 
	                         (etsi ? " *" : "");
	tableWidget	-> item (row, 3) -> setText (ss);
	tableWidget	-> item (row, 4) -> setText (QString::number (strength, 'f', 1) );
	tableWidget	-> item (row, 5) -> setText (channel);
	tableWidget	-> item (row, 6) -> setText (ensemble);
	tableWidget	-> item (row, 7) -> setText (transmitterName);
	tableWidget	-> item (row, 8) -> setText (QString::number (distance, 'f', 1) + " km");
	tableWidget	-> item (row, 9) -> setText (QString::number (azimuth, 'f', 1) + QString::fromLatin1 (" \xb0 "));
	tableWidget	-> item (row, 10) -> setText (QString::number (power, 'f', 1) + " KW ");
	tableWidget	-> item (row, 11) -> setText (QString::number (altitude) +  " m");
	tableWidget	-> item (row, 12) -> setText (QString::number (height) +  " m");
	tableWidget	-> item (row, 13) -> setText (dir);
	tableWidget	-> item (row, 0) -> setText (b ? "***" : "");
	if (b)
	   theDial -> setValue (azimuth);
}

void	dxDisplay::addRow (uint8_t mainId, uint8_t subId,
	                                       const QString &channel) {
int16_t	row	= tableWidget -> rowCount ();

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
	tableWidget	-> item (row, 4) -> setText ("unknown");
	tableWidget	-> item (row, 5) -> setText ("unknown");
	tableWidget	-> item (row, 6) -> setText ("??");
	tableWidget	-> item (row, 7) -> setText ("??");
	tableWidget	-> item (row, 8) -> setText ("??");
	tableWidget	-> item (row, 9) -> setText ("??");
	tableWidget	-> item (row, 10) -> setText ("??");
	tableWidget	-> item (row, 11) -> setText ("??");
	tableWidget	-> item (row, 0) -> setText ( "");
}

