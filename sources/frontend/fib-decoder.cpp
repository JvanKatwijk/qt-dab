#
/*
 *    Copyright (C) 2018 .. 2025
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
 *
 * 	fib decoder. Functionality is shared between fic handler, i.e. the
 *	one preparing the FIC blocks for processing, and the mainthread
 *	from which calls are coming on selecting a program
 */
#include	"fib-decoder.h"
#include	<cstring>
#include	<vector>
#include	"radio.h"
#include	"charsets.h"
#include	"bit-extractors.h"
#include	"fib-config.h"
#include	"ensemble.h"
#include	"fib-table.h"
#include	<QStringList>
#include	"dab-tables.h"
#include	"time-converter.h"

#include	"dab-tables.h"
//
//
//	The fibDecoder was rewritten since the "old" one
//	contained (a) errors and (b) was incomplete on
//	some issues.
//	The current one is a straight forward implementation,
//	where the FIG's are stored in a (kind of) database
//	maintained in a class fibConfig

	fibDecoder::fibDecoder (RadioInterface *mr) {
	myRadioInterface	= mr;

	connect (this, &fibDecoder::ensembleName,
	         myRadioInterface, &RadioInterface::ensembleName);
	connect (this, &fibDecoder::clockTime,
	         myRadioInterface, &RadioInterface::clockTime);
	connect (this, &fibDecoder::changeinConfiguration,
	         myRadioInterface, &RadioInterface::changeinConfiguration);
	connect (this, &fibDecoder::announcement,
	         myRadioInterface, &RadioInterface::announcement);
	connect (this, &fibDecoder::nrServices,
	         myRadioInterface, &RadioInterface::nrServices);
	connect (this, &fibDecoder::lto_ecc,
	         myRadioInterface, &RadioInterface::lto_ecc);
	connect (this, &fibDecoder::setFreqList,
	         myRadioInterface, &RadioInterface::setFreqList);
//
//	Note that they may change "roles", 
	currentConfig	= new fibConfig();
	nextConfig	= new fibConfig();
	CIFcount	= 0;
	mjd		= 0;
}
	
	fibDecoder::~fibDecoder () {
	delete	nextConfig;
	delete	currentConfig;
}

