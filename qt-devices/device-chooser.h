#
/*
 *    Copyright (C) 2016 .. 2024
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
 *    along with Qt-DAB-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#pragma once

#include	"device-handler.h"
#include	<QSettings>
#include	<QString>
#include	<QStringList>
#include	<vector>

class	deviceChooser final {
public:
			deviceChooser	(QSettings *);
			~deviceChooser	();
	QStringList	getDeviceList	();
	int		getDeviceIndex	(const QString &);	
	deviceHandler	*createDevice	(const QString &, const QString &);
private:

	QString		getFileName	(uint8_t &);
	deviceHandler	*_createDevice	(const QString &, const QString &);
	class	deviceItem {
	public:
		QString	deviceName;
		int deviceNumber;
		deviceItem	(const QString &s, int n) {
	   	   deviceName	= s;
	   	   deviceNumber	= n;
		};
		~deviceItem	() {};
	};
	std::vector<deviceItem> deviceList;
	QSettings	*dabSettings;
};

