#
/*
 *	Copyright (C) 2022
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

#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<sys/types.h>
#ifndef	__MINGW32__
#include	<sys/socket.h>
#include	<fcntl.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<arpa/inet.h>
#else
#include        <winsock2.h>
#include        <windows.h>
#include        <ws2tcpip.h>
#endif
//#include	<err.h>
#include	<cstring>

#include	"http-handler.h"
#include	"radio.h"

#include	"converted_map.h"

	httpHandler::httpHandler (RadioInterface *parent,
	                          const QString & mapPort,
	                          const QString &browserAddress,
	                          std::complex<float> homeAddress,
	                          const QString &saveName,
	                          bool autoBrowser_off) {
	this	-> parent	= parent;
	this	-> mapPort	= mapPort;
	QString temp		= browserAddress + ":" + mapPort;
	this	-> homeAddress	= homeAddress;
	this	-> autoBrowser_off	= autoBrowser_off;
#ifdef	__MINGW32__
	this	-> browserAddress	= temp. toStdWString ();
#else
	this	-> browserAddress	= temp. toStdString ();
#endif
	this	-> running. store (false);

	connect (this, SIGNAL (terminating ()),
	     parent, SLOT (http_terminate ()));
	saveFile	= fopen (saveName. toUtf8 (). data (), "w");
	if (saveFile != nullptr) {
	   fprintf (saveFile, "Home location; %f; %f\n\n", 
	                                   real (homeAddress), imag (homeAddress));
	   fprintf (saveFile, "channel; latitude; longitude;transmitter;date and time; mainId; subId; distance; azimuth; power\n\n");
	}

	transmitterVector. resize (0);
	start ();
}

	httpHandler::~httpHandler	() {
	if (running. load ()) {
	   running. store (false);
	   threadHandle. join ();
	}
	if (saveFile != nullptr)
	   fclose (saveFile);
}

void	httpHandler::start	() {
	threadHandle = std::thread (&httpHandler::run, this);
	if (autoBrowser_off)
	   return;
#ifdef	__MINGW32__
	ShellExecute (nullptr, L"open", browserAddress. c_str (),
	                                   nullptr, nullptr, SW_SHOWNORMAL);
#else
	std::string x = "xdg-open " + browserAddress;
	system (x. c_str ());
#endif
}

void	httpHandler::stop	() {
	if (running. load ()) {
	   running. store (false);
	   threadHandle. join ();
	}
}

#ifndef	__MINGW32__
void	httpHandler::run	() {
char	buffer [4096];
bool	keepalive;
char	*url;
int one = 1, ClientSocket, ListenSocket;
struct sockaddr_in svr_addr, cli_addr;
std::string	content;
std::string	ctype;

	running. store (true);
	socklen_t sin_len = sizeof (cli_addr);
	ListenSocket = socket (AF_INET, SOCK_STREAM, 0);
	if (ListenSocket < 0) {
	   running. store (false);
	   terminating ();
	   return;
	}

	int flags	= fcntl (ListenSocket, F_GETFL);
	fcntl (ListenSocket, F_SETFL, flags | O_NONBLOCK);
	setsockopt (ListenSocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = INADDR_ANY;
	svr_addr.sin_port = htons (mapPort. toInt ());

	if (::bind (ListenSocket, (struct sockaddr *) &svr_addr,
	                                   sizeof (svr_addr)) == -1) {
	   close (ListenSocket);
	   running. store (false);
	   terminating ();
	   return;
	}
//
//	Now, we are listening to port XXXX, ready to accept a
//	socket for anyone who needs us

	::listen (ListenSocket, 5);
	while (running. load ()) {
	   ClientSocket = accept (ListenSocket,
	                          (struct sockaddr *) &cli_addr, &sin_len);
	   if (ClientSocket == -1) {
	      usleep (2000000);
	      continue;
	   }
//
//	someone needs us, let us see what (s)he wants
	   while (running. load ()) {
	      if (read (ClientSocket, buffer, 4096) < 0) {
	         running. store (false);
	         break;
	      }

//	      fprintf (stderr, "Buffer - %s\n", buffer);
	      int httpver = (strstr (buffer, "HTTP/1.1") != nullptr) ? 11 : 10;
	      if (httpver == 11)
//	HTTP 1.1 defaults to keep-alive, unless close is specified.
	         keepalive = strstr (buffer, "Connection: close") == nullptr;
	      else // httpver == 10
	         keepalive = strstr (buffer, "Connection: keep-alive") != nullptr;

/*	Identify the URL. */
	      char *p = strchr (buffer, ' ');
	      if (p == nullptr)
	         break;
	      url = ++p; // Now this should point to the requested URL.
	      p = strchr (p, ' ');
	      if (p == nullptr)
	         break;
	      *p = '\0';

