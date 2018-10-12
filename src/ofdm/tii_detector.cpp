#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the Qt-DAB program
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

#include	"tii_detector.h"
#include	<stdio.h>
#include	<inttypes.h>
//
//	Transmitter Identification Info is carrier in the null period
//	of a DAB frame. In case the FIB's carry information on the
//	set of transmitters used in the SFN, an attempt is made
//	to identify the transmitter by inspecting the null period.
//	The information in the null-period is encoded in a "p"
//	a "pattern" and a "c", a "carrier"
//	value. The "p" value defines the
//	pattern within the null-period as well as a set of
//	startcarriers, i.e. carrier numbers where the pattern
//	could start.
//	The start carrier itself determines the "c" value.
//	Basically, within an SFN the "p" is fixed for all transmitters,
//	while the latter show the pattern on different positions in
//	the carriers of the null-period.
//
//	Matching the position of the pattern is relatively easy, since
//	the standard defines the signals (i.e. phase and amplitude) of
//	the carriers in the pattern.
//
//	As it turns out, the pattern is represented by a sequence
//	consisting of elements with two subsequent bins with the same
//	value, followed by a "gap" of K * 48 (-1) bins.
//
//	The constructor of the class generates the patterns, according
//	to the algorithm in the standard.
		TII_Detector::TII_Detector (uint8_t dabMode):
	                                          phaseTable (dabMode),
	                                          params (dabMode),
	                                          my_fftHandler (dabMode) {
int16_t	p, c, k;
int16_t	i;
float	Phi_k;

	this	-> T_u		= params. get_T_u ();
	carriers		= params. get_carriers ();
	theBuffer. resize	(T_u);
	fillCount		= 0;
	fft_buffer		= my_fftHandler. getVector ();	
	window. resize 		(T_u);
	for (i = 0; i < T_u; i ++)
	   window [i]  = (0.42 -
	            0.5 * cos (2 * M_PI * (float)i / T_u) +
	            0.08 * cos (4 * M_PI * (float)i / T_u));

	refTable.               resize (T_u);

	memset (refTable. data (), 0, sizeof (std::complex<float>) * T_u);
	for (i = 1; i <= params. get_carriers () / 2; i ++) {
	   Phi_k =  get_Phi (i);
	   refTable [T_u / 2 + i] = std::complex<float> (cos (Phi_k), sin (Phi_k));
	   Phi_k = get_Phi (-i);
	   refTable [T_u / 2 - i] = std::complex<float> (cos (Phi_k), sin (Phi_k));
	}

	initInvTable	();
	createPattern ();
//	for (i = 0; i < 70; i ++)
//	   printOverlap (i, 15);
}

		TII_Detector::~TII_Detector (void) {
}


//	Zm (0, k) is a function of the P and the C, together forming the key to
//	the database where he transmitter locations are described.
//
//	p is in the range  0 .. 69
//	c is in the range 0 .. 23
//	The transmitted signal - for a given p and c -
//	   Zm (0, k) = A (c, p) (k) e^jPhi (k) + A (c, p) (k - 1) e ^jPhi (k - 1)
//
//	a (b, p) = getBit (table [p], b);

