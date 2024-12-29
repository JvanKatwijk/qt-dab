#
/*
 *    Copyright (C) 2015
 *    Sebastian Held <sebastian.held@imst.de>
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

#include  <QThread>
#include  <QSettings>
#include  <QFrame>
#include  <QObject>
#include  <uhd/usrp/multi_usrp.hpp>
#include  "ui_uhd-widget.h"
#include  "device-handler.h"
#include  "ringbuffer.h"

class uhdHandler;

class uhd_streamer : public QThread {
public:
		uhd_streamer	(uhdHandler *);
		~uhd_streamer	();
	void	stop		();

private:
	uhdHandler	*theStick;
	void	run		();
	std::atomic<bool> stop_called;
};

class uhdHandler : public deviceHandler, public Ui_uhdWidget {
Q_OBJECT
	friend class uhd_streamer;
public:
		uhdHandler	(QSettings * dabSettings);
		~uhdHandler	();

	bool	restartReader	(int32_t freq);
	void	stopReader	();
	int32_t getSamples	(std::complex<float> *, int32_t size);
	int32_t Samples		();
	void	resetBuffer	();
	int16_t bitDepth	();
	QString deviceName	();

private:
	QSettings	*uhdSettings;
	uhd::usrp::multi_usrp::sptr m_usrp;
	uhd::rx_streamer::sptr rx_stream;
	uhd_streamer * m_workerHandle = nullptr;
	RingBuffer<std::complex<float>> * theBuffer;
	int32_t		ringBufferSize;
	int16_t		maxGain		();
	int		inputRate;

private slots:
	void	setExternalGain	(int);
	void 	handle_ant_selector (const QString &);
};

