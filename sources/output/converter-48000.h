#
/*
 *    Copyright (C)  2016 .. 2025
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB.
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
#include	"riff-writer.h"
#include	"dab-constants.h"
#include	<cstdio>
#include	<mutex>
#include	<QObject>
#include	"ringbuffer.h"
#include	"fir-filters.h"
#include	<vector>
#include	<complex>

class	RadioInterface;

class	converter_48000: public QObject {
Q_OBJECT
public:
			converter_48000		(RadioInterface *);
			~converter_48000	();
	int		convert			(complex16*,
	                                          int32_t, int, 
	                                          std::vector<float> &);
	                                           
	void		start_audioDump		(const QString &);
	void		stop_audioDump		();
private:
	int		convert_16000		(complex16 *,
	                                         int, std::vector<float> &);
	int		convert_24000		(complex16 *,
	                                         int, std::vector<float> &);
	int		convert_32000		(complex16 *,
	                                         int, std::vector<float> &);
	int		convert_48000		(complex16 *,
	                                         int, std::vector<float> &);
	riffWriter	theWriter;
	std::vector<std::complex<float>>	buffer_32_96;
	LowPassFIR	filter_16_48;
	LowPassFIR	filter_24_48;
	LowPassFIR	filter_32_96;
	std::mutex	locker;
	void		dump			(const float *, int);
	void		dump			(const Complex *, int);
	void		dump			(const complex16 *, int);
};