//	FIB's are segments of 256 bits. When here, we already
//	passed the crc and we start unpacking into FIGs
//	This is merely a dispatcher
void	fibDecoder::processFIB (uint8_t *p, uint16_t fib) {
int8_t	processedBytes	= 0;
uint8_t	*d		= p;

	fibLocker. lock();
	(void)fib;
	while (processedBytes  < 30) {
	   uint8_t FIGtype	= getBits_3 (d, 0);
	   uint8_t FIGlength	= getBits_5 (d, 3);
	   if ((FIGtype == 0x07) && (FIGlength == 0x3F))
	      return;

	   switch (FIGtype) {
	      case 0:			
	         process_FIG0 (d);	
	         break;

	      case 1:			
	         process_FIG1 (d);
	         break;

	      case 2:		// not yet implemented
	         break;

	      case 7:
	         break;

	      default:
	         break;
	   }
//
//	Thanks to Ronny Kunze, who discovered that I used
//	a p rather than a d
	      processedBytes += getBits_5 (d, 3) + 1;
//	      processedBytes += getBits (p, 3, 5) + 1;
	      d = p + processedBytes * 8;
	}
	fibLocker. unlock();
}
//
//
void	fibDecoder::process_FIG0 (uint8_t *d) {
uint8_t	extension	= getBits_5 (d, 8 + 3);

	switch (extension) {
	   case 0:		// ensemble information (6.4.1)
	      FIG0Extension0 (d);
	      break;

	   case 1:		// sub-channel organization (6.2.1)
	      FIG0Extension1 (d);
	      break;

	   case 2:		// service organization (6.3.1)
	      FIG0Extension2 (d);
	      break;

	   case 3:		// service component in packet mode (6.3.2)
	      FIG0Extension3 (d);
	      break;

	   case 4:		// service component with CA (6.3.3)
	      break;

	   case 5:		// service component language (8.1.2)
	      FIG0Extension5 (d);
	      break;

	   case 6:		// service linking information (8.1.15)
	      break;

	   case 7:		// configuration information (6.4.2)
	      FIG0Extension7 (d);
	      break;

	   case 8:		// service component global definition (6.3.5)
	      FIG0Extension8 (d);
	      break;

	   case 9:              // country, LTO & international table (8.1.3.2)
	      FIG0Extension9 (d);
	      break;

	   case 10:             // date and time (8.1.3.1)
	      FIG0Extension10 (d);
	      break;

	   case 11:		// Reserved
	      break;

	   case 12:		// Reserved
	      break;

	   case 13:             // user application information (6.3.6)
	      FIG0Extension13 (d);
	      break;

	   case 14:             // FEC subchannel organization (6.2.2)
	      FIG0Extension14 (d);
	      break;

	   case 15:		// Emergency warning (ETSI TS 104 089
	                       	// Not implemented yet
	      break;

	   case 16:		// Reserved
	      break;

	   case 17:		// Program type (8.1.5)
	      FIG0Extension17 (d);
	      break;

	   case 18:             // announcement support (8.1.6.1)
	      FIG0Extension18 (d);
	      break;

	   case 19:             // announcement switching (8.1.6.2)
	      FIG0Extension19 (d);
	      break;

	   case 20:		// service component information (8.1.4)
	      FIG0Extension20 (d);	 // not encountered yet
	      break;	

	   case 21:		// frequency information (8.1.8)
	      FIG0Extension21 (d);
	      break;

	   case 22:		// Reserved
	      break;

	   case 23:		// Reserved
	      break;

	   case 24:		// OE services (8.1.10)
	      break;		// not implemented

	   case 25:		// OE announcement support (8.1.6.3)
	      break;		// not implemented

	   case 26:		// OE announcement switching (8.1.6.4)
	      break;		// not implemented

	   case 27:		// Reserved
	   case 28:		// Reserved
	   case 29:		// Reserved
	   case 30:		// Reserved
	   case 31:		// Reserved
	      break;

	   default:
//	      fprintf (stderr, "Missed %d\n", extension);
	      break;
	}
}
//	Ensemble information, 6.4.1
//	FIG0/0 indicated a change in channel organization
//	The info is MCI
void	fibDecoder::FIG0Extension0 (uint8_t *d) {
uint16_t        EId;
uint8_t         changeFlag;
uint16_t        highpart, lowpart;
int16_t         occurrenceChange;
//uint8_t CN_bit	= getBits_1 (d, 8 + 0);
uint8_t		alarmFlag;
static	uint8_t prevChangeFlag	= 0;

//	(void)CN_bit;
	EId                     = getBits   (d, 16, 16);
	(void)EId;
	changeFlag              = getBits_2 (d, 16 + 16);
	alarmFlag		= getBits_1 (d, 16 + 16 + 2);
	highpart                = getBits_5 (d, 16 + 19);
	lowpart                 = getBits_8 (d, 16 + 24);
	(void)alarmFlag;
	occurrenceChange        = getBits_8 (d, 16 + 32);
	(void)occurrenceChange;
	CIFcount_hi		= highpart;
	CIFcount_lo		= lowpart;
	CIFcount 		= highpart * 250 + lowpart;


	if ((changeFlag == 0) && (prevChangeFlag == 3)) {
	   fibConfig 	*temp	= currentConfig;
	   currentConfig	= nextConfig;
	   nextConfig		= temp;
	   nextConfig	->  reset ();
//	   cleanupServiceList ();
	   emit changeinConfiguration ();
	}
	prevChangeFlag	= changeFlag;
//	if (alarmFlag)
//	   fprintf (stderr, "serious problem\n");
}
//
//	Subchannel organization 6.2.1
//	FIG0 extension 1 creates a mapping between the
//	sub channel identifications and the positions in the
//	relevant CIF.
void	fibDecoder::FIG0Extension1 (uint8_t *d) {
int16_t	used		= 2;		// offset in bytes
int16_t	Length		= getBits_5 (d, 3);
uint8_t	CN_bit		= getBits_1 (d, 8 + 0);
uint8_t	OE_bit		= getBits_1 (d, 8 + 1);
uint8_t	PD_bit		= getBits_1 (d, 8 + 2);

	while (used < Length - 1)
	   used = HandleFIG0Extension1 (d, used, CN_bit, OE_bit, PD_bit);
}
//
//	defining the channels 
int16_t	fibDecoder::HandleFIG0Extension1 (uint8_t *d, int16_t offset,
	                                  const uint8_t CN_bit,
	                                  const uint8_t OE_bit,
	                                  const uint8_t PD_bit) {

int16_t	bitOffset	= offset * 8;
const int16_t subChId	= getBits_6 (d, bitOffset);
const int16_t startAdr	= getBits (d, bitOffset + 6, 10);
int16_t	tabelIndex;
int16_t	option, protLevel, chanSize;
fibConfig::subChannel	channel;
fibConfig	*localBase = CN_bit == 0 ? currentConfig : nextConfig;
static	int table_1 [] = {12, 8, 6, 4};
static	int table_2 [] = {27, 21, 18, 15};

	(void)OE_bit; (void)PD_bit;
	channel. subChId	= subChId;
	channel. startAddr	= startAdr;
	channel. Length	= 0;
	channel. FEC_scheme	= 0;	// corrected later on

	if (getBits_1 (d, bitOffset + 16) == 0) {	// short form
	   tabelIndex = getBits_6 (d, bitOffset + 18);
	   channel. Length	= ProtLevel [tabelIndex][0];
	   channel. shortForm	= true;		// short form
	   channel. protLevel	= ProtLevel [tabelIndex][1];
	   channel. bitRate		= ProtLevel [tabelIndex][2];
	   bitOffset += 24;
	}
	else { 	// EEP long form
	   channel. shortForm	= false;
	   option = getBits_3 (d, bitOffset + 17);
	   if (option == 0) { 		// A Level protection
	      protLevel			= getBits (d, bitOffset + 20, 2);
	      channel.	protLevel	= protLevel;
	      chanSize	= getBits (d, bitOffset + 22, 10);
	      channel. Length	= chanSize;
	      channel. bitRate	= chanSize / table_1 [protLevel] * 8;
	   }
	   else			// option should be 001
	   if (option == 001) {		// B Level protection
	      protLevel			= getBits_2 (d, bitOffset + 20);
	      channel. protLevel	= protLevel + (1 << 2);
	      chanSize		= getBits (d, bitOffset + 22, 10);
	      channel. Length	= chanSize;
	      channel. bitRate	= chanSize / table_2 [protLevel] * 32;
	   }
	   bitOffset += 32;
	}
//
//	in case the subchannel data was already computed
//	we merely compute the offset
	if (localBase -> findIndex_subChannel_table (subChId) >= 0)
	   return bitOffset / 8;
//
	
	localBase -> subChannel_table. push_back (channel);
	return bitOffset / 8;	// we return bytes
}
//
//	Service organization, 6.3.1
//	bind channels to SIds
void	fibDecoder::FIG0Extension2 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
int16_t	Length	= getBits_5 (d, 3);
const uint8_t	CN_bit	= getBits_1 (d, 8 + 0);
const uint8_t	OE_bit	= getBits_1 (d, 8 + 1);
const uint8_t	PD_bit	= getBits_1 (d, 8 + 2);

	while (used < Length) {
	   used = HandleFIG0Extension2 (d, used, CN_bit, OE_bit, PD_bit);
	}
}
//
//
int16_t	fibDecoder::HandleFIG0Extension2 (uint8_t *d,
	                                  int16_t offset,
	                                  const uint8_t CN_bit,
	                                  const uint8_t OE_bit,
	                                  const uint8_t PD_bit) {
int16_t		bitOffset	= 8 * offset;
uint8_t		ecc = 0;
uint8_t		cId;
uint32_t	SId;
int16_t		numberofComponents;

fibConfig	*localBase = CN_bit == 0 ? currentConfig : nextConfig;
	
	(void)OE_bit;

	if (PD_bit == 1) {		// long Sid, data
	   ecc	= getBits_8 (d, bitOffset);	(void)ecc;
	   cId	= getBits_4 (d, bitOffset + 4);
	   SId	= getLBits  (d, bitOffset, 32);
	   bitOffset	+= 32;
	}
	else {
	   cId	= getBits_4 (d, bitOffset);	(void)cId;
	   SId	= getBits   (d, bitOffset, 16);
	   bitOffset	+= 16;
	}
	
	numberofComponents	= getBits_4 (d, bitOffset + 4);

	fibConfig::SId_struct SId_element;
	SId_element. announcing = 0;
	SId_element. SId = SId;
	for (auto &ss: localBase -> SId_table) {
	   if (ss. SId == SId) {
	      bitOffset += numberofComponents * 16 + 8;
	      return bitOffset / 8;
	   }
	}
	bitOffset	+= 8;
	for (uint16_t i = 0; i < numberofComponents; i ++) {
	   fibConfig::serviceComp_C comp;
	   comp. SId	= SId;
	   comp. compNr	= i;
	   
	   const uint8_t TMid	= getBits_2 (d, bitOffset);
	   comp. TMid = TMid;
	   if (TMid == 00)  {	// Audio
	      comp. 	ASCTy	= getBits_6 (d, bitOffset + 2);
	      comp.	subChId	= getBits_6 (d, bitOffset + 8);
	      comp.	PS_flag	= getBits_1 (d, bitOffset + 14);
	   }
	   else
	   if (TMid == 3) { // MSC packet data
	      comp. SCId	= getBits   (d, bitOffset + 2, 12);
	      comp. PS_flag	= getBits_1 (d, bitOffset + 14);
//	      uint8_t CA_flag	= getBits_1 (d, bitOffset + 15);
	   }
	   else 
	      {;}
	   bitOffset += 16;
	   if (!localBase -> compIsKnown (comp)) {	
	      SId_element. comps. push_back (localBase -> SC_C_table. size ());
	      localBase -> SC_C_table. push_back (comp);
	   }
	}
	localBase -> SId_table. push_back (SId_element);
	return bitOffset / 8;		// in Bytes
}

