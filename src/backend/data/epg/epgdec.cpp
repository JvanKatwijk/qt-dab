/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2006
 *
 * Author(s):
 *	Julian Cable
 *
 * Description:
 *	ETSI DAB/DRM Electronic Programme Guide XML Decompressor
 *
 *
 ******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
\******************************************************************************/

#include <string>
#include <cstring>
#include <map>
#include	<cstdio>
#include <iostream>
#include <sstream>
#include "epgdec.h"

static QDomElement element (QDomDocument &doc, const tag_length_value &tlv);

void	CEPGDecoder::decode (const vector<_BYTE>& vecData,
                                             const QString &name) {
//	clear the doc, allowing re-use 
	doc. setContent (QString (""));
	tag_length_value tlv (&vecData [0]);
//	fprintf (stderr, "de tag is %d\n", tlv. tag);
	if (tlv. is_epg ()) {
	   doc. appendChild (element (doc, tlv));
	   QString test = doc. toString();
	   FILE *epgFile = fopen (name. toUtf8(). data (), "w");
	   if (epgFile != nullptr) {
	      fprintf (stderr, "filename = %s\n", name. toUtf8(). data());
	      fprintf (epgFile, test. toLatin1(). data());
	      fclose (epgFile);
	   }
	   else
	      fprintf (stderr, "cannot open %s\n", name. toUtf8(). data());
	}
}

typedef enum {nu_attr, enum_attr, string_attr,
	      u16_attr, u24_attr, datetime_attr,
	      duration_attr, sid_attr,
	      genre_href_attr, bitrate_attr} enum_attr_t;

typedef struct {
	const char *name;
	const char **vals;
	enum_attr_t decode;
} dectab;

static char token_list[20][255];

static uint32_t default_content_id;

static const char *enums0 [] = { (char*)2, "DAB", "DRM" };
static const char *enums1[] = { (char*)9, nullptr, "series",
	"show",
	"programConcept",
	"magazine",
	"programCompilation",
	"otherCollection",
	"otherChoice",
	"topic"
};

static const char *enums2[] = { (char*)3, "URL", "DAB", "DRM" };
static const char *enums3[] = { (char*)4, "identical", "more", "less", "similar" };
static const char *enums4[] = { (char*)2, "primary", "alternative" };
static const char *enums5[] = { (char*)7, "audio", "DLS",
	       "MOTSlideshow", "MOTBWS", "TPEG", "DGPS", "proprietary"
};
static const char *enums6[] = { (char*)2, "primary", "secondary" };
static const char *enums7[] = { (char*)2, "none", "unspecified" };
static const char *enums8[] = { (char*)2, "on-air", "off-air" };
static const char *enums9[] = { (char*)2, "no", "yes" };
static const char *enums10[] = { (char*)4, nullptr, "logo_unrestricted",
	                         "logo_mono_square",
	                         "logo_colour_square",
	                         "logo_mono_rectangle"
};

static const char *enums11 [] = { (char*)3, "main", "secondary", "other"};

static const char *classificationScheme [] = {
    nullptr,
    "IntentionCS",
    "FormatCS",
    "ContentCS",
    "IntendedAudienceCS",
    "OriginationCS",
    "ContentalertCS",
    "MediaTypeCS",
    "AtmosphereCS",
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
};


static dectab attribute_tags_epg[] = {
	{"system", enums0, enum_attr},
	{"id", nullptr, string_attr}
};

static dectab attribute_tags_sch[] = {
	{"version", nullptr, u16_attr},
	{"creationTime", nullptr, datetime_attr},
	{"originator", nullptr, string_attr}
};

static dectab attribute_tags_si[] = {
    {"version", nullptr, u16_attr},
    {"creationTime", nullptr, datetime_attr},
    {"originator", nullptr, string_attr},
    {"serviceProvider", nullptr, string_attr},
    {"system", enums0, enum_attr}
};

