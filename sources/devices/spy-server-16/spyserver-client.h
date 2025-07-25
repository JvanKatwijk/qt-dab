#
/*
 *    Copyright (C) 2016 .. 2023
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

#pragma once

#include	<QObject>
#include	<QSettings>
#include	<QTimer>
#include	<atomic>
#include	<QLabel>
#include	<QMessageBox>
#include	<QLineEdit>
#include	<QHostAddress>
#include	<QByteArray>
#include	<QScopedPointer>
#include	<cstdio>
#include	"dab-constants.h"
#include	"device-handler.h"
#include	"ringbuffer.h"
#include	"ui_spyserver-widget.h"
#include	"spy-handler.h"

class	spyServer_client: //public QObject,
	                  public deviceHandler, Ui_spyServer_widget {
Q_OBJECT
public:
			spyServer_client	(QSettings *);
			~spyServer_client	();
	int32_t		getRate		();
	bool		restartReader	(int32_t, int skipped = 0);
	void		stopReader	();
	int32_t		getSamples	(std::complex<float> *V, int32_t size);
	int32_t		Samples		();
	int16_t		bitDepth	();
	void		connect_on	();
struct {
	int		gain;
	qint64		basePort;
	uint32_t	sample_rate;
	float		resample_ratio;
	int		desired_decim_stage;
	int		resample_quality;
	int		batchSize;
	int		sample_bits;
	int		auto_gain;
} settings;

private slots:
	void		setConnection	();
	void		wantConnect	();
	void		setGain		(int);
	void		handle_autogain	(int);
	void		handle_checkTimer	();
	void		set_portNumber	(int);
public slots:
	void		data_ready	();
private:
	RingBuffer<std::complex<float>>	_I_Buffer;
	RingBuffer<int16_t>	tmpBuffer;
	QTimer		checkTimer;
	QLineEdit	hostLineEdit;
	QScopedPointer<spyHandler> theServer;
	bool		isvalidRate	(int32_t);
	QSettings	*spyServer_settings;
	int32_t		theRate;
	bool		connected;
	bool		running;
	QHostAddress	serverAddress;
	qint64		basePort;
	bool		dumping;
	FILE		*dumpfilePointer;
	std::atomic<bool>	onConnect;
	bool		timedOut;
	int16_t         convBufferSize;
        int16_t         convIndex;
        std::vector <std::complex<float> >      convBuffer;
        int16_t         mapTable_int   [SAMPLERATE / 1000];
        float           mapTable_float [SAMPLERATE / 1000];
	int		selectedRate;
};


