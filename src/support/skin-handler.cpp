#
/*
 *    Copyright (C) 2016
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
#include	<cstdio>
#include	<QVBoxLayout>
#include	"skin-handler.h"
//

	skinHandler::skinHandler	() {
	toptext		= new QLabel (this);
	toptext		-> setText ("select a skin");
	selectorDisplay	= new QListView (this);
	QVBoxLayout	*layOut = new QVBoxLayout;
	layOut		-> addWidget (selectorDisplay);
	layOut		-> addWidget (toptext);
	setWindowTitle (tr("skin select"));
	setLayout (layOut);

	skins = QStringList();
	skins << "skins" << "no skin" << "Adaptic" <<
	                        "Combinear" << "Darkeum" << "Fibers";
	skinList. setStringList (skins);
	selectorDisplay	-> setModel (&skinList);
	connect (selectorDisplay, SIGNAL (clicked (QModelIndex)),
	         this, SLOT (select_skin (QModelIndex)));
	selectedItem	= -1;
}

	skinHandler::~skinHandler	() {
}

void	skinHandler::select_skin (QModelIndex s) {
	QDialog::done (s. row());
}