//	Service component in packet mode 6.3.2
void	fibDecoder::FIG0Extension3 (uint8_t *d) {
int16_t used    = 2;            // offset in bytes
int16_t Length  = getBits_5 (d, 3);
const uint8_t CN_bit  = getBits_1 (d, 8 + 0);
const uint8_t OE_bit  = getBits_1 (d, 8 + 1);
const uint8_t PD_bit  = getBits_1 (d, 8 + 2);

	while (used < Length)
	   used = HandleFIG0Extension3 (d, used, CN_bit, OE_bit, PD_bit);
}
//
//	Note that the SCId (Service Component Identifier) is
//	a unique 12 bit number in the ensemble
int16_t fibDecoder::HandleFIG0Extension3 (uint8_t	*d,
	                                  int16_t	used,
	                                  const uint8_t	CN_bit,
	                                  const uint8_t	OE_bit,
	                                  const uint8_t	PD_bit) {
const int16_t SCId            = getBits   (d, used * 8,  12);
const int16_t CAOrgflag       = getBits_1 (d, used * 8 + 15);
const int16_t DGflag          = getBits_1 (d, used * 8 + 16);
const int16_t DSCTy           = getBits_6 (d, used * 8 + 18);
const int16_t SubChId         = getBits_6 (d, used * 8 + 24);
const int16_t packetAddress   = getBits   (d, used * 8 + 30, 10);
uint16_t  CAOrg		= 0;
fibConfig	*localBase = CN_bit == 0 ? currentConfig : nextConfig;

	(void)OE_bit; (void)PD_bit;

	if (CAOrgflag == 1) {
	   CAOrg = getBits (d, used * 8 + 40, 16);
	   used += 16 / 8;
	}
	(void)CAOrg;
	used += 40 / 8;

	for (auto &comp : localBase ->  SC_P_table)
	   if (comp. SCId == SCId)
	      return used;
	fibConfig::serviceComp_P element;
	element. SCId  		= SCId;
	element. subChId  	= SubChId;
	element. DSCTy		= DSCTy;
	element. DG_flag	= DGflag;
	element.  packetAddress	= packetAddress;
	localBase -> SC_P_table. push_back (element);
	return used;
}

//	Service component language 8.1.2
void	fibDecoder::FIG0Extension5 (uint8_t *d) {
int16_t	used		= 2;		// offset in bytes
int16_t	Length		= getBits_5 (d, 3);
const uint8_t CN_bit	= getBits_1 (d, 8 + 0);
const uint8_t OE_bit	= getBits_1 (d, 8 + 1);
const uint8_t PD_bit	= getBits_1 (d, 8 + 2);

	while (used < Length) {
	   used = HandleFIG0Extension5 (d, used,  CN_bit, OE_bit, PD_bit);
	}
}

int16_t	fibDecoder::HandleFIG0Extension5 (uint8_t* d,
	                                  uint16_t 	offset,
	                                  const uint8_t CN_bit,
	                                  const uint8_t OE_bit,
	                                  const uint8_t PD_bit) {
int16_t	bitOffset	= offset * 8;
const uint8_t	LS_flag	= getBits_1 (d, bitOffset);
fibConfig	*localBase	= CN_bit == 0 ? currentConfig : nextConfig;
fibConfig::SC_language comp;
	(void)OE_bit;
	(void)PD_bit;
	comp. LS_flag = LS_flag;
	if (LS_flag == 0) {
	   comp. subChId = getBits (d, bitOffset + 2, 6);
	   comp. SCId	= 255;
	   uint8_t language = getBits (d, bitOffset + 8, 8);
	   for (auto &scId : localBase -> subChannel_table) {
	      if (scId. subChId == comp. subChId) {
	         comp. language = language;
	         break;
	      }
	   }
	   bitOffset += 16;
	}
	else {
	   comp. SCId = getBits (d, bitOffset + 4, 12);
	   comp. subChId = 255;
	   uint8_t language = getBits (d, bitOffset + 16, 8);
	   for (auto & scId : localBase -> SC_P_table) {
	      if (scId. SCId == comp. SCId) {
	         for (auto &subch : localBase -> subChannel_table) {
	            if (subch. subChId == scId. subChId) {
	               comp. language = language;
	               break;
	            }
	         }
	      }
	   }
	   bitOffset += 24;
	}
	for (auto &lanComp : localBase -> language_table) {
	   if ((lanComp. LS_flag == 0) &&
	       (lanComp. subChId == comp. subChId))	
	      return bitOffset / 8;
	   if ((lanComp. LS_flag != 0) &&
	       (lanComp. SCId == comp. SCId))
	      return bitOffset / 8;
	}
	localBase -> language_table. push_back (comp);
	return bitOffset / 8;
}
//
// FIG0/7: Configuration linking information 6.4.2,
void    fibDecoder::FIG0Extension7 (uint8_t *d) {
int16_t used		= 2;            // offset in bytes
int16_t Length          = getBits_5 (d, 3);
const uint8_t CN_bit	= getBits_1 (d, 8 + 0);
const uint8_t OE_bit	= getBits_1 (d, 8 + 1);
const uint8_t PD_bit	= getBits_1 (d, 8 + 2);

const int serviceCount	= getBits_6 (d, used * 8);
//int     counter		= getBits   (d, used * 8 + 6, 10);

	(void)Length;
	(void)CN_bit; (void)OE_bit; (void)PD_bit;
//	fprintf (stderr, "services : %d\n", serviceCount);
	if (CN_bit == 0)	// only current configuration for now
	   nrServices (serviceCount);
//	(void)counter;
}

// FIG0/8:  Service Component Global Definition (6.3.5)
void	fibDecoder::FIG0Extension8 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
int16_t	Length		= getBits_5 (d, 3);
const uint8_t	CN_bit	= getBits_1 (d, 8 + 0);
const uint8_t	OE_bit	= getBits_1 (d, 8 + 1);
const uint8_t	PD_bit	= getBits_1 (d, 8 + 2);

	while (used < Length) {
	   used = HandleFIG0Extension8 (d, used, CN_bit, OE_bit, PD_bit);
	}
}

int16_t	fibDecoder::HandleFIG0Extension8 (uint8_t	*d,
	                                  int16_t	used,
	                                  const uint8_t	CN_bit,
	                                  const uint8_t	OE_bit,
	                                  const uint8_t	PD_bit) {
int16_t	bitOffset	= used * 8;
const uint32_t	SId	= getLBits (d, bitOffset, PD_bit == 1 ? 32 : 16);
uint8_t		LS_flag;
uint8_t		extensionFlag;
fibConfig	*localBase	= CN_bit == 0 ? currentConfig : nextConfig;

fibConfig::serviceComp_G comp;
	(void)OE_bit;
	bitOffset	+= PD_bit == 1 ? 32 : 16;
	extensionFlag   = getBits_1 (d, bitOffset);
	uint8_t SCIds	= getBits_4 (d, bitOffset + 4);

//	int serviceIndex = find_service (SId);
	bitOffset	+= 8;
	LS_flag		= getBits_1 (d, bitOffset);

	comp. SId	= SId;
	comp. SCIds	= SCIds;
	comp. LS_flag	= LS_flag;

	if (LS_flag == 0) {	// short form
	   comp. subChId = getBits_6 (d, bitOffset + 2);
	   bitOffset += 8;
	}
	else {			// long form
	   comp. SCId	= getBits (d, bitOffset + 4, 12);
	   bitOffset += 16;
	}
	if (extensionFlag)
	   bitOffset += 8;	// skip Rfa
	for (auto &el : localBase -> SC_G_table)
	   if ((el. SId == SId) && (el. SCIds == SCIds))
	      return bitOffset / 8;
	localBase -> SC_G_table. push_back (comp);
	return bitOffset / 8;
}

