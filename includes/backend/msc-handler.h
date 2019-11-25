#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB
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
#ifndef	__MSC_HANDLER__
#define	__MSC_HANDLER__

#include	<QThread>
#include	<QWaitCondition>
#include	<QMutex>
#include	<QSemaphore>
#include	<atomic>
#include	<cstdio>
#include	<cstdint>
#include	<cstdio>
#include	<vector>
#include	"dab-constants.h"
#include	"dab-params.h"
#include        "fft-handler.h"
#include        "ringbuffer.h"
#include        "phasetable.h"
#include        "freq-interleaver.h"

class	RadioInterface;
class	Backend;

class mscHandler: public QThread  {
public:
			mscHandler		(RadioInterface *,
	                                         uint8_t,
	                                         QString,
	                                         RingBuffer<uint8_t> *);
			~mscHandler();
	void		processBlock_0		(DSPCOMPLEX *);
	void		process_Msc		(DSPCOMPLEX *, int);
	void		set_Channel		(descriptorType *,
	                                           RingBuffer<int16_t> *,
	                                           RingBuffer<uint8_t> *);
	void		unset_Channel		(const QString &);
//
	void		stop			();
	void		reset			();
private:
	void		process_mscBlock	(std::vector<int16_t>, int16_t);
	std::vector<int>	blockTable;
	int		T_u;
	RadioInterface	*myRadioInterface;
	RingBuffer<uint8_t>	*frameBuffer;
	QString		picturesPath;
	dabParams	params;
	fftHandler      my_fftHandler;
	DSPCOMPLEX	*fft_buffer;
	std::vector<DSPCOMPLEX>     phaseReference;

        interLeaver     myMapper;
	QMutex		locker;
	std::vector<Backend *>theBackends;
	std::vector<int16_t> cifVector;
	std::atomic<bool> work_to_be_done;
	int16_t		BitsperBlock;
	int16_t		numberofblocksperCIF;
	void            run		();
        std::atomic<bool>       running;
        std::vector<std::vector<DSPCOMPLEX> > command;
	QSemaphore	freeSlots;
	QSemaphore	usedSlots;
	int		nextIn;
	int		nextOut;
	int		nrBlocks;
};

#endif


