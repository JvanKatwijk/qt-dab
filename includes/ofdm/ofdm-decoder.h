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
#ifndef	__OFDM_DECODER__
#define	__OFDM_DECODER__

#include	"dab-constants.h"
#include	<QObject>
#include	<vector>
#include	<cstdint>
#include	"fft-handler.h"
#include	"ringbuffer.h"
#include	"phasetable.h"
#include	"freq-interleaver.h"
#include	"dab-params.h"

class	RadioInterface;

class	ofdmDecoder: public QObject {
Q_OBJECT
public:
		ofdmDecoder		(RadioInterface *,
	                                 uint8_t,
	                                 int16_t,
	                                 RingBuffer<std::complex<float>> * iqBuffer = nullptr);
		~ofdmDecoder();
	void	processBlock_0		(std::vector<std::complex<float> >);
	void	decode			(std::vector<std::complex<float> >,
	                                 int32_t n, int16_t *);
	int16_t	get_snr			(std::complex<float> *);
	void	stop();
	void	reset();
private:
	RadioInterface	*myRadioInterface;
	dabParams	params;
	fftHandler	my_fftHandler;
	interLeaver     myMapper;

	RingBuffer<std::complex<float>> *iqBuffer;
	float		computeQuality	(std::complex<float> *);
	int32_t		T_s;
	int32_t		T_u;
	int32_t		T_g;
	int32_t		nrBlocks;
	int32_t		carriers;
	int16_t		getMiddle();
	std::vector<complex<float>>	phaseReference;
	std::vector<int16_t>		ibits;
	std::complex<float>	*fft_buffer;
	phaseTable	*phasetable;
	int32_t		blockIndex;
	int16_t		snrCount;
	int16_t		snr;
	int16_t		maxSignal;

signals:
	void		show_snr	(int);
	void		showIQ		(int);
	void		showQuality	(float);
};

#endif

