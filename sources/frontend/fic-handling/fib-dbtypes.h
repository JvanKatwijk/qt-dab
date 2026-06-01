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
 *	fib-db types. In this implementation we have chosen to
 *	use a separate entry (stack or value) for each type of FIG data
 */
#pragma once

#include	<stdint.h>
#include	<QString>


//	FIG0/0	is defined 6.4.1
typedef struct {
	uint16_t	EId;
	uint8_t		changeFlag;
	uint8_t		prevChangeFlag;
	uint8_t		alarmFlag;
	uint16_t	CIF_major;
	uint16_t	CIF_minor;
	uint8_t		occurrenceChange;
} FIG00;

//	FIG0/1	is defined in 6.2.1
typedef struct {
	uint16_t	subChId;
	uint16_t	startAddr;
	uint8_t		shortForm;
	uint16_t	Length;
	uint8_t		protLevel;
	uint8_t		bitRate;
} FIG01;
//
//	while not in FIG0/2, we added the result of FIG0/8
//	i.e. the SCIds, to this struct
typedef struct {
	uint8_t		compNr;
	uint8_t		SCIds;		// later added
	uint8_t		TMid;
	uint8_t		ASCTy;
	uint8_t		subChId;
	uint8_t		PS_flag;
	uint16_t	SCId;
} FIG02_comp;

//	FIG02	is defined in 6.3.1
typedef struct {
	uint32_t	SId;
	uint8_t		PD_bit;
	uint8_t		nrComps;
	std::vector<FIG02_comp> components;
} FIG02;

//	fIG0/3 is defined in 6.3.2
typedef	struct {
	uint16_t	SCId;
	uint8_t		DG_flag;
	uint8_t		DSCTy;
	uint8_t		subChId;
	uint16_t	packetAddress;
} FIG03;

//	FIG0/4 is defined in 6.3.3
//	FIG0/4 is not implemented

//	FIG0/5	is defined in 8.1.2
//	Bij de processing wordt de subchannel gevonden
typedef	struct {
	uint16_t	subChId;
	uint8_t		language;
} FIG05;

//	FIG0/6 is defined in 8.1.15
//	FIG0/6	is not implementd (yet)

//	FIG0/7 is implemented in 6.4.2
typedef	struct {
	uint8_t		servicesField;
	uint8_t		countField;
} FIG07;

//	FIG0/8 is defined in 6.3.5
typedef struct {
	uint32_t	SId;
	uint16_t	SCIds;
	uint8_t		LS_flag;
	uint8_t		subChId;
	uint16_t	SCId;
} FIG08;

//	FIG0/9 is defined in 8.1.3.2
//	if extFlag is set, the additional ECC info is distributed
//	to FIG02 items
struct local_ecc {
	uint32_t	SId;
	uint8_t		service_ecc;
};

typedef	struct {
	uint8_t		extFlag;
	uint8_t		LTO;
	uint8_t		ECC;
	uint8_t		tableId;
	std::vector<struct local_ecc> ecc_local;
} FIG09;

//	FIG0/10 is defined in 8.1.3.1
typedef struct {
	uint16_t	MJD;
	uint8_t		LSI;
	uint8_t		UTC_flg;
	uint8_t		hours;
	uint8_t		minuts;
	uint8_t		seconds;
	uint16_t	milliseconds;
} FIG010;
//
//	FIG0/11	is obsolete

//	FIG012 is obsolete

//	FIG0/13 is defined in 6.3.6
typedef struct {
	uint32_t	SId;
	uint8_t		SCIds;
	uint16_t	Apptype;
} FIG013;

//	FIG0/14 is defined in 6.2.2
typedef struct {
	uint8_t 	subChId;
	uint8_t		FEC_scheme;
} FIG014;

//	FIG0/15 is being defined, but not yet implemented

//	FIG0/16 is not defined

//	FIG0/17	is defined in 8.1.5, 
typedef	struct {
	uint16_t	SId;
	uint8_t		SD_flag;
	uint8_t		typecode;
} FIG017;

//	FIG0/18 is defined in 8.1.6.1
typedef	struct {
	uint16_t	SId;
	uint16_t	ASUFlags;	
	std::vector<uint8_t> clusterIds;
} FIG018;

//	Not used as struct
//	FIG0/19 is defined in 8.1.6.2
typedef struct {
	uint8_t		clusterId;
	uint16_t	ASW_flags;
	uint8_t		newFlag;
	uint8_t		subChId;
} FIG019;

//	FIG0/20 is defined in 8.1.4
//	FIG20 not yet implemented

//	FIG0/21 is defined in 8.1.8
typedef	struct {
	uint16_t	SId;
	std::vector<uint32_t> freqList;
} FIG021;

//	FIG0/22 is obsolete
//	FIG0/23 is obsolete
//	FIG0/24 is defined in 8.1.10
//	FIG0/24	is not implemented

//	FIG0/25 is defined in 8.1.6.3
//	FIG0/25 is not implemented

//	FIG0/26 is defined in 8.1.6.4
//	FIG0/26 is not implemented


//	FIG1/0	is defined in 8.1.13 
//	Values are signalled
typedef	struct {
	QString		ensembleLabel;
	uint16_t	EId;
	uint8_t		eccByte;
	uint8_t		lto;
	uint8_t		internatTable;
	bool		isSynced;
} FIG10;

//	FIG1/1 is defined in 8.1.14.1
typedef	struct {
	QString serviceName;
	QString shortName;
	uint16_t	SId;
} FIG11;

//	FIG1/4 is defined in 8.1.14.3
typedef	struct {
	uint32_t	SId;
	uint8_t		SCIds;
	QString		serviceName;
	QString		shortName;
} FIG14;

//	FIG1/5 is defined in 8.1.14.4
typedef struct {
	QString serviceName;
	QString shortName;
	uint32_t	SId;
} FIG15;

typedef	struct {
	uint8_t		PD_flag;
	uint8_t		SCIds;
	uint32_t	SId;
	uint8_t		XPadAppType;
} FIG16;


