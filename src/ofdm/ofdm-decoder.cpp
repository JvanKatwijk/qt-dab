#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
	connect (this, SIGNAL (showQuality (float)),
	         myRadioInterface, SLOT (showQuality (float)));
//
	this	-> T_s			= params. get_T_s();
	this	-> T_u			= params. get_T_u();
	this	-> nrBlocks		= params. get_L();
	this	-> carriers		= params. get_carriers();

	this	-> T_g			= T_s - T_u;
	fft_buffer			= my_fftHandler. getVector();
	phaseReference			.resize (T_u);

	connect (this, SIGNAL (show_snr (int)),
	         mr, SLOT (show_snr (int)));
	snrCount		= 0;
	snr			= 0;	
}

	ofdmDecoder::~ofdmDecoder() {
}
//
void	ofdmDecoder::stop() {
}

void	ofdmDecoder::reset() {
}

/**
  */
void	ofdmDecoder::processBlock_0 (std::vector <std::complex<float> > buffer) {
	memcpy (fft_buffer, buffer. data(),
	                             T_u * sizeof (std::complex<float>));

	my_fftHandler. do_FFT();
/**
  *	The SNR is determined by looking at a segment of bins
  *	within the signal region and bits outside.
  *	It is just an indication
  */

	if (++snrCount > 10) {
	   snr	= 0.8 * snr + 0.2 * get_snr (fft_buffer);
//	   show_snr (snr);
	   snrCount = 0;
	}
/**
  *	we are now in the frequency domain, and we keep the carriers
  *	as coming from the FFT as phase reference.
  */
	memcpy (phaseReference. data(), fft_buffer,
	                   T_u * sizeof (std::complex<float>));
}
//
//	Just interested. In the ideal case the constellation of the
//	decoded symbols is precisely in the four points 
//	k * (1, 1), k * (1, -1), k * (-1, -1), k * (-1, 1)
//	To ease computation, we map all incoming values onto quadrant 1
float	ofdmDecoder::computeQuality (std::complex<float> *v) {
int16_t i;
std::complex<float>	avgPoint	= std::complex<float> (0, 0);
std::complex<float>	x [T_u];
float	avg	= 0;
float	S	= 0;

	for (i = 0; i < carriers; i ++) {
	   x [i]	= std::complex<float> (abs (real (v [T_u / 2 - carriers / 2 + i])), abs (imag (v [T_u / 2 - carriers / 2 + i])));
	   avgPoint	+= x [i];
	}

	avg	= arg (avgPoint * conj (std::complex<float> (1, 1)));

	for (i = 0; i < carriers; i ++) {
	   float f = arg (x [i] * conj (std::complex<float> (1, 1))) - avg;
	   f = f / M_PI * 360;
	   S += f * f;
	}

	S /= (carriers - 1);
	
	return sqrt (S);
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
	   float ab1	= jan_abs (r1);
//	split the real and the imaginary part and scale it
//	we make the bits into softbits in the range -127 .. 127
	   ibits [i]		=  - real (r1) / ab1 * 127.0;
	   ibits [carriers + i] =  - imag (r1) / ab1 * 127.0;
	}

	memcpy (phaseReference. data(), fft_buffer,
	                            T_u * sizeof (std::complex<float>));
//	From time to time we show the constellation of symbol 2.
	if (blkno == 2) {
	   if (++cnt > 7) {
	      iqBuffer	-> putDataIntoBuffer (&conjVector [T_u / 2 - carriers / 2],
	                                      carriers);
	      showIQ	(carriers);
	      showQuality (computeQuality (conjVector));
	      cnt = 0;
	   }
	}
}
/**
  *	for the snr we have a full T_u wide vector, with in the middle
  *	K carriers.
  *	Just get the strength from the selected carriers compared
  *	to the strength of the carriers outside that region
  */
int16_t	ofdmDecoder::get_snr (std::complex<float>  *v) {
int16_t	i;
float	noise 	= 0;
float	signal	= 0;

	for (i = -100; i < 100; i ++)
	   noise += abs (v [(T_u / 2 + i)]);

	noise	/= 200;
	for (i =  - carriers / 4;  i <  carriers / 4; i ++)
	   signal += abs (v [(T_u + i) % T_u]);
	signal	/= (carriers / 2);

	return 20 * log10 ((signal + 0.005) / (noise + 0.005));
}


