#
/*
 *    Copyright (C) 2014 .. 2025
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
 */
#include	"backend-driver.h"
#include        "mp2processor.h"
#include        "mp4processor.h"
#include	"data-processor.h"
#include	"logger.h"
//
//	Driver program for the selected backend. Embodying that in a
//	separate class makes the "Backend" class simpler.


	backendDriver::backendDriver (RadioInterface *mr,
	                              logger		*theLogger,
	                              descriptorType *d,
	                              bool	backgroundFlag,
	                              RingBuffer<std::complex<int16_t>> *audioBuffer,
	                              RingBuffer<uint8_t> *dataBuffer,
	                              RingBuffer<uint8_t> *frameBuffer,
	                              FILE *dump) {
	if (d -> type == AUDIO_SERVICE) {
	   if (((audiodata *)d) -> ASCTy == DAB_OLD) {
	      theLogger ->  log (logger::LOG_SERVICE_STARTS,
	                               "MP2 service ", d -> bitRate);
              theProcessor. reset (new mp2Processor (mr,
	                                       d -> bitRate,
                                               audioBuffer,
	                                       frameBuffer,
	                                       dump,
	                                       backgroundFlag));
	   }
           else
           if (((audiodata *)d) -> ASCTy == DAB_PLUS) {
	      theLogger -> log (logger::LOG_SERVICE_STARTS,
	                               "MP4 service ", d -> bitRate);
              theProcessor. reset (new mp4Processor (mr,
	                                        d -> bitRate,
                                                audioBuffer,
	                                        frameBuffer,
	                                        dump,
	                                        backgroundFlag));
	   }
	   else
	      theProcessor. reset (new frameProcessor ());	// should not happen
	}
	else
	if (d -> type == PACKET_SERVICE) {
	   theProcessor. reset (new dataProcessor (mr,
	                                     (packetdata *)d,
	                                     dataBuffer,
	                                     backgroundFlag));
	}
	else
	   theProcessor. reset (new frameProcessor ());	// should not happen
	running. store (true);;
}


    backendDriver::~backendDriver() {
	running. store (false);
	theProcessor. reset ();
}

//
void	backendDriver::addtoFrame (const std::vector<uint8_t> &theData) {
	if (running. load ())
	   theProcessor	-> addtoFrame (theData);
}
//
//	Note that asking the processor to stop is - if configured -
//	in a different thread.
void	backendDriver::stop	() {
	running. store (false);
	theProcessor -> stop ();
	theProcessor. reset ();
}

