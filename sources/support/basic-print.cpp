#
/*
 *    Copyright (C) 2016 .. 2024
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

#include	"basic-print.h"
#include	"dab-tables.h"

//	the fib decoder passes on the (more or less) raw data,
//	needed to show the attributes of the different services
//
static
const char *audioHeader =
	" ; serviceName; serviceId; subChannel; start address (CU's); "\
	" length (CU's); protection; code rate; bitrate; dab type;" \
	" language; program type; fm freq;";
static
const char *packetHeader =
	" ; serviceName; serviceId; subChannel; start address (CU's); "\
	" length (CU's); protection; code rate; appType; FEC_scheme;" \
	 "packetAddress; DSCTy;";

	basicPrint::basicPrint	()	{}

	basicPrint::~basicPrint	()	{}

QStringList basicPrint::print (QList<contentType> serviceData) {
QStringList out;
	bool hasContents = false;
	for (auto &ct : serviceData) {
	   if (ct. TMid != 0)	// no audio
	      continue;
	   if (!hasContents)
	      out <<  QString (audioHeader);
	   hasContents = true;
	   out << audioData (ct);
	}
	hasContents = false;
	for (auto &ct : serviceData) {
	   if (ct. TMid != 3)	// no packet
	      continue;
	   if (!hasContents)
	      out << QString (packetHeader);
	   hasContents = true;
	   out << packetData (ct);
	}
	return out;
}

QString	basicPrint::audioData	(contentType &ct) {
	return QString (ct. isRunning ? "+" : "") + ";" +
	       QString (ct. serviceName)+ ";" +
	       QString::number (ct. SId, 16) + ";" +
	       QString::number (ct. subChId) + ";" +
	       QString::number (ct. startAddress)+ ";" +
	       QString::number (ct. length) + ";" +
	       ct. protLevel + ";" +
	       ct. codeRate + ";" +
	       QString::number (ct. bitRate) + ";" +
	       (ct. ASCTy_DSCTy == DAB_PLUS ? "DAB+" : "DAB") + ";" +
	       getLanguage (ct. language) + ";" +
	       getProgramType (ct. programType) + ";" +
	       ((ct. fmFrequencies. size () > 0) ?
                   QString::number (ct. fmFrequencies [0]) : " ");
}

static
QString kindOfService (uint8_t DSCTy, uint16_t appType) {
QString dtype;
	switch (DSCTy) {
	   case 1:
	      return "TMC (unsupported)";
	   case 2:
	      return "EWS, (unsupported)";
	   case 3:
	      return "ITTS, (unsupported)";
	   case 4:
	      return "paging";
	   case 5 :
	      if (appType == 0x44a)
	         return "journaline";
	      else
	      if (appType == 1500)
	         return "adv_adc data";
	      else
	      if (appType == 4)
	         return "tdc data";
	      else
	         return "unsupported";
	      break;
	   case 24:
	      return "MPEG-2 Transport stream";
	   case 44 :
	      dtype =  "journaline";
	      break;
	   case 59:
	      dtype = "ip data";
	      break;
	   case  60 :
	      if (appType == 7)
	         return "EPG/SPI";
	      else
	         return "MOT data";
	      break;
	   case 61:
	      return "Proprietary";
	   default:
	      return "unsupported";
	}
	return QString ("");
}
	
QString	basicPrint::packetData	(contentType &ct) {
QString res	= QString (ct. isRunning ? "+" : "") + ";" +
	          QString (ct. serviceName)+ ";" +
	          QString::number (ct. SId, 16) + ";" +
	          QString::number (ct. subChId) + ";" +
	          QString::number (ct. startAddress)+ ";" +
	          QString::number (ct. length) + ";" +
	          ct. protLevel + ";" +
	          ct. codeRate + ";" +
	          QString::number (ct. appType) + ";" +
	          QString::number (ct. FEC_scheme) + ";" +
	          QString::number (ct. packetAddress) + ";" +
	          kindOfService (ct. ASCTy_DSCTy, ct. appType) + ";";
	         return res;
}

//	We terminate the sequences with a ";", so that is why the
//	actual number is 1 smaller
int	basicPrint::scanWidth	() {
QString s1	= audioHeader;
QString s2	= packetHeader;
QStringList l1 = s1. split (";");
QStringList l2 = s2. split (";");
	return l1. size () >= l2. size () ? l1. size () -1 : l2. size () - 1;
}

