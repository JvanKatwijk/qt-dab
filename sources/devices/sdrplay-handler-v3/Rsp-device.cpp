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
#include	"Rsp-device.h"

#include	"dab-constants.h"
#include	<stdint.h>
#include	<sdrplay_api.h>
#include	"sdrplay-handler-v3.h"

	RspDevice::RspDevice 	(sdrplayHandler_v3 *parent,
	                         sdrplay_api_DeviceT *chosenDevice,
	                         int startFreq,
	                         bool agcMode,
	                         int lnaState,
	                         int GRdB,
	                         bool biasT,
	                         double ppmValue) {
sdrplay_api_ErrT        err;
	this	-> parent	= parent;
	this	-> chosenDevice	=  chosenDevice;
	this	-> freq		= startFreq;
	this	-> agcMode	= agcMode;
	this	-> lnaState	= lnaState;
	this	-> GRdB		= GRdB;
	this	-> biasT	= biasT;

	connect (this, &RspDevice::setLnaBoundsSignal,
                 parent, &sdrplayHandler_v3::setLnaBounds);
	connect (this, &RspDevice::showLnaGain,
	         parent, &sdrplayHandler_v3::showLnaGain);
	connect (this, &RspDevice::showState,
	         parent, &sdrplayHandler_v3::showState);

	err = parent -> sdrplay_api_GetDeviceParams (chosenDevice -> dev,
	                                             &deviceParams);

	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "sdrplay_api_GetDeviceParams failed %s\n",
	                         parent -> sdrplay_api_GetErrorString (err));
	   throw (21);
	}

	if (deviceParams == nullptr) {
	   fprintf (stderr, "sdrplay_api_GetDeviceParams return null as par\n");
	   throw (22);
	}

	this	-> chParams	= deviceParams -> rxChannelA;
	deviceParams	-> devParams	-> ppm		= ppmValue;
	deviceParams    -> devParams	-> fsFreq. fsHz    = SAMPLERATE;
        chParams        -> tunerParams. bwType = sdrplay_api_BW_1_536;
        chParams        -> tunerParams. ifType = sdrplay_api_IF_Zero;
//
//      these will change:
        chParams        -> tunerParams. rfFreq. rfHz    = (float)220000000;
//
//	It is known that all supported Rsp's can handle the values
//	as given below. It is up to the particular Rsp to check
//	correctness of the given lna and GRdB
	if (GRdB > 59)
	   this -> GRdB = 59;
	if (GRdB < 20)
	   this -> GRdB = 20;

        chParams        -> tunerParams. gain.gRdB       = this -> GRdB;
	chParams        -> tunerParams. gain.LNAstate   = 3;

//	Thanks to Peter:
	chParams	-> ctrlParams. agc. setPoint_dBfs = -30;
	chParams	-> ctrlParams. agc. attack_ms = 500;
	chParams	-> ctrlParams. agc. decay_ms = 500;
	chParams	-> ctrlParams. agc. decay_delay_ms = 200;
	chParams	-> ctrlParams. agc. decay_threshold_dB = 3;

	if (this -> agcMode) 
	   chParams	-> ctrlParams. agc. enable = sdrplay_api_AGC_CTRL_EN;
        else
           chParams	-> ctrlParams. agc. enable =
                                             sdrplay_api_AGC_DISABLE;

	err	= parent -> sdrplay_api_Init (chosenDevice -> dev,
	                                      &parent -> cbFns, parent);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "sdrplay_api_Init failed %s\n",
                                    parent -> sdrplay_api_GetErrorString (err));
	   throw (23);
	}
}

	RspDevice::~RspDevice	() {}

//
//	lna states are model dependent
int	RspDevice::lnaStates	(int frequency) {
	(void)frequency;
	return 0;
}

//	restart is model deependent
bool	RspDevice::restart	(int freq) {
	(void)freq;
	return false;
}

//	setting an lna state is model deoendent
bool	RspDevice::setLna	(int lnaState) {
	(void)lnaState;
	return false;
}
//
//	setting agc is common to all models
bool	RspDevice::setAgc	(int setPoint, bool on) {
sdrplay_api_ErrT err;

	if (on) {
	   chParams -> ctrlParams. agc. setPoint_dBfs = setPoint;
	   chParams -> ctrlParams. agc.enable = sdrplay_api_AGC_CTRL_EN;
	}
	else
	   chParams->ctrlParams.agc.enable = sdrplay_api_AGC_DISABLE;

	err = parent -> sdrplay_api_Update (chosenDevice -> dev,
	                                    chosenDevice -> tuner,
	                                    sdrplay_api_Update_Ctrl_Agc,
	                                    sdrplay_api_Update_Ext1_None);
	return err == sdrplay_api_Success;
}

//	setting  GRdB is common to all models
bool	RspDevice::setGRdB	(int GRdBValue) {
sdrplay_api_ErrT err;

	chParams -> tunerParams. gain.gRdB = GRdBValue;
	err = parent -> sdrplay_api_Update (chosenDevice -> dev,
	                                    chosenDevice -> tuner,
	                                    sdrplay_api_Update_Tuner_Gr,
	                                    sdrplay_api_Update_Ext1_None);
	if (err == sdrplay_api_Success) {
	   GRdB = GRdBValue;
	   return true;
	}
	return false;
}
//
//	setting ppm is common to all models
bool	RspDevice::setPpm	(double ppmValue) {
sdrplay_api_ErrT err;

	deviceParams -> devParams -> ppm = ppmValue;
	err = parent -> sdrplay_api_Update (chosenDevice -> dev,
	                                    chosenDevice -> tuner,
	                                    sdrplay_api_Update_Dev_Ppm,
	                                    sdrplay_api_Update_Ext1_None);
	return err == sdrplay_api_Success;
}

//	setting an antenna select is model dependent
bool	RspDevice::setAntenna	(int antenna) {
	(void)antenna;
	return false;
}

//	setting amPort is model dependent
bool	RspDevice::setAmPort 	(int amPort) {
	(void)amPort;
	return false;
}

//	setting the bias is model dependent, RspI does not have it
bool	RspDevice::setBiasT (bool  b) {
	(void)b;
	return false;
}

//	setting the notch is model dependent (RspI does not have it)
bool	RspDevice::setNotch (bool  b) {
	(void)b;
	return false;
}

bool	RspDevice::setTuner	(int tuner) {
	(void)tuner;
	return false;
}

