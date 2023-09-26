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
#include	"font-selector.h"
#include	<cstdio>
#include	<QVBoxLayout>
//
	fontSelector::fontSelector	() {
	toptext		= new QLabel (this);
	toptext		-> setText ("Select a font");
	selectorDisplay	= new QListView (this);
	QVBoxLayout	*layOut = new QVBoxLayout;
	layOut		-> addWidget (selectorDisplay);
	layOut		-> addWidget (toptext);
	setWindowTitle (tr("font select"));
	setLayout (layOut);

	theFonts	= QStringList ();
	selectorDisplay	-> setModel (&fontList);
	connect (selectorDisplay, SIGNAL (clicked (QModelIndex)),
	         this, SLOT (select_font (QModelIndex)));
	selectedItem	= -1;
}

	fontSelector::~fontSelector	() {
	delete selectorDisplay;
	delete	toptext;
}

void	fontSelector::add	(const QString s) {
	theFonts << s;
        fontList. setStringList (theFonts);
        selectorDisplay -> setModel (&fontList);
}

void	fontSelector::select_font (QModelIndex s) {
	QDialog::done (s. row());
}

