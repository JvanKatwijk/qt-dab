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

#include	"preset-handler.h"
#include	"radio.h"
//#include	<QComboBox>
#include	<QFile>

	presetHandler::presetHandler	(RadioInterface *radio,
	                                         const QString &fileName):
	                                              QListView (nullptr) {
	this	-> radio_p	= radio;
	this	-> fileName	= fileName;
	QDomDocument xmlBOM;

//
//	start with an empty lis, waiting ....
	thePresets. resize (0);
	presetList. clear ();
	displayList. setStringList (presetList);
	this	-> setModel (&displayList);
	connect (this, &QListView::clicked,
	         this, &presetHandler::selectElement);

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
	      addElement (pd. channel, pd. serviceName);
	   }
	   component = component. nextSibling (). toElement ();
	}
	displayList. setStringList (presetList);
	this	-> setModel (&displayList);
	setWindowTitle ("Favorites");
}

        presetHandler::~presetHandler   () {
QDomDocument the_presets;
QDomElement root = the_presets. createElement ("preset_db");

	the_presets. appendChild (root);

	for (int i = 0; i < (int)(thePresets. size ()); i ++) {
	   QString channel	= thePresets. at (i). channel;
	   QString serviceName	= thePresets. at (i). serviceName;
	   QDomElement presetService = the_presets.
	                            createElement ("PRESET_ELEMENT");
	   presetService. setAttribute ("SERVICE_NAME", serviceName);
	   presetService. setAttribute ("CHANNEL", channel);
	   root. appendChild (presetService);
	}

	QFile file (this -> fileName);
	if (!file. open (QIODevice::WriteOnly | QIODevice::Text))
	   return;

	QTextStream stream (&file);
	stream << the_presets. toString ();
	file. close ();
}

void	presetHandler::addElement	(const QString &channel,
	                         const QString &serviceName) {
	for (auto &pd: thePresets) 
	   if ((pd. channel == channel) && (pd. serviceName == serviceName))
	      return;
	presetData pd;
	pd. channel = channel;
	pd. serviceName	= serviceName;
	thePresets. push_back (pd);
	
	presetList. append (serviceName);
	displayList. setStringList (presetList);
	this	-> setModel (&displayList);
}

void	presetHandler::removeElement	(const QString &service) {
	for (int i = 0; i < presetList. size (); i ++) {
	   if (presetList. at (i) == service) {
	      thePresets. erase (thePresets. begin () + i);
	      presetList. removeAt (i);
	      displayList. setStringList (presetList);
	      this	-> setModel (&displayList);
	      return;
	   }
	}
	
}
	      
void	presetHandler::addElement (const QString &service) {
	QStringList list = service.
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 2)
	                        split (":", Qt::SkipEmptyParts);
#else
	                        split (":", QString::SkipEmptyParts);
#endif
	if (list. length () != 2)
	   return;
	addElement (list. at (0), list. at (1));
}
	
void	presetHandler::clear_presetList	() {
	thePresets. resize (0);
	presetList. clear ();
	displayList. setStringList (presetList);
	this	-> setModel (&displayList);
}
      
//      selecting a scanList service is not different from
//      selecting a preset
//      
void    presetHandler::selectElement (QModelIndex ind) {
QString currentProgram = displayList. data (ind, Qt::DisplayRole). toString ();
	for (uint32_t i = 0; i < thePresets. size (); i ++)
	   if (thePresets. at (i). serviceName == currentProgram) 
              emit handle_presetSelect (thePresets. at (i). channel,
	                                currentProgram);
}       

int	presetHandler::nrElements	() {
	return thePresets. size ();
}

QString	presetHandler::candidate	(int i) {
	if ((i < 0) || (i >= nrElements ()))
	   return "";
	QString element = thePresets. at (i). channel + ":" +
	                  thePresets. at (i). serviceName;
	return element;
}

