#
/*
 *    Copyright (C) 2013 .. 2023
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
#include	"dab-constants.h"

static  inline
int16_t valueFor (int16_t b) {
int16_t res     = 1;
	while (--b > 0)
	   res <<= 1;
	return res;
}

static inline
float	average (float avg, float inp, float factor) {
	return (1.0 - factor) * avg + factor * inp;
}

static inline
void    constrain (float &testVal, const float limit) {
        if (testVal > limit)
           testVal = limit;
        else
        if (testVal < -limit) {
           testVal = -limit;
        }
}

static
Complex oscillatorTable [SAMPLERATE];
constexpr float ALPHA = 1.0f / SAMPLERATE;

	sampleReader::sampleReader (RadioInterface *mr,
	                            deviceHandler	*theRig_i,
	                            RingBuffer<Complex> *spectrumBuffer_i):
	                               theRig (theRig_i),
	                               spectrumBuffer (spectrumBuffer_i) {
int	i;
	bufferSize		= 32768;
	localBuffer. resize (bufferSize);
	localCounter		= 0;
	currentPhase	= 0;
	sLevel		= 0;
	sampleCount	= 0;
	dcRemoval	= false;
	dcReal		= 0;
	dcImag		= 0;
	IQ_Real		= 0;
	IQ_Imag		= 0;

	repetitionCounter	= 8;
	for (i = 0; i < SAMPLERATE; i ++)
	   oscillatorTable [i] = Complex
	                            (cos (2.0 * M_PI * i / SAMPLERATE),
	                             sin (2.0 * M_PI * i / SAMPLERATE));

	bufferContent	= 0;
	corrector	= 0;
	dumpIndex	= 0;
	dumpScale	= valueFor (theRig -> bitDepth());
	fprintf (stderr, "bitDepth %d, scale %d\n", theRig -> bitDepth (), dumpScale);
	connect (this, &sampleReader::show_spectrum,
	         mr,  &RadioInterface::show_spectrum);
	connect (this, &sampleReader::show_dcOffset,
	         mr, &RadioInterface::show_dcOffset);
	running. store (true);

}

	sampleReader::~sampleReader () {
}

void	sampleReader::setRunning (bool b) {
	running. store (b);
}

float	sampleReader::getSLevel () {
	return sLevel;
}

Complex	sampleReader::getSample (float phaseOffset) {
std::vector<Complex> buffer (1);

	getSamples (buffer, 0, 1, phaseOffset, false);
	return buffer [0];
}

void	sampleReader::getSamples (std::vector<Complex>  &v_out,
	                           int index,
	                           int32_t nrSamples,
	                           int32_t phaseOffset, bool saving) {
auto *buffer	= dynVec (std::complex<float>, nrSamples);
	corrector	= phaseOffset;

//	if we get a kill signal, do the kill
	if (!running. load())
	   throw 21;
//
//	wait for samples
	if (nrSamples > bufferContent) {
	   bufferContent = theRig -> Samples();
	   while ((bufferContent < nrSamples) && running. load()) {
	      usleep (10);
	      bufferContent = theRig -> Samples();
	   }
	}

	if (!running. load())	
	   throw 20;
//
//	so here, bufferContent >= n
	nrSamples	= theRig -> getSamples (buffer, nrSamples);
	bufferContent	-= nrSamples;

//	if dumping is "on" dump
	if (sourceDumper. isActive ()) {
	   for (int i = 0; i < nrSamples; i ++) {
	      dumpBuffer [2 * dumpIndex    ] = real (buffer [i]) * dumpScale;
	      dumpBuffer [2 * dumpIndex + 1] = imag (buffer [i]) * dumpScale;
	      if (++ dumpIndex >= DUMPSIZE / 2) {
	         sourceDumper. write (dumpBuffer, dumpIndex);
	         dumpIndex = 0;
	      }
	   }
	}
//	OK, we have samples!!
	for (int i = 0; i < nrSamples; i ++) {
	   float Alpha	= 1.0 / SAMPLERATE;
	   std::complex<float> v = buffer [i];
	   if (dcRemoval) {
	      dcReal		= compute_avg (dcReal, real (v), Alpha);
	      dcImag		= compute_avg (dcImag, imag (v), Alpha);
	      v = std::complex<float> (real (v) - dcReal, imag (v) - dcImag);
	      v = theEqualizer. equalize (v);
	      DABFLOAT real_V	= abs (real (v));
	      DABFLOAT imag_V	= abs (imag (v)); 
	      IQ_Real		= compute_avg (IQ_Real, real_V, Alpha);
	      IQ_Imag		= compute_avg (IQ_Imag, imag_V, Alpha);
	      static int teller = 0; 
	      if (++teller >= SAMPLERATE) {
	         show_dcOffset (10 * (IQ_Real - IQ_Imag) / 
	                                     ((IQ_Real + IQ_Imag) / 2));
	         teller = 0;
	      }
	   }

//	first: adjust frequency. We need Hz accuracy
//	Note that "phase" itself might be negative
	   currentPhase	-= phaseOffset;
	   currentPhase	= (currentPhase + SAMPLERATE) % SAMPLERATE;
	   if (saving && (localCounter < bufferSize))
	      localBuffer [localCounter ++]     = v;
	   v_out  [index + i]	= Complex (real (v),
	                                   imag (v)) * oscillatorTable [currentPhase];
	   sLevel = 0.00001 * jan_abs (v_out [i]) + (1 - 0.00001) * sLevel;
	}

	sampleCount	+= nrSamples;
	
	if (saving && (spectrumBuffer != nullptr) &&
	             (sampleCount > SAMPLERATE / repetitionCounter)) {
//	   show_corrector	(corrector);
	   sampleCount = 0;
	   spectrumBuffer -> putDataIntoBuffer (localBuffer. data (),
	                                                       bufferSize);
	   emit show_spectrum (bufferSize);
	   localCounter = 0;
	}
}

void	sampleReader::startDumping (const QString &fileName,
	                            int freq, int bitDepth, 
	                            const QString deviceName) {
	sourceDumper. init (fileName, SAMPLERATE,
	                    freq, bitDepth, deviceName);
}

void	sampleReader::stopDumping() {
	sourceDumper.close ();
}

void	sampleReader::set_dcRemoval	(bool b) {
	dcRemoval	= b;
}