//	FIG0/9 Country, LTO and International table, clause 8.1.3.2;
void	fibDecoder::FIG0Extension9 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
//int16_t	Length		= getBits_5 (d, 3);
//uint8_t	CN_bit		= getBits_1 (d, 8 + 0);
//uint8_t	OE_bit		= getBits_1 (d, 8 + 1);
//uint8_t	PD_bit		= getBits_1 (d, 8 + 2);
//	6 indicates the number of hours
        const int signbit = getBits_1 (d, used * 8 + 2);
        currentConfig -> dateTime [6] = (signbit == 1)?
                                         -1 * getBits_4 (d, used * 8 + 3):
                                         getBits_4 (d, used * 8 + 3);
//      7 indicates a possible remaining half our
        currentConfig -> dateTime [7] =
	                   (getBits_1 (d, used * 8 + 7) == 1) ? 30 : 0;
        if (signbit != 0)
           currentConfig -> dateTime [7] = - currentConfig -> dateTime [7];

	uint8_t	LTO	= currentConfig -> dateTime [6];
	uint8_t ecc	= getBits (d, used * 8 + 8, 8);
	theEnsemble.	eccByte	= ecc;
	theEnsemble.	lto	= LTO;
	lto_ecc (LTO, ecc);
}

int	monthLength [] {
31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
//
//	Time in 10 is given in UTC, for other time zones
//	we add (or subtract) a number of Hours (half hours)
void	fibDecoder::adjustTime (int32_t *dateTime) {
//	first adjust the half hour  in the amount of minutes
	(void)dateTime;
	currentConfig -> dateTime [4] += 
	            (currentConfig -> dateTime [7] == 1) ? 30 : 0;
	if (currentConfig -> dateTime [4] >= 60) {
	   currentConfig -> dateTime [4] -= 60;
	   currentConfig -> dateTime [3] ++;
	}

	if (currentConfig -> dateTime [4] < 0) {
	   currentConfig -> dateTime [4] += 60;
	   currentConfig -> dateTime [3] --;
	}

	currentConfig -> dateTime [3] += currentConfig -> dateTime [6];
	if ((0 <= currentConfig ->  dateTime [3]) && 
	    (currentConfig -> dateTime [3] <= 23))
	   return;

	if (currentConfig -> dateTime [3] > 23) {
	   currentConfig -> dateTime [3] -= 24;
	   currentConfig -> dateTime [2] ++;
	}

	if (currentConfig -> dateTime [3] < 0) {
	   currentConfig -> dateTime [3] += 24;
	   currentConfig -> dateTime [2] --;
	}

	if (currentConfig -> dateTime [2] >
	             monthLength [currentConfig -> dateTime [1] - 1]) {
	   currentConfig -> dateTime [2] = 1;
	   currentConfig -> dateTime [1] ++;
	   if (currentConfig -> dateTime [1] > 12) {
	      currentConfig -> dateTime [1] = 1;
	      currentConfig -> dateTime [0] ++;
	   }
	}

	if (currentConfig -> dateTime [2] < 0) {
	   if (currentConfig -> dateTime [1] > 1) {
	      currentConfig -> dateTime [2] =
	                  monthLength [currentConfig -> dateTime [1] - 1 - 1];
	      currentConfig -> dateTime [1] --;
	   }
	   else {
	      currentConfig -> dateTime [2] = monthLength [11];
	      currentConfig -> dateTime [1] = 12;
	      currentConfig -> dateTime [0] --;
	   }
	}
}

//	8.1.3.1 Date and time (d&t)
void fibDecoder::FIG0Extension10 (uint8_t *dd) {
int16_t		offset = 16;
this	->	mjd	= getLBits (dd, offset + 1, 17);
uint16_t	theTime	[6];

	convertTime (mjd, theTime);
//	theTime [0] = Y;	// Year
//	theTime [1] = M;	// Month
//	theTime [2] = D;	// Day
	theTime [3] = getBits_5 (dd, offset + 21); // Hours
	theTime [4] = getBits_6 (dd, offset + 26); // Minutes

	if (getBits_6 (dd, offset + 26) != currentConfig -> dateTime [4]) 
	   theTime [5] =  0;	// Seconds

	if (dd [offset + 20] == 1)
	   theTime [5] = getBits_6 (dd, offset + 32);	// Seconds
//
//	take care of different time zones
	bool	change = false;
	for (int i = 0; i < 6; i ++) {
	   if (theTime [i] != currentConfig -> dateTime [i])
	      change = true;
	   currentConfig -> dateTime [i] = theTime [i];
	}

#ifdef	CLOCK_STREAMER
	change = true;
#endif
	if (change) {
	   int utc_day		= currentConfig -> dateTime [2];
	   int utc_hour		= currentConfig -> dateTime [3];
	   int utc_minute 	= currentConfig -> dateTime [4];
	   int utc_seconds	= currentConfig -> dateTime [5];
	   adjustTime (currentConfig -> dateTime);
	   emit  clockTime (currentConfig -> dateTime [0],
	                    currentConfig -> dateTime [1],
	                    currentConfig -> dateTime [2],
	                    currentConfig -> dateTime [3],
	                    currentConfig -> dateTime [4],
	                    utc_day, utc_hour, utc_minute, utc_seconds);
	}
}
//
//	User Application Information 6.3.6
void	fibDecoder::FIG0Extension13 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
int16_t	Length		= getBits_5 (d, 3);
const uint8_t	CN_bit		= getBits_1 (d, 8 + 0);
const uint8_t	OE_bit		= getBits_1 (d, 8 + 1);
const uint8_t	PD_bit		= getBits_1 (d, 8 + 2);

	while (used < Length) 
	   used = HandleFIG0Extension13 (d, used, CN_bit, OE_bit, PD_bit);
}
//
//	section 6.3.6 User application Data
int16_t	fibDecoder::HandleFIG0Extension13 (uint8_t *d,
	                                   int16_t used,
	                                   const uint8_t CN_bit,
	                                   const uint8_t OE_bit,
	                                   const uint8_t pdBit) {
int16_t	bitOffset	= used * 8;
uint32_t	SId	= getLBits (d, bitOffset, pdBit == 1 ? 32 : 16);
uint16_t	SCIds;
int16_t		NoApplications;
int16_t		i;
int16_t		appType;
fibConfig	*localBase	= CN_bit == 0 ? currentConfig : nextConfig;

fibConfig::AppType element;
	(void)OE_bit;
	bitOffset	+= pdBit == 1 ? 32 : 16;
	SCIds		= getBits_4 (d, bitOffset);
	NoApplications	= getBits_4 (d, bitOffset + 4);
	bitOffset	+= 8;
	element. SId	= SId;
	element. SCIds	= SCIds;

	for (i = 0; i < NoApplications; i ++) {
	   appType		= getBits (d, bitOffset, 11);
	   int16_t length	= getBits_5 (d, bitOffset + 11);
	   element. Apptype	= appType;
	   bitOffset 		+= (11 + 5 + 8 * length);
	}
	for (auto &comp : localBase -> AppType_table)
	   if ((comp. SId == SId) && (comp. SCIds == SCIds))
	      return bitOffset / 8;
	localBase -> AppType_table. push_back (element);
	return bitOffset / 8;
}

