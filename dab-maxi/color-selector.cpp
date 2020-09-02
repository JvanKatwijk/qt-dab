#
/*
 *    Copyright (C) 2020
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
#include	"color-selector.h"
//
//	Whenever there are two or more sdrplay devices connected
//	to the computer, the user is asked to make a choice.

	colorSelector::colorSelector	(const QString &topText) {
	toptext		= new QLabel (this);
	toptext		-> setText (topText);
	selectorDisplay	= new QListView (this);
	QVBoxLayout	*layOut = new QVBoxLayout;
	layOut		-> addWidget (selectorDisplay);
	layOut		-> addWidget (toptext);
	setWindowTitle (tr("color select"));
	setLayout (layOut);

	colors = QStringList();
	colors << "colors" << "white" << "black" << "red" <<
	          "darkRed" << "green" << "darkGreen" << "blue" <<
	          "darkBlue" << "cyan" << "darkCyan" << "magenta" <<
	          "darkMagenta" << "yellow" << "darkYellow" <<
	          "gray" << "darkGray";
	colorList. setStringList (colors);
	selectorDisplay	-> setModel (&colorList);
	connect (selectorDisplay, SIGNAL (clicked (QModelIndex)),
	         this, SLOT (select_color (QModelIndex)));
	selectedItem	= -1;
}

	colorSelector::~colorSelector() {
}

void	colorSelector::select_color (QModelIndex s) {
	QDialog::done (s. row());
}

QString	colorSelector::getColor (int index) {
	return colors. at (index);
}

