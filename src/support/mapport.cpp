#
/*
 *    Copyright (C) 2013, 2014, 2015, 2016, 2017
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
#include	"mapport.h"
#include	<QFormLayout>
#include	<QVBoxLayout>
#include	<QSettings>

	mapPortHandler::mapPortHandler	(QSettings *dabSettings) {
	this	-> dabSettings = dabSettings;
	mapPortText	= new QLabel (this);
	mapPortText	-> setText ("Port Address for maps");
	mapPortSetting	= new QLineEdit (this);	
	acceptButton	= new QPushButton ("accept");
	QFormLayout	*layout = new QFormLayout;
	layout		-> addWidget (mapPortText);
	layout		-> addWidget (mapPortSetting);
	setWindowTitle ("set Port Address");
	QVBoxLayout	*total = new QVBoxLayout;
	total		-> addItem (layout);
	total		-> addWidget (acceptButton);
	setLayout (total);
	connect (acceptButton, SIGNAL (clicked ()),
	         this, SLOT (handle_acceptButton ()));
	show ();
}

	mapPortHandler::~mapPortHandler	() {
	hide ();
	delete	mapPortText;
	delete	mapPortSetting;
	delete	acceptButton;
}

void	mapPortHandler::handle_acceptButton () {
	dabSettings	-> setValue ("mapPort", mapPortSetting -> text ());
	QDialog::done (0);
}

