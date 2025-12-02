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
#include	"device-exceptions.h"
#include	"http-handler.h"
#include	"radio.h"

static bool stopLoading	= false;

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

	QString temp		= address + ":" + QString::number (mapPort);
	this	-> homeAddress	= homeAddress;
	this	-> autoBrowser_off	= autoBrowser_off;
#ifdef	__MINGW32__
	this	-> browserAddress	= temp. toStdWString ();
#else
	this	-> browserAddress	= temp. toStdString ();;
#endif
	this	-> running. store (false);
	this	-> dabSettings		= settings;

	transmitterVector. resize (0);
	connect (this, &QTcpServer::newConnection,
	         this, &httpHandler::newConnection);
	connect (this, &httpHandler::setChannel,
	         parent, &RadioInterface::channelSignal);

	if (!autoBrowser_off) {
	   if (!QDesktopServices::
	        openUrl(QUrl (QString::fromStdString (browserAddress)))) {
	      fprintf (stderr, "cannot open URL\n");
	      throw device_exception ("cannot open URL");
	      return;
	   }
	}

	if (!this -> listen (QHostAddress::Any, mapPort)) {
	   throw device_exception ("listening to port failed");
	   return;
	}

	saveFile	= fopen (saveName. toUtf8 (). data (), "w");
	if (saveFile != nullptr) {
	   fprintf (saveFile, "Home location; %f; %f\n\n", 
	                         homeAddress. latitude, homeAddress. longitude);
	   fprintf (saveFile, "channel; latitude; longitude;transmitter;date and time; mainId; subId; distance; azimuth; power, altitude, height, dir\n\n");
	}
}

	httpHandler::~httpHandler	() {
	if (this -> isListening ())
	   this -> close ();
}

void	httpHandler::newConnection	() {
	if (this -> hasPendingConnections ()) {
	   QTcpSocket *client = this -> nextPendingConnection ();
	   connect (client, &QTcpSocket::readyRead,
	            this, &httpHandler::readData);
	   connect (client, &QAbstractSocket::disconnected,
	            &QTcpSocket::deleteLater);
	}
}

void	httpHandler::readData () {
QTcpSocket *worker = qobject_cast<QTcpSocket *> (sender ());
	if (worker == nullptr)
	   return;

	bool	keepAlive	= true;
	QByteArray data	= worker -> readAll ();
	QString request	= QString (data);
	int version	= request. contains ("HTTP/1.1") ? 11 : 10;
	keepAlive	= (version == 11) ? 
	                     request. contains ("Connection: close"):
	                     request. contains ("Connection: keep-alive");
//	fprintf (stderr, "request -> %s\n", request. toLatin1 (). data ());
//	Identify the URL
	QStringList list	= request. split (" ");
	if (list. size () < 2) 
	   return;
	QString theQuestion	= list [1];
	QString content;
	QString ctype;
//	fprintf (stderr, "theQuestion = %s\n", theQuestion. toLatin1 (). data ());
	closing	= false;
	if (theQuestion == "/data.json") {
	   if (transmitterList. size () > 0) {
	      locker. lock ();
	      transmitter t = transmitterList [0];
	      transmitterList. erase (transmitterList. begin ());
	      locker. unlock ();
	      content = coordinatesToJson (t);
	      if (t. type == MAP_CLOSE) {
	         closing = true;
	         keepAlive = false;
	      }
	   }
	   if (content != "") {
	      ctype       = "application/json;charset=utf-8";
	   }
	}
	else	
	if (theQuestion == "/channelSelector::") {
           setChannel (list [2]);
        }
        else {
           content	= theMap (homeAddress);
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
	int e = worker -> write (theData);
	int f = worker -> write (theContents);
	
	if (closing)
	   worker -> waitForBytesWritten ();
	if (!keepAlive)
	   worker -> disconnectFromHost ();
	if (closing)
	   emit mapClose_processed ();
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
QString httpHandler::coordinatesToJson (transmitter &t) { 
char buf [512];
QString Jsontxt;
	if (t. type == MAP_CLOSE)
	   fprintf (stderr, "close signaal\n");
	else
	   fprintf (stderr, "type = %d\n", t. type);
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

void	httpHandler::putData	(uint8_t type,
	                         position target) {
	for (unsigned long i = 0; i < transmitterList. size (); i ++)
	   if ((transmitterList [i]. latitude == target. latitude) &&
	       (transmitterList [i]. longitude == target. longitude))
	      return;

	transmitter t;
	t. type			= type;
	t. dateTime		= "";
	locker. lock ();
	if ((type == MAP_RESET) || (type == MAP_CLOSE))
	   transmitterList. resize (0);
	transmitterList. push_back (t);
	stopLoading	= true;
	locker. unlock ();
}

void	httpHandler::putData	(uint8_t	type,
	                         transmitter	&theTr,
	                         const QString	&theTime) {
//	                         const QString	&channelName,
//	                         float	 snr) {
	float latitude	= theTr. latitude;
	float longitude	= theTr. longitude;
	for (unsigned long i = 0; i < transmitterList. size (); i ++)
	   if ((transmitterList [i]. latitude == latitude) &&
	       (transmitterList [i]. longitude == longitude ) &&
	       (transmitterList [i]. type == type))
	      return;

	fprintf (stderr, "type = %d\n", type);
	theTr. type		= type;
	theTr. dateTime		= theTime;
//	theTr. strength		= snr;

	locker. lock ();
	transmitterList. push_back (theTr);
//	transmitterList. push_back (t);
	locker. unlock ();
	return;
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

