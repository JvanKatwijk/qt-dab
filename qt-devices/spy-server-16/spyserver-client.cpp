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
 *
 *    A simple client for syyServer
 *	
 *	Inspired by the spyserver client from Mike Weber
 *	and some functions are copied.
 *	The code is simplified since Qt-DAB functions best with
 *	16 bit codes and a samplerate of 2048000 S/s
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
#include	<samplerate.h>
#include	"device-exceptions.h"
#include	"spyserver-client.h"

#define	DEFAULT_FREQUENCY	(Khz (227360))

	spyServer_client::spyServer_client	(QSettings *s):
	                                        _I_Buffer (32 * 32768),
	                                        tmpBuffer (32 * 32768) {
	spyServer_settings	= s;
	setupUi (&myFrame);
	myFrame. show		();

    //	setting the defaults and constants
	spyServer_settings	-> beginGroup ("spyServer_client");
	settings. gain		= spyServer_settings ->
	                          value ("spyServer-gain", 20). toInt();
	settings. auto_gain	= spyServer_settings ->
	                          value ("spyServer-auto_gain", 0). toInt();
	if (settings. auto_gain != 0)
	   autogain_selector	-> setChecked (true);
	settings. basePort	= spyServer_settings -> value ("spyServer+port", 5555).toInt();
	spyServer_settings	-> endGroup();
	spyServer_gain	-> setValue (theGain);
	lastFrequency	= DEFAULT_FREQUENCY;
	connected	= false;
	theServer	= nullptr;
	hostLineEdit 	= new QLineEdit (nullptr);
	dumping		= false;
	settings. resample_quality	= 2;
	settings. batchSize		= 4096;
	settings. sample_bits		= 16;
//
	connect (spyServer_connect, SIGNAL (clicked ()),
	         this, SLOT (wantConnect ()));
	connect (spyServer_gain, SIGNAL (valueChanged (int)),
	         this, SLOT (setGain (int)));
	theState	-> setText ("waiting to start");
}

	spyServer_client::~spyServer_client () {
	spyServer_settings ->  beginGroup ("spyServer_client");
	if (connected) {		// close previous connection
	   stopReader();
	   connected = false;
	}
	spyServer_settings -> setValue ("spyServer_client-gain", 
	                                            settings. gain);
	spyServer_settings -> endGroup();
	if (theServer != nullptr)
	   delete theServer;
	delete	hostLineEdit;
}
//
void	spyServer_client::wantConnect () {
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
	spyServer_settings -> beginGroup ("spyServer_client");
	ipAddress = spyServer_settings ->
	                value ("remote-server", ipAddress). toString();
	spyServer_settings -> endGroup();
	hostLineEdit	-> setText (ipAddress);

	hostLineEdit	-> setInputMask ("000.000.000.000");
//	Setting default IP address
	hostLineEdit	-> show();
	theState	-> setText ("Enter IP address, \nthen press return");
	connect (hostLineEdit, SIGNAL (returnPressed (void)),
	         this, SLOT (setConnection (void)));
}

