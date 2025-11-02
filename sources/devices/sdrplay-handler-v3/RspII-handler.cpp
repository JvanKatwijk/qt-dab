#
/*
 *    Copyright (C) 2020 .. 2024
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
 *
 *    Qt-Dab is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation version 2 of the License.
 *
 *    Qt-Dab is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-Dab if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"RspII-handler.h"
#include	"sdrplay-handler-v3.h"
#include	"errorlog.h"

	RspII_handler::RspII_handler (sdrplayHandler_v3 *parent,
	                              errorLogger	*theLogger,
	                              sdrplay_api_DeviceT *chosenDevice,
	                              int	freq,
	                              bool	agcMode,
	                              int	lnaState,
	                              int 	GRdB, 
	                              int	antennaValue,
	                              bool 	biasT,
	                              bool	notch,
	                              double	ppmValue) :
	                              RspDevice (parent,
	                                          chosenDevice, 
	                                          freq,
	                                          agcMode,
	                                          lnaState,
	                                          GRdB,
	                                          1,
	                                          biasT,
	                                          ppmValue) {
//	setAntenna (antennaValue);
	theErrorLogger			= theLogger;
	this	-> deviceModel		= "RSP-II";
	this	-> nrBits		= 14;
	if (freq < Mhz (420))
	   lnaUpperBound = 9;
	else
	   lnaUpperBound = 6;
	setLnaBoundsSignal (0, lnaUpperBound);
	if (lnaState > lnaUpperBound)
	   this -> lnaState = lnaUpperBound - 1;
	setLna (this -> lnaState);
	if (biasT)
	   setBiasT (true);
	if (notch)
	   setNotch (true);
}

	RspII_handler::~RspII_handler	() {}

static
int     RSPII_Table [3] [10] = {
	{9, 0, 10, 15, 21, 24, 34, 39, 45, 64},
	{6, 0,  7, 10, 17, 22, 41, -1, -1, -1},
	{6, 0,  5, 21, 15, 15, 32, -1, -1, -1}
};

int16_t RspII_handler::bankFor_rspII (int freq) { 
	if (freq < MHz (420))
	   return 0;
	if (freq < MHz (1000))
	   return 1;
	return 2;
}

int	RspII_handler::lnaStates (int frequency) {
int band	= bankFor_rspII (frequency);
	return RSPII_Table [band][0];
}

int	RspII_handler::getLnaGain (int lnaState, int freq) {
int	band	= bankFor_rspII (freq);
	return RSPII_Table [band][lnaState + 1];
}

bool	RspII_handler::restart (int freq) {
sdrplay_api_ErrT        err;

	chParams -> tunerParams. rfFreq. rfHz = (float)freq;
	err =parent ->  sdrplay_api_Update (chosenDevice -> dev,
	                                    chosenDevice -> tuner,
                                            sdrplay_api_Update_Tuner_Frf,
                                            sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   showState (QString (parent -> sdrplay_api_GetErrorString (err)));
	   return false;
	}

	this -> freq	= freq;
	if (freq < MHz (420))
	   this	-> lnaUpperBound	= 9;
	else
	   this -> lnaUpperBound	= 6;
	
	setLnaBoundsSignal	(0, lnaUpperBound);
	showLnaGain (getLnaGain (lnaState, freq));
	showState (QString ("Restarted at ") + QString::number (freq / 1000) + "Khz");
	return true;
}

bool	RspII_handler::setLna	(int lnaState) {
sdrplay_api_ErrT        err;

	chParams -> tunerParams. gain. LNAstate = lnaState;
	err = parent -> sdrplay_api_Update (chosenDevice -> dev,
	                                    chosenDevice -> tuner,
	                                    sdrplay_api_Update_Tuner_Gr,
	                                    sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   QString errString = parent -> sdrplay_api_GetErrorString (err);
	   showState (errString);
	   theErrorLogger -> add (deviceModel, errString);
	   return false;
	}
	this	-> lnaState	= lnaState;
	showLnaGain (getLnaGain (lnaState, freq));
	return true;
}

bool	RspII_handler::setAntenna (int antenna) {
sdrplay_api_Rsp2TunerParamsT *rsp2TunerParams;
sdrplay_api_ErrT        err;

	rsp2TunerParams   = &(chParams -> rsp2TunerParams);
	rsp2TunerParams -> antennaSel =
	                           antenna == 'A' ?
                                             sdrplay_api_Rsp2_ANTENNA_A :
                                             sdrplay_api_Rsp2_ANTENNA_B;
	    
	err = parent ->  sdrplay_api_Update (chosenDevice -> dev, 
	                                     chosenDevice -> tuner,
	                                     sdrplay_api_Update_Rsp2_AntennaControl,
	                                     sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   QString errString = parent -> sdrplay_api_GetErrorString (err);
	   showState (errString);
	   theErrorLogger -> add (deviceModel, errString);
	}
	return err == sdrplay_api_Success;
}

bool	RspII_handler::setBiasT (bool biasT_value) {
sdrplay_api_Rsp2TunerParamsT *rsp2TunerParams;
sdrplay_api_ErrT        err;

	rsp2TunerParams   = &(chParams -> rsp2TunerParams);
	rsp2TunerParams -> biasTEnable = biasT_value;
	err	= parent ->   sdrplay_api_Update (chosenDevice -> dev,
                                       chosenDevice -> tuner,
                                       sdrplay_api_Update_Rsp2_BiasTControl,
                                       sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   QString errString = parent -> sdrplay_api_GetErrorString (err);
	   showState (errString);
	   theErrorLogger -> add (deviceModel, errString);
	}
	return err == sdrplay_api_Success;
}

bool	RspII_handler::setNotch (bool on) {
sdrplay_api_ErrT err;
sdrplay_api_Rsp2TunerParamsT *rspIITunerParams;

	rspIITunerParams = &(chParams->rsp2TunerParams);
	rspIITunerParams -> rfNotchEnable = on;
	err = parent -> sdrplay_api_Update (chosenDevice -> dev,
                                            chosenDevice -> tuner,
                                            sdrplay_api_Update_Rsp2_RfNotchControl,
                                            sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   QString errString = parent -> sdrplay_api_GetErrorString (err);
	   showState (errString);
	   theErrorLogger -> add (deviceModel, errString);
	}
	return err == sdrplay_api_Success;
}

