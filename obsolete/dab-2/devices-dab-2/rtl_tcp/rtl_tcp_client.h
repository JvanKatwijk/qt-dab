#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of dab-2
 *
 *    dab-2 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    dab-2 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dab-2; if not, write to the Free Software
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
#include	<cstdio>
#include	"dab-constants.h"
#include	"device-handler.h"
#include	"ringbuffer.h"
#include	"ui_rtl_tcp-widget.h"

class	RadioInterface;
class	dabProcessor;

class	rtl_tcp_client: public deviceHandler, Ui_rtl_tcp_widget {
Q_OBJECT
public:
			rtl_tcp_client	(RadioInterface	*,
	                                 QSettings	*,
	                                 dabProcessor	*);
			~rtl_tcp_client	();
	bool		restartReader	(int32_t);
	void		stopReader	();
	int32_t		getVFOFrequency	();
	void		resetBuffer	();
	int16_t		bitDepth	();
	void		show		();
	void		hide		();
	bool		isHidden	();
	QString		deviceName	();
private slots:
	void		sendGain	(int);
	void		set_Offset	(int);
	void		set_fCorrection	(int);
	void		readData	();
	void		setConnection	();
	void		wantConnect	();
	void		setDisconnect	();
private:
	dabProcessor	*base;
	void		handle_Value	(int, float, float);
	QFrame		myFrame;
	void		sendVFO		(int32_t);
	void		sendRate	(int32_t);
	void		setGainMode	(int32_t gainMode);
	void		sendCommand	(uint8_t, int32_t);
	QLineEdit	*hostLineEdit;
	bool		isvalidRate	(int32_t);
	QSettings	*remoteSettings;
	int32_t		theRate;
	int32_t		vfoFrequency;
	int		totalOffset;
	RingBuffer<std::complex<float>>	*_I_Buffer;
	bool		connected;
	int16_t		theGain;
	int16_t		thePpm;
	int		vfoOffset;
	QHostAddress	serverAddress;
	QTcpSocket	toServer;
	qint64		basePort;
	bool		dumping;
	FILE		*dumpfilePointer;
};

#endif

