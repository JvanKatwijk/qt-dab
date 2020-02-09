#
/*
    DABlin - capital DAB experience
    Copyright (C) 2015-2019 Stefan Pöschel

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include	"bitWriter.h"
#include	<stdio.h>


// --- BitWriter -------------------------------------------------------------
void BitWriter::Reset() {
	data.clear();
	byte_bits = 0;
}

void BitWriter::AddBits(int data_new, size_t count) {
	while (count > 0) {
//	add new byte, if needed
	   if (byte_bits == 0)
	      data.push_back (0x00);

	   size_t copy_bits = std::min(count, 8 - byte_bits);
	   uint8_t copy_data =
	       (data_new >> (count - copy_bits)) & (0xFF >> (8 - copy_bits));
	   data.back() |= copy_data << (8 - byte_bits - copy_bits);

//		fprintf(stderr, "data_new: 0x%04X, count: %zu / byte_bits: %zu, copy_bits: %zu, copy_data: 0x%02X\n", data_new, count, byte_bits, copy_bits, copy_data);

	   byte_bits = (byte_bits + copy_bits) % 8;
	   count -= copy_bits;
	}
}

void BitWriter::AddBytes (const uint8_t *data, size_t len) {
	for(size_t i = 0; i < len; i++)
	   AddBits (data[i], 8);
}

void BitWriter::WriteAudioMuxLengthBytes() {
	size_t len = data.size () - 3;
	data [1] |= (len >> 8) & 0x1F;
	data [2] = len & 0xFF;
#if 0
	fprintf (stderr, "%x %x  %x  %d(lengte)zijn de eerste bytes\n",
	                 data [0], data [1] >> 5, data [2], len);
#endif
}

