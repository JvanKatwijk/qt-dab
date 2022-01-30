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
#include	<QSettings>
#include	"content-table.h"
#include        "radio.h"
#include	"dab-constants.h"
#include	"findfilenames.h"

	contentTable::contentTable (RadioInterface *theRadio, 
	                                        QSettings *s) {
	this	-> theRadio	= theRadio;
	this	-> dabSettings	= s;
	myWidget        = new QScrollArea (NULL);
        myWidget        -> resize (200, 200);
        myWidget        -> setWidgetResizable(true);
        contentWidget	= new QTableWidget (0, 7);
	contentWidget	-> setColumnWidth (0, 150);
	contentWidget	-> setColumnWidth (4, 150);
        myWidget	-> setWidget (contentWidget);
        contentWidget	-> setHorizontalHeaderLabels (
	                          QStringList () << tr ("current ensemble"));

	connect (contentWidget, SIGNAL (cellClicked (int, int)),
                 this, SLOT (selectService (int, int)));
	connect (contentWidget, SIGNAL (cellDoubleClicked (int, int)),
                 this, SLOT (dump (int, int)));
	connect (this, SIGNAL (goService (const QString &)),
                theRadio, SLOT (handle_contentSelector (const QString &)));

	addRow ();	// for the ensemble name
	new_headline ();
	is_clear	= true;
}

	contentTable::~contentTable () {
	clearTable ();
        delete  contentWidget;
        delete  myWidget;
}

void	contentTable::clearTable	() {
int	rows	= contentWidget -> rowCount ();
	if (is_clear)
	   return;
	for (int i = rows; i > 0; i --) 
	   contentWidget -> removeRow (i - 1);
	addRow ();	// for the ensemble name
	new_headline ();
	is_clear	= true;
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
QTableWidgetItem* theItem = contentWidget  -> item (row, 0);

	if (row < 2)
	   return;
        (void)column;
        QString theService = channel + ":" + theItem -> text ();
	fprintf (stderr, "selecting %s\n", theService. toUtf8 (). data ());
	goService (theService);
}

void	contentTable::dump	(int row, int column) {
findfileNames filenameFinder (dabSettings);
FILE	*dumpFile 	= filenameFinder. findContentDump_fileName (channel);

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

int16_t	contentTable::addRow () {
int16_t row	= contentWidget -> rowCount ();

        contentWidget     -> insertRow (row);
        QTableWidgetItem *item0 = new QTableWidgetItem;
        item0           -> setTextAlignment (Qt::AlignLeft |Qt::AlignVCenter);
        contentWidget     -> setItem (row, 0, item0);

        QTableWidgetItem *item1 = new QTableWidgetItem;
        item1           -> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        contentWidget     -> setItem (row, 1, item1);

        QTableWidgetItem *item2 = new QTableWidgetItem;
        item2           -> setTextAlignment (Qt::AlignRight |Qt::AlignVCenter);
        contentWidget    -> setItem (row, 2, item2);

        QTableWidgetItem *item3 = new QTableWidgetItem;
        item3           -> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        contentWidget    -> setItem (row, 3, item3);

        QTableWidgetItem *item4 = new QTableWidgetItem;
        item4           -> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        contentWidget    -> setItem (row, 4, item4);

        QTableWidgetItem *item5 = new QTableWidgetItem;
        item5           -> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        contentWidget    -> setItem (row, 5, item5);

        QTableWidgetItem *item6 = new QTableWidgetItem;
        item6           -> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        contentWidget    -> setItem (row, 6, item6);
	return row;
}

void	contentTable::new_headline	() {
int16_t	row	= addRow ();
	contentWidget	-> item (row, 0) -> setText ("serviceName");
	contentWidget	-> item (row, 1) -> setText ("serviceId");
	contentWidget	-> item (row, 2) -> setText ("type ");
	contentWidget	-> item (row, 3) -> setText ("bitRate ");
	contentWidget	-> item (row, 4) -> setText ("programType");
	contentWidget	-> item (row, 5) -> setText ("Language");
	contentWidget	-> item (row, 6) -> setText ("fm frequency");
}

void	contentTable::ensemble		(const QString &ensembleName,
	                                 const QString &channel,
	                                 const QString &theTime) {
	this	-> ensembleName	= ensembleName;
	this	-> channel	= channel;
	is_clear	= false;
	contentWidget	-> item (0, 0) -> setText (ensembleName);
	contentWidget	-> item (0, 1) -> setText (channel);
	contentWidget	-> item (0, 2) -> setText (theTime);
}

void	contentTable::add_to_Ensemble	(audiodata *theDescriptor) {
QString serviceName;
QString serviceId;
QString dabType;
QString bitRate;
QString programType;
QString Language;

	if (theDescriptor-> type != AUDIO_SERVICE)
	   return;

	is_clear	= false;
	int16_t	row = addRow ();

	serviceName	= theDescriptor -> serviceName;
	contentWidget	-> item (row, 0) -> setText (serviceName);
	serviceId	= QString::number (theDescriptor -> SId, 16);
	contentWidget	-> item (row, 1) -> setText (serviceId);
	dabType		= (theDescriptor) -> ASCTy == 077 ?
	                                          "DAB+" : "DAB";
	contentWidget	-> item (row, 2) -> setText (dabType);
	bitRate		= QString::number (theDescriptor -> bitRate);
	contentWidget	-> item (row, 3) -> setText (bitRate);
	programType	= theMapper. get_programm_type_string (
	                   (theDescriptor) -> programType);
	contentWidget	-> item (row, 4) -> setText (programType);
	Language	= theMapper.get_programm_language_string (
	                   (theDescriptor) -> language);
	contentWidget	-> item (row, 5) -> setText (Language);
	if (theDescriptor -> fmFrequency != -1)
	   contentWidget -> item (row, 6) -> setText (QString::number (
	                    theDescriptor -> fmFrequency));
}

