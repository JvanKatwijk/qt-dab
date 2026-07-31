#
/*
 *    Copyright (C)  2016, 2026
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

#pragma once

#include	<QObject>
#include	<QString>
#include	<QList>
#include	<vector>
#include	"service-descriptor.h"
#include	"channel-descriptor.h"

class	serviceBase: public QObject {
Q_OBJECT
public:
		serviceBase	();
		~serviceBase	();
	void	load		(const QString &fileName, bool withPackets);
	void	store		(const QString &fileName);
	void	reset		();
	void	set_ensembleName	(const QString &, const QString &);
	void	set_countryName	(const QString &, const QString &);
	void	add		(const serviceDescriptor &);
	void	remove		(const QString &, const QString &);
QList <serviceDescriptor> 
		getData		(int, int);	// the Mode, the order
QList<serviceDescriptor> 
		getData		(int, int, const QString &);	// the Mode, the order
	void	update		(const serviceDescriptor &, bool);
	void	insert		(const theChannel &);
	QList<theChannel> theData;
private:
	void	clearTable	();
	bool 	changed;
	QString	dbName;
};

