#
/*
 *    Copyright (C) 2010, 2011, 2012
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
 *
 * 	Default (void) implementation of
 * 	virtual input class
 */
#include	"virtual-input.h"

	virtualInput::virtualInput() {
	lastFrequency	= 100000;
	vfoOffset	= 0;
	theGain		= 50;
	coarseOffset	= 0;
	myFrame		= nullptr;
}

	virtualInput::~virtualInput() {
	myFrame		= nullptr;
}

void	virtualInput::setVFOFrequency (int32_t f) {
	lastFrequency = f;
}

int32_t	virtualInput::getVFOFrequency() {
	return lastFrequency;
}

bool	virtualInput::restartReader() {
	return true;
}

void	virtualInput::stopReader() {
}

int32_t	virtualInput::getSamples	(std::complex<float> *v, int32_t amount) {
	(void)v; 
	(void)amount; 
	return amount;
}

int32_t	virtualInput::Samples() {
	return 1024;
}

int32_t	virtualInput::defaultFrequency() {
	return Khz (220000);
}

void	virtualInput::resetBuffer() {
}

void	virtualInput::setOffset		(int32_t o) {
	coarseOffset = o;
}

int32_t	virtualInput::getOffset() {
	return coarseOffset;
}

void	virtualInput::hide	() {
	if (myFrame != nullptr)
	   myFrame	-> hide ();
}

void	virtualInput::show	() {
	if (myFrame != nullptr)
	   myFrame -> show ();
}

bool	virtualInput::isHidden	() { 
	if (myFrame != nullptr)
	   return myFrame -> isHidden ();
	return false;
}

int	virtualInput::getBufferSpace	() {
	return -200;
}

