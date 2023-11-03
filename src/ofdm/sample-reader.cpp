#
/*
 *    Copyright (C) 2013 .. 2017
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
#include	"sample-reader.h"
#include	"radio.h"

static  inline
int16_t valueFor (int16_t b) {
int16_t res     = 1;
	while (--b > 0)
	   res <<= 1;
	return res;
}

static
Complex oscillatorTable [INPUT_RATE];

	sampleReader::sampleReader (RadioInterface *mr,
	                            deviceHandler	*theRig_i,
	                            RingBuffer<Complex> *spectrumBuffer_i):
	                               theRig (theRig_i),
	                               spectrumBuffer (spectrumBuffer_i) {
int	i;
	bufferSize		= 32768;
	connect (this, SIGNAL (show_spectrum (int)),
	         mr, SLOT (show_spectrum (int)));
	localBuffer. resize (bufferSize);
	localCounter		= 0;
	currentPhase	= 0;
	sLevel		= 0;
	sampleCount	= 0;
	clipped		= false;
	for (i = 0; i < INPUT_RATE; i ++)
	   oscillatorTable [i] = Complex
	                            (cos (2.0 * M_PI * i / INPUT_RATE),
	                             sin (2.0 * M_PI * i / INPUT_RATE));

	bufferContent	= 0;
	corrector	= 0;
	dumpfilePointer. store (nullptr);
	dumpIndex	= 0;
	dumpScale	= valueFor (theRig -> bitDepth());
	running. store (true);
}

	sampleReader::~sampleReader () {
}

void	sampleReader::setRunning (bool b) {
	running. store (b);
}

float	sampleReader::get_sLevel() {
	return sLevel;
}

Complex	sampleReader::get_sample (float phaseOffset) {
std::vector<Complex> buffer (1);

	get_samples (buffer, 0, 1, phaseOffset, false);
	return buffer [0];
}

void	sampleReader::get_samples (std::vector<Complex>  &v,
	                           int index,
	                           int32_t n,
	                           int32_t phaseOffset, bool saving) {
Complex buffer [n];

	corrector	= phaseOffset;
	if (!running. load())
	   throw 21;
	if (n > bufferContent) {
	   bufferContent = theRig -> Samples();
	   while ((bufferContent < n) && running. load()) {
	      usleep (10);
	      bufferContent = theRig -> Samples();
	   }
	}

	if (!running. load())	
	   throw 20;
//
//	so here, bufferContent >= n
	n	= theRig -> getSamples (buffer, n);
	bufferContent -= n;
	if (dumpfilePointer. load () != nullptr) {
	   for (int i = 0; i < n; i ++) {
	      dumpBuffer [2 * dumpIndex    ] = real (v [i]) * dumpScale;
	      dumpBuffer [2 * dumpIndex + 1] = imag (v [i]) * dumpScale;
	      if (++dumpIndex >= DUMPSIZE / 2) {
	         sf_writef_short (dumpfilePointer. load(),
	                          dumpBuffer, dumpIndex);
	         dumpIndex = 0;
	      }
	   }
	}

//	OK, we have samples!!
//	first: adjust frequency. We need Hz accuracy
	for (int i = 0; i < n; i ++) {
	   currentPhase	-= phaseOffset;
//
//	Note that "phase" itself might be negative
	   currentPhase	= (currentPhase + INPUT_RATE) % INPUT_RATE;
	   if (localCounter < bufferSize)
	      localBuffer [localCounter ++]     = v [i];
	   float abs_lev = jan_abs_clipped (v [i], clipped, 0.95f);
	   v  [index + i]	= buffer [i] * oscillatorTable [currentPhase];
	   sLevel	= 0.00001 * jan_abs (v [i]) + (1 - 0.00001) * sLevel;
	}
#define	N 5
	sampleCount	+= n;
	if (sampleCount > INPUT_RATE / N) {
	   show_corrector	(corrector);
	   sampleCount = 0;
	   if ((spectrumBuffer != nullptr) && saving) {
	      spectrumBuffer -> putDataIntoBuffer (localBuffer. data (),
	                                                       bufferSize);
	      emit show_spectrum (bufferSize);
	      localCounter = 0;
	   }
	}
}

void	sampleReader::start_dumping (SNDFILE *f) {
	dumpfilePointer. store (f);
}

void	sampleReader::stop_dumping() {
	dumpfilePointer. store (nullptr);
}

bool	sampleReader::check_clipped	() {
bool res	= clipped;
	clipped	= false;
	return res;
}

