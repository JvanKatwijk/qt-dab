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

#include	<QMutex>
#include	<stdio.h>
#include	<stdint.h>
#include	<stdio.h>
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	"dab-params.h"

class	RadioInterface;
class	dabVirtual;

class mscHandler  {
public:
		mscHandler		(RadioInterface *,
	                                 uint8_t,
	                                 RingBuffer<int16_t> *,
	                                 QString,
	                                 bool);
		~mscHandler		(void);
	void	process_mscBlock	(int16_t *, int16_t);
	void	set_audioChannel	(audiodata	*);
	void    set_dataChannel         (packetdata	*);
	void	stopProcessing		(void);
	void	stopHandler		(void);
private:
	RadioInterface	*myRadioInterface;
	RingBuffer<int16_t>	*audioBuffer;
	QString		picturesPath;
	dabParams	params;
	bool		show_crcErrors;
	QMutex		locker;
	bool		audioService;
	dabVirtual	*dabHandler;
	int16_t		*cifVector;
	int16_t		cifCount;
	int16_t		blkCount;
	bool		work_to_be_done;
	int16_t		packetAddress;
	int16_t		appType;
	int16_t		ASCTy;
	int16_t		DSCTy;
	int16_t		startAddr;
	int16_t		Length;
	bool		shortForm;
	int16_t		protLevel;
	uint8_t		DGflag;
	int16_t		bitRate;
	int16_t		language;
	int16_t		type;
	int16_t		FEC_scheme;
	int8_t		dabModus;
	int16_t		BitsperBlock;
	int16_t		numberofblocksperCIF;
	int16_t		blockCount;
};

#endif


