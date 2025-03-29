#
/*
 *    Copyright (C)  2015 .. 2025
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
#include	<stdint.h>
#include	<vector>
#include	<QString>

//	ensemble information relates to FIG1, basically some
//	general, i.e. ensemble wide, data and mapping tables for
//	primary and secondary services to SId's resp (SId x SCIds)
//	Itis now completely separated from the FIG1 tables
class ensemble {
public:
		ensemble	() {
	   reset ();
	}
		~ensemble	() {
	}

	uint8_t		eccByte;
	uint8_t		lto;
	uint16_t	EId;
	QString		ensembleName;
	bool		namePresent;
	typedef struct {
	   QString	name;
	   QString	shortName;
	   uint32_t	SId;
	   uint8_t	programType;
	   uint8_t	SCIds;
	   int		fmFrequency;
	} service;


	bool	isSynced;
	std::vector<service> primaries;
	std::vector<service> secondaries;

	void	reset		();
	uint32_t serviceToSId	(const QString &s);
	QString	SIdToserv	(uint32_t SId);
	int	programType	(uint32_t);
	int	fmFrequency	(uint32_t);

};


