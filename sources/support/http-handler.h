#
/*
 *	Copyright (C) 2025
 *	Jan van Katwijk (J.vanKatwijk@gmail.com)
 *	Lazy Chair Computing
 *
 *	This file is part of Qt-DAB
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
#include	<atomic>
#include	<string>
#include	<vector>
#include	<complex>
#include	<mutex>
#include	<QString>
#include	<QSettings>
#include	"tii-mapper.h"
#include	"db-element.h"
#include	<QTcpSocket>
#include	<QTcpServer>
class	RadioInterface;
//

class	httpHandler: public QTcpServer {
Q_OBJECT
public:
		httpHandler	(RadioInterface *,
	                         position	address,
	                         const QString	&saveName,
	                         bool		autoBrowse,
	                         QSettings	*settings = nullptr);
		~httpHandler	();
	void	putData		(uint8_t, position);
	void	putData		(uint8_t	type,
	                         transmitter	&Tr,
	                         const QString & theTime);
private:
	QSettings		*dabSettings;
	FILE			*saveFile;
	RadioInterface		*parent;
	int			mapPort;
	position		homeAddress;
	std::vector<transmitter> transmitterVector;

#ifdef	__MINGW32__
	std::wstring	browserAddress;
#else
	std::string	browserAddress;
#endif
	std::atomic<bool>	running;
	QString		theMap		(position address);
	QString		coordinatesToJson (transmitter &t);
	std::vector<transmitter> transmitterList;
	std::mutex	locker;
	bool		autoBrowser_off;
	bool		runs;
	bool		closing;
signals:
	void		setChannel	(const QString &);
	void		mapClose_processed	();
private slots:
	void		newConnection	();
	void		readData	();
};

