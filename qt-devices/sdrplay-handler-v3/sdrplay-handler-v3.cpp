#
/*
 *    Copyright (C) 2020
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
#include	<QTime>
#include	<QDate>
#include	<QLabel>
#include	<QPoint>
#include	<QFileDialog>
#include	"sdrplay-handler-v3.h"
#include	"position-handler.h"
#include	"sdrplay-commands.h"
#include	"xml-filewriter.h"
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

#define	SDRPLAY_SETTINGS	"sdrplaySettings_v3"
#define	SDRPLAY_IFGRDB		"sdrplay-ifgrdb"
#define	SDRPLAY_LNASTATE	"sdrplay-lnastate"
#define	SDRPLAY_PPM		"sdrplay-ppm"
#define	SDRPLAY_AGCMODE		"sdrplay_agcMode"
#define	SDRPLAY_BIAS_T		"biasT_selector"
#define	SDRPLAY_ANTENNA		"Antenna"

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

	sdrplayHandler_v3::sdrplayHandler_v3  (QSettings *s,
	                                       const QString &recorderVersion):
	                                          _I_Buffer (4 * 1024 * 1024) {
	sdrplaySettings			= s;
	inputRate			= 2048000;
	this	-> recorderVersion	= recorderVersion;
        setupUi (&myFrame);
	QString	groupName	= SDRPLAY_SETTINGS;
	set_position_and_size (s, &myFrame, groupName);
	myFrame. show	();

	overloadLabel -> setStyleSheet ("QLabel {background-color : green}");
	antennaSelector		-> hide	();
	tunerSelector		-> hide	();
	nrBits			= 12;	// default
	denominator		= 2048;	// default

	xmlDumper		= nullptr;
	dumping. store	(false);
//	See if there are settings from previous incarnations
//	and config stuff

	sdrplaySettings		-> beginGroup (groupName);
	GRdBSelector 		-> setValue (
	            sdrplaySettings -> value (SDRPLAY_IFGRDB, 20). toInt());
	GRdBValue		= GRdBSelector -> value ();

	lnaGainSetting		-> setValue (
	            sdrplaySettings -> value (SDRPLAY_LNASTATE, 4). toInt());

	lnaState		= lnaGainSetting -> value ();

	ppmControl		-> setValue (
	            sdrplaySettings -> value (SDRPLAY_PPM, 0). toInt());

	agcMode		=
	       sdrplaySettings -> value (SDRPLAY_AGCMODE, 0). toInt() != 0;
	sdrplaySettings	-> endGroup	();

	if (agcMode) {
	   agcControl -> setChecked (true);
	   GRdBSelector         -> hide ();
	   gainsliderLabel      -> hide ();
	}

	biasT           =
               sdrplaySettings -> value (SDRPLAY_BIAS_T, 0). toInt () != 0;
        if (biasT)
           biasT_selector -> setChecked (true);
	
//	and be prepared for future changes in the settings
	connect (GRdBSelector, qOverload<int>(&QSpinBox::valueChanged),
	         this, &sdrplayHandler_v3::set_ifgainReduction);
	connect (lnaGainSetting, qOverload<int>(&QSpinBox::valueChanged),
	         this, &sdrplayHandler_v3::set_lnagainReduction);
	connect (agcControl, &QCheckBox::stateChanged,
	         this, &sdrplayHandler_v3::set_agcControl);
	connect (ppmControl, qOverload<int>(&QSpinBox::valueChanged),
	         this, &sdrplayHandler_v3::set_ppmControl);
	connect (dumpButton, &QPushButton::clicked,
                 this, &sdrplayHandler_v3::set_xmlDump);
	connect (biasT_selector, &QCheckBox::stateChanged,	
	         this, &sdrplayHandler_v3::set_biasT);
	connect (this, &sdrplayHandler_v3::overload_state_changed,
	         this, &sdrplayHandler_v3::report_overload_state);

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
	threadRuns. store (false);
	while (isRunning ())
	   usleep (1000);
//	thread should be stopped by now
	myFrame. hide ();
	QString groupName	= SDRPLAY_SETTINGS;
	store_widget_position (sdrplaySettings, &myFrame, SDRPLAY_SETTINGS);

	sdrplaySettings	-> beginGroup (SDRPLAY_SETTINGS);
	sdrplaySettings -> setValue (SDRPLAY_PPM,
	                                           ppmControl -> value ());
	sdrplaySettings -> setValue (SDRPLAY_IFGRDB,
	                                           GRdBSelector -> value ());
	sdrplaySettings -> setValue (SDRPLAY_LNASTATE,
	                                           lnaGainSetting -> value ());
	sdrplaySettings	-> setValue (SDRPLAY_AGCMODE,
	                                  agcControl -> isChecked() ? 1 : 0);
	sdrplaySettings	-> endGroup ();
	sdrplaySettings	-> sync();
}

/////////////////////////////////////////////////////////////////////////
//	Implementing the interface
/////////////////////////////////////////////////////////////////////////


bool	sdrplayHandler_v3::restartReader	(int32_t newFreq) {
restartRequest r (newFreq);

        if (receiverRuns. load ())
           return true;
        lastFrequency    = newFreq;
	_I_Buffer. FlushRingBuffer();
	return messageHandler (&r);
}

void	sdrplayHandler_v3::stopReader	() {
stopRequest r;
	close_xmlDump ();
        if (!receiverRuns. load ())
           return;
        messageHandler (&r);
}
//
int32_t	sdrplayHandler_v3::getSamples (std::complex<float> *V, int32_t size) { 
auto *temp 	= dynVec (std::complex<int16_t>, size);

	int amount      = _I_Buffer. getDataFromBuffer (temp, size);
        for (int i = 0; i < amount; i ++)
           V [i] = std::complex<float> (real (temp [i]) / (float) denominator,
                                        imag (temp [i]) / (float) denominator);
        if (dumping. load ())
           xmlWriter -> add (temp, amount);
        return amount;
}

int32_t	sdrplayHandler_v3::Samples	() {
	return _I_Buffer. GetRingBufferReadAvailable();
}

void	sdrplayHandler_v3::resetBuffer	() {
	_I_Buffer. FlushRingBuffer();
}

int16_t	sdrplayHandler_v3::bitDepth	() {
	return nrBits;
}

QString	sdrplayHandler_v3::deviceName	() {
	return deviceModel;
}

///////////////////////////////////////////////////////////////////////////
//	Handling the GUI
//////////////////////////////////////////////////////////////////////
//
//	Since the daemon is not threadproof, we have to package the
//	actual interface into its own thread.
//	Communication with that thread is synchronous!
//

void	sdrplayHandler_v3::set_lnabounds(int low, int high) {
	lnaGainSetting	-> setRange (low, high);
}

void	sdrplayHandler_v3::set_serial	(const QString& s) {
	serialNumber	-> setText (s);
}

void	sdrplayHandler_v3::set_apiVersion (float version) {
QString v = QString::number (version, 'r', 2);
	api_version	-> display (v);
}

void    sdrplayHandler_v3::show_lnaGain (int g) {
        lnaGRdBDisplay  -> display (g);
}

void	sdrplayHandler_v3::set_ifgainReduction	(int GRdB) {
GRdBRequest r (GRdB);
	
	if (!receiverRuns. load ())
           return;
        messageHandler (&r);
}

void	sdrplayHandler_v3::set_lnagainReduction (int lnaState) {
lnaRequest r (lnaState);

	if (!receiverRuns. load ())
           return;
        messageHandler (&r);
}

void	sdrplayHandler_v3::set_agcControl (int dummy) {
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

void	sdrplayHandler_v3::set_ppmControl (int ppm) {
ppmRequest r (ppm);
        messageHandler (&r);
}

void	sdrplayHandler_v3::set_biasT (int v) {
biasT_Request r (biasT_selector -> isChecked () ? 1 : 0);

	(void)v;
	messageHandler (&r);
	sdrplaySettings -> beginGroup (SDRPLAY_SETTINGS);
	sdrplaySettings -> setValue (SDRPLAY_BIAS_T,
	                              biasT_selector -> isChecked () ? 1 : 0);
	sdrplaySettings -> endGroup ();
}

void	sdrplayHandler_v3::set_selectAntenna	(const QString &s) {
	messageHandler (new antennaRequest (s == "Antenna A" ? 'A' :
	                                    s == "Antenna B" ? 'B' : 'C'));
	sdrplaySettings -> beginGroup (SDRPLAY_SETTINGS);
	sdrplaySettings	-> setValue (SDRPLAY_ANTENNA, s);
	sdrplaySettings	-> endGroup ();
}

void	sdrplayHandler_v3::set_xmlDump () {
	if (xmlDumper == nullptr) {
	  if (setup_xmlDump ())
	      dumpButton	-> setText ("writing");
	}
	else {
	   close_xmlDump ();
	   dumpButton	-> setText ("Dump");
	}
}
//
////////////////////////////////////////////////////////////////////////
//	showing data
////////////////////////////////////////////////////////////////////////
int	sdrplayHandler_v3::set_antennaSelect (int sdrDevice) {
	if ((sdrDevice == SDRPLAY_RSPdx_) || (sdrDevice == SDRPLAY_RSPduo_)) {
	   antennaSelector      -> addItem ("Antenna B");
	   antennaSelector      -> addItem ("Antenna C");
           antennaSelector	-> show ();
        }
	else
	if (sdrDevice == SDRPLAY_RSP2_) {
	   antennaSelector	-> addItem ("Antenna B");
	   antennaSelector	-> show ();
	}

	sdrplaySettings -> beginGroup (SDRPLAY_SETTINGS);
	QString setting	=
	      sdrplaySettings	-> value (SDRPLAY_ANTENNA,
	                                         "Antenna A"). toString ();
	sdrplaySettings	-> endGroup ();
	int k	= antennaSelector -> findText (setting);
	if (k >= 0) 
	   antennaSelector -> setCurrentIndex (k);
	connect (antennaSelector, &QComboBox::textActivated,
	         this, &sdrplayHandler_v3::set_selectAntenna);
	return k == 2 ? 'C' : k == 1 ? 'B' : 'A';
}

void	sdrplayHandler_v3::show_tunerSelector	(bool b) {
	if (b)
	   tunerSelector	-> show	();
	else
	   tunerSelector	-> hide	();
}

static inline
bool	isValid (QChar c) {
	return c. isLetterOrNumber () || (c == '-');
}

bool	sdrplayHandler_v3::setup_xmlDump () {
QTime theTime;
QDate theDate;
QString saveDir = sdrplaySettings -> value (SAVEDIR_XML,
                                           QDir::homePath ()). toString ();
        if ((saveDir != "") && (!saveDir. endsWith ("/")))
           saveDir += "/";

	QString channel		= sdrplaySettings -> value ("channel", "xx").
	                                                      toString ();
	QString timeString      = theDate. currentDate (). toString () + "-" +
	                          theTime. currentTime (). toString ();
        for (int i = 0; i < timeString. length (); i ++)
           if (!isValid (timeString. at (i)))
              timeString. replace (i, 1, "-");

	QString suggestedFileName =
                    saveDir + deviceModel + "-" + channel + "-" +timeString;
	QString fileName =
	           QFileDialog::getSaveFileName (nullptr,
	                                         tr ("Save file ..."),
	                                         suggestedFileName +".uff",
	                                         tr ("Xml (*.uff)"));
        fileName        = QDir::toNativeSeparators (fileName);
        xmlDumper	= fopen (fileName. toUtf8(). data(), "w");
	if (xmlDumper == nullptr)
	   return false;
	
	xmlWriter	= new xml_fileWriter (xmlDumper,
	                                      nrBits,
	                                      "int16",
	                                      2048000,
	                                      lastFrequency,
	                                      "SDRplay",
	                                      "????",
	                                      recorderVersion);
	dumping. store (true);

	QString dumper	= QDir::fromNativeSeparators (fileName);
	int x		= dumper. lastIndexOf ("/");
	saveDir		= dumper. remove (x, dumper. count () - x);
        sdrplaySettings -> setValue ("saveDir_xmlDump", saveDir);
	return true;
}

void	sdrplayHandler_v3::close_xmlDump () {
	if (xmlDumper == nullptr)	// this can happen !!
	   return;
	dumping. store (false);
	usleep (1000);
	xmlWriter	-> computeHeader ();
	delete xmlWriter;
	fclose (xmlDumper);
	xmlDumper	= nullptr;
}
//
///////////////////////////////////////////////////////////////////////
//	the real controller starts here
///////////////////////////////////////////////////////////////////////

bool    sdrplayHandler_v3::messageHandler (generalCommand *r) {
        server_queue. push (r);
	serverjobs. release (1);
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
	p -> theGain	= params -> gainParams. currGain;
	switch (eventId) {
	   case sdrplay_api_GainChange:
	      break;

	   case sdrplay_api_PowerOverloadChange:
	      p -> update_PowerOverload (params);
	      break;

	   default:
	      fprintf (stderr, "event %d\n", eventId);
	      break;
	}
}

void	sdrplayHandler_v3::
	         update_PowerOverload (sdrplay_api_EventParamsT *params) {
	sdrplay_api_Update (chosenDevice -> dev,
	                    chosenDevice -> tuner,
	                    sdrplay_api_Update_Ctrl_OverloadMsgAck,
	                    sdrplay_api_Update_Ext1_None);
	emit overload_state_changed (
	        params -> powerOverloadParams.powerOverloadChangeType ==
	                                   sdrplay_api_Overload_Detected);
}

void	sdrplayHandler_v3::run		() {
sdrplay_api_ErrT        err;
sdrplay_api_DeviceT     devs [6];
uint32_t                ndev;

        threadRuns. store (false);
	receiverRuns. store (false);

	chosenDevice		= nullptr;

	connect (this, &sdrplayHandler_v3::set_serial_signal,
	         this, &sdrplayHandler_v3::set_serial);
	connect (this, &sdrplayHandler_v3::set_apiVersion_signal,
	         this, &sdrplayHandler_v3::set_apiVersion);

	denominator		= 2048;		// default
	nrBits			= 12;		// default

	Handle			= fetchLibrary ();
	if (Handle == nullptr) {
	   failFlag. store (true);
	   errorCode	= 1;
	   return;
	}

//	load the functions
	bool success	= loadFunctions ();
	if (!success) {
	   failFlag. store (true);
	   releaseLibrary ();
	   errorCode	= 2;
	   return;
        }
//	fprintf (stderr, "functions loaded\n");

//	try to open the API
	err	= sdrplay_api_Open ();
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "sdrplay_api_Open failed %s\n",
	                          sdrplay_api_GetErrorString (err));
	   failFlag. store (true);
	   releaseLibrary ();
	   errorCode	= 3;
	   return;
	}

	fprintf (stderr, "api opened\n");

//	Check API versions match
        err = sdrplay_api_ApiVersion (&apiVersion);
        if (err  != sdrplay_api_Success) {
           fprintf (stderr, "sdrplay_api_ApiVersion failed %s\n",
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
	      fprintf (stderr, "sdrplay_api_GetDevices failed %s\n",
	                      sdrplay_api_GetErrorString (err));
	      errorCode		= 6;
	      goto unlockDevice_closeAPI;
	   }
	}

	if (ndev == 0) {
	   fprintf (stderr, "no valid device found\n");
	   errorCode	= 7;
	   goto unlockDevice_closeAPI;
	}

	fprintf (stderr, "%d devices detected\n", ndev);
	chosenDevice	= &devs [0];
	err	= sdrplay_api_SelectDevice (chosenDevice);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "sdrplay_api_SelectDevice failed %s\n",
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
	serial		= devs [0]. SerNo;
	hwVersion	= devs [0]. hwVer;
//
	try {
	   int antennaValue;
	   switch (hwVersion) {
	      case SDRPLAY_RSPdx_ :
	         antennaValue = set_antennaSelect (SDRPLAY_RSPdx_);
	         nrBits		= 14;
	         denominator	= 4096;
	         deviceModel	= "RSPDx";
	         theRsp	= new RspDx_handler (this,
	                                     chosenDevice,
	                                     inputRate,
	                                     KHz (14070),
	                                     agcMode,
	                                     lnaState,
	                                     GRdBValue,
	                                     antennaValue,
	                                     biasT);
	         break;

	      case SDRPLAY_RSP1_ :
	         nrBits		= 12;
	         denominator	= 2048;
	         deviceModel	= "RSP1";
	         theRsp	= new Rsp1_handler  (this,
	                                     chosenDevice,
	                                     inputRate,
	                                     KHz (14070),
	                                     agcMode,
	                                     lnaState,
	                                     GRdBValue,
	                                     biasT);
	         break;

	      case SDRPLAY_RSP1A_ :
	      case SDRPLAY_RSP1B_ :
	         nrBits		= 14;
	         denominator	= 4096;
	         deviceModel	= hwVersion == SDRPLAY_RSP1A_ ? "RSO-1A" :
	                                                        "RSP-1B";
	         theRsp	= new Rsp1A_handler (this,
	                                     chosenDevice,
	                                     inputRate,
	                                     KHz (14070),
	                                     agcMode,
	                                     lnaState,
	                                     GRdBValue,
	                                     biasT);
	         break;

	      case SDRPLAY_RSP2_ :
	         antennaValue = set_antennaSelect (SDRPLAY_RSP2_);
	         nrBits		= 14;
	         denominator	= 4096;
	         deviceModel	= "RSP-II";
	         theRsp	= new RspII_handler (this,
	                                     chosenDevice,
	                                     inputRate,
	                                     KHz (14070),
	                                     agcMode,
	                                     lnaState,
	                                     GRdBValue,
	                                     antennaValue,
	                                     biasT);
	         break;

	      case SDRPLAY_RSPduo_ :
	         antennaValue = set_antennaSelect (SDRPLAY_RSPdx_);
	         nrBits		= 14;
	         denominator	= 4096;
	         deviceModel	= "RSP-Duo";
	         theRsp	= new RspDuo_handler (this,
	                                     chosenDevice,
	                                     inputRate,
	                                     KHz (14070),
	                                     agcMode,
	                                     lnaState,
	                                     GRdBValue,
	                                     antennaValue,
	                                     biasT);
	         break;

	      default:
	         nrBits		= 14;
	         denominator	= 4096;
	         deviceModel	= "UNKNOWN";
	         theRsp	= new Rsp_device (this,
	                                  chosenDevice,
	                                  2112000,
	                                  KHz (14070),
	                                  agcMode,
	                                  lnaState,
	                                  GRdBValue,
	                                  biasT);
	         break;
	   }
	} catch (int e) {
	   goto closeAPI;
	}

	deviceLabel		-> setText (deviceModel);
	set_serial_signal       (serial);
        set_apiVersion_signal   (apiVersion);

	threadRuns. store (true);       // it seems we can do some work
	successFlag. store (true);
	while (threadRuns. load ()) {
	   while (!serverjobs. tryAcquire (1, 1000))
	   if (!threadRuns. load ())
	      goto normal_exit;

//	here we could assert that the server_queue is not empty
//	Note that we emulate synchronous calling, so
//	we signal the caller when we are done
	   switch (server_queue. front () -> cmd) {
	      case RESTART_REQUEST: {
	         restartRequest *p = (restartRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = theRsp -> restart (p -> freq);
	         receiverRuns. store (true);
	         p -> waiter. release (1);
	         break;
	      }
	       
	      case STOP_REQUEST: {
	         stopRequest *p = (stopRequest *)(server_queue. front ());
	         server_queue. pop ();
	         receiverRuns. store (false);
	         p -> waiter. release (1);
	         break;
	      }
	       
	      case AGC_REQUEST: {
	         agcRequest *p = 
	                    (agcRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = theRsp -> set_agc (-p -> setPoint, p -> agcMode);
	         p -> waiter. release (1);
	         break;
	      }

	      case GRDB_REQUEST: {
	         GRdBRequest *p =  (GRdBRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = theRsp -> set_GRdB (p -> GRdBValue);
                 p -> waiter. release (1);
	         break;
	      }

	      case PPM_REQUEST: {
	         ppmRequest *p = (ppmRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = theRsp -> set_ppm (p -> ppmValue);
	         p -> waiter. release (1);
	         break;
	      }

	      case LNA_REQUEST: {
	         lnaRequest *p = (lnaRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = theRsp -> set_lna (p -> lnaState);
                 p -> waiter. release (1);
	         break;
	      }

	      case ANTENNASELECT_REQUEST: {
	         antennaRequest *p = (antennaRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = theRsp -> set_antenna (p -> antenna);
                 p -> waiter. release (1);
	         break;
	      }

	      case BIAS_T_REQUEST: {
	         biasT_Request *p = (biasT_Request *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = theRsp -> set_biasT (p -> checked);
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
	   fprintf (stderr, "sdrplay_api_Uninit failed %s\n",
	                          sdrplay_api_GetErrorString (err));

	err = sdrplay_api_ReleaseDevice	(chosenDevice);
	if (err != sdrplay_api_Success) 
	   fprintf (stderr, "sdrplay_api_ReleaseDevice failed %s\n",
	                          sdrplay_api_GetErrorString (err));

//	sdrplay_api_UnlockDeviceApi	(); ??
        sdrplay_api_Close               ();
	if (err != sdrplay_api_Success) 
	   fprintf (stderr, "sdrplay_api_Close failed %s\n",
	                          sdrplay_api_GetErrorString (err));

	releaseLibrary			();
	fprintf (stderr, "library released, ready to stop thread\n");
	msleep (200);
	return;

unlockDevice_closeAPI:
	sdrplay_api_UnlockDeviceApi	();
closeAPI:	
	failFlag. store (true);
	sdrplay_api_ReleaseDevice       (chosenDevice);
        sdrplay_api_Close               ();
	releaseLibrary	();
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
//	      fprintf (stderr, "Met de key is het gelukt? %d\n", 
//	                                               Handle != nullptr);
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
	   fprintf (stderr, "error report %s\n", dlerror());
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
	   fprintf (stderr, "Could not find sdrplay_api_Open\n");
	   return false;
	}

	sdrplay_api_Close	= (sdrplay_api_Close_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_Close");
	if (sdrplay_api_Close == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_Close\n");
	   return false;
	}

	sdrplay_api_ApiVersion	= (sdrplay_api_ApiVersion_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_ApiVersion");
	if (sdrplay_api_ApiVersion == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_ApiVersion\n");
	   return false;
	}

	sdrplay_api_LockDeviceApi	= (sdrplay_api_LockDeviceApi_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_LockDeviceApi");
	if (sdrplay_api_LockDeviceApi == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_LockdeviceApi\n");
	   return false;
	}

	sdrplay_api_UnlockDeviceApi	= (sdrplay_api_UnlockDeviceApi_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_UnlockDeviceApi");
	if (sdrplay_api_UnlockDeviceApi == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_UnlockdeviceApi\n");
	   return false;
	}

	sdrplay_api_GetDevices		= (sdrplay_api_GetDevices_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_GetDevices");
	if (sdrplay_api_GetDevices == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_GetDevices\n");
	   return false;
	}

	sdrplay_api_SelectDevice	= (sdrplay_api_SelectDevice_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_SelectDevice");
	if (sdrplay_api_SelectDevice == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_SelectDevice\n");
	   return false;
	}

	sdrplay_api_ReleaseDevice	= (sdrplay_api_ReleaseDevice_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_ReleaseDevice");
	if (sdrplay_api_ReleaseDevice == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_ReleaseDevice\n");
	   return false;
	}

	sdrplay_api_GetErrorString	= (sdrplay_api_GetErrorString_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_GetErrorString");
	if (sdrplay_api_GetErrorString == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_GetErrorString\n");
	   return false;
	}

	sdrplay_api_GetLastError	= (sdrplay_api_GetLastError_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_GetLastError");
	if (sdrplay_api_GetLastError == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_GetLastError\n");
	   return false;
	}

	sdrplay_api_DebugEnable		= (sdrplay_api_DebugEnable_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_DebugEnable");
	if (sdrplay_api_DebugEnable == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_DebugEnable\n");
	   return false;
	}

	sdrplay_api_GetDeviceParams	= (sdrplay_api_GetDeviceParams_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_GetDeviceParams");
	if (sdrplay_api_GetDeviceParams == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_GetDeviceParams\n");
	   return false;
	}

	sdrplay_api_Init		= (sdrplay_api_Init_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_Init");
	if (sdrplay_api_Init == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_Init\n");
	   return false;
	}

	sdrplay_api_Uninit		= (sdrplay_api_Uninit_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_Uninit");
	if (sdrplay_api_Uninit == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_Uninit\n");
	   return false;
	}

	sdrplay_api_Update		= (sdrplay_api_Update_t)
	                 GETPROCADDRESS (Handle, "sdrplay_api_Update");
	if (sdrplay_api_Update == nullptr) {
	   fprintf (stderr, "Could not find sdrplay_api_Update\n");
	   return false;
	}

	return true;
}

void	sdrplayHandler_v3::report_overload_state (bool b) {
	if (b)
	   overloadLabel -> setStyleSheet ("QLabel {background-color : red}");
	else
	   overloadLabel -> setStyleSheet ("QLabel {background-color : green}");

}

