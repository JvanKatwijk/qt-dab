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

#include "device-handler.h"

#include	<uhd/usrp/multi_usrp.hpp>
#include	"ringbuffer.h"
#include	<QThread>
#include	<QSettings>
#include	<QFrame>
#include	<QObject>
#include	<atomic>
#include	<complex>
#include	"ui_uhd-widget.h"

class uhdHandler;
//
//	the real worker:
class uhd_streamer : public QThread  {
public:
	uhd_streamer	(uhdHandler *d);
	void stop	();

private:
	uhdHandler	* m_theStick;
virtual void 		run	();
	std::atomic<bool> m_stop_signal_called;
};


class	uhdHandler: public deviceHandler, public Ui_uhdWidget {
Q_OBJECT
	friend class uhd_streamer;
public:
		uhdHandler 	(QSettings *dabSettings);
	 	~uhdHandler 	();
	int32_t	getVFOFrequency	();
	bool	restartReader	(int32_t, int32_t);
	void	stopReader	();
	int32_t	getSamples	(std::complex<float> *, int32_t size);
	int32_t	Samples		();
	void	resetBuffer	();
	int16_t	maxGain		();
	int16_t	bitDepth	();
	QString	deviceName	();
//
private:
	QSettings	*uhdSettings;
	uhd::usrp::multi_usrp::sptr m_usrp;
	uhd::rx_streamer::sptr m_rx_stream;
	RingBuffer<std::complex<float> > *theBuffer;
	uhd_streamer* m_workerHandle;
	int32_t		inputRate;
	int32_t		ringbufferSize;
	int32_t		vfoOffset;
private slots:
	void	setExternalGain	(int);
	void	set_fCorrection	(int);
	void	set_KhzOffset	(int);
	void	set_antenna	(const QString &);
};

