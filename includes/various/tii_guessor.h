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

#ifndef	__TII_GUESSOR__
#define	__TII_GUESSOR__

#include	<stdint.h>
#include	"dab-params.h"
#include	"fft-handler.h"
//
//	Transmitter Identification Info is carrier in the null period
//	of a DAB frame. In case the FIB's carry information on the
//	set of transmitters used in the SFN, an attempt is made
//	to identify the transmitter by inspecting the null period.
//	The information in the null-period is encoded in a "p"
//	a "pattern" and a "c", a "carrier"
//	value. The "p" value, derived from the FIB, defines the
//	pattern within the null-period as well as a set of
//	startcarriers, i.e. carrier numbers where the pattern
//	could start.
//	The start carrier itself determined the "c" value.
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

	class TII_Guessor {
public:
		TII_Guessor	(uint8_t);
		~TII_Guessor	(void);
	void	reset		(void);
	void	addBuffer	(std::complex<float> *);
	bool	guess		(int16_t *);
private:
	dabParams	 params;
	fftHandler	my_fftHandler;
	int		T_u;
	int		 carriers;
	std::complex<float>*	 theBuffer;
	std::complex<float>	*fft_buffer;
	float			*window;
};
#endif

