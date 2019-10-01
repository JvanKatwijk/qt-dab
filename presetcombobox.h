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
#ifndef	__PRESET_COMBOBOX__
#define	__PRESET_COMBOBOX__

#include	<QComboBox>

class KHBoxEventFilter : public QObject {
Q_OBJECT
public:
    explicit KHBoxEventFilter(QObject *parent = nullptr) : QObject(parent) {}

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

//! An event filter for the popup list of a presetComboBox, e.g.
// it deletes the current item when the user presses Shift+Del
class KHBoxListEventFilter : public QObject {
Q_OBJECT
public:
    explicit KHBoxListEventFilter (QObject *parent = nullptr):
	                                             QObject(parent) {}

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

//! A specialized version of a QComboBox, e.g. it deletes the current item when the user presses Shift+Del
class presetComboBox : public QComboBox {
Q_OBJECT

public:
	explicit presetComboBox(QWidget *parent = nullptr);

protected:
	KHBoxEventFilter boxEF;
	KHBoxListEventFilter listEF;
};

#endif 
