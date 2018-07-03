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

#define FFTW_MALLOC		fftwf_malloc
#define FFTW_PLAN_DFT_1D	fftwf_plan_dft_1d
#define FFTW_DESTROY_PLAN	fftwf_destroy_plan
#define FFTW_FREE		fftwf_free
#define FFTW_PLAN		fftwf_plan
#define FFTW_EXECUTE		fftwf_execute
#include    <fftw3.h>

/*
 *  a simple wrapper
 */

class   fftHandler {
public:
		fftHandler	(uint8_t);
        	~fftHandler	(void);
        std::complex<float>  *getVector (void);
        void		do_FFT (void);
        void		do_IFFT (void);
    private:
	dabParams	p;
        int32_t		fftSize;
        std::complex<float>  *vector;
        FFTW_PLAN   plan;
};

class   common_ifft {
public:
		common_ifft (int32_t);
        	~common_ifft(void);
        std::complex<float>  *getVector(void);
        void	do_IFFT(void);
    private:
        int32_t	fft_size;
        std::complex<float>  *vector;
        FFTW_PLAN   plan;
        void        Scale(std::complex<float> *);
};
#endif

