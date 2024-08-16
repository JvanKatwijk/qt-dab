#
/*
 *    Copyright (C)  2016 .. 2023
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
#include	"wavWriter.h"
#include	"dab-constants.h"
#include	<cstdio>
#include	<samplerate.h>
#include	<mutex>
#include	<QObject>
#include	"newconverter.h"
#include	"ringbuffer.h"

class	RadioInterface;

	class	converter_48000: public QObject {
Q_OBJECT
public:
			converter_48000		(RadioInterface *);
			~converter_48000	();
	int		convert			(std::complex<int16_t>*,
	                                          int32_t, int, 
	                                          std::vector<float> &);
	                                           
	void		start_audioDump		(const QString &);
	void		stop_audioDump		();
private:
	int		convert_16000		(std::complex<int16_t> *,
	                                         int, std::vector<float> &);
	int		convert_24000		(std::complex<int16_t> *,
	                                         int, std::vector<float> &);
	int		convert_32000		(std::complex<int16_t> *,
	                                         int, std::vector<float> &);
	int		convert_48000		(std::complex<int16_t> *,
	                                         int, std::vector<float> &);
	wavWriter	theWriter;
	newConverter	mapper_16;
	newConverter	mapper_24;
	newConverter	mapper_32;
	std::mutex	locker;
	void		dump			(const Complex *, int);
	void		dump			(const std::complex<int16_t> *, int);
//
//	For thermo
	void		evaluatePeakLevel	(const Complex);
	void		eval			(Complex *, int);

	int32_t		peakLevelCurSampleCnt;  
        int32_t		peakLevelSampleMax;
        float		absPeakLeft;
        float		absPeakRight;

	int		repetitionCounter;
signals:
	void		showPeakLevel		(float, float);
};

