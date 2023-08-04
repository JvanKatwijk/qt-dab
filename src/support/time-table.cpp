#
/*
 *    Copyright (C) 2020
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
//
//
#include	"time-table.h"
#include	"radio.h"

	timeTableHandler::timeTableHandler (RadioInterface *radio):
	                                             QListView (nullptr) {
	this	-> radio	= radio;
//
//	start with an empty list, waiting ...
	timeTableList. clear ();
	displayList. setStringList (timeTableList);
	this	-> setModel (&displayList);
}

	timeTableHandler::~timeTableHandler   () {
}

void	timeTableHandler::addElement (int theTime,
	                              int	epgWidth,
	                              const QString &theText,
	                              const QString &theDescr) {
//int	hours	= theTime / 60;
//int	minutes	= theTime % 60;
char t [6];

	if (theTime >= 24 * 60)
	   theTime -= 24 * 60;
	sprintf (t, "%.2d:%.2d", theTime / 60, theTime % 60);
	timeTableList. append (QString (t) + " -- " + theText);

	QString listElement;
	if (theDescr != "")
	   listElement = " \n\t-- " + theDescr;

	bool tooLong = false;
	if  (listElement. size () > epgWidth) {
	   int breaker = listElement. indexOf (' ', epgWidth - 10);
	   if (breaker > 0) {
	      tooLong = true;
	      QString L = listElement. left (breaker);
	      listElement = listElement. mid (breaker, -1);
	      timeTableList. append (L);
	      while (listElement. size () > epgWidth) {
	         breaker = listElement. indexOf (' ', epgWidth - 10);
	         if (breaker < 0)
	         break;
	         QString L2 = listElement. left (breaker);
	         listElement = listElement. mid (breaker, -1);
	         timeTableList. append ("\t   " + L2);
	      }
	   }
	}
	if (tooLong)
	   timeTableList. append ("\t    " + listElement + "\n");
	else
	   timeTableList. append (listElement + "\n");
	
	displayList. setStringList (timeTableList);
	this	-> setModel (&displayList);
}

void	timeTableHandler::clear () {
        timeTableList. clear ();
        displayList. setStringList (timeTableList);
        this    -> setModel (&displayList);
}

