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

#include	"fib-config.h"
#include	<stdio.h>
#include	<string.h>
//
//	Implementation of the FIG database
//
//	While in previous versions I tried to "optimize" by
//	adding properties to SIds and serviceNames, the code became
//	messy and a few errors could not be handled properly.
//
//	Here I took the easy approach: for (almost) each FIG, I added
//	a table, and the interpretation is "as and when needed"
//	Of course, on querying the FIG database, it needs more
//	cycles, but at least the code now is (in my opinion)
//	reasonable clear.

	fibConfig::fibConfig	() {}
	fibConfig::~fibConfig	() {}

void	fibConfig::reset	() {
	SId_table. 		resize (0);
	subChannel_table.	resize (0);
	SC_C_table.		resize (0);
	SC_P_table.		resize (0);
	SC_G_table. 		resize (0);
	language_table.		resize (0);
	programType_table.	resize (0);
	AppType_table.		resize (0);
	announcement_table. 	resize (0);
	memset (dateTime, 0, sizeof (dateTime));
}

int	fibConfig::serviceIdOf		(int index) {
	return SC_C_table [index]. SId;
}

int	fibConfig::SCIdsOf		(int index) {
serviceComp_C &comp = SC_C_table [index];
	if (comp. TMid == 0) {
	   for (int i = 0; i < (int)SC_G_table. size (); i ++) {
	      if (SC_G_table [i]. LS_flag != 0)
	         continue;
	      if (SC_G_table [i]. SId != comp. SId)
	         continue;
	      if (SC_G_table [i]. subChId == comp. subChId)
	         return SC_G_table [i]. SCIds;
	   }
	   return 0;		//  primary component
	}
	if (comp. TMid == 3) {	// should be it
	   for (int i = 0; i < (int) SC_G_table. size (); i ++) {
	      if (SC_G_table [i]. LS_flag == 0)
	         continue;
	      if (SC_G_table [i]. SId != comp. SId)
	         continue;
	      if (SC_G_table [i]. SCId == comp. SCId)
	         return SC_G_table [i]. SCIds;
	   }
	   return 0;
	}
	return -1;
}

int	fibConfig::subChannelOf		(int index) {
serviceComp_C &comp = SC_C_table [index];
	if (comp. TMid == 0)	// audio
	   return comp. subChId;
	int SCId_index = findIndex_SC_P_Table (comp. SCId);
	if (SCId_index < 0)
	   return -1;
	return SC_P_table [SCId_index]. subChId;
}

int	fibConfig::findIndex_subChannel_table (uint8_t subChId) {
	for (int i = 0; i < (int)subChannel_table. size (); i ++)
	   if (subChannel_table [i]. subChId == subChId)
	      return i;
	return -1;
}

int	fibConfig::startAddressOf	(int index) {
int subChId = subChannelOf (index);
int subCh_index = findIndex_subChannel_table (subChId);
	if (subCh_index < 0)
	   return -1;
	return subChannel_table [subCh_index]. startAddr;
}

int	fibConfig::lengthOf 		(int index) {
int subChId = subChannelOf (index);
int subCh_index = findIndex_subChannel_table (subChId);	
	if (subCh_index < 0)
	   return -1;
	return subChannel_table [subCh_index]. Length;
}

bool	fibConfig::shortFormOf		(int index) {
int subChId = subChannelOf (index);
int subCh_index = findIndex_subChannel_table (subChId);	
	if (subCh_index < 0)
	   return -1;
	return subChannel_table [subCh_index]. shortForm;
}

int16_t	fibConfig::protLevelOf		(int index) {
int subChId = subChannelOf (index);
int subCh_index = findIndex_subChannel_table (subChId);	
	if (subCh_index < 0)
	   return -1;
	return subChannel_table [subCh_index]. protLevel;
}

int	fibConfig::bitRateOf 		(int index) {
int16_t subChId = subChannelOf (index);
int subCh_index = findIndex_subChannel_table (subChId);
	if (subCh_index < 0)
	   return -1;
	return subChannel_table [subCh_index]. bitRate;
}

