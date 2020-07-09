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

#ifndef __PLUTO_2__
#define	__PLUTO_2__

#include	<QtNetwork>
#include        <QMessageBox>
#include        <QByteArray>
#include	<QObject>
#include	<QFrame>
#include	<QSettings>
#include	<atomic>
#include	<iio.h>
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	"device-handler.h"
#include	"ui_pluto-2.h"

/* common RX and TX streaming params */
struct stream_cfg {
        long long	bw_hz; // Analog banwidth in Hz
        long long	fs_hz; // Baseband sample rate in Hz
        long long	lo_hz; // Local oscillator frequency in Hz
        const char* rfport; // Port name
	struct	iio_channel	*lo_channel;
	struct	iio_channel	*gain_channel;
};

#ifndef	PLUTO_RATE
#define	PLUTO_RATE	2500000
#define	DAB_RATE	2048000
#define	DIVIDER		1000
#define	CONV_SIZE	(PLUTO_RATE / DIVIDER)
#endif
class	pluto_2: public deviceHandler, public Ui_pluto_2 {
Q_OBJECT
public:
			pluto_2		(QSettings *, QString &);
            		~pluto_2		();
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
	RingBuffer<std::complex<float>>	_I_Buffer;
	void			run		();
	QSettings		*plutoSettings;
	QString			recorderVersion;
	int32_t			inputRate;
	int32_t			vfoFrequency;
	std::atomic<bool>	running;
	struct	iio_device	*rx;
	struct	iio_context	*ctx;
	struct	iio_channel	*rx0_i;
	struct	iio_channel	*rx0_q;
	struct	iio_buffer	*rxbuf;
	struct	stream_cfg	rxcfg;
	bool			connected;
	std::complex<float>	convBuffer	[CONV_SIZE + 1];
	int			convIndex;
	int16_t			mapTable_int	[DAB_RATE / DIVIDER];
	float			mapTable_float	[DAB_RATE / DIVIDER];
private slots:
	void		set_gainControl		(int);
	void		set_agcControl		(int);
	void		toggle_debugButton	();
};
#endif

