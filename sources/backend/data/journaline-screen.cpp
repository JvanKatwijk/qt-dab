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

	journalineScreen::journalineScreen (std::vector<tableElement> &table,
	                                                   std::mutex *locker):
	                                        myFrame (nullptr) {

	this	-> table = &table;
	this	-> locker	= locker;
	pathVector. resize (0);
	resetButton	= new QPushButton ("home");
	upButton	= new QPushButton ("up");
	ackLabel	= new QLabel ("Features NewsService Journaline(R) decoder technology by\n * Fraunhofer IIS, Erlangen, Germany.\n * For more information visit http://www.iis.fhg.de/dab");
	mainText	= new QLabel ("");
	subContent	= new QListView ();
	subContent	-> setToolTip ("Features NewsService Journaline(R) decoder technology by\n * Fraunhofer IIS, Erlangen, Germany.\n * For more information visit http://www.iis.fhg.de/dab"
);
	QHBoxLayout *LH	= new QHBoxLayout ();
	QVBoxLayout *LV	= new QVBoxLayout ();

	LH		-> addWidget (resetButton);
	LH		-> addWidget (upButton);
	LV		-> addLayout (LH);
	LV		-> addWidget (ackLabel);
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
	myFrame. show ();
}

	journalineScreen::~journalineScreen () {
	myFrame. hide ();
}

void	journalineScreen::hide			() {
	myFrame. hide ();
}

void	journalineScreen::handle_resetButton	() {
	pathVector. resize (0);
	locker -> lock ();
	for (int i = 0; i < (int)((*table). size ()); i ++) {
	   if ((*table) [i]. key == 0) {
	      displayElement (*((*table) [i].element), false);
	      pathVector. push_back (0);
	      break ;
	   }
	}
	locker -> unlock ();
}

void	journalineScreen::handle_upButton	() {
	if (pathVector. size () < 2) 
	   return;
	pathVector. pop_back ();
	locker -> lock ();
	int index	= findIndex (pathVector. back ());
	if (index >= 0) 
	   displayElement (*((*table) [index].element), false);
	locker -> unlock ();
}

void	journalineScreen::select_sub (QModelIndex ind) {
//
//	first, identify the current element
	int t = pathVector. back ();
	locker -> lock ();
	int currentIndex	= findIndex (t);
	if (currentIndex < 0) {
	   locker -> unlock ();
	   return;
	}
	NML::News_t *currentElement	= (*table) [currentIndex]. element;
	locker -> unlock ();
//
//	for sure, the PLAIN element does not have siblings
	if (currentElement -> object_type == NML::PLAIN)
	   return;
	locker -> lock ();
	NML::Item_t item	= currentElement -> item [ind. row ()];
	locker -> unlock ();
	if (true) {
//	if (item. link_id_available) {
	   locker -> lock ();
	   int ind = findIndex (item. link_id);
	   if (ind >= 0) {
	      NML::News_t *target = (*table) [ind]. element;
	      displayElement (*target, (*table)[ind]. updated);
	      (*table)[ind]. updated = false;
	      pathVector. push_back (item. link_id);
	   }
	   locker -> unlock ();
	}
}

void	journalineScreen::displayElement (NML::News_t &element, bool updated) {
	switch (element. object_type) {
	   case NML::MENU:
	      display_Menu (element, updated);
	      break;
	   case NML::PLAIN:
	      display_Plain (element, updated);
	      break;
	   case NML::LIST:
	      display_List (element, updated);
	      break;
	   default:
	      break;
	}
}

void	journalineScreen::display_Menu (NML::News_t &element, bool updated) {
	QString addendum = updated ? " *" : "";
	std::string t = element. title;
	mainText	-> setText (QString::fromStdString (t));
	model. clear ();
	for (int i = 0; i < (int)(element. item. size ()); i ++)  {
	   NML::Item_t *item = &(element. item [i]);
	   model. appendRow (new QStandardItem (QString::fromStdString (item -> text) + addendum));
	}
	subContent	-> setModel (&model);

}

void	journalineScreen::display_Plain (NML::News_t &element, bool updated) {
	std::string t = element. title;
	mainText	-> setText (QString::fromStdString (t));
	model. clear ();
	NML::Item_t *item = &(element. item [0]);
	QString addendum = updated ? " *" : "";
	model. appendRow (new QStandardItem (QString::fromStdString (item -> text) + addendum));
}

void	journalineScreen::display_List (NML::News_t &element, bool updated) {
	QString addendum = updated ? " *": "";
	std::string t = element. title;
	mainText	-> setText (QString::fromStdString (t));
	model. clear ();
	for (int i = 0; i < (int)(element. item. size ()); i ++)  {
	   NML::Item_t *item = &(element. item [i]);
	   model. appendRow (new QStandardItem (QString::fromStdString (item -> text) + addendum));
	}
	subContent	-> setModel (&model);
}

int	journalineScreen::findIndex	(int key) {
int res	= -1;
	for (uint16_t i = 0; i < table -> size (); i ++)
	   if ((*table) [i]. key == key) {
	      res = i;
	      break;
	   }
	return res;
}

void	journalineScreen::start		(int index) {
	pathVector. push_back (0);
	locker -> lock ();
	displayElement (*(*table) [index]. element, false);
	locker -> unlock ();
}

