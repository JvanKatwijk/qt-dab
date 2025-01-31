#
/*
 *    Copyright (C) 2013 .. 2023
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
#include	"fic-handler.h"
#include	"msc-handler.h"
#include	"freq-interleaver.h"
#include	"dab-params.h"
#include	"dab-constants.h"
/**
  *	\brief ofdmDecoder
  *	The class ofdmDecoder is
  *	taking the data from the ofdmProcessor class in, and
  *	will extract the Tu samples, do an FFT and extract the
  *	carriers and map them on (soft) bits
  */

#define	Alpha	0.05f
static inline
Complex	normalize (const Complex &V) {
float Length	= jan_abs (V);
	return Length == 0.0f ? Complex (0.0, 0.0) : V / (DABFLOAT)Length;
}

	ofdmDecoder::ofdmDecoder	(RadioInterface *mr,
	                                 uint8_t	dabMode,
	                                 int16_t	bitDepth,
	                                 RingBuffer<float>   *devBuffer_i,
	                                 RingBuffer<Complex> *iqBuffer_i) :
	                                    myRadioInterface (mr),
	                                    params (dabMode),
	                                    theTable (dabMode),
	                                    myMapper (dabMode),
	                                    fft (params. get_T_u (), false),
	                                    devBuffer (devBuffer_i),
	                                    iqBuffer (iqBuffer_i),
	                                    conjVector (params. get_T_u ()),
	                                    fft_buffer (params. get_T_u ()) {
	(void)bitDepth;
	connect (this, &ofdmDecoder::showIQ,
	         myRadioInterface, &RadioInterface::showIQ);
	connect (this, &ofdmDecoder::show_quality,
	         myRadioInterface, &RadioInterface::show_quality);
	connect (this, &ofdmDecoder::show_stdDev,
	         myRadioInterface, &RadioInterface::show_stdDev);
//
	this	-> T_s			= params. get_T_s	();
	this	-> T_u			= params. get_T_u	();
	this	-> nrBlocks		= params. get_L		();
	this	-> carriers		= params. get_carriers	();

	repetitionCounter	= 10;
	this	-> T_g		= T_s - T_u;
	phaseReference		.resize (T_u);
	offsetVector.		resize (T_u);
	carrierCenters.		resize (T_u);

	for (int32_t i = 0; i < T_u; i ++) {
	   offsetVector		[i] = 0;
	   carrierCenters	[i] = Complex (1, 1);
	}

	meanValue		= 1.0f;
	iqSelector		= SHOW_DECODED;
	decoder			= DECODER_1;
}

	ofdmDecoder::~ofdmDecoder	() {
}
//
void	ofdmDecoder::stop ()	{
}

void	ofdmDecoder::reset ()	{
	for (int i = 0; i <  T_u; i ++) {
	   offsetVector		[i] = 0;
	   carrierCenters	[i] = Complex (1, 1);
	}
}
//
//
float	ofdmDecoder::processBlock_0 (
	                std::vector <Complex> buffer, bool withTII) {
	fft. fft (buffer);
/**
  *	we are now in the frequency domain, and we keep the carriers
  *	as coming from the FFT as phase reference.
  */
	memcpy (phaseReference. data (), buffer. data (),
	                                      T_u * sizeof (Complex));

	Complex temp	= Complex (0, 0);;
	if (withTII) {
	   return arg (temp);
	}
	
	for (int i = 0; i < carriers; i ++) {
	   int16_t	index	= myMapper.  mapIn (i);
	   if (index < 0) 
	      index += T_u;
	}
	return arg (temp);
}
//
//	Just interested. In the ideal case the constellation of the
//	decoded symbols is precisely in the four points 
//	k * (1, 1), k * (1, -1), k * (-1, -1), k * (-1, 1)
//	To ease computation, we map all incoming values onto quadrant 1
//
//	For the computation of the MER we use the definition
//	from ETSI TR 101 290 (appendix C1)
float	ofdmDecoder::computeQuality (Complex *v) {
static float f_n = 1;
static float f_d = 1;
	for (int i = 0; i < carriers; i ++) {
	   Complex ss	= v [T_u / 2 - carriers / 2 + i];
	   float ab	= jan_abs (ss) / sqrt (2);
	   f_n		=  0.99 * f_n + 0.01 * (jan_abs (ss) * jan_abs (ss));
	   float R	= abs (abs (real (ss)) - ab);
	   float I	= abs (abs (imag (ss)) - ab);
	   f_d		= 0.99 * f_d + 0.01 * (R * R + I * I);
	}
	return 10 * log10 (f_n / f_d + 0.1);
}
/**
  *	for the other blocks of data, the first step is to go from
  *	time to frequency domain, to get the carriers.
  *	we distinguish between FIC blocks and other blocks,
  *	only to spare a test. The mapping code is the same
  */

