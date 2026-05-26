#
/*
 *    Copyright (C) 2018 .. 2026
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
 *	To get things manageable, we just (almost) copy the values of the
 *	FIG's in local structures (with some basic interpretation), and
 *	extract what we need from different FIG stacks and values.
 *	The stacks for those FIGs that have a CN_bit that is "on" are
 *	stored in the "fib-config" file, such that we can implement
 *	"current" and "next" properly
 *	Somehe
 */
#include	<QStringList>
#include	"fib-decoder.h"
#include	<cstring>
#include	<vector>
#include	"radio.h"
#include	"charsets.h"
#include	"bit-extractors.h"
#include	"fib-config.h"
#include	"fib-table.h"
#include	"ITU_tables.h"
#include	"time-converter.h"
//
//

	fibDecoder::fibDecoder (RadioInterface *mr) {
	myRadioInterface	= mr;
//
//	Note that they may change "roles", 
	currentConfig	= new fibConfig	(myRadioInterface);
	nextConfig	= new fibConfig (myRadioInterface);
	mjd		= 0;
	reset ();
	connect (this, &fibDecoder::signal_FIG00,
	         myRadioInterface, &RadioInterface::handle_FIG00);
	connect (this, &fibDecoder::signal_FIG09,
	         myRadioInterface, &RadioInterface::handle_FIG09);
	connect (this, &fibDecoder::signal_FIG010,
	         myRadioInterface, &RadioInterface::handle_FIG010);
	connect (this, &fibDecoder::signal_FIG019,
	         myRadioInterface, &RadioInterface::handle_FIG019);
	connect (this, &fibDecoder::signal_FIG021,
	         myRadioInterface, &RadioInterface::handle_FIG021);
	connect (this, &fibDecoder::signal_FIG10,
	         myRadioInterface, &RadioInterface::handle_FIG10);
	connect (this, &fibDecoder::signal_FIG11,
	         myRadioInterface, &RadioInterface::handle_FIG11);
	connect (this, &fibDecoder::signal_FIG14,
	         myRadioInterface, &RadioInterface::handle_FIG14);
	connect (this, &fibDecoder::signal_FIG15,
	         myRadioInterface, &RadioInterface::handle_FIG15);
}
	
	fibDecoder::~fibDecoder () {
	delete	nextConfig;
	delete	currentConfig;
}

