#
/*
 *    Copyright (C) 2021
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
 *    along with dab-scanner; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef	__CONTENT_TABLE_H
#define	__CONTENT_TABLE_H

#include	<QWidget>
#include	<QObject>
#include	<QScrollArea>
#include	<QTableWidget>
#include	<QStringList>
#include	<QTableWidgetItem>
#include	<QObject>
#include	<QString>
#include	<QByteArray>

class	RadioInterface;
class	QSettings;
class	audiodata;

class	contentTable: public QObject {
Q_OBJECT
public:
		contentTable	(RadioInterface *, QSettings *,
	                            const QString &, int);
		~contentTable	();
	void	show		();
	void	hide		();
	bool	isVisible	();
	void	clearTable	();
	void	addLine		(const QString &);
	void	dump		(FILE *);
private:
	QString		channel;
	int		columns;
	RadioInterface	*theRadio;
	QSettings	*dabSettings;
	QScrollArea	*myWidget;
	QTableWidget	*contentWidget;
	int16_t		addRow	();
	bool		is_clear;
private slots:
	void		selectService	(int, int);
	void		dump		(int, int);
signals:
	void		goService	(const QString &);
};

#endif
