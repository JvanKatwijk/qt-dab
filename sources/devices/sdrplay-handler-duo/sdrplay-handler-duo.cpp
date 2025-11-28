#
/*
 *    Copyright (C) 2025
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
#include	"sdrplay-handler-duo.h"
#include	"position-handler.h"
#include	"duo-commands.h"

#include	"errorlog.h"
#include	"settingNames.h"
#include	"settings-handler.h"
#include	"device-exceptions.h"

#include	<QMessageBox>

#define SDRPLAY_RSPduo_ 3

#define SDRPLAY_SETTINGS        "SDRPLAY_SETTINGS_V3"
#define SDRPLAY_IFGRDB          "sdrplay-ifgrdb"
#define SDRPLAY_LNASTATE        "sdrplay-lnastate"
#define SDRPLAY_PPM             "sdrplay-ppm"
#define SDRPLAY_AGCMODE         "sdrplay_agcMode"
#define SDRPLAY_BIAS_T          "biasT_selector"
#define SDRPLAY_NOTCH           "notch_selector" 
#define SDRPLAY_ANTENNA_DX      "Antenna_dx" 
#define SDRPLAY_ANTENNA_RSP2    "Antenna_rsp2" 
#define SDRPLAY_ANTENNA_duo     "Antenna_duo"
#define SDRPLAY_TUNER           "tuner"

static int lnaStates [] = {0, 6, 12, 18, 20, 26, 32, 38, 57, 62};
//

#define	INRATE	8192000
static
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

	sdrplayHandler_duo::
	           sdrplayHandler_duo  (QSettings *s,
	                                const QString &recorderVersion,
	                                errorLogger *theLogger):
	                                          _I_Buffer (4 * 1024 * 1024) {
	sdrplaySettings			= s;
	this	-> recorderVersion	= recorderVersion;
	theErrorLogger			= theLogger;
        setupUi (&myFrame);
	setPositionAndSize (s, &myFrame, SDRPLAY_SETTINGS);
	myFrame. show	();

	overloadLabel_A -> setStyleSheet ("QLabel {background-color : green}");
	overloadLabel_B -> setStyleSheet ("QLabel {background-color : green}");
	nrBits			= 14;	// default
	denominator		= 2048.0f;	// default

//	See if there are settings from previous incarnations
//	and config stuff

	GRdBSelector_A 		-> setValue (
	            value_i (sdrplaySettings, SDRPLAY_SETTINGS,
	                                          SDRPLAY_IFGRDB, 20));
	GRdBValue		= GRdBSelector_A -> value ();
	
	lnaGainSetting_A		-> setValue (
	            value_i (sdrplaySettings, SDRPLAY_SETTINGS,
	                                          SDRPLAY_LNASTATE, 4));
	lnaGainSetting_A		-> setMaximum (9);
	lnaState		= lnaGainSetting_A -> value ();

	ppmControl_A		-> setValue (
	            value_f (sdrplaySettings, SDRPLAY_SETTINGS,
	                                          SDRPLAY_PPM, 0.0));
	ppmValue		= ppmControl_A -> value ();

	agcMode		= value_i (sdrplaySettings, SDRPLAY_SETTINGS,
	                                      SDRPLAY_AGCMODE, 0) != 0;

	if (agcMode) {
	   agcControl_A -> setChecked (true);
	   GRdBSelector_A         -> hide ();
	   gainsliderLabel_A      -> hide ();
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
	   notch_selector_A -> setChecked (true);
//
//	and be prepared for future changes in the settings
	connect (GRdBSelector_A, qOverload<int>(&QSpinBox::valueChanged),
	         this, &sdrplayHandler_duo::setIfGainReduction_A);
	connect (GRdBSelector_B, qOverload<int>(&QSpinBox::valueChanged),
	         this, &sdrplayHandler_duo::setIfGainReduction_B);
	connect (lnaGainSetting_A, qOverload<int>(&QSpinBox::valueChanged),
	         this, &sdrplayHandler_duo::setLnaGainReduction_A);
	connect (lnaGainSetting_B, qOverload<int>(&QSpinBox::valueChanged),
	         this, &sdrplayHandler_duo::setLnaGainReduction_B);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (agcControl_A, &QCheckBox::checkStateChanged,
#else
	connect (agcControl_A, &QCheckBox::stateChanged,
#endif
	         this, &sdrplayHandler_duo::setAgcControl_A);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (agcControl_B, &QCheckBox::checkStateChanged,
#else
	connect (agcControl_B, &QCheckBox::stateChanged,
#endif
	         this, &sdrplayHandler_duo::setAgcControl_B);

	connect (ppmControl_A, qOverload<double>(&QDoubleSpinBox::valueChanged),
	         this, &sdrplayHandler_duo::setPpmControl_A);
	connect (ppmControl_B, qOverload<double>(&QDoubleSpinBox::valueChanged),
	         this, &sdrplayHandler_duo::setPpmControl_B);

#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (notch_selector_A, &QCheckBox::checkStateChanged,	
#else
	connect (notch_selector_A, &QCheckBox::stateChanged,	
#endif
	         this, &sdrplayHandler_duo::setNotch_A);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (notch_selector_B, &QCheckBox::checkStateChanged,	
#else
	connect (notch_selector_B, &QCheckBox::stateChanged,	
#endif
	         this, &sdrplayHandler_duo::setNotch_B);

#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (biasT_selector, &QCheckBox::checkStateChanged,	
#else
	connect (biasT_selector, &QCheckBox::stateChanged,	
#endif
	         this, &sdrplayHandler_duo::setBiasT);

#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (this, &sdrplayHandler_duo::overloadStateChanged_A,
#else
	connect (this, &sdrplayHandler_duo::overloadStateChanged_A,
#endif
	         this, &sdrplayHandler_duo::reportOverloadState_A);

#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (this, &sdrplayHandler_duo::overloadStateChanged_B,
#else
	connect (this, &sdrplayHandler_duo::overloadStateChanged_B,
#endif
	         this, &sdrplayHandler_duo::reportOverloadState_B);

	connect (tunerA_selector, &QPushButton::clicked,
	         this, &sdrplayHandler_duo::handle_Tuner_A);
	connect (tunerB_selector, &QPushButton::clicked,
	         this, &sdrplayHandler_duo::handle_Tuner_B);

	currentTuner. store ('A');
	lastFrequency	= MHz (220);
	theGain		= -1;
	debugControl	-> hide ();
	failFlag. store (false);
	successFlag. store (false);
	errorCode	= 0;

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

//	run the "handler" task
	start ();
	while (!failFlag. load () && !successFlag. load () && isRunning ())
	   usleep (1000);
	if (failFlag. load ()) {
	   while (isRunning ())
	      usleep (1000);
	   throw device_exception (errorMessage (errorCode));
	}
	
	fprintf (stderr, "setup sdrplay duo seems successfull\n");
}

	sdrplayHandler_duo::~sdrplayHandler_duo () {
	threadRuns. store (false);
	while (isRunning ())
	   usleep (1000);
//	thread should be stopped by now
	myFrame. hide ();
	storeWidgetPosition (sdrplaySettings, &myFrame, SDRPLAY_SETTINGS);

	store (sdrplaySettings, SDRPLAY_SETTINGS,
	                          SDRPLAY_PPM, ppmControl_A -> value ());
	store (sdrplaySettings, SDRPLAY_SETTINGS,
	                          SDRPLAY_IFGRDB, GRdBSelector_A -> value ());
	store (sdrplaySettings, SDRPLAY_SETTINGS,
	                          SDRPLAY_LNASTATE, lnaGainSetting_A -> value ());
	store (sdrplaySettings, SDRPLAY_SETTINGS,
	                          SDRPLAY_AGCMODE, agcControl_A -> isChecked() ? 1 : 0);
	sdrplaySettings	-> sync();
}

/////////////////////////////////////////////////////////////////////////
//	Implementing the interface
/////////////////////////////////////////////////////////////////////////

bool	sdrplayHandler_duo::restartReader (int32_t newFreq, int skipped) {
duo_restart r (newFreq);

        if (receiverRuns. load ())
           return true;
        lastFrequency    = newFreq;
	this ->  toSkip	= skipped;
	_I_Buffer. FlushRingBuffer();
	return messageHandler (&r);
}

void	sdrplayHandler_duo::stopReader	() {
duo_stop r;
        if (!receiverRuns. load ())
           return;
        messageHandler (&r);	// synchronous call
	_I_Buffer. FlushRingBuffer();
}

int32_t	sdrplayHandler_duo::getSamples (std::complex<float> *V, int32_t size) { 
	if (!receiverRuns. load ())
	   return 0;
	int amount      = _I_Buffer. getDataFromBuffer (V, size);
        return amount;
}

int32_t	sdrplayHandler_duo::Samples	() {
	if (!receiverRuns. load ())
	   return 0;
	return _I_Buffer. GetRingBufferReadAvailable ();
}

void	sdrplayHandler_duo::resetBuffer	() {
	_I_Buffer. FlushRingBuffer();
}

int16_t	sdrplayHandler_duo::bitDepth	() {
	return nrBits;
}

QString	sdrplayHandler_duo::deviceName	() {
	return deviceModel + ":" + serial;
}

void	sdrplayHandler_duo::
	         updatePowerOverload_A (sdrplay_api_EventParamsT *params) {
	sdrplay_api_Update (chosenDevice -> dev,
	                    sdrplay_api_Tuner_A,
	                    sdrplay_api_Update_Ctrl_OverloadMsgAck,
	                    sdrplay_api_Update_Ext1_None);
	emit overloadStateChanged_A (
	        params -> powerOverloadParams.powerOverloadChangeType ==
	                                   sdrplay_api_Overload_Detected);
}

void	sdrplayHandler_duo::
	         updatePowerOverload_B (sdrplay_api_EventParamsT *params) {
	sdrplay_api_Update (chosenDevice -> dev,
	                    sdrplay_api_Tuner_B,
	                    sdrplay_api_Update_Ctrl_OverloadMsgAck,
	                    sdrplay_api_Update_Ext1_None);
	emit overloadStateChanged_B (
	        params -> powerOverloadParams.powerOverloadChangeType ==
	                                   sdrplay_api_Overload_Detected);
}

//	private slots
void	sdrplayHandler_duo::setAgcControl_A (int dummy) {
bool    agcMode = agcControl_A -> isChecked ();
duo_Agc_A r (agcMode, 30);
	(void)dummy;
        messageHandler (&r);
	if (agcMode) {
           GRdBSelector_A         -> hide ();
           gainsliderLabel_A      -> hide ();
	}
	else {
	   duo_GRdB_A r2 (GRdBSelector_A -> value ());
	   GRdBSelector_A		-> show ();
	   gainsliderLabel_A	-> show ();
	   messageHandler  (&r2);
	}
}

void	sdrplayHandler_duo::setAgcControl_B (int dummy) {
bool    agcMode = agcControl_B -> isChecked ();
duo_Agc_B r (agcMode, 30);
	(void)dummy;
        messageHandler (&r);
	if (agcMode) {
           GRdBSelector_B         -> hide ();
           gainSliderLabel_B      -> hide ();
	}
	else {
	   duo_GRdB_B r2 (GRdBSelector_B -> value ());
	   GRdBSelector_B	-> show ();
	   gainSliderLabel_B	-> show ();
	   messageHandler  (&r2);
	}
}

void	sdrplayHandler_duo::setIfGainReduction_A (int GRdB) {
duo_GRdB_A r (GRdB);
	
	if (!receiverRuns. load ())
           return;
        messageHandler (&r);
}

void	sdrplayHandler_duo::setIfGainReduction_B (int GRdB) {
duo_GRdB_B r (GRdB);
	
	if (!receiverRuns. load ())
           return;
        messageHandler (&r);
}

void	sdrplayHandler_duo::setLnaGainReduction_A (int lnaState) {
duo_lna_A r (lnaState);

	if (!receiverRuns. load ())
           return;
        messageHandler (&r);
}

void	sdrplayHandler_duo::setLnaGainReduction_B (int lnaState) {
duo_lna_B r (lnaState);

	if (!receiverRuns. load ())
           return;
        messageHandler (&r);
}

void	sdrplayHandler_duo::setPpmControl_A (int ppm) {
duo_ppm_A r (ppm);
        messageHandler (&r);
}

void	sdrplayHandler_duo::setPpmControl_B (int ppm) {
duo_ppm_B r (ppm);
        messageHandler (&r);
}

void	sdrplayHandler_duo::setNotch_A (int v) {
duo_notch_A r (notch_selector_A -> isChecked () ? 1 : 0);
	(void)v;
	messageHandler (&r);
	store (sdrplaySettings, SDRPLAY_SETTINGS,
	                           SDRPLAY_NOTCH,
	                              notch_selector_A -> isChecked () ? 1 : 0);
}

void	sdrplayHandler_duo::setNotch_B (int v) {
duo_notch_B r (notch_selector_B -> isChecked () ? 1 : 0);
	(void)v;
	messageHandler (&r);
	store (sdrplaySettings, SDRPLAY_SETTINGS,
	                           SDRPLAY_NOTCH,
	                              notch_selector_B -> isChecked () ? 1 : 0);
}

void	sdrplayHandler_duo::setBiasT (int v) {
duo_biasT r (biasT_selector -> isChecked () ? 1 : 0);

	(void)v;
	messageHandler (&r);
	store (sdrplaySettings, SDRPLAY_SETTINGS,
	                          SDRPLAY_BIAS_T,
	                              biasT_selector -> isChecked () ? 1 : 0);
}

	
//
///////////////////////////////////////////////////////////////////////////
//	Handling the GUI
//////////////////////////////////////////////////////////////////////
//
//	Since the daemon is not threadproof, we have to package the
//	actual interface into its own thread.
//	Communication with that thread is synchronous!
//

void	sdrplayHandler_duo::setLnaBounds (int low, int high) {
	lnaGainSetting_A	-> setRange (low, high - 1);
	lnaGainSetting_B	-> setRange (low, high - 1);
}

void	sdrplayHandler_duo::setSerial	(const QString& s) {
	serialNumber	-> setText (s);
}

void	sdrplayHandler_duo::setApiVersion (float version) {
QString v = QString::number (version, 'r', 2);
	api_version	-> display (v);
}

void    sdrplayHandler_duo::showLnaGain (int g) {
        lnaGRdBDisplay_A  -> display (g);
}
///////////////////////////////////////////////////////////////////////
//	the real controller starts here
///////////////////////////////////////////////////////////////////////

bool    sdrplayHandler_duo::messageHandler (duoCommand *r) {
        serverQueue. push (r);
	serverJobs. release (1);
	while (!r -> waiter. tryAcquire (1, 1000))
	   if (!threadRuns. load ())
	      return false;
	return true;
}

static
void    StreamBCallback (short *xi, short *xq,
                         sdrplay_api_StreamCbParamsT *params,
                         unsigned int numSamples,
	                 unsigned int reset,
                         void *cbContext) {
sdrplayHandler_duo *p	= static_cast<sdrplayHandler_duo *> (cbContext);
std::complex<float> localBuf [numSamples];
	(void)params;
	if (reset)
	   return;
	if (!p -> receiverRuns. load ())
	   return;

	if (p -> currentTuner. load () != 'B')
	   return;
	for (int i = 0; i <  (int)numSamples; i ++) {
	   std::complex<float> symb =
	            std::complex<float> (xi [i] / 2048.0, xq [i] / 2048.0);
	   localBuf [i] = symb;
	}
	p -> processInput (localBuf, numSamples);
}

static
void	StreamACallback (short *xi, short *xq,
                         sdrplay_api_StreamCbParamsT *params,
                         unsigned int numSamples, unsigned int reset,
                         void *cbContext) {
sdrplayHandler_duo *p	= static_cast<sdrplayHandler_duo *> (cbContext);
std::complex<float> localBuf [numSamples];
	(void)params;
	if (reset)
	   return;
	if (!p -> receiverRuns. load ())
	   return;

	if (p -> currentTuner. load () !='A')
	   return;
	for (int i = 0; i <  (int)numSamples; i ++) {
	   std::complex<float> symb =
	            std::complex<float> (xi [i] / 2048.0, xq [i] / 2048.0);
	   localBuf [i] = symb;
	}
	p -> processInput (localBuf, numSamples);
}

static
void	EventCallback (sdrplay_api_EventT eventId,
                       sdrplay_api_TunerSelectT tuner,
                       sdrplay_api_EventParamsT *params,
                       void *cbContext) {
sdrplayHandler_duo *p	= static_cast<sdrplayHandler_duo *> (cbContext);
	switch (eventId) {
	   case sdrplay_api_GainChange:
	      if (tuner == sdrplay_api_Tuner_A)
	         p -> showTunerGain_A (params -> gainParams. currGain);
	      if (tuner == sdrplay_api_Tuner_B)
	         p -> showTunerGain_B (params -> gainParams. currGain);
	      break;

	   case sdrplay_api_PowerOverloadChange:
	      if (tuner == sdrplay_api_Tuner_A)
	         p -> updatePowerOverload_A (params);
	      if (tuner == sdrplay_api_Tuner_B)
	         p -> updatePowerOverload_B (params);
	      break;

	case sdrplay_api_RspDuoModeChange:
	fprintf (stderr, 
	          "sdrplay_api_EventCb: %s, tuner=%s modeChangeType=%s\n",
	                 "sdrplay_api_RspDuoModeChange",
	                 (tuner == sdrplay_api_Tuner_A)?
	                      "sdrplay_api_Tuner_A": "sdrplay_api_Tuner_B",
                         (params -> rspDuoModeParams.modeChangeType ==
	                              sdrplay_api_MasterInitialised)?
	                      "sdrplay_api_MasterInitialised":
	                 (params -> rspDuoModeParams.modeChangeType ==
	                              sdrplay_api_SlaveAttached)?
	                      "sdrplay_api_SlaveAttached":
	                 (params -> rspDuoModeParams.modeChangeType ==
	                              sdrplay_api_SlaveDetached)?
	                       "sdrplay_api_SlaveDetached":
	                 (params -> rspDuoModeParams.modeChangeType ==
	                              sdrplay_api_SlaveInitialised)?
	                       "sdrplay_api_SlaveInitialised":
	                 (params -> rspDuoModeParams.modeChangeType ==
	                              sdrplay_api_SlaveUninitialised)?
	                       "sdrplay_api_SlaveUninitialised":
	                 (params -> rspDuoModeParams.modeChangeType ==
	                              sdrplay_api_MasterDllDisappeared)?
	                       "sdrplay_api_MasterDllDisappeared":
	                 (params->rspDuoModeParams.modeChangeType ==
	                              sdrplay_api_SlaveDllDisappeared)?
	                             "sdrplay_api_SlaveDllDisappeared":
	                                                   "unknown type");

	break;

	   default:
	      fprintf (stderr, "event %d\n", eventId);
	      break;
	}
}

void	sdrplayHandler_duo::run		() {
sdrplay_api_ErrT        err;
sdrplay_api_DeviceT     devs [6];
uint32_t                ndev;
int	deviceIndex	= 0;
        threadRuns. store (false);
	receiverRuns. store (false);

	showState ("");
	chosenDevice		= nullptr;

	connect (this, &sdrplayHandler_duo::setSerialSignal,
	         this, &sdrplayHandler_duo::setSerial);
	connect (this, &sdrplayHandler_duo::setApiVersionSignal,
	         this, &sdrplayHandler_duo::setApiVersion);

	denominator		= 2048.0f;		// default
	nrBits			= 12;		// default


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

//	Check API versions match
        err = sdrplay_api_ApiVersion (&apiVersion);
        if (err  != sdrplay_api_Success) {
	   theErrorLogger -> add (recorderVersion,
                                     sdrplay_api_GetErrorString (err));
	   errorCode	= 4;
	   goto closeAPI;
        }

	if (apiVersion < 3.07) {
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
	      fprintf (stderr, "Fout 6\n");
	      goto unlockDevice_closeAPI;
	   }
	}

	fprintf (stderr, "MaxDevs=%d NumDevs=%d\n",
                   (int)(sizeof(devs) / sizeof(sdrplay_api_DeviceT)), ndev);

	if (ndev == 0) {
	   theErrorLogger -> add (recorderVersion, "no valid device found\n");
	   errorCode	= 7;
	   goto unlockDevice_closeAPI;
	}

	for (int i = 0; i < (int)ndev; i++) 
           if (devs [i]. hwVer == SDRPLAY_RSPduo_ID)
              fprintf (stderr,
	               "Dev%d: SerNo=%s hwVer=%d tuner=0x%.2x rspDuoMode=0x%.2x\n",
                          i,
                          devs [i]. SerNo,
                          devs [i]. hwVer,
                          devs [i]. tuner, devs[i].rspDuoMode);

	deviceIndex	= 0;
	hwVersion	= devs [deviceIndex]. hwVer;
	if (hwVersion != SDRPLAY_RSPduo_)
	   throw device_exception ("This handler is solely for the RspDuo\n");

	chosenDevice	= &devs [deviceIndex];

	serial		= devs [deviceIndex]. SerNo;
	chosenDevice	-> tuner  = sdrplay_api_Tuner_Both;
	chosenDevice	-> rspDuoMode = sdrplay_api_RspDuoMode_Dual_Tuner;
	chosenDevice	-> rspDuoSampleFreq	= INRATE;

	fprintf (stderr,
	        "Dev%d: SerNo=%s hwVer=%d tuner=0x%.2x rspDuoMode=0x%.2x\n",
	                  deviceIndex,
                          devs [deviceIndex]. SerNo,
                          devs [deviceIndex]. hwVer,
                          devs [deviceIndex]. tuner,
	                  devs [deviceIndex].rspDuoMode);
//	select chosen device
	err	= sdrplay_api_SelectDevice (chosenDevice);
	if (err != sdrplay_api_Success) {
	   theErrorLogger -> add (recorderVersion,
	                         sdrplay_api_GetErrorString (err));
	   errorCode	= 8;
	   fprintf (stderr, "fout 8\n");
	   goto unlockDevice_closeAPI;
	}

	fprintf (stderr, "selected samplerate %f\n",
	                chosenDevice -> rspDuoSampleFreq);
//	Unlock API now that device is selected
	sdrplay_api_UnlockDeviceApi ();

	err		= sdrplay_api_GetDeviceParams (chosenDevice -> dev,
	                                                &deviceParams);
	fprintf (stderr, "device parameters\n");
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "sdrplay_api_GetDeviceParams failed %s\n",
	                         sdrplay_api_GetErrorString (err));
	   throw (21);
	}

	if (deviceParams == nullptr) {
	   fprintf (stderr, "sdrplay_api_GetDeviceParams return null as par\n");
	   throw (22);
	}


	deviceParams	-> devParams -> fsFreq. fsHz	= INRATE;
//	Configure tuner parameters (depends on selected Tuner
//	which parameters to use)
	chParams_A	= deviceParams -> rxChannelA;
	if (chParams_A == nullptr) {
	   fprintf (stderr, "ChannelParams = NULL\n");
	   throw (25);
	}

	chParams_A	-> tunerParams. rfFreq. rfHz    = (float)220000000;
	chParams_A	-> tunerParams. bwType = sdrplay_api_BW_1_536;
	chParams_A	-> tunerParams. ifType = sdrplay_api_IF_2_048;
	chParams_A	-> tunerParams. gain.gRdB = 40;
	chParams_A	-> tunerParams. gain.LNAstate = 3;

        chParams_A	-> ctrlParams. agc. setPoint_dBfs = -30;
        chParams_A	-> ctrlParams. agc. attack_ms = 500;
        chParams_A	-> ctrlParams. agc. decay_ms = 500;
        chParams_A	-> ctrlParams. agc. decay_delay_ms = 200;
        chParams_A	-> ctrlParams. agc. decay_threshold_dB = 3;
//	chParams_A	-> ctrlParams.agc.enable = sdrplay_api_AGC_CTRL_EN;
	chParams_A	-> ctrlParams.agc.enable = sdrplay_api_AGC_DISABLE;

//	assign callback functions
	cbFns. StreamACbFn	= StreamACallback;
	cbFns. StreamBCbFn	= StreamBCallback;
	cbFns. EventCbFn	= EventCallback;

	err	= sdrplay_api_Init (chosenDevice -> dev, &cbFns, this);
	fprintf (stderr, "%s\n", sdrplay_api_GetErrorString (err));

        if (GRdBValue > 59)
           this -> GRdBValue = 59;
        if (GRdBValue < 20)
           this -> GRdBValue = 20;

        if (this -> agcMode) { 
           chParams_A	-> ctrlParams. agc. enable = sdrplay_api_AGC_CTRL_EN;
	}
	else {
	   chParams_A   -> ctrlParams. agc. enable =
                                                  sdrplay_api_AGC_DISABLE;
	}
	deviceLabel	-> setText (deviceModel);
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
	      case DUO_RESTART: {
	         duo_restart *p = (duo_restart *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = do_restart (p -> freq);
	         receiverRuns. store (true);
	         p -> waiter. release (1);
	         fprintf (stderr, "we zijn gestart %d\n", p -> freq);
	         break;
	      }
	       
	      case DUO_STOP: {
	         duo_stop *p = (duo_stop *)(serverQueue. front ());
	         serverQueue. pop ();
	         receiverRuns. store (false);
	         p -> waiter. release (1);
	         fprintf (stderr, "en weer gestopt\n");
	         break;
	      }
	       
	      case DUO_SETAGC_A: {
	         duo_Agc_A *p = 
	                    (duo_Agc_A *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = do_setAgc ('A', -p -> setPoint, p -> agcMode);
	         p -> waiter. release (1);
	         break;
	      }

	      case DUO_SETAGC_B: {
	         duo_Agc_B *p = 
	                    (duo_Agc_B *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = do_setAgc ('B', -p -> setPoint, p -> agcMode);
	         p -> waiter. release (1);
	         break;
	      }

	      case DUO_SETLNA_A: {
	         duo_lna_A *p =
	                    (duo_lna_A *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = do_setLna ('A', p -> lnaState);
                 p -> waiter. release (1);
	         break;
	      }

	      case DUO_SETLNA_B: {
	         duo_lna_B *p =
	                    (duo_lna_B *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = do_setLna ('B', p -> lnaState);
                 p -> waiter. release (1);
	         break;
	      }

	      case DUO_SETGRDB_A: {
	         duo_GRdB_A *p =
	                     (duo_GRdB_A *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = do_setGRdB ('A', p -> GRdBValue);
                 p -> waiter. release (1);
	         break;
	      }

	      case DUO_SETGRDB_B: {
	         duo_GRdB_B *p =
	                     (duo_GRdB_B *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = do_setGRdB ('B', p -> GRdBValue);
                 p -> waiter. release (1);
	         break;
	      }

	      case DUO_SETPPM_A: {
	         duo_ppm_A *p =
	                    (duo_ppm_A *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = do_setPpm ('A', p -> ppmValue);
	         p -> waiter. release (1);
	         break;
	      }

	      case DUO_SETPPM_B: {
	         duo_ppm_B *p =
	                    (duo_ppm_B *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = do_setPpm ('B', p -> ppmValue);
	         p -> waiter. release (1);
	         break;
	      }

	      case DUO_SETNOTCH_A: {
	         duo_notch_A *p =
	                       (duo_notch_A *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = do_setNotch ('A', p -> checked);
                 p -> waiter. release (1);
	         break;
	      }

	      case DUO_SETNOTCH_B: {
	         duo_notch_B *p =
	                       (duo_notch_B *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = do_setNotch ('B', p -> checked);
                 p -> waiter. release (1);
	         break;
	      }

	      case DUO_SETBIAS_T: {
	         duo_biasT *p =
	                    (duo_biasT *)(serverQueue. front ());
	         serverQueue. pop ();
	         p -> result = do_setBiasT (p -> checked);
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
//closeApi:
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

bool	sdrplayHandler_duo::do_restart (int freq) {
sdrplay_api_ErrT        err;

	chParams_A -> tunerParams. rfFreq. rfHz = (float)freq;
	err =  sdrplay_api_Update (chosenDevice -> dev,
	                           sdrplay_api_Tuner_Both,
                                   sdrplay_api_Update_Tuner_Frf,
                                   sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   QString errorString =  sdrplay_api_GetErrorString (err);
	   showState (errorString);
	   theErrorLogger -> add (deviceModel, errorString);
	   return false;
	}

	this -> freq	= freq;
	showState (QString ("Restart at ") + QString::number (freq / 1000) + "Khz");
	return true;
}

bool	sdrplayHandler_duo::do_setPpm	(char tuner, double ppmValue) {
sdrplay_api_ErrT err;

	deviceParams -> devParams -> ppm = ppmValue;
	err = sdrplay_api_Update (chosenDevice -> dev,
	                          tuner == 'A' ? sdrplay_api_Tuner_A :
	                                         sdrplay_api_Tuner_B,
	                          sdrplay_api_Update_Dev_Ppm,
	                          sdrplay_api_Update_Ext1_None);
	return err == sdrplay_api_Success;
}

//	setting agc is common to all models
bool	sdrplayHandler_duo::do_setAgc (char tuner, int setPoint, bool on) {
sdrplay_api_ErrT err;

	if (on) {
	   chParams_A -> ctrlParams. agc. setPoint_dBfs = setPoint;
	   chParams_A -> ctrlParams. agc. enable = sdrplay_api_AGC_CTRL_EN;
	}
	else {
	   chParams_A -> ctrlParams.agc.enable = sdrplay_api_AGC_DISABLE;
	}

	err = sdrplay_api_Update (chosenDevice -> dev,
	                          tuner == 'A' ? sdrplay_api_Tuner_A:
	                                        sdrplay_api_Tuner_B,
	                          sdrplay_api_Update_Ctrl_Agc,
	                          sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   QString errorString = sdrplay_api_GetErrorString (err);
	   fprintf (stderr, "error %s in setAgc\n",
	                            errorString. toLatin1 (). data ());
	}
	return err == sdrplay_api_Success;
}

bool	sdrplayHandler_duo::do_setLna	(char tuner, int lnaState) {
sdrplay_api_ErrT        err;

	chParams_A -> tunerParams. gain. LNAstate = lnaState;
	err = sdrplay_api_Update (chosenDevice -> dev,
	                          tuner == 'A' ? sdrplay_api_Tuner_A:
	                                         sdrplay_api_Tuner_B,
	                          sdrplay_api_Update_Tuner_Gr,
	                          sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   QString errorString = sdrplay_api_GetErrorString (err);
	   showState (errorString);
	   theErrorLogger -> add (deviceModel, errorString);
	   return false;
	}
	this	-> lnaState	= lnaState;
	if (tuner == 'A')
           lnaGRdBDisplay_A  -> display (lnaStates [lnaState]);
	else
           lnaGRdBDisplay_B  -> display (lnaStates [lnaState]);
	return true;
}

bool	sdrplayHandler_duo::do_setGRdB	(char tuner, int GRdBValue) {
sdrplay_api_ErrT err;

	chParams_A -> tunerParams. gain.gRdB = GRdBValue;
	err =  sdrplay_api_Update (chosenDevice -> dev,
	                           tuner == 'A' ? sdrplay_api_Tuner_A:
	                                          sdrplay_api_Tuner_B,
	                           sdrplay_api_Update_Tuner_Gr,
	                           sdrplay_api_Update_Ext1_None);
	if (err == sdrplay_api_Success) {
	   this -> GRdBValue = GRdBValue;
	   return true;
	}
	return false;
}

bool	sdrplayHandler_duo::do_setNotch (char tuner, bool on) {
	(void)tuner; (void)on;
	return false;
}

bool	sdrplayHandler_duo::do_setBiasT	(bool biasT_value) {
	(void)biasT_value;
	return false;
}

//

/////////////////////////////////////////////////////////////////////////////
//	handling the library
/////////////////////////////////////////////////////////////////////////////

HINSTANCE	sdrplayHandler_duo::fetchLibrary () {
HINSTANCE	Handle	= nullptr;
#ifdef	__MINGW32__
HKEY APIkey;
wchar_t APIkeyValue [256];
ULONG APIkeyValue_length = 255;

	wchar_t *libname = (wchar_t *)L"sdrplay_api.dll";
	Handle	= LoadLibrary (libname);
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

void	sdrplayHandler_duo::releaseLibrary () {
#ifdef __MINGW32__
        FreeLibrary (Handle);
#else
	dlclose (Handle);
#endif
}

bool	sdrplayHandler_duo::loadFunctions () {
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

	sdrplay_api_SwapRspDuoActiveTuner =
	                          (sdrplay_api_SwapRspDuoActiveTuner_t)
	                 GETPROCADDRESS (Handle, 
	                       "sdrplay_api_SwapRspDuoActiveTuner");
	if (sdrplay_api_SwapRspDuoActiveTuner == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                    "Could not load sdrplay_api_SwapRspDuoActiveTuner");
	   return false;
	}

	sdrplay_api_SwapRspDuoDualTunerModeSampleRate =
	                 (sdrplay_api_SwapRspDuoDualTunerModeSampleRate_t)
	                 GETPROCADDRESS (Handle, 
	                 "sdrplay_api_SwapRspDuoDualTunerModeSampleRate");
	if (sdrplay_api_SwapRspDuoDualTunerModeSampleRate == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                 "could not load sdrplay_api_SwapRspDuoDualTunerModeSampleRate");
	   return false;
	}

	sdrplay_api_SwapRspDuoMode =
	                (sdrplay_api_SwapRspDuoMode_t)
	                 GETPROCADDRESS (Handle, 
	                              "sdrplay_api_SwapRspDuoMode");
	if (sdrplay_api_SwapRspDuoMode == nullptr) {
	   theErrorLogger -> add (recorderVersion,
	                         "could not load sdrplay_api_SwapRspDuoMode");
	   return false;
	}
	
	return true;
}

void	sdrplayHandler_duo::reportOverloadState_A (bool b) {
	if (b)
	   overloadLabel_A -> setStyleSheet ("QLabel {background-color : red}");
	else
	   overloadLabel_A -> setStyleSheet ("QLabel {background-color : green}");
}

void	sdrplayHandler_duo::reportOverloadState_B (bool b) {
	if (b)
	   overloadLabel_B -> setStyleSheet ("QLabel {background-color : red}");
	else
	   overloadLabel_B -> setStyleSheet ("QLabel {background-color : green}");
}

void	sdrplayHandler_duo::showTunerGain_A	(double gain) {
	gainDisplay_A -> setText (QString::number (gain, 'f', 0) + "dB");
}

void	sdrplayHandler_duo::showTunerGain_B	(double gain) {
	gainDisplay_B -> setText (QString::number (gain, 'f', 0) + "dB");
}

void	sdrplayHandler_duo::showState	(const QString &s) {
	stateLabel	-> setAlignment (Qt::AlignCenter);
	stateLabel	-> setText (s);
}

void	sdrplayHandler_duo::enableBiasT (bool b) {
	if (b)
	  biasT_selector -> show ();
	else
	  biasT_selector -> hide ();
}

void	sdrplayHandler_duo::processInput (std::complex<float> *b, int amount) {
	_I_Buffer. putDataIntoBuffer (b,  amount);
}

void	sdrplayHandler_duo::handle_Tuner_A () {
	currentTuner. store ('A');
	showTuner	-> setText ("Tuner A");
}

void	sdrplayHandler_duo::handle_Tuner_B () {
	currentTuner. store ('B');
	showTuner	-> setText ("Tuner B");
}

