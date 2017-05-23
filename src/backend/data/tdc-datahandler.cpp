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

#include	"tdc-datahandler.h"

	tdc_dataHandler::tdc_dataHandler (int16_t appType) {
}

	tdc_dataHandler::~tdc_dataHandler (void) {
}

#define	swap(a)	(((a) << 8) | ((a) >> 8))

//---------------------------------------------------------------------------
uint16_t usCalculCRC (uint8_t *buf, int lg) {
uint16_t crc;
uint	count;
	crc = 0xFFFF;
	for (count= 0; count < lg; count++) {
	   crc = (uint16_t) (swap (crc) ^ (uint16_t)buf [count]);
	   crc ^= ((uint8_t)crc) >> 4;
	   crc = (uint16_t)
	         (crc ^ (swap((uint8_t)(crc)) << 4) ^ ((uint8_t)(crc) << 5));
	}
	return ((uint16_t)(crc ^ 0xFFFF));
}

void	tdc_dataHandler::add_mscDatagroup (QByteArray &m) {
int32_t	offset	= 0;
uint8_t	*data	= (uint8_t *)(m. data ());
int32_t	size	= m. size ();
int16_t	i;
uint16_t	crc;

//	we maintain offsets in bits, the "m" array has one bit per byte
	while (offset < size) {
	   while (offset + 16 < size) {
	      if (getBits (data, offset, 16) == 0xFF0F) {
	         break;
	      }
	      else
	         offset += 8;
	   }
	   if (offset + 16 >= size)
	      return;

//	we have a syncword
	   uint16_t syncword	= getBits (data, offset,      16);
	   int16_t length	= getBits (data, offset + 16, 16);
	   uint16_t crc		= getBits (data, offset + 32, 16);
	   uint8_t frametypeIndicator =
	                          getBits (data, offset + 48,  8);
	   if ((length < 0) || (length >= (size - offset) / 8))
	      return;		// garbage
//
//	OK, prepare to check the crc
	   uint8_t checkVector [18];
//
//	first the syncword and the length
	   for (i = 0; i < 4; i ++)
	      checkVector [i] = getBits (data, offset + i * 8, 8);
//
//	we skip the crc in the incoming data and take the frametype
	   checkVector [4] = getBits (data, offset + 6 * 8, 8);

	   int size = length < 11 ? length : 11;
	   for (i = 0; i < size; i ++)
	      checkVector [5 + i] = getBits (data,  offset + 7 * 8 + i * 8, 8);
	   checkVector [5 + length] = getBits (data, offset + 4 * 8, 8);
	   checkVector [5 + length + 1] =
	                                 getBits (data, offset + 5 * 8, 8);
	   if (check_crc_bytes (checkVector, 5 + length + 2) != 0) {
	      fprintf (stderr, "crc failed\n");
	      return;
	   }

	   if (frametypeIndicator == 0)
	      offset = handleFrame_type_0 (data, offset + 7 * 8, length);
	   else
	   if (frametypeIndicator == 1)
	      offset = handleFrame_type_1 (data, offset + 7 * 8, length);
	   else
	      return;	// failure
	}
}

int32_t	tdc_dataHandler::handleFrame_type_0 (uint8_t *data,
	                                    int32_t offset, int32_t length) {
int16_t i;
int16_t noS	= getBits (data, offset, 8);
	fprintf (stderr, "frametype 0 :");
	for (i = 0; i < noS; i ++)
	    fprintf (stderr, "%o %o %o   ", getBits (data, offset + 8, 8),
	                                    getBits (data, offset + 16, 8),
	                                    getBits (data, offset + 24, 8));
	fprintf (stderr, "\n");
	return offset + length * 8;
}

int32_t	tdc_dataHandler::handleFrame_type_1 (uint8_t *data,
	                                     int32_t offset, int32_t length) {
int16_t i;
	fprintf (stderr, " frametype 1 met %o %o %o\n",
	                             getBits (data, offset,      8),
	                             getBits (data, offset + 8,  8),
	                             getBits (data, offset + 16, 8));

	fprintf (stderr, "encryption %d\n", getBits (data, offset + 24, 8));
	if (getBits (data, offset + 24, 8) == 0) {	// encryption 0
	   int llength = length - 5; 
	   int loffset = offset + 32;
	   do {
	      fprintf (stderr, "component identifier = %d\n",
	                                    getBits (data, loffset + 0, 8));
	      fprintf (stderr, "fieldlength = %d\n", 
	                                    getBits (data, loffset + 8, 16));
	      fprintf (stderr, "header crc = %o\n",
	                                    getBits (data, loffset + 24, 16));
	      if (serviceComponentFrameheaderCRC (data, loffset, llength))
	         fprintf (stderr, "ready to handle component frame\n");
	      else
	         fprintf (stderr, "crc check failed\n");
	      if (getBits (data, loffset + 0, 8) == 0) {
	         for (i = 0; i < 30; i ++)
	            fprintf (stderr, "%o ", getBits (data, loffset + 40 + 8 * i, 8));
	         fprintf (stderr, "\n");
	      }
	      int16_t fieldLength = getBits (data, loffset + 8, 16);
	      llength -= fieldLength + 5;
	      loffset += fieldLength * 8 + 5 * 8;
	   } while (llength > 0);
	}
	else
	   fprintf (stderr, "need to decompress\n");

	return offset + length * 8;
}
//	The component header CRC is two bytes long,
//	and based on the ITU-T polynomial x^16 + x*12 + x^5 + 1.
//	The component header CRC is calculated from the service component
//	identifier, the field length and the first 13 bytes of the
//	component data. In the case of component data shorter
//	than 13 bytes, the component identifier, the field
//	length and all component data shall be taken into account.
bool	tdc_dataHandler::serviceComponentFrameheaderCRC (uint8_t *data,
	                                                 int16_t offset,
	                                                 int16_t maxL) {
uint8_t testVector [18];
int16_t	i;
int16_t	length	= getBits (data, offset + 8, 16);
int16_t	size	= length < 13 ? length : 13;
uint16_t	crc;
	if (length < 0)
	   return false;		// assumed garbage
	crc	= getBits (data, offset + 24, 16); 	// the crc
	testVector [0]	= getBits (data, offset + 0,  8);
	testVector [1]	= getBits (data, offset + 8,  8);
	testVector [2]	= getBits (data, offset + 16, 8);
	for (i = 0; i < size; i ++) 
	   testVector [3 + i] = getBits (data, offset + 40 + i * 8, 8);

	return usCalculCRC (testVector, 3 + size) == crc;
}


