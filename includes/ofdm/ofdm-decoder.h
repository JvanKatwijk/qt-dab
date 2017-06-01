#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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
#include	<QThread>
#include	<QWaitCondition>
#include	<QMutex>
#include	<QSemaphore>
#include	<stdint.h>
#include	"fft.h"
#include	"ringbuffer.h"
#include	"phasetable.h"
#include	"freq-interleaver.h"
#include	"dab-params.h"

class	RadioInterface;
class	ficHandler;
class	mscHandler;

class	ofdmDecoder: public QThread {
Q_OBJECT
public:
		ofdmDecoder		(RadioInterface *,
	                                 uint8_t,
#ifdef	HAVE_SPECTRUM
	                                 RingBuffer<DSPCOMPLEX> *,
#endif
	                                 int16_t,
	                                 ficHandler	*,
	                                 mscHandler	*);
		~ofdmDecoder		(void);
	void	processBlock_0		(DSPCOMPLEX *);
	void	decodeFICblock		(DSPCOMPLEX *, int32_t n);
	void	decodeMscblock		(DSPCOMPLEX *, int32_t n);
	int16_t	get_snr			(DSPCOMPLEX *);
	void	stop			(void);
private:
	RadioInterface	*myRadioInterface;
	dabParams	params;
#ifdef	HAVE_SPECTRUM
	RingBuffer<DSPCOMPLEX> *iqBuffer;
#ifdef	__QUALITY
	float		computeQuality	(DSPCOMPLEX *);
#endif
#endif
	ficHandler	*my_ficHandler;
	mscHandler	*my_mscHandler;
	void		run		(void);
	bool		running;
	DSPCOMPLEX	**command;
	int16_t		amount;
	int16_t		currentBlock;
	void		processBlock_0		(void);
	void		decodeFICblock		(int32_t n);
	void		decodeMscblock		(int32_t n);
	QSemaphore	bufferSpace;
	QWaitCondition	commandHandler;
	QMutex		helper;
	int32_t		T_s;
	int32_t		T_u;
	int32_t		T_g;
	int32_t		nrBlocks;
	int32_t		carriers;
	int16_t		getMiddle	(void);
	DSPCOMPLEX	*phaseReference;
	common_fft	*fft_handler;
	DSPCOMPLEX	*fft_buffer;
	interLeaver	myMapper;
	phaseTable	*phasetable;
	int32_t		blockIndex;
	int16_t		*ibits;
	int16_t		snrCount;
	int16_t		snr;
	int16_t		maxSignal;
signals:
	void		show_snr	(int);
	void		showIQ		(int);
	void		showQuality	(float);
};

#endif


