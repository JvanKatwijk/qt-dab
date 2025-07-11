#
/*
 *    Copyright (C) 2015
 *    Sebastian Held <sebastian.held@imst.de>
 *
 *    This file is part of the dab-cmdline
 *
 *    dab-cmdline is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    dab-cmdline is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dab-cmdline; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#pragma once

#include	"device-handler.h"

#include	<uhd/usrp/multi_usrp.hpp>
#include	"ringbuffer.h"
#include	<QThread>
#include	<QSettings>
#include	<QFrame>
#include	<QObject>
#include	"ui_uhd-widget.h"

class uhdHandler;
//
//	the real worker:
class uhd_streamer : public QThread {
public:
		uhd_streamer	(uhdHandler *d);
		~uhd_streamer	();
	void	stop		();

private:
	uhdHandler* m_theStick;
virtual void	run		();
	std::atomic<bool> m_stop_signal_called;
};


class	uhdInput: public deviceHandler, public Ui_uhdWidget {
Q_OBJECT
	friend class uhd_streamer;
public:
		uhdHandler	(QSettings *dabSettings);
		~uhdHandler 	();
	bool	restartReader	(int32_t freqm int skipped = 0);
	void	stopReader	();
	int32_t	getSamples	(std::complex<float> *, int32_t size);
	int32_t	Samples		();
	void	resetBuffer	();
	int16_t	maxGain		();
	int16_t	bitDepth	();
	QString	deviceName	();
	int32_t	getVFOFrequency	();
//
private:
	QSettings	*uhdSettings;
	uhd::usrp::multi_usrp::sptr m_usrp;
	uhd::rx_streamer::sptr m_rx_stream;
	uhd_streamer* m_workerHandle;
	RingBuffer<std::complex<float> > *theBuffer;
	int32_t		inputRate;
	int32_t		ringbufferSize;
private slots:
	void	setExternalGain	(int);
	void	set_fCorrection	(int);
	void	set_KhzOffset	(int);
};

