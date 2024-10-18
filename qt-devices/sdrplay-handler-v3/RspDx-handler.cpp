#
/*
 *    Copyright (C) 2020
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
#include	"RspDx-handler.h"
#include	"sdrplay-handler-v3.h"

	RspDx_handler::RspDx_handler (sdrplayHandler_v3 *parent,
	                              sdrplay_api_DeviceT *chosenDevice,
	                              int	sampleRate,
	                              int	freq,
	                              bool	agcMode,
	                              int	lnaState,
	                              int 	GRdB,
	                              int	antennaValue,
	                              bool	biasT) :
	                              Rsp_device (parent,
	                                          chosenDevice, 
	                                          sampleRate,
	                                          freq,
	                                          agcMode,
	                                          lnaState,
	                                          GRdB,
	                                          biasT) {
	set_antenna (antennaValue);
	this	-> deviceModel		= "RSP-Dx";
	this	-> nrBits		= 14;
	this	-> lna_upperBound	= lnaStates (freq);
	set_lnabounds_signal	(0, lna_upperBound);
	if (lnaState > lna_upperBound)
	   this -> lnaState = lna_upperBound - 1;
	set_lna (this -> lnaState);
	if (biasT)
	   set_biasT (true);
}

	RspDx_handler::~RspDx_handler	() {}
//
//	while we know that the regular frequencies for DAB reception
//	are in the 60 - 250 MHz, there is always a chance that someone
//	experiments with his own defined band on a different frequency
static
int	RSPdx_Table [6][29] = {
	{19, 0, 3,  6,  9, 12, 15, 18, 24, 27, 30, 33, 36, 39, 42,
	           45, 48, 51, 54, 57, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{27, 0, 3,  6,  9, 12, 15, 24, 27, 30, 33, 36, 39, 42, 45,
	           48, 51, 54, 57, 60, 63, 66, 69, 72, 75, 78, 81, 84, 0},
	{28, 0, 3,  6,  9, 12, 15, 18, 24, 27, 30, 33, 36, 39, 42,
	          45, 48, 51, 54, 57, 60, 63, 66, 69, 72, 75, 78, 81, 84},
	{21, 0, 7, 10, 13, 16, 19, 22, 25, 31, 34, 37, 40, 43, 46,
	          49,  52, 55, 58, 61, 64, 67, 0, 0, 0, 0, 0, 0, 0},
	{19, 0, 5,  8, 11, 14, 17, 20, 32, 35, 38, 41, 44, 47, 50,
	          53, 56, 59, 62, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};


int16_t RspDx_handler::bankFor_rspdx (int freq) {
	if (freq < MHz (12))
	   return 0;
	if (freq < Mhz (60))
	   return 1;
	if (freq < MHz (250))
	   return 2;
	if (freq < MHz (420))
	   return 3;
	if (freq < MHz (1000))
	   return 4;
	return 5;
}

int	RspDx_handler::lnaStates (int frequency) {
int band	= bankFor_rspdx (frequency);
	return RSPdx_Table [band][0];
}

int     RspDx_handler::get_lnaGain (int lnaState, int freq) {
int     band    = bankFor_rspdx (freq);
        return RSPdx_Table [band][lnaState + 1];
}

bool	RspDx_handler::restart (int freq) {
sdrplay_api_ErrT        err;

	fprintf (stderr, "restartRequest\n");
	chParams -> tunerParams. rfFreq. rfHz = (float)freq;
	err =parent ->  sdrplay_api_Update (chosenDevice -> dev,
	                                    chosenDevice -> tuner,
                                            sdrplay_api_Update_Tuner_Frf,
                                            sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "restart: error %s\n",
	                         parent -> sdrplay_api_GetErrorString (err));
	   return false;
	}

	this	-> freq	= freq;
	set_lnabounds_signal	(0, lnaStates (freq));
	show_lnaGain (get_lnaGain (lnaState, freq));

	return true;
}

bool	RspDx_handler::set_agc	(int setPoint, bool on) {
sdrplay_api_ErrT        err;

	if (on) {
	   chParams    -> ctrlParams. agc. setPoint_dBfs = - setPoint;
	   chParams    -> ctrlParams. agc. enable = sdrplay_api_AGC_100HZ;
	}
	else
	   chParams    -> ctrlParams. agc. enable =
                                             sdrplay_api_AGC_DISABLE;

	err = parent ->  sdrplay_api_Update (chosenDevice -> dev,
	                                     chosenDevice -> tuner,
                                             sdrplay_api_Update_Ctrl_Agc,
                                             sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "agc: error %s\n",
	                          parent -> sdrplay_api_GetErrorString (err));
	   return false;
	}

	this	-> agcMode = on;
	return true;
}

bool	RspDx_handler::set_GRdB	(int GRdBValue) {
sdrplay_api_ErrT        err;

	chParams -> tunerParams. gain. gRdB = GRdBValue;
	err = parent ->  sdrplay_api_Update (chosenDevice -> dev,
	                                     chosenDevice -> tuner,
	                                     sdrplay_api_Update_Tuner_Gr,
	                                     sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "grdb: error %s\n",
                                   parent -> sdrplay_api_GetErrorString (err));
	   return false;
	}
	this	-> GRdB = GRdBValue;
	return true;
}

bool	RspDx_handler::set_ppm	(int ppmValue) {
sdrplay_api_ErrT        err;

	deviceParams    -> devParams -> ppm = ppmValue;
	err = parent -> sdrplay_api_Update (chosenDevice -> dev,
	                                    chosenDevice -> tuner,
	                                    sdrplay_api_Update_Dev_Ppm,
	                                    sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "lna: error %s\n",
	                          parent -> sdrplay_api_GetErrorString (err));
	   return false;
	}
	return true;
}

bool	RspDx_handler::set_lna	(int lnaState) {
sdrplay_api_ErrT        err;

	chParams -> tunerParams. gain. LNAstate = lnaState;
	err = parent -> sdrplay_api_Update (chosenDevice -> dev,
	                                    chosenDevice -> tuner,
	                                    sdrplay_api_Update_Tuner_Gr,
	                                    sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "grdb: error %s\n",
	                         parent -> sdrplay_api_GetErrorString (err));
	   return false;
	}

	this	-> lnaState = lnaState;
	show_lnaGain (get_lnaGain (lnaState, freq));
	return true;
}

bool	RspDx_handler::set_antenna (int antenna) {
sdrplay_api_ErrT        err;

	deviceParams    -> devParams -> rspDxParams. antennaSel =
	                           antenna == 'A' ?
                                             sdrplay_api_RspDx_ANTENNA_A:
	                           antenna == 'B' ?
                                             sdrplay_api_RspDx_ANTENNA_B :
	                                        sdrplay_api_RspDx_ANTENNA_C;
	    
	err = parent ->  sdrplay_api_Update (chosenDevice -> dev, 
	                                     chosenDevice -> tuner,
	                                     sdrplay_api_Update_None,
	                                     sdrplay_api_Update_RspDx_AntennaControl);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "Updating antenna to %c mislukt\n", antenna);
	   return false;
	}
	fprintf (stderr, "Update to antenna %c successfull\n", antenna);
	return true;
}

bool	RspDx_handler::set_amPort (int amPort) {
sdrplay_api_ErrT        err;

	deviceParams    -> devParams -> rspDxParams. hdrEnable = amPort;
	err = parent -> sdrplay_api_Update (chosenDevice -> dev,
	                                    chosenDevice -> tuner,
	                                    sdrplay_api_Update_None,
	                                    sdrplay_api_Update_RspDx_HdrEnable);
	if (err != sdrplay_api_Success)
	   return false;

	return true;
}

bool	RspDx_handler::set_biasT (bool biasT_value) {
sdrplay_api_DevParamsT *xxx;
sdrplay_api_RspDxParamsT *rspDxParams;
sdrplay_api_ErrT        err;

	xxx = deviceParams -> devParams;
	rspDxParams	= &(xxx -> rspDxParams);
	rspDxParams	-> biasTEnable = biasT_value;
	err = parent ->  sdrplay_api_Update (chosenDevice -> dev,
	                                 chosenDevice -> tuner,
	                                 sdrplay_api_Update_None,
	               	                 sdrplay_api_Update_RspDx_BiasTControl);

	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "setBiasY: error %s\n",
                                 parent -> sdrplay_api_GetErrorString (err));
           return false;
	}

	return true;
}

