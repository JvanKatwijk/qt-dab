#
/*
 *    Copyright (C) 2013 .. 2024
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
 *
 *	The commented decoders were from Thomas Neder and used in an
 *	experiment, the "shifting" from fftBinRaw to fftBin is from his work
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
#define	ALPHA	0.01f
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
	                                 phaseReference (params. get_T_u ()),
	                                 conjVector (params. get_T_u ()),
	                                 fft_buffer (params. get_T_u ()),
		                         stdDevVector (params. get_T_u ()),
	                                 IntegAbsPhaseVector (params. get_T_u ()),
	                                 meanLevelVector (params. get_T_u ()),
	                                 meanPowerVector (params. get_T_u ()),
	                                 meanSigmaSqVector (params. get_T_u ())
{
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
	this	-> T_g			= T_s - T_u;

	repetitionCounter	= 10;

	reset ();
	iqSelector		= SHOW_DECODED;
	decoder			= DECODER_1;

	nullPower		= 0.1f;
}

	ofdmDecoder::~ofdmDecoder	() {
}
//
void	ofdmDecoder::stop ()	{
}
//
void	ofdmDecoder::reset ()	{
	memset (stdDevVector. data (),		0, T_u * sizeof (DABFLOAT));
	memset (IntegAbsPhaseVector. data (),	0, T_u * sizeof (DABFLOAT));
	memset (meanLevelVector. data (),	0, T_u * sizeof (DABFLOAT));
	memset (meanPowerVector. data (),	0, T_u * sizeof (DABFLOAT));
	memset (meanSigmaSqVector. data (),	0, T_u * sizeof (DABFLOAT));
	meanValue	= 1.0f;
}
//
void	ofdmDecoder::setPowerLevel	(DABFLOAT level) {
	nullPower = level * level;
}

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

	return 0;
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

void	limit_symmetrically (DABFLOAT &v, float limit) {
	if (v < -limit)
	   v = -limit / 2;
	if (v > limit)
	   v = limit / 2;
}
//
//	How to compute a sin or cos for (hopefully) small angles,
//	Some tests showed the using the first few terms of the
//	Taylor series for angles up to PI / 4 were up to  the fourth
//	decimal correct.
//	A more performance oriented solution could be a table,
//	but then, the granularity of the table should be pretty high.
Complex makeComplex (DABFLOAT phase) {
DABFLOAT p2	= phase * phase;
DABFLOAT p3	= p2 * phase;
DABFLOAT p4	= p3 * phase;
DABFLOAT p5	= p4 * phase;
DABFLOAT p6	= p5 * phase;
DABFLOAT sine	= phase - p3 / 6 + p5 / 120;
DABFLOAT cosi	= 1 - p2 / 2 + p4 / 24 - p6 / 720;
	return Complex (cosi, sine);
}

void	ofdmDecoder::decode (std::vector <Complex> &buffer,
	                     int32_t blkno,
	                     std::vector<int16_t> &ibits) {
DABFLOAT sum = 0;

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
  *     decoding is computing the phase difference between
  *     carriers with the same index in subsequent blocks.
  *     The carrier of a block is the reference for the carrier
  *     on the same position in the next block
  */
	   Complex fftBinRaw = fft_buffer [index] *
	                       normalize (conj (phaseReference [index]));
	   conjVector   [index] = fftBinRaw;

//	   Complex fftBin	= fftBinRaw;
	   Complex fftBin	= fftBinRaw *
	                     makeComplex (-IntegAbsPhaseVector [index]);
//	Get the phase (real and absolute) 
	   DABFLOAT	re	= real (fftBin);
	   DABFLOAT	im	= imag (fftBin);
	   if (re < 0)
	      re = -re;
	   if (im < 0)
	      im = -im;
	   Complex	fftBin_at_1	=
	            Complex (re, im);
	   DABFLOAT	binAbsLevel	= jan_abs (fftBin_at_1);
	   IntegAbsPhaseVector [index] = 
	                           0.5f * ALPHA * (arg (fftBin_at_1) - M_PI_4);
	   limit_symmetrically (IntegAbsPhaseVector [index],
	                                RAD_PER_DEGREE * (DABFLOAT)30.0);

