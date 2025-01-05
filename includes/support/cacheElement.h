#
/*
 *    Copyright (C)  2015, 2023
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

//
//	In case there is aaccess to a database, the elements of
//	that database are read in and per line converted to am accessible
//	struct
#include	<string>
#include	<QString>

class cacheElement {
public:
	bool		valid;
	QString		country;
	QString		channel;
	QString		ensemble;
	QString		location;
	uint16_t 	Eid;
	uint8_t		mainId;
	uint16_t	pattern;
	uint8_t		subId;
	QString		transmitterName;
	float		latitude;
	float		longitude;
	float		power;
	float		altitude;
	float		height;
	QString		polarization;
	float		frequency;
	QString		direction;
//
//	There are values differing per instance of the 
//	same transmitter
	float		strength;
	float		distance;
	float		azimuth;
	float		phase;
	bool		norm;
	bool		collision;
	
	cacheElement	() {
	   valid		= false;
	   country		= "not known";
	   channel		= "";
	   ensemble		= "";
	   Eid			= 0;
	   mainId		= 255;
	   subId		= 255;
	   transmitterName	= "not in database";
	   latitude		= 0;
	   longitude		= 0;
	   power		= 0;
	   height		= 0;
	   power		= 0;
	   polarization		= "";
	   frequency		= 0;
	   direction		= "";
	   phase		= 0;
	   norm			= false;
	   collision		= false;
	}
	~cacheElement	()	{}
};


struct transmitterDesc {
	bool	isValid;
	bool	isStrongest;
	bool	isFurthest;
	cacheElement theTransmitter;
};

