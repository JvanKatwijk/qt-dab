#
/*
 *    Copyright (C) 2016 .. 2023
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
#include	"audiosystem-selector.h"
#include	<cstdio>
#include	<QVBoxLayout>
#include	<QSettings>
#include	"settingNames.h"

	audiosystemSelector::audiosystemSelector (QSettings *s) {
	this		-> dabSettings	= s;
	toptext		= new QLabel (this);
	toptext		-> setText ("Select an audio support system");
	servicesDisplay	= new QListView (this);
	QVBoxLayout	*layOut = new QVBoxLayout;
	layOut		-> addWidget (servicesDisplay);
	layOut		-> addWidget (toptext);
	setWindowTitle (tr("audio system select"));
	setLayout (layOut);

	services = QStringList();
	services << S_PORT_AUDIO << S_QT_AUDIO;
	serviceList. setStringList (services);
	servicesDisplay	-> setModel (&serviceList);
	servicesDisplay	-> adjustSize();
	adjustSize();
	connect (servicesDisplay, &QListView::clicked,
	         this, &audiosystemSelector::selectService);
}

	audiosystemSelector::~audiosystemSelector () {
}

void	audiosystemSelector::selectService (QModelIndex ind) {
QString select	= ind. data (Qt::DisplayRole). toString ();
	fprintf (stderr, "selected system %s\n", select. toLatin1 (). data ());
	dabSettings	-> setValue (S_SOUND_HANDLER, select);
	QDialog::done (ind. row ());
}

