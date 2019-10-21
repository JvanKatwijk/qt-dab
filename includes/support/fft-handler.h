/*
 *
 *    Copyright (C) 2009 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB
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

#ifndef _COMMON_FFT
#define _COMMON_FFT

//  Simple wrapper around fftwf
#include	"dab-constants.h"
#include	"dab-params.h"

#include    <fftw3.h>

/*
 *  a simple wrapper
 */

class   fftHandler {
public:
		fftHandler	(uint8_t);
        	~fftHandler();
        DSPCOMPLEX	*getVector();
        void		do_FFT();
        void		do_IFFT();
    private:
	dabParams	p;
        int32_t		fftSize;
        DSPCOMPLEX  *vector;
        FFTW_PLAN   plan;
};

#endif

