#
/*
 *	Copyright (C) 2026
 *	Jan van Katwijk (J.vanKatwijk@gmail.com)
 *	Lazy Chair Computing
 *
 *	This file is part of Qt-DAB
 *
 *    Qt-DAB is free software; you can redistribute it
 *    and/or modify it under the terms of the GNU General
 *    Public License as published by the Free Software Foundation,
 *    version 2 of the License.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB interface; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#pragma once

#include	<QObject>
#include	<QTimer>
#include	<QUrl>
#include	<QtWebSockets/QWebSocket>
#include	<QAbstractSocket>
#include	<QString>
#include	<QSettings>
#include	<atomic>
#include	<string>
#include	<vector>
#include	<complex>
#include	<mutex>
#include	"ringbuffer.h"
//
//	Lowest level, connecting reading and writing raw data
//	from and to an SDRconnect instance
class	socketHandler: public QObject {
Q_OBJECT
public:
		socketHandler	(const QString &hostAddress,
	                         int		portNumber,
	                         RingBuffer<std::complex<int16_t>> *);
		~socketHandler	();

	void		sendMessage	(const QString &);
private:
	QString		hostAddress;
	int		portNumber;
	QWebSocket	*socket;
	bool		connected;
	RingBuffer<std::complex<int16_t>> *_I_Buffer;
public slots:
	void		onConnected		();	
	void		onDisconnect		();
	void		onSocketError 		(QAbstractSocket::SocketError); 
	void		binaryMessageReceived	(const QByteArray &);
	void		textMessageReceived	(const QString &);
signals:
	void		dispatchMessage		(const QString &);
	void		binDataAvailable	();
	void		reportConnect		();
	void		reportDisconnect	();
};
