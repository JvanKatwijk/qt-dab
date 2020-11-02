#
/*
 *    Copyright (C) 2015 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB
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
//
//	Interface between msc packages and real MOT handling
#include	"mot-handler.h"
#include	"mot-object.h"
#include	"mot-dir.h"
#include	"radio.h"
//
//	we "cache" the most recent single motSlides (not those in a directory)
//
struct motTable_ {
	uint16_t	transportId;
	int32_t		orderNumber;
	motObject	*motSlide;
} motTable [15];

	motHandler::motHandler (RadioInterface *mr) {
	myRadioInterface	= mr;
	orderNumber		= 0;

	theDirectory		= nullptr;
	for (int i = 0; i < 15; i ++) {
	   motTable [i]. orderNumber	= -1;
	   motTable [i]. motSlide	= nullptr;
	}
}

	motHandler::~motHandler() {
int	i;

	for (i = 0; i < 15; i ++)
	   if (motTable [i]. orderNumber > 0) {
	      if (motTable [i]. motSlide != nullptr) {
	         delete motTable [i]. motSlide;
	         motTable [i]. motSlide = nullptr;
	      }
	   }
	if (theDirectory != nullptr)
	   delete theDirectory;
}

void	motHandler::add_mscDatagroup (std::vector<uint8_t> msc) {
uint8_t *data		= (uint8_t *)(msc. data());
bool	extensionFlag	= getBits_1 (data, 0) != 0;
bool	crcFlag		= getBits_1 (data, 1) != 0;
bool	segmentFlag	= getBits_1 (data, 2) != 0;
bool	userAccessFlag	= getBits_1 (data, 3) != 0;
uint8_t	groupType	= getBits_4 (data, 4);
uint8_t	CI		= getBits_4 (data, 8);
int32_t	next		= 16;		// bits
bool	lastFlag	= false;
uint16_t segmentNumber	= 0;
bool transportIdFlag	= false;
uint16_t transportId	= 0;
uint8_t	lengthInd;
int32_t	i;

	(void)CI;
	if (msc. size() <= 0) {
	   return;
	}

	if (crcFlag && !check_CRC_bits (data, msc.size())) 
	   return;

	if (extensionFlag)
	   next += 16;

	if (segmentFlag) {
	   lastFlag	= getBits_1 (data, next) != 0;
	   segmentNumber = getBits (data, next + 1, 15);
	   next += 16;
	}

	if (userAccessFlag) {
	   transportIdFlag	= getBits_1 (data, next + 3);
	   lengthInd		= getBits_4 (data, next + 4);
	   next	+= 8;
	   if (transportIdFlag) {
	      transportId = getBits (data, next, 16);
	   }
	   next	+= lengthInd * 8;
	}

	int32_t		sizeinBits	=
	              msc. size() - next - (crcFlag != 0 ? 16 : 0);

	if (!transportIdFlag)
	   return;

	std::vector<uint8_t> motVector;
	motVector. resize (sizeinBits / 8);
	for (i = 0; i < sizeinBits / 8; i ++)
	   motVector [i] = getBits_8 (data, next + 8 * i);

	uint32_t segmentSize    = ((motVector [0] & 0x1F) << 8) |
	                                motVector [1];

	switch (groupType) {
	   case 3:
	      if (segmentNumber == 0) {
	         motObject *h = getHandle (transportId);
	         if (h != nullptr) 
	            break;
	         h = new motObject (myRadioInterface,
	                            false,	// not within a directory
	                            transportId,
	                            &motVector [2],	
	                            segmentSize,
	                            lastFlag);
	         setHandle (h, transportId);
	      }
	      break; 

	   case 4: {
	         motObject *h = getHandle (transportId);
	         if ((h == nullptr) && (segmentNumber != 0))
	            break;
	         if ((h == nullptr) && (segmentNumber == 0)) {
	            h = new motObject (myRadioInterface,
                                       false,      // not within a directory
                                       transportId,
                                       &motVector [2],
                                       segmentSize,
                                       lastFlag);
                    setHandle (h, transportId);
	            break;
	         }

	         h -> addBodySegment (&motVector [2],
	                              segmentNumber,
	                              segmentSize,
	                              lastFlag);
	      }
	      break;

	   case 6:
	      if (segmentNumber == 0) { 	// MOT directory
	         if (theDirectory != nullptr)
	            if (theDirectory -> get_transportId() == transportId)
	               break;	// already existing

	         if (theDirectory != nullptr)	// an old one, replace it
	            delete theDirectory;

	         int32_t segmentSize = ((motVector [0] & 0x1F) << 8) |
	                                 motVector [1];
	         uint8_t *segment = &motVector [2];
	         int dirSize	= ((segment [0] & 0x3F) << 24) |
	                           ((segment [1]) << 16) |
	                           ((segment [2]) <<  8) | segment [3];
	         uint16_t numObjects
	                        = (segment [4] << 8) | segment [5];
//	         int32_t period = (segment [6] << 16) |
//	                          (segment [7] <<  8) | segment [8];
//	         int32_t segSize
//	                        = ((segment [9] & 0x1F) << 8) | segment [10];
	         theDirectory	= new motDirectory (myRadioInterface,
	                                            transportId,
	                                            segmentSize,
	                                            dirSize,
	                                            numObjects,
	                                            segment);
	      }
	      else {
	         if ((theDirectory == nullptr) || 
	                (theDirectory -> get_transportId() != transportId))
	            break;
	         theDirectory -> directorySegment (transportId,
	                                           &motVector [2],
	                                           segmentNumber,
	                                           segmentSize,
	                                           lastFlag);
	      }
	      break;

	   default:
	      return;
	}
}

motObject	*motHandler::getHandle (uint16_t transportId) {
int	i;

	for (i = 0; i < 15; i ++) 
	   if ((motTable [i]. orderNumber >= 0) &&
	                   (motTable [i]. transportId == transportId))
	      return motTable [i]. motSlide;
	if (theDirectory != nullptr)
	   return theDirectory -> getHandle (transportId);
	return nullptr;
}

void	motHandler::setHandle (motObject *h, uint16_t transportId) {
int	i;
int	oldest	= orderNumber;
int	index	= 0;

	for (i = 0; i < 15; i ++)
	   if (motTable [i]. orderNumber == -1) {
	      motTable [i]. orderNumber = orderNumber ++;
	      motTable [i]. transportId = transportId;
	      motTable [i]. motSlide	= h;
	      return;
	   }
//
//	if here, the cache is full, so we delete the oldest one
	index	= 0;
	for (i = 0; i < 15; i ++)
	   if (motTable [i]. orderNumber < oldest) {
	      oldest = motTable [i]. orderNumber;
	      index = i;
	   }

	delete motTable [index]. motSlide;
	motTable [index]. orderNumber 	= orderNumber ++;
	motTable [index]. transportId 	= transportId;
	motTable [index]. motSlide		= h;
}

