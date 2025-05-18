#
/*
 *    Copyright (C) 2014 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB
 *
 *    Many of the ideas as implemented in Qt-DAB are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
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
 *
 */
#pragma once

#include	<QObject>
#include	<atomic>
#include	<thread>
#include	"device-handler.h"
#include	"ringbuffer.h"
#include	<SoapySDR/Device.h>
#include	"soapy-converter.h"
#include	"ui_soapy-widget.h"

class SoapySdr_Thread;
class	QSettings;


class soapyHandler : //public QObject,
	              public deviceHandler, public Ui_soapyWidget {
Q_OBJECT
public:
		soapyHandler	(QSettings *);
		~soapyHandler	();
	
	bool	restartReader	(int, int skipped = 0);
	void	stopReader	();
	void	reset		();
	int32_t getSamples	(std::complex<float> * Buffer, int32_t Size);
	int32_t	Samples		();
	float	getGain		() const;
	int32_t	getGainCount	();
	bool	isFileInput	();

private:
	RingBuffer<std::complex<float>> m_sampleBuffer;
	soapyConverter	theConverter;
	SoapySDRStream	*rxStream;
	void	setAntenna	(const std::string& antenna);
	void	decreaseGain	();
	void	increaseGain	();

	void	createDevice	(const QString &);
	int m_freq = 0;
	std::string m_driver_args;
	std::string m_antenna;
	SoapySDRDevice *m_device = nullptr;
	std::atomic<bool> m_running;
	std::atomic<bool> deviceReady;
	bool m_sw_agc = false;
	bool	hasAgc;
	std::vector<double> m_gains;

	std::thread m_thread;
	void workerthread(void);
	void process(SoapySDRStream *stream);

	int	findDesiredRange (SoapySDRRange * theRanges, int length);
private slots:
	void	setAgc		(int);
	void	setGain		(int);
};