static	int	cnt	= 0;

//
//	DAB (and DAB+) bits are encoded is DPSK, 2 bits per carrier,
//	depending on the quadrant the carrier is in. There are
//	of course two different approaches in decoding the bits
//	One is looking at the X and Y components, and 
//	their length, relative to each other,
//	Ideally, the X and Y are of equal size, in practice they are not.

int	sign (DABFLOAT x) {
	return x < 0 ? -1 : x > 0 ? 1 : 0;
}

void	ofdmDecoder::decode (std::vector <Complex> &buffer,
	                     int32_t blkno,
	                     std::vector<int16_t> &ibits) {
float sum = 0;

	memcpy (fft_buffer. data (), &((buffer. data ()) [T_g]),
	                               T_u * sizeof (Complex));
//	first step: do the FFT
	fft. fft (fft_buffer. data ());

//	a little optimization: we do not interchange the
//	positive/negative frequencies to their right positions.
//	The de-interleaving understands this
//	DABFLOAT max	= 0;

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
	                     normalize (conj (phaseReference [index]));
	   conjVector	[index] = r1;

	   DABFLOAT ab1	= jan_abs (r1);

	   DABFLOAT fftPhase	= 
	            arg (Complex (abs (real (r1)),  abs (imag (r1))));
	   DABFLOAT phaseOffset	= M_PI_4 - fftPhase;
	   offsetVector [index] = 
	           compute_avg (offsetVector [index], phaseOffset, Alpha);
	   carrierCenters [index] =
	            Complex (
	                    compute_avg (real (carrierCenters [index]),
	                                         abs (real (r1)), Alpha),
	                    compute_avg (imag (carrierCenters [index]),
	                                         abs (imag (r1)), Alpha));
	   DABFLOAT	weight_x = 0;
	   DABFLOAT	weight_y = 0;
//
	   switch (decoder) {
//
//	Decoder 1 is the most simple one,  just compute the relative strength of
//	x and y coordinate, related to a fictitious midddle
	      case DECODER_1:
	      default:		// should not happen
	         weight_x	= MAX_VITERBI / ab1;
	         weight_y	= MAX_VITERBI / ab1;
	         ibits [i]	= -sign (real (r1)) *
	                                abs (real (r1)) * weight_x; 
	         ibits [carriers + i]	= -sign (imag (r1)) *
	                                abs (imag (r1)) * weight_y; 
	      break;

	      case DECODER_2:
//	here we look at the error of the sample wrt a filtered "centerpoint"
//	and give the X and Y the error as penalty
//	works actually as best of the three
	      {	 Complex   base	= carrierCenters [index];
	         weight_x	= MAX_VITERBI / real (base);
	         weight_y	= MAX_VITERBI / imag (base);
	         ibits [i]	= -sign (real (r1)) *
	                                abs (real (r1)) * weight_x; 
	         ibits [carriers + i]	= -sign (imag (r1)) *
	                                abs (imag (r1)) * weight_y; 
	         break;
	      }
	   }
	}

//	From time to time we show the constellation of symbol 2.
	if (blkno == 2) {
	   if (++cnt > repetitionCounter) {
	      DABFLOAT maxAmp = 0;
	      for (int j = -carriers / 2; j < carriers / 2; j ++)
	         if (j != 0)
	            if (jan_abs (fft_buffer [(T_u + j) % T_u]) > maxAmp)
	               maxAmp = jan_abs (fft_buffer [(T_u + j) % T_u]);
	      Complex displayVector [carriers];

	      if (iqSelector == SHOW_RAW) {
	         for (int j = 0; j < carriers; j ++)
	            displayVector [j] =
	              fft_buffer [(T_u - carriers / 2 - 1 + j) % T_u] / maxAmp;
	      }
	      else {
	         for (int j = 1; j < carriers; j ++) {
	            displayVector [j] =
	                      conjVector [T_u / 2 - carriers / 2 + j] / maxAmp; 
	         }
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
	      float Quality	= computeQuality (conjVector. data ());
	      float timeOffset	= compute_timeOffset (fft_buffer. data (),
	                                              phaseReference. data ());
	      float freqOffset	= compute_frequencyOffset (fft_buffer. data (),
	                                              phaseReference. data ());
	      show_quality (Quality, timeOffset, freqOffset);
	      cnt = 0;
	   }
	}
	memcpy (phaseReference. data(), fft_buffer. data (),
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

void	ofdmDecoder::handle_decoderSelector	(int decoder) {
	this	-> decoder	= decoder;
}

