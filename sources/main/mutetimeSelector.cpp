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
#include	"mutetimeSelector.h"
#include	<QHBoxLayout>
#include	<QVBoxLayout>


	mutetimeSelector::mutetimeSelector	(int oldValue) {
	theBox		= new QSpinBox;
	theLabel	= new QLabel ("set mute time (minutes)");
	theBox		-> setValue (oldValue);
	QHBoxLayout	*layout = new QHBoxLayout;
	layout		-> addWidget (theLabel);
	layout		-> addWidget (theBox);
	theButton	= new QPushButton;
	theButton	-> setText ("accept");
	QVBoxLayout	*l2	= new QVBoxLayout;
	l2		-> addLayout (layout);
	l2		-> addWidget (theButton);
	setWindowTitle ("mute time");
	
	setLayout (l2);
	connect (theButton, &QPushButton::clicked,
	         this, &mutetimeSelector::clicked);
	show ();
}

	mutetimeSelector::~mutetimeSelector	() {
	hide ();
}

void	mutetimeSelector::clicked () {
int	newVal	= theBox	-> value ();
	QDialog::done (newVal);
}

