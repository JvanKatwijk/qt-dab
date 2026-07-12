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
#define	ALPHA	0.05f
static inline
Complex	normalize (const Complex &V) {
DABFLOAT length	= jan_abs (V);
	if (length < 0.0001)
	   return Complex (0.001, 0.001);
	return Complex (V) / length;
}
//
//	The bessel function is under windows too slow too work with
//	that is why we created a table that is filled on startup
DABFLOAT besselTable [2048];
static inline
DABFLOAT IO_Bessel	(DABFLOAT x) {
	return std::cyl_bessel_i (0.0f, x);
}

// and table access is with this function
static inline
DABFLOAT IO (DABFLOAT x) {
	return besselTable [((int)(x * 32)) % 2048];
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
	return jan_abs  (prevS + W_table [i] * S);
}

//
//	Provided for computing the phase error by old-dab
//	(aka Rolf Zerr)
static
Complex makeComplex (DABFLOAT phase) {
//	Minimax polynomial for sin(x) and cos(x) on [-pi/4, pi/4]
//	Coefficients via Remez algorithm (Sollya)
//	Max |error| < 1.5e-4 for sinus, < 6e-4 for cosinus

	const DABFLOAT x2 =  phase * phase;
	const DABFLOAT s1 =  0.99903142452239990234375f;
	const DABFLOAT s2 = -0.16034401953220367431640625;
	const DABFLOAT sine = phase * (x2 * s2 + s1);

	const DABFLOAT c1 =  0.9994032382965087890625f;
	const DABFLOAT c2 = -0.495580852031707763671875;
	const DABFLOAT c3 =  3.679168224334716796875e-2;
	const DABFLOAT cosine = c1 + x2 * (x2 * c3 + c2);
	return Complex (cosine, sine);
}

//
//	The elements of the ofdm decoder take as input the
//	block of the DAB frame, apply an FFT and transform the
//	carriers into softbits (-127 .. 127)
//
//	DAB (and DAB+) bits are encoded is DPSK, 2 bits per carrier,
//	depending on the quadrant the carrier is in. There are
//	of course two different approaches in decoding the bits
//	One is looking at the X and Y components, and 
//	their length, relative to each other,
//	Ideally, the X and Y are of equal size, in practice they are not.
//
//	Decoder 3 is the simplified version, implementing formula 10 from
//	"Soft decisions for DQPSK demodulation for the Viterbi
//	decoding of the convolutional codes"
//	Thushara C Hewavithana and Mike Brooks
//	essentially looking at the X and Y component of S[n] * conj (S [n - 1]))
//
//	Decoders 1 and 2 are based on formula 9, which essentially states
//	sqrt (2) / (sigma [n] * (1 / snr + 2));
//
//	* if sigma is small the X and Y component are enlarged,
//	* if snr is large idem
//	Conversely, in sigma is large and snr is small (i.e. a poor signal)
//	then the resulting X and Y values will be small

