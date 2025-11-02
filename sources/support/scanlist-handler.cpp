#
/*
 *    Copyright (C) 2016 .. 2023
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
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
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
//
//	start with an empty list, waiting ...
	scanList. clear ();
	displayList. setStringList (scanList);
	this	-> setModel (&displayList);
	connect (this, &QListView::clicked,
	         this, &scanListHandler::selectElement);

	QFile f (fileName);
	if (!f. open (QIODevice::ReadOnly)) 
	   return;

	xmlBOM. setContent (&f);
	f. close ();
	QDomElement root	= xmlBOM. documentElement ();
	QDomElement component	= root. firstChild (). toElement ();
	while (!component. isNull ()) {
	   if (component. tagName () == "HISTORY_ELEMENT") {
	      QString serviceName =
	                component. attribute ("SERVICE_NAME", "???");
	      QString  channel     = component. attribute ("CHANNEL", "5A");
	      scanList. append (channel + ":" + serviceName);
	   }
	   component = component. nextSibling (). toElement ();
	}
	displayList. setStringList (scanList);
	this	-> setModel (&displayList);
}

	scanListHandler::~scanListHandler   () {}

void	scanListHandler::dump   () {
QDomDocument theScanList;
QDomElement root = theScanList. createElement ("history_db");

	theScanList. appendChild (root);

	for (int i = 1; i < scanList. size (); i ++) {
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 2)
	   QStringList list = scanList. at (i).
	                           split (":", Qt::SkipEmptyParts);
#else
	   QStringList list = scanList. at (i).
	                           split (":", QString::SkipEmptyParts);
#endif
           if (list. length () != 2)
	      continue;
           QString channel = list. at (0);
           QString serviceName = list. at (1);
	   QDomElement scanListService = theScanList.
	                            createElement ("HISTORY_ELEMENT");
	   scanListService. setAttribute ("SERVICE_NAME", serviceName);
	   scanListService. setAttribute ("CHANNEL", channel);
	   root. appendChild (scanListService);
	}

	QFile file (this -> fileName);
	if (!file. open (QIODevice::WriteOnly | QIODevice::Text))
	   return;

	QTextStream stream (&file);
	stream << theScanList. toString ();
	file. close ();
}

void	scanListHandler::addElement (const QString &channel,
	                            const QString &serviceName) {
const QString scanListElement = channel + ":" + serviceName;

QStringList res;

	for (int i = 0; i < scanList. size (); i ++)
	   if (scanList. at (i) == scanListElement)
	      return;
	bool inserted = false;
	for (int i = 0; i < scanList. size (); i ++) {
	   if (inserted) {
	      res << scanList [i];
	      continue;
	   }
	   QStringList ss = scanList [i]. split (":", Qt::SkipEmptyParts);
	   QString ch = ss [0];
	   bool ok;
	   if (ch. toUInt (&ok, 16) < channel. toUInt (&ok, 16))
	      res << scanList [i];
	   else {
	      res << scanListElement;
	      res << scanList [i];
	      inserted = true;
	   }
	}
	if (!inserted)
	   res. append (scanListElement);
	scanList = res;
	displayList. setStringList (scanList);
	this	-> setModel (&displayList);
}

void	scanListHandler::clearScanList () {
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
//	fprintf (stderr, "scan %s\n", currentProgram. toLatin1 (). data ());
	emit handleScanListSelect (currentProgram);
}

