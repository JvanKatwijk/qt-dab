#
/*
 *    Copyright (C) 2016 .. 2024
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

void	timeTableHandler::display	(const scheduleDescriptor &schedule) {
}

void	timeTableHandler::clear () {
        timeTableList. clear ();
        displayList. setStringList (timeTableList);
        this    -> setModel (&displayList);
}

