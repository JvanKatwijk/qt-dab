#
/*
 *    Copyright (C) 2014 .. 2023
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

#include	<stdio.h>
#include	<QString>
#include	<math.h>
#include	"dab-constants.h"
#include	<QSettings>

typedef struct {
	QString	country;
	QString	channel;
	QString ensemble;
	uint16_t Eid;
	uint8_t mainId;
	uint8_t	subId;
	QString transmitterName;
	float	latitude;
	float	longitude;
	float	power;
} cacheElement;

//	DLL and ".so" function prototypes
typedef void    *(*init_tii_P)  ();
typedef void    (*close_tii_P)  (void *);
typedef void    (*loadTable_P)  (void *, const std::string &);

class	tableLoader {
public:
		tableLoader	();
		~tableLoader	();
//
	std::vector<cacheElement>
	        readFile	(const QString &s);
	int	readColumns	(std::vector<QString> &v, char *b, int N);
	char	*eread		(char *buffer, int amount,
	                                        FILE *f, uint8_t shift);
	bool	loadTable	(const QString &tf);
private:
	void    *handler;
        HINSTANCE       Handle; 
        HINSTANCE       getLibraryHandle        ();

	float	convert		(const QString &);
	uint16_t get_Eid	(const QString &);
	uint8_t	get_mainId	(const QString &);
	uint8_t	get_subId	(const QString &);
	bool	loadFunctions   ();
        init_tii_P      init_tii_L;
        close_tii_P     close_tii_L;
        loadTable_P     loadTable_L;
};