//	FIB's are segments of 256 bits. When here, the segments already
//	passed the crc and we start unpacking the bits into FIGs
void	fibDecoder::processFIB (uint8_t *p, uint16_t fib) {
int8_t	availableBytes	= 30;
uint8_t	*d		= p;

	(void)fib;
	while (availableBytes > 0) {
	   uint8_t FIGtype	= getBits_3 (d, 0);
	   uint8_t FIGlength	= getBits_5 (d, 3);
	   if ((FIGlength >= availableBytes) ||
	       ((FIGtype == 0x07) && (FIGlength == 0x3F))) {
	      return;
	   }

	   switch (FIGtype) {
	      case 0:
	         if (availableBytes >= 2)
	            process_FIG0 (d);	
	         break;

	      case 1:			
	         if (availableBytes >= 2) 
	            process_FIG1 (d);
	         break;

	      case 2:		// not encountered yet
	         fprintf (stderr, "FIG2 label\n");
	         break;

	      case 7:
	         break;

	      default:
	         break;
	   }
//
	   availableBytes -= (FIGlength + 1);
	   d = d + (FIGlength + 1) * 8;
	}
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
	      FIG0Extension6 (d);
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
//	The info is MCI, flags are Rfu
void	fibDecoder::FIG0Extension0 (uint8_t *d) {

	FIG00_value. EId	=  getBits   (d, 16, 16);
	FIG00_value. changeFlag = getBits_2 (d, 16 + 16);
	FIG00_value. alarmFlag	= getBits_1 (d, 16 + 16 + 2);
	FIG00_value. CIF_major	= getBits_5 (d, 16 + 19);
	FIG00_value. CIF_minor	= getBits_8 (d, 16 + 24);
	FIG00_value. occurrenceChange = getBits_8 (d, 16 + 32);

//	if a switch from current to next happened:
	if ((FIG00_value. changeFlag == 0) &&
	     (FIG00_value. prevChangeFlag == 3)) {
	   fprintf (stderr, "changing\n");
	   fibConfig 	*temp	= currentConfig;
	   currentConfig	= nextConfig;
	   nextConfig		= temp;
	   nextConfig		->  reset ();
	// we hope that the radio knows what to do
	   emit signal_FIG00 ();
	}
	FIG00_value. prevChangeFlag	= FIG00_value. changeFlag;
//	if (alarmFlag)
//	   fprintf (stderr, "serious problem\n");
}
//
//	Subchannel organization 6.2.1
//	FIG0 extension 1 creates a mapping between the
//	sub channel identifications and the positions in the
//	relevant CIF.
//	CN Yes, OE PD are Rfu
void	fibDecoder::FIG0Extension1 (uint8_t *d) {
int16_t	used		= 2;		// offset in bytes
const int16_t	Length			= getBits_5 (d, 3);
const uint8_t	CN_bit	= getBits_1 (d, 8 + 0);
	while (used <= Length)
	   used = HandleFIG0Extension1 (d, used, CN_bit);
}
//
//	defining the channels 
int16_t	fibDecoder::HandleFIG0Extension1 (uint8_t *d, int16_t offset,
	                                  const uint8_t CN_bit) {
int16_t	bitOffset	= offset * 8;
const int16_t subChId	= getBits_6 (d, bitOffset);
const int16_t startAddr	= getBits (d, bitOffset + 6, 10);
int16_t	tabelIndex;
int16_t	option, protLevel, chanSize;
FIG01 channel;

fibConfig *localBase = CN_bit == 0 ? currentConfig : nextConfig;
static	int table_1 [] = {12, 8, 6, 4};
static	int table_2 [] = {27, 21, 18, 15};

	channel. subChId	= subChId;
	channel. startAddr	= startAddr;
	channel. Length		= 0;	// will change

	if (getBits_1 (d, bitOffset + 16) == 0) {	// short form
	   tabelIndex		= getBits_6 (d, bitOffset + 18);
	   channel. Length	= ProtLevel [tabelIndex][0];
	   channel. shortForm	= 1;		// short form
	   channel. protLevel	= ProtLevel [tabelIndex][1];
	   channel. bitRate	= ProtLevel [tabelIndex][2];
	   bitOffset += 24;
	}
	else { 	// EEP long form
	   channel. shortForm	= 0;
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
	if (!localBase -> in_FIG01_stack (channel)) {
	   localBase -> FIG01_stack. push_back (channel);
	}
	return bitOffset / 8;	// we return bytes
}
//
//	Service organization, 6.3.1
//	bind channels to SIds
//	PD and CN bit active, OE is Rfu
void	fibDecoder::FIG0Extension2 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
const int16_t	Length	= getBits_5 (d, 3);
const uint8_t	CN_bit	= getBits_1 (d, 8 + 0);
const uint8_t	PD_bit	= getBits_1 (d, 8 + 2);

int16_t		bitOffset	= 8 * used;
uint32_t	SId;
fibConfig	*localBase	= CN_bit == 0 ? currentConfig : nextConfig;

	while (bitOffset < Length * 8) {
	   if (PD_bit == 1) {		// long Sid, data
	      SId	= getLBits  (d, bitOffset, 32);
	      bitOffset	+= 32;
	   }
	   else {
	      SId	= getBits   (d, bitOffset, 16);
	      bitOffset	+= 16;
	   }


	   for (auto &f02 : localBase -> FIG02_stack) 
	      if (f02. SId == SId) {
	         return;
	      }

	   FIG02 element;
	   element. SId		= SId;
	   element. PD_bit	= PD_bit;
	   element. nrComps	=  getBits_4 (d, bitOffset + 4);
	   bitOffset	+= 8;

	   for (uint16_t i = 0; i < element. nrComps; i ++) {
	      FIG02_comp comp;
	      comp. compNr	= i;
	      comp. SCIds	= 0;	// default
	      const uint8_t TMid	= getBits_2 (d, bitOffset);
	      comp. TMid	= TMid;
	      if (TMid == 00)  {	// Audio
	         comp. ASCTy	= getBits_6 (d, bitOffset + 2);
	         comp. subChId	= getBits_6 (d, bitOffset + 8);
	         comp. PS_flag	= getBits_1 (d, bitOffset + 14);
	      }
	      else
	      if (TMid == 3) { // MSC packet data
	         comp. SCId	= getBits   (d, bitOffset + 2, 12);
	         comp. PS_flag	= getBits_1 (d, bitOffset + 14);
	      }
	      else 
	         {;}
	      bitOffset += 16;
	      element. components. push_back (comp);
	   }
	   localBase -> FIG02_stack. push_back (element);
	}
}

//	Service component in packet mode 6.3.2
//	CN bit active, OE and PD Rfu
void	fibDecoder::FIG0Extension3 (uint8_t *d) {
int16_t used    = 2;            // offset in bytes
const int16_t Length  = getBits_5 (d, 3);
const uint8_t CN_bit  = getBits_1 (d, 8 + 0);

	while (used <= Length)
	   used = HandleFIG0Extension3 (d, used, CN_bit);
}
//
//	Note that the SCId (Service Component Identifier) is
//	a unique 12 bit number in the ensemble
int16_t fibDecoder::HandleFIG0Extension3 (uint8_t	*d,
	                                  int16_t	used,
	                                  const uint8_t	CN_bit) {
const int16_t SCId            = getBits   (d, used * 8,  12);
const int16_t CAOrgflag       = getBits_1 (d, used * 8 + 15);
const int16_t DGflag          = getBits_1 (d, used * 8 + 16);
const int16_t DSCTy           = getBits_6 (d, used * 8 + 18);
const int16_t SubChId         = getBits_6 (d, used * 8 + 24);
const int16_t packetAddress   = getBits   (d, used * 8 + 30, 10);
uint16_t  CAOrg		= 0;
fibConfig	*localBase = CN_bit == 0 ? currentConfig : nextConfig;

	if (CAOrgflag == 1) {
	   CAOrg = getBits (d, used * 8 + 40, 16);
	   used += 16 / 8;
	}
	(void)CAOrg;
	used += 40 / 8;

	FIG03 element;
	element. SCId  		= SCId;
	element. subChId  	= SubChId;
	element. DSCTy		= DSCTy;
	element. DG_flag	= DGflag;
	element. packetAddress	= packetAddress;
	if (!localBase -> in_FIG03_stack (element))
	   localBase -> FIG03_stack . push_back (element);
	return used;
}

//	Service component language 8.1.2
//	no CN_bit, OE_bit and PD_bit 
void	fibDecoder::FIG0Extension5 (uint8_t *d) {
int16_t	used		= 2;		// offset in bytes
const int16_t Length	= getBits_5 (d, 3);

	while (used <= Length) {
	   used = HandleFIG0Extension5 (d, used);
	}
}

//	CN_bit, OE_bit and PD_bit are Rfu
int16_t	fibDecoder::HandleFIG0Extension5 (uint8_t *d, uint16_t offset) {
int16_t	bitOffset	= offset * 8;
const uint8_t	LS_flag	= getBits_1 (d, bitOffset);
FIG05 comp;

	if (LS_flag == 0) {
	   comp. subChId = getBits (d, bitOffset + 2, 6);
	   comp. language = getBits (d, bitOffset + 8, 8);
	   bitOffset += 16;
	   if (!in_FIG05_stack (comp))
	      FIG05_stack. push_back (comp);
	   return bitOffset / 8;
	}
	else {
	   uint16_t SCId	= getBits (d, bitOffset + 4, 12);
	   for (auto &f: currentConfig ->  FIG03_stack) {
	      if (f. SCId == SCId) {
	         comp. subChId	= f. subChId;
	         comp. language	= getBits (d, bitOffset + 16, 8);
	         if (!in_FIG05_stack (comp))
	            FIG05_stack. push_back (comp);
	         break;
	      }
	   }
	   bitOffset += 24;
	   return bitOffset / 8;
	}
}

// FIG0/6: Service linking information 8.1.5
//	CN_bit is SIV, OE_bit and PD_bit are Rfu
void    fibDecoder::FIG0Extension6 (uint8_t *d) {
	(void)d;
}

//
// FIG0/7: Configuration linking information 6.4.2,
//	CN_bit on, OE_bit and PD_bit are Rfu
void    fibDecoder::FIG0Extension7 (uint8_t *d) {
int16_t used		= 2;            // offset in bytes
//const int16_t Length	= getBits_5 (d, 3);
const uint8_t CN_bit	= getBits_1 (d, 8 + 0);
const int serviceCount	= getBits_6 (d, used * 8);
int     counter		= getBits   (d, used * 8 + 6, 10);
fibConfig	*localBase = CN_bit == 0 ? currentConfig : nextConfig;

	localBase	-> FIG07_value	= serviceCount;
	(void)counter;
}

// FIG0/8:  Service Component Global Definition (6.3.5)
//	CN_bit and PD_bit active, no OE_bit
void	fibDecoder::FIG0Extension8 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
const int16_t	Length	= getBits_5 (d, 3);
const uint8_t	CN_bit	= getBits_1 (d, 8 + 0);
const uint8_t	PD_bit	= getBits_1 (d, 8 + 0);

	while (used <= Length) {
	   used = HandleFIG0Extension8 (d, used, CN_bit, PD_bit);
	}
}