static dectab attribute_tags2[] = {
    {"shortId", nullptr, u24_attr},
    {"version", nullptr, u16_attr},
    {"type", enums1, enum_attr},
    {"numOfItems", nullptr, u16_attr},
};

static dectab attribute_tags_scope[] = {
    {"startTime", nullptr, datetime_attr},
    {"stopTime", nullptr, datetime_attr}
};

static dectab attribute_tags4[] = {
    {"protocol", enums2, enum_attr},
    {"type", enums3, enum_attr},
    {"url", nullptr, string_attr},
};

//static dectab attribute_tags5[] = {
//	{"id", 0, string_attr},
//	{"version", 0, u16_attr}
//};

static dectab attribute_tags6[] = {
    {"type", enums4, enum_attr},
    {"kHz", nullptr, u24_attr}
};

static dectab attribute_tags7[] = {
    {"version", nullptr, u16_attr},
    {"format", enums5, enum_attr},
    {"Not used", nullptr, nu_attr},
    {"bitrate", nullptr, bitrate_attr}
};

static dectab attribute_tags8[] = {
    {"id", nullptr, string_attr},
    {"type", enums6, enum_attr}
};

static dectab attribute_tags_name[] = {
    {"xml:lang", nullptr, string_attr}
};

static dectab attribute_tags10[] = {
    {"mimeValue", nullptr, string_attr},
    {"xml:lang", nullptr, string_attr},
    {"url", nullptr, string_attr},
    {"type", enums10, enum_attr},
    {"width", nullptr, u16_attr},
    {"height", nullptr, u16_attr}
};

static dectab attribute_tags11[] = {
    {"time", nullptr, datetime_attr},
    {"duration", nullptr, duration_attr},
    {"actualTime", nullptr, datetime_attr},
    {"actualDuration", nullptr, duration_attr}
};

static dectab attribute_tags12[] = {
    {"id", nullptr, sid_attr},
    {"trigger", nullptr, u16_attr}
};

static dectab attribute_tags13[] = {
    {"id", nullptr, string_attr},
    {"shortId", nullptr, u24_attr},
    {"index", nullptr, u16_attr}
};

static dectab attribute_tags14[] = {
    {"url", nullptr, string_attr},
    {"mimeValue", nullptr, string_attr},
    {"xml:lang", nullptr, string_attr},
    {"description", nullptr, string_attr},
    {"expiryTime", nullptr, datetime_attr}
};

static dectab attribute_tags15[] = {
    {"id", nullptr, string_attr},
    {"shortId", nullptr, u24_attr},
    {"version", nullptr, u16_attr},
    {"recommendation", enums9, enum_attr},
    {"broadcast", enums8, enum_attr},
    {"Not used", nullptr, nu_attr},
    {"xml:lang", nullptr, string_attr},
    {"bitrate", nullptr, string_attr}
};

static dectab attribute_tags_genre[] = {
    {"href", nullptr, genre_href_attr},
    {"type", enums11, enum_attr}
};

static dectab attribute_tags18[] = {
    {"type", enums7, enum_attr}
};

struct eltab_t {
	const char * element_name;
	dectab* tags;
	size_t size;
};

