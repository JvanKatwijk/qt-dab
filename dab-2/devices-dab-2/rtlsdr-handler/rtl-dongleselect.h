#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of dab-2
 *
 *    dab-2 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    dab-2 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dab-2; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#ifndef	__DONGLE_SELECT__
#define	__DONGLE_SELECT__
#
#include	<QDialog>
#include	<QLabel>
#include	<QListView>
#include	<QStringListModel>
#include	<QStringList>
#include	<cstdint>

class	rtl_dongleSelect: public QDialog {
Q_OBJECT
public:
			rtl_dongleSelect();
			~rtl_dongleSelect();
	void		addtoDongleList		(const char *);
private:
	QLabel		*toptext;
	QListView	*selectorDisplay;
	QStringListModel	dongleList;
	QStringList	Dongles;
	int16_t		selectedItem;
private slots:
	void		selectDongle	(QModelIndex);
};

#endif

