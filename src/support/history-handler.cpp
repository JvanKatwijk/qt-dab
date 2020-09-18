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
//
//
//	The qt-dab software will save some data of all services,
//	ever seen.
//	The list can be deleted by a simple mouseclick (right button)
//
#include	"history-handler.h"
#include	"radio.h"
#include	<QDomDocument>
#include	<QFile>

	historyHandler::historyHandler	(RadioInterface *radio,
	                                 QString fileName):QListView (NULL) {
	this	-> radio	= radio;
	this	-> fileName	= fileName;
	QDomDocument xmlBOM;
	this	-> fileName = fileName;
//
//	start with an empty list, waiting ...
	historyList. clear ();
	displayList. setStringList (historyList);
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
	      historyList. append (pd. channel + ":" + pd. serviceName);
	   }
	   component = component. nextSibling (). toElement ();
	}
	displayList. setStringList (historyList);
	this	-> setModel (&displayList);
}


	historyHandler::~historyHandler   () {
QDomDocument the_history;
QDomElement root = the_history. createElement ("history_db");

	the_history. appendChild (root);

	for (int i = 1; i < historyList. size (); i ++) {
	   QStringList list = historyList. at (i).
	                        split (":", QString::SkipEmptyParts);
           if (list. length () != 2)
	      continue;
           QString channel = list. at (0);
           QString serviceName = list. at (1);
	   QDomElement historyService = the_history.
	                            createElement ("HISTORY_ELEMENT");
	   historyService. setAttribute ("SERVICE_NAME", serviceName);
	   historyService. setAttribute ("CHANNEL", channel);
	   root. appendChild (historyService);
	}

	QFile file (this -> fileName);
	if (!file. open (QIODevice::WriteOnly | QIODevice::Text))
	   return;

	QTextStream stream (&file);
	stream << the_history. toString ();
	file. close ();
}

void	historyHandler::addElement (const QString &channel,
	                            const QString &serviceName) {
const QString listElement = channel + ":" + serviceName;

	for (int i = 0; i < historyList. size (); i ++)
	   if (historyList. at (i) == listElement)
	      return;
//	fprintf (stderr, "adding %s %s\n", channel. toLatin1 (). data (),
//	                                   serviceName. toLatin1 (). data ());
	historyList. append (listElement);
	displayList. setStringList (historyList);
	this	-> setModel (&displayList);
}

void	historyHandler::clearHistory () {
	historyList. clear ();
	displayList. setStringList (historyList);
	this	-> setModel (&displayList);
}
//
//	selecting a historical service is not different from
//	selecting a preset
//
void	historyHandler::selectElement (QModelIndex ind) {
QString currentProgram = displayList. data (ind, Qt::DisplayRole). toString ();
	emit handle_historySelect (currentProgram);
}