int16_t	fibDecoder::HandleFIG0Extension8 (uint8_t	*d,
	                                  int16_t	used,
	                                  const uint8_t	CN_bit,
	                                  const uint8_t PD_bit) {
int16_t	bitOffset	= used * 8;
const uint32_t	SId	= getLBits (d, bitOffset, PD_bit == 1 ? 32 : 16);
uint8_t		LS_flag;
uint8_t		extensionFlag;
fibConfig	*localBase	= CN_bit == 0 ? currentConfig : nextConfig;
FIG08 comp;

	bitOffset	+= PD_bit == 1 ? 32 : 16;
	extensionFlag   = getBits_1 (d, bitOffset);
	uint8_t SCIds	= getBits_4 (d, bitOffset + 4);

	bitOffset	+= 8;
	LS_flag		= getBits_1 (d, bitOffset);

	comp. subChId	= 0;
	comp. SId	= 0;
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

	if (comp. SCIds	== 0)	// not secondary
	   return bitOffset / 8;

	if (localBase -> in_FIG08_stack (comp))
	   return bitOffset / 8;

	for (auto &g :  localBase -> FIG02_stack) {
	   if (g. SId != SId)  
	      continue;

	   if (LS_flag == 0) {	// short form
	      for (auto &h : g. components) {
	         if (h. TMid != 0)
	            continue;
	         if (h. subChId != comp. subChId)
	            continue;
	         h. SCIds =  comp. SCIds;
	         break;
	      }
	   }
	   else {	// long form
	      for (auto &h : g. components) {
	         if (h. TMid != 3)
	            continue;
	         if (h. SCId != comp. SCId)
	            continue;
	         h. SCIds = comp.  SCIds;
	         break;
	      }
	   }	// loop in components
	}	// loop in FIG02 stack
	localBase -> FIG08_stack. push_back (comp);
	return bitOffset / 8;
}

//	FIG0/9 Country, LTO and International table, clause 8.1.3.2;
void	fibDecoder::FIG0Extension9 (uint8_t *d) {
uint8_t used		= 2; 		// offset in bytes
int16_t	Length		= getBits_5 (d, 3);
	
//	bit 6 indicates the number of hours
	uint8_t extFlag		= getBits_1 (d, used * 8 + 0);
        const int signbit	= getBits_1 (d, used * 8 + 2);
        dateTime [6] = (signbit == 1)?  -1 * getBits_4 (d, used * 8 + 3):
                                         getBits_4 (d, used * 8 + 3);
	uint8_t	LTO	= dateTime [6];
	uint8_t ecc	= getBits (d, used * 8 + 8, 8);
	uint16_t table	= getBits (d, used * 8 + 16, 8);

	FIG09_value.	ECC		= ecc;
	FIG09_value.	LTO		= LTO;
	FIG09_value.	tableId		= table;
	signal_FIG09 (LTO, ecc, table);
	if (!extFlag)
	   return;
	int bitOffset	= used * 8 + 16;
	bitOffset += 8;
	while (bitOffset < Length * 8) {
	   uint16_t nrServices = getBits_2 (d, bitOffset);
	   bitOffset += 2;
//	Rfa2	
	   bitOffset += 6;
	   int service_ecc = getBits_8 (d, bitOffset);
	   bitOffset += 8;
	   for (int i = 0; i < nrServices; i ++) {
	      uint16_t SId = getLBits (d, bitOffset, 16);
	      struct local_ecc ecc;
	      ecc. SId		= SId;
	      ecc. service_ecc	= service_ecc;
	      bool flag		= false;
	      for (auto &se : FIG09_value. ecc_local) {
	         if ((se. SId == SId) && (se. service_ecc == service_ecc)) {
	            flag = true;
	            break;
	         }
	      }
	      if (!flag)
	         FIG09_value. ecc_local. push_back (ecc);
	      bitOffset += 16;
	   }
	}
}

