#
/*
 * Copyright 2004,2010 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
//
//	This is a (partial) rewrite of the GNU radio code, for use
//	within the DAB/DAB+  software
//
#include "firecode-checker.h"
#include <cstring>

#include	<stdio.h>

const uint8_t pattern [124] = {
// 45 * 4 bit shift
	17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 30, 31, 34,
	36, 38, 40, 42, 44, 46, 50, 52, 54, 56, 60, 62, 68, 72, 76,
	84, 88, 92,100,104,108,120,124,136,152,168,184,200,216,248,
// 15 * 2 bit shift
	33, 35, 37, 39, 41, 43, 45, 49, 51, 53, 55, 57, 59, 61, 63,
// 15 * 2 and 6 bit shift
	66, 70, 74, 78, 82, 86, 90, 98,102,106,110,114,118,122,126,
// 15 * 6 bit shift
	132,140,148,156,164,172,180,196,204,212,220,228,236,244,252,
// 34 * 0 bit shift
	1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	16, 29, 32, 48, 58, 64, 80, 96,112,116,128,144,160,176,192,
	208,224,232,240
};
//	g (x) = (x^11 + 1) (x^5+x^3+x^2+x+1)
//	      = 1+x+x^2+x^3+x^5+x^11+x^12+x^13+x^14+x^16
//const uint8_t firecodeChecker::g [16] = { 
//	      1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0};
//	0xF81D

	firecodeChecker::firecodeChecker () {
//	prepare the table
	for (int i = 0; i < 256; i++) {
	   uint16_t crc = i << 8;
	   for (int j = 0; j < 8; j++) {
	      if (crc & 0x8000) {
	         crc <<= 1;
	         crc ^= polynom;
	      }
	      else
	         crc <<= 1;
	   }
	   crcTable[i] = crc;
	}
	fillSyndromeTable ();
}

	firecodeChecker::~firecodeChecker () {}

void	firecodeChecker::fillSyndromeTable () {
uint8_t	 error [11];
int  bit;

	memset (error, 0, 11);
	memset (syndromeTable, 0, 65536 * sizeof (syndromeTable [0]));
//	0 bit shifted
	for (int16_t i = 0; i < 11; i++) {
	   for (int16_t j = 0; j < 124; j++) {
	      bit = i * 8;
	      error [i] = pattern [j];
	      uint16_t syndrome = crc16 (error);
	      if (syndromeTable [syndrome] == 0)
	         syndromeTable[syndrome] = (bit << 8) + pattern [j];
	      error [i] = 0;
	   }
	}
//	4 bits shifted
	for (int16_t i = 0; i < 10; i++) {
	   for (int16_t j = 0; j < 45; j++) {
	      bit = i * 8 + 4;
	      error [i]   = pattern [j] >> 4;
	      error [i + 1] = pattern [j] << 4;
	      uint16_t syndrome = crc16 (error);
	      if (syndromeTable [syndrome] == 0)
	         syndromeTable [syndrome] = (bit << 8) + pattern [j];
	      error [i]   = 0;
	      error [i + 1] = 0;
	   }
	}
//	2 bits shifted
	for (int16_t i = 0; i < 10; i++) {
	   for (int16_t j = 45; j < 75; j++) {
	      bit = i * 8 + 2;
	      error [i]	= pattern [j] >> 2;
	      error [i + 1] = pattern [j] << 6;
	      uint16_t syndrome = crc16 (error);
	      if (syndromeTable[syndrome] == 0)
	         syndromeTable [syndrome] = (bit << 8) + pattern [j];
	      error [i]		= 0;
	      error [i + 1]	= 0;
	   }
	}
//	6 bits shifted
	for (int16_t i = 0; i < 10; i++) {
	   for (int16_t j = 60; j < 90; j++) {
	      int bit = i * 8 + 6;
	      error [i]   = pattern [j] >> 6;
	      error [i + 1] = pattern[j] << 2;
	      uint16_t syndrome = crc16 (error);
	      if (syndromeTable [syndrome] == 0)
	         syndromeTable[syndrome] = (bit << 8) + pattern [j];
	      error [i]		= 0;
	      error [i + 1]	= 0;
	   }
	}
}

//	x[0-1] contains parity, x[2-10] contains data
uint16_t  firecodeChecker::crc16 (const uint8_t *x) {
uint16_t crc = 0;

	for (int i = 2; i < 11; i++) {
	   int pos = (crc >> 8) ^ x [i];
	   crc = (crc << 8) ^ crcTable [pos];
	}

	for (int i = 0; i < 2; i++) {
	   int pos = (crc >> 8) ^ x [i];
	   crc = (crc << 8) ^ crcTable [pos];
	}
	return crc;
}

//	return true if firecode check is passed
bool	firecodeChecker::check (const uint8_t *x) {
	return crc16 (x) == 0;
}

//	return true if firecode check is passed OR 
//	an  error burst up to 6 bits was corrected
bool	firecodeChecker::checkAndCorrect (uint8_t  *x) {
uint16_t syndrome = crc16 (x);

	if (syndrome == 0) // no error
	   return true;

	uint8_t error = syndromeTable [syndrome] & 0xff;
	if (error != 0) {
	   int bit = syndromeTable [syndrome] >> 8;
	   x [bit/8]   ^= error >> (bit % 8);
	   x [bit/8+1] ^= error << (8 - (bit % 8));
	   return true;
	}
	return false;
}
