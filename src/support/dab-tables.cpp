#
/*
 *    Copyright (C) 2018
 *    Hayati Ayguen (h_ayguen@web.de)
 *
 *    DAB-library is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DAB-library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DAB-library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "dab-tables.h"

#include <cstring>
#include <cstdlib>


//	ETSI TS 101 756 V2.2.1: Registered Tables
//	for Extended Country Code, Country Id, Language Code,
//	Program Type, User Application Type, Content Type
//	https://www.etsi.org/deliver/etsi_ts/101700_101799/101756/02.02.01_60/ts_101756v020201p.pdf

//	ETSI TR 101 496-3 V1.1.2
//	protection level/classes, ..
//	https://www.etsi.org/deliver/etsi_tr/101400_101499/10149603/01.01.02_60/tr_10149603v010102p.pdf

static const char *uep_rates [] = {"7/20", "2/5", "1/2", "3/5", "3/4"};
static const char *eep_Arates[] = {"1/4",  "3/8", "1/2", "3/4"};  // see ETSI EN 300 401 V1.3.2 (2000-09), Table 8, page 46
static const char *eep_Brates[] = {"4/9",  "4/7", "4/6", "4/5"};  // see ETSI EN 300 401 V1.3.2 (2000-09), Table 9, page 46


struct country_codes {
    uint8_t ecc;
    uint8_t countryId;
    const char *countryName;
};

//	following countryTable[] is sorted over all regions to make
//	it easier to find "missing" / "old" but still unused entries.
//	achieved by searching old standard tables document for each ECC code.
//	Table 3: ITU Region 1 - European broadcasting area
//	Table 4: ITU Region 1 - African broadcasting area
//	Table 6: ITU Region 2 - North and South Americas
//	Table 7: ITU Region 3 - Asia and Pacific

static country_codes countryTable[] = {
{0xA0, 0x1, "United States of America"},
{0xA0, 0x2, "United States of America"},
{0xA0, 0x3, "United States of America"},
{0xA0, 0x4, "United States of America"},
{0xA0, 0x5, "United States of America"},
{0xA0, 0x6, "United States of America"},
{0xA0, 0x7, "United States of America"},
{0xA0, 0x8, "United States of America"},
{0xA0, 0x9, "United States of America"},
{0xA0, 0xA, "United States of America"},
{0xA0, 0xB, "United States of America"},
{0xA0, 0xC, "United States of America"},
{0xA0, 0xD, "United States of America"},
{0xA0, 0xE, "United States of America"},

{0xA1, 0xC, "Canada"},
{0xA1, 0xF, "Greenland"},

{0xA2, 0x1, "Anguilla"},
{0xA2, 0x2, "Antigua and Barbuda"},
{0xA2, 0x3, "Equador"},
{0xA2, 0x4, "Falkland Islands"},
{0xA2, 0x5, "Barbados"},
{0xA2, 0x6, "Belize"},
{0xA2, 0x7, "Cayman Islands"},
{0xA2, 0x8, "Costa Rica"},
{0xA2, 0x9, "Cuba"},
{0xA2, 0xA, "Argentina"},
{0xA2, 0xB, "Brazil"},
{0xA2, 0xC, "Bermuda"},
{0xA2, 0xD, "Netherlands Antilles"},
{0xA2, 0xE, "Guadeloupe"},
{0xA2, 0xF, "Bahamas"},

{0xA3, 0x1, "Bolivia"},
{0xA3, 0x2, "Colombia"},
{0xA3, 0x3, "Jamaica"},
{0xA3, 0x4, "Martinique"},
{0xA3, 0x6, "Paraguay"},
{0xA3, 0x7, "Nicaragua"},
{0xA3, 0x8, "Puerto Rico"},
{0xA3, 0x9, "Panama"},
{0xA3, 0xA, "Dominica"},
{0xA3, 0xB, "Dominican Republic"},
{0xA3, 0xC, "Chile"},
{0xA3, 0xD, "Grenada"},
{0xA3, 0xE, "Turks and Caicos islands"},
{0xA3, 0xF, "Guyana"},

{0xA4, 0x1, "Guatemala"},
{0xA4, 0x2, "Honduras"},
{0xA4, 0x3, "Aruba"},
{0xA4, 0x5, "Montserrat"},
{0xA4, 0x6, "Trinidad and Tobago"},
{0xA4, 0x7, "Peru"},
{0xA4, 0x8, "Surinam"},
{0xA4, 0x9, "Uruguay"},
{0xA4, 0xA, "St. Kitts"},
{0xA4, 0xB, "St. Lucia"},
{0xA4, 0xC, "El Salvador"},
{0xA4, 0xD, "Haiti"},
{0xA4, 0xE, "Venezuela"},
{0xA4, 0xF, "Mexico"},

{0xA5, 0xC, "St. Vincent"},
{0xA5, 0xF, "Virgin islands (British or USA)"}, // British and USA have same IDs! amended "or USA"
{0xA5, 0xF, "Virgin islands (USA)"},            // keep this entry .. doesn't hurt

{0xA6, 0xF, "St. Pierre and Miquelon"},

{0xD0, 0x1, "Cameroon"},
{0xD0, 0x2, "Central African Republic"},
{0xD0, 0x3, "Djibouti"},
{0xD0, 0x4, "Madagascar"},
{0xD0, 0x5, "Mali"},
{0xD0, 0x6, "Angola"},
{0xD0, 0x7, "Equatorial Guinea"},
{0xD0, 0x8, "Gabon"},
{0xD0, 0x9, "Republic of Guinea"},
{0xD0, 0xA, "South Africa"},
{0xD0, 0xB, "Burkina Faso"},
{0xD0, 0xC, "Congo"},
{0xD0, 0xD, "Togo"},
{0xD0, 0xE, "Benin"},
{0xD0, 0xF, "Malawi"},

{0xD1, 0x1, "Namibia"},
{0xD1, 0x2, "Liberia"},
{0xD1, 0x3, "Ghana"},
{0xD1, 0x4, "Mauritania"},
{0xD1, 0x5, "Sao Tome & Principe"},
{0xD1, 0x6, "Cape Verde"},
{0xD1, 0x7, "Senegal"},
{0xD1, 0x8, "Gambia"},
{0xD1, 0x9, "Burundi"},
{0xD1, 0xA, "Ascension island"},
{0xD1, 0xB, "Botswana"},
{0xD1, 0xC, "Comoros"},
{0xD1, 0xD, "Tanzania"},
{0xD1, 0xE, "Ethiopia"},
{0xD1, 0xF, "Nigeria"},

{0xD2, 0x1, "Sierra Leone"},
{0xD2, 0x2, "Zimbabwe"},
{0xD2, 0x3, "Mozambique"},
{0xD2, 0x4, "Uganda"},
{0xD2, 0x5, "Swaziland"},
{0xD2, 0x6, "Kenya"},
{0xD2, 0x7, "Somalia"},
{0xD2, 0x8, "Niger"},
{0xD2, 0x9, "Chad"},
{0xD2, 0xA, "Guinea-Bissau"},
{0xD2, 0xB, "Zaire"},
{0xD2, 0xC, "Cote d'Ivoire"},
{0xD2, 0xD, "Zanzibar"},
{0xD2, 0xE, "Zambia"},

{0xD3, 0x3, "Western Sahara"},
{0xD3, 0x4, "Cabinda"},             // was in V1.4.1. was removed later. but IDs are unused .. so keep it here
{0xD3, 0x5, "Rwanda"},
{0xD3, 0x6, "Lesotho"},
{0xD3, 0x8, "Seychelles"},
{0xD3, 0xA, "Mauritius"},
{0xD3, 0xC, "Sudan"},

{0xE0, 0x1, "Germany"},
{0xE0, 0x2, "Algeria"},
{0xE0, 0x3, "Andorra"},
{0xE0, 0x4, "Israel"},
{0xE0, 0x5, "Italy"},
{0xE0, 0x6, "Belgium"},
{0xE0, 0x7, "Russian Federation"},
{0xE0, 0x8, "Azores (Portugal)"},   // was in V1.4.1. was removed later. but IDs are unused .. so keep it here
{0xE0, 0x9, "Albania"},
{0xE0, 0xA, "Austria"},
{0xE0, 0xB, "Hungary"},
{0xE0, 0xC, "Malta"},
{0xE0, 0xD, "Germany"},
{0xE0, 0xE, "Canary Islands (Spain)"},  // amended "(Spain)"
{0xE0, 0xF, "Egypt"},

{0xE1, 0x1, "Greece"},
{0xE1, 0x2, "Cyprus"},
{0xE1, 0x3, "San Marino"},
{0xE1, 0x4, "Switzerland"},
{0xE1, 0x5, "Jordan"},
{0xE1, 0x6, "Finland"},
{0xE1, 0x7, "Luxembourg"},
{0xE1, 0x8, "Bulgaria"},
{0xE1, 0x9, "Faroe (Denmark)"},
{0xE1, 0xA, "Gibraltar (UK)"},      // amended "(UK)"
{0xE1, 0xB, "Iraq"},
{0xE1, 0xC, "United Kingdom"},
{0xE1, 0xD, "Libya"},
{0xE1, 0xE, "Romania"},
{0xE1, 0xF, "France"},

{0xE2, 0x1, "Marocco"},
{0xE2, 0x2, "Czech Republic"},
{0xE2, 0x3, "Poland"},
{0xE2, 0x4, "Vatican"},
{0xE2, 0x5, "Slovak Republic"},
{0xE2, 0x6, "Syria"},
{0xE2, 0x7, "Tunisia"},
{0xE2, 0x8, "Madeira"},             // was in V1.4.1. was removed later. but IDs are unused .. so keep it here
{0xE2, 0x9, "Liechtenstein"},
{0xE2, 0xA, "Iceland"},
{0xE2, 0xB, "Monaco"},
{0xE2, 0xC, "Lithuania"},
{0xE2, 0xD, "Serbia"},
{0xE2, 0xE, "Spain"},
{0xE2, 0xF, "Norway"},

{0xE3, 0x1, "Montenegro"},
{0xE3, 0x2, "Ireland"},
{0xE3, 0x3, "Turkey"},
{0xE3, 0x4, "Macedonia"},
{0xE3, 0x5, "Tajikistan"},          // was in V1.4.1. was removed later. but IDs are unused .. so keep it here
{0xE3, 0x8, "Netherlands"},
{0xE3, 0x9, "Latvia"},
{0xE3, 0xA, "Lebanon"},
{0xE3, 0xB, "Azerbaijan"},          // was in V1.4.1. was removed later. but IDs are unused .. so keep it here
{0xE3, 0xC, "Croatia"},
{0xE3, 0xD, "Kazakhstan"},          // was in V1.4.1. was removed later. but IDs are unused .. so keep it here
{0xE3, 0xE, "Sweden"},
{0xE3, 0xF, "Belarus"},

{0xE4, 0x1, "Moldova"},
{0xE4, 0x2, "Estonia"},
{0xE4, 0x3, "Macedonia / Kyrghyzstan"}, // amended "/ Kyrghyzstan" double used Id
{0xE4, 0x3, "Kyrghyzstan"},         // keep this entry .. doesn't hurt
{0xE4, 0x6, "Ukraine"},
{0xE4, 0x7, "Kosovo"},
{0xE4, 0x8, "Portugal"},
{0xE4, 0x9, "Slovenia"},
{0xE4, 0xA, "Armenia"},
{0xE4, 0xB, "Uzbekistan"},
{0xE4, 0xC, "Georgia"},
{0xE4, 0xE, "Turkmenistan"},
{0xE4, 0xF, "Bosnia Herzegovina"},

{0xF0, 0x1, "Australia: Capital Cities (commercial and community)"},
{0xF0, 0x2, "Australia: New South Wales and ACT"},
{0xF0, 0x3, "Australia: Capital Cities (national broadcasters, was Victoria)"},
{0xF0, 0x4, "Australia: Queensland"},
{0xF0, 0x5, "Australia: South and Northern Territory"},
{0xF0, 0x6, "Australia: Western"},
{0xF0, 0x7, "Australia: Victoria and Tasmania"},
{0xF0, 0x8, "Australia: (future, was Northern Territory)"},
{0xF0, 0x9, "Saudi Arabia"},
{0xF0, 0xA, "Afghanistan"},
{0xF0, 0xB, "Myanmar (Burma)"},
{0xF0, 0xC, "China"},
{0xF0, 0xD, "Korea (North)"},
{0xF0, 0xE, "Bahrain"},
{0xF0, 0xF, "Malaysia"},

{0xF1, 0x1, "Kiribati"},
{0xF1, 0x2, "Bhutan"},
{0xF1, 0x3, "Bangladesh"},
{0xF1, 0x4, "Pakistan"},
{0xF1, 0x5, "Fiji"},
{0xF1, 0x6, "Oman"},
{0xF1, 0x7, "Nauru"},
{0xF1, 0x8, "Iran"},
{0xF1, 0x9, "New Zealand"},
{0xF1, 0xA, "Solomon Islands"},
{0xF1, 0xB, "Brunei Darussalam"},
{0xF1, 0xC, "Sri Lanka"},
{0xF1, 0xD, "Taiwan"},
{0xF1, 0xE, "Korea (South)"},
{0xF1, 0xF, "Hong Kong"},

{0xF2, 0x1, "Kuwait"},
{0xF2, 0x2, "Qatar"},
{0xF2, 0x3, "Cambodia"},
{0xF2, 0x4, "Western Samoa"},
{0xF2, 0x5, "India"},
{0xF2, 0x6, "Macau"},
{0xF2, 0x7, "Vietnam"},
{0xF2, 0x8, "Philippines"},
{0xF2, 0x9, "Japan"},
{0xF2, 0xA, "Singapore"},
{0xF2, 0xB, "Maldives"},
{0xF2, 0xC, "Indonesia"},
{0xF2, 0xD, "United Arab Emirates"},
{0xF2, 0xE, "Nepal"},
{0xF2, 0xF, "Vanuatu"},

{0xF3, 0x1, "Laos"},
{0xF3, 0x2, "Thailand"},
{0xF3, 0x3, "Tonga"},
{0xF3, 0x9, "Papua New Guinea"},
{0xF3, 0xB, "Yemen"},
{0xF3, 0xE, "Micronesia"},
{0xF3, 0xF, "Mongolia"},

{0x00, 0x0, nullptr }
};

// from Table 2a
const char * getASCTy (int16_t ASCTy) {
	switch (ASCTy) {
	   case 0:     return "DAB";
	   case 63:    return "DAB+";
	   default:    return "unknown";
	}
}

// from Table 2b
const char * getDSCTy (int16_t DSCTy) {
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

//	from Table 9 and 10
const char * getLanguage (int16_t language) {
	switch (language) {
	   case 0x00:  return "Unknown/na";
	   case 0x01:  return "Albanian";
	   case 0x02:  return "Breton";
	   case 0x03:  return "Catalan";
	   case 0x04:  return "Croatian";
	   case 0x05:  return "Welsh";
	   case 0x06:  return "Czech";
	   case 0x07:  return "Danish";
	   case 0x08:  return "German";
	   case 0x09:  return "English";
	   case 0x0A:  return "Spanish";
	   case 0x0B:  return "Esperanto";
	   case 0x0C:  return "Estonian";
	   case 0x0D:  return "Basque";
	   case 0x0E:  return "Faroese";
	   case 0x0F:  return "French";
	   case 0x10:  return "Frisian";
	   case 0x11:  return "Irish";
	   case 0x12:  return "Gaelic";
	   case 0x13:  return "Galician";
	   case 0x14:  return "Icelandic";
	   case 0x15:  return "Italian";
	   case 0x16:  return "Sami";
	   case 0x17:  return "Latin";
	   case 0x18:  return "Latvian";
	   case 0x19:  return "Luxembourgian";
	   case 0x1A:  return "Lithuanian";
	   case 0x1B:  return "Hungarian";
	   case 0x1C:  return "Maltese";
	   case 0x1D:  return "Dutch";
	   case 0x1E:  return "Norwegian";
	   case 0x1F:  return "Occitan";
	   case 0x20:  return "Polish";
	   case 0x21:  return "Portuguese";
	   case 0x22:  return "Romanian";
	   case 0x23:  return "Romansh";
	   case 0x24:  return "Serbian";
	   case 0x25:  return "Slovak";
	   case 0x26:  return "Slovene";
	   case 0x27:  return "Finnish";
	   case 0x28:  return "Swedish";
	   case 0x29:  return "Turkish";
	   case 0x2A:  return "Flemish";
	   case 0x2B:  return "Walloon";
	   case 0x30:  // no break
	   case 0x31:  // no break
	   case 0x32:  // no break
	   case 0x33:  // no break
	   case 0x34:  // no break
	   case 0x35:  // no break
	   case 0x36:  // no break
	   case 0x37:  // no break
	   case 0x38:  // no break
	   case 0x39:  // no break
	   case 0x3A:  // no break
	   case 0x3B:  // no break
	   case 0x3C:  // no break
	   case 0x3D:  // no break
	   case 0x3E:  // no break
	   case 0x3F:  return "Reserved for national assignment";
	   case 0x7F:  return "Amharic";
	   case 0x7E:  return "Arabic";
	   case 0x7D:  return "Armenian";
	   case 0x7C:  return "Assamese";
	   case 0x7B:  return "Azerbaijani";
	   case 0x7A:  return "Bambora";
	   case 0x79:  return "Belorussian";
	   case 0x78:  return "Bengali";
	   case 0x77:  return "Bulgarian";
	   case 0x76:  return "Burmese";
	   case 0x75:  return "Chinese";
	   case 0x74:  return "Chuvash";
	   case 0x73:  return "Dari";
	   case 0x72:  return "Fulani";
	   case 0x71:  return "Georgian";
	   case 0x70:  return "Greek";
	   case 0x6F:  return "Gujurati";
	   case 0x6E:  return "Gurani";
	   case 0x6D:  return "Hausa";
	   case 0x6C:  return "Hebrew";
	   case 0x6B:  return "Hindi";
	   case 0x6A:  return "Indonesian";
	   case 0x69:  return "Japanese";
	   case 0x68:  return "Kannada";
	   case 0x67:  return "Kazakh";
	   case 0x66:  return "Khmer";
	   case 0x65:  return "Korean";
	   case 0x64:  return "Laotian";
	   case 0x63:  return "Macedonian";
	   case 0x62:  return "Malagasay";
	   case 0x61:  return "Malaysian";
	   case 0x60:  return "Moldavian";
	   case 0x5F:  return "Marathi";
	   case 0x5E:  return "Ndebele";
	   case 0x5D:  return "Nepali";
	   case 0x5C:  return "Oriya";
	   case 0x5B:  return "Papiamento";
	   case 0x5A:  return "Persian";
	   case 0x59:  return "Punjabi";
	   case 0x58:  return "Pushtu";
	   case 0x57:  return "Quechua";
	   case 0x56:  return "Russian";
	   case 0x55:  return "Rusyn";
	   case 0x54:  return "Serbo-Croat";
	   case 0x53:  return "Shona";
	   case 0x52:  return "Sinhalese";
	   case 0x51:  return "Somali";
	   case 0x50:  return "Sranan Tongo";
	   case 0x4F:  return "Swahili";
	   case 0x4E:  return "Tadzhik";
	   case 0x4D:  return "Tamil";
	   case 0x4C:  return "Tatar";
	   case 0x4B:  return "Telugu";
	   case 0x4A:  return "Thai";
	   case 0x49:  return "Ukranian";
	   case 0x48:  return "Urdu";
	   case 0x47:  return "Uzbek";
	   case 0x46:  return "Vietnamese";
	   case 0x45:  return "Zulu";
	   case 0x40:  return "Background sound/clean feed";
	   default:    return "unknown";
	}
}

const char *getCountry (uint8_t ecc, uint8_t countryId) {
int16_t	i = 0;

	while (countryTable [i].ecc != 0) {
	   if ((countryTable[i].ecc == ecc) &&
	           (countryTable[i].countryId == countryId) )
	   return countryTable[i].countryName;
	   ++i;
	}

	return nullptr;
}

//	from Table 12
const char *getProgramType_Not_NorthAmerica (int16_t programType) {

	switch (programType) {
	   case 0:     return "No programme type";
	   case 1:     return "News";
	   case 2:     return "Current Affairs";
	   case 3:     return "Information";
	   case 4:     return "Sport";
	   case 5:     return "Education";
	   case 6:     return "Drama";
	   case 7:     return "Culture";
	   case 8:     return "Science";
	   case 9:     return "Varied";    //Talk
	   case 10:    return "Pop Music";
	   case 11:    return "Rock Music";
	   case 12:    return "Easy Listening Music";
	   case 13:    return "Light Classical";
	   case 14:    return "Serious Classical";
	   case 15:    return "Other Music";
	   case 16:    return "Weather/meteorology";
	   case 17:    return "Finance/Business";
	   case 18:    return "Children's programmes";
	   case 19:    return "Social Affairs";    //Factual
	   case 20:    return "Religion";
	   case 21:    return "Phone In";
	   case 22:    return "Travel";
	   case 23:    return "Leisure";
	   case 24:    return "Jazz Music";
	   case 25:    return "Country Music";
	   case 26:    return "National Music";
	   case 27:    return "Oldies Music";
	   case 28:    return "Folk Music";
	   case 29:    return "Documentary";
	   case 30:    return "unknown programme type 30";
	   case 31:    return "unknown programme type 31";
	   default:    return "unknown programme type";
	}
}

//	from Table 13
const char *getProgramType_For_NorthAmerica (int16_t programType) {
	switch (programType) {
	   case 0:     return "No programme type";
	   case 1:     return "News";
	   case 2:     return "Information";
	   case 3:     return "Sports";
	   case 4:     return "Talk";
	   case 5:     return "Rock";
	   case 6:     return "Classic Rock";
	   case 7:     return "Adult Hits";
	   case 8:     return "Soft Rock";
	   case 9:     return "Top 40";
	   case 10:    return "Country";
	   case 11:    return "Oldies";
	   case 12:    return "Soft";
	   case 13:    return "Nostalgia";
	   case 14:    return "Jazz";
	   case 15:    return "Classical";
	   case 16:    return "Rhythm and Blues";
	   case 17:    return "Soft Rhythm and Blues";
	   case 18:    return "Foreign Language";
	   case 19:    return "Religious Music";
	   case 20:    return "Religious Talk";
	   case 21:    return "Personality";
	   case 22:    return "Public";
	   case 23:    return "College";
	   case 24:    return "unknown programme type 24";
	   case 25:    return "unknown programme type 25";
	   case 26:    return "unknown programme type 26";
	   case 27:    return "unknown programme type 27";
	   case 28:    return "unknown programme type 28";
	   case 29:    return "Weather";
	   case 30:    return "unknown programme type 30";
	   case 31:    return "unknown programme type 31";
	   default:    return "unknown programme type";
	}
}

const char *getProgramType (bool gotInterTabId,
	                    uint8_t interTabId, int16_t programType) {
	if (gotInterTabId &&  (interTabId == 1))
	   return getProgramType_Not_NorthAmerica (programType);
	else
	if (gotInterTabId && (interTabId == 2))
	   return getProgramType_For_NorthAmerica (programType);
	else {
	   switch (programType) {
	      case 0:     return "unknown programme type 0";
	      case 1:     return "unknown programme type 1";
	      case 2:     return "unknown programme type 2";
	      case 3:     return "unknown programme type 3";
	      case 4:     return "unknown programme type 4";
	      case 5:     return "unknown programme type 5";
	      case 6:     return "unknown programme type 6";
	      case 7:     return "unknown programme type 7";
	      case 8:     return "unknown programme type 8";
	      case 9:     return "unknown programme type 9";
	      case 10:    return "unknown programme type 10";
	      case 11:    return "unknown programme type 11";
	      case 12:    return "unknown programme type 12";
	      case 13:    return "unknown programme type 13";
	      case 14:    return "unknown programme type 14";
	      case 15:    return "unknown programme type 15";
	      case 16:    return "unknown programme type 16";
	      case 17:    return "unknown programme type 17";
	      case 18:    return "unknown programme type 18";
	      case 19:    return "unknown programme type 19";
	      case 20:    return "unknown programme type 20";
	      case 21:    return "unknown programme type 21";
	      case 22:    return "unknown programme type 22";
	      case 23:    return "unknown programme type 23";
	      case 24:    return "unknown programme type 24";
	      case 25:    return "unknown programme type 25";
	      case 26:    return "unknown programme type 26";
	      case 27:    return "unknown programme type 27";
	      case 28:    return "unknown programme type 28";
	      case 29:    return "unknown programme type 29";
	      case 30:    return "unknown programme type 30";
	      case 31:    return "unknown programme type 31";
	      default:    return "unknown programme type";
	   }
	}
	return "unknown programme type";
}

//	11-bit from HandleFIG0Extension13, see ETSI TS 101 756 table 16
const char *getUserApplicationType (int16_t appType) {
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

const char * getFECscheme (int16_t FEC_scheme) {
	switch (FEC_scheme) {
	   case 0:     return "no FEC";
	   case 1:     return "FEC";
	   default:    return "unknown";
	}
}


const char * getProtectionLevel (bool shortForm, int16_t protLevel) {
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

const char *getCodeRate (bool shortForm, int16_t protLevel) {
int h = protLevel;

	if (!shortForm)
	   return ((h & (1 << 2)) == 0) ?
	                    eep_Arates [h & 03] :
	                    eep_Brates [h & 03]; // EEP -A/-B
	else
	   return uep_rates [h - 1];     // UEP
}