int	monthLength [] {
31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
//
//	Time in 10 is given in UTC, for other time zones
//	we add (or subtract) a number of Hours (half hours)
void	fibDecoder::adjustTime (int32_t *dateTime) {
//	first adjust the half hour  in the amount of minutes
	(void)dateTime;
	dateTime [4] += 
	            (dateTime [7] == 1) ? 30 : 0;
	if (dateTime [4] >= 60) {
	    dateTime [4] -= 60;
	    dateTime [3] ++;
	}

	if (dateTime [4] < 0) {
	    dateTime [4] += 60;
	    dateTime [3] --;
	}

	dateTime [3] += dateTime [6];
	if ((0 <= dateTime [3]) && (dateTime [3] <= 23))
	   return;

	if (dateTime [3] > 23) {
	   dateTime [3] -= 24;
	   dateTime [2] ++;
	}

	if (dateTime [3] < 0) {
	   dateTime [3] += 24;
	   dateTime [2] --;
	}

	if (dateTime [2] > monthLength [dateTime [1] - 1]) {
	   dateTime [2] = 1;
	   dateTime [1] ++;
	   if (dateTime [1] > 12) {
	      dateTime [1] = 1;
	      dateTime [0] ++;
	   }
	}

	if (dateTime [2] < 0) {
	   if (dateTime [1] > 1) {
	      dateTime [2] =
	                  monthLength [dateTime [1] - 1 - 1];
	      dateTime [1] --;
	   }
	   else {
	      dateTime [2] = monthLength [11];
	      dateTime [1] = 12;
	      dateTime [0] --;
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

	theTime [3] = getBits_5 (dd, offset + 21) % 24; // Hours
	theTime [4] = getBits_6 (dd, offset + 26) % 60; // Minutes

	if (getBits_6 (dd, offset + 26) != dateTime [4]) 
	   theTime [5] =  0;	// Seconds

	if (dd [offset + 20] == 1)
	   theTime [5] = getBits_6 (dd, offset + 32);	// Seconds
//
//	take care of different time zones
	bool	change = false;
	for (int i = 0; i < 6; i ++) {
	   if (theTime [i] != dateTime [i])
	      change = true;
	   dateTime [i] = theTime [i];
	}

#ifdef	CLOCK_STREAMER
	change = true;
#endif
	if (change) {
	   int utc_day		= dateTime [2];
	   int utc_hour		= dateTime [3];
	   int utc_minute 	= dateTime [4];
	   int utc_seconds	= dateTime [5];
	   adjustTime (dateTime);
	   emit  signal_FIG010 (dateTime [0],
	                        dateTime [1],
	                        dateTime [2],
	                        dateTime [3],
	                        dateTime [4],
	                        utc_day, utc_hour, utc_minute, utc_seconds);
	}
}

//
//	FIG0/11	obsolete

//	FIG0/12	obsolete

//	User Application Information 6.3.6
//	CN_bit and PD_Bit active
void	fibDecoder::FIG0Extension13 (uint8_t *d) {
int16_t	used			= 2;		// offset in bytes
int16_t	length			= getBits_5 (d, 3);
const uint8_t	CN_bit		= getBits_1 (d, 8 + 0);
const uint8_t	PD_bit		= getBits_1 (d, 8 + 2);

	while (used <= length) 
	   used = HandleFIG0Extension13 (d, used, CN_bit, PD_bit);
}
//
//	section 6.3.6 User application Data
int16_t	fibDecoder::HandleFIG0Extension13 (uint8_t *d,
	                                   int16_t used,
	                                   const uint8_t CN_bit,
	                                   const uint8_t pdBit) {
int16_t	bitOffset	= used * 8;
uint32_t	SId	= getLBits (d, bitOffset, pdBit == 1 ? 32 : 16);
int16_t		appType;
fibConfig	*localBase	= CN_bit == 0 ? currentConfig : nextConfig;
FIG013 element;

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
	if (localBase -> in_FIG013_stack (element))
	   return bitOffset / 8;
	localBase -> FIG013_stack. push_back (element);
	return bitOffset / 8;
}

//	FEC sub-channel organization 6.2.2
//	CN_bit active, OE_bit and PD_bit are Rfu
void	fibDecoder::FIG0Extension14 (uint8_t *d) {
int16_t	Length		= getBits_5 (d, 3);	// in Bytes
const uint8_t	CN_bit	= getBits_1 (d, 8 + 0);
int16_t	used	= 2;			// in Bytes
fibConfig	*localBase	= CN_bit == 0 ? currentConfig : nextConfig;
FIG014 element;

	while (used <= Length) {
	   element. subChId	= getBits_6 (d, used * 8);
	   element. FEC_scheme	= getBits_2 (d, used * 8 + 6);
	   used = used + 1;
	   if (!localBase -> in_FIG014_stack (element))
	      localBase -> FIG014_stack. push_back (element);
	}
}
//
//
//	8.1.7
//	CN_bit SIV, OE_bit Rfu and PD_bit special use
void	fibDecoder::FIG0Extension15 (uint8_t *d) {
	(void)d;
//	to be researched
}
//
//	FIG0/16	obsolete

//	program type 8.1.5 only current config
void	fibDecoder::FIG0Extension17 (uint8_t *d) {
int16_t	length	= getBits_5 (d, 3);
int16_t	offset	= 16;

	while (offset < length * 8) {
	   FIG017 element;
	   element. SId		= getBits	(d, offset, 16);
	   element. SD_flag	= getBits_1	(d, offset);
	   element. typecode	= getBits_5	(d, offset + 27);
	   if (!in_FIG017_stack (element))
	      FIG017_stack. push_back (element);
	   offset += 32;
	}
}
//
//	Announcement support 8.1.6.1 only current config
void	fibDecoder::FIG0Extension18 (uint8_t *d) {
int16_t	Length		= getBits_5 (d, 3);	// in Bytes
int16_t	used		= 2;			// in Bytes
int16_t	bitOffset	= used * 8;

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
	      FIG018 aC;
	      aC. SId = SId;
	      aC. ASUFlags = asuFlags;
	      aC. clusterIds. push_back (clusterId);
	      if (!in_FIG018_stack (aC))
	         FIG018_stack. push_back (aC);
	   }
	   bitOffset	+= nrClusters * 8;
	}
}

//	Announcement switching 8.1.6.2 only current config
void	fibDecoder::FIG0Extension19 (uint8_t *d) {
int16_t	Length		= getBits_5 (d, 3);	// in Bytes
int16_t	used		= 2;			// in Bytes
int16_t	bitOffset	= used * 8;
	
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
	   check_announcements (clusterId, AswFlags, newFlag, subChId);
	}
}

//	FOG0/20 is not encountered yet
void	fibDecoder::FIG0Extension20 (uint8_t *d) {
	(void)d;
}

