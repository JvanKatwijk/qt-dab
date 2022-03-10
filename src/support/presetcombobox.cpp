#
/*
 *    Copyright (C) 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
#include "presetcombobox.h"

#include <QEvent>
#include <QAbstractItemView>

presetComboBox::presetComboBox (QWidget *parent):
	                                    QComboBox(parent) {
	QAbstractItemView* iv = view ();
	iv -> viewport () -> installEventFilter (this);
}
//
//	It seems that the combobox reacts on pree as well as release
//	events
bool	presetComboBox::eventFilter (QObject *obj, QEvent *event) {
//
//	Ignore the release event explicitly
	if ((event -> type () == QEvent::MouseButtonRelease) &&
	   (static_cast<QMouseEvent *>(event) -> button () ==
	                                          Qt::RightButton)) {
	   return true;
	}
//
//	This is what we are going for
	if (event -> type () == QEvent::MouseButtonPress) {
	   if (static_cast<QMouseEvent  *>(event) -> button () ==
	                                            Qt::RightButton) {
 	      QAbstractItemView* view = this -> view ();
	      QMouseEvent *me = static_cast<QMouseEvent *> (event);
	      QModelIndex index = view -> indexAt (me -> pos ());
 	      if (!index.isValid()) 
	          return QObject::eventFilter (obj, event);
//	      QString item = this -> model () -> data (index, Qt::DisplayRole).toString();
	      this	-> removeItem (index. row ());
	      return true;
	   }
	}
 	return QObject::eventFilter (obj, event);
}
	   
QSize   presetComboBox:: sizeHint ()const {
QSize   temp = QComboBox::sizeHint ();
        return QSize (temp. rwidth () / 6, temp. rheight () / 4);
}

