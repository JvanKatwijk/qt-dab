#
/*
 *    Copyright (C) 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
//
//
//	The qt-dab software will save some data of all services,
//	ever seen.
//
#include	"scanlist-handler.h"
#include	"radio.h"
#include	<QDomDocument>
#include	<QFile>

	scanListHandler::scanListHandler	(RadioInterface *radio,
	                                         QString fileName):
	                                             QListView (nullptr) {
	this	-> radio	= radio;
	this	-> fileName	= fileName;
	QDomDocument xmlBOM;
	this	-> fileName = fileName;
//
//	start with an empty list, waiting ...
	scanList. clear ();
	displayList. setStringList (scanList);
	this	-> setModel (&displayList);
	connect (this, SIGNAL (clicked (QModelIndex)),
	         this, SLOT (selectElement (QModelIndex)));

	QFile f (fileName);
	if (!f. open (QIODevice::ReadOnly)) 
	   return;

	xmlBOM. setContent (&f);
	f. close ();
	QDomElement root	= xmlBOM. documentElement ();
	QDomElement component	= root. firstChild (). toElement ();
	while (!component. isNull ()) {
	   if (component. tagName () == "HISTORY_ELEMENT") {
	      presetData pd;
	      pd. serviceName = component. attribute ("SERVICE_NAME", "???");
	      pd. channel     = component. attribute ("CHANNEL", "5A");
	      scanList. append (pd. channel + ":" + pd. serviceName);
	   }
	   component = component. nextSibling (). toElement ();
	}
	displayList. setStringList (scanList);
	this	-> setModel (&displayList);
}


	scanListHandler::~scanListHandler   () {
QDomDocument the_scanList;
QDomElement root = the_scanList. createElement ("history_db");

	the_scanList. appendChild (root);

	for (int i = 1; i < scanList. size (); i ++) {
	   QStringList list = scanList. at (i).
	                        split (":", QString::SkipEmptyParts);
           if (list. length () != 2)
	      continue;
           QString channel = list. at (0);
           QString serviceName = list. at (1);
	   QDomElement scanListService = the_scanList.
	                            createElement ("HISTORY_ELEMENT");
	   scanListService. setAttribute ("SERVICE_NAME", serviceName);
	   scanListService. setAttribute ("CHANNEL", channel);
	   root. appendChild (scanListService);
	}

	QFile file (this -> fileName);
	if (!file. open (QIODevice::WriteOnly | QIODevice::Text))
	   return;

	QTextStream stream (&file);
	stream << the_scanList. toString ();
	file. close ();
}

void	scanListHandler::addElement (const QString &channel,
	                            const QString &serviceName) {
const QString scanListElement = channel + ":" + serviceName;

	for (int i = 0; i < scanList. size (); i ++)
	   if (scanList. at (i) == scanListElement)
	      return;
	scanList. append (scanListElement);
	displayList. setStringList (scanList);
	this	-> setModel (&displayList);
}

void	scanListHandler::clear_scanList () {
	scanList. clear ();
	displayList. setStringList (scanList);
	this	-> setModel (&displayList);
}
//
//	selecting a scanList service is not different from
//	selecting a preset
//
void	scanListHandler::selectElement (QModelIndex ind) {
QString currentProgram = displayList. data (ind, Qt::DisplayRole). toString ();
	emit handle_scanListSelect (currentProgram);
}

