#
/*
 *    Copyright (C) 2017
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
#include	"dab-constants.h"
#include	<vector>

class	tii_element {
public:
	int16_t	subId;
	float	latitudeOffset;
	float	longitudeOffset;
	tii_element (int16_t subId,
	             float latitudeOffset, float longitudeOffset);
	~tii_element (void) {}
};

class	tii_table {
public:
		tii_table		(void);
		~tii_table		(void);
	void	cleanUp			(void);
	void	add_element		(tii_element *);
	void	add_main		(int16_t, float, float);
	DSPCOMPLEX	get_coordinates (int16_t, int16_t, bool *);
	void	print_coordinates	(void);
	int16_t	get_mainId		(void);
	int16_t	mainId;
	float	latitude;
	float	longitude;
	std::vector<tii_element> offsets;
};

