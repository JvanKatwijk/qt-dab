#
/*
 *    Copyright (C) 2014 .. 2023
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
#include	<stdio.h>
#include	<QDir>
#include	<QString>
#include	<QStringList>
#include	<math.h>
#include	"dab-constants.h"
#include	"tii-mapper.h"
#include	"ITU_Region_1.h"
#include	"tii-reader.h"
#include	<QSettings>
#include	"tii-reader.h"

		tiiMapper::tiiMapper	(QSettings *dabSettings) {
tiiReader theReader;
	tiifileName = QDir::homePath () + "/.txdata.tii";
	tiifileName = dabSettings -> value ("tiiFileName", tiifileName). toString ();                                         
	if (tiifileName != "")
	   theCache	= theReader. readFile (tiifileName);
}
		tiiMapper::~tiiMapper	() {
}

void	tiiMapper::reload	() {
tiiReader theReader;
	if (tiifileName == "")
	   return;
	theCache. resize (0);
	theCache	= theReader. readFile (tiifileName);
}

bool	tiiMapper::has_tiiFile	() {
	return theCache. size () > 10;
}

cacheElement *tiiMapper::
		get_transmitter (const QString &channel,
	                         uint16_t Eid, 
	                         uint8_t mainId, uint8_t subId) {
	for (int i = 1; i < (int)(theCache. size ()); i ++) {
	   if (((channel == "any") ||
	        (channel ==  theCache [i]. channel)) &&
	        (theCache [i]. Eid == Eid) &&
	        (theCache [i]. mainId == mainId) &&
	        (theCache [i]. subId == subId)) {
	      return &theCache [i];
	   }
	}
	return &theCache [0];
}

void	tiiMapper::set_black (uint16_t Eid, uint8_t mainId, uint8_t subId) {
black element;
	element. Eid = Eid;
	element. mainId	= mainId;
	element. subId	= subId;
	blackList. push_back (element);
}

bool	tiiMapper::is_black (uint16_t Eid, uint8_t mainId, uint8_t subId) {

	for (int i = 0; i < (int)(blackList. size ()); i ++)
	   if ((blackList [i]. Eid == Eid) &&
	       (blackList [i]. mainId == mainId) &&
	       (blackList [i]. subId == subId))
	      return true;
	return false;
}

