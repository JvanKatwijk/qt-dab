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
#ifndef	__TII_HANDLER_H
#define	__TII_HANDLER_H
#include	<stdint.h>
#include	<QString>
#include	<stdio.h>
#include	<vector>

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
} cacheElement;

typedef struct {
	uint16_t Eid;
	uint8_t mainId;
	uint8_t subId;
} black;

class	tiiHandler {
public:
		tiiHandler	();
		~tiiHandler	();
	bool	tiiFile 	(const QString &);
	QString	get_transmitterName (const QString &,
	                             const QString &,
	                             uint16_t Eid,
	                             uint8_t mainId, uint8_t subId);
	void	get_coordinates	(float *, float *,
	                         const QString &, const QString &);
	int	distance	(float, float, float, float);
	int	corner		(float, float, float, float);
	bool	is_black	(uint16_t, uint8_t, uint8_t);
	void	set_black	(uint16_t, uint8_t, uint8_t);	
	void	loadTable	(const QString &tf);
	bool	valid		();

private:
std::vector<black> blackList;
std::vector<cacheElement> cache;
QString	tiifileName;
	float	convert		(const QString &);
	uint16_t get_Eid	(const QString &);
	uint8_t get_mainId	(const QString &);
	uint8_t	get_subId	(const QString &);
	int	readColumns	(std::vector<QString> &, char *, int);
	void	readFile	(FILE *);
	char	*eread		(char *, int, FILE *);
	uint8_t	shift;
	QString	entry		(const char *);
	FILE	*curl_f;
};


#endif
