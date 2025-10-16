#
/*
 *    Copyright (C) 2015 .. 2025
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

//
//	Since handling journaline implies showing an interactive screen
//	it was found to be "better" to handle the journaline data
//	in the main thread.
//	A small controller is added, interfacing between
//	the backend processes and the actual handler
#include	"journaline-controller.h"
#include	"radio.h"
//
static int key	= 0;

	journalineController::journalineController (RadioInterface *mr) {
	connect (this, &journalineController::startJournaline,
	         mr, &RadioInterface::startJournaline);
	connect (this, &journalineController::stopJournaline,
	         mr, &RadioInterface::stopJournaline);
	connect (this, &journalineController::send_mscDatagroup,
	         mr, &RadioInterface::journalineData);
	currentKey	= key;
	key ++;
	emit startJournaline (currentKey);
}

	journalineController::~journalineController () {
	emit stopJournaline (currentKey);
}

void	journalineController::
	                add_mscDatagroup (const std::vector<uint8_t> &msc) {
QByteArray v;
	for (int i = 0; i < (int)(msc. size ()); i ++)
	   v. push_back (msc [i]);
	emit send_mscDatagroup (v, currentKey);
}

