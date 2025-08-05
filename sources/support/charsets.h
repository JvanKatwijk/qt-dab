#
/*
 *    Copyright (C) 2016 .. 2026
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
 *	This charset handling was kindly added by Przemyslaw Wegrzyn	
 *	all rights acknowledged
 */
#pragma once

#include <QString>

/*
 * Codes assigned to character sets, as defined
 * in ETSI TS 101 756 v1.6.1, section 5.2.
 */
typedef enum {
    EbuLatin = 0x00, // Complete EBU Latin based repertoire - see annex C
    UnicodeUcs2 = 0x06,
    UnicodeUtf8 = 0x0F
} CharacterSet;

/**
 * Converts the null-terminated character string to QString, using a given character set.
 *
 * @param buffer    null-terminated buffer to convert
 * @param charset   character set used in buffer
 * @return converted QString
 */
QString toQStringUsingCharset(const char* buffer, CharacterSet charset, int size = -1);


