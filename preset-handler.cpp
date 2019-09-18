#
/*
 *    Copyright (C) 2019
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

#include	"preset-handler.h"
#include	"radio.h"

	presetHandler::presetHandler	(RadioInterface *radio,
	                                 QString fileName) {
	this	-> radio	= radio;
	this    -> fileName     = fileName;
        presets. resize (0);
	myWidget		= new QScrollArea (NULL);
        myWidget        -> resize (240, 200);
        myWidget        -> setWidgetResizable(true);

        tableWidget		= new QTableWidget (0, 2);
        myWidget        -> setWidget (tableWidget);
        tableWidget     -> setHorizontalHeaderLabels (
                    QStringList () << tr ("channel") << tr ("service"));
        connect (tableWidget, SIGNAL (cellClicked (int, int)),
                 this, SLOT (tableSelect (int, int)));
        connect (tableWidget, SIGNAL (cellDoubleClicked (int, int)),
                 this, SLOT (removeRow (int, int)));
        connect (this, SIGNAL (select_presetService (QString, QString)),
                 radio, SLOT (select_presetService (QString, QString)));
        loadTable ();
	show ();
}

        presetHandler::~presetHandler   () {
int16_t i;
int16_t rows    = tableWidget -> rowCount ();

        for (i = rows; i > 0; i --)
           tableWidget -> removeRow (i);
        delete  tableWidget;
        delete  myWidget;
}

void    presetHandler::show        (void) {
	myWidget        -> show ();
}

void    presetHandler::hide        (void) {
	myWidget        -> hide ();
}

void	presetHandler::loadTable () {
QDomDocument xmlBOM;
QFile f (fileName);

	if (!f. open (QIODevice::ReadOnly)) 
	   return;

	xmlBOM. setContent (&f);
	f. close ();
	QDomElement root	= xmlBOM. documentElement ();
	QDomElement component	= root. firstChild (). toElement ();
	while (!component. isNull ()) {
	   if (component. tagName () == "PRESET_ELEMENT") {
	      presetData pd;
	      pd. serviceName = component. attribute ("SERVICE_NAME", "???");
	      pd. channel     = component. attribute ("CHANNEL", "5A");
	      if (!inPresets (&pd)) {
	         addRow (pd. channel, pd. serviceName);
	         presets. push_back (pd);
	      }
	   }
	   component = component. nextSibling (). toElement ();
	}
}

bool	presetHandler::inPresets	(presetData *pd) {
	for (uint i = 0; i < presets. size (); i ++)
	   if ((presets. at (i). serviceName == pd -> serviceName) &&	
	       (presets. at (i). channel     == pd -> channel))
	      return true;
	return false;
}

int	presetHandler::nrItems		() {
	return presets. size ();
}

void	presetHandler::item		(int nr, presetData *pd) {
	if ((uint)nr < presets. size ()) {
	   pd -> serviceName	= presets. at (nr). serviceName;
	   pd -> channel	= presets. at (nr). channel;
	}
}

bool	presetHandler::item		(QString s, presetData *pd) {
	fprintf (stderr, "Looking for %s\n", s. toLatin1 (). data ());
	for (uint i = 0; i < presets. size (); i ++) {
	   if (presets. at (i). serviceName == s) {
	      pd -> serviceName = s;
	      pd -> channel     = presets. at (i). channel;
	      fprintf (stderr, "found\n");
	      return true;
	   }
	}
	return false;
}

void	presetHandler::update		(presetData *pd) {
	if (inPresets (pd))
	   return;

	addRow (pd -> channel, pd -> serviceName);
	presets. push_back (*pd);
	writeFile ();
}

void	presetHandler::writeFile	() {
QDomDocument the_presets;
QDomElement root = the_presets. createElement ("preset_db");

	the_presets. appendChild (root);

	for (uint i = 0; i < presets. size (); i ++) {
	   QDomElement presetService = the_presets. createElement ("PRESET_ELEMENT");
	   presetService. setAttribute ("SERVICE_NAME", 
	                                 presets. at (i). serviceName);
	   presetService. setAttribute ("CHANNEL",
	                                 presets. at (i). channel);
	   root. appendChild (presetService);
	}
	QFile file (this -> fileName);
	if (!file. open (QIODevice::WriteOnly | QIODevice::Text))
	   return;

	QTextStream stream (&file);
	stream << the_presets. toString ();
	file. close ();
}

void	presetHandler::removeRow (int row, int column) {
QTableWidgetItem* theItem_1 = tableWidget -> item (row, 0);
QTableWidgetItem* theItem_2 = tableWidget -> item (row, 1);

	(void)column;
	QString service = theItem_2 -> text ();

	for (std::vector<presetData>::iterator iter = presets. begin ();
	     iter != presets. end (); iter ++) {
	   if (iter -> serviceName == service) {
	      presets. erase (iter);
	      tableWidget	-> removeRow (row);
	      break;
	   }
	}
	writeFile ();
}

void	presetHandler::addRow (const QString &channel, const QString &name) {
int16_t	row	= tableWidget -> rowCount ();

	tableWidget	-> insertRow (row);
	QTableWidgetItem *item0	= new QTableWidgetItem;
	item0		-> setTextAlignment (Qt::AlignRight |Qt::AlignVCenter);
	tableWidget	-> setItem (row, 0, item0);

	QTableWidgetItem *item1 = new QTableWidgetItem;
	item1		-> setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 1, item1);
	tableWidget	-> resizeColumnToContents (1);

	tableWidget	-> setCurrentItem (item0);
	tableWidget	-> item (row, 0) -> setText (channel);
	tableWidget	-> item (row, 1) -> setText (name);
}
//
//	Locally we dispatch the "click" and "translate"
//	it into a frequency and a call to the main gui to change
//	the frequency

void	presetHandler::tableSelect (int row, int column) {
QTableWidgetItem* theItem_1 = tableWidget -> item (row, 0);
QTableWidgetItem* theItem_2 = tableWidget -> item (row, 1);

	(void)column;
	QString channel	= theItem_1 -> text ();
	QString service = theItem_2 -> text ();
	emit select_presetService (channel, service);
}


