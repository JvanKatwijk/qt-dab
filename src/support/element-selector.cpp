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
#include	"element-selector.h"
#include	<cstdio>
#include	<QVBoxLayout>
#include	<QTime>
//
	elementSelector::elementSelector (const QString &serviceName):
	                                   theService (serviceName),
	                                   hourBox (),
	                                   minuteBox (),
	                                   readyBox ("ready") {
	                                   
QTime currentTime = QTime::currentTime ();
QHBoxLayout	*layOut = new QHBoxLayout ();

	hourBox. setToolTip ("select the hour in the range 0 .. 23");
	hourBox. setMaximum (23);
	hourBox. setValue (currentTime. hour ());
	minuteBox. setToolTip ("select the minute");
	minuteBox. setMaximum (59);
	minuteBox. setValue (currentTime. minute ());
	readyBox. setToolTip ("click here when time is set");
	layOut -> addWidget (&theService);
	layOut -> addWidget (&hourBox);
	layOut -> addWidget (&minuteBox);
	layOut -> addWidget (&readyBox);
	setWindowTitle (tr("time select"));
	setLayout (layOut);

	connect (&readyBox, SIGNAL (stateChanged (int)),
	         this, SLOT (collectData ()));
}

	elementSelector::~elementSelector () {
}

void	elementSelector::collectData () {
int	val	= hourBox. value () * 60 + minuteBox. value ();
	if (readyBox. isChecked ())
	   QDialog::done (val);
}

