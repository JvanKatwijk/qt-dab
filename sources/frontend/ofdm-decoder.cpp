#
/*
 *    Copyright (C) 2014 .. 2024
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
#define	ALPHA	0.005f
static inline
Complex	normalize (const Complex &V) {
DABFLOAT length	= jan_abs (V);
	if (length < 0.001)
	   return Complex (0, 0);
	return Complex (V) / length;
}
//
//	The bessel function is under windows too slow too work with
//	that is why we created a table that is filled on startup
DABFLOAT besselTable [1024];
static inline
DABFLOAT IO_Bessel	(DABFLOAT x) {
	return std::cyl_bessel_i (0.0f, x);
}

// and table access is with this function
static inline
DABFLOAT IO (DABFLOAT x) {
	return besselTable [((int)(x * 32)) % 1024];
}

static inline
Complex w (DABFLOAT kn) {
	DABFLOAT re	= cos (kn * M_PI / 4);
	DABFLOAT im	= sin (kn * M_PI / 4);
	return Complex (re, im);
}

static Complex W_table [8];
static inline
DABFLOAT makeA (int i, Complex S, Complex prevS) {
	return abs  (prevS + W_table [i] * S);
//	return abs  (prevS + w (-i) * S);
}

#include	<bit>	//C++20
//fast_log abs(rel) : avgError = 2.85911e-06(3.32628e-08), MSE = 4.67298e-06(5.31012e-08), maxError = 1.52588e-05(1.7611e-07)
const float s_log_C0 = -19.645704f;
const float s_log_C1 = 0.767002f;
const float s_log_C2 = 0.3717479f;
const float s_log_C3 = 5.2653985f;
const float s_log_C4 = -(1.0f + s_log_C0) * (1.0f + s_log_C1) / ((1.0f + s_log_C2) * (1.0f + s_log_C3)); //ensures that log(1) == 0
const float s_log_2 = 0.6931472f;

// assumes x > 0 and that it's not a subnormal.
// Results for 0 or negative x won't be -Infinity or NaN
static inline
float fast_log (float x) {
	unsigned int ux = std::bit_cast<unsigned int>(x);
	int e = static_cast<int>(ux - 0x3f800000) >> 23; //e = exponent part can be negative
	ux |= 0x3f800000;
	ux &= 0x3fffffff; // 1 <= x < 2  after replacing the exponent field
	x = std::bit_cast<float>(ux);
	float a = (x + s_log_C0) * (x + s_log_C1);
	float b = (x + s_log_C2) * (x + s_log_C3);
	float c = (float (e) + s_log_C4);
	float d = a / b;
	return (c + d) * s_log_2;
}

	ofdmDecoder::ofdmDecoder	(RadioInterface *mr,
	                                 uint8_t	dabMode,
	                                 int16_t	bitDepth,
	                                 RingBuffer<float>   *devBuffer_i,
	                                 RingBuffer<Complex> *iqBuffer_i) :
	                                    myRadioInterface (mr),
	                                    params	(dabMode),
	                                    theTable	(dabMode),
	                                    myMapper	(dabMode),
	                                    fft (params. get_T_u (), false),
	                                    devBuffer	(devBuffer_i),
	                                    iqBuffer	(iqBuffer_i),
	                                    phaseReference (params. get_T_u ()),
	                                    conjVector	(params. get_T_u ()),
	                                    fft_buffer	(params. get_T_u ()),
	                                    sigmaSQ_Vector (params. get_T_u ()),
	                                    meanLevelVector (params. get_T_u ()),
	                                    stdDevVector (params. get_T_u ()),
	                                    angleVector (params. get_T_u ()) {
	(void)bitDepth;
	connect (this, &ofdmDecoder::showIQ,
	         myRadioInterface, &RadioInterface::showIQ);
	connect (this, &ofdmDecoder::show_quality,
	         myRadioInterface, &RadioInterface::show_quality);
	connect (this, &ofdmDecoder::show_stdDev,
	         myRadioInterface, &RadioInterface::show_stdDev);
//
	this	-> T_s		= params. get_T_s	();
	this	-> T_u		= params. get_T_u	();
	this	-> nrBlocks	= params. get_L		();
	this	-> carriers	= params. get_carriers	();
	this	-> T_g		= T_s - T_u;

	repetitionCounter	= 10;

	reset ();
	iqSelector		= SHOW_DECODED;
//	iqSelector		= SHOW_RAW;
	decoder			= DECODER_1;
	sqrt_2			= sqrt (2);
//
//	Prefil some tables for faster access
	for (int i = 0; i < 1024; i ++) {
	   besselTable [i] = IO_Bessel (((float)i) / 32.0);
	}

	for (int i = 0; i < 8; i ++)
	   W_table [i] = w (-i);
}

	ofdmDecoder::~ofdmDecoder	() {
}
//
void	ofdmDecoder::stop ()	{
}
//
void	ofdmDecoder::reset ()	{
	for (int i = 0; i < T_u; i ++) {
	   sigmaSQ_Vector [i]	= 0;
	   meanLevelVector [i]	= 0;
	   stdDevVector [i]	= 0;
	   angleVector [i]	= M_PI_4;
	}
	meanValue	= 1.0f;
	avgBit		= 10.0f;
}
//
void	ofdmDecoder::processBlock_0 (std::vector <Complex> buffer) {
	fft. fft (buffer);
//	we are now in the frequency domain, and we keep the carriers
//	for their phases.
	memcpy (phaseReference. data (), buffer. data (),
	                                      T_u * sizeof (Complex));
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
	   float ab	= jan_abs (ss) / sqrt_2;
	   f_n		=  0.99 * f_n + 0.01 * (jan_abs (ss) * jan_abs (ss));
	   float R	= abs (abs (real (ss)) - ab);
	   float I	= abs (abs (imag (ss)) - ab);
	   f_d		= 0.99 * f_d + 0.01 * (R * R + I * I);
	}
	return 10 * log10 (f_n / f_d + 0.1);
}

//	for the other blocks of data, the first step is to go from
//	time to frequency domain, to get the carriers.
//	we distinguish between FIC blocks and other blocks,
//	only to spare a test. The mapping code is the same

static	int	cnt	= 0;

//
//	DAB (and DAB+) bits are encoded is DPSK, 2 bits per carrier,
//	depending on the quadrant the carrier is in. There are
//	of course two different approaches in decoding the bits
//	One is looking at the X and Y components, and 
//	their length, relative to each other,
//	Ideally, the X and Y are of equal size, in practice they are not.

static inline
int	sign (DABFLOAT x) {
	return x < 0 ? -1 : x > 0 ? 1 : 0;
}


void	limit_symmetrically (DABFLOAT &v, DABFLOAT limit) {
	if (v < -limit)
	   v = -limit;
	if (v > limit)
	   v = limit;
}
//
//	The decoders 1  and 2  are based on "Soft optimal 2" in
//	"Soft decisions for DQPSK demodulation for the Viterbi
//	decoding of the convolutional codes"
//	Thushara C Hewavithana and Mike Brooks
//
//	the formula (decoders 1 and 2) (in my own words)
//	Corrector = sqrt (2) / (SigmaSQ * (1 /SNR + 2))
//	where corrector is to be applied on real (symbol) and imag (symbol)
//	The implied assumption here is that abs (symbol) == 1,
//
//	The basic idea behind the formula is to enlarge the
//	spread in sizes of the real (symb) and imag (symb),
//	which is obviously inversely proportional with the sigmaSq
//
//	It shows that the resulting values for "Corrector" are
//	small, so for mapping those on -127 .. 127 a "scaler" is needed
//	(Thanks to Rolf Zerr, aka Old-dab).
//
//	Decoder 4 is an interpretation of the so-called "Optimal 3" 
//	version in the aforementioned paper.


void	ofdmDecoder::decode (std::vector <Complex> &buffer,
	                     int32_t blkno,
	                     std::vector<int16_t> &softbits,
	                     DABFLOAT snr) {

DABFLOAT sum	= 0;
DABFLOAT bitSum	= 0;

	memcpy (fft_buffer. data (), &((buffer. data ()) [T_g]),
	                               T_u * sizeof (Complex));
//	first step: do the FFT
	fft. fft (fft_buffer. data ());

//	a little optimization: we do not interchange the
//	positive/negative frequencies to their right positions.
//	The de-interleaving understands this

	for (int i = 0; i < carriers; i ++) {
	   int16_t	index	= myMapper.  mapIn (i);
	   if (index < 0) 
	      index += T_u;
	   Complex current	= fft_buffer [index];
	   Complex prevS	= phaseReference [index];
	   Complex fftBin	= current * normalize (conj (prevS));
	   conjVector [index]	= fftBin;
	   DABFLOAT binAbsLevel	= jan_abs (fftBin);
//
//	updates
	   
	   Complex fftBin_at_1	= Complex (abs (real (fftBin)),
	                                   abs (imag (fftBin)));

	   DABFLOAT angle	= arg (fftBin_at_1) - angleVector [index];
	   angleVector [index]	=
	                 compute_avg (angleVector [index], angle, ALPHA);
	   stdDevVector [index]	= 
	                 compute_avg (stdDevVector [index],
	                                         angle * angle, ALPHA);

	   meanLevelVector [index] =
	        compute_avg (meanLevelVector [index], binAbsLevel, ALPHA);

	   DABFLOAT d_x		=  abs (real (fftBin_at_1)) -
	                                  meanLevelVector [index] / sqrt_2;
	   DABFLOAT d_y		=  abs (imag (fftBin_at_1)) -
	                                  meanLevelVector [index] / sqrt_2;
	   DABFLOAT sigmaSQ	= d_x * d_x + d_y * d_y;
	   sigmaSQ_Vector [index] =
	             compute_avg (sigmaSQ_Vector [index], sigmaSQ, ALPHA);
//
//	Ran over quite a number of examples, I found DECODER_1
//	working best
	   if (this -> decoder == DECODER_1) {
	      DABFLOAT corrector	=
	          1.5 *  meanLevelVector [index] / sigmaSQ_Vector [index];
	      corrector			/= (1 / snr + 2);
	      Complex R1	= corrector * normalize (fftBin) * 
	                           (DABFLOAT)(sqrt (binAbsLevel *
	                                       jan_abs (phaseReference [index])));
	      DABFLOAT scaler		=  140.0 / meanValue;
	      DABFLOAT leftBit		= - real (R1) * scaler;
	      limit_symmetrically (leftBit, MAX_VITERBI);
	      softbits [i]		= (int16_t)leftBit;

	      DABFLOAT rightBit		= - imag (R1) * scaler;
	      limit_symmetrically (rightBit, MAX_VITERBI);
	      softbits [i + carriers]	= (int16_t)rightBit;

	      sum			+= jan_abs (R1);
	   }
	   else 
	   if (this -> decoder == DECODER_2) {	// decoder 2
	      DABFLOAT corrector	=
	          meanLevelVector [index] / sigmaSQ_Vector [index];
	      corrector		/= (1 / snr + 3);
	      Complex R1	= corrector * normalize (fftBin) * 
	                           (DABFLOAT)(sqrt (binAbsLevel *
	                                      jan_abs (phaseReference [index])));
	      DABFLOAT scaler		=  100.0 / meanValue;
	      DABFLOAT leftBit		= - real (R1) * scaler;
	      limit_symmetrically (leftBit, MAX_VITERBI);
	      softbits [i]		= (int16_t)leftBit;

	      DABFLOAT rightBit		= - imag (R1) * scaler;
	      limit_symmetrically (rightBit, MAX_VITERBI);
	      softbits [i + carriers]	= (int16_t)rightBit;

	      sum			+= jan_abs (R1);
	   }
	   else
	   if (this -> decoder == DECODER_3) {  // Optimal 1
	      Complex R1	= fftBin * (DABFLOAT)(jan_abs (prevS));
	      DABFLOAT scaler   =  140.0 / meanValue;

	      DABFLOAT leftBit  = - real (R1) * scaler;
	      limit_symmetrically (leftBit, MAX_VITERBI);
	      softbits [i]      = (int16_t)leftBit;

	      DABFLOAT rightBit = - imag (R1) * scaler;
	      limit_symmetrically (rightBit, MAX_VITERBI);
	      softbits [i + carriers]   = (int16_t)rightBit;
	      sum += jan_abs (R1);
	   }
	   else 	// experimental optimum 3
	   if (this -> decoder == DECODER_4) {	// decoder 4
	      DABFLOAT P1 =  makeA (1, current, prevS) / sigmaSQ_Vector [index];
	      DABFLOAT P7 =  makeA (7, current, prevS) / sigmaSQ_Vector [index];
	      DABFLOAT P3 =  makeA (3, current, prevS) / sigmaSQ_Vector [index];
	      DABFLOAT P5 =  makeA (5, current, prevS) / sigmaSQ_Vector [index];

	      DABFLOAT IO_P1 = IO (P1);
	      DABFLOAT IO_P7 = IO (P7);
	      DABFLOAT IO_P3 = IO (P3);
	      DABFLOAT IO_P5 = IO (P5);

	      DABFLOAT F1	= (IO_P1 + IO_P7) / (IO_P3 + IO_P5);
	      DABFLOAT F2	= (IO_P1 + IO_P3) / (IO_P5 + IO_P7);
	      if (std::isinf (F1))
	         F1 = 10.0;
	      if (std::isinf (F2))
	         F2 = 10.0;
	      if (F1 < 0.01)
	         F1 = 0.01;
	      if (F2 < 0.01)
	         F2 = 0.01;
	      float b1 = abs (log (F1));
	      float b2 = abs (log (F2));

	      if (std::isnan (b1))
	         b1 = 0;
	      if (std::isnan (b2))
	         b2 = 0;
	      DABFLOAT scaler   =  140.0 / avgBit;
	      bitSum		+= (b1 + b2) / 2;
	      DABFLOAT xx1	=  - sign (real (fftBin)) * b1 * scaler;
	      DABFLOAT xx2	=  - sign (imag (fftBin)) * b2 * scaler;
	      limit_symmetrically (xx1, MAX_VITERBI);
	      limit_symmetrically (xx2, MAX_VITERBI);
	      softbits [i]	= (int16_t)xx1;
	      softbits [carriers + i] = (int16_t)xx2;
	      Complex R1	= fftBin * (DABFLOAT)(jan_abs (prevS));
	      sum		+= abs (R1);
	   }
	}

	avgBit		= compute_avg (avgBit, bitSum / carriers, 0.1);
	meanValue	= compute_avg (meanValue, sum /carriers, 0.1);
	
//		end of decoding	, now for displaying things	//
//////////////////////////////////////////////////////////////////
	                     
//	From time to time we show the constellation of symbol 2.
	if (blkno == 2) {
	   if (++cnt > repetitionCounter) {
	      DABFLOAT maxAmp = 00;
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
float	ofdmDecoder::compute_frequencyOffset (Complex *r, Complex *c) {
Complex theta = Complex (0, 0);
static float vv	=  0;

	for (int i = - carriers / 2; i < carriers / 2; i += 6) {
	   int index = i < 0 ? i + T_u : i;
	   Complex val = r [index] * conj (c [index]);
	   val		= Complex (abs (real (val)), abs (imag (val)));
	   theta	+= val * Complex (1, -1);
	}

	float uu =  arg (theta) / (2 * M_PI) * SAMPLERATE / T_u;
	vv	= 0.9 * vv + 0.1 * abs (uu);;
	return vv;
}

float	ofdmDecoder::compute_clockOffset (Complex *r, Complex *v) {
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

