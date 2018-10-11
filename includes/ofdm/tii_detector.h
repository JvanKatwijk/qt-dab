#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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

#ifndef	__TII_DETECTOR__
#define	__TII_DETECTOR__

#include	<stdint.h>
#include	"dab-params.h"
#include	"fft-handler.h"
#include	"phasetable.h"
#include	<vector>

class	TII_Detector : public phaseTable {
public:
		TII_Detector	(uint8_t dabMode);
		~TII_Detector	(void);
	void	reset		(void);
	void	addBuffer	(std::vector<std::complex<float>>);
	void	processNULL	(int16_t *, int16_t *);

private:
	void			collapse	(std::complex<float> *,
	                                         float *);
	uint8_t			invTable [256];
	void			initInvTable	(void);
	dabParams		params;
	fftHandler		my_fftHandler;
	int16_t			T_u;
	int16_t			carriers;
	bool			ind;
	std::complex<float>	*fft_buffer;
	std::vector<complex<float> >	theBuffer;
	std::vector<float>	window;
	std::vector<complex<float> >	refTable;
	int16_t		fillCount;
	int16_t		A		(uint8_t c,
	                                            uint8_t p, int16_t k);
	void		createPattern	(void);
	void		printOverlap (int pNum, int cNum);

	struct nullTable {
	   int16_t	carrier;
	   uint64_t	pattern;
	} theTable [70];
};

#endif

