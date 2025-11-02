#
/*
 *    Copyright (C) 2025
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

#include	"device-handler.h"
#include	<QLabel>
#include	<QListView>
#include	<QStringListModel>
#include	<QSettings>
#include	<QString>
#include	<QStringList>
#include	<vector>
class		logger;
class		errorLogger;

class	deviceChooser: public QWidget {
Q_OBJECT
public:
			deviceChooser	(errorLogger *, QSettings *);
			~deviceChooser	();
	int		getDeviceIndex	(const QString &);	
	deviceHandler	*createDevice	(const QString &, const QString &);
private:
	void		addtoList	(const QString &);
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
	errorLogger	*theErrorLogger;
	std::vector<deviceItem> deviceList;
	QSettings	*dabSettings;

	QListView       *selectorDisplay;
        QStringListModel theDevices;
        QStringList     Devices; 

private slots:
	void		select_device	(QModelIndex);
signals:
	void		deviceSelected	(const QString &);
};

