#
/*
 *    Copyright (C) 2017 .. 2018
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

#ifndef __PLUTO_HANDLER__
#define	__PLUTO_HANDLER__

#include	<QtNetwork>
#include        <QMessageBox>
#include        <QLineEdit>
#include        <QHostAddress>
#include        <QByteArray>
#include	<QObject>
#include	<QFrame>
#include	<QSettings>
#include	<atomic>
#include	<iio.h>
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	"device-handler.h"
#include	"ui_pluto-widget.h"

/* common RX and TX streaming params */
struct stream_cfg {
        long long bw_hz; // Analog banwidth in Hz
        long long fs_hz; // Baseband sample rate in Hz
        long long lo_hz; // Local oscillator frequency in Hz
        const char* rfport; // Port name
};

class	plutoHandler: public deviceHandler, public Ui_plutoWidget {
Q_OBJECT
public:
			plutoHandler		(QSettings *, QString &);
            		~plutoHandler		();
	void		setVFOFrequency		(int32_t);
	int32_t		getVFOFrequency		();

	bool		restartReader		(int32_t);
	void		stopReader		();
	int32_t		getSamples		(std::complex<float> *,
	                                                          int32_t);
	int32_t		Samples			();
	void		resetBuffer		();
	int16_t		bitDepth		();

	void		show			();
	void		hide			();
	bool		isHidden		();
	QString		deviceName		();
private:
	QFrame			myFrame;
	QLineEdit		hostLineEdit;
	RingBuffer<std::complex<float>>	_I_Buffer;
	void			run		();
	QSettings		*plutoSettings;
	QString			recorderVersion;
	int32_t			inputRate;
	int32_t			vfoFrequency;
	std::atomic<bool>	running;
	bool			agcMode;
	struct	iio_device	*rx;
	struct	iio_context	*ctx;
	struct	iio_channel	*rx0_i;
	struct	iio_channel	*rx0_q;
	struct	iio_buffer	*rxbuf;
	struct	stream_cfg	rxcfg;
	bool			connected;
private slots:
	void		set_gainControl	(int);
	void		set_agcControl	(int);
//	void		set_connection	();
};
#endif

