#
/*
 *    Copyright (C) 2017 .. 2024
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
 */
#pragma once

#include	<Qt>
#include	<QtGlobal>
#include	<QIODevice>
#include	<QObject>
#include	"dab-constants.h"
#include	"ringbuffer.h"
class	RadioInterface;

//
//	Note: this class is NOT refefrences in the Qt5 version, only in
//	the Qt6 version
class Qt_AudioDevice : public QIODevice {
Q_OBJECT
public:
		Qt_AudioDevice	(RadioInterface *,
	                         RingBuffer<char> *, QObject *parent = nullptr);
		~Qt_AudioDevice	();

	void	start		();
	void	stop		();
	void	samplesMissed	(int &, int &);
	qint64	readData	(char *data, qint64 maxlen);
	qint64	writeData	(const char *data, qint64 len);

private:
	RingBuffer<char> *Buffer;
	int	totalBytes_l;
	int	missedBytes_l;
	std::atomic<bool> running;
};

