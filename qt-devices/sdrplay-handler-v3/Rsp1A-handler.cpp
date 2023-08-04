
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
	                              int	sampleRate,
	                              int	freq,
	                              bool	agcMode,
	                              int	lnaState,
	                              int 	GRdB,
	                              bool	biasT) :
	                              Rsp_device (parent,
	                                          chosenDevice, 
	                                          sampleRate,
	                                          freq,
	                                          agcMode,
	                                          lnaState,
	                                          GRdB, biasT) {

	this	-> deviceModel		= "RSP-1A";
	this	-> nrBits		= 14;
	this	-> lna_upperBound =  lnaStates (freq);
	set_lnabounds_signal	(0, lna_upperBound);
	set_deviceName_signal	(deviceModel);
	set_nrBits_signal	(nrBits);
	if (lnaState > lna_upperBound)
	   this -> lnaState = lna_upperBound;
	set_lna (this -> lnaState);

	if (biasT)
	   set_biasT (true);
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

int	Rsp1A_handler::get_lnaGain (int lnaState, int freq) {
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
	   fprintf (stderr, "restart: error %s\n",
	                         parent -> sdrplay_api_GetErrorString (err));
	   return false;
	}

	this -> freq	= freq;
	if (freq < MHz (60))
	   this -> lna_upperBound = RSPIA_NUM_LNA_STATES_AM;
	else
	if  (freq < MHz (1000))
	   this -> lna_upperBound = RSPIA_NUM_LNA_STATES;
	else	
	   this -> lna_upperBound = RSPIA_NUM_LNA_STATES_LBAND;
	set_lnabounds_signal	(0, lna_upperBound);
	show_lnaGain (get_lnaGain (lnaState, freq));
	return true;
}

bool	Rsp1A_handler::set_agc	(int setPoint, bool on) {
sdrplay_api_ErrT        err;

	if (on) {
	   chParams    -> ctrlParams. agc. setPoint_dBfs = setPoint;
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

bool	Rsp1A_handler::set_GRdB	(int GRdBValue) {
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

bool	Rsp1A_handler::set_ppm	(int ppmValue) {
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

bool	Rsp1A_handler::set_lna	(int lnaState) {
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

bool	Rsp1A_handler::set_biasT	(bool biasT_value) {
sdrplay_api_Rsp1aTunerParamsT *rsp1aTunerParams;
sdrplay_api_ErrT        err;

	rsp1aTunerParams	= &(chParams -> rsp1aTunerParams);
	rsp1aTunerParams -> biasTEnable = biasT_value;
	err = parent ->  sdrplay_api_Update (chosenDevice -> dev,
                                  chosenDevice -> tuner,
	                          sdrplay_api_Update_Rsp1a_BiasTControl,
		                  sdrplay_api_Update_Ext1_None);
	if (err != sdrplay_api_Success) {
	   fprintf (stderr, "setBiasY: error %s\n",
	                         parent -> sdrplay_api_GetErrorString (err));
	   return false;
	}
	return true;
}

