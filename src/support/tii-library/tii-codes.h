#
/*
 *    Copyright (C) 2014 .. 2022
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation recorder 2 of the License.
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
#pragma once

#include	<stdint.h>
#include	<QString>
#include	<QSettings>
#include	"dlfcn.h"
typedef	void	*HINSTANCE;

typedef struct {
	float latitude;
	float longitude;
} position;

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

typedef struct {
	uint16_t Eid;
	uint8_t mainId;
	uint8_t subId;
} black;

//	DLL and ".so" function prototypes
typedef void	*(*init_tii_P)	();
typedef void	(*close_tii_P)	(void *);
typedef void	(*loadTable_P)	(void *, const std::string &);

class	tiiHandler {
public:
		tiiHandler	();
		~tiiHandler	();
	bool	tiiFile		(const QString &);
	QString	get_transmitterName	(const QString &,
	                                 uint16_t, uint8_t, uint8_t);
	void	get_coordinates	(float *, float *, float *,
	                         const QString &, const QString &);
	void	get_coordinates	(position &, float &,
	                         const QString &, const QString &);
        int     distance_2	(float, float, float, float);
        int	distance	(position &, position &);
        int	distance	(float, float, float, float);
        int     corner		(position, position);
        bool    is_black        (uint16_t, uint8_t, uint8_t);
        void    set_black       (uint16_t, uint8_t, uint8_t);
        void    loadTable       (const QString &tf);
        bool    valid           ();
private:
	std::vector<black> blackList;
	std::vector<cacheElement> cache;
	QString tiifileName;

	void	*handler;
	HINSTANCE	Handle;
	HINSTANCE       getLibraryHandle        ();

	float	convert		(const QString &);
        uint16_t get_Eid        (const QString &);
        uint8_t get_mainId      (const QString &);
        uint8_t get_subId       (const QString &);
        int     readColumns     (std::vector<QString> &, char *, int);
        void    readFile        (FILE *);
        char    *eread          (char *, int, FILE *, uint8_t);

	bool		loadFunctions	();
	init_tii_P	init_tii_L;
	close_tii_P	close_tii_L;
	loadTable_P	loadTable_L;
};


