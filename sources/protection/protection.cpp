#
/*
 *    Copyright (C) 2014 .. 2017
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
 *
 *
 *	Simple base class for combining uep and eep deconvolvers
 */
#include	<vector>
#include	<cstring>
#include	<cstdio>
#include	"protection.h"

       protection::protection  (int16_t bitRate,
	                        int16_t protLevel, uint8_t cpuSupport):
                                       viterbi (24 * bitRate, true, cpuSupport),
//                                        viterbiSpiral (24 * bitRate, true),
                                        outSize (24 * bitRate),
	                                indexTable   (outSize * 4 + 24),
                                        viterbiBlock (outSize * 4 + 24){
	(void)protLevel;
	this	-> bitRate	= bitRate;
}

	protection::~protection	() {}

bool	protection::deconvolve (int16_t *v,
	                        int32_t size,
	                        uint8_t *outBuffer) {
int16_t	inputCounter	= 0;
	(void)size;
//	clear the bits in the viterbiBlock,
//	only the non-punctured ones are set
	memset (viterbiBlock. data(), 0,
	                        (outSize * 4 + 24) * sizeof (int16_t)); 
///	The actual deconvolution is done by the viterbi decoder

	for (int i = 0; i < outSize * 4 + 24; i ++)
	   if (indexTable [i])
	      viterbiBlock [i] = v [inputCounter ++];
	viterbi::deconvolve (viterbiBlock. data(), outBuffer);
//	viterbiSpiral::deconvolve (viterbiBlock. data(), outBuffer);
	return true;
}

void	protection::getParameters (int16_t &bitRate,
	                                int16_t &protLevel, bool &uepFlag) {
	bitRate		= this -> bitRate;
	protLevel	= this -> protLevel;
        uepFlag		= false;
	fprintf (stderr, "We should not be here at all\n");
}

