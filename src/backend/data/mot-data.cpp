#
/*
 *    Copyright (C) 2015 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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
#include	"mot-data.h"
#include	"radio.h"
#include	<QDir>
//
//	First attempt to do "something" with the MOT data
//
//	Two cases
//	The "single item" case, where an item is made up of an
//	header together with a body
//	The "directory" case, where a directory of files is maintained
//	to form together a slideshow or a website
//
		motHandler::motHandler (RadioInterface *mr,
	                                QString		picturesPath) {
int16_t	i, j;

	this	-> picturesPath	= picturesPath;
//	For "non-directory" MOT's, we have a descriptortable
	for (i = 0; i < 16; i ++) {
	   table [i]. ordernumber = -1;
	   for (j = 0; j < 100; j ++)
	      table [i]. marked [j] = false;
	}
	ordernumber	= 1;
	theDirectory	= NULL;
	old_slide	= NULL;
	connect (this, SIGNAL (the_picture (QByteArray, int, QString)),
	         mr, SLOT (showMOT (QByteArray, int, QString)));
}

	 	motHandler::~motHandler (void) {
}

void	motHandler::process_mscGroup (uint8_t	*data,
	                              uint8_t	groupType,
	                              bool	lastSegment,
	                              int16_t	segmentNumber,
	                              uint16_t	transportId) {
uint32_t segmentSize	= ((data [0] & 0x1F) << 8) | data [1];

	if ((segmentNumber == 0) && (groupType == 3))  // header
	   processHeader (transportId, &data [2], segmentSize, lastSegment);
	else
	if (groupType == 4) 
	   processSegment  (transportId, &data [2], segmentNumber,
	                    segmentSize, lastSegment);
	else
	if ((segmentNumber == 0) && (groupType == 6)) 	// MOT directory
	    processDirectory (transportId, &data [2], segmentSize, lastSegment);
	else
	if (groupType == 6) 	// fields for MOT directory
	   directorySegment (transportId, &data [2],
	                     segmentNumber, segmentSize, lastSegment);
}

//
//	Process a regular header, i.e. a type 3
//	This strongly resembles the newEntry method that
//	creates a header for an item in a directory
void	motHandler::processHeader (int16_t	transportId,
	                           uint8_t	*segment,
	                           int32_t	segmentSize,
	                           bool		lastFlag) {
uint32_t headerSize	=
	     ((segment [3] & 0x0F) << 9) | (segment [4]) | (segment [5] >> 7);
uint32_t bodySize	=
	      (segment [0] << 20) | (segment [1] << 12) |
	                    (segment [2] << 4 ) | ((segment [3] & 0xF0) >> 4);
uint8_t contentType	= ((segment [5] >> 1) & 0x3F);
uint16_t contentsubType = ((segment [5] & 0x01) << 8) | segment [6];
int32_t	pointer	= 7;
QString	name 	= QString ("");

//	If we had a header with that transportId, do not do anything
	if (getHandle (transportId) != NULL) {
	   return;
	}

	while (pointer < headerSize) {
	   uint8_t PLI = (segment [pointer] & 0300) >> 6;
	   uint8_t paramId = (segment [pointer] & 077);
	   uint16_t	length;
	   switch (PLI) {
	      case 00:
	         pointer += 1;
	         break;

	      case 01:
	         pointer += 2;
	         break;

	      case 02:
	         pointer += 5;
	         break;

	      case 03:
	         if ((segment [pointer + 1] & 0200) != 0) {
	            length = (segment [pointer + 1] & 0177) << 8 |
	                      segment [pointer + 2];
	            pointer += 3;
	         }
	         else {
	            length = segment [pointer + 1] & 0177;
	            pointer += 2;
	         }
	         if (paramId == 12) {
	            int16_t i;
	            for (i = 0; i < length - 1; i ++) 
	               name. append (segment [pointer + i + 1]);
	         }
	         pointer += length;
	   } 
	}
	if (getHandle (transportId) != NULL)
	   return;

	newEntry (transportId, bodySize, contentType, contentsubType, name);
}
//
//	type 4 is a segment. These segments are only useful is
//	the header for the transportId is known
void	motHandler::processSegment	(int16_t	transportId,
	                                 uint8_t	*bodySegment,
	                                 int16_t	segmentNumber,
	                                 int32_t	segmentSize,
	                                 bool		lastFlag) {
int32_t	i;

	motElement *handle = getHandle (transportId);
	if (handle == NULL) 	// cannot happen
	  return;

	if (handle -> marked [segmentNumber])  {// copy that we already have
	   return;
	}

//	Note that the last segment may have a different size
	if (!lastFlag && (handle -> segmentSize == -1))
	   handle -> segmentSize = segmentSize;
//
//	If we only have a "last" segment, we do not need to register
//	the segment size
//	sanity check
	if (segmentNumber * handle -> segmentSize + segmentSize >
	                                                handle -> bodySize)
	   return;

	handle -> segments [segmentNumber]. resize (segmentSize);
	for (i = 0; i < segmentSize; i ++) 
	   handle -> segments [segmentNumber][i] = bodySegment [i];
	
	handle -> marked [segmentNumber] = true;
	if (lastFlag) 
	   handle -> numofSegments = segmentNumber + 1;

	if (isComplete (handle)) 
	   handleComplete (handle);
}
//
//	we have data for all directory entries
void	motHandler::handleComplete (motElement *p) {
int16_t i;

QByteArray result;
	for (i = 0; i < p -> numofSegments; i ++)
	   result. append (p -> segments [i]);

	if (p -> contentType == 7) {
	   std::vector<uint8_t> epgData (result. begin (), result. end ());
#ifdef  TRY_EPG
	   QString realName = picturesPath;
	   realName. append (p -> name);
	   realName	= QDir::toNativeSeparators (realName);
	   fprintf (stderr, "epgdata being handled\n");
	   epgHandler. decode (epgData, realName);
#endif
	   return;
	}

	if (p -> contentType != 2) {
           if (p -> name != QString ("")) {
	      QString realName = picturesPath;
	      realName. append (p -> name);
	      realName	= QDir::toNativeSeparators (realName);
	      fprintf (stderr, "going to write file %s\n",
	                           realName. toUtf8 (). data ());
	      checkDir (realName);
	      FILE *x = fopen (realName. toUtf8 (). data (), "wb");
	      if (x == NULL)
	         fprintf (stderr, "cannot write file %s\n",
	                            realName. toUtf8 (). data ());
	      else {
	         (void)fwrite (result. data (), 1, p -> bodySize, x);
	         fclose (x);
	      }
	   }
	   return;
	}
//	MOT slide
	if (old_slide != NULL) {
	   for (i = 0; i < p ->  numofSegments; i ++) {
	      p -> marked [i] = false;
	      p -> segments [i]. clear ();
	   }
	}

	QString realName = picturesPath;
	if (p -> name == QString ("")) 
	   realName. append (QString ("no name"));
	else
	   realName. append (p -> name);
	realName	= QDir::toNativeSeparators (realName);
	checkDir (realName);
	the_picture (result, p -> contentsubType, realName);
	old_slide	= p;
}

void	motHandler::checkDir (QString &s) {
int16_t	ind	= s. lastIndexOf (QChar ('/'));
int16_t	i;
QString	dir;
	if (ind == -1)		// no slash, no directory
	   return;

	for (i = 0; i < ind; i ++)
	   dir. append (s [i]);

	if (QDir (dir). exists ())
	   return;
	QDir (). mkpath (dir);
}

bool	motHandler::isComplete (motElement *p) {
int16_t	i;

	if (p -> numofSegments == -1)
	   return false;
	for (i = 0; i < p ->  numofSegments; i ++) {
//	   fprintf (stderr, "segment [%d] = %d\n", i, p -> marked [i]);
	   if (!(p -> marked [i]))
	      return false;
	}

	return true;
}

motElement	*motHandler::getHandle (uint16_t transportId) {
int16_t	i;
//
//	we first look for the "free" MOT slides, then
//	for the carrousel
	for (i = 0; i < 16; i ++)
	   if (table [i]. ordernumber != -1 &&
	                  table [i]. transportId == transportId)
	      return &table [i];
	if (theDirectory == NULL)
	   return NULL;

	for (i = 0; i < theDirectory -> numObjects; i ++) {
	   if (theDirectory -> dir_proper [i]. ordernumber == -1)
	      continue;
	   if (theDirectory -> dir_proper [i]. transportId == transportId)
	      return &(theDirectory -> dir_proper [i]);
	}
	return NULL;
}
//

void	motHandler::processDirectory (int16_t	transportId,
                                      uint8_t	*segment,
                                      int32_t	segmentSize,
                                      bool	lastFlag) {
uint32_t directorySize	= ((segment [0] & 0x3F) << 24) |
	                  ((segment [1]) << 16) |
	                  ((segment [2]) <<  8) | segment [3];
uint16_t numObjects	= (segment [4] << 8) | segment [5];
int32_t	period		= (segment [6] << 16) |
	                  (segment [7] <<  8) | segment [8];
int32_t segSize		= ((segment [9] & 0x1F) << 8) |
	                   segment [10];
//
//	If we had already a directory with that transportId, do not do anything
	if ((theDirectory != NULL) &&
	                (theDirectory -> transportId == transportId)) 
	   return;		// already in!!
//
//	We handle one directory at a time
	if (theDirectory != NULL)	// other directory now
	   delete theDirectory;

	theDirectory = new MOT_directory (transportId, segmentSize,
	                                  directorySize, numObjects);
	memcpy (theDirectory -> dir_segments, segment, segmentSize);
	theDirectory -> marked [0] = true;
}

void	motHandler::directorySegment (uint16_t	transportId,
                                      uint8_t	*segment,
                                      int16_t	segmentNumber,
                                      int32_t	segmentSize,
                                      bool	lastSegment) {
int16_t	i;

	if (theDirectory == NULL)
	   return;
	if (theDirectory -> transportId != transportId)
	   return;
	if (theDirectory -> marked [segmentNumber])
	   return;
	if (lastSegment)
	   theDirectory -> num_dirSegments = segmentNumber + 1;
	theDirectory	-> marked [segmentNumber] = true;
	uint8_t	*address = &theDirectory -> dir_segments [segmentNumber *
	                                    theDirectory -> dir_segmentSize];
	memcpy (address, segment, segmentSize);
//
//	we are "complete" if we know the number of segments and
//	all segments are "in"
	if (theDirectory -> num_dirSegments != -1) {
	   for (i = 0; i < theDirectory -> num_dirSegments; i ++)
	      if (!theDirectory -> marked [i])
	         return;
	}
//
//	yes we have all data to build up the directory
	analyse_theDirectory ();
}
//
//	The directory
void	motHandler::analyse_theDirectory (void) {
uint16_t	numObjects	= theDirectory -> numObjects;
uint32_t	currentBase	= 11;	// in bytes
uint8_t	*data			= theDirectory -> dir_segments;
uint16_t extensionLength	= (data [currentBase] << 8) |
	                                            data [currentBase + 1];
int16_t	i;

	currentBase += 2 + extensionLength;
	for (i = 0; i < numObjects; i ++)
	   currentBase = get_dirEntry (i, data, currentBase);
}

int16_t	motHandler::get_dirEntry	(int16_t	index,
	                                 uint8_t	*data,
	                                 uint32_t	currentBase) {
QString		name ("");

uint16_t transportId	=  (data [currentBase] << 8) | data [currentBase + 1];
uint32_t bodySize	=  (data [currentBase + 2] << 20) |
	                   (data [currentBase + 3] << 12) |
	                   (data [currentBase + 4] <<  4) |
	                  ((data [currentBase + 5] & 0xF0) >> 4);
uint16_t headerSize	= ((data [currentBase + 5] & 0x0F) << 9) |
	                   (data [currentBase + 6] << 1) |
	                  ((data [currentBase + 7] >> 7) & 0x01);
uint8_t  contentType	=  (data [currentBase + 7] >> 1) & 0x3F;
uint16_t subType	= ((data [currentBase + 7] & 0x1) << 8) |
	                    data [currentBase + 8];
int16_t theEnd		= currentBase + 2 + headerSize;

	if (headerSize == 0)
	   return currentBase + 2;
	currentBase	+= 7 + 2;
	while ((int)currentBase < (int)theEnd) {
	   uint8_t PLI = (data [currentBase] & 0300) >> 6;
	   uint8_t paramId = (data [currentBase] & 077);
	   uint16_t	length;
//	   fprintf (stderr, "PLI = %d, paramId = %d\n", PLI, paramId);
//	   fprintf (stderr, "currentBase %d, theEnd %d\n", currentBase, theEnd);
	   switch (PLI) {
	      case 00:
	         currentBase += 1;
	         break;
	      case 01:
	         currentBase += 2;
	         break;

	      case 02:
	         currentBase += 5;
	         break;

	      case 03:
	         if ((data [currentBase + 1] & 0200) != 0) {
	            length = (data [currentBase + 1] & 0177) << 8 |
	                      data [currentBase + 2];
	            currentBase += 3;
	         }
	         else {
	            length = data [currentBase + 1] & 0177;
	            currentBase += 2;
	         }
	         if (paramId == 12) {
	            int16_t i;
	            for (i = 0; i < length - 1; i ++) 
	               name. append (data [currentBase + i + 1]);
	         }
	         currentBase += length;
	   } 
	}

	if ((transportId == 0) || (getHandle (transportId) != NULL))
	   return currentBase;
//
//	creating an entry for an object mentioned in the directory
//	strongly resembles creating a standalone entry, some differences though
	newEntry (index, transportId, bodySize,
	          contentType, subType, name);
	return currentBase;
}

//	Handling a plain header is by:
void	motHandler::newEntry (uint16_t	transportId,
	                      int16_t	size,
	                      int16_t	contentType,
	                      int16_t	contentsubType,
	                      QString	name) {
int16_t		i;
uint16_t	lowest;
int16_t		lowIndex;

	
	for (i = 0; i < 16; i ++) {
	   if (table [i]. ordernumber == -1) {
	      table [i]. ordernumber	= ordernumber ++;
	      table [i]. transportId	= transportId;
	      table [i]. bodySize	= size;
	      table [i]. contentType	= contentType;
	      table [i]. contentsubType	= contentsubType;
	      table [i]. segmentSize	= -1;
	      table [i]. numofSegments	= -1;
	      table [i]. name		= QString (name);
	      return;
	   }
	}
//
//	table full, delete the oldest one
//
	lowest		= 65377;
	lowIndex	= 0;
	for (i = 0; i < 16; i ++) {
	   if (table [i]. ordernumber < lowest) {
	      lowIndex = i;
	      lowest = table [i]. ordernumber;
	   }
	}

	table [lowIndex]. ordernumber	= ordernumber ++;
	table [lowIndex]. transportId	= transportId;
	table [lowIndex]. bodySize	= size;
	table [lowIndex]. contentType	= contentType;
	table [lowIndex]. contentsubType	= contentsubType;
	table [lowIndex]. segmentSize	= -1;
	table [lowIndex]. numofSegments	= -1;
	table [lowIndex]. name		= name;
}
//
//	handling an entry in a directory is
void	motHandler::newEntry (int16_t	index,
	                      uint16_t	transportId,
	                      int16_t	size,
	                      int16_t	contentType,
	                      int16_t	contentsubType,
	                      QString	name) {
motElement	*currEntry = &(theDirectory -> dir_proper [index]);
	currEntry -> ordernumber	= ordernumber ++;
	currEntry -> transportId	= transportId;
	currEntry -> bodySize		= size;
	currEntry -> contentType	= contentType;
	currEntry -> contentsubType	= contentsubType;
	currEntry -> segmentSize	= -1;
	currEntry -> numofSegments	= -1;
	currEntry -> name		= QString (name);
}

