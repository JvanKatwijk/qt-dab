
#ifndef	__RSPDX_HANDLER_H
#define	__RSPDX_HANDLER_H

#include	"Rsp-device.h"

class	sdrplayHandler_v3;

class	RspDx_handler: public Rsp_device {
public:
		RspDx_handler (sdrplayHandler_v3 *parent,
	                       sdrplay_api_DeviceT *chosenDevice,
	                       int	sampleRate,
	                       int	freq,
	                       bool	agcMode,
	                       int	lnaState,
	                       int 	GRdB,
	                       bool	biasT);
		~RspDx_handler	();

	int	lnaStates	(int frequency);
	bool	restart		(int freq);
	bool	set_agc		(int setPoint, bool on);
	bool	set_GRdB	(int GRdBValue);
	bool	set_ppm		(int ppm);
	bool	set_lna		(int lnaState);
	bool	set_antenna 	(int antenna);
	bool	set_amPort 	(int amPort);
	bool	set_biasT	(bool biasT);
private:
	int16_t	bankFor_rspdx 	(int freq);
	int	get_lnaGain	(int, int);
};
#endif



