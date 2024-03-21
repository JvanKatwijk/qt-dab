#
/*
 *    Copyright (C) 2015 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
 *
 *	Communication via network to the Qt-DAB receiver to 
 *	show the tdc data
 */

#include	<QSettings>
#include	<QLabel>
#include	<QMessageBox>
#include	<QtNetwork>
#include	<QTcpSocket>
#include	<QHostAddress>
#include	"client.h"
//


#define swap(a) (((a) << 8) | ((a) >> 8))

//---------------------------------------------------------------------------
uint16_t usCalculCRC (uint8_t *buf, int lg) {
uint16_t crc;
uint    count;
        crc = 0xFFFF;
        for (count= 0; count < lg; count++) {
           crc = (uint16_t) (swap (crc) ^ (uint16_t)buf [count]);
           crc ^= ((uint8_t)crc) >> 4;
           crc = (uint16_t)
                 (crc ^ (swap((uint8_t)(crc)) << 4) ^ ((uint8_t)(crc) << 5));
        }
        return ((uint16_t)(crc ^ 0xFFFF));
}

static inline
bool    check_crc_bytes (uint8_t *msg, int16_t len) {
int i, j;
uint16_t        accumulator     = 0xFFFF;
uint16_t        crc;
uint16_t        genpoly         = 0x1021;

        for (i = 0; i < len; i ++) {
           int16_t data = msg [i] << 8;
           for (j = 8; j > 0; j--) {
              if ((data ^ accumulator) & 0x8000)
                 accumulator = ((accumulator << 1) ^ genpoly) & 0xFFFF;
              else
                 accumulator = (accumulator << 1) & 0xFFFF;
              data = (data << 1) & 0xFFFF;
           }
        }
//
//      ok, now check with the crc that is contained
//      in the au
        crc     = ~((msg [len] << 8) | msg [len + 1]) & 0xFFFF;
        return (crc ^ accumulator) == 0;
}

	Client::Client (QWidget *parent):QDialog (parent) {
int16_t	i;
	setupUi (this);
	connected	= false;
	connect (connectButton, SIGNAL (clicked ()),
	         this, SLOT (wantConnect ()));
	connect (terminateButton, SIGNAL (clicked ()),
	         this, SLOT (terminate ()));
	state	-> setText ("waiting to start");

	connectionTimer	= new QTimer ();
	connectionTimer	-> setInterval (1000);
	connect (connectionTimer, SIGNAL (timeout ()),
	         this, SLOT (timerTick ()));
}
//

	Client::~Client	() {
	connected	= false;
}
//
void	Client::wantConnect () {
QString ipAddress;
int16_t	i;
QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

	if (connected)
	   return;
	// use the first non-localhost IPv4 address
	for (i = 0; i < ipAddressesList.size(); ++i) {
	   if (ipAddressesList.at (i) != QHostAddress::LocalHost &&
	      ipAddressesList. at (i). toIPv4Address ()) {
	      ipAddress = ipAddressesList. at(i). toString();
	      break;
	   }
	}
	// if we did not find one, use IPv4 localhost
	if (ipAddress. isEmpty())
	   ipAddress = QHostAddress (QHostAddress::LocalHost).toString ();
	hostLineEdit -> setText (ipAddress);

	hostLineEdit -> setInputMask ("000.000.000.000");
//	Setting default IP address
	state	-> setText ("Give IP address, return");
	connect (hostLineEdit, SIGNAL (returnPressed ()),
	         this, SLOT (setConnection ()));
}

//	if / when a return is pressed in the line edit,
//	a signal appears and we are able to collect the
//	inserted text. The format is the IP-V4 format.
//	Using this text, we try to connect,
void	Client::setConnection () {
QString s	= hostLineEdit -> text ();
QHostAddress theAddress	= QHostAddress (s);
int32_t	basePort;
	basePort	= 8888;
	disconnect (hostLineEdit, SIGNAL (returnPressed ()),
	            this, SLOT (setConnection ()));
//
//	The streamer will provide us with the raw data
	streamer. connectToHost (theAddress, basePort);
	if (!streamer. waitForConnected (2000)) {
	   QMessageBox::warning (this, tr ("client"),
	                                   tr ("setting up stream failed\n"));
	   return;
	}

	connected	= true;
	state -> setText ("Connected");
	connect (&streamer, SIGNAL (readyRead ()),
	         this, SLOT (readData ()));
	connectionTimer	-> start (1000);
}

