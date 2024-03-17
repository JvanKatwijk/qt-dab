#
/*
 *    Copyright (C) 2014 .. 2022
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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
 *    along with Qt-DAB if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"audio-player.h"
#include	<stdio.h>


	audioPlayer::audioPlayer	() {}
	audioPlayer::~audioPlayer	() {}

void	audioPlayer::audioOutput	(float *buffer, int amount) {
	fprintf (stderr, "You should not be here\n");
	(void)buffer; (void) amount;
}

void	audioPlayer::stop		() {}

void	audioPlayer::restart		() {}

void	audioPlayer::suspend		() {stop ();}

void	audioPlayer::resume		() {restart ();}

bool	audioPlayer::selectDevice	(int16_t k) {(void)k; return false;}

bool	audioPlayer::hasMissed		() {return false;}

int	audioPlayer::missed		() { return -1;}

