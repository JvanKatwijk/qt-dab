#
/*
 *    Copyright (C) 2018, 2019, 2020
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
#include	"dab-config.h"
#include	"fib-table.h"
#include	<QStringList>
//#include	"text-mapper.h"
#include	"dab-tables.h"
//
//
	fibDecoder::fibDecoder (RadioInterface *mr) {
	myRadioInterface	= mr;
	memset (dateTime, 0, sizeof (dateTime));

//	connect (this, SIGNAL (addtoEnsemble (const QString &, int)),
//	         myRadioInterface, SLOT (addtoEnsemble (const QString &, int)));
	connect (this, SIGNAL (nameofEnsemble (int, const QString &)),
	         myRadioInterface,
	                    SLOT (nameofEnsemble (int, const QString &)));
	connect (this, SIGNAL (clockTime (int, int, int, int, int,
	                                               int, int, int, int)),
	         myRadioInterface, SLOT (clockTime (int, int, int, int, int,
	                                                int, int, int, int)));
	connect (this, SIGNAL (changeinConfiguration ()),
	         myRadioInterface, SLOT (changeinConfiguration ()));
	connect (this, SIGNAL (startAnnouncement (const QString &, int)),
	         myRadioInterface, SLOT (startAnnouncement (const QString &, int)));
	connect (this, SIGNAL (stopAnnouncement (const QString &, int)),
	         myRadioInterface, SLOT (stopAnnouncement (const QString &, int)));
	connect (this, SIGNAL (nrServices (int)),
	         myRadioInterface, SLOT (nrServices (int)));

	currentConfig	= new dabConfig();
	nextConfig	= new dabConfig();
	ensemble	= new ensembleDescriptor();
	CIFcount	= 0;
	mjd		= 0;
}
	
	fibDecoder::~fibDecoder() {
	delete	nextConfig;
	delete	currentConfig;
	delete	ensemble;
}

//static
//uint8_t bits [] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

//	FIB's are segments of 256 bits. When here, we already
//	passed the crc and we start unpacking into FIGs
//	This is merely a dispatcher
void	fibDecoder::process_FIB (uint8_t *p, uint16_t fib) {
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

	   case 11:		// obsolete
	      break;

	   case 12:		// obsolete
	      break;

	   case 13:             // user application information (6.3.6)
	      FIG0Extension13 (d);
	      break;

	   case 14:             // FEC subchannel organization (6.2.2)
	      FIG0Extension14 (d);
	      break;

	   case 15:		// obsolete
	      break;

	   case 16:		// obsolete
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
	      break;		// to be implemented

	   case 21:		// frequency information (8.1.8)
	      FIG0Extension21 (d);
	      break;

	   case 22:		// obsolete
	      break;

	   case 23:		// obsolete
	      break;

	   case 24:		// OE services (8.1.10)
	      break;		// not implemented

	   case 25:		// OE announcement support (8.1.6.3)
	      break;		// not implemented

	   case 26:		// OE announcement switching (8.1.6.4)
	      break;		// not implemented

	   case 27:
	   case 28:
	   case 29:		// undefined
	      break;

	   default:
	      break;
	}
}

//	Ensemble information, 6.4.1
//	FIG0/0 indicated a change in channel organization
//	we are not equipped for that, so we just ignore it for the moment
//	The info is MCI
void	fibDecoder::FIG0Extension0 (uint8_t *d) {
uint16_t        EId;
uint8_t         changeFlag;
uint16_t        highpart, lowpart;
int16_t         occurrenceChange;
uint8_t CN_bit	= getBits_1 (d, 8 + 0);
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
	CIFcount_hi		= highpart;
	CIFcount_lo		= lowpart;
	CIFcount 		= highpart * 250 + lowpart;

	if ((changeFlag == 0) && (prevChangeFlag == 3)) {
	   fprintf (stderr, "handling change\n");
	   dabConfig 	*temp	= currentConfig;
	   currentConfig	= nextConfig;
	   nextConfig		= temp;
	   nextConfig	->  reset ();
	   cleanupServiceList ();
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
	                                  uint8_t CN_bit,
	                                  uint8_t OE_bit,
	                                  uint8_t PD_bit) {

int16_t	bitOffset	= offset * 8;
int16_t	subChId		= getBits_6 (d, bitOffset);
int16_t startAdr	= getBits (d, bitOffset + 6, 10);
int16_t	tabelIndex;
int16_t	option, protLevel, subChanSize;
subChannelDescriptor	subChannel;
dabConfig	*localBase = CN_bit == 0 ? currentConfig : nextConfig;
static	int table_1 [] = {12, 8, 6, 4};
static	int table_2 [] = {27, 21, 18, 15};

	(void)OE_bit; (void)PD_bit;
	subChannel. startAddr	= startAdr;
	subChannel. inUse	= true;

	if (getBits_1 (d, bitOffset + 16) == 0) {	// short form
	   tabelIndex = getBits_6 (d, bitOffset + 18);
	   subChannel. Length		= ProtLevel [tabelIndex][0];
	   subChannel. shortForm	= true;		// short form
	   subChannel. protLevel	= ProtLevel [tabelIndex][1];
	   subChannel. bitRate		= ProtLevel [tabelIndex][2];
	   bitOffset += 24;
	}
	else { 	// EEP long form
	   subChannel. shortForm	= false;
	   option = getBits_3 (d, bitOffset + 17);
	   if (option == 0) { 		// A Level protection
	      protLevel			= getBits (d, bitOffset + 20, 2);
	      subChannel. protLevel	= protLevel;
	      subChanSize		= getBits (d, bitOffset + 22, 10);
	      subChannel. Length	= subChanSize;
	      subChannel. bitRate	= subChanSize / table_1 [protLevel] * 8;
	   }
	   else			// option should be 001
	   if (option == 001) {		// B Level protection
	      protLevel			= getBits_2 (d, bitOffset + 20);
	      subChannel. protLevel	= protLevel + (1 << 2);
	      subChanSize		= getBits (d, bitOffset + 22, 10);
	      subChannel. Length	= subChanSize;
	      subChannel. bitRate	=
	                           subChanSize / table_2 [protLevel] * 32;
	   }
	   bitOffset += 32;
	}
//
//	in case the subchannel data was already computed
//	we merely compute the offset
	if (localBase -> subChannels [subChId]. inUse)
	   return bitOffset / 8;
//
//	and here we fill in the structure
	localBase -> subChannels [subChId]. SubChId	= subChId;
	localBase -> subChannels [subChId]. inUse       = true;
	localBase -> subChannels [subChId]. startAddr   =
	                                    subChannel. startAddr;
	localBase -> subChannels [subChId]. Length	=
	                                    subChannel. Length;
	localBase -> subChannels [subChId]. shortForm =
	                                    subChannel. shortForm;
	localBase -> subChannels [subChId]. protLevel =
	                                    subChannel. protLevel;
	localBase -> subChannels [subChId]. bitRate	=
	                                    subChannel. bitRate;

	return bitOffset / 8;	// we return bytes
}
//
//	Service organization, 6.3.1
//	bind channels to SIds
void	fibDecoder::FIG0Extension2 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
int16_t	Length	= getBits_5 (d, 3);
uint8_t	CN_bit	= getBits_1 (d, 8 + 0);
uint8_t	OE_bit	= getBits_1 (d, 8 + 1);
uint8_t	PD_bit	= getBits_1 (d, 8 + 2);

	while (used < Length) {
	   used = HandleFIG0Extension2 (d, used, CN_bit, OE_bit, PD_bit);
	}
}
//
//
int16_t	fibDecoder::HandleFIG0Extension2 (uint8_t *d,
	                                  int16_t offset,
	                                  uint8_t CN_bit,
	                                  uint8_t OE_bit,
	                                  uint8_t PD_bit) {
int16_t		bitOffset	= 8 * offset;
int16_t		i;
uint8_t		ecc = 0;
uint8_t		cId;
uint32_t	SId;
int16_t		numberofComponents;

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

	ensemble -> countryId	= cId;
	numberofComponents	= getBits_4 (d, bitOffset + 4);
	bitOffset	+= 8;

	for (i = 0; i < numberofComponents; i ++) {
	   uint8_t	TMid	= getBits_2 (d, bitOffset);
	   if (TMid == 00)  {	// Audio
	      uint8_t	ASCTy	= getBits_6 (d, bitOffset + 2);
	      uint8_t	SubChId	= getBits_6 (d, bitOffset + 8);
	      uint8_t	PS_flag	= getBits_1 (d, bitOffset + 14);
	      bind_audioService (CN_bit == 0 ? currentConfig : nextConfig,
	                            TMid, SId, i, SubChId, PS_flag, ASCTy);
	   }
	   else
	   if (TMid == 3) { // MSC packet data
	      int16_t SCId	= getBits   (d, bitOffset + 2, 12);
	      uint8_t PS_flag	= getBits_1 (d, bitOffset + 14);
	      uint8_t CA_flag	= getBits_1 (d, bitOffset + 15);
	      bind_packetService (CN_bit == 0 ? currentConfig : nextConfig,
	                            TMid, SId, i, SCId, PS_flag, CA_flag);
	   }
	   else 
	      {;}
	   bitOffset += 16;
	}
	return bitOffset / 8;		// in Bytes
}

//	Service component in packet mode 6.3.2
//      The Extension 3 of FIG type 0 (FIG 0/3) gives
//      additional information about the service component
//      description in packet mode.
void	fibDecoder::FIG0Extension3 (uint8_t *d) {
int16_t used    = 2;            // offset in bytes
int16_t Length  = getBits_5 (d, 3);
uint8_t CN_bit  = getBits_1 (d, 8 + 0);
uint8_t OE_bit  = getBits_1 (d, 8 + 1);
uint8_t PD_bit  = getBits_1 (d, 8 + 2);


	while (used < Length)
	   used = HandleFIG0Extension3 (d, used, CN_bit, OE_bit, PD_bit);
}
//
//	Note that the SCId (Service Component Identifier) is
//	a unique 12 bit number in the ensemble
int16_t fibDecoder::HandleFIG0Extension3 (uint8_t	*d,
	                                  int16_t	used,
	                                  uint8_t	CN_bit,
	                                  uint8_t	OE_bit,
	                                  uint8_t	PD_bit) {
int16_t SCId            = getBits   (d, used * 8,  12);
int16_t CAOrgflag       = getBits_1 (d, used * 8 + 15);
int16_t DGflag          = getBits_1 (d, used * 8 + 16);
int16_t DSCTy           = getBits_6 (d, used * 8 + 18);
int16_t SubChId         = getBits_6 (d, used * 8 + 24);
int16_t packetAddress   = getBits   (d, used * 8 + 30, 10);
uint16_t  CAOrg		= 0;

int	serviceCompIndex;
int	serviceIndex;
dabConfig	*localBase = CN_bit == 0 ? currentConfig : nextConfig;

	(void)OE_bit; (void)PD_bit;

	if (CAOrgflag == 1) {
	   CAOrg = getBits (d, used * 8 + 40, 16);
	   used += 16 / 8;
	}
	(void)CAOrg;
	used += 40 / 8;

	serviceCompIndex = findServiceComponent (localBase, SCId);
	if (serviceCompIndex == -1)
	   return used;

//	We want to have the subchannel OK
	if (!localBase -> subChannels [SubChId]. inUse)
	   return used;
//
//	If the component exists, we first look whether is
//	was already handled
	if (localBase -> serviceComps [serviceCompIndex]. is_madePublic)
	   return used;
//
//	if the Data Service Component Type == 0, we do not deal
//	with it
	if (DSCTy == 0)
	   return used;
//
	serviceIndex =
	    findService (localBase -> serviceComps [serviceCompIndex]. SId);
	if (serviceIndex == -1)
	   return used;

	QString serviceName =
	    ensemble -> services [serviceIndex]. serviceLabel;

	if (!ensemble -> services [serviceIndex]. is_shown) {
	   addtoEnsemble (serviceName,
	                  ensemble -> services [serviceIndex]. SId);
	}
	ensemble -> services [serviceIndex]. is_shown			= true;
	localBase -> serviceComps [serviceCompIndex]. is_madePublic	= true;
	localBase -> serviceComps [serviceCompIndex]. subchannelId 	= SubChId;
	localBase -> serviceComps [serviceCompIndex]. DSCTy		= DSCTy;
	localBase -> serviceComps [serviceCompIndex]. DGflag		= DGflag;
	localBase -> serviceComps [serviceCompIndex]. packetAddress	= packetAddress;
	return used;
}

//	Service component language 8.1.2
void	fibDecoder::FIG0Extension5 (uint8_t *d) {
int16_t	used		= 2;		// offset in bytes
int16_t	Length		= getBits_5 (d, 3);
uint8_t CN_bit          = getBits_1 (d, 8 + 0);
uint8_t OE_bit          = getBits_1 (d, 8 + 1);
uint8_t PD_bit          = getBits_1 (d, 8 + 2);

	while (used < Length) {
	   used = HandleFIG0Extension5 (d, CN_bit, OE_bit, PD_bit, used);
	}
}

int16_t	fibDecoder::HandleFIG0Extension5 (uint8_t* d,
	                                  uint8_t CN_bit,
	                                  uint8_t OE_bit,
	                                  uint8_t PD_bit, int16_t offset) {
int16_t	bitOffset	= offset * 8;
uint8_t	lsFlag	= getBits_1 (d, bitOffset);
int16_t language;
dabConfig	*localBase	= CN_bit == 0 ? currentConfig : nextConfig;

	(void)OE_bit; (void)PD_bit;
	if (lsFlag == 0) {	// short form
	   if (getBits_1 (d, bitOffset + 1) == 0) {
	      int16_t subChId	= getBits_6 (d, bitOffset + 2);
	      language	= getBits_8 (d, bitOffset + 8);
	      localBase -> subChannels [subChId]. language = language;
	   }
	   bitOffset += 16;
	}
	else {			// long form
	   int16_t SCId		= getBits (d, bitOffset + 4, 12);
	   language		= getBits_8 (d, bitOffset + 16);
	   int compIndex = findServiceComponent (localBase, SCId);

	   if (compIndex != -1)
	      localBase -> serviceComps [compIndex]. language = language;
	   bitOffset += 24;
	}

	return bitOffset / 8;
}

//
// FIG0/7: Configuration linking information 6.4.2,
//
void    fibDecoder::FIG0Extension7 (uint8_t *d) {
int16_t used		= 2;            // offset in bytes
int16_t Length          = getBits_5 (d, 3);
uint8_t CN_bit          = getBits_1 (d, 8 + 0);
uint8_t OE_bit          = getBits_1 (d, 8 + 1);
uint8_t PD_bit          = getBits_1 (d, 8 + 2);

int     serviceCount	= getBits_6 (d, used * 8);
int     counter		= getBits   (d, used * 8 + 6, 10);

	(void)Length;
	(void)CN_bit; (void)OE_bit; (void)PD_bit;
	fprintf (stderr, "services : %d\n", serviceCount);
	if (CN_bit == 0)	// only current configuration for now
	   nrServices (serviceCount);
	(void)counter;
}

// FIG0/8:  Service Component Global Definition (6.3.5)
//	
void	fibDecoder::FIG0Extension8 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
int16_t	Length		= getBits_5 (d, 3);
uint8_t	CN_bit		= getBits_1 (d, 8 + 0);
uint8_t	OE_bit		= getBits_1 (d, 8 + 1);
uint8_t	PD_bit		= getBits_1 (d, 8 + 2);

	while (used < Length) {
	   used = HandleFIG0Extension8 (d, used, CN_bit, OE_bit, PD_bit);
	}
}

int16_t	fibDecoder::HandleFIG0Extension8 (uint8_t	*d,
	                                  int16_t	used,
	                                  uint8_t	CN_bit,
	                                  uint8_t	OE_bit,
	                                  uint8_t	PD_bit) {
int16_t	bitOffset	= used * 8;
uint32_t	SId	= getLBits (d, bitOffset, PD_bit == 1 ? 32 : 16);
uint8_t		lsFlag;
uint16_t	SCIds;
uint8_t		extensionFlag;
dabConfig	*localBase	= CN_bit == 0 ? currentConfig : nextConfig;

	(void)OE_bit;
	bitOffset	+= PD_bit == 1 ? 32 : 16;
	extensionFlag   = getBits_1 (d, bitOffset);
	SCIds		= getBits_4 (d, bitOffset + 4);

//	int serviceIndex = findService (SId);
	bitOffset	+= 8;
	lsFlag		= getBits_1 (d, bitOffset);

	if (lsFlag == 0) {	// short form
	   int16_t compIndex;
	   int16_t subChId	= getBits_6 (d, bitOffset + 2);
	   if  (localBase -> subChannels [subChId]. inUse) {
	      compIndex = findComponent (localBase, SId, subChId);
	      if (compIndex != -1) {
	         localBase -> serviceComps [compIndex]. SCIds = SCIds;
	      }
	   }
	   bitOffset += 8;
	}
	else {			// long form
	   int SCId	= getBits (d, bitOffset + 4, 12);
	   int16_t compIndex = findServiceComponent (localBase, SCId);
	   if (compIndex != -1) {
	      localBase -> serviceComps [compIndex]. SCIds = SCIds;
	   }
	   bitOffset += 8;
	}
	if (extensionFlag)
	   bitOffset += 8;	// skip Rfa
	return bitOffset / 8;
}
//
//	User Application Information 6.3.6
void	fibDecoder::FIG0Extension13 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
int16_t	Length		= getBits_5 (d, 3);
uint8_t	CN_bit		= getBits_1 (d, 8 + 0);
uint8_t	OE_bit		= getBits_1 (d, 8 + 1);
uint8_t	PD_bit		= getBits_1 (d, 8 + 2);

	while (used < Length) 
	   used = HandleFIG0Extension13 (d, used, CN_bit, OE_bit, PD_bit);
}
//
//	section 6.3.6 User application Data
int16_t	fibDecoder::HandleFIG0Extension13 (uint8_t *d,
	                                   int16_t used,
	                                   uint8_t CN_bit,
	                                   uint8_t OE_bit,
	                                   uint8_t pdBit) {
int16_t	bitOffset	= used * 8;
//	fprintf (stderr, "FIG13: pdBit = %d, bitOffset = %d\n",
//	                     pdBit, bitOffset);
uint32_t	SId	= getLBits (d, bitOffset, pdBit == 1 ? 32 : 16);
uint16_t	SCIds;
int16_t		NoApplications;
int16_t		i;
int16_t		appType;
dabConfig	*localBase	= CN_bit == 0 ? currentConfig : nextConfig;

	(void)OE_bit;
	bitOffset	+= pdBit == 1 ? 32 : 16;
	SCIds		= getBits_4 (d, bitOffset);
	NoApplications	= getBits_4 (d, bitOffset + 4);
	bitOffset	+= 8;

	int serviceIndex = findService (SId);

	for (i = 0; i < NoApplications; i ++) {
	   appType		= getBits (d, bitOffset, 11);
	   int16_t length	= getBits_5 (d, bitOffset + 11);
	   bitOffset 		+= (11 + 5 + 8 * length);

	   if (serviceIndex == -1)
	      continue;

	   int compIndex =
	               findServiceComponent (localBase, SId, SCIds);
	   if (compIndex != -1) {
	      if (localBase -> serviceComps [compIndex]. TMid == 3)
	         localBase -> serviceComps [compIndex]. appType = appType;
	          
	   }
	}
	return bitOffset / 8;
}

//	FEC sub-channel organization 6.2.2
void	fibDecoder::FIG0Extension14 (uint8_t *d) {
int16_t	Length		= getBits_5 (d, 3);	// in Bytes
uint8_t	CN_bit		= getBits_1 (d, 8 + 0);
uint8_t	OE_bit		= getBits_1 (d, 8 + 1);
uint8_t	PD_bit		= getBits_1 (d, 8 + 2);
int16_t	used	= 2;			// in Bytes
dabConfig	*localBase	= CN_bit == 0 ? currentConfig : nextConfig;

	(void)OE_bit; (void)PD_bit;
	while (used < Length) {
	   int16_t subChId	= getBits_6 (d, used * 8);
	   uint8_t FEC_scheme	= getBits_2 (d, used * 8 + 6);
	   used = used + 1;
	   if (localBase -> subChannels [subChId]. inUse)
	      localBase -> subChannels [subChId]. FEC_scheme = FEC_scheme;
	}
}

void	fibDecoder::FIG0Extension17 (uint8_t *d) {
int16_t	length	= getBits_5 (d, 3);
int16_t	offset	= 16;
int	serviceIndex;

	while (offset < length * 8) {
	   uint16_t	SId	= getBits (d, offset, 16);
	   bool	L_flag	= getBits_1 (d, offset + 18);
	   bool	CC_flag	= getBits_1 (d, offset + 19);
	   int16_t type;
	   int16_t Language = 0x00;	// init with unknown language
	   serviceIndex	= findService (SId);
	   if (L_flag) {		// language field present
	      Language = getBits_8 (d, offset + 24);
	      offset += 8;
	   }

	   type	= getBits_5 (d, offset + 27);
	   if (CC_flag)			// cc flag
	      offset += 40;
	   else
	      offset += 32;
	   if (serviceIndex != -1) {
	      ensemble -> services [serviceIndex]. language	= Language;
	      ensemble -> services [serviceIndex]. programType	= type;
	   }
	}
}
//
//	Announcement support 8.1.6.1
void	fibDecoder::FIG0Extension18 (uint8_t *d) {
	(void)d;
int16_t	Length		= getBits_5 (d, 3);	// in Bytes
uint8_t	CN_bit		= getBits_1 (d, 8 + 0);
uint8_t	OE_bit		= getBits_1 (d, 8 + 1);
uint8_t	PD_bit		= getBits_1 (d, 8 + 2);
int16_t	used	= 2;			// in Bytes
int16_t	bitOffset		= used * 8;
dabConfig	*localBase	= CN_bit == 0 ? currentConfig : nextConfig;

	(void)OE_bit; (void)PD_bit;

	while (bitOffset < Length * 8) {
	   uint16_t SId		= getBits (d, bitOffset, 16);
	   int16_t serviceIndex = findService (SId);
	   bitOffset		+= 16;
	   uint16_t asuFlags	= getBits (d, bitOffset, 16);
	   (void)asuFlags;
	   bitOffset		+= 16;
	   uint8_t Rfa		= getBits (d, bitOffset,  5);
	   (void)Rfa;
	   uint8_t nrClusters	= getBits (d, bitOffset + 5, 3);
	   bitOffset		+= 8;

	   for (int i = 0; i < nrClusters; i ++) {
	      if (getBits (d, bitOffset + 8 * i, 8) == 0)
	         continue;
	      if ((serviceIndex != -1) &&
	          (ensemble -> services [serviceIndex]. hasName))
	         setCluster (localBase,
	                     getBits (d, bitOffset + 8 * i, 8),
	                     serviceIndex, asuFlags);
	   }
	   bitOffset	+= nrClusters * 8;
	}
}
//
//	Announcement switching 8.1.6.2
void	fibDecoder::FIG0Extension19 (uint8_t *d) {	
int16_t	Length		= getBits_5 (d, 3);	// in Bytes
uint8_t	CN_bit		= getBits_1 (d, 8 + 0);
uint8_t	OE_bit		= getBits_1 (d, 8 + 1);
uint8_t	PD_bit		= getBits_1 (d, 8 + 2);
int16_t	used		= 2;			// in Bytes
int16_t	bitOffset	= used * 8;
dabConfig *localBase	= CN_bit == 0 ? currentConfig : nextConfig;

	(void)OE_bit; (void)PD_bit;
	while (bitOffset < Length * 8) {
	   uint8_t clusterId	= getBits (d, bitOffset, 8);
	   bitOffset += 8;
	   uint16_t AswFlags	= getBits (d, bitOffset, 16);
	   bitOffset		+= 16;

	   uint8_t newFlag	= getBits (d, bitOffset, 1);
	   (void)newFlag;
	   bitOffset	+= 1;
	   uint8_t regionFlag	= getBits (d, bitOffset, 1);
	   bitOffset	+= 1;
	   uint8_t subChId	= getBits (d, bitOffset, 6);
	   bitOffset	+= 6;
	   if (regionFlag == 1) {
	      bitOffset		+= 2;	// skip Rfa
	      uint8_t regionId	= getBits (d, bitOffset, 6);
	      bitOffset 	+= 6;
	      (void)regionId;
	   }

	   if (!syncReached ())
	      return;
	   Cluster *myCluster = getCluster (localBase, clusterId);
	   if (myCluster == nullptr) {	// should not happen
//	      fprintf (stderr, "cluster fout\n");
	      return;
	   }
	      
	   if ((myCluster -> flags & AswFlags) != 0) {
	      myCluster -> announcing ++;
	      if (myCluster -> announcing == 5) {
	         for (uint16_t i = 0;
	                 i < myCluster -> services. size (); i ++) {
	            const QString name =
	              ensemble	-> services [myCluster -> services [i]]. serviceLabel;
	              emit startAnnouncement (name, subChId);
	         }
	      }
	   }
	   else {	// end of announcement
	      if (myCluster -> announcing > 0) {
	         myCluster -> announcing = 0;
	         for (uint16_t i = 0;
	                 i < myCluster -> services. size (); i ++) {
	            const QString name =
	               ensemble  -> services [myCluster -> services [i]]. serviceLabel;
                     emit stopAnnouncement (name, subChId);
                 }
	      }
	   }
	}
}
//
//	Frequency information (FI) 8.1.8
void	fibDecoder::FIG0Extension21 (uint8_t *d) {
int16_t	used		= 2;		// offset in bytes
int16_t	Length		= getBits_5 (d, 3);
uint8_t	CN_bit		= getBits_1 (d, 8 + 0);
uint8_t	OE_bit		= getBits_1 (d, 8 + 1);
uint8_t	PD_bit		= getBits_1 (d, 8 + 2);

	while (used < Length) 
	   used = HandleFIG0Extension21 (d, CN_bit, OE_bit, PD_bit, used);
}

int16_t	fibDecoder::HandleFIG0Extension21 (uint8_t	*d,
	                                   uint8_t	CN_bit,
	                                   uint8_t	OE_bit,
	                                   uint8_t	PD_bit,
	                                   int16_t	offset) {
int16_t	l_offset	= offset * 8;
int16_t	l	= getBits_5 (d, l_offset + 11);
int16_t		upperLimit	= l_offset + 16 + l * 8;
int16_t		base		= l_offset + 16;

	(void)CN_bit; (void)OE_bit, (void)PD_bit;

	while (base < upperLimit) {
	   uint16_t idField	= getBits (d, base, 16);
	   uint8_t  RandM	= getBits_4 (d, base + 16);
	   uint8_t  continuity	= getBits_1 (d, base + 20);
	   (void)continuity;
	   uint8_t  length	= getBits_3 (d, base + 21);
	   if (RandM == 0x08) {
	      uint16_t fmFrequency_key	= getBits (d, base + 24, 8);
	      int32_t  fmFrequency	= 87500 + fmFrequency_key * 100;
	      int16_t serviceIndex	= findService (idField);
	      if (serviceIndex != -1) { 
	         if ((ensemble -> services [serviceIndex]. hasName) &&
	             (ensemble -> services [serviceIndex]. fmFrequency == -1))
	               ensemble -> services [serviceIndex].fmFrequency =
	                                                        fmFrequency;
	      }
	   }
	   base += 24 + length * 8;
	}
	         
	return upperLimit / 8;
}
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

	   case 2:		// obsolete
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
	      FIG1Extension6 (d);
	      break;

	   default:
	      ;
	}
}
//	Name of the ensemble
//
void	fibDecoder::FIG1Extension0 (uint8_t *d) {
uint8_t		charSet, extension;
uint8_t		Rfu;
uint32_t	EId	= 0;
int16_t		offset	= 0;
char		label [17];

//      from byte 1 we deduce:
	charSet         = getBits_4 (d, 8);
	Rfu             = getBits_1 (d, 8 + 4);
	extension       = getBits_3 (d, 8 + 5);
	label [16]      = 0x00;
	(void)Rfu;
	(void)extension;

	EId	= getBits (d, 16, 16);
	offset	= 32;
	if ((charSet <= 16)) { // EBU Latin based repertoire
	   for (int i = 0; i < 16; i ++) {
	      label [i] = getBits_8 (d, offset + 8 * i);
	   }
//         fprintf (stderr, "Ensemblename: %16s\n", label);
	   const QString name = toQStringUsingCharset (
	                                        (const char *) label,
	                                        (CharacterSet) charSet);
	   if (!ensemble -> namePresent) {
	      ensemble ->  ensembleName	= name;
	      ensemble ->  ensembleId	= EId;
	      ensemble ->  namePresent	= true;
	      nameofEnsemble (EId, name);
	   }
	   ensemble	-> isSynced = true;
	}
}
//
//	Name of service
void	fibDecoder::FIG1Extension1 (uint8_t *d) {
uint8_t		charSet, extension;
uint8_t		Rfu;
int32_t		SId	= getBits (d, 16, 16);
int16_t		offset	= 32;
int		serviceIndex;
int16_t		i;
char		label [17];

//      from byte 1 we deduce:
	charSet         = getBits_4 (d, 8);
	Rfu             = getBits_1 (d, 8 + 4);
	extension       = getBits_3 (d, 8 + 5);
	label [16]      = 0x00;
	(void)Rfu;
	(void)extension;
	if (charSet > 16) 	// does not seem right
	   return;

	for (i = 0; i < 16; i ++) 
	   label [i] = getBits_8 (d, offset + 8 * i);
	QString dataName = toQStringUsingCharset (
	                                  (const char *) label,
	                                  (CharacterSet) charSet);
	serviceIndex	= findService (dataName);
	if (serviceIndex == -1) {
	   createService (dataName, SId, 0);
	} else {
	  ensemble -> services [serviceIndex]. SCIds = 0;
	  ensemble -> services [serviceIndex]. hasName = true;
	}
}

// service component label 8.1.14.3
void	fibDecoder::FIG1Extension4 (uint8_t *d) {
uint8_t		PD_bit;
uint8_t		SCIds;
uint8_t		Rfu;
uint32_t	SId;
int16_t		offset;

	PD_bit	= getBits_1 (d, 16);
	Rfu	= getBits_3 (d, 17);	(void)Rfu;
	SCIds	= getBits_4 (d, 20);

	if (PD_bit) {	// 32 bit identifier field for data components
	   SId		= getLBits (d, 24, 32);
	   offset	= 56;
	}
	else {	// 16 bit identifier field for program components
	   SId		= getLBits (d, 24, 16);
	   offset	= 40;
	}

	char label [17];
	label [16] = 0;
	for (int i = 0; i < 16; i ++) {
	   label [i] = getBits_8 (d, offset + 8 * i);
	}

	int charSet		= getBits_4 (d, 8);
	QString dataName	=
	               toQStringUsingCharset ((const char *) label,
                                              (CharacterSet) charSet);
	int16_t compIndex =
	            findServiceComponent (currentConfig, SId, SCIds);
	if (compIndex > 0) {
	   if (findService (dataName) == -1) {
	      if (currentConfig -> serviceComps [compIndex]. TMid == 0) {
	         createService (dataName, SId, SCIds);
	         addtoEnsemble (dataName, SId);
	      }
	   }
	}
}

//	Data service label - 32 bits 8.1.14.2
void	fibDecoder::FIG1Extension5 (uint8_t *d) {
uint8_t		charSet, extension;
uint8_t		Rfu;
int		serviceIndex;
int16_t		i;
char		label [17];

uint32_t	SId	= getLBits (d, 16, 32);
int16_t		offset	= 48;

//      from byte 1 we deduce:
	charSet         = getBits_4 (d, 8);
	Rfu             = getBits_1 (d, 8 + 4);
	extension       = getBits_3 (d, 8 + 5);
	label [16]      = 0x00;
	(void)Rfu;
	(void)extension;

	
	serviceIndex   = findService (SId);
	if (serviceIndex != -1)
	   return;

	if (charSet > 16) 
	   return;	// something wrong

	for (i = 0; i < 16; i ++) {
	   label [i] = getBits_8 (d, offset + 8 * i);
	}

 	QString serviceName = toQStringUsingCharset ((const char *) label,
	                                             (CharacterSet) charSet);
	createService (serviceName, SId, 0);
}

//	XPAD label - 8.1.14.4
void	fibDecoder::FIG1Extension6 (uint8_t *d) {
uint32_t	SId	= 0;
uint8_t		Rfu;
int16_t		offset	= 0;
uint8_t		PD_bit;
uint8_t		SCIds;
uint8_t		XPAD_apptype;

	PD_bit	= getBits_1 (d, 16);
	Rfu	= getBits_3 (d, 17);	(void)Rfu;
	SCIds	= getBits_4 (d, 20);
	if (PD_bit) {	// 32 bits identifier for XPAD label
	   SId		= getLBits (d, 24, 32);
	   XPAD_apptype	= getBits_5 (d, 59);
	   offset 	= 64;
	}
	else {	// 16 bit identifier for XPAD label
	   SId		= getLBits (d, 24, 16);
	   XPAD_apptype	= getBits_5 (d, 43);
	   offset		= 48;
	}

	(void)SId; (void)SCIds; (void)XPAD_apptype; 
	(void)offset;
}
//	Programme Type (PTy) 8.1.5
/////////////////////////////////////////////////////////////////////////
//	Support functions
//
//	bind_audioService is the main processor for - what the name suggests -
//	connecting the description of audioservices to a SID
//	by creating a service Component
void	fibDecoder::bind_audioService (dabConfig *base,
	                               int8_t	TMid,
	                               uint32_t	SId,
	                               int16_t	compnr,
	                               int16_t	subChId,
	                               int16_t	ps_flag,
	                               int16_t	ASCTy) {
int16_t	i;
int16_t	firstFree	= -1;
bool	showFlag	= true;
int	serviceIndex	= findService (SId);

	if (serviceIndex == -1)
	   return;

	if (ensemble -> services [serviceIndex]. programType == 0)
	   return;
	if (!base -> subChannels [subChId]. inUse)
	   return;

	for (i = 0; i < 64; i ++) {
	   if (!base -> serviceComps [i]. inUse) {
	      if (firstFree == -1)
	         firstFree = i;
	      break;
	   }

	   if ((base -> serviceComps [i]. SId == SId) &&
	       (base -> serviceComps [i]. componentNr == compnr))
	      return;
	}

	QString dataName = ensemble -> services [serviceIndex]. serviceLabel;
	if (ensemble -> services [serviceIndex]. is_shown)
	   showFlag = false;

	bool	useFlag	 = base -> serviceComps [firstFree]. inUse;
	if (!useFlag) {
	   base -> serviceComps [firstFree]. SId		= SId;
	   base	-> serviceComps [firstFree]. SCIds		= 0;
	   base	-> serviceComps [firstFree]. TMid		= TMid;
	   base	-> serviceComps [firstFree]. componentNr	= compnr;
	   base	-> serviceComps [firstFree]. subchannelId	= subChId;
	   base	-> serviceComps [firstFree]. PS_flag		= ps_flag;
	   base	-> serviceComps [firstFree]. ASCTy		= ASCTy;
	   base	-> serviceComps [firstFree]. inUse		= true;
	   ensemble -> services [serviceIndex]. SCIds		= 0;

	   addtoEnsemble (dataName, SId);
	}
	ensemble -> services [serviceIndex]. is_shown	= true;
}

//      bind_packetService is the main processor for - what the name suggests -
//      connecting the service component defining the service to the SId,
//	So, here we create a service component. Note however,
//	that FIG0/3 provides additional data, after that we
//	decide whether it should be visible or not
void    fibDecoder::bind_packetService (dabConfig *base,
	                                int8_t	TMid,
	                                uint32_t SId,
	                                int16_t	compnr,
	                                int16_t SCId,
	                                int16_t ps_flag,
	                                int16_t CAflag) {
int serviceIndex;
int16_t i;
QString name;
int	firstFree = -1;

	serviceIndex = findService (SId);
	if (serviceIndex == -1) {
	   return;
	}

	QString serviceName = ensemble -> services [serviceIndex]. serviceLabel;

	for (i = 0; i < 64; i ++) {
	   if (!base -> serviceComps [i]. inUse) {
	      if (firstFree == -1)
	         firstFree = i;
	      break;
	   }

	   if ((base -> serviceComps [i]. SId == SId) &&
	       (base -> serviceComps [i]. componentNr == compnr))
	      return;
	}

	if (!base -> serviceComps [firstFree]. inUse) {
	   base -> serviceComps [firstFree]. inUse		= true;
	   base -> serviceComps [firstFree]. SId		= SId;
	   if (compnr == 0)
	      base -> serviceComps [firstFree]. SCIds		= 0;
	   else
	      base -> serviceComps [firstFree]. SCIds		= -1;
	   base -> serviceComps [firstFree]. SCId		= SCId;
	   base -> serviceComps [firstFree]. TMid		= TMid;
	   base -> serviceComps [firstFree]. componentNr	= compnr;
	   base -> serviceComps [firstFree]. PS_flag		= ps_flag;
	   base -> serviceComps [firstFree]. CAflag		= CAflag;
	   base -> serviceComps [firstFree]. is_madePublic	= false;
	}
}

static inline
bool	match (QString s1, QString s2) {
	return s1 == s2;
}
//
int	fibDecoder::findService		(const QString &s) {
int	i;
	for (i = 0; i < 64; i ++) {
	   if (!ensemble -> services [i]. inUse)
	      continue;
	   if (match (s, ensemble -> services [i]. serviceLabel))
	      return i;
	}
	return -1;
}

int	fibDecoder::findService	 (uint32_t SId) {
int	i;
	for (i = 0; i < 64; i ++) {
	   if (!ensemble -> services [i]. inUse)
	      return -1;
	   if (ensemble -> services [i]. SId == SId)
	      return i;
	}
	return -1;
}

//	find data component using the SCId
int	fibDecoder::findServiceComponent (dabConfig *db, int16_t SCId) {
	for (int i = 0; i < 64; i ++)
	   if (db -> serviceComps [i]. inUse &&
	       (db -> serviceComps [i]. SCId == SCId))
	      return i;
	return -1;
}
//
//	find serviceComponent using the SId and the SCIds
int	fibDecoder::findServiceComponent (dabConfig *db, 
	                                   uint32_t SId, uint8_t SCIds) {

int serviceIndex = findService (SId);
	if (serviceIndex == -1)
	   return -1;
	
	for (int i = 0; i < 64; i ++) {
	   if (!db -> serviceComps [i]. inUse)
	      return -1;
	   if ((db -> serviceComps [i]. SCIds == SCIds) &&
	       (db -> serviceComps [i]. SId == SId))
	      return i;
	}
	return -1;
}

//	find serviceComponent using the SId and the subchannelId
int	fibDecoder::findComponent	(dabConfig *db,
	                                 uint32_t SId, int16_t subChId) {
	for (int i = 0; i < 64; i ++) {
	   if (!db -> serviceComps [i]. inUse)
	      return -1;
	   if ((db ->serviceComps [i]. SId == SId) &&
	       (db -> serviceComps [i]. subchannelId == subChId))
	      return i;
	}
	return -1;
}

void	fibDecoder::createService (QString name, uint32_t SId, int SCIds) {
	for (int i = 0; i < 64; i ++) {
	   if (ensemble -> services [i]. inUse) {
	      continue;
	   }
	   ensemble	-> services [i]. inUse		= true;
	   ensemble	-> services [i]. hasName	= true;
	   ensemble	-> services [i]. serviceLabel	= name;
	   ensemble	-> services [i]. SId		= SId;
	   ensemble	-> services [i]. SCIds		= SCIds;
	   return;
	}
}
//
//	called after a change in configuration to verify
//	the services health
//
void	fibDecoder::cleanupServiceList () {
	for (int i = 0; i < 64; i ++) {
	   if (!ensemble -> services [i]. inUse)
	      continue;
	   uint32_t SId		= ensemble -> services [i]. SId;
	   int	    SCIds	= ensemble -> services [i]. SCIds;
	   if (findServiceComponent (currentConfig, SId, SCIds) == -1) {
	      ensemble -> services [i]. inUse = false;
	   }
	}
}

QString	fibDecoder::announcements (uint16_t a) {
	switch (a) {
	   case 0:
	   default:
	      return QString ("Alarm");

	   case 1:
	      return QString ("Road Traffic Flash");

	   case 2:
	      return QString ("Traffic Flash");

	   case 4:
	      return QString ("Warning/Service");
	
	   case 8:
	      return QString ("News Flash");

	   case 16:
	      return QString ("Area Weather flash");

	   case 32:
	      return QString ("Event announcement");

	   case 64:
	      return QString ("Special Event");

	   case 128:
	      return QString ("Programme Information");
	}
}

void	fibDecoder::setCluster (dabConfig *localBase, int clusterId,
	                        int16_t serviceIndex, uint16_t asuFlags) {

	if (!syncReached ())
	   return;
	Cluster *myCluster = getCluster (localBase, clusterId);
	if (myCluster == nullptr)
	   return;
	if (myCluster -> flags != asuFlags) {
//	   fprintf (stderr, "for cluster %d, the flags change from %x to %x\n",
//	                       clusterId, myCluster -> flags, asuFlags);
	   myCluster -> flags = asuFlags;
	}

	for (uint16_t i = 0; i < myCluster -> services. size (); i ++)
	   if (myCluster -> services [i] == serviceIndex)
	      return;
	myCluster -> services. push_back (serviceIndex);
}

Cluster	*fibDecoder::getCluster (dabConfig *localBase, int16_t clusterId) {
	for (int i = 0; i < 64; i ++)
	   if ((localBase -> clusterTable [i]. inUse) &&
	       (localBase -> clusterTable [i]. clusterId == clusterId))
	      return &(localBase -> clusterTable [i]);

	for (int i = 0; i < 64; i ++) {
	   if (!localBase -> clusterTable [i]. inUse) {
	      localBase -> clusterTable [i]. inUse = true;
	      localBase -> clusterTable [i]. clusterId = clusterId;
	      return &(localBase -> clusterTable [i]);
	   }
	}
	return &(localBase -> clusterTable [0]);	// cannot happen
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//	
//	Implementation of API functions
//
void	fibDecoder::clearEnsemble() {
	
	fibLocker. lock();
}

void	fibDecoder::connect_channel () {
	fibLocker. lock();
	currentConfig	-> reset ();
	nextConfig	-> reset ();
	ensemble	-> reset ();
	connect (this, SIGNAL (addtoEnsemble (const QString &, int)),
	         myRadioInterface, SLOT (addtoEnsemble (const QString &, int)));
	fibLocker. unlock();
}

void	fibDecoder::disconnect_channel () {
	fibLocker. lock ();
	disconnect (this, SIGNAL (addtoEnsemble (const QString &, int)),
	         myRadioInterface, SLOT (addtoEnsemble (const QString &, int)));
	currentConfig	-> reset ();
	nextConfig	-> reset ();
	ensemble	-> reset ();
	fibLocker. unlock();
}

bool	fibDecoder::syncReached() {
	return  ensemble -> isSynced;
}

int	fibDecoder::getSubChId	(const QString &s, uint32_t dummy_SId) {
int serviceIndex	= findService (s);

	(void)dummy_SId;
	fibLocker. lock();

	int SId		= ensemble	-> services [serviceIndex]. SId;
	int SCIds	= ensemble	-> services [serviceIndex]. SCIds;

	int compIndex	= findServiceComponent (currentConfig, SId, SCIds);
	if (compIndex == -1) {
	   fibLocker. unlock ();
	   return -1;
	}

	int subChId = currentConfig -> serviceComps [compIndex]. subchannelId;
	fibLocker. unlock ();
	return subChId;
}

void	fibDecoder::dataforAudioService	(const QString &s, audiodata *ad) {
int	serviceIndex;

	ad       -> defined      = false;	// default
	serviceIndex		= findService (s);
	if (serviceIndex == -1)
	   return;

	fibLocker. lock();

	int SId		= ensemble	-> services [serviceIndex]. SId;
	int SCIds	= ensemble	-> services [serviceIndex]. SCIds;

	int compIndex	= findServiceComponent (currentConfig, SId, SCIds);
	if (compIndex == -1) {
	   fibLocker. unlock ();
	   return;
	}

	if (currentConfig -> serviceComps [compIndex]. TMid != 0) {
	   fibLocker. unlock ();
	   return;
	}

	int subChId	=
	           currentConfig -> serviceComps [compIndex]. subchannelId;
	if (!currentConfig -> subChannels [subChId]. inUse) {
	   fibLocker. unlock ();
	   return;
	}

	ad	-> SId		= SId;
	ad	-> SCIds	= SCIds;
	ad	-> subchId      = subChId;
	ad	-> serviceName  = s;
	ad	-> startAddr    =
	                 currentConfig -> subChannels [subChId]. startAddr;
	ad	-> shortForm    =
	                 currentConfig -> subChannels [subChId]. shortForm;
	ad	-> protLevel    =
	                 currentConfig -> subChannels [subChId]. protLevel;
	ad	-> length       =
	                 currentConfig -> subChannels [subChId]. Length;
	ad	-> bitRate      =
	                 currentConfig -> subChannels [subChId]. bitRate;
	ad	-> ASCTy        = 
	                 currentConfig -> serviceComps [compIndex]. ASCTy;
	ad	-> language     = 
	                 ensemble -> services [serviceIndex]. language;
	ad	-> programType	=
	                 ensemble -> services [serviceIndex]. programType;
	ad	-> fmFrequency	=
	                 ensemble -> services [serviceIndex]. fmFrequency;
	ad	-> defined	= true;

	fibLocker. unlock();
}

void	fibDecoder::dataforPacketService (const QString &s,
	                                  packetdata *pd, int16_t SCIds) {
int     serviceIndex;

	pd       -> defined      = false;
	serviceIndex    = findService (s);
	if (serviceIndex == -1)
	   return;

	fibLocker. lock();

	int	SId	= ensemble -> services [serviceIndex]. SId;
	
	int compIndex	= findServiceComponent (currentConfig, SId, SCIds);

	if ((compIndex == -1) ||
	            (currentConfig -> serviceComps [compIndex]. TMid != 3)) {
	   fibLocker. unlock ();
	   return;
	}

	int subchId	=
	           currentConfig -> serviceComps [compIndex]. subchannelId;
	if (!currentConfig -> subChannels [subchId]. inUse) {
	   fibLocker. unlock ();
	   return;
	}

	pd	-> serviceName	= s;
	pd	-> SId		= SId;
	pd	-> SCIds	= SCIds;
	pd	-> subchId      = subchId;
	pd	-> startAddr    =
	                     currentConfig -> subChannels [subchId]. startAddr;
	pd	-> shortForm    = 
	                     currentConfig -> subChannels [subchId]. shortForm;
	pd	-> protLevel    = 
	                     currentConfig -> subChannels [subchId]. protLevel;
	pd	-> length       = 
	                     currentConfig -> subChannels [subchId]. Length;
	pd	-> bitRate      = 
	                     currentConfig -> subChannels [subchId]. bitRate;
	pd	-> FEC_scheme   = 
	                     currentConfig -> subChannels [subchId]. FEC_scheme;
	pd	-> DSCTy        = 
	                     currentConfig -> serviceComps [compIndex]. DSCTy;
	pd	-> DGflag       = 
	                     currentConfig -> serviceComps [compIndex]. DGflag;
	pd	-> packetAddress = 
	                     currentConfig -> serviceComps [compIndex]. packetAddress;
	pd	-> compnr       = 
	                     currentConfig -> serviceComps [compIndex]. componentNr;
	pd	-> appType      = 
	                     currentConfig -> serviceComps [compIndex]. appType;
	pd	-> defined      = true;

	fibLocker. unlock();
}

std::vector<serviceId> fibDecoder::getServices (int order) {
std::vector<serviceId> services;

	for (int i = 0; i < 64; i ++)
	   if (ensemble -> services [i]. inUse &&
	       ensemble -> services [i]. hasName) {
	      serviceId ed;
	      ed. name = ensemble -> services [i]. serviceLabel;
	      ed. SId  = ensemble -> services [i]. SId;

	      services = insert (services, ed, order);
	   }
	return services;
}

std::vector<serviceId>
	fibDecoder::insert (std::vector<serviceId> l,
	                        serviceId n, int order) {
std::vector<serviceId> k;
	if (l . size () == 0) {
	   k. push_back (n);
	   return k;
	}
	int 	baseN		= 0;
	QString baseS		= "";
	bool	inserted	= false;
	for (const auto  &serv : l) {
	   if (!inserted &&
	         (order == ID_BASED ?
	             ((baseN < (int) n. SId) &&
	                               ((int)n. SId <= (int) (&serv) -> SId)):
	             ((baseS < n. name) && (n. name < (&serv) -> name)))) {
	      k. push_back (n);
	      inserted = true;
	   }
	   baseS	= serv. name;
	   baseN	= serv. SId;
	   k. push_back (serv);
	}
	if (!inserted)
	   k. push_back (n);
	return k;
}

QString	fibDecoder::findService (uint32_t SId, int SCIds) {
QString result;
	for (int i = 0; i < 64; i ++)
	   if (ensemble -> services [i]. inUse &&
	       (ensemble -> services [i]. SId == SId) &&
	       (ensemble -> services [i]. SCIds == SCIds))
	      return ensemble -> services [i]. serviceLabel;
	return "";
}

void	fibDecoder::getParameters	(const QString &s,
	                                 uint32_t *p_SId, int *p_SCIds) {
int	serviceIndex = findService (s);
	if (serviceIndex == -1) {
	   *p_SId	= 0;
	   *p_SCIds	= 0;
	}
	else {
	   *p_SId	= ensemble -> services [serviceIndex]. SId;
	   *p_SCIds	= ensemble -> services [serviceIndex]. SCIds;
	}
}

int32_t	fibDecoder::get_ensembleId () {
	if (ensemble -> namePresent)
	   return ensemble -> ensembleId;
	else
	   return 0;
}

QString	fibDecoder::get_ensembleName() {
	if (ensemble -> namePresent)
	   return ensemble -> ensembleName;
	else
	   return " ";
}

int32_t fibDecoder::get_CIFcount() {
	return CIFcount;
}

void	fibDecoder::get_CIFcount (int16_t *h, int16_t *l) {
	*h	= CIFcount_hi;
	*l	= CIFcount_lo;
}

uint8_t	fibDecoder::get_ecc	() {
	if (ensemble -> ecc_Present)
	   return ensemble -> ecc_byte;
	return 0;
}

uint16_t fibDecoder::get_countryName () {
	return (get_ecc () << 8) | get_countryId ();
}

uint8_t	fibDecoder::get_countryId () {
	return ensemble -> countryId;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Country, LTO & international table 8.1.3.2
void fibDecoder::FIG0Extension9 (uint8_t *d) {
int16_t	offset	= 16;
uint8_t ecc;
//
//	6 indicates the number of hours
	int	signbit = getBits_1 (d, offset + 2);
	dateTime [6] = (signbit == 1)?
	                -1 * getBits_4 (d, offset + 3):
	                     getBits_4 (d, offset + 3);
//
//	7 indicates a possible remaining half our
	dateTime [7] = (getBits_1 (d, offset + 7) == 1) ? 30 : 0;
	if (signbit == 1)
	   dateTime [7] = -dateTime [7];
	ecc	     = getBits (d, offset + 8, 8);
	if (!ensemble -> ecc_Present) {
	   ensemble -> ecc_byte = ecc;
	   ensemble -> ecc_Present = true;
	}
}

//static
//QString monthTable [] = {
//"jan", "feb", "mar", "apr", "may", "jun",
//"jul", "aug", "sep", "oct", "nov", "dec"};

int	monthLength [] {
31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
//
//	Time in 10 is given in UTC, for other time zones
//	we add (or subtract) a number of Hours (half hours)
void	adjustTime (int32_t *dateTime) {
//	first adjust the half hour  in the amount of minutes
	dateTime [4] += (dateTime [7] == 1) ? 30 : 0;
	if (dateTime [4] >= 60) {
	   dateTime [4] -= 60; dateTime [3] ++;
	}

	if (dateTime [4] < 0) {
	   dateTime [4] += 60; dateTime [3] --;
	}

	dateTime [3] += dateTime [6];
	if ((0 <= dateTime [3]) && (dateTime [3] <= 23))
	   return;

	if (dateTime [3] > 23) {
	   dateTime [3] -= 24; dateTime [2] ++;
	}

	if (dateTime [3] < 0) {
	   dateTime [3] += 24; dateTime [2] --;
	}

	if (dateTime [2] > monthLength [dateTime [1] - 1]) {
	   dateTime [2] = 1; dateTime [1] ++;
	   if (dateTime [1] > 12) {
	      dateTime [1] = 1;
	      dateTime [0] ++;
	   }
	}

	if (dateTime [2] < 0) {
	   if (dateTime [1] > 1) {
	      dateTime [2] = monthLength [dateTime [1] - 1 - 1];
	      dateTime [1] --;
	   }
	   else {
	      dateTime [2] = monthLength [11];
	      dateTime [1] = 12; dateTime [0] --;
	   }
	}
}

//QString	mapTime (int32_t *dateTime) {
//QString result	= QString::number (dateTime [0]);
//	result. append ("-");
//	result. append (monthTable [dateTime [1] - 1]);
//	result. append ("-");
//
//	QString day	= QString ("%1").
//	                      arg (dateTime [2], 2, 10, QChar ('0'));
//	result. append (day);
//	result. append (" ");
//	int hours	= dateTime [3];
//	if (hours < 0)	hours += 24;
//	if (hours >= 24) hours -= 24;
//
//	dateTime [3] = hours;
//	QString hoursasString 
//		= QString ("%1"). arg (hours, 2, 10, QChar ('0'));
//	result. append (hoursasString);
//	result. append (":");
//	QString minutesasString =
//	              QString ("%1"). arg (dateTime [4], 2, 10, QChar ('0'));
//	result. append (minutesasString);
//	return result;
//}
//
//	Date and Time
//	FIG0/10 are copied from the work of
//	Michael Hoehn
void fibDecoder::FIG0Extension10 (uint8_t *dd) {
int16_t		offset = 16;
this	->	mjd	= getLBits (dd, offset + 1, 17);

//	Modified Julian Date (recompute according to wikipedia)
int32_t J	= mjd + 2400001;
int32_t j	= J + 32044;
int32_t g	= j / 146097; 
int32_t	dg	= j % 146097;
int32_t c	= ((dg / 36524) + 1) * 3 / 4; 
int32_t dc	= dg - c * 36524;
int32_t b	= dc / 1461;
int32_t db	= dc % 1461;
int32_t a	= ((db / 365) + 1) * 3 / 4; 
int32_t da	= db - a * 365;
int32_t y	= g * 400 + c * 100 + b * 4 + a;
int32_t m	= ((da * 5 + 308) / 153) - 2;
int32_t d	= da - ((m + 4) * 153 / 5) + 122;
int32_t Y	= y - 4800 + ((m + 2) / 12); 
int32_t M	= ((m + 2) % 12) + 1; 
int32_t D	= d + 1;
int32_t	theTime	[6];

	theTime [0] = Y;	// Year
	theTime [1] = M;	// Month
	theTime [2] = D;	// Day
	theTime [3] = getBits_5 (dd, offset + 21); // Hours
	theTime [4] = getBits_6 (dd, offset + 26); // Minutes

	if (getBits_6 (dd, offset + 26) != dateTime [4]) 
	   theTime [5] =  0;	// Seconds (Ubergang abfangen)

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
	   int utc_day	= dateTime [2];
	   int utc_hour	= dateTime [3];
	   int utc_minute = dateTime [4];
	   int utc_seconds = dateTime [5];
	   adjustTime (dateTime);
	   emit  clockTime (dateTime [0], dateTime [1],
	                    dateTime [2], dateTime [3], dateTime [4],
	                            utc_day, utc_hour, utc_minute, utc_seconds);
	}
}

void	fibDecoder::set_epgData	(uint32_t SId, int32_t theTime, 
	                         const QString &theText,
	                         const QString &theDescr) {
	for (int i = 0; i < 64; i ++) {
           if (ensemble -> services [i]. inUse &&
               ensemble -> services [i]. SId == SId) {
	   
	      service *S = &(ensemble -> services [i]);
	      for (uint16_t j = 0; j < S -> epgData. size (); j ++) {
	         if (S -> epgData. at (j). theTime == theTime)  {
	            S -> epgData.at (j). theText = theText;
	            S -> epgData.at (j). theDescr = theDescr;
	            return;	
	         }
	      }
	      epgElement ep;
	      ep. theTime	= theTime;
	      ep. theText	= theText;
	      ep. theDescr	= theDescr;
	      S -> epgData. push_back (ep);
	      return;
	   }
	}
}

std::vector<epgElement> fibDecoder::get_timeTable (uint32_t SId) {
std::vector<epgElement> res;
int	index	= findService (SId);
	if (index == -1)
	   return res;
	 return ensemble -> services [index]. epgData;
}

std::vector<epgElement> fibDecoder::get_timeTable (const QString &service) {
std::vector<epgElement> res;
int	index	= findService (service);
	if (index == -1)
	   return res;
	 return ensemble -> services [index]. epgData;
}

bool	fibDecoder::has_timeTable	(uint32_t SId) {
int index	= findService (SId);
std::vector<epgElement> t;
	if (index == -1)
	   return false;
	t = ensemble -> services [index]. epgData;
	return t. size () > 2;
}

std::vector<epgElement>	fibDecoder::find_epgData	(uint32_t SId) {
int index	= findService (SId);
std::vector<epgElement> res;

	if (index == -1)
	   return res;

	service *s = &(ensemble -> services [index]);

	res = s -> epgData;
	return res;
}


//	the generic print function generates - using the component descriptors
//	as index - a QStringList as a model for a csv file
//
//	header for audio services
//	serviceName
//	serviceId
//	subChannel
//	startAddress
//	length (in CU's)
//	DAB or DAB+
//	prot level
//	code rate
//	bitrate
//	language
//	program type
//	alternative fm frequency
//
//	header for data services
//	serviceName
//	serviceId
//	subChannel
//	startAddress
//	length
//	prot level
//	code rate
//	appType
//	FEC
//	packet address
//	comp nr

QStringList	fibDecoder::basicPrint () {
QStringList out;
	bool hasContents = false;
	for (int i = 0; i < 64; i ++) {
	   if (currentConfig -> serviceComps [i]. inUse) {
	      if (currentConfig -> serviceComps [i]. TMid != 0) // audio
	         continue;
	      if (!hasContents) 
	         out << audioHeader ();
	      hasContents = true;
	      out << audioData (i);
	   }
	}
	hasContents = false;
	for (int i = 0; i < 64; i ++) {
	   if (currentConfig -> serviceComps [i]. inUse) {
	      if (currentConfig -> serviceComps [i]. TMid != 3) // packet
	         continue;
	      if (subChannelOf (i) == "")
	         continue;
	      if (!hasContents) {
	         out << "\n\n";
	         out << packetHeader ();
	      }
	      hasContents = true;
	      out << packetData (i);
	   }
	}
	return out;
}
//
//
QString	fibDecoder::serviceName		(int index) { 
int sid	= currentConfig -> serviceComps [index]. SId;
int serviceIndex	= findService (sid);
	if (serviceIndex != -1)
	   return ensemble -> services [serviceIndex]. serviceLabel;
	return "";
}

QString	fibDecoder::serviceIdOf		(int index) { 
	return QString::number (currentConfig -> serviceComps [index]. SId, 16);
}

QString	fibDecoder::subChannelOf 	(int index) {
int subChannel	= currentConfig -> serviceComps [index]. subchannelId;
	if (subChannel < 0)
	   return "";
	return QString::number (subChannel);
}

QString	fibDecoder::startAddressOf 	(int index) {
int subChannel	= currentConfig -> serviceComps [index]. subchannelId;
int startAddr	= currentConfig -> subChannels [subChannel]. startAddr;
	return QString::number (startAddr);
}

QString	fibDecoder::lengthOf 		(int index) {
int subChannel	= currentConfig -> serviceComps [index]. subchannelId;
int Length	= currentConfig -> subChannels [subChannel]. Length;
	return QString::number (Length);
}

QString	fibDecoder::protLevelOf 	(int index) {
int subChannel	= currentConfig -> serviceComps [index]. subchannelId;
bool	shortForm	= currentConfig -> subChannels [subChannel]. shortForm;
int	protLevel	= currentConfig -> subChannels [subChannel]. protLevel;
	return getProtectionLevel (shortForm, protLevel);
}

QString	fibDecoder::codeRateOf 		(int index) {
int subChannel	= currentConfig -> serviceComps [index]. subchannelId;
bool	shortForm	= currentConfig -> subChannels [subChannel]. shortForm;
int	protLevel	= currentConfig -> subChannels [subChannel]. protLevel;
	return getCodeRate (shortForm, protLevel);
}

QString	fibDecoder::bitRateOf		(int index) {
int subChannel	= currentConfig -> serviceComps [index]. subchannelId;
int bitRate	= currentConfig -> subChannels [subChannel]. bitRate;
	return QString::number (bitRate);
}

QString	fibDecoder::dabType 		(int index) {
int dabType	= currentConfig -> serviceComps [index]. ASCTy;
	return dabType == 077 ? "DAB+" : "DAB";
}

QString	fibDecoder::languageOf 		(int index) {
int subChannel	= currentConfig -> serviceComps [index]. subchannelId;
int language	= currentConfig -> subChannels [subChannel]. language;
//textMapper theMapper;

	return  getLanguage (language);
//	return theMapper. get_programm_language_string (language);
}

QString	fibDecoder::programTypeOf	(int index) {
int sid = currentConfig -> serviceComps [index]. SId;
int serviceIndex        = findService (sid);
int programType		=  ensemble -> services [serviceIndex]. programType;
//textMapper theMapper;

	return getProgramType (programType);
//	return theMapper. get_programm_type_string (programType);
}

QString	fibDecoder::fmFreqOf		(int index) {
int sid	= currentConfig -> serviceComps [index]. SId;
int serviceIndex	= findService (sid);
	return ensemble -> services [serviceIndex]. fmFrequency != -1 ?
              QString::number (ensemble -> services [serviceIndex].fmFrequency):
	                                      "    ";
}

QString	fibDecoder::appTypeOf		(int index) {
int appType	=   currentConfig -> serviceComps [index]. appType;
	return QString::number (appType);
}

QString	fibDecoder::FEC_scheme		(int index) {
int subChannel	= currentConfig -> serviceComps [index]. subchannelId;
int FEC_scheme	= currentConfig -> subChannels [subChannel]. FEC_scheme;
	return QString::number (FEC_scheme);
}

QString	fibDecoder::packetAddress	(int index) {
int packetAddr = currentConfig -> serviceComps [index]. packetAddress;
	return QString::number (packetAddr);
}

QString	fibDecoder::DSCTy		(int index) {
int DSCTy	= currentConfig -> serviceComps [index]. DSCTy;
	switch (DSCTy) {
	   case 60 :
	      return  "mot data";
	   case 59:
	      return "ip data";
	   case 44 :
	      return  "journaline data";
	   case  5 :
	      return  "tdc data";
	   default:
	      return "unknow data";
	}
}
//
QString	fibDecoder::audioHeader		() {
	return	QString ("serviceName") + ";" +
	        "serviceId" + ";" +
		"subChannel" + ";" +
		"start address (CU's)" + ";" +
		"length (CU's)" + ";" +
		"protection" + ";" +
		"code rate" + ";" +
	        "bitrate" + ";" +
		"dab type" + ";" +
		"language" + ";" +
		"program type" + ";" +
		"fm freq" + ";";
}

QString	fibDecoder::audioData		(int index) {
	return QString (serviceName (index)) + ";" +
	       serviceIdOf (index) + ";" +
	       subChannelOf (index) + ";" +
	       startAddressOf (index) + ";" +
	       lengthOf (index) + ";" +
	       protLevelOf (index) + ";" +
	       codeRateOf (index) + ";" +
	       bitRateOf (index) + ";" +
	       dabType (index) + ";" +
	       languageOf (index) + ";" +
	       programTypeOf (index) + ";" +
	       fmFreqOf (index) + ";";
}
//
QString	fibDecoder::packetHeader		() {
	return	QString ("serviceName") + ";" +
	        "serviceId" + ";" + 
		"subChannel" + ";" +
		"start address" + ";" +
		"length" + ";" +
		"protection" + ";" +
		"code rate" + ";" +
	        "appType" + ";" +
	        "FEC_scheme" + ";" +
	        "packetAddress" + ";" +
	        "DSCTy" + ";";
}

QString	fibDecoder::packetData	(int index) {
	return serviceName (index) + ";" +
	       serviceIdOf (index) + ";" +
	       subChannelOf (index) + ";" +
	       startAddressOf (index) + ";" +
	       lengthOf (index) + ";" +
	       protLevelOf (index) + ";" +
	       codeRateOf (index) + ";" +
	       appTypeOf (index) + ";" +
	       FEC_scheme (index) + ";" +
	       packetAddress (index) + ";" +
	       DSCTy (index) + ";";
}
//
//	We terminate the sequences with a ";", so that is why the
//	actual number is 1 smaller
int	fibDecoder::scanWidth	() {
QString s1	= audioHeader ();
QString s2	= packetHeader ();
QStringList l1 = s1. split (";");
QStringList l2 = s2. split (";");
	return l1. size () >= l2. size () ? l1. size () -1 : l2. size () - 1;
}

uint32_t	fibDecoder::julianDate	() {
	return mjd;
}

void	fibDecoder::get_channelInfo (channel_data *d, int n) {
        d       -> in_use	= currentConfig -> subChannels [n]. inUse;
        d       -> id		= currentConfig -> subChannels [n]. SubChId;
        d       -> start_cu	= currentConfig -> subChannels [n]. startAddr;
        d       -> protlev	= currentConfig -> subChannels [n]. protLevel; 
        d       -> size		= currentConfig -> subChannels [n]. Length;
        d       -> bitrate	= currentConfig -> subChannels [n]. bitRate;
        d       -> uepFlag	= currentConfig -> subChannels [n]. shortForm;
}

