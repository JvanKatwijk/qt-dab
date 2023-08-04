#
/*
 *    Copyright (C) 2014 .. 2020
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
 *    along with Qt-DAB-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"device-handler.h"

	deviceHandler::deviceHandler	() {
	lastFrequency	= 100000;
	vfoOffset	= 0;
	theGain		= 50;
	coarseOffset	= 0;
}

	deviceHandler::~deviceHandler	() {
}

bool	deviceHandler::restartReader	(int32_t freq) {
	(void)freq;
	return true;
}

void	deviceHandler::stopReader	() {
}

void	deviceHandler::setVFOFrequency	(int freq) {
	(void)freq;
}

int32_t	deviceHandler::getSamples	(std::complex<float> *v,
	                                               int32_t amount) {
	(void)v; 
	(void)amount; 
	return amount;
}

int32_t	deviceHandler::Samples		() {
	return 1024;
}

void	deviceHandler::resetBuffer	() {
}

void	deviceHandler::hide		() {
}

void	deviceHandler::show		() {
}

bool	deviceHandler::isHidden		() { 
	return false;
}

QString	deviceHandler::deviceName	() {
	return "";
}

bool	deviceHandler::isFileInput	() {
	return false;
}

QPoint	deviceHandler::get_coords	() {
	return QPoint (0, 0);
}

void	deviceHandler::moveTo		(QPoint p) {
	(void)p;
}


