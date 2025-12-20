#
/*
 *    Copyright (C) 2014 .. 2024
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
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */
//
//	This implementation of the TII decoder is taken from
//	Rolf Zerr's (aka old-dab) implemementation as done in DABstar.
//	All rights acknowledged.

#include "tii-detector-1.h"

constexpr float F_DEG_PER_RAD = (float)(180.0 / M_PI);

#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif

static
uint8_t patternTable [] = {
	0017,		// 0 0 0 0 1 1 1 1		0
	0027,		// 0 0 0 1 0 1 1 1		1
	0033,		// 0 0 0 1 1 0 1 1		2
	0035,		// 0 0 0 1 1 1 0 1		3
	0036,		// 0 0 0 1 1 1 1 0		4
	0047,		// 0 0 1 0 0 1 1 1		5
	0053,		// 0 0 1 0 1 0 1 1		6
	0055,		// 0 0 1 0 1 1 0 1		7
	0056,		// 0 0 1 0 1 1 1 0		8
	0063,		// 0 0 1 1 0 0 1 1		9

	0065,		// 0 0 1 1 0 1 0 1		10
	0066,		// 0 0 1 1 0 1 1 0		11
	0071,		// 0 0 1 1 1 0 0 1		12
	0072,		// 0 0 1 1 1 0 1 0		13
	0074,		// 0 0 1 1 1 1 0 0		14
	0107,		// 0 1 0 0 0 1 1 1		15
	0113,		// 0 1 0 0 1 0 1 1		16
	0115,		// 0 1 0 0 1 1 0 1		17
	0116,		// 0 1 0 0 1 1 1 0		18
	0123,		// 0 1 0 1 0 0 1 1		19

	0125,		// 0 1 0 1 0 1 0 1		20
	0126,		// 0 1 0 1 0 1 1 0		21
	0131,		// 0 1 0 1 1 0 0 1		22
	0132,		// 0 1 0 1 1 0 1 0		23
	0134,		// 0 1 0 1 1 1 0 0		24
	0143,		// 0 1 1 0 0 0 1 1		25
	0145,		// 0 1 1 0 0 1 0 1		26
	0146,		// 0 1 1 0 0 1 1 0		27
	0151,		// 0 1 1 0 1 0 0 1		28	
	0152,		// 0 1 1 0 1 0 1 0		29

	0154,		// 0 1 1 0 1 1 0 0		30
	0161,		// 0 1 1 1 0 0 0 1		31
	0162,		// 0 1 1 1 0 0 1 0		32
	0164,		// 0 1 1 1 0 1 0 0		33
	0170,		// 0 1 1 1 1 0 0 0		34
	0207,		// 1 0 0 0 0 1 1 1		35
	0213,		// 1 0 0 0 1 0 1 1		36
	0215,		// 1 0 0 0 1 1 0 1		37
	0216,		// 1 0 0 0 1 1 1 0		38
	0223,		// 1 0 0 1 0 0 1 1		39

	0225,		// 1 0 0 1 0 1 0 1		40
	0226,		// 1 0 0 1 0 1 1 0		41
	0231,		// 1 0 0 1 1 0 0 1		42
	0232,		// 1 0 0 1 1 0 1 0		43
	0234,		// 1 0 0 1 1 1 0 0		44
	0243,		// 1 0 1 0 0 0 1 1		45
	0245,		// 1 0 1 0 0 1 0 1		46
	0246,		// 1 0 1 0 0 1 1 0		47
	0251,		// 1 0 1 0 1 0 0 1		48
	0252,		// 1 0 1 0 1 0 1 0		49

	0254,		// 1 0 1 0 1 1 0 0		50
	0261,		// 1 0 1 1 0 0 0 1		51
	0262,		// 1 0 1 1 0 0 1 0		52
	0264,		// 1 0 1 1 0 1 0 0		53
	0270,		// 1 0 1 1 1 0 0 0		54
	0303,		// 1 1 0 0 0 0 1 1		55
	0305,		// 1 1 0 0 0 1 0 1		56
	0306,		// 1 1 0 0 0 1 1 0		57
	0311,		// 1 1 0 0 1 0 0 1		58
	0312,		// 1 1 0 0 1 0 1 0		59

	0314,		// 1 1 0 0 1 1 0 0		60
	0321,		// 1 1 0 1 0 0 0 1		61
	0322,		// 1 1 0 1 0 0 1 0		62
	0324,		// 1 1 0 1 0 1 0 0		63
	0330,		// 1 1 0 1 1 0 0 0		64
	0341,		// 1 1 1 0 0 0 0 1		65
	0342,		// 1 1 1 0 0 0 1 0		66
	0344,		// 1 1 1 0 0 1 0 0		67
	0350,		// 1 1 1 0 1 0 0 0		68
	0360		// 1 1 1 1 0 0 0 0		69
};

