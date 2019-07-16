#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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
#ifndef	__VIRTUAL_READER__
#define	__VIRTUAL_READER__

#include	<stdint.h>
#include	<stdio.h>
#include	"ringbuffer.h"
#include	"dab-constants.h"
//
//	The virtualReader is the mother of the readers.
//	The cardReader is slighty different, however
//	made fitting the framework
class	virtualReader {
protected:
RingBuffer<std::complex<float>>	*theBuffer;
int32_t	blockSize;
public:
		virtualReader	(RingBuffer<std::complex<float>> *p,
	                                                  int32_t rate);
virtual		~virtualReader	(void);
virtual void	restartReader	(int32_t s);
virtual void	stopReader	(void);
virtual void	processData	(float IQoffs, void *data, int cnt);
virtual	int16_t	bitDepth	(void);
protected:
	int32_t	base;
	void	convertandStore		(std::complex<float> *, int32_t);
private:
	void	setMapper	(int32_t, int32_t);
	float	*mapTable;
	int16_t	conv;
	int16_t	inSize;
	int16_t	outSize;
	std::complex<float>	*inTable;
	std::complex<float>	*outTable;
	
};

#endif