static eltab_t element_tables[] = {
    { "", nullptr, 0 },
    { "", nullptr, 0 },
    { "epg", attribute_tags_epg, sizeof (attribute_tags_epg) / sizeof (dectab) },
    { "serviceInformation", attribute_tags_si, sizeof (attribute_tags_si) / sizeof (dectab) },
    { "", nullptr, 0 },
    { "", nullptr, 0 },
    { "", nullptr, 0 },
    { "", nullptr, 0 },
    { "", nullptr, 0 },
    { "", nullptr, 0 },
    { "", nullptr, 0 },
    { "", nullptr, 0 },
    { "", nullptr, 0 },
    { "", nullptr, 0 },
    { "", nullptr, 0 },
    { "", nullptr, 0 },
    { "shortName", attribute_tags_name, sizeof (attribute_tags_name) / sizeof (dectab) },
    { "mediumName", attribute_tags_name, sizeof (attribute_tags_name) / sizeof (dectab) },
    { "longName", attribute_tags_name, sizeof (attribute_tags_name) / sizeof (dectab) },
    { "mediaDescription", nullptr, 0 },
    { "genre", attribute_tags_genre, sizeof (attribute_tags_genre) / sizeof (dectab) },
    { "CA", attribute_tags18, sizeof (attribute_tags18) / sizeof (dectab) },
    { "keywords", attribute_tags_name, sizeof (attribute_tags_name) / sizeof (dectab) },
    { "memberOf", attribute_tags13, sizeof (attribute_tags13) / sizeof (dectab) },
    { "link", attribute_tags14, sizeof (attribute_tags14) / sizeof (dectab) },
    { "location", nullptr, 0 },
    { "shortDescription", attribute_tags_name, sizeof (attribute_tags_name) / sizeof (dectab) },
    { "longDescription", attribute_tags_name, sizeof (attribute_tags_name) / sizeof (dectab) },
    { "programme", attribute_tags15, sizeof (attribute_tags15) / sizeof (dectab) },
    { "", nullptr, 0 },
    { "", nullptr, 0 },
    { "", nullptr, 0 },
    { "programmeGroups", attribute_tags_sch, sizeof (attribute_tags_sch) / sizeof (dectab) },
    { "schedule", attribute_tags_sch, sizeof (attribute_tags_sch) / sizeof (dectab) },
    { "alternateSource", attribute_tags4, sizeof (attribute_tags4) / sizeof (dectab) },
    { "programmeGroup", attribute_tags2, sizeof (attribute_tags2) / sizeof (dectab) },
    { "scope", attribute_tags_scope, sizeof (attribute_tags_scope) / sizeof (dectab) },
    { "serviceScope", attribute_tags12, sizeof (attribute_tags12) / sizeof (dectab) },
    { "ensemble", attribute_tags6, sizeof (attribute_tags6) / sizeof (dectab) },
    { "frequency", attribute_tags7, sizeof (attribute_tags7) / sizeof (dectab) },
    { "service", attribute_tags8, sizeof (attribute_tags8) / sizeof (dectab) },
    { "serviceID", attribute_tags6, sizeof (attribute_tags6) / sizeof (dectab) },
    { "epgLanguage", attribute_tags_name, sizeof (attribute_tags_name) / sizeof (dectab) },
    { "multimedia", attribute_tags10, sizeof (attribute_tags10) / sizeof (dectab) },
    { "time", attribute_tags11, sizeof (attribute_tags11) / sizeof (dectab) },
    { "bearer", attribute_tags12, sizeof (attribute_tags12) / sizeof (dectab) },
    { "programmeEvent", attribute_tags15, sizeof (attribute_tags15) / sizeof (dectab) },
    { "relativeTime", attribute_tags11, sizeof (attribute_tags11) / sizeof (dectab) },
    { "simulcast", attribute_tags_epg, sizeof (attribute_tags_epg) / sizeof (dectab) }
};

//
//	forward declarations
string decode_string (const _BYTE * p, size_t len);
const string element_name (_BYTE tag);

static
void attribute (map<string,string>  &out, _BYTE element_tag,
	                                        tag_length_value &tlv);
static
void string_token_table (const tag_length_value &tlv);

uint16_t get_uint16 (const _BYTE *p) {
uint16_t h = p[0], l = p[1];

	return ((h << 8) | l);
}

uint32_t get_uint24 (const _BYTE* p) {
uint32_t h = p[0], m = p[1], l = p[2];

	return ((((h << 8) | m) << 8) | l);
}

tag_length_value::tag_length_value (const _BYTE* q) {
_BYTE* p = const_cast<_BYTE*> (q);

	tag	= *p++;
	length	= *p++;
	if (length == 0xFE) {
	   length = get_uint16 (p);
	   p += 2;
	}
	else
	if (length == 0xFF) {
	   length = get_uint24 (p);
	   p += 3;
	}

	value = p;
}

