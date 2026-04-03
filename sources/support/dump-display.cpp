#
/*
 *    Copyright (C)  2015 .. 2025
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

#include	<stdio.h>
#include	"dump-display.h"
#include	<QVBoxLayout>

	dumpDisplay::dumpDisplay	(QString type, const QString &name):
	                                                     QFrame (nullptr) {
	QVBoxLayout *lo	= new QVBoxLayout;
	dumpName	= new QLabel (name);
	typeName	= new QLabel (type);
	durationLabel	= new QLabel ();
	lo	-> addWidget (dumpName);
	lo	-> addWidget (typeName);
	lo	-> addWidget (durationLabel);
	this	-> setLayout (lo);
	setWindowTitle ("dumping monitor");
	duration	= 0;
	connect (&theClock, &QTimer::timeout,
	         this, &dumpDisplay::handle_timer);
	theClock. setInterval (1000);
	theClock. setSingleShot	(false);
	theClock. start (1000);
	show ();
}

	dumpDisplay::~dumpDisplay	() {
	theClock. stop ();
}

void	dumpDisplay::handle_timer	() {
	duration ++;
	durationLabel	-> setText (QString ("Running for ") +
	                              QString::number (duration) + " sec");
}

	
