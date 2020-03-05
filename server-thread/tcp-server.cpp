#
/*
 *    Copyright (C) 2016, 2017
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
 *    along with DAB-library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	Simple streaming server, for e.g. epg data and tpg data
 */

#include	<stdint.h>
#include	<ringbuffer.h>
#include	<arpa/inet.h>
#include	"tcp-server.h"

	tcpServer::tcpServer (int port) {
	   buffer	= new RingBuffer<uint8_t> (32 * 32768);
	   connected. store (false);
	   socketDesc	= -1;
	   threadHandle	= std::thread (&tcpServer::run, this, port);
}

	tcpServer::~tcpServer (void) {
	if (running. load()) {
           running. store (false);
	   if (socketDesc != -1) {
	      shutdown (socketDesc, SHUT_RDWR);
	      socketDesc = -1;
	   }
           usleep (1000);
           threadHandle. join();
        }
	delete buffer;
}

void	tcpServer::sendData (uint8_t *data, int32_t amount) {
	if (connected)
	   buffer -> putDataIntoBuffer (data, amount);
}
#define	BUF_SIZE	1024

void	tcpServer::run (int port) {
// Variables for writing a server. 
/*
 *	1. Getting the address data structure.
 *	2. Opening a new socket.
 *	3. Bind to the socket.
 *	4. Listen to the socket. 
 *	5. Accept Connection.
 *	6. Receive Data.
 *	7. Close Connection. 
 */
	int client_sock , c , read_size;
	struct sockaddr_in server , client;

//	Create socket
	socketDesc = socket (AF_INET , SOCK_STREAM , 0);
	if (socketDesc == -1) {
	   fprintf (stderr, "Could not create socket");
	   return;
	}
	fprintf (stderr, "Socket created");
	running. store (true);
//	Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons (port);
	
//	Bind
	if (bind (socketDesc,
	          (struct sockaddr *)&server , sizeof(server)) < 0) {
//	print the error message
	   perror ("bind failed. Error");
	   return;
	}
	
	fprintf (stderr, "I am now accepting connections ...\n");
//	Listen
	listen (socketDesc , 3);
	while (running) {
//	Accept a new connection and return back the socket desciptor 
	   c = sizeof(struct sockaddr_in);
     
//	accept connection from an incoming client
	   client_sock = accept (socketDesc, 
	                         (struct sockaddr *)&client,
	                         (socklen_t*)&c);
	   if (client_sock < 0) {
	      perror("accept failed");
	      return;
	   }
	   fprintf (stderr, "Connection accepted");
	   connected = true;
	   try {
	      uint8_t	localBuffer [BUF_SIZE];
	      int16_t	amount;
	      int status;
	      while (running. load()) {
	         while (running. load() &&
	                 (buffer -> GetRingBufferReadAvailable() < BUF_SIZE)) 
	            usleep (1000);
	         amount = buffer -> getDataFromBuffer (localBuffer, BUF_SIZE);
	         status = send (client_sock, localBuffer, amount ,0);
	         if (status == -1) {
	            throw (22);
	         }
	      }
	   }
	   catch (int e) {}
	   connected = false;
	}
	// Close the socket before we finish 
	if (socketDesc != -1)
	   close (socketDesc);	
	fprintf (stderr, "socket is closed now\n");
}
