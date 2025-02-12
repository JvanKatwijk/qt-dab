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
#include "tii-detector-1.h"
constexpr float F_DEG_PER_RAD = (float)(180.0 / M_PI);


#define	SECTION_SIZE	192
#define	NR_SECTIONS	4

	TII_Detector_A::TII_Detector_A (uint8_t dabMode,
	                                phaseTable *theTable) :
	                                     TII_Detector (dabMode,
	                                                   theTable) {
	carrierDelete	= false;
	reset();
}

	TII_Detector_A::~TII_Detector_A () { }


void	TII_Detector_A::reset		() {
	resetBuffer();
	memset ((void *)decodedBuffer, 0, carriers / 2 * sizeof (Complex));
}

//	we map the "K" carriers (complex values) onto
//	a collapsed vector of "K / 8" length,
//	considered to consist of 8 segments of 24 values
//	Each "value" is the sum of 4 pairs of subsequent carriers,
//	taken from the 4 quadrants -768 .. 385, 384 .. -1, 1 .. 384, 385 .. 768
void	TII_Detector_A::collapse (const Complex *inVec,
	                          Complex *etsiVec, Complex *nonetsiVec,
	                          bool tiiFilter) {
Complex buffer [carriers / 2];

	memcpy (buffer, inVec, carriers / 2 * sizeof (Complex));

	int nrSections	= tiiFilter ? 2 : 4;
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
	      nonetsiVec [i] += x * conj (table_2 [i + j * SECTION_SIZE]);
	   }
	}
}

//	We determine first the offset of the "best fit",
//	the offset indicates the subId
static uint8_t bits [] = {0x80, 0x40, 0x20, 0x10 , 0x08, 0x04, 0x02, 0x01};

// Sort the elemtnts accordibg to their strength
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

QVector<tiiData> TII_Detector_A::processNULL (int16_t threshold_db, 
	                                      uint8_t selected_subId,
	                                      bool tiiFilter) {
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

	collapse (decodedBuffer, etsiTable, nonetsiTable, tiiFilter);


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
	      float etsi_noiseLevel = tiiFilter ? avg_etsi [i] : noise;
	      float nonetsi_noiseLevel = tiiFilter ? avg_nonetsi [i] : noise;
	      if (etsi_floatTable [subId + i * GROUPSIZE] >
	                                      etsi_noiseLevel * threshold) {
	         etsi_count++;
	         etsi_pattern	|= bits [i];
	         etsi_sum	+= etsiTable [subId + GROUPSIZE * i]; 
	      }
	      if (nonetsi_floatTable [subId + i * GROUPSIZE] >
	                                    nonetsi_noiseLevel * threshold) {
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
	         if (getPattern (mainId) == pattern)
	            break;
	   }

//	Find the best match. We extract the four max values as bits
	   else
	   if (count > 4) {
	      float mm = 0;
	      for (int k = 0; k < (int)nrPatterns (); k++) {
	         Complex val = Complex (0, 0);
	         for (int i = 0; i < NUM_GROUPS; i++) {
	            if (getPattern (k) & bits [i]) {
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
	      element. strength = jan_abs (sum) / max / (tiiFilter ? 2 : 4);
	      element. phase	= arg (sum) * F_DEG_PER_RAD;
	      element. norm	= norm;
	      element. collision	= false;
	      element. pattern	= getPattern (mainId);
	      theResult. push_back (element);
	   }
//
//	Collisions still to be done
	   if ((count > 4) && (selected_subId != 0)) {
	      sum = Complex (0,0);

//	Calculate the level of the second main ID
	      for (int i = 0; i < NUM_GROUPS; i++) {
	         if ((getPattern (mainId) & bits [i]) == 0) {
	            int index = subId + GROUPSIZE * i;
	            if (float_ptr [index] > noise * threshold)
	               sum += cmplx_ptr [index];
	         }
	      }

	      if (subId == selected_subId) { // List all possible main IDs
	         for (int k = 0; k < (int)nrPatterns (); k++) {
	            int pattern2 = getPattern (k) & pattern;
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
	               element. pattern		= getPattern (mainId);
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