//	if/when a return is pressed in the line edit,
//	a signal appears and we are able to collect the
//	inserted text. The format is the IP-V4 format.
//	Using this text, we try to connect,
void	spyServer_client::setConnection () {
QString s	= hostLineEdit -> text();
QString theAddress	= QHostAddress (s). toString ();
	onConnect. store (false);
	theServer	= nullptr;
	try {
	   theServer	= new spyHandler (this, theAddress,
	                                  (int)settings. basePort,
	                                   &tmpBuffer);
	} catch (...) {
	   QMessageBox::warning (nullptr, tr ("Warning"),
                                          tr ("Connection failed"));
	   return;
	}
	if (theServer == nullptr) {
	   fprintf (stderr, "Connecting failed\n");
	   return;
	}
	fprintf (stderr, "We wachten op de spyHandler\n");

	connect (&checkTimer, SIGNAL (timeout ()),
	         this, SLOT (handle_checkTimer ()));
	checkTimer. start (2000);
	timedOut	= false;
	while (!onConnect. load () && !timedOut) 
	   usleep (1000);
	if (timedOut) {
	   QMessageBox::warning (nullptr, tr ("Warning"),
                                          tr ("no answers, fatal"));
	   delete theServer;
	   connected = false;
	   return;
	}
	checkTimer. stop ();	
	disconnect (&checkTimer, SIGNAL (timeout ()),
	            this, SLOT (handle_checkTimer ()));
//	fprintf (stderr, "We kunnen echt beginnen\n");
	theServer	-> connection_set ();

//	fprintf (stderr, "going to ask for device info\n");
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
	   theState -> setText ("Invalid device");
	   return;
	}

	this -> deviceNumber	-> setText (QString::number (theDevice. DeviceSerial));
	uint32_t max_samp_rate	= theDevice. MaximumSampleRate;
	uint32_t decim_stages	= theDevice. DecimationStageCount;
	int desired_decim_stage = -1;
	double resample_ratio	= 1.0;

	if (max_samp_rate == 0) {
	   delete theServer;
	   theServer	= nullptr;
	   return;
	}
	for (uint16_t i = 0; i < decim_stages; ++i ) {
	   uint32_t cand_rate = (uint32_t)(max_samp_rate / (1 << i));
	   if (cand_rate == INPUT_RATE) {
	      desired_decim_stage = i;
	      resample_ratio = INPUT_RATE / (double)cand_rate;
	      break;
	   } else
	   if (cand_rate > INPUT_RATE) {
//	remember the next-largest rate that is available
	      desired_decim_stage = i;
	      resample_ratio = INPUT_RATE / (double)cand_rate;
	      settings. sample_rate = cand_rate;
	   }

	   if (desired_decim_stage < 0) {
	      delete theServer;
	      theServer	= nullptr;
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

	int maxGain		= theDevice. MaximumGainIndex;
	spyServer_gain		-> setMaximum (maxGain);
	settings. resample_ratio	= resample_ratio;
	settings. desired_decim_stage	= desired_decim_stage;
	connected	= true;
	fprintf (stderr, "going to set samplerate stage %d\n",
	                                    desired_decim_stage);
	if (!theServer -> set_sample_rate_by_decim_stage (
	                                    desired_decim_stage)) {
	   std::cerr << "Failed to set sample rate " <<
	                           desired_decim_stage << "\n";
	   return;
	}

	theServer	-> set_gain_mode (settings. auto_gain != 0, 0);

	disconnect (spyServer_connect, SIGNAL (clicked ()),
	            this, SLOT (wantConnect ()));
	fprintf (stderr, "The samplerate = %f\n",
	                      (float)(theServer -> get_sample_rate ()));
	theState	-> setText ("connected");
//	start ();		// start the reader
	in_f		= nullptr;
	out_f	= nullptr;
	inputLimit	= settings. batchSize;
	outputLimit	= settings. batchSize;
	resampler	= nullptr;
	totalSamples	= 0;
//	Since we are down sampling, creating an outputbuffer with the
//	same size as the input buffer is OK
	if (settings. resample_ratio != 1.0 ) {
	   int error;
	   in_f	= new float [inputLimit * 2];
	   out_f = new float [outputLimit * 2];
	   data. data_in = in_f;
	   data. data_out = out_f;
	   data. end_of_input = 0;
	   data. src_ratio = settings. resample_ratio;
	   data. output_frames = settings. batchSize * data. src_ratio;
	   resampler	= src_new (settings. resample_quality, 2 , &error);
	}
}

int32_t	spyServer_client::getRate	() {
	return INPUT_RATE;
}

bool	spyServer_client::restartReader	(int32_t freq) {
	if (!connected)
	   return false;
	std::cerr << "spy-handler: setting center_freq to " <<
	                                            freq << std::endl;
	if (!theServer -> set_iq_center_freq (freq)) {
	   std::cerr << "Failed to set freq\n";
	   return false;
	}
	if (!theServer -> set_gain (settings. gain)) {
	   std::cerr << "Failed to set gain\n";
	   return false;
	}
	theServer -> start_running ();
	running	= true;
	return true;
}

void	spyServer_client::stopReader() {
	fprintf (stderr, "stopReader is called\n");
	if (theServer == nullptr)
	   return;
	if (!connected || !running)	// seems double???
	   return;
	if (!theServer -> is_streaming ())
	   return;
	theServer	-> stop_running ();
	running		= false;
}
//
//
int32_t	spyServer_client::getSamples (std::complex<float> *V, int32_t size) { 
int amount	= 0;
	amount =  _I_Buffer. getDataFromBuffer (V, size);
	return amount;
}

int32_t	spyServer_client::Samples () {
	return  _I_Buffer. GetRingBufferReadAvailable ();
}
//
int16_t	spyServer_client::bitDepth () {
	return 12;
}

void	spyServer_client::setGain	(int gain) {
	settings. gain = gain;
	if (!theServer -> set_gain (settings.gain)) {
	   std::cerr << "Failed to set gain\n";
	   return;
	}
}

void	spyServer_client::handle_autogain	(int d) {
	(void)d;
	int x = autogain_selector -> isChecked ();
	settings. auto_gain	= x != 0;
	spyServer_settings	-> beginGroup ("spyServer_client");
	spyServer_settings	-> setValue ("spyServer-auto_gain", x);
	if (connected)
	   theServer -> set_gain_mode (d != x, 0);
}

void	spyServer_client::connect_on () {
	fprintf (stderr, "onConnect gezet!!!!!!!!!!\n");
	onConnect. store (true);
}

void	spyServer_client::data_ready	() {
int16_t buffer_16 [settings. batchSize * 2];
static int fillP	= 0;
	while (connected && 
	          (tmpBuffer. GetRingBufferReadAvailable () > 2 * settings. batchSize)) {
	   uint32_t samps =	
	            tmpBuffer. getDataFromBuffer (buffer_16,
	                                         2 * settings. batchSize) / 2;
	   if (!running)
	      continue;

	   if (settings. resample_ratio != 1) {
	      for (uint32_t i = 0; i < 2 * samps; i ++) {
	         in_f [fillP ++] =
	                  (float) (buffer_16 [i])/ 32768.0;
	         if (fillP <  2 * inputLimit) 
	            continue;
	         data. input_frames = inputLimit;
	         error = src_process (resampler, &data);
	         if (error != 0) {
	            std::cerr << "Resampler process error: " <<
	                                       src_strerror(error) << std::endl;
	            continue;
	         }
	         _I_Buffer. putDataIntoBuffer ((std::complex<float> *)out_f,
	                                         data. output_frames);
	         for (fillP = 0; 
	              fillP < 2 * (inputLimit - data. input_frames_used); fillP ++) {
	            in_f [fillP] = in_f [2 * data. input_frames_used + fillP];
	            fillP ++;
	         }
	      }
	   }
	   else {	// no resmpling
	      std::complex<float> outB [samps];
	      for (uint32_t i = 0; i < samps; i ++) 
	         outB [i] = std::complex<float> (
	                    buffer_16 [2 * i] / 32768.0,
	                    buffer_16 [2 * i + 1] / 32760.0);
	      _I_Buffer. putDataIntoBuffer (outB, samps);
	   }
	}
}

void	spyServer_client::handle_checkTimer () {
	timedOut = true;
}