//	FEC sub-channel organization 6.2.2
void	fibDecoder::FIG0Extension14 (uint8_t *d) {
int16_t	Length		= getBits_5 (d, 3);	// in Bytes
const uint8_t	CN_bit	= getBits_1 (d, 8 + 0);
const uint8_t	OE_bit	= getBits_1 (d, 8 + 1);
const uint8_t	PD_bit	= getBits_1 (d, 8 + 2);
int16_t	used	= 2;			// in Bytes
fibConfig	*localBase	= CN_bit == 0 ? currentConfig : nextConfig;

	(void)OE_bit; (void)PD_bit;
	while (used < Length) {
	   
	   int16_t subChId	= getBits_6 (d, used * 8);
	   uint8_t FEC_scheme	= getBits_2 (d, used * 8 + 6);
	   used = used + 1;
	   for (auto &subC: localBase -> subChannel_table)
	      if (subC. subChId == subChId)
	         subC. FEC_scheme = FEC_scheme;
	}
}
//
//	program type 8.1.5
void	fibDecoder::FIG0Extension17 (uint8_t *d) {
int16_t	length	= getBits_5 (d, 3);
int16_t	offset	= 16;

	while (offset < length * 8) {
	   uint16_t	SId	= getBits (d, offset, 16);
	   uint8_t typeCode	= getBits_5 (d, offset + 27);
	   for (uint16_t i = 0; i < theEnsemble. primaries. size (); i ++) {
	      if (theEnsemble. primaries [i]. SId == SId) {
	         theEnsemble. primaries [i]. programType = typeCode;
	         break;
	      }
	   }
	   offset += 32;
	}
}
//
//	Announcement support 8.1.6.1
void	fibDecoder::FIG0Extension18 (uint8_t *d) {
int16_t	Length		= getBits_5 (d, 3);	// in Bytes
const uint8_t	CN_bit	= getBits_1 (d, 8 + 0);
const uint8_t	OE_bit	= getBits_1 (d, 8 + 1);
const uint8_t	PD_bit	= getBits_1 (d, 8 + 2);
int16_t	used	= 2;			// in Bytes
int16_t	bitOffset		= used * 8;
fibConfig	*localBase	= CN_bit == 0 ? currentConfig : nextConfig;

	(void)OE_bit; (void)PD_bit;

	while (bitOffset < Length * 8) {
	   uint16_t SId		= getBits (d, bitOffset, 16);
	   bitOffset		+= 16;
	   uint16_t asuFlags	= getBits (d, bitOffset, 16);
	   bitOffset		+= 16;
	   (void)		  getBits (d, bitOffset,  5); // Rfa
	   uint8_t nrClusters	= getBits (d, bitOffset + 5, 3);
	   bitOffset		+= 8;

	   for (int i = 0; i < nrClusters; i ++) {
	      fibConfig::FIG18_cluster aC;
	      uint8_t clusterId = getBits (d, bitOffset + 8 * i, 8);
	      if (clusterId == 0)
	         continue;
	      bool inTable = false;
	      for (auto &ACe : localBase -> announcement_table) {
	         if ((ACe. SId == SId) && (clusterId == ACe. clusterId)) {
	            inTable = true;
	            break;
	         }
	      }
	      if (!inTable) {
	         aC. SId = SId;
	         aC. asuFlags = asuFlags;
	         aC. clusterId = clusterId;
	         localBase -> announcement_table. push_back (aC);
	      }
	   }
	   bitOffset	+= nrClusters * 8;
	}
}

//	Announcement switching 8.1.6.2
void	fibDecoder::FIG0Extension19 (uint8_t *d) {
int16_t	Length		= getBits_5 (d, 3);	// in Bytes
const uint8_t	CN_bit	= getBits_1 (d, 8 + 0);
const uint8_t	OE_bit	= getBits_1 (d, 8 + 1);
const uint8_t	PD_bit	= getBits_1 (d, 8 + 2);
int16_t	used		= 2;			// in Bytes
int16_t	bitOffset	= used * 8;
fibConfig *localBase	= CN_bit == 0 ? currentConfig : nextConfig;

	(void)OE_bit; (void)PD_bit;
	while (bitOffset < Length * 8) {
	   uint8_t clusterId	= getBits (d, bitOffset, 8);
	   bitOffset += 8;
	   uint16_t AswFlags	= getBits (d, bitOffset, 16);
	   bitOffset		+= 16;
	   uint8_t newFlag	= getBits (d, bitOffset, 1);
	   bitOffset		+= 1;
	   uint8_t Rfa		= getBits (d, bitOffset, 1);
	   (void)Rfa;
	   bitOffset		+= 1;
	   uint8_t subChId	= getBits (d, bitOffset, 6);
	   bitOffset		+= 6;
	   for (auto &ac : localBase -> announcement_table) {
	      if ((ac. clusterId == clusterId) && newFlag)
	         handleAnnouncement (ac. SId,
	                               ac. asuFlags & AswFlags, subChId);
	   }
	}
	return;
}
//
//	Service Component Information
void	fibDecoder::FIG0Extension20 (uint8_t *d) {
int16_t	used		= 2;		// offset in bytes
const int16_t	Length	= getBits_5 (d, 3);
const uint8_t	CN_bit	= getBits_1 (d, 8 + 0);
const uint8_t	OE_bit	= getBits_1 (d, 8 + 1);
const uint8_t	PD_bit	= getBits_1 (d, 8 + 2);

	return;
	while (used < Length) 
	   used = HandleFIG0Extension21 (d, used, CN_bit, OE_bit, PD_bit);
}

