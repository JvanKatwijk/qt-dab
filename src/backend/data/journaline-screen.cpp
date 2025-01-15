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
	subContent	-> setToolTip ("Features NewsService Journaline(R) decoder technology by\n * Fraunhofer IIS, Erlangen, Germany.\n * For more information visit http://www.iis.fhg.de/dab"
);
	QHBoxLayout *LH	= new QHBoxLayout ();
	QVBoxLayout *LV	= new QVBoxLayout ();

	LH		-> addWidget (resetButton);
	LH		-> addWidget (upButton);
	LV		-> addLayout (LH);
	LV		-> addWidget (mainText);
	LV		-> addWidget (subContent);
	myFrame. setWindowTitle ("Journaline");
	myFrame. setLayout (LV);

	connect (resetButton, &QPushButton::clicked,	
	         this, &journalineScreen::handle_resetButton);
	connect (upButton, &QPushButton::clicked,	
	         this, &journalineScreen::handle_upButton);
	connect (subContent, &QListView::clicked,
	         this, &journalineScreen::select_sub);
//#ifdef	__USE_JOURNALINE__
	myFrame. show ();
//#endif
}

	journalineScreen::~journalineScreen () {
	myFrame. hide ();
}

void	journalineScreen::handle_resetButton	() {
	pathVector. resize (0);
	for (int i = 0; i < (int)((*table). size ()); i ++) {
	   if ((*table) [i]. key == 0) {
	      displayElement (*((*table) [i].element));
	      pathVector. push_back (0);
	      return;
	   }
	}
}

void	journalineScreen::handle_upButton	() {
	if (pathVector. size () < 2) 
	   return;
	pathVector. pop_back ();
	int index	= findIndex (pathVector. back ());
	if (index < 0)
	   return;
	displayElement (*((*table) [index].element));
}

void	journalineScreen::select_sub (QModelIndex ind) {
//
//	first, identify the current element
	int t = pathVector. back ();
	int currentIndex	= findIndex (t);
	if (currentIndex < 0) {
	   return;
	}
	NML::News_t *currentElement	= (*table) [currentIndex]. element;
//
//	for sure, the PLAIN element does not have siblings
	if (currentElement -> object_type == NML::PLAIN)
	   return;
	NML::Item_t item	= currentElement -> item [ind. row ()];
	if (true) {
//	if (item. link_id_available) {
	   int ind = findIndex (item. link_id);
	   if (ind < 0) {
//	      fprintf (stderr, "Link %d not found\n", item. link_id);
	      return;
	   }
	   NML::News_t *target = (*table) [ind]. element;
	   displayElement (*target);
	   pathVector. push_back (item. link_id);
	}
//	else
//	   fprintf (stderr, "No element for this link\n");
}

void	journalineScreen::displayElement (NML::News_t &element) {
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
	std::string t = element. title;
	mainText	-> setText (QString::fromStdString (t));
	model. clear ();
	for (int i = 0; i < (int)(element. item. size ()); i ++)  {
	   NML::Item_t *item = &(element. item [i]);
	   model. appendRow (new QStandardItem (QString::fromStdString (item -> text)));
	}
	subContent	-> setModel (&model);
}

int	journalineScreen::findIndex	(int key) {
	for (uint16_t i = 0; i < table -> size (); i ++)
	   if ((*table) [i]. key == key)
	      return i;
	return -1;
}

void	journalineScreen::start		(int index) {
	fprintf (stderr, "we zijn in start\n");
	pathVector. push_back (0);
	displayElement (*(*table) [index]. element);
}

