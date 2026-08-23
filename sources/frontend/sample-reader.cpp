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
#ifdef	__HAVE_VOLK__
#include	<volk/volk.h>
#endif
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
std::complex<float>cmplx_from_phase (float x) {
  /*
   * Minimax polynomial for sin(x) and cos(x) on [-pi/4, pi/4]
   * Coefficients via Remez algorithm (Sollya)
   * Max |error| < 1.5e-4 for sinus, < 6e-4 for cosinus
   */
  float x2 = x * x; 
  float s1 =  0.99903142452239990234375f;
  float s2 = -0.16034401953220367431640625; 
  float sine = x * (x2 * s2 + s1);
 
  float c1 =  0.9994032382965087890625f;
  float c2 = -0.495580852031707763671875;
  float c3 =  3.679168224334716796875e-2;
  float cosine = c1 + x2 * (x2 * c3 + c2);

  return std::complex<float> (cosine, sine);
}

static
Complex oscillatorTable [SAMPLERATE];
constexpr float ALPHA = 1.0f / SAMPLERATE;

	sampleReader::sampleReader (RadioInterface *mr,
	                            deviceHandler	*theRig_i,
	                            RingBuffer<Complex> *spectrumBuffer_i):
	                               theRig (theRig_i),
	                               dcRemover (41),
	                               spectrumBuffer (spectrumBuffer_i) {
	bufferSize		= 32768;
	localBuffer. resize (bufferSize);
	localCounter		= 0;
	currentPhase		= 0;
	sLevel			= 0.1f;	// average power level
	sampleCount		= 0;
	dcRemoval		= false;
	dcReal			= 0;
	dcImag			= 0;
	IQ_Real			= 0;
	IQ_Imag			= 0;

	phase			= std::complex<float> (1.0, 0.0);
	repetitionCounter	= 8;
	for (int i = 0; i < SAMPLERATE; i ++)
	   oscillatorTable [i] = Complex
	                            (cos (2.0 * M_PI * i / SAMPLERATE),
	                             sin (2.0 * M_PI * i / SAMPLERATE));

	bufferContent	= 0;
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

	getSamples (buffer, 0, 1, phaseOffset,  false);
	return buffer [0];
}

void	sampleReader::getSamples (std::vector<Complex>  &v_out,
	                           int index,
	                           int32_t nrSamples,
	                           float phaseOffset, bool saving) {
auto *buffer	= dynVec (std::complex<float>, nrSamples);

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
	   auto *dumpBuffer = dynVec (int16_t, 2 * nrSamples);
	   for (int i = 0; i < nrSamples; i ++) {
	      dumpBuffer [2 * i            ] = real (buffer [i]) * dumpScale;
	      dumpBuffer [2 * dumpIndex + 1] = imag (buffer [i]) * dumpScale;
	      sourceDumper. write (dumpBuffer, nrSamples);
	   }
	}

//	OK, we have samples!!

	float Alpha	= 1.0 / SAMPLERATE;
//	compute_avg	-> res = (1 - Alpha * res + Alpha * new
	for (int i = 0; i < nrSamples; i ++) 
	   IQ_Real	= average (IQ_Real, abs (real (buffer [i])), Alpha);
	for (int i = 0; i < nrSamples; i ++) 
	   IQ_Imag	= average (IQ_Imag, abs (imag (buffer [i])), Alpha);
	if (dcRemoval) 
	   for (int i = 0; i < nrSamples; i ++) 
	      buffer [i]	= dcRemover. filter (buffer [i]);
#ifndef	__HAVE_VOLK__
	for (int i = 0; i < nrSamples; i ++)
	   sLevel = 0.00001 * jan_abs (buffer [i]) + (1 - 0.00001) * sLevel;
#else
	{  alignas(64) float *realTable = dynVec (float, nrSamples);
	   alignas(64) float *imagTable = dynVec (float, nrSamples);
	   volk_32fc_deinterleave_32f_x2_u (realTable, imagTable,
                                            buffer, nrSamples);
	   float	I_sum;
	   float	Q_sum;
	   volk_32f_accumulator_s32f_a (&I_sum, realTable, nrSamples);
	   volk_32f_accumulator_s32f_a (&Q_sum, imagTable, nrSamples);
	   I_sum	/= nrSamples;
	   Q_sum	/= nrSamples;
	   sLevel	= average (sLevel,
	                   sqrt (I_sum * I_sum + Q_sum * Q_sum),
	                   Alpha); 
	}
#endif
	static int teller = 0; 
	teller += nrSamples;
	if (teller >= SAMPLERATE) {
	   show_dcOffset (10 * (IQ_Real - IQ_Imag) / 
	                                  ((IQ_Real + IQ_Imag) / 2));
	   teller = 0;
	}

	if (saving) {
	   int samples = localCounter + nrSamples >= bufferSize ?
	                          bufferSize - localCounter :
	                          nrSamples;
	   if (samples > 0) {
	      memcpy (&localBuffer [localCounter], buffer, 
	                             samples * sizeof (std::complex<float>));
	      localCounter += samples;
	   }
	}

#ifndef	__HAVE_VOLK__
	for (int i = 0; i < nrSamples; i ++) {
	   currentPhase	-= std::round (phaseOffset);
	   currentPhase	= (currentPhase + SAMPLERATE) % SAMPLERATE;
	   v_out  [index + i]	=
	           std::complex<float> (real (buffer [i]), imag (buffer [i])) *
	                                oscillatorTable [currentPhase];
	}
#else
	std::complex<float> phaseInc = 
	      cmplx_from_phase (- (float)phaseOffset / SAMPLERATE * 2 * M_PI);
//
//	Note that for U22 parameter is phaseInc rather than &phaseInc
        volk_32fc_s32fc_x2_rotator2_32fc_u (buffer, buffer,
	                                    &phaseInc, &phase, nrSamples);
	memcpy (&v_out [index], buffer,
	                nrSamples * sizeof (std::complex<float>));
#endif
	sampleCount	+= nrSamples;

	if (saving && (spectrumBuffer != nullptr) &&
	             (sampleCount > SAMPLERATE / repetitionCounter)) {
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

