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
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<sys/types.h>
#include	"distances.h"
#include	<QDesktopServices>
#include	<cstring>

#include	"settings-handler.h"
#include	"settingNames.h"
#include	"http-handler.h"
#include	"radio.h"

	httpHandler::httpHandler (RadioInterface *parent,
	                          position	 homeAddress,
	                          const QString &saveName,
	                          bool		autoBrowser_off,
	                          QSettings	*settings) {
	this	-> parent	= parent;
	this	-> mapPort			=
	            value_i (settings, MAP_HANDLING, MAP_PORT_SETTING,
                                                                  8080);
	QString address		= 
	            value_s (settings, MAP_HANDLING, BROWSER_ADDRESS,
	                                                "http://localhost");

	QString temp		= address + ":" + QString::number (8080);
	this	-> homeAddress	= homeAddress;
	this	-> autoBrowser_off	= autoBrowser_off;
#ifdef	__MINGW32__
	this	-> browserAddress	= temp. toStdWString ();
#else
	this	-> browserAddress	= temp. toStdString ();;
#endif
	this	-> running. store (false);
	this	-> dabSettings		= settings;
	saveFile	= fopen (saveName. toUtf8 (). data (), "w");
	if (saveFile != nullptr) {
	   fprintf (saveFile, "Home location; %f; %f\n\n", 
	                         homeAddress. latitude, homeAddress. longitude);
	   fprintf (saveFile, "channel; latitude; longitude;transmitter;date and time; mainId; subId; distance; azimuth; power, altitude, height, dir\n\n");
	}

	connect (this, &httpHandler::setChannel,
	         parent, &RadioInterface::channelSignal);
	transmitterVector. resize (0);
	closing	= false;
	my_tcpServer	= new QTcpServer	(this);
	connect (my_tcpServer, &QTcpServer::newConnection,
	         this, &httpHandler::newConnection);

	
	if (!QDesktopServices::openUrl(QUrl (QString::fromStdString (browserAddress)))) {
	   fprintf (stderr, "cannot open URL\n");
	   throw (22);
	   return;
	}

	if (!my_tcpServer -> listen (QHostAddress::Any, 8080)) {
	   fprintf (stderr, "listen error, fatal\n");
	   throw (21);
	   return;
	}
}

	httpHandler::~httpHandler	() {
	if (my_tcpServer -> isListening ())
	   my_tcpServer -> close ();
	delete my_tcpServer;
}

void	httpHandler::newConnection	() {
	while (my_tcpServer -> hasPendingConnections ()) {
	   QTcpSocket *client = my_tcpServer -> nextPendingConnection ();
	   connect (client, &QTcpSocket::readyRead,
	            this, &httpHandler::readData);
	   connect (client, &QTcpSocket::disconnected,
	            &QTcpSocket::deleteLater);
	}
}

