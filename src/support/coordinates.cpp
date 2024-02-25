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
#include	"coordinates.h"
#include	<QDoubleValidator>
#include	<QFormLayout>
#include	<QFormLayout>
#include	<QVBoxLayout>
#include	<QSettings>

#include	"settingNames.h"
	coordinates::coordinates	(QSettings *dabSettings) {
	this	-> dabSettings = dabSettings;
	QString latVal = dabSettings  -> value (HOME_LATITUDE,
	                                              "0"). toString ();
        QString lonVal = dabSettings  -> value (HOME_LONGITUDE,
	                                              "0"). toString ();

	QLocale	currentLocale	= QLocale::system ();
	QLocale englishLocale(QLocale::English, QLocale::UnitedStates);
	latitudeText	= new QLabel (this);
	latitudeText	-> setText ("latitude (decimal, US style");
	QDoubleValidator *la = new QDoubleValidator (-90.0, 9.0, 5);
	latitude	= new QLineEdit (this);	
	latitude	-> setText (latVal);
	latitude	-> setValidator (la);

	longitudeText	= new QLabel (this);
	longitudeText	-> setText ("longitude (decimal)");
	QDoubleValidator *lo = new QDoubleValidator (-180.0, 180.0, 5);
	longitude	= new QLineEdit (this);
	longitude	-> setText (lonVal);
	longitude	-> setValidator (lo);
	QLocale::setDefault (currentLocale);
	QFormLayout	*layout = new QFormLayout;
	layout		-> addWidget (latitudeText);
	layout		-> addWidget (latitude);
	layout		-> addWidget (longitudeText);
	layout		-> addWidget (longitude);

	setWindowTitle ("select coordinates");
	acceptButton	= new QPushButton ("accept");
	QVBoxLayout	*total = new QVBoxLayout;
	total		-> addItem (layout);
	total		-> addWidget (acceptButton);
	setLayout (total);

	connect (latitude, SIGNAL (returnPressed ()),
	         this, SLOT (set_latitude ()));
	connect (longitude, SIGNAL (returnPressed ()),
	         this, SLOT (set_longitude ()));
	connect (acceptButton, SIGNAL (clicked ()),
	         this, SLOT (handle_acceptButton ()));
	show ();
	latitudeValue	= false;	
	longitudeValue	= false;
}

	coordinates::~coordinates	() {
	hide ();
	delete	acceptButton;
	delete	latitudeText;
	delete	longitudeText;	
	delete	latitude;
	delete	longitude;	
}

void	coordinates::set_latitude () {
	latitudeValue	= true;
}

void	coordinates::set_longitude () {
	longitudeValue = true;
}

void	coordinates::handle_acceptButton () {
	if (!latitude || !longitude)
	   return;
	dabSettings	-> setValue (HOME_LATITUDE, latitude -> text ());
	dabSettings	-> setValue (HOME_LONGITUDE, longitude -> text ());
	QDialog::done (0);
}

