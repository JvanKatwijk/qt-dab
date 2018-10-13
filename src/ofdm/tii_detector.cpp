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


#define	NUM_GROUPS	8
#define	GROUPSIZE	24
void	TII_Detector::processNULL (int16_t *mainId, int16_t *subId) {
int i, j;
float	hulpTable	[NUM_GROUPS * GROUPSIZE];
float	C_table		[GROUPSIZE];	// contains the values
int	D_table		[GROUPSIZE];	// marks for indices in C_table with data
float	avgTable	[NUM_GROUPS];
//
//	defaults:
	*mainId	= -1;
	*subId	= -1;


//	we map the "carriers" carriers (complex values) onto
//	a collapsed vector of "carriers / 8" length, 
//	considered to consist of 8 segments of 24 values
//	Each "value" is the sum of 4 pairs of subsequent carriers,
//	taken from the 4 quadrants -768 .. 385, 384 .. -1, 1 .. 384, 385 .. 768

	collapse (theBuffer. data (), hulpTable);
//
//	since the "energy levels" in the different GROUPSIZE'd values
//	may differ, we compute an average for each of the
//	NUM_GROUPS GROUPSIZE-value groups

	memset (avgTable, 0, NUM_GROUPS * sizeof (float));
	for (i = 0; i < NUM_GROUPS; i ++) {
	   for (j = 0; j < GROUPSIZE; j ++)
	      avgTable [i] += hulpTable [i * GROUPSIZE + j];
	   avgTable [i] /= GROUPSIZE;
	}
//
//	
//	Determining the offset is then easy, look at the corresponding
//	elements in the NUM_GROUPS sections and mark the highest ones
//	The summation of the high values are stored in the C_table,
//	the number of times the limit is reached in the group
//	is recorded in the D_table
//
//	4 * avgTable is 6dB, we consider that a minimum
	memset (D_table, 0, GROUPSIZE * sizeof (int));
	memset (C_table, 0, GROUPSIZE * sizeof (float));
//
	for (j = 0; j < NUM_GROUPS; j ++) {
	   for (i = 0; i < GROUPSIZE; i ++) {
	      if (hulpTable [j * GROUPSIZE + i] > 4 * avgTable [j]) {
	         C_table [i] += hulpTable [j * GROUPSIZE + i];
	         D_table [i] ++;
	      }
	   }
	}
	
//
//	we extract from the group the two highest values that
//	meet the constraint of 4 values that are sufficiently high
	float	Max_1	= 0;
	int	ind1	= -1;
	float	Max_2	= 0;
	int	ind2	= -1;

	for (j = 0; j < GROUPSIZE; j ++) {
	   if ((D_table [j] >= 4) && (C_table [j] > Max_1)) {
	      Max_2	= Max_1;
	      ind1	= ind2;
	      Max_1	= C_table [j];
	      ind1	= j;
	   }
	   else
	   if ((D_table [j] >= 4) && (C_table [j] > Max_2)) {
	      Max_2	= C_table [j];
	      ind2	= j;
	   }
	}
//
//	The - almost - final step is then to figure out which
//	groups contributed, obviously only where ind1 > 0
//	we start with collecting the values of the correct
//	elements of the NUM_GROUPS groups
//
//	for the qt-dab, we only need the "top" performer
	if (ind1 > 0) {
	   uint16_t pattern	= 0;
	   float x [NUM_GROUPS];
	   for (i = 0; i < NUM_GROUPS; i ++) 
	      x [i] = hulpTable [ind1 + GROUPSIZE * i];
//
//	we extract the four max values (it is known that they exist)
	   for (i = 0; i < 4; i ++) {
	      float	mmax	= 0;
	      int ind	= -1;
	      for (j = 0; j < NUM_GROUPS; j ++) {
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
	   *subId	= ind1;
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


