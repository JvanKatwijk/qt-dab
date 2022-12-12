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
	                                 RingBuffer<std::complex<float>> *iqBuffer) :
	                                    params (dabMode),
	                                    my_fftHandler (dabMode),
	                                    myMapper (dabMode) {
	this	-> myRadioInterface	= mr;
	this	-> iqBuffer		= iqBuffer;
	connect (this, SIGNAL (showIQ (int)),
	         myRadioInterface, SLOT (showIQ (int)));
	connect (this, SIGNAL (showQuality (float, float, float, float)),
	         myRadioInterface, SLOT (showQuality (float, float, float, float)));
//
	this	-> T_s			= params. get_T_s();
	this	-> T_u			= params. get_T_u();
	this	-> nrBlocks		= params. get_L();
	this	-> carriers		= params. get_carriers();

	this	-> T_g			= T_s - T_u;
	fft_buffer			= my_fftHandler. getVector();
	phaseReference			.resize (T_u);
}

	ofdmDecoder::~ofdmDecoder() {
}
//
void	ofdmDecoder::stop () {
}

void	ofdmDecoder::reset () {
}

/**
  */
void	ofdmDecoder::processBlock_0 (
	                std::vector <std::complex<float> > buffer) {
	memcpy (fft_buffer, buffer. data(),
	                             T_u * sizeof (std::complex<float>));

	my_fftHandler. do_FFT();
/**
  *	we are now in the frequency domain, and we keep the carriers
  *	as coming from the FFT as phase reference.
  */
	memcpy (phaseReference. data (), fft_buffer,
	                   T_u * sizeof (std::complex<float>));
}
//
//	Just interested. In the ideal case the constellation of the
//	decoded symbols is precisely in the four points 
//	k * (1, 1), k * (1, -1), k * (-1, -1), k * (-1, 1)
//	To ease computation, we map all incoming values onto quadrant 1
static inline
float square (float v) {
	return v * v;
}

float	ofdmDecoder::computeQuality (std::complex<float> *v) {
int16_t i;
std::complex<float>	avgPoint	= std::complex<float> (0, 0);
float		absVal			= 0;
std::complex<float>	x [T_u];
float	nominator	= 0;
float	denominator	= 0;
//
//	since we do not equalize, we have a kind of "fake"
//	reference point. We know that the ideal point 
//	is (x, x, where x = abs (I, Q) / sqrt (2);

	for (i = 0; i < carriers; i ++) {
	   x [i]	= std::complex<float> (abs (real (v [T_u / 2 - carriers / 2 + i])), abs (imag (v [T_u / 2 - carriers / 2 + i])));
	   avgPoint	+= x [i];
	}

	avgPoint	= cdiv (avgPoint, carriers);
	absVal		= abs (avgPoint) / sqrt (2);

	for (i = 0; i < carriers; i ++) {
	   nominator	+= square (absVal) + square (absVal);
	   denominator	+= square (absVal - abs (real (x [i]))) +
	                   square (absVal - abs (imag (x [i])));
	}

	return 20 * log10 (1.0 + nominator / denominator);
}
/**
  *	for the other blocks of data, the first step is to go from
  *	time to frequency domain, to get the carriers.
  *	we distinguish between FIC blocks and other blocks,
  *	only to spare a test. The mapping code is the same
  */

