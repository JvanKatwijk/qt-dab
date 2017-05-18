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
	   crc = (uint16_t) (swap(crc) ^ (uint16_t)buf [count]);
	   crc ^= ((uint8_t)(crc) >> 4);
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

	return;
//	we maintain offsets in bits, the "m" array has one bit per byte
	fprintf (stderr, "\n\n\nNew frame with length (bytes) %d\n", size / 8);
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

	   uint16_t syncword	= getBits (data, offset,      16);
	   int32_t length	= getBits (data, offset + 16, 16);
	   uint16_t crc		= getBits (data, offset + 32, 16);
	   uint8_t frametypeIndicator =
	                          getBits (data, offset + 48,  8);

	   fprintf (stderr, "%x %d\n", syncword, length);
	   uint8_t checkVector [18];
//
//	first the syncword and the length
	   for (i = 0; i < 4; i ++)
	      checkVector [i] = getBits (data, offset + i * 8, 8);
//
//	we skip the crc, and take the frametype
	   checkVector [4] = getBits (data, offset + 6 * 8, 8);

	   if (length < 11) {
	      for (i = 0; i < length; i ++)
	         checkVector [5 + i] = getBits (data,  offset + 7 * 8 + i * 8, 8);
	      checkVector [5 + length] = getBits (data, offset + 4 * 8, 8);
	      checkVector [5 + length + 1] =
	                                 getBits (data, offset + 5 * 8, 8);
	      if (usCalculCRC (checkVector, 5 + length + 2)) {
	         fprintf (stderr, "crc success\n");
	      }
	      else {
	         fprintf (stderr, "crc for frametype 0 failed\n");
	         return;
	      }
	   }
	   else {
	      for (i = 0; i < 11; i ++)
	         checkVector [5 + i] = getBits (data, offset + 7 * 8 + i * 8, 8);
	      checkVector [5 + 11]	= getBits (data, offset + 4 * 8, 8);
	      checkVector [5 + 11 + 1]	= getBits (data, offset + 5 * 8, 8);

	      if (usCalculCRC (checkVector, 5 + 11 + 2)) {
	         fprintf (stderr, "crc success\n");
	      }
	      else {
	         fprintf (stderr, "crc test failed for frame type %d\n",
	                                   frametypeIndicator);
	         return;
	      }
	   }

	   fprintf (stderr, "serviceframe type %d has length %d\n",
	                                  frametypeIndicator, length);
	   if (frametypeIndicator == 0)
	      offset = handleFrame_type_0 (data, offset + 56, length);
	   else
	   if (frametypeIndicator == 1)
	      offset = handleFrame_type_1 (data, offset + 56, length);
	   else
	      return;	// failure
	   fprintf (stderr, "offset = %d (%d)\n", offset, offset / 8);
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
	                                    getBits (data, loffset, 8));
	      fprintf (stderr, "fieldlength = %d\n", 
	                                    getBits (data, loffset + 8, 16));
	      fprintf (stderr, "header crc = %o\n",
	                                    getBits (data, offset + 16, 16));
	      if (serviceComponentFrameheaderCRC (data, loffset, llength))
	         fprintf (stderr, "ready to handle component frame\n");
	      llength -= getBits (data, loffset + 8, 16) * 8 + 5 * 8;
	      loffset += getBits (data, loffset + 8, 16) * 8 + 5 * 8;
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
int16_t	length	= getBits (data, 8, 16);
int16_t	size	= length < 13 ? length : 13;
	if ((length < 0) ||( length >= maxL))
	   return false;
	testVector [0]	= getBits (data, 0,  8);
	testVector [1]	= getBits (data, 8,  8);
	testVector [2]	= getBits (data, 16, 8);
	for (i = 0; i < size; i ++) 
	   testVector [2 + i] = getBits (data, 40 + i * 8, 8);
	testVector [3 + size    ] = getBits (data, 24, 8);
	testVector [3 + size + 1] = getBits (data, 32, 8);

	fprintf (stderr, "size = %d\n", 5 + size);
	return usCalculCRC (testVector, 5 + size) == 0;
}


