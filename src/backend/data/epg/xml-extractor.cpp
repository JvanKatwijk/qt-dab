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

#include	"xml-extractor.h"
#include	<QStringList>

static inline
QStringList splitter (const QString &s, QString splitString) {
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 2)
        QStringList list = s.split (splitString, Qt::SkipEmptyParts);
#else 
        QStringList list = s.split (splitString, QString::SkipEmptyParts);
#endif
        return list;
}

static inline
int	numValue (char c) {
	if (('0' <= c) && (c <= '9'))
	   return c - (int)('0');
	if (('a' <= c) && (c <= 'f'))
	   return (int)c - 'a' + 10;
	return (int)c - 'A' + 10;
}

static inline
uint32_t toIntFrom (const QString &s, int base) {
uint32_t res = 0;
	for (int i = 0; i < s. size (); i ++)
	   res =  res * base + numValue (s [i]. toLatin1 ());
	return res;
}


	scheduleDescriptor::scheduleDescriptor	() {}
	scheduleDescriptor::~scheduleDescriptor	() {}
	xmlExtractor::xmlExtractor	()	{}
	xmlExtractor::~xmlExtractor	()	{}

//
//	The xml format for date is xxxx-xx-xxTyy:yy
QDate	xmlExtractor::stringToDate (const QString &date) {
int index = date. lastIndexOf ('T');
QString pureDate = date. left (index);
QStringList temp = splitter (pureDate, QString ("-"));
int year	= toIntFrom (temp [0], 10);
int month	= toIntFrom (temp [1], 10);
int day		= toIntFrom (temp [2], 10);
	return QDate (year, month, day);
}

QDateTime xmlExtractor::stringToDateTime (const QString &date) {
int index	= date. lastIndexOf ('T');
#if QT_VERSION >= QT_VERSION_CHECK (6, 1, 1)
QString pureDate = date. first (index);
QString pureTime = date. last (date. size () - index - 1);
#else
QString pureDate = date. left (index);
QString pureTime	= date;
pureTime. remove (0, index);
#endif
QStringList temp = splitter (pureDate, QString ("-"));
int year	= toIntFrom (temp [0], 10);
int month	= toIntFrom (temp [1], 10);
int day		= toIntFrom (temp [2], 10);
temp		= splitter (pureTime, QString (":"));
int hours	= toIntFrom (temp [0], 10);
int minutes	= toIntFrom (temp [1], 10);
QDate datum (year, month, day);
QTime time (hours, minutes);
	return QDateTime (datum, time);
}
//
//	duration has the form PTXXHXXM of PTXXM
int	xmlExtractor::durationToInt (const QString &dur) {
int	hours	= 0;
int	minutes	= 0;
	if ((dur [0]. toLatin1 () != 'P') ||
	    (dur [1]. toLatin1 () != 'T'))
	   return -1;
#if QT_VERSION >= QT_VERSION_CHECK (6, 1, 1)
	QString ss	= dur. last (dur. size () - 2);
#else
	QString ss	= dur;
	ss. remove (0, 2);
#endif
	if (ss. contains ('H')) { // hours and may be minutes
	   QStringList hh = splitter (ss, QString ("H"));
	   hours = toIntFrom (hh [0], 10);
	   if (hh [1]. size () > 1)
	      minutes = toIntFrom (hh [1]. left (hh [1]. size () - 1), 10);
	 }
	 else
	   minutes	= toIntFrom (ss. left (ss. size () - 1), 10);
	return 60 * hours + minutes;
}

programDescriptor	xmlExtractor::
	                    process_programme (const QDomElement &node) {
programDescriptor res;
	res. valid = false;
	QDomElement location = 
	              node. firstChildElement ("location");
	if (location. isNull ())
	   return res;
	QDomElement time =
	              location. firstChildElement ("time");
	if (time. isNull ())
	   return res;
	QString tt = time. attribute ("time");
	res. startTime = stringToDateTime (tt);
	QString dd = time. attribute ("duration");
	res. duration = durationToInt (dd);
	QDomElement md	= node. firstChildElement ("mediaDescription");
	QString ps;
	if (!md. isNull ()) {
	   QDomElement sd = md. firstChildElement ("shortDescription");
	   if (!sd. isNull ()) 
	      ps = sd. attribute ("xml:lang");
	}
	if (ps == "") {
	   QDomElement name = node. firstChildElement ("longName");
	   if (name. isNull ())
	      name = node. firstChildElement ("mediumName");
	   ps = name. attribute ("xml:lang");
	}
	res. program = ps;
	if (res. program != "")
	   res. valid = true;
	return res;
}

int	xmlExtractor::getVersion (const QDomElement &node) {
	if (!node. hasAttribute ("version"))
	   return 1;
	QString version = node. attribute ("version");
	return toIntFrom (version, 10);
}

scheduleDescriptor xmlExtractor::
	                 getScheduleDescriptor (const QDomElement &node) {
scheduleDescriptor result;
QDomElement scope = node. firstChildElement ("scope");
	result. valid = false;
	if (scope. isNull ())
	   return result;
	result. Version		= getVersion (node);
	QDomElement name = node. firstChildElement ("mediumName");
	if (name. isNull ())
	   name = node. firstChildElement ("longName");
	result. name =
	             !name. isNull () ? name. attribute ("xml:lang") : "";
	QString startDate	= scope. attribute ("startTime");
	QString	stopDate	= scope. attribute ("stopTime");
	result. startTime	= stringToDateTime (startDate);
	result. stopTime	= stringToDateTime (stopDate);
	QDomElement serviceScope	=
	              scope. firstChildElement ("serviceScope");
	if (serviceScope. isNull ()) 
	   return result;
	QString identity	= serviceScope. attribute ("id");
	QStringList ident	= splitter (identity, QString (":"));
	if (ident. size () != 3)
	   return result;
	result. Eid	= toIntFrom (ident [1], 16);
	result. Sid	= toIntFrom (ident [2], 16);
	result. valid 	= true;
	return result;
}

QString	xmlExtractor::service_url (const QDomElement &service) {
	QDomElement mediaDescription =
	                    service. firstChildElement ("mediaDescription");
	if (mediaDescription. isNull ())
	   return "";
	QDomElement multimedia =
	                     mediaDescription. firstChildElement ("multimedia");
	if (multimedia. isNull ())
	   return "";
	if (multimedia. hasAttribute ("url"))
	   return multimedia. attribute ("url");
	return "";
}

uint32_t  xmlExtractor::serviceSid (const QDomElement &service) {
	QDomElement bearer = service. firstChildElement ("bearer");
	if (bearer. isNull ())
	   return 0;
	if (!bearer. hasAttribute ("id"))
	   return 0;
	QString id = bearer. attribute ("id");
	QStringList t = splitter (id, ":");
	if (t. size () < 3)
	   return 0;
	return toIntFrom (t [2], 16);
}