int16_t	fibDecoder::HandleFIG0Extension20 (uint8_t	*d,
	                                   uint16_t	offset,
	                                   const uint8_t CN_bit,
	                                   const uint8_t OE_bit,
	                                   const uint8_t PD_bit) {
	(void)CN_bit; (void)OE_bit;
	uint32_t SId		= PD_bit? getLBits (d, offset, 32) :
	                                  getLBits (d, offset, 16);
	offset += PD_bit ? 32 : 16;
	uint8_t SCIds		= getBits_4 (d, offset); offset += 4;
	uint8_t ChangeFlags	= getBits_2 (d, offset); offset += 2;
	uint8_t PT_flag		= getBits_1 (d, offset); offset += 1;
	uint8_t SC_flag		= getBits_1 (d, offset); offset += 1;
	uint8_t AD_flag		= getBits_1 (d, offset + 1); offset += 2;
	uint8_t SCTy		= getBits_6 (d, offset); offset += 6;
	uint8_t Date		= getBits_5 (d, offset); offset += 5;
	uint8_t Hour		= getBits_5 (d, offset); offset += 5;
	uint8_t Minutes		= getBits_6 (d, offset); offset += 6;
	uint8_t Seconds		= getBits_6 (d, offset); offset += 6;
	uint8_t SId_flag	= getBits_1 (d, offset); offset += 1;
	uint8_t Eid_flag	= getBits_1 (d, offset); offset += 1;
	uint32_t Transfer_Id	= PD_bit ? getLBits (d, offset, 32) :
	                                   getLBits (d, offset, 16);
	offset += PD_bit ? 32 : 16;
	uint16_t Transfer_EId	= Eid_flag ? getLBits (d, offset, 16) : 0;
	offset += Eid_flag ? 16 : 0;
	fprintf (stderr, "%X (%d) is in fig 20\n", SId, SCIds); 
	(void)ChangeFlags; (void)PT_flag; (void) SC_flag; 
	(void)AD_flag; (void)SCTy; (void)Date; (void)Hour;
	(void)Minutes; (void)Seconds; (void)SId_flag;
	(void)Transfer_Id; (void)Transfer_EId;
	return offset;
}
	   
//	Frequency information (FI) 8.1.8
void	fibDecoder::FIG0Extension21 (uint8_t *d) {
int16_t	used		= 2;		// offset in bytes
const int16_t	Length	= getBits_5 (d, 3);
const uint8_t	CN_bit	= getBits_1 (d, 8 + 0);
const uint8_t	OE_bit	= getBits_1 (d, 8 + 1);
const uint8_t	PD_bit	= getBits_1 (d, 8 + 2);

	while (used < Length) 
	   used = HandleFIG0Extension21 (d, used, CN_bit, OE_bit, PD_bit);
}

int16_t	fibDecoder::HandleFIG0Extension21 (uint8_t	*d,
	                                   uint16_t	offset,
	                                   const uint8_t CN_bit,
	                                   const uint8_t OE_bit,
	                                   const uint8_t PD_bit) {
int16_t	l_offset	= offset * 8;
int16_t	l	= getBits_5 (d, l_offset + 11);
int16_t		upperLimit	= l_offset + 16 + l * 8;
int16_t		base		= l_offset + 16;

	(void)CN_bit; (void)OE_bit, (void)PD_bit;
	if (OE_bit == 1)	// this ios not for use
	   return upperLimit / 8;

	bool newData = false;
	while (base < upperLimit) {
	   uint16_t idField	= getBits (d, base, 16);
	   uint8_t  RandM	= getBits_4 (d, base + 16);
	   uint8_t  continuity	= getBits_1 (d, base + 20);
	   (void)continuity;
	   uint8_t  length	= getBits_3 (d, base + 21);
	   if (RandM == 0x08) {
	      uint16_t fmFrequency_key	= getBits (d, base + 24, 8);
	      int32_t  fmFrequency	= 87500 + fmFrequency_key * 100;
	      for (auto &serv : theEnsemble. primaries) {
	         if ((serv. SId == idField)) {
	            bool alreadyIn = false;
	            for (auto freq : serv. fmFrequencies) {
	               if (fmFrequency == freq) {
	                  alreadyIn = true;
	                  break;
	               }
	            }
	            if (!alreadyIn) {
	               serv. fmFrequencies. push_back (fmFrequency);
	               newData = true;
	            }
	         }
	      }
	   }
	   base += 24 + length * 8;
	}
	if (newData)
	   emit setFreqList ();
	         
	return upperLimit / 8;
}
//
//	FIG 1 - Cover the different possible labels, section 5.2
void	fibDecoder::process_FIG1 (uint8_t *d) {
uint8_t	extension	= getBits_3 (d, 8 + 5); 

	switch (extension) {
	   case 0:		// ensemble name
	      FIG1Extension0 (d);
	      break;

	   case 1:		// service name
	      FIG1Extension1 (d);
	      break;

	   case 2:		// Labels etc
	      break;

	   case 3:		// obsolete
	      break;

	   case 4:		// Service Component Label
	      FIG1Extension4 (d);
	      break;

	   case 5:		// Data service label
	      FIG1Extension5 (d);
	      break;

	   case 6:		// XPAD label - 8.1.14.4
	      break;

	   default:
	      ;
	}
}
//	Name of the ensemble
//
void	fibDecoder::FIG1Extension0 (uint8_t *d) {
int16_t		offset	= 0;
char		label [17];

//      from byte 1 we deduce:
	const uint8_t charSet	= getBits_4 (d, 8);
	const uint8_t Rfu	= getBits_1 (d, 8 + 4);
	label [16]      = 0x00;
	(void)Rfu;

	const uint16_t EId	= getBits (d, 16, 16);
	offset	= 32;
	if ((charSet <= 16)) { // EBU Latin based repertoire
	   for (int i = 0; i < 16; i ++) {
	      label [i] = getBits_8 (d, offset + 8 * i);
	   }
	   const QString name = toQStringUsingCharset (
	                                        (const char *) label,
	                                        (CharacterSet) charSet);
	   QString realName = name;
	   for (int i = name. length (); i < 16; i ++)
	      realName. append (' ');
	   if (!theEnsemble. namePresent) {
	      theEnsemble. ensembleName	= realName;
	      theEnsemble. EId		= EId;
	      theEnsemble. namePresent	= true;
	      ensembleName (EId, name);
	   }
	   theEnsemble. isSynced = true;
	}
}
//
//	Name of service
void	fibDecoder::FIG1Extension1 (uint8_t *d) {
int16_t		offset	= 32;
char		label [17];

//      from byte 1 we deduce:
	const uint8_t charSet	= getBits_4 (d, 8);
	const uint8_t Rfu	= getBits_1 (d, 8 + 4);
	const uint8_t extension	= getBits_3 (d, 8 + 5);
	const uint32_t SId	= getBits (d, 16, 16);
	label [16]      = 0x00;
	(void)Rfu; (void)extension;
	if (charSet >= 16) 	// does not seem right
	   return;
	
	for (auto &serv : theEnsemble. primaries) {
	   if (SId == serv. SId) 
	      return;
	}

	for (int i = 0; i < 16; i ++) 
	   label [i] = getBits_8 (d, offset + 8 * i);
	QString dataName = toQStringUsingCharset (
	                                  (const char *) label,
	                                  (CharacterSet) charSet);
	for (int i = dataName. length (); i < 16; i ++)
	   dataName. append (' ');
	QString shortName;		
	for (int i = 0; i < 16; i ++) 
	   if (getBits_1 (d, offset + 16 * 8 + i) != 0)
	      shortName. append (dataName. at (i));

	ensemble::service prim;
	prim. programType	= 0;
	prim. name 		= dataName;
	prim. shortName		= shortName;
	prim. SId		= SId;
	prim. fmFrequencies. resize (0);
	theEnsemble. primaries. push_back (prim);
	int subChId = -1;
	for (int i = 0; i < (int)(currentConfig -> SC_C_table. size ()); i ++) {
	   fibConfig::serviceComp_C  comp  = currentConfig -> SC_C_table [i];
	   if ((comp. compNr == 0) && (comp. SId == SId))
	      subChId	= currentConfig -> subChannelOf (i);
	}
	addToEnsemble (dataName, SId, subChId);
	if (theEnsemble. primaries. size () >= 2)
	   theEnsemble. isSynced = true;
}

