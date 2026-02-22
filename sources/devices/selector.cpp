#
/*
 *    Copyright (C) 2026
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB
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
#include    "selector.h"
#include    <cstdio>
#include    <QVBoxLayout>
//
//  Whenever there are two or more soapy devices connected
//  to the computer, the user is asked to make a choice.

	selector::selector (const QString &s) {
	toptext		= new QLabel(this);
	toptext ->	setText ("device select");
	selectorDisplay = new QListView (this);
	QVBoxLayout *layOut = new QVBoxLayout;
	layOut	->	addWidget (toptext);
	layOut	->	addWidget (selectorDisplay);
	setWindowTitle (s);
	setLayout(layOut);

	Devices		= QStringList ();
	deviceList. setStringList (Devices);
	selectorDisplay -> setModel (&deviceList);
	connect (selectorDisplay, &QListView::clicked,
	         this, &selector::select_device);
	selectedItem = -1;
}

	selector::~selector () {
}

void	selector::addtoList (const QString &v) {
	Devices << v;
	deviceList. setStringList (Devices);
	selectorDisplay -> setModel (&deviceList);
	selectorDisplay -> adjustSize ();
	adjustSize ();
}

void	selector::select_device (QModelIndex s) {
	QDialog::done (s.row ());
}
