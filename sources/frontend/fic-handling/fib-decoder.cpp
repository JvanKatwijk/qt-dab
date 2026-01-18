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
 *	Somehe
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
//
//
//	The fibDecoder was rewritten since the "old" one
//	contained (a) errors and (b) was incomplete on
//	some issues.
//	The current one is a straight forward implementation,
//	where the FIG's are stored in a (kind of) database
//	maintained in a class fibConfig
//
//	Since the threads filling the database, and reading the
//	database are different, a simple locking scheme is applied,

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
	connect (this, &fibDecoder::tell_programType,
	         myRadioInterface, &RadioInterface::tell_programType);
//
//	Note that they may change "roles", 
	currentConfig	= new fibConfig	(&theEnsemble, myRadioInterface);
	nextConfig	= new fibConfig (&theEnsemble, myRadioInterface);
	mjd		= 0;
}
	
	fibDecoder::~fibDecoder () {
	delete	nextConfig;
	delete	currentConfig;
}

//	FIB's are segments of 256 bits. When here, the segments already
//	passed the crc and we start unpacking the bits into FIGs
void	fibDecoder::processFIB (uint8_t *p, uint16_t fib) {
int8_t	processedBytes	= 0;
uint8_t	*d		= p;

	fibLocker. lock();
	(void)fib;
	while (processedBytes  < 30) {
	   uint8_t FIGtype	= getBits_3 (d, 0);
	   uint8_t FIGlength	= getBits_5 (d, 3);
	   if ((FIGtype == 0x07) && (FIGlength == 0x3F)) {
	      fibLocker. unlock ();
	      return;
	   }

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
				// not implemented
	      break;

	   case 5:		// service component language (8.1.2)
	      FIG0Extension5 (d);
	      break;

	   case 6:		// service linking information (8.1.15)
	                        // not implemented
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

	   case 11:		// Obsolete
	      break;

	   case 12:		// Obsolete
	      break;

	   case 13:             // user application information (6.3.6)
	      FIG0Extension13 (d);
	      break;

	   case 14:             // FEC subchannel organization (6.2.2)
	      FIG0Extension14 (d);
	      break;

	   case 15:		// Emergency warning (ETSI TS 104 089)
	      FIG0Extension15 (d);
	      break;

	   case 16:		// Obsolete
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

	   case 22:		// Obsolete
	      break;

	   case 23:		// Reserved
	      break;

	   case 24:		// OE services (8.1.10)
	      break;		// not implemented

	   case 25:		// OE announcement support (8.1.6.3)
	      break;		// not implemented

	   case 26:		// OE announcement switching (8.1.6.4)
	      break;		// not implemented

	   case 27:		// Obsolete
	   case 28:		// Obsolete
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
const uint8_t CN_bit	= getBits_1 (d, 8 + 0);
uint16_t        EId;
uint8_t         changeFlag;
uint16_t        highpart, lowpart;
int16_t         occurrenceChange;
uint8_t		alarmFlag;
static	uint8_t prevChangeFlag	= 0;

	(void)CN_bit;
	EId                     = getBits   (d, 16, 16);
	(void)EId;
	changeFlag              = getBits_2 (d, 16 + 16);
	alarmFlag		= getBits_1 (d, 16 + 16 + 2);
	highpart                = getBits_5 (d, 16 + 19);
	lowpart                 = getBits_8 (d, 16 + 24);
	(void)alarmFlag;
	occurrenceChange        = getBits_8 (d, 16 + 32);
	(void)occurrenceChange;
	CIFcount_hi. store (highpart);
	CIFcount_lo. store (lowpart);


	if ((changeFlag == 0) && (prevChangeFlag == 3)) {
	   fibConfig 	*temp	= currentConfig;
	   currentConfig	= nextConfig;
	   nextConfig		= temp;
	   nextConfig		->  reset ();
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
const int16_t	Length			= getBits_5 (d, 3);
const uint8_t	CN_bit	= getBits_1 (d, 8 + 0);
const uint8_t	OE_bit	= getBits_1 (d, 8 + 1);
const uint8_t	PD_bit	= getBits_1 (d, 8 + 2);

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
fibConfig *localBase = CN_bit == 0 ? currentConfig : nextConfig;
static	int table_1 [] = {12, 8, 6, 4};
static	int table_2 [] = {27, 21, 18, 15};

	(void)OE_bit; (void)PD_bit;
	channel. subChId	= subChId;
	channel. startAddr	= startAdr;
	channel. Length		= 0;	// will change
	channel. FEC_scheme	= 0;	// corrected later on

	if (getBits_1 (d, bitOffset + 16) == 0) {	// short form
	   tabelIndex		= getBits_6 (d, bitOffset + 18);
	   channel. Length	= ProtLevel [tabelIndex][0];
	   channel. shortForm	= true;		// short form
	   channel. protLevel	= ProtLevel [tabelIndex][1];
	   channel. bitRate	= ProtLevel [tabelIndex][2];
	   bitOffset += 24;
	}
	else { 	// EEP long form
	   channel. shortForm	= false;
	   option = getBits_3 (d, bitOffset + 17);
	   if (option == 0) { 		// A Level protection
	      protLevel		= getBits (d, bitOffset + 20, 2);
	      channel.	protLevel	= protLevel;
	      chanSize		= getBits (d, bitOffset + 22, 10);
	      channel. Length	= chanSize;
	      channel. bitRate	= chanSize / table_1 [protLevel] * 8;
	   }
	   else			// option should be 001
	   if (option == 001) {		// B Level protection
	      protLevel		= getBits_2 (d, bitOffset + 20);
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
	if (localBase -> subChId_exists (subChId))
	   return bitOffset / 8;
//
	localBase -> add_to_subChannel_table (channel);
	return bitOffset / 8;	// we return bytes
}
//
//	Service organization, 6.3.1
//	bind channels to SIds
void	fibDecoder::FIG0Extension2 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
const int16_t	Length	= getBits_5 (d, 3);
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
	if (localBase -> SId_exists (SId)) {
	   bitOffset += numberofComponents * 16 + 8;
	   return bitOffset / 8;
	}

	SId_element. announcing = 0;
	SId_element. SId = SId;
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
	   }
	   else 
	      {;}
	   bitOffset += 16;
	   if (!localBase -> compIsKnown (comp)) {	
	      int index =  localBase -> add_to_SC_C_table (comp);
	      SId_element. comps. push_back (index);
	   }
	}
	localBase -> add_to_SId_table (SId_element);
	return bitOffset / 8;		// in Bytes
}

//	Service component in packet mode 6.3.2
void	fibDecoder::FIG0Extension3 (uint8_t *d) {
int16_t used    = 2;            // offset in bytes
const int16_t Length  = getBits_5 (d, 3);
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

	if (localBase -> SCId_exists (SCId))
	   return used;
	fibConfig::serviceComp_P element;
	element. SCId  		= SCId;
	element. subChId  	= SubChId;
	element. DSCTy		= DSCTy;
	element. DG_flag	= DGflag;
	element. packetAddress	= packetAddress;
	localBase -> add_to_SC_P_table (element);
	return used;
}

//	Service component language 8.1.2
void	fibDecoder::FIG0Extension5 (uint8_t *d) {
int16_t	used		= 2;		// offset in bytes
const int16_t Length	= getBits_5 (d, 3);
const uint8_t CN_bit	= getBits_1 (d, 8 + 0);
const uint8_t OE_bit	= getBits_1 (d, 8 + 1);
const uint8_t PD_bit	= getBits_1 (d, 8 + 2);

	while (used < Length) {
	   used = HandleFIG0Extension5 (d, used,  CN_bit, OE_bit, PD_bit);
	}
}

int16_t	fibDecoder::HandleFIG0Extension5 (uint8_t	*d,
	                                  uint16_t 	offset,
	                                  const uint8_t CN_bit,
	                                  const uint8_t OE_bit,
	                                  const uint8_t PD_bit) {
int16_t	bitOffset	= offset * 8;
const uint8_t	LS_flag	= getBits_1 (d, bitOffset);
fibConfig	*localBase	= CN_bit == 0 ? currentConfig : nextConfig;
fibConfig::SC_language comp;

	(void)OE_bit; (void)PD_bit;

	comp. LS_flag = LS_flag;
	if (LS_flag == 0) {
	   comp. subChId = getBits (d, bitOffset + 2, 6);
	   comp. SCId	= 255;
	   uint8_t language = getBits (d, bitOffset + 8, 8);
	   if (localBase -> subChId_exists (comp. subChId)) 
	      comp. language = language;
	   bitOffset += 16;
	   if (localBase -> language_comp_exists (comp. subChId))
	      return bitOffset / 8;

	   localBase -> add_to_language_table (comp);
	   return bitOffset / 8;
	}
	else {
	   comp. SCId = getBits (d, bitOffset + 4, 12);
	   comp. subChId = 255;
	   comp. language = 0;
	   uint8_t language = getBits (d, bitOffset + 16, 8);
	   bitOffset += 24;

	   int res = localBase -> subChId_in_SCId (comp. SCId);
	   if (res < 0) 	// not found yet
	      return bitOffset / 8;
	   comp. language = language;
	   if (localBase -> language_comp_exists (comp. SCId))
	      return bitOffset / 8;

	   localBase -> add_to_language_table (comp);
	   return bitOffset / 8;
	}
}
//
// FIG0/7: Configuration linking information 6.4.2,
void    fibDecoder::FIG0Extension7 (uint8_t *d) {
int16_t used		= 2;            // offset in bytes
const int16_t Length	= getBits_5 (d, 3);
const uint8_t CN_bit	= getBits_1 (d, 8 + 0);
const uint8_t OE_bit	= getBits_1 (d, 8 + 1);
const uint8_t PD_bit	= getBits_1 (d, 8 + 2);

const int serviceCount	= getBits_6 (d, used * 8);
int     counter		= getBits   (d, used * 8 + 6, 10);

	(void)Length; (void)OE_bit; (void)PD_bit;

	if (CN_bit == 0)	// only current configuration for now
	   nrServices (serviceCount);
	(void)counter;
}

// FIG0/8:  Service Component Global Definition (6.3.5)
void	fibDecoder::FIG0Extension8 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
const int16_t	Length	= getBits_5 (d, 3);
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

	if (localBase -> SC_G_element_exists (comp.SId, comp. SCIds))
	   return bitOffset / 8;

	localBase -> add_to_SC_G_table (comp);
	return bitOffset / 8;
}