//	service component label - 32 bits 8.1.14.3
void	fibDecoder::FIG1Extension4 (uint8_t *d) {
char		label [17];
int		bitOffset = 16;
uint32_t	SId;

//      from byte 1 we deduce:
	const uint8_t charSet	= getBits_4 (d, 8);
	const uint8_t Rfu	= getBits_1 (d, 8 + 4);
	const uint8_t extension	= getBits_3 (d, 8 + 5);
	const uint8_t PD_flag	= getBits_1 (d, bitOffset);
	const uint8_t SCIds	= getBits   (d, bitOffset + 4, 4);
	if (PD_flag) {
	   SId	= getLBits  (d, bitOffset + 8, 32);
	   bitOffset += 32 + 8;
	}
	else {
	   SId = getLBits (d, bitOffset + 8, 16);
	   bitOffset += 16 + 8;
	}
//
//	just a check if we already have the servicename
	for (auto &serv : theEnsemble. secondaries)
	   if (serv. SId == SId)
	      return;
	for (auto &serv :theEnsemble. primaries)
	   if (serv. SId == SId)
	     return;
	
	label [16]      = 0x00;
	(void)Rfu;
	(void)extension;
	if (charSet >= 16) 	// does not seem right
	   return;

	for (int i = 0; i < 16; i ++) 
	   label [i] = getBits_8 (d, bitOffset + 8 * i);
	QString dataName = toQStringUsingCharset (
	                                  (const char *) label,
	                                  (CharacterSet) charSet);
	QString shortName;		
	for (int i = 0; i < 16; i ++) 
	   if (getBits_1 (d, bitOffset + 16 * 8 + i) != 0)
	      shortName. append (dataName. at (i));

	ensemble::service seco;
	seco. name 	= dataName;
	seco. shortName	= shortName;
	seco. SId	= SId;
	seco. SCIds	= SCIds;
	theEnsemble. secondaries. push_back (seco);
//
//	if a secondary service has the name of an existing primary one,
//	we do not want it in the list
	for (auto &prim : theEnsemble. primaries)
	   if (prim. name == seco. name)
	      return;
	addToEnsemble (dataName, SId, -1);
}

//	Data service label - 32 bits 8.1.14.2
void	fibDecoder::FIG1Extension5 (uint8_t *d) {
char		label [17];

uint32_t	SId	= getLBits (d, 16, 32);
int16_t		bitOffset	= 48;

//      from byte 1 we deduce:
	uint8_t charSet	= getBits_4 (d, 8);
	uint8_t Rfu	= getBits_1 (d, 8 + 4);
	uint8_t	extension	= getBits_3 (d, 8 + 5);
	label [16]      = 0x00;
	(void)Rfu; (void)extension;

	for (auto &serv : theEnsemble. primaries) {
	   if (SId == serv. SId) 
	      return;
	}

	if (charSet > 16) 
	   return;	// something wrong

	for (int i = 0; i < 16; i ++) {
	   label [i] = getBits_8 (d, bitOffset + 8 * i);
	}
	QString dataName = toQStringUsingCharset (
	                                  (const char *) label,
	                                  (CharacterSet) charSet);
	QString shortName;		
	for (int i = 0; i < 16; i ++) 
	   if (getBits_1 (d, bitOffset + 16 * 8 + i) != 0)
	      shortName. append (dataName. at (i));


	ensemble::service prim;
	prim. programType	= 0;
	prim. name 	= dataName;
	prim. shortName = shortName;
	prim. SId	= SId;
	theEnsemble. primaries. push_back (prim);
	addToEnsemble (dataName, SId, -1);
}
//
//////////////////////////////////////////////////////////////////////

void	fibDecoder::connectChannel () {
	fibLocker. lock();
	currentConfig	-> reset ();
	nextConfig	-> reset ();
	theEnsemble.	reset ();
	connect (this, &fibDecoder::addToEnsemble,
	         myRadioInterface, &RadioInterface::addToEnsemble);
	fibLocker. unlock();
}

void	fibDecoder::disconnectChannel () {
	fibLocker. lock ();
	disconnect (this, &fibDecoder::addToEnsemble,
	            myRadioInterface, &RadioInterface::addToEnsemble);
	currentConfig	-> reset ();
	nextConfig	-> reset ();
	theEnsemble.	reset ();
	fibLocker. unlock();
}
//
//	ofdmHandler ask for syncReached
bool	fibDecoder::syncReached() {
	return  theEnsemble. isSynced;
}

uint32_t fibDecoder::getSId	(const int index) {
	return currentConfig -> SC_C_table [index]. SId;
}

uint8_t	fibDecoder::serviceType (const int index) {
	return currentConfig -> SC_C_table [index]. TMid;
}

void	fibDecoder::audioData	(const int index, audiodata &ad) {
fibConfig::serviceComp_C &comp = currentConfig -> SC_C_table [index];
	for (auto &serv : theEnsemble. primaries) {
	   if (serv. SId == comp. SId) {
	      ad. serviceName	= serv. name;
	      ad. shortName	= serv. shortName;
	      ad. SId		= serv. SId;
	      ad. programType	= serv. programType;
	      ad. fmFrequencies	= serv. fmFrequencies;
	      break;
	   }
	}
	int subChId	= currentConfig -> subChannelOf (index);
	ad. subchId	= subChId;
	int subChannel_index =
	               currentConfig -> findIndex_subChannel_table (subChId);
	if (subChannel_index < 0)
	   return;
	fibConfig::subChannel &channel =
	               currentConfig -> subChannel_table [subChannel_index];
	ad. startAddr	= channel. startAddr;	
	ad. shortForm	= channel. shortForm;
	ad. protLevel	= channel. protLevel;
	ad. length	= channel. Length;
	ad. bitRate	= channel. bitRate;
	ad. ASCTy	= currentConfig -> dabTypeOf (index);
	ad. language	= currentConfig -> languageOf (index);
	ad. defined	= true;
}

void	fibDecoder::packetData		(const int index, packetdata &pd) {
fibConfig::serviceComp_C &comp = currentConfig -> SC_C_table [index];
	for (auto &serv : theEnsemble. primaries) {
	   if (serv. SId == comp. SId) {
	      pd. serviceName	= serv. name;
	      pd. shortName	= serv. shortName;
	      pd. SId		= serv. SId;
	      break;
	   }
	}
	int subChId	= currentConfig -> subChannelOf (index);
	pd. subchId	= subChId;
	int subChannel_index =
	               currentConfig -> findIndex_subChannel_table (subChId);
	if (subChannel_index < 0)
	   return;
	fibConfig::subChannel &channel =
	               currentConfig -> subChannel_table [subChannel_index];
	pd. startAddr	= channel. startAddr;	
	pd. shortForm	= channel. shortForm;
	pd. protLevel	= channel. protLevel;
	pd. length	= channel. Length;
	pd. bitRate	= channel. bitRate;
	pd. FEC_scheme	= currentConfig -> FEC_schemeOf (index);
	pd. appType	= currentConfig -> appTypeOf (index);
	pd. DGflag	= currentConfig -> DG_flag (index);
	pd. DSCTy	= currentConfig -> DSCTy (index);
	pd. packetAddress = currentConfig -> packetAddressOf (index);
	pd. defined = true;
}

