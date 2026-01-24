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
#include	<QDateTime>

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
	                          bool		close_map_on_exit,
	                          const QString &saveName,
	                          QSettings	*settings) {
	this	-> theRadio	= theRadio;
	this	-> nameOfMap	= fileName;
	this	-> close_map_on_exit = close_map_on_exit;
	this	-> saveName	= saveName;
	this	-> dabSettings	= settings;
	int  mapPort		=
	            value_i (settings, MAP_HANDLING, MAP_PORT_SETTING,
                                                                  8080);
	QString address		= 
	            value_s (settings, MAP_HANDLING, BROWSER_ADDRESS,
	                                                "http://localhost");
	QString browserAddress		= address + ":" +
	                                          QString::number (mapPort);
	this	-> homeAddress		= homeAddress;

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
	closingInProgress. store (false);
}

	httpHandler::~httpHandler	() {
	if (this -> isListening ())
	   this -> close ();
	if (this	-> saveName != "")
	   saveMap	();
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
void	httpHandler::discardSocket () {
QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(sender());
	socket -> deleteLater ();
}
//
//	If "closingInProgress" is true, the user has indicated that the
//	http driver is to be stopped, with as secondary effect that the
//	browser stops displaying the map and issues an error 1
//	Otherwise, the user killed the browser and the
//	http Handler is killed if the close_map_on_exit is true
//	Note that the function is a "slot", issuing a signal
//	that directly kills the httpHandler will crash the system
void	httpHandler::onSocketError (QAbstractSocket::SocketError socketerror) {
//QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(sender());
	if (socketerror == QAbstractSocket::RemoteHostClosedError) {
	   if (closingInProgress. load ()) {	// reacting on button switch
	      connect (this, &httpHandler::mapClose_processed,
	               theRadio, &RadioInterface::http_terminate);
//	      fprintf (stderr, "Going to close a map\n");
	      emit mapClose_processed ( );
	   }
	   else {	// 
//	      fprintf (stderr, "de http handler zou moeten sluiten\n");
	      if (close_map_on_exit)
	         if (!delayTimer. isActive ())
	            delayTimer. start (1000);
	   }
	}
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
	if (closingInProgress. load ())
	   return;
//	closingInProgress. store (false);
	QByteArray data		= worker -> readAll ();
	QString request		= QString (data);
	int version		= request. contains ("HTTP/1.1") ? 11 : 10;
	bool keepAlive	= (version == 11) ? 
	                     !request. contains ("Connection: close"):
	                     request. contains ("Connection: keep-alive");
//	Identify the URL
	QStringList list	= request. split (" ");
	if (list. size () < 2) 
	   return;
	QString askingFor	= list [1];
	QString content;
	QString ctype;
	if (askingFor == "/data.json") {
	   locker. lock ();
	   if (transmitterList. size () > 0) { 
	      transmitter t	= transmitterList [0];
	      content		= transmitterToJsonObject (t);
	      transmitterList. erase (transmitterList. begin ());
	      if (t. type == MAP_CLOSE) {
	         closingInProgress. store (true);
	      }
	   }
	   locker. unlock ();
	   if (content != "") {
	      ctype       = "application/json;charset=utf-8";
	   }
	}
	else	
	if (askingFor. startsWith ("/channelSelector::")) {
	   QStringList s = askingFor. split ("::");
	   if (s. size () > 1)
              setChannel (s [1]);
        }
        else {
           content	= theMap (nameOfMap, homeAddress);
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
//
//	In case we have sent a close signal
	if (closingInProgress. load ()) {
	   worker -> waitForBytesWritten ();
	}
	delayTimer. start (4000);
}
//
//	This timeout is initiated after a (potential) disconnect
//	Under normal circumstances, after pushing the httpButton,
//	an error is signalled and the error function takes
//	care 
void	httpHandler::handle_timeOut () {
	if (!close_map_on_exit)
	   return;
	connection_stopped	= true;
	if (closingInProgress. load ())
	   connect (this, &httpHandler::mapClose_processed,
	            theRadio, &RadioInterface::http_terminate);
	else
	   connect (this, &httpHandler::mapClose_processed,
	            theRadio, &RadioInterface::cleanUp_mapHandler);
	emit mapClose_processed ();
}

QString	httpHandler::theMap (const QString &nameOfMap, position homeAddress) {
int	bodySize;
char	*body;
std::string latitude	= std::to_string (homeAddress. latitude);
std::string longitude	= std::to_string (homeAddress. longitude);
int	index		= 0;
int	cc;
int teller	= 0;
int params	= 0;

// read map file from resource file
	QFile file (nameOfMap);
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
	                         bool		utc) {
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
	QDateTime theTime	= utc ?  QDateTime::currentDateTimeUtc () :
                                             QDateTime::currentDateTime ();

	theTr. dateTime		= theTime. toString (Qt::TextDate);
	transmitterList. push_back (theTr);
	locker. unlock ();
	if (saveName != "") 
	   if (!seenAlready (theTr))
	      saveStack. push_back (theTr);
}

bool	httpHandler::seenAlready (const transmitter &tr) {
	for (auto &member: saveStack) 
	   if ((member. channel == tr. channel) &&
	       (member. transmitterName == tr. transmitterName))
	      return true;
	return false;
}

void	httpHandler::saveMap	() {
QDomDocument theDocument;
	QDomElement root	= theDocument. createElement ("mapView");
	QDateTime theTime		= QDateTime::currentDateTime ();
	theDocument. appendChild (root);
	QDomElement element = theDocument. createElement ("Creator");
	element. setAttribute ("generator", "Qt-DAB");
	element. setAttribute ("dateTime", keyText);
	element. setAttribute ("home-X", homeAddress. latitude);
	element. setAttribute ("home-Y", homeAddress. longitude);
	root. appendChild (element);

	for (auto &tr: saveStack) {
	   QDomElement transm = getTransmitter (theDocument, tr);
	   element. appendChild (transm);
	}

	QFile file (this -> saveName);
        if (!file. open (QIODevice::WriteOnly | QIODevice::Text))
           return;

        QTextStream stream (&file);
        stream << theDocument. toString ();
        file. close ();
}

QDomElement httpHandler::getTransmitter (QDomDocument &doc,
	                                        const transmitter &tr) {
QDomElement element = doc. createElement ("Transmitter");

	element. setAttribute ("country", tr. country);
	element. setAttribute ("tiiPattern", QString::number (tr. pattern, 8));
	element. setAttribute ("channel", tr. channel);
	element. setAttribute ("ensemble", tr. ensemble);
	element. setAttribute ("transmitterName", tr. transmitterName);
	element. setAttribute ("Eid", QString::number (tr. Eid, 16));
	element. setAttribute ("mainId", tr. mainId);
	element. setAttribute ("subId", tr. subId);
	element. setAttribute ("latitude", tr. latitude);
	element. setAttribute ("longitude", tr. longitude);
	element. setAttribute ("power", tr. power);
	element. setAttribute ("height", tr. height);
	element. setAttribute ("altitude", tr. altitude);
	element. setAttribute ("polarization", tr. polarization);
	element. setAttribute ("direction",  tr. direction);
	element. setAttribute ("strength", tr. strength);
	return element;
}

	
