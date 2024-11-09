#
/*
 *    Copyright (C) 2016 .. 2024
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
 :*
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    A simple client for rtl_tcp
 */

#include	<QSettings>
#include	<QLabel>
#include	<QMessageBox>
#include	<QHostAddress>
#include	<QTcpSocket>
#include	<QFileDialog>
#include	<QDir>
#include	"rtl_tcp_client.h"
#include	"dab-constants.h"
#include	"device-exceptions.h"
#include	"settingNames.h"
#include	"settings-handler.h"

#define	RTL_TCP_CLIENT	"RTL_TCP_CLIENT"

#define	DEFAULT_FREQUENCY	(Khz (220000))

	rtl_tcp_client::rtl_tcp_client	(QSettings *s):
	                          _I_Buffer (32 * 32768) {
	remoteSettings		= s;

	setupUi (&myFrame);
	myFrame.setWindowFlag (Qt::Tool, true); 

	myFrame. show		();
	myFrame. hide		();
	myFrame. show		();

    //	setting the defaults and constants
	theGain		= value_i (remoteSettings, RTL_TCP_CLIENT,
	                                    "rtl_tcp_client-gain", 20);
	thePpm		= value_i (remoteSettings, RTL_TCP_CLIENT,
	                                    "rtl_tcp_client-ppm", 0);
	vfoOffset	= value_i (remoteSettings, RTL_TCP_CLIENT,
	                                    "rtl_tcp_client-offset", 0);
	basePort	= value_i (remoteSettings, RTL_TCP_CLIENT,
	                                    "rtl_tcp_port", 1234);
	tcp_gain	-> setValue (theGain);
	tcp_ppm		-> setValue (thePpm);
	lastFrequency	= DEFAULT_FREQUENCY;
	connected	= false;
	hostLineEdit 	= new QLineEdit (nullptr);
	dumping		= false;
//
	connect (tcp_connect, &QPushButton::clicked,
	         this, &rtl_tcp_client::wantConnect);
	connect (tcp_disconnect, &QPushButton::clicked,
	         this, &rtl_tcp_client::setDisconnect);
	connect (tcp_gain, qOverload<int>(&QSpinBox::valueChanged),
	         this, &rtl_tcp_client::sendGain);
	connect (tcp_ppm, qOverload<int>(&QSpinBox::valueChanged),
	         this, &rtl_tcp_client::set_fCorrection);
	connect (khzOffset, qOverload<int>(&QSpinBox::valueChanged),
	         this, &rtl_tcp_client::set_Offset);
	theState	-> setText ("waiting to start");
}

	rtl_tcp_client::~rtl_tcp_client() {
	if (connected) {		// close previous connection
	   stopReader();
//	   streamer. close();
	   QString peerAddress	= toServer. peerAddress (). toString ();
	   store (remoteSettings, RTL_TCP_CLIENT,
	                           "remote-server", peerAddress);
	   QByteArray datagram;
	}
	store (remoteSettings, RTL_TCP_CLIENT, 
	                           "rtl_tcp_client-gain",   theGain);
	store (remoteSettings, RTL_TCP_CLIENT,
	                           "rtl_tcp_client-ppm",    thePpm);
	store (remoteSettings, RTL_TCP_CLIENT,
	                           "rtl_tcp_client-offset", vfoOffset);
	toServer. close();
	delete	hostLineEdit;
}
//
void	rtl_tcp_client::wantConnect () {
QString ipAddress;
int16_t	i;
QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

	if (connected)
	   return;
	// use the first non-localhost IPv4 address
	for (i = 0; i < ipAddressesList.size (); ++i) {
	   if (ipAddressesList.at (i) != QHostAddress::LocalHost &&
	      ipAddressesList. at (i). toIPv4Address()) {
	      ipAddress = ipAddressesList. at(i). toString();
	      break;
	   }
	}
	// if we did not find one, use IPv4 localhost
	if (ipAddress. isEmpty())
	   ipAddress = QHostAddress (QHostAddress::LocalHost).toString();
	ipAddress	=  value_s (remoteSettings, RTL_TCP_CLIENT,
	                           "remote-server", ipAddress);
	hostLineEdit -> setText (ipAddress);

	hostLineEdit	-> setInputMask ("000.000.000.000");
//	Setting default IP address
	hostLineEdit	-> show();
	theState	-> setText ("Enter IP address, \nthen press return");
	connect (hostLineEdit, &QLineEdit::returnPressed,
	         this, &rtl_tcp_client::setConnection);
}