//	FIG0/9 Country, LTO and International table, clause 8.1.3.2;
void	fibDecoder::FIG0Extension9 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
//int16_t	Length		= getBits_5 (d, 3);
//uint8_t	CN_bit		= getBits_1 (d, 8 + 0);
//uint8_t	OE_bit		= getBits_1 (d, 8 + 1);
//uint8_t	PD_bit		= getBits_1 (d, 8 + 2);
//	bit 6 indicates the number of hours
        const int signbit = getBits_1 (d, used * 8 + 2);
        currentConfig -> dateTime [6] = (signbit == 1)?
                                         -1 * getBits_4 (d, used * 8 + 3):
                                         getBits_4 (d, used * 8 + 3);
//	bit 7 indicates a possible remaining half our
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
int16_t	used			= 2;		// offset in bytes
int16_t	length			= getBits_5 (d, 3);
const uint8_t	CN_bit		= getBits_1 (d, 8 + 0);
const uint8_t	OE_bit		= getBits_1 (d, 8 + 1);
const uint8_t	PD_bit		= getBits_1 (d, 8 + 2);

	while (used < length) 
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
int16_t		appType;
fibConfig	*localBase	= CN_bit == 0 ? currentConfig : nextConfig;
fibConfig::AppType element;

	(void)OE_bit;
	bitOffset		+= pdBit == 1 ? 32 : 16;
	uint16_t SCIds		= getBits_4 (d, bitOffset);
	int16_t NoApplications	= getBits_4 (d, bitOffset + 4);
	bitOffset		+= 8;
	element. SId		= SId;
	element. SCIds		= SCIds;
	for (int i = 0; i < NoApplications; i ++) {
	   appType		= getBits (d, bitOffset, 11);
	   int16_t length	= getBits_5 (d, bitOffset + 11);
	   element. Apptype	= appType;
	   bitOffset 		+= (11 + 5 + 8 * length);
	}
	if (localBase -> findIndexApptype_table (SId, SCIds) != -1)
	   return bitOffset / 8;
	localBase -> add_to_apptype_table (element);
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
	   localBase	-> set_FECscheme (subChId, FEC_scheme);
	}
}
//
//	
void	fibDecoder::FIG0Extension15 (uint8_t *d) {
//int16_t Length          = getBits_5 (d, 3);     // in Bytes
const uint8_t   CN_bit  = getBits_1 (d, 8 + 0);
const uint8_t   OE_bit  = getBits_1 (d, 8 + 1);
const uint8_t   PD_bit  = getBits_1 (d, 8 + 2);
int16_t used    = 2;                    // in Bytes
int16_t bitOffset               = used * 8;
uint8_t secondsCount		= 0;

	if (CN_bit == 1)	// Next config, not implemented yet
	   return;
	if (PD_bit == 1)	// discard
	   return;
	if (OE_bit == 1)	// other ensemble, not implemented
	   return;
//
//	Handling the Id field
	uint8_t phase		= getBits_2 (d, bitOffset);
	uint8_t subChId		= getBits_6 (d, bitOffset + 2);
	bitOffset		+= 8;
	if (phase == 00) {
	   secondsCount		= getBits_6 (d, bitOffset + 2);
	   bitOffset		+= 8;
	}
	(void)secondsCount;
	(void)phase;
	(void)subChId;
//	Handling the Statusfield
	bitOffset += 8;
//	Handling the location codes
//	to be researched
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
	         if (theEnsemble. primaries [i]. programType == 0)
	            tell_programType (SId, typeCode);
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
int16_t	used		= 2;			// in Bytes
int16_t	bitOffset	= used * 8;
fibConfig *localBase	= CN_bit == 0 ? currentConfig : nextConfig;

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
	      uint8_t clusterId = getBits (d, bitOffset + 8 * i, 8);
	      if (clusterId == 0)
	         continue;
	      if (!localBase -> announcement_exists (SId, clusterId)) {
	         fibConfig::FIG18_cluster aC;
	         aC. SId = SId;
	         aC. asuFlags = asuFlags;
	         aC. clusterId = clusterId;
	         localBase -> add_to_announcement_table (aC);
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
//fibConfig *localBase	= CN_bit == 0 ? currentConfig : nextConfig;

	if (CN_bit != 0)	// next config
	   return;
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
//	   fprintf (stderr, "%d %d %d -> %d\n",
//	                 clusterId, AswFlags, newFlag, subChId);
	   currentConfig -> check_announcements (clusterId, AswFlags, newFlag);
	}
}
//
//	Service Component Information
//	Not encountered yet
void	fibDecoder::FIG0Extension20 (uint8_t *d) {
int16_t	used		= 2;		// offset in bytes
const int16_t	Length	= getBits_5 (d, 3);
const uint8_t	CN_bit	= getBits_1 (d, 8 + 0);
const uint8_t	OE_bit	= getBits_1 (d, 8 + 1);
const uint8_t	PD_bit	= getBits_1 (d, 8 + 2);
#ifndef	__MINGW32__
	static bool shown = false;
	if (!shown) {
	   if (theEnsemble. namePresent) {
	      fprintf (stderr, "FIG0/20 appears in %s\n",
	                           theEnsemble. ensembleName.
	                                     toLatin1 (). data ());
	      shown = true;
	   }
	}
#endif
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
//	fprintf (stderr, "%X (%d) is in fig 20\n", SId, SCIds); 
	(void)SId; (void)SCIds;
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
/////////////////////////end of FIG0//////////////////////////////////

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
//	charSet 0 .. 15, 4 bits, checks are superfluous
	const uint8_t Rfu	= getBits_1 (d, 8 + 4);
	label [16]      = 0x00;
	(void)Rfu;

	const uint16_t EId	= getBits (d, 16, 16);
	offset	= 32;
	for (int i = 0; i < 16; i ++) {
	   label [i] = getBits_8 (d, offset + 8 * i);
	}
	const QString name = toQStringUsingCharset (
	                                     (const char *) label,
	                                     (CharacterSet) charSet);
	QString realName = name;
	if (!theEnsemble. namePresent) {
	   theEnsemble. ensembleName	= realName;
	   theEnsemble. EId		= EId;
	   theEnsemble. namePresent	= true;
	   ensembleName (EId, name);
	}
	theEnsemble. isSynced = true;
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
	for (auto &serv : theEnsemble. primaries) {
	   if (SId == serv. SId) 
	      return;
	}
//
//	if no subchannel is found for the service, we do not record it yet
	int subChId	= currentConfig -> subChId_for_SId (0, SId);
	if (subChId < 0)
	   return;

	for (int i = 0; i < 16; i ++) 
	   label [i] = getBits_8 (d, offset + 8 * i);
	QString dataName = toQStringUsingCharset (
	                                  (const char *) label,
	                                  (CharacterSet) charSet);

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
//
//	If no subch is know (yet), we do not record the service component
	int subChId	= currentConfig -> subChId_for_SId (SCIds, SId);
	if (subChId < 0)
	   return;

	if (currentConfig -> findIndexApptype_table (SId, 0) < 0)
	   return;	// we want the apptype to be available

	label [16]      = 0x00;
	(void)Rfu;
	(void)extension;
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
uint8_t charSet		= getBits_4 (d, 8);
uint8_t Rfu		= getBits_1 (d, 8 + 4);
uint8_t	extension	= getBits_3 (d, 8 + 5);

	label [16]      = 0x00;
	(void)Rfu; (void)extension;

	for (auto &serv : theEnsemble. primaries) {
	   if (SId == serv. SId) 
	      return;
	}
//
//	if no subch is known (yet) we do not record the service yet
	int subChId	= currentConfig -> subChId_for_SId (0, SId);
	if (subChId < 0)
	   return;
	if (currentConfig -> findIndexApptype_table (SId, 0) < 0)
	   return;	// we want the apptype to be available
//
//	It seems the service is (more or less) complete
	for (int i = 0; i < 16; i ++) {
	   label [i] = getBits_8 (d, bitOffset + 8 * i);
	}
	QString dataName = toQStringUsingCharset (
	                                  (const char *) label,
	                                  (CharacterSet) charSet);
	if (dataName. size () < 16)
	   fprintf (stderr, "a");
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
//////////////////////end of FIG1 ///////////////////////////////////////////

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
//	ofdmHandler asks for syncReached
bool	fibDecoder::syncReached() {
	return  theEnsemble. isSynced;
}
///////////////////user functions//////////////////////////////////////
//
//	The GUI (and others) ask for on data  that is  (at least
//	superficially) based on the servicename
//	We focus on the components, so (almost)
//	all enquiries take an component (or index to the
//	component table) as parameter
//
//	for primary services we return the index of the first
//	component, the secondary services, the index of the
//	component with the matching SCIds
//	Note that components here are identified by an integer number,
//	a key to the database;
//	Notice further that the actual extraction and interpretation
//	of the database contents is done in "fibConfig",
//	here we provide the interface with locking
//	
int	fibDecoder::getServiceComp		(const QString &service) {
int res;
	fibLocker. lock ();
	res = currentConfig -> getServiceComp (service);
	fibLocker. unlock ();
	return res;
}
//
//	Find the component with the indicated number
int	fibDecoder::getServiceComp		(const uint32_t SId,
	                                                const int compnr) {
int res;
	fibLocker. lock ();
	res = currentConfig -> getServiceComp (SId, compnr);
	fibLocker. unlock ();
	return res;
}

//	Find the component with the indicated SCIds
int	fibDecoder::getServiceComp_SCIds	(const uint32_t SId,
	                                         const int SCIds) {
int res;
	fibLocker. lock ();
	res = currentConfig ->  getServiceComp_SCIds (SId, SCIds);
	fibLocker. unlock ();
	return res;
}

uint32_t fibDecoder::getSId	(const int index) {
uint32_t	res;
	fibLocker. lock ();
	res = currentConfig -> getSId (index);
	fibLocker. unlock ();
	return res;
}

uint8_t	fibDecoder::serviceType (const int index) {
uint8_t res;
	fibLocker. lock ();
	res = currentConfig -> serviceType (index);
	fibLocker. unlock ();
	return res;
}

int	fibDecoder::getNrComps			(const uint32_t SId) {
int	res;
	fibLocker. lock ();
	res =  currentConfig -> getNrComps (SId);
	fibLocker. unlock ();
	return res;
}
//
//		
bool	fibDecoder::isPrimary	(const QString &s) {
	for (auto &serv : theEnsemble. primaries) {
	   if (s == serv. name)
	      return true;
	}
	return false;
}
	
void	fibDecoder::audioData	(const int index, audiodata &ad) {
	fibLocker. lock ();
	currentConfig -> audioData (index, ad);
	fibLocker. unlock ();
}

void	fibDecoder::packetData		(const int index, packetdata &pd) {
	fibLocker. lock ();
	currentConfig -> packetData (index, pd);
	fibLocker. unlock ();
}

uint16_t fibDecoder::getAnnouncing	(uint16_t SId) {
uint8_t res;
	fibLocker. lock ();
	res = currentConfig -> getAnnouncing (SId);
	fibLocker. unlock ();
	return res;
}

std::vector<int> fibDecoder::getFrequency	(const QString &s) {
std::vector<int> res;
	for (auto &serv : theEnsemble. primaries) {
	   if (serv. name == s)
	      return serv. fmFrequencies;
	}
	return  res;
}
	   
//
//	needed for generating eti files
//	required for ETI generation
void	fibDecoder::getCIFcount		(int16_t &high, int16_t &low) {
	high	= CIFcount_hi. load ();
	low	= CIFcount_lo. load ();
}

int	fibDecoder::nrChannels	() {
int res;
	fibLocker. lock ();
	res = currentConfig -> nrChannels ();
	fibLocker. unlock ();
	return res;
}

void	fibDecoder::getChannelInfo (channel_data *d, const int n) {
	fibLocker. lock ();
	currentConfig -> getChannelInfo (d, n);
	fibLocker. unlock ();
}

bool	fibDecoder::nonTIIFrame		() {
	return (CIFcount_lo. load () & 0x07) >= 4;
}

//

uint32_t fibDecoder::julianDate		() {
	return mjd;
}
//
int	fibDecoder::freeSpace		() {
	return currentConfig -> freeSpace ();
}
//
QList<contentType> fibDecoder::contentPrint () {
QList<contentType> res;
	fibLocker. lock ();
	res = currentConfig -> contentPrint ();
	fibLocker. unlock ();
	return res;
}

bool	fibDecoder::is_SPI	(const uint32_t SId) {
bool res;
	fibLocker. lock ();
	res = currentConfig -> is_SPI (SId);
	fibLocker. unlock ();
return res;
}

std::vector<basicService> fibDecoder::getServices	() {
	return theEnsemble. getServices ();
}

