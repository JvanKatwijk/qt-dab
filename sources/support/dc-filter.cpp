#
/*
 *	Copyright (C) 2025
 *	Jan van Katwijk (J.vanKatwijk@gmail.com)
 *	Lazy Chair Computing
 *
 *	This file is part of Qt-DAB
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

#include	"dc-filter.h"

#define	ALPHA	0.95

		dcFilter::dcFilter (int n) {
	ip		= 0;
	filterSize	= n;
	filterData. resize (n);
	for (uint16_t i = 0; i < filterData. size (); i++)
	   filterData [i] = Complex (0, 0);
	filterSum	= Complex (0, 0);
}
		dcFilter::~dcFilter () {}

//Complex	dcFilter::filter (Complex v) {
//	Complex last	= filterData [ip];
//	filterSum	-= last;
//	filterSum	+= v;
//	filterData [ip]	= v;
//	ip = (ip + 1) % filterSize;
//	Complex element	= filterData [(ip + (filterSize - 1) / 2) % filterSize];
//	return element - filterSum / (float)filterSize;
//}

Complex	dcFilter::filter (Complex v) {
	Complex res = v - filterSum;
	filterSum += res * (DABFLOAT)(1.0f - ALPHA);
	return res;
}
	

	 
