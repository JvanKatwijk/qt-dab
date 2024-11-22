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
//	The band handler now manages the scanTable
//
#include	<QSettings>
#include	"scantable-handler.h"
#include	"dab-constants.h"
#include	<QHeaderView>
#include	<QDomDocument>
#include	<stdio.h>
#include	"settings-handler.h"

	scantableHandler::scantableHandler (QSettings *s):
	                                theTable (nullptr) {
	dabSettings	= s;
	theTable. setColumnCount (2);
	QStringList header;
	header	<< tr ("channel") << tr ("scan");
	theTable. setHorizontalHeaderLabels (header);
	theTable. verticalHeader () -> hide ();
	theTable. setShowGrid	(true);
}

	scantableHandler::~scantableHandler	() {
	theTable. hide ();
}

void	scantableHandler::setup_scanTable	(dabFrequencies *theBand) {
	this -> selectedBand = theBand;
	for (int i = 0; selectedBand [i]. fKHz != 0; i ++)  {
	   theTable. insertRow (i);
	   theTable. setItem (i, 0,
                           new QTableWidgetItem (selectedBand [i]. key));
           theTable. setItem (i, 1,
                           new QTableWidgetItem (QString ("+")));
	   selectedBand [i]. scan = true;
        }
	connect (&theTable, &QTableWidget::cellDoubleClicked,
	         this, &scantableHandler::cellSelected);
}

void	scantableHandler::load_scanTable	(const QString &source) {
	if (source == "")  {	// just load the current table
           for (int i = 0; selectedBand [i]. fKHz != 0; i ++) {
	      theTable. item (i, 0) -> setText (selectedBand [i]. key);
	      theTable. item (i, 1) -> setText ("+");
              selectedBand [i]. scan = true;
              bool scanValue = value_i (dabSettings,
	                                "scanTable",
	                                selectedBand [i]. key, 1) != 0;
              if (!scanValue) {
                 selectedBand [i]. scan = false;
                 theTable. item (i, 1) -> setText ("-");
              }
	   }
	   theTable. show ();
	}
	else {
           for (int i = 0; selectedBand [i]. fKHz != 0; i ++) {
	      theTable. item (i, 0) -> setText (selectedBand [i]. key);
	      theTable. item (i, 1) -> setText ("+");
	      selectedBand [i]. scan = true;
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
                    QString scanItem = component. attribute ("VALUE", "+");
                    if ((channel != "???") && (scanItem == "-"))
                       updateEntry (channel, false);
                 }
                 component = component. nextSibling (). toElement ();
              }
	      theTable. show ();
	   }
	}
}

void	scantableHandler::updateEntry (const QString &channel, bool val) {
        for (int i = 0; selectedBand [i]. key != nullptr; i ++)  {
           if (selectedBand [i]. key == channel) {
              selectedBand [i]. scan = val;
              theTable. item (i, 1) ->  setText (val ? "+" : "-");
              return;
           }
        }
}

void	scantableHandler::save_scanTable	(const QString &target) {
	if (target == "")  {	// dump in the ini file
           for (int i = 0; selectedBand [i]. fKHz != 0; i ++) {
              if (selectedBand [i]. scan)
                 store (dabSettings, "scanTable", selectedBand [i]. key, 1);
              else
                 store (dabSettings, "scanTable", selectedBand [i]. key, 0);
           }
	}
	else {
	   QDomDocument scanList;
	   QDomElement root;

	   root	= scanList. createElement ("scanList");
	   scanList. appendChild (root);

	   for (int i = 0; selectedBand [i]. fKHz != 0; i ++) {
	      QDomElement scanElement = scanList.
	                                createElement ("BAND_ELEMENT");
	      scanElement. setAttribute ("CHANNEL", selectedBand [i]. key);
	      scanElement. setAttribute ("VALUE",
	                                  selectedBand [i]. scan ? "+" : "-");
	      root. appendChild (scanElement);
	   }

	   QFile file (target);
           if (!file. open (QIODevice::WriteOnly | QIODevice::Text))
              return;

           QTextStream stream (&file);
           stream << scanList. toString ();
           file. close ();
	}
}

void	scantableHandler::clear_scanTable	() {
	for (int i = 0; selectedBand [i]. fKHz != 0; i ++) {
	   selectedBand [i]. scan = true;
	   theTable. setItem (i, 0,
                           new QTableWidgetItem (selectedBand [i]. key));
           theTable. setItem (i, 1,
                           new QTableWidgetItem (QString ("+")));
        }
}

void	scantableHandler::cellSelected (int row, int column) {
QString s1 = theTable. item (row, 0) -> text ();
QString s2 = theTable. item (row, 1) -> text ();
	(void)column;
        if (s2 == "-") 
           theTable. item (row, 1) -> setText ("+");
	else
           theTable. item (row, 1) -> setText ("-");
	
	selectedBand [row]. scan = s2 != "-";
	for (int i = 0; selectedBand [i]. fKHz != 0; i ++) {
	   if (selectedBand [i]. key == s1) {
	      selectedBand [i]. scan = s2 != "+";
	      fprintf (stderr, "Band %s (%s) wordt %d\n", 
	                          selectedBand [i]. key. toLatin1 (). data (),
	                          s1. toLatin1 (). data (),
	                          s2 != "+");
	      break;
	   }
	}
}

void	scantableHandler::show_scanTable	() {
	theTable. show ();
}

void	scantableHandler::hide_scanTable	() {
	theTable. hide ();
}

bool	scantableHandler::isHidden		() {
	return theTable. isHidden ();
}

