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
#include	"radio.h"

	tdc_dataHandler::tdc_dataHandler (RadioInterface *mr,
	                                  RingBuffer<uint8_t> *dataBuffer,
	                                  int16_t appType) {
	myRadioInterface	= mr;
	this	-> dataBuffer	= dataBuffer;
//	for the moment we assume appType 4
	connect (this, SIGNAL (bytesOut (int, int)),
	         myRadioInterface, SLOT (handle_tdcdata (int, int)));
}

	tdc_dataHandler::~tdc_dataHandler() {
}

#define	swap(a)	(((a) << 8) | ((a) >> 8))

//---------------------------------------------------------------------------
uint16_t usCalculCRC (uint8_t *buf, int lg) {
uint16_t crc;
int	count;
	crc = 0xFFFF;
	for (count = 0; count < lg; count++) {
	   crc = (uint16_t) (swap (crc) ^ (uint16_t)buf [count]);
	   crc ^= ((uint8_t)crc) >> 4;
	   crc = (uint16_t)
	         (crc ^ (swap((uint8_t)(crc)) << 4) ^ ((uint8_t)(crc) << 5));
	}
	return ((uint16_t)(crc ^ 0xFFFF));
}

void	tdc_dataHandler::add_mscDatagroup (std::vector<uint8_t> m) {
int32_t	offset	= 0;
uint8_t	*data	= (uint8_t *)(m. data());
int32_t	size	= m. size();
int16_t	i;

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
//	   uint16_t syncword	= getBits (data, offset,      16);
	   int16_t length	= getBits (data, offset + 16, 16);
	   uint16_t crc		= getBits (data, offset + 32, 16);

	   (void)crc;
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
	   checkVector [5 + size]	= getBits (data, offset + 4 * 8, 8);
	   checkVector [5 + size + 1]	= getBits (data, offset + 5 * 8, 8);
	   if (!check_crc_bytes (checkVector, 5 + size)) {
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
//int16_t noS	= getBits (data, offset, 8);
uint8_t buffer [length];
	
	for (i = 0; i < length; i ++)
	   buffer [i] = getBits (data, offset + i * 8, 8);
	if (!check_crc_bytes (buffer, length - 2))
	   fprintf (stderr, "crc ook hier fout\n");
#if 0
	fprintf (stderr, "nrServices %d, SID-A %d SID-B %d SID-C %d\n",
	                  buffer [0], buffer [1], buffer [2], buffer [3]);
#endif
	dataBuffer -> putDataIntoBuffer (buffer, length);
	bytesOut (0, length);
	return offset + length * 8;
}

int32_t	tdc_dataHandler::handleFrame_type_1 (uint8_t *data,
	                                     int32_t offset,
	                                     int32_t length) {
int16_t i;
uint8_t buffer [length];
int	lOffset;
int	llengths = length - 4;
#if 0
	fprintf (stderr, " frametype 1  (length %d) met %d %d %d\n", length,
	                             getBits (data, offset,      8),
	                             getBits (data, offset + 8,  8),
	                             getBits (data, offset + 16, 8));
	fprintf (stderr, "encryption %d\n", getBits (data, offset + 24, 8));
#endif
	for (i = 0; i < length; i ++)
	   buffer [i] = getBits (data, offset + i * 8, 8);
	dataBuffer	-> putDataIntoBuffer (buffer, length);
	if (getBits (data, offset + 24, 8) == 0) {	// no encryption
	   lOffset	= offset + 4 * 8;
	   do {
//	      int compInd	= getBits (data, lOffset, 8);	
	      int flength	= getBits (data, lOffset + 8, 16);
//	      int crc		= getBits (data, lOffset + 3 * 8, 8);
#if 0
	      fprintf (stderr, "segment %d, length %d\n",
	                                 compInd, flength);
	      for (i = 5; i < flength; i ++)
	         fprintf (stderr, "%c", buffer [i]);
	      fprintf (stderr, "\n");
#endif
	      lOffset	+= (flength + 5) * 8;
	      llengths -= flength + 5;
	   } while (llengths > 10);
	}
	bytesOut (1, length);
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


