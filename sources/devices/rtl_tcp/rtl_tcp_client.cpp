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
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *    GNU General Public License for more details.
 P
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
#include	"dab-constants.h"
#include	"rtl_tcp_client.h"
#include	"rtl-sdr.h"
#include	"device-exceptions.h"
#include	"position-handler.h"
#include	"settings-handler.h"
#include	"errorlog.h"
#include	"xml-filewriter.h"

#if (!defined (__MINGW32__))
	#include	<netinet/in.h>
#endif

#define	RTL_TCP_SETTINGS	"rtl_tcp_settings"
#define	DEFAULT_FREQUENCY	(kHz (220000))

typedef struct {  	// 12 bytes, 3 * 4 bytes
	char Magic [4];
	uint32_t tunerType;
	uint32_t tunerGainCount;
} dongleInfo_t;

	rtl_tcp_client::rtl_tcp_client (QSettings *s,
	                                const QString &recorder,
	                                errorLogger *theLogger):
	                                        _I_Buffer (32 * 32768) {

	remoteSettings		= s;
	recorderVersion		= recorder;
	theErrorLogger		= theLogger;
	setupUi (&myFrame);
        setPositionAndSize (s, &myFrame, RTL_TCP_SETTINGS);
        myFrame. show   ();

	for (int i = 0; i < 256; i ++)
	   convTable [i] = ((float)i - 128.0) / 128.0;

	ipAddress		= "127.0.0.1";	// the default

	Gain		=
	         value_i (remoteSettings, RTL_TCP_SETTINGS, "Gain", 20);
	Ppm		=
	         value_f (remoteSettings, RTL_TCP_SETTINGS, "Ppm",  0);
	biasT		=
	         value_f (remoteSettings, RTL_TCP_SETTINGS, "biasT",  0);
	AgcMode		=
	         value_i (remoteSettings, RTL_TCP_SETTINGS, "AgcMode", 0);
	basePort 	=
	         value_i (remoteSettings, RTL_TCP_SETTINGS, "basePort",  1234);
	ipAddress	=
	         value_s (remoteSettings, RTL_TCP_SETTINGS, "remoteserver", ipAddress);

	gainSelector	-> setValue (Gain);
	PpmSelector	-> setValue (Ppm);
	agcSelector	-> setChecked (AgcMode);
	portSelector	-> setValue (basePort);
	addressSelector	-> setText (ipAddress);
	vfoFrequency	= DEFAULT_FREQUENCY;
	connected	= false;
//
	connect (tcp_connect, &QPushButton::clicked,
	         this, &rtl_tcp_client::wantConnect);
	connect (tcp_disconnect, &QPushButton::clicked,
	         this, &rtl_tcp_client::setDisconnect);
	connect (gainSelector, qOverload<int>(&QSpinBox::valueChanged),
	         this, &rtl_tcp_client::sendGain);
	connect (PpmSelector, qOverload<double>(&QDoubleSpinBox::valueChanged),
	         this, &rtl_tcp_client::set_fCorrection);
	connect (xml_dumpButton, &QPushButton::clicked,
	         this, &rtl_tcp_client::set_xmlDump);
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (agcSelector, &QCheckBox::checkStateChanged,
#else
	connect (agcSelector, &QCheckBox::stateChanged,
#endif
	         this, &rtl_tcp_client::setAgcMode);
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (biasTSelector, &QCheckBox::checkStateChanged,
#else
	connect (biasTSelector, &QCheckBox::stateChanged,
#endif
	         this, &rtl_tcp_client::setBiasT);
	connect (portSelector, qOverload<int>(&QSpinBox::valueChanged),
	         this, &rtl_tcp_client::setPort);
	connect (addressSelector, &QLineEdit::returnPressed,
	         this, &rtl_tcp_client::setAddress);

	xml_dumping. store (false);
	theState	-> setText("waiting to start");
	xml_dumpButton	-> setText ("Dump to xml");
	xmlWriter	= nullptr;
}

	rtl_tcp_client::~rtl_tcp_client () {
	storeWidgetPosition (remoteSettings, &myFrame, RTL_TCP_SETTINGS);

	stopReader ();
	store (remoteSettings, RTL_TCP_SETTINGS, "remoteserver",
	                                toServer. peerAddress (). toString ());
	store (remoteSettings, RTL_TCP_SETTINGS, "basePort", basePort);
	store (remoteSettings, RTL_TCP_SETTINGS, "Gain", Gain);
	store (remoteSettings, RTL_TCP_SETTINGS, "Ppm", Ppm);
	store (remoteSettings, RTL_TCP_SETTINGS, "AgcMode", AgcMode);
	store (remoteSettings, RTL_TCP_SETTINGS, "biasT", biasT);
	toServer. close ();
	connected = false;
	myFrame. hide ();
	toServer. close ();
}

void	rtl_tcp_client::wantConnect () {
	if (connected) {
	   setDisconnect ();
	}

	QString s = ipAddress;
	toServer. connectToHost (QHostAddress (s), basePort);
	if (!toServer.waitForConnected(2000)) {
	   QMessageBox::warning(&myFrame,
	                      tr("sdr"), tr("connection failed\n"));
	   QString t = "Connection to " + s + "failed";
	   theErrorLogger -> add ("RTL_TCP", t);
	   return;
	}

	connected	= true;
	theState -> setText ("connected");
	sendRate	(SAMPLERATE);
	setAgcMode	(AgcMode);
	set_fCorrection (Ppm);
	setBandwidth	(1536000);
	setBiasT	(biasT);
	toServer. waitForBytesWritten ();
	dongleInfoIn	= false;
}

bool	rtl_tcp_client::restartReader (int32_t freq, int skipped) {
	if (!connected)
	   return true;
	vfoFrequency = freq;
	this -> toSkip = skipped;
//	here the command to set the frequency
	sendVFO (freq);
	connect (&toServer, &QIODevice::readyRead,
	         this, &rtl_tcp_client::readData);
	return true;
}

void	rtl_tcp_client::stopReader	() {

	if (!connected)
	   return;
	close_xmlDump ();
	disconnect (&toServer, &QIODevice::readyRead,
	            this, &rtl_tcp_client::readData);
	_I_Buffer. FlushRingBuffer ();
}

//
//	The brave old getSamples.For the dab stick, we get
//	size: still in I/Q pairs, but we have to convert the data from
//	uint8_t to std::complex<float>
int32_t	rtl_tcp_client::getSamples (std::complex<float> *V, int32_t size) {
	return  _I_Buffer. getDataFromBuffer (V, size);
}

int32_t	rtl_tcp_client::Samples		() {
	return _I_Buffer. GetRingBufferReadAvailable ();
}

int16_t	rtl_tcp_client::bitDepth	() {
	return 8;
}

#define	SEGMENT_SIZE 4096
//	These functions are typical for network use
void	rtl_tcp_client::readData () {
uint8_t buffer [2 * SEGMENT_SIZE];
std::complex<float> localBuffer [SEGMENT_SIZE];

	if (!dongleInfoIn) {
	   dongleInfo_t dongleInfo;
	   if (toServer. bytesAvailable () >=
	                                (qint64)sizeof (dongleInfo)) {
	      toServer. read ((char *)&dongleInfo, sizeof (dongleInfo));
	      dongleInfoIn = true;
	      if (memcmp (dongleInfo. Magic, "RTL0", 4) == 0) {
	         switch (htonl (dongleInfo. tunerType)) {
	            case RTLSDR_TUNER_E4000:
	               tunerText = "E4000";
                       break;
                    case RTLSDR_TUNER_FC0012:
                       tunerText = "FC0012";
                       break;
                    case RTLSDR_TUNER_FC0013:
                       tunerText = "FC0013";
                       break;
                    case RTLSDR_TUNER_FC2580:
                       tunerText = "FC2580";
                       break;
                    case RTLSDR_TUNER_R820T:
                       tunerText = "R820T";
                       break;
                    case RTLSDR_TUNER_R828D:
                       tunerText = "R828D";
                       break;
                    default:
                      tunerText = "unknown";
	              break;
	         }
	         tunerLabel -> setText (tunerText);
	         fprintf (stderr, "gainType %X\n", dongleInfo. tunerGainCount);
	      }
	   }
	}

	if (dongleInfoIn) {
	   if (xml_dumping. load ())
	      xmlWriter -> add ((std::complex<uint8_t> *)buffer, SEGMENT_SIZE);

	   while (toServer. bytesAvailable() > 2 * SEGMENT_SIZE) {
	      toServer. read ((char *)buffer, SEGMENT_SIZE);
	      for (int i = 0; i < SEGMENT_SIZE; i ++)
	         localBuffer [i] =
	           std::complex<float> (convTable [buffer [2 * i]],
	                                convTable [buffer [2 * i + 1]]);
	      if (toSkip > 0)
	         toSkip -= SEGMENT_SIZE;
	      else 
	         _I_Buffer. putDataIntoBuffer (localBuffer, SEGMENT_SIZE);
	   }
	}
}
//
//	commands are packed in 5 bytes, one "command byte"
//	and an integer parameter
struct command {
	unsigned char cmd;
	unsigned int param;
}__attribute__((packed));

#define	ONE_BYTE	8

void rtl_tcp_client::sendCommand (uint8_t cmd, int32_t param) {
	if (connected) {
	   QByteArray datagram;
	   datagram. resize (5);
	   datagram [0] = cmd;		// command to set rate
	   datagram [4] = param & 0xFF;  //lsb last
	   datagram [3] = (param >> ONE_BYTE) & 0xFF;
	   datagram [2] = (param >> (2 * ONE_BYTE)) & 0xFF;
	   datagram [1] = (param >> (3 * ONE_BYTE)) & 0xFF;
	   toServer. write (datagram. data (), datagram. size ());
	}
}

void	rtl_tcp_client::sendVFO (int32_t frequency) {
	sendCommand (0x01, frequency);
}

void	rtl_tcp_client::sendRate (int32_t rate) {
	sendCommand (0x02, rate);
}

void	rtl_tcp_client::setAgcMode (int agc) {
	(void)agc;
	bool b	= agcSelector -> isChecked ();
	sendCommand (0x03, b);
}
void	rtl_tcp_client::sendGain (int gain) {
	Gain	= gain;
	sendCommand (0x04, 10 * gain);
}

//	correction is in ppm
void	rtl_tcp_client::set_fCorrection (double ppm) {
	Ppm = ppm;
	sendCommand (0x83, (int)(ppm * 1000));
}


void	rtl_tcp_client::setBiasT (int biast) {
	biasT = biasTSelector -> isChecked () ? 1 : 0;
	sendCommand (0x0e, biast);
}

void	rtl_tcp_client::setBandwidth (int bw) {
	sendCommand (0x40, bw);
}

void	rtl_tcp_client::setPort (int port) {
	basePort = port;
}

void	rtl_tcp_client::setAddress () {
	addressSelector -> setInputMask ("000.000.000.000");
	ipAddress = addressSelector -> text ();
}

void	rtl_tcp_client::setDisconnect () {
	if (!connected)
	   return;

	stopReader ();
	store (remoteSettings, RTL_TCP_SETTINGS, "remoteserver",
	                                toServer. peerAddress (). toString ());
	store (remoteSettings, RTL_TCP_SETTINGS, "basePort", basePort);
	store (remoteSettings, RTL_TCP_SETTINGS, "Gain", Gain);
	store (remoteSettings, RTL_TCP_SETTINGS, "Ppm", Ppm);
	store (remoteSettings, RTL_TCP_SETTINGS, "AgcMode", AgcMode);
	store (remoteSettings, RTL_TCP_SETTINGS, "biasT", biasT);
	toServer. close ();
	connected = false;
	theState -> setText("disconnected");
}

bool	rtl_tcp_client::isFileInput	() {
	return false;
}

void	rtl_tcp_client::resetBuffer	() {
	_I_Buffer. FlushRingBuffer ();
}

QString rtl_tcp_client::deviceName	() {
	return "RtlTcp";
}

void	rtl_tcp_client::set_xmlDump () {
	if (!xml_dumping. load ()) {
	   setup_xmlDump (); 
	}
	else {
	   close_xmlDump ();
	}
}

bool	rtl_tcp_client::setup_xmlDump () {
QString channel		= remoteSettings -> value ("channel", "xx").
	                                                      toString ();
	xmlWriter	= nullptr;
	try {
	   xmlWriter	= new xml_fileWriter (remoteSettings,
	                                      channel,
	                                      8,
	                                      "uint8",
	                                      2048000,
	                                      lastFrequency,
	                                      100,
	                                      "RTL_TCP",
	                                      "rtl_tcp",
	                                      "rtl_tcp");
	} catch (...) {
	   return false;
	}
	xml_dumping. store (true);
	xml_dumpButton	-> setText ("writing xml file");
	return true;
}
	
void	rtl_tcp_client::close_xmlDump () {
	if (xmlWriter == nullptr)
	   return;
	usleep (1000);
	xmlWriter	-> computeHeader ();
	delete xmlWriter;
	xmlWriter	= nullptr;
	xml_dumping. store (false);
	xml_dumpButton	-> setText ("Dump to xml");
}

