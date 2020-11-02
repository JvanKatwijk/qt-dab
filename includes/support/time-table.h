#
/*
 *    Copyright (C) 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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

#ifndef	__TIMETABLE_HANDLER__
#define	__TIMETABLE_HANDLER__

#include	"radio.h"
#include	<QListView>
#include	<QStringList>
#include	<QStringListModel>
#include	<QModelIndex>

class	RadioInterface;

class	timeTableHandler:public QListView {
Q_OBJECT
public:
		timeTableHandler	(RadioInterface *radio);
		~timeTableHandler	();
	void	addElement		(int, const QString &);
	void	clear			();
private:
	QStringList		timeTableList;
	QStringListModel	displayList;
	RadioInterface	*radio;
};

#endif

