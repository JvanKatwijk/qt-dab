#
/*
 *    Copyright (C) 2016 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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

#include "tcp-client.h"

#ifdef _WIN32
# include <winsock2.h>
# include <ws2tcpip.h>
# include <atomic>
#   ifdef _MSC_VER
#   pragma comment(lib, "ws2_32.lib")
#   endif
# ifndef MSG_WAITALL
#   define MSG_WAITALL (1 << 3)
# endif
#else
# include <sys/socket.h>
# include <arpa/inet.h>
# include <sys/resource.h>
# include <sys/select.h>
# include <sys/ioctl.h>
# include <netdb.h>
# include <unistd.h>
# define ioctlsocket ioctl
#endif
#if defined(_WIN32) || defined(__APPLE__)
    #ifndef MSG_NOSIGNAL
        #define MSG_NOSIGNAL 0
    #endif
#endif


	tcp_client::tcp_client (const QString &addr, int port) {
int RetCode;
	this	-> tcpAddress	= addr;
	this	-> tcpPort	= port;
	connected	= false;

#ifdef	__MINGW32__
	WSAData wsaData;
	WSAStartup (MAKEWORD (2, 2), &wsaData);
	fprintf (stderr, "Client: Winsock DLL is %s\n",
	                                 wsaData. szSystemStatus);
#endif
	SendingSocket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
#ifdef	__MINGW32__
	if (SendingSocket == INVALID_SOCKET) {
	   fprintf (stderr, "Client: socket failed: Error code: %ld\n", WSAGetLastError());
	   WSACleanup ();
	   return;
	}
#endif
	int	inBuffer = 32768;
	int	outBuffer;
//#ifdef	__MINGW32__
	int R2 = setsockopt (SendingSocket, SOL_SOCKET, SO_RCVBUF,
	                       (char *)(&inBuffer), 4);
	if (R2 != 0)
	   fprintf (stderr, "setsockoption gave error %d\n", errno);
//#endif
	fprintf (stderr, "Socket is OK\n");
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(this -> tcpPort);
	ServerAddr.sin_addr.s_addr =
	                     inet_addr (tcpAddress. toLatin1 (). data ());

	RetCode = ::connect (SendingSocket,
	                     (struct sockaddr *) &ServerAddr, sizeof(ServerAddr));
	fprintf (stderr, "Return code connect %d\n", RetCode);
	if (RetCode != 0) {
#ifdef	__MINGW32__
	   printf ("Client: connect() failed! Error code: %ld\n",
	                                             WSAGetLastError());
#endif
	   close (SendingSocket);
#ifdef	__MINGW32__
	   WSACleanup();
#endif
	   return;
	}
	else {
	   fprintf (stderr, "Client: connect() is OK, got connected...\n");
	   fprintf (stderr, "Client: Ready for sending and/or receiving data...\n");
	}
	connected	= true;
}

	tcp_client::~tcp_client () {
	fprintf (stderr, "request to terminate\n");
}

bool	tcp_client::is_connected () {
	return connected;
}

void tcp_client::connect_conn	() {
}

void tcp_client::close_conn	() {
	if (SendingSocket > 0) {
           close (SendingSocket);
        } 
}

int	tcp_client::receive_data (uint8_t *data, int length) {
	locker. lock ();
	int received = recv (SendingSocket, (char *)data, length, MSG_WAITALL);
	locker. unlock ();
	return received;
}

void	tcp_client::send_data (uint8_t *data, int length) {
	locker. lock ();
	send (SendingSocket, (char *)data, length, MSG_NOSIGNAL);
	locker. unlock ();
}

uint64_t	tcp_client::available_data() {
unsigned long bytesAvailable = 0;
	locker. lock ();
	int ret = ioctlsocket (SendingSocket, FIONREAD, &bytesAvailable);
	switch (ret) {
	   case EINVAL:
	   case EFAULT:
	   case ENOTTY:
	      locker. unlock ();
	      return 0;
	   case EBADF:
	      locker. unlock ();
	      return 0;
	   default:
	      break;
	}
	locker. unlock ();
	return bytesAvailable;
}

