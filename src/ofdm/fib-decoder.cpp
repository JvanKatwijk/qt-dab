#
/*
 *    Copyright (C) 2018, 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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
//
//
// Tabelle ETSI EN 300 401 Page 50
// Table is copied from the work of Michael Hoehn
   const int ProtLevel[64][3]   = {{16,5,32},	// Index 0
	                           {21,4,32},
	                           {24,3,32},
	                           {29,2,32},
	                           {35,1,32},	// Index 4
	                           {24,5,48},
	                           {29,4,48},
	                           {35,3,48},
	                           {42,2,48},
	                           {52,1,48},	// Index 9
	                           {29,5,56},
	                           {35,4,56},
	                           {42,3,56},
	                           {52,2,56},
	                           {32,5,64},	// Index 14
	                           {42,4,64},
	                           {48,3,64},
	                           {58,2,64},
	                           {70,1,64},
	                           {40,5,80},	// Index 19
	                           {52,4,80},
	                           {58,3,80},
	                           {70,2,80},
	                           {84,1,80},
	                           {48,5,96},	// Index 24
	                           {58,4,96},
	                           {70,3,96},
	                           {84,2,96},
	                           {104,1,96},
	                           {58,5,112},	// Index 29
	                           {70,4,112},
	                           {84,3,112},
	                           {104,2,112},
	                           {64,5,128},
	                           {84,4,128},	// Index 34
	                           {96,3,128},
	                           {116,2,128},
	                           {140,1,128},
	                           {80,5,160},
	                           {104,4,160},	// Index 39
	                           {116,3,160},
	                           {140,2,160},
	                           {168,1,160},
	                           {96,5,192},
	                           {116,4,192},	// Index 44
	                           {140,3,192},
	                           {168,2,192},
	                           {208,1,192},
	                           {116,5,224},
	                           {140,4,224},	// Index 49
	                           {168,3,224},
	                           {208,2,224},
	                           {232,1,224},
	                           {128,5,256},
	                           {168,4,256},	// Index 54
	                           {192,3,256},
	                           {232,2,256},
	                           {280,1,256},
	                           {160,5,320},
	                           {208,4,320},	// index 59
	                           {280,2,320},
	                           {192,5,384},
	                           {280,3,384},
	                           {416,1,384}};

//
class	service {
public:
	service	(void) {
	   clear ();
	}
	~service	(void) {
		clear ();
	}
	void		clear	(void) {
	   inUse	= false;
	   hasName	= false;
	   language	= 0;
	   programType	= 0;
	   serviceLabel	= "";
	   is_shown	= false;
	   fmFrequency	= -1;
	}
	bool		inUse;
	uint32_t	serviceId;
	bool		hasName;
	QString		serviceLabel;
	int		language;
	int		programType;
	bool		is_shown;
	int32_t		fmFrequency;
};

class ensembleDescriptor {
public:
	ensembleDescriptor (void) {
	   clear ();
	   for (int i = 0; i < 64; i ++)
	   services [i]. clear ();
	}
	~ensembleDescriptor	(void) {}
	void		clear (void) {
	   namePresent	= false;
	   ecc_Present	= false;
	   isSynced	= false;
	}
	QString ensembleName;
	int32_t ensembleId;
	bool    namePresent;
	bool	ecc_Present;
	uint8_t	ecc_byte;
	bool	isSynced;
	service	services [64];
};

class	subChannelDescriptor {
public:
	subChannelDescriptor	(void) {
	   clear ();
	}
	~subChannelDescriptor	(void) {}
	void		clear (void) {
	   inUse	= false;
	   language	= 0;
	   FEC_scheme	= 0;
	}
	bool		inUse;
	int32_t		SubChId;
	int32_t		startAddr;
	int32_t		Length;
	bool		shortForm;
	int32_t		protLevel;
	int32_t		bitRate;
	int16_t		language;
	int16_t		FEC_scheme;
	int16_t		SCIds;		// for audio channels
};

//      The service component describes the actual service
//      It really should be a union, the component data for
//      audio and data are quite different
class	serviceComponentDescriptor {
public:
	serviceComponentDescriptor	(void) {
	   clear	();
	}
	~serviceComponentDescriptor	(void) {}
	void		clear		(void) {
	   inUse		= false;
	   is_madePublic	= false;
	   SCIds		= -1;
	   componentNr		= -1;
	   SCId			= -1;
	   componentNr		= -1;
	}

	bool		inUse;		// field in use
	int8_t		TMid;		// the transport mode
	int16_t		serviceIndex;	// belongs to the service
	int16_t		subchannelId;	// used in both audio and packet
	int16_t		componentNr;    // component
	int16_t		SCIds;		// component within service
	int16_t		ASCTy;          // used for audio
	int16_t		PS_flag;	// use for both audio and packet
	uint16_t	SCId;           // Component Id (12 bit, unique)
	uint8_t		CAflag;         // used in packet (or not at all)
	int16_t		DSCTy;		// used in packet
	uint8_t		DGflag;         // used for TDC
	int16_t		packetAddress;  // used in packet
	int16_t		appType;        // used in packet and Xpad
	int16_t		language;
	bool		is_madePublic;  // used to make service visible
};

//
//	The implementation of the services is in service components
//	and subchannels. We prepare - a little - for the possibility
//	that the transmitter changes the configuration while running
class	dataBase {
public:
	dataBase	(void) {
	   clear ();
	}
	~dataBase	(void) {
	   clear ();
	}
	void	clear	(void) {
	int i;
	   for (i = 0; i < 64; i ++) {
	      subChannels  [i]. clear ();
	      serviceComps [i]. clear ();
	   }
	}
	subChannelDescriptor    subChannels [64];
	serviceComponentDescriptor      serviceComps [64];
};

//
	fibDecoder::fibDecoder (RadioInterface *mr) {
	myRadioInterface	= mr;
	memset (dateTime, 0, sizeof (dateTime));

	connect (this, SIGNAL (addtoEnsemble (const QString &)),
	         myRadioInterface, SLOT (addtoEnsemble (const QString &)));
	connect (this, SIGNAL (nameofEnsemble (int, const QString &)),
	         myRadioInterface,
	                    SLOT (nameofEnsemble (int, const QString &)));
	connect (this, SIGNAL (setTime (const QString &)),
	         myRadioInterface, SLOT (showTime (const QString &)));
	connect (this, SIGNAL (changeinConfiguration (void)),
	         myRadioInterface, SLOT (changeinConfiguration (void)));
	currentBase	= new dataBase ();
	nextBase	= new dataBase ();
	ensemble	= new ensembleDescriptor ();
	CIFcount	= 0;
}
	
	fibDecoder::~fibDecoder (void) {
	delete	nextBase;
	delete	currentBase;
	delete	ensemble;
}

void	fibDecoder::newFrame (void) {
}

//
//	FIB's are segments of 256 bits. When here, we already
//	passed the crc and we start unpacking into FIGs
//	This is merely a dispatcher
void	fibDecoder::process_FIB (uint8_t *p, uint16_t fib) {
int8_t	processedBytes	= 0;
uint8_t	*d		= p;

	fibLocker. lock ();
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
	fibLocker. unlock ();
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
	      FIG0Extension4 (d);
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

	   case 9:		// country, LTO & international table (8.1.3.2)
	      FIG0Extension9 (d);
	      break;

	   case 10:		// date and time (8.1.3.1)
	      FIG0Extension10 (d);
	      break;

	   case 11:		// obsolete
	      break;

	   case 12:		// obsolete
	      break;

	   case 13:		// user application information (6.3.6)
	      FIG0Extension13 (d);
	      break;

	   case 14:		// FEC subchannel organization (6.2.2)
	      FIG0Extension14 (d);
	      break;

	   case 15:		// obsolete
	      break;

	   case 16:		// obsolete
	      break;

	   case 17:		// Program type (8.1.5)
	      FIG0Extension17 (d);
	      break;

	   case 18:		// announcement support (8.1.6.1)
	      FIG0Extension18 (d);
	      break;

	   case 19:		// announcement switching (8.1.6.2)
	      FIG0Extension19 (d);
	      break;

	   case 20:		// service component information (8.1.4)
	      FIG0Extension20 (d);
	      break;

	   case 21:		// frequency information (8.1.8)
	      FIG0Extension21 (d);
	      break;

	   case 22:		// obsolete
	      break;

	   case 23:		// obsolete
	      break;

	   case 24:		// OE services (8.1.10)
	      FIG0Extension24 (d);
	      break;

	   case 25:		// OE announcement support (8.1.6.3)
	      FIG0Extension25 (d);
	      break;

	   case 26:		// OE announcement switching (8.1.6.4)
	      FIG0Extension26 (d);
	      break;

	   case 27:
	   case 28:
	   case 29:		// undefined
	      break;

	   default:
//	      fprintf (stderr, "FIG0/%d passed by\n", extension);
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
uint8_t CN      = getBits_1 (d, 8 + 0);
uint8_t		alarmFlag;

	(void)CN;

	EId                     = getBits   (d, 16, 16);
	(void)EId;
	changeFlag              = getBits_2 (d, 16 + 16);
	alarmFlag		= getBits_1 (d, 16 + 16 + 2);
	highpart                = getBits_5 (d, 16 + 19) % 20;
	(void)highpart;
	lowpart                 = getBits_8 (d, 16 + 24) % 250;
	(void)lowpart;
	occurrenceChange        = getBits_8 (d, 16 + 32);
	(void)occurrenceChange;
	CIFcount 		= highpart * 250 + lowpart;

//	if (changeFlag == 3)
//	   fprintf (stderr, "MCI change in %d CIFs\n");
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
dataBase	*localBase = CN_bit == 0 ? currentBase : nextBase;

	(void)OE_bit;
	(void)PD_bit;

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
	      if (protLevel == 0)
	         subChannel. bitRate	= subChanSize / 12 * 8;
	      if (protLevel == 1)
	         subChannel. bitRate	= subChanSize / 8 * 8;
	      if (protLevel == 2)
	         subChannel. bitRate	= subChanSize / 6 * 8;
	      if (protLevel == 3)
	         subChannel. bitRate	= subChanSize / 4 * 8;
	   }
	   else			// option should be 001
	   if (option == 001) {		// B Level protection
	      protLevel			= getBits_2 (d, bitOffset + 20);
	      subChannel. protLevel	= protLevel + (1 << 2);
	      subChanSize		= getBits (d, bitOffset + 22, 10);
	      subChannel. Length	= subChanSize;
	      if (protLevel == 0)
	         subChannel. bitRate	= subChanSize / 27 * 32;
	      if (protLevel == 1)
	         subChannel. bitRate	= subChanSize / 21 * 32;
	      if (protLevel == 2)
	         subChannel. bitRate	= subChanSize / 18 * 32;
	      if (protLevel == 3)
	         subChannel. bitRate	= subChanSize / 15 * 32;
	   }
	   bitOffset += 32;
	}
//
//	in case the subchannel data was already computed
//	we merely computed the offset
	if (localBase -> subChannels [subChId]. inUse)
	   return bitOffset / 8;
//
//	and here we fill in the structure
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
//	bind channels to serviceIds
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
uint8_t		ecc;
uint8_t		cId;
uint32_t	SId;
int16_t		numberofComponents;

	(void)OE_bit;

	if (PD_bit == 1) {		// long Sid
	   ecc	= getBits_8 (d, bitOffset);	(void)ecc;
	   cId	= getBits_4 (d, bitOffset + 1);
	   SId	= getLBits  (d, bitOffset, 32);
	   bitOffset	+= 32;
	}
	else {
	   cId	= getBits_4 (d, bitOffset);	(void)cId;
	   SId	= getBits   (d, bitOffset, 16);
	   bitOffset	+= 16;
	}

	numberofComponents	= getBits_4 (d, bitOffset + 4);
	bitOffset	+= 8;

	for (i = 0; i < numberofComponents; i ++) {
	   uint8_t	TMid	= getBits_2 (d, bitOffset);
	   if (TMid == 00)  {	// Audio
	      uint8_t	ASCTy	= getBits_6 (d, bitOffset + 2);
	      uint8_t	SubChId	= getBits_6 (d, bitOffset + 8);
	      uint8_t	PS_flag	= getBits_1 (d, bitOffset + 14);
	      bind_audioService (CN_bit == 0 ? currentBase : nextBase,
	                            TMid, SId, i, SubChId, PS_flag, ASCTy);
	   }
	   else
	   if (TMid == 3) { // MSC packet data
	      int16_t SCId	= getBits   (d, bitOffset + 2, 12);
	      uint8_t PS_flag	= getBits_1 (d, bitOffset + 14);
	      uint8_t CA_flag	= getBits_1 (d, bitOffset + 15);
	      bind_packetService (CN_bit == 0 ? currentBase : nextBase,
	                            TMid, SId, i, SCId, PS_flag, CA_flag);
	   }
	   else
	      {;}
	   bitOffset += 16;
	}
	return bitOffset / 8;		// in Bytes
}
//
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
int16_t fibDecoder::HandleFIG0Extension3 (uint8_t *d,
	                                  int16_t used,
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
dataBase	*localBase;

	if (CN_bit == 0)
	   localBase	= currentBase;
	else
	   localBase	= nextBase;
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
	    localBase -> serviceComps [serviceCompIndex]. serviceIndex;
	QString serviceName =
	    ensemble -> services [serviceIndex]. serviceLabel;

	if (!ensemble -> services [serviceIndex]. is_shown)
	   addtoEnsemble (serviceName);

	ensemble -> services [serviceIndex]. is_shown			= true;

	localBase -> serviceComps [serviceCompIndex]. is_madePublic	= true;
	localBase -> serviceComps [serviceCompIndex]. SCId		= SCId;
	localBase -> serviceComps [serviceCompIndex]. subchannelId 	= SubChId;
	localBase -> serviceComps [serviceCompIndex]. DSCTy		= DSCTy;
	localBase -> serviceComps [serviceCompIndex]. DGflag		= DGflag;
	localBase -> serviceComps [serviceCompIndex]. packetAddress	= packetAddress;
	return used;
}

//	Service component with CA in stream mode 6.3.3
//	Not implemented (yet)
void	fibDecoder::FIG0Extension4 (uint8_t *d) {
	(void)d;
//	CN_bit determined current or next configuration
}

//	Service component language 8.1.2
void	fibDecoder::FIG0Extension5 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
int16_t	Length	= getBits_5 (d, 3);

	while (used < Length) {
	   used = HandleFIG0Extension5 (d, used);
	}
}

int16_t	fibDecoder::HandleFIG0Extension5 (uint8_t* d, int16_t offset) {
int16_t	bitOffset	= offset * 8;
uint8_t	lsFlag	= getBits_1 (d, bitOffset);
int16_t	subChId, serviceCompId, language;

	if (lsFlag == 0) {	// short form
	   if (getBits_1 (d, bitOffset + 1) == 0) {
	      subChId	= getBits_6 (d, bitOffset + 2);
	      language	= getBits_8 (d, bitOffset + 8);
	      currentBase -> subChannels [subChId]. language = language;
	   }
	   bitOffset += 16;
	}
	else {			// long form
	   serviceCompId	= getBits (d, bitOffset + 4, 12);
	   language		= getBits_8 (d, bitOffset + 16);
	   int serviceCompIndex = findServiceComponent (currentBase,
	                                                serviceCompId);
	   if (serviceCompIndex != -1)
	      currentBase -> serviceComps [serviceCompIndex]. language = language;
	
	   bitOffset += 24;
	}

	return bitOffset / 8;
}
//
// FIG0/6: Service linking information 8.1.15, not implemented
void	fibDecoder::FIG0Extension6 (uint8_t *d) {
	(void)d;
//	CN_bit determines the SIV
}
//
// FIG0/7: Configuration linking information 6.4.2, not implemented
//	does not seem to appear in the input so far
void	fibDecoder::FIG0Extension7 (uint8_t *d) {
	(void)d;
}

// FIG8/8:  Service Component Global Definition (6.3.5)
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

int16_t	fibDecoder::HandleFIG0Extension8 (uint8_t *d,
	                                  int16_t used,
	                                  uint8_t	CN_bit,
	                                  uint8_t	OE_bit,
	                                  uint8_t	PD_bit) {
int16_t	bitOffset	= used * 8;
//uint32_t	SId	= getLBits (d, bitOffset, PD_bit == 1 ? 32 : 16);
uint8_t		lsFlag;
uint16_t	SCIds;
//int16_t		Rfu;
uint8_t		extensionFlag;
dataBase	*localBase	= CN_bit == 0 ? currentBase : nextBase;

	bitOffset += PD_bit == 1 ? 32 : 16;
	extensionFlag   = getBits_1 (d, bitOffset);
	SCIds   = getBits_4 (d, bitOffset + 4);

	bitOffset += 8;
	lsFlag  = getBits_1 (d, bitOffset);
	if (lsFlag == 1) {
	   int SCId = getBits (d, bitOffset + 4, 12);
	   bitOffset += 16;
	   int serviceCompIndex = findServiceComponent (localBase, SCId);
	   if (serviceCompIndex != -1) {
	      localBase -> serviceComps [serviceCompIndex]. SCIds = SCIds;
	   }
	}
	else {
	   int subChId	= getBits_6 (d, bitOffset + 2);
	   if  (localBase -> subChannels [subChId]. inUse)
	      localBase -> subChannels [subChId]. SCIds = SCIds;
	   bitOffset += 8;
	}

	if (extensionFlag)
	   bitOffset += 8;	// skip Rfa
	return bitOffset / 8;
}
//
//	Country, LTO & international table 8.1.3.2
void fibDecoder::FIG0Extension9 (uint8_t *d) {
int16_t	offset	= 16;
uint8_t ecc;

	dateTime [6] = (getBits_1 (d, offset + 2) == 1)?
	                -1 * getBits_4 (d, offset + 3):
	                     getBits_4 (d, offset + 3);
	dateTime [7] = (getBits_1 (d, offset + 7) == 1) ? 30 : 0;

	ecc	     = getBits (d, offset + 8, 8);
	if (!ensemble -> ecc_Present) {
	   ensemble -> ecc_byte = ecc;
	   ensemble -> ecc_Present = true;
	}
}

QString monthTable [] = {
"jan", "feb", "mar", "apr", "may", "jun",
"jul", "aug", "sep", "oct", "nov", "dec"};

QString	mapTime (int32_t *dateTime) {
QString result	= QString::number (dateTime [0]);
	result. append ("-");
	result. append (monthTable [dateTime [1] - 1]);
	result. append ("-");
	QString day	= QString ("%1"). arg (dateTime [2], 2, 10, QChar ('0'));
	result. append (day);
	result. append (":");
	int hours	= dateTime [3] + dateTime [6];
	if (hours < 0)
	   hours += 24;
	if (hours >= 24)
	   hours -= 24;
	QString hoursasString 
		= QString ("%1"). arg (hours, 2, 10, QChar ('0'));
	result. append (hoursasString);
	result. append (":");
	QString minutesasString =
	              QString ("%1"). arg (dateTime [4], 2, 10, QChar ('0'));
	result. append (minutesasString);
	return result;
}

//
//	Date and Time
//	FIG0/10 are copied from the work of
//	Michael Hoehn
void fibDecoder::FIG0Extension10 (uint8_t *dd) {
int16_t		offset = 16;
int32_t		mjd	= getLBits (dd, offset + 1, 17);
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
	theTime [3] = getBits_5 (dd, offset + 21);	// Hours
	if (getBits_6 (dd, offset + 26) != dateTime [4]) 
	   theTime [5] =  0;	// Seconds (Uebergang abfangen)

	theTime [4] = getBits_6 (dd, offset + 26);	// Minutes
	if (dd [offset + 20] == 1)
	   theTime [5] = getBits_6 (dd, offset + 32);	// Seconds

	bool	change = false;
	for (int i = 0; i < 5; i ++) {
	   if (theTime [i] != dateTime [i])
	      change = true;
	   dateTime [i] = theTime [i];
	}

	if (change) {
	   const QString timeString = mapTime (dateTime);
	   emit  setTime (timeString);
	}
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

int16_t	fibDecoder::HandleFIG0Extension13 (uint8_t *d,
	                                   int16_t used,
	                                   uint8_t CN_bit,
	                                   uint8_t OE_bit,
	                                   uint8_t pdBit) {
int16_t	bitOffset		= used * 8;
uint32_t	SId	= getLBits (d, bitOffset, pdBit == 1 ? 32 : 16);
uint16_t	SCIds;
int16_t		NoApplications;
int16_t		i;
int16_t		appType;
dataBase	*localBase	= CN_bit == 0 ? currentBase : nextBase;

	bitOffset		+= pdBit == 1 ? 32 : 16;
	SCIds		= getBits_4 (d, bitOffset);
	NoApplications	= getBits_4 (d, bitOffset + 4);
	bitOffset += 8;

	for (i = 0; i < NoApplications; i ++) {
	   appType		= getBits (d, bitOffset, 11);
	   int16_t length	= getBits_5 (d, bitOffset + 11);
	   bitOffset 		+= (11 + 5 + 8 * length);
;
	   int serviceCompIndex =
	               findServiceComponentinService (localBase, SId, SCIds);
	   if (serviceCompIndex != -1) 
	        localBase -> serviceComps [serviceCompIndex]. appType = appType;
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
dataBase	*localBase	= CN_bit == 0 ? currentBase : nextBase;

	while (used < Length) {
	   int16_t subChId	= getBits_6 (d, used * 8);
	   uint8_t FEC_scheme	= getBits_2 (d, used * 8 + 6);
	   used = used + 1;
	   if (localBase -> subChannels [subChId]. inUse)
	      localBase -> subChannels [subChId]. FEC_scheme = FEC_scheme;
	}
}
//
//	Programme Type (PTy) 8.1.5
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
	   serviceIndex	= setServiceIndex (SId);
	   if (L_flag) {		// language field present
	      Language = getBits_8 (d, offset + 24);
	      ensemble -> services [serviceIndex]. language = Language;
	      offset += 8;
	   }

	   type	= getBits_5 (d, offset + 27);
	   ensemble -> services [serviceIndex]. programType	= type;
	   if (CC_flag)			// cc flag
	      offset += 40;
	   else
	      offset += 32;
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

	while (bitOffset < Length * 8) {
	   uint16_t SId	= getBits (d, bitOffset, 16);
	   bitOffset	+= 16;
	   uint16_t asuFlags	= getBits (d, bitOffset, 16);
	   bitOffset	+= 16;
	   uint8_t Rfa		= getBits (d, bitOffset,  5);
	   uint8_t nrClusters	= getBits (d, bitOffset + 5, 3);
	   bitOffset	+= 8;
	   bitOffset	+= nrClusters * 8;
	   int16_t serviceIndex = findServiceIndex (SId);
	   if (serviceIndex != -1) {
	      if (ensemble -> services [serviceIndex]. hasName) {
//	         fprintf (stderr, "announcement for %s (%x)\n",
//	             ensemble -> services [serviceIndex]. serviceLabel. toLatin1 (). data (), asuFlags);
	      }
	   }
	}
}
//
//	Announcement switching 8.1.6.2
void	fibDecoder::FIG0Extension19 (uint8_t *d) {	
	(void)d;
}

//	Service component information 8.1.4
void	fibDecoder::FIG0Extension20 (uint8_t *d) {
//	fprintf (stderr, "fig0/20\n");
	(void)d;
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
	   used = HandleFIG0Extension21 (d, used);
}

int16_t	fibDecoder::HandleFIG0Extension21 (uint8_t* d,
	                                   int16_t offset) {
int16_t	l_offset	= offset * 8;
int16_t	l	= getBits_5 (d, l_offset + 11);
int16_t		upperLimit	= l_offset + 16 + l * 8;
int16_t		base		= l_offset + 16;

	while (base < upperLimit) {
	   uint16_t idField	= getBits (d, base, 16);
	   uint8_t  RandM	= getBits_4 (d, base + 16);
	   uint8_t  continuity	= getBits_1 (d, base + 20);
	   uint8_t  length	= getBits_3 (d, base + 21);
	   if (RandM == 0x08) {
	      uint16_t fmFrequency_key	= getBits (d, base + 24, 8);
	      int32_t  fmFrequency	= 87500 + fmFrequency_key * 100;
	      int16_t serviceIndex	= findServiceIndex (idField);
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

//	OE Services
void	fibDecoder::FIG0Extension24 (uint8_t *d) {
	(void)d;
}
//
//	OE Announcement support
void	fibDecoder::FIG0Extension25 (uint8_t *d) {
	(void)d;
}
//
//	OE Announcement Switching
void	fibDecoder::FIG0Extension26 (uint8_t *d) {
	(void)d;
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
	serviceIndex	= setServiceIndex (SId);
	if ((!ensemble -> services [serviceIndex]. hasName) &&
	                                               (charSet <= 16)) {
	   for (i = 0; i < 16; i ++) {
	      label [i] = getBits_8 (d, offset + 8 * i);
	   }
	   ensemble -> services [serviceIndex]. serviceLabel =
	                                  toQStringUsingCharset (
	                                  (const char *) label,
	                                  (CharacterSet) charSet);
	   ensemble -> services [serviceIndex]. hasName = true;
	}
}

// service component label 8.1.14.3
void	fibDecoder::FIG1Extension4 (uint8_t *d) {
uint8_t		PD_bit;
uint8_t		SCId;
uint8_t		Rfu;
uint32_t	SId;
int16_t		offset;
//uint8_t		flagfield;

	PD_bit	= getBits_1 (d, 16);
	Rfu	= getBits_3 (d, 17);	(void)Rfu;
	SCId	= getBits_4 (d, 20);

	if (PD_bit) {	// 32 bit identifier field for data components
	   SId		= getLBits (d, 24, 32);
	   offset	= 56;
	}
	else {	// 16 bit identifier field for program components
	   SId		= getLBits (d, 24, 16);
	   offset	= 40;
	}
	(void)SId; (void)SCId;
	(void)offset;
//	bindServiceComponenttoService (SId, SCId, PD_bit);
}

//	Data service label - 32 bits 8.1.14.2
void	fibDecoder::FIG1Extension5 (uint8_t *d) {
uint8_t		charSet, extension;
uint8_t		Rfu;
//uint32_t	EId	= 0;
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

	serviceIndex   = setServiceIndex (SId);
	if ((!ensemble -> services [serviceIndex]. hasName) &&
	                                          (charSet <= 16)) {
	   for (i = 0; i < 16; i ++) {
	      label [i] = getBits_8 (d, offset + 8 * i);
	   }

	   ensemble -> services [serviceIndex]. serviceLabel =
	                                       toQStringUsingCharset (
	                                       (const char *) label,
	                                       (CharacterSet) charSet);
	   ensemble -> services [serviceIndex]. hasName = true;
	}
}

//	XPAD label - 8.1.14.4
void	fibDecoder::FIG1Extension6 (uint8_t *d) {
uint32_t	SId	= 0;
uint8_t		Rfu;
int16_t		offset	= 0;
uint8_t		PD_bit;
uint8_t		SCId;
uint8_t		XPAD_apptype;

	PD_bit	= getBits_1 (d, 16);
	Rfu	= getBits_3 (d, 17);	(void)Rfu;
	SCId	= getBits_4 (d, 20);
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

	(void)SId; (void)SCId; (void)XPAD_apptype; 
	(void)offset;
//	bind_xpadComponent (SId, SCId, XPAD_apptype);
}

//	bind_audioService is the main processor for - what the name suggests -
//	connecting the description of audioservices to a SID
//	by creating a service Component
void	fibDecoder::bind_audioService (dataBase *base,
	                               int8_t	TMid,
	                               uint32_t	SId,
	                               int16_t	compnr,
	                               int16_t	subChId,
	                               int16_t	ps_flag,
	                               int16_t	ASCTy) {
int serviceIndex	= setServiceIndex	(SId);
int16_t	i;
int16_t	firstFree	= -1;

	if (!ensemble -> services [serviceIndex]. hasName)
	   return;

	if (!base -> subChannels [subChId]. inUse)
	   return;

	for (i = 0; i < 64; i ++) {
	   if (!base -> serviceComps [i]. inUse) {
	      if (firstFree == -1)
	         firstFree = i;
	      break;
	   }

	   if ((base -> serviceComps [i]. serviceIndex == serviceIndex) &&
	       (base -> serviceComps [i]. componentNr == compnr))
	      return;
	}

	QString dataName = ensemble -> services [serviceIndex]. serviceLabel;
	if (!ensemble -> services [serviceIndex]. is_shown)
	   addtoEnsemble (dataName);

	ensemble -> services [serviceIndex]. is_shown	= true;

	base -> serviceComps [firstFree]. inUse		= true;
	base -> serviceComps [firstFree]. TMid		= TMid;
	base -> serviceComps [firstFree]. componentNr	= compnr;
	base -> serviceComps [firstFree]. serviceIndex	= serviceIndex;
	base -> serviceComps [firstFree]. subchannelId	= subChId;
	base -> serviceComps [firstFree]. PS_flag	= ps_flag;
	base -> serviceComps [firstFree]. ASCTy		= ASCTy;
}

//      bind_packetService is the main processor for - what the name suggests -
//      connecting the service component defining the service to the SId,
//	So, here we create a service component. Note however,
//	that FIG0/3 provides additional data, after that we
//	decide whether it should be visible or not
void    fibDecoder::bind_packetService (dataBase *base,
	                                int8_t TMid,
	                                uint32_t SId,
	                                int16_t compnr,
	                                int16_t SCId,
	                                int16_t ps_flag,
	                                int16_t CAflag) {
int serviceIndex    = setServiceIndex (SId);
int16_t i;
QString name;

	if (!ensemble -> services [serviceIndex]. hasName)	// wait until we have a name
	   return;

	for (i = 0; i < 64; i ++) {
	   if (base -> serviceComps [i]. inUse)
	      if (base -> serviceComps [i]. SCId == SCId)
	         return;
	   if (!base -> serviceComps [i]. inUse) {
	      base -> serviceComps [i]. inUse		= true;
	      base -> serviceComps [i]. TMid		= TMid;
	      base -> serviceComps [i]. serviceIndex	= serviceIndex;
	      base -> serviceComps [i]. componentNr	= compnr;
	      base -> serviceComps [i]. SCId		= SCId;
	      base -> serviceComps [i]. PS_flag		= ps_flag;
	      base -> serviceComps [i]. CAflag		= CAflag;
	      base -> serviceComps [i]. is_madePublic	= false;
	      return;
	   }
	}
}

int	fibDecoder::setServiceIndex	(uint32_t SId) {
int	i;
	for (i = 0; i < 64; i ++) {
	   if (!ensemble -> services [i]. inUse) {
	      ensemble -> services [i]. inUse = true;
	      ensemble -> services [i]. serviceId = SId;
	      return i;
	   }
	   if (ensemble -> services [i]. serviceId == SId) {
	      return i;
	   }
	}
}

int	fibDecoder::findServiceIndex	(uint32_t SId) {
int	i;
	for (i = 0; i < 64; i ++) {
	   if (!ensemble -> services [i]. inUse)
	      return -1;
	   if (ensemble -> services [i]. serviceId == SId)
	      return i;
	}
	return -1;
}

int	fibDecoder::findServiceComponent (dataBase *db, int16_t SCId) {
int	i;
	for (i = 0; i < 64; i ++) {
	   if (!db -> serviceComps [i]. inUse)
	      return -1;
	   if (db -> serviceComps [i]. SCId == SCId)
	      return i;
	}
	return -1;
}

int	fibDecoder::findServiceComponentinService (dataBase *db,
	                                           uint32_t SId,
	                                           int16_t SCIds) {
int	i;
	for (i = 0; i < 64; i ++) {
	   if (!db -> serviceComps [i]. inUse)
	      return -1;
	   if (ensemble -> services [db -> serviceComps [i]. serviceIndex]. serviceId == SId) 
	      if (db -> serviceComps [i]. SCIds == SCIds)
	         return i;
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//	
//	Implementation of API functions
//
void	fibDecoder::clearEnsemble	(void) {
	fibLocker. lock ();
	delete currentBase;
	currentBase	= new dataBase ();
	delete nextBase;
	nextBase	= new dataBase ();
	delete ensemble;
	ensemble	= new ensembleDescriptor ();
	fibLocker. unlock ();
}

bool	fibDecoder::syncReached		(void) {
	return  ensemble -> isSynced;
}

static inline
bool	match (QString s1, QString s2) {
	return s1 == s2;
}
//
//	The array with services is filled from 0 .. ??, so
//	once we have a "not in use", we are done
int	fibDecoder::findService		(const QString &s) {
int	i;
	for (i = 0; i < 64; i ++) {
	   if (!ensemble -> services [i]. inUse)
	      return -1;
	   if (match (s, ensemble -> services [i]. serviceLabel))
	      return i;
	}
	return -1;
}

void	fibDecoder::dataforAudioService	(const QString &s,
	                                 audiodata *ad, int16_t compnr) {
int	j;
int	serviceIndex;

	ad       -> defined      = false;
	fibLocker. lock ();
	
	serviceIndex	= findService (s);
	if (serviceIndex == -1) {
	   fibLocker. unlock ();
	   return;
	}

	for (j = 0; j < 64; j ++) {
	   int16_t subChId;
	   
	   if (!currentBase -> serviceComps [j]. inUse)
	      break;

	   if (currentBase -> serviceComps [j]. TMid != 0)
	      continue;

	   if (serviceIndex != currentBase -> serviceComps [j]. serviceIndex)
	      continue;

	   subChId	= currentBase -> serviceComps [j]. subchannelId;

	   if (currentBase -> subChannels [subChId]. SCIds != compnr)
	      continue;
	                                     
	   ad	-> serviceId    =
	                 ensemble -> services [serviceIndex]. serviceId;
	   ad	-> serviceName  = s;
	   ad	-> language     = 
	                 ensemble -> services [serviceIndex]. language;
	   ad	-> programType	=
	                 ensemble -> services [serviceIndex]. programType;
	   ad	-> fmFrequency	=
	                 ensemble -> services [serviceIndex]. fmFrequency;
	   ad	-> subchId      = subChId;
	   ad	-> startAddr    =
	                 currentBase -> subChannels [subChId]. startAddr;
	   ad	-> shortForm    =
	                 currentBase -> subChannels [subChId]. shortForm;
	   ad	-> protLevel    =
	                 currentBase -> subChannels [subChId]. protLevel;
	   ad	-> length       =
	                 currentBase -> subChannels [subChId]. Length;
	   ad	-> bitRate      =
	                 currentBase -> subChannels [subChId]. bitRate;
	   ad	-> ASCTy        = 
	                 currentBase -> serviceComps [j]. ASCTy;
	   ad	-> defined	= true;
	   break;
	}
	fibLocker. unlock ();
}

void	fibDecoder::dataforPacketService (const QString &s,
	                                  packetdata *pd, int16_t compnr) {
int     j;
int     serviceIndex;

	pd       -> defined      = false;
	fibLocker. lock ();

	serviceIndex    = findService (s);
	if (serviceIndex == -1) {
	   fibLocker. unlock ();
	   return;
	}

	for (j = 0; j < 64; j ++) {
	   int16_t subchId;
	   if (!currentBase -> serviceComps [j]. inUse)
	      break;

	   if (currentBase -> serviceComps [j]. TMid != 03)
	      continue;

	   if (serviceIndex != currentBase -> serviceComps [j]. serviceIndex)
	      continue;

//	   if (currentBase -> serviceComps [j]. componentNr != compnr)
	   if (currentBase -> serviceComps [j]. SCIds != compnr)
	      continue;

	   pd	-> serviceId	=
	                 ensemble -> services [serviceIndex]. serviceId;
	   subchId              = currentBase -> serviceComps [j]. subchannelId;
	   pd	-> subchId      = subchId;
	   pd	-> startAddr    =
	                     currentBase -> subChannels [subchId]. startAddr;
	   pd	-> shortForm    = 
	                     currentBase -> subChannels [subchId]. shortForm;
	   pd	-> protLevel    = 
	                     currentBase -> subChannels [subchId]. protLevel;
	   pd	-> length       = 
	                     currentBase -> subChannels [subchId]. Length;
	   pd	-> bitRate      = 
	                     currentBase -> subChannels [subchId]. bitRate;
	   pd	-> FEC_scheme   = 
	                     currentBase -> subChannels [subchId]. FEC_scheme;
	   pd	-> DSCTy        = 
	                     currentBase -> serviceComps [j]. DSCTy;
 	   pd	-> DGflag       = 
	                     currentBase -> serviceComps [j]. DGflag;
	   pd	-> packetAddress = 
	                      currentBase -> serviceComps [j]. packetAddress;
	   pd	-> compnr       = 
	                      currentBase -> serviceComps [j]. componentNr;
	   pd	-> appType      = 
	                      currentBase -> serviceComps [j]. appType;
	   pd	-> defined      = true;
	   break;
	}

	fibLocker. unlock ();
}

int32_t	fibDecoder::get_ensembleId	(void) {
	if (ensemble -> namePresent)
	   return ensemble -> ensembleId;
	else
	   return 0;
}

QString	fibDecoder::get_ensembleName	(void) {
	if (ensemble -> namePresent)
	   return ensemble -> ensembleName;
	else
	   return " ";
}

int32_t fibDecoder::get_CIFcount	(void) {
	return CIFcount;
}

uint8_t	fibDecoder::get_ecc (void) {
	if (ensemble -> ecc_Present)
	   return ensemble -> ecc_byte;
	return 0;
}

