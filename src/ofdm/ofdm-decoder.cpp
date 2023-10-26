#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
 *
 *	Once the bits are "in", interpretation and manipulation
 *	should reconstruct the data blocks.
 *	Ofdm_decoder is called for Block_0 and the FIC blocks,
 *	its invocation results in 2 * Tu bits
 */
#include	<vector>
#include	"ofdm-decoder.h"
#include	"radio.h"
#include	"phasetable.h"
#include	"fic-handler.h"
#include	"msc-handler.h"
#include	"freq-interleaver.h"
#include	"dab-params.h"
/**
  *	\brief ofdmDecoder
  *	The class ofdmDecoder is
  *	taking the data from the ofdmProcessor class in, and
  *	will extract the Tu samples, do an FFT and extract the
  *	carriers and map them on (soft) bits
  */

	ofdmDecoder::ofdmDecoder	(RadioInterface *mr,
	                                 uint8_t	dabMode,
	                                 int16_t	bitDepth,
	                                 RingBuffer<float>   *devBuffer_i,
	                                 RingBuffer<Complex> *iqBuffer_i) :
	                                    myRadioInterface (mr),
	                                    params (dabMode),
	                                    myMapper (dabMode),
	                                    fft (params. get_T_u (), false),
	                                    devBuffer (devBuffer_i),
	                                    iqBuffer (iqBuffer_i) {
	(void)bitDepth;
	connect (this, SIGNAL (showIQ (int)),
	         myRadioInterface, SLOT (showIQ (int)));
	connect (this, SIGNAL (show_quality (float, float, float)),
	         myRadioInterface, SLOT (show_quality (float, float, float)));
	connect (this, SIGNAL (show_stdDev (int)),
	         myRadioInterface, SLOT (show_stdDev (int)));
//
	this	-> T_s			= params. get_T_s	();
	this	-> T_u			= params. get_T_u	();
	this	-> nrBlocks		= params. get_L		();
	this	-> carriers		= params. get_carriers	();

	this	-> T_g			= T_s - T_u;
	phaseReference			.resize (T_u);
	offsetVector. resize (T_u);
	for (int i = 0; i < offsetVector. size (); i ++)
           offsetVector [i] = 0;

	iqSelector			= SHOW_DECODED;
}

	ofdmDecoder::~ofdmDecoder	() {
}
//
void	ofdmDecoder::stop ()	{
}

void	ofdmDecoder::reset ()	{
	for (int i = 0; i < offsetVector. size (); i ++)
	   offsetVector [i] = 0;
}
//
//
void	ofdmDecoder::processBlock_0 (
	                std::vector <Complex> buffer) {
	fft. fft (buffer);
/**
  *	we are now in the frequency domain, and we keep the carriers
  *	as coming from the FFT as phase reference.
  */
	memcpy (phaseReference. data (), buffer. data (),
	                   T_u * sizeof (Complex));
}
//
//	Just interested. In the ideal case the constellation of the
//	decoded symbols is precisely in the four points 
//	k * (1, 1), k * (1, -1), k * (-1, -1), k * (-1, 1)
//	To ease computation, we map all incoming values onto quadrant 1

float	ofdmDecoder::computeQuality (Complex *v) {
Complex XX  [carriers];

//
//	since we do not equalize, we have a kind of "fake"
//	reference point.
//
	Complex middle = Complex (0, 0);
	for (int i = 0; i < carriers; i ++) {
	   std::complex<float> ss = v [T_u / 2 - carriers / 2 + i];
	   XX [i] = Complex (abs (real (ss)), abs (imag (ss)));
	   middle += XX [i];
	}
	middle = middle / (float)carriers;
	middle	= Complex (real (middle) + imag (middle) / 2,
	                   real (middle) + imag (middle) / 2);
//	middle	= Complex (1, 1);
	float nominator		= 0;
	float denominator	= 0;
	for (int i = 0; i < carriers; i ++) {
	   float I_component	= real (v [T_u / 2 - carriers / 2 + i]);
	   float Q_component	= imag (v [T_u / 2 - carriers / 2 + i]);
	   float delta_I	= abs (I_component) - real (middle);
	   float delta_Q	= abs (Q_component) - imag (middle);
	   nominator	+= square (I_component) + square (Q_component);
	   denominator	+= square (delta_I) + square (delta_Q);
	}
	return 20 * log10 (nominator / denominator);
}
/**
  *	for the other blocks of data, the first step is to go from
  *	time to frequency domain, to get the carriers.
  *	we distinguish between FIC blocks and other blocks,
  *	only to spare a test. The mapping code is the same
  */

