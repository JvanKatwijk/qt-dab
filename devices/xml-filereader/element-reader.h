#
/*
 *    Copyright (C) 2014 .. 2019
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
 */
//
//
//	THIS FILE WILL BE INCLUDED IN xml-reader.cpp
//
static 
float uint8_Table [] = {
 -128 / 128.0 , -127 / 128.0 , -126 / 128.0 , -125 / 128.0 , -124 / 128.0 , -123 / 128.0 , -122 / 128.0 , -121 / 128.0 , -120 / 128.0 , -119 / 128.0 , -118 / 128.0 , -117 / 128.0 , -116 / 128.0 , -115 / 128.0 , -114 / 128.0 , -113 / 128.0 
, -112 / 128.0 , -111 / 128.0 , -110 / 128.0 , -109 / 128.0 , -108 / 128.0 , -107 / 128.0 , -106 / 128.0 , -105 / 128.0 , -104 / 128.0 , -103 / 128.0 , -102 / 128.0 , -101 / 128.0 , -100 / 128.0 , -99 / 128.0 , -98 / 128.0 , -97 / 128.0 
, -96 / 128.0 , -95 / 128.0 , -94 / 128.0 , -93 / 128.0 , -92 / 128.0 , -91 / 128.0 , -90 / 128.0 , -89 / 128.0 , -88 / 128.0 , -87 / 128.0 , -86 / 128.0 , -85 / 128.0 , -84 / 128.0 , -83 / 128.0 , -82 / 128.0 , -81 / 128.0 
, -80 / 128.0 , -79 / 128.0 , -78 / 128.0 , -77 / 128.0 , -76 / 128.0 , -75 / 128.0 , -74 / 128.0 , -73 / 128.0 , -72 / 128.0 , -71 / 128.0 , -70 / 128.0 , -69 / 128.0 , -68 / 128.0 , -67 / 128.0 , -66 / 128.0 , -65 / 128.0 
, -64 / 128.0 , -63 / 128.0 , -62 / 128.0 , -61 / 128.0 , -60 / 128.0 , -59 / 128.0 , -58 / 128.0 , -57 / 128.0 , -56 / 128.0 , -55 / 128.0 , -54 / 128.0 , -53 / 128.0 , -52 / 128.0 , -51 / 128.0 , -50 / 128.0 , -49 / 128.0 
, -48 / 128.0 , -47 / 128.0 , -46 / 128.0 , -45 / 128.0 , -44 / 128.0 , -43 / 128.0 , -42 / 128.0 , -41 / 128.0 , -40 / 128.0 , -39 / 128.0 , -38 / 128.0 , -37 / 128.0 , -36 / 128.0 , -35 / 128.0 , -34 / 128.0 , -33 / 128.0 
, -32 / 128.0 , -31 / 128.0 , -30 / 128.0 , -29 / 128.0 , -28 / 128.0 , -27 / 128.0 , -26 / 128.0 , -25 / 128.0 , -24 / 128.0 , -23 / 128.0 , -22 / 128.0 , -21 / 128.0 , -20 / 128.0 , -19 / 128.0 , -18 / 128.0 , -17 / 128.0 
, -16 / 128.0 , -15 / 128.0 , -14 / 128.0 , -13 / 128.0 , -12 / 128.0 , -11 / 128.0 , -10 / 128.0 , -9 / 128.0 , -8 / 128.0 , -7 / 128.0 , -6 / 128.0 , -5 / 128.0 , -4 / 128.0 , -3 / 128.0 , -2 / 128.0 , -1 / 128.0 
, 0 / 128.0 , 1 / 128.0 , 2 / 128.0 , 3 / 128.0 , 4 / 128.0 , 5 / 128.0 , 6 / 128.0 , 7 / 128.0 , 8 / 128.0 , 9 / 128.0 , 10 / 128.0 , 11 / 128.0 , 12 / 128.0 , 13 / 128.0 , 14 / 128.0 , 15 / 128.0 
, 16 / 128.0 , 17 / 128.0 , 18 / 128.0 , 19 / 128.0 , 20 / 128.0 , 21 / 128.0 , 22 / 128.0 , 23 / 128.0 , 24 / 128.0 , 25 / 128.0 , 26 / 128.0 , 27 / 128.0 , 28 / 128.0 , 29 / 128.0 , 30 / 128.0 , 31 / 128.0 
, 32 / 128.0 , 33 / 128.0 , 34 / 128.0 , 35 / 128.0 , 36 / 128.0 , 37 / 128.0 , 38 / 128.0 , 39 / 128.0 , 40 / 128.0 , 41 / 128.0 , 42 / 128.0 , 43 / 128.0 , 44 / 128.0 , 45 / 128.0 , 46 / 128.0 , 47 / 128.0 
, 48 / 128.0 , 49 / 128.0 , 50 / 128.0 , 51 / 128.0 , 52 / 128.0 , 53 / 128.0 , 54 / 128.0 , 55 / 128.0 , 56 / 128.0 , 57 / 128.0 , 58 / 128.0 , 59 / 128.0 , 60 / 128.0 , 61 / 128.0 , 62 / 128.0 , 63 / 128.0 
, 64 / 128.0 , 65 / 128.0 , 66 / 128.0 , 67 / 128.0 , 68 / 128.0 , 69 / 128.0 , 70 / 128.0 , 71 / 128.0 , 72 / 128.0 , 73 / 128.0 , 74 / 128.0 , 75 / 128.0 , 76 / 128.0 , 77 / 128.0 , 78 / 128.0 , 79 / 128.0 
, 80 / 128.0 , 81 / 128.0 , 82 / 128.0 , 83 / 128.0 , 84 / 128.0 , 85 / 128.0 , 86 / 128.0 , 87 / 128.0 , 88 / 128.0 , 89 / 128.0 , 90 / 128.0 , 91 / 128.0 , 92 / 128.0 , 93 / 128.0 , 94 / 128.0 , 95 / 128.0 
, 96 / 128.0 , 97 / 128.0 , 98 / 128.0 , 99 / 128.0 , 100 / 128.0 , 101 / 128.0 , 102 / 128.0 , 103 / 128.0 , 104 / 128.0 , 105 / 128.0 , 106 / 128.0 , 107 / 128.0 , 108 / 128.0 , 109 / 128.0 , 110 / 128.0 , 111 / 128.0 
, 112 / 128.0 , 113 / 128.0 , 114 / 128.0 , 115 / 128.0 , 116 / 128.0 , 117 / 128.0 , 118 / 128.0 , 119 / 128.0 , 120 / 128.0 , 121 / 128.0 , 122 / 128.0 , 123 / 128.0 , 124 / 128.0 , 125 / 128.0 , 126 / 128.0 , 127 / 128.0 };

	class elementReader {
protected:
	FILE		*theFile;
	xmlDescriptor	*fd;
	float		scaler;
public:
		elementReader	(FILE *theFile, xmlDescriptor *fd) {
	this	-> theFile	= theFile;
	this	-> fd		= fd;
	int 	a		= fd -> bitsperChannel;
	int	r		= 1;
	while (-- a > 0) 
	   r <<= 1;
	scaler			= float (r);
}

virtual		~elementReader	() {}
virtual	float	readElement	() { return 0; }
};

	class	int8_reader: public elementReader {
public:
	int8_reader	(FILE *theFile, xmlDescriptor *fs):
	                          elementReader (theFile, fd) {
}

	~int8_reader	() {
}

float	readElement	() {
uint8_t	s1;
	fread (&s1, 1, 1, theFile);
	return (float)((int8_t)s1) / 127.0;
}
};

	class uint8_reader : public elementReader {
public:
	uint8_reader	(FILE *theFile, xmlDescriptor *fd):
	                       elementReader (theFile, fd){
}
	~uint8_reader	() {
}

float	readElement	() {
uint8_t s1;

	fread (&s1, 1, 1, theFile);
	return uint8_Table [s1];
}
};

	class int16_reader : public elementReader {
public:
	int16_reader	(FILE *theFile, xmlDescriptor *fd):
	                       elementReader (theFile, fd) {
}

	~int16_reader	() {
}

float	readElement	() {
uint8_t	bytes_16 [2];
int16_t	temp_16;

	if (fd -> byteOrder == "MSB") {
	   fread (bytes_16, 2, 1, theFile);
	      temp_16 = (bytes_16 [0] << 8) | bytes_16 [1];
	      return ((float)temp_16) / scaler;
	}
	else {
	   fread (&temp_16, 2, 1, theFile);
	   return ((float)temp_16) / scaler;
	}
}

};

	class int24_reader : public elementReader {
public:
	int24_reader (FILE *theFile, xmlDescriptor *fd):
	                   elementReader (theFile, fd) {
}

	~int24_reader () {}

float	readElement	() {
uint8_t	bytes_24 [3];
uint32_t temp_32;

	fread (bytes_24, 3, 1, theFile);
           if (fd -> byteOrder == "MSB")
              temp_32 = (bytes_24 [0]<< 16) |
                            (bytes_24 [1] << 8) | bytes_24 [2];
           else
              temp_32 = (bytes_24 [2]<< 16) |
                           (bytes_24 [1] << 8) | bytes_24 [0];
           if (temp_32 & 0x800000)
              temp_32 |= 0xFF000000;
           return (float)temp_32 / scaler;
}

};

	class int32_reader : public elementReader {
public:
	int32_reader (FILE *theFile, xmlDescriptor *fd) :
	                     elementReader (theFile, fd) {
}

	~int32_reader	() {}

float	readElement	() {
uint8_t	bytes_32 [4];
uint32_t temp_32;

	fread (bytes_32, sizeof (int32_t), 1, theFile);
	if (fd -> byteOrder == "MSB")
	   temp_32 = (bytes_32 [0]<< 24) | (bytes_32 [1] << 16) |
	             (bytes_32 [2] << 8) | bytes_32 [3];
	else
	   temp_32 = (bytes_32 [3] << 24) | (bytes_32 [2] << 16) |
	             (bytes_32 [1] << 8) | bytes_32 [0];
	return (float)temp_32 / scaler;
}
};

	class float_reader : public elementReader {
public:
	float_reader (FILE *theFile, xmlDescriptor *fd):
	                   elementReader (theFile, fd) {
}

	~float_reader	() {}

float	readElement	() {
uint8_t bytes_32 [4];
uint32_t temp_32;

	fread (bytes_32, sizeof (float), 1, theFile);
	if (fd -> byteOrder == "MSB")
	   temp_32 = (bytes_32 [0]<< 24) | (bytes_32 [1] << 16) |
	             (bytes_32 [2] << 8) | bytes_32 [3];
	else
	   temp_32 = (bytes_32 [3] << 24) | (bytes_32 [2] << 16) |
	             (bytes_32 [1] << 8) | bytes_32 [0];
	return *(float*)(&temp_32);
}
};


