#
/*
 *    Copyright (C) 2017 .. 2020
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

#include	<QObject>
#include	<QSettings>
#include	<QHBoxLayout>
#include	<QLabel>
#include	<QFileDialog>
#include	<complex>
#include	"sdrplay-handler.h"
#include	"sdrplayselect.h"
#include	"xml-filewriter.h"
#include	"dab-processor.h"
#include	"radio.h"

static
int     RSP1_Table [] = {0, 24, 19, 43};

static
int     RSP1A_Table [] = {0, 6, 12, 18, 20, 26, 32, 38, 57, 62};

static
int     RSP2_Table [] = {0, 10, 15, 21, 24, 34, 39, 45, 64};

//static
//int	RSPduo_Table [] = {0, 6, 12, 18, 20, 26, 32, 38, 57, 62};

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
	sdrplayHandler::sdrplayHandler  (RadioInterface *mr,
	                                 QSettings *s,
	                                 dabProcessor *base,
	                                 QString &recorderVersion):
	                                     myFrame (nullptr) {
mir_sdr_ErrT	err;
float	ver;
mir_sdr_DeviceT devDesc [4];
//mir_sdr_GainValuesT gainDesc;
sdrplaySelect	*sdrplaySelector;

	(void)mr;
	sdrplaySettings		= s;
	this	-> base		= base;
	this	-> recorderVersion	= recorderVersion;
	setupUi (&myFrame);
	myFrame. show	();
	antennaSelector		-> hide ();
	tunerSelector		-> hide ();
	this	-> inputRate	= 2048000;

	bool success            = fetchLibrary ();
        if (!success) {
           throw (23);
        }
        success = loadFunctions();
        if (!success) {
           releaseLibrary       ();
           throw (23);
        }

	err		= my_mir_sdr_ApiVersion (&ver);
	if (err != mir_sdr_Success) {
	   fprintf (stderr, "error at ApiVersion %s\n",
	                 errorCodes (err). toLatin1 (). data ());
	   throw (24);
	}
	
	if (ver < 2.13) {
	   fprintf (stderr, "sorry, library too old\n");
	   throw (24);
	}

	vfoFrequency	= Khz (220000);		// default
	totalOffset	= 0;

//	See if there are settings from previous incarnations
//	and config stuff

	sdrplaySettings		-> beginGroup ("sdrplaySettings");
	lnaGainSetting		-> setValue (
	            sdrplaySettings -> value ("sdrplay-lnastate", 3). toInt ());
	lnaState	= lnaGainSetting	-> value ();

	ppmControl		-> setValue (
	            sdrplaySettings -> value ("sdrplay-ppm", 0). toInt ());
	gain_setpoint		-> setValue (
	            sdrplaySettings -> value ("gain_setpoint", -35). toInt ());
	bool	debugFlag	=
	            sdrplaySettings -> value ("sdrplay-debug", 0). toInt ();
	if (!debugFlag)
	   debugControl -> hide ();
	bool	agcFlag		=
	            sdrplaySettings -> value ("agc", 1). toInt ();
	sdrplaySettings	-> endGroup ();

	err = my_mir_sdr_GetDevices (devDesc, &numofDevs, uint32_t (4));
	if (err != mir_sdr_Success) {
	   fprintf (stderr, "error at GetDevices %s \n",
	                   errorCodes (err). toLatin1 (). data ());

	   throw (25);
	}

	if (numofDevs == 0) {
	   fprintf (stderr, "Sorry, no device found\n");
	   throw (25);
	}

	if (numofDevs > 1) {
	   sdrplaySelector       = new sdrplaySelect ();
	   for (deviceIndex = 0; deviceIndex < numofDevs; deviceIndex ++) {
#ifndef	__MINGW32__
	      sdrplaySelector ->
	           addtoList (devDesc [deviceIndex]. DevNm);
#else
	      sdrplaySelector ->
	           addtoList (devDesc [deviceIndex]. SerNo);
#endif
	   }
	   deviceIndex = sdrplaySelector -> QDialog::exec ();
	   delete sdrplaySelector;
	}
	else
	   deviceIndex = 0;

	hwVersion = devDesc [deviceIndex]. hwVer;
	fprintf (stderr, "hwVer = %d\n", hwVersion);
	fprintf (stderr, "devicename = %s\n", devDesc [deviceIndex]. DevNm);

	err = my_mir_sdr_SetDeviceIdx (deviceIndex);
	if (err != mir_sdr_Success) {
	   fprintf (stderr, "error at SetDeviceIdx %s \n",
	                   errorCodes (err). toLatin1 (). data ());
	   throw (25);
	}

	serialNumber	-> setText (devDesc [deviceIndex]. SerNo);
	api_version	-> display (ver);
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
	      antennaSelector	-> show ();
	      selectedAntenna = 'a';
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
	      tunerSelector	-> show ();
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

	if (agcFlag)
	   agcControl	-> setChecked (true);
//	and be prepared for future changes in the settings
	connect (lnaGainSetting, SIGNAL (valueChanged (int)),
	         this, SLOT (set_lnagainReduction (int)));
	connect (debugControl, SIGNAL (stateChanged (int)),
	         this, SLOT (set_debugControl (int)));
	connect (ppmControl, SIGNAL (valueChanged (int)),
	         this, SLOT (set_ppmControl (int)));
	connect (agcControl, SIGNAL (stateChanged (int)),
	         this, SLOT (set_agcControl (int)));
	connect (xml_dumpButton, SIGNAL (clicked ()),
	         this, SLOT (set_xmlDump ()));

	lnaValueDisplay		-> display (get_lnaGRdB (hwVersion,
	                                         lnaGainSetting -> value ()));
	running. store (false);
	xmlDumping. store	(false);
	xmlDumper	= nullptr;
}

	sdrplayHandler::~sdrplayHandler	(void) {
	stopReader ();
	sdrplaySettings	-> beginGroup ("sdrplaySettings");
	sdrplaySettings -> setValue ("sdrplay-ppm", ppmControl -> value ());
	sdrplaySettings -> setValue ("gain_setpoint",
	                                    gain_setpoint -> value ());
	sdrplaySettings -> setValue ("sdrplay-lnastate",
	                                    lnaGainSetting -> value ());
	sdrplaySettings -> setValue ("agc",
	                              agcControl -> isChecked () ? 1 : 0);
	sdrplaySettings	-> endGroup ();
	sdrplaySettings	-> sync ();

	if (numofDevs > 0)
	   my_mir_sdr_ReleaseDeviceIdx (1);
}

void	sdrplayHandler::setOffset		(int32_t offset) {
int	newFrequency	= vfoFrequency + offset;
mir_sdr_ErrT err;

	if (offset != 0) {
	   totalOffset	+= offset;
	   err = my_mir_sdr_SetRf (double (newFrequency), 1, 0);
	   if (err != mir_sdr_Success)
	     fprintf (stderr, "error in update frequency with %d %s\n",
	                     offset, errorCodes (err). toLatin1 (). data ());
	   else
	      vfoFrequency	= newFrequency;
	}
	freq_offsetDisplay	-> display (totalOffset);
	freq_errorDisplay	-> display (offset);
}

static inline
int	constrain (int v, int l, int h) {
	if (v < l)
	   return l;
	if (v > h)
	   return h;
}

void	sdrplayHandler::setGains	(float lowVal, float highVal) {
mir_sdr_GainValuesT gains;
mir_sdr_ErrT err = my_mir_sdr_GetCurrentGain (&gains);

	if (err != mir_sdr_Success)
	   fprintf (stderr, "error getting gain values %s\n",
	                     errorCodes (err). toLatin1 (). data ());
	
	float str = 10 * log10 ((highVal + 0.005)  / denominator);
	float lvv = 10 * log10 ((lowVal  + 0.005)  / denominator);
//
//	we compute the "error" in the gain setting,
//	and we derive the GRdB value needed to correct that
	int gainCorr	= gain_setpoint -> value () - str;
	gainCorr 	= constrain (gainCorr, -20, 20);

	int GRdB	= gains. curr - get_lnaGRdB (hwVersion, lnaState);
	GRdB		= constrain (GRdB + gainCorr, 20, 59);

	if ((GRdB != 0) && (!agcControl -> isChecked ())) {
	   err = my_mir_sdr_RSP_SetGr (GRdB, lnaState, 1 , 0);
	   if (err != mir_sdr_Success)
	      fprintf (stderr, "error updating GainReduction: GRdb = %d, lnaState = %d, curr = %f %d (%s)\n",
	                       GRdB,
	                       lnaState,
	                       gains. curr,
	                       gainCorr,
	                       errorCodes (err). toLatin1 (). data ());
	}
	averageValue		-> display (str);
	nullValue		-> display (lvv);
	reportedGain		-> display (gains. curr);
}

void	sdrplayHandler::set_lnagainReduction (int lnaState) {
mir_sdr_ErrT err;

	this	-> lnaState	= lnaState;
	err			=
	                 my_mir_sdr_RSP_SetGr (30, lnaState, 1 , 0);
	if (err != mir_sdr_Success)
	   fprintf (stderr, "error in lna state (%d) %s\n",
	                              lnaState,
	                              errorCodes (err). toLatin1 (). data ());
	else
	   lnaValueDisplay	-> display (get_lnaGRdB (hwVersion, lnaState));
}

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
std::complex<int16_t> dumpBuf [numSamples];
static	int teller	= 0;
mir_sdr_ErrT	err;

	if (reset || hwRemoved)
	   return;

	for (i = 0; i < (int)numSamples; i ++) {
	   std::complex<float> symb = std::complex<float> (
	                                       (float) (xi [i]) / denominator,
	                                       (float) (xq [i]) / denominator);
	   localBuf [i] = symb;
	   dumpBuf [i] = std::complex<int16_t> (xi [i], xq [i]);
	}
	int res = p -> base -> addSymbol (localBuf, numSamples);
	if (p -> xmlDumping. load ())
	   p -> xmlWriter -> add (dumpBuf, numSamples);
	(void)	firstSampleNum;
	(void)	grChanged;
	(void)	rfChanged;
	(void)	fsChanged;
}

void	myGainChangeCallback (uint32_t	GRdB,
	                      uint32_t	lnaGRdB,
	                      void	*cbContext) {
sdrplayHandler	*p	= static_cast<sdrplayHandler *> (cbContext);
	if ((GRdB & 0x80000000) != 0)
	   return;
//	p -> GRdBDisplay	-> display ((int)GRdB);
	(void)lnaGRdB;
//	p -> lnaGRdBDisplay	-> display ((int)lnaGRdB);
}

bool	sdrplayHandler::restartReader	(int32_t frequency) {
int	gRdBSystem;
int	samplesPerPacket;
mir_sdr_ErrT	err;
int	GRdB		= 30;

	if (running. load ())
	   return true;

	vfoFrequency	= frequency;
	totalOffset	= 0;
//	fprintf (stderr, "restart op freq %d\n", frequency);
	err	= my_mir_sdr_StreamInit (&GRdB,
	                              double (inputRate) / MHz (1),
	                              double (frequency) / Mhz (1),
	                              mir_sdr_BW_1_536,
	                              mir_sdr_IF_Zero,
	                              lnaState,
	                              &gRdBSystem,
	                              mir_sdr_USE_RSP_SET_GR,
	                              &samplesPerPacket,
	                              (mir_sdr_StreamCallback_t)myStreamCallback,
	                              (mir_sdr_GainChangeCallback_t)myGainChangeCallback,
	                              this);
	if (err != mir_sdr_Success) {
	   fprintf (stderr, "error = %s\n",
	                errorCodes (err). toLatin1 (). data ());
	   return false;
	}
	err	= my_mir_sdr_SetPpm (double (ppmControl -> value ()));
	if (err != mir_sdr_Success) 
	   fprintf (stderr, "error = %s\n",
	                errorCodes (err). toLatin1 (). data ());

	err		= my_mir_sdr_SetDcMode (4, 1);
	if (err != mir_sdr_Success)
	   fprintf (stderr, "error = %s\n",
	                errorCodes (err). toLatin1 (). data ());
	err		= my_mir_sdr_SetDcTrackTime (63);
	if (err != mir_sdr_Success)
	   fprintf (stderr, "error = %s\n",
	                errorCodes (err). toLatin1 (). data ());
	if (agcControl -> isChecked ()) 
	   my_mir_sdr_AgcControl (mir_sdr_AGC_100HZ , -30,
                                  0, 0, 0, 0, lnaGainSetting -> value());
	else
	   my_mir_sdr_AgcControl (mir_sdr_AGC_DISABLE, -30,
                                  0, 0, 0, 0, lnaGainSetting -> value());
	running. store (true);
	return true;
}

void	sdrplayHandler::stopReader	(void) {
mir_sdr_ErrT err;

	if (!running. load ())
	   return;

	close_xmlDump ();
	err	= my_mir_sdr_StreamUninit	();
	if (err != mir_sdr_Success)
	   fprintf (stderr, "error = %s\n",
	                errorCodes (err). toLatin1 (). data ());
	running. store (false);
}

int32_t	sdrplayHandler::getVFOFrequency	(void) {
	return vfoFrequency;
}

void	sdrplayHandler::resetBuffer	(void) {
}

int16_t	sdrplayHandler::bitDepth	(void) {
	return nrBits;
}

void	sdrplayHandler::set_debugControl (int debugMode) {
	(void)debugMode;
	my_mir_sdr_DebugEnable (debugControl -> isChecked () ? 1 : 0);
}

void	sdrplayHandler::set_agcControl (int agcMode) {
	(void)agcMode;
	if (agcControl -> isChecked ()) 
	   my_mir_sdr_AgcControl (mir_sdr_AGC_100HZ , -30,
                                  0, 0, 0, 0, lnaGainSetting -> value());
	else
	   my_mir_sdr_AgcControl (mir_sdr_AGC_DISABLE, -30,
                                  0, 0, 0, 0, lnaGainSetting -> value());
}

void	sdrplayHandler::set_ppmControl (int ppm) {
	if (running. load ()) {
	   my_mir_sdr_SetPpm	((float)ppm);
	   my_mir_sdr_SetRf	((float)vfoFrequency, 1, 0);
	}
}

void	sdrplayHandler::set_antennaSelect (const QString &s) {
mir_sdr_ErrT err;

	if (hwVersion != 2)	// should not happen
	   return;

	if (s == "Antenna A") {
	   selectedAntenna = 'a';
	   err = my_mir_sdr_RSPII_AntennaControl (mir_sdr_RSPII_ANTENNA_A);
	}
	else {
	   selectedAntenna = 'b';
	   err = my_mir_sdr_RSPII_AntennaControl (mir_sdr_RSPII_ANTENNA_B);
	}

	if (err != mir_sdr_Success) 
	   fprintf (stderr, "error in set antenna (%s) %s\n",
	                     s. toLatin1 (). data (),
	                     errorCodes (err). toLatin1 (). data ());
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

void	sdrplayHandler::show	(void) {
	myFrame. show	();
}

void	sdrplayHandler::hide	(void) {
	myFrame. hide	();
}

bool	sdrplayHandler::isHidden	(void) {
	return !myFrame. isVisible ();
}

QString	sdrplayHandler::deviceName	(void) {
	return deviceLabel -> text ();
}

bool	sdrplayHandler::loadFunctions () {
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

bool	sdrplayHandler::fetchLibrary	() {
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
	      return false;
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
	      return false;
	   }
	}
#else
	Handle		= dlopen ("libusb-1.0.so", RTLD_NOW | RTLD_GLOBAL);
	Handle		= dlopen ("libmirsdrapi-rsp.so", RTLD_NOW);
	if (Handle == nullptr)
	   Handle	= dlopen ("libmir_sdr.so", RTLD_NOW);

	if (Handle == nullptr) {
	   fprintf (stderr, "error report %s\n", dlerror());
	   return false;
	}
#endif
	return true;
}

void	sdrplayHandler::releaseLibrary	() {
#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
           dlclose (Handle);
#endif
}

void	sdrplayHandler::handle_Value (int offset, float lowVal, float highVal) {
	setOffset (offset);
	setGains  (lowVal, highVal);
}

void	sdrplayHandler::set_xmlDump () {
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

bool	sdrplayHandler::setup_xmlDump () {
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
	                                      "SDRplay",
	                                      deviceLabel -> text (),
	                                      recorderVersion);
	xmlDumping. store (true);

	QString dumper	= QDir::fromNativeSeparators (fileName);
	int x		= dumper. lastIndexOf ("/");
        saveDir		= dumper. remove (x, dumper. count () - x);
	sdrplaySettings	-> setValue ("saveDir_xmlDump", saveDir);
	return true;
}

void	sdrplayHandler::close_xmlDump () {
	if (xmlDumper == nullptr)	// this can happen !!
	   return;
	xmlDumping. store (false);
	usleep (1000);
	xmlWriter	-> computeHeader ();
	delete xmlWriter;
	fclose (xmlDumper);
	xmlDumper	= nullptr;
}

