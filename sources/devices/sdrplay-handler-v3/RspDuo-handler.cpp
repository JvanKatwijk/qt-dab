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
#include	<QMessageBox>
#include	"RspDuo-handler.h"
#include	"sdrplay-handler-v3.h"
#include	"errorlog.h"

	RspDuo_handler::RspDuo_handler (sdrplayHandler_v3 *parent,
	                                errorLogger	*theLogger,
	                                sdrplay_api_DeviceT *chosenDevice,
	                                int	freq,
	                                bool	agcMode,
	                                int	lnaState,
	                                int 	GRdB,
	                                int	antennaValue,
	                                int	tuner,
	                                bool	biasT,
	                                bool	notch,
	                                double	ppmValue) :
	                                RspDevice (parent,
	                                           chosenDevice, 
	                                           freq,
	                                           agcMode,
	                                           lnaState,
	                                           GRdB,
	                                           tuner,
	                                           biasT,
	                                           ppmValue) {
	(void)antennaValue;
	this	-> parent		= parent;
	theErrorLogger			= theLogger;
	this	-> deviceModel		= "RSP-Duo";
	this	-> nrBits		= 14;
	this    -> lnaUpperBound	=  lnaStates (freq);
	setLnaBoundsSignal    (0, lnaUpperBound);
        if (lnaState > lnaUpperBound)
           this -> lnaState = lnaUpperBound;
        setLna (this -> lnaState);
	showLnaGain (getLnaGain (lnaState, freq));

	connect (this, &RspDuo_handler::enableBiasT,
	         parent, &sdrplayHandler_v3::enableBiasT);

	if (notch)
	   setNotch (true);
//
//	The Rspdevice will always set tuner to 1
	if (tuner != 1) {
	   sdrplay_api_ErrT err =
	           parent -> sdrplay_api_SwapRspDuoActiveTuner (
	                          chosenDevice ->  dev,
	                          &chosenDevice -> tuner, 
	                          sdrplay_api_RspDuo_AMPORT_2);
	   chParams	= deviceParams -> rxChannelB;
	   restart (freq);
	}
	currentTuner	= tuner;
}

	RspDuo_handler::~RspDuo_handler	() {}

static
int     RSPDuo_Table [4] [11] = {
	{7,  0, 6, 12, 18, 37, 42, 61, -1, -1, -1},
	{10, 0, 6, 12, 18, 20, 26, 32, 38, 57, 62},
	{10, 0, 7, 13, 19, 20, 27, 33, 39, 45, 64},
	{9, 0, 6, 12, 20, 26, 32, 38, 43, 62, -1}
};

int16_t RspDuo_handler::bankFor_rspDuo (int freq) {
	if (freq < MHz (60))
	   return 0;
	if (freq < MHz (420))
	   return 1;
	if (freq < MHz (1000))
	   return 2;
	return 3;
}

int	RspDuo_handler::lnaStates (int frequency) {
int band	= bankFor_rspDuo (frequency);
	return RSPDuo_Table [band][0];
}

int	RspDuo_handler::getLnaGain (int lnaState, int freq) {
int	band	= bankFor_rspDuo (freq);
	return RSPDuo_Table [band][lnaState + 1];
}

bool	RspDuo_handler::restart (int freq) {
sdrplay_api_ErrT        err;

	chParams -> tunerParams. rfFreq. rfHz = (float)freq;
	err = parent ->  sdrplay_api_Update (chosenDevice -> dev,
	                                     chosenDevice -> tuner,
                                             sdrplay_api_Update_Tuner_Frf,
                                             sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   QString errorString = parent -> sdrplay_api_GetErrorString (err);
	   showState (errorString);
	   theErrorLogger -> add (deviceModel, errorString);
	   return false;
	}

	this -> freq	= freq;
	if (freq < MHz (60))
	   this -> lnaUpperBound = RSPDUO_NUM_LNA_STATES_AM;
	else
	if  (freq < MHz (1000))
	   this -> lnaUpperBound = RSPDUO_NUM_LNA_STATES;
	else	
	   this -> lnaUpperBound = RSPDUO_NUM_LNA_STATES_LBAND;
	setLnaBoundsSignal	(0, lnaUpperBound);
	showLnaGain (getLnaGain (lnaState, freq));
	showState (QString ("Restart at ") + QString::number (freq / 1000) + "Khz");
	return true;
}