//	if/when a return is pressed in the line edit,
//	a signal appears and we are able to collect the
//	inserted text. The format is the IP-V4 format.
//	Using this text, we try to connect,
void	rtl_tcp_client::setConnection () {
QString s	= hostLineEdit -> text();
QHostAddress theAddress	= QHostAddress (s);

	serverAddress	= QHostAddress (s);
	disconnect (hostLineEdit, &QLineEdit::returnPressed,
	            this, &rtl_tcp_client::setConnection);
	toServer. connectToHost (serverAddress, basePort);
	if (!toServer. waitForConnected (2000)) {
	   QMessageBox::warning (&myFrame, tr ("sdr"),
	                                   tr ("connection failed\n"));
	   return;
	}

	sendGain (theGain);
	sendRate (INPUT_RATE);
	sendVFO	(DEFAULT_FREQUENCY - INPUT_RATE / 4);
	toServer. waitForBytesWritten();
	theState -> setText ("Connected");
	connected	= true;
}

int32_t	rtl_tcp_client::getRate	() {
	return INPUT_RATE;
}

bool	rtl_tcp_client::restartReader	(int32_t freq) {
	if (!connected)
	   return true;
	lastFrequency	= freq;
//	here the command to set the frequency
	sendVFO (freq);
	connect (&toServer, &QTcpSocket::readyRead,
	         this, &rtl_tcp_client::readData);
	return true;
}

void	rtl_tcp_client::stopReader() {
	if (!connected)
	   return;
	disconnect (&toServer, &QTcpSocket::readyRead,
	            this, &rtl_tcp_client::readData);
}
//
//
//	The brave old getSamples. For the dab stick, we get
//	size: still in I/Q pairs, but we have to convert the data from
//	uint8_t to DSPCOMPLEX *
int32_t	rtl_tcp_client::getSamples (std::complex<float> *V, int32_t size) { 
int32_t	amount =  0;
	amount = _I_Buffer. getDataFromBuffer (V, size);
	return amount;
}

int32_t	rtl_tcp_client::Samples () {
	return  _I_Buffer. GetRingBufferReadAvailable ();
}
//
int16_t	rtl_tcp_client::bitDepth () {
	return 8;
}

