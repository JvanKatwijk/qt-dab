#
/*
 *    Copyright (C) 2017 .. 2024
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
#include	"epg-compiler.h"
#include	<QDomDocument>
//#include	<QFile>
#include	<QTextStream>

#include	"time-converter.h"

//	miniparser for epg
//	input:	a vector delivered by the MOT handler
//	output:	A QDOMDocument

static
uint8_t	bitTable [] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

static inline
int	getBit (const std::vector<uint8_t> &v, int bitnr) {
int bytenr	= bitnr / 8;

	bitnr 	= bitnr % 8;
	return (v [bytenr] & bitTable [bitnr]) != 0 ? 1 : 0;
}

static inline
uint32_t getBits (const std::vector<uint8_t> &v, int bitnr, int length) {
uint16_t res	= 0;
	for (int i = 0; i < length; i ++) {
	   res <<= 1;
	   res |= getBit (v, bitnr + i);
	}
	return res;
}

static inline
int	setLength (const std::vector<uint8_t> &v, int &index) {
int length	= v [index + 1];

	if (length == 0xFE) {
	   length = (v [index + 2] << 8) | v [index + 3];
	   index	+= 4;
	}
	else
	if (length == 0xFF) {
	   length = (v [index + 2] << 16) | (v [index + 3] << 8) | v [index + 4];
	   index	+= 5;
	}
	else
	   index	+= 2;
	return	index + length;
}

#define	EPG_TAG			0X02
#define	SERVICE_TAG		0X03
//
	epgCompiler::epgCompiler	() {
}

	epgCompiler::~epgCompiler	() {
}

int	epgCompiler::process_epg	(QDomDocument &doc,
	                                 const std::vector<uint8_t> &v) {
uint8_t	tag	= v [0];
int	index	= 0;

	for (int i = 0; i < 20; i ++)
	   stringTable [i] = "";
	int endPoint = setLength (v, index);

	if (tag == EPG_TAG) {
	   QDomElement epg;
	   epg = doc. createElement ("epg");
	   doc. appendChild (epg);
	   epg. setAttribute ("system", "DAB");
	   while (index < endPoint) {
	      switch (v [index]) {
	         case 0x04:		// process tokenTable
	            (void)process_tokenTable (v, index);
	            break;

	         case 0x06: {		// default language
	            QDomElement child =  process_defaultLanguage (doc, v, index);
	            epg. appendChild (child);
	            break;
	         }
	         case 0x20: {		// process ProgramGroups
	            QDomElement t = process_programmeGroups (doc, v, index);
	            epg. appendChild (t);
	            break;
	         }
	         case 0x21: {		// process_schedule
	            QDomElement child = process_schedule (doc, v,  index);
	            epg. appendChild (child);
	            break;
	         }
	         case 0x05:		// obsolete
	            (void)process_obsolete (v, index);
	            break;

	         default:
	            process_forgotten ("epg", v, index);
	            break;
	      }
	   }
	}
	else
	if (tag == SERVICE_TAG)	{	// superfluous test
	   QDomElement serviceInformation;
	   serviceInformation = doc. createElement ("serviceInformation");
	   doc. appendChild (serviceInformation);
	   while (index < endPoint) {
	      switch (v [index]) {
	         case 0x06: {	// default language
	            QDomElement child =  process_defaultLanguage (doc, v, index);
	            serviceInformation. appendChild (child);
	            break;
	         }
	         case 0x26: {	// ensemble
	            QDomElement t = process_ensemble (doc, v, index);
	            serviceInformation. appendChild (t);
	            break;
	         }
	         case 0x28: {	// process_service
	            QDomElement t = process_service (doc, v, index);
	            serviceInformation. appendChild (t);
	            break;
	         }
	         case 0x80: {	// version
	            QString s = process_483 (v, index);
	            if (s != "")
	               serviceInformation. setAttribute ("Version", s);
	            break;
	         }
	         case 0x81: {	// creation time
	   	    QString s =  process_474 (v, index);
	            serviceInformation. setAttribute ("creationTime", s);
	            break;
	         }
	         case 0x82: {	// originator
	            QString s = process_440 (v, index);
	            serviceInformation. setAttribute ("originator", s);
	            break;
	         }
	         case 0x83: {	// serviceProvider
	            QString s =  process_440 (v, index);
	            serviceInformation. setAttribute ("serviceprovider", s);
	            break;
	         }
	         default:
	            process_forgotten ("serviceInformation:", v, index);
	            break;
	      }
	   }
	}
	return endPoint;
}

QDomElement epgCompiler::process_defaultLanguage (QDomDocument &doc,
	                                          const std::vector<uint8_t> &v,
	                                             int  &index) {
int endPoint	= setLength (v, index);
QDomElement child;
	child = doc. createElement ("defaultLanguage");
	QString res;
	for (int i = index; i < endPoint; i ++)
	   res = res + QChar (v [i]);
	child. setAttribute ("defaultLanguage", res);
	index	= endPoint;
	return child;
}

QDomElement  epgCompiler::process_shortName (QDomDocument &doc,
	                                     const std::vector<uint8_t> &v,
	                                     int &index) {
QDomElement child;
int endPoint = setLength (v, index);
QString s;
	child = doc. createElement ("shortName");
	if (v [index] == 0x80)
	   ignore (v, index);
	if (v [index] == 1) {
	   QByteArray text;
	   if (v [index + 1] < 20)
	      s = stringTable [v [index + 1]];
	   else {
	      for (;index < endPoint; index ++)
	         text. push_back (v [index]);
	      s = QString::fromUtf8 (text);
	   }
	}
	QDomText t = doc. createTextNode (s);
	child. appendChild (t);
	index = endPoint;
	return child;
}

QDomElement epgCompiler::process_mediumName (QDomDocument &doc,
	                                    const std::vector<uint8_t> &v,
	                                    int &index) {
QDomElement child;
int endPoint = setLength (v, index);
QString res;
	child	= doc. createElement ("mediumName");
	if (v [index] == 0x80)
	   ignore (v, index);
	if (v [index] == 1) {
	   QByteArray text;
	   if (v [index + 1] < 20)
	      res = stringTable [v [index + 1]];
	   else {
	      for (;index < endPoint; index ++)
	         text. push_back (v [index]);
	      res = QString::fromUtf8 (text);
	   }
	}
	QDomText t = doc. createTextNode (res);
	child. appendChild (t);
	index = endPoint;
	return child;
}

QDomElement epgCompiler::process_longName (QDomDocument &doc,
	                                  const std::vector<uint8_t> &v,
	                                  int &index) {
QDomElement child;
QString res;
int endPoint = setLength (v, index);
	child	= doc. createElement ("longName");
	if (v [index] == 0x80)
	   ignore (v, index);
	if (v [index] == 1) {
	   QByteArray text;
	   if (v [index + 1] < 20)
	      res = stringTable [v [index + 1]];
	   else {
	      for (;index < endPoint; index ++)
	         text. push_back (v [index]);
	      res = QString::fromUtf8 (text);
	   }
	}
	QDomText t = doc. createTextNode (res);
	child. appendChild (t);
	index = endPoint;
	return child;
}

QDomElement epgCompiler::process_mediaDescription (QDomDocument &doc,
	                                         const std::vector<uint8_t> &v,
	                                         int &index) {
int endPoint = setLength (v, index);
QDomElement t;
QString res;
	t = doc. createElement ("mediaDescription");
	switch (v [index]) {
	   case 0x1A: {
	      QDomElement tt = process_shortDescription (doc, v, index);
	      t. appendChild (tt);
	      break;
	   }
	   case 0x1B: {
	      QDomElement tt = process_longDescription (doc, v, index);
	      t. appendChild (tt);
	      break;
	   }
	   case 0x2B: {
	      QDomElement tt = process_multimedia (doc, v, index);
	      t. appendChild (tt);
	      break;
	   }
	   default:
	      process_forgotten ("mediaDescription", v, index);
	      break;
        }
	index = endPoint;
	return t;
}

QDomElement	epgCompiler::process_genre (QDomDocument &doc,
	                                   const std::vector<uint8_t> &v,
	                                   int &index) {
int endPoint	= setLength (v, index);
static
const char *genres [] = {
"IntentionCS", "FormatCS", "CntentCS", "OriginationCS", 
"IntendedAudienceCS", "ContentAlertCS", "KediaTupeCS", "AtmosphereCS"};
QDomElement t;
	t = doc. createElement ("genre");
QString s;
	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x80: {
	         int oldIndex = index;
	         int localEnd = setLength (v, index);
	         s = QString::number (v [index]);
	         for (int i = index + 1; i < localEnd; i ++)
	            s += "." + QString::number (v [i]);
	         s = QString ("urn:tva:metadata:cs:") +
	             QString (genres [v [index]]) +"2008:" + s;
	         t. setAttribute ("href", s);
	         index = localEnd;
	         break;
	      }
	      case 0x81: {	// type 
	         int localEnd = setLength (v, index);
	         uint8_t xx = v [index];
	         s = xx == 0x01 ? "main" :
	             xx == 0x02 ? "secondary" : "other";
	         t. setAttribute ("type", s);
	         index =  localEnd;
	         break;
	      }
	      case 0x01: {
	         QString s;
	         if (v [index + 1] == 1)
	            s = stringTable [v [index + 2]];
	         else {
	            QByteArray text;
	            for (int i = 0; i < v [index + 1]; i++)
	               text. push_back (v [index + 2 + i]);
	            s = QString::fromUtf8 (text);
	         }
	         index = endPoint;
	         QDomText tt = doc. createTextNode (s);
	         t. appendChild (tt);
	         break;
	      }
	      default:
	         process_forgotten ("genre", v, index);
	         break;
	   }
	}
	index = endPoint;
	return t;
}

QDomElement epgCompiler::process_keyWords (QDomDocument &doc,
	                                  const std::vector<uint8_t> &v,
	                                  int &index) {
int endPoint = setLength (v, index);
QDomElement child;
QString res;
	child = doc. createElement ("keyWords");
	if (v [index] == 0x80)
	   ignore (v, index);
	if (v [index] == 1) {
	   QByteArray text;
	   if (v [index + 1] < 20)
	      res = stringTable [v [index + 1]];
	   else {
	      for (;index < endPoint; index ++)
	         text. push_back (v [index]);
	      res = QString::fromUtf8 (text);
	   }
	}
	QDomText t = doc. createTextNode (res);
	child. appendChild (t);
	index = endPoint;
	return child;
}

QDomElement epgCompiler::process_memberOf (QDomDocument &doc,
	                                  const std::vector<uint8_t> &v,
	                                  int &index) {
int endPoint = setLength (v, index);
QDomElement t;
        t = doc. createElement ("memberOf");
	while (index < endPoint) {
	   switch (v [index]) {
	      case  0x80: {	// 4.7.1, id
	         int localEnd = setLength (v, index);
	         QString s;
	         for (int i = index; i < localEnd; i ++)
	            s += QChar (v [i]);
	         t. setAttribute ("id", s);
	         index = localEnd;
	         break;
	      }
	      case 0x81: { 	// 4.7.2  shortId
	         QString s = process_472 (v, index);
	         t. setAttribute ("shortId", s);
	         break;
	      }
	      case  0x82: {	// 5.8.2 index
	         QString s = process_482 (v, index);
	         t. setAttribute ("index", s);
	         break;
	      }
	   }
	}
	index = endPoint;
	return t;
}

QDomElement epgCompiler::process_link (QDomDocument &doc,
	                              const std::vector<uint8_t> &v,
	                              int &index) {
int endPoint = setLength (v, index);
QDomElement t;
	t = doc. createElement ("link");
	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x80: {	//uri
	         int localEnd = setLength (v, index);
	         QString res;
	         for (int i = index; i < localEnd; i ++)
	            res += QChar (v [i]);
	         t. setAttribute ("uri", res);
	         index = localEnd;
	         break;
	      }
	      case 0x81: {	// mime value 473
	         QString s = process_473 (v, index);
	         t. setAttribute ("mime value", s);
	         break;
	      }
	      case 0x82: {	// xml:lang	481
	         ignore (v, index);
	         break;
	      }
	      case 0x83: {	// description 440
	         int localEnd = setLength (v, index);
	         QString res;
	         for (int i = index; i < localEnd; i ++)
	            res += QChar (v [i]);
	         t. setAttribute ("description", res);
	         index = localEnd;
	         break;
	      }
	      case 0x84: {	//expiry time
	         QString s = process_474 (v, index);
	         t. setAttribute ("expiryTime", s);
	         break;
	      }
	      default:
	         process_forgotten ("process_link", v, index);
	         break;
	   }
	}
	index = endPoint;
	return t;
}

QDomElement	epgCompiler::process_location (QDomDocument &doc,
 	                                      const std::vector<uint8_t> &v,
	                                      int &index) {
int endPoint	= setLength (v, index);
QDomElement location;
	location = doc. createElement ("location");
	switch (v [index]) {
	   case 0x2D: {		// bearer
	      QDomElement t = process_bearer (doc, v, index);
	      location. appendChild (t);
	      break;
	   }
	   case 0x2c: {		// time
	      QDomElement t  = process_time (doc, v, index);
	      location. appendChild (t);
	      break;
	   }
	   case 0x2F:  {	// relative time
	      QDomElement t = process_relativeTime (doc, v, index);
	      location. appendChild (t);
	      break;
	   }
	   default:
	      process_forgotten ("location", v, index);
	      location. setAttribute ("unknown time key", "XXX");
	      break;
	}
	index = endPoint;
	return location;
}

QDomElement epgCompiler::process_shortDescription (QDomDocument &doc,
	                                          const std::vector<uint8_t> &v,
	                                           int &index) {
int endPoint = setLength (v, index);
QDomElement t;
QString res;
	t = doc. createElement ("shortDescription");
	switch (v [index]) {
	   case 0x80: {		// xml:lang, ignore
	      ignore (v, index);
              break;
	   }
	   case 0x01: {
	      QString s;
	      if (v [index + 1] < 20)
	         s = stringTable [v [index + 1]];
	      else {
	         QByteArray text;
	         for (int i = index + 1; i < endPoint; i ++)
	            text. push_back (v [i]);
	         s = QString::fromUtf8 (text);
	      }
	      QDomText n = doc. createTextNode (s);
	      t. appendChild (n);
	      break;
	   }
	   default:
	      process_forgotten ("shortDescription", v, index);
	      break;
        }
	index = endPoint;
	return t;
}

QDomElement epgCompiler::process_longDescription (QDomDocument &doc,
	                                         const std::vector<uint8_t> &v,
	                                         int &index) {
int endPoint = setLength (v, index);
QDomElement t;
QString res;
	t = doc. createElement ("longDescription");
	switch (v [index]) {
	   case 0x80: {		// xml:lang
	      ignore (v, index);
	   }
	   case 0x01: {
	      QString s;
	      if (v [index + 1] < 20)
	         s = stringTable [v [index + 1]];
	      else {
	         QByteArray text;
	         for (int i = index + 1; i < endPoint; i ++)
	            text. push_back (v [i]);
	         s = QString::fromUtf8 (text);
	      }
	      QDomText dt = doc. createTextNode (s);
	      t. appendChild (dt);
	      break;
	   }
	   default:
	      process_forgotten ("shortName", v, index);
	      break;
        }
	index = endPoint;
	return t;
}

QDomElement epgCompiler::process_programme (QDomDocument &doc,
	                                   const std::vector<uint8_t> &v,
	                                   int  &index) {
int	endPoint = setLength (v, index);
QDomElement program;
bool	recommended	= false;
bool	broadcasting	= false;
	program = doc. createElement ("programme");
	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x10: { 	// shortName
	         QDomElement child = process_shortName (doc, v, index);
	         program. appendChild (child);
	         break;
	      }
	      case 0x11: {	// mediumName
	         QDomElement child = process_mediumName (doc, v, index);
	         program. appendChild (child);
	         break;
	      }
	      case 0x12: {	// longName
	         QDomElement child = process_longName (doc, v, index);
	         program. appendChild (child);
	         break;
	      }
	      case 0x13: {	// media description 
	         QDomElement child = process_mediaDescription (doc, v, index);
	         program. appendChild (child);
	         break;
	      }
	      case 0x14: {	// genre
	         QDomElement child = process_genre (doc, v, index);
	         program. appendChild (child);
	         break;
	      }
	      case 0x16: {	// keyWords
	         QDomElement child = process_keyWords (doc, v, index);
	         program. appendChild (child);
	         break;
	      }
	      case 0x17: {	// memberOf
	         QDomElement t =  process_memberOf (doc, v, index);
	         program. appendChild (t);
	         break;
	      }
	      case 0x18: {	// link
	         QDomElement link = process_link (doc, v, index);
	         program. appendChild (link);
	         break;
	      }
	      case 0x19: {	// location
	         QDomElement child = process_location (doc, v, index);
	         program. appendChild (child);
	         break;
	      }
	      case 0x29: {	// bearer
	         QDomElement child = process_bearer (doc, v, index);
	         program.  appendChild (child);
	         break;
	      }
	      case 0x31: {	// radiodns
	         QDomElement child = process_radiodns (doc, v, index); 
	         program. appendChild (child);
	         break;
	      }
	      case 0x32: {	// geolocation
	         QDomElement child = process_geolocation (doc, v, index);
	         program. appendChild (child);
	         break;
	      }
	      case 0x80: {	// 4.7.1 Id
	          QString s = process_471 (v, index);
	          program. setAttribute ("id", s);
	          break;
	      }
	      case 0x81: {	// 472, shortId
	         QString s = process_472 (v, index);
	         program. setAttribute ("shortId", s);
	         break;
	      }
	      case 0x82: {	// 4.8.3 version
	         QString s = process_483 (v, index);
	         program.  setAttribute ("version", s);
	         break;
	      }
	      case 0x83: {	// 4.6, recommendation
	        recommended	= true;
	        QString s = process_recommendation (v, index);
	        program. setAttribute ("recommendation", s);
	        break;
	      }
	      case 0x84: {	// 4.6, broadcast
	         broadcasting	= true;
	         QString s = process_broadcast (v, index);
	         program. setAttribute ("broadcast", s);
	         break;
	      }
	      case 0x86: {	// 481, xml:lang
	         ignore (v, index);
	         break;
	      }
	      default:
	         process_forgotten ("program::xml:lan", v, index);
	         break;
	   }
	}
	index	= endPoint;
	if (!broadcasting)
	   program. setAttribute ("broadcast", "on-air");
	if (!recommended)
	   program. setAttribute ("recommendation", "no");
	return program;
}

QDomElement epgCompiler::process_programmeGroups (QDomDocument &doc,
	                                         const std::vector<uint8_t> &v,
	                                          int &index) {
int endPoint = setLength (v, index);
QDomElement t;
QString	s;
	t = doc. createElement ("programGroups");
	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x23: {	// programmegroup
	         QDomElement c = process_programmeGroup (doc, v, index);
	         t. appendChild (c);
	         break;
	      }
	      case 0x80: {	// version
	         QString s = process_483 (v, index);
	         t.  setAttribute ("version", s);
	         break;
	      }
	      case 0x81: {	// creation time
	   	 QString s =  process_474 (v, index);
	         t. setAttribute ("creationTime", s);
	         break;
	      }
	
	      case 0x82: {	// originator
	         QString s = process_440 (v, index);
	         t. setAttribute ("originator", s);
	         break;
	      }

	      default:
	         process_forgotten ("programGroups", v, index);
	   }
	}
	index = endPoint;
	return t;
}

QDomElement epgCompiler::process_schedule (QDomDocument &doc,
	                                  const std::vector<uint8_t> &v,
	                                  int &index) {
int endPoint	= setLength (v, index);
QDomElement schedule;
	schedule = doc. createElement ("schedule");

	while (index < endPoint) {
	   QString s;
	   switch (v [index]) {
	      case 0x11: {	// mediumname
	         QDomElement child = process_mediumName (doc, v, index);
	         schedule. appendChild (child);
	         break;
	      }
	      case 0x12: {	// longName
	         QDomElement child = process_longName (doc, v, index);
	         schedule. appendChild (child);
	         break;
	      }
	      case 0x13: {	// mediaDescription
	         QDomElement child = process_mediaDescription (doc, v, index);
	         schedule. appendChild (child);
	         break;
	      }
	      case 0x17: {	// memberOf
	         QDomElement t =  process_memberOf (doc, v, index);
	         schedule. appendChild (t);
	         break;
	      }
	      case 0x19: {	// location
	         QDomElement t = process_location (doc, v, index);
	         schedule. appendChild (t);
	         break;
	      }
	      case 0x1C: {	// programme
	         QDomElement t =  process_programme (doc, v, index);
	         schedule. appendChild (t);
	         break;
	      }
	      case 0x24: {	// scope
	         QDomElement t = process_scope (doc, v, index);
	         schedule. appendChild (t);
	         break;
	      }
	      case 0x80: {	// version
	         QString s = process_483 (v, index);
	         schedule. setAttribute ("version", s);
	         break;
	      }
	      case 0x81: {	// creation time
	   	 QString s =  process_474 (v, index);
	         schedule. setAttribute ("creationTime", s);
	         break;
	      }
	      case 0x82: {	// originator
	         QString s = process_440 (v, index);
	         schedule. setAttribute ("originator", s);
	         break;
	      }
	      default:
	         process_forgotten ("schedule", v, index);
	         break;
	   }
	}
	if (!schedule. hasAttribute ("version");
	   schedule. setAttribute ("version", 1);
	index	= endPoint;
	return schedule;
}

QDomElement epgCompiler::process_programmeGroup (QDomDocument &doc,
	                                        const std::vector<uint8_t> &v,
	                                        int &index) {
int endPoint = setLength (v, index);
QDomElement t;
	t = doc. createElement ("programGroup");
	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x10: {		// shortName
	         QDomElement child = process_shortName (doc, v, index);
	         t. appendChild (child);
	         break;
	      }
	      case 0x11: {		// mediumName
	         QDomElement child = process_mediumName (doc, v, index);
	         t. appendChild (child);
	         break;
	      }
	      case 0x12: {		// longName
	         QDomElement child = process_longName (doc, v, index);
	         t. appendChild (child);
	         break;
	      }
	      case 0x13: {		// mediaDescription
	         QDomElement child = process_mediaDescription (doc, v, index);
	         t. appendChild (child);
	         break;
	      }
	      case 0x14: {		// genre
	         QDomElement child = process_genre (doc, v, index);
	         t. appendChild (child);
	         break;
	      }
	      case 0x16: {		// keyWords
	         QDomElement child = process_keyWords (doc, v, index);
	         t. appendChild (child);
	         break;
	      }
	      case 0x17: {		// memberOf
	         QDomElement child = process_memberOf (doc, v, index);
	         t. appendChild (child);
	         break;
	      }
	      case 0x18: {		// link
	         QDomElement child = process_link (doc, v, index);
	         t. appendChild (child);
	         break;
	      }
	      case 0x80: {		// id
	         QString s = process_471 (v, index);
	         t. setAttribute ("version", s);
	         break;
	      }
	      case 0x81: {		// shortId
	   	 QString s =  process_472 (v, index);
	         t. setAttribute ("shortId", s);
	         break;
	      }
	      case 0x82: {		// version
	         QString s = process_483 (v, index);
	         t. setAttribute ("version", s);
	         break;
	      }
	      case 0x83: {		// type
	         QString s = process_groupType (v, index);
	         t. setAttribute ("type", s);
	         break;
	      }
	      case 0x64: {	//  numOfOtems 484
	         QString s = process_484 (v, index);
	         t. setAttribute ("numOfItems", s);
	         break;
	      }
	      default:
	         process_forgotten ("programGroup", v, index);
	   }
	}
	index = endPoint;
	return t;
}

QDomElement epgCompiler::process_scope (QDomDocument &doc,
	                               const std::vector<uint8_t> &v,
	                               int &index) {
int endPoint	= setLength (v, index);
QDomElement scope;
	scope = doc. createElement ("scope");
	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x25: {	// serviceScope
	         QDomElement x = process_serviceScope (doc, v, index);
	         scope. appendChild (x);
	         break;
	      }
	      case 0x80: { //	startTime
	         QString startTime = process_474 (v, index);
	         scope. setAttribute ("startTime", startTime);
	         break;
	      }
	      case 0x81: {	// stopTime
	         QString stopTime = process_474 (v, index);
	         scope. setAttribute ("stopTime", stopTime);
	         break;
	      }
	      default:
	         process_forgotten ("scope", v, index);
	         break;
	   }
	}
	index = endPoint;
	return scope;
}

QDomElement epgCompiler:: process_serviceScope (QDomDocument &doc,
	                                       const std::vector<uint8_t> &v,
	                                       int &index) {
int endPoint	= setLength (v, index);
QDomElement serviceScope;
	serviceScope = doc. createElement ("serviceScope");
	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x80: {	// Id, 476
	         QString res = process_476 (v, index);
	         serviceScope. setAttribute ("id", res);
	         break;
	      }
	      default:
	         process_forgotten ("serviceScope", v, index);
	         break;
	   }
	}
	index = endPoint;
	return serviceScope;
}

QDomElement epgCompiler::process_ensemble (QDomDocument &doc,
	                                  const std::vector<uint8_t> &v,
	                                  int &index) {
int endPoint	= setLength (v, index);
QDomElement ensemble;
	ensemble = doc. createElement ("ensemble");
	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x10: {	// shortName
	         QDomElement child = process_shortName (doc, v, index);
	         ensemble. appendChild (child);
	         break;
	      }
	      case 0x11: {	// mediumName
	         QDomElement child = process_mediumName (doc, v, index);
	         ensemble. appendChild (child);
	         break;
	      }
	      case 0x12: {	// longname
	         QDomElement child = process_longName (doc, v, index);
	         ensemble. appendChild (child);
	         break;
	      }
	      case 0x13: {	// media description 
	         QDomElement child = process_mediaDescription (doc, v, index);
	         ensemble. appendChild (child);
	         break;
	      }
	      case 0x16: {	// keyWords
	         QDomElement child = process_keyWords (doc, v, index);
	         ensemble. appendChild (child);
	         break;
	      }
	      case 0x18: {	// link
	         QDomElement child = process_link (doc, v, index);
	         ensemble. appendChild (child);
	         break;
	      }
	      case 0x28: {	// service
	         QDomElement child = process_service (doc, v, index);
	         ensemble. appendChild (child);
	         break;
	      }
	      case 0x80: {	//id 
	         int localEnd = setLength (v, index);
	         int ecc = v [index];
	         int Eid = (v [index + 1] << 8) | v [index + 2];
	         ensemble. setAttribute ("ecc", QString::number (ecc, 16));
	         ensemble. setAttribute ("Eid", QString::number (Eid, 16));
	         index = localEnd;
	         break;
	      }
	      default:
	         process_forgotten ("enemble", v, index);
	         break;
	   }
	}
	index = endPoint;
	return ensemble;
}

QDomElement epgCompiler::process_service (QDomDocument &doc,
	                                 const std::vector<uint8_t> &v,
	                                 int &index) {
int endPoint	= setLength (v, index);
QDomElement service;
	   service = doc. createElement ("service");

	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x10: {	// shortName
	         QDomElement child = process_shortName (doc, v, index);
	         service. appendChild (child);
	         break;
	      }
	      case 0x11: {	// mediumName
	         QDomElement child = process_mediumName (doc, v, index);
	         service. appendChild (child);
	         break;
	      }
	      case 0x12: {	// longname
	         QDomElement child = process_longName (doc, v, index);
	         service. appendChild (child);
	         break;
	      }
	      case 0x13: {	// media description 
	         QDomElement child = process_mediaDescription (doc, v, index);
	         service. appendChild (child);
	         break;
	      }
	      case 0x14: {	// genre
	         QDomElement child = process_genre (doc, v, index);
	         service. appendChild (child);
	         break;
	      }
	      case 0x16: {	// keyWords
	         QDomElement child = process_keyWords (doc, v, index);
	         service. appendChild (child);
	         break;
	      }
	      case 0x29: {	// bearer
	         QDomElement child = process_bearer (doc, v, index);
	         service. appendChild (child);
	         break;
	      }
	      case 0x31: {	// radiodns
	         QDomElement child =  process_radiodns (doc, v, index);
	         service. appendChild (child);
	         break;
	      }
	      case 0x32: {	// geolocation
	         QDomElement child = process_geolocation (doc, v, index);
	         service. appendChild (child);
	         break;
	      }
	      case 0x80: {	// version
	         QString s = process_483 (v, index);
	         service. setAttribute ("version", s);
	         break;
	      }
	      default:
	         process_forgotten ("service", v, index);
	         break;
	   }
	}
	return service;
}

QDomElement	epgCompiler::process_bearer (QDomDocument &doc,
	                                    const std::vector<uint8_t> &v,
	                                    int &index) {
int endPoint = setLength (v, index);
QDomElement bearer;
	bearer = doc. createElement ("bearer");
	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x80: {	// id, 476
	         QString res = process_476 (v, index);
	         bearer. setAttribute ("id", res);
	         break;
	      }
	      case 0x82: {	// url, 440
	         int localEnd = setLength (v, index);
	         QString s;
	         for (int i = index; i < endPoint; i ++)
	            s += QChar (v [i]);
	         bearer. setAttribute ("url", s);
	         index = localEnd;
	         break;
	      }
	      default:
	         process_forgotten ("bearer", v, index);
	         break;
	   }
	}
	index = endPoint;
	return bearer;
}

QDomElement epgCompiler::process_multimedia (QDomDocument &doc,
	                                    const std::vector<uint8_t> &v,
	                                    int &index) {
int endPoint = setLength (v, index);
QDomElement multimedia;
	multimedia = doc. createElement ("multimedia");
QString s;
	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x80: {	// mime value	473
	         QString s = process_473 (v, index);
	         multimedia. setAttribute ("mime value", s);
	         break;
	      }
	      case 0x81: {	// xml:lang	ignore
	         ignore (v, index);
	         break;
	      }
	      case 0x82: { 	//440,  url
	         int localEnd = setLength (v, index);
	         for (int i = index; i < localEnd; i ++)
	            s += QChar (v [i]);
	         multimedia. setAttribute ("url", s);
	         index = localEnd;
	         break;
	      }
	      case 0x83: {	// 46, type
	         int localEnd = setLength (v, index);
	         switch (v [index]) {
	            case 0x02:	// logo_unrestricted
	               s = "logo unrestricted";
	               break;
	            case 0x03:	// Not used
	            case 0x05:
	            default:
	               s = "notUsed";
	               break;
	            case 0x04:
	               s = "logo_colour_square";
	               break;
	            case 0x06:
	               s = "logo_colour_rectangle";
	               break;
	         }
	         multimedia. setAttribute ("type", s);
	         index = localEnd;
	         break;
	      }

	      case 0x84: {	// width	485
	         QString s = process_485 (v, index);
	         multimedia. setAttribute ("width", s);
	         break;
	      }

	      case 0x85: {	// height	485
	         QString s = process_485 (v, index);
	         multimedia. setAttribute ("height", s);
	         break;
	      }
	      default:
	         process_forgotten ("multimedia", v, index);
	         break;
	   }
	}
	index = endPoint;
	return multimedia;
}

QDomElement epgCompiler::process_time (QDomDocument &doc,
	                              const std::vector<uint8_t> &v,
	                              int &index) {
int endPoint = setLength (v, index);
QDomElement time;
	time = doc. createElement ("time");
	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x80: {	// time
	         QString s = process_474 (v, index);
	         time. setAttribute ("time", s);
	         break;
	      }
	      case 0x81: {	// duration
	         QString s = process_475 (v, index);
	         time. setAttribute ("duration", s);
	         break;
	      }
	      case 0x82: {	// actual time
	         QString s = process_474 (v, index);
	         time. setAttribute ("actualTime", s);
	         break;
	      }
	      case 0x83: {	// actal duration
	         QString s = process_475 (v, index);
	         time. setAttribute ("actualDuration", s);
	         break;
	      }
	      default:
	         process_forgotten ("time", v, index);
	         break;
	   }
	}
	index = endPoint;
	return time;
}

QDomElement epgCompiler::process_relativeTime (QDomDocument &doc,
	                                      const std::vector<uint8_t> &v,
	                                      int &index) {
int endPoint = setLength (v, index);
QDomElement t;
	 t = doc. createElement ("relativeTime");
	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x80: {	// time
	         QString s = process_474 (v, index);
	         t. setAttribute ("time", s);
	         break;
	      }
	      case 0x81: {	// duration
	         QString s = process_475 (v, index);
	         t. setAttribute ("duration", s);
	         break;
	      }
	      case 0x82: {	// actual time
	         QString s = process_474 (v, index);
	         t. setAttribute ("actualTime", s);
	         break;
	      }
	      case 0x83: {	// actal duration
	         QString s = process_475 (v, index);
	         t. setAttribute ("actualDuration", s);
	         break;
	      }
	      default:
	         process_forgotten ("time", v, index);
	         break;
	   }
	}
	index = endPoint;
	return t;
}

QDomElement epgCompiler::process_programmeEvent (QDomDocument &doc,
	                                        const std::vector<uint8_t> &v,
	                                        int &index) {
int endPoint = setLength (v, index);
QDomElement programmeEvent;
	programmeEvent = doc. createElement ("link");
	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x10: {	// shortName
	         QDomElement child = process_shortName (doc, v, index);
	         programmeEvent. appendChild (child);
	         break;
	      }
	      case 0x11: {	// mediumname
	         QDomElement child = process_mediumName (doc, v, index);
	         programmeEvent. appendChild (child);
	         break;
	      }
	      case 0x12: {	//longName
	         QDomElement child = process_longName (doc, v, index);
	         programmeEvent. appendChild (child);
	         break;
	      }
	      case 0x13: {	// mediaDescription
	         QDomElement s = process_mediaDescription (doc, v, index);
	         programmeEvent. appendChild (s);
	         break;
	      }
	      case 0x14: {	// genre
	         QDomElement s = process_genre (doc, v, index);
	         programmeEvent. appendChild (s);
	         break;
	      }
	      case 0x16: {	//keywords
	         QDomElement s = process_keyWords (doc, v, index);
	         programmeEvent. appendChild (s);
	         break;
	      }
	      case 0x17: {	// memberOf
	         QDomElement s = process_memberOf (doc, v, index);
	         programmeEvent. appendChild (s);
	         break;
	      }
	      case 0x18: {	// link
	         QDomElement s = process_link (doc, v, index);
	         programmeEvent. appendChild (s);
	         break;
	      }
	      case 0x19: {	// location
	         QDomElement s = process_location (doc, v, index);
	         programmeEvent. appendChild (s);
	         break;
	      }
	      case 0x36: {	// onDemand
	         QDomElement s = process_onDemand (doc, v, index);
	         programmeEvent. appendChild (s);
	         break;
	      }
//
//	the attributes
	      case 0x80: {	// Id
	         QString s = process_471 (v, index);
	         programmeEvent. setAttribute ("id", s);
	         break;
	      }
	      case 0x81: {	//shortId
	         QString s = process_472 (v, index);
	         programmeEvent. setAttribute ("shortId", s);
	         break;
	      }
	      case 0x82: {	// version
	         QString s = process_483 (v, index);
	         programmeEvent. setAttribute ("version", s);
	         break;
	      }
	      case 0x83: {	// recommendation
	        QString s = process_recommendation (v, index);
	        programmeEvent. setAttribute ("recommendation", s);
	        break;
	      }
	      case 0x84: {	// broadcast
	         QString s = process_broadcast (v, index);
	         programmeEvent. setAttribute ("broadcast", s);
	         break;
	      }
	      case 0x86: {	//xml:lang 481
	         ignore (v, index);
	         break;
	      }
	      default:
	         process_forgotten ("programma event", v, index);
	         break;
	   }
	}
	index = endPoint;
	return programmeEvent;
}

QDomElement epgCompiler::process_radiodns (QDomDocument &doc,
	                                  const std::vector<uint8_t> &v,
	                                  int &index) {
int endPoint = setLength (v, index);
QDomElement radiodns;
	radiodns = doc. createElement ("radiodns");
	index = endPoint;
	return radiodns;
}

QDomElement epgCompiler::process_geolocation (QDomDocument &doc,	
	                                     const std::vector<uint8_t> &v,
	                                     int &index) {
int endPoint = setLength (v, index);
QDomElement geolocation;
	geolocation = doc. createElement ("geolocation");
	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x33: { 	// country
	         QDomElement t = process_country (doc, v, index);
	         geolocation. appendChild (t);
	         break;
	      }
	      case 0x34: {	// point
	         QDomElement t = process_point (doc, v, index);
	         geolocation. appendChild (t);
	         break;
	      }
	      case 0x35: {	// polygon
	         QDomElement t = process_polygon (doc, v, index);
	         geolocation. appendChild (t);
	         break;
	      }
	      case 0x80: 	// xml:id
	      case 0x81:	// ref
	      default:
	         process_forgotten ("geolocation", v, index);
	         break;
	   }
	}
	index = endPoint;
	return geolocation;
}

QDomElement epgCompiler::process_country (QDomDocument &doc,
	                                 const std::vector<uint8_t> &v,
	                                 int &index) {
int endPoint = setLength (v, index);
QDomElement t;
	t = doc. createElement ("country");
	index = endPoint;
	return t;
}

QDomElement epgCompiler::process_point (QDomDocument &doc,
	                               const std::vector<uint8_t> &v,
	                               int &index) {
int endPoint = setLength (v, index);
QDomElement t;
	 t = doc. createElement ("point");
	index = endPoint;
	return t;
}

QDomElement epgCompiler::process_polygon (QDomDocument &doc,
	                                 const std::vector<uint8_t> &v,
	                                 int &index) {
int endPoint = setLength (v, index);
QDomElement t;
	 t = doc. createElement ("polygon");
	index = endPoint;
	return t;
}

QDomElement epgCompiler::process_onDemand (QDomDocument &doc,
	                                  const std::vector<uint8_t> &v,
	                                  int &index) {
int endPoint = setLength (v, index);
QDomElement onDemand;
	onDemand = doc. createElement ("onDemand");
	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x2D: {	// bearer
	         QDomElement t = process_bearer (doc, v, index);
	         onDemand. appendChild (t);
	         break;
	      }
	      case 0x37: {	// presentationTime
	         QDomElement t = process_presentationTime (doc, v, index);
	         onDemand. appendChild (t);
	         break;
	      }
	      case 0x38: {	//acquisitionTime
	         QDomElement t = process_acquisitionTime (doc, v, index);
	         onDemand. appendChild (t);
	         break;
	      }
	      default:
	         process_forgotten ("onDemand", v, index);
	         break;
	   }
	}
	index = endPoint;
	return onDemand;
}

QDomElement epgCompiler::process_presentationTime (QDomDocument &doc,
	                                         const std::vector<uint8_t> &v,
	                                         int &index) {
int endPoint = setLength (v, index);
QDomElement t;
	t = doc. createElement ("oresentationTime");
	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x80: {	// start
	         QString s = process_474 (v, index);
	         t. setAttribute ("start", s);
	         break;
	      }
	      case 0x81: {	// end
	         QString s = process_474 (v, index);
	         t. setAttribute ("end", s);
	         break;
	      }
	      case 0x82: {	// duration
	         QString s = process_475 (v, index);
	         t. setAttribute ("duration", s);
	         break;
	      }
	      default:
	         process_forgotten ("presentationTime", v, index);
	         break;
	   }
	}
	index = endPoint;
	return t;
}

QDomElement epgCompiler::process_acquisitionTime (QDomDocument &doc,
	                                         const std::vector<uint8_t> &v,
	                                         int &index) {
int endPoint = setLength (v, index);
QDomElement t;
	t = doc. createElement ("acquisitionTime");
	while (index < endPoint) {
	   switch (v [index]) {
	      case 0x80: {	// start
	         QString s = process_474 (v, index);
	         t. setAttribute ("start", s);
	         break;
	      }
	      case 0x81: {	// end
	         QString s = process_474 (v, index);
	         t. setAttribute ("end", s);
	         break;
	      }
	      default:
	         process_forgotten ("acquisitionTime", v, index);
	         break;
	   }
	}
	index = endPoint;
	return t;
}

QString	epgCompiler::process_440	(const std::vector<uint8_t> &v, int &index) {
int endPoint	= setLength (v, index);

	QString res;
	for (int i = index; i < endPoint; i ++)
	   res += QChar (v [i]);
	index = endPoint;
	return res;
}

//	CRId datatype
QString	epgCompiler::process_471	(const std::vector<uint8_t> &v, int &index) {
int endPoint	= setLength (v, index);
	
	QString s;
	for (int i = index; i < endPoint; i ++)
	   s += QChar (v [i]);
	index	= endPoint;
	return s;
}

QString epgCompiler::process_472 (const std::vector<uint8_t> &v, int &index) {
int endPoint = setLength (v, index);
uint32_t res = (v [index] << 16) | (v [index + 1] << 8) | v [index + 2];
	index = endPoint;
	return QString::number (res);
}

//	MIME type
QString	epgCompiler::process_473	(const std::vector<uint8_t> &v, int &index) {
int endPoint	= setLength (v, index);
QString s;
	if (v [index] < 20)
	   s = stringTable [v [index + 1]];
	else {
	   QByteArray text;
	   for (int i = index; i < endPoint; i ++)
	      text.push_back (v [i]);
	   s = QString::fromUtf8 (text);
	}
	index = endPoint;
	return s;
}

QString	twoDigits (int16_t v) {
	if (v >= 10)
	   return QString::number (v);
	else
	   return '0' + QString::number (v);
}
//
//	ETSI TS 102 371: 4.7.4 time point
QString epgCompiler::process_474 (const std::vector<uint8_t> &v, int &index) {
	if ((v [index] != 0x80) && (v [index] != 0x81))
	   return QString ("ik weet het niet");
	
	int endPoint = setLength (v, index);
	uint32_t mjd	= getBits (v, 8 * index + 1, 17);
	uint16_t dateOut [4];
	convertTime (mjd, dateOut);
	int16_t Y	= dateOut [0];
	int16_t M	= dateOut [1];
	int16_t D	= dateOut [2];
//	we need to know whether it is today or not
	int ltoFlag	= getBit (v, 8 * index + 19);
	int utcFlag	= getBit (v, 8 * index + 20);
	int ltoBase	= utcFlag == 1 ? 48 : 32;
	int hours	= getBits (v, 8 * index + 21, 5);
	int minutes	= getBits (v, 8 * index + 26, 6);
	index = endPoint;
	QString res = QString::number (Y) + "-" +
	                       QString::number (M) + "-" +
	                       QString::number (D) + "T" +
	                       twoDigits (hours) + ":" +
	                       twoDigits (minutes);
	return res;
}

//	ETSI TS 102 371: 4.7.5 Duration type
QString	epgCompiler::process_475	(const std::vector<uint8_t> &v, int &index) {
int endPoint	= setLength (v, index);

	int duration	= (v [index] << 8) | v [index + 1];
	int minutes	= (duration / 60) % 60;
	int hours	= duration / 3600;
	QString res	= "PT";
	if (hours > 0)
	   res = res + twoDigits (hours) + "H";
	res += twoDigits (minutes) + "M";
	index = endPoint;
	return res;
}

//	BearerURI type
QString	epgCompiler::process_476	(const std::vector<uint8_t> &v, int &index) {
int endPoint	= setLength (v, index);
	uint8_t ecc = v [index + 1];
	uint16_t eid = (v [index + 2] << 8) | v [index + 3];
	uint32_t SId = 0;
	int upTo = (v [index] % bitTable [3]) ? 4 : 2;
	for (int i = 0; i < upTo; i ++) {
	   SId = SId << 8;
	   SId |= v [index + 4 + i];
	}
	QString result = QString::number (ecc, 16) + ":" +
	QString::number (eid, 16) + ":" +
	QString::number (SId, 16);
	index = endPoint;
	return result;
}
//	index
QString	epgCompiler::process_482	(const std::vector<uint8_t> &v, int &index) {
int endPoint	= setLength (v, index);
uint16_t res	= (v [index] << 8) | v [index + 1];
	index	= endPoint;
	return QString::number (res);
}
//	version
QString	epgCompiler::process_483	(const std::vector<uint8_t> &v, int &index) {
int endPoint	= setLength (v, index);
	int numbers = (v [index] << 8) | v [index + 1];
	index	= endPoint;
	return QString::number (numbers);
}
//	numof Items
QString	epgCompiler::process_484	(const std::vector<uint8_t> &v, int  &index) {
int endPoint	= setLength (v, index);
	int numbers = (v [index] << 8) | v [index + 1];
	index	= endPoint;
	return QString::number (numbers);
}

QString epgCompiler::process_485 (const std::vector<uint8_t> &v, int &index) {
int endPoint	= setLength (v, index);
	int size	= (v [index] << 8) | v [index];
	index = endPoint;
	return QString::number (size);
}
//
//	strong table handling
//
void	epgCompiler::process_tokenTable	(const std::vector<uint8_t> &v,
	                                 int  &index) {
int endPoint	= setLength (v, index);
//	token table element, section 4.9
	while (index < endPoint)
	   process_token (v, index);
	index = endPoint;
}

void	epgCompiler::process_token (const std::vector<uint8_t> &v,
	                                             int  &index) {
uint8_t tag = v [index];
int endPoint	= index + 2 + v [index + 1];
int length	= v [index + 1];
	index += 2;
	QByteArray text;
	for (int i = index; i < endPoint; i ++) {
	   text. push_back (v [i]);
	}
	if (tag <= 20) {
	   stringTable [tag] = QString::fromUtf8 (text);
	}
	index = endPoint;
}

void	epgCompiler::process_obsolete (const std::vector<uint8_t> &v,
	                              int &index) {
int endPoint	= setLength (v, index);
	index	= endPoint;
}

void	epgCompiler::process_forgotten (const QString s,
	                               const std::vector<uint8_t> &v,
	                               int &index) {
int key	= v [index];
int endPoint	= setLength (v, index);
	fprintf (stderr, "for %s the key %x was not recognized\n",
	                              s. toLatin1 (). data (), key);
	index = endPoint;
}

QString epgCompiler::process_broadcast  (const std::vector<uint8_t> &v,
	                                int &index) {
int endPoint	= setLength (v, index);
	uint8_t valueByte = v [index];
	index	= endPoint;
	return (valueByte == 0x01) ? "on-air" :  "off-air";
}

QString	epgCompiler::process_recommendation (const std::vector<uint8_t> &v,
	                                    int &index) {
int endPoint	= setLength (v, index);
	int valueByte = v [index];
	index = endPoint;
	return valueByte == 0x01 ? "no" : "yes";
}

QString epgCompiler::process_groupType	(const std::vector<uint8_t> &v,
	                                 int &index) {
int endPoint	= setLength (v, index);
static
const char *typeList [] = {
	"series", "show", "programConcept", "magazine", "programCompilation",
	"otherCollection", "otherChoise", "topic"};
	
	uint8_t valueByte = v [index];
	index = endPoint;
	if ((0x02 <= valueByte) && (valueByte <= 0x09))
	   return QString (typeList [valueByte - 2]);
	else
	   return "unknown Type";
}

void	epgCompiler::ignore	(const std::vector<uint8_t> &v, int &index) {
int endPoint	= setLength (v, index);
	index	= endPoint;
}

