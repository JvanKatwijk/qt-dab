#
/*
 *    Copyright (C) 2018 .. 2025
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
 *    along with Qt-DAB if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
//
//	The eti processor reads in the eti file, preprocesses
//	the FIB/FIG data and passed it on to the fib decoder
//
//	It also implements the interface to the backends (i.e.
//	the mscBackend class in the regular processing line
//
//	The code is taken from the eti-backend and integrated here.
//	eti frames are described in ETS 300 799 (Figure 5)
//
//	The approach taken here is to cmpletely decouple the processing
//	of eti input from the regular processing, 
//	In a previous version eti handling was incorpoated in the
//	ofdm handler, which lead to unreadable code

#include	<QSettings>
#include	"frontend-handler.h"
#include	"eti-processor.h"
#include	"dab-constants.h"
#include	"dab-params.h"
#include	"crc-handlers.h"
#include	"radio.h"
#include        <sys/time.h>
#include        <time.h>
#include	"device-handler.h"
#include	"process-params.h"
#include	"logger.h"

#include	"eti-files.h"

#include	"mp2processor.h"
#include	"mp4processor.h"
#include	"data-processor.h"

	etiProcessor::etiProcessor  	(RadioInterface *mr,
	                                 deviceHandler	*theDevice,
	                                 processParams	*p,
	                                 QSettings	*dabSettings,
	                                 logger		*theLogger,
	                                 uint8_t	cpuSupport):
	                                   my_fibDecoder (mr) {
	this	-> myRadioInterface	= mr;
	this	-> theDevice		= theDevice;
	this	-> p			= p;
	this	-> dabSettings		= dabSettings;
	this	-> theLogger		= theLogger;
	this	-> cpuSupport		= cpuSupport;

	fibCounter      = 1;
        successRatio    = 0;
	running. store (false);
	connect (this, &etiProcessor::showFICQuality,
                 mr, &RadioInterface::show_ficQuality);
}

	etiProcessor::~etiProcessor	() {}

void	etiProcessor::start		() {
	QThread::start ();
}

void	etiProcessor::stop		() {
	running. store (false);
}

bool	etiProcessor::isPrimaryService	(uint32_t SId, uint8_t SCIds) {
	return my_fibDecoder. isPrimaryService (SId, SCIds);
}

bool	etiProcessor::isAudioService	(uint32_t SId, uint8_t SCIds) {
	return my_fibDecoder. isAudioService (SId, SCIds);
}

bool	etiProcessor::isPacketService	(uint32_t SId, uint8_t SCIds) {
	return my_fibDecoder. isPacketService (SId, SCIds);
}

bool	etiProcessor::is_SPI		(uint32_t SId) {
	return my_fibDecoder. is_SPI (SId);
}

std::vector<uint8_t>
	etiProcessor::get_secondaryServices	(uint32_t SId) {
	return my_fibDecoder. get_secondaryServices (SId);
}

void etiProcessor::getServiceName	(QString & serviceName,
	                                 QString & shortName,
                                         uint32_t SId, uint8_t SCIds) {
	my_fibDecoder. getServiceName (serviceName, shortName, SId, SCIds);
}

void etiProcessor::mapNameToId		(const QString & serviceName,
                                         uint32_t &SId, uint8_t & SCIds) {
	my_fibDecoder. mapNameToId (serviceName, SId, SCIds);
}

void etiProcessor::audioData		(uint32_t SId, uint8_t SCIds,
                                                         audiodata &ad) {
	my_fibDecoder. audioData (SId, SCIds, ad);
}

void etiProcessor::packetData		(uint32_t SId, uint8_t SCIds,
                                                         packetdata &pd) {
	my_fibDecoder. packetData (SId, SCIds, pd);
}

QList<contentType> etiProcessor::contentPrint    () {
        QList<contentType> res = my_fibDecoder. contentPrint ();
        for (auto &ct : res) 
           ct. isRunning = serviceRuns (ct. SId, ct. SCIds);
        return res;
}

uint32_t etiProcessor::julianDate	() {
	return 	my_fibDecoder. julianDate ();
}

void	etiProcessor::run		() {
uint8_t buffer [6144];
int16_t n_frame;
fibElement fe;
int16_t subChannelId;
int16_t data_offset;
int16_t data_length     = -1;
int     n;
int16_t NST, FICF;
int64_t	startTime;
int	counter		= 0;
	running. store (true);
//	OK, ready to read a frame
//	However, we need some kind of clock. We know that we have 
//	for 24 msec audio in each frame
	while (running. load ()) {
	   while (running. load () &&
	           ((etiFiles *) theDevice) -> Samples () < 6144)
	      usleep (100);
	   if (!running. load ())
	      return;

	   n = ((etiFiles *)theDevice) -> getSamples (buffer, 6144);
//	   if (n == 0) {
//	      fprintf (stderr, "end of file, (counter = %d) we quit\n", counter);
//	         return;
//	      }
//
	   if (n < 0) {
	      perror ("Read error, exiting");
	      return;
	   }
	   counter ++;
//	   fprintf (stderr, "\t%d\r", counter);
//	extract the relevant data for handling the FIC
//	   fprintf (stderr, "			%d\r", buffer [4] & 0xFF);
	   if (buffer [0] == 0)		// error frame
	      continue;

	   FICF			= (buffer [5] & 0x80) >> 7;
	   NST			=  buffer [5] & 0x7F;
	   int16_t fibBase	= 12 + 4 * NST;
	   if (FICF != 0) {
	      memcpy (fe. fibdata, &buffer [fibBase], 96);
	      fe. fibno = buffer [4] & 0x03;
	      process_FIC (fe. fibdata, fe. fibno);
	   }
//
//	In the loop we look for running services getting their data
//	Characteristic parameters of running services are on
//	a "runStack"
//	While checking we do not want to be interfered
	   locker. lock ();
	   for (auto &s : runStack) {
	      int data_offset = 0;
	       for (int i = 0; i < NST; i++) {
                 int16_t SCID   = ( buffer [8 + 4 * i] & 0xfc) >> 2;
                 int16_t SAD    = ((buffer [8 + 4 * i] & 0x03) << 8) |
                                                      buffer [8 + 4 * i+1];
                 int16_t TPL    = (buffer [8 + 4 * i + 2] & 0xfc) >> 2;
                 int16_t STL    = ((buffer [8 + 4 * i + 2] & 0x03) << 8) |
                                                      buffer [8 + 4 *i+3];
                 if (s. subChId == SCID) {
                    data_length = STL * 8;
//	            fprintf (stderr,
//	               "Extracting channel: SCID=%d, SAD=%d, TPL=%d, STL=%d\n",
//	                                         SCID,SAD,TPL,STL);
	            int index = 12 + 4 * NST + FICF * 96 + data_offset;
	            if (s. p_a != nullptr) {
	               std::vector<uint8_t> dataVector (24 * s. bitRate);
	               for (int i = 0; i < 24 * s. bitRate / 8; i ++) {
	                  for (int j = 0; j < 8; j ++) {
	                     dataVector [8 * i + j] =
                                (buffer [index + i] >> (7 - j)) & 01;
	                  }
	               }
	               s. p_a -> addtoFrame (dataVector);
	            }
                 } 
	         data_offset += STL * 8;
              }
	   }
	   locker. unlock ();
	}
}

//	FIB's are segments of 256 bits. When here, got the FIB's
//	from the eti file, and to be certain, we apply a crc check
//
//	Bote that in this context, the incoming buffer is packed,
//	while the fibprocessor expects "one bit one byte",
//	so, unpacking first
#define	RANGE	50

void	etiProcessor::process_FIC (uint8_t *p, uint16_t fib) {
uint8_t	buffer [768];
	for (int i = 0; i < 96; i ++)
	   for (int j = 0; j < 8; j ++)
	      buffer [8 * i + j] = (p [i] >> (7 - j)) & 01;

	for (int i = 0; i < 3; i ++) {
	   if (++fibCounter > RANGE)
	      fibCounter = 0;
	   if (!check_CRC_bits (&buffer [i * 256], 256)) {
              if (successRatio > 0)
                 successRatio --;
              if (fibCounter == 0)
                 showFICQuality (successRatio, 100 / 50);
              continue;
           }

           if (successRatio < RANGE)
              successRatio ++;
           if (fibCounter == 0)
              showFICQuality (successRatio, 100 / RANGE);
	   my_fibDecoder. processFIB (&buffer [i * 256], 3 * fib);
	}
}
//
//	For starting and stopping
void	etiProcessor::stopService	(uint32_t SId, uint8_t SCIds) {
	for (int i = 0; i < (int)(runStack. size ()); i ++) {
           if (((uint32_t)(runStack. at (i). SId) ==  SId) &&
	                 (runStack. at (i). SCIds == SCIds)) {
              qInfo () << "halting backend" <<
                           QString::number (SId, 16);
	      locker. lock ();
	      if (runStack. at (i). p_a != nullptr)
	         delete runStack. at (i). p_a;
              runStack. erase (runStack. begin () + i);
	      locker. unlock ();
	      return;
	   }
	}
}

bool	etiProcessor::setAudioChannel	(audiodata & ad,
                                         RingBuffer<std::complex<int16_t>> *b,
                                         FILE *f, int flag) {
runElement el;
	el. SId	= ad. SId;
	el. SCIds	= ad. SCIds;
	el. subChId	= ad. subchId;
	el. bitRate	= ad. bitRate;
	el. p_a		= nullptr;

	for (auto &r : runStack) 
	   if ((ad. SId == r. SId) && (ad. SCIds == r. SCIds))
	      return false;	// already running

	if (ad.  ASCTy == DAB_PLUS) {
	   el. p_a	=  new mp4Processor (myRadioInterface,
	                                     ad. SId,
	                                     ad. bitRate,
	                                     false,
	                                     b,
	                                     p	-> frameBuffer,
	                                     f,
	                                     false);
	   locker. lock ();
	   runStack. push_back (el);
	   locker. unlock ();
	   return true;
	}
	else
	if (ad. ASCTy == DAB_OLD) {
	   el. p_a	= new mp2Processor (myRadioInterface,
	                                    ad. SId,
	                                    ad. bitRate,
	                                    false,
	                                    b,
	                                    p  -> frameBuffer,
	                                    f,
	                                    false);
	   locker. lock ();
	   runStack. push_back (el);
	   locker. unlock ();
	   return true;
	}
	return false;
}

bool	etiProcessor::setDataChannel	(packetdata &pd,
                                         RingBuffer<uint8_t> *b, int flag) {
runElement el;
	el. SId		= pd. SId;
	el. SCIds	= pd. SCIds;
	el. subChId	= pd. subchId;
	el. bitRate	= pd. bitRate;
	el. p_a		= nullptr;
	for (auto &r : runStack) 
	   if ((pd. SId == r. SId) && (pd. SCIds == r. SCIds))
	      return false;	// already running

	   
	el. p_a = new dataProcessor (myRadioInterface, &pd, b);
	locker. lock ();
	runStack. push_back (el);
	locker. unlock ();
	return true;
}

bool	etiProcessor::serviceRuns	(uint32_t SId, uint8_t SCIds) {
	for (auto &r : runStack) 
	   if ((r. SId == SId) && (r. SCIds == SCIds))
	      return true;
	return false;
}
