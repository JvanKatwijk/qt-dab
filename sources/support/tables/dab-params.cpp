#
/*
 *    Copyright (C) 2016 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
#include	"dab-params.h"

constexpr int16_t	L		= 76;
constexpr int16_t	K		= 1536;
constexpr int32_t	T_F		= 196608;
constexpr int16_t	T_null		= 2656;
constexpr int16_t	T_s		= 2552;
constexpr int16_t	T_u		= 2048;
constexpr int16_t	T_g		= 504;
constexpr int16_t	carrierDiff	= 1000;
constexpr int16_t	CIFs		= 4;

int16_t get_L () {
	return L;
}

int16_t	get_carriers () {
	return K;
}

int16_t	get_T_null () {
	return T_null;
}

int16_t	get_T_s () {
	return T_s;
}

int16_t	get_T_u () {
	return T_u;
}

int16_t	get_T_g () {
	return T_g;
}

int32_t	get_T_F () {
	return T_F;
}

int32_t	get_carrierDiff () {
	return carrierDiff;
}

int16_t	get_CIFs () {
	return CIFs;
}

