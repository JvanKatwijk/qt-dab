#
/*
 *    Copyright (C) 2016 .. 2025
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
 */
//
#pragma once

#include <cinttypes>

const char * getASCTy (int16_t ASCTy);
const char * getDSCTy (int16_t DSCTy);
const char * getLanguage (int16_t language);
const char * getCountry	(uint8_t ecc, uint8_t countryId);
//const char * getProgramType_Not_NorthAmerica(int16_t programType);
const char * getProgramType (int16_t programType);
const char * getProgramType_For_NorthAmerica(int16_t programType);
//const char * getProgramType(bool, uint8_t interTabId, int16_t programType);
const char * getUserApplicationType(int16_t appType);
const char * getFECscheme(int16_t FEC_scheme);
const char * getProtectionLevel (bool shortForm, int16_t protLevel);
const char * getCodeRate (bool shortForm, int16_t protLevel);

