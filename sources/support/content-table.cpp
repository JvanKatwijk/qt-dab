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
#include        <QDataStream>
#include	<QSettings>
#include	"content-table.h"
#include        "radio.h"
#include	"dab-constants.h"
#include	"findfilenames.h"
#include	"position-handler.h"

	contentTable::contentTable (RadioInterface *theRadio, 
	                                        QSettings *s,
	                                        const QString &channel,
	                                        int cols) {
	this	-> theRadio	= theRadio;
	this	-> dabSettings	= s;
	this	-> channel	= channel;
	this	-> columns	= cols;
	myWidget        = new QScrollArea (nullptr);
        myWidget        -> setWidgetResizable (true);
	setPositionAndSize (dabSettings, myWidget,  CONTENT_TABLE);
	
        contentWidget	= new QTableWidget (0, cols);
	contentWidget	-> setColumnWidth (0, 150);
	contentWidget	-> setColumnWidth (4, 150);
        myWidget	-> setWidget (contentWidget);
        contentWidget	-> setHorizontalHeaderLabels (
	                          QStringList () << tr ("current ensemble"));

	connect (contentWidget, &QTableWidget::cellClicked,
	         this, &contentTable::selectService);
	connect (contentWidget, &QTableWidget::cellDoubleClicked,
                 this, &contentTable::dump_local);

	addRow ();	// for the ensemble name
}

	contentTable::~contentTable () {
	storeWidgetPosition (dabSettings, myWidget, CONTENT_TABLE);
	clearTable ();
        delete  contentWidget;
        delete  myWidget;
}

void	contentTable::clearTable	() {
int	rows	= contentWidget -> rowCount ();
	for (int i = rows; i > 0; i --) 
	   contentWidget -> removeRow (i - 1);
	addRow ();	// for the ensemble name
}

void	contentTable::show	() {
	myWidget	-> show ();
}

void	contentTable::hide	() {
	myWidget	-> hide ();
}

bool	contentTable::isVisible	() {
	return !myWidget -> isHidden ();
}

void	contentTable::selectService (int row, int column) {
QTableWidgetItem* theItem = contentWidget  -> item (row, 1);

	if (row < 2)
	   return;
        (void)column;
        QString theService = theItem -> text ();
//	fprintf (stderr, "selecting %s\n", theService. toUtf8 (). data ());
	goService (theService);
}

void	contentTable::dump_local (int row, int column) {
findfileNames filenameFinder (dabSettings);
FILE	*dumpFile 	= filenameFinder. findContentDump_fileName (channel);

	(void)row;
	(void)column;
	if (dumpFile == nullptr)
	   return;

	for (int i = 0; i < contentWidget -> rowCount (); i ++) {
	   for (int j = 0; j < contentWidget -> columnCount (); j ++) {
	      QString t = contentWidget -> item (i, j) -> text ();
	      fprintf (dumpFile, "%s;", t. toUtf8 (). data ());
	   }
	   fprintf (dumpFile, "\n");
	}
	fclose (dumpFile);
}

void	contentTable::dump	(FILE * dumpFilePointer) {
	if (dumpFilePointer == nullptr)
	   return;
	for (int i = 0; i < contentWidget -> rowCount (); i ++) {
	   for (int j = 0; j < contentWidget -> columnCount (); j ++) {
	      QString t = contentWidget -> item (i, j) -> text ();
	      fprintf (dumpFilePointer, "%s;", t. toUtf8 (). data ());
	   }
	   fprintf (dumpFilePointer, "\n");
	}
}

QString	contentTable::upload	() {
QString result;
	for (int i = 0; i < contentWidget -> rowCount (); i ++) {
	   for (int j = 0; j < contentWidget -> columnCount (); j ++) {
	      QString t = contentWidget -> item (i, j) -> text ();
	      result = result + t + ";";
	   }
	   result = result + "\n";
	}

	return result;
}

int16_t	contentTable::addRow () {
int16_t row	= contentWidget -> rowCount ();

        contentWidget     -> insertRow (row);

	for (int i = 0; i < columns; i ++) {
           QTableWidgetItem *item0 = new QTableWidgetItem;
           item0           -> setTextAlignment (Qt::AlignLeft |
	                                                Qt::AlignVCenter);
           contentWidget     -> setItem (row, i, item0);
	}
	return row;
}

void	contentTable::addLine (const QString &s) {
int	row	= addRow ();
QStringList h	= s. split (";");

	for (int i = 0; i < h. size (); i ++)
	   if (i < columns) 		// just for safety
	      contentWidget -> item (row, i) -> setText (h. at (i));
}

