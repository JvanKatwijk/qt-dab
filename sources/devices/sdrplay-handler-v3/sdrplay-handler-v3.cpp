#
/*
 *    Copyright (C) 2020 .. 2024
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation version 2 of the License.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	<QThread>
#include	<QSettings>
#include	<QLabel>
#include	<QPoint>
#include	"dab-constants.h"
#include	"sdrplay-handler-v3.h"
#include	"position-handler.h"
#include	"sdrplay-commands.h"
#include	"xml-filewriter.h"

#include	"errorlog.h"
#include	"settingNames.h"
#include	"settings-handler.h"

//	The Rsp's
#include	"Rsp-device.h"
#include	"RspI-handler.h"
#include	"Rsp1A-handler.h"
#include	"RspII-handler.h"
#include	"RspDuo-handler.h"
#include	"RspDx-handler.h"

#include	"device-exceptions.h"
#define SDRPLAY_RSP1_   1
#define SDRPLAY_RSP1A_  255
#define SDRPLAY_RSP2_   2
#define SDRPLAY_RSPduo_ 3
#define SDRPLAY_RSPdx_  4
#define SDRPLAY_RSP1B_  6
#define SDRPLAY_RSPdxR2_  7

#define	SDRPLAY_SETTINGS	"SDRPLAY_SETTINGS_V3"
#define	SDRPLAY_IFGRDB		"sdrplay-ifgrdb"
#define	SDRPLAY_LNASTATE	"sdrplay-lnastate"
#define	SDRPLAY_PPM		"sdrplay-ppm"
#define	SDRPLAY_AGCMODE		"sdrplay_agcMode"
#define	SDRPLAY_BIAS_T		"biasT_selector"
#define	SDRPLAY_NOTCH		"notch_selector"
#define	SDRPLAY_ANTENNA		"Antenna"
#define	SDRPLAY_TUNER		"tuner"

std::string errorMessage (int errorCode) {
	switch (errorCode) {
	   case 1:
	      return std::string ("Could not fetch library");
	   case 2:
	      return std::string ("error in fetching functions from library");
	   case 3:
	      return std::string ("sdrplay_api_Open failed");
	   case 4:
	      return std::string ("could not open sdrplay_api_ApiVersion");
	   case 5:
	      return std::string ("API versions do not match");
	   case 6:
	      return std::string ("sdrplay_api_GetDevices failed");
	   case 7:
	      return std::string ("no valid SDRplay device found");
	   case 8:
	      return std::string ("sdrplay_api_SelectDevice failed");
	   case 9:
	      return std::string ("sdrplay_api_GetDeviceParams failed");
	   case 10:
	      return std::string ("sdrplay_api+GetDeviceParams returns null");
	   default:
	      return std::string ("unidentified error with sdrplay device");
	}
	return "";
}

	sdrplayHandler_v3::
	           sdrplayHandler_v3  (QSettings *s,
	                               const QString &recorderVersion,
	                               errorLogger *theLogger):
	                                          _I_Buffer (4 * 1024 * 1024) {
	sdrplaySettings			= s;
	this	-> recorderVersion	= recorderVersion;
	theErrorLogger			= theLogger;
        setupUi (&myFrame);
	QString	groupName	= SDRPLAY_SETTINGS;
	setPositionAndSize (s, &myFrame, groupName);
	myFrame. show	();

	xmlWriter		= nullptr;
	overloadLabel -> setStyleSheet ("QLabel {background-color : green}");
	antennaSelector		-> hide	();
	tunerSelector		-> hide	();
	nrBits			= 12;	// default
	denominator		= 2048.0f;	// default

	dumping. store	(false);
//	See if there are settings from previous incarnations
//	and config stuff

	GRdBSelector 		-> setValue (
	            value_i (sdrplaySettings, SDRPLAY_SETTINGS,
	                                          SDRPLAY_IFGRDB, 20));
	GRdBValue		= GRdBSelector -> value ();

	lnaGainSetting		-> setValue (
	            value_i (sdrplaySettings, SDRPLAY_SETTINGS,
	                                          SDRPLAY_LNASTATE, 4));

	lnaState		= lnaGainSetting -> value ();

	ppmControl		-> setValue (
	            value_f (sdrplaySettings, SDRPLAY_SETTINGS,
	                                          SDRPLAY_PPM, 0.0));
	ppmValue		= ppmControl -> value ();

	agcMode		= value_i (sdrplaySettings, SDRPLAY_SETTINGS,
	                                      SDRPLAY_AGCMODE, 0) != 0;

	if (agcMode) {
	   agcControl -> setChecked (true);
	   GRdBSelector         -> hide ();
	   gainsliderLabel      -> hide ();
	}

	biasT           =
               value_i (sdrplaySettings, SDRPLAY_SETTINGS,
	                                      SDRPLAY_BIAS_T, 0) != 0;
        if (biasT)
           biasT_selector -> setChecked (true);

	bool notch	=
               value_i (sdrplaySettings, SDRPLAY_SETTINGS,
	                                      SDRPLAY_NOTCH, 0) != 0;
	if (notch)
	   notch_selector -> setChecked (true);

//	and be prepared for future changes in the settings
	connect (GRdBSelector, qOverload<int>(&QSpinBox::valueChanged),
	         this, &sdrplayHandler_v3::setIfGainReduction);
	connect (lnaGainSetting, qOverload<int>(&QSpinBox::valueChanged),
	         this, &sdrplayHandler_v3::setLnaGainReduction);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (agcControl, &QCheckBox::checkStateChanged,
#else
	connect (agcControl, &QCheckBox::stateChanged,
#endif
	         this, &sdrplayHandler_v3::setAgcControl);
	connect (ppmControl, qOverload<double>(&QDoubleSpinBox::valueChanged),
	         this, &sdrplayHandler_v3::setPpmControl);
	connect (dumpButton, &QPushButton::clicked,
                 this, &sdrplayHandler_v3::setXmlDump);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (biasT_selector, &QCheckBox::checkStateChanged,	
#else
	connect (biasT_selector, &QCheckBox::stateChanged,	
#endif
	         this, &sdrplayHandler_v3::setBiasT);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (notch_selector, &QCheckBox::checkStateChanged,	
#else
	connect (notch_selector, &QCheckBox::stateChanged,	
#endif
	         this, &sdrplayHandler_v3::setNotch);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (this, &sdrplayHandler_v3::overloadStateChanged,
#else
	connect (this, &sdrplayHandler_v3::overloadStateChanged,
#endif
	         this, &sdrplayHandler_v3::reportOverloadState);

	lastFrequency	= MHz (220);
	theGain		= -1;
	debugControl	-> hide ();
	failFlag. store (false);
	successFlag. store (false);
	errorCode	= 0;
	start ();
	while (!failFlag. load () && !successFlag. load () && isRunning ())
	   usleep (1000);
	if (failFlag. load ()) {
	   while (isRunning ())
	      usleep (1000);
	   throw device_exception (errorMessage (errorCode));
	}
	
	fprintf (stderr, "setup sdrplay v3 seems successfull\n");
}

	sdrplayHandler_v3::~sdrplayHandler_v3 () {
	closeXmlDump ();
	threadRuns. store (false);
	while (isRunning ())
	   usleep (1000);
	theRsp. reset ();
//	thread should be stopped by now
	myFrame. hide ();
	QString groupName	= SDRPLAY_SETTINGS;
	storeWidgetPosition (sdrplaySettings, &myFrame, SDRPLAY_SETTINGS);

	store (sdrplaySettings, SDRPLAY_SETTINGS,
	                          SDRPLAY_PPM, ppmControl -> value ());
	store (sdrplaySettings, SDRPLAY_SETTINGS,
	                          SDRPLAY_IFGRDB, GRdBSelector -> value ());
	store (sdrplaySettings, SDRPLAY_SETTINGS,
	                          SDRPLAY_LNASTATE, lnaGainSetting -> value ());
	store (sdrplaySettings, SDRPLAY_SETTINGS,
	                          SDRPLAY_AGCMODE, agcControl -> isChecked() ? 1 : 0);
	sdrplaySettings	-> sync();
}

/////////////////////////////////////////////////////////////////////////
//	Implementing the interface
/////////////////////////////////////////////////////////////////////////


bool	sdrplayHandler_v3::restartReader (int32_t newFreq, int skipped) {
restartRequest r (newFreq);

        if (receiverRuns. load ())
           return true;
        lastFrequency    = newFreq;
	this ->  toSkip	= skipped;
	_I_Buffer. FlushRingBuffer();
	return messageHandler (&r);
}

void	sdrplayHandler_v3::stopReader	() {
stopRequest r;
	closeXmlDump ();
        if (!receiverRuns. load ())
           return;
        messageHandler (&r);	// synchronous call
	_I_Buffer. FlushRingBuffer();
}
void	sdrplayHandler_v3::setIfGainReduction	(int GRdB) {
GRdBRequest r (GRdB);
	
	if (!receiverRuns. load ())
           return;
        messageHandler (&r);
}

void	sdrplayHandler_v3::setLnaGainReduction (int lnaState) {
lnaRequest r (lnaState);

	if (!receiverRuns. load ())
           return;
        messageHandler (&r);
}

void	sdrplayHandler_v3::setAgcControl (int dummy) {
bool    agcMode = agcControl -> isChecked ();
agcRequest r (agcMode, 30);
	(void)dummy;
        messageHandler (&r);
	if (agcMode) {
           GRdBSelector         -> hide ();
           gainsliderLabel      -> hide ();
	}
	else {
	   GRdBRequest r2 (GRdBSelector -> value ());
	   GRdBSelector		-> show ();
	   gainsliderLabel	-> show ();
	   messageHandler  (&r2);
	}
}

void	sdrplayHandler_v3::setPpmControl (int ppm) {
ppmRequest r (ppm);
        messageHandler (&r);
}

void	sdrplayHandler_v3::setBiasT (int v) {
biasT_Request r (biasT_selector -> isChecked () ? 1 : 0);

	(void)v;
	messageHandler (&r);
	store (sdrplaySettings, SDRPLAY_SETTINGS,
	                          SDRPLAY_BIAS_T,
	                              biasT_selector -> isChecked () ? 1 : 0);
}

void	sdrplayHandler_v3::setNotch (int v) {
notch_Request r (notch_selector -> isChecked () ? 1 : 0);
	(void)v;
	messageHandler (&r);
	store (sdrplaySettings, SDRPLAY_SETTINGS,
	                           SDRPLAY_NOTCH,
	                              notch_selector -> isChecked () ? 1 : 0);
}
	
void	sdrplayHandler_v3::setSelectAntenna	(const QString &s) {
	antennaRequest request (s == "Antenna A" ? 'A' :
	                        s == "Antenna B" ? 'B' : 'C');
	messageHandler (&request);
//	messageHandler (new antennaRequest (s == "Antenna A" ? 'A' :
//	                                    s == "Antenna B" ? 'B' : 'C'));
	QString ss = s;
	store (sdrplaySettings, SDRPLAY_SETTINGS, SDRPLAY_ANTENNA, ss);
}

void	sdrplayHandler_v3::setSelectTuner	(const QString &s) {
	int tuner = s == "Tuner 1" ? 1 : 2; 
	messageHandler (new tunerRequest (tuner));
	store (sdrplaySettings, SDRPLAY_SETTINGS, SDRPLAY_TUNER, tuner);
}

//
int32_t	sdrplayHandler_v3::getSamples (std::complex<float> *V, int32_t size) { 
auto *temp 	= dynVec (std::complex<int16_t>, size);
	if (!receiverRuns. load ())
	   return 0;
	int amount      = _I_Buffer. getDataFromBuffer (temp, size);
        for (int i = 0; i < amount; i ++)
           V [i] = std::complex<float> ((float)real (temp [i]) / denominator,
                                        (float)imag (temp [i]) / denominator);
        if (dumping. load ())
           xmlWriter -> add (temp, amount);
        return amount;
}

int32_t	sdrplayHandler_v3::Samples	() {
	if (!receiverRuns. load ())
	   return 0;
	return _I_Buffer. GetRingBufferReadAvailable();
}

void	sdrplayHandler_v3::resetBuffer	() {
	_I_Buffer. FlushRingBuffer();
}

int16_t	sdrplayHandler_v3::bitDepth	() {
	return nrBits;
}

QString	sdrplayHandler_v3::deviceName	() {
	return deviceModel + ":" + serial;
}

///////////////////////////////////////////////////////////////////////////
//	Handling the GUI
//////////////////////////////////////////////////////////////////////
//
//	Since the daemon is not threadproof, we have to package the
//	actual interface into its own thread.
//	Communication with that thread is synchronous!
//

void	sdrplayHandler_v3::setLnaBounds (int low, int high) {
	lnaGainSetting	-> setRange (low, high - 1);
}

void	sdrplayHandler_v3::setSerial	(const QString& s) {
	serialNumber	-> setText (s);
}

void	sdrplayHandler_v3::setApiVersion (float version) {
QString v = QString::number (version, 'r', 2);
	api_version	-> display (v);
}

void    sdrplayHandler_v3::showLnaGain (int g) {
        lnaGRdBDisplay  -> display (g);
}

void	sdrplayHandler_v3::setXmlDump () {
	if (xmlWriter == nullptr) {
	   setupXmlDump (false);
	}
	else {
	   closeXmlDump ();
	}
}

void	sdrplayHandler_v3::startDump	() {
	setupXmlDump (true);
}

void	sdrplayHandler_v3::stopDump	() {
	closeXmlDump ();
}

//
////////////////////////////////////////////////////////////////////////
//	showing data
////////////////////////////////////////////////////////////////////////
int	sdrplayHandler_v3::setAntennaSelect (int sdrDevice) {
	if ((sdrDevice == SDRPLAY_RSPdx_) || (sdrDevice == SDRPLAY_RSPdxR2_)) {
	   antennaSelector      -> addItem ("Antenna B");
	   antennaSelector      -> addItem ("Antenna C");
           antennaSelector	-> show ();
        }
	else
	if (sdrDevice == SDRPLAY_RSP2_) {
	   antennaSelector	-> addItem ("Antenna B");
	   antennaSelector	-> show ();
	}

	QString setting	=
	      value_s (sdrplaySettings, SDRPLAY_SETTINGS, 
	                                SDRPLAY_ANTENNA, "Antenna A");
	int k	= antennaSelector -> findText (setting);
	if (k >= 0) 
	   antennaSelector -> setCurrentIndex (k);
	connect (antennaSelector,
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 2)
	         &QComboBox::textActivated,
#else
	         qOverload<const QString &>(&QComboBox::activated),
#endif
	         this, &sdrplayHandler_v3::setSelectAntenna);
	return k == 2 ? 'C' : k == 1 ? 'B' : 'A';
}

bool	sdrplayHandler_v3::setupXmlDump (bool direct) {
QString channel		= value_s (sdrplaySettings, DAB_GENERAL,
	                                               "channel", "xx");
	xmlWriter	= nullptr;
	try {
	   xmlWriter	= new xml_fileWriter (sdrplaySettings,
	                                      channel,
	                                      nrBits,
	                                      "int16",
	                                      SAMPLERATE,
	                                      lastFrequency,
	                                      theGain,
	                                      "SDRplay",
	                                      deviceModel,
	                                      recorderVersion,
	                                      direct);
	} catch (...) {
	   theErrorLogger -> add (recorderVersion, "Setup_xml handler failed");
	   return false;
	}
	dumping. store (true);
	dumpButton	-> setText ("writing");
	return true;
}
	
void	sdrplayHandler_v3::closeXmlDump () {
	if (xmlWriter == nullptr)	// this can happen !!
	   return;
	usleep (1000);
	xmlWriter	-> computeHeader ();
	delete xmlWriter;
	xmlWriter	= nullptr;
	dumping. store (false);
	dumpButton	-> setText ("Dump");
}
//
///////////////////////////////////////////////////////////////////////
//	the real controller starts here
///////////////////////////////////////////////////////////////////////

bool    sdrplayHandler_v3::messageHandler (generalCommand *r) {
        serverQueue. push (r);
	serverJobs. release (1);
	while (!r -> waiter. tryAcquire (1, 1000))
	   if (!threadRuns. load ())
	      return false;
	return true;
}

static
void    StreamACallback (short *xi, short *xq,
                         sdrplay_api_StreamCbParamsT *params,
                         unsigned int numSamples,
	                 unsigned int reset,
                         void *cbContext) {
sdrplayHandler_v3 *p	= static_cast<sdrplayHandler_v3 *> (cbContext);
std::complex<int16_t> localBuf [numSamples];

	(void)params;
	if (reset)
	   return;
	if (!p -> receiverRuns. load ())
	   return;

	if (p -> toSkip > 0) {
	   p -> toSkip -= numSamples;
	   return;
	}
	for (int i = 0; i <  (int)numSamples; i ++) {
	   std::complex<int16_t> symb = std::complex<int16_t> (xi [i], xq [i]);
	   localBuf [i] = symb;
	}
	p -> _I_Buffer. putDataIntoBuffer (localBuf, numSamples);
}

static
void	StreamBCallback (short *xi, short *xq,
                         sdrplay_api_StreamCbParamsT *params,
                         unsigned int numSamples, unsigned int reset,
                         void *cbContext) {
	(void)xi; (void)xq; (void)params; (void)cbContext;
        if (reset)
           printf ("sdrplay_api_StreamBCallback: numSamples=%d\n", numSamples);
}

static
void	EventCallback (sdrplay_api_EventT eventId,
                       sdrplay_api_TunerSelectT tuner,
                       sdrplay_api_EventParamsT *params,
                       void *cbContext) {
sdrplayHandler_v3 *p	= static_cast<sdrplayHandler_v3 *> (cbContext);
	(void)tuner;
	switch (eventId) {
	   case sdrplay_api_GainChange:
	      p -> showTunerGain (params -> gainParams. currGain);
	      break;

	   case sdrplay_api_PowerOverloadChange:
	      p -> updatePowerOverload (params);
	      break;

	   default:
	      fprintf (stderr, "event %d\n", eventId);
	      break;
	}
}

void	sdrplayHandler_v3::
	         updatePowerOverload (sdrplay_api_EventParamsT *params) {
	sdrplay_api_Update (chosenDevice -> dev,
	                    chosenDevice -> tuner,
	                    sdrplay_api_Update_Ctrl_OverloadMsgAck,
	                    sdrplay_api_Update_Ext1_None);
	emit overloadStateChanged (
	        params -> powerOverloadParams.powerOverloadChangeType ==
	                                   sdrplay_api_Overload_Detected);
}

void	sdrplayHandler_v3::run		() {
sdrplay_api_ErrT        err;
sdrplay_api_DeviceT     devs [6];
uint32_t                ndev;
int	deviceIndex	= 0;
        threadRuns. store (false);
	receiverRuns. store (false);

	showState ("");
	chosenDevice		= nullptr;

	connect (this, &sdrplayHandler_v3::setSerialSignal,
	         this, &sdrplayHandler_v3::setSerial);
	connect (this, &sdrplayHandler_v3::setApiVersionSignal,
	         this, &sdrplayHandler_v3::setApiVersion);
	connect (this, &sdrplayHandler_v3::showTunerGain,
	         this, &sdrplayHandler_v3::displayGain);

	denominator		= 2048.0f;		// default
	nrBits			= 12;		// default

	Handle                  = fetchLibrary ();
        if (Handle == nullptr) {
           failFlag. store (true);
           errorCode    = 1;
           return;
        }

//      load the functions
        bool success    = loadFunctions ();
        if (!success) {
           failFlag. store (true);
           releaseLibrary ();
           errorCode    = 2;
           return;
        }
        fprintf (stderr, "functions loaded\n");

//	try to open the API
	err	= sdrplay_api_Open ();
	if (err != sdrplay_api_Success) {
	   theErrorLogger -> add (recorderVersion,
	                          sdrplay_api_GetErrorString (err));
	   failFlag. store (true);
	   releaseLibrary	();
	   errorCode	= 3;
	   return;
	}

	fprintf (stderr, "api opened\n");

//	Check API versions match
        err = sdrplay_api_ApiVersion (&apiVersion);
        if (err  != sdrplay_api_Success) {
	   theErrorLogger -> add (recorderVersion,
                                     sdrplay_api_GetErrorString (err));
	   errorCode	= 4;
	   goto closeAPI;
        }

	if (apiVersion < 3.07) {
//	if (apiVersion < (SDRPLAY_API_VERSION - 0.01)) {
           fprintf (stderr, "API versions don't match (local=%.2f dll=%.2f)\n",
                                              SDRPLAY_API_VERSION, apiVersion);
	   errorCode	= 5;
	   goto closeAPI;
	}
	
	fprintf (stderr, "api version %f detected\n", apiVersion);
//
//	lock API while device selection is performed
	sdrplay_api_LockDeviceApi ();
	{  int s	= sizeof (devs) / sizeof (sdrplay_api_DeviceT);
	   err	= sdrplay_api_GetDevices (devs, &ndev, s);
	   if (err != sdrplay_api_Success) {
	      theErrorLogger -> add (recorderVersion,
	                           sdrplay_api_GetErrorString (err));
	      errorCode		= 6;
	      goto unlockDevice_closeAPI;
	   }
	}

	fprintf (stderr, "we have devices\n");
	if (ndev == 0) {
	   theErrorLogger -> add (recorderVersion, "no valid device found\n");
	   errorCode	= 7;
	   goto unlockDevice_closeAPI;
	}

	deviceIndex	= 0;
	chosenDevice	= &devs [deviceIndex];
	chosenDevice	-> rspDuoMode = sdrplay_api_RspDuoMode_Single_Tuner;
	chosenDevice	-> tuner  = sdrplay_api_Tuner_A;
	err	= sdrplay_api_SelectDevice (chosenDevice);
	if (err != sdrplay_api_Success) {
	   theErrorLogger -> add (recorderVersion,
	                         sdrplay_api_GetErrorString (err));
	   errorCode	= 8;
	   goto unlockDevice_closeAPI;
	}
//
//	assign callback functions
	cbFns. StreamACbFn	= StreamACallback;
	cbFns. StreamBCbFn	= StreamBCallback;
	cbFns. EventCbFn	= EventCallback;

//	we have a device, unlock
	sdrplay_api_UnlockDeviceApi ();
//
	serial		= devs [deviceIndex]. SerNo;
	hwVersion	= devs [deviceIndex]. hwVer;
//
	try {
	   int antennaValue;
	   int	lnaBounds;
	   bool	notch	= value_i (sdrplaySettings, SDRPLAY_SETTINGS,
	                                          SDRPLAY_NOTCH, 0) != 0;
	   switch (hwVersion) {
	      case SDRPLAY_RSPdx_ :
	      case SDRPLAY_RSPdxR2_ :
	         antennaValue = setAntennaSelect (hwVersion);
	         nrBits		= 14;
	         denominator	= 8192.0f;
	         deviceModel	=  hwVersion == SDRPLAY_RSPdx_?
	            	                     "RSPDx" : "RSPDxR2";
	         theRsp. reset (new  RspDx_handler (this,
	                                            theErrorLogger,
	                                            chosenDevice,
	                                            KHz (220000),
	                                            agcMode,
	                                            lnaState,
	                                            GRdBValue,
	                                            antennaValue,
	                                            biasT,
	                                            notch,  ppmValue));
	         break;

	      case SDRPLAY_RSP1_ :
	         nrBits		= 12;
	         denominator	= 4096.0f;
	         deviceModel	= "RSP1";
	         biasT_selector -> setEnabled (false);
	         notch_selector -> setEnabled (false);
	         theRsp. reset (new Rsp1_handler  (this,
	                                           theErrorLogger,
	                                           chosenDevice,
	                                           KHz (220000),
	                                           agcMode,
	                                           lnaState,
	                                           GRdBValue,
	                                           biasT, ppmValue));
	         break;

	      case SDRPLAY_RSP1A_ :
	      case SDRPLAY_RSP1B_ :
	         nrBits		= 14;
	         denominator	= 4096.0f;
	         deviceModel	= hwVersion == SDRPLAY_RSP1A_ ? "RSP-1A" :
	                                                        "RSP-1B";
	         theRsp. reset (new Rsp1A_handler (this,
	                                           theErrorLogger,
	                                           chosenDevice,
	                                           KHz (220000),
	                                           agcMode,
	                                           lnaState,
	                                           GRdBValue,
	                                           biasT,
	                                           notch,
	                                           ppmValue));
	         break;

	      case SDRPLAY_RSP2_ :
	         antennaValue = setAntennaSelect (SDRPLAY_RSP2_);
	         nrBits		= 14;
	         denominator	= 4096.0f;
	         deviceModel	= "RSP-II";
	         theRsp. reset (new RspII_handler (this,
		                                   theErrorLogger,
	                                           chosenDevice,
	                                           KHz (220000),
	                                           agcMode,
	                                           lnaState,
	                                           GRdBValue,
	                                           antennaValue,
	                                           biasT,
	                                           notch,  ppmValue));
	         break;

	      case SDRPLAY_RSPduo_ :
	         nrBits		= 14;
	         denominator	= 4096.0f;
	         deviceModel	= "RSP-Duo";
	         tunerSelector	-> show	();
	         connect (tunerSelector,
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 2)
	         &QComboBox::textActivated,
#else
	         qOverload<const QString &>(&QComboBox::activated),
#endif
	         this, &sdrplayHandler_v3::setSelectTuner);
	         theRsp. reset (new RspDuo_handler (this,
	                                            theErrorLogger,
	                                            chosenDevice,
	                                            KHz (220000),
	                                            agcMode,
	                                            lnaState,
	                                            GRdBValue,
	                                            antennaValue,
	                                            1,
	                                            biasT,
	                                            notch, ppmValue));
	         break;

	      default:
	         nrBits		= 14;
	         denominator	= 4096.0f;
	         deviceModel	= "UNKNOWN";
	         lnaBounds	= 4;
	         lnaGainSetting	-> setRange (0, lnaBounds - 1);
	         theRsp. reset (new RspDevice (this,
	                                       chosenDevice,
	                                       KHz (220000),
	                                       agcMode,
	                                       lnaState,
	                                       GRdBValue,
	                                       biasT, ppmValue));
	         break;
	   }
	} catch (int e) {
	   goto closeAPI;
	}

	deviceLabel		-> setText (deviceModel);
	setSerialSignal       (serial);
        setApiVersionSignal   (apiVersion);

	threadRuns. store (true);       // it seems we can do some work
	successFlag. store (true);

	while (threadRuns. load ()) {
	   while (!serverJobs. tryAcquire (1, 1000))
	   if (!threadRuns. load ())
	      goto normal_exit;

//	here we could assert that the serverQueue is not empty
//	Note that we emulate synchronous calling, so
//	we signal the caller when we are done
	   switch (serverQueue. front () -> cmd) {
	      case RESTART_REQUEST: {
	         restartRequest *p = (restartRequest *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = theRsp -> restart (p -> freq);
	         receiverRuns. store (true);
	         p -> waiter. release (1);
	         break;
	      }
	       
	      case STOP_REQUEST: {
	         stopRequest *p = (stopRequest *)(serverQueue. front ());
	         serverQueue. pop ();
	         receiverRuns. store (false);
	         p -> waiter. release (1);
	         break;
	      }
	       
	      case AGC_REQUEST: {
	         agcRequest *p = 
	                    (agcRequest *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = theRsp -> setAgc (-p -> setPoint, p -> agcMode);
	         p -> waiter. release (1);
	         break;
	      }

	      case GRDB_REQUEST: {
	         GRdBRequest *p =  (GRdBRequest *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = theRsp -> setGRdB (p -> GRdBValue);
                 p -> waiter. release (1);
	         break;
	      }

	      case PPM_REQUEST: {
	         ppmRequest *p = (ppmRequest *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = theRsp -> setPpm (p -> ppmValue);
	         p -> waiter. release (1);
	         break;
	      }

	      case LNA_REQUEST: {
	         lnaRequest *p = (lnaRequest *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = theRsp -> setLna (p -> lnaState);
                 p -> waiter. release (1);
	         break;
	      }

	      case ANTENNASELECT_REQUEST: {
	         antennaRequest *p = (antennaRequest *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = theRsp -> setAntenna (p -> antenna);
                 p -> waiter. release (1);
	         break;
	      }

	      case BIAS_T_REQUEST: {
	         biasT_Request *p = (biasT_Request *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = theRsp -> setBiasT (p -> checked);
                 p -> waiter. release (1);
	         break;
	      }

	      case NOTCH_REQUEST: {
	         notch_Request *p = (notch_Request *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = theRsp -> setNotch (p -> checked);
                 p -> waiter. release (1);
	         break;
	      }

	      case TUNERSELECT_REQUEST: {
	         tunerRequest *p =
	               (tunerRequest *)(serverQueue. front ());
	         serverQueue. pop();
//	         fprintf (stderr, "Going to call set_tuner\n");
	         p -> result = theRsp -> setTuner (p -> tuner);
	         p -> waiter. release (1);
	         break;
	      }

	      default:		// cannot happen
	         fprintf (stderr, "Helemaal fout\n");
	         break;
	   }
	}


normal_exit:
	err = sdrplay_api_Uninit	(chosenDevice -> dev);
	if (err != sdrplay_api_Success) 
	   theErrorLogger -> add (recorderVersion,
	                          sdrplay_api_GetErrorString (err));

	err = sdrplay_api_ReleaseDevice	(chosenDevice);
	if (err != sdrplay_api_Success) 
	   theErrorLogger -> add (recorderVersion,
	                          sdrplay_api_GetErrorString (err));

//	sdrplay_api_UnlockDeviceApi	(); ??
        sdrplay_api_Close               ();
	if (err != sdrplay_api_Success) 
	   theErrorLogger -> add (recorderVersion,
	                          sdrplay_api_GetErrorString (err));

	releaseLibrary	();
	fprintf (stderr, "library released, ready to stop thread\n");
	msleep (200);
	return;

unlockDevice_closeAPI:
	sdrplay_api_UnlockDeviceApi	();
closeAPI:	
	failFlag. store (true);
	sdrplay_api_ReleaseDevice       (chosenDevice);
        sdrplay_api_Close               ();
	releaseLibrary			();
	fprintf (stderr, "De taak is gestopt\n");
}

/////////////////////////////////////////////////////////////////////////////
//	handling the library
/////////////////////////////////////////////////////////////////////////////

HINSTANCE	sdrplayHandler_v3::fetchLibrary () {
HINSTANCE	Handle	= nullptr;
#ifdef	__MINGW32__
HKEY APIkey;
wchar_t APIkeyValue [256];
ULONG APIkeyValue_length = 255;

	wchar_t *libname = (wchar_t *)L"sdrplay_api.dll";
	Handle	= LoadLibrary (libname);
//	fprintf (stderr, "loadLib is gewoon gelukt ? %d\n",
//	                                 Handle != nullptr);
	if (Handle == nullptr) {
	   if (RegOpenKey (HKEY_LOCAL_MACHINE,
//	                   TEXT ("Software\\MiricsSDR\\API"),
	                   TEXT ("Software\\SDRplay\\Service\\API"),
	                   &APIkey) != ERROR_SUCCESS) {
              fprintf (stderr,
	           "failed to locate API registry entry, error = %d\n",
	           (int)GetLastError());
	      theErrorLogger -> add (recorderVersion, 
	                         errorMessage ((int)GetLastError ()). c_str ());
	   }
	   else {
	      RegQueryValueEx (APIkey,
	                       (wchar_t *)L"Install_Dir",
	                       nullptr,
	                       nullptr,
	                       (LPBYTE)&APIkeyValue,
	                       (LPDWORD)&APIkeyValue_length);
//	Ok, make explicit it is in the 32/64 bits section
	      wchar_t *x =
#ifndef __BITS64__
	        wcscat (APIkeyValue, (wchar_t *)L"\\x86\\sdrplay_api.dll");
#else
	        wcscat (APIkeyValue, (wchar_t *)L"\\x64\\sdrplay_api.dll");
#endif
	      RegCloseKey (APIkey);

	      Handle	= LoadLibrary (x);
	   }
	   if (Handle == nullptr) {
	      fprintf (stderr, "Failed to open sdrplay_api.dll\n");
	      return nullptr;
	   }
	}
#else
	Handle		= dlopen ("libusb-1.0.so", RTLD_NOW | RTLD_GLOBAL);
	Handle		= dlopen ("libsdrplay_api.so", RTLD_NOW);
	if (Handle == nullptr) {
	   theErrorLogger -> add (recorderVersion, dlerror ());
	   return nullptr;
	}
#endif
	return Handle;
}

void	sdrplayHandler_v3::releaseLibrary () {
#ifdef __MINGW32__
        FreeLibrary (Handle);
#else
	dlclose (Handle);
#endif
}
bool	sdrplayHandler_v3::loadFunctions () {
	sdrplay_api_Open	= (sdrplay_api_Open_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_Open");
	if ((void *)sdrplay_api_Open == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                     "Could not find sdrplay_api_Open\n");
	   return false;
	}

	sdrplay_api_Close	= (sdrplay_api_Close_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_Close");
	if (sdrplay_api_Close == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                     "Could not find sdrplay_api_Close\n");
	   return false;
	}

	sdrplay_api_ApiVersion	= (sdrplay_api_ApiVersion_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_ApiVersion");
	if (sdrplay_api_ApiVersion == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                     "Could not find sdrplay_api_ApiVersion\n");
	   return false;
	}

	sdrplay_api_LockDeviceApi	= (sdrplay_api_LockDeviceApi_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_LockDeviceApi");
	if (sdrplay_api_LockDeviceApi == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                     "Could not find sdrplay_api_LockdeviceApi\n");
	   return false;
	}

	sdrplay_api_UnlockDeviceApi	= (sdrplay_api_UnlockDeviceApi_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_UnlockDeviceApi");
	if (sdrplay_api_UnlockDeviceApi == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                     "Could not find sdrplay_api_UnlockdeviceApi\n");
	   return false;
	}

	sdrplay_api_GetDevices		= (sdrplay_api_GetDevices_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_GetDevices");
	if (sdrplay_api_GetDevices == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                     "Could not find sdrplay_api_GetDevices\n");
	   return false;
	}

	sdrplay_api_SelectDevice	= (sdrplay_api_SelectDevice_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_SelectDevice");
	if (sdrplay_api_SelectDevice == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                     "Could not find sdrplay_api_SelectDevice\n");
	   return false;
	}

	sdrplay_api_ReleaseDevice	= (sdrplay_api_ReleaseDevice_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_ReleaseDevice");
	if (sdrplay_api_ReleaseDevice == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                     "Could not find sdrplay_api_ReleaseDevice\n");
	   return false;
	}

	sdrplay_api_GetErrorString	= (sdrplay_api_GetErrorString_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_GetErrorString");
	if (sdrplay_api_GetErrorString == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                     "Could not find sdrplay_api_GetErrorString\n");
	   return false;
	}

	sdrplay_api_GetLastError	= (sdrplay_api_GetLastError_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_GetLastError");
	if (sdrplay_api_GetLastError == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                     "Could not find sdrplay_api_GetLastError\n");
	   return false;
	}

	sdrplay_api_DebugEnable		= (sdrplay_api_DebugEnable_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_DebugEnable");
	if (sdrplay_api_DebugEnable == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                     "Could not find sdrplay_api_DebugEnable\n");
	   return false;
	}

	sdrplay_api_GetDeviceParams	= (sdrplay_api_GetDeviceParams_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_GetDeviceParams");
	if (sdrplay_api_GetDeviceParams == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                     "Could not find sdrplay_api_GetDeviceParams\n");
	   return false;
	}

	sdrplay_api_Init		= (sdrplay_api_Init_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_Init");
	if (sdrplay_api_Init == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                     "Could not find sdrplay_api_Init\n");
	   return false;
	}

	sdrplay_api_Uninit		= (sdrplay_api_Uninit_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_Uninit");
	if (sdrplay_api_Uninit == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                     "Could not find sdrplay_api_Uninit\n");
	   return false;
	}

	sdrplay_api_Update		= (sdrplay_api_Update_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_Update");
	if (sdrplay_api_Update == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                     "Could not find sdrplay_api_Update\n");
	   return false;
	}

	return true;
}
void	sdrplayHandler_v3::reportOverloadState (bool b) {
	if (b)
	   overloadLabel -> setStyleSheet ("QLabel {background-color : red}");
	else
	   overloadLabel -> setStyleSheet ("QLabel {background-color : green}");

}

void	sdrplayHandler_v3::displayGain	(double gain) {
	gainDisplay -> setText (QString::number (gain, 'f', 0) + "dB");
}

void	sdrplayHandler_v3::showState	(const QString &s) {
	stateLabel	-> setAlignment (Qt::AlignCenter);
	stateLabel	-> setText (s);
}

