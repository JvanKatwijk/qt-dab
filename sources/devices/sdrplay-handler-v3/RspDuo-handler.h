#
/*
 *    Copyright (C) 2020 .. 2024
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation version 2 of the License.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#pragma once

#include	"Rsp-device.h"

class	sdrplayHandler_v3;

class	RspDuo_handler: public RspDevice {
public:
		RspDuo_handler (sdrplayHandler_v3 *parent,
	                       sdrplay_api_DeviceT *chosenDevice,
	                       int	freq,
	                       bool	agcMode,
	                       int	lnaState,
	                       int 	GRdB,
	                       int	antennaValue,
	                       int	tuner,
	                       bool	biasT,
	                       bool	notch,
	                       double	ppmValue);
		~RspDuo_handler	();

	int	lnaStates	(int frequency);
	bool	restart		(int freq);
	bool	setLna		(int lnaState);
	bool	setAntenna 	(int antenna);
	bool	setTuner	(int tuner);
	bool	setBiasT	(bool biasT);
	bool	setNotch	(bool on);
private:
	int16_t	bankFor_rspDuo 	(int freq);
	int	getLnaGain	(int, int);
	sdrplayHandler_v3	*parent;
	int	currentTuner;
};
