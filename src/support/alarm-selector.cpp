#
/*
 *    Copyright (C) 2020
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
#include	"alarm-selector.h"
#include	<cstdio>
#include	<QVBoxLayout>
//
	alarmSelector::alarmSelector () {
	toptext		= new QLabel (this);
	toptext		-> setText ("Select a service");
	servicesDisplay	= new QListView (this);
	QVBoxLayout	*layOut = new QVBoxLayout;
	layOut		-> addWidget (servicesDisplay);
	layOut		-> addWidget (toptext);
	setWindowTitle (tr("dongle select"));
	setLayout (layOut);

	services = QStringList();
	serviceList. setStringList (services);
	servicesDisplay	-> setModel (&serviceList);
	connect (servicesDisplay, SIGNAL (clicked (QModelIndex)),
	         this, SLOT (selectService (QModelIndex)));
	selectedItem	= -1;
}

	alarmSelector::~alarmSelector () {
}

void	alarmSelector::addtoList (const QString &s) {
	services << s;
	serviceList. setStringList (services);
	servicesDisplay	-> setModel (&serviceList);
	servicesDisplay	-> adjustSize();
	adjustSize();
}

void	alarmSelector::selectService (QModelIndex ind) {
	QDialog::done (ind. row ());
}

