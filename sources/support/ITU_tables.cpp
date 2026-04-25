#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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

#include	"ITU_tables.h"


//      ETSI TS 101 756 V2.2.1: Registered Tables
//      for Extended Country Code, Country Id, Language Code,
//      Program Type, User Application Type, Content Type
//      https://www.etsi.org/deliver/etsi_ts/101700_101799/101756/02.02.01_60/ts_101756v020201p.pdf

//      ETSI TR 101 496-3 V1.1.2
//      protection level/classes, ..
//      https://www.etsi.org/deliver/etsi_tr/101400_101499/10149603/01.01.02_60/tr_10149603v010102p.pdf

static
const char *uep_rates [] = {"7/20", "2/5", "1/2", "3/5", "3/4"};
// see ETSI EN 300 401 V1.3.2 (2000-09), Table 8, page 46
static
const char *eep_Arates[] = {"1/4",  "3/8", "1/2", "3/4"}; 

// see ETSI EN 300 401 V1.3.2 (2000-09), Table 9, page 46
static
const char *eep_Brates[] = {"4/9",  "4/7", "4/6", "4/5"};


// from Table 2a
QString	getASCTy (int16_t ASCTy) {
	switch (ASCTy) {
	   case 0:     return "DAB";
	   case 63:    return "DAB+";
	   default:    return "unknown";
        }
}

// from Table 2b
QString	getDSCTy (uint16_t DSCTy) {
	switch (DSCTy) {
	   case 1:     return "Traffic Message CHannel (TMC)";
	   case 2:     return "Emergency Warning System (EWS)";
	   case 3:     return "Interactive Text Transmission System (ITTS)";
	   case 4:     return "Paging";
	   case 5:     return "Transparent Data Channel (TDC)";
	   case 24:    return "MPEG-2 Transport Stream";
	   case 59:    return "Embedded IP packets";
	   case 60:    return "Multimedia Object Transfer (MOT)";
	   case 61:    return "Proprietary service";
	   default:    return "unknown";
	}
}

//	11-bit from HandleFIG0Extension13, see ETSI TS 101 756 table 16
QString	getUserApplicationType (int16_t appType) {
	switch (appType) {
	   case 1:     return "Dynamic labels (X-PAD only)";
	   case 2:     return "MOT Slide Show";
	   case 3:     return "MOT Broadcast Web Site";
	   case 4:     return "TPEG";
	   case 5:     return "DGPS";
	   case 6:     return "TMC";
	   case 7:     return "SPI, was EPG";
	   case 8:     return "DAB Java";
	   case 9:     return "DMB";
	   case 0x00a: return "IPDC services";
	   case 0x00b: return "Voice applications";
	   case 0x00c: return "Middleware";
	   case 0x00d: return "Filecasting";
	   case 0x44a: return "Journaline";
	   default:    return "unknown";
	}
}

QString	getFECscheme (int16_t FEC_scheme) {
	switch (FEC_scheme) {
	   case 0:     return "no FEC";
	   case 1:     return "FEC";
	   default:    return "unknown";
	}
}

QString	getProtectionLevel (bool shortForm, int16_t protLevel) {
	if (!shortForm) {
	   switch (protLevel) {
	      case 0:     return "EEP 1-A";
	      case 1:     return "EEP 2-A";
	      case 2:     return "EEP 3-A";
	      case 3:     return "EEP 4-A";
	      case 4:     return "EEP 1-B";
	      case 5:     return "EEP 2-B";
	      case 6:     return "EEP 3-B";
	      case 7:     return "EEP 4-B";
	      default:    return "EEP unknown";
	   }
	}
	else {
	   switch (protLevel) {
	      case 1:     return "UEP 1";
	      case 2:     return "UEP 2";
	      case 3:     return "UEP 3";
	      case 4:     return "UEP 4";
	      case 5:     return "UEP 5";
	      default:    return "UEP unknown";
	   }
	}
}

QString	getCodeRate (bool shortForm, int16_t protLevel) {
int h = protLevel;

	if (!shortForm)
	   return ((h & (1 << 2)) == 0) ?
	                    eep_Arates [h & 03] :
	                    eep_Brates [h & 03]; // EEP -A/-B
	else
	   return uep_rates [h - 1];     // UEP
}

