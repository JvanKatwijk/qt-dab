#
/*
 *    Copyright (C) 2013, 2014, 2015, 2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB .
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
#ifndef	__MAPPORT_HANDLER_H
#define	__MAPPORT_HANDLER_H
#include	<QDialog>
#include	<QLabel>
#include	<QPushButton>
#include	<QLineEdit>

class	QSettings;

class	mapPortHandler: public QDialog {
Q_OBJECT
	public:
		mapPortHandler	(QSettings *);
		~mapPortHandler	();
	private:
	QSettings	*dabSettings;
	QLabel		*mapPortText;
        QLineEdit	*mapPortSetting;
	QPushButton	*acceptButton;
private slots:
	void		handle_acceptButton 	();
};

#endif