#define	invalidTag(x) ((x < 0) || (x > 50))
static
QDomElement element (QDomDocument &doc, const tag_length_value &tlv) {
	if (invalidTag (tlv. tag)) {
	   QString name (" ");
	   QDomElement e = doc. createElement (name);
	   return e;
	}
	
QString	name (element_tables [tlv. tag]. element_name);
QDomElement e	= doc. createElement (name);
map <string, string> attr;
_BYTE *end	= tlv. value + tlv. length;
dectab *at	= element_tables [tlv. tag]. tags;
//	set default attributes 
	for (size_t k = 0; k < element_tables [tlv.tag]. size; k++) {
	   if (at [k]. decode == enum_attr)
	      attr [at [k]. name] = at [k]. vals [1];
	}

	tag_length_value a (tlv. value);
	while (a. is_attribute()) {
	   attribute (attr, tlv. tag, a);
	   _BYTE* p = a. value + a. length;
	   if (p >= end)
	      break;
	   tag_length_value b (p);
	   a = b;
	}

	for (map <string, string>::iterator i = attr. begin();
	                                      i != attr. end(); i++) {
	   e. setAttribute (QString (i -> first. c_str()),
	                    QString (i -> second. c_str()));
	}
	_BYTE* p = a. value;
	while (p < end) {
	   if (a. is_string_token_table() && !tlv. is_child_element())
	      string_token_table (a);
	   else
	   if (a. is_default_id() && !tlv.is_child_element()) {
	      default_content_id = get_uint24 (p);
	      p += 3;
	   }
	   else
	   if (a. is_child_element()) {
	      e. appendChild (element(doc, a));
	   }
	   else
	   if (a. is_cdata()) {
	      string value = decode_string (a. value, a. length);
	      QDomText t = doc. createTextNode (QString(). fromUtf8 (value. c_str()));
	      e. appendChild (t);
	   }
	   p = a. value + a. length;

	   if (p >= end)
	      break;
	   tag_length_value b (p);
	   a = b;
	}
	return e;
}

static
string decode_genre_href (const _BYTE* p, size_t len) {
int cs = p[0] & 0xff;

	if(cs < 1 || cs > 8)
	   return "";
	stringstream out;
	out << "urn:tva:metadata:cs:" << classificationScheme[cs] << ":2005:";
	switch (len) {
	   case 2:
	      out << cs << '.' << int(p[1]);
	      break;
	   case 3:
	      out << cs << '.' << int(p[1]) << '.' << int(p[2]);
	      break;
	   case 4:
	      out << cs << '.' << int(p[1]) << '.' << int(p[2]) << '.' << int(p[3]);
	      break;
	}

	return out. str();
}

string	decode_string (const _BYTE* p, size_t len) {
size_t i;
string out;

	for (i = 0; i < len; i++) {
	   char c = p[i];
	   if (1 <= c && c <= 19)
	      if (c == 0x9 || c == 0xa || c == 0xd)
	         out += c;
	      else
	         out += token_list [p [i]];
	   else
	      out += c;
	}
	return out;
}

static
string	decode_uint16 (const _BYTE* p) {
stringstream out;

	out << get_uint16(p);
	return out. str();
}

static
string	decode_uint24 (const _BYTE* p) {
stringstream out;

	out << int (get_uint24(p));
	return out. str();
}

static
string	decode_sid (const _BYTE* p) {
stringstream out;

	out << hex << int(p[0]) << '.' << int(p[1]) << '.' << int(p[2]);
	return out. str();
}

