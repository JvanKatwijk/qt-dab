#
/*
 *    Copyright (C) 2015
 *    Sebastian Held <sebastian.held@imst.de>
 *
 *    This file is part of Qt-DAB
 *    Many of the ideas as implemented in SDR-J are derived from
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
 */
#ifndef	__UHDINPUT
#define	__UHDINPUT

#include	"device-handler.h"

#include	<QThread>
#include	<QSettings>
#include	<QObject>
#include	"dab-constants.h"
#include	<uhd/usrp/multi_usrp.hpp>
#include	"ringbuffer.h"
#include	"ui_uhd-widget.h"

class uhdHandler;
//
//	the real worker:
class uhd_streamer : public QThread {
public:
	uhd_streamer	(uhdHandler *d);
	void stop	();

private:
	uhdHandler* m_theStick;
	virtual void run();
	volatile bool m_stop_signal_called;
};

class	uhdHandler: public QObject, public deviceHandler, public Ui_uhdWidget {
Q_OBJECT
	friend class uhd_streamer;
public:
		uhdHandler	(QSettings *dabSettings);
	 	~uhdHandler 	();

	bool	restartReader	(int32_t freq);
	void	stopReader	();
	int32_t	getSamples	(std::complex<float> *, int32_t size);
	int32_t	Samples		();
	void	resetBuffer	();
	int16_t	bitDepth	();
	QString	deviceName	();
//
private:
	int16_t		maxGain		();
	QSettings	*uhdSettings;

	uhd::usrp::multi_usrp::sptr m_usrp;
	uhd::rx_streamer::sptr m_rx_stream;
	RingBuffer<std::complex<float>> *theBuffer;
	uhd_streamer*	m_workerHandle;
	int32_t		inputRate;
	int32_t		ringbufferSize;
private slots:
	void		handle_externalGain	(int);
	void		handle_ppmOffset	(int);
};
#endif

