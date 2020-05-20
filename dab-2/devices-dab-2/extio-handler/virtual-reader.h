#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of dab-2
 *
 *    dab-2 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    dab-2 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dab-2; if not, write to the Free Software
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

class	dabProcessor;
//
//	The virtualReader is the mother of the readers.
//	The cardReader is slighty different, however
//	made fitting the framework
class	virtualReader {
protected:
int32_t	blockSize;
public:
		virtualReader	(dabProcessor *, int32_t rate);
virtual		~virtualReader	(void);
virtual void	restartReader	(int32_t s);
virtual void	stopReader	(void);
virtual void	processData	(float IQoffs, void *data, int cnt);
virtual	int16_t	bitDepth	(void);
protected:
	dabProcessor	*base;
	int32_t	numberBase;
	void	convertandStore		(std::complex<float> *, int32_t);
private:
	void		setMapper	(int32_t, int32_t);
	float		*mapTable;
	int16_t		conv;
	int16_t		inSize;
	int16_t		outSize;
	std::complex<float>	*inTable;
	std::complex<float>	*outTable;
	
};

#endif

