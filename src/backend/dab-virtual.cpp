#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB.
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
 */
//
//	dummy for the dab handler
//
#include	"dab-constants.h"
#include	"dab-virtual.h"

#include	"radio.h"		// defines RadioInterface
#include	"audiosink.h"	// defines audioSink

	dabVirtual::dabVirtual	(void) {
}

	dabVirtual::~dabVirtual	(void) {
}

int32_t	dabVirtual::process	(int16_t *v, int16_t c) {
	(void)v;
	(void)c;
	return 32768;
}

void	dabVirtual::stopRunning	(void) {
}

void	dabVirtual::stop	(void) {
}

