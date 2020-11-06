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
	                              const QString &theText) {
int	hours	= theTime / 60;
int	minutes	= theTime % 60;
char t [6];
	sprintf (t, "%.2d:%.2d", theTime / 60, theTime % 60);
	
	const QString listElement = QString (t)+ " -- " + theText;
	timeTableList. append (listElement);
	displayList. setStringList (timeTableList);
	this	-> setModel (&displayList);
}

void	timeTableHandler::clear () {
        timeTableList. clear ();
        displayList. setStringList (timeTableList);
        this    -> setModel (&displayList);
}

