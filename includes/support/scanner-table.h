#
/*
 *    Copyright (C) 2019
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
#ifndef	__SCANNER_TABLE__
#define	__SCANNER_TABLE__

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

class	scannerTable: QObject {
Q_OBJECT
public:
		scannerTable	(RadioInterface *);
		~scannerTable	();
	void	show		();
	void	hide		();
	void	newEnsemble	(QString, QString, QString,
	                                     QString, QString, QByteArray);
	void	new_headline	();
	void	add_to_Ensemble	(QString, QString, QString,
	                                     QString, QString, QString);
	void	clear		();
	void	dump		(const QString &);
private:
	QScrollArea	*myWidget;
	QTableWidget	*outputWidget;
	int16_t		addRow	();
	bool		is_clear;
};

#endif

