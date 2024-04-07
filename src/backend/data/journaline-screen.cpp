#
/*
 *    Copyright (C) 2017 .. 2024
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
 *
 */

#include	"journaline-screen.h"
#include	<QHBoxLayout>
#include	<QVBoxLayout>

	journalineScreen::journalineScreen (std::vector<tableElement> &table):
	                                        myFrame (nullptr) {

	this	-> table = &table;
	pathVector. resize (0);
	resetButton	= new QPushButton ("reset");
	upButton	= new QPushButton ("up");
	mainText	= new QLabel ("");
	subContent	= new QListView ();
	QHBoxLayout *LH	= new QHBoxLayout ();
	QVBoxLayout *LV	= new QVBoxLayout ();

	LH		-> addWidget (resetButton);
	LH		-> addWidget (upButton);
	LV		-> addLayout (LH);
	LV		-> addWidget (mainText);
	LV		-> addWidget (subContent);
	myFrame. setWindowTitle ("Journaline");
	myFrame. setLayout (LV);

	connect (resetButton, SIGNAL (clicked ()),
	         this, SLOT (handle_resetButton ()));
	connect (upButton, SIGNAL (clicked ()),
	         this, SLOT (handle_upButton ()));
	connect (subContent, SIGNAL (clicked (QModelIndex)),
	         this, SLOT (select_sub (QModelIndex)));

	myFrame. show ();
}

	journalineScreen::~journalineScreen () {
	myFrame. hide ();
}

void	journalineScreen::handle_resetButton	() {
	pathVector. resize (0);
	fprintf (stderr, "Tablesize %d\n", (int)((*table). size ()));
	for (int i = 0; i < (int)((*table). size ()); i ++) {
	   if ((*table) [i]. key == 0) {
	      displayElement (*((*table) [i].element));
	      pathVector. push_back (0);
	   }
	}
}

void	journalineScreen::handle_upButton	() {
	if (pathVector. size () == 1) 
	   return;
	pathVector. pop_back ();
	int index	= pathVector. back ();
	displayElement (*((*table) [index].element));
}

void	journalineScreen::select_sub (QModelIndex ind) {
	fprintf (stderr, "We vinden element %d\n", ind. row ());
	int currentElement = pathVector. back ();
	if ((*table)[currentElement].element -> object_type == NML::PLAIN)
	   return;
	NML::News_t *temp	= (*table) [currentElement]. element;
	int next	= temp	-> item [ind. row ()]. link_id;
	fprintf (stderr, "De linkid is %d\n", next);
	for (auto &tabEl: *table) {
	   if (tabEl. key == next) {
	      pathVector. push_back (next);
	      displayElement (*(tabEl. element));
	      return;
	   }
	}
	fprintf (stderr, "No element for this link\n");
}

void	journalineScreen::displayElement (NML::News_t &element) {
	fprintf (stderr, "Display een element met type %d\n",
	                              element. object_type);
	switch (element. object_type) {
	   case NML::MENU:
	      display_Menu (element);
	      break;
	   case NML::PLAIN:
	      display_Plain (element);
	      break;
	   case NML::LIST:
	      display_List (element);
	      break;
	   default:
	      break;
	}
}

void	journalineScreen::display_Menu (NML::News_t &element) {
	std::string t = element. title;
	mainText	-> setText (QString::fromStdString (t));
	model. clear ();
	for (int i = 0; i < (int)(element. item. size ()); i ++)  {
	   NML::Item_t *item = &(element. item [i]);
	   model. appendRow (new QStandardItem (QString::fromStdString (item -> text)));
	}
	subContent	-> setModel (&model);

}

void	journalineScreen::display_Plain (NML::News_t &element) {
	std::string t = element. title;
	mainText	-> setText (QString::fromStdString (t));
	model. clear ();
	NML::Item_t *item = &(element. item [0]);
	model. appendRow (new QStandardItem (QString::fromStdString (item -> text)));
}

void	journalineScreen::display_List (NML::News_t &element) {
}