//
//	Frequency information (FI) 8.1.8
//	CN_bit SIV, OE_flag active, PD_flag rfu
void	fibDecoder::FIG0Extension21 (uint8_t *d) {
int16_t	used		= 2;		// offset in bytes
const int16_t	Length	= getBits_5 (d, 3);
const uint8_t	CN_bit	= getBits_1 (d, 8 + 0);
const uint8_t	OE_bit	= getBits_1 (d, 8 + 1);

	while (used <= Length) 
	   used = HandleFIG0Extension21 (d, used, CN_bit, OE_bit);
}

int16_t	fibDecoder::HandleFIG0Extension21 (uint8_t	*d,
	                                   uint16_t	offset,
	                                   const uint8_t CN_bit,
	                                   const uint8_t OE_bit) {
int16_t	l_offset	= offset * 8;
int16_t	l	= getBits_5 (d, l_offset + 11);
int16_t		upperLimit	= l_offset + 16 + l * 8;
int16_t		base		= l_offset + 16;
//	for now
	(void)CN_bit; (void)OE_bit;

	while (base < upperLimit) {
	   uint16_t idField	= getBits (d, base, 16);
	   uint8_t  RandM	= getBits_4 (d, base + 16);
	   uint8_t  continuity	= getBits_1 (d, base + 20);
	   (void)continuity;
	   uint8_t  length	= getBits_3 (d, base + 21);
	   if (RandM == 0x08) {
	      bool SId_bekend = false;
	      for (auto &f : FIG021_stack) {
	         if (f. SId == idField) {
	            SId_bekend = true;
	            break;
	         }
	      }
	      if (SId_bekend) {
	         base += 24 + length * 8;
	         continue;
	      }
	         
	      FIG021 f21;
	      f21. SId       = idField;
	      for (int i = 0; i < length; i ++) {
	         uint16_t fmFrequency_key	=
                           getBits (d, base + 24 + i * 8, 8);
	         uint32_t  fmFrequency	=
	                           87500 + fmFrequency_key * 100;
	         f21. freqList. push_back (fmFrequency);
	         signal_FIG021 (idField, fmFrequency);
	      }
	      
	      FIG021_stack. push_back (f21);
	   }
	   base += 24 + length * 8;
	}
	return upperLimit / 8;
}
//
/////////////////////////end of FIG0//////////////////////////////////

