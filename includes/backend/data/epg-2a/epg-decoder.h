#
/*
 *    Copyright (C) 2013 .. 2020
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
 *    along with Qt-TAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef	__EPG_DECODER__
#define	__EPG_DECODER__

#include	<QObject>
#include	<stdio.h>
#include	<stdint.h>
#include	<stdlib.h>
#include	<QString>

class progDesc {
public:
	QString		ident;
	QString		shortName;
	QString		mediumName;
	QString		longName;
	int		startTime;
	progDesc	() {
	   startTime	= -1;
	}
	void	clean		() {
	   startTime	= -1;
	   longName	= "";
	   mediumName	= "";
	   shortName	= "";
	   ident	= "";
	}
	   
};

class	epgDecoder: public QObject {
Q_OBJECT
public:
			epgDecoder		();
			~epgDecoder		();

	int		process_epg		(uint8_t *, int, uint32_t);
private:
	uint32_t	SId;
	QString		stringTable [20];
	int		getBit			(uint8_t *, int);
	uint32_t	getBits			(uint8_t *, int, int);

	int		process_epgElement	(uint8_t *, int);
	int		schedule_element	(uint8_t *, int);
	int		programme_element	(uint8_t *, int, progDesc *);
	int		process_mediaDescription	(uint8_t *, int);
	int		process_location	(uint8_t *, int, progDesc *);
	int		location_element	(uint8_t *, int, progDesc *);
	int		time_element		(uint8_t *, int);
	int		genre_element		(uint8_t *, int);
	int		bearer_element		(uint8_t *, int);
	int		multimedia		(uint8_t *, int);

	void		process_tokens		(uint8_t *, int, int);
	int		process_token		(uint8_t *, int);
	void		process_45		(uint8_t *, int, int);
	void		process_46		(uint8_t *, int, int);
	QString		process_471		(uint8_t *, int, int);
	void		process_472		(uint8_t *, int, int);
	void		process_473		(uint8_t *, int, int);
	int		process_474		(uint8_t *, int, int);
	void		process_475		(uint8_t *, int, int);
	void		process_476		(uint8_t *, int, int);
	QString		process_481		(uint8_t *, int, int);
	void		process_483		(uint8_t *, int, int);
	void		record			(progDesc *);
signals:
	void		set_epgData		(int, int, const QString &);
};

#endif

