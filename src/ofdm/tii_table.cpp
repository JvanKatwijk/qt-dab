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
#include	"radio.h"

	tii_element::tii_element (int16_t subId,
	                          float latitudeOffset,
	                          float longitudeOffset) {
	this	-> subId	= subId;
	this	-> latitudeOffset = latitudeOffset;
	this	-> longitudeOffset = longitudeOffset;
}

	tii_table::tii_table (RadioInterface *mr) {
	   myRadio	= mr;
//	   connect (this, SIGNAL (show_tiiLabel (int)),
//	            mr,   SLOT   (show_tiiLabel (int)));
}

	tii_table::~tii_table() {
}

void	tii_table::cleanUp() {
	tiiLocker. lock();
	offsets. clear();
	mainId		= -1;
	tiiLocker. unlock();
//	show_tiiLabel (mainId);
}

void	tii_table::add_main	(int16_t mainId,
	                         float latitude, float longitude) {
	tiiLocker. lock();
	if (this -> mainId > 0) {
	   tiiLocker. unlock();
	   return;
	}
	this	-> mainId	= mainId;
	this	-> latitude	= latitude;
	this	-> longitude	= longitude;
	tiiLocker. unlock();
//	show_tiiLabel (mainId);
}

void	tii_table::add_element (tii_element *t) {
int16_t i;
	tiiLocker. lock();
	if (this -> mainId < 0) {
	   tiiLocker. unlock();
	   return;
	}
	for (i = 0; i < (int16_t) offsets. size(); i ++)
	   if (offsets [i]. subId == t -> subId) {
	      tiiLocker. unlock();
	      return;
	   }
	fprintf (stderr, "transmitter  %d at\t(%f %f)\n",
	                        t -> subId,
	                        latitude + t -> latitudeOffset,
	                        longitude + t -> longitudeOffset);
	offsets. push_back (*t);
	tiiLocker. unlock();
	
}

std::complex<float>
	tii_table::get_coordinates (int16_t mainId,
	                            int16_t subId, bool *success) {
int16_t i;
float x, y;

	tiiLocker. lock();
	*success	= false;
	if (this -> mainId != mainId) {
	   tiiLocker. unlock();
	   return std::complex<float> (0, 0);
	}

//	print_coordinates();
	for (i = 0; i < (int16_t) offsets. size(); i ++) {
	   if (offsets [i]. subId != subId)
	      continue;
	   x	= latitude + offsets [i]. latitudeOffset;
	   y	= longitude + offsets [i]. longitudeOffset;
	
	   *success = true;
	   tiiLocker. unlock();
	   return std::complex<float> (x, y);
	}
	tiiLocker. unlock();
	return std::complex<float> (0, 0);
}

void	tii_table::print_coordinates() {
int16_t	i;

	tiiLocker. lock();
	if (mainId < 0) {
	   tiiLocker. unlock();
	   return;
	}

	fprintf (stderr, "mainId = %d\n", mainId);
	fprintf (stderr, "Transmitter coordinates (%f %f)\n",
	                              latitude, longitude);
	for (i = 0; i <  (int16_t)offsets. size(); i ++) {
	   float x	= latitude + offsets [i]. latitudeOffset;
	   float y	= longitude + offsets [i]. longitudeOffset;
	   fprintf (stderr, "%d\t-> %f\t%f\n", offsets [i]. subId, x, y);
	}
	tiiLocker. unlock();
}

int16_t	tii_table::get_mainId() {
int16_t m;
	tiiLocker. lock();
	m	= mainId;
	tiiLocker. unlock();
	return m;
}

