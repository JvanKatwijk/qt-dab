#
/*
 *    Copyright (C) 2014 .. 2025
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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
//
//	inline functions for crc handling
#pragma once
#include	<stdint.h>
#include	"dab-constants.h"
//	there depend on an large table
//	calc_crc computes - as the name suggests the crc value
uint16_t calc_crc	(const uint8_t *data, int length);
//	check_crc_bytes checks the crc assuming it is contained
//	in the last 16 bits of the data
bool	check_crc_bytes (const uint8_t *data, int length);

//	this one is inline

static inline
bool	check_CRC_bits (const uint8_t *inBuf, int32_t size) {
static
const uint8_t crcPolynome [] =
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0};	// MSB .. LSB
uint8_t *workVector  = dynVec (uint8_t, size);
uint8_t	b [16];
int16_t	Sum	= 0;

	memset (b, 1, 16);
	memcpy (workVector, inBuf, size - 16);

	for (int i = size - 16; i < size; i ++)
	   workVector [i] = inBuf [i] ^ 1;

	for (int i = 0; i < size; i++) {
	   if ((b [0] ^ workVector [i]) == 1) {
	      for (int f = 0; f < 15; f++) 
	         b [f] = crcPolynome [f] ^ b[f + 1];
	      b [15] = 1;
	   }
	   else {
	      memmove (&b [0], &b[1], sizeof (uint8_t ) * 15); // Shift
	      b [15] = 0;
	   }
	}

	for (int i = 0; i < 16; i++)
	   Sum += b [i];

	return Sum == 0;
}


//static inline
//bool	check_crc_bytes (const uint8_t *msg, int32_t len) {
//uint16_t	accumulator	= 0xFFFF;
//uint16_t	crc;
//uint16_t	genpoly		= 0x1021;
//
//uint16_t r1 = calc_crc (msg, len, crctab_1021, 0xFFFF);
//	for (int i = 0; i < len; i ++) {
//	   uint16_t xx = msg [i];
//	   int16_t data = xx << 8;
//	   for (int j = 8; j > 0; j--) {
//	      if ((data ^ accumulator) & 0x8000)
//	         accumulator = ((accumulator << 1) ^ genpoly) & 0xFFFF;
//	      else
//	         accumulator = (accumulator << 1) & 0xFFFF;
//	      data = (data << 1) & 0xFFFF;
//	   }
//	}
////
////	ok, now check with the crc that is contained
////	in the au
//	crc	= ~((msg [len] << 8) | msg [len + 1]) & 0xFFFF;
//
//	static int teller = 0 ;
//	teller ++;
//	if (teller > 25) {
//	   fprintf (stderr, "%x - %x\n", r1, crc);
//	   teller = 0;
//	}
//	return (crc ^ accumulator) == 0;
//}