//	If |S [n]| = 1, then it is clear that sigma is smaller than 1
//	and in an ideal case - sigma nears 0 - the resulting values for the
//	X and Y component are large, if sigma is maximal, i.e. 0.5 * sqrt (2),
//	(assuming snr is very large) the X and Y values are less than 1.
//
//	Decoders 1 and 2 differ in the way a scaling is determined for the
//	different sizes of the input.
//
//	Decoder 4 is an interpretation of the so-called "Optimal 3" 
//	version in the aforementioned paper.
//	see formula 13 and 14.
//
//	The decoders are all based o
	ofdmDecoder::ofdmDecoder	(RadioInterface *mr,
	                                 int16_t	bitDepth,
	                                 RingBuffer<float>   *devBuffer_i,
	                                 RingBuffer<DABFLOAT> *carrierBuffer,
	                                 RingBuffer<Complex> *iqBuffer_i) :
	                                    myRadioInterface (mr),
	                                    theTable	(),
	                                    myMapper	(),
	                                    fft (get_T_u (), false),
	                                    devBuffer	(devBuffer_i),
	                                    iqBuffer	(iqBuffer_i),
	                                    phaseReference (get_T_u ()),
	                                    conjVector	(get_T_u ()),
	                                    fft_buffer	(get_T_u ()),
	                                    sigmaSQ_Vector (get_T_u ()),
	                                    meanNullSymbVector (get_T_u ()),
	                                    meanLevelVector (get_T_u ()),
	                                    meanPowerVector (get_T_u ()) {
	(void)bitDepth;
	this	-> carrierBuffer	= carrierBuffer;
	connect (this, &ofdmDecoder::showIQ,
	         myRadioInterface, &RadioInterface::showIQ);
	connect (this, &ofdmDecoder::show_quality,
	         myRadioInterface, &RadioInterface::show_quality);
	connect (this, &ofdmDecoder::show_stdDev,
	         myRadioInterface, &RadioInterface::show_stdDev);
	connect (this, &ofdmDecoder::show_carriers,
	         myRadioInterface, &RadioInterface::show_carriers);
//
	this	-> T_s		= get_T_s	();
	this	-> T_u		= get_T_u	();
	this	-> nrBlocks	= get_L		();
	this	-> carriers	= get_carriers	();
	this	-> T_g		= T_s - T_u;

	this	-> carriers_2	= carriers / 2;
	repetitionCounter	= 10;
	reset ();
	iqSelector		= SHOW_DECODED;
//	iqSelector		= SHOW_RAW;
	decoder			= DECODER_1;

	sqrt_2			= sqrt (2);
//
//	Prefil some tables for faster access
	for (int i = 0; i < 2048; i ++) {
	   besselTable [i] = IO_Bessel (((float)i) / 32.0);
	}

	for (int i = 0; i < 8; i ++)
	   W_table [i] = w (-i);

	viewCarriers_mode	= 255;
}

	ofdmDecoder::~ofdmDecoder	() {
}
//
void	ofdmDecoder::stop ()	{
}
//
void	ofdmDecoder::reset ()	{
	for (int i = 0; i < T_u; i ++) {
	   sigmaSQ_Vector [i]		= 1;
	   meanLevelVector [i]		= 1;
	   meanPowerVector [i]		= 1;
	   meanNullSymbVector [i]	= 0;
	}
	meanValue	= 1.0f;
}
//
void	ofdmDecoder::processBlock_0 (std::vector <Complex> buffer) {
	fft. fft (buffer);
//	we are now in the frequency domain, and we keep the carriers
//	for their phases.
	memcpy (phaseReference. data (), buffer. data (),
	                                      T_u * sizeof (Complex));

	for (int i = 0; i < carriers; i ++) {
           int16_t      index           = myMapper.  mapIn (i);
           if (index < 0) {
              index     += T_u;
           }
	   DABFLOAT Power = std::norm (buffer [index]) + 0.001;
	   DABFLOAT level	= 10 * log10 (Power + 0.05);
	   meanNullSymbVector [i] =
	              compute_avg (meanNullSymbVector [i], level, ALPHA);
	}
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

static	int	cnt	= 0;

//

//	Note for the reader
//	Some cheap dabsticks show a - sometime pretty large - clock offset.
//	indicating that the samplerate is slightly off.
//	We show the samplerate offset already for a long time,
//	old-dab suggested - in a version derived from Qt-DAB - to compensate
//	for the error
//	If the clockerror is N, then the relative error increase between
//	two successive block is app N / 750, the phase of that error
//	is computed. Of course higher frequencies suffer more than lower
//	frequencies, so old-dab had a scheme where, depending the
//	relative frequency of the bin, the correction was stronger
//
//	The decoders are based on
//	"SOFT DECISIONS FOR DQPSK DEMODULATION FOR THE VITERBI
//	DECODING OF THE CONVOLUTIONAL CODES"
//	Thushara C. Hewavithana and Mike Brookes
//	Department of Electrical and Electronic Engineering
//	Imperial College, Exhibition Road, London SW7 2BT, UK
//
//	In particular
//	- decoders 1 and 2 are implementations of formula 9
//	with additional interpretations for scaling the results
//	- decoder 3 is an implementation of formula 10
//	- decoder 4 is an attempt to implement formulas 13 and 14
void	ofdmDecoder::decode (std::vector <Complex> &buffer,
	                     int32_t	blkno,
	                     std::vector<int16_t> &softBits,
	                     DABFLOAT	snr, float clockError) {

DABFLOAT sum		= 0;
DABFLOAT leftBit	= 0;
DABFLOAT rightBit	= 0;
DABFLOAT scaler		= 0;
//	Note that in case of file input, elements of the fft_buffer
//	may contain a zero value.

	memcpy (fft_buffer. data (), &((buffer. data ()) [T_g]),
	                               T_u * sizeof (Complex));
//	first step: do the FFT
	fft. fft (fft_buffer. data ());

//	here we really start
	for (int i = 0; i < carriers; i ++) {
	   int16_t	index		= myMapper.  mapIn (i);
	   int16_t	binIndex	= index;
	   if (index < 0) {
	      binIndex	+= carriers_2;
	      index	+= T_u;
	   }
	   else
	      binIndex	+= carriers_2 - 1;

	   Complex current	= fft_buffer [index];
	   if (current == Complex (0, 0)) {
	      current = Complex (1, 1);
	      fft_buffer [index] = current;
	   }
	   Complex prevS	= phaseReference [index];
	   Complex fftBin	= current * normalize (conj (prevS));
//
//	correction on the fftBin value using the approach from
//	old-dab, see text above

	   DABFLOAT phaseError	= clockError / (DABFLOAT)carriers_2 * M_PI * 
	                          (carriers_2 - binIndex) / (carriers_2);
	   fftBin		*= makeComplex (-phaseError);
	   conjVector [index]	= fftBin;

//	updates
	   DABFLOAT fftBinPower	= std::norm (fftBin);
	   meanPowerVector [i] =
	        compute_avg (meanPowerVector [i], fftBinPower, ALPHA);

	   DABFLOAT binAbsLevel	= jan_abs (fftBin) / sqrt_2;
	   meanLevelVector [i] =
	        compute_avg (meanLevelVector [i], binAbsLevel, ALPHA);

	   DABFLOAT d_x		=  abs (real (fftBin)) -
	                                  meanLevelVector [i] / binAbsLevel;
	   DABFLOAT d_y		=  abs (imag (fftBin)) -
	                                  meanLevelVector [i] / binAbsLevel;
	   DABFLOAT sigmaSQ	= d_x * d_x + d_y * d_y;
	   sigmaSQ_Vector [i] =
	             compute_avg (sigmaSQ_Vector [i], sigmaSQ, ALPHA);
//	actual decoding
	   switch (this -> decoder) {
	     default:
	     case DECODER_1: //	The denomainator is (formula 9)
	         {  DABFLOAT amplifier	= 
	   	                sqrt (abs (fftBin) * abs (prevS)) *
	                                          meanPowerVector [i];
	            amplifier	/= sigmaSQ_Vector [i];
	            amplifier	/= 1.0 / snr + 3;
	           Complex R1	= normalize (fftBin) * amplifier;
//	scaler (due to old-dab)
	           scaler	= -100 / meanValue;
	           leftBit	= real (R1) * scaler;
	           rightBit	= imag (R1) * scaler;
	           sum		+= jan_abs (R1);
	           break;
	      }

	   case DECODER_2:
	      {  DABFLOAT amplifier	= 
	                        std::sqrt (abs (fftBin) * abs (prevS)) *
	                                            meanLevelVector [i];
	         amplifier	/= sigmaSQ_Vector [i] * abs (fftBin);
	         amplifier	/= 1.0 / snr + 0.7f;
	         Complex R1	= fftBin * amplifier;
//	scaler (due to old-dab)
	         scaler		= -100 / meanValue;
	         leftBit	= real (R1) * scaler;
	         rightBit	= imag (R1) * scaler;
	         sum		+= jan_abs (R1);
	      }
	      break;

	  case DECODER_3:
	     {  Complex R1	= fftBin * (DABFLOAT)(jan_abs (prevS));
	        scaler		=  -140.0 / meanValue;

	        leftBit		= real (R1) * scaler;
	        rightBit	= imag (R1) * scaler;
	        sum		+= jan_abs (R1);
	      }
	      break;

	   case DECODER_4:
	      {  DABFLOAT A	= 1.0 / sigmaSQ_Vector [i];
	         DABFLOAT P1	= makeA (1, current, prevS) * A;
	         DABFLOAT P7	= makeA (7, current, prevS) * A;
	         DABFLOAT P3	= makeA (3, current, prevS) * A;
	         DABFLOAT P5	= makeA (5, current, prevS) * A;

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
	         DABFLOAT b1 = log (F1);
	         DABFLOAT b2 = log (F2);

	         if (std::isnan (b1))
	            b1 = 0;
	         if (std::isnan (b2))
	            b2 = 0;
	         scaler 	=  100.0 / meanValue;

	         leftBit	=  - b1 * scaler;
	         rightBit	=  - b2 * scaler;
	         sum		+= jan_abs (Complex (b1, b2));
	      }
	      break;
	   }
	   softBits [i]		= std::clamp ((int)leftBit,
	                                      -MAX_VITERBI, MAX_VITERBI);
	   softBits [carriers + i]
	                       = std::clamp ((int)rightBit,
	                                      -MAX_VITERBI, MAX_VITERBI);
	}
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
	      Complex *displayVector = dynVec (Complex, carriers);

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
	         float *tempVector = dynVec (float, carriers);
	         for (int i = 0; i < carriers; i ++) {
	            tempVector [i] = 
	                  softBits [carriers / 2 + i];
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
	      if (viewCarriers_mode == SYNC_CARRIERS) {
	         carrierBuffer ->
	             putDataIntoBuffer (meanNullSymbVector. data (), carriers);
	         emit show_carriers (viewCarriers_mode, carriers);
	      }
	      else	
	      if (viewCarriers_mode == MEAN_CARRIERS) {
	         carrierBuffer ->
	             putDataIntoBuffer (meanLevelVector. data (), carriers);
	         emit show_carriers (viewCarriers_mode, carriers);
	      }
	      else
	      if (viewCarriers_mode == SIGMA_CARRIERS) {
	         carrierBuffer ->
	             putDataIntoBuffer (sigmaSQ_Vector. data (), carriers);
	         emit show_carriers (viewCarriers_mode, carriers);
	      }
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
//	Ideally, the processed carrier have a value
//	equal to (2 * k + 1) * PI / 4 (k : 0 .. 4)
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
//
static inline
Complex toQ1	(const Complex f) {
	return Complex (real (f) >= 0 ? real (f) : - real (f),
	                imag (f) >= 0 ? imag (f) : - imag (f));
}

//

void	ofdmDecoder::set_correctPhase (bool b) {
	correctPhase	= b;
}

void	ofdmDecoder::viewCarriers	(uint8_t carrierMode) {
	viewCarriers_mode	= carrierMode;
}

