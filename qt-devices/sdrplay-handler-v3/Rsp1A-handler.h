
#ifndef	__RSP1A_HANDLER_H
#define	__RSP1A_HANDLER_H

#include	"Rsp-device.h"

class	sdrplayHandler_v3;

class	Rsp1A_handler: public Rsp_device {
public:
		Rsp1A_handler (sdrplayHandler_v3 *parent,
	                       sdrplay_api_DeviceT *chosenDevice,
	                       int	sampleRate,
	                       int	freq,
	                       bool	agcMode,
	                       int	lnaState,
	                       int 	GRdB,
	                       bool	biasT);
		~Rsp1A_handler	();

	int	lnaStates	(int frequency);
	bool	restart		(int freq);
	bool	set_agc		(int setPoint, bool on);
	bool	set_GRdB	(int GRdBValue);
	bool	set_ppm		(int ppm);
	bool	set_lna		(int lnaState);
	bool	set_biasT	(bool);
private:
	int16_t	bankFor_rsp1A 	(int freq);
	int	get_lnaGain	(int, int);
};
#endif



