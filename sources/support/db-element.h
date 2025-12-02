#
/*
 *    Copyright (C)  2015 .. 2023
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
//	In case there is access to a database, the elements of
//	that database are read in and per line converted to am accessible
//	struct
//
//	we use the "key's" for fast detecting the database index (if any)
#include	<string>
#include	<QString>
#include	"distances.h"
//
//	The dbElements are the ones in the database, they contain
//	the (apparently) static data
//	keys are added for easyier lookup
class dbElement {
public:
	uint32_t	key_1;	// Eid + mainId + subId
	uint16_t	key_2;	// channel in Hex format
	bool		valid;
//
//	data returned from the database
	QString		country;
	QString		channel;
	QString		ensemble;
	uint16_t 	Eid;
	uint8_t		mainId;
	uint8_t		subId;
	QString		transmitterName;
	float		latitude;
	float		longitude;
	float		altitude;
	float		height;
	QString		polarization;
	float		frequency;
	float		power;
	QString		direction;
//
	dbElement	() {
	   key_1		= 0;
	   key_2		= 0;
	   valid		= false;
	   ensemble		= "";
	   country		= "not known";
	   channel		= "";
	   ensemble		= "";
	   Eid			= 0;
	   mainId		= 255;
	   subId		= 255;
	   transmitterName	= "not in database";
	   latitude		= 0;
	   longitude		= 0;
	   altitude		= 0;
	   height		= 0;
	   polarization		= "";
	   frequency		= 0;
	   power		= 0;
	   direction		= "";
	}
	~dbElement	()	{}
};

//
//	During processing we add some non-static data
//	and call the stuff "transmitter"
class	transmitter: public dbElement  {
public:
//
//	There are values differing per instance of the 
//	same transmitter
	uint16_t	pattern;
	float		strength;
	float		distance;
	float		azimuth;
	float		phase;
	bool		norm;
	bool		collision;
	bool		isStrongest;
//
//	for the http driver
	uint8_t		type;
	QString		dateTime;
	transmitter	(const dbElement *dbE) {
	   country	= dbE -> country;
           channel	= dbE -> channel;
	   ensemble	= dbE -> ensemble;
	   Eid		= dbE -> Eid;
	   mainId	= dbE -> mainId;
	   subId	= dbE -> subId;
	   transmitterName = dbE -> transmitterName;
	   latitude	= dbE -> latitude;
	   longitude	= dbE -> longitude;
	   altitude	= dbE -> altitude;
	   height	= dbE -> height;
	   polarization	= dbE -> polarization;
	   frequency	= dbE -> frequency;
	   power	= dbE -> power;
	   direction	= dbE -> direction;

	   phase	= 0;
	   norm		= false;
	   collision	= false;
	   isStrongest	= false;
	}

	transmitter () {
	   valid		= false;
	   country		= "not known";
	   channel		= "";
	   ensemble		= "";
	   Eid			= 0;
	   mainId		= 0;
	   subId		= 0;
	   transmitterName	= "not in database";
	   latitude		= 0;
	   longitude		= 0;
	   altitude		= 0;
	   height		= 0;
	   polarization		= "";
	   frequency		= 0;
	   power		= 0;
	   direction		= "";

	   pattern		= 0;
	   strength		= 0;
	   distance		= 0;
	   azimuth		= 0;
	   phase		= 0;
	   norm			= false;
	   collision		= false;
	   isStrongest		= false;
	}
	~transmitter	()	{}
};