//	These functions are typical for network use
void	Client::readData	() {
QByteArray d;
int16_t	i;

	d. resize (4 * 512);
	while (streamer. bytesAvailable () > 4 * 512) {
	   streamer. read (d. data (), d. size ());
	   for (i = 0; i < d. size (); i ++)
	      handle ((uint8_t)(d [i]));
	}
}

#define	SEARCH_HEADER	0
#define	HEADER_FOUND	1

int	searchState = SEARCH_HEADER;

void	Client::handle (uint8_t d) {
int16_t i;
	if (searchState == SEARCH_HEADER) {
	   headertester. shift (d);
	   if (headertester. hasHeader ()) {
	      toRead = headertester. length ();
	      dataLength	= toRead;
	      dataBuffer	= new uint8_t [toRead];
	      dataIndex		= 0;
	      frameType = headertester. frametype ();
	      searchState = HEADER_FOUND;
	      headertester. reset ();
	   }
	   return;
	}
	toRead --;
	dataBuffer [dataIndex ++] = d;
	if (toRead == 0) {
	   searchState = SEARCH_HEADER;
	   if (frameType == 0)
	      handleFrameType_0 (dataBuffer, dataLength);
	   else
	      handleFrameType_1 (dataBuffer, dataLength);
	   delete[] dataBuffer;
	}
}

void	Client::terminate	() {
	if (connected) {
	   streamer. close ();
	}
	accept ();
}

void	Client::timerTick () {
	if (streamer. state () == QAbstractSocket::UnconnectedState) {
	   state	-> setText ("not connected");
	   connected	= false;
	   connectionTimer	-> stop ();
	}
}

void	Client::handleFrameType_0 (uint8_t *dataBuffer, int16_t Length) {
	fprintf (stderr, "number of services %d\n", dataBuffer [0]);
}

void	Client::handleFrameType_1 (uint8_t *dataBuffer, int16_t Length) {
	fprintf (stderr, "type 1 %x %x %x %x\n",
	         dataBuffer [0],
	         dataBuffer [1], dataBuffer [2], dataBuffer [3]);

	if (dataBuffer [3] == 0) {	// encryption 0
	   int16_t index   = 4;
	   while (index < Length) {
	      int16_t i;
	      uint8_t sCi     = dataBuffer [index];
	      uint16_t segLen = (dataBuffer [index + 1] << 8) |
	                         dataBuffer [index + 2];
	      uint16_t crc    = (dataBuffer [index + 3] << 8) |
	                         dataBuffer [index + 4];
	      uint8_t testVector [18];
	      testVector [0] = sCi;
	      testVector [1] = dataBuffer [index + 1];
	      testVector [2] = dataBuffer [index + 2];
	      for (i = 0; i < 13; i ++)
	         testVector [i + 3] = dataBuffer [index + i + 5];
	      testVector [16] = dataBuffer [index + 3];
	      testVector [17] = dataBuffer [index + 4];
	      if (usCalculCRC (testVector, 16) == crc)
	         fprintf (stderr,
	                  "sCi = %o, segLen = %d (index = %d, Length = %d)\n",
	                   sCi, segLen, index, Length);
	      else {
	         fprintf (stderr, "failing crc\n");
	      }
	      index += segLen + 5;
	   }
	}
	else
	   fprintf (stderr, "encryption not zero\n");
}

bool	Client::serviceComponentFrameheaderCRC (uint8_t *data,
	                                        int16_t offset,
	                                        int16_t maxL) {
uint8_t testVector [18];
int16_t i;
int16_t length  = (data [offset + 1] << 8) | data [offset + 2];
int16_t size    = length < 13 ? length : 13;
uint16_t        crc;
        if (length < 0)
           return false;                // assumed garbage
        crc     = (data [offset + 3] << 8) | data [offset + 4];      // the crc
        testVector [0]  = data [offset + 0];
        testVector [1]  = data [offset + 1];
        testVector [2]  = data [offset + 2];
        for (i = 0; i < size; i ++)
           testVector [3 + i] = data [offset + 5 + i];
	return usCalculCRC (testVector, 3 + size) == crc;
}


