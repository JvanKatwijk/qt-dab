#
/*
 *    Copyright (C) 2016 .. 2025
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
//
//	This version of riffWriter starts assuming the filesize will remain
//	below 4 G, if - when closing - it turns out the the file size exceeds
//	the 4G border, the type is changed into BW64

#pragma once

#include	<QString>
#include	<stdio.h>
#include	<stdint.h>

class riffWriter {
public:
		riffWriter	();
		~riffWriter	();
	bool	init		(const QString &, int,
	                         int bitDepth = 0,
	                         int frequency = 0,
	                         QString creator = "Qt-DAB-6.9.3");
	void	write		(int16_t *buff, uint64_t samples);
	void	close		();
	bool	isActive	();
private:
	FILE		*filePointer;
	uint64_t	nrElements;
	std::fpos_t	dataPos;
	bool		isValid;
	uint64_t	locationCounter;
};

