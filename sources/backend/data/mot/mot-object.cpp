#
/*
 *    Copyright (C) 2015 .. 2024
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
 *
 *	MOT handling is a crime, here we have a single class responsible
 *	for handling a single MOT message with a given transportId
 */
#include	"mot-object.h"
#include	"radio.h"
#include	<algorithm>
#include	"charsets.h"

	   motObject::motObject (RadioInterface *mr,
	                         bool		dirElement,
	                         uint16_t	transportId,
	                         const uint8_t	*segment,
	                         int32_t	segmentSize,
	                         bool		lastFlag,
	                         bool		backgroundFlag) {
int32_t pointer = 7;
uint16_t	rawContentType = 0;

	(void)segmentSize;
	(void)lastFlag;
	this	-> dirElement		= dirElement;
	this	-> backgroundFlag	= backgroundFlag;

	this	-> name			= "";
	connect (this, &motObject::handle_motObject,
	         mr, &RadioInterface::handle_motObject);
	this	-> transportId		= transportId;
	this	-> numofSegments	= -1;
	this	-> segmentSize		= -1;

	headerSize     =
	   ((segment [3] & 0x0F) << 9) |
                   (segment [4] << 1) | ((segment [5] >> 7) & 0x01);
	bodySize       =
	   (segment [0] << 20) | (segment [1] << 12) |
                            (segment [2] << 4 ) | ((segment [3] & 0xF0) >> 4);

// Extract the content type
//	int b	= (segment [5] >> 1) & 0x3F;
	rawContentType  |= ((segment [5] >> 1) & 0x3F) << 8;
	rawContentType	|= ((segment [5] & 0x01) << 8) | segment [6];
	contentType = static_cast<MOTContentType>(rawContentType);

//	fprintf (stderr, "headerSize %d, bodySize %d. contentType %d, transportId %X, segmentSize %d, lastFlag %d\n",
//	               headerSize, bodySize, b, transportId, 
//	               segmentSize, lastFlag);

//	we are actually only interested in the name, if any
	int reference = segmentSize == -1 ? headerSize :
	                 headerSize == -1 ? segmentSize :
	                  std::min ((int)headerSize, (int)segmentSize);
        while ((uint16_t)pointer < reference) {
           uint8_t PLI	= (segment [pointer] & 0300) >> 6;
           uint8_t paramId = (segment [pointer] & 077);
           uint16_t     length;
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

              case 03: {
                 if ((segment [pointer + 1] & 0200) != 0) {
                    length = (segment [pointer + 1] & 0177) << 8 |
                              segment [pointer + 2];
                    pointer += 3 ;
                 }
                 else {
                    length = segment [pointer + 1] & 0177;
                    pointer += 2;
                 }
	         switch (paramId) {
	            case 0:	// reserved for MOT protocol extensons
	            case 2:	// reserved for MOT protocol extenions
	            case 3:	// reserved for MOT protocol extenions
	            case 4:	// reserved for MOT protocol extenions
	            case 6:	// reserved for MOT protocol extenions
	            case 8:	// reserved for MOT protocol extenions
	            case 14:	// reserved for MOT protocol extenions
	            case 15:	// reserved for MOT protocol extenions
	               pointer += length;
	               break;

	            case 5:	// trigger time
	               pointer += length;	// 
	               break;

	            case 7:	// retransmsission distance
	               pointer += length;	// 6.3.4.1.5
	               break;

	            case 9:	// expiration time
	               pointer += length;	//6.2.3.1.1
	               break;

	            case 10:	// priority
	               pointer += length;	// 6.2.3.1.4
	               break;
	 
	            case 11:	// label
	               pointer += length;
	               break;

	            case 12: {	// contentName 6.2.2.1.1
                       uint8_t charSet = segment [pointer] >> 4;
	               QByteArray nameText;
                       for (int i = 1; i < length; i ++) {
	                  if (pointer + i < 12)
                             nameText. append (segment [pointer + i]);
	               }
	               name = toQStringUsingCharset (
	                           (const char *)nameText. data (),
	                           (CharacterSet) charSet,
	                           nameText. size ());
                       pointer += length;
	               break;
	            }

	            case 13:	// Unique Body Version
	               pointer += length;
	               break;

	            case 16:	// Mime type
                       pointer += length;	// 6.2.2.1.2
	               break;

	            case 17:	// compression type
	               pointer += length;	// 6.2.2.1.3
	               break;

	            default:
	               pointer += headerSize;	// this is so wrong!!!
	               break;
	         }
              }
	   }
	}
}

	motObject::~motObject () {
}

uint16_t	motObject::get_transportId () {
	return transportId;
}

//      type 4 is a segment.
//	The pad/dir software will only call this whenever it has
//	established that the current slide has the right transportId
//
//	Note that segments do not need to come in in the right order
void	motObject::addBodySegment (const uint8_t	*bodySegment,
	                           int16_t	segmentNumber,
	                           int32_t	segmentSize,
	                           bool		lastFlag) {

	if ((segmentNumber < 0) || (segmentNumber >= 8192))
	   return;

	if (motMap. find (segmentNumber) != motMap. end ())
	   return;

//	fprintf (stderr, "adding segment %d to %d (last ? %d)\n",
//	                    segmentNumber, transportId, lastFlag);
//      Note that the last segment may have a different size
        if (!lastFlag && (this -> segmentSize == -1))
           this -> segmentSize = segmentSize;

	QByteArray segment;
	segment. resize (segmentSize);
	for (int16_t i = 0; i < segmentSize; i ++)
	   segment [i] = bodySegment [i];
	motMap. insert (std::make_pair (segmentNumber, segment));
//
        if (lastFlag)
           numofSegments = segmentNumber + 1;

	if (numofSegments == -1)
	   return;
//
//	once we know how many segments there are/should be,
//	we check for completeness
	for (int16_t i = 0; i < numofSegments; i ++) {
	   if (motMap. find (i) == motMap. end())
	      return;
	}
//	The motObject is (seems to be) complete
	handleComplete ();
}

void	motObject::handleComplete	() {
QByteArray result;

	for (const auto &it : motMap)
	   result. append (it. second);
//	fprintf (stderr,
//	"Handling complete %X %s, type %X, dirElement %d\n",
//	                  transportId, name. toLatin1 (). data (),
//	                                                  (int)contentType,
//	                                dirElement);
//	if ((name == "") && !dirElement) 
//	   name = QString::number (get_transportId (), 16);
	if (name != "")
	   handle_motObject (result, name, (int)contentType,
	                            dirElement, backgroundFlag);
}

int	motObject::get_headerSize	() {
	return headerSize;
}

