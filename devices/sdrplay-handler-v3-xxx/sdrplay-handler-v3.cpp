#
/*
 *    Copyright (C) 2014 .. 2019
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
#include	<QHBoxLayout>
#include	<QLabel>
#include	"sdrplay-handler-v3.h"
#include	"control-queue.h"

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

//	here we start
	sdrplayHandler_v3::sdrplayHandler_v3  (QSettings *s) {
	sdrplaySettings		= s;
	myFrame			= new QFrame (nullptr);
	setupUi (this -> myFrame);
	this	-> myFrame	-> show	();
	antennaSelector		-> hide	();
	tunerSelector		-> hide	();
	_I_Buffer		= new RingBuffer<
	                              std::complex<float>>(8 *1024 * 1024);
	theQueue		= new controlQueue ();

	Handle	= fetchLibrary ();
	if (Handle == nullptr)
	   throw (21);

//	load the functions
	bool success	= loadFunctions ();
	if (!success) {
#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
	   delete	_I_Buffer;
	   delete	theQueue;
           throw (23);
        }
//	See if there are settings from previous incarnations
//	and config stuff

	sdrplaySettings		-> beginGroup ("sdrplaySettings");
	GRdBSelector 		-> setValue (
	            sdrplaySettings -> value ("sdrplay-ifgrdb", 20). toInt());
	theQueue -> add (GRDB_REQUEST, GRdBSelector -> value ());

	lnaGainSetting		-> setValue (
	            sdrplaySettings -> value ("sdrplay-lnastate", 4). toInt());
	theQueue -> add (LNA_REQUEST, lnaGainSetting -> value ());

	ppmControl		-> setValue (
	            sdrplaySettings -> value ("sdrplay-ppm", 0). toInt());
	theQueue -> add (PPM_REQUEST, ppmControl -> value ());

	agcMode		=
	       sdrplaySettings -> value ("sdrplay-agcMode", 0). toInt() != 0;
	if (agcMode) 
	   theQueue -> add (AGC_REQUEST, true, GRdBSelector -> value ());
	else
	   theQueue -> add (AGC_REQUEST, false);
	if (agcMode) {
	   agcControl -> setChecked (true);
	   GRdBSelector         -> hide ();
	   gainsliderLabel      -> hide ();
	}
	sdrplaySettings	-> endGroup	();

//	and be prepared for future changes in the settings
	connect (GRdBSelector, SIGNAL (valueChanged (int)),
	         this, SLOT (set_ifgainReduction (int)));
	connect (lnaGainSetting, SIGNAL (valueChanged (int)),
	         this, SLOT (set_lnagainReduction (int)));
	connect (agcControl, SIGNAL (stateChanged (int)),
	         this, SLOT (set_agcControl (int)));
	connect (ppmControl, SIGNAL (valueChanged (int)),
	         this, SLOT (set_ppmControl (int)));
	connect (antennaSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (set_antennaSelect (const QString &)));
//

        connect (this, SIGNAL (showTotalGain (int)),
                 this, SLOT (displayGain (int)));
	connect (this, SIGNAL (setDeviceData (const QString &, int, float)),
	         this, SLOT   (showDeviceData (const QString &, int, float)));
	connect (this, SIGNAL (error (int)),
	         this, SLOT   (show_error (int)));
	running. store (false);
	receiverRuns	= false;
	start ();
}

	sdrplayHandler_v3::~sdrplayHandler_v3 () {
	stopReader ();

	threadRuns. store (false);
	while (this -> isRunning ())
	   msleep (10);
//
//	thread should be stopped by now
	sdrplaySettings	-> beginGroup ("sdrplaySettings");
	sdrplaySettings -> setValue ("sdrplay-ppm",
	                                           ppmControl -> value ());
	sdrplaySettings -> setValue ("sdrplay-ifgrdb",
	                                           GRdBSelector -> value ());
	sdrplaySettings -> setValue ("sdrplay-lnastate",
	                                           lnaGainSetting -> value ());
	sdrplaySettings	-> setValue ("sdrplay-agcMode",
	                                  agcControl -> isChecked() ? 1 : 0);
	sdrplaySettings	-> endGroup ();
	sdrplaySettings	-> sync();

	myFrame	-> hide ();
	delete	myFrame;
	delete _I_Buffer;
	delete	theQueue;

#ifdef __MINGW32__
        FreeLibrary (Handle);
#else
//	dlclose (Handle);
#endif
}
//
int32_t	sdrplayHandler_v3::defaultFrequency	() {
	return Mhz (220);
}

void	sdrplayHandler_v3::setVFOFrequency	(int32_t newFrequency) {
	theQueue	-> add (FREQUENCY_REQUEST, newFrequency);
	vfoFrequency	= newFrequency;
}

int32_t	sdrplayHandler_v3::getVFOFrequency() {
	return vfoFrequency;
}

void	sdrplayHandler_v3::set_ifgainReduction	(int gRdB) {
	theQueue	->  add (GRDB_REQUEST, gRdB);
}

void	sdrplayHandler_v3::set_lnagainReduction (int lnaState) {
	theQueue	-> add (LNA_REQUEST, lnaState);
	lnaGRdBDisplay	-> display (get_lnaGRdB (hwVersion, lnaState));
}

void	sdrplayHandler_v3::set_agcControl (int dummy) {
bool agcMode	= agcControl -> isChecked();

	(void)dummy;
	if (agcMode) 
	   theQueue	-> add (AGC_REQUEST, true, GRdBSelector -> value ());
	else
	   theQueue	-> add (AGC_REQUEST, false);

	if (!agcMode) {
	   GRdBSelector		-> show();
	   gainsliderLabel	-> show();	// old name actually
	}
	else {
	   GRdBSelector		-> hide();
	   gainsliderLabel	-> hide();
	}
}

void	sdrplayHandler_v3::set_ppmControl (int ppm) {
	theQueue	-> add (PPM_REQUEST, ppm);
}

void	sdrplayHandler_v3::set_antennaSelect	(const QString &s) {

        if (hwVersion != 2)     // should not happen
           return;

        if (s == "Antenna A")
	   theQueue	-> add (ANTENNASELECT_REQUEST, 'A');
        else
	   theQueue	-> add (ANTENNASELECT_REQUEST, 'B');
}

bool	sdrplayHandler_v3::restartReader	(int32_t freq) {
	if (running. load ())
	   return true;

	theQueue	-> add (RESTART_REQUEST, freq);
	vfoFrequency	= freq;
	running. store (true);
	return true;
}

void	sdrplayHandler_v3::stopReader	() {

	if (!running. load ())
	   return;
	running. store (false);
	theQueue	-> add (STOP_REQUEST);
}

static
void    StreamACallback (short *xi, short *xq,
                         sdrplay_api_StreamCbParamsT *params,
                         unsigned int numSamples, unsigned int reset,
                         void *cbContext) {
sdrplayHandler_v3	*p	= static_cast<sdrplayHandler_v3 *> (cbContext);
float	denominator	= (float)(p -> denominator);
std::complex<float> localBuf [numSamples];

	(void)params;
	if (reset)
	   return;
	if (!p -> receiverRuns)
	   return;
	for (int i = 0; i <  (int)numSamples; i ++)
	   localBuf [i] = std::complex<float> (float (xi [i]) / denominator,
	                                       float (xq [i]) / denominator);
	int n = (int)(p -> _I_Buffer -> GetRingBufferWriteAvailable ());
	if (n >= (int)numSamples) 
	   p -> _I_Buffer -> putDataIntoBuffer (localBuf, numSamples);
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
sdrplayHandler_v3	*p	= static_cast<sdrplayHandler_v3 *> (cbContext);
	switch (eventId) {
	   case sdrplay_api_GainChange:
	      p -> setTotalGain (params -> gainParams. currGain);
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

void	sdrplayHandler_v3::run	() {
sdrplay_api_ErrT        err;
sdrplay_api_DeviceT     devs [6];
float			apiVersion;
uint32_t                ndev;

        deviceParams            = nullptr;
        chosenDevice            = nullptr;
	denominator		= 2048;		// default
	nrBits			= 12;		// default

//	try to open the API
	err	= sdrplay_api_Open ();
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "sdrplay_api_Open failed %s\n",
	                          sdrplay_api_GetErrorString (err));
#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
	   throw (24);
	}

//	Check API versions match
        err = sdrplay_api_ApiVersion (&apiVersion);
        if (err  != sdrplay_api_Success) {
           fprintf (stderr, "sdrplay_api_ApiVersion failed %s\n",
                                     sdrplay_api_GetErrorString (err));
	   goto closeAPI;
        }

        if (apiVersion != SDRPLAY_API_VERSION) {
           fprintf (stderr, "API version don't match (local=%.2f dll=%.2f)\n",
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

	fprintf (stderr, "unlocked passed\n");
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
	deviceParams	-> devParams -> fsFreq. fsHz	= 2048000.0;
	chParams	-> tunerParams. bwType = sdrplay_api_BW_1_536;
	chParams	-> tunerParams. ifType = sdrplay_api_IF_Zero;
	chParams	-> tunerParams. rfFreq. rfHz    = 220000000.0;
	chParams	-> tunerParams. gain.gRdB	= 40;
	chParams	-> tunerParams. gain.LNAstate	= 3;
	chParams	-> ctrlParams.agc.enable = sdrplay_api_AGC_DISABLE;
//
//	assign callback functions
	cbFns. StreamACbFn	= StreamACallback;
	cbFns. StreamBCbFn	= StreamBCallback;
	cbFns. EventCbFn	= EventCallback;

	err	= sdrplay_api_Init (chosenDevice -> dev, &cbFns, this);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "sdrplay_api_Init failed %s\n",
                                       sdrplay_api_GetErrorString (err));
	}
	setDeviceData (devs [0]. SerNo, devs [0]. hwVer, apiVersion);

	switch (hwVersion) {
	   case 1:		// old RSP
	      denominator	= 2048;
	      nrBits		= 12;
	      break;

	   case 2:		// RSP II
	      denominator	= 2048;
	      nrBits		= 12;
              break;

	   case 3:		// RSP-DUO
	      denominator	= 8192;
	      nrBits		= 12;

	   default:
	   case 255:		// RSP-1A
	      denominator	= 8192;
	      nrBits		= 14;
              break;
	}

//	Now run the loop "listening" to commands
	threadRuns. store (true);
	receiverRuns	= false;
	while (threadRuns. load ()) {
	   while ((theQueue ->  size () < 1) && threadRuns. load ())
	      msleep (20);
	   if (!threadRuns. load ())
	      break;

	   int C = theQueue -> getHead ();
	   fprintf (stderr, "command = %o\n", C);
	   switch (C) {
	      case RESTART_REQUEST: {
	         int newFrequency	= theQueue -> getHead ();
	         chParams -> tunerParams. rfFreq. rfHz = (float)newFrequency;
	         fprintf (stderr, "setting frequency to %d\n", newFrequency);

	         err = sdrplay_api_Update (chosenDevice -> dev,
	                                   chosenDevice -> tuner,
	                                   sdrplay_api_Update_Tuner_Frf,
	                                   sdrplay_api_Update_Ext1_None);
	         if (err != sdrplay_api_Success) {
	            fprintf (stderr, "restart: error %s\n",
	                              sdrplay_api_GetErrorString (err));
	            emit error (FREQUENCY_UPDATE_ERROR);
	         }
	         receiverRuns	= true;
	         break;
	      }

	      case STOP_REQUEST:
	         receiverRuns	= false;
	         _I_Buffer	-> FlushRingBuffer ();
	         break;

	      case GRDB_REQUEST: {
	         int gRdB	= theQueue -> getHead ();
	         chParams -> tunerParams. gain. gRdB = gRdB;

                 err = sdrplay_api_Update (chosenDevice -> dev,
                                           chosenDevice -> tuner,
                                           sdrplay_api_Update_Tuner_Gr,
                                           sdrplay_api_Update_Ext1_None);
	         if (err != sdrplay_api_Success) {
	            fprintf (stderr, "grdb: error %s\n",
	                              sdrplay_api_GetErrorString (err));
	            emit error (GRDB_UPDATE_ERROR);
	         }
	         break;
	      }

	      case LNA_REQUEST: {
	         int lnaState = theQueue -> getHead ();	
	         chParams -> tunerParams. gain. LNAstate = lnaState;

                 err = sdrplay_api_Update (chosenDevice -> dev,
                                           chosenDevice -> tuner,
                                           sdrplay_api_Update_Tuner_Gr,
                                           sdrplay_api_Update_Ext1_None);
	         if (err != sdrplay_api_Success) {
	            fprintf (stderr, "grdb: error %s\n",
	                              sdrplay_api_GetErrorString (err));
	            emit error (LNA_UPDATE_ERROR);
	         }
	         break;
	      }

	      case PPM_REQUEST: {
	         int ppm	= theQueue -> getHead ();
	         deviceParams    -> devParams -> ppm = ppm;

                 err = sdrplay_api_Update (chosenDevice -> dev,
                                           chosenDevice -> tuner,
                                           sdrplay_api_Update_Dev_Ppm,
                                           sdrplay_api_Update_Ext1_None);
	         if (err != sdrplay_api_Success) {
	            fprintf (stderr, "lna: error %s\n",
	                              sdrplay_api_GetErrorString (err));
	            emit error (PPM_UPDATE_ERROR);
	         }
	         break;
	      }

	      case AGC_REQUEST: {
	         bool agcMode;
	         agcMode	= theQueue -> getHead ();
	         if (agcMode) {
	            int setPoint = theQueue -> getHead ();
	            chParams	-> ctrlParams. agc. setPoint_dBfs =  - setPoint;
	            chParams	-> ctrlParams. agc. enable =
                                             sdrplay_api_AGC_100HZ;
	         }
	         else {
	            chParams	-> ctrlParams. agc. enable =
	                                     sdrplay_api_AGC_DISABLE;
	         }

	         err = sdrplay_api_Update (chosenDevice -> dev,
	                                   chosenDevice -> tuner,
	                                   sdrplay_api_Update_Ctrl_Agc,
	                                   sdrplay_api_Update_Ext1_None);
	         if (err != sdrplay_api_Success) {
	            fprintf (stderr, "agc: error %s\n",
	                              sdrplay_api_GetErrorString (err));
	            emit error (AGC_UPDATE_ERROR);
	         }
	         break;
	      }

	      case FREQUENCY_REQUEST: {	
	         int f = theQueue	-> getHead ();
	      }

	      case ANTENNASELECT_REQUEST: {
	         int s = theQueue	->  getHead ();
	         break;
	         deviceParams    -> rxChannelA -> rsp2TunerParams. antennaSel =
	                            s == 'A' ?
	                                     sdrplay_api_Rsp2_ANTENNA_A:
	                                     sdrplay_api_Rsp2_ANTENNA_B;
	         err = sdrplay_api_Update (chosenDevice -> dev,
	                                   chosenDevice -> tuner,
	                                   sdrplay_api_Update_Rsp2_AntennaControl,
	                                   sdrplay_api_Update_Ext1_None);
	         if (err != sdrplay_api_Success)
	            emit error (ANTENNA_UPDATE_ERROR);
	         break;
	      }

	      default:	// should not happen
	         break;
	   }
	}

	sdrplay_api_Uninit		(chosenDevice -> dev);
	sdrplay_api_ReleaseDevice       (chosenDevice);
        sdrplay_api_Close               ();
	msleep (200);
	return;

unlockDevice_closeAPI:
	sdrplay_api_UnlockDeviceApi	();
closeAPI:
	sdrplay_api_ReleaseDevice       (chosenDevice);
        sdrplay_api_Close               ();

#ifdef __MINGW32__
        FreeLibrary (Handle);
#else
	 dlclose (Handle);
#endif
}
//	The brave old getSamples. For the sdrplay, we get
//	size still in I/Q pairs
int32_t	sdrplayHandler_v3::getSamples (std::complex<float> *V, int32_t size) { 
	return _I_Buffer	-> getDataFromBuffer (V, size);
}

int32_t	sdrplayHandler_v3::Samples	() {
	return _I_Buffer	-> GetRingBufferReadAvailable();
}

void	sdrplayHandler_v3::resetBuffer	() {
	_I_Buffer	-> FlushRingBuffer();
}

int16_t	sdrplayHandler_v3::bitDepth	() {
//	return nrBits;
	return 14;
}

void	sdrplayHandler_v3::setTotalGain (int g) {
	emit showTotalGain (g);
}

void	sdrplayHandler_v3::displayGain (int g) {
	totalGainDisplay	-> display (g);
}
	
void	sdrplayHandler_v3::showDeviceData (const QString &s,
	                                   int hwVersion, float apiVersion) {
	serialNumber	-> setText (s);
	this		-> hwVersion	= hwVersion;
	
	api_version	-> display (apiVersion);
	fprintf (stderr, "hwVer = %d\n", hwVersion);
//
//	we know we are only in the frequency range 175 .. 230 Mhz,
//	so we can rely on a single table for the lna reductions.
	switch (hwVersion) {
	   case 1:		// old RSP
	      lnaGainSetting	-> setRange (0, 3);
	      deviceLabel	-> setText ("RSP-I");
	      break;
	   case 2:		// RSP II
	      lnaGainSetting	-> setRange (0, 8);
	      deviceLabel	-> setText ("RSP-II");
	      antennaSelector -> show();
	      break;
	   case 3:		// RSP-DUO
	      lnaGainSetting	-> setRange (0, 9);
	      deviceLabel	-> setText ("RSP-DUO");
	      tunerSelector	-> show();
	      break;
	   default:
	   case 255:		// RSP-1A
	      lnaGainSetting	-> setRange (0, 9);
	      deviceLabel	-> setText ("RSP-1A");
	      break;
	}
	
	lnaGRdBDisplay	-> display (get_lnaGRdB (hwVersion,
	                                         lnaGainSetting -> value ()));
}

void	sdrplayHandler_v3::show_error	(int e) {
	fprintf (stderr, "error %o detected\n", e);
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
//	Ok, make explicit it is in the 32 bits section
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
	                 GETPROCADDRESS (Handle, "sdrplay_api_SelectDevice");
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

//	sdrplay_api_GetLastError	= (sdrplay_api_GetLastError_t)
//	                 GETPROCADDRESS (Handle, "sdrplay_api_GetLastError");
//	if (sdrplay_api_GetLastError == nullptr) {
//	   fprintf (stderr, "Could not find sdrplay_api_GetLastErrorg\n");
//	   return false;
//	}
//
//	sdrplay_api_DebugEnable		= (sdrplay_api_DebugEnable_t)
//	                 GETPROCADDRESS (Handle, "sdrplay_api_DebugEnable");
//	if (sdrplay_api_DebugEnable == nullptr) {
//	   fprintf (stderr, "Could not find sdrplay_api_DebugEnable\n");
//	   return false;
//	}
//
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
