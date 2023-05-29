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
#ifdef	__KISS_FFT__
	fftVector_in            = new kiss_fft_cpx [size];
        fftVector_out           = new kiss_fft_cpx [size];
        plan			= kiss_fft_alloc (size, dir, nullptr, nullptr);
#elif	__FFTW3__
	fftVector		= (std::complex<float> *)
	                          fftwf_malloc (sizeof (std::complex<float>)* size);
	plan			= fftwf_plan_dft_1d (size,
	                           reinterpret_cast <fftwf_complex *>(fftVector),
                                   reinterpret_cast <fftwf_complex *>(fftVector),
                                   FFTW_FORWARD, FFTW_ESTIMATE);
#endif
}

	fftHandler::~fftHandler	() {
#ifdef	__KISS_FFT__
	delete fftVector_in;
	delete fftVector_out;
#elif	__FFTW3__
	fftwf_destroy_plan (plan);
	fftwf_free (fftVector);
#endif
}

void	fftHandler::fft		(std::vector<std::complex<float>> &v) {
#ifdef	__KISS_FFT__
	for (int i = 0; i < size; i ++) {
	   fftVector_in [i]. r = real (v [i]);
	   fftVector_in [i]. i = imag (v [i]);
	}
	kiss_fft (plan, fftVector_in, fftVector_out);
	for (int i = 0; i < size; i ++) {
	   v [i] = std::complex<float> (fftVector_out [i]. r,
	                                fftVector_out [i]. i);
	}
#elif __FFTW3__
	if (dir)
	   for (int i = 0; i < size; i ++)
	      fftVector [i] = conj (v [i]);
	else
	   for (int i = 0; i < size; i ++)
	      fftVector [i] = v [i];
	fftwf_execute (plan);
	if (dir)
	   for (int i = 0;  i < size; i ++)
	      v [i] = conj (fftVector [i]);
	else
	   for (int i = 0; i < size; i ++)
	      v [i] = fftVector [i];
#else
	Fft_transform (v. data (), size, dir);
#endif
}

void	fftHandler::fft		(std::complex<float>  *v) {
#ifdef	__KISS_FFT__
	for (int i = 0; i < size; i ++) {
	   fftVector_in [i]. r = real (v [i]);
	   fftVector_in [i]. i = imag (v [i]);
	}
	kiss_fft (plan, fftVector_in, fftVector_out);
	for (int i = 0; i < size; i ++) {
	   v [i] = std::complex<float> (fftVector_out [i]. r,
	                                fftVector_out [i]. i);
	}
#elif	__FFTW3__
	for (int i = 0; i < size; i ++)
	   fftVector [i] = v [i];
	fftwf_execute (plan);
	for (int i = 0;  i < size; i ++)
	   v [i] = fftVector [i];
#else
	Fft_transform (v, size, dir);
#endif
}

