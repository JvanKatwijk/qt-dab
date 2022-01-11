

#include	"ITU_Region_1.h"


struct {
	uint8_t ecc;
	uint8_t countryId;
	const char * ITU_Code;
	const char * Country;
} ITU_table_1  [] = {
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
{0xE0, 0x0D, "D",   "Germany"},
{0xE0, 0x01, "D",   "Germany"},
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
{0xE3, 0x08, "HOL", "Netherlands"},
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
{0xE1, 0x0C, "G",   "United Kingdom"},
{0xE4, 0x0B, "UZB", "Uzbekistan"},
{0xE2, 0x04, "CVA", "Vatican"},
{0, 0, "", ""}
};

struct {
	uint8_t ecc;
	uint8_t countryId;
	const char * ITU_Code;
	const char * Country;
} ITU_table_4  [] = {
{0xF0, 0x0A, "AFG", "Afghanistan"},
{0xF0, 0x09, "ARS", "Saudi Arabia"},
{0xF0, 0x01, "acc", "Australia"},
{0xF0, 0x02, "arn", "Australia"},
{0xF0, 0x03, "acn", "Australia"},
{0xF0, 0x04, "arq", "Australia"},
{0xF0, 0x05, "arc", "Australia"},
{0xF0, 0x06, "arw", "Australia"},
{0xF0, 0x06, "arc", "Australia"},
{0, 0, "", ""}
};


QString         find_ITU_code (uint8_t ecc, uint8_t countryId) {
	if ((ecc >> 4) == 0xE) {
	   for (int i = 0; ITU_table_1 [i]. ecc != 0; i ++)
	      if ((ITU_table_1 [i]. ecc == ecc) &&
	                    (ITU_table_1 [i]. countryId == countryId))
	         return  ITU_table_1 [i]. ITU_Code;	
	}
	else
	if ((ecc >> 4) == 0xF) {
	   for (int i = 0; ITU_table_1 [i]. ecc != 0; i ++)
	      if ((ITU_table_4 [i]. ecc == ecc) &&
	                    (ITU_table_4 [i]. countryId == countryId))
	         return  ITU_table_4 [i]. ITU_Code;
	}
	return "";
}

QString         find_Country (uint8_t ecc, uint8_t countryId) {
	if ((ecc >> 4) == 0xE) {
	   for (int i = 0; ITU_table_1 [i]. ecc != 0; i ++)
	      if ((ITU_table_1 [i]. ecc == ecc) &&
	                    (ITU_table_1 [i]. countryId == countryId))
	         return  ITU_table_1 [i]. Country;
	}
	else
	if ((ecc >> 4) == 0xF) {
	   for (int i = 0; ITU_table_4 [i]. ecc != 0; i ++)
	      if ((ITU_table_4 [i]. ecc == ecc) &&
	                    (ITU_table_4 [i]. countryId == countryId))
	         return  ITU_table_4 [i]. Country;
	}

	return "";
}

