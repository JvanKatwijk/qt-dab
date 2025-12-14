#
/*
 *    Copyright (C) 2016 .. 2023
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

#include	"tii-detector.h"
#include	"dab-params.h"
#include	<cstdio>
#include	<cinttypes>
#include	<cstring>
#include	<QSettings>

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


		TII_Detector::TII_Detector (uint8_t dabMode,
	                                     phaseTable *theTable):
	                                     params (dabMode),
	                                     T_u (params. get_T_u ()),
	                                     T_g (params. get_T_g ()),
	                                     carriers (params. get_carriers ()),
	                                     my_fftHandler (params. get_T_u (),
	                                                    false) {
	nullSymbolBuffer. resize (T_u);
	window. resize (T_u);
	for (int i = 0; i < T_u; i ++)
	   window [i] = 0.54 - 0.46 * cos (2 * M_PI * (DABFLOAT)i / T_u);

	rotationTable. resize (carriers);
        int teller = 0;
        for (int carrier = - carriers / 2;
                       carrier < carriers / 2; carrier += 2) {
           int index    = carrier < 0 ? carrier + T_u : carrier + 1;
	   Complex r	= theTable -> refTable [index] *
                                 conj (theTable -> refTable [index + 1]);
//
//	the "arg" or r is one of 0 .. 3 * PI/2
	   float aa = (arg (r) < 0) ? arg (r) + 2 * M_PI : arg (r);
	   int bb  = aa < 1 ? 0 :
	                aa < 2 ? 1:
	                aa < 3.5 ? 2 : 3;
	   rotationTable [teller ++] = bb;
        }

}

	TII_Detector::~TII_Detector () {
}

void	TII_Detector::resetBuffer	() {
	for (int i = 0; i < T_u; i ++)
	   nullSymbolBuffer [i] = Complex (0, 0);
}

void	TII_Detector::reset		() {
	resetBuffer ();
}

//	To eliminate (reduce?) noise in the input signal, we might
//	add a few spectra before computing (up to the user)
void	TII_Detector::addBuffer (const std::vector<Complex>  &v) {
Complex tmpBuffer [T_u];

	for (int i = 0; i < T_u; i ++)
//	   tmpBuffer [i] = v [i];
	   tmpBuffer [i] = v [T_g + i];
	my_fftHandler. fft (tmpBuffer);
	for (int i = 0; i < T_u; i ++)
	   nullSymbolBuffer [i] += tmpBuffer [i];
}


QVector<tiiData> TII_Detector::processNULL (int16_t threshold_db,
                                              uint8_t selected_subId,
                                              bool tiiFilter) {
	(void)threshold_db;
	(void)selected_subId,
	(void)tiiFilter;
	QVector<tiiData> result;
	return result;
}

uint8_t		TII_Detector::getRotation (int n) {
	return rotationTable [n];
}

uint16_t	TII_Detector::getPattern (int n) {
	return patternTable [n];
}

uint16_t	TII_Detector::nrPatterns () {
	return (uint16_t)sizeof (patternTable);
}

// rotate  0, 90, 180 or -90 degrees
Complex		TII_Detector::rotate (Complex value, uint8_t phaseIndicator) {   
	switch (phaseIndicator) {   
	   case 3:
	      return Complex (-imag (value), real (value));
	   case 2:
	      return -value;
	   case 1:
	      return Complex (imag (value), -real (value));
	   case 0:
              return value;
	   default: 	// should not happen
	      return value;
	}
}


