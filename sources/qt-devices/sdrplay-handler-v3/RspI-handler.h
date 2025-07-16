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
#pragma once

#include	"Rsp-device.h"

class	sdrplayHandler_v3;

class	Rsp1_handler: public RspDevice {
public:
		Rsp1_handler (sdrplayHandler_v3 *parent,
	                       sdrplay_api_DeviceT *chosenDevice,
	                       int	freq,
	                       bool	agcMode,
	                       int	lnaState,
	                       int 	GRdB,
	                       bool	biasT,
	                       double	ppmValue);
		~Rsp1_handler	();

	int	lnaStates	(int frequency);
	bool	restart		(int freq);
	bool	setLna		(int lnaState);
	bool	setBiasT	(bool);
	bool	setNotch	(bool);
private:
	int16_t	bankFor_rsp1 	(int freq);
	int	getLnaGain	(int, int);
};


