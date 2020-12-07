#
/*
 *    Copyright (C) 2013, 2014, 2015, 2016, 2017
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
#include	<cstdio>
#include	<QStringList>
#include	"ensemble-printer.h"
#include	"country-codes.h"
#include	"dab-tables.h"
#include	"text-mapper.h"
#include	"dab-processor.h"

static
const char *uep_rates  [] = {nullptr, "7/20", "2/5", "1/2", "3/5", "3/4"};
static
const char *eep_Arates [] = {nullptr, "1/4",  "3/8", "1/2", "3/4"};
static
const char *eep_Brates [] = {nullptr, "4/9",  "4/7", "4/6", "4/5"};


		ensemblePrinter:: ensemblePrinter() {
}

		ensemblePrinter::~ensemblePrinter() {
}

QString		ensemblePrinter::code_to_string (uint8_t ecc,
	                                           uint8_t countryId) {
int16_t	i = 0;

	while (countryTable [i]. ecc != 0) {
	   if ((countryTable [i]. ecc == ecc) &&
	       (countryTable [i]. countryId == countryId))
	      return QString (countryTable [i]. countryName);
	   i ++;
	}
	return QString ("          ");
}

static
QString textfor (QByteArray transmitters) {
QString s;
//	fprintf (stderr, "in textfor: transmitters. size = %d\n", transmitters. size ());
	if (transmitters. size () == 0)
	   return " ";
	for (int i = 0; i < transmitters. size (); i += 2)
	   s = s + " (" + QString::number (transmitters. at (i)) +
	           "+" + QString::number (transmitters. at (i + 1)) + ")";
	return s;
}

void	ensemblePrinter::showSummaryData  (QString	channel,
	                                   int32_t	freq,
	                                   QString	SNR,
	                                   QString	theTime,
	                                   QByteArray	transmitters,
	                                   std::vector<serviceId> Services,
	                                   dabProcessor *my_dabProcessor,
	                                   FILE		*file_P) {
uint8_t ecc_byte	= my_dabProcessor -> get_ecc();
QString	ensembleLabel	= my_dabProcessor -> get_ensembleName();
int32_t	ensembleId	= my_dabProcessor -> get_ensembleId();
QString currentChannel	= channel;
int32_t	frequency	= freq;
bool	firstData;

	if (ensembleLabel == QString (""))
	   return;

	fprintf (file_P, "\n\n\n");
	fprintf (file_P, "%s; %s; %X; frequency %d; transmitterId(s) %s; %s; nr services %d;%s \n\n",
	                  currentChannel. toUtf8(). data(),
	                  ensembleLabel. toUtf8(). data(),
	                  ensembleId,
	                  frequency / 1000,
	                  textfor (transmitters). toLatin1 (). data (),
	                  SNR. toLatin1 (). data (),
	                  Services. size (),
	                  theTime. toUtf8(). data ());
}



void	ensemblePrinter::showEnsembleData (QString	channel,
	                                   int32_t	freq,
	                                   QString	theTime,
	                                   QByteArray	transmitters,
	                                   std::vector<serviceId> Services,
	                                   dabProcessor *my_dabProcessor,
	                                   FILE		*file_P) {
uint8_t	countryId;
int16_t	i;
textMapper	theMapper;
uint8_t ecc_byte	= my_dabProcessor -> get_ecc();
QString	ensembleLabel	= my_dabProcessor -> get_ensembleName();
int32_t	ensembleId	= my_dabProcessor -> get_ensembleId();
QString currentChannel	= channel;
int32_t	frequency	= freq;
bool	firstData;

	if (ensembleLabel == QString (""))
	   return;

	fprintf (file_P, "\n\n\n");
	fprintf (file_P, "%s; ensembleId %X; channel %s; frequency %d; time of recording  %s; tii %s\n\n",
	                  ensembleLabel. toUtf8(). data(),
	                  ensembleId,
	                  currentChannel. toUtf8(). data(),
	                  frequency / 1000,
	                  theTime. toUtf8(). data (),
	                  textfor (transmitters). toLatin1 (). data ());
	                
	fprintf (file_P, "\nAudio services\nprogram name;country;serviceId;subchannelId;start address;length (CU); bit rate;DAB/DAB+; prot level; code rate; language; program type\n\n");

	for (const auto serv : Services) {
	   QString audioService = serv. name;
	   audiodata d;
	   my_dabProcessor -> dataforAudioService (audioService, &d);
	   if (!d. defined)
	      continue;
	   QString protL	= getProtectionLevel (d. shortForm, 
	                                           d. protLevel);
	   QString codeRate	= getCodeRate (d. shortForm, 
	                                       d. protLevel);
	   countryId = (d. SId >> 12) & 0xF;
	   fprintf (file_P, "%s;%s;%X;%d;%d;%d;%d;%s;%s;%s;%s;%s;\n",
	                 audioService. toUtf8(). data(),
	                 code_to_string (ecc_byte, countryId). toUtf8(). data(),
	                 d. SId,
	                 d. subchId,
	                 d. startAddr,
	                 d. length,
	                 d. bitRate,
	                 d. ASCTy == 077 ? "DAB+" : "DAB",
	                 protL. toUtf8(). data(),
	                 codeRate. toUtf8(). data(),
	                 theMapper. get_programm_language_string (d. language),
	                 theMapper. get_programm_type_string (d. programType) );
	}

	firstData	= true;
	for (serviceId dataService: Services) {
	   for (i = 0; i < 5; i ++) {
	      packetdata d;
	      my_dabProcessor -> dataforPacketService (dataService. name,
                                                        &d, i);
	      if (!d. defined)
	         continue;

	      if (firstData) {
	         fprintf (file_P, "\n\n\nData Services\nprogram name;;serviceId;subchannelId;start address;length (CU); bit rate; FEC; prot level; appType ; subService ; \n\n");
	         firstData = false;
	      }

	      uint16_t h = d. protLevel;
	      QString protL;
	      QString codeRate;
	      if (!d. shortForm) {
	         protL = "EEP ";
	         protL. append (QString::number ((h & 03) + 1));
	         if ((h & (1 << 2)) == 0) {
	            protL. append ("-A");
	            codeRate = eep_Arates [(h & 03) + 1];
	         }
	         else {
	            protL. append ("-B");
	            codeRate = eep_Brates [(h & 03) + 1];
	         }
	         h = (h & 03) + 1;
	      }
	      else  {
	         protL = "UEP ";
	         protL. append (QString::number (h));
	         codeRate = uep_rates [h];
	      }
	      countryId = (d. SId >> (5 * 4)) & 0xF;
	      fprintf (file_P, "%s;%s;%X;%d;%d;%d;%d;%d;%s;%d;%s;;\n",
	                        dataService. name. toUtf8(). data(),
	                     code_to_string (ecc_byte, countryId). toUtf8(). data(),
	                     d. SId,
	                     d. subchId,
	                     d. startAddr,
	                     d. length,
	                     d. bitRate,
	                     d. FEC_scheme,
	                     protL. toUtf8(). data(),
	                     d. appType,
	                     d. compnr == 0 ? "no": "yes");
	   }
	}
}