typedef struct {
	uint8_t ecc;
	uint8_t countryId;
	const char * ITU_Code;
	const char * Country;
} ITU_countryCodes;

ITU_countryCodes ITU_table_1  [] = {
{0xE0, 0x09, "ALB", "Albania"},
{0xE0, 0x02, "ALG", "Algeria"},
{0xE0, 0x03, "AND", "Andorra"},
{0xE4, 0x0A, "ARM", "Armenia"},
{0xE0, 0x0A, "AUT", "Austria"},
{0xE3, 0x0B, "AZE", "Azerbaijan"},
{0xE0, 0x0B, "AZR", "Azores"},
{0xE0, 0x06, "BEL", "Belgium"},
{0xE3, 0x0F, "BLR", "Belarus"},
{0xE4, 0x0F, "BIH", "Bosnia"},
{0xE1, 0x08, "BUL", "Bulgaria"},
{0xE0, 0x0E, "CNR", "Canaries"},
{0xE3, 0x0C, "HRV", "Croatia"},
{0xE1, 0x02, "CYP", "Cyprus"},
{0xE2, 0x02, "CZE", "Czech Republic"},
{0xE1, 0x09, "DNK", "Danmark"},
{0xE0, 0x0F, "EGY", "Egypt"},
{0xE4, 0x02, "EST", "Estonia"},
{0xE1, 0x09, "DNK", "Faroer"},
{0xE1, 0x06, "FNL", "Finland"},
{0xE1, 0x0F, "F",   "France"},
{0xE4, 0x0C, "GEO", "Georgia"},
{0xE0, 0x0D, "DE",   "Germany"},
{0xE0, 0x01, "DE",   "Germany"},
{0xE1, 0x0A, "GIB", "Gibraltar"},
{0xE1, 0x01, "GRC", "Greece"},
{0xE0, 0x0B, "HNG", "Hungary"},
{0xE2, 0x0A, "ISL", "Iceland"},
{0xE1, 0x0B, "IRQ", "Iraq"},
{0xE3, 0x02, "IRL", "Ireland"},
{0xE0, 0x04, "ISR", "Israel"},
{0xE0, 0x05, "I",   "Italy"},
{0xE1, 0x05, "JOR", "Jordan"},
{0xE3, 0x0D, "KAZ", "Kazakhstan"},
{0xE4, 0x07, "---", "Kosovo"},
{0xE4, 0x03, "KGZ", "Kyrgyzstan"},
{0xE3, 0x09, "LVA", "Latvia"},
{0xE3, 0x0A, "LBN", "Lebanon"},
{0xE1, 0x0D, "LBY", "Libya"},
{0xE2, 0x09, "LIE", "Liechtenstein"},
{0xE1, 0x07, "LUX", "Luxembourg"},
{0xE4, 0x03, "MKD", "Macedonia"},
{0xE2, 0x08, "MDR", "Madeira"},
{0xE0, 0x0C, "MLT", "Malta"},
{0xE4, 0x01, "MDA", "Moldova"},
{0xE2, 0x0B, "MCO", "Monaco"},
{0xE3, 0x01, "MNE", "Montenegro"},
{0xE2, 0x01, "MRC", "Morocco"},
{0xE3, 0x08, "NL", "Netherlands"},
//{0xE3, 0x08, "HOL", "Netherlands"},
{0xE2, 0x0F, "NOR", "Norway"},
{0xE0, 0x08, "---", "Palestine"},
{0xE2, 0x03, "POL", "Poland"},
{0xE4, 0x08, "POR", "Portugal"},
{0xE1, 0x0E, "ROU", "Romania"},
{0xE0, 0x07, "RUS", "Russian Federation"},
{0xE1, 0x03, "SM ", "San Marino"},
{0xE2, 0x0D, "SRB", "Serbia"},
{0xE2, 0x05, "SVK", "Slovakia"},
{0xE4, 0x09, "SVN", "Slovenia"},
{0xE2, 0x0E, "E",   "Spain"},
{0xE3, 0x0E, "S",   "Sweden"},
{0xE1, 0x04, "SUI", "Switzerland"},
{0xE2, 0x06, "SYR", "Syria"},
{0xE3, 0x05, "TJK", "Tajikistan"},
{0xE2, 0x07, "TUN", "Tunesia"},
{0xE3, 0x03, "TUR", "Turkey"},
{0xE4, 0x0E, "TKM", "Turkmenistan"},
{0xE4, 0x06, "UKR", "Ukraine"},
{0xE1, 0x0C, "GB",   "United Kingdom"},
{0xE1, 0x0C, "G",   "United Kingdom"},
{0xE4, 0x0B, "UZB", "Uzbekistan"},
{0xE2, 0x04, "CVA", "Vatican"},
{0xF0, 0x0A, "AFG", "Afghanistan"},
{0xF0, 0x09, "ARS", "Saudi Arabia"},
{0xF0, 0x01, "AU", "Australia"},
//{0xF0, 0x01, "acc", "Australia"},
{0xF0, 0x02, "arn", "Australia"},
{0xF0, 0x03, "acn", "Australia"},
{0xF0, 0x04, "arq", "Australia"},
{0xF0, 0x05, "arc", "Australia"},
{0xF0, 0x06, "arw", "Australia"},
{0xF0, 0x06, "arc", "Australia"},
{0, 0, "", ""}
};

