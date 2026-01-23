#
/*
 *    Copyright (C) 2014 .. 2025
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

#include	"tii-detector-2.h"
#include	<cstdio>
#include	<cinttypes>
#include	<cstring>
#include	<QSettings>

#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif
constexpr float F_DEG_PER_RAD = (float)(180.0 / M_PI);

		TII_Detector_B::TII_Detector_B (uint8_t dabMode,
	                                       phaseTable *theTable,
	                                       QSettings *dabSettings):
	                                       TII_Detector (dabMode,
	                                                     theTable) {
	this	-> dabSettings	= dabSettings;
	memset (invTable, 0x377, 256);
	for (int i = 0; i < 70; ++i) 
	   invTable [getPattern (i)] = i;
}

		TII_Detector_B::~TII_Detector_B () {
}

void	TII_Detector_B::reset	() {
	memset ((void *)nullSymbolBuffer. data (), 0, T_u * sizeof (Complex));
}

//	Note that the input is fft output, not yet reordered
void	TII_Detector_B::collapse (std::vector<Complex> &inVec,
	                          Complex *outVec_etsi,
	                          Complex *outVec_nonetsi,
	                          bool tiiFilter) {
int	teller = 0;
    for (int32_t idx = -carriers / 2; idx < carriers / 2; idx += 2) {
           const int32_t fftIdx = idx < 0 ? idx + T_u : idx + 1;
           decodedBuffer [teller++]  +=
               inVec [fftIdx] * conj (inVec [fftIdx + 1]);
        }

	int nrSections	= tiiFilter ? 2 : 4;
	if (tiiFilter) {
	   for (int i = 0; i < 192; i ++) {
	      float x [4];
	      float max = 0;
	      float sum = 0;
	      int index = 0;
	      for (int j = 0; j < nrSections; j++) {
	         x [j] = jan_abs (decodedBuffer [i + j * 192]);
	         sum += x [j];
	         if (x [j] > max) {
	            max = x[j];
	            index = j;
	         }
	      }

	      float min = (sum - max) / 3;
	      if (sum < max * 1.5 && max > 0.0) {
	        decodedBuffer [i + index * 192] *= min / max;
	      }
	   }
	}

	for (int i = 0; i < 192; i ++) {
	   outVec_etsi [i] = Complex (0, 0);
	   outVec_nonetsi [i] = Complex (0, 0);
	   for (int j = 0; j < nrSections; j ++) {
	      Complex X = decodedBuffer [i + j * 192];
	      outVec_etsi [i] += X ;
	      outVec_nonetsi [i] += rotate (X, getRotation (i + j * 192));

	   }
	}
}

static
uint8_t bits [] = {0x80, 0x40, 0x20, 0x10 , 0x08, 0x04, 0x02, 0x01};


resultPair TII_Detector_B::findBestIndex (Complex *vector_192,
	                                  float *avgTable, float threshold) {
Complex C_table [GROUPSIZE];
int	D_table [GROUPSIZE] = {0};
resultPair	bestPair;
	for (int i = 0; i < GROUPSIZE; i ++)
	   C_table [i] = Complex (0, 0);
//
//	We only use the C and D table to locate the start offset
	for (int i = 0; i < GROUPSIZE; i ++) {
	   for (int j = 0; j < NUM_GROUPS; j ++) {
	      if (jan_abs (vector_192 [j * GROUPSIZE + i]) >
	                                     threshold * avgTable [j]) {
	         C_table [i] +=
	            vector_192 [j * GROUPSIZE + i] / (DABFLOAT) (threshold * avgTable [j]);
	         D_table [i] ++;
	      }
	   }
	}
//	we extract from this result the highest values that
//	meet the constraint of 4 values being sufficiently high
	float	maxTable	= 0;
	int	maxIndex	= -1;
	
	for (int j = 0; j < GROUPSIZE; j ++) {
	   if ((D_table [j] >= 4) && (jan_abs (C_table [j]) > maxTable)) {
	      maxTable = jan_abs (C_table [j]);
	      maxIndex = j;
	      break;
	   }
	}
	bestPair. index = maxIndex;
	bestPair. value	= maxTable;
	return bestPair;;
}
//	We determine first the offset of the "best fit", the offset
//	indicates the subId
QVector<tiiData>	TII_Detector_B::processNULL (int16_t threshold_db,
	                                             uint8_t selected_subId,
	                                             bool tiiFilter) {
// collapses values:
Complex	collapsed_etsi	[NUM_GROUPS * GROUPSIZE];
// collapses values
Complex	collapsed_nonetsi	[NUM_GROUPS * GROUPSIZE];
float	avgTable	[NUM_GROUPS];
QVector<tiiData> theResult;

float threshold = pow (10, (float)threshold_db / 10); // threshold above noise

	(void)selected_subId;
//	we map the "carriers" carriers (complex values) onto
//	a collapsed vector of "carriers / 8" length, 
//	considered to consist of 8 segments of 24 values
//	Each "value" is the sum of 4 pairs of subsequent carriers,
//	taken from the 4 quadrants -768 .. 385, 384 .. -1, 1 .. 384, 385 .. 768
	collapse (nullSymbolBuffer,
	                 collapsed_etsi, collapsed_nonetsi, tiiFilter);

//	since the "energy levels" in the different GROUPSIZE'd values
//	may differ, we compute an average for each of the
//	NUM_GROUPS GROUPSIZE - value groups. 

	while (true) {
	   memset (avgTable, 0, NUM_GROUPS * sizeof (float));

	   for (int i = 0; i < NUM_GROUPS; i ++) {
	      avgTable [i] = 0;
	      for (int j = 0; j < GROUPSIZE; j ++) 
	         avgTable [i] += jan_abs (collapsed_etsi [i * GROUPSIZE + j]);
	      avgTable [i] /= GROUPSIZE;
	   }

//
//	first we look to where we could start best, both for
//	the "local" and the transformed vectors

	   resultPair result_1 =
	           findBestIndex (collapsed_etsi, avgTable, threshold);
	   resultPair result_2 =
	           findBestIndex (collapsed_nonetsi, avgTable, threshold);
	
	   if ((result_1. index < 0) && (result_2. index < 0)) {
	      resetBuffer ();
	      for (int i = 0; i < carriers / 2; i ++)
	         decodedBuffer [i] *= 0.0;
	      return theResult;
	   }

	   resultPair result;
	   Complex *collapsed = nullptr;
	   if (result_2. value > result_1. value) {
	      result = result_2;
	      result. norm = true;
	      collapsed = collapsed_nonetsi;
	   }
	   else {
	      result = result_1;
	      collapsed = collapsed_etsi;
	      result. norm = false;
	   }
	
	float strength = 10 * log10 (result. value / 4);
//	The - almost - final step is then to figure out which
//	group contributed most, obviously only where maxIndex  > 0
//	we start with collecting the values of the correct
//	elements of the NUM_GROUPS groups

	   float x [NUM_GROUPS];
	   for (int i = 0; i < NUM_GROUPS; i ++) {
	      x [i] = jan_abs (collapsed [result. index + GROUPSIZE * i]);
	   }

//	find the best match
	   int finInd = -1;
////	we extract the four max values as bits
	   uint16_t pattern	= 0;
	   for (int i = 0; i < 4; i ++) {
	      float mmax	= 0;
	      int ind	= -1;
	      for (int k = 0; k < NUM_GROUPS; k ++) {
	         if (x [k] > mmax) {
	            mmax = x [k];
	            ind  = k;
	         }
	      }
	      if (ind != -1) {
	         x [ind] = 0;
	         pattern |= bits [ind];
	      }
	   }
	   finInd = invTable [pattern];
	   Complex phaseHolder = std::complex<float> (0, 0);
	   for (int i = 0; i < 8; i ++) {
	      if (pattern & bits [i]) {
	         int index = result. index + i * 24;
	         phaseHolder += collapsed [index];
	         collapsed_etsi [index] = Complex (0, 0);
	         collapsed_nonetsi [index] = Complex (0, 0);
	      }
	   }
	   tiiData v;
	   v. subId	= result. index;
	   v. mainId	= finInd;	
	   v. strength	= strength;
	   v. pattern	= pattern;
	   v. phase	= arg (phaseHolder) * F_DEG_PER_RAD;
	   v. norm	= result. norm;
	   v. collision	= 0;
	   theResult. push_back (v);
	}
	return theResult;
}

