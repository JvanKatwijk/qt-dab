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
#include	<QString>
#include	"dab-constants.h"
class	ensemble;

class	fibConfig {
public:
			fibConfig	(ensemble *);
			~fibConfig	();
//	The tables cannot made private (unless set functions for
//	all structs and components are made)
//	Most components are a direct translation of the
//	FIG 
//	SId is a small optimization, not really needed
//	of service components
	ensemble	*theEnsemble;
	uint32_t	getSId			(const int);
	bool		SId_exists		(const int);
	bool		SCId_exists		(const int);
	bool		subChId_exists		(const int);
	bool		SC_G_element_exists	(const int SId,
	                                             const int SCIds);
	bool		announcement_exists	(const int, const int);

	uint8_t		serviceType		(const int);
	int		getNrComps		(const uint32_t);
	int		getServiceComp		(const QString &);
	int		getServiceComp		(const uint32_t, const int);
	int		getServiceComp_SCIds	(const uint32_t, const int);

	void		audioData		(const int, audiodata &);
	void		packetData		(const int, packetdata &);

	uint16_t	getAnnouncing		(uint16_t);

	int		nrChannels		();
	void		getChannelInfo		(channel_data *, const int);
	QList<contentType>
	                contentPrint		();

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
//
//	for each type a table
	std::vector<SId_struct>		SId_table;		// FIG0/2
	std::vector<subChannel>		subChannel_table;	// FIG0/1
	std::vector<serviceComp_C>	SC_C_table;		// FIG0/2
	std::vector<serviceComp_P>	SC_P_table;		// FIG0/3
	std::vector<serviceComp_G>	SC_G_table;		// FIG0/8
	std::vector<SC_language>	language_table;		// FIG0/5
	std::vector<AppType>		AppType_table;		// FIG0/13
	std::vector<programType> 	programType_table;	// FIG0/17
	std::vector<FIG18_cluster>	announcement_table;	// FIG0/18

	int32_t dateTime [8];
	void	reset			();
	int	subChannelOf		(int index);
	int	findIndex_subChannel_table (uint8_t subChId);
	int	findIndexApptype_table	(uint32_t SId, uint8_t SCIds);
	bool	compIsKnown		(serviceComp_C &newComp);
	int	freeSpace		();
private:
	int	serviceIdOf		(int index);
	int	SCIdsOf			(int index);
	int	dabTypeOf		(int index);
	int	languageOf 		(int index);
	int	appTypeOf		(int index);
	int	FEC_schemeOf		(int index);
	int	packetAddressOf		(int index);
	int	DSCTy			(int index);
	int	DG_flag			(int index);
	int	findIndex_SC_P_Table	(uint16_t SCId);
//	int	findIndex_languageTable	(uint8_t key_1, uint16_t key_2);

};

