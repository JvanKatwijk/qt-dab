#
/*
 * ## Copyright
 *
 * dabtools is written by Dave Chapman <dave@dchapman.com> 
 *
 * Large parts of the code are copied verbatim (or with trivial
 * modifications) from David Crawley's OpenDAB and hence retain his
 * copyright.
 *
 *	Parts of this software are copied verbatim (or with trivial
 *	Modifications) from David Chapman's dabtools and hence retain
 *	his copyright. In particular, the crc, descramble and init_eti
 *	functions are - apart from naming - a verbatim copy. Thanks
 *	for the nice work
 *
 *    Copyright (C) 2016, 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the eti library
 *    eti library is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    eti library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with eti library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * 	eti generator
 */

#pragma once

#include	<stdio.h>
#include	<stdint.h>
#include	<stdio.h>
#include	<vector>
#include	<atomic>
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	"fic-handler.h"
#include	"dab-params.h"
#include	"protection.h"

class	RadioInterface;

class	parameter;

//
//	to build a simple cache for the protection handlers
typedef	struct {
	bool    uepFlag;
        int     bitRate;
        int     protLevel;
	uint8_t	*dispersionVector;
        protection *theDeconvolver;
} protDesc;

class etiGenerator {
public:
                etiGenerator		(uint8_t  dabMode,
                                         ficHandler     *my_ficHandler);
		~etiGenerator		();
	void	newFrame		();
	void	processBlock		(std::vector<int16_t> &fbits,
	                                                        int blkno);
	void	reset			();
	bool	start_etiGenerator	(const QString &);
	void	stop_etiGenerator	();
private:
	ficHandler	*my_ficHandler;
	FILE		*etiFile;
	dabParams	params;
	bool		running;
	int16_t		index_Out;
	int		Minor;
	int16_t		CIFCount_hi;
	int16_t		CIFCount_lo;
	std::atomic <int16_t>	amount;
	int16_t		BitsperBlock;
	int16_t		numberofblocksperCIF;
	uint8_t		fibBits			[4 * 768];
	
	int32_t		init_eti		(uint8_t *,
	                                         int16_t, int16_t, int16_t);
	int32_t		process_CIF		(int16_t *,
	                                         uint8_t *, int32_t);
	void		process_subCh		(int, parameter *,
	                                         protection *prot, uint8_t *);

	void		postProcess		(uint8_t *, int32_t);
};



