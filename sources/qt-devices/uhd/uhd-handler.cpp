/*
 *
 *    Copyright (C) 2015
 *    Sebastian Held <sebastian.held@imst.de>
 *
 *    This file is adapted for use with Qt-DAB
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
#include	"uhd-handler.h"

#include <uhd/types/tune_request.hpp>
#include <uhd/utils/thread_priority.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/exception.hpp>
#include <iostream>
#include <fstream>
#include <csignal>
#include <complex>
#include	"settings-handler.h"

	uhd_streamer::uhd_streamer (uhdHandler *d) {
	m_theStick		= d;
	m_stop_signal_called. store (false);
//create a receive streamer
	uhd::stream_args_t stream_args( "fc32", "sc16" );
	m_theStick -> m_rx_stream =
	          m_theStick -> m_usrp -> get_rx_stream (stream_args);
//setup streaming
	uhd::stream_cmd_t stream_cmd (uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS );
	stream_cmd.num_samps	= 0;
	stream_cmd.stream_now	= true;
	stream_cmd.time_spec	= uhd::time_spec_t();
	m_theStick -> m_rx_stream -> issue_stream_cmd (stream_cmd);

	start();
}

void	uhd_streamer::stop () {
	m_stop_signal_called. store (true);
	while (isRunning ())
	   wait(1);
}

void	uhd_streamer::run () {
	while (!m_stop_signal_called. load ()) {
//	get write position, ignore data2 and size2
	   int32_t size1, size2;
	   void *data1, *data2;
	   m_theStick -> theBuffer -> GetRingBufferWriteRegions (10000,
	                                                         &data1,
	                                                         &size1,
	                                                         &data2,
	                                                         &size2);

	   if (size1 == 0) {
// no room in ring buffer, wait for main thread to process the data
	      usleep (100); // wait 100 us
	      continue;
	   }

	   uhd::rx_metadata_t md;
	   size_t num_rx_samps =
	         m_theStick -> m_rx_stream -> recv (data1, size1, md, 1.0);
	   m_theStick -> theBuffer -> AdvanceRingBufferWriteIndex (num_rx_samps);

	   if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT) {
	      std::cout << "Timeout while streaming" << std::endl;
	      continue;
	   }

	   if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_OVERFLOW) {
	      std::cerr << "Got an overflow indication" << std::endl;
	      continue;
	   }

//	   if (md.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE) {
//	      std::cerr << boost::format("Receiver error: %s") % md.strerror() << std::endl;
//	      continue;
//	   }
	}
}

	uhdHandler::uhdHandler (QSettings *s ) {
	this	-> uhdSettings	= s;
	setupUi (&myFrame);
	setPositionAndSize (s, &myFrame, "uhdSettings");
	myFrame. show ();
	this	-> inputRate	= Khz (2048);
	this	-> ringbufferSize	= 1024;	// blocks of 1024 complexes
	this	-> theBuffer	= nullptr;	// also indicates good init or not
	lastFrequency		= 100000;
	m_workerHandle		= nullptr;
//	create a usrp device.
	std::string args;
	try {
	   m_usrp = uhd::usrp::multi_usrp::make (args);
//	Lock mboard clocks

	   std::string ref ("internal");
	   m_usrp -> set_clock_source (ref);

//	set sample rate
	   m_usrp -> set_rx_rate (inputRate);
	   inputRate = m_usrp -> get_rx_rate ();
	   fprintf (stderr, "Actual RX Rate: %f Msps...\n", inputRate/1e6);

//	allocate the rx buffer
	   theBuffer	= new RingBuffer<std::complex<float> >(ringbufferSize * 1024);
	}
	catch (...) {
	   fprintf (stderr, "No luck with uhd\n");
	   throw (std_exception_string ("No luck with USRP device");
	}
//	some housekeeping for the local frame
	externalGain		-> setMaximum (maxGain ());
	uhdSettings		-> beginGroup ("uhdSettings");
	externalGain 		-> setValue (
	            uhdSettings -> value ("externalGain", 40). toInt ());
	f_correction		-> setValue (
	            uhdSettings -> value ("f_correction", 0). toInt ());
	KhzOffset		-> setValue (
	            uhdSettings -> value ("KhzOffset", 0). toInt ());
	uhdSettings	-> endGroup ();

	setExternalGain	(externalGain	-> value ());
	set_KhzOffset	(KhzOffset	-> value ());
	connect (externalGain, SIGNAL (valueChanged (int)),
	         this, SLOT (setExternalGain (int)));
	connect (KhzOffset, SIGNAL (valueChanged (int)),
	         this, SLOT (set_KhzOffset (int)));
}

	uhdHandler::~uhdHandler () {
	storeWidgetPosition (uhdSettings, &myFrame, "uhdSettings");
	if (theBuffer != NULL) {
	   stopReader();
	   uhdSettings	-> beginGroup ("uhdSettings");
	   uhdSettings	-> setValue ("externalGain", 
	                                      externalGain -> value ());
	   uhdSettings	-> setValue ("f_correction",
	                                      f_correction -> value ());
	   uhdSettings	-> setValue ("KhzOffset",
	                                      KhzOffset	-> value ());
	   uhdSettings	-> endGroup ();
	   delete theBuffer;
	}
}

int32_t	uhdhandler::getVFOFrequency	() {
int32_t freq = m_usrp -> get_rx_freq ();
	std::cout << boost::format("Actual RX Freq: %f MHz...") % (freq/1e6) << std::endl << std::endl;
	return freq;
}

bool	uhdHandler::restartReader	(int32_t freq, int32_t skipped) {
	if (m_workerHandle != 0)
	   return true;

	uhd::tune_request_t tune_request (freq);
	m_usrp->set_rx_freq (tune_request);
	theBuffer -> FlushRingBuffer ();
	m_workerHandle = new uhd_streamer (this);
	return true;
}

void	uhdHandler::stopReader	() {
	if (m_workerHandle == 0)
	   return;

	m_workerHandle -> stop ();
	delete m_workerHandle;
	m_workerHandle = 0;
}
//
int32_t	uhdHandler::getSamples	(DSPCOMPLEX *v, int32_t size) {
	size = std::min ((uint32_t)size,
	                 (uint32_t)(theBuffer -> GetRingBufferReadAvailable ()));
	theBuffer -> getDataFromBuffer (v, size);
	return size;
}

int32_t	uhdHandler::Samples		() {
	return theBuffer -> GetRingBufferReadAvailable();
}

void	uhdHandler::resetBuffer	() {
	theBuffer -> FlushRingBuffer();
}

int16_t	uhdHandler::maxGain		() {
	uhd::gain_range_t range = m_usrp->get_rx_gain_range();
	return	range.stop();
}

void	uhdHandler::setExternalGain	(int32_t gain) {
	m_usrp -> set_rx_gain (gain);
}

int16_t	uhdHandler::bitDepth	() {
	return 16;
}

QString uhdHandler::deviceName	() {
	return "USRP device";
}


