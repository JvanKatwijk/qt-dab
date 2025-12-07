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

#include	<QFile>
#include	<QTextStream>
#include	<QDataStream>
#include	<QByteArray>
#include	<QDesktopServices>
#include	<QMessageBox>

#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<sys/types.h>
#include	"distances.h"
#include	<cstring>

#include	"settings-handler.h"
#include	"settingNames.h"
#include	"device-exceptions.h"
#include	"http-handler.h"
#include	"radio.h"

	httpHandler::httpHandler (RadioInterface *theRadio,
	                          const QString &fileName,
	                          position	homeAddress,
	                          bool		autoBrowser_on,
	                          QSettings	*settings) {
	this	-> theRadio	= theRadio;
	this	-> fileName	= fileName;
	int  mapPort		=
	            value_i (settings, MAP_HANDLING, MAP_PORT_SETTING,
                                                                  8080);
	QString address		= 
	            value_s (settings, MAP_HANDLING, BROWSER_ADDRESS,
	                                                "http://localhost");
	QString browserAddress		= address + ":" + QString::number (mapPort);
	this	-> homeAddress		= homeAddress;
	this	-> maxDelay		=
	            value_i (settings, MAP_HANDLING, MAP_TIMEOUT, 3000);

	delayTimer. setSingleShot (true);
	connect (this, &httpHandler::setChannel,
	         theRadio, &RadioInterface::channelSignal);
	connect (this, &QTcpServer::newConnection,
	         this, &httpHandler::newConnection);
	connect (&delayTimer, &QTimer::timeout,
	         this, &httpHandler::handle_timeOut);

	if (!autoBrowser_on) {
	   if (!QDesktopServices::openUrl(QUrl (browserAddress))) {
	      fprintf (stderr, "cannot open URL\n");
	      throw device_exception ("cannot open URL");
	      return;
	   }
	}

	if (!this -> listen (QHostAddress::Any, mapPort)) {
	   throw device_exception ("listening to port failed");
	   return;
	}

	transmitterList. resize (0);
	connection_stopped	= false;
	closingLevel		= 0;
}

	httpHandler::~httpHandler	() {
	if (this -> isListening ())
	   this -> close ();
}

void	httpHandler::newConnection	() {
	if (this -> hasPendingConnections ()) {
	   QTcpSocket *socket = this -> nextPendingConnection ();
	   connect (socket, &QTcpSocket::readyRead,
	            this, &httpHandler::readSocket);
	   connect (socket, &QAbstractSocket::disconnected,
	            this, &httpHandler::discardSocket);
	   connect  (socket, &QTcpSocket::errorOccurred,
	             this, &httpHandler::onSocketError);
	}
}
//
//	There are three possibilities here
//	a. connection will be restored and we go on
//	b. the issues a MAP_CLOSE and are now ready to give up
//	c. if the map wa skilled, the timer will go off and we give up
void	httpHandler::discardSocket () {
QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(sender());
	if (closingLevel >= 2) {
	   delayTimer. stop ();
	   disconnect (socket, &QTcpSocket::readyRead,
                       this, &httpHandler::readSocket);
	   disconnect  (socket, &QTcpSocket::errorOccurred,
	                this, &httpHandler::onSocketError);
	   socket	-> close ();
	   socket	= nullptr;
	   connect (this, &httpHandler::mapClose_processed,
	            theRadio, &RadioInterface::http_terminate);
	   emit mapClose_processed ();
	}
	else {
	   socket -> deleteLater ();
	   delayTimer. start (maxDelay);
	}
}

void	httpHandler::onSocketError (QAbstractSocket::SocketError socketerror) {
QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(sender());
	(void)socketerror;
	fprintf (stderr, "error %s\n",
	              socket -> errorString (). toLatin1 (). data ());
}
//
bool	httpHandler::isConnected () {
	return !connection_stopped;
}

void	httpHandler::readSocket () {
QTcpSocket *worker = qobject_cast<QTcpSocket *> (sender ());
	if (worker == nullptr)
	   return;
	delayTimer. stop ();	// it seems we are reconnected
	if (closingLevel > 0)
	   return;
	closingLevel		= 0;
	bool	keepAlive	= false;
	QByteArray data		= worker -> readAll ();
	QString request		= QString (data);
	int version		= request. contains ("HTTP/1.1") ? 11 : 10;
	keepAlive	= (version == 11) ? 
	                     !request. contains ("Connection: close"):
	                     request. contains ("Connection: keep-alive");
//	Identify the URL
	QStringList list	= request. split (" ");
	if (list. size () < 2) 
	   return;
	QString askingFor	= list [1];
	QString content;
	QString ctype;
//	fprintf (stderr, "askingFor = %s\n", theQuestion. toLatin1 (). data ());
	if (askingFor == "/data.json") {
	   locker. lock ();
	   if (transmitterList. size () > 0) { 
	      transmitter t	= transmitterList [0];
	      content		= transmitterToJsonObject (t);
	      transmitterList. erase (transmitterList. begin ());
	      if (t. type == MAP_CLOSE) {
	         closingLevel = 1;
	      }
	   }
	   locker. unlock ();
	   if (content != "") {
	      ctype       = "application/json;charset=utf-8";
	   }
	}
	else	
	if (askingFor == "/channelSelector::") {
           setChannel (list [2]);
        }
        else {
           content	= theMap (fileName, homeAddress);
           ctype	= "text/html;charset=utf-8";
        }
	QByteArray theContents = content. toUtf8 ();

//	Create the header
	char hdr [2048];
	sprintf (hdr,
	      "HTTP/1.1 200 OK\r\n"
	      "Server: qt-dab\r\n"
	      "Content-Type: %s\r\n"
	      "Connection: %s\r\n"
	      "Content-Length: %d\r\n"
	      "\r\n",
	      ctype. toLatin1 (). data (),
	      keepAlive ? "keep-alive" : "close",
	      (int)(theContents. size ()));
	QByteArray theData = hdr;
	(void)worker -> write (theData);
	(void)worker -> write (theContents);

	if (closingLevel > 0) {
	   worker -> waitForBytesWritten ();
	   closingLevel = 2;
	}
	if (!keepAlive || (closingLevel > 0))
	   worker -> disconnectFromHost ();
}
//
//	This timeout is initiated after a disconnect
void	httpHandler::handle_timeOut () {
	connection_stopped	= true;
	connect (this, &httpHandler::mapClose_processed,
	         theRadio, &RadioInterface::cleanUp_mapHandler);
	emit mapClose_processed ();
}

