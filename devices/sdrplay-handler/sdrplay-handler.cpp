#
/*
 *    Copyright (C) 2014 .. 2017
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
#include	"sdrplay-handler.h"
#include	"sdrplayselect.h"

static
int     RSP1_Table [] = {0, 24, 19, 43};

static
int     RSP1A_Table [] = {0, 6, 12, 18, 20, 26, 32, 38, 57, 62};

static
int     RSP2_Table [] = {0, 10, 15, 21, 24, 34, 39, 45, 64};

static
int	RSPduo_Table [] = {0, 6, 12, 18, 20, 26, 32, 38, 57, 62};

static
int	get_lnaGRdB (int hwVersion, int lnaState) {
	switch (hwVersion) {
	   case 1:
	      return RSP1_Table [lnaState];

	   case 2:
	      return RSP2_Table [lnaState];

	   default:
	      return RSP1A_Table [lnaState];
	}
}
//
//	here we start
	sdrplayHandler::sdrplayHandler  (QSettings *s) {
mir_sdr_ErrT	err;
float	ver;
mir_sdr_DeviceT devDesc [4];
mir_sdr_GainValuesT gainDesc;
sdrplaySelect	*sdrplaySelector;

	sdrplaySettings			= s;
	this	-> myFrame		= new QFrame (nullptr);
	setupUi (this -> myFrame);
	this	-> myFrame	-> show();
	antennaSelector		-> hide();
	tunerSelector		-> hide();
	this	-> inputRate		= Khz (2048);
	_I_Buffer			= nullptr;
	libraryLoaded			= false;

#ifdef	__MINGW32__
HKEY APIkey;
wchar_t APIkeyValue [256];
ULONG APIkeyValue_length = 255;


	wchar_t *libname = (wchar_t *)L"mir_sdr_api.dll";
	Handle	= LoadLibrary (libname);
	if (Handle == nullptr) {
	   if (RegOpenKey (HKEY_LOCAL_MACHINE,
	                   TEXT("Software\\MiricsSDR\\API"),
	                   &APIkey) != ERROR_SUCCESS) {
              fprintf (stderr,
	           "failed to locate API registry entry, error = %d\n",
	           (int)GetLastError());
	      delete myFrame;
	      throw (21);
	   }

	   RegQueryValueEx (APIkey,
	                    (wchar_t *)L"Install_Dir",
	                    nullptr,
	                    nullptr,
	                    (LPBYTE)&APIkeyValue,
	                    (LPDWORD)&APIkeyValue_length);
//	Ok, make explicit it is in the 32 bits section
	   wchar_t *x =
	        wcscat (APIkeyValue, (wchar_t *)L"\\x86\\mir_sdr_api.dll");
	   RegCloseKey(APIkey);

	   Handle	= LoadLibrary (x);
	   if (Handle == nullptr) {
	      fprintf (stderr, "Failed to open mir_sdr_api.dll\n");
	      delete myFrame;
	      throw (22);
	   }
	}
#else
	Handle		= dlopen ("libusb-1.0.so", RTLD_NOW | RTLD_GLOBAL);
	Handle		= dlopen ("libmirsdrapi-rsp.so", RTLD_NOW);
	if (Handle == nullptr)
	   Handle	= dlopen ("libmir_sdr.so", RTLD_NOW);

	if (Handle == nullptr) {
	   fprintf (stderr, "error report %s\n", dlerror());
	   delete myFrame;
	   throw (23);
	}
#endif
	libraryLoaded	= true;

	bool success = loadFunctions();
	if (!success) {
#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
           dlclose (Handle);
#endif
	   delete myFrame;
	   throw (23);
	}

	err		= my_mir_sdr_ApiVersion (&ver);
	if (err != mir_sdr_Success) {
	   fprintf (stderr, "error at ApiVersion %s\n",
	                 errorCodes (err). toLatin1(). data());
#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
           dlclose (Handle);
#endif
	   delete myFrame;
	   throw (24);
	}
	
	if (ver < 2.13) {
	   fprintf (stderr, "sorry, library too old\n");
#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
           dlclose (Handle);
#endif
	   delete myFrame;
	   throw (24);
	}

	api_version	-> display (ver);
	_I_Buffer	= new RingBuffer<std::complex<float>>(1024 * 1024);
	vfoFrequency	= Khz (220000);		// default

//	See if there are settings from previous incarnations
//	and config stuff

	sdrplaySettings		-> beginGroup ("sdrplaySettings");
	coarseOffset	=
	            sdrplaySettings -> value ("sdrplayOffset", 0). toInt();
	GRdBSelector 		-> setValue (
	            sdrplaySettings -> value ("sdrplay-ifgrdb", 20). toInt());
	lnaGainSetting		-> setValue (
	            sdrplaySettings -> value ("sdrplay-lnastate", 0). toInt());

	ppmControl		-> setValue (
	            sdrplaySettings -> value ("sdrplay-ppm", 0). toInt());
	bool	debugFlag	=
	            sdrplaySettings -> value ("sdrplay-debug", 0). toInt();
	if (!debugFlag)
	   debugControl -> hide();
	bool agcMode		=
	       sdrplaySettings -> value ("sdrplay-agcMode", 0). toInt() != 0;
	if (agcMode) {
	   agcControl -> setChecked (true);
	   GRdBSelector         -> hide();
	   gainsliderLabel      -> hide();
	}
	sdrplaySettings	-> endGroup();

	err = my_mir_sdr_GetDevices (devDesc, &numofDevs, uint32_t (4));
	if (err != mir_sdr_Success) {
	   fprintf (stderr, "error at GetDevices %s \n",
	                   errorCodes (err). toLatin1(). data());

#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
           dlclose (Handle);
#endif
	   delete myFrame;
	   throw (25);
	}

	if (numofDevs == 0) {
	   fprintf (stderr, "Sorry, no device found\n");
#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
           dlclose (Handle);
#endif
	   delete myFrame;
	   throw (25);
	}

	if (numofDevs > 1) {
           sdrplaySelector       = new sdrplaySelect();
           for (deviceIndex = 0; deviceIndex < numofDevs; deviceIndex ++) {
#ifndef	__MINGW32__
              sdrplaySelector ->
                   addtoList (devDesc [deviceIndex]. DevNm);
#else
              sdrplaySelector ->
                   addtoList (devDesc [deviceIndex]. SerNo);
#endif
           }
           deviceIndex = sdrplaySelector -> QDialog::exec();
           delete sdrplaySelector;
        }
	else
	   deviceIndex = 0;

	serialNumber -> setText (devDesc [deviceIndex]. SerNo);
	hwVersion = devDesc [deviceIndex]. hwVer;
	fprintf (stderr, "hwVer = %d\n", hwVersion);
	fprintf (stderr, "devicename = %s\n", devDesc [deviceIndex]. DevNm);

	err = my_mir_sdr_SetDeviceIdx (deviceIndex);
	if (err != mir_sdr_Success) {
	   fprintf (stderr, "error at SetDeviceIdx %s \n",
	                   errorCodes (err). toLatin1(). data());
	   my_mir_sdr_ReleaseDeviceIdx (deviceIndex);

#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
           dlclose (Handle);
#endif
	   delete myFrame;
	   throw (25);
	}
//
//	we know we are only in the frequency range 175 .. 230 Mhz,
//	so we can rely on a single table for the lna reductions.
	switch (hwVersion) {
	   case 1:		// old RSP
	      lnaGainSetting	-> setRange (0, 3);
	      deviceLabel	-> setText ("RSP-I");
	      nrBits		= 12;
	      denominator	= 2048;
	      break;
	   case 2:
	      lnaGainSetting	-> setRange (0, 8);
	      deviceLabel	-> setText ("RSP-II");
	      nrBits		= 12;
	      denominator	= 2048;
	      antennaSelector -> show();
	      err = my_mir_sdr_RSPII_AntennaControl (mir_sdr_RSPII_ANTENNA_A);
	      if (err != mir_sdr_Success) 
	         fprintf (stderr, "error %d in setting antenna\n", err);
	      connect (antennaSelector, SIGNAL (activated (const QString &)),
	            this, SLOT (set_antennaSelect (const QString &)));
	      break;
	   case 3:	
	      lnaGainSetting	-> setRange (0, 9);
	      deviceLabel	-> setText ("RSP-DUO");
	      nrBits		= 14;
	      denominator	= 8192;
	      tunerSelector	-> show();
	      err	= my_mir_sdr_rspDuo_TunerSel (mir_sdr_rspDuo_Tuner_1);
	      if (err != mir_sdr_Success) 
	         fprintf (stderr, "error %d in setting of rspDuo\n", err);
	      connect (tunerSelector, SIGNAL (activated (const QString &)),
	               this, SLOT (set_tunerSelect (const QString &)));
	      break;
	   default:
	      lnaGainSetting	-> setRange (0, 9);
	      deviceLabel	-> setText ("RSP-1A");
	      nrBits		= 14;
	      denominator	= 8192;
	      break;
	}

//	and be prepared for future changes in the settings
	connect (GRdBSelector, SIGNAL (valueChanged (int)),
	         this, SLOT (set_ifgainReduction (int)));
	connect (lnaGainSetting, SIGNAL (valueChanged (int)),
	         this, SLOT (set_lnagainReduction (int)));
	connect (agcControl, SIGNAL (stateChanged (int)),
	         this, SLOT (set_agcControl (int)));
	connect (debugControl, SIGNAL (stateChanged (int)),
	         this, SLOT (set_debugControl (int)));
	connect (ppmControl, SIGNAL (valueChanged (int)),
	         this, SLOT (set_ppmControl (int)));

	lnaGRdBDisplay		-> display (get_lnaGRdB (hwVersion,
	                                         lnaGainSetting -> value()));
	running. store (false);
}

	sdrplayHandler::~sdrplayHandler() {
	if (!libraryLoaded)
	   return;
	stopReader();
	sdrplaySettings	-> beginGroup ("sdrplaySettings");
	sdrplaySettings	-> setValue ("sdrplayOffset", coarseOffset);
	sdrplaySettings -> setValue ("sdrplay-ppm", ppmControl -> value());
	sdrplaySettings -> setValue ("sdrplay-ifgrdb",
	                                    GRdBSelector -> value());
	sdrplaySettings -> setValue ("sdrplay-lnastate",
	                                    lnaGainSetting -> value());
	sdrplaySettings	-> setValue ("sdrplay-agcMode",
	                                  agcControl -> isChecked() ? 1 : 0);
	sdrplaySettings	-> endGroup();
	sdrplaySettings	-> sync();
	delete	myFrame;

	if (numofDevs > 0)
	   my_mir_sdr_ReleaseDeviceIdx (deviceIndex);
	if (_I_Buffer != nullptr)
	   delete _I_Buffer;
#ifdef __MINGW32__
        FreeLibrary (Handle);
#else
        dlclose (Handle);
#endif
}
//
static inline
int16_t	bankFor_sdr (int32_t freq) {
	if (freq < 12 * MHz (1))
	   return mir_sdr_BAND_AM_LO;
	if (freq < 30 * MHz (1))
	   return mir_sdr_BAND_AM_MID;
	if (freq < 60 * MHz (1))
	   return mir_sdr_BAND_AM_HI;
	if (freq < 120 * MHz (1))
	   return mir_sdr_BAND_VHF;
	if (freq < 250 * MHz (1))
	   return mir_sdr_BAND_3;
	if (freq < 420 * MHz (1))
	   return mir_sdr_BAND_X;
	if (freq < 1000 * MHz (1))
	   return mir_sdr_BAND_4_5;
	if (freq < 2000 * MHz (1))
	   return mir_sdr_BAND_L;
	return -1;
}

int32_t	sdrplayHandler::defaultFrequency() {
	return Mhz (220);
}

void	sdrplayHandler::setVFOFrequency		(int32_t newFrequency) {
int	gRdBSystem;
int	samplesPerPacket;
mir_sdr_ErrT	err;
int	localGred	= GRdBSelector	-> value();
int	lnaState	= lnaGainSetting -> value();

	if (bankFor_sdr (newFrequency) == -1)
	   return;

	if (!running. load()) {
	   vfoFrequency = newFrequency;
	   return;
	}

	if (bankFor_sdr (newFrequency) == bankFor_sdr (vfoFrequency)) 
	   err = my_mir_sdr_SetRf (double (newFrequency), 1, 0);
	else
	   err = my_mir_sdr_Reinit (&localGred,
	                             double (inputRate) / Mhz (1),
	                             double (newFrequency) / Mhz (1),
	                             mir_sdr_BW_1_536,
	                             mir_sdr_IF_Zero,
	                             mir_sdr_LO_Undefined,	// LOMode
	                             lnaState,	// LNA enable
	                             &gRdBSystem,
	                             mir_sdr_USE_RSP_SET_GR,
	                             &samplesPerPacket,
	                             mir_sdr_CHANGE_RF_FREQ);
	if (err != mir_sdr_Success) 
	   fprintf (stderr, "Error at setVFO %s\n",
	                    errorCodes (err). toLatin1(). data());
	else
	   vfoFrequency = newFrequency;
}

int32_t	sdrplayHandler::getVFOFrequency() {
	return vfoFrequency;
}

void	sdrplayHandler::set_ifgainReduction	(int newGain) {
mir_sdr_ErrT	err;
int	GRdB		= GRdBSelector	-> value();
int	lnaState	= lnaGainSetting -> value();

	(void)newGain;

	err	=  my_mir_sdr_RSP_SetGr (GRdB, lnaState, 1, 0);
	if (err != mir_sdr_Success) 
	   fprintf (stderr, "Error at set_ifgain %s\n",
	                    errorCodes (err). toLatin1(). data());
	else {
	   lnaGRdBDisplay	-> display (get_lnaGRdB (hwVersion, lnaState));
	}
}

void	sdrplayHandler::set_lnagainReduction (int lnaState) {
mir_sdr_ErrT err;

	if (!agcControl -> isChecked()) {
	   set_ifgainReduction (0);
	   return;
	}

	err	= my_mir_sdr_AgcControl (mir_sdr_AGC_100HZ,
	                                 -30, 0, 0, 0, 0, lnaState);
	if (err != mir_sdr_Success) 
	   fprintf (stderr, "Error at set_lnagainReduction %s\n",
	                       errorCodes (err). toLatin1(). data());
	else
	   lnaGRdBDisplay	-> display (get_lnaGRdB (hwVersion, lnaState));
}

//
static
void myStreamCallback (int16_t		*xi,
	               int16_t		*xq,
	               uint32_t		firstSampleNum, 
	               int32_t		grChanged,
	               int32_t		rfChanged,
	               int32_t		fsChanged,
	               uint32_t		numSamples,
	               uint32_t		reset,
	               uint32_t		hwRemoved,
	               void		*cbContext) {
int16_t	i;
sdrplayHandler	*p	= static_cast<sdrplayHandler *> (cbContext);
float	denominator	= (float)(p -> denominator);
std::complex<float> localBuf [numSamples];

	if (reset || hwRemoved)
	   return;
	for (i = 0; i <  (int)numSamples; i ++)
	   localBuf [i] = std::complex<float> (float (xi [i]) / denominator,
	                                       float (xq [i]) / denominator);
	p -> _I_Buffer -> putDataIntoBuffer (localBuf, numSamples);
	(void)	firstSampleNum;
	(void)	grChanged;
	(void)	rfChanged;
	(void)	fsChanged;
}

void	myGainChangeCallback (uint32_t	GRdB,
	                      uint32_t	lnaGRdB,
	                      void	*cbContext) {
sdrplayHandler	*p	= static_cast<sdrplayHandler *> (cbContext);
	(void)lnaGRdB;
//	p -> lnaGRdBDisplay	-> display ((int)lnaGRdB);
}

bool	sdrplayHandler::restartReader() {
int	gRdBSystem;
int	samplesPerPacket;
mir_sdr_ErrT	err;
int	GRdB		= GRdBSelector	-> value();
int	lnaState	= lnaGainSetting -> value();

	if (running. load())
	   return true;

	err	= my_mir_sdr_StreamInit (&GRdB,
	                                 double (inputRate) / MHz (1),
	                                 double (vfoFrequency) / Mhz (1),
	                                 mir_sdr_BW_1_536,
	                                 mir_sdr_IF_Zero,
	                                 lnaState,
	                                 &gRdBSystem,
	                                 mir_sdr_USE_RSP_SET_GR,
	                                 &samplesPerPacket,
	                                 (mir_sdr_StreamCallback_t)myStreamCallback,
	                                 (mir_sdr_GainChangeCallback_t)myGainChangeCallback,
	                                 this);
	fprintf (stderr, "overall gain reduction %d\n", gRdBSystem);
	if (err != mir_sdr_Success) {
	   fprintf (stderr, "error = %s\n",
	                errorCodes (err). toLatin1(). data());
	   return false;
	}
	err	= my_mir_sdr_SetPpm (double (ppmControl -> value()));
	if (err != mir_sdr_Success) 
	   fprintf (stderr, "error = %s\n",
	                errorCodes (err). toLatin1(). data());

	if (agcControl -> isChecked()) {
	   my_mir_sdr_AgcControl (mir_sdr_AGC_100HZ,
	                          -30,
	                          0, 0, 0, 0, lnaGainSetting -> value());
	   GRdBSelector		-> hide();
	}

	err		= my_mir_sdr_SetDcMode (4, 1);
	if (err != mir_sdr_Success)
	   fprintf (stderr, "error = %s\n",
	                errorCodes (err). toLatin1(). data());
	err		= my_mir_sdr_SetDcTrackTime (63);
	if (err != mir_sdr_Success)
	   fprintf (stderr, "error = %s\n",
	                errorCodes (err). toLatin1(). data());
	running. store (true);
	return true;
}

void	sdrplayHandler::stopReader() {
mir_sdr_ErrT err;

	if (!running. load())
	   return;

	err	= my_mir_sdr_StreamUninit();
	if (err != mir_sdr_Success)
	   fprintf (stderr, "error = %s\n",
	                errorCodes (err). toLatin1(). data());
	running. store (false);
}

//
//	The brave old getSamples. For the sdrplay, we get
//	size still in I/Q pairs
int32_t	sdrplayHandler::getSamples (std::complex<float> *V, int32_t size) { 
	return _I_Buffer	-> getDataFromBuffer (V, size);
}

int32_t	sdrplayHandler::Samples() {
	return _I_Buffer	-> GetRingBufferReadAvailable();
}

void	sdrplayHandler::resetBuffer() {
	_I_Buffer	-> FlushRingBuffer();
}

int16_t	sdrplayHandler::bitDepth() {
	return nrBits;
}

bool	sdrplayHandler::loadFunctions() {
	my_mir_sdr_StreamInit	= (pfn_mir_sdr_StreamInit)
	                    GETPROCADDRESS (this -> Handle,
	                                    "mir_sdr_StreamInit");
	if (my_mir_sdr_StreamInit == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_StreamInit\n");
	   return false;
	}

	my_mir_sdr_StreamUninit	= (pfn_mir_sdr_StreamUninit)
	                    GETPROCADDRESS (this -> Handle,
	                                    "mir_sdr_StreamUninit");
	if (my_mir_sdr_StreamUninit == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_StreamUninit\n");
	   return false;
	}

	my_mir_sdr_SetRf	= (pfn_mir_sdr_SetRf)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetRf");
	if (my_mir_sdr_SetRf == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_SetRf\n");
	   return false;
	}

	my_mir_sdr_SetFs	= (pfn_mir_sdr_SetFs)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetFs");
	if (my_mir_sdr_SetFs == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_SetFs\n");
	   return false;
	}

	my_mir_sdr_SetGr	= (pfn_mir_sdr_SetGr)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetGr");
	if (my_mir_sdr_SetGr == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_SetGr\n");
	   return false;
	}

	my_mir_sdr_RSP_SetGr	= (pfn_mir_sdr_RSP_SetGr)
	                    GETPROCADDRESS (Handle, "mir_sdr_RSP_SetGr");
	if (my_mir_sdr_RSP_SetGr == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_RSP_SetGr\n");
	   return false;
	}

	my_mir_sdr_SetGrParams	= (pfn_mir_sdr_SetGrParams)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetGrParams");
	if (my_mir_sdr_SetGrParams == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_SetGrParams\n");
	   return false;
	}

	my_mir_sdr_SetDcMode	= (pfn_mir_sdr_SetDcMode)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetDcMode");
	if (my_mir_sdr_SetDcMode == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_SetDcMode\n");
	   return false;
	}

	my_mir_sdr_SetDcTrackTime	= (pfn_mir_sdr_SetDcTrackTime)
	                    GETPROCADDRESS (Handle, "mir_sdr_SetDcTrackTime");
	if (my_mir_sdr_SetDcTrackTime == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_SetDcTrackTime\n");
	   return false;
	}

	my_mir_sdr_SetSyncUpdateSampleNum = (pfn_mir_sdr_SetSyncUpdateSampleNum)
	               GETPROCADDRESS (Handle, "mir_sdr_SetSyncUpdateSampleNum");
	if (my_mir_sdr_SetSyncUpdateSampleNum == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_SetSyncUpdateSampleNum\n");
	   return false;
	}

	my_mir_sdr_SetSyncUpdatePeriod	= (pfn_mir_sdr_SetSyncUpdatePeriod)
	                GETPROCADDRESS (Handle, "mir_sdr_SetSyncUpdatePeriod");
	if (my_mir_sdr_SetSyncUpdatePeriod == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_SetSyncUpdatePeriod\n");
	   return false;
	}

	my_mir_sdr_ApiVersion	= (pfn_mir_sdr_ApiVersion)
	                GETPROCADDRESS (Handle, "mir_sdr_ApiVersion");
	if (my_mir_sdr_ApiVersion == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_ApiVersion\n");
	   return false;
	}

	my_mir_sdr_AgcControl	= (pfn_mir_sdr_AgcControl)
	                GETPROCADDRESS (Handle, "mir_sdr_AgcControl");
	if (my_mir_sdr_AgcControl == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_AgcControl\n");
	   return false;
	}

	my_mir_sdr_Reinit	= (pfn_mir_sdr_Reinit)
	                GETPROCADDRESS (Handle, "mir_sdr_Reinit");
	if (my_mir_sdr_Reinit == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_Reinit\n");
	   return false;
	}

	my_mir_sdr_SetPpm	= (pfn_mir_sdr_SetPpm)
	                GETPROCADDRESS (Handle, "mir_sdr_SetPpm");
	if (my_mir_sdr_SetPpm == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_SetPpm\n");
	   return false;
	}

	my_mir_sdr_DebugEnable	= (pfn_mir_sdr_DebugEnable)
	                GETPROCADDRESS (Handle, "mir_sdr_DebugEnable");
	if (my_mir_sdr_DebugEnable == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_DebugEnable\n");
	   return false;
	}

	my_mir_sdr_rspDuo_TunerSel = (pfn_mir_sdr_rspDuo_TunerSel)
	               GETPROCADDRESS (Handle, "mir_sdr_rspDuo_TunerSel");
	if (my_mir_sdr_rspDuo_TunerSel == nullptr) {
           fprintf (stderr, "Could not find mir_sdr_rspDuo_TunerSel\n");
           return false;
        }

	my_mir_sdr_DCoffsetIQimbalanceControl	=
	                     (pfn_mir_sdr_DCoffsetIQimbalanceControl)
	                GETPROCADDRESS (Handle, "mir_sdr_DCoffsetIQimbalanceControl");
	if (my_mir_sdr_DCoffsetIQimbalanceControl == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_DCoffsetIQimbalanceControl\n");
	   return false;
	}


	my_mir_sdr_ResetUpdateFlags	= (pfn_mir_sdr_ResetUpdateFlags)
	                GETPROCADDRESS (Handle, "mir_sdr_ResetUpdateFlags");
	if (my_mir_sdr_ResetUpdateFlags == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_ResetUpdateFlags\n");
	   return false;
	}

	my_mir_sdr_GetDevices		= (pfn_mir_sdr_GetDevices)
	                GETPROCADDRESS (Handle, "mir_sdr_GetDevices");
	if (my_mir_sdr_GetDevices == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_GetDevices");
	   return false;
	}

	my_mir_sdr_GetCurrentGain	= (pfn_mir_sdr_GetCurrentGain)
	                GETPROCADDRESS (Handle, "mir_sdr_GetCurrentGain");
	if (my_mir_sdr_GetCurrentGain == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_GetCurrentGain");
	   return false;
	}

	my_mir_sdr_GetHwVersion	= (pfn_mir_sdr_GetHwVersion)
	                GETPROCADDRESS (Handle, "mir_sdr_GetHwVersion");
	if (my_mir_sdr_GetHwVersion == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_GetHwVersion");
	   return false;
	}

	my_mir_sdr_RSPII_AntennaControl	= (pfn_mir_sdr_RSPII_AntennaControl)
	                GETPROCADDRESS (Handle, "mir_sdr_RSPII_AntennaControl");
	if (my_mir_sdr_RSPII_AntennaControl == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_RSPII_AntennaControl");
	   return false;
	}

	my_mir_sdr_SetDeviceIdx	= (pfn_mir_sdr_SetDeviceIdx)
	                GETPROCADDRESS (Handle, "mir_sdr_SetDeviceIdx");
	if (my_mir_sdr_SetDeviceIdx == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_SetDeviceIdx");
	   return false;
	}

	my_mir_sdr_ReleaseDeviceIdx	= (pfn_mir_sdr_ReleaseDeviceIdx)
	                GETPROCADDRESS (Handle, "mir_sdr_ReleaseDeviceIdx");
	if (my_mir_sdr_ReleaseDeviceIdx == nullptr) {
	   fprintf (stderr, "Could not find mir_sdr_ReleaseDeviceIdx");
	   return false;
	}

	return true;
}

void	sdrplayHandler::set_agcControl (int dummy) {
bool agcMode	= agcControl -> isChecked();
	my_mir_sdr_AgcControl (agcMode ? mir_sdr_AGC_100HZ :
	                                 mir_sdr_AGC_DISABLE,
	                       -30,
	                       0, 0, 0, 0, lnaGainSetting -> value());
	if (!agcMode) {
	   GRdBSelector		-> show();
	   gainsliderLabel	-> show();	// old name actually
	   set_ifgainReduction (0);
	}
	else {
	   GRdBSelector		-> hide();
	   gainsliderLabel	-> hide();
	}
}

void	sdrplayHandler::set_debugControl (int debugMode) {
	(void)debugMode;
	my_mir_sdr_DebugEnable (debugControl -> isChecked() ? 1 : 0);
}

void	sdrplayHandler::set_ppmControl (int ppm) {
	if (running. load()) {
	   my_mir_sdr_SetPpm	((float)ppm);
	   my_mir_sdr_SetRf	((float)vfoFrequency, 1, 0);
	}
}

void	sdrplayHandler::set_antennaSelect (const QString &s) {
mir_sdr_ErrT err;

	if (hwVersion != 2)	// should not happen
	   return;

	if (s == "Antenna A")
	   err = my_mir_sdr_RSPII_AntennaControl (mir_sdr_RSPII_ANTENNA_A);
	else
	   err = my_mir_sdr_RSPII_AntennaControl (mir_sdr_RSPII_ANTENNA_B);
}

void	sdrplayHandler::set_tunerSelect (const QString &s) {
mir_sdr_ErrT err;

	if (hwVersion != 3)	// should not happen
	   return;
	if (s == "Tuner 1") 
	   err	= my_mir_sdr_rspDuo_TunerSel (mir_sdr_rspDuo_Tuner_1);
	else
	   err	= my_mir_sdr_rspDuo_TunerSel (mir_sdr_rspDuo_Tuner_2);

	if (err != mir_sdr_Success) 
	   fprintf (stderr, "error %d in selecting  rspDuo\n", err);
}

QString	sdrplayHandler::errorCodes (mir_sdr_ErrT err) {
	switch (err) {
	   case mir_sdr_Success:
	      return "success";
	   case mir_sdr_Fail:
	      return "Fail";
	   case mir_sdr_InvalidParam:
	      return "invalidParam";
	   case mir_sdr_OutOfRange:
	      return "OutOfRange";
	   case mir_sdr_GainUpdateError:
	      return "GainUpdateError";
	   case mir_sdr_RfUpdateError:
	      return "RfUpdateError";
	   case mir_sdr_FsUpdateError:
	      return "FsUpdateError";
	   case mir_sdr_HwError:
	      return "HwError";
	   case mir_sdr_AliasingError:
	      return "AliasingError";
	   case mir_sdr_AlreadyInitialised:
	      return "AlreadyInitialised";
	   case mir_sdr_NotInitialised:
	      return "NotInitialised";
	   case mir_sdr_NotEnabled:
	      return "NotEnabled";
	   case mir_sdr_HwVerError:
	      return "HwVerError";
	   case mir_sdr_OutOfMemError:
	      return "OutOfMemError";
	   case mir_sdr_HwRemoved:
	      return "HwRemoved";
	   default:
	      return "???";
	}
}

