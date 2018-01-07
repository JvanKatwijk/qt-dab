#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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
#
#ifndef	__FREQ_INTERLEAVER__
#define	__FREQ_INTERLEAVER__
#include	<stdint.h>
#include	<vector>
#include	"dab-constants.h"
#include	"dab-params.h"

/**
  *	\class interLeaver
  *	Implements frequency interleaving according to section 14.6
  *	of the DAB standard
  */
class	interLeaver {
public:
		interLeaver	(uint8_t);
		~interLeaver	(void);
	int16_t	mapIn		(int16_t);
private:
	dabParams	p;
	void    createMapper	(int16_t T_u, int16_t V1,
                                 int16_t lwb, int16_t upb,
	                         int16_t * v);
	std::vector<int16_t> permTable;
};

#endif

