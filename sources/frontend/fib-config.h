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

#include	<QObject>
#include	<stdint.h>
#include	<vector>
#include	<QString>
#include	"dab-constants.h"

class	ensemble;
class	RadioInterface;

class	fibConfig: public QObject {
Q_OBJECT
public:
			fibConfig	(ensemble *, RadioInterface *);
			~fibConfig	();
//
//	The db types are included
//	Most components are a direct translation of the
//	FIG 
#include	"fib-dbtypes.h"
//	SId is a small optimization, not really needed
//	of service components
	void		reset			();
	int		freeSpace		();
	uint32_t	getSId			(const int);
	bool		SId_exists		(const int);
	bool		SCId_exists		(const int);
	bool		subChId_exists		(const int);

	bool		SC_G_element_exists	(const int SId,
	                                             const int SCIds);
	bool		announcement_exists	(const int, const int);
	bool		language_comp_exists	(int);
	uint8_t		serviceType		(const int);
	int		getNrComps		(const uint32_t);
	int		getServiceComp		(const QString &);
	int		getServiceComp		(const uint32_t, const int);
	int		getServiceComp_SCIds	(const uint32_t, const int);

	int		subChId_for_SId		(const int, const uint32_t);
	int		subChId_in_SCId		(const int);
	void		audioData		(const int, audiodata &);
	void		packetData		(const int, packetdata &);

	uint16_t	getAnnouncing		(uint16_t);

	int		nrChannels		();
	void		getChannelInfo		(channel_data *, const int);
	QList<contentType>
	                contentPrint		();

	void		set_FECscheme		(const int, int);

	void		check_announcements	(uint8_t, uint8_t, uint8_t);

	ensemble	*theEnsemble;
	
	void		add_to_SId_table	(const SId_struct &comp);
	void		add_to_subChannel_table (const subChannel &comp);
//
//	This one returns the index
//	many operations are based on finding the component,
//	that is why the index is passed as result
	int		add_to_SC_C_table	(const serviceComp_C &comp);
	void		add_to_SC_P_table	(const serviceComp_P &comp);
	void		add_to_SC_G_table 	(const serviceComp_G &comp);
	void		add_to_language_table 	(const SC_language &comp);
	void		add_to_apptype_table 	(const AppType &comp);
	void		add_to_announcement_table (const FIG18_cluster &comp);

	int32_t dateTime [8];
	int		subChannelOf		(int index);
	int		findIndex_subChannel_table (uint8_t subChId);
	int		findIndexApptype_table	(uint32_t SId, uint8_t SCIds);
	bool		compIsKnown		(serviceComp_C &newComp);
//
private:
//	for each type a table
	std::vector<SId_struct>		SId_table;		// FIG0/2
	std::vector<subChannel>		subChannel_table;	// FIG0/1
	std::vector<serviceComp_C>	SC_C_table;		// FIG0/2
	std::vector<serviceComp_P>	SC_P_table;		// FIG0/3
	std::vector<SC_language>	language_table;		// FIG0/5
	std::vector<serviceComp_G>	SC_G_table;		// FIG0/8
	std::vector<AppType>		AppType_table;		// FIG0/13
	std::vector<programType> 	programType_table;	// FIG0/17
	std::vector<FIG18_cluster>	announcement_table;	// FIG0/18

	int	SCIdsOf			(int index);
	int	dabTypeOf		(int index);
	int	languageOf 		(int index);
	int	appTypeOf		(int index);
	int	FEC_schemeOf		(int index);
	int	packetAddressOf		(int index);
	int	DSCTy			(int index);
	int	DG_flag			(int index);
	int	findIndex_SC_P_Table	(uint16_t SCId);
signals:
	void	announcement		(int, int);
};