static	int	cnt	= 0;
void	ofdmDecoder::decode (std::vector <std::complex<float>> buffer,
	                     int32_t blkno, int16_t *ibits) {
int16_t	i;
std::complex<float> conjVector [T_u];

	memcpy (fft_buffer, &((buffer. data()) [T_g]),
	                               T_u * sizeof (std::complex<float>));

//fftlabel:
/**
  *	first step: do the FFT
  */
	my_fftHandler. do_FFT();
/**
  *	a little optimization: we do not interchange the
  *	positive/negative frequencies to their right positions.
  *	The de-interleaving understands this
  */
//toBitsLabel:
/**
  *	Note that from here on, we are only interested in the
  *	"carriers", the useful carriers of the FFT output
  */
	for (i = 0; i < carriers; i ++) {
	   int16_t	index	= myMapper.  mapIn (i);
	   if (index < 0) 
	      index += T_u;
/**
  *	decoding is computing the phase difference between
  *	carriers with the same index in subsequent blocks.
  *	The carrier of a block is the reference for the carrier
  *	on the same position in the next block
  */
	   std::complex<float>	r1 = fft_buffer [index] *
	                                    conj (phaseReference [index]);
	   conjVector [index] = r1;
	   float ab1	= abs (r1);
//	split the real and the imaginary part and scale it
//	we make the bits into softbits in the range -127 .. 127
	   ibits [i]		=  - (real (r1) * 255) / ab1;
	   ibits [carriers + i] =  - (imag (r1) * 255) / ab1;
	}

//	From time to time we show the constellation of symbol 2.
	
	if (blkno == 2) {
	   if (++cnt > 7) {
	      iqBuffer	-> putDataIntoBuffer (&conjVector [T_u / 2 - carriers / 2],
	                                      carriers);
	      showIQ	(carriers);
	      float Quality	= computeQuality (conjVector);
	      float timeOffset	= compute_timeOffset (fft_buffer,
	                                              phaseReference. data ());
	      float sco		= compute_clockOffset (fft_buffer,
	                                              phaseReference. data ());
	      float freqOffset	= compute_frequencyOffset (fft_buffer,
	                                              phaseReference. data ());
	      showQuality (Quality, timeOffset, sco, freqOffset);
	      cnt = 0;
	   }
	}

	memcpy (phaseReference. data(), fft_buffer,
	                            T_u * sizeof (std::complex<float>));
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
float	ofdmDecoder::compute_timeOffset (std::complex<float> *r,
	                                      std::complex<float> *v) {
std::complex<float> leftTerm;
std::complex<float> rightTerm;
std::complex<float> sum	= std::complex<float> (0, 0);

	for (int i = -carriers / 2; i < carriers / 2; i += 6) {
	   int index_1 = i < 0 ? i + T_u : i;
	   int index_2 = (i + 1) < 0 ? (i + 1) + T_u : (i + 1);
	   std::complex<float> s = r [index_1] * conj (v [index_2]);
	   s = std::complex<float> (abs (real (s)), abs (imag (s)));
	   leftTerm	= s * conj (std::complex<float> (abs (s) / sqrt (2),
	                                                 abs (s) / sqrt (2)));
	   s = r [index_2] * conj (v [index_2]);
	   s = std::complex<float> (abs (real (s)), abs (imag (s)));
	   rightTerm	= s * conj (std::complex<float> (abs (s) / sqrt (2),
	                                                 abs (s) / sqrt (2)));
	   sum += conj (leftTerm) * rightTerm;
	}

	return arg (sum);
}

float	ofdmDecoder::compute_frequencyOffset (std::complex<float> *r,
	                                      std::complex<float> *c) {

std::complex<float> theta = std::complex<float> (0, 0);

	for (int i = - carriers / 2; i < carriers / 2; i += 6) {
	   int index = i < 0 ? i + T_u : i;
	   std::complex<float> val = r [index] * conj (c [index]);
	   val		= std::complex<float> (abs (real (val)),
	                                       abs (imag (val)));
	   theta	+= val * std::complex<float> (1, -1);
	}

	return arg (theta) / (2 * M_PI) * 2048000 / T_u;
}

float	ofdmDecoder::compute_clockOffset (std::complex<float> *r,
	                                  std::complex<float> *v) {
float	offsa	= 0;
int	offsb	= 0;

	for (int i = - carriers / 2; i < carriers / 2; i += 6) {
	   int index = i < 0 ? (i + T_u) : i;
	   int index_2 = i + carriers / 2;
	   std::complex<float> a1 =
	              std::complex<float> (abs (real (r [index])),
	                                   abs (imag (r [index])));
	   std::complex<float> a2 =
	              std::complex<float> (abs (real (v [index])),
	                                   abs (imag (v [index])));
	   float s = abs (arg (a1 * conj (a2)));
	   offsa += index * s;
	   offsb += index_2 * index_2;
	}
	
	float sampleClockOffset = 
	           offsa / (2 * M_PI * (float)T_s/ T_u * offsb);

	return sampleClockOffset;
}


