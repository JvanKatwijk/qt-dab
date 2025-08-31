#
/*
 *    Copyright (C) 2016 .. 2024
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

#include  <QtNetwork>
#include  <QSettings>
#include  <QLabel>
#include  <QMessageBox>
#include  <QLineEdit>
#include  <QHostAddress>
#include  <QByteArray>
#include  <QTcpSocket>
#include  <QTimer>
#include  <QComboBox>
#include  <cstdio>
#include  "dab-constants.h"
#include  "device-handler.h"
#include  "ringbuffer.h"
#include  "ui_rtl_tcp-widget.h"

class	xml_fileWriter;
class	errorLogger;

class rtl_tcp_client :  public deviceHandler, Ui_rtl_tcp_widget {
Q_OBJECT
public:
			rtl_tcp_client	(QSettings *,
	                                 const QString &,
	                                 errorLogger *);
			~rtl_tcp_client	();
	int32_t		getRate		();
	bool		restartReader	(int32_t, int skipped = 0);
	void		stopReader	();
	int32_t		getSamples	(std::complex<float> *V, int32_t size);
	int32_t 	Samples		();
	int16_t 	bitDepth	();
	void		resetBuffer	();
	QString		deviceName	();
	bool		isFileInput	();

	QString		tunerText;
	bool		dongleInfoIn;
private:
	RingBuffer<std::complex<float>>  _I_Buffer;
	errorLogger	*theErrorLogger;
	void		sendVFO		(int32_t);
	void		sendRate	(int32_t);
	void		sendCommand	(uint8_t, int32_t);
	QLineEdit	* hostLineEdit;
	bool		isvalidRate(int32_t);
	QSettings	*remoteSettings;
	int32_t		Bitrate;
	int32_t		vfoFrequency;
	bool		connected;
	int16_t		Gain;
	double		Ppm;
	int16_t		AgcMode;
	int16_t		biasT;
	QString		ipAddress;
	QTcpSocket	toServer;
	qint64		basePort;

	float		convTable [256];

	QString		recorderVersion;
	xml_fileWriter	*xmlWriter;
 	bool            setup_xmlDump           ();
        void            close_xmlDump           ();
        std::atomic<bool> xml_dumping;

private slots:
	void		sendGain	(int);
	void		set_fCorrection	(double);
	void		readData	();
	void		wantConnect	();
	void		setDisconnect	();
	void		setBiasT	(int);
	void		setBandwidth	(int);
	void		setPort		(int);
	void		setAddress	();
	void		setAgcMode	(int);

	void		set_xmlDump	();
};

