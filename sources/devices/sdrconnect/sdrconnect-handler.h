#
/*
 *    Copyright (C) 2026
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	"device-handler.h"
#include	"ringbuffer.h"
#include	"message-handler.h"
#include	"ui_sdrconnect-widget.h"
//
//	Implements the basic functions of "deviceHandler"

class	sdrConnectHandler: public deviceHandler,
	                              Ui_sdrconnectWidget {
Q_OBJECT
public:
		sdrConnectHandler	();
		~sdrConnectHandler	();
	bool	restartReader		(int32_t freq, int skipped);
	void	stopReader		();
	int32_t	getSamples		(std::complex<float> *b, int32_t size);
	int32_t	Samples			();
	void	resetBuffer		();
	int16_t	bitDepth		();
	QString	deviceName		();
	bool	isFileInput		();
	int32_t	getVFOFrequency		();
private:
	RingBuffer<std::complex<float>> _O_Buffer;
	messageHandler	*theMessager;
	bool	OK_to_run;
	int32_t	currentFrequency;
public slots:
	void	handle_hostName		();
	void	connection_failed	();
	void	signalPower		(double v);
	void	dataAvailable		(int);
	void	rateOK			();
	void	rateError		();
	void	show_dropCount		(int);
};



