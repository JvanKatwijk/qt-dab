#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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

#ifndef	__RTL_TCP_CLIENT__
#define	__RTL_TCP_CLIENT__

#include	<QtNetwork>
#include	<QSettings>
#include	<QLabel>
#include	<QMessageBox>
#include	<QLineEdit>
#include	<QHostAddress>
#include	<QByteArray>
#include	<QTcpSocket>
#include	<QTimer>
#include	<QComboBox>
#include	<stdio.h>
#include	"dab-constants.h"
#include	"virtual-input.h"
#include	"ringbuffer.h"
#include	"ui_rtl_tcp-widget.h"

class	rtl_tcp_client: public virtualInput, Ui_rtl_tcp_widget {
Q_OBJECT
public:
			rtl_tcp_client (QSettings *);
			~rtl_tcp_client	(void);
	int32_t		getRate		(void);
	int32_t		defaultFrequency	(void);
	void		setVFOFrequency	(int32_t);
	int32_t		getVFOFrequency	(void);
	bool		restartReader	(void);
	void		stopReader	(void);
	int32_t		getSamples	(std::complex<float> *V, int32_t size);
	int32_t		Samples		(void);
	int16_t		bitDepth	(void);
private slots:
	void		sendGain	(int);
	void		set_Offset	(int);
	void		set_fCorrection	(int);
	void		readData	(void);
	void		setConnection	(void);
	void		wantConnect	(void);
	void		setDisconnect	(void);
private:
	void		sendVFO		(int32_t);
	void		sendRate	(int32_t);
	void		setGainMode (int32_t gainMode);
	void		sendCommand	(uint8_t, int32_t);
	QLineEdit	*hostLineEdit;
	bool		isvalidRate	(int32_t);
	QSettings	*remoteSettings;
	QFrame		*theFrame;
	int32_t		theRate;
	int32_t		vfoFrequency;
	RingBuffer<uint8_t>	*theBuffer;
	bool		connected;
	int16_t		theGain;
	int16_t		thePpm;
	QHostAddress	serverAddress;
	QTcpSocket	toServer;
	qint64		basePort;
	bool		dumping;
	FILE		*dumpfilePointer;
};

#endif