static inline
uint16_t	nrPatterns () {
        return (uint16_t)sizeof (patternTable);
}

#define	SECTION_SIZE	192
#define	NR_SECTIONS	4

	TII_Detector::TII_Detector (uint8_t dabMode,
	                            phaseTable *theTable) :
	                                params (dabMode),
                                        T_u (params. get_T_u ()),
                                        T_g (params. get_T_g ()),
                                        carriers (params. get_carriers ()),
                                        my_fftHandler (params. get_T_u (),
                                                                    false) {
        nullSymbolBuffer. resize (T_u);
	rotationTable. resize (carriers);
	int teller = 0;
	for (int carrier = - carriers / 2;
	               carrier < carriers / 2; carrier += 2) {
	   int index    = carrier < 0 ? carrier + T_u : carrier + 1;
	   Complex r    = theTable -> refTable [index] *
	                        conj (theTable -> refTable [index + 1]);
//
           float aa = (arg (r) < 0) ? arg (r) + 2 * M_PI : arg (r);
//	the "arg" of r is one of 0 .. 3 * PI/2
	   rotationTable [teller ++]= (int)(floor ((aa + 0.1) / (M_PI / 2)));
        }
	carrierDelete	= false;
	reset ();
}

	TII_Detector::~TII_Detector () { }

void	TII_Detector::resetBuffer       () {
	for (int i = 0; i < T_u; i ++)
	   nullSymbolBuffer [i] = Complex (0, 0);
}

void	TII_Detector::reset		() {
	resetBuffer();
	memset ((void *)decodedBuffer, 0, carriers / 2 * sizeof (Complex));
}

//	To eliminate (reduce?) noise in the input signal, we might
//	add a few spectra before computing (up to the user)
void    TII_Detector::addBuffer (const std::vector<Complex>  &v) {
Complex tmpBuffer [T_u];

        for (int i = 0; i < T_u; i ++)
           tmpBuffer [i] = v [T_g + i];
        my_fftHandler. fft (tmpBuffer);
        for (int i = 0; i < T_u; i ++)
           nullSymbolBuffer [i] += tmpBuffer [i];
}

// rotate  0, 90, 180 or 270 degrees
Complex	TII_Detector::rotate (Complex value, uint8_t phaseIndicator) {   
        switch (phaseIndicator) {
           case 0:
              return value;
           case 1:	// PI / 2
              return Complex (imag (value), -real (value));
           case 2:	// PI
              return -value;
           case 3:	// 3 * PI / 2
              return Complex (-imag (value), real (value));
           default:     // should not happen
              return value;
        }
}

//	we map the "K" carriers (complex values) onto
//	a collapsed vector of "K / 8" length,
//	considered to consist of 8 segments of 24 values
//	Each "value" is the sum of 4 pairs of subsequent carriers,
//	taken from the 4 quadrants -768 .. 385, 384 .. -1, 1 .. 384, 385 .. 768
void	TII_Detector::collapse (const Complex *inVec,
	                        Complex *etsiVec, Complex *nonetsiVec) {
Complex buffer [carriers / 2];

	memcpy (buffer, inVec, carriers / 2 * sizeof (Complex));

	int nrSections	= 4;
//	a single carrier cannot be a TII carrier.
	if (carrierDelete) {
	   for (int i = 0; i < SECTION_SIZE; i++) {
	      float x [4];
	      float max = 0;
	      float sum = 0;
	      int index = 0;
	      for (int j = 0; j < nrSections; j++) {
	         x [j] = jan_abs (buffer [i + j * SECTION_SIZE]);
		 sum += x [j];
		 if (x [j] > max) {
	            max = x[j];
	            index = j;
	         }
	      }

	      float min = (sum - max) / 3;
	      if (sum < max * 1.5 && max > 0.0) {
	         buffer [i + index * SECTION_SIZE] *= min / max;
	      }
	   }
	}

	for (int i = 0; i < SECTION_SIZE; i++) {
	   etsiVec [i]	= Complex (0, 0);
	   nonetsiVec [i] =  Complex (0, 0);
	   for (int j = 0; j < nrSections; j++) {
	      Complex x = buffer [i + j * SECTION_SIZE];
	      etsiVec [i] += x;
	      nonetsiVec [i] += rotate (x,
	                                rotationTable [i + j * SECTION_SIZE]);
	   }
	}
}