static inline
float	constrain	(float val, float min, float max) {
	if (val > max)
	   return max;
	if (val < min)
	   return min;
	return val;
}

static	int	cnt	= 0;

#define DELTA	0.05
//
//	DAB (and DAB+) bits are encoded is DPSK, 2 bits per carrier,
//	depending on the quadrant the carrier is in. There are
//	of course two different approaches in decoding the bits
//	One is looking at the X and Y components, and 
//	their length, relative to each other,
//	Ideally, the X and Y are of equal size, in practice they are not.

void	ofdmDecoder::decode (std::vector <Complex> &buffer,
	                     int32_t blkno,
	                     std::vector<int16_t> &ibits) {
Complex conjVector [T_u];
Complex fft_buffer [T_u];

	memcpy (fft_buffer, &((buffer. data()) [T_g]),
	                               T_u * sizeof (std::complex<float>));

//fftlabel:
//	first step: do the FFT
	fft. fft (fft_buffer);

//	a little optimization: we do not interchange the
//	positive/negative frequencies to their right positions.
//	The de-interleaving understands this

	float max	= 0;
	for (int i = 0; i < carriers; i ++) {
	   int16_t	index	= myMapper.  mapIn (i);
	   if (index < 0) 
	      index += T_u;
/**
  *	decoding is computing the phase difference between
  *	carriers with the same index in subsequent blocks.
  *	The carrier of a block is the reference for the carrier
  *	on the same position in the next block
  */
	   Complex	r1 = fft_buffer [index] *
	                                    conj (phaseReference [index]);
	   conjVector	[index] = r1;

//	we need the phase error to compute quality
	   Complex r2		= Complex (abs (real (r1)), abs (imag (r1)));
	   max	+= jan_abs (r2);
//
//	Note that the phase Offset does not lead to an accumulated error
//	so, we just average 
	   float phaseOffset	= arg (r2 * Complex (1, -1));
	   offsetVector [index] = 
	             compute_avg (offsetVector [index],
	                                      square (phaseOffset), DELTA);
	
//	The approach taken  decoding approach is
//	looking at the X and Y coordinates of the "dots"
//	and taking their size as element.
	   float ab1	= jan_abs (r1);
	   ibits [i]	=  (int16_t)(- (real (r1)  * 127.0) / ab1);
	   ibits [carriers + i] =  (int16_t)(- (imag (r1) * 127) / ab1);
	}


//	From time to time we show the constellation of symbol 2.
	
	if (blkno == 2) {
	   if (++cnt > 8) {
	      max /= carriers;
	      Complex displayVector [carriers];
	      if (iqSelector == SHOW_RAW) {
	         float maxAmp = 0;
	         for (int j = -carriers / 2; j < carriers / 2; j ++)
	            if (j != 0)
	               if (abs (fft_buffer [(T_u + j) % T_u]) > maxAmp)
	                  maxAmp = abs (fft_buffer [(T_u + j) % T_u]);
	         for (int j = 0; j < carriers; j ++)
	            displayVector [j] =
	              fft_buffer [(T_u - carriers / 2 - 1 + j) % T_u] / maxAmp;
	      }
	      else {
	         for (int j = 1; j < carriers; j ++)
	            displayVector [j] =
	                      conjVector [T_u / 2 - carriers / 2 + j] / max; 
	      }
	      iqBuffer -> putDataIntoBuffer (displayVector, carriers);

	      if (devBuffer != nullptr) {
	         float tempVector [carriers];
	         for (int i = 0; i < carriers; i ++) {
	            tempVector [i] =
	                  offsetVector [(T_u - carriers / 2 + i) % T_u];
	            tempVector [i] = tempVector [i] /  M_PI * 180.0;
	         }
	         devBuffer -> putDataIntoBuffer (tempVector, carriers);
	         show_stdDev (carriers);
	      }

	      showIQ (carriers);
	      float Quality	= computeQuality (conjVector);
	      float timeOffset	= compute_timeOffset (fft_buffer,
	                                              phaseReference. data ());
	      float freqOffset	= compute_frequencyOffset (fft_buffer,
	                                              phaseReference. data ());
	      show_quality (Quality, timeOffset, freqOffset);

	      cnt = 0;
	   }
	}
	memcpy (phaseReference. data(), fft_buffer,
	                            T_u * sizeof (Complex));
}

