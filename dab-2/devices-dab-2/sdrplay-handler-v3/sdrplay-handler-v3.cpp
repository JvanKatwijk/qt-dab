#
/*
 *    Copyright (C) 2014 .. 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of dab-2
 *
 *    dab-2 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation version 2 of the License.
 *
 *    dab-2 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dab-2 if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	<QThread>
#include	<QSettings>
#include	<QHBoxLayout>
#include	<QLabel>
#include	<QFileDialog>
#include	"sdrplay-handler-v3.h"
#include	"sdrplay-commands.h"
#include	"dab-processor.h"
#include	"xml-filewriter.h"
#include	"radio.h"

static
int     RSP1_Table []	= {0, 24, 19, 43};

static
int     RSP1A_Table []	= {0, 6, 12, 18, 20, 26, 32, 38, 57, 62};

static
int     RSP2_Table []	= {0, 10, 15, 21, 24, 34, 39, 45, 64};

static
int	RSPduo_Table []	= {0, 6, 12, 18, 20, 26, 32, 38, 57, 62};

static
int	get_lnaGRdB (int hwVersion, int lnaState) {
	switch (hwVersion) {
	   case 1:
	   default:
	      return RSP1_Table [lnaState];

	   case 2:
	      return RSP2_Table [lnaState];

	   case 255: 
	      return RSP1A_Table [lnaState];

	   case 3:
	      return RSPduo_Table [lnaState];
	}
}

	sdrplayHandler_v3::sdrplayHandler_v3  (RadioInterface	*mr,
	                                       QSettings	*s,
	                                       dabProcessor 	*base,
	                                       QString		&recorder):
	                                             myFrame (nullptr) {
	(void)mr;
	sdrplaySettings			= s;
	this	-> base			= base;
	this	-> recorderVersion	= recorder;
	this	-> inputRate		= 2048000;
	setupUi (&myFrame);
	myFrame.		show	();
	antennaSelector		-> hide	();
	tunerSelector		-> hide	();
	nrBits			= 12;	// default

	totalOffset		= 0;
	sdrplaySettings		-> beginGroup ("sdrplaySettings");
	gainSetPoint		=
	            sdrplaySettings -> value ("sdrplay-gainSetPoint", -35).
	                                                     toInt ();
	fprintf (stderr, "gainsetpoint found %d\n", gainSetPoint);
	gain_setpoint		-> setValue (gainSetPoint);
	lnaState		=
	            sdrplaySettings -> value ("sdrplay-lnastate", 4). toInt();
	lnaGainSetting	-> setValue (lnaState);
	ppmValue		=
	            sdrplaySettings -> value ("sdrplay-ppm", 0). toInt();
	ppmControl	-> setValue (ppmValue);

	agcMode			=
	       sdrplaySettings -> value ("sdrplay-agcMode", 1). toInt() != 0;
	if (agcMode) {
	   agcControl -> setChecked (true);
	}
	sdrplaySettings	-> endGroup	();

//	and be prepared for future changes in the settings
	connect (lnaGainSetting, SIGNAL (valueChanged (int)),
	         this, SLOT (set_lnagainReduction (int)));
	connect (ppmControl, SIGNAL (valueChanged (int)),
	         this, SLOT (set_ppmControl (int)));
	connect (agcControl, SIGNAL (stateChanged (int)),
	         this, SLOT (set_agcControl (int)));
	connect (xml_dumpButton, SIGNAL (clicked ()),
	         this, SLOT (set_xmlDump ()));

	xmlDumping. store (false);
	xmlDumper	= nullptr;

	connect (antennaSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (set_antennaSelect (const QString &)));
	connect (tunerSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (set_tunerSelect (const QString &)));
	connect (gain_setpoint, SIGNAL (valueChanged (int)),
	         this, SLOT (set_gain (int)));
	threadRuns. store (false);
	reportIndicator. store (0);
	start ();
	while (reportIndicator. load () == 0)
	   usleep (1000);
	if (reportIndicator. load () > 1) {
	   while (isRunning ())
	      usleep (1000);
	   fprintf (stderr, "foute boel\n");
	   throw (24);
	}

//	OK, the controller runs, let us extract the
//	data to show on the gui

	lnaGainSetting		-> setRange (0, lna_upperBound);
	deviceLabel		-> setText (deviceName);
	api_version		-> display (apiVersion);
	serialNumber		-> setText (serial);
	lnaValueDisplay		-> display (get_lnaGRdB (hwVersion, lnaState));
	if (has_antennaSelect)
	   antennaSelector	-> show ();

	debugControl	-> hide ();
}

	sdrplayHandler_v3::~sdrplayHandler_v3 () {
	threadRuns. store (false);
	while (this -> isRunning ())
	   usleep (1000);
//	thread should be stopped by now

	sdrplaySettings	-> beginGroup ("sdrplaySettings");
	sdrplaySettings -> setValue ("sdrplay-gainSetPoint",
	                                 gain_setpoint -> value ());
	sdrplaySettings -> setValue ("sdrplay-ppm",
	                                 ppmControl -> value ());
	sdrplaySettings -> setValue ("sdrplay-lnastate",
	                                 lnaGainSetting -> value ());
	sdrplaySettings	-> setValue ("sdrplay-agcMode",
	                                  agcControl -> isChecked() ? 1 : 0);
	sdrplaySettings	-> endGroup ();
	sdrplaySettings	-> sync();

	myFrame. hide ();
}

void	sdrplayHandler_v3::avgValue	(float v) {
	averageValue	-> display (v);
}

void	sdrplayHandler_v3::dipValue	(float v) {
	nullValue	-> display (v);
}

void	sdrplayHandler_v3::freq_offset	(int f) {
	freq_offsetDisplay	-> display (f);
}

void	sdrplayHandler_v3::freq_error	(int f) {
	freq_errorDisplay	-> display (f);
}

void	sdrplayHandler_v3::show_TotalGain (float f) {
	reportedGain	-> display (f);
}

int32_t	sdrplayHandler_v3::getVFOFrequency() {
	return vfoFrequency;
}

bool	sdrplayHandler_v3::messageHandler (generalCommand *r) {
	server_queue. push (r);
	serverjobs. release (1);
	while (!r -> waiter. tryAcquire (1, 1000))
	   if (!threadRuns. load ())
              return false;
	return true;
}

void	sdrplayHandler_v3::set_lnagainReduction (int lnaState) {
lnaRequest r (lnaState);
	messageHandler (&r);
}

void	sdrplayHandler_v3::set_GRdB (int GRdB) {
GRdBRequest r (GRdB);
	messageHandler (&r);
}

void	sdrplayHandler_v3::set_agcControl (int dummy) {
bool	agcMode	= agcControl -> isChecked ();
agcRequest r (agcMode, 30);
	messageHandler (&r);
}

void	sdrplayHandler_v3::set_ppmControl (int ppm) {
ppmRequest r (ppm);
	messageHandler (&r);
}

void	sdrplayHandler_v3::set_antennaSelect	(const QString &s) {
antennaRequest r (s == "Antenna A" ? 'A' : 'B');
	messageHandler (&r);
}

void	sdrplayHandler_v3::set_tunerSelect	(const QString &s) {
	fprintf (stderr, "tuner select not implemented\n");
}

bool	sdrplayHandler_v3::restartReader	(int32_t newFreq) {
restartRequest r (newFreq);
        if (receiverRuns. load ())
           return true;
        vfoFrequency    = newFreq;
	totalOffset	= 0;
	set_GRdB (gainSetPoint);
	return messageHandler (&r);
}

void	sdrplayHandler_v3::stopReader	() {
stopRequest r;
        if (!receiverRuns. load ())
           return;
	close_xmlDump ();
	messageHandler (&r);
}

void	sdrplayHandler_v3::handle_Value (int offset,
	                                  float lowVal, float highVal) {
	totalOffset	+= offset;
	set_frequencyRequest r (vfoFrequency + totalOffset);
	freq_offsetDisplay	-> display (totalOffset);
	averageValue	->
	           display (10 * log10 ((highVal + 0.005) / denominator));
	nullValue	-> 
	           display (10 * log10 ((lowVal + 0.005) / denominator));
	messageHandler (&r);
}

void    sdrplayHandler_v3::setVFOFrequency (int newFreq) {
set_frequencyRequest r (newFreq);
        vfoFrequency    = newFreq;
	messageHandler (&r);
}

void	sdrplayHandler_v3::set_gain	(int gain) {
	set_GRdB (gain);
	gainSetPoint	= gain;
}

void	sdrplayHandler_v3::resetBuffer	() {
}

int16_t	sdrplayHandler_v3::bitDepth	() {
	return nrBits;
}

void	sdrplayHandler_v3::show		() {
	myFrame. show ();
}

void	sdrplayHandler_v3::hide		() {
	myFrame. hide ();
}

bool	sdrplayHandler_v3::isHidden	() {
	return !myFrame. isVisible ();
}

//////////////////////////////////////////////////////////////////////////////

static
void    StreamACallback (short *xi, short *xq,
                         sdrplay_api_StreamCbParamsT *params,
                         unsigned int numSamples,
	                 unsigned int reset,
                         void *cbContext) {
sdrplayHandler_v3 *p	= static_cast<sdrplayHandler_v3 *> (cbContext);
float	denominator	= (float)(p -> denominator);
std::complex<float> localBuf [numSamples];
std::complex<int16_t> dumpBuf [numSamples];
static int teller	= 0;

	(void)params;
	if (reset)
	   return;
	if (!p -> receiverRuns. load ())
	   return;

	for (int i = 0; i <  (int)numSamples; i ++) {
	   std::complex<float> symb = std::complex<float> (
	                                       (float) (xi [i]) / denominator,
	                                       (float) (xq [i]) / denominator);
	   localBuf [i] = symb;
	   dumpBuf [i] = std::complex<int16_t> (xi [i], xq [i]);
	}
	(void) p -> base -> addSymbol (localBuf, numSamples);
	if (p -> xmlDumping. load ())
	   p -> xmlWriter -> add (dumpBuf, numSamples);
}

static
void	StreamBCallback (short *xi, short *xq,
                         sdrplay_api_StreamCbParamsT *params,
                         unsigned int numSamples, unsigned int reset,
                         void *cbContext) {
	(void)xi; (void)xq; (void)params; (void)cbContext;
        if (reset)
           printf ("sdrplay_api_StreamBCallback: numSamples=%d\n", numSamples);
        return;
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
	if (params -> powerOverloadParams.powerOverloadChangeType ==
	                                    sdrplay_api_Overload_Detected) {
//	   fprintf (stderr, "Qt-DAB sdrplay_api_Overload_Detected");
	}
	else {
//	   fprintf (stderr, "Qt-DAB sdrplay_api_Overload Corrected");
	}
}

void	sdrplayHandler_v3::run		() {
sdrplay_api_ErrT        err;
sdrplay_api_DeviceT     devs [6];
uint32_t                ndev;

	reportIndicator. store (0);
        threadRuns. store (false);
	receiverRuns. store (false);

	chosenDevice		= nullptr;
	deviceParams		= nullptr;

	connect (this, SIGNAL (dipValue_signal (float)),
	         this, SLOT (dipValue (float)));
	connect (this, SIGNAL (freq_offset_signal (int)),
	         this, SLOT (freq_offset (int)));
	connect (this, SIGNAL (freq_error_signal (int)),
	         this, SLOT (freq_error (int)));
	connect (this, SIGNAL (avgValue_signal (float)),
	         this, SLOT (avgValue (float)));
	connect (this, SIGNAL (show_TotalGain_signal (float)),
	         this, SLOT (show_TotalGain (float)));

	denominator		= 2048;		// default
	nrBits			= 12;		// default

	Handle			= fetchLibrary ();
	if (Handle == nullptr)
	   return;

//	load the functions
	bool success	= loadFunctions ();
	if (!success) {
	   releaseLibrary ();
	   return;
        }
	fprintf (stderr, "functions loaded\n");

//	try to open the API
	err	= sdrplay_api_Open ();
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "sdrplay_api_Open failed %s\n",
	                          sdrplay_api_GetErrorString (err));
	   releaseLibrary ();
	   return;
	}

	fprintf (stderr, "api opened\n");

//	Check API versions match
        err = sdrplay_api_ApiVersion (&apiVersion);
        if (err  != sdrplay_api_Success) {
           fprintf (stderr, "sdrplay_api_ApiVersion failed %s\n",
                                     sdrplay_api_GetErrorString (err));
	   goto closeAPI;
        }

        if (apiVersion < 3.05) {
           fprintf (stderr, "API versions don't match (local=%.2f dll=%.2f)\n",
                                              SDRPLAY_API_VERSION, apiVersion);
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
	      goto unlockDevice_closeAPI;
	   }
	}

	if (ndev == 0) {
	   fprintf (stderr, "no valid device found\n");
	   goto unlockDevice_closeAPI;
	}

	fprintf (stderr, "%d devices detected\n", ndev);
	chosenDevice	= &devs [0];
	err	= sdrplay_api_SelectDevice (chosenDevice);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "sdrplay_api_SelectDevice failed %s\n",
	                         sdrplay_api_GetErrorString (err));
	   goto unlockDevice_closeAPI;
	}
//
//	we have a device, unlock
	sdrplay_api_UnlockDeviceApi ();

	err	= sdrplay_api_DebugEnable (chosenDevice -> dev, 
	                                         (sdrplay_api_DbgLvl_t)1);
//	retrieve device parameters, so they can be changed if needed
	err	= sdrplay_api_GetDeviceParams (chosenDevice -> dev,
	                                                     &deviceParams);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "sdrplay_api_GetDeviceParams failed %s\n",
	                         sdrplay_api_GetErrorString (err));
	   goto closeAPI;
	}

	if (deviceParams == nullptr) {
	   fprintf (stderr, "sdrplay_api_GetDeviceParams return null as par\n");
	   goto closeAPI;
	}
//
	vfoFrequency	= Khz (220000);		// default
//	set the parameter values
	chParams	= deviceParams -> rxChannelA;
	deviceParams	-> devParams -> fsFreq. fsHz	= inputRate;
	chParams	-> tunerParams. bwType = sdrplay_api_BW_1_536;
	chParams	-> tunerParams. ifType = sdrplay_api_IF_Zero;
//
//	these will change:
	chParams	-> tunerParams. rfFreq. rfHz    = 220000000.0;
	chParams	-> tunerParams. gain.gRdB	= 30;
	chParams	-> tunerParams. gain.LNAstate	= lnaState;
	chParams	-> ctrlParams.agc.enable = sdrplay_api_AGC_DISABLE;
	if (agcMode) {
	   chParams    -> ctrlParams. agc. setPoint_dBfs = -30;
	   chParams    -> ctrlParams. agc. enable =
                                             sdrplay_api_AGC_100HZ;
	}
	else
	   chParams    -> ctrlParams. agc. enable =
                                             sdrplay_api_AGC_DISABLE;
//
//	assign callback functions
	cbFns. StreamACbFn	= StreamACallback;
	cbFns. StreamBCbFn	= StreamBCallback;
	cbFns. EventCbFn	= EventCallback;

	err	= sdrplay_api_Init (chosenDevice -> dev, &cbFns, this);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "sdrplay_api_Init failed %s\n",
                                       sdrplay_api_GetErrorString (err));
	   goto unlockDevice_closeAPI;
	}
//
//	Let the parent display the values
	serial	= devs [0]. SerNo;

	hwVersion = devs [0]. hwVer;
	switch (hwVersion) {
	   case 1:		// old RSP
	      lna_upperBound	= 3;
	      deviceName	= "RSP-I";
	      denominator	= 2048;
	      nrBits		= 12;
	      has_antennaSelect	= false;
	      break;
	   case 2:		// RSP II
	      lna_upperBound	= 8;
	      deviceName 	= "RSP-II";
	      denominator	= 2048;
	      nrBits		= 14;
	      has_antennaSelect	= true;
	      break;
	   case 3:		// RSP-DUO
	      lna_upperBound	= 9;
	      deviceName	= "RSP-DUO";
	      denominator	= 2048;
	      nrBits		= 12;
	      has_antennaSelect	= false;
	      break;
	   default:
	   case 255:		// RSP-1A
	      lna_upperBound	= 9;
	      deviceName	= "RSP-1A";
	      denominator	= 8192;
	      nrBits		= 14;
	      has_antennaSelect	= false;
	      break;
	}

	threadRuns. store (true);	// it seems we can do some work
	reportIndicator. store (1);

	fprintf (stderr, "ready for action\n");
	usleep (1000);
	while (threadRuns. load ()) {
	   while (!serverjobs. tryAcquire (1, 1000))
	   if (!threadRuns. load ())
	      goto normal_exit;
//
//	here we assert that theQueue is not empty
	   switch (server_queue. front () -> cmd) {
	      case RESTART_REQUEST: {
	         restartRequest *p = (restartRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = true;
	         chParams -> tunerParams. rfFreq. rfHz =
	                                            (float)(p -> frequency);
                 err = sdrplay_api_Update (chosenDevice -> dev,
                                           chosenDevice -> tuner,
                                           sdrplay_api_Update_Tuner_Frf,
                                           sdrplay_api_Update_Ext1_None);
                 if (err != sdrplay_api_Success) {
                    fprintf (stderr, "restart: error %s\n",
                                      sdrplay_api_GetErrorString (err));
	            p -> result = false;
                 }
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
	       
	      case SETFREQUENCY_REQUEST: {
	         set_frequencyRequest *p =
	                      (set_frequencyRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = true;
	         chParams -> tunerParams. rfFreq. rfHz =
	                                    (float)(p -> frequency);
                 err = sdrplay_api_Update (chosenDevice -> dev,
                                           chosenDevice -> tuner,
                                           sdrplay_api_Update_Tuner_Frf,
                                           sdrplay_api_Update_Ext1_None);
                 if (err != sdrplay_api_Success) {
                    fprintf (stderr, "restart: error %s\n",
                                      sdrplay_api_GetErrorString (err));
	            p -> result	= false;
                 }
	         p -> waiter. release (1);
	         break;
	      }

	      case GETFREQUENCY_REQUEST: {
	         get_frequencyRequest *p =
	                      (get_frequencyRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> frequency = 
	                 chParams -> tunerParams. rfFreq. rfHz;
	         p -> result	= true;
	         p -> waiter. release (1);
	         break;
	      }

	      case AGC_REQUEST: {
	         agcRequest *p = 
	                    (agcRequest *)(server_queue. front ());
	         server_queue. pop ();
	         if (p -> agcMode) {
	            chParams    -> ctrlParams. agc. setPoint_dBfs =
	                                             - p -> setPoint;
                    chParams    -> ctrlParams. agc. enable =
                                             sdrplay_api_AGC_100HZ;
	         }
	         else
	            chParams    -> ctrlParams. agc. enable =
                                             sdrplay_api_AGC_DISABLE;

	         p -> result = true;
	         err = sdrplay_api_Update (chosenDevice -> dev,
                                           chosenDevice -> tuner,
                                           sdrplay_api_Update_Ctrl_Agc,
                                           sdrplay_api_Update_Ext1_None);
                 if (err != sdrplay_api_Success) {
                    fprintf (stderr, "agc: error %s\n",
	                                   sdrplay_api_GetErrorString (err));
	            p -> result = false;
	         }
	         p -> waiter. release (1);
	         break;
	      }

	      case GRDB_REQUEST: {
	         GRdBRequest *p =  (GRdBRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = true;
	         chParams -> tunerParams. gain. gRdB = p -> GRdBValue;
                 err = sdrplay_api_Update (chosenDevice -> dev,
                                           chosenDevice -> tuner,
                                           sdrplay_api_Update_Tuner_Gr,
                                           sdrplay_api_Update_Ext1_None);
                 if (err != sdrplay_api_Success) {
                    fprintf (stderr, "grdb: error %s\n",
                                      sdrplay_api_GetErrorString (err));
	            p -> result = false;
	         }
	         p -> waiter. release (1);
	         break;
	      }

	      case PPM_REQUEST: {
	         ppmRequest *p = (ppmRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result	= false;
	         deviceParams    -> devParams -> ppm = p -> ppmValue;
                 err = sdrplay_api_Update (chosenDevice -> dev,
                                           chosenDevice -> tuner,
                                           sdrplay_api_Update_Dev_Ppm,
                                           sdrplay_api_Update_Ext1_None);
                 if (err != sdrplay_api_Success) {
                    fprintf (stderr, "lna: error %s\n",
                                      sdrplay_api_GetErrorString (err));
	            p -> result = false;
	         }
	         p -> waiter. release (1);
	         break;
	      }

	      case LNA_REQUEST: {
	         lnaRequest *p = (lnaRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = true;
	         chParams -> tunerParams. gain. LNAstate =
	                                          p -> lnaState;
                 err = sdrplay_api_Update (chosenDevice -> dev,
                                           chosenDevice -> tuner,
                                           sdrplay_api_Update_Tuner_Gr,
                                           sdrplay_api_Update_Ext1_None);
                 if (err != sdrplay_api_Success) {
                    fprintf (stderr, "grdb: error %s\n",
                                      sdrplay_api_GetErrorString (err));
	            p -> result = false;
	         }
	         p -> waiter. release (1);
	         break;
	      }

	      case ANTENNASELECT_REQUEST: {
	         antennaRequest *p = (antennaRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = true;
	         deviceParams    -> rxChannelA -> rsp2TunerParams. antennaSel =
                                    p -> antenna == 'A' ?
                                             sdrplay_api_Rsp2_ANTENNA_A:
                                             sdrplay_api_Rsp2_ANTENNA_B;
                 err = sdrplay_api_Update (chosenDevice -> dev,
                                           chosenDevice -> tuner,
                                           sdrplay_api_Update_Rsp2_AntennaControl,
                                           sdrplay_api_Update_Ext1_None);
                 if (err != sdrplay_api_Success)
	            p -> result = false;

	         p -> waiter. release (1);
	         break;
	      }
	
	      case GAINVALUE_REQUEST: {
	         gainvalueRequest *p = 
	                        (gainvalueRequest *)(server_queue. front ());
	         server_queue. pop ();
	         p -> result = false;
	         p -> gainValue = -1;
	         p -> waiter. release (1);
	         break;
	      }

	      default:		// cannot happen
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
	reportIndicator. store (2);
	sdrplay_api_ReleaseDevice       (chosenDevice);
        sdrplay_api_Close               ();
	releaseLibrary	();
	fprintf (stderr, "De taak is gestopt\n");
}

static inline
int     constrain (int v, int l, int h) {
        if (v < l)
           return l;
        if (v > h)
           return h;
	return v;
}

HINSTANCE	sdrplayHandler_v3::fetchLibrary () {
HINSTANCE	Handle	= nullptr;
#ifdef	__MINGW32__
HKEY APIkey;
wchar_t APIkeyValue [256];
ULONG APIkeyValue_length = 255;

	wchar_t *libname = (wchar_t *)L"sdrplay_api.dll";
	Handle	= LoadLibrary (libname);
	if (Handle == nullptr) {
	   if (RegOpenKey (HKEY_LOCAL_MACHINE,
	                   TEXT("Software\\MiricsSDR\\API"),
	                   &APIkey) != ERROR_SUCCESS) {
              fprintf (stderr,
	           "failed to locate API registry entry, error = %d\n",
	           (int)GetLastError());
	      return nullptr;
	   }

	   RegQueryValueEx (APIkey,
	                    (wchar_t *)L"Install_Dir",
	                    nullptr,
	                    nullptr,
	                    (LPBYTE)&APIkeyValue,
	                    (LPDWORD)&APIkeyValue_length);
//	Ok, make explicit it is in the 32/64 bits section
	   wchar_t *x =
	        wcscat (APIkeyValue, (wchar_t *)L"\\x86\\sdrplay_api.dll");
//	        wcscat (APIkeyValue, (wchar_t *)L"\\x64\\sdrplay_api.dll");
	   RegCloseKey(APIkey);

	   Handle	= LoadLibrary (x);
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

void	sdrplayHandler_v3::set_xmlDump () {
	if (xmlDumper == nullptr) {
	  if (setup_xmlDump ())
	      xml_dumpButton	-> setText ("writing");
	}
	else {
	   close_xmlDump ();
	   xml_dumpButton	-> setText ("Dump");
	}
}

static inline
bool	isValid (QChar c) {
	return c. isLetterOrNumber () || (c == '-');
}

bool	sdrplayHandler_v3::setup_xmlDump () {
QTime	theTime;
QDate	theDate;
QString	saveDir	= sdrplaySettings -> value ("saveDir_xmlDump",
	                                   QDir::homePath ()). toString ();
	if ((saveDir != "") && (!saveDir. endsWith ("/")))
	   saveDir += "/";

	QString channel		= sdrplaySettings -> value ("channel", "xx").
	                                                       toString ();
	QString timeString      = theDate. currentDate (). toString () + "-" +
	                           theTime. currentTime(). toString ();
	for (int i = 0; i < timeString. length (); i ++)
           if (!isValid (timeString. at (i)))
              timeString. replace (i, 1, '-');
	
        QString suggestedFileName =
                    saveDir + deviceLabel -> text () + "-" + channel + "-" + timeString;

	QString fileName = QFileDialog::getSaveFileName (nullptr,
	                                         tr ("Save file ..."),
	                                         suggestedFileName + ".uff",
	                                         tr ("Xml (*.uff)"));
        fileName        = QDir::toNativeSeparators (fileName);
        xmlDumper	= fopen (fileName. toUtf8(). data(), "w");
	if (xmlDumper == nullptr)
	   return false;
	
	xmlWriter	= new xml_fileWriter (xmlDumper,
	                                      nrBits,
	                                      "int16",
	                                      2048000,
	                                      getVFOFrequency (),
	                                      "SDRplay-3",
	                                      deviceLabel -> text (),
	                                      recorderVersion);
	xmlDumping. store (true);

	QString dumper	= QDir::fromNativeSeparators (fileName);
	int x		= dumper. lastIndexOf ("/");
        saveDir		= dumper. remove (x, dumper. count () - x);
	sdrplaySettings	-> setValue ("saveDir_xmlDump", saveDir);
	return true;
}

void	sdrplayHandler_v3::close_xmlDump () {
	if (xmlDumper == nullptr)	// this can happen !!
	   return;
	xmlDumping. store (false);
	usleep (1000);
	xmlWriter	-> computeHeader ();
	delete xmlWriter;
	fclose (xmlDumper);
	xmlDumper	= nullptr;
}