static uint8_t bits [] = {0x80, 0x40, 0x20, 0x10 , 0x08, 0x04, 0x02, 0x01};

// Sort the elements according to their strength
static
int	fcmp (const void *a, const void *b) {
	tiiData *element1 = (tiiData *)a;
	tiiData *element2 = (tiiData *)b;
	if (element1 -> strength > element2 -> strength)
	   return -1;
	else
	if (element1 -> strength < element2 -> strength)
	   return 1;
	else
	   return 0;
}

QVector<tiiData> TII_Detector::processNULL (int16_t threshold_db, 
	                                    uint8_t selected_subId) {
//	collapsed ETSI float values
float	etsi_floatTable [NUM_GROUPS * GROUPSIZE];	
//	collapsed non-ETSI float values
float	nonetsi_floatTable [NUM_GROUPS * GROUPSIZE];
//	collapsed ETSI complex values

Complex etsiTable [NUM_GROUPS * GROUPSIZE];
//	collapsed non-ETSI complex values
Complex nonetsiTable [NUM_GROUPS * GROUPSIZE];

float	max = 0;		// abs value of the strongest carrier
float	noise = 1e9;	// noise level
QVector<tiiData> theResult;		// results
float	avg_etsi	[NUM_GROUPS];
float	avg_nonetsi	[NUM_GROUPS];

float threshold = pow (10, (float)threshold_db / 10); // threshold above noise
int Teller = 0;

	for (int32_t idx = -carriers / 2; idx < carriers / 2; idx += 2) {
	   const int32_t fftIdx = idx < 0 ? idx + T_u : idx + 1;
	   decodedBuffer [Teller++] += 
	       nullSymbolBuffer [fftIdx] * conj (nullSymbolBuffer [fftIdx + 1]);
	}

	collapse (decodedBuffer, etsiTable, nonetsiTable);


// fill the float tables, determine the abs value of the strongest carrier
	for (int i = 0; i < NUM_GROUPS * GROUPSIZE; i++) {
	   float x = jan_abs (etsiTable [i]);
	   etsi_floatTable [i] = x;
	   if (x > max)
	      max = x;
	   x = jan_abs (nonetsiTable [i]);
	   nonetsi_floatTable [i] = x;
	   if (x > max)
	      max = x;
	}

	for (int group = 0; group < NUM_GROUPS; group ++) {
	   avg_etsi [group] = 0;
	   avg_nonetsi [group] = 0;
	   for (int j = 0; j < GROUPSIZE; j ++) {
	      avg_etsi [group] += etsi_floatTable [group * GROUPSIZE + j];
	      avg_nonetsi [group] += nonetsi_floatTable [group * GROUPSIZE + j];
	   }
	   avg_etsi [group] /= GROUPSIZE;
	   avg_nonetsi [group] /= GROUPSIZE;
	}

//	determine the noise level by taking the level of the lowest group
	for (int subId = 0; subId < GROUPSIZE; subId++) {
	   float avg = 0;
	   for (int i = 0; i < NUM_GROUPS; i++)
	      avg += etsi_floatTable [subId + i * GROUPSIZE];
	   avg /= NUM_GROUPS;
	   if (avg < noise)
	      noise = avg;
	}

	for (int subId = 0; subId < GROUPSIZE; subId++) {
	   tiiData element;
	   Complex sum		= Complex (0,0);
	   Complex etsi_sum	= Complex (0,0);
	   Complex nonetsi_sum	= Complex (0,0);
	   int count		= 0;
	   int etsi_count	= 0;
	   int nonetsi_count	= 0;
	   int pattern		= 0;
	   int etsi_pattern	= 0;
	   int nonetsi_pattern	= 0;
	   int mainId		= 0;
	   bool norm		= false;
	   Complex *cmplx_ptr	= nullptr;;
	   float *float_ptr	= nullptr;
//	The number of times the limit is reached in the group is counted
	   for (int i = 0; i < NUM_GROUPS; i++) {
	      float thresholdLevel = 
	               noise * std::pow (10.0f, threshold_db / 10.0f);
	      if (etsi_floatTable [subId + i * GROUPSIZE] > thresholdLevel) {
	         etsi_count++;
	         etsi_pattern	|= bits [i];
	         etsi_sum	+= etsiTable [subId + GROUPSIZE * i]; 
	      }
	      if (nonetsi_floatTable [subId + i * GROUPSIZE] > thresholdLevel) {
	         nonetsi_count++;
	         nonetsi_pattern |= bits [i];
	         nonetsi_sum += nonetsiTable [subId + GROUPSIZE * i];
	      }
	   }
//
	   if ((etsi_count >= 4) || (nonetsi_count >= 4))  {
	      if (jan_abs (nonetsi_sum) > jan_abs (etsi_sum)) {
	         norm		= true;
	         sum		= nonetsi_sum;
	         cmplx_ptr	= nonetsiTable;
	         float_ptr	= nonetsi_floatTable;
	         count		= nonetsi_count;
	         pattern	= nonetsi_pattern;
	      }
	      else {
	         sum		= etsi_sum;
	         cmplx_ptr	= etsiTable;
	         float_ptr	= etsi_floatTable;
	         count		= etsi_count;
	         pattern	= etsi_pattern;
	      }
	   }

//	Find the Main Id that matches the pattern
	   if (count == 4) {
	      for (; mainId < (int)nrPatterns (); mainId++)
	         if (patternTable [mainId] == pattern)
	            break;
	   }

//	Find the best match. We extract the four max values as bits
	   else
	   if (count > 4) {
	      float mm = 0;
	      for (int k = 0; k < (int)nrPatterns (); k++) {
	         Complex val = Complex (0, 0);
	         for (int i = 0; i < NUM_GROUPS; i++) {
	            if (patternTable [k] & bits [i]) {
	               val += cmplx_ptr [subId + GROUPSIZE * i];
	            }
	         }

	         if (jan_abs (val) > mm) {
	            mm = jan_abs (val);
	            sum = val;
	            mainId = k;
	         }
	      }
	   }	// end if (count > 4),  List the result

	   if (count >= 4) {
	      element. mainId	= mainId;
	      element. subId	= subId;
	      element. strength = jan_abs (sum) / max / 4;
	      element. phase	= arg (sum) * F_DEG_PER_RAD;
	      element. norm	= norm;
	      element. collision	= false;
	      element. pattern	= patternTable [mainId];
	      theResult. push_back (element);
	   }
//
//	Collisions still to be done
	   if ((count > 4) && (selected_subId != 0)) {
	      sum = Complex (0,0);

//	Calculate the level of the second main ID
	      for (int i = 0; i < NUM_GROUPS; i++) {
	         if ((patternTable [mainId] & bits [i]) == 0) {
	            int index = subId + GROUPSIZE * i;
	            if (float_ptr [index] > noise * threshold)
	               sum += cmplx_ptr [index];
	         }
	      }

	      if (subId == selected_subId) { // List all possible main IDs
	         for (int k = 0; k < (int)nrPatterns (); k++) {
	            int pattern2 = patternTable [k] & pattern;
	            int count2 = 0;
	            for (int i = 0; i < NUM_GROUPS; i++)
	               if (pattern2 & bits [i])
	                  count2++;
	            if ((count2 == 4) && (k != mainId)) {
	               element. mainId		= k;
	               element. subId		= selected_subId;
	               element. strength	= jan_abs(sum) / max / (count - 4);
	               element. phase		= arg(sum) * F_DEG_PER_RAD;
	               element. norm		= norm;
	               element. collision	= true;
	               element. pattern		= patternTable [mainId];
	               theResult. push_back (element);
	            }
	         }
	      }
	      else { // List only additional main ID 99
//	         element. mainId = 99;
//	         element. strength = abs(sum)/max/(count-4);
//	         element. phase = arg(sum) * F_DEG_PER_RAD;
//	         element. norm = norm;
//	         theResult.push_back(element);
	      }
	   }
	}
//	fprintf(stderr, "max =%.0f, noise = %.1fdB\n", max, 10 * log10(noise/max));
	if (max > 4000)
//	if (max > 4000000)
	   for (int i = 0; i < carriers / 2; i++)
	      decodedBuffer [i] *= 0.9;
	resetBuffer();
	qsort (theResult. data (), theResult. size(),
	                     sizeof (tiiData), &fcmp);
	return theResult;
}
