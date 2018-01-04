#
/*
 *    Copyright (C) 2016
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

#include	"fic-handler.h"
#include	"radio.h"
#include	"protTables.h"
//
//	The 3072 bits of the serial motherword shall be split into
//	24 blocks of 128 bits each.
//	The first 21 blocks shall be subjected to
//	puncturing (per 32 bits) according to PI_16
//	The next three blocks shall be subjected to 
//	puncturing (per 32 bits) according to PI_15
//	The last 24 bits shall be subjected to puncturing
//	according to the table 8

/**
  *	\class ficHandler
  * 	We get in - through process_ficBlock - the FIC data
  * 	in units of 768 bits.
  * 	We follow the standard and apply convolution decoding and
  * 	puncturing.
  *	The data is sent through to the fib processor
  */

		ficHandler::ficHandler (RadioInterface *mr,
	                                uint8_t dabMode):
	                                    params (dabMode),
	                                    fib_processor (mr),
	                                    myViterbi (768, true) {
int16_t	i, j, k;
int	local	= 0;

	index		= 0;
	BitsperBlock	= 2 * params. get_carriers ();
	ficno		= 0;
	ficBlocks	= 0;
	ficMissed	= 0;
	ficRatio	= 0;

	memset (shiftRegister, 1, 9);

	for (i = 0; i < 768; i ++) {
	   PRBS [i] = shiftRegister [8] ^ shiftRegister [4];
	   for (j = 8; j > 0; j --)
	      shiftRegister [j] = shiftRegister [j - 1];

	   shiftRegister [0] = PRBS [i];
	}
//
//	Since the depuncturing is the same throughout all calls
//	(even through all instances, so we could create a static
//	table), we make an punctureTable that contains the indices of
//	the ofdmInput table
	memset (punctureTable, 0, (3072 + 24) * sizeof (uint8_t));
	for (i = 0; i < 21; i ++) {
	   for (k = 0; k < 32 * 4; k ++) {
	      if (get_PCodes (16 - 1) [k % 32] != 0)  
	         punctureTable [local] = true;
	      local ++;
	   }
	}
/**
  *	In the second step
  *	we have 3 blocks with puncturing according to PI_15
  *	each 128 bit block contains 4 subblocks of 32 bits
  *	on which the given puncturing is applied
  */
	for (i = 0; i < 3; i ++) {
	   for (k = 0; k < 32 * 4; k ++) {
	      if (get_PCodes (15 - 1) [k % 32] != 0)  
	         punctureTable [local] = true;
	      local ++;
	   }
	}

/**
  *	we have a final block of 24 bits  with puncturing according to PI_X
  *	This block constitues the 6 * 4 bits of the register itself.
  */
	for (k = 0; k < 24; k ++) {
	   if (get_PCodes (8 - 1) [k] != 0) 
	      punctureTable [local] = true;
	   local ++;
	}

	connect (this, SIGNAL (show_ficSuccess (bool)),
	         mr, SLOT (show_ficSuccess (bool)));
}

		ficHandler::~ficHandler (void) {
}

	
/**
  *	\brief process_ficBlock
  *	The number of bits to be processed per incoming block
  *	is 2 * p -> K, which still depends on the Mode.
  *	for Mode I it is 2 * 1536, for Mode II, it is 2 * 384,
  *	for Mode III it is 192, Mode IV gives 2 * 768.
  *	for Mode II we will get the 2304 bits after having read
  *	the 3 FIC blocks, each with 768 bits.
  *	for Mode IV we will get 3 * 2 * 768 = 4608, i.e. two resulting blocks
  *	Note that Mode III is NOT supported
  *	
  *	The function is called with a blkno. This should be 1, 2 or 3
  *	for each time 2304 bits are in, we call process_ficInput
  */
void	ficHandler::process_ficBlock (std::vector<int16_t> data,
	                              int16_t blkno) {
int32_t	i;

	if (blkno == 1) {
	   index = 0;
	   ficno = 0;
	   fib_processor::newFrame ();
	}
//
	if ((1 <= blkno) && (blkno <= 3)) {
	   for (i = 0; i < BitsperBlock; i ++) {
	      ofdm_input [index ++] = data [i];
	      if (index >= 2304) {
	         process_ficInput (ficno);
	         index = 0;
	         ficno ++;
	      }
	   }
	}
	else
	   fprintf (stderr, "You should not call ficBlock here\n");
//	we are pretty sure now that after block 4, we end up
//	with index = 0
}

/**
  *	\brief process_ficInput
  *	we have a vector of 2304 (0 .. 2303) soft bits that has
  *	to be de-punctured and de-conv-ed into a block of 768 bits
  *	In this approach we first create the full 3072 block (i.e.
  *	we first depuncture, and then we apply the deconvolution
  *	In the next coding step, we will combine this function with the
  *	one above
  */
void	ficHandler::process_ficInput (int16_t ficno) {
int16_t	i;
int16_t	viterbiBlock [3072 + 24];
int16_t	inputCount	= 0;

	memset (viterbiBlock, 0, (3072 + 24) * sizeof (int16_t));


	for (i = 0; i < 3072 + 24; i ++)
	   if (punctureTable [i])
	      viterbiBlock [i] = ofdm_input [inputCount ++];
/**
  *	Now we have the full word ready for deconvolution
  *	deconvolution is according to DAB standard section 11.2
  */
	myViterbi. deconvolve (viterbiBlock, bitBuffer_out);
/**
  *	if everything worked as planned, we now have a
  *	768 bit vector containing three FIB's
  *
  *	first step: energy dispersal according to the DAB standard
  *	We use a predefined vector PRBS
  */
	for (i = 0; i < 768; i ++)
	   bitBuffer_out [i] ^= PRBS [i];
/**
  *	each of the fib blocks is protected by a crc
  *	(we know that there are three fib blocks each time we are here)
  *	we keep track of the successrate
  *	and show that per 100 fic blocks
  *	One issue is what to do when we really believe the synchronization
  *	was lost.
  */

	for (i = ficno * 3; i < ficno * 3 + 3; i ++) {
	   uint8_t *p = &bitBuffer_out [(i % 3) * 256];
	   if (!check_CRC_bits (p, 256)) {
	      show_ficSuccess (false);
	      continue;
	   }

	   show_ficSuccess (true);
	   fib_processor::process_FIB (p, ficno);
	}
}

void	ficHandler::stop (void) {
}

void	ficHandler::reset	(void) {
	clearEnsemble ();
}


