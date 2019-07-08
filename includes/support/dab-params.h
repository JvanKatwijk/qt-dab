#
/*
 *    Copyright (C) 2013, 2014, 2015, 2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB (formerly SDR-J, JSDR).
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

#ifndef	__DAB_PARAMS__
#define	__DAB_PARAMS__

#include	<cstdint>

class	dabParams {
public:
			dabParams	(uint8_t);
			~dabParams();
	int16_t		get_dabMode();
	int16_t		get_L();
	int16_t		get_carriers();
	int16_t		get_T_null();
	int16_t		get_T_s();
	int16_t		get_T_u();
	int16_t		get_T_g();
	int32_t		get_T_F();
	int32_t		get_carrierDiff();
	int16_t		get_CIFs();
private:
	uint8_t		dabMode;
	int16_t		L;
	int16_t		K;
	int16_t		T_null;
	int32_t		T_F;
	int16_t		T_s;
	int16_t		T_u;
	int16_t		T_g;
	int16_t		carrierDiff;
	int16_t		CIFs;
};

#endif

