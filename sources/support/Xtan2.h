#
/*
 *    Copyright (C) 2014 .. 2024
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
 *
 *	This LUT implementation of atan2 is a C++ translation of
 *	a Java discussion on the net
 *	http://www.java-gaming.org/index.php?topic=14647.0
 */

#pragma once

#include	<math.h>
#include	<stdio.h>
#include	<stdint.h>
#include	<cstdlib>
#include	<limits>
#include	"dab-constants.h"
#
class	compAtan {
public:
		compAtan	(void);
		~compAtan	(void);
	float	atan2		(float, float);
	float	argX		(Complex);
private:
	float	*ATAN2_TABLE_PPY;
	float	*ATAN2_TABLE_PPX;
	float	*ATAN2_TABLE_PNY;
	float	*ATAN2_TABLE_PNX;
	float	*ATAN2_TABLE_NPY;
	float	*ATAN2_TABLE_NPX;
	float	*ATAN2_TABLE_NNY;
	float	*ATAN2_TABLE_NNX;
	float	Stretch;
};