//	Select the content to send, we have just two so far:
//	 "/" -> Our google map application.
//	 "/data.json" -> Our ajax request to update planes. */
	      bool jsonUpdate	= false;
	      if (strstr (url, "/data.json")) {
	         content	= coordinatesToJson (transmitterList);
	         if (content != "") {
	            ctype	= "application/json;charset=utf-8";
	            jsonUpdate	= true;
	         }
	      }
	      else {
	         content	= theMap (homeAddress);
	         ctype		= "text/html;charset=utf-8";
	      }

//	Create the header
	      char hdr [2048];
	      sprintf (hdr,
	               "HTTP/1.1 200 OK\r\n"
	               "Server: qt-dab\r\n"
	               "Content-Type: %s\r\n"
	               "Connection: %s\r\n"
	               "Content-Length: %d\r\n"
//	               "Access-Control-Allow-Origin: *\r\n"
	               "\r\n",
	               ctype. c_str (),
	               keepalive ? "keep-alive" : "close",
	               (int)(strlen (content. c_str ())));
	      int hdrlen = strlen (hdr);
//	      fprintf (stderr, "reply header %s \n", hdr);
	      if (jsonUpdate) {
//	         fprintf (stderr, "Json update requested\n");
//	         fprintf (stderr, "%s\n", content. c_str ());
	      }
