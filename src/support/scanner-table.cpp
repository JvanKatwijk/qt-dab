#
/*
 *    Copyright (C) 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
#include        <QDataStream>
#include	"scanner-table.h"
#include        "radio.h"

	scannerTable::scannerTable (RadioInterface *) {
	myWidget        = new QScrollArea (NULL);
        myWidget        -> resize (200, 200);
        myWidget        -> setWidgetResizable(true);
	
        outputWidget     = new QTableWidget (0, 6);
	outputWidget	-> setColumnWidth (0, 150);
	outputWidget	-> setColumnWidth (4, 150);
        myWidget        -> setWidget (outputWidget);
        outputWidget	-> setHorizontalHeaderLabels (
	                          QStringList () << tr ("scan result"));
	is_clear	= true;
}

	scannerTable::~scannerTable () {
int16_t i;
int16_t rows    = outputWidget -> rowCount ();

	if (!is_clear)
           for (i = rows; i > 0; i --)
              outputWidget -> removeRow (i - 1);
        delete  outputWidget;
        delete  myWidget;
}

void	scannerTable::dump	(const QString &fileName) {
FILE	*dumpFile;
	dumpFile	= fopen (fileName. toUtf8 (). data (), "w");
	if (dumpFile == nullptr)
	   return;

	for (int i = 0; i < outputWidget -> rowCount (); i ++) {
	   for (int j = 0; j < outputWidget -> columnCount (); j ++) {
	      QString t = outputWidget -> item (i, j) -> text ();
	      fprintf (dumpFile, "%s;", t. toUtf8 (). data ());
	   }
	   fprintf (dumpFile, "\n");
	}
	fclose (dumpFile);
}

void	scannerTable::clear	() {
int	rows	= outputWidget -> rowCount ();
	if (is_clear)
	   return;
	for (int i = rows; i > 0; i --) 
	   outputWidget -> removeRow (i - 1);
	is_clear	= true;
}

void	scannerTable::show	() {
	myWidget	-> show ();
}

void	scannerTable::hide	() {
	myWidget	-> hide ();
}

int16_t	scannerTable::addRow () {
int16_t row     = outputWidget -> rowCount ();

	is_clear	= false;
        outputWidget     -> insertRow (row);
        QTableWidgetItem *item0 = new QTableWidgetItem;
        item0           -> setTextAlignment (Qt::AlignLeft |Qt::AlignVCenter);
        outputWidget     -> setItem (row, 0, item0);

        QTableWidgetItem *item1 = new QTableWidgetItem;
        item1           -> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        outputWidget     -> setItem (row, 1, item1);

        QTableWidgetItem *item2 = new QTableWidgetItem;
        item2           -> setTextAlignment (Qt::AlignRight |Qt::AlignVCenter);
        outputWidget    -> setItem (row, 2, item2);

        QTableWidgetItem *item3 = new QTableWidgetItem;
        item3           -> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        outputWidget    -> setItem (row, 3, item3);

        QTableWidgetItem *item4 = new QTableWidgetItem;
        item4           -> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        outputWidget    -> setItem (row, 4, item4);

        QTableWidgetItem *item5 = new QTableWidgetItem;
        item4           -> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        outputWidget    -> setItem (row, 5, item5);
	return row;
}

static
QString textfor (QByteArray transmitters) {
QString res;
	if (transmitters. size () == 0)
	   return " ";
	for (int i = 0; i < transmitters. size (); i += 2)
	   res = res + " (" + QString::number (transmitters. at (i)) +
	               "+" + QString::number (transmitters. at (i + 1)) + ")";
	return res;
}
	
void	scannerTable::newEnsemble	(QString currentChannel,
	                                 QString ensembleName,
	                                 QString ensembleId,
	                                 QString freq,
	                                 QString snr,
	                                 QByteArray transmitters) {
int16_t row	= addRow ();
	outputWidget	-> item (row, 0) -> setText (currentChannel);
	outputWidget	-> item (row, 1) -> setText (ensembleName);
	outputWidget	-> item (row, 2) -> setText (ensembleId);
	outputWidget	-> item (row, 3) -> setText (freq);
	outputWidget	-> item (row, 4) -> setText (snr);
	outputWidget	-> item (row, 5) -> setText (textfor (transmitters));
}

void	scannerTable::new_headline	() {
int16_t	row	= addRow ();
	outputWidget	-> item (row, 0) -> setText ("serviceName");
	outputWidget	-> item (row, 1) -> setText ("serviceId");
	outputWidget	-> item (row, 2) -> setText ("type ");
	outputWidget	-> item (row, 3) -> setText ("bit rate");
	outputWidget	-> item (row, 4) -> setText ("protection level");
	outputWidget	-> item (row, 5) -> setText ("code rate");
}

void	scannerTable::add_to_Ensemble	(QString serviceName,
	                                 QString serviceId,
	                                 QString dabType,
	                                 QString bitRate,
	                                 QString protLevel,
	                                 QString codeRate) {
int16_t	row = addRow ();

	outputWidget	-> item (row, 0) -> setText (serviceName);
	outputWidget	-> item (row, 1) -> setText (serviceId);
	outputWidget	-> item (row, 2) -> setText (dabType);
	outputWidget	-> item (row, 3) -> setText (bitRate);
	outputWidget	-> item (row, 4) -> setText (protLevel);
	outputWidget	-> item (row, 5) -> setText (codeRate);
}