static 
float mapTable [] = {
 -128 / 128.0 , -127 / 128.0 , -126 / 128.0 , -125 / 128.0 , -124 / 128.0 , -123 / 128.0 , -122 / 128.0 , -121 / 128.0 , -120 / 128.0 , -119 / 128.0 , -118 / 128.0 , -117 / 128.0 , -116 / 128.0 , -115 / 128.0 , -114 / 128.0 , -113 / 128.0 
, -112 / 128.0 , -111 / 128.0 , -110 / 128.0 , -109 / 128.0 , -108 / 128.0 , -107 / 128.0 , -106 / 128.0 , -105 / 128.0 , -104 / 128.0 , -103 / 128.0 , -102 / 128.0 , -101 / 128.0 , -100 / 128.0 , -99 / 128.0 , -98 / 128.0 , -97 / 128.0 
, -96 / 128.0 , -95 / 128.0 , -94 / 128.0 , -93 / 128.0 , -92 / 128.0 , -91 / 128.0 , -90 / 128.0 , -89 / 128.0 , -88 / 128.0 , -87 / 128.0 , -86 / 128.0 , -85 / 128.0 , -84 / 128.0 , -83 / 128.0 , -82 / 128.0 , -81 / 128.0 
, -80 / 128.0 , -79 / 128.0 , -78 / 128.0 , -77 / 128.0 , -76 / 128.0 , -75 / 128.0 , -74 / 128.0 , -73 / 128.0 , -72 / 128.0 , -71 / 128.0 , -70 / 128.0 , -69 / 128.0 , -68 / 128.0 , -67 / 128.0 , -66 / 128.0 , -65 / 128.0 
, -64 / 128.0 , -63 / 128.0 , -62 / 128.0 , -61 / 128.0 , -60 / 128.0 , -59 / 128.0 , -58 / 128.0 , -57 / 128.0 , -56 / 128.0 , -55 / 128.0 , -54 / 128.0 , -53 / 128.0 , -52 / 128.0 , -51 / 128.0 , -50 / 128.0 , -49 / 128.0 
, -48 / 128.0 , -47 / 128.0 , -46 / 128.0 , -45 / 128.0 , -44 / 128.0 , -43 / 128.0 , -42 / 128.0 , -41 / 128.0 , -40 / 128.0 , -39 / 128.0 , -38 / 128.0 , -37 / 128.0 , -36 / 128.0 , -35 / 128.0 , -34 / 128.0 , -33 / 128.0 
, -32 / 128.0 , -31 / 128.0 , -30 / 128.0 , -29 / 128.0 , -28 / 128.0 , -27 / 128.0 , -26 / 128.0 , -25 / 128.0 , -24 / 128.0 , -23 / 128.0 , -22 / 128.0 , -21 / 128.0 , -20 / 128.0 , -19 / 128.0 , -18 / 128.0 , -17 / 128.0 
, -16 / 128.0 , -15 / 128.0 , -14 / 128.0 , -13 / 128.0 , -12 / 128.0 , -11 / 128.0 , -10 / 128.0 , -9 / 128.0 , -8 / 128.0 , -7 / 128.0 , -6 / 128.0 , -5 / 128.0 , -4 / 128.0 , -3 / 128.0 , -2 / 128.0 , -1 / 128.0 
, 0 / 128.0 , 1 / 128.0 , 2 / 128.0 , 3 / 128.0 , 4 / 128.0 , 5 / 128.0 , 6 / 128.0 , 7 / 128.0 , 8 / 128.0 , 9 / 128.0 , 10 / 128.0 , 11 / 128.0 , 12 / 128.0 , 13 / 128.0 , 14 / 128.0 , 15 / 128.0 
, 16 / 128.0 , 17 / 128.0 , 18 / 128.0 , 19 / 128.0 , 20 / 128.0 , 21 / 128.0 , 22 / 128.0 , 23 / 128.0 , 24 / 128.0 , 25 / 128.0 , 26 / 128.0 , 27 / 128.0 , 28 / 128.0 , 29 / 128.0 , 30 / 128.0 , 31 / 128.0 
, 32 / 128.0 , 33 / 128.0 , 34 / 128.0 , 35 / 128.0 , 36 / 128.0 , 37 / 128.0 , 38 / 128.0 , 39 / 128.0 , 40 / 128.0 , 41 / 128.0 , 42 / 128.0 , 43 / 128.0 , 44 / 128.0 , 45 / 128.0 , 46 / 128.0 , 47 / 128.0 
, 48 / 128.0 , 49 / 128.0 , 50 / 128.0 , 51 / 128.0 , 52 / 128.0 , 53 / 128.0 , 54 / 128.0 , 55 / 128.0 , 56 / 128.0 , 57 / 128.0 , 58 / 128.0 , 59 / 128.0 , 60 / 128.0 , 61 / 128.0 , 62 / 128.0 , 63 / 128.0 
, 64 / 128.0 , 65 / 128.0 , 66 / 128.0 , 67 / 128.0 , 68 / 128.0 , 69 / 128.0 , 70 / 128.0 , 71 / 128.0 , 72 / 128.0 , 73 / 128.0 , 74 / 128.0 , 75 / 128.0 , 76 / 128.0 , 77 / 128.0 , 78 / 128.0 , 79 / 128.0 
, 80 / 128.0 , 81 / 128.0 , 82 / 128.0 , 83 / 128.0 , 84 / 128.0 , 85 / 128.0 , 86 / 128.0 , 87 / 128.0 , 88 / 128.0 , 89 / 128.0 , 90 / 128.0 , 91 / 128.0 , 92 / 128.0 , 93 / 128.0 , 94 / 128.0 , 95 / 128.0 
, 96 / 128.0 , 97 / 128.0 , 98 / 128.0 , 99 / 128.0 , 100 / 128.0 , 101 / 128.0 , 102 / 128.0 , 103 / 128.0 , 104 / 128.0 , 105 / 128.0 , 106 / 128.0 , 107 / 128.0 , 108 / 128.0 , 109 / 128.0 , 110 / 128.0 , 111 / 128.0 
, 112 / 128.0 , 113 / 128.0 , 114 / 128.0 , 115 / 128.0 , 116 / 128.0 , 117 / 128.0 , 118 / 128.0 , 119 / 128.0 , 120 / 128.0 , 121 / 128.0 , 122 / 128.0 , 123 / 128.0 , 124 / 128.0 , 125 / 128.0 , 126 / 128.0 , 127 / 128.0 };

