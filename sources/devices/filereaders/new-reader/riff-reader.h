#
/*
 *    Copyright (C) 2016 .. 2023
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

#pragma once

#include	<stdio.h>
#include	<complex>
#include	<QString>
#include        "device-exceptions.h"

class	riffReader {
public:
			riffReader	(const QString &);
			~riffReader	();
	void		reset		();
	int		read		(std::complex<float> *, uint64_t);
	uint64_t	elementCount	();
	QString		fileType	();
	int		sampleSize	();
	uint64_t	currentPos	();
	void		set_newPosition	(uint64_t);
	int		getVFOFrequency	();
	QString		getDevice	();
private:

	QString		fileName;
	QString		theDevice;
	uint16_t	formatTag;
        uint16_t	nrChannels;
        uint32_t	samplingRate;

	std::atomic<uint64_t> newPosition;
	void	setupFor_wavType	(uint32_t);
	void	setupFor_bw64Type	(uint32_t);
	int	read4Bytes		(std::complex<float> *, uint64_t);
	int	read6Bytes		(std::complex<float> *, uint64_t);
	int	read8Bytes		(std::complex<float> *, uint64_t);
	FILE	*filePointer;
	uint64_t	remainingElements;
	uint64_t	nrElements;
	uint16_t	blockAlign;
	std::fpos_t	baseofData;
	int		tunedFrequency;
	int		bitDepth;
	float		denominator;
	int		readBytes;
	QString		typeOfFile;

	uint64_t	basePosition;
};