static
string	decode_dateandtime (const _BYTE* p) {
uint32_t mjd;
uint32_t h = p[0], m = p[1], l = p[2];
uint16_t n, year;
_BYTE month, day;
int hours, minutes, seconds = 0;
int utc_flag, lto_flag, sign = 0, lto = 0;

	mjd		= (((((h << 8) | m) << 8) | l) >> 6) & 0x1ffff;
	lto_flag	= p[2] & 0x10;
	utc_flag	= p[2] & 0x08;
	n		= (p[2] << 8) | p[3];
	hours		= (n >> 6) & 0x1f;
	minutes		= n & 0x3f;
	n		= 4;
	if (utc_flag) {
	   seconds = p[n] >> 2;
	   n += 2;
	}
	stringstream out;
	string tz = "Z";

	if (lto_flag) {
	   sign = p[n] & 0x20;
	   lto = p[n] & 0x3f;
	   int mins = 60*hours+minutes;
//	   cerr << mjd << " " << hours << ":" << minutes << " " << mins << " " << sign << " " << lto << endl;
	   if (sign) {
	      mins -= 30 * lto;
	      if (mins < 0) {
	         mjd --;
	         mins += 24 * 60;
	      }
    	   }
    	   else {
	      mins += 30 * lto;
	      if (mins > (24 * 60)) {
	         mjd ++;
	         mins -= 24 * 60;
	      }
    	   }
	   hours	= mins / 60;
	   minutes = mins % 60;
	   stringstream tzs;
	   tzs << (sign ? '-' : '+');
	   int ltoh = lto / 2;
	   if (ltoh < 10)
	      tzs << '0';
	   tzs << ltoh << ':';
	   int ltom = (30 * lto) % 30;
	   if (ltom < 10)
	      tzs << '0';
	   tzs << ltom;
	   tz = tzs. str();
	}
	if (hours >= 24) {
	   hours -= 24;
	   mjd ++;
	}
	CModJulDate ModJulDate (mjd);
	year	= ModJulDate. GetYear();
	month	= ModJulDate .GetMonth();
	day	= ModJulDate. GetDay();
	out << year << '-';
	if (month < 10) out << '0';
	out << int (month) << '-';
	if (day < 10) out << '0';
	out << int(day) << 'T';
	if (hours < 10) out << '0';
	out << hours << ':';
	if (minutes < 10) out << '0';
	out << minutes << ':';
	if (seconds < 10) out << '0';
	out << seconds << tz;
	return out. str();
}

static
string	decode_duration (const _BYTE* p) {
uint16_t hours, minutes, seconds;

	seconds = get_uint16(p);
	minutes = seconds / 60;
	seconds = seconds % 60;
	hours = minutes / 60;
	minutes = minutes % 60;
	stringstream out;

	out << "PT";
	if (hours > 0)
	   out << hours << 'H';
	if (minutes > 0)
	   out <<  minutes << 'M';
	if (seconds > 0)
	   out << seconds << 'S';
	return out. str();
}

static
string	decode_bitrate (const _BYTE* p) {
stringstream out;
uint16_t n	= get_uint16(p);

	out << float(n) / 0.1f;
	return out. str();
}

static
string	decode_attribute_name (const dectab& tab) {

	if (tab.name == nullptr) {
	   return "unknown";
	}
	if (strlen (tab.name) > 64) { /* some reasonably big number */
	   return "too long";
	}
	return tab. name;
}

static
string	decode_attribute_value (enum_attr_t format,
	                        const _BYTE* p, size_t len) {
	switch (format) {
	   case nu_attr:
	      return "undefined";

	   case enum_attr:
	      return "undecoded enum";

	   case string_attr:
	      return decode_string (p, len);

	   case u16_attr:
	      return decode_uint16 (p);

	   case u24_attr:
	      return decode_uint24 (p);

	   case datetime_attr:
	      return decode_dateandtime (p);

	   case duration_attr:
	      return decode_duration (p);

	   case sid_attr:
	      return decode_sid (p);

	   case genre_href_attr:
	      return decode_genre_href (p, len);

	   case bitrate_attr:
	      return decode_bitrate(p);

	   default:
	      return "";
	}
}

