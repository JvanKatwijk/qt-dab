#
/*
 *	Copyright (C) 2026
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

#include	<QByteArray>

#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<sys/types.h>
#include	<cstring>

#include	"socket-handler.h"
//
//	The lowest level
	socketHandler::
	            socketHandler (const QString &hostAddress,
	                          int	portNumber, 
	                          RingBuffer<std::complex<int16_t>> *b) {
	this	-> hostAddress	= hostAddress;
	this	-> portNumber	= portNumber;
	this	-> _I_Buffer	= b;
	this	-> socket	= new QWebSocket;
	connect (socket, &QWebSocket::connected,
	        this, &socketHandler::onConnected);
	connect (socket, &QWebSocket::disconnected,
	         this, &socketHandler::onDisconnect);
	connect (socket, &QWebSocket::errorOccurred,
                        this, &socketHandler::onSocketError);
	connected	= false;
	QString urlString = "ws://%1:%2";
	socket -> open (QUrl (urlString. arg (hostAddress). arg(QString::number (portNumber))));
	dropCount	= 0;
}

	socketHandler::~socketHandler	() {
	if (socket != nullptr)
	   delete socket;
}

void	socketHandler::onConnected	() {
	connected	= true;
	connect (socket, &QWebSocket::textFrameReceived,
                 this, &socketHandler::textMessageReceived);
	connect (socket, &QWebSocket::textMessageReceived,
                 this, &socketHandler::textMessageReceived);
	connect (socket, &QWebSocket::binaryFrameReceived,
                 this, &socketHandler::binaryMessageReceived);
	emit reportConnect ();
}

void	socketHandler::onDisconnect	() {
	if (socket != nullptr)
	   socket	-> deleteLater ();
	emit reportDisconnect ();
//	socket	= nullptr;
}
	
void	socketHandler::onSocketError	(QAbstractSocket::SocketError error) {
	(void)error;
	onDisconnect ();
	emit reportDisconnect ();
}

void	socketHandler::sendMessage	(const QJsonObject &json) {
	if (socket == nullptr)
	   return;
	QJsonDocument doc (json);
	QByteArray ba	= doc. toJson (QJsonDocument::Compact);
	socket -> sendTextMessage (QString (ba));
}

void	socketHandler::binaryMessageReceived	(const QByteArray &m) {
int16_t *p	= (int16_t *)(m. data ());
	if (p [0] != 2)
	   return;
	int written = _I_Buffer -> putDataIntoBuffer (&(p [1]), (m. size () - 1)/ 4);
	if ((++dropCount % 100) == 0) {
	   int dropped = (m. size () - 1) / 4 - written;
           reportStatus (dropped);
	}
	emit binDataAvailable ();
}

void	socketHandler::textMessageReceived	(const QString &m) {
	dispatchMessage (m);
}

