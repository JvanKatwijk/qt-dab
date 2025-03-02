#
/*
 *    Copyright (C) 2015 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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
 *
 *	Communication via network to a DAB receiver to 
 *	show the tdc data
 */

#pragma once
#include	"constants.h"
#include	<QDialog>
#include	<QSettings>
#include	<QLabel>
#include	<QMessageBox>
#include	<QTcpSocket>
#include	<QHostAddress>
#include	<QTimer>
#include	"ui_widget.h"
#include	"header-test.h"
//

class	Client:public QDialog, public Ui_widget {
Q_OBJECT
public:
		Client	(QWidget *parent = NULL);
		~Client	(void);

	bool		connected;
private	slots:
	void		wantConnect	();
	void		setConnection	();
	void		readData	();
	void		handle		(uint8_t);
	void		terminate	();
	void		timerTick	();
private:
	void		handleFrameType_0 (uint8_t *, int16_t);
	void		handleFrameType_1 (uint8_t *, int16_t);
	bool		serviceComponentFrameheaderCRC (uint8_t *data,
	                                                int16_t offset,
	                                                int16_t maxL);

	QTcpSocket	streamer;
	QTimer		*connectionTimer;
	headerTest	headertester;
	int16_t		toRead;
	int16_t		dataLength;
	uint8_t		*dataBuffer;
	uint8_t		frameType;
	int16_t		dataIndex;
};


