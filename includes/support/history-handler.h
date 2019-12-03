#
/*
 *    Copyright (C) 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the  qt-dab program
 *
 *    qt-dab is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    qt-dab is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef	__HISTORY_HANDLER__
#define	__HISTORY_HANDLER__

#include	"radio.h"
#include	<QListView>
#include	<QStringList>
#include	<QStringListModel>
#include	<QModelIndex>

class	RadioInterface;
class	descriptorType;

class	historyHandler:public QListView {
Q_OBJECT
public:
		historyHandler	(RadioInterface *radio,
	                                 QString fileName);
		~historyHandler	();
	void	addElement	(const QString &, const QString &);
	void	clearHistory	();
public slots:
	void	selectElement	(QModelIndex);
signals:
	void	handle_historySelect (const QString &);
private:
	QStringList	historyList;
	QStringListModel	displayList;
	RadioInterface	*radio;
	QString		fileName;
};

#endif

