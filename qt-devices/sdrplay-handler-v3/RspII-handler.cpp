
#include	"RspII-handler.h"
#include	"sdrplay-handler-v3.h"

	RspII_handler::RspII_handler (sdrplayHandler_v3 *parent,
	                              sdrplay_api_DeviceT *chosenDevice,
	                              int	sampleRate,
	                              int	freq,
	                              bool	agcMode,
	                              int	lnaState,
	                              int 	GRdB, 
	                              bool 	biasT) :
	                              Rsp_device (parent,
	                                          chosenDevice, 
	                                         sampleRate,
	                                         freq,
	                                         agcMode,
	                                         lnaState,
	                                         GRdB, biasT) {

	this	-> lna_upperBound	= 9;
	this	-> deviceModel		= "RSP-II";
	this	-> nrBits		= 14;
	this	-> antennaSelect	= true;
	set_deviceName_signal	(deviceModel);
	set_antennaSelect_signal (1);
	set_nrBits_signal	(nrBits);
	if (freq < Mhz (420))
	   lna_upperBound = 9;
	else
	   lna_upperBound = 6;
	if (lnaState > lna_upperBound)
	   this -> lnaState = lna_upperBound - 1;
	set_lna (this -> lnaState);
	if (biasT)
	   set_biasT (true);
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

int	RspII_handler::get_lnaGain (int lnaState, int freq) {
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
	   fprintf (stderr, "restart: error %s\n",
	                         parent -> sdrplay_api_GetErrorString (err));
	   return false;
	}

	this -> freq	= freq;
	if (freq < MHz (420))
	   this	-> lna_upperBound	= 9;
	else
	   this -> lna_upperBound	= 6;
	
	set_lnabounds_signal	(0, lna_upperBound);
	show_lnaGain (get_lnaGain (lnaState, freq));
	
	return true;
}

bool	RspII_handler::set_agc	(int setPoint, bool on) {
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

bool	RspII_handler::set_GRdB	(int GRdBValue) {
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

bool	RspII_handler::set_ppm	(int ppmValue) {
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

bool	RspII_handler::set_lna	(int lnaState) {
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
	this	-> lnaState	= lnaState;
	show_lnaGain (get_lnaGain (lnaState, freq));
	return true;
}

bool	RspII_handler::set_antenna (int antenna) {
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
	if (err != sdrplay_api_Success)
	   return false;

	return true;
}

bool	RspII_handler::set_biasT (bool biasT_value) {
sdrplay_api_Rsp2TunerParamsT *rsp2TunerParams;
sdrplay_api_ErrT        err;

	rsp2TunerParams   = &(chParams -> rsp2TunerParams);
	rsp2TunerParams -> biasTEnable = biasT_value;
	err	= parent ->   sdrplay_api_Update (chosenDevice -> dev,
                                       chosenDevice -> tuner,
                                       sdrplay_api_Update_Rsp2_BiasTControl,
                                       sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success)
	   return false;

	return true;
}

