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
#include	"settings-handler.h"

	coordinates::coordinates	(QSettings *dabSettings) {
	this	-> dabSettings = dabSettings;
	QString latVal = value_s (dabSettings, "MAP_HANDLING",
	                  	                HOME_LATITUDE, "0");
        QString lonVal = value_s (dabSettings, "MAP_HANDLING",
	                                        HOME_LONGITUDE, "0");

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

	connect (latitude, &QLineEdit::returnPressed,
	         this, &coordinates::setLatitude);
	connect (longitude, &QLineEdit::returnPressed,
	         this, &coordinates::setLongitude);
	connect (acceptButton, &QPushButton::clicked,
	         this, &coordinates::handleAcceptButton);
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

void	coordinates::setLatitude () {
	latitudeValue	= true;
}

void	coordinates::setLongitude () {
	longitudeValue = true;
}

void	coordinates::handleAcceptButton () {
	if (!latitude || !longitude)
	   return;
	QString lat	= latitude -> text ();
	QString lon	= longitude -> text ();
	store (dabSettings, "MAP_HANDLING", HOME_LATITUDE, lat);
	store (dabSettings, "MAP_HANDLING", HOME_LONGITUDE, lon);
	QDialog::done (0);
}

