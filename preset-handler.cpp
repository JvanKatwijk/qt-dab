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
#include	<QComboBox>

	presetHandler::presetHandler	(RadioInterface *radio) {
	this	-> radio	= radio;
	this	-> fileName	= "";
        presets. resize (0);
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
	      if (!inPresets (&pd)) {
	         cb -> addItem (pd. channel + ":" + pd. serviceName);
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

void	presetHandler::update		(presetData *pd, QComboBox *cb) {
	if (inPresets (pd))
	   return;

	QString entry = pd -> channel + QString (":") + pd -> serviceName;
	cb -> addItem (entry);
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

