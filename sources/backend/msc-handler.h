#
/*
 *    Copyright (C) 2013 .. 2024
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
#
#pragma once

#ifdef __MSC_THREAD__
#include	<QThread>
#include	<QWaitCondition>
#include	<QSemaphore>
#include	"fft-handler.h"
#else
#include	<QObject>
#endif
#include	<QMutex>
#include	<atomic>
#include	<cstdio>
#include	<cstdint>
#include	<cstdio>
#include	<vector>
#include	"dab-constants.h"
#include	"dab-params.h"
#include        "ringbuffer.h"
#include        "phasetable.h"
#include        "freq-interleaver.h"


class	RadioInterface;
class	Backend;
class	logger;

#ifdef	__MSC_THREAD__
class mscHandler: public QThread  {
#else
class	mscHandler: public QObject {
#endif
Q_OBJECT
public:
			mscHandler		(RadioInterface *,
	                                         uint8_t,
	                                         RingBuffer<uint8_t> *,
	                                         logger		*,
	                                         uint8_t);
			~mscHandler		();
	void		processBlock_0		(Complex *);
	void		processMsc		(std::vector<Complex> &,
	                                                  int, int);
	void		processMscBlock		(std::vector<int16_t> &,
	                                                  int16_t);
	bool		startBackend		(descriptorType &,
	                                         RingBuffer<std::complex<int16_t>> *,
	                                         RingBuffer<uint8_t> *,
	                                         FILE *, int);
	void		stopBackend		(const QString &, int, int);
	void		resetChannel		();
	void		resetBuffers		();
	bool		serviceRuns		(uint32_t SId, uint16_t subChId);
private:

	dabParams	params;
        interLeaver     myMapper;
	RadioInterface		*myRadioInterface;
	RingBuffer<uint8_t>	*frameBuffer;
	logger		*theLogger;
	uint8_t		cpuSupport;
#ifdef	__MSC_THREAD__
	fftHandler	fft;
        QSemaphore      bufferSpace;
#endif

	RingBuffer<uint8_t>	*dataBuffer;
	QMutex		locker;
	bool		audioService;
	std::vector<Backend*>theBackends;
	std::vector<int16_t> cifVector;
	int16_t		cifCount;
	int16_t		blkCount;
	int16_t		BitsperBlock;
	std::vector<int16_t> softBits;

	int16_t		numberofblocksperCIF;
	int16_t		blockCount;
        void            processMsc	(int32_t n);
        QMutex          helper;
	int		nrBlocks;
#ifdef	__MSC_THREAD__
	std::vector<Complex>     phaseReference;
        void            processBlock_0	();
        std::vector<std::vector<Complex > > command;
        int16_t         amount;
	void            run();
        QWaitCondition  commandHandler;
        std::atomic<bool>       running;
#endif
signals:
	void		nrServices	(int);
};