/**
  *	When trying alternative decoder implementations
  *	as implemented in DABstar by Rolf Zerr (aka old-dab) and
  *	Thomas Neder (aka) Tomneda,  I wanted to do some investigation
  (	to get  actual figures.
  *	The different decoders were tested with an old file
  *	with a recording of a poor signal, that ran for (almost) exact
  *	two  minutes from the start, and the BER results were accumulated
  *	to get a more or less reliable answer.
  *	It turned out that the major effect on the decoding quality 
  *	was with the phase shifting as done above.
  *	With that setting there turned out to be a marginal difference
  *	between decoders 1 and decoder 4,
  *	the other two decoders 2 and 3 performed
  *	slightly less (roughly speaking app 740000 repairs by
  *	decoder 1 and 4, and 746000 by decoders 2 and 3).
  *	So, we have chosen decoders 1 (most simple one) and 4 (log likelihood)
  *	as decoders here.
  *	the contributions of Rolf Zerr and Thomas Neder
  *	for their decoders is greatly acknowledged
  */
	   DABFLOAT	phaseError	= arg (fftBin_at_1) - M_PI_4;
	   DABFLOAT	stdDev		= phaseError * phaseError;
	   stdDevVector [index] =
	        compute_avg (stdDevVector [index], stdDev, ALPHA);
//
	   meanLevelVector [index] =
	         compute_avg (meanLevelVector[index],
	                                 binAbsLevel, ALPHA);
	   meanPowerVector [index] =
	            compute_avg (meanPowerVector [index],
	                                 binAbsLevel * binAbsLevel, ALPHA);
	   DABFLOAT meanLevelPerBin	= meanLevelVector [index] / sqrt (2);

//	x distance to reference point
	   DABFLOAT x_distance		= 
	                             abs (real (fftBin)) - meanLevelPerBin;
//	y distance to reference point
	   DABFLOAT y_distance		=
	                             abs (imag (fftBin)) - meanLevelPerBin;

	   DABFLOAT sigmaSq		= x_distance * x_distance +
	                                  y_distance * y_distance;
	   meanSigmaSqVector [index]	=
	             compute_avg (meanSigmaSqVector [index], sigmaSq, ALPHA);

	   DABFLOAT signalPower		= meanPowerVector [index] - nullPower;

	   if (signalPower <= 0.0f)
	      signalPower = 0.1f;

	   DABFLOAT snr		= signalPower / nullPower;
	   DABFLOAT ff 		=  meanLevelVector [index] /
	                                         meanSigmaSqVector [index];
	   ff /= 1 / snr + 2;
	   Complex R1;
	   
	   DABFLOAT weight_r;
	   DABFLOAT weight_i;
	   switch (decoder) {
	      default:
	      case DECODER_1:
	         R1 =  normalize (fftBin) * ff *
	                   (DABFLOAT)(sqrt (jan_abs (fftBin) * jan_abs (phaseReference [index])));
	         weight_r = weight_i	= -100 / meanValue;
	         ibits [i]		= (int16_t)(real (R1) * weight_r);
	         ibits [carriers + i]	= (int16_t)(imag (R1) * weight_i);
	         break;
	      case DECODER_2:
	         R1		= fftBin;
	         ibits [i]
	                   = - real (R1) / meanLevelPerBin * MAX_VITERBI;
	         ibits [carriers + i]
	                   = - imag (R1) / meanLevelPerBin * MAX_VITERBI; 
	         break;
	   }

	   sum += jan_abs (R1);
	}	// end of decode loop

	meanValue	= sum / carriers;

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

	      float freqOffset	= compute_frequencyOffset (fft_buffer. data (),
	                                              phaseReference. data ());
	      if (devBuffer != nullptr) {
	         float tempVector [carriers];
	         for (int i = 0; i < carriers; i ++) {
	            tempVector [i] =
	                  stdDevVector [(T_u - carriers / 2 + i) % T_u];
	            tempVector [i] = tempVector [i] /  M_PI * 180.0;
	         }

	         devBuffer -> putDataIntoBuffer (tempVector, carriers);
	         show_stdDev (carriers);
	      }

	      showIQ (carriers);
	      float Quality	= computeQuality (conjVector. data ());
	      float timeOffset	= compute_timeOffset (fft_buffer. data (),
	                                              phaseReference. data ());
//	      float freqOffset	= compute_frequencyOffset (fft_buffer. data (),
//	                                              phaseReference. data ());
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
//
//	Ideally, the processed carrier should have a value
//	equal to (2 * k + 1) * PI / 4
//	The offset is a measure of the frequency "error"
float	ofdmDecoder::compute_frequencyOffset (Complex *r,
	                                      Complex *c) {

Complex theta = Complex (0, 0);
static float vv	=  0;

	for (int i = - carriers / 2; i < carriers / 2; i += 6) {
	   int index = i < 0 ? i + T_u : i;
	   Complex val = r [index] * conj (c [index]);
	   val		= Complex (abs (real (val)), abs (imag (val)));
	   theta	+= val * Complex (1, -1);
	}

	float uu =  arg (theta) / (2 * M_PI) * 2048000 / T_u;
	vv	= 0.9 * vv + 0.1 * abs (uu);;
	return vv;
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
	