//	and send the reply
	      if (write (ClientSocket, hdr, hdrlen) != hdrlen ||
	          write (ClientSocket, content. c_str (),
	                      content. size ()) != content. size ())  {
//	         fprintf (stderr, "WRITE PROBLEM\n");
//	         break;
	      }
	   }
	}
	fprintf (stderr, "mapServer quits\n");
	close (ListenSocket);
	close (ClientSocket);
	emit terminating ();
}
#else
//
//	windows version
//
void	httpHandler::run	() {
char	buffer [4096];
bool	keepalive;
char	*url;
std::string	content;
std::string	ctype;
WSADATA	wsa;
int	iResult;
SOCKET	ListenSocket	= INVALID_SOCKET;
SOCKET	ClientSocket	= INVALID_SOCKET;

struct addrinfo *result = nullptr;
struct addrinfo hints;

	if (WSAStartup (MAKEWORD (2, 2), &wsa) != 0) {
	   terminating ();
	   return;
	}

	ZeroMemory (&hints, sizeof(hints));
	hints.ai_family		= AF_INET;
	hints.ai_socktype	= SOCK_STREAM;
	hints.ai_protocol	= IPPROTO_TCP;
	hints.ai_flags		= AI_PASSIVE;

//	Resolve the server address and port

	iResult = getaddrinfo (nullptr, mapPort. toLatin1 (). data (),
	                                               &hints, &result);
	if (iResult != 0 ) {
	       WSACleanup();
	   terminating ();
	   return;
	}

// Create a SOCKET for connecting to server
	ListenSocket = socket (result -> ai_family,
	                       result -> ai_socktype, result -> ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
	   freeaddrinfo(result);
	   WSACleanup ();
	   terminating ();
	   return;
	}
	unsigned long mode = 1;
	ioctlsocket (ListenSocket, FIONBIO, &mode);

// Setup the TCP listening socket
	iResult = bind (ListenSocket, result -> ai_addr,
	                                (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
	   freeaddrinfo (result);
	   closesocket (ListenSocket);
	   WSACleanup ();
	   terminating ();
	   return;
	}

	freeaddrinfo (result);

	running. store (true);

	listen (ListenSocket, 5);
	while (running. load ()) {
	   ClientSocket = accept (ListenSocket, nullptr, nullptr);
	   if (ClientSocket == -1)  {
	      usleep (2000000);
	      continue;
	   }

	   while (running. load ()) {
	      int xx;
L1:	      if ((xx = recv (ClientSocket, buffer, 4096, 0)) < 0) {
// shutdown the connection since we're done
	         iResult = shutdown (ClientSocket, SD_SEND);
	         if (iResult == SOCKET_ERROR) {
	            closesocket (ClientSocket);
	            closesocket (ListenSocket);
	            WSACleanup ();
	            running.store (false);
	            terminating ();
	            return;
	         }
	         break;
	      }
	      if (xx == 0) {
	         if (!running. load ()) {
	            closesocket (ClientSocket);
	            closesocket(ListenSocket);
	            WSACleanup ();
	            terminating ();
	            return;
	         }
	         Sleep (1);
	         goto L1;
	      }

	      int httpver = (strstr (buffer, "HTTP/1.1") != nullptr) ? 11 : 10;
	      if (httpver == 11)
//	HTTP 1.1 defaults to keep-alive, unless close is specified.
	         keepalive = strstr (buffer, "Connection: close") == nullptr;
	      else // httpver == 10
	         keepalive = strstr (buffer, "Connection: keep-alive") != nullptr;

	/* Identify the URL. */
	      char *p = strchr (buffer, ' ');
	      if (p == nullptr)
	         break;
	      url = ++p; // Now this should point to the requested URL.
	      p = strchr (p, ' ');
	      if (p == nullptr)
	         break;
	      *p = '\0';

//	Select the content to send, we have just two so far:
//	 "/" -> Our google map application.
//	 "/data.json" -> Our ajax request to update transmitters. */
	      bool jsonUpdate	= false;
	      if (strstr (url, "/data.json")) {
	         content	= coordinatesToJson (transmitterList);
	         if (content != "") {
	            ctype	= "application/json;charset=utf-8";
	            jsonUpdate	= true;
//	            fprintf (stderr, "%s will be sent\n", content. c_str ());
	         }
	      }
	      else {
	         content	= theMap (homeAddress);
	         ctype		= "text/html;charset=utf-8";
	      }
//	Create the header
	      char hdr [2048];
	      sprintf (hdr,
	               "HTTP/1.1 200 OK\r\n"
	               "Server: SDRunoPlugin_1090\r\n"
	               "Content-Type: %s\r\n"
	               "Connection: %s\r\n"
	               "Content-Length: %d\r\n"
//	               "Access-Control-Allow-Origin: *\r\n"
	               "\r\n",
	               ctype. c_str (),
	               keepalive ? "keep-alive" : "close",
	               (int)(strlen (content. c_str ())));
	      int hdrlen = strlen (hdr);
//	      if (jsonUpdate) {
//	         parent -> show_text (std::string ("Json update requested\n"));
//	         parent -> show_text (content);
//	      }
//	and send the reply
	      if ((send (ClientSocket, hdr, hdrlen, 0) == SOCKET_ERROR) ||
	          (send (ClientSocket, content. c_str (),
	                          content. size (), 0) == SOCKET_ERROR))  {
	         fprintf (stderr, "WRITE PROBLEM\n");
	         break;
	      }
	   }
	}
// cleanup
	closesocket(ClientSocket);
	closesocket(ListenSocket);
	WSACleanup();
	running. store (false);
	emit	terminating ();
}
#endif

std::string	httpHandler::theMap (std::complex<float> homeAddress) {
std::string res;
int	bodySize;
char	*body;
std::string latitude	= std::to_string (real (homeAddress));
std::string longitude	= std::to_string (imag (homeAddress));
int	index		= 0;
int	cc;
int teller	= 0;
int params	= 0;

	bodySize	= sizeof (qt_map);
	body		=  (char *)malloc (bodySize + 40);
	while (qt_map [index] != 0) {
	   cc =  (char)(qt_map [index]);
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
	res	= std::string (body);
//	fprintf (stderr, "The map :\n%s\n", res. c_str ());
	free (body);
	return res;
}

std::string dotNumber (float f) {
char temp [256];
std::string s = std::to_string (f);
	for (int i = 0; i < s. size (); i ++)
	   if (s. c_str () [i] == ',')
	      temp [i] = '.';
	   else
	      temp [i] = s. c_str () [i];
	temp [s. size ()] = 0;
	return std::string (temp);
}
//
std::string httpHandler::coordinatesToJson (std::vector<httpData> &t) {
std::complex<float> home;
std::complex<float> target = std::complex<float> (0, 0);
char buf [512];
QString Jsontxt;

	if (t. size () == 0)
	   return "";
	Jsontxt += "[\n";
	locker. lock ();
//	the Target
	snprintf (buf, 512,
	      "{\"type\":%d, \"lat\":%s, \"lon\":%s, \"name\":\"%s\", \"channel\":\"%s\", \"dateTime\":\"%s\", \"dist\":%d, \"azimuth\":%d, \"power\":%d}",
	       t [0]. type,
	       dotNumber (real (t [0]. coords)). c_str (),
	       dotNumber (imag (t [0]. coords)). c_str (),
	       t [0]. transmitterName. toUtf8 (). data (),
	       t [0]. channelName. toUtf8 (). data (),
	       t [0]. dateTime. toUtf8 (). data (),
	       t [0]. distance,
	       t [0]. azimuth,
	       (int)(t [0]. power * 100));
	Jsontxt += QString (buf);
//
//
	for (unsigned long i = 1; i < t. size (); i ++) {
	   snprintf (buf, 512,
	      ",\n{\"type\":%d, \"lat\":%s, \"lon\":%s, \"name\":\"%s\", \"channel\":\"%s\", \"dateTime\":\"%s\",  \"dist\":%d, \"azimuth\":%d, \"power\":%d, }",
	        t [i]. type,
	        dotNumber (real (t [i]. coords)). c_str (),
	        dotNumber (imag (t [i]. coords)). c_str (),
	        t [i]. transmitterName. toUtf8 (). data (),
	        t [i]. channelName. toUtf8 (). data (),
	        t [i]. dateTime. toUtf8 (). data (),
	        t [i]. distance,
	        t [i]. azimuth,
	        (int)(t [i]. power * 100));
	   Jsontxt += QString (buf);
	}
	t. resize (0);
	locker. unlock ();
	Jsontxt += "\n]\n";
//	fprintf (stderr, "Json = %s\n", Jsontxt. toLatin1 (). data ());
	return Jsontxt. toStdString ();
}

void	httpHandler::putData	(uint8_t	type,
	                         std::complex<float> target,
	                         QString transmitterName,
	                         QString channelName,
	                         QString dateTime,
	                         int ttiId,
	                         int distance,
	                         int azimuth,
	                         float power) {
	for (unsigned long i = 0; i < transmitterList. size (); i ++)
	   if (transmitterList [i]. coords == target)
	      return;

	   
	httpData t;
	t. type			= type;
	t. coords		= target;
	t. transmitterName	= transmitterName;
	t. channelName		= channelName;
	t. dateTime		= dateTime;
	t. ttiId		= ttiId;
	t. distance		= distance;
	t. azimuth		= azimuth;
	t. power		= power;

	locker. lock ();
	transmitterList. push_back (t);
	locker. unlock ();

	for (int i = 0; i < transmitterVector. size (); i ++) {
	   if ((transmitterVector. at (i). transmitterName == transmitterName) &&
	       (transmitterVector. at (i). channelName == channelName))
	      return;
	}
	
	if ((saveFile != nullptr)  &&
	           ((type != MAP_RESET) && (type != MAP_FRAME))) {
	   fprintf (saveFile, "%s; %f; %f; %s; %s; %d; %d; %d; %d; %f\n",
	                      channelName. toUtf8 (). data (),
	                      real (target), imag (target),
	                      transmitterName. toUtf8 (). data (),
	                      t. dateTime. toUtf8 (). data (),
	                      ttiId / 100, ttiId % 100,
	                      distance, azimuth, power);
	   transmitterVector. push_back (t);
	}
}