QString	getCountry (int tableNo, uint8_t ecc, uint8_t countryId) {
	(void)tableNo;
	for (int i = 0; ITU_table_1 [i]. ecc != 0; i ++) {
           if ((ITU_table_1 [i]. ecc == ecc) &&
                   (ITU_table_1 [i]. countryId == countryId) )
           return ITU_table_1 [i]. Country; 
        }

	return "unknown";
}

typedef struct {
	int language;
	QString languageName;
} ITU_languages;

ITU_languages ITU_table_9_10 [] = {
	   {0x00,  "Unknown/na"}, {0x01, "Albanian"}, {0x02, "Breton"},
	   {0x03, "Catalan"}, {0x04, "Croatian"}, {0x05, "Welsh"},
	   {0x06, "Czech"}, {0x07, "Danish"}, {0x08, "German"},
	   {0x09, "English"}, {0x0A, "Spanish"}, {0x0B, "Esperanto"},
	   {0x0C, "Estonian"}, {0x0D, "Basque"}, {0x0E, "Faroese"},
	   {0x0F, "French"}, {0x10, "Frisian"}, {0x11, "Irish"},
	   {0x12, "Gaelic"}, {0x13, "Galician"}, {0x14, "Icelandic"},
	   {0x15, "Italian"}, {0x16, "Sami"}, {0x17, "Latin"},
	   {0x18, "Latvian"}, {0x19, "Luxembourgian"}, {0x1A, "Lithuanian"},
	   {0x1B, "Hungarian"}, {0x1C, "Maltese"}, {0x1D, "Dutch"},
	   {0x1E, "Norwegian"}, {0x1F, "Occitan"}, {0x20, "Polish"},
	   {0x21, "Portuguese"}, {0x22, "Romanian"}, {0x23, "Romansh"},
	   {0x24, "Serbian"}, {0x25, "Slovak"}, {0x26, "Slovene"},
	   {0x27, "Finnish"}, {0x28, "Swedish"}, {0x29, "Turkish"},
	   {0x2A, "Flemish"}, {0x2B, "Walloon"}, {0x7F, "Amharic"},
	   {0x7E, "Arabic"}, {0x7D, "Armenian"},
	   {0x7C, "Assamese"}, {0x7B, "Azerbaijani"}, {0x7A, "Bambora"},
	   {0x79, "Belorussian"}, {0x78, "Bengali"}, {0x77, "Bulgarian"},
	   {0x76, "Burmese"}, {0x75, "Chinese"}, {0x74, "Chuvash"},
	   {0x73, "Dari"}, {0x72, "Fulani"}, {0x71, "Georgian"},
	   {0x70, "Greek"}, {0x6F, "Gujurati"}, {0x6E, "Gurani"},
	   {0x6D, "Hausa"}, {0x6C, "Hebrew"}, {0x6B, "Hindi"},
	   {0x6A, "Indonesian"}, {0x69, "Japanese"}, {0x68, "Kannada"},
	   {0x67, "Kazakh"}, {0x66, "Khmer"}, {0x65, "Korean"},
	   {0x64, "Laotian"}, {0x63, "Macedonian"}, {0x62, "Malagasay"},
	   {0x61, "Malaysian"}, {0x60, "Moldavian"}, {0x5F, "Marathi"},
	   {0x5E, "Ndebele"}, {0x5D, "Nepali"}, {0x5C, "Oriya"},
	   {0x5B, "Papiamento"}, {0x5A, "Persian"}, {0x59, "Punjabi"},
	   {0x58, "Pushtu"}, {0x57, "Quechua"}, {0x56, "Russian"},
	   {0x55, "Rusyn"}, {0x54, "Serbo-Croat"}, {0x53, "Shona"},
	   {0x52, "Sinhalese"}, {0x51, "Somali"}, {0x50, "Sranan Tongo"},
	   {0x4F, "Swahili"}, {0x4E, "Tadzhik"}, {0x4D, "Tamil"},
	   {0x4C, "Tatar"}, {0x4B, "Telugu"}, {0x4A, "Thai"},
	   {0x49, "Ukranian"}, {0x48, "Urdu"}, {0x47, "Uzbek"},
	   {0x46, "Vietnamese"}, {0x45, "Zulu"},
	   {0x40, "Background sound/clean feed"},
	   {0x0, "no language"}
};

