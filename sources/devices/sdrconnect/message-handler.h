#
/*
 *    Copyright (C)   2026
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
#pragma once

#include	<QString>
#include	<complex>
#include	<atomic>
#include	"ringbuffer.h"
#include	"socket-handler.h"
#include	"dab-constants.h"

class messageHandler: public socketHandler {
Q_OBJECT
public:
		messageHandler (const QString &hostAddress,
	                        int	portNumber,
	                        int	startFreq,
	                        RingBuffer<std::complex<float>> *);
		~messageHandler	();
	bool	restartReader	(int32_t, int);
	void	stopReader	();
	int32_t	getVFOFrequency	();
private:
	RingBuffer<std::complex<int16_t>> _I_Buffer;
	RingBuffer<std::complex<float>> *_O_Buffer;
	void		iqStreamEnable	(bool);
	void		setFrequency	(int32_t);
	void		setProperty	(const QString, const QString);
	void		askProperty	(const QString);
	int		outputRate;
	int		theSamplerate;
	int		vfo_frequency;
	bool		runMode;
	int16_t         convBufferSize;
        std::atomic<int16_t>  convIndex;
        std::vector <std::complex<float> >      convBuffer;
        int16_t         mapTable_int   [SAMPLERATE / 1000];
        float           mapTable_float [SAMPLERATE / 1000];
private slots:
	void	connection_set		();
	void	no_connection		();
	void	binDataAvailable	();
	void	dispatchMessage		(const QString &);
signals:
	void	connection_success	();
	void	connection_failed	();
	void	frequency_changed	(int);
	void	signalPower		(double);
	void	dataAvailable		(int);
	void	rateOK			();	
	void	rateError		();
};

