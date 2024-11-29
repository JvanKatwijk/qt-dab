#
/*
 *    Copyright (C) 2016 .. 2023
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
 */
#pragma once

#include	"dab-constants.h"
#include	<QObject>
#include	<vector>
#include	<cstdint>
#include	"ringbuffer.h"
#include	"phasetable.h"
#include	"freq-interleaver.h"
#include	"dab-params.h"
#include	"fft-handler.h"

class	RadioInterface;

#define	SHOW_RAW	0100
#define	SHOW_DECODED	0101

#ifndef	M_PI_2
#define	M_PI_2	(M_PI / 2)
#endif
#ifndef	M_PI_4
#define	M_PI_4	(M_PI / 4)
#endif
class	ofdmDecoder: public QObject {
Q_OBJECT
public:
		ofdmDecoder		(RadioInterface *,
	                                 uint8_t,
	                                 int16_t,
	                                 RingBuffer<float> *devBuffer,
	                                 RingBuffer<Complex> * iqBuffer = nullptr);
		~ofdmDecoder		();
	void	processBlock_0		(std::vector<Complex>, bool);
	void	decode			(std::vector<Complex> &,
	                                 int32_t n,
	                                 std::vector<int16_t> &);
	void	stop			();
	void	reset			();
	void	handle_iqSelector	();
	void	handle_decoderSelector	(int);
private:
	RadioInterface	*myRadioInterface;
	dabParams	params;
	interLeaver     myMapper;
	fftHandler	fft;
	RingBuffer<float>	*devBuffer;
	RingBuffer<Complex>	*iqBuffer;
	float		computeQuality		(Complex *);
        float		compute_timeOffset      (Complex *,
                                                 Complex *);
        float		compute_clockOffset     (Complex *,
                                                 Complex *);
        float		compute_frequencyOffset (Complex *,
                                                 Complex *);
	int32_t		T_s;
	int32_t		T_u;
	int32_t		T_g;
	int32_t		nrBlocks;
	int32_t		carriers;
	std::vector<Complex>	phaseReference;
	std::vector<int16_t>	ibits;
	std::vector<Complex>	carrierCenters;
	std::vector<Complex>	conjVector;
	std::vector<Complex>	fft_buffer;
	
	std::vector<float>	offsetVector;
	std::vector<float>	amplitudeVector;
        std::vector<float>	avgSigmaSqPerBin;
	std::vector<float>	avgPowerPerBin;
	std::vector<float>	avgNullPower;

	float		sum;
//	phaseTable	*phasetable;
	int		iqSelector;
	int		decoder;
	int		repetitionCounter;
signals:
	void		showIQ		(int);
	void		show_quality	(float, float, float);
	void		show_stdDev	(int);
};


