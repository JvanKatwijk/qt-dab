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


#pragma once

#include	<QObject>
#include	<stdio.h>
#include	<stdint.h>
#include	<stdlib.h>
#include	<QString>

class progDesc {
public:
	QString		ident;
	QString		bearer;
	QString		shortName;
	QString		mediumName;
	QString		longName;
	QString		shortDescription;
	QString		longDescription;
	int		startTime;
	int		stopTime;
	progDesc	() {
	   startTime	= -1;
	   stopTime	= -1;
	}
	void	clean		() {
	   startTime	= -1;
	   longName	= "";
	   mediumName	= "";
	   shortName	= "";
	   ident	= "";
	   shortDescription	= "";
	   longDescription	= "";
	}
	   
};

class	epgDecoder: public QObject {
Q_OBJECT
public:
		epgDecoder	();
		~epgDecoder	();

int	process_epg	(uint8_t *v, int e_length,
	                 uint32_t SId, int subType, uint32_t theDay);
private:
	uint32_t        SId;
        QString         stringTable [20];
	int		subType;
	uint32_t	julianDate;
	int	getBit			(uint8_t *v, int bitnr);
	uint32_t getBits		(uint8_t *v, int bitnr, int length);
	int	process_programGroups	(uint8_t *v, int index);
	int	process_programGroup	(uint8_t *v, int index);
	int	process_schedule	(uint8_t *v, int index);
	int	process_program		(uint8_t *v, int index, progDesc *);
	int	process_scope		(uint8_t *v, int index, progDesc *);
	int	process_serviceScope	(uint8_t *v, int index, progDesc *);
	int	process_mediaDescription (uint8_t *v, int index, progDesc *);
	int	process_ensemble	(uint8_t *v, int index);
	int	process_service		(uint8_t *v, int index, progDesc *);
	int	process_location	(uint8_t *v, int index, progDesc *);
	int	process_bearer		(uint8_t *v, int index, progDesc *);
	int	process_geoLocation	(uint8_t *v, int index);
	int	process_programmeEvent	(uint8_t *v, int index);
	int	process_onDemand	(uint8_t *v, int index);
	int	process_genre		(uint8_t *v, int index, progDesc *);
	int	process_keyWords	(uint8_t *v, int index);
	int	process_link		(uint8_t *v, int index);
	int	process_shortName	(uint8_t *v, int index, progDesc *);
	int	process_mediumName	(uint8_t *v, int index, progDesc *);
	int	process_longName	(uint8_t *v, int index, progDesc *);
	int	process_shortDescription (uint8_t *v, int index, progDesc *);
	int	process_longDescription (uint8_t *v, int index, progDesc *);
	int	process_multiMedia	(uint8_t *v, int index);
	int	process_radiodns	(uint8_t *v, int index);
	int	process_time		(uint8_t *v, int index, int *);
	int	process_relativeTime	(uint8_t *v, int index);
	int	process_memberOf	(uint8_t *v, int index);
	int	process_presentationTime (uint8_t *v, int index);
	int	process_acquisitionTime (uint8_t *v, int index);

	int	process_country		(uint8_t *v, int index);
	int	process_point		(uint8_t *v, int index);
	int	process_polygon		(uint8_t *v, int index);
//
	int	process_412		(uint8_t *v, int index);
	int	process_440		(uint8_t *v, int index);
	int	process_46		(uint8_t *v, int index);
	int	process_471		(uint8_t *v, int index);
	int	process_472		(uint8_t *v, int index);
	int	process_473		(uint8_t *v, int index);
	int	process_474		(uint8_t *v, int index, int *);
	int	process_475		(uint8_t *v, int index);
	int	process_476_p		(uint8_t *v, int index, QString *);
	int	process_476		(uint8_t *v, int index);
	int	process_481		(uint8_t *v, int index);
	int	process_482		(uint8_t *v, int index);
	int	process_483		(uint8_t *v, int index);
	int	process_484		(uint8_t *v, int index);
	int	process_485		(uint8_t *v, int index);
	int	process_4171		(uint8_t *v, int index);

	int	process_tokenTable	(uint8_t *v, int index);
	int	process_token		(uint8_t *v, int index);
	int	process_defaultLanguage	(uint8_t *v, int index);
	int	process_obsolete	(uint8_t *v, int index);

	void	record			(progDesc *);
	QString	getCData		(uint8_t *v, int index, int elength);
signals:
	void	set_epgData		(int, int,
	                                 const QString &,
	                                 const QString &);
};

