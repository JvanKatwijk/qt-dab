#
/*
 *    Copyright (C)  2015 .. 2024
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

#include	<QDomElement>
#include	<QDateTime>
#include	<QString>
#include	<stdint.h>


typedef struct {
	bool	valid;
	QDateTime	startTime;
	int	duration;
	QString shortName;
	QString	mediumName;
	QString	longName;
	QString	shortDescriptor;
	QString	longDescriptor;
} programDescriptor;


class	scheduleDescriptor {
public:
		scheduleDescriptor	();
		~scheduleDescriptor	();
bool		valid;
int		Version;
QDateTime	startTime;
QDateTime	stopTime;
uint32_t	Eid;
uint32_t	Sid;
uint32_t	shortId;
QString		name;
std::vector<programDescriptor> thePrograms;
};

class	multimediaElement {
public:
		multimediaElement	();
		~multimediaElement	();
bool		valid;
QString		url;
int		width;
};

	
class	xmlExtractor {
public:
		xmlExtractor	();
		~xmlExtractor	();
	scheduleDescriptor	getScheduleDescriptor	(const QDomElement &,
	                                                 QDate &,
	                                                 uint32_t, uint32_t);
	int			getVersion		(const QDomElement &);
	programDescriptor	process_programme	(const QDomElement &);

	uint32_t		serviceSid		(const QDomElement &);
	QString			service_url		(const QDomElement &);
	multimediaElement	extract_multimedia	(const QDomElement &);

	QString			find_xmlFile		(const QDate &,
	                                                 uint32_t, uint32_t);
	QString			extractName		(const QString &);
private:
	QDate		stringToDate		(const QString &);
	QDateTime	stringToDateTime	(const QString &);
	int		durationToInt		(const QString &);
};