void	httpHandler::readData () {
QTcpSocket *worker = qobject_cast<QTcpSocket *> (sender ());
	if (worker == nullptr)
	   return;

	QByteArray data	= worker -> readAll ();
	QString request	= QString (data);
	int version	= request. contains ("HTTP/1.1") ? 11 : 10;
	int keepAlive	= (version == 11) ? 
	                     request. contains ("Connection: close"):
	                     request. contains ("Connection: keep-alive");
//
//	Identify the URL
	QStringList list	= request. split (" ");
	if (list. size () < 2)
	   return;
	QString theQuestion	= list [1];
	QString content;
	QString ctype;
	if (theQuestion == "/data.json") {
	   content = coordinatesToJson (transmitterList);
	   if (content != "") {
	      ctype       = "application/json;charset=utf-8";
	   }
	}
	else	
	if (theQuestion == "/channelSelector::") {
           setChannel (list [2]);
	   return;
        }
        else {
           content        = theMap (homeAddress);
           ctype          = "text/html;charset=utf-8";
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
	worker -> write (theData);
	worker -> write (theContents);
}

QString	httpHandler::theMap (position homeAddress) {
int	bodySize;
char	*body;
std::string latitude	= std::to_string (homeAddress. latitude);
std::string longitude	= std::to_string (homeAddress. longitude);
int	index		= 0;
int	cc;
int teller	= 0;
int params	= 0;

	
// read map file from resource file
	QFile file (":res/qt-map-69.html");
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
QString httpHandler::coordinatesToJson (std::vector<transmitter> &t) { 
char buf [512];
QString Jsontxt;

	if (t. size () == 0)
	   return "";
	Jsontxt += "[\n";
	locker. lock ();
	QString direction	= t [0]. direction;
	if (direction. size () < 3)
	   direction = "??";
	QString polarization	= t [0]. polarization;
	if (polarization. size () == 0)
	   polarization = "??";
//	the Target
	snprintf (buf, 512,
	      "{\"type\":%d, \"ensemble\":\"%s\", \"lat\":%s, \"lon\":%s, \"name\":\"%s\", \"channel\":\"%s\", \"dateTime\":\"%s\", \"tiiValue\":%d, \"snr\":%f,  \"dist\":%d, \"azimuth\":%d, \"power\":%d,\"altitude\":%d, \"height\":%d, \"direction\":\"%s\", \"polarisation\":\"%s\"}",
	       t [0]. type,
	       t [0]. ensemble. toUtf8 (). data (),
	       dotNumber (t [0]. latitude). c_str (),
	       dotNumber (t [0]. longitude). c_str (),
	       t [0]. transmitterName. toUtf8 (). data (),
	       t [0]. channel. toUtf8 (). data (),
	       t [0]. dateTime. toUtf8 (). data (),
	       (t [0]. mainId << 8) | (t [0]. subId),
	       10 * log10 (t [0]. strength + 0.01),
	       (int)(t [0]. distance),
	       (int)(t [0]. azimuth),
	       (int)(t [0]. power * 100),
	       (int)(t [0]. altitude),
	       (int)(t [0]. height),
	       direction. toUtf8 (). data (),
	       polarization. toUtf8 (). data ());

	Jsontxt += QString (buf);
//
	transmitterList. erase (transmitterList. begin ());
	locker. unlock ();
	Jsontxt += "\n]\n";
//	fprintf (stderr, "Json = %s\n", Jsontxt. toLatin1 (). data ());
	return Jsontxt;
}

void	httpHandler::putData	(uint8_t type,
	                         position target) {
	for (unsigned long i = 0; i < transmitterList. size (); i ++)
	   if ((transmitterList [i]. latitude == target. latitude) &&
	       (transmitterList [i]. longitude == target. longitude))
	      return;
	   
	transmitter t;
	t. type			= type;
	t. country		= "unknown";
	t. channel		= "unknown";
	t. ensemble		= "unknown";
	t. Eid			= 0;
	t. mainId		= 0;
	t. subId		= 0;
	t. transmitterName	= "not in database";
	t. altitude		= 0;
	t. height		= 0;
	t. latitude		= target. latitude;
	t. longitude		= target. longitude;
	t. polarization		= "";
	t. power		= 0;
	t. direction		= "";

	t. strength		= 0;
	t. distance		= 0;
	t. azimuth		= 0;
	t. dateTime		= "";
	locker. lock ();
	if (type == MAP_RESET)
	   transmitterList. resize (0);
	transmitterList. push_back (t);
	locker. unlock ();
	if (type == MAP_CLOSE)
	   closing	= true;
}

void	httpHandler::putData	(uint8_t	type,
	                         transmitter	&theTr,
	                         const QString	&theTime) {
//	                         const QString	&channelName,
//	                         float	 snr) {
	if (closing)
	   return;
	float latitude	= theTr. latitude;
	float longitude	= theTr. longitude;
	for (unsigned long i = 0; i < transmitterList. size (); i ++)
	   if ((transmitterList [i]. latitude == latitude) &&
	       (transmitterList [i]. longitude == longitude ) &&
	       (transmitterList [i]. type == type))
	      return;

	theTr. type		= type;
	theTr. dateTime		= theTime;
//	theTr. strength		= snr;

	locker. lock ();
	transmitterList. push_back (theTr);
//	transmitterList. push_back (t);
	locker. unlock ();

	for (int i = 0; i < (int)(transmitterVector. size ()); i ++) {
	   if ((transmitterVector. at (i). transmitterName ==
	               theTr. transmitterName) &&
	       (transmitterVector. at (i). channel ==
	               theTr. channel) &&
	       (transmitterVector. at (i). type ==
	                         theTr. type)) {
	      return;
	   }
	}
	
	if ((saveFile != nullptr)  &&
	           ((type != MAP_RESET) && (type != MAP_FRAME))) {
	   fprintf (saveFile, "%s; %f; %f; %s; %s; %d; %d; %d; %d; %f, %d, %d, %s\n",
	                      theTr. channel. toUtf8 (). data (),
	                      theTr. latitude,
	                      theTr. longitude,
	                      theTr. transmitterName. toUtf8 (). data (),
	                      theTr. dateTime. toUtf8 (). data (),
	                      theTr. mainId,
	                      theTr. subId,
	                      (int)(theTr. distance),
	                      (int)(theTr. azimuth),
	                      theTr. power,
	                      (int)(theTr. altitude),
	                      (int)(theTr. height),
	                      theTr. direction. toUtf8 (). data ());
	   transmitterVector. push_back (theTr);
	}
}

