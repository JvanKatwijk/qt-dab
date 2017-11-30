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
//
//	For the different formats for input, we have
//	different readers, with one "mother" reader.
//	Note that the cardreader is quite different here
//
//	This is the - almost empty - default implementation
#include	"virtual-reader.h"

	virtualReader::virtualReader	(RingBuffer<std::complex<float>> *p,
	                                                       int32_t rate) {
	theBuffer	= p;
	blockSize	= -1;
	setMapper (rate, 2048000);
}

	virtualReader::~virtualReader		(void) {
}

void	virtualReader::restartReader	(int32_t s) {
	fprintf (stderr, "Restart met block %d\n", s);
	blockSize	= s;
}

void	virtualReader::stopReader	(void) {
}

void	virtualReader::processData	(float IQoffs, void *data, int cnt) {
	(void)IQoffs;
	(void)data;
	(void)cnt;
}

int16_t	virtualReader::bitDepth	(void) {
	return 12;
}

void	virtualReader::setMapper	(int32_t inRate, int32_t outRate) {
int32_t	i;

	this	-> inSize	= inRate / 1000;
	this	-> outSize	= outRate / 1000;
	inTable			= new std::complex<float> [inSize];
	outTable		= new std::complex<float> [outSize];
	mapTable		= new float [outSize];
	for (i = 0; i < outSize; i ++)
	   mapTable [i] = (float) i * inRate / outRate;
	conv	= 0;
}

void	virtualReader::convertandStore (std::complex<float> *s,
	                                             int32_t amount) {
int32_t	i, j;

	for (i = 0; i < amount; i ++) {
	   inTable [conv++]	= s [i];
	   if (conv >= inSize) {	// full buffer, map
	      for (j = 0; j < outSize - 1; j ++) {
	         int16_t base	= (int)(floor (mapTable [j]));
	         float  frac	= mapTable [j] - base;
	         outTable [j]	= cmul (inTable [base], 1 - frac) +
	                          cmul (inTable [base + 1], frac);
	      }
	      
//
//	let op, het laatste element was nog niet gebruikta
	      conv	= 1;
	      inTable [0] = inTable [inSize - 1];
	      theBuffer -> putDataIntoBuffer (outTable, outSize - 1);
	   }
	}
}

