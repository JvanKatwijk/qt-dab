#
/*
 *    Copyright (C) 2020
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
#include	"si-processor.h"

class	serviceInfo {
public:
	uint32_t	serviceId;
	QString		serviceName;
	QString		description;
	QString		keywords;
};

class	ensembleInfo {
public:
	uint32_t	ensembleId;
	std::vector<serviceInfo> services;
};

static
uint8_t	bitTable [] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

static
int	getBit (uint8_t *v, int bitnr) {
int bytenr	= bitnr / 8;

	bitnr 	= bitnr % 8;
	return (v [bytenr] & bitTable [bitnr]) != 0 ? 1 : 0;
}

static
uint32_t getBits (uint8_t *v, int bitnr, int length) {
uint16_t res	= 0;
	for (int i = 0; i < length; i ++) {
	   res <<= 1;
	   res |= getBit (v, bitnr + i);
	}
	return res;
}

#define	STRING		0x01
#define	SI		0x03
#define	STRING_TABLE	0x04
#define	LONG_NAME	0x12
#define	MEDIA_DESCRIPTION	0x13
#define	KEYWORDS	0x16
#define	ENSEMBLE	0x26
#define	SERVICE		0x28
#define	BEARER		0x29


		siProcessor::siProcessor	() {}

		siProcessor::~siProcessor	() {}

ensembleInfo *siProcessor::process_SI (uint8_t *data, int length) {
int	pos	= 0;
uint8_t c	= data [pos];		// 
ensembleInfo	*res	= nullptr;
int	ilength;

	this	-> data		= data;
	this	-> length	= length;
	ilength	= header (&pos);

	if (c != SI) {
	   pos += length;
	   return nullptr;
	}

	int lpos	= pos;
	while (lpos < pos + ilength) {
//	   fprintf (stderr, "current key %x\n", data [lpos]);
	   if (data [lpos] == STRING_TABLE) { // create a token table
	      int llength = header (&lpos);
	      lpos += llength;
	   }
	   else
	   if (data [lpos] == ENSEMBLE) {
	      res = process_ensemble (&lpos);
	   }
	   else {		// creation time, ignore
//	      int llpos = lpos;
	      int llength = header (&lpos);
//	      fprintf (stderr, "key %x met lengte %d\n",
//	                         data [llpos], llength);
	      lpos += llength;
	   }
	   
	}
	return res;
}

ensembleInfo	*siProcessor::process_ensemble (int *pos) {
int	lpos	= *pos;
int	length	= header (&lpos);
int	eoFrame	= lpos + length;
int	res	= 0;
int	llength	= 0;

ensembleInfo *ensemble	= new ensembleInfo;
	ensemble -> ensembleId = 0;
	while (lpos < eoFrame) {
	   res		= 0;
	   int tag	= data [lpos];
	   llength	= header (&lpos);
	   switch (tag) {
	      case 0x80:
	         res = (data [lpos] << 16) |
	               (data [lpos + 1] << 8) |
	                data [lpos + 2];
	         ensemble -> ensembleId = res;
	         lpos += llength;
	         break;

	      case SERVICE:
	         { int llpos = lpos;
	           ensemble ->services.
	                  push_back (process_service (&llpos, llength));
	           lpos += llength;
	         }
	         break;

	      default:
	         fprintf (stderr, "tag %x with length %d\n",
	                             tag, llength);
	         lpos += llength;
	         break;
	   }
	}
	*pos += length;
	fprintf (stderr, "ensemble info\n");
	if (ensemble -> ensembleId != 0)
	   fprintf (stderr, "ensembleId = %X\n\n", ensemble -> ensembleId);
	for (uint i = 0; i < ensemble -> services. size (); i ++) {
	   fprintf (stderr, "\nservice: %s, %x\n",
	                    ensemble -> services. at (i). serviceName. toLatin1 (). data (),
	                    ensemble -> services. at (i). serviceId);
	   fprintf (stderr, "description: %s\n",
	                    ensemble -> services. at (i). description. toLatin1 (). data ());
	   if (ensemble -> services. at (i). keywords != "")
	      fprintf (stderr, "keywords: %s\n",
	                    ensemble -> services. at (i). keywords. toLatin1 (). data ());
	}
	return ensemble;
}

serviceInfo siProcessor::process_service (int *pos, int length) {
int	lpos	= *pos;
int	eoFrame	= lpos + length;
serviceInfo	res;

	while (lpos < eoFrame) {
	   int tag	= data [lpos];
	   int llength	= header (&lpos);
	   switch (tag) {
	      case BEARER:
	         res. serviceId		= process_serviceId (lpos);
	         break;
	      case LONG_NAME:
	         res. serviceName	= process_serviceName (lpos);
	         break;
	      case MEDIA_DESCRIPTION: {
	         QString h 		= process_mediaDescription (lpos);
	         if (h != "")
	            res. description	= h;
	         break;
	      }
	      case KEYWORDS:
	         res. keywords	= process_keywords (lpos);
	         break;

	      default:
	         break;
	   }
	   lpos += llength;
	}
	*pos = eoFrame;
	return res;
}

int		siProcessor::process_serviceId		(int pos) {
int	key	= data [pos];
int	length	= header (&pos);
int	res	= 0;
	switch (key) {
	   case 0x80:
	      {  uint8_t sidFlag = getBits (data, 8 * pos + 3, 1);
                 if (sidFlag == 0)
                    res = getBits (data, 8 * pos + 32, 16);
	         else
                    res = getBits (data, 8 * pos + 32, 32);
//	         fprintf (stderr, "SId %X\n", res);
	         return res;
	      }
	   default:
	      fprintf (stderr, "in serviceId unprocessed key %d\n", key);
	      break;
	}
	return 0;
}

QString	siProcessor::process_serviceName	(int pos) {
char theName [255];
int	key	= data [pos];
int	length	= header (&pos);
int	index;
	switch (key) {
	   case STRING:
	      for (index = 0; index < length; index ++)
	         theName [index] = data [pos + index];
	      theName [length] = 0;
//	      fprintf (stderr, "we found name %s\n", theName);
	      return QString (theName);
	   default:
	      return  QString ("no name");
	}
}

QString	siProcessor::process_mediaDescription	(int pos) {
int	key	= data [pos];
int	length	= header (&pos);
	switch (key) {
	   case 0x1A:
	   case 0x2B:
	   default:
	      return QString ("");
	   case 0x1B:
	      return longDescription (pos);
	}
}

QString	siProcessor::process_keywords		(int pos) {
char theString [255];
int	key	= data [pos];
int	length	= header (&pos);
int	index	= 0;
	switch (key) {
	   case 0x01:		// string
	      for (index = 0; index < length; index ++)
	         theString [index] = data [pos + index];
	      theString [length] = 0;
//	      fprintf (stderr, "%s\n", theString);
	      return QString (theString);
	   default:
	      return QString ("");
	}
}

QString	siProcessor::longDescription		(int pos) {
char theString [1024];
int	key	= data [pos];
int	length	= header (&pos);
int	index	= 0;
	switch (key) {
	   case 0x01:		// string
	      for (index = 0; index < length; index ++)
	         theString [index] = data [pos + index];
	      theString [length] = 0;
//	      fprintf (stderr, "%s\n", theString);
	      return QString (theString);
	   default:
	      return QString ("");
	}
}

int	siProcessor::header (int *pos) {
int lByte	= data [*pos + 1];
int length;
	if (lByte == 0xFE) {
	   length = (data [*pos + 2] << 8) | data [*pos + 3];
	   *pos += 4;
	}
	else
	if (lByte == 0xFF) {
	   length = (((data [*pos + 2] << 8) | data [*pos + 3]) << 8) |
	                                          data [*pos + 4];
	   *pos += 5;
	}
	else {
	   length = lByte;
	   *pos += 2;
	}
	return length;
}
