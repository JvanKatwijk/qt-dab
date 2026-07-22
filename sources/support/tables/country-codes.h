#
/*
 *    Copyright (C) 2016 .. 2025
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
#pragma once

#include	<cstdint>

struct country_codes {
	uint8_t ecc;
	uint8_t countryId;
	const char *countryName;
} countryTable [] = {
{0xE0, 0x9, "Albania"},
{0xE0, 0x2, "Algeria"},
{0xE0, 0x3, "Andorra"},
{0xE0, 0xA, "Austria"},
{0xE4, 0x8, "Azores"},
{0xE0, 0x6, "Belgium"},
{0xE3, 0xF, "Belarus"},
{0xE4, 0xF, "Bosnia"},
{0xE0, 0x9, "Albania"},
{0xE1, 0x8, "Bulgaria"},
{0xE2, 0xE, "Canaries"},
{0xE3, 0xC, "Croatia"},
{0xE1, 0x2, "Cyprus"},
{0xE2, 0x2, "Czech Republic"},
{0xE1, 0x9, "Denmark"},
{0xE0, 0xF, "Egypt"},
{0xE4, 0x2, "Estonia"},
{0xE1, 0x9, "Faroe"},
{0xE1, 0x6, "Finland"},
{0xE1, 0xF, "France"},
{0xE0, 0xD, "Germany"},
{0xE0, 0x1, "Germany"},
{0xE1, 0xA, "Gibraltar"},
{0xE1, 0x1, "Greece"},
{0xE0, 0xB, "Hungary"},
{0xE2, 0xA, "Iceland"},
{0xE1, 0xB, "Iraq"},
{0xE3, 0x2, "Ireland"},
{0xE0, 0x4, "Israel"},
{0xE0, 0x5, "Italy"},
{0xE1, 0x5, "Jordan"},
{0xE3, 0x9, "Latvia"},
{0xE3, 0xA, "Lebanon"},
{0xE1, 0xD, "Libya"},
{0xE2, 0x9, "Liechtenstein"},
{0xE2, 0xC, "Lithuania"},
{0xE1, 0x7, "Luxembourg"},
{0xE3, 0x4, "Macedonia"},
{0xE4, 0x8, "Madeira"},
{0xE0, 0xC, "Malta"},
{0xE2, 0x1, "Morocco"},
{0xE4, 0x1, "Moldova"},
{0xE2, 0xB, "Monaco"},
{0xE3, 0x1, "Montenegro"},
{0xE3, 0x8, "Netherlands"},
{0xE2, 0xF, "Norway"},
{0xE2, 0x3, "Poland"},
{0xE4, 0x8, "Portugal"},
{0xE1, 0xE, "Romania"},
{0xE0, 0x7, "Russian Federation"},
{0xE1, 0x3, "San Marino"},
{0xE2, 0xD, "Serbia"},
{0xE4, 0x9, "Slovenia"},
{0xE2, 0x5, "Slovak Republic"},
{0xE2, 0xE, "Spain"},
{0xE3, 0xE, "Sweden"},
{0xE1, 0x4, "Switzerland"},
{0xE2, 0x6, "Syria"},
{0xE2, 0x7, "Tunisia"},
{0xE3, 0x3, "Turkey"},
{0xE4, 0x6, "Ukraine"},
{0xE1, 0xC, "United Kingdom"},
{0xE2, 0x4, "Vatican"},
{0x00, 0x0, " "}
};