QString	getLanguage	(int table, uint16_t languageKey) {
	(void)table;
	for (int i = 0; ITU_table_9_10 [i].language != 0; i ++)
	   if (ITU_table_9_10 [i]. language == languageKey)
	      return ITU_table_9_10 [i]. languageName;
	return "unknown";
}

//	from Table 12 and 13

typedef	struct {
	int progType;
	QString	typeName;
} ITU_progType;


ITU_progType ITU_table_12 [] = {
	   { 0,  "No programme type"},
	   { 1,  "News"},
	   { 2,  "Current Affairs"},
	   { 3,  "Information"},
	   { 4,  "Sport"},
	   { 5,  "Education"},
	   { 6,  "Drama"},
	   { 7,  "Culture"},
	   { 8,  "Science"},
	   { 9,  "Varied"},    //Talk
	   { 10, "Pop Music"},
	   { 11, "Rock Music"},
	   { 12, "Easy Listening Music"},
	   { 13, "Light Classical"},
	   { 14, "Serious Classical"},
	   { 15, "Other Music"},
	   { 16, "Weather/meteorology"},
	   { 17, "Finance/Business"},
	   { 18, "Children's programmes"},
	   { 19, "Social Affairs"},    //Factual
	   { 20, "Religion"},
	   { 21, "Phone In"},
	   { 22, "Travel"},
	   { 23, "Leisure"},
	   { 24, "Jazz Music"},
	   { 25, "Country Music"},
	   { 26, "National Music"},
	   { 27, "Oldies Music"},
	   { 28, "Folk Music"},
	   { 29, "Documentary"},
	   { 30, "unknown programme type 30"},
	   { 31, "unknown programme type 31"},
	   {  0, "illegal"}
};

//	from Table 13
ITU_progType ITU_table_13 [] = {
	   {0 , "No programme type"},
	   {1 , "News"},
	   {2 , "Information"},
	   {3 , "Sports"},
	   {4 , "Talk"},
	   {5 , "Rock"},
	   {6 , "Classic Rock"},
	   {7 , "Adult Hits"},
	   {8 , "Soft Rock"},
	   {9 , "Top 40"},
	   {10, "Country"},
	   {11, "Oldies"},
	   {12, "Soft"},
	   {13, "Nostalgia"},
	   {14, "Jazz"},
	   {15, "Classical"},
	   {16, "Rhythm and Blues"},
	   {17, "Soft Rhythm and Blues"},
	   {18, "Foreign Language"},
	   {19, "Religious Music"},
	   {20, "Religious Talk"},
	   {21, "Personality"},
	   {22, "Public"},
	   {23, "College"},
	   {24, "unknown programme type 24"},
	   {25, "unknown programme type 25"},
	   {26, "unknown programme type 26"},
	   {27, "unknown programme type 27"},
	   {28, "unknown programme type 28"},
	   {29, "Weather"},
	   {30, "unknown programme type 30"},
	   {31, "unknown programme type 31"},
	   {0,  "illegal"},
};

QString	getProgramType	(int tableId, uint16_t programType) {
ITU_progType *theTable;
	theTable	= tableId == 2 ? ITU_table_13 : ITU_table_12;

	for (int i = 0; theTable [i]. progType != 0; i ++)
	   if (theTable [i]. progType == programType)
	      return theTable [i]. typeName;
	return "";
}

