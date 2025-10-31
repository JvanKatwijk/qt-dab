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
#pragma once

#include	<QObject>
#include	<QFrame>
#include	<QPushButton>
#include	<QLabel>
#include	<QListView>
#include	"NML.h"
#include	<QModelIndex>
#include	<QStandardItemModel>

#include	<mutex>
typedef struct {
	int key;
	NML::News_t *element;
	bool updated;
} tableElement;

class	journalineScreen: public QObject {
Q_OBJECT
public:
		journalineScreen	(std::vector<tableElement> &table,
	                                             std::mutex *locker);
		~journalineScreen	();
	void	displayElement		(NML::News_t &element, bool);
	void	display_Menu		(NML::News_t &element, bool);
	void	display_Plain		(NML::News_t &element, bool);
	void	display_List		(NML::News_t &element, bool);
	void	hide			();
private:
	std::vector<tableElement> *table;
	std::mutex		*locker;
	tableElement		currentElement;
	std::vector<int>	pathVector;

	QFrame		myFrame;
	QPushButton	*resetButton;
	QPushButton	*upButton;
	QLabel		*mainText;
	QListView	*subContent;
	QStandardItemModel      model;

	int		findIndex	(int);
public slots:
	void		start		(int);
private slots:
	void	handle_resetButton	();
	void	handle_upButton		();
	void	select_sub		(QModelIndex ind);
};