bool	RspDuo_handler::setLna	(int lnaState) {
sdrplay_api_ErrT        err;

	chParams -> tunerParams. gain. LNAstate = lnaState;
	err = parent -> sdrplay_api_Update (chosenDevice -> dev,
	                                    chosenDevice -> tuner,
	                                    sdrplay_api_Update_Tuner_Gr,
	                                    sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   QString errorString = parent -> sdrplay_api_GetErrorString (err);
	   showState (errorString);
	   theErrorLogger -> add (deviceModel, errorString);
	   return false;
	}
	this	-> lnaState	= lnaState;
	showLnaGain (getLnaGain (lnaState, freq));
	return true;
}
//
//	Not used
bool	RspDuo_handler::setAntenna (int antenna) {
sdrplay_api_RspDuoTunerParamsT *rspDuoTunerParams;
sdrplay_api_ErrT        err;
	if (chosenDevice -> tuner != 1)
	   return false;

        rspDuoTunerParams   = &(chParams -> rspDuoTunerParams);
        rspDuoTunerParams -> tuner1AmPortSel =
                                   antenna == 'A' ?
                                             sdrplay_api_RspDuo_AMPORT_2 :
                                             sdrplay_api_RspDuo_AMPORT_2;

	    
	err = parent ->  sdrplay_api_Update (chosenDevice -> dev, 
	                                     chosenDevice -> tuner,
	                                     sdrplay_api_Update_RspDuo_AmPortSelect,
	                                     sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   QString errorString = parent -> sdrplay_api_GetErrorString (err);
	   showState (errorString);
	   theErrorLogger -> add (deviceModel, errorString);
	   return false;
	}

	return true;
}

bool	RspDuo_handler::setTuner	(int tuner) {
	if (tuner == currentTuner)
	   return true;

	sdrplay_api_ErrT err =
	           parent -> sdrplay_api_SwapRspDuoActiveTuner (
	                          chosenDevice ->  dev,
	                          &chosenDevice -> tuner, 
	                          sdrplay_api_RspDuo_AMPORT_2);
	if (err != sdrplay_api_Success) {
	   QString errorString =  QString ("Tunerswitch ") + 
	                       parent -> sdrplay_api_GetErrorString (err);
	   showState (errorString);
	   theErrorLogger -> add (deviceModel, errorString);
	}
	else {
	   showState (QString  ("Switched to tuner ") +
	                                 QString::number (tuner));
	   currentTuner = tuner;
	}
	if (tuner == 1) 
	   chParams	= deviceParams -> rxChannelA;
	else
	   chParams	= deviceParams -> rxChannelB;
	restart (freq);
	return true;
}
//
//	If tuner != 2 the selector should not be visible
bool	RspDuo_handler::setBiasT	(bool biasT_value) {
sdrplay_api_RspDuoTunerParamsT *rspDuoTunerParams;
sdrplay_api_ErrT        err;

	if (currentTuner != 2) {
	   QMessageBox::warning (nullptr, tr ("Warning"),
                                       tr ("BiasT only at port B with tuner 2"));
	   return false;
	}
	rspDuoTunerParams	= &(chParams -> rspDuoTunerParams);
	rspDuoTunerParams	-> biasTEnable = biasT_value;
	err = parent ->  sdrplay_api_Update (chosenDevice -> dev,
                                  chosenDevice	-> tuner,
	                          sdrplay_api_Update_RspDuo_BiasTControl,
		                  sdrplay_api_Update_Ext1_None);
	                                                  
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "Error -> %s\n",
	                              parent -> sdrplay_api_GetErrorString (err));
	   QString errorString = parent -> sdrplay_api_GetErrorString (err);
	   showState (errorString);
	   theErrorLogger -> add (deviceModel, errorString);
	}
	return err == sdrplay_api_Success;
}

bool	RspDuo_handler::setNotch (bool on) {
sdrplay_api_ErrT err;
sdrplay_api_RspDuoTunerParamsT * rspDuoTunerParams;

	rspDuoTunerParams = &(chParams -> rspDuoTunerParams);
	rspDuoTunerParams -> rfNotchEnable = on;
	rspDuoTunerParams -> tuner1AmNotchEnable = on;

	err = parent -> sdrplay_api_Update (chosenDevice -> dev,
	                                    chosenDevice -> tuner,
                                            (sdrplay_api_ReasonForUpdateT)(sdrplay_api_Update_RspDuo_RfNotchControl | sdrplay_api_Update_RspDuo_Tuner1AmNotchControl),
	                                      sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   QString errorString = parent -> sdrplay_api_GetErrorString (err);
	   showState (errorString);
	   theErrorLogger -> add (deviceModel, errorString);
	}
	return err == sdrplay_api_Success;
}

