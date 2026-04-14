#
/*
 *    Copyright (C)  2016, 2026
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

#include        <QDomDocument>
#include	<QTextStream>
#include        <QFile>

#include	"database.h"

	serviceBase::serviceBase	(const QString &fileName) {
	load (fileName);
}

	serviceBase::~serviceBase	() {
}

void	serviceBase::add		(const serviceDescriptor &sd) {
	for (auto &ch :theData) {
	   if (ch. channelName == sd. channelName) {
	      ch. add (sd);
	      return;
	   }
	}
	channelDescriptor channel;
	channel. setName (sd. channelName);
	channel. add (sd);
	theData. push_back (channel);
}

void	serviceBase::update		(const serviceDescriptor &sd, bool f) {
	for (auto &ch: theData) {
	   if (ch. channelName == sd. channelName) {
	      ch. update (sd, f);
	      return;
	   }
	}
	// should not happen
}

std::vector<serviceDescriptor> 
	serviceBase::getData (int Mode) {
std::vector<serviceDescriptor> res;
	for (auto &ch: theData) {
	   std::vector<serviceDescriptor> res_c =  ch. getData (Mode);
	   for (auto &sd : res_c) 
	      res. push_back (sd);
	}
	return res;
}

void	serviceBase::load	(const QString &fileName) {
QDomDocument xmlBOM;

	clearTable ();
	QFile f (fileName);
	if (!f. open (QIODevice::ReadOnly)) 
	   return;

	xmlBOM. setContent (&f);
	f. close ();
	QDomElement root	= xmlBOM. documentElement ();
	QDomElement component	= root. firstChild (). toElement ();
	
	while (!component. isNull ()) {
	   if (component. tagName () == "channel") {
	      QString channelName =
	                component. attribute ("channelName", "???");
	      channelDescriptor channel;
	      channel. setName (channelName);
	      QDomElement subComp = component. firstChild (). toElement ();
	      while (!subComp. isNull ()) {
	         serviceDescriptor sd;
	         sd. channelName = channel. channelName;
	         sd. serviceName = component. attribute ("serviceName", "??");
	         QString tt = component. attribute ("SID", "0");
	         bool b;
	         sd. SID = tt. toInt (&b, 16);
	         tt  = component. attribute ("subChId", "0");
	         sd. subChId = tt. toInt (&b);
	         sd. isFavorite = component. attribute ("isFavorite", "0") == "1";
	         channel. add (sd);
	         subComp = subComp. nextSibling (). toElement ();
	      }
	      theData. push_back (channel);
	   }
	   component = component. nextSibling (). toElement ();
	}
}


void	serviceBase::store	(const QString &fileName) {
QDomDocument serviceDB;
QDomElement root = serviceDB. createElement ("serviceList");

        serviceDB. appendChild (root);

        for (auto &channel : theData) { 
	   QDomElement channelElement = serviceDB.
	                          createElement ("channel");
	   channelElement. setAttribute ("channelName", channel. channelName);
	   for (auto &sd : channel. channelData) {
	      QDomElement serv = serviceDB. createElement ("serviceDesc");
	      serv. setAttribute ("serviceName", sd. serviceName);
	      serv. setAttribute ("SID", QString::number (sd. SID, 16));
	      serv. setAttribute ("subChId", QString::number (sd. subChId));
	      serv. setAttribute ("isFavorite", sd. isFavorite ? "1" : "0");
	      channelElement. appendChild (serv);
	   }
	   root. appendChild (channelElement);
	}
	QFile file (fileName);
	if (!file. open (QIODevice::WriteOnly | QIODevice::Text))
	   return;

	QTextStream stream (&file);
	stream << serviceDB. toString ();
	file. close ();
}

void	serviceBase::clearTable	() {
	for (auto &ch : theData)
	   ch. channelData. resize (0);
	theData. resize (0);
}