int	fibConfig::dabTypeOf		(int index) {
	if (SC_C_table [index]. TMid != 0)
	   return -1;
	return SC_C_table [index]. ASCTy;
}

int	fibConfig::languageOf 		(int index) {
	if (SC_C_table [index]. TMid == 0) {
	   int subChId = SC_C_table [index]. subChId;
	   for (auto &lt : language_table)
	      if ((lt. LS_flag == 0) && (lt. subChId == subChId))
	         return lt. language;
	}
	if (SC_C_table [index]. TMid == 3) {  // it should be
	   int SCId	= SC_C_table [index]. SCId;
	   for (auto &lt : language_table)
	      if ((lt. LS_flag == 0) && (lt. SCId == SCId))
	         return lt. language;
	}
	return -1;
}
	
int	fibConfig::appTypeOf		(int index) {
uint32_t SId	= SC_C_table [index]. SId;
	int SCIds = SCIdsOf (index);
	if (SCIds == -1)
	   return -1;
	int appIndex = findIndexApptype_table (SId, SCIds);
	if (appIndex < 0)
	   return -1;
	return AppType_table [appIndex]. Apptype;
}

int	fibConfig::FEC_schemeOf		(int index) {
int16_t subChId	= subChannelOf (index);
int subCh_index = findIndex_subChannel_table (subChId);
	if (index < 0)
	   return -1;
	return subChannel_table [subCh_index]. FEC_scheme;
}

int	fibConfig::packetAddressOf	(int index) {
serviceComp_C &comp = SC_C_table [index];
	if (comp. TMid != 3)
	   return -1;
	int SCId_index = findIndex_SC_P_Table (comp. SCId);
	if (SCId_index < 0)
	   return -1;
	return SC_P_table [SCId_index]. packetAddress;
}

int	fibConfig::DSCTy		(int index) {
serviceComp_C &comp = SC_C_table [index];
	if (comp. TMid != 3)
	   return -1;
	int SCId_index = findIndex_SC_P_Table (comp. SCId);
	if (SCId_index < 0)
	   return -1;
	return SC_P_table [SCId_index]. DSCTy;
}

int	fibConfig::DG_flag		(int index) {
serviceComp_C &comp = SC_C_table [index];
	if (comp. TMid != 3)
	   return -1;
	int SCId_index = findIndex_SC_P_Table (comp. SCId);
	if (SCId_index < 0)
	   return -1;
	return SC_P_table [SCId_index]. DG_flag;
}

int	fibConfig::findIndex_SC_P_Table (uint16_t SCId) {
	for (int i = 0; i < (int)SC_P_table. size (); i ++)
	   if (SC_P_table [i]. SCId == SCId)
	      return i;
	return -1;
}

int	fibConfig::findIndex_languageTable (uint8_t key_1, uint16_t key_2) {
	for (int i = 0; i < (int) language_table. size (); i ++) {
	   if (language_table [i]. LS_flag != key_1)
	      continue;
	   if ((key_1 == 0) && (language_table [i]. subChId == key_2))
	      return i;
	   if ((key_1 == 1) && (language_table [i]. SCId == key_2))
	      return i;
	}
	return -1;
}

int	fibConfig::findIndexApptype_table (uint32_t SId, uint8_t SCIds) {
	for (int i = 0; i < (int)AppType_table. size (); i ++)
	   if ((AppType_table [i]. SId == SId) &&
	       (AppType_table [i]. SCIds == SCIds))
	   return i;
	return -1;
}

bool	fibConfig::compIsKnown	(serviceComp_C &newComp) {
	for (auto &comp : SC_C_table) {
	   if (comp. SId != newComp. SId)
	      continue;
	   if (comp. compNr != newComp. compNr)
	      continue;
	   return true;
	}
	return false;
}

int	fibConfig::freeSpace	() {
int amount = 0;
	for (auto &ss: subChannel_table) 
	   amount += ss. Length;
	return 864 - amount;
}

