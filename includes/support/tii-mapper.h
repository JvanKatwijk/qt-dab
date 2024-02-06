#
/*
 *    Copyright (C) 2014 .. 2017
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
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#pragma once
#include	<stdint.h>
#include	<QString>
#include	<stdio.h>
#include	<vector>
#include	<QSettings>
#include	"table-loader.h"

typedef struct {
	float	latitude;
	float	longitude;
} position;

typedef struct {
	uint16_t Eid;
	uint8_t mainId;
	uint8_t subId;
} black;

class	tiiMapper {
public:
		tiiMapper	();
		~tiiMapper	();
	bool	tiiFile 	(const QString &);
	bool	loadTable	(const QString &);
	QString	get_transmitterName (const QString &,
	                             uint16_t Eid,
	                             uint8_t mainId, uint8_t subId);
	void	get_coordinates	(float *, float *, float *,
	                         const QString &, const QString &);
	void	get_coordinates	(position &, float &,
	                         const QString &, const QString &);
	float	distance_2	(float, float, float, float);
	float	distance	(position, position);
	float	distance	(float, float, float, float);
	float	corner		(position, position);
	bool	is_black	(uint16_t, uint8_t, uint8_t);
	void	set_black	(uint16_t, uint8_t, uint8_t);	

private:
std::vector<black> blackList;
std::vector<cacheElement> theCache;
QString	tiifileName;
	tableLoader	theReader;
};

