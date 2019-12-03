#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB.
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

#ifndef	__GALOIS
#define	__GALOIS

#include	<cstdint>
#include	<vector>

class	galois {
private:
	uint16_t mm;		/* Bits per symbol */
	uint16_t gfpoly;
	uint16_t codeLength;	/* Symbols per block (= (1<<mm)-1) */
	uint16_t d_q;
	std::vector<uint16_t> alpha_to;	/* log lookup table */
	std::vector<uint16_t> index_of;	/* Antilog lookup table */
public:
		galois		(uint16_t mm, uint16_t poly);
		~galois();
	int	modnn	(int);
 	uint16_t add_poly	(uint16_t a, uint16_t b);
	uint16_t add_power	(uint16_t a, uint16_t b);
	uint16_t multiply_poly	(uint16_t a, uint16_t b);	// a*b
	uint16_t multiply_power	(uint16_t a, uint16_t b);
	uint16_t divide_poly	(uint16_t a, uint16_t b); 	// a/b
	uint16_t divide_power	(uint16_t a, uint16_t b);
	uint16_t pow_poly	(uint16_t a, uint16_t n);	// a^n
	uint16_t pow_power	(uint16_t a, uint16_t n);
	uint16_t power2poly	(uint16_t a);
	uint16_t poly2power	(uint16_t a);
	uint16_t inverse_poly	(uint16_t a);
	uint16_t inverse_power	(uint16_t a);
};
#endif

