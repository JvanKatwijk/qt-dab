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
//	In this version the fib-decoder is (logically) abstracted from
//	the fib database
//
//	The first part of the fib-decoder interprets the figs, builds the
//	db elements and passes them on to the db,
//	The second part of the fib decoder - called from the users -
//	extracts the data. Due to the abstraction brought in,
//	a decent locking is now possible.
//	The database elements need to be visible from the decoder.

	typedef struct {
	   uint32_t 	SId;
	   uint16_t	announcing;
	   std::vector<int> comps;
	} SId_struct;

//	Subchannel is filled by FIG0/1
	typedef struct  {
	   bool		inUse;		// needed for handling eti
	   uint16_t	subChId;
	   int32_t	Length;
	   int32_t	startAddr;
	   bool		shortForm;
	   int32_t	protLevel;
	   int32_t	bitRate;
	   int16_t	FEC_scheme;
	   uint8_t	compLanguage;
	} subChannel;

//	service components	filled by FIG0/2
//	The "C" stands for Common
	typedef struct {
	   uint32_t	SId;
	   uint16_t	SCId;
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
