#
/*
 *    Copyright (C) 2016 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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
#include	<QComboBox>

	presetHandler::presetHandler	(RadioInterface *radio) {
	this	-> radio	= radio;
	this	-> fileName	= "";
}

        presetHandler::~presetHandler   () {
}

void	presetHandler::loadPresets (QString fileName, QComboBox *cb) {
QDomDocument xmlBOM;
QFile f (fileName);

	this	-> fileName = fileName;
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
	      cb -> addItem (pd. channel + ":" + pd. serviceName);
	   }
	   component = component. nextSibling (). toElement ();
	}
}

void	presetHandler::savePresets (QComboBox *cb) {
QDomDocument the_presets;
QDomElement root = the_presets. createElement ("preset_db");

	the_presets. appendChild (root);

	for (int i = 1; i < cb -> count (); i ++) {
	   QStringList list = cb -> itemText (i).
	                        split (":", QString::SkipEmptyParts);
           if (list. length () != 2)
	      continue;
           QString channel = list. at (0);
           QString serviceName = list. at (1);
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

