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
#include	<QTimer>
#include	"tii-mapper.h"
#include	"db-element.h"
#include	<QTcpSocket>
#include	<QTcpServer>
#include	<QAbstractSocket>
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
	                         QSettings	*settings);
		~httpHandler	();
	bool	isConnected	();
	void	putData		(uint8_t	type);
	void	putData		(uint8_t	type,
	                         transmitter	&Tr,
	                         const QString & theTime);
private:
	QTimer			delayTimer;
	RadioInterface		*theRadio;
	QString			fileName;
	position		homeAddress;
	bool			close_map_on_exit;
	QString			theMap		(const QString &fileName,
	                                         position address);
	QString			transmitterToJsonObject (transmitter &t);
	std::vector<transmitter> transmitterList;
	std::mutex		locker;
	bool			connection_stopped;
	std::atomic<bool>	closingInProgress;
	int			maxDelay;
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
