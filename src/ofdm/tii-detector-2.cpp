#
/*
 *    Copyright (C) 2014 .. 2023
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

		TII_Detector_B::TII_Detector_B (uint8_t dabMode,
	                                       QSettings *dabSettings):
	                                       TII_Detector (dabMode) {
	this	-> dabSettings	= dabSettings;
	memset (invTable, 0x377, 256);
	for (int i = 0; i < 70; ++i) 
	   invTable [getPattern (i)] = i;
}

		TII_Detector_B::~TII_Detector_B () {
}

void	TII_Detector_B::reset	() {
	memset (nullSymbolBuffer. data (), 0, T_u * sizeof (Complex));
}

//	Note that the input is fft output, not yet reordered
void	TII_Detector_B::collapse (std::vector<Complex> &inVec,
	                          float *outVec, bool tiiFilter) {
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
	         x [j] = abs (decodedBuffer [i + j * 192]);
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
	   outVec [i] = 0;
	   for (int j = 0; j < nrSections; j ++)
	      outVec [i] += abs (decodedBuffer [i + j * 192]);
	}
}

static
uint8_t bits [] = {0x80, 0x40, 0x20, 0x10 , 0x08, 0x04, 0x02, 0x01};

//
//	We determine first the offset of the "best fit", the offset
//	indicates the subId
QVector<tiiData>	TII_Detector_B::processNULL (int16_t threshold_db,
	                                             uint8_t selected_subId,
	                                             bool tiiFilter) {
float	hulpTable	[NUM_GROUPS * GROUPSIZE]; // collapses values
float	C_table		[GROUPSIZE];		  // contains the values
int	D_table		[GROUPSIZE];	// count of indices in C_table with data
float	avgTable	[NUM_GROUPS];
float	max		= 0;
QVector<tiiData> theResult;

float threshold = pow (10, (float)threshold_db / 10); // threshold above noise

	(void)selected_subId;
	bool dxMode	= true;
//	we map the "carriers" carriers (complex values) onto
//	a collapsed vector of "carriers / 8" length, 
//	considered to consist of 8 segments of 24 values
//	Each "value" is the sum of 4 pairs of subsequent carriers,
//	taken from the 4 quadrants -768 .. 385, 384 .. -1, 1 .. 384, 385 .. 768
	collapse (nullSymbolBuffer, hulpTable, tiiFilter);

//	since the "energy levels" in the different GROUPSIZE'd values
//	may differ, we compute an average for each of the
//	NUM_GROUPS GROUPSIZE - value groups. 

	for (int i = 0; i < 192; i ++)
	   if (hulpTable [i] > max)
	      max = hulpTable [i];
	while (true) {
	   memset (avgTable, 0, NUM_GROUPS * sizeof (float));

	   for (int i = 0; i < NUM_GROUPS; i ++) {
	      avgTable [i] = 0;
	      for (int j = 0; j < GROUPSIZE; j ++) 
	         avgTable [i] += hulpTable [i * GROUPSIZE + j];

	      avgTable [i] /= GROUPSIZE;
	   }
	   float noise = 100000.0;
	   for (int i = 0; i < NUM_GROUPS; i ++)
	      if (avgTable [i] < noise)
	         noise = avgTable [i];

//
//	Determining the offset is then easy, look at the corresponding
//	elements in the NUM_GROUPS sections and mark the highest ones.
//	The summation of the high values are stored in the C_table,
//	the number of times the limit is reached in the group
//	is recorded in the D_table
//
//	So, basically we look into GROUPSIZE colums of NUMGROUPS
//	values and look for the maximum
//	Threshold 4 * avgTable is 6 dB, we consider that a minimum
//	measurement shows that that is a reasonable value,
//	alternatively, we could take the "minValue" as reference
//	and "raise" the threshold. However, that might be
//	too  much for 8-bit incoming values
	   memset (D_table, 0, GROUPSIZE * sizeof (int));
	   memset (C_table, 0, GROUPSIZE * sizeof (float));
//
//	We only use the C and D table to locate the start offset
	   for (int i = 0; i < GROUPSIZE; i ++) {
	      for (int j = 0; j < NUM_GROUPS; j ++) {
//	         if (hulpTable [j * GROUPSIZE + i] > threshold * noise) {
	         if (hulpTable [j * GROUPSIZE + i] > threshold * avgTable [j]) {
	            C_table [i] += hulpTable [j * GROUPSIZE + i];
	            D_table [i] ++;
	         }
	      }
	   }
	   float newAvg	= 0;
	   for (int i = 0; i < NUM_GROUPS; i ++)
	      newAvg += avgTable [i];
	   newAvg /= NUM_GROUPS;

//	we extract from this result the highest values that
//	meet the constraint of 4 values being sufficiently high
	   float	maxTable	= 0;
	   int		maxIndex	= -1;
	
	   for (int j = 0; j < GROUPSIZE; j ++) {
	      if ((D_table [j] >= 4) && (C_table [j] > maxTable)) {
	         maxTable = C_table [j];
	         maxIndex = j;
	         break;
	      }
	   }

	   if (maxIndex < 0) {
	      resetBuffer ();
	      for (int i = 0; i < carriers / 2; i ++)
	         decodedBuffer [i] *= 0.0;
	      return theResult;
	   }

	float strength = 10 * log10 (maxTable / (4 * newAvg));
//	The - almost - final step is then to figure out which
//	group contributed most, obviously only where maxIndex  > 0
//	we start with collecting the values of the correct
//	elements of the NUM_GROUPS groups

	   float x [NUM_GROUPS];
	   for (int i = 0; i < NUM_GROUPS; i ++) {
	      x [i] = hulpTable [maxIndex + GROUPSIZE * i];
	   }

//	find the best match
	   int finInd = -1;
////	we extract the four max values as bits
	   float theStrength = 0;
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
	   for (int i = 0; i < 8; i ++) {
	      if (pattern & bits [i]) {
	         int index = maxIndex + i * 24;
	         hulpTable [index] = 0;
	      }
	   }
	   tiiData v;
	   v. subId	= maxIndex;
	   v. mainId	= finInd;	
	   v. strength	= strength;
	   v. pattern	= pattern;
	   v. phase	= 0;
	   v. norm	= false;
	   v. collision	= 0;
	   theResult. push_back (v);
	}
	return theResult;
}

