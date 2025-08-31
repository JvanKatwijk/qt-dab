
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

//	This is a (partial) rewrite of the GNU radio code, for use
//	within the DAB/DAB+ sdr-j receiver software
//
#pragma once
#include	<cstdint>

class firecodeChecker {
public:
		firecodeChecker ();
		~firecodeChecker();
// error detection. x[0-1] contains parity, x[2-10] contains data
// return true if firecode check is passed
	bool	check		(const uint8_t *);
	bool	checkAndCorrect	(uint8_t *);

private:
	void	fillSyndromeTable	();
	uint16_t crc16			(const uint8_t *);

//	g(x)=(x^11+1)(x^5+x^3+x^2+x+1)=x^16+x^14+x^13+x^12+x^11+x^5+x^3+x^2+x+1
	const uint16_t	polynom = 0x782f;
	uint16_t	 crcTable [256];
	uint16_t	 syndromeTable[65536];
};

