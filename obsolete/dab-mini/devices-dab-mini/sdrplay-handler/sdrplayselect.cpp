#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
 *    Many of the ideas as implemented in Qt-DAB are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
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
 *
 */
#include	<stdio.h>
#include	<QVBoxLayout>
#include	"sdrplayselect.h"
//
//	Whenever there are two or more sdrplay devices connected
//	to the computer, the user is asked to make a choice.

	sdrplaySelect::sdrplaySelect (void) {
	toptext		= new QLabel (this);
	toptext		-> setText ("Select an rsp device");
	selectorDisplay	= new QListView (this);
	QVBoxLayout	*layOut = new QVBoxLayout;
	layOut		-> addWidget (selectorDisplay);
	layOut		-> addWidget (toptext);
	setWindowTitle (tr("RSP select"));
	setLayout (layOut);

	Devices = QStringList ();
	deviceList. setStringList (Devices);
	selectorDisplay	-> setModel (&deviceList);
	connect (selectorDisplay, SIGNAL (clicked (QModelIndex)),
	         this, SLOT (select_rsp (QModelIndex)));
	selectedItem	= -1;
}

	sdrplaySelect::~sdrplaySelect (void) {
}

void	sdrplaySelect::addtoList (const char *v) {
QString s (v);

	Devices << s;
	deviceList. setStringList (Devices);
	selectorDisplay	-> setModel (&deviceList);
	selectorDisplay	-> adjustSize ();
	adjustSize ();
}

void	sdrplaySelect::select_rsp (QModelIndex s) {
	QDialog::done (s. row ());
}

