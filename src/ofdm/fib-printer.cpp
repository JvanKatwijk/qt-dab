#
/*
 *    Copyright (C) 2018 .. 2024
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
 * 	fib printer.
 *	the "contents" of the database is output to a stringlist
 */
#include	"fib-printer.h"
#include	"dab-tables.h"

	fibPrinter::fibPrinter (fibConfig *currentConfig,
	                                   ensemble *theEnsemble) {
	this	-> currentConfig = currentConfig;
	this	-> theEnsemble	= theEnsemble;
}

	fibPrinter::~fibPrinter () {}
	
QStringList	fibPrinter::basicPrint () {
QStringList out;
	bool hasContents = false;
	for (int i = 0; i < (int) currentConfig -> SC_C_table. size (); i ++) {
	   fibConfig::serviceComp_C &comp = currentConfig -> SC_C_table [i];
	   if (comp. TMid != 0) // audio
	      continue;
	   if (!hasContents) 
	      out << audioHeader ();
	   hasContents = true;
	   out << audioData (i);
	}
	hasContents = false;
	for (int i = 0; i < (int) currentConfig -> SC_C_table. size (); i ++) {
	   fibConfig::serviceComp_C &comp = currentConfig -> SC_C_table [i];
	   if (comp. TMid != 3) // packet
	      continue;
	   if (subChannelOf (i) == "")	// should not happen
	      continue;
	   if (!hasContents) {
	      out << "\n";
	      out << packetHeader ();
	   }
	   hasContents = true;
	   out << packetData (i);
	}
	return out;
}
//
QString	fibPrinter::serviceName		(int index) { 
uint32_t sid	=  currentConfig -> serviceIdOf (index);
	return theEnsemble -> SIdToserv (sid);
}

QString	fibPrinter::serviceIdOf		(int index) { 
	return QString::number (currentConfig -> SC_C_table [index]. SId, 16);
}

QString	fibPrinter::subChannelOf 	(int index) {
int subChannel	= currentConfig -> subChannelOf (index);
	if (subChannel < 0)
	   return "";
	return QString::number (subChannel);
}

QString	fibPrinter::startAddressOf 	(int index) {
int startAddr = currentConfig -> startAddressOf (index);
	return QString::number (startAddr);
}

QString	fibPrinter::lengthOf 		(int index) {
int length	= currentConfig -> lengthOf (index);
	return QString::number (length);
}

QString	fibPrinter::protLevelOf 	(int index) {
int	shortForm	= currentConfig -> shortFormOf (index);
int	protLevel	= currentConfig -> protLevelOf (index);
	return getProtectionLevel (shortForm, protLevel);
}

QString	fibPrinter::codeRateOf 		(int index) {
int	shortForm	= currentConfig -> shortFormOf (index);
int	protLevel	= currentConfig -> protLevelOf (index);
	return getCodeRate (shortForm, protLevel);
}

QString	fibPrinter::bitRateOf		(int index) {
int bitRate	= currentConfig -> bitRateOf (index);
	return QString::number (bitRate);
}

QString	fibPrinter::dabType 		(int index) {
int dabType	= currentConfig -> dabTypeOf (index);
	if (dabType == -1)
	   return " ";
	return dabType == 077 ? "DAB+" : "DAB";
}

QString	fibPrinter::languageOf 		(int index) {
int language	= currentConfig -> languageOf (index);
	return  getLanguage (language);
}

QString	fibPrinter::programTypeOf	(int index) {
uint32_t sid	= currentConfig -> serviceIdOf (index);
int Pt		= theEnsemble -> programType (sid);
	if (Pt > 0)
	   return getProgramType (Pt);
	return " ";
}

QString	fibPrinter::fmFreqOf		(int index) {
uint32_t sid	= currentConfig -> serviceIdOf (index);
int	fmFrequency	= theEnsemble -> fmFrequency (sid);
	if (fmFrequency != -1)
	   return QString::number (fmFrequency);
	return " ";
}

QString	fibPrinter::appTypeOf		(int index) {
int appType	= currentConfig -> appTypeOf (index);
	return QString::number (appType);
}

QString	fibPrinter::FEC_scheme		(int index) {
int FEC_Scheme	= currentConfig -> FEC_schemeOf (index);
	return QString::number (FEC_Scheme);
}

QString	fibPrinter::packetAddress	(int index) {
int packetAddress	= currentConfig -> packetAddressOf (index);
	return QString::number (packetAddress);
}

QString	fibPrinter::DSCTy		(int index) {
int DSCTy	= currentConfig -> DSCTy (index);
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
QString	fibPrinter::audioHeader		() {
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

QString	fibPrinter::audioData		(int index) {
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
QString	fibPrinter::packetHeader		() {
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

QString	fibPrinter::packetData	(int index) {
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
int	fibPrinter::scanWidth	() {
QString s1	= audioHeader ();
QString s2	= packetHeader ();
QStringList l1 = s1. split (";");
QStringList l2 = s2. split (";");
	return l1. size () >= l2. size () ? l1. size () -1 : l2. size () - 1;
}

