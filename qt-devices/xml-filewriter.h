#
/*
 *    Copyright (C) 2014 .. 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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

#include <QtXml>

#include	<QString>
#include	<stdint.h>
#include	<stdio.h>
#include	<complex>

class Blocks	{
public:
			Blocks		() {}
			~Blocks		() {}
	int		blockNumber;
	int		nrElements;
	QString		typeofUnit;
	int		frequency;
	int		deviceGain;
	QString		modType;
};

class xml_fileWriter {
public:
		xml_fileWriter	(FILE *,
	                         int,
	                         QString,
	                         int,
	                         int,
	                         int,
	                         QString,
	                         QString,
	                         QString);
	                         
			~xml_fileWriter		();
	void		add			(std::complex<int16_t> *, int);
	void		add			(std::complex<uint8_t> *, int);
	void		add			(std::complex<int8_t> *, int);
	void		computeHeader		();
private:
	int		nrBits;
	QString		container;
	int		sampleRate;
	int		frequency;
	QString		deviceName;
	int		deviceGain;
	QString		deviceModel;
	QString		recorderVersion;
	QString		create_xmltree		();
	FILE		*xmlFile;
	QString		byteOrder;
	int		nrElements;
	QString		timeString;
};

