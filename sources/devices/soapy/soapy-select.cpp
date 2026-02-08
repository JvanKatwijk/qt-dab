#
/*
 *    Copyright (C) 2026
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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
#include	<cstdio>
#include	<QVBoxLayout>
#include	"soapy-select.h"
//
//	Whenever there are two or more sdrplay devices connected
//	to the computer, the user is asked to make a choice.

	soapySelect::soapySelect	() {
	toptext		= new QLabel (this);
	toptext		-> setText ("Qt-DAB selector");
	selectorDisplay	= new QListView (this);
	QVBoxLayout	*layOut = new QVBoxLayout;
	layOut		-> addWidget (selectorDisplay);
	layOut		-> addWidget (toptext);
	setWindowTitle (tr("soapy select"));
	setLayout (layOut);

	Devices = QStringList();
	deviceList. setStringList (Devices);
	selectorDisplay	-> setModel (&deviceList);
	connect (selectorDisplay, &QListView::clicked,
	         this, &soapySelect::select_device);
	selectedItem	= -1;
}

	soapySelect::~soapySelect () {
}

void	soapySelect::addtoList (const char *v) {
QString s (v);

	Devices << s;
	deviceList. setStringList (Devices);
	selectorDisplay	-> setModel (&deviceList);
	selectorDisplay	-> adjustSize();
	adjustSize();
	fprintf (stderr, "adding %s\n", v);
}

void	soapySelect::select_device (QModelIndex s) {
	QDialog::done (s. row());
}