int	fibDecoder::getNrComps			(const uint32_t SId) {
	for (auto &SId_element : currentConfig -> SId_table)
	   if (SId_element. SId == SId)
	      return SId_element. comps. size ();
	return 0;
}
//
//	for primary services we return the index of the first
//	component, the secondary services, the index of the
//	component with the matching SCIds
//	
int	fibDecoder::getServiceComp		(const QString &service) {
//	first we check to see if the service is a primary one
	for (auto &serv : theEnsemble. primaries) {
	   if (serv. name != service)
	      continue;
	   for (auto & SId_element: currentConfig -> SId_table) {
	      if (SId_element. SId == serv. SId)
	         return SId_element. comps [0];
	   }
	}
	
	for (auto &serv : theEnsemble. secondaries) {
	   if (serv. name != service)
	      continue;
	   return getServiceComp_SCIds (serv. SId, serv. SCIds);
	}
	return -1;
}

int	fibDecoder::getServiceComp		(const uint32_t SId,
	                                         const int compnr) {
	for (auto &SId_element : currentConfig -> SId_table) {
	   if (SId_element. SId == SId) {
	      return SId_element. comps [compnr];
	   }
	}
	return -1;
}

int	fibDecoder::getServiceComp_SCIds	(const uint32_t SId,
	                                         const int SCIds) {
//	fprintf (stderr, "Looking for serviceComp %X %d\n", SId, SCIds);
	for (auto &SId_element : currentConfig -> SId_table) {
	   if (SId_element. SId != SId)
	      continue;
	   for (int i = 0; i < (int) SId_element. comps. size (); i ++) {
	      int index = SId_element. comps [i];
	      if (currentConfig -> SCIdsOf   (index) == SCIds)
	         return index;
	   }
	}
	return -1;
}

bool	fibDecoder::isPrimary	(const QString &s) {
	for (auto &serv : theEnsemble. primaries) {
	   if (s == serv. name)
	      return true;
	}
	return false;
}
	
std::vector<int> fibDecoder::getFrequency	(const QString &s) {
std::vector<int> res;
	for (auto &serv : theEnsemble. primaries) {
	   if (serv. name == s)
	      return serv. fmFrequencies;
	}
	return  res;
}
	   
//	required for ETI generation
int	fibDecoder::nrChannels	() {
	return currentConfig -> subChannel_table. size ();
}
//
//	needed for generating eti files
void	fibDecoder::getChannelInfo (channel_data *d, const int n) {
	d       -> in_use	= true;
	d       -> id		= currentConfig -> subChannel_table [n]. subChId;
	d       -> start_cu	= currentConfig -> subChannel_table [n]. startAddr;
	d       -> protlev	= currentConfig -> subChannel_table [n]. protLevel; 
	d       -> size		= currentConfig -> subChannel_table [n]. Length;
	d       -> bitrate	= currentConfig -> subChannel_table [n]. bitRate;
	d       -> uepFlag	= currentConfig -> subChannel_table [n]. shortForm;
}

int32_t	fibDecoder::getCIFcount		() {
	return CIFcount;
}
	
void	fibDecoder::getCIFcount		(int16_t &high, int16_t &low) {
	high	= CIFcount_hi;
	low	= CIFcount_lo;
}

uint32_t fibDecoder::julianDate		() {
	return mjd;
}

void	fibDecoder::handleAnnouncement (uint16_t SId, uint16_t flags,
	                                                uint8_t subChId) {
	(void)subChId;
	for (auto &serv : currentConfig -> SId_table)
	   if (serv. SId == SId) {
	      if (serv. announcing != flags)
	         emit announcement (SId, flags);
	      serv. announcing = flags;
	   }
}

uint16_t fibDecoder::getAnnouncing	(uint16_t SId) {
	for (auto &serv : currentConfig -> SId_table)
	   if (serv. SId == SId)
	      return serv. announcing;
	return 0;
}

int	fibDecoder::freeSpace		() {
	return currentConfig -> freeSpace ();
}
//
//	Some GUI functions, such as content printer and
//	printer with the scan function, need to get a description
//	of the attributes of the service.
//	Here we collect all attributes as specified by "contentType"
//	
QList<contentType> fibDecoder::contentPrint () {
QList<contentType> res;
	for (int i = 0; i < (int)(currentConfig -> SC_C_table. size ()); i ++) {
	   fibConfig::serviceComp_C &comp = currentConfig -> SC_C_table [i];
	   contentType theData;
	   theData. TMid	= comp. TMid;
	   theData. SId		= comp. SId;
	   theData. isActive	= false;
	   if (comp. TMid == 0) {	// audio data
	      audiodata ad;
	      audioData (i, ad);
	      if (!ad. defined)		// should not happen
	         continue;
	      theData. serviceName	= ad. serviceName;
	      theData. subChId		= ad. subchId;
	      theData. SCIds		= ad. SCIds;
	      theData. startAddress	= ad. startAddr;
	      theData. length		= ad. length;
	      theData. codeRate		= getCodeRate (ad. shortForm,
	                                               ad. protLevel);
	      theData. protLevel	= getProtectionLevel (ad. shortForm,
	                                                      ad. protLevel);
	      theData. bitRate		= ad. bitRate;
	      theData. language		= ad. language;
	      theData. FEC_scheme	= 0;
	      theData. packetAddress	= 0;
	      theData. ASCTy_DSCTy	= ad. ASCTy;
	      theData. programType	= theEnsemble. programType (ad. SId);
	      theData. fmFrequencies	= theEnsemble. fmFrequencies (ad. SId);
	      res. push_back (theData);
	   }
	   else
	   if (comp. TMid == 3) {	// packet 
	      packetdata pd;
	      packetData (i, pd);
	      if (!pd. defined)		// should not happen
	         continue;
	      theData. serviceName	= pd. serviceName;
	      theData. subChId		= pd. subchId;
	      theData. SCIds		= pd. SCIds;
	      theData. startAddress	= pd. startAddr;
	      theData. length		= pd. length;
	      theData. codeRate		= getCodeRate (pd. shortForm,
	                                               pd. protLevel);
	      theData. protLevel	= getProtectionLevel (pd. shortForm,
	                                                      pd. protLevel);
	      theData. bitRate		= pd. bitRate;
	      theData. FEC_scheme	= pd. FEC_scheme;
	      theData. packetAddress	= pd. packetAddress;
	      theData. ASCTy_DSCTy	= pd. DSCTy;
	      theData. appType		= pd. appType;
	      theData. language		= 0;
	      theData. programType	= 0;
	      res. push_back (theData);
	   }
	}
	return res;
}
