#
/*
 *    Copyright (C)  2018 .. 2025
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
#include	"ensemble.h"
#include	"dab-tables.h"
//
//	Implementation of the FIG database
//
//	Here I took the easy approach: for (almost) each FIG, I added
//	a table, and the interpretation is "as and when needed"
//	Of course, on querying the FIG database, it needs more
//	cycles, but at least the code now is (in my opinion)
//	reasonable clear.
//
//	Note that while the config may switch, the ensemble info
//	is (almost) stable while the channel it is running in 
//	does not change
	fibConfig::fibConfig	(ensemble *theEnsemble) {
	this	-> theEnsemble	= theEnsemble;
	reset ();
}
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

//
//	For the following few functions, it holds that they all
//	fetch attributes from the subChannel, 
//int	fibConfig::startAddressOf	(int index) {
//int subChId = subChannelOf (index);
//int subCh_index = findIndex_subChannel_table (subChId);
//	if (subCh_index < 0)
//	   return -1;
//	return subChannel_table [subCh_index]. startAddr;
//}
//
//int	fibConfig::lengthOf 		(int index) {
//int subChId = subChannelOf (index);
//int subCh_index = findIndex_subChannel_table (subChId);	
//	if (subCh_index < 0)
//	   return -1;
//	return subChannel_table [subCh_index]. Length;
//}
//
//bool	fibConfig::shortFormOf		(int index) {
//int subChId = subChannelOf (index);
//int subCh_index = findIndex_subChannel_table (subChId);	
//	if (subCh_index < 0)
//	   return -1;
//	return subChannel_table [subCh_index]. shortForm;
//}
//
//int16_t	fibConfig::protLevelOf		(int index) {
//int subChId = subChannelOf (index);
//int subCh_index = findIndex_subChannel_table (subChId);	
//	if (subCh_index < 0)
//	   return -1;
//	return subChannel_table [subCh_index]. protLevel;
//}
//
//int	fibConfig::bitRateOf 		(int index) {
//int16_t subChId = subChannelOf (index);
//int subCh_index = findIndex_subChannel_table (subChId);
//	if (subCh_index < 0)
//	   return -1;
//	return subChannel_table [subCh_index]. bitRate;
//}
//
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
//
////////////////////////////////////////////////////////////////////

uint32_t fibConfig::getSId	(const int index) {
	return SC_C_table [index]. SId;
}

uint8_t	fibConfig::serviceType (const int index) {
	return SC_C_table [index]. TMid;
}

int	fibConfig::getNrComps			(const uint32_t SId) {
	for (auto &SId_element : SId_table)
	   if (SId_element. SId == SId)
	      return SId_element. comps. size ();
	return 0;
}

int	fibConfig::getServiceComp	(const QString &service) {
//	first we check to see if the service is a primary one
	for (auto &serv : theEnsemble -> primaries) {
	   if (serv. name != service)
	      continue;
	   for (auto & SId_element: SId_table) {
	      if (SId_element. SId == serv. SId)
	         return SId_element. comps [0];
	   }
	}
	
	for (auto &serv : theEnsemble -> secondaries) {
	   if (serv. name != service)
	      continue;
	   return getServiceComp_SCIds (serv. SId, serv. SCIds);
	}
	return -1;
}

int	fibConfig::getServiceComp	(const uint32_t SId,
	                                          const int compnr) {
	for (auto &SId_element : SId_table) {
           if (SId_element. SId == SId) {
              return SId_element. comps [compnr];
           }
        }
        return -1;
}

int	fibConfig::getServiceComp_SCIds	(const uint32_t SId,
	                                             const int SCIds) {
//	fprintf (stderr, "Looking for serviceComp %X %d\n", SId, SCIds);
	for (auto &SId_element : SId_table) {
	   if (SId_element. SId != SId)
	      continue;
	   for (int i = 0; i < (int) SId_element. comps. size (); i ++) {
	      int index = SId_element. comps [i];
	      if (SCIdsOf   (index) == SCIds)
	         return index;
	   }
	}
	return -1;
}

void	fibConfig::audioData	(const int index, audiodata &ad) {
serviceComp_C &comp = SC_C_table [index];
	for (auto &serv : theEnsemble -> primaries) {
	   if (serv. SId == comp. SId) {
	      ad. serviceName	= serv. name;
	      ad. shortName	= serv. shortName;
	      ad. SId		= serv. SId;
	      ad. programType	= serv. programType;
	      ad. fmFrequencies	= serv. fmFrequencies;
	      break;
	   }
	}
	int subChId	= subChannelOf (index);
	ad. subchId	= subChId;
	int subChannel_index = findIndex_subChannel_table (subChId);
	if (subChannel_index < 0)
	   return;
	subChannel &channel = subChannel_table [subChannel_index];
	ad. startAddr	= channel. startAddr;	
	ad. shortForm	= channel. shortForm;
	ad. protLevel	= channel. protLevel;
	ad. length	= channel. Length;
	ad. bitRate	= channel. bitRate;
	ad. ASCTy	= dabTypeOf (index);
	ad. language	= languageOf (index);
	ad. defined	= true;
}

void	fibConfig::packetData		(const int index, packetdata &pd) {
serviceComp_C &comp = SC_C_table [index];
	for (auto &serv : theEnsemble -> primaries) {
	   if (serv. SId == comp. SId) {
	      pd. serviceName	= serv. name;
	      pd. shortName	= serv. shortName;
	      pd. SId		= serv. SId;
	      break;
	   }
	}
	int subChId	= subChannelOf (index);
	pd. subchId	= subChId;
	int subChannel_index = findIndex_subChannel_table (subChId);
	if (subChannel_index < 0)
	   return;
	subChannel &channel = subChannel_table [subChannel_index];
	pd. startAddr	= channel. startAddr;	
	pd. shortForm	= channel. shortForm;
	pd. protLevel	= channel. protLevel;
	pd. length	= channel. Length;
	pd. bitRate	= channel. bitRate;
	pd. FEC_scheme	= FEC_schemeOf (index);
	pd. appType	= appTypeOf (index);
	pd. DGflag	= DG_flag (index);
	pd. DSCTy	= DSCTy (index);
	pd. packetAddress = packetAddressOf (index);
	pd. defined 	= true;
}

uint16_t fibConfig::getAnnouncing	(uint16_t SId) {
	for (auto &serv : SId_table)
	   if (serv. SId == SId)
	      return serv. announcing;
	return 0;
}

//	needed for generating eti files
int	fibConfig::nrChannels		() {
	return subChannel_table. size ();
}

void	fibConfig::getChannelInfo (channel_data *d, const int n) {
subChannel *selected = &subChannel_table [n];
	d       -> in_use	= true;
	d       -> id		= selected ->  subChId;
	d       -> start_cu	= selected ->  startAddr;
	d       -> protlev	= selected ->  protLevel; 
	d       -> size		= selected ->  Length;
	d       -> bitrate	= selected ->  bitRate;
	d       -> uepFlag	= selected ->  shortForm;
}

//
//	Some GUI functions, such as content printer and
//	printer with the scan function, need to get a description
//	of the attributes of the service.
//	Here we collect all attributes as specified by "contentType"
//	
QList<contentType> fibConfig::contentPrint () {
QList<contentType> res;
	for (int i = 0; i < (int)(SC_C_table. size ()); i ++) {
	   serviceComp_C &comp = SC_C_table [i];
	   contentType theData;
	   theData. TMid	= comp. TMid;
	   theData. SId		= comp. SId;
	   theData. isActive	= false;
	   if (comp. TMid == 0) {	// audio data
	      audiodata ad;
	      audioData (i, ad);
	      if (!ad. defined)		// should not happen
	         continue;
	      theData. serviceName	= ad. serviceName;
	      theData. subChId		= ad. subchId;
	      theData. SCIds		= ad. SCIds;
	      theData. startAddress	= ad. startAddr;
	      theData. length		= ad. length;
	      theData. codeRate		= getCodeRate (ad. shortForm,
	                                               ad. protLevel);
	      theData. protLevel	= getProtectionLevel (ad. shortForm,
	                                                      ad. protLevel);
	      theData. bitRate		= ad. bitRate;
	      theData. language		= ad. language;
	      theData. FEC_scheme	= 0;
	      theData. packetAddress	= 0;
	      theData. ASCTy_DSCTy	= ad. ASCTy;
	      theData. programType	=
	                          theEnsemble -> programType (ad. SId);
	      theData. fmFrequencies	=
	                          theEnsemble -> fmFrequencies (ad. SId);
	      res. push_back (theData);
	   }
	   else
	   if (comp. TMid == 3) {	// packet 
	      packetdata pd;
	      packetData (i, pd);
	      if (!pd. defined)		// should not happen
	         continue;
	      theData. serviceName	= pd. serviceName;
	      theData. subChId		= pd. subchId;
	      theData. SCIds		= pd. SCIds;
	      theData. startAddress	= pd. startAddr;
	      theData. length		= pd. length;
	      theData. codeRate		= getCodeRate (pd. shortForm,
	                                               pd. protLevel);
	      theData. protLevel	= getProtectionLevel (pd. shortForm,
	                                                      pd. protLevel);
	      theData. bitRate		= pd. bitRate;
	      theData. FEC_scheme	= pd. FEC_scheme;
	      theData. packetAddress	= pd. packetAddress;
	      theData. ASCTy_DSCTy	= pd. DSCTy;
	      theData. appType		= pd. appType;
	      theData. language		= 0;
	      theData. programType	= 0;
	      res. push_back (theData);
	   }
	}
	return res;
}
