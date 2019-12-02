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

	sampleReader::sampleReader (RadioInterface *mr,
	                            virtualInput	*theRig
	                            ,RingBuffer<std::complex<float>> *spectrumBuffer
	                           ) {
int	i;
	this	-> theRig	= theRig;
        bufferSize		= 32768;
        this    -> spectrumBuffer       = spectrumBuffer;
        connect (this, SIGNAL (show_Spectrum (int)),
                 mr, SLOT (showSpectrum (int)));
        localBuffer. resize (bufferSize);
        localCounter		= 0;
	connect (this, SIGNAL (show_Corrector (int)),
	         mr, SLOT (set_CorrectorDisplay (int)));
	currentPhase	= 0;
	sLevel		= 0;
	sampleCount	= 0;
	oscillatorTable = new std::complex<float> [INPUT_RATE];
        for (i = 0; i < INPUT_RATE; i ++)
           oscillatorTable [i] = std::complex<float>
	                            (cos (2.0 * M_PI * i / INPUT_RATE),
                                     sin (2.0 * M_PI * i / INPUT_RATE));

	bufferContent	= 0;
	corrector	= 0;
	dumpfilePointer. store (nullptr);
	dumpIndex	= 0;
	dumpScale	= valueFor (theRig -> bitDepth());
	running. store (true);
}

	sampleReader::~sampleReader() {
	delete[] oscillatorTable;
}

void	sampleReader::setRunning (bool b) {
	running. store (b);
}

float	sampleReader::get_sLevel() {
	return sLevel;
}

std::complex<float> sampleReader::getSample (int32_t phaseOffset) {
std::complex<float> temp;

	corrector	= phaseOffset;
	if (!running. load())
	   throw 21;

///	bufferContent is an indicator for the value of ... -> Samples()
	if (bufferContent == 0) {
	   bufferContent = theRig -> Samples();
	   while ((bufferContent <= 2048) && running. load()) {
	      usleep (10);
	      bufferContent = theRig -> Samples(); 
	   }
	}

	if (!running. load())	
	   throw 20;
//
//	so here, bufferContent > 0
	theRig -> getSamples (&temp, 1);
	bufferContent --;
	if (dumpfilePointer. load() != nullptr) {
	   dumpBuffer [2 * dumpIndex    ] = real (temp) * dumpScale;
	   dumpBuffer [2 * dumpIndex + 1] = imag (temp) * dumpScale;
	   if ( ++dumpIndex >= DUMPSIZE / 2) {
	      sf_writef_short (dumpfilePointer. load(),
	                       dumpBuffer, dumpIndex);
	      dumpIndex = 0;
	   }
	}

	if (localCounter < bufferSize)
	   localBuffer [localCounter ++]        = temp;
//
//	OK, we have a sample!!
//	first: adjust frequency. We need Hz accuracy
	currentPhase	-= phaseOffset;
	currentPhase	= (currentPhase + INPUT_RATE) % INPUT_RATE;

	temp		*= oscillatorTable [currentPhase];
	sLevel		= 0.00001 * jan_abs (temp) + (1 - 0.00001) * sLevel;
#define	N	5
	if (++ sampleCount > INPUT_RATE / N) {
	   show_Corrector	(corrector);
	   sampleCount = 0;
	   if (spectrumBuffer != nullptr) {
              spectrumBuffer -> putDataIntoBuffer (localBuffer. data(),
	                                                       localCounter);
              emit show_Spectrum (bufferSize);
	   }
           localCounter = 0;
	}
	return temp;
}

void	sampleReader::getSamples (std::complex<float>  *v,
	                          int32_t n, int32_t phaseOffset) {
int32_t		i;

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
	n	= theRig -> getSamples (v, n);
	bufferContent -= n;
	if (dumpfilePointer. load() != nullptr) {
	   for (i = 0; i < n; i ++) {
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
	for (i = 0; i < n; i ++) {
	   currentPhase	-= phaseOffset;
//
//	Note that "phase" itself might be negative
	   currentPhase	= (currentPhase + INPUT_RATE) % INPUT_RATE;
	   if (localCounter < bufferSize)
	      localBuffer [localCounter ++]     = v [i];
	   v [i]	*= oscillatorTable [currentPhase];
	   sLevel	= 0.00001 * jan_abs (v [i]) + (1 - 0.00001) * sLevel;
	}

	sampleCount	+= n;
	if (sampleCount > INPUT_RATE / N) {
	   show_Corrector	(corrector);
	   if (spectrumBuffer != nullptr) {
	      spectrumBuffer -> putDataIntoBuffer (localBuffer. data(),
	                                                       bufferSize);
	      emit show_Spectrum (bufferSize);
	   }
	   localCounter = 0;
	   sampleCount = 0;
	}
}

void	sampleReader::startDumping (SNDFILE *f) {
	dumpfilePointer. store (f);
}

void	sampleReader::stopDumping() {
	dumpfilePointer. store (nullptr);
}