static
void	attribute (map <string,string> &out,
	          _BYTE element_tag, tag_length_value &tlv) {
size_t el	= size_t (element_tag);
size_t e	= sizeof (element_tables) / sizeof (eltab_t);

	if (el >= e) {
	   cerr << "illegal element id" << int(el) << endl;
	   return;
	}

	eltab_t a	= element_tables [el];
	size_t attr	= tlv. tag & 0x0f;
	size_t n	= a. size;

	if (attr >= n) {
	   cerr << "out of range attribute id " <<
	                 attr << " for element id " << int(el) << endl;
	} else {
	   dectab tab = a. tags [attr];
	   string name = decode_attribute_name(tab);
	   string value;
	   if (tab.decode == enum_attr) {
//	needed for 64 bit compatibility 
	      ptrdiff_t index	= tlv. value [0];
	      ptrdiff_t num_vals = (tab. vals [0] - (const char*)nullptr);
	      if (index <= num_vals && index > 0)
	         value = tab. vals [index];
	      else
	         value = "out of range";
	   } else {
	      value = decode_attribute_value (tab. decode,
	                                      tlv. value, tlv.length);
	   }
	   out [name] = value;
	}
}

static
void	string_token_table (const tag_length_value& tlv) {
size_t i = 0;
_BYTE* p = tlv.value;

	for (i = 0; i < 20; i++)
	   token_list [i][0] = 0;
	for (i = 0; i < tlv. length;) {
	   _BYTE tok	= p [i++];
	   size_t tlen	= p [i++];
	   memcpy (token_list [tok], &p [i], tlen);
	   token_list [tok][tlen] = 0;
	   i += tlen;
	}
}

/******************************************************************************\
* Modified Julian Date                                                         *
\******************************************************************************/
void	CModJulDate::Set(const uint32_t iModJulDate) {
uint32_t iZ, iA, iAlpha, iB, iC, iD, iE;
double rJulDate, rF;

	/* Definition of the Modified Julian Date */
	rJulDate = (double) iModJulDate + 2400000.5;

//	Get "real" date out of Julian Date
//	(Taken from "http://mathforum.org/library/drmath/view/51907.html") */
//	1. Add .5 to the JD and let Z = integer part of (JD+.5) and F the
//	fractional part F = (JD+.5)-Z
	iZ = (uint32_t) (rJulDate + (double) 0.5);
	rF = (rJulDate + (double) 0.5) - iZ;

//	2. If Z < 2299161, take A = Z
//	If Z >= 2299161, calculate alpha = INT((Z-1867216.25)/36524.25)
//	and A = Z + 1 + alpha - INT(alpha/4).
	if (iZ < 2299161)
	   iA = iZ;
	else {
	   iAlpha = (int) (((double)iZ - (double)1867216.25) / (double)36524.25);
	   iA = iZ + 1 + iAlpha - (int) ((double) iAlpha / (double) 4.0);
	}

//	3. Then calculate:
//	B = A + 1524
//	C = INT( (B-122.1)/365.25)
//	D = INT( 365.25*C )
//	E = INT( (B-D)/30.6001 )
	iB = iA + 1524;
	iC = (int) (((double) iB - (double) 122.1) / (double) 365.25);
	iD = (int) ((double) 365.25 * iC);
	iE = (int) (((double) iB - iD) / (double) 30.6001);

//	The day of the month dd (with decimals) is:
//	dd = B - D - INT(30.6001*E) + F
	iDay = iB - iD - (int) ((double) 30.6001 * iE);	// + rF;

//	The month number mm is:
//	mm = E - 1, if E < 13.5
//	or
//	mm = E - 13, if E > 13.5
	if ((double) iE < 13.5)
	   iMonth = iE - 1;
	else
	   iMonth = iE - 13;

//	The year yyyy is:
//	yyyy = C - 4716   if m > 2.5
//	or
//	yyyy = C - 4715   if m < 2.5
	if ((float) iMonth > 2.5)
	   iYear = iC - 4716;
	else
	   iYear = iC - 4715;
}