//	These functions are typical for network use
void	rtl_tcp_client::readData() {
uint8_t	buffer [8192];
std::complex<float> localBuffer [4096];

	while (toServer. bytesAvailable() > 8192) {
	   toServer. read ((char *)buffer, 8192);
	   for (int i = 0; i < 4096; i ++)
	      localBuffer [i] = std::complex<float> (
	                                    mapTable [buffer [2 * i]],
	                                    mapTable [buffer [2 * i + 1]]);
	   _I_Buffer. putDataIntoBuffer (localBuffer, 4096);
	}
}
//
//
//	commands are packed in 5 bytes, one "command byte" 
//	and an integer parameter
struct command {
	unsigned char cmd;
	unsigned int param;
}__attribute__((packed));

#define	ONE_BYTE	8

void	rtl_tcp_client::sendCommand (uint8_t cmd, int32_t param) {
QByteArray datagram;

	datagram. resize (5);
	datagram [0] = cmd;		// command to set rate
	datagram [4] = param & 0xFF;  //lsb last
	datagram [3] = (param >> ONE_BYTE) & 0xFF;
	datagram [2] = (param >> (2 * ONE_BYTE)) & 0xFF;
	datagram [1] = (param >> (3 * ONE_BYTE)) & 0xFF;
	toServer. write (datagram. data(), datagram. size());
}

void rtl_tcp_client::sendVFO (int32_t frequency) {
	sendCommand (0x01, frequency);
}

void	rtl_tcp_client::sendRate (int32_t theRate) {
	sendCommand (0x02, theRate);
}

void	rtl_tcp_client::setGainMode (int32_t gainMode) {
	sendCommand (0x03, gainMode);
}

void	rtl_tcp_client::sendGain (int gain) {
	sendCommand (0x04, 10 * gain);
	theGain		= gain;
}

//	correction is in ppm
void	rtl_tcp_client::set_fCorrection	(int32_t ppm) {
	sendCommand (0x05, ppm);
	thePpm		= ppm;
}

void	rtl_tcp_client::setDisconnect() {
	if (connected) {		// close previous connection
	   stopReader();
	   QString peerAddress	= toServer. peerAddress (). toString ();
	   store (remoteSettings, RTL_TCP_CLIENT,
	                               "remote-server", peerAddress);
	   store (remoteSettings, RTL_TCP_CLIENT,
	                               "rtl_tcp_client-gain", theGain);
	   store (remoteSettings, RTL_TCP_CLIENT,
	                                "rtl_tcp_client-ppm", thePpm);
	   toServer. close();
	}
	connected	= false;
	theState	-> setText ("disconnected");
}

void	rtl_tcp_client::set_Offset	(int32_t o) {
	sendCommand (0x0a, Khz (o));
	vfoOffset	= o;
}

bool	rtl_tcp_client::isFileInput	() {
	return false;
}

QString	rtl_tcp_client::deviceName	() {
	return "rtl_tcp_client";
}

