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
#include	"sdrplay-controller.h"
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
	sdrplayController::sdrplayController (sdrplayHandler_v3 *parent,
	                                      RingBuffer<std::complex<int16_t>> *b,
	                                      controlQueue *theQueue) {
	this	-> parent	= parent;
	this	-> _I_Buffer	= b;
	this	-> theQueue	= theQueue;

        connect (this, SIGNAL	(setTotalGain (int)),
                 parent, SLOT	(show_TotalGain (int)));
	connect (this, SIGNAL	(setDeviceData (const QString &, int, float)),
	         parent, SLOT   (show_DeviceData (const QString &, int, float)));
	connect (this, SIGNAL	(error (int)),
	         parent, SLOT   (show_Error (int)));
	connect (this, SIGNAL	(set_runFlag (bool)),
	         parent, SLOT   (show_runFlag (bool)));
	connect (this, SIGNAL	(set_lnaGain (int)),
	         parent, SLOT   (show_lnaGain (int)));
	connect (this, SIGNAL	(set_lnaRange	(int, int)),
	         parent, SLOT	(set_lnaRange	(int, int)));
	connect	(this, SIGNAL	(set_deviceLabel (const QString &, int)),
	         parent, SLOT	(show_deviceLabel (const QString &, int)));
	connect (this, SIGNAL	(set_antennaSelector (bool)),
	         parent, SLOT	(show_antennaSelector (bool)));
	connect (this, SIGNAL	(set_tunerSelector (bool)),
	         parent, SLOT	(show_tunerSelector (bool)));
	start ();
}

	sdrplayController::~sdrplayController () {
	threadRuns. store (false);
	while (this -> isRunning ())
	   msleep (10);
}
//
static
void    StreamACallback (short *xi, short *xq,
                         sdrplay_api_StreamCbParamsT *params,
                         unsigned int numSamples, unsigned int reset,
                         void *cbContext) {
sdrplayController *p	= static_cast<sdrplayController *> (cbContext);
float	denominator	= (float)(p -> denominator);
std::complex<float> localBuf [numSamples];

	(void)params;
	if (reset)
	   return;
	if (!p -> receiverRuns. load ())
	   return;

	for (int i = 0; i <  (int)numSamples; i ++)
	   localBuf [i] = std::complex<int16_t> (xi [i], xq [i]);
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
sdrplayController *p	= static_cast<sdrplayController *> (cbContext);
	(void)tuner;
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

void	sdrplayController::
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

void	sdrplayController::run	() {
sdrplay_api_ErrT        err;
sdrplay_api_DeviceT     devs [6];
float			apiVersion;
uint32_t                ndev;

	chosenDevice	= nullptr;
	deviceParams	= nullptr;

	denominator		= 2048;		// default
	nrBits			= 12;		// default
	threadRuns. store (false);

	Handle			= fetchLibrary ();
	if (Handle == nullptr)
	   throw (21);
//	load the functions
	bool success	= loadFunctions ();
	if (!success) {
	   releaseLibrary ();
	   emit set_runFlag (false);
           throw (23);
        }
	fprintf (stderr, "functions loaded\n");

//	try to open the API
	err	= sdrplay_api_Open ();
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "sdrplay_api_Open failed %s\n",
	                          sdrplay_api_GetErrorString (err));
	   releaseLibrary ();
	   emit set_runFlag (false);
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
	deviceParams	-> devParams -> fsFreq. fsHz	= 2048000.0;
	chParams	-> tunerParams. bwType = sdrplay_api_BW_1_536;
	chParams	-> tunerParams. ifType = sdrplay_api_IF_Zero;
//
//	these will change:
	chParams	-> tunerParams. rfFreq. rfHz    = 220000000.0;
	chParams	-> tunerParams. gain.gRdB	= 30;
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
	   goto unlockDevice_closeAPI;
	}
//
//	Let the parent display the values
	setDeviceData (devs [0]. SerNo, devs [0]. hwVer, apiVersion);

	hwVersion = devs [0]. hwVer;
	switch (hwVersion) {
	   case 1:		// old RSP
	      set_lnaRange	(0, 3);
	      set_deviceLabel ("RSP-I", 12);
	      denominator	= 2048;
	      nrBits		= 12;
	      break;
	   case 2:		// RSP II
	      set_lnaRange	(0, 8);
	      set_deviceLabel ("RSP-II", 12);
	      denominator	= 2048;
	      nrBits		= 14;
	      set_antennaSelector (true);
	      break;
	   case 3:		// RSP-DUO
	      set_lnaRange	(0, 9);
	      set_deviceLabel	("RSP-DUO", 12);
	      denominator	= 2048;
	      nrBits		= 12;
	      set_tunerSelector (true);
	      break;
	   default:
	   case 255:		// RSP-1A
	      set_lnaRange	 (0, 9);
	      set_deviceLabel	("RSP-1A", 14);
	      denominator	= 8192;
	      nrBits		= 14;
	      break;
	}

	threadRuns. store (true);	// it seems we can do some work
	set_runFlag (true);		// signal the parent that it seems OK
	receiverRuns. store (false);	// Initially, not buffering data

//	Now run the loop "listening" to commands
	while (threadRuns. load ()) {
	   while ((theQueue ->  size () < 1) && threadRuns. load ())
	      msleep (10);
	   if (!threadRuns. load ())
	      break;

	   int C = theQueue -> getHead ();
	   switch (C) {
	      case RESTART_REQUEST: {
	         int newFrequency	= theQueue -> getHead ();
	         chParams -> tunerParams. rfFreq. rfHz = (float)newFrequency;
	         usleep (10000);
	         err = sdrplay_api_Update (chosenDevice -> dev,
	                                   chosenDevice -> tuner,
	                                   sdrplay_api_Update_Tuner_Frf,
	                                   sdrplay_api_Update_Ext1_None);
	         if (err != sdrplay_api_Success) {
	            fprintf (stderr, "restart: error %s\n",
	                              sdrplay_api_GetErrorString (err));
	            emit error (FREQUENCY_UPDATE_ERROR);
	         }
	         _I_Buffer	-> FlushRingBuffer ();
	         receiverRuns. store (true);
	         break;
	      }

	      case STOP_REQUEST:
	         receiverRuns. store (false);
	         _I_Buffer	-> FlushRingBuffer ();
	         break;

	      case GRDB_REQUEST: {
	         int gRdB	= theQueue -> getHead ();
	         if (gRdB < 20)
	            gRdB = 20;
	         if (gRdB > 59)
	            gRdB = 59;
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
	         emit set_lnaGain (get_lnaGRdB (hwVersion, lnaState));
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
	         usleep (1000);
	         break;
	      }

	      case FREQUENCY_REQUEST: {	// does not happen
	         int f = theQueue	-> getHead ();
	      }

	      case ANTENNASELECT_REQUEST: {
	         int s = theQueue	->  getHead ();
	         if (hwVersion != 2)
	            return;
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
//
//	normal exit:
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
	sdrplay_api_ReleaseDevice       (chosenDevice);
        sdrplay_api_Close               ();
	releaseLibrary	();
	set_runFlag (false);
}

bool	sdrplayController::is_threadRunning () {
	return isRunning ();
}

bool	sdrplayController::is_receiverRunning	() {
	return receiverRuns. load ();
}

void	sdrplayController::releaseLibrary () {
#ifdef __MINGW32__
        FreeLibrary (Handle);
#else
	dlclose (Handle);
#endif
}

HINSTANCE	sdrplayController::fetchLibrary () {
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

bool	sdrplayController::loadFunctions () {
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
