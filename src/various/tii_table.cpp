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
#include	"tii_table.h"


	tii_element::tii_element (int16_t subId,
	                     float latitudeOffset, float longitudeOffset) {
	this	-> subId	= subId;
	this	-> latitudeOffset = latitudeOffset;
	this	-> longitudeOffset = longitudeOffset;
}

	tii_table::tii_table (void) {
}

	tii_table::~tii_table (void) {
}

void	tii_table::cleanUp (void) {
	offsets. clear ();
	mainId		= -1;
}

void	tii_table::add_main	(int16_t mainId, float latitude, float longitude) {
	if (this -> mainId > 0)
	   return;
	this	-> mainId = mainId;
	this	-> latitude	= latitude;
	this	-> longitude	= longitude;
}

void	tii_table::add_element (tii_element *t) {
int16_t i;

	for (i = 0; i < offsets. size (); i ++)
	   if (offsets [i]. subId == t -> subId)
	      return;

	fprintf (stderr, "adding %d (%f %f)\n",
	                        t -> subId,
	                        t -> latitudeOffset,
	                        t -> longitudeOffset);
	offsets. push_back (*t);
	
}

DSPCOMPLEX tii_table::get_coordinates (int16_t mainId,
	                               int16_t subId, bool *success) {
int16_t i;
float x, y;

	*success	= false;
	if (this -> mainId != mainId)
	   return DSPCOMPLEX (0, 0);

	fprintf (stderr, "central point (%f %f)\n", 
	                               latitude, longitude);
//	print_coordinates ();
	for (i = 0; i < offsets. size (); i ++) {
	   if (offsets [i]. subId != subId)
	      continue;

	   x	= latitude + offsets [i]. latitudeOffset;
	   y	= longitude + offsets [i]. longitudeOffset;
	
	   *success = true;
	   return DSPCOMPLEX (x, y);
	}
	return DSPCOMPLEX (0, 0);
}

void	tii_table::print_coordinates	(void) {
int16_t	i;

	if (mainId < 0)
	   return;

	fprintf (stderr, "mainId = %d\n", mainId);
	fprintf (stderr, "Transmitter coordinates (%f %f)\n",
	                              latitude, longitude);
	for (i = 0; i < offsets. size (); i ++) {
	   float x	= latitude + offsets [i]. latitudeOffset;
	   float y	= longitude + offsets [i]. longitudeOffset;
	   fprintf (stderr, "%d\t-> %f\t%f\n", offsets [i]. subId, x, y);
	}
}

int16_t	tii_table::get_mainId	(void) {
	return mainId;
}

