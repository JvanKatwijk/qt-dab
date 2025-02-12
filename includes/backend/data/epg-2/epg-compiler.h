#
/*
 *    Copyright (C) 2013 .. 2024
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
#include	<QDomElement>

class	epgProgramme {
public:
	uint32_t	shortId;
	QString		shortName;
	QString		mediumName;
	QString		longName;
	struct {
	   uint32_t startTime;
	   uint32_t stopTime;
	} programTime;
};

class	programSchedule {
public:
	uint32_t	time;
	int version;
	struct {
	   uint32_t	Eid;
	   uint32_t	SId;
	} identity;
	QString mediumName;
	QString longName;
	QString mediaDescription;
	QList<epgProgramme> programmes;
};

class	epgCompiler: public QObject {
Q_OBJECT
public:
		epgCompiler	();
		~epgCompiler	();

int	process_epg	(QDomDocument &,
	                 const std::vector<uint8_t> &v);
private:
	QString	stringTable [20];
//
//	element handlers
	QDomElement	process_defaultLanguage	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_shortName	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_mediumName	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_longName	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_mediaDescription	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_genre		(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_keyWords	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_memberOf	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_link		(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_location	(QDomDocument &, const std::vector<uint8_t> &v, int &index);

	QDomElement	process_shortDescription (QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_longDescription	(QDomDocument &, const std::vector<uint8_t> &v, int &index);

	QDomElement	process_programme	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement 	process_programmeGroups	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_schedule	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_programmeGroup	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement 	process_scope		(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement 	process_serviceScope	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement 	process_ensemble	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement 	process_service		(QDomDocument &, const std::vector<uint8_t> &v, int &index);

	QDomElement	process_bearer		(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_multimedia	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_time		(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_programmeEvent	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_relativeTime	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_radiodns	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_geolocation	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_country		(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_point		(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_polygon		(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_onDemand	(QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_presentationTime (QDomDocument &, const std::vector<uint8_t> &v, int &index);
	QDomElement	process_acquisitionTime	(QDomDocument &, const std::vector<uint8_t> &v, int &index);

//
//	attribute handlers
	QString	process_440		(const std::vector<uint8_t> &v, int &index);
	QString	process_471		(const std::vector<uint8_t> &v, int &index);
	QString	process_472		(const std::vector<uint8_t> &v, int &index);
	QString	process_473		(const std::vector<uint8_t> &v, int &index);
	QString	process_474		(const std::vector<uint8_t> &v, int &index);
	QString	process_475		(const std::vector<uint8_t> &v, int &index);
	QString	process_476		(const std::vector<uint8_t> &v, int &index);
	QString	process_481		(const std::vector<uint8_t> &v, int &index);
	QString	process_482		(const std::vector<uint8_t> &v, int &index);
	QString	process_483		(const std::vector<uint8_t> &v, int &index);
	QString	process_484		(const std::vector<uint8_t> &v, int &index);
	QString	process_485		(const std::vector<uint8_t> &v, int &index);

	void	process_tokenTable	(const std::vector<uint8_t> &v, int &index);
	void	process_token		(const std::vector<uint8_t> &v, int &index);

	void	process_obsolete	(const std::vector<uint8_t> &v, int &index);

	QString	getCData		(const std::vector<uint8_t> &v, int &index);

	void    process_forgotten	(const QString s, 
	                                const std::vector<uint8_t> &v,
	                                int &index);

	QString process_broadcast	(const std::vector<uint8_t> &v,
	                                 int &index);
	QString	process_recommendation	(const std::vector<uint8_t> &v,
	                                 int &index);
	QString process_groupType	(const std::vector<uint8_t> &v,
	                                 int &index);
};

