#
/*
 *    Copyright (C) 2013 .. 2024
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB 
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

#include	"time-converter.h"

//	Modified Julian Date (recompute according to wikipedia)
void	convertTime (uint32_t mjd, uint16_t *out) {
int32_t J	= mjd + 2400001;
int32_t j	= J + 32044;
int32_t g	= j / 146097; 
int32_t	dg	= j % 146097;
int32_t c	= ((dg / 36524) + 1) * 3 / 4; 
int32_t dc	= dg - c * 36524;
int32_t b	= dc / 1461;
int32_t db	= dc % 1461;
int32_t a	= ((db / 365) + 1) * 3 / 4; 
int32_t da	= db - a * 365;
int32_t y	= g * 400 + c * 100 + b * 4 + a;
int32_t m	= ((da * 5 + 308) / 153) - 2;
int32_t d	= da - ((m + 4) * 153 / 5) + 122;
int32_t Y	= y - 4800 + ((m + 2) / 12); 
int32_t M	= ((m + 2) % 12) + 1; 
int32_t D      = d + 1;


	out [0] = Y;
	out [1] = M;
	out [2] = D;
}


