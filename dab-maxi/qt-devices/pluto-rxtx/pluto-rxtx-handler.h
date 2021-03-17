#
/*
 *    Copyright (C) 2020
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

#ifndef __PLUTO_RXTX_HANDLER__
#define	__PLUTO_RXTX_HANDLER__

#include	<QObject>
#include        <QMessageBox>
#include        <QByteArray>
#include	<QFrame>
#include	<QSettings>
#include	<atomic>
#include	<thread>
#include	<iio.h>
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	"device-handler.h"
#include	"ui_pluto-rxtx-widget.h"

#include	"up-filter.h"

class	xml_fileWriter;

#ifndef	RX_RATE
#define	RX_RATE		2112000
#define	DAB_RATE	2048000
#define	DIVIDER		1000
#define	CONV_SIZE	(RX_RATE / DIVIDER)
#define	FM_RATE		2112000
#endif


struct stream_cfg {
        long long bw_hz;
        long long fs_hz;
        long long lo_hz;
        const char *rfport;
};

class	plutoHandler: public QObject,
	              public deviceHandler, public Ui_plutorxtxWidget {
Q_OBJECT
public:
			plutoHandler		(QSettings *,
	                                         QString &, int fmFreq = 0);
            		~plutoHandler		();
	void		setVFOFrequency		(int32_t);
	int32_t		getVFOFrequency		();
	bool		restartReader		(int32_t);
	void		stopReader		();
	int32_t		getSamples		(std::complex<float> *,
	                                                          int32_t);
	int32_t		Samples			();
	void		sendSample		(std::complex<float>);
	void		startTransmitter	(int);
	void		stopTransmitter		();
	void		resetBuffer		();
	int16_t		bitDepth		();

	void		show			();
	void		hide			();
	bool		isHidden		();
	QString		deviceName		();
private:
	QFrame			myFrame;
	RingBuffer<std::complex<float>>	_I_Buffer;
	RingBuffer<std::complex<float>>	_O_Buffer;
	upFilter		theFilter;
	int			fmFrequency;
	QSettings		*plutoSettings;
	QString			recorderVersion;
	FILE			*xmlDumper;
	xml_fileWriter		*xmlWriter;
	bool			setup_xmlDump	();
	void			close_xmlDump	();
	std::atomic<bool>	dumping;
	bool			filterOn;
	void			run_receiver	();
	void			run_transmitter	();
	int32_t			inputRate;
	int32_t			vfoFrequency;
	std::atomic<bool>	running;
	std::atomic<bool>	transmitting;
	bool			debugFlag;
	std::thread		threadHandle_r;
	std::thread		threadHandle_t;
//      configuration items

	struct  iio_device      *rx;
        struct  iio_device      *tx;
        struct  iio_context     *ctx;
        struct  iio_channel     *rx0_i;
        struct  iio_channel     *rx0_q;
        struct  iio_channel     *tx0_i;
        struct  iio_channel     *tx0_q;
        struct  iio_buffer      *rxbuf;
        struct  iio_buffer      *txbuf;
        struct  stream_cfg      rx_cfg;
        struct  stream_cfg      tx_cfg;

	bool			connected;
	std::complex<float>	convBuffer	[CONV_SIZE + 1];
	int			convIndex;
	int16_t			mapTable_int	[DAB_RATE / DIVIDER];
	float			mapTable_float	[DAB_RATE / DIVIDER];

	void			record_gainSettings	(int);
	void			update_gainSettings	(int);
	bool			save_gainSettings;
//
signals:
	void		new_gainValue		(int);
	void		new_agcValue		(bool);
private slots:
	void		set_gainControl		(int);
	void		set_agcControl		(int);
	void		toggle_debugButton	();
	void		set_filter		();
	void		set_xmlDump		();
};
#endif

