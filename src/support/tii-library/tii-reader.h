#
/*
 *    Copyright (C) 2014 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
#include	<QString>
#include	<math.h>
#include	"dab-constants.h"
#include	"cacheElement.h"
#include	<QSettings>

#ifdef	__MING32__
#include	<winsock2.h>
#include	<windows.h>
#endif
class	tiiReader {
public:
		tiiReader	();
		~tiiReader	();
//
	std::vector<cacheElement>
	        readFile	(const QString &s);
	int	readColumns	(std::vector<QString> &v, char *b, int N);
	char	*eread		(char *buffer, int amount,
	                                        FILE *f, uint8_t shift);
//	bool	loadTable	(const QString &tf);
private:
	float	convert		(const QString &);
	uint16_t get_Eid	(const QString &);
	uint8_t	get_mainId	(const QString &);
	uint8_t	get_subId	(const QString &);
	FILE	*curl_f;
	bool	Rflag;
};