QString	httpHandler::theMap (const QString &fileName, position homeAddress) {
int	bodySize;
char	*body;
std::string latitude	= std::to_string (homeAddress. latitude);
std::string longitude	= std::to_string (homeAddress. longitude);
int	index		= 0;
int	cc;
int teller	= 0;
int params	= 0;

// read map file from resource file
	QFile file (fileName);
	if (file. open (QFile::ReadOnly)) {
	   QByteArray record_data (1, 0);
	   QDataStream in (&file);
	   bodySize	= file. size ();
	   body		=  (char *)malloc (bodySize + 40);
	   while (!in. atEnd ()) {
	      in . readRawData (record_data. data (), 1);	
	      cc = (*record_data. constData ());
	      if (cc == '$') {
	         if (params == 0) {
	            for (int i = 0; latitude. c_str () [i] != 0; i ++)
	               if (latitude. c_str () [i] == ',')
	                  body [teller ++] = '.';
	               else
	                  body [teller ++] = latitude. c_str () [i];
	            params ++;
	         }
	         else
	         if (params == 1) {
	            for (int i = 0; longitude. c_str () [i] != 0; i ++)
	               if (longitude. c_str () [i] == ',')
	                  body [teller ++] = '.';
	               else
	                  body [teller ++] = longitude. c_str () [i];
	            params ++;
	         }
	         else
	            body [teller ++] = (char)cc;
	      }
	      else
	         body [teller ++] = (char)cc;
	      index ++;
	   }
	   body [teller ++] = 0;
	}
	else {
	   fprintf (stderr, "cannot open file\n");
	   return "";
	}

	body [teller ++] = 0;
	QString	res	= QString (body);
//	fprintf (stderr, "The map :\n%s\n", res. c_str ());
	file. close ();
	free (body);
	return res;
}

std::string dotNumber (float f) {
char temp [256];
std::string s = std::to_string (f);
	for (int i = 0; i < (int)(s. size ()); i ++)
	   if (s. c_str () [i] == ',')
	      temp [i] = '.';
	   else
	      temp [i] = s. c_str () [i];
	temp [s. size ()] = 0;
	return std::string (temp);
}
//
QString httpHandler::transmitterToJsonObject (transmitter &t) { 
char buf [512];
QString Jsontxt;

	Jsontxt += "[\n";
	QString direction	= t. direction;
	if (direction. size () < 3)
	   direction = "??";
	QString polarization	= t. polarization;
	if (polarization. size () == 0)
	   polarization = "??";
//	the Target
	snprintf (buf, 512,
	      "{\"type\":%d, \"ensemble\":\"%s\", \"lat\":%s, \"lon\":%s, \"name\":\"%s\", \"channel\":\"%s\", \"dateTime\":\"%s\", \"tiiValue\":%d, \"snr\":%f,  \"dist\":%d, \"azimuth\":%d, \"power\":%d,\"altitude\":%d, \"height\":%d, \"direction\":\"%s\", \"polarisation\":\"%s\"}",
	       t. type,
	       t. ensemble. toUtf8 (). data (),
	       dotNumber (t. latitude). c_str (),
	       dotNumber (t. longitude). c_str (),
	       t. transmitterName. toUtf8 (). data (),
	       t. channel. toUtf8 (). data (),
	       t. dateTime. toUtf8 (). data (),
	       (t. mainId << 8) | (t. subId),
	       10 * log10 (t. strength + 0.01),
	       (int)(t. distance),
	       (int)(t. azimuth),
	       (int)(t. power * 100),
	       (int)(t. altitude),
	       (int)(t. height),
	       direction. toUtf8 (). data (),
	       polarization. toUtf8 (). data ());

	Jsontxt += QString (buf);
	Jsontxt += "\n]\n";
	return Jsontxt;
}
//
//	For "special" keys we use 
void	httpHandler::putData	(uint8_t type) {

	transmitter t;
	t. type			= type;
	t. dateTime		= "";
	locker. lock ();
	if ((type == MAP_RESET) || (type == MAP_CLOSE))
	   transmitterList. resize (0);
	transmitterList. push_back (t);
	locker. unlock ();
}

void	httpHandler::putData	(uint8_t	type,
	                         transmitter	&theTr,
	                         const QString	&theTime) {
	float latitude	= theTr. latitude;
	float longitude	= theTr. longitude;
	locker. lock ();
	for (unsigned long i = 0; i < transmitterList. size (); i ++)
	   if ((transmitterList [i]. latitude == latitude) &&
	       (transmitterList [i]. longitude == longitude ) &&
	       (transmitterList [i]. type == type)) {
	      locker. unlock ();
	      return;
	   }

	theTr. type		= type;
	theTr. dateTime		= theTime;

	transmitterList. push_back (theTr);
	locker. unlock ();
}

