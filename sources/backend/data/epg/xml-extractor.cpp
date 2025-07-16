#
/*
 *    Copyright (C)  2015 .. 2025
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

	multimediaElement::multimediaElement	() {
	valid	= false;
}

	multimediaElement::~multimediaElement	() {}

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
QStringList t	= splitter (date, "T");
QStringList temp = splitter (t [0], QString ("-"));
int year	= toIntFrom (temp [0], 10);
int month	= toIntFrom (temp [1], 10);
int day		= toIntFrom (temp [2], 10);
	return QDate (year, month, day);
}

QDateTime xmlExtractor::stringToDateTime (const QString &date) {
QStringList t	= splitter (date, "T");
QString	pureDate = t [0];
QStringList temp = splitter (pureDate, QString ("-"));
int year	= toIntFrom (temp [0], 10);
int month	= toIntFrom (temp [1], 10);
int day		= toIntFrom (temp [2], 10);
QDate datum	= QDate (year, month, day);
QTime time; 
	if (t. size () < 2) { // should not happen
	   time = QTime::currentTime ();
	}
	else { 
	   temp		= splitter (t [1], QString (":"));
	   int hours	= toIntFrom (temp [0], 10);
	   int minutes	= toIntFrom (temp [1], 10);
	   time = QTime (hours, minutes);
	}
	return QDateTime (datum, time);
}
//
//	duration has the form PTXXHXXM of PTXXM
int	xmlExtractor::durationToInt (const QString &dur) {
int	hours	= 0;
int	minutes	= 0;
	if (!dur. startsWith ("PT"))
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
	QDomElement sn	= node. firstChildElement ("shortName");
	res. shortName	= sn. text ();
	QDomElement mn	= node. firstChildElement ("mediumName");
	res. mediumName	= mn. text ();
	QDomElement ln	= node. firstChildElement ("longName");
	res. longName	= ln. text ();
	QDomElement md	= node. firstChildElement ("mediaDescription");
	QDomElement sd	= md. firstChildElement ("shortDescriptor");
	res. shortDescriptor	= sd. text ();
	QDomElement ld	= md. firstChildElement ("longDescriptor");
	res. longDescriptor	= ld. text ();
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
	                 getScheduleDescriptor (const QDomElement &node,
	                                        QDate  &theDate,
	                                        uint32_t ensembleId,
	                                        uint32_t serviceId) {
scheduleDescriptor result;
	result. valid = false;
	result. Version		= getVersion (node);
	QDomElement name = node. firstChildElement ("mediumName");
	if (name. isNull ())
	   name = node. firstChildElement ("longName");
	result. name =
	             !name. isNull () ? name. text () : "";
//
	QString startDate;
	QString stopDate;
	QString duration;
//	The german schedule descriptions usually contain the scope element
	QDomElement scope = node. firstChildElement ("scope");
	if (!scope. isNull ()) {
	   if (scope. hasAttribute ("startTime"))
	      startDate = scope. attribute ("startTime");
	   if (scope. hasAttribute ("stopTime"))
	      stopDate	= scope. attribute ("stopTime");
	   QDomElement serviceScope	=
	              scope. firstChildElement ("serviceScope");
	   if (!serviceScope. isNull ()) {
	      QString identity	= serviceScope. attribute ("id");
	      QStringList ident	= splitter (identity, QString (":"));
	      ensembleId	= toIntFrom (ident [1], 16);
	      serviceId		= toIntFrom (ident [2], 16);
	   }
	}
	QDomElement location	= node. firstChildElement ("location");
	if (!location. isNull ()) {
	   QDomElement time = location. firstChildElement ("time");
	   if (!time. isNull ()) {
	      if (time. hasAttribute ("time")) 
	         startDate = location. attribute ("time");
	      if (time. hasAttribute ("duration")) 
	         duration  = location. attribute ("duration");
	   }
	}
	QDateTime startTime;
	QDateTime stopTime;
	if (startDate == "") {
	   QTime beginTime	= QTime (0, 0, 0);
	   QTime endTime	= QTime (23, 59, 59);
	   startTime = QDateTime (theDate, beginTime);
	   stopTime  = QDateTime (theDate, endTime);
	}
	else {
	   startTime	= stringToDateTime (startDate);
	   if (duration != "") {
	      int minutes = durationToInt (duration);
	      stopTime = startTime. addSecs (minutes * 60);
	   }
	   else
	      stopTime = stringToDateTime (stopDate);
	}
	result. startTime	= startTime;
	result. stopTime	= stopTime;
	result. Eid		= ensembleId;
	result. Sid		= serviceId;
	result. valid 	= true;
	return result;
}

multimediaElement xmlExtractor::extract_multimedia (const QDomElement &multimedia) {
multimediaElement res;
	if (multimedia. hasAttribute ("url"))
	   res. url = multimedia. attribute ("url");
	else
	   return res;
	if (multimedia. hasAttribute ("width")) {
	   res. width = toIntFrom (multimedia. attribute ("width"), 10);
	}
	else
	   res. width = 0;
	res. valid = true;
	return res;
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

