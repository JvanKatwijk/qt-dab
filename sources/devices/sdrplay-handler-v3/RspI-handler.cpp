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
#include	"RspI-handler.h"
#include	"sdrplay-handler-v3.h"
#include	"errorlog.h"

static
int	RSP1_Table [3][5] = {
	{4, 0, 24, 19, 43},
	{4, 0,  7, 19, 26},
	{4, 0,  5, 19, 24}};


	Rsp1_handler::Rsp1_handler   (sdrplayHandler_v3 *parent,
	                              errorLogger	*theLogger,
	                              sdrplay_api_DeviceT *chosenDevice,
	                              int	freq,
	                              bool	agcMode,
	                              int	lnaState,
	                              int 	GRdB,
	                              bool	biasT,
	                              double	ppmValue):
	                              RspDevice (parent,
	                                          chosenDevice, 
	                                          freq,
	                                          agcMode,
	                                          lnaState,
	                                          GRdB,
	                                          1,
	                                          biasT, ppmValue) {
	theErrorLogger			= theLogger;
	this	-> deviceModel		= "RSP-1";
	this	-> nrBits		= 12;
	this	-> lnaUpperBound =  lnaStates (freq);
	setLnaBoundsSignal	(0, lnaUpperBound);
	if (lnaState > lnaUpperBound)
	   this -> lnaState = lnaUpperBound;
	setLna (this -> lnaState);

	if (biasT)
	   setBiasT (true);
}

	Rsp1_handler::~Rsp1_handler	() {}

int16_t Rsp1_handler::bankFor_rsp1 (int freq) {
	if (freq < MHz (420))
	   return 0;
	if (freq < MHz (1000))
	   return 1;
	return 2;
}

int	Rsp1_handler::lnaStates (int frequency) {
int band	= bankFor_rsp1 (frequency);
	return RSP1_Table [band][0];
}

int	Rsp1_handler::getLnaGain (int lnaState, int freq) {
int	band	= bankFor_rsp1 (freq);
	return RSP1_Table [band][lnaState + 1];
}

bool	Rsp1_handler::restart (int freq) {
sdrplay_api_ErrT        err;

	chParams -> tunerParams. rfFreq. rfHz = (float)freq;
	err =parent ->  sdrplay_api_Update (chosenDevice -> dev,
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
	this	-> lnaUpperBound	= 3;
	setLnaBoundsSignal	(0, lnaUpperBound);
	showLnaGain (getLnaGain (lnaState, freq));
	showState (QString ("Restart at ") + QString::number (freq / 1000) + "Khz");
	return true;
}

bool	Rsp1_handler::setLna	(int lnaState) {
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

bool	Rsp1_handler::setBiasT	(bool biasT_value) {
	(void)biasT_value;
	return true;
}

bool	Rsp1_handler::setNotch	(bool on) {
	(void)on;
	return true;
}

