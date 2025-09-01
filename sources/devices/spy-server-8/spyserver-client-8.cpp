#
/*
 *    Copyright (C) 2016 .. 2023
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
 *    A simple client for spyServer
 *	
 *	Inspired by the spyserver client from Mike Weber
 *	and some functions are copied.
 *	The code is simplified since Qt-DAB functions best with
 *	16 bit codes and a samplerate of  SAMPLERATE (2048000) S/s
 *	for Functions copied (more or less) from Mike weber's version
 *	copyrights are gratefully acknowledged
 */

#include	<QtNetwork>
#include	<QSettings>
#include	<QLabel>
#include	<QMessageBox>
#include	<QTcpSocket>
#include	<QDir>
#include	"dab-constants.h"
#include	"device-exceptions.h"
#include	"spyserver-client-8.h"
#include	"position-handler.h"
#include	"settings-handler.h"

#include	"xml-filewriter.h"
#include	"errorlog.h"

#define	DEFAULT_FREQUENCY	(Khz (227360))
#define	SPY_SERVER_8_SETTINGS	"SPY_SERVER_8_SETTINGS"

	spyServer_client_8::spyServer_client_8	(QSettings *s,
	                                         const QString &recorder,
	                                         errorLogger *theLogger):
	                                            _I_Buffer (32 * 32768),
	                                            tmpBuffer (32 * 32768) {
	spyServer_settings	= s;
	recorderVersion		= recorder;
	theErrorLogger		= theLogger;
	setupUi (&myFrame);
	setPositionAndSize (s, &myFrame, SPY_SERVER_8_SETTINGS);
	myFrame. show		();

    //	setting the defaults and constants
	settings. gain		= value_i (spyServer_settings,
	                                   SPY_SERVER_8_SETTINGS,
	                                   "spyServer-gain", 20);
	settings. auto_gain     = value_i (spyServer_settings,
                                           SPY_SERVER_8_SETTINGS,
                                           "spyServer-auto_gain", 0);
	settings. basePort	= value_i (spyServer_settings,
	                                   SPY_SERVER_8_SETTINGS,
	                                   "spyServer+port", 5555);
	portNumber      -> setValue     (settings. basePort); 
        if (settings. auto_gain != 0)
           autogain_selector    -> setChecked (true);
	spyServer_gain	-> setValue (theGain);
	connected	= false;
	dumping		= false;
	settings. resample_quality	= 2;
	settings. batchSize		= 4096;
	settings. sample_bits		= 16;
//
	for (int i = 0; i < 256; i ++)
	   convTable [i] = ((float)i - 127.38) / 128.0;

	connect (spyServer_connect, &QPushButton::clicked,
                 this, &spyServer_client_8::wantConnect);
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
        connect (autogain_selector, &QCheckBox::checkStateChanged,
#else
        connect (autogain_selector, &QCheckBox::stateChanged,
#endif
                 this, &spyServer_client_8::handle_autogain);
        connect (spyServer_gain, qOverload<int>(&QSpinBox::valueChanged),
                 this, &spyServer_client_8::setGain);
        connect (portNumber, qOverload<int>(&QSpinBox::valueChanged),
                 this, &spyServer_client_8::set_portNumber);
	connect (xml_dumpButton, &QPushButton::clicked,
	         this, &spyServer_client_8::set_xmlDump);
	theState	-> setText ("waiting to start");
	xml_dumpButton	-> setText ("Dump to xml");

	xmlWriter	= nullptr;
	xml_dumping. store (false);
}

	spyServer_client_8::~spyServer_client_8 () {
	if (connected) {		// close previous connection
	   stopReader();
	
	   if (!theServer. isNull ()) {
	      theServer -> stop_running ();
	      theServer. reset ();
	   }
	   connected = false;
	}
	store (spyServer_settings, SPY_SERVER_8_SETTINGS,
	                         "spyServer_client-gain", settings. gain);
	if (!theServer. isNull ())
	   theServer. reset ();
	storeWidgetPosition (spyServer_settings, &myFrame,
	                                 SPY_SERVER_8_SETTINGS);
}
//
void	spyServer_client_8::wantConnect () {
QString ipAddress;
QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

	if (connected) {
	   stopReader ();
	   if (!theServer. isNull ()) {	// it better be
	      theServer -> stop_running ();
	      theServer. reset ();
	   }
	   spyServer_connect	-> setText ("connect");
	   theState	-> setText ("Enter IP address, \nthen press return");
	   return;
	}
	// use the first non-localhost IPv4 address
	for (uint16_t i = 0; i < ipAddressesList.size (); i ++) {
	   if (ipAddressesList.at (i) != QHostAddress::LocalHost &&
	      ipAddressesList. at (i). toIPv4Address()) {
	      ipAddress = ipAddressesList. at(i). toString();
	      break;
	   }
	}
	// if we did not find one, use IPv4 localhost
	if (ipAddress. isEmpty())
	   ipAddress = QHostAddress (QHostAddress::LocalHost).toString();
	ipAddress	= value_s (spyServer_settings,
	                           SPY_SERVER_8_SETTINGS,
	                           "remote-server", ipAddress);
	hostLineEdit ->  setText (ipAddress);

	hostLineEdit -> setInputMask ("000.000.000.000");
//	Setting default IP address
	theState	-> setText ("Enter IP address, \nthen press return");
	connect (hostLineEdit, &QLineEdit::returnPressed,
	         this, &spyServer_client_8::setConnection);
}

//	if/when a return is pressed in the line edit,
//	a signal appears and we are able to collect the
//	inserted text. The format is the IP-V4 format.
//	Using this text, we try to connect,
void	spyServer_client_8::setConnection () {
QString s	= hostLineEdit -> text();
QString theAddress	= QHostAddress (s). toString ();
	onConnect. store (false);
	settings. basePort	= portNumber -> value ();
	try {
	   theServer. reset (new spyHandler_8 (this, theAddress,
	                                    (int)settings. basePort,
	                                    &tmpBuffer));
	} catch (...) {
	   theServer. reset ();		// ???
	   QMessageBox::warning (nullptr, tr ("Warning"),
                                          tr ("Connection failed"));
	   QString t = "Connecting to " + theAddress + " failed";
	   theErrorLogger  -> add ("spyServer-8", t);
	   return;
	}

	if (theServer. isNull ()) {
	   theErrorLogger -> add ("spyServer-8", "Connecting failed");
	   return;
	}

	connect (&checkTimer, &QTimer::timeout,
	         this, &spyServer_client_8::handle_checkTimer);
	timedOut	= false;
	checkTimer. start (1000);
//
//	If the server connects but could not find a suitable
//	device, connection stands but no data is transferred
	int delay	= 2000;		// 2 seconds
	while (!onConnect. load () && !timedOut) {
	   delay --;
	   if (delay == 0) {
	      checkTimer. stop ();
	      timedOut	= true;
	      break;
	   }
	   usleep (1000);
	}

	if (timedOut) {
	   theState	-> setText ("No connection or no device");
	   theServer. reset ();
	   connected = false;
	   return;
	}

	checkTimer. stop ();	
	disconnect (&checkTimer, &QTimer::timeout,
	            this, &spyServer_client_8::handle_checkTimer);
	fprintf (stderr, "WE HAVE A CONNECTION\n");
	theServer	-> connection_set ();

	spyServer_connect	-> setText ("disconnect");
	struct DeviceInfo theDevice;
	theServer	-> get_deviceInfo (theDevice);

	if (theDevice. DeviceType == DEVICE_AIRSPY_ONE) {
	   nameOfDevice	-> setText ("Airspy One");
	}
	else
	if (theDevice. DeviceType == DEVICE_RTLSDR) {
	   nameOfDevice	-> setText ("RTLSDR");
	}
	else {
	   theState -> setText ("not supported device");
	   return;
	}

	if (theDevice. DeviceSerial == 0)
	   this -> deviceNumber -> hide ();
	else
	   this -> deviceNumber	-> setText (QString::number (theDevice. DeviceSerial));
	uint32_t max_samp_rate	= theDevice. MaximumSampleRate;
	uint32_t decim_stages	= theDevice. DecimationStageCount;
	int desired_decim_stage = -1;
	double resample_ratio	= 1.0;

	if (max_samp_rate == 0) {
	   theServer. reset ();
	   return;
	}

	targetRate	= 0;
	for (uint16_t i = 0; i < decim_stages; ++i ) {
	   targetRate = (uint32_t)(max_samp_rate / (1 << i));
	   if (targetRate == SAMPLERATE) {
	      desired_decim_stage = i;
	      resample_ratio = 1;
	      break;
	   } else
	   if (targetRate > SAMPLERATE) {
	      if ((i < decim_stages - 1) &&
	          ((uint32_t) max_samp_rate / (i << (i + 1)))) {
	         desired_decim_stage = i;
	         resample_ratio = SAMPLERATE / ((double)targetRate);
	         settings. sample_rate = targetRate;
	         break;
	      }	
	   } else
           if (targetRate < SAMPLERATE) {
              desired_decim_stage = i;
              resample_ratio = SAMPLERATE / (double)targetRate;
              settings. sample_rate = targetRate;
	      break;
           }

	   if (desired_decim_stage < 0) {
	      theServer. reset ();
	      return;
	   }

/*
	   std::cerr << "Desired decimation stage: " <<
	                 desired_decim_stage <<
	                 " (" << max_samp_rate << " / " <<
	                 (1 << desired_decim_stage) <<
	                 " = " << max_samp_rate / (1 <<
	                 desired_decim_stage) << ") resample ratio: " <<
	                 resample_ratio << std::endl;
 */
	}

	rateLabel		-> setText (QString::number (targetRate));
	if (targetRate < 1800000) {
	   theState -> setText ("Rate Low");
	   theServer. reset ();
	   return;	
	}
	int maxGain		= theDevice. MaximumGainIndex;
	spyServer_gain		-> setMaximum (maxGain);
	settings. resample_ratio	= resample_ratio;
	settings. desired_decim_stage	= desired_decim_stage;
	connected	= true;
	theState	-> setText ("Connected");
	fprintf (stderr, "going to set samplerate stage %d\n",
	                                    desired_decim_stage);
	if (!theServer -> set_sample_rate_by_decim_stage (
	                                    desired_decim_stage)) {
	   std::cerr << "Failed to set sample rate " <<
	                           desired_decim_stage << "\n";
	   return;
	}

//	disconnect (spyServer_connect, &QPushButton::clicked,
//	            this, &spyServer_client_8::wantConnect);
	fprintf (stderr, "The samplerate = %f\n",
	                      (float)(theServer -> get_sample_rate ()));
	theState	-> setText ("connected");

//	Since we are down sampling, creating an outputbuffer with the
//	same size as the input buffer is OK
	if (settings. resample_ratio != 1.0 ) {
//	we process chunks of 1 msec
	   convBufferSize          = settings. sample_rate / 1000;
	   convBuffer. resize (convBufferSize + 1);
	   float samplesPerMsec = SAMPLERATE / 1000.0;
	   for (int i = 0; i < SAMPLERATE / 1000; i ++) {
	      float inVal  = float (settings. sample_rate / 1000);
	      mapTable_int [i]     = int (floor (i * (inVal / samplesPerMsec)));
	      mapTable_float [i]   = i * (inVal / samplesPerMsec) - mapTable_int [i];
	   }
	   convIndex       = 0;
	}
}

int32_t	spyServer_client_8::getRate	() {
	return targetRate;
}

bool	spyServer_client_8::restartReader	(int32_t freq, int skipped) {
	if (!connected)
	   return false;
	std::cerr << "spy-handler: setting center_freq to " <<
	                                            freq << std::endl;
	if (!theServer -> set_iq_center_freq (freq)) {
	   theState -> setText ("Setting freq to" + QString::number (freq) + "failed");
	   return false;
	}
	if (!theServer -> set_gain (settings. gain)) {
	   std::cerr << "Failed to set gain\n";
	   return false;
	}
	toSkip	= skipped;
	theServer -> start_running ();
	running	= true;
	return true;
}

void	spyServer_client_8::stopReader	() {
	fprintf (stderr, "stopReader is called\n");
	if (theServer. isNull ())
	   return;
	if (!theServer -> is_streaming ())
	   return;
	close_xmlDump ();
	theState -> setText ("Trying to stop server");
	fprintf (stderr, "Tell the server to stop running\n");
	theServer	-> stop_running ();
	running		= false;
}
//
//
int32_t	spyServer_client_8::getSamples (std::complex<float> *V, int32_t size) { 
int amount	= 0;
	amount =  _I_Buffer. getDataFromBuffer (V, size);
	return amount;
}

int32_t	spyServer_client_8::Samples () {
	return  _I_Buffer. GetRingBufferReadAvailable ();
}

int16_t	spyServer_client_8::bitDepth () {
	return 8;
}

void	spyServer_client_8::setGain	(int gain) {
	settings. gain = gain;
	store (spyServer_settings, SPY_SERVER_8_SETTINGS,
                                      "spyServer_client-gain", settings. gain);
	if (theServer. isNull ())
	   return;
	if (!theServer -> set_gain (settings.gain)) {
	   std::cerr << "Failed to set gain\n";
	   return;
	}
}

void    spyServer_client_8::handle_autogain       (int d) {
        (void)d;
        int x = autogain_selector -> isChecked ();
        settings. auto_gain     = x != 0;
        store (spyServer_settings, SPY_SERVER_8_SETTINGS,
                                     "spyServer-auto_gain", x ? 1 : 0);
	if (theServer. isNull ())
	   return;
        if (connected)
           theServer -> set_gain_mode (d != x, 0);
}

void	spyServer_client_8::connect_on () {
	onConnect. store (true);
}

void	spyServer_client_8::data_ready	() {
uint8_t buffer_8 [settings. batchSize * 2];

	while (connected && 
	          (tmpBuffer. GetRingBufferReadAvailable () > 2 * settings. batchSize)) {
	   uint32_t samps =	
	            tmpBuffer. getDataFromBuffer (buffer_8,
	                                         2 * settings. batchSize) / 2;
	   if (!running)
	      continue;

	   if (xml_dumping. load ())
	      xmlWriter -> add ((std::complex<uint8_t> *)buffer_8, 
	                                             settings. batchSize / 2);


	   if (settings. resample_ratio != 1) {
	      std::complex<float> temp [SAMPLERATE / 1000];
	      for (uint32_t i = 0; i < samps; i ++) {
                 convBuffer [convIndex ++] =
                             std::complex<float> (
                                          convTable [buffer_8 [2 * i]],
                                          convTable [buffer_8 [2 * i + 1]]);
              
                 if (convIndex > convBufferSize) {
                    for (int j = 0; j < SAMPLERATE / 1000; j ++) {
                       int16_t  inpBase    = mapTable_int [j];
                       float    inpRatio   = mapTable_float [j];
                       temp [j]    = convBuffer [inpBase + 1] * inpRatio +
                                     convBuffer [inpBase] * (1 - inpRatio);
                    }
	            if (toSkip > 0)
	               toSkip -= SAMPLERATE / 1000;
	            else
                       _I_Buffer. putDataIntoBuffer (temp, SAMPLERATE / 1000);
                    convBuffer [0] = convBuffer [convBufferSize];
                    convIndex = 1;
                 }
              }
	   }
	   else {	// no resmpling
	      std::complex<float> outB [samps];
	      for (uint32_t i = 0; i < samps; i ++) 
	         outB [i] = std::complex<float> (
	                       convTable [buffer_8 [2 * i]],
	                       convTable [buffer_8 [2 * i + 1]]);
	      _I_Buffer. putDataIntoBuffer (outB, samps);
	   }
	}
}

void	spyServer_client_8::handle_checkTimer () {
	timedOut = true;
}

void    spyServer_client_8::set_portNumber        (int v) {
        settings. basePort = v;
        store (spyServer_settings, SPY_SERVER_8_SETTINGS,  
                                 "spyServer-port", v);
}

void	spyServer_client_8::set_xmlDump () {
	if (!xml_dumping. load ()) {
	   setup_xmlDump ();
	}
	else {
	   close_xmlDump ();
	}
}

bool	spyServer_client_8::setup_xmlDump () {
QString channel		= spyServer_settings -> value ("channel", "xx").
	                                                      toString ();

	xmlWriter	= nullptr;
	try {
	   xmlWriter	= new xml_fileWriter (spyServer_settings,
	                                      channel,
	                                      8,
	                                      "uint8",
	                                      2048000,
	                                      lastFrequency,
	                                      settings. gain,
	                                      "RTLSDR",
	                                      "???",
	                                      recorderVersion);
	} catch (...) {
	   return false;
	}
	xml_dumping. store (true);
	xml_dumpButton	-> setText ("writing xml file");
	return true;
}
	
void	spyServer_client_8::close_xmlDump () {
	if (xmlWriter == nullptr)
	   return;
	usleep (1000);
	xmlWriter	-> computeHeader ();
	delete xmlWriter;
	xmlWriter	= nullptr;
	xml_dumping. store (false);
	xml_dumpButton	-> setText ("Dump to xml");
}