//
//	While DAB symbols do not carry pilots, it is known that
//	arg (carrier [i, j] * conj (carrier [i + 1, j])
//	should be K * M_PI / 4,  (k in {1, 3, 5, 7}) so basically
//	carriers in decoded symbols can be used as if they were pilots
//
//	so, with that in mind we experiment with formula 5.39
//	and 5.40 from "OFDM Baseband Receiver Design for Wireless
//	Communications (Chiueh and Tsai)"
float	ofdmDecoder::compute_timeOffset (Complex *r, Complex *v) {
Complex sum	= Complex (0, 0);

	for (int i = -carriers / 2; i < carriers / 2; i += 6) {
	   int index_1 = i < 0 ? i + T_u : i;
	   int index_2 = (i + 1) < 0 ? (i + 1) + T_u : (i + 1);
	   Complex s = r [index_1] * conj (v [index_2]);
	   s = Complex (abs (real (s)), abs (imag (s)));
	   Complex leftTerm = s * conj (Complex (abs (s) / sqrt (2),
	                                                 abs (s) / sqrt (2)));
	   s = r [index_2] * conj (v [index_2]);
	   s = Complex (abs (real (s)), abs (imag (s)));
	   Complex rightTerm = s * conj (Complex (abs (s) / sqrt (2),
	                                                 abs (s) / sqrt (2)));
	   sum += conj (leftTerm) * rightTerm;
	}

	return arg (sum);
}

float	ofdmDecoder::compute_frequencyOffset (Complex *r,
	                                      Complex *c) {

Complex theta = Complex (0, 0);

	for (int i = - carriers / 2; i < carriers / 2; i += 6) {
	   int index = i < 0 ? i + T_u : i;
	   Complex val = r [index] * conj (c [index]);
	   val		= Complex (abs (real (val)),
	                                       abs (imag (val)));
	   theta	+= val * Complex (1, -1);
	}

	return arg (theta) / (2 * M_PI) * 2048000 / T_u;
}

float	ofdmDecoder::compute_clockOffset (Complex *r,
	                                  Complex *v) {
float	offsa	= 0;
int	offsb	= 0;

	for (int i = - carriers / 2; i < carriers / 2; i += 6) {
	   int index = i < 0 ? (i + T_u) : i;
	   int index_2 = i + carriers / 2;
	   Complex a1 =
	              Complex (abs (real (r [index])),
	                                   abs (imag (r [index])));
	   Complex a2 =
	              Complex (abs (real (v [index])),
	                                   abs (imag (v [index])));
	   float s = abs (arg (a1 * conj (a2)));
	   offsa += index * s;
	   offsb += index_2 * index_2;
	}
	
	float sampleClockOffset = 
	           offsa / (2 * M_PI * (float)T_s/ T_u * offsb);

	return sampleClockOffset;
}

void	ofdmDecoder::handle_iqSelector	() {
	if (iqSelector == SHOW_RAW)
	   iqSelector = SHOW_DECODED;
	else
	   iqSelector = SHOW_RAW;
}