static
uint8_t table [] = {
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


void	TII_Detector::initInvTable() {
	int	i;
	for (i = 0; i < 256; ++i)
	    invTable [i] =  -1;
	for (i = 0; i < 70; ++i) 
	    invTable [table [i]] = i;
}

static
uint8_t bits [] = {0x80, 0x40, 0x20, 0x10 , 0x08, 0x04, 0x02, 0x01};
static inline
uint8_t getbit (uint8_t value, int16_t bitPos) {
	return value & bits [bitPos] ? 1 : 0;
}

static inline
uint8_t delta (int16_t a, int16_t b) {
	return a == b ? 1 : 0;
}
//
//	Litterally copied from the standard. There is no doubt that
//	creating the patterns above can be done more efficient, however
//	this approach feels more readable, while initialization, i.e.
//	executing the constructor, is done once
int16_t	TII_Detector::A (uint8_t c, uint8_t p, int16_t k) {
int16_t b;
int16_t res	= 0;

	if ((-768 <= k) && (k < - 384))
	   for (b = 0; b <= 7; b ++)
	      res += delta (k, -768 + 2 * c + 48 * b) * getbit (table [p], b);
	else
	if ((-384 <= k) && (k < 0))
	   for (b = 0; b <= 7; b ++)
	      res += delta (k, -384 + 2 * c + 48 * b) * getbit (table [p], b);
	else
	if ((0 < k) && (k <= 384))
	   for (b = 0; b <= 7; b ++)
	      res += delta (k, 1 + 2 * c + 48 * b) * getbit (table [p], b);
	else
	if ((384 < k) && (k <= 768))
	   for (b = 0; b <= 7; b ++)
	      res += delta (k, 385 + 2 * c + 48 * b) * getbit (table [p], b);
	else
	   return 0;

	return res;
}
//

void	TII_Detector::reset (void) {
	memset (theBuffer. data (), 0, T_u * sizeof (std::complex<float>));
}

//	To eliminate (reduce?) noise in the input signal, we might
//	add a few spectra before computing (up to the user)
void	TII_Detector::addBuffer (std::vector<std::complex<float>> v) {
int	i;

	for (i = 0; i < T_u; i ++)
	   fft_buffer [i] = cmul (v [i], window [i]);
	my_fftHandler. do_FFT ();

	for (i = 0; i < T_u; i ++)
	    theBuffer [i] = cmul (theBuffer [i], 0.9) +
	                    cmul (fft_buffer [i], 0.1);
}

void	TII_Detector::collapse (std::complex<float> *inVec, float *outVec) {
int	i;

	for (i = 0; i < carriers / 8; i ++) {
	   int carr = - carriers / 2 + 2 * i;
	   outVec [i] = abs (real (inVec [(T_u + carr) % T_u] *
	                            conj (inVec [(T_u + carr + 1) % T_u])));

	   carr	= - carriers / 2 + 1 * carriers / 4 + 2 * i;
	   outVec [i] += abs (real (inVec [(T_u + carr) % T_u] *
	                            conj (inVec [(T_u + carr + 1) % T_u])));

	   carr	= - carriers / 2 + 2 * carriers / 4 + 2 * i + 1;
	   outVec [i] += abs (real (inVec [(T_u + carr) % T_u] *
	                            conj (inVec [(T_u + carr + 1) % T_u])));

	   carr	= - carriers / 2 + 3 * carriers / 4 + 2 * i + 1;
	   outVec [i] += abs (real (inVec [(T_u + carr) % T_u] *
	                            conj (inVec [(T_u + carr + 1) % T_u])));
	}
}

void	TII_Detector::processNULL (int16_t *mainId, int16_t *subId) {
int i, j;
float	hulpTable	[carriers / 8];
float	C_table		[24];	// contains the values
int	D_table		[24];	// marks for locs with data
float	avgTable	[24];
float MMax	= 0;

	*mainId	= -1;
	*subId	= -1;


//	we map the "carriers" carriers (complex values) onto
//	a collapsed vector of "carriers / 8" length, 
//	to be split up into 8 segments of 24 values

	collapse (theBuffer. data (), hulpTable);
//
//	we have now a vector, length 8 times a segment of 24 values
//	where we investigate what the "C" offset is
	memset (C_table, 0, 24 * sizeof (float));
	memset (D_table, 0, 24 * sizeof (int));
//
//	The amplitudes are far from constant over the "carriers" carriers
//	so it seems best to collect an avg value for each of the 
//	segments of 24 values (note however that these are collected
//	from all four regions in the full range of carriers
	memset (avgTable, 0, 24 * sizeof (float));
	for (i = 0; i < 24; i ++) {
	   for (j = 0; j < 8; j ++) 
	      avgTable [i] += hulpTable [i * 8 + j];
	   avgTable [i] /= 8;
	}

//	float	MMin	= 1000000;
//	float avgTotal	= 0;
//	MMax		= 0;
//	for (i = 0; i < 24; i ++) {
//	   avgTotal += avgTable [i];
//	   if (avgTable [i] < MMin)
//	      MMin = avgTable [i];
//	   if (avgTable [i] > MMax)
//	      MMax = avgTable [i];
//	}
//
//	fprintf (stderr, "MMax = %f, MMin = %f, avgTotal = %f\n",
//	                  MMax, MMin, avgTotal / 24);
//	fprintf (stderr, "signal ratio %f, signal avg %f\n",
//	                    20 / 2 * log10 (MMax / MMin),
//	                    20 / 2 * log10 (avgTotal / 24 / MMin));
//	Determining the offset is then easy, look at the corresponding
//	elements in the 8 sections and mark the highest ones
//	The C_table contains the summed values, the
//	D_table counts the amount of groups that contribute
	for (j = 0; j < 8; j ++) {
	   for (i = 0; i < 24; i ++) {
	      if (hulpTable [j * 24 + i] > 3 * avgTable [i]) {
	         C_table [i] += hulpTable [j * 24 + i];
	         D_table [i] ++;
	      }
	   }
	}

//	we mark the highest one that have a score of (at least) 4
//	groups with "high" values

	MMax		= 0;
//	float	MMax	= 0;
	int	indexM	= -1;
//	just walk over the 24 D_table elements to see
//	for indices with enough groups contributing
//	and collect the 4 largest contributers
	for (j = 0; j < 24; j ++) {
	   if (D_table [j] < 4)
	      continue;
	   if (C_table [j] > MMax) {
	      MMax = C_table [j];
	      indexM	= j;
	   }
	}
//
//	The - almost - final step is then to figure out which
//	groups contributed, obviously only where maxTable [x]. index > 0
//	We start with collecting the values of the correct
//	elements of the 8 groups

	if (indexM > 0) {
	   uint16_t pattern	= 0;
	   float x [8];
	   for (i = 0; i < 8; i ++) 
	      x [i] = hulpTable [i * 24 + indexM];
//
//	we extract the four max values (it is known that they are
//	at least as large as N * avg
	   for (i = 0; i < 4; i ++) {
	      float	mmax	= 0;
	      int ind	= -1;
	      for (j = 0; j < 8; j ++) {
	         if (x [j] > mmax) {
	            mmax = x [j];
	            ind  = j;
	         }
	      }
	      if (ind != -1) {
	         x [ind] = 0;
	         pattern |= bits [ind];
	      }
	   }
//
//	The mainId is found using the match with the invTable
	   *mainId	= int (invTable [pattern]);
	   *subId	= indexM;
	   return;
	}
}

void	TII_Detector:: createPattern (void) {
int	p, k, c;

	for (p = 0; p < 70; p ++) {
	   int16_t digits	= 0;
	   c = 0;		// patterns are equal for all c values
	   {
	      bool first = true;
	      int lastOne = 0;
	      for (k = -carriers / 2; k < -carriers / 4; k ++) {
	         if (A (c, p, k) > 0) {
	            if (first) {
	               first = false;
	               theTable [p]. carrier = k;
	               theTable [p]. pattern = 0;
	            }
	            else {
	               theTable [p]. pattern <<= 4;
	               theTable [p]. pattern |= (k - lastOne) / 48;
	               digits ++;
	            }
	            lastOne = k;
	         }
	      }

	      for (k = -carriers / 4; k < 0; k ++) {
	         if (A (c, p, k) > 0) {
	            theTable [p]. pattern <<= 4;
	            theTable [p]. pattern |= (k - lastOne) / 48;
	            lastOne = k;
	            digits ++;
	         }
	      }

	      for (k = 1; k <= carriers / 4; k ++) {
	         if (A (c, p, k) > 0) {
	            theTable [p]. pattern <<= 4;
	            theTable [p]. pattern |= (k - lastOne) / 48;
	            lastOne = k;
	            digits ++;
	         }
	      }

	      for (k = carriers / 4 + 1; k <= carriers / 2; k ++) {
	         if (A (c, p, k) > 0) {
	            theTable [p]. pattern <<= 4;
	            theTable [p]. pattern |= (k - lastOne) / 48;
	            lastOne = k;
	            digits ++;
	         }
	      }
	   }
//	   fprintf (stderr, "p = %d\tc = %d\tk=%d\tpatter=%llX (digits = %d)\n",
//	                     p, c, theTable [p]. carrier,
//	                        theTable [p]. pattern, digits);
	}
}

int	shorten (int carrier) {
	if (carrier > 1 + 384)
	   carrier = carrier - 1 - 3 * 384;
	if (carrier > 0)
	   carrier = carrier - 1 - 2 * 384;
	if (carrier > -384)
	   carrier = carrier - 384;
	return carrier;
}

void	TII_Detector::printOverlap (int pNum, int cNum) {
int i, j;
int	carrier		= theTable [pNum]. carrier + cNum * 2;
uint64_t pattern	= theTable [pNum]. pattern;
int	list [16];
int	list_2 [16];
bool	changes;

	fprintf (stderr, "p (%d) %d:\t", pNum, theTable [pNum]. carrier);
	
        for (i = 0; i < 15; i ++) {

           carrier      += ((pattern >> 56) & 0xF) * 48;
	   list [i]	= carrier;
	   if (list [i] >= 0) list [i] ++;
           pattern <<= 4;
	   list_2 [i] =  shorten (list [i]);
        }

//	for (i = 0; i < 15; i ++)
//	   fprintf (stderr, " %d", list [i]);
	fprintf (stderr, " || ");
	for (i = 0; i < 15; i ++)
	   fprintf (stderr, " %d", list_2 [i]);

	fprintf (stderr, "\n");
}


