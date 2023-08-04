#
/*
 *    Copyright (C) 2014 .. 2023
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
#include	"upload.h"
#include	<QVBoxLayout>
#include	<QHBoxLayout>
#include	<QSettings>

	uploadHandler::uploadHandler	(QSettings *dabSettings) {
	this	-> dabSettings = dabSettings;
	uploadText	= new QLabel (this);
	uploadText	-> setText ("upload the scanresult?");
	yesButton	= new QPushButton ("yes");
	noButton	= new QPushButton ("no");
	QHBoxLayout	*layout = new QHBoxLayout;
	layout		-> addWidget (yesButton);
	layout		-> addWidget (noButton);
	QVBoxLayout	*l2	= new QVBoxLayout;
	l2		-> addWidget (uploadText);
	l2		-> addItem (layout);
	setLayout (l2);
	connect (yesButton, SIGNAL (clicked ()),
	         this, SLOT (handle_yesButton ()));
	connect (noButton, SIGNAL (clicked ()),
	         this, SLOT (handle_noButton ()));
	show ();
}

	uploadHandler::~uploadHandler	() {
	hide ();
	delete	uploadText;
	delete	yesButton;
	delete	noButton;
}

void	uploadHandler::handle_yesButton () {
	QDialog::done (true);
}

void	uploadHandler::handle_noButton	() {
	QDialog::done (false);
}

