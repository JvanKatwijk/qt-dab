#
/*
 *    Copyright (C) 2026
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB 
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
#include	"ringbuffer.h"
#include	<complex>
#include	<vector>
#include	<samplerate.h>

class soapyConverter: public QObject {
Q_OBJECT
public:
		soapyConverter (RingBuffer<std::complex<float>> *outBuffer);
		~soapyConverter	();
	void	setup		(int, int);
	void	add		(std::complex<float> *, uint32_t size);
	void	reset		();
private:
	uint32_t	inputRate;
	uint32_t	targetRate;
	SRC_STATE       *converter;
        SRC_DATA        src_data;
        uint32_t	inputLimit;
        uint32_t	outputLimit;
	uint32_t	inp;
	void		copyDirect	(std::complex<float> *, int);
	void		convert		();
	int		dropCount;
        std::vector<float> inBuffer;
        std::vector<float> uitBuffer;
	RingBuffer<std::complex<float>> *outBuffer;
signals:
	void		reportStatus	(const QString &);
};

