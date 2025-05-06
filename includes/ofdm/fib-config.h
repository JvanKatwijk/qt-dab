#
/*
 *    Copyright (C)  2015 .. 2025
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB 
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

#pragma once
//
//	Implementation of the FIG database
#include	<stdint.h>
#include	<vector>

class	fibConfig {
public:
			fibConfig	();
			~fibConfig	();
//	The tables cannot made private (unless set functions for
//	all structs and components are made)
//	Most components are a direct translation of the
//	FIG 
//	SId is a small optimization, not really needed
//	of service components
	typedef struct {
	   uint32_t 	SId;
	   std::vector<int> comps;
	   uint16_t	announcing;
	} SId_struct;
//
//	Subchannel is filled by FIG0/1
	typedef struct  {
	   bool		inUse;		// will be removed after handling eti
	   uint16_t	subChId;
	   int32_t	Length;
	   int32_t	startAddr;
	   bool		shortForm;
	   int32_t	protLevel;
	   int32_t	bitRate;
	   int16_t	FEC_scheme;
	} subChannel;

//	service components	filled by FIG0/2
//	The "C" stands for Common
	typedef struct {
	   uint32_t	SId;
	   uint8_t	SCId;
	   uint8_t	subChId;
	   uint8_t	TMid;
	   int		compNr;
	   uint8_t	ASCTy;
	   uint8_t	PS_flag;
	} serviceComp_C;
//
//	additional component information for packet data,
//	the "P" stands for Packet, FIG0/3
	typedef struct {
	   uint16_t	SCId;
	   uint8_t	CAOrg_flag;
	   uint8_t	DG_flag;
	   uint8_t	DSCTy;
	   uint8_t	subChId;
	   uint16_t	packetAddress;
	} serviceComp_P;
//
//	some more general component information 
//	the "G" stands for General, FIG0/8
	typedef	struct {
	   uint32_t	SId;
	   uint8_t	LS_flag;
	   uint8_t	SCIds;
	   uint8_t	subChId;
	   uint16_t	SCId;
	} serviceComp_G;
//
//	Data for the service components Language
//	FIG0/5
	typedef struct {
	   uint8_t	LS_flag;
	   uint8_t	subChId;
	   uint16_t	SCId;
	   uint8_t	language;
	} SC_language;

//
//	Data for the apptype of packet components FIG0/13
	typedef	struct {
	   uint32_t	SId;
	   uint8_t	SCIds;
	   uint16_t	Apptype;
	} AppType;
//
//	Data for the program type  FIG0/17
	typedef struct {
	   uint32_t	SId;
	   uint8_t	typeCode;
	} programType;

//
//	data for the announcement support FIG0/18
	typedef struct {
	   uint16_t	SId;
	   uint16_t	asuFlags;
	   uint8_t	clusterId;
	} FIG18_cluster;
//
//	for each type a table
	std::vector<SId_struct>	SId_table;
	std::vector<subChannel> subChannel_table;	// FIG0/1
	std::vector<serviceComp_C> SC_C_table;		// FIG0/2
	std::vector<serviceComp_P> SC_P_table;		// FIG0/3
	std::vector<serviceComp_G> SC_G_table;		// FIG0/8
	std::vector<SC_language> language_table;		// FIG0/5
	std::vector<AppType>	AppType_table;		// FIG0/13
	std::vector<programType> programType_table;	// FIG017
	std::vector<FIG18_cluster> announcement_table; // FIG0/18

	int32_t dateTime [8];
	void	reset			();
	int	serviceIdOf		(int index);
	int	SCIdsOf			(int index);
	int	subChannelOf		(int index);
	int	startAddressOf		(int index);
	int	lengthOf 		(int index);
	bool	shortFormOf		(int index);
	int16_t	protLevelOf		(int index);
	int	bitRateOf 		(int index);
	int	dabTypeOf		(int index);
	int	languageOf 		(int index);
	int	appTypeOf		(int index);
	int	FEC_schemeOf		(int index);
	int	packetAddressOf		(int index);
	int	DSCTy			(int index);
	int	DG_flag			(int index);
	int	findIndex_SC_P_Table	(uint16_t SCId);
	int	findIndex_subChannel_table (uint8_t subChId);
	int	findIndex_languageTable	(uint8_t key_1, uint16_t key_2);
	int	findIndexApptype_table	(uint32_t SId, uint8_t SCIds);
	bool	compIsKnown		(serviceComp_C &newComp);

	int	freeSpace		();
};

