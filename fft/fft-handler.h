#
/*
 *    Copyright (C) 2015 .. 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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
#
#ifndef	__FFT_HANDLER__
#define	__FFT_HANDLER__

#include	<complex>
#include	<vector>
#include	"kiss_fft.h"
#ifdef	__FFTW3__
#include	<fftw3.h>
#endif

class	fftHandler {
public:
			fftHandler	(int size, bool);
			~fftHandler	();
	void		fft 		(std::vector<std::complex<float>> &);
	void		fft 		(std::complex<float> *);
private:
	int		size;
	bool		dir;
#ifdef	__KISS_FFT__
	kiss_fft_cfg	plan;
	kiss_fft_cpx	*fftVector_in;
	kiss_fft_cpx	*fftVector_out;
#elif	__FFTW3__
	fftwf_plan	plan;
	std::complex<float> *fftVector;
#endif
};
#endif
