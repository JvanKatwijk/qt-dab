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
#include	<QString>
#include	<QSettings>
#include	<QTimer>
#include	<QTcpSocket>
#include	<QTcpServer>
#include	<QAbstractSocket>
#include	<QDomDocument>
#include	<QFile>
#include	<atomic>
#include	<string>
#include	<vector>
#include	<complex>
#include	<mutex>
#include	"tii-mapper.h"
#include	"db-element.h"
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<sys/types.h>

class	RadioInterface;
//

class	httpHandler: public QTcpServer {
Q_OBJECT
public:
		httpHandler	(RadioInterface *,
	                         const QString &fileName,
	                         position	address,
	                         bool		autoBrowse,
	                         bool		map_close_on_exit,
	                         const QString &,
	                         QSettings	*settings);
		~httpHandler	();
	bool	isConnected	();
	void	putData		(uint8_t	type);
	void	putData		(uint8_t	type,
	                         transmitter	&Tr,
	                         bool		utc);
private:
	QTimer			delayTimer;
	RadioInterface		*theRadio;
	QString			nameOfMap;
	QString			saveName;
	QSettings		*dabSettings;
	position		homeAddress;
	bool			close_map_on_exit;
	QString			theMap		(const QString &fileName,
	                                         position address);
	QByteArray		transmitterToJsonObject (transmitter &t);
	std::vector<transmitter> transmitterList;
	std::mutex		locker;
	bool			connection_stopped;
	std::atomic<bool>	closingInProgress;
	int			maxDelay;
	std::vector<transmitter> saveStack;
	bool			seenAlready	(const transmitter &);
	QDomElement		getTransmitter	(QDomDocument &,
	                                           const transmitter &tr);

	void			saveMap		();
signals:
	void		setChannel		(const QString &);
	void		mapClose_processed	();
private slots:
	void		newConnection		();
	void		readSocket		();
	void		discardSocket		();
	void		handle_timeOut		();
	void		onSocketError 		(QAbstractSocket::SocketError); 
};
