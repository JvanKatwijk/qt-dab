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
#include	"Rsp1A-handler.h"
#include	"sdrplay-handler-v3.h"

static
int     RSP1A_Table [4] [11] = {
	{7,  0, 6, 12, 18, 37, 42, 61, -1, -1, -1},
	{10, 0, 6, 12, 18, 20, 26, 32, 38, 57, 62},
	{10, 0, 7, 13, 19, 20, 27, 33, 39, 45, 64},
	{9, 0, 6, 12, 20, 26, 32, 38, 43, 62, -1}
};

	Rsp1A_handler::Rsp1A_handler (sdrplayHandler_v3 *parent,
	                              sdrplay_api_DeviceT *chosenDevice,
	                              int	freq,
	                              bool	agcMode,
	                              int	lnaState,
	                              int 	GRdB,
	                              bool	biasT,
	                              bool	notch,
	                              double	ppmValue) :
	                              RspDevice (parent,
	                                          chosenDevice, 
	                                          freq,
	                                          agcMode,
	                                          lnaState,
	                                          GRdB,
	                                          biasT, ppmValue) {

	this	-> deviceModel		= "RSP-1A";
	this	-> nrBits		= 12;
	this	-> lnaUpperBound	= lnaStates (freq);
	setLnaBoundsSignal (0, lnaUpperBound);
	if (lnaState > lnaUpperBound)
	   this -> lnaState = lnaUpperBound;
	setLna (this -> lnaState);

	if (biasT)
	   setBiasT (true);
	if (notch)
	   setNotch (true);
}

	Rsp1A_handler::~Rsp1A_handler	() {}

int16_t Rsp1A_handler::bankFor_rsp1A (int freq) {
	if (freq < MHz (60))
	   return 0;
	if (freq < MHz (420))
	   return 1;
	if (freq < MHz (1000))
	   return 2;
	return 3;
}

int	Rsp1A_handler::lnaStates (int frequency) {
int band	= bankFor_rsp1A (frequency);
	return RSP1A_Table [band][0];
}

int	Rsp1A_handler::getLnaGain (int lnaState, int freq) {
int	band	= bankFor_rsp1A (freq);
	return RSP1A_Table [band][lnaState + 1];
}

bool	Rsp1A_handler::restart (int freq) {
sdrplay_api_ErrT        err;

	chParams -> tunerParams. rfFreq. rfHz = (float)freq;
	err =parent ->  sdrplay_api_Update (chosenDevice -> dev,
	                                    chosenDevice -> tuner,
                                            sdrplay_api_Update_Tuner_Frf,
                                            sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   showState (parent -> sdrplay_api_GetErrorString (err));
	   return false;
	}

	this -> freq	= freq;
	if (freq < MHz (60))
	   this -> lnaUpperBound = RSPIA_NUM_LNA_STATES_AM;
	else
	if  (freq < MHz (1000))
	   this -> lnaUpperBound = RSPIA_NUM_LNA_STATES;
	else	
	   this -> lnaUpperBound = RSPIA_NUM_LNA_STATES_LBAND;
	setLnaBoundsSignal	(0, lnaUpperBound);
	showLnaGain (getLnaGain (lnaState, freq));
	showState (QString ("Restart ") + QString::number(freq / 1000) + "Khz");
	return true;
}

bool	Rsp1A_handler::setLna	(int lnaState) {
sdrplay_api_ErrT        err;

	chParams -> tunerParams. gain. LNAstate = lnaState;
	err = parent -> sdrplay_api_Update (chosenDevice -> dev,
	                                    chosenDevice -> tuner,
	                                    sdrplay_api_Update_Tuner_Gr,
	                                    sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   showState (parent -> sdrplay_api_GetErrorString (err));
	   return false;
	}
	this	-> lnaState	= lnaState;
	showLnaGain (getLnaGain (lnaState, freq));
	return true;
}

bool	Rsp1A_handler::setBiasT	(bool biasT_value) {
sdrplay_api_Rsp1aTunerParamsT *rsp1aTunerParams;
sdrplay_api_ErrT        err;

	rsp1aTunerParams	= &(chParams -> rsp1aTunerParams);
	rsp1aTunerParams -> biasTEnable = biasT_value;
	err = parent ->  sdrplay_api_Update (chosenDevice -> dev,
                                  chosenDevice -> tuner,
	                          sdrplay_api_Update_Rsp1a_BiasTControl,
		                  sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   showState (parent -> sdrplay_api_GetErrorString (err));
	   fprintf (stderr, "setBiasT: error %s\n",
	                         parent -> sdrplay_api_GetErrorString (err));
	   return false;
	}
	return true;
}

bool	Rsp1A_handler::setNotch (bool on) {
sdrplay_api_ErrT err;

	deviceParams -> devParams -> rsp1aParams. rfNotchEnable = on;
	err = parent -> sdrplay_api_Update (chosenDevice -> dev,
                                           chosenDevice -> tuner,
                                           sdrplay_api_Update_Rsp1a_RfNotchControl, sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success)
	   showState (parent -> sdrplay_api_GetErrorString (err));
	return err == sdrplay_api_Success;
}
