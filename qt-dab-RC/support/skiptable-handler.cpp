#
/*
 *    Copyright (C) 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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
//	The band handler now manages the skipTable
//
#include	<QSettings>
#include	"skiptable-handler.h"
#include	"dab-constants.h"
#include	<QHeaderView>
#include	<QDomDocument>
#include	<stdio.h>

	skiptableHandler::skiptableHandler (QSettings *s):
	                                theTable (nullptr) {
	dabSettings	= s;
	theTable. setColumnCount (2);
	QStringList header;
	header	<< tr ("channel") << tr ("scan");
	theTable. setHorizontalHeaderLabels (header);
	theTable. verticalHeader () -> hide ();
	theTable. setShowGrid	(true);
	skipFile	= "";
}

	skiptableHandler::~skiptableHandler	() {
	theTable. hide ();
}

void	skiptableHandler::setup_skipTable	(dabFrequencies *theBand) {
	this -> selectedBand = theBand;
	for (int i = 0; selectedBand [i]. fKHz != 0; i ++)  {
	   theTable. insertRow (i);
	   theTable. setItem (i, 0,
                           new QTableWidgetItem (selectedBand [i]. key));
           theTable. setItem (i, 1,
                           new QTableWidgetItem (QString ("+")));
        }
	connect (&theTable, &QTableWidget::cellDoubleClicked,
	         this, &skiptableHandler::cellSelected);
}

void	skiptableHandler::load_skipTable	(const QString &source) {
	if (source == "")  {	// kust load the current table
	   dabSettings     ->  beginGroup ("skipTable");
           for (int i = 0; selectedBand [i]. fKHz != 0; i ++) {
	      theTable. item (i, 0) -> setText (selectedBand [i]. key);
	      theTable. item (i, 1) -> setText ("+");
              bool skipValue =
                 dabSettings -> value (selectedBand [i]. key, 0). toInt () == 1;
              if (skipValue) {
                 selectedBand [i]. skip = true;
                 theTable. item (i, 1) -> setText ("-");
	fprintf (stderr, "found skip value for channel %s\n", 
	                             selectedBand [i]. key. toLatin1 (). data ());
              }
           }
           dabSettings     -> endGroup ();
	}
	else {
           for (int i = 0; selectedBand [i]. fKHz != 0; i ++) {
	      theTable. item (i, 0) -> setText (selectedBand [i]. key);
	      theTable. item (i, 1) -> setText ("+");
	   }
	   QDomDocument xml_bestand;
           QFile f (source);
           if (f. open (QIODevice::ReadOnly)) {
              xml_bestand. setContent (&f);
              QDomElement root     = xml_bestand. documentElement ();
              QDomElement component     = root. firstChild (). toElement ();
              while (!component. isNull ()) {
                 if (component. tagName () == "BAND_ELEMENT") {
                    QString channel = component. attribute ("CHANNEL", "???");
                    QString skipItem = component. attribute ("VALUE", "+");
                    if ((channel != "???") && (skipItem == "-"))
                       updateEntry (channel);
                 }
                 component = component. nextSibling (). toElement ();
              }
	   }
	}
}

void	skiptableHandler::updateEntry (const QString &channel) {
        for (int i = 0; selectedBand [i]. key != nullptr; i ++)  {
           if (selectedBand [i]. key == channel) {
              selectedBand [i]. skip = true;
              theTable. item (i, 1) -> setText ("-");
              return;
           }
        }
}

void	skiptableHandler::save_skipTable	(const QString &target) {
	if (target == "")  {	// dump in the ini file
	   dabSettings  -> beginGroup ("skipTable");
           for (int i = 0; selectedBand [i]. fKHz != 0; i ++) {
              if (selectedBand [i]. skip)
                 dabSettings    -> setValue (selectedBand [i]. key, 1);
              else
                 dabSettings    -> remove (selectedBand [i]. key);
           }
           dabSettings  -> endGroup ();
	}
	else {
	   QDomDocument skipList;
	   QDomElement root;

	   root	= skipList. createElement ("skipList");
	   skipList. appendChild (root);

	   for (int i = 0; selectedBand [i]. fKHz != 0; i ++) {
	      if (!selectedBand [i]. skip)
	         continue;
	      QDomElement skipElement = skipList.
	                                createElement ("BAND_ELEMENT");
	      skipElement. setAttribute ("CHANNEL", selectedBand [i]. key);
	      skipElement. setAttribute ("VALUE", "-");
	      root. appendChild (skipElement);
	   }

	   QFile file (target);
           if (!file. open (QIODevice::WriteOnly | QIODevice::Text))
              return;

           QTextStream stream (&file);
           stream << skipList. toString ();
           file. close ();
	}
}

void	skiptableHandler::cellSelected (int row, int column) {
QString s1 = theTable. item (row, 0) -> text ();
QString s2 = theTable. item (row, 1) -> text ();
	(void)column;
        if (s2 == "-") 
           theTable. item (row, 1) -> setText ("+");
	else
           theTable. item (row, 1) -> setText ("-");
	selectedBand [row]. skip = s2 != "-";
//	fprintf (stderr, "we zetten voor %s de zaak op %d\n",
//	              selectedBand [row]. key. toUtf8 (). data (),
//	              selectedBand [row]. skip);
}

void	skiptableHandler::show_skipTable	() {
	theTable. show ();
}

void	skiptableHandler::hide_skipTable	() {
	theTable. hide ();
}

bool	skiptableHandler::isHidden		() {
	return theTable. isHidden ();
}


	

