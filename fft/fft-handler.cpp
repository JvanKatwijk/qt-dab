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
//
//	a simple wrapper for using kiss-fft, fftw3 or fft-complex
//
#include	"fft-handler.h"
#include	"fft-complex.h"

	fftHandler::fftHandler	(int size, bool dir) {
	this	-> size		= size;
	this	-> dir		= dir;

#ifdef	USE_DOUBLE
	fftVector		= (Complex *)
	                          fftw_malloc (sizeof (Complex) * size);
	plan			= fftw_plan_dft_1d (size,
	                           reinterpret_cast <fftw_complex *>(fftVector),
                                   reinterpret_cast <fftw_complex *>(fftVector),
                                   FFTW_FORWARD, FFTW_ESTIMATE);
#else
	fftVector		= (Complex *)
	                          fftwf_malloc (sizeof (Complex) * size);
	plan			= fftwf_plan_dft_1d (size,
	                           reinterpret_cast <fftwf_complex *>(fftVector),
                                   reinterpret_cast <fftwf_complex *>(fftVector),
                                   FFTW_FORWARD, FFTW_ESTIMATE);
#endif
}

	fftHandler::~fftHandler	() {
#ifdef	USE_DOUBLE
	fftw_destroy_plan (plan);
	fftw_free (fftVector);
#else
	fftwf_destroy_plan (plan);
	fftwf_free (fftVector);
#endif
}

void	fftHandler::fft		(std::vector<Complex> &v) {
	if (dir)
	   for (int i = 0; i < size; i ++)
	      fftVector [i] = conj (v [i]);
	else
	   for (int i = 0; i < size; i ++)
	      fftVector [i] = v [i];
#ifdef	USE_DOUBLE
	fftw_execute (plan);
#else
	fftwf_execute (plan);
#endif
	if (dir)
	   for (int i = 0;  i < size; i ++)
	      v [i] = conj (fftVector [i]);
	else
	   for (int i = 0; i < size; i ++)
	      v [i] = fftVector [i];
}

void	fftHandler::fft		(Complex  *v) {
	for (int i = 0; i < size; i ++)
	   fftVector [i] = v [i];
#ifdef	USE_DOUBLE
	fftw_execute (plan);
#else
	fftwf_execute (plan);
#endif
	for (int i = 0;  i < size; i ++)
	   v [i] = fftVector [i];
}

