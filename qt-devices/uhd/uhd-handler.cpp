#
/*
 *
 *    Copyright (C) 2015
 *    Sebastian Held <sebastian.held@imst.de>
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
 *    along with Qt-DAB-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <uhd/types/tune_request.hpp>
#include <uhd/utils/thread.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/exception.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <fstream>
#include <csignal>
#include <complex>
#include "uhd-handler.h"
#include "device-exceptions.h"

	uhd_streamer::uhd_streamer (uhdHandler * d) :
	                                       m_theStick (d) {
	m_stop_signal_called. store (false);
	uhd::stream_args_t stream_args ("fc32", "sc16");
	m_theStick->m_rx_stream =
	             m_theStick -> m_usrp->get_rx_stream(stream_args);
	uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
	stream_cmd.num_samps = 0;
	stream_cmd.stream_now = true;
	stream_cmd.time_spec = uhd::time_spec_t();
	m_theStick->m_rx_stream->issue_stream_cmd(stream_cmd);

	start();
}

	uhd_streamer::~uhd_streamer	() {}

void	uhd_streamer::stop	() {
	m_stop_signal_called. store (true);
	while (isRunning ()) {
	   wait (1);
	}
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
//	no room in ring buffer, wait for main thread to process the data
	      usleep (100); // wait 100 us
	      continue;
	   }

	   uhd::rx_metadata_t md;
	   size_t num_rx_samps =
                 m_theStick -> m_rx_stream -> recv (data1, size1, md, 1.0);
	   m_theStick -> theBuffer -> AdvanceRingBufferWriteIndex (num_rx_samps);

	   if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT) {
	      std::cout << boost::format("Timeout while streaming") << std::endl;
	      continue;
	   }

	   if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_OVERFLOW) {
	      std::cerr << boost::format("Got an overflow indication") << std::endl;
	      continue;
	   }

    //	   if (md.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE) {
    //	      std::cerr << boost::format("Receiver error: %s") % md.strerror() << std::endl;
    //	      continue;
    //	   }
	}
}

		uhdHandler::uhdHandler (QSettings * s) :
	                                            Ui_uhdWidget (),
	                                            uhdSettings (s) {

std::vector<std::string> antList;
	setupUi (&myFrame);
	myFrame.show ();
	inputRate	= 2048000;
	ringBufferSize	= 1024;

// create a usrp device.
	std::string args;
	std::cout << std::endl;
	std::cout << boost::format("Creating the USRP device with: %s...") % args << std::endl;
	try {
	   m_usrp = uhd::usrp::multi_usrp::make(args);
	   std::string ref("internal");
	   m_usrp->set_clock_source(ref);

//	fill antenna connectors combobox
	   antList = m_usrp -> get_rx_antennas ();
	   if (antList.empty()) {
	      antList.emplace_back("(empty)");
	   }
	   for (const auto & al : antList)
	      antennaSelect -> addItem (al.c_str());

	   std::cout << boost::format("Using Device: %s") % m_usrp->get_pp_string() << std::endl;
//	set sample rate
	   m_usrp -> set_rx_rate (inputRate);
	   inputRate = (int32_t)std::round(m_usrp->get_rx_rate());
	   std::cout << boost::format("Actual RX Rate: %f Msps...") % (inputRate / 1e6) << std::endl << std::endl;

//	allocate the rx buffer
	   theBuffer = new RingBuffer<std::complex<float>>(ringBufferSize * 1024);
	} catch (...) {
	   qWarning("No luck with UHD\n");
	   throw (uhd_exception ("No luck with UHD"));
	}
//	some housekeeping for the local frame
	externalGain->setMaximum (maxGain ());
	uhdSettings -> beginGroup ("uhd_settings");
	externalGain -> setValue (
	            uhdSettings -> value("externalGain", 40).toInt());
	f_correction -> setValue (
	            uhdSettings -> value ("f_correction", 0).toInt());
	KhzOffset -> setValue (
	            uhdSettings -> value ("KhzOffset", 0).toInt());
	const QString h = uhdSettings -> value ("antSelect", "default").toString();
	const int32_t k = antennaSelect -> findText (h);
	if (k != -1)
	   antennaSelect -> setCurrentIndex(k);

	uhdSettings -> endGroup();
	setExternalGain		(externalGain -> value ());
	handle_ant_selector	(antennaSelect->currentText());

	connect (externalGain, SIGNAL (valueChanged (int)),
	         this, SLOT (setExternalGain (int)));
	connect (antennaSelect, SIGNAL (activated (const QString &)),
	         this, SLOT (slot_handle_ant_selector (const QString &)));
}

	uhdHandler::~uhdHandler () {
	if (theBuffer != nullptr) {
	   stopReader ();
	   uhdSettings -> beginGroup("uhdSettings");
	   uhdSettings -> setValue("externalGain", externalGain -> value());
	   uhdSettings -> setValue ("f_correction", f_correction -> value ());
	   uhdSettings -> setValue ("KhzOffset", KhzOffset -> value());
	   uhdSettings -> endGroup();
	   delete theBuffer;
	}
}

bool	uhdHandler::restartReader(int32_t freq) {

	std::cout << boost::format("Setting RX Freq: %f MHz...") % (freq / 1e6) << std::endl;
	uhd::tune_request_t tune_request(freq);
	m_usrp->set_rx_freq(tune_request);

	if (m_workerHandle != nullptr) {
	   return true;
	}

	theBuffer -> FlushRingBuffer();
	m_workerHandle = new uhd_streamer(this);
	return true;
}

void	uhdHandler::stopReader () {
	if (m_workerHandle == nullptr)
	   return;

	m_workerHandle -> stop ();
	delete m_workerHandle;
	m_workerHandle = nullptr;
}

int32_t uhdHandler::getSamples (std::complex<float> * v, int32_t size) {
	size = std::min (size, theBuffer -> GetRingBufferReadAvailable());
	return	theBuffer -> getDataFromBuffer (v, size);
}

int32_t uhdHandler::Samples () {
	return theBuffer -> GetRingBufferReadAvailable ();
}

void	uhdHandler::resetBuffer () {
	theBuffer -> FlushRingBuffer();
}

int16_t uhdHandler::bitDepth () {
	return 12;
}

QString uhdHandler::deviceName () {
	return "UHD";
}

int16_t uhdHandler::maxGain () {
	uhd::gain_range_t range = m_usrp->get_rx_gain_range();
	return (int16_t)std::round(range.stop());
}

void	uhdHandler::setExternalGain (int gain) {
	std::cout << boost::format("Setting RX Gain: %f dB...") % gain << std::endl;
	m_usrp -> set_rx_gain(gain);
}

void	uhdHandler::handle_ant_selector(const QString & iAnt) {

	try {
	   m_usrp -> set_rx_antenna(iAnt. toStdString ());
	   uhdSettings	-> beginGroup ("uhd_settings");
	   uhdSettings	-> setValue ("antSelector", iAnt);
	   uhdSettings	-> endGroup ();
	} catch (...) {
	   qWarning("Unknown Antenna name: %s", iAnt.toStdString().c_str());
	}
}
