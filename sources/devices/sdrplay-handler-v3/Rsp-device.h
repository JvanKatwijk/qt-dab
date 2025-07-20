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

#include	<QObject>
#include	<stdint.h>
#include	<stdio.h>
#include	<sdrplay_api.h>

class	sdrplayHandler_v3;

class RspDevice : public QObject {
Q_OBJECT
protected:
	sdrplay_api_DeviceT *chosenDevice;
	int	freq;
	bool	agcMode;
	int	lnaState;
	int	GRdB;
	sdrplay_api_RxChannelParamsT	*chParams;
	sdrplay_api_DeviceParamsT	*deviceParams;
	sdrplayHandler_v3	*parent;
	int	lnaUpperBound;
	QString	deviceModel;
	bool	antennaSelect;
	int	nrBits;
	bool	biasT;
public:
		RspDevice 	(sdrplayHandler_v3 *parent,
	                         sdrplay_api_DeviceT *chosenDevice,
	                         int startFrequency,
	                         bool agcMode,
	                         int lnaState,
	                         int GRdB,
	                         bool biasT, double ppmValue);
	virtual	~RspDevice	();
virtual int	lnaStates	(int frequency);

virtual	bool	restart		(int freq);
	bool	setAgc		(int setPoint, bool on);
virtual	bool	setLna		(int lnaState);
	bool	setGRdB		(int GRdBValue);
	bool	setPpm		(double ppm);
virtual	bool	setAntenna	(int antenna);
virtual	bool	setAmPort 	(int amPort);
virtual	bool	setBiasT 	(bool biasT);
virtual	bool	setNotch 	(bool notch);
virtual	bool	setTuner	(int tuner);
signals:
	void	setLnaBoundsSignal	(int, int);
	void	showLnaGain		(int);
};

