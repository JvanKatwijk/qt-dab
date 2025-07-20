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

#include "tcp-client-8.h"

/*
 *	Thanks to Youssef Touil (well know from Airspy and spyServer),
 *	the structure of this program part is an adapted C++ translation
 *	of a C# fragment that he is using
 */
#ifdef __MINGW32__
#pragma comment(lib, "ws2_32.lib")
#else
# include <arpa/inet.h>
# include <sys/resource.h>
# include <sys/select.h>
# include <sys/ioctl.h>
# include <netdb.h>
# define ioctlsocket ioctl
#endif
#if defined(__MINGW32__) || defined(__APPLE__)
    #ifndef MSG_NOSIGNAL
        #define MSG_NOSIGNAL 0
    #endif
#endif

	tcp_client_8::tcp_client_8 (const QString &addr, int port,
	                            RingBuffer<uint8_t> *inBuffer):
	                                          outBuffer (32768) {
int RetCode;
	this	-> tcpAddress	= addr;
	this	-> tcpPort	= port;
	this	-> inBuffer	= inBuffer;
	connected		= false;

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
	int	bufSize	= 8 * 32768;
	setsockopt(SendingSocket, SOL_SOCKET, SO_RCVBUF,
	                               (char*)&bufSize, sizeof(bufSize));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(this -> tcpPort);
	ServerAddr.sin_addr.s_addr =
	                     inet_addr (tcpAddress. toLatin1 (). data ());

	RetCode = ::connect (SendingSocket,
	                     (struct sockaddr *) &ServerAddr, sizeof(ServerAddr));
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
	start ();	// this is the reader
	connected	= true;
}

	tcp_client_8::~tcp_client_8 () {
	if (isRunning ()) {
	   running. store (false);
	   while (isRunning ())
	      usleep (1000);
	}
}

bool	tcp_client_8::is_connected () {
	return connected;
}

void	tcp_client_8::connect_conn	() {
}

void	tcp_client_8::close_conn	() {
	if (SendingSocket > 0) {
           close (SendingSocket);
        } 
}

void	tcp_client_8::send_data (uint8_t *data, int length) {
	outBuffer. putDataIntoBuffer (data, length);
}

char	tempBuffer_8 [1000000];
void	tcp_client_8:: run	() {
int	received	= 0;
struct timeval m_timeInterval;
fd_set m_readFds;

	running. store (true);
	while (running. load ()) {
	   FD_ZERO (&m_readFds);
	   FD_SET  (SendingSocket, &m_readFds);
	   m_timeInterval. tv_usec	= 100;
	   m_timeInterval. tv_sec	= 0;
	   int m_receivingStatus	= select (SendingSocket + 1, &m_readFds,
	                                  nullptr, nullptr, &m_timeInterval);

	   if (m_receivingStatus < 0) {
	      std::cerr << "ERROR" << std::endl;
	   }
	   if (m_receivingStatus == 0) {
	      int amount = outBuffer. GetRingBufferReadAvailable ();
	      if (amount > 0) {
	         (void)outBuffer. getDataFromBuffer (tempBuffer_8, amount);
	         send (SendingSocket, (char *)tempBuffer_8, amount, MSG_NOSIGNAL);
	      }
	   }	
	   else {
	      sockaddr t;
#ifndef	__MINGW32__
	      uint32_t	tt = 10;
#else
	      int	tt = 10;
#endif
	      unsigned long bytesAvailable = 0;
	      int ret = ioctlsocket (SendingSocket, FIONREAD, &bytesAvailable);
	      (void)ret;
	      received = recvfrom (SendingSocket, (char *)tempBuffer_8,
	                                             bytesAvailable, 0, &t, &tt);
	      inBuffer -> putDataIntoBuffer (tempBuffer_8, received);
	   }
	}
}