//	FIG 1 - Cover the different possible labels, section 5.2
void	fibDecoder::process_FIG1 (uint8_t *d) {
uint8_t	extension	= getBits_3 (d, 8 + 5); 

	switch (extension) {
	   case 0:		// ensemble name 8.1.13
	      FIG1Extension0 (d);
	      break;

	   case 1:		// program service name 8.1.14.1
	      FIG1Extension1 (d);
	      break;

	   case 2:		// Labels etc not seen yet
	      break;

	   case 3:		// obsolete
	      break;

	   case 4:		// Service Component Label 8.1.14.3
	      FIG1Extension4 (d);
	      break;

	   case 5:		// Data service label, 8.1.14.2
	      FIG1Extension5 (d);
	      break;

	   case 6:		// XPAD label - 8.1.14.4
	      FIG1Extension6 (d);
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

	label [16] = 0;
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
	QString name = toQStringUsingCharset (
                                       (const char *) label,
                                       (CharacterSet) charSet);
	if (FIG10_value. EId == 0) {
	   FIG10_value. ensembleLabel	= name;
	   FIG10_value. EId		= EId;
	   FIG10_value. isSynced	= true;
	   emit  signal_FIG10 (name, EId);
	}
}
//
//	Name of primary service
void	fibDecoder::FIG1Extension1 (uint8_t *d) {
int16_t		offset	= 32;
char		label [17];

	label [16] = 0;
//      from byte 1 we deduce:
	const uint8_t charSet	= getBits_4	(d, 8);
	const uint8_t Rfu	= getBits_1	(d, 8 + 4);
	const uint8_t extension	= getBits_3	(d, 8 + 5);
	const uint32_t SId	= getBits	(d, 16, 16);
	label [16]      = 0x00;
	(void)Rfu; (void)extension;
	for (auto &serv : FIG11_stack) {
	   if (SId == serv. SId) 
	      return;
	}
// assume we are defined
	if (!get_subChId (SId, 0))
	   return;		// wait for a next occurrence
	QString serviceName;
	QString shortName;		
	for (int i = 0; i < 16; i ++) 
	   label [i] = getBits_8 (d, offset + 8 * i);
	serviceName = toQStringUsingCharset (
	                               (const char *) label,
	                               (CharacterSet) charSet);

	for (int i = 0; i < 16; i ++) 
	   if (getBits_1 (d, offset + 16 * 8 + i) != 0)
	      shortName. append (serviceName. at (i));

	signal_FIG11 (serviceName, SId);
	FIG11 f;
	f. serviceName	= serviceName;
	f. shortName	= shortName;
	f. SId		= SId;
	fibLocker. lock ();
	FIG11_stack. push_back (f);
	fibLocker. unlock ();
}

//	service component label - 32 bits 8.1.14.3
void	fibDecoder::FIG1Extension4 (uint8_t *d) {
char		label [17];
int		bitOffset = 16;
uint32_t	SId;

	label [16] = 0;
//      from byte 1 we deduce:
	const uint8_t PD_flag	= getBits_1 (d, bitOffset);
	const uint8_t SCIds	= getBits_4 (d, bitOffset + 4);
	const uint8_t charSet	= getBits_4 (d, 8);
	const uint8_t Rfu	= getBits_1 (d, 8 + 4);
	const uint8_t extension	= getBits_3 (d, 8 + 5);
	if (PD_flag) {
	   SId	= getLBits  (d, bitOffset + 8, 32);
	   bitOffset += 32 + 8;
	}
	else {
	   SId = getLBits (d, bitOffset + 8, 16);
	   bitOffset += 16 + 8;
	}
	if (SCIds == 0)
	   return;
//	just a check if we already have the servicename
	for (auto &serv : FIG14_stack)
	   if ((serv. SId == SId) && (serv. SCIds == SCIds))
	      return;
	if (!get_subChId (SId, SCIds))
	   return;
//
	label [16]      = 0x00;
	(void)Rfu;
	(void)extension;
	for (int i = 0; i < 16; i ++) 
	   label [i] = getBits_8 (d, bitOffset + 8 * i);
	QString serviceName = toQStringUsingCharset (
	                                  (const char *) label,
	                                  (CharacterSet) charSet);
	QString shortName;		
	for (int i = 0; i < 16; i ++) 
	   if (getBits_1 (d, bitOffset + 16 * 8 + i) != 0)
	      shortName. append (serviceName. at (i));

	signal_FIG14 (serviceName, SId, SCIds);
	FIG14 f;
	f. serviceName	= serviceName;
	f. shortName	= shortName;
	f. SId		= SId;
	f. SCIds	= SCIds; 
	fibLocker. lock ();
	FIG14_stack. push_back (f);
	fibLocker. unlock ();
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

	for (auto &serv : FIG15_stack) {
	   if (SId == serv. SId) 
	      return;
	}

//	if no subch is not known (yet) we do not record the service yet
	if (!get_subChId (SId, 0))
	   return;

//	we alsowant the apptype to be defined
	bool haveAppType	= false;
	for (auto &f : currentConfig -> FIG013_stack) {
           if (f. SId == SId) {
	      haveAppType = true;
	      break;
	   }
	}
	if (!haveAppType) {
	   return;
	}

//	It seems the service is (more or less) complete
	for (int i = 0; i < 16; i ++) {
	   label [i] = getBits_8 (d, bitOffset + 8 * i);
	}
	QString serviceName = toQStringUsingCharset (
	                                  (const char *) label,
	                                  (CharacterSet) charSet);
	QString shortName;		
	for (int i = 0; i < 16; i ++)  
	   if (getBits_1 (d, bitOffset + 16 * 8 + i) != 0)
	      shortName. append (serviceName. at (i));
	signal_FIG15 (serviceName, SId);
	FIG15 f;
	f. serviceName	= serviceName;
	f. shortName	= shortName;
	f. SId		= SId;
	fibLocker. lock ();
	FIG15_stack. push_back (f);
	fibLocker. unlock ();
}

void	fibDecoder::FIG1Extension6 (uint8_t *d) {
uint16_t	bitOffset	= 0;
const uint8_t PD_bit	= getBits_1 (d, bitOffset);
const uint8_t SCIds	= getBits_4 (d, bitOffset + 4);
uint32_t SId;
	
	bitOffset += 8;

	if (PD_bit != 0) {
	   SId	 = getLBits (d, bitOffset, 32);
	   bitOffset += 32;
	}
	else {
	   SId	= getLBits (d, bitOffset, 16);
	   bitOffset += 16;
	}

	uint8_t xpadType	= getBits (d, bitOffset + 3, 5);
//	fprintf (stderr, "XPad type for %X %d is %d\m",
//	                           SId, SCIds, xpadType);
}

//
//////////////////////end of FIG1 ///////////////////////////////////////////

void	fibDecoder::connectChannel () {
	currentConfig	-> reset ();
	nextConfig	-> reset ();
	reset ();
}

void	fibDecoder::disconnectChannel () {
	currentConfig	-> reset ();
	nextConfig	-> reset ();
	reset ();
}

void	fibDecoder::reset	() {

	FIG00_value. 	prevChangeFlag	= 0;
	FIG00_value.	EId		= 0;
	FIG00_value.	changeFlag	= 0;
	FIG00_value.	prevChangeFlag	= 0;
	FIG05_stack.	resize (0);
	FIG09_value.	extFlag	= 0;
	FIG09_value.	LTO	= 0;
	FIG09_value.	ECC	= 0;
	FIG09_value.	tableId	= 0;
	FIG09_value.	ecc_local. resize (0);
	FIG010_value.	MJD	= 0;
	FIG010_value.	LSI	= 0;
	
	FIG010_value.	UTC_flg	= 0;
	FIG017_stack.	resize (0);
	FIG018_stack.	resize (0);
	FIG021_stack.	resize (0);

	FIG10_value. EId	= 0;
	FIG10_value. isSynced	= false;
	FIG10_value. ensembleLabel	= "";
	FIG11_stack.	resize (0);
	FIG14_stack.	resize (0);
	FIG15_stack.	resize (0);
	FIG16_stack.	resize (0);
}

//
//	ofdmHandler asks for syncReached
bool	fibDecoder::syncReached() {
	return  FIG10_value. isSynced;
}
//
////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

bool	fibDecoder::isPrimaryService	(uint32_t SId, uint8_t SCIds) {
QString serviceName;
QString shortName;
	getServiceName (serviceName, shortName, SId, SCIds);
	for (auto &f : currentConfig -> FIG02_stack) {
	   if (f. SId != SId) 
	      continue;
	   for (auto &g : f. components) 
	      if ((g. SCIds == SCIds) && (g. PS_flag == 1)) {
	         return true;
	      }
	}
	return false;
}

bool	fibDecoder::isAudioService	(uint32_t SId, uint8_t SCIds) {
	for (auto &f : currentConfig -> FIG02_stack) {
	   if (f. SId != SId)
	      continue;
	   for (auto &g : f. components) {
	      if (g. SCIds == SCIds) {
	         return g. TMid == 0;
	      }
	   }
	}
	return false;
}

bool	fibDecoder::isPacketService	(uint32_t SId, uint8_t SCIds) {
	for (auto &f : currentConfig -> FIG02_stack) {
	   if (f. SId != SId)
	      continue;
	   for (auto &g : f. components) {
	      if (g. SCIds == SCIds) {
	         return g. TMid == 3;
	      }
	   }
	}
	return false;
}

bool	fibDecoder::is_SPI		(uint32_t SId) {
	for (auto &f : currentConfig -> FIG013_stack)
	   if (f. SId == SId)
	      return f. Apptype == 7;
	return false;
}

std::vector<uint8_t>
	fibDecoder::get_secondaryServices (uint32_t SId) {
std::vector<uint8_t> res;
	for (auto &g : FIG14_stack)
	      if (g. SId == SId)
	         res. push_back (g. SCIds);
	return res;
}

int	fibDecoder::FIG07_value		() {
	return currentConfig	-> FIG07_value;
}

void	fibDecoder::getFreqs		(uint32_t SId,
	                                      std::vector<uint32_t> &freqList) {
	for (auto &f21: FIG021_stack) 
	   if (f21. SId == SId) {
//	      fprintf (stderr, "for %X we have entries :", SId);
//	      for (uint32_t i = 0; i < f21. freqList. size (); i ++)
//	            fprintf (stderr, "%d ", f21. freqList [i]);
//	      fprintf (stderr, "\n");
	      freqList = f21. freqList;
	      return;
	   }
}
	   
void	fibDecoder::getServiceName	(QString &serviceName,
	                                 QString &shortName,
	                                 uint32_t SId, uint8_t SCIds) {

	serviceName	= "";
	shortName	= "";
	if (SCIds != 0) {
	   for (auto &f :  FIG14_stack) {
	      if ((f. SId == SId) && (f. SCIds == SCIds)) {
	         serviceName	= f. serviceName;
	         shortName	= f. shortName;
	         return;
	      }
	   }
	   return;
	}
	for (auto &f : FIG11_stack) {
	   if (f. SId == SId) {
	      serviceName	= f. serviceName;
	      shortName		= f. shortName;
	      return;
	   }
	}
	for (auto &f : FIG15_stack) {
	   if (f. SId == SId) {
              serviceName       = f. serviceName;
              shortName         = f. shortName;
              return;
           }
        }
}

void	fibDecoder::mapNameToId (const QString &s,
	                                uint32_t &SId, uint8_t &SCIds) {
	fibLocker. lock ();
	for (auto &f : FIG11_stack) {
	   if (f. serviceName == s) {
	      SId	= f. SId;
	      SCIds	= 0;
	      fibLocker. unlock ();
	      return;
	   }
	}
	for (auto &f : FIG15_stack) {
	   if (f. serviceName == s) {
	      SId	= f. SId;
	      SCIds	= 0;
	      fibLocker. unlock ();
	      return;
	   }
	}

	for (auto &f : FIG14_stack) {
	   if (f. serviceName == s) {
	      SId	= f. SId;
	      SCIds	= f. SCIds;
	      fibLocker. unlock ();
	      return;
	   }
	}
	SId	= 0;
	SCIds	= 0;
	fibLocker. unlock ();
}

//
void	fibDecoder::audioData (uint32_t SId, uint8_t SCIds, audiodata &ad) {
	ad. defined = false;
	for (auto &f: currentConfig -> FIG02_stack) {
	   if (f. SId != SId)
	      continue;
	   for (auto &g: f. components) {
	      if ((g. SCIds != SCIds) || (g. TMid != 0))
	        continue;
	      QString serviceName;
	      QString shortName;
	      getServiceName (serviceName, shortName, SId, SCIds);
	      if (serviceName == "")
	         return;
	      ad. serviceName	= serviceName;
	      ad. shortName	= shortName;
	      ad. SId		= SId;
	      ad. SCIds		= SCIds;
	      ad. subchId	= g. subChId;
	      ad. ASCTy		= g. ASCTy;
	      for (auto &h : currentConfig -> FIG01_stack) {
	         if (h. subChId == g. subChId) {
	            ad. startAddr	= h. startAddr;
	            ad. shortForm	= h. shortForm;
	            ad. protLevel	= h. protLevel;
	            ad. length		= h. Length;
	            ad. bitRate		= h. bitRate;
	            ad. defined		= true;
	            break;
	         }
	      }
	      ad. language = 0;		// default
	      for (auto &h : FIG05_stack) {
	         if (h. subChId == ad. subchId) {
	            ad. language = h. language;
	            break;
	         }
	      }
	      ad. programType = 0;	// default
	      for (auto &h : FIG017_stack) {
	         if (h. SId == ad. SId) {
	            ad. programType = h. typecode;
	            break;
	         }
	      }
	      ad. ecc	= 0;		// default;
	      for (auto &h : FIG09_value. ecc_local) {
	         if (h. SId ==  (uint32_t)ad. SId) {
	            ad. ecc = h. service_ecc;
	            break;
	         }
	      }
	   }
	}
}

void	fibDecoder::packetData (uint32_t SId, uint8_t SCIds, packetdata &pd) {
	pd. defined = false;
	for (auto &f:  currentConfig -> FIG02_stack) {
	   if (f. SId != SId)
	      continue;
	   for (auto &g : f. components) {
	      if ((g. TMid != 3) || (g. SCIds != SCIds))
	         continue;
	      QString serviceName;
	      QString shortName;
	      getServiceName (serviceName, shortName, SId, SCIds);
	      pd. serviceName	= serviceName;
	      pd. shortName	= shortName;
	      pd. SId		= SId;
	      pd. SCIds		= SCIds;
	      pd. subchId	= 100;
	      for (auto &h :  currentConfig -> FIG03_stack) {
	         if (h. SCId == g. SCId) {
	            pd. DG_flag		= h. DG_flag;
	            pd. DSCTy		= h. DSCTy;
	            pd. packetAddress	= h. packetAddress;
	            pd. subchId		= h. subChId;
	            break;
	         }
	      }
	      if (pd. subchId == 100) {
	         return;
	      }
	      for (auto &h :currentConfig -> FIG01_stack) {
	         if (h. subChId == pd. subchId) {
	            pd. startAddr	= h. startAddr;
	            pd. shortForm	= h. shortForm;
	            pd. protLevel	= h. protLevel;
	            pd. length		= h. Length;
	            pd. bitRate		= h. bitRate;
	            pd. defined		= true;
	            break;
	         }
	      }
	      pd. FEC_scheme	= 0;
	      for (auto &h : currentConfig -> FIG014_stack) {
	         if (h. subChId == pd. subchId) {
	            pd. FEC_scheme	= h. FEC_scheme;
	            break;
	         }
	      }
	      pd. appType	= 0;
	      for (auto &h : currentConfig -> FIG013_stack) {
	         if ((h. SId == SId) && (h. SCIds == SCIds)) {
	            pd . appType = h. Apptype;
	            break;
	         }
	      }
	   }
	}
}

bool	fibDecoder::check_FIG01 (uint16_t subChId) {
	for (auto &h : currentConfig -> FIG01_stack)
	   if (h. subChId == subChId)
	      return true;
	return false;
}

bool fibDecoder::get_subChId (uint32_t SId, uint8_t SCIds) {
	for (auto &f : currentConfig -> FIG02_stack) {
	   if (f. SId != SId)
	      continue;
	   for (auto &g : f. components) {
	      if ((g. TMid == 0) && (g. SCIds == SCIds))
	         return check_FIG01 (g. subChId);
	      else
	      if (g. TMid == 3)
	         for (auto &h : currentConfig -> FIG03_stack)
	            if (g. SCId == h. SCId)
	               return check_FIG01 (h. subChId);
	   }
	}
	return false;
}
//
//
void	fibDecoder::getCIFcount	(int16_t &high, int16_t &low) {
	high	= FIG00_value. CIF_major;
	low	= FIG00_value. CIF_minor;
}

int	fibDecoder::freeSpace	() {
	return	currentConfig	-> freeSpace ();
}
//
//	for contenttable and scan results
QList<contentType> fibDecoder::contentPrint () {
QList<contentType> res;

	for (auto &f02 : currentConfig -> FIG02_stack) {
	   for (auto &g : f02. components) {
	      contentType theData;
	      theData. isActive	= false;
	      theData. SId	= f02. SId;
	      theData. TMid	= g. TMid;
	      theData. PS_flag	= g. PS_flag;
	      if (g. TMid == 0) {	// audio data
	         audiodata ad;
	         audioData (f02. SId, g. SCIds, ad);
	         if (!ad. defined)		// should not happen
	            continue;
	         theData. serviceName	= ad. serviceName;
	         theData. shortName	= ad. shortName;
	         theData. subChId	= ad. subchId;
	         theData. SCIds		= ad. SCIds;
	         theData. startAddress	= ad. startAddr;
	         theData. length	= ad. length;
	         theData. codeRate	= getCodeRate (ad. shortForm,
	                                               ad. protLevel);
	         theData. protLevel	= getProtectionLevel (ad. shortForm,
	                                                      ad. protLevel);
	         theData. bitRate	= ad. bitRate;
	         theData. language	= ad. language;
	         theData. FEC_scheme	= 0;
	         theData. packetAddress	= 0;
	         theData. ASCTy_DSCTy	= ad. ASCTy;
	         theData. programType	= ad. programType;
	         if (ad. SCIds == 0)
	            getFreqs (ad. SId, theData. fmFrequencies);
	         res. push_back (theData);
	      }
	      else
	      if (g. TMid == 3) {	// packet 
	         packetdata pd;
	         packetData (f02. SId, g. SCIds, pd);
	         if (!pd. defined)		// should not happen
	            continue;
	         theData. serviceName	= pd. serviceName;
	         theData. shortName	= pd. shortName;
	         theData. subChId	= pd. subchId;
	         theData. SCIds		= pd. SCIds;
	         theData. startAddress	= pd. startAddr;
	         theData. length	= pd. length;
	         theData. codeRate	= getCodeRate (pd. shortForm,
	                                               pd. protLevel);
	         theData. protLevel	= getProtectionLevel (pd. shortForm,
	                                                      pd. protLevel);
	         theData. bitRate	= pd. bitRate;
	         theData. FEC_scheme	= pd. FEC_scheme;
	         theData. packetAddress	= pd. packetAddress;
	         theData. ASCTy_DSCTy	= pd. DSCTy;
	         theData. appType	= pd. appType;
	         theData. language	= 0;
	         theData. programType	= 0;
	         res. push_back (theData);
	      }
	   }
	}
        return res;
}  
//
//	Needed for the timetables
std::vector<basicService>
	fibDecoder::getServices   () {
std::vector<basicService> res;
	for (auto &serv : FIG11_stack) {
	   basicService b;
	   b. serviceName	= serv. serviceName;
	   b. SId		= serv. SId;
	   res. push_back (b);
	}
	return res;
}

//
//	For ETI handling
int	fibDecoder::nrChannels	() {
	return currentConfig -> FIG01_stack. size ();
}

void	fibDecoder::getChannelInfo (channel_data *d, const int n) {
	FIG01 *selected = &currentConfig -> FIG01_stack [n];
        d       -> in_use       = true; 
        d       -> id           = selected ->  subChId;
        d       -> start_cu     = selected ->  startAddr;
        d       -> protlev      = selected ->  protLevel;
        d       -> size         = selected ->  Length;
        d       -> bitrate      = selected ->  bitRate;
        d       -> uepFlag      = selected ->  shortForm;
}

void	fibDecoder::check_announcements (uint8_t clusterId, 
	                                 uint8_t AswFlags,
	                                 uint8_t newFlag, uint16_t subChId) {
	for (auto &ac : FIG018_stack) {
	   for (auto &cli : ac. clusterIds) {
	      if ((cli == clusterId) && newFlag) {
	         uint16_t flags = (ac. ASUFlags & AswFlags);
	         uint16_t theSId = 0;
//
//	we know that signalling is only for audio services
	         for (auto & comp : currentConfig -> FIG02_stack) {
	             for (auto &g : comp. components) {
	               if (g. subChId == subChId) {
	                  theSId = comp. SId;
	                  signal_FIG019 (theSId, flags);
	                  return;
	               }
	            }
	         }
	      }
	   }
	}
}

uint32_t fibDecoder::julianDate	() {
	return mjd;
}

bool    fibDecoder::in_FIG05_stack (const FIG05 &el) {
        for (auto &f: FIG05_stack)
           if (f. subChId == el. subChId)
              return true;
        return false; 
}

bool    fibDecoder::in_FIG017_stack (const FIG017 &el) {
        for (auto &f : FIG017_stack)
           if (f. SId == el. SId)
              return true;
        return false; 
}
 
bool    fibDecoder::in_FIG018_stack (const FIG018 &el) {
        for (auto &f: FIG018_stack)
           if (f. SId == el. SId) 
              return true;
        return false;
}

bool    fibDecoder::in_FIG021_stack (const FIG021 &el) {
        for (auto &f : FIG021_stack)
           if (f. SId == el. SId)
              return true;
        return false;
}

