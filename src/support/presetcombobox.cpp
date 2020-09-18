/*****************************************************************************
 * Copyright (C) 2018 Shie Erlich <krusader@users.sourceforge.net>           *
 * Copyright (C) 2018 Rafi Yanai <krusader@users.sourceforge.net>            *
 * Copyright (C) 2018 Krusader Krew [https://krusader.org]                   *
 *                                                                           *
 * This file is part of Krusader [https://krusader.org].                     *
 *                                                                           *
 * Krusader is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation, either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * Krusader is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with Krusader.  If not, see [http://www.gnu.org/licenses/].         *
 *****************************************************************************/
#include "presetcombobox.h"

#include <QEvent>
#include <QKeyEvent>
#include <QAbstractItemView>

bool KHBoxEventFilter::eventFilter (QObject *obj, QEvent *event) {
	if (event -> type () == QEvent::KeyPress) {
	   auto keyEvent = static_cast<QKeyEvent *>(event);
	   if ((keyEvent->modifiers() == Qt::ShiftModifier) &&
	       (keyEvent->key() == Qt::Key::Key_Delete)) {
//	      auto box = dynamic_cast<QComboBox *>(obj);
	      auto box = dynamic_cast<presetComboBox *>(obj);
	      if (box != nullptr) {
//	Delete the current item
	         QString content = box -> currentText ();
	         box->removeItem(box->currentIndex());
	         return true;
	      }
	   }
	}
//	Perform the usual event processing
	return QObject::eventFilter(obj, event);
}

bool	KHBoxListEventFilter::eventFilter (QObject *obj, QEvent *event) {
	if (event -> type () == QEvent::KeyPress) {
	   auto keyEvent = static_cast<QKeyEvent *>(event);
	   if ((keyEvent->modifiers() == Qt::ShiftModifier) &&
	       (keyEvent->key() == Qt::Key::Key_Delete)) {
	      auto iv = dynamic_cast<QAbstractItemView *>(obj);
	      if (iv->model() != nullptr) {
//	Delete the current item from the popup list
	         iv -> model()->removeRow(iv->currentIndex().row());
	         return true;
	      }
	   }
	}
//	Perform the usual event processing
	return QObject::eventFilter(obj, event);
}

presetComboBox::presetComboBox (QWidget *parent):
	                                    QComboBox(parent) {
	installEventFilter (&boxEF);
	QAbstractItemView* iv = view ();
	if (iv != nullptr)
	   iv -> installEventFilter (&listEF);
}

QSize   presetComboBox:: sizeHint ()const {
QSize   temp = QComboBox::sizeHint ();
        return QSize (temp. rwidth () / 6, temp. rheight () / 4);
}

void	presetComboBox::mouseDoubleClickEvent (QMouseEvent *e) {
	fprintf (stderr, "double mouse click\n");
}

