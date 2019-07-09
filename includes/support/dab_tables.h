#pragma once

#include <cinttypes>

const char * getASCTy (int16_t ASCTy);
const char * getDSCTy (int16_t DSCTy);
const char * getLanguage (int16_t language);
const char * getCountry	(uint8_t ecc, uint8_t countryId);
const char * getProgramType_Not_NorthAmerica(int16_t programType);
const char * getProgramType_For_NorthAmerica(int16_t programType);
const char * getProgramType(bool, uint8_t interTabId, int16_t programType);
const char * getUserApplicationType(int16_t appType);
const char * getFECscheme(int16_t FEC_scheme);
const char * getProtectionLevel (bool shortForm, int16_t protLevel);
const char * getCodeRate (bool shortForm, int16_t protLevel);

