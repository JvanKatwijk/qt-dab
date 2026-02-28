/*
 *    Copyright (C) 2026
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation recorder 2 of the License.
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
#include	<cstdio> 
#include 	<iostream>
#include	<QSettings>
#include 	"soapy-handler.h"
#include	"selector.h"
#include	"settings-handler.h"
#include	"position-handler.h"
#include	"device-exceptions.h"
#include	"dab-constants.h"
#include	"xml-filewriter.h"

	soapyHandler::soapyHandler (QSettings * settings):
	                                     m_sampleBuffer (8 * 1024 * 1024),
	                                     theConverter (&m_sampleBuffer) {
std::vector<QString> deviceString;
std::vector<QString> serialString;
std::vector<QString> labelString;

	this	-> soapySettings = settings;
	setupUi (&myFrame);
	setPositionAndSize (soapySettings, &myFrame, "soapySettings");

	connect (&theConverter, &soapyConverter::reportStatus,
	         this, &soapyHandler::reportStatus);

	SoapySDR::KwargsList results = SoapySDR::Device::enumerate ();
//	const auto results = SoapySDR::Device::enumerate ();
	size_t length = results. size ();
	if (length == 0) 
	   throw device_exception ("No devices found\n");

	for (size_t i = 0; i < length; i++) {
	   for (const auto &it : results [i]) {
	      if (it. first ==  std::string ("driver")) {
	         QString second = QString::fromStdString (it. second);
	         if (second != "audio") {
	            if (it. first ==  std::string ("serial"))
	               serialString.
	                   push_back (QString::fromStdString (it. second));
	            if (it. first == std::string ("label"))
	               labelString.
	                   push_back (QString::fromStdString (it. second));
	         }
	      }
	   }
	}

	int deviceIndex = 0;
	if (labelString. size () > 1) {
	   selector deviceSelector (labelString [0]);
	   for (auto &s : labelString) 
	      deviceSelector. addtoList (s);
	   deviceIndex = deviceSelector.QDialog::exec();
	}

	selectedString	= deviceString [deviceIndex];
	selectedSerial	= serialString [deviceIndex];

	deviceLabel	-> setText (selectedString);
	serialNumber	-> setText (selectedSerial);

	antennaSelector -> hide ();
	gainSelector_0  -> hide ();
	gainSelector_1  -> hide ();
	gainSelector_2  -> hide ();
	gainLabel_0	-> hide ();
	gainLabel_1	-> hide ();
	gainLabel_2	-> hide ();
	agcControl	-> hide ();

	m_device	= nullptr;
	m_stream	= nullptr;
	m_running. store (false);
	m_dumping. store (false);
	toSkip. store (0);
	xmlWriter	= nullptr;
	connect (dumpButton, &QPushButton::clicked,
	         this, &soapyHandler::handle_xmlDump);

	m_freq		= 220000000;
	createDevice (selectedString, selectedSerial);
}

	soapyHandler::~soapyHandler () {
	close_xmlDump ();
	m_running. store (false);
	if (m_thread. joinable ())
	   m_thread. join ();
	if (m_stream != nullptr)
	   m_device -> closeStream (m_stream);
	if (m_device != nullptr) {
	   SoapySDR::Device::unmake (m_device);
	   m_device = nullptr;
	}
	storeWidgetPosition (soapySettings, &myFrame, "soapySettings");
	myFrame. hide ();
}

void	soapyHandler::createDevice (QString driver, QString serial) {
std::stringstream ss;

	QString handlerName = "driver=" + driver + ",serial=" + serial;
	m_device = SoapySDR::Device::make (handlerName. toLatin1 (). data ());
	if (m_device == nullptr)
	   throw device_exception ("Could not find soapy support\n");

	deviceNameLabel -> setText (QString::fromStdString (m_device -> getHardwareKey ()));

	if (m_device -> hasGainMode (SOAPY_SDR_RX, 0)) {
	   agcControl -> show ();
	   if (m_device -> getGainMode (SOAPY_SDR_RX, 0))
	      agcControl -> setChecked (true);
#if QT_VERSION >= QT_VERSION_CHECK (6, 7, 0)
	   connect(agcControl, &QCheckBox::checkStateChanged,
#else
	   connect(agcControl, &QCheckBox::stateChanged,
#endif
            this, &soapyHandler::set_agcControl);
	}

//formats

//	antennas
	std::vector <std::string> antennas =
	          m_device -> listAntennas (SOAPY_SDR_RX, 0);
	if (antennas. size () > 1) {
	   for (size_t i = 0; i < antennas.size (); i++)
	      antennaSelector -> addItem (QString::fromStdString (antennas [i]));
	   connect (antennaSelector, &QComboBox::textActivated,
                    this, &soapyHandler::handleAntenna);
	   antennaSelector -> show();
	}
	if (antennas. size () > 0)
	   m_device -> setAntenna (SOAPY_SDR_RX, 0, antennas [0]);

//	gains

	gainsList = m_device -> listGains (SOAPY_SDR_RX, 0);
	if (gainsList. size () > 0) {
	   SoapySDR::Range r =
	            m_device -> getGainRange (SOAPY_SDR_RX, 0, gainsList[0]);
	   gainSelector_0 -> setMinimum (r. minimum ());
	   gainSelector_0 -> setMaximum (r. maximum ());
	   gainLabel_0	-> setText (QString::fromStdString (gainsList [0]));
	   gainSelector_0 -> show ();
	   gainLabel_0 -> show();
	   connect (gainSelector_0, qOverload<int>(&QSpinBox::valueChanged),
	            this, &soapyHandler::setGain_0);
	}

	if (gainsList. size () > 1) {
	   SoapySDR::Range r =
	            m_device -> getGainRange (SOAPY_SDR_RX, 0, gainsList [1]);
	   gainSelector_1 -> setMinimum (r. minimum ());
	   gainSelector_1 -> setMaximum (r. maximum ());
	   gainLabel_1 -> setText (QString::fromStdString (gainsList [1]));
	   gainSelector_1 -> show ();
	   gainLabel_1 -> show ();
	   connect (gainSelector_2, qOverload<int>(&QSpinBox::valueChanged),
	            this, &soapyHandler::setGain_1);
	}

	if (gainsList. size () > 2) {
	   SoapySDR::Range r =
	            m_device -> getGainRange (SOAPY_SDR_RX, 0, gainsList [2]);
	   gainSelector_2 -> setMinimum (r. minimum ());
	   gainSelector_2 -> setMaximum (r. maximum ());
	   gainLabel_2 -> setText (QString::fromStdString (gainsList [2]));
	   gainSelector_2 -> show ();
	   gainLabel_2 -> show ();
	   connect (gainSelector_2, qOverload<int>(&QSpinBox::valueChanged),
	            this, &soapyHandler::setGain_2);
	}

	balanceIndicator	-> hide ();
	if (m_device -> hasIQBalanceMode (SOAPY_SDR_RX, 0)) {
	   balanceIndicator	-> show ();
#if QT_VERSION >= QT_VERSION_CHECK (6, 7, 0)
	   connect (balanceIndicator, &QCheckBox::checkStateChanged,
#else   
	   connect (balanceIndicator, &QCheckBox::stateChanged,
#endif
            this, &soapyHandler::handle_balanceIndicator);
	}

	DCModeIndicator	-> hide ();
	if (m_device -> hasDCOffsetMode (SOAPY_SDR_RX, 0)) {
	   DCModeIndicator	-> show ();
#if QT_VERSION >= QT_VERSION_CHECK (6, 7, 0)
	   connect (DCModeIndicator, &QCheckBox::checkStateChanged,
#else   
	   connect (DCModeIndicator, &QCheckBox::stateChanged,
#endif
            this, &soapyHandler::handle_DCModeIndicator);
	}

	ppmIndicator	-> hide ();
	if (m_device -> hasFrequencyCorrection (SOAPY_SDR_RX, 0)) {
	   ppmIndicator		-> show ();
	   connect (ppmIndicator, &QDoubleSpinBox::valueChanged,
	            this, &soapyHandler::handle_ppmIndicator);
	}

//	frequencies
	SoapySDR::RangeList freqList =
	            m_device -> getFrequencyRange (SOAPY_SDR_RX, 0);
	bool low	= false;
	bool high	= false;
	for (auto &fr : freqList) {
	   if (fr. minimum () <= MHz (75))
	      low = true;
	   if (fr. maximum () >= MHz (230))
	      high = true;
	}
	if (!(low & high))
	   throw device_exception ("Device no suitable for DAB reception");

//	rates
	SoapySDR::RangeList rangelist =
	            m_device -> getSampleRateRange (SOAPY_SDR_RX, 0);
	selectedRate = findDesiredSamplerate (rangelist);
	if ((driver == "uhd") || (driver == "UHD"))
	   selectedRate = SAMPLERATE;

	if (selectedRate < 0)
	   throw device_exception ("no usable samplerate\n");

	theConverter. setup (selectedRate, SAMPLERATE);

	samplerateLabel -> setText (QString::number(selectedRate));
	m_device -> setSampleRate (SOAPY_SDR_RX, 0, selectedRate);

//	bandwidths
	int32_t selectedWidth = 0;
	SoapySDR::RangeList bandwidthList =
	            m_device -> getBandwidthRange (SOAPY_SDR_RX, 0);
	if (!bandwidthList. empty ())
	   selectedWidth = findDesiredBandwidth (bandwidthList);

	if (selectedWidth > 0) {
    	   bandwidthLabel -> setText (QString::number (selectedWidth));
	   m_device  -> setBandwidth (SOAPY_SDR_RX, 0, selectedWidth);
	}
	else
	   bandwidthLabel -> setText ("????");

	m_device -> setFrequency (SOAPY_SDR_RX, 0, 220000000.0);

	double dd;
	streamFormat =
	         m_device -> getNativeStreamFormat (SOAPY_SDR_RX, 0, dd);

	std::vector<size_t> xxx;
	m_stream = m_device -> setupStream (SOAPY_SDR_RX, "CF32",
	                                       xxx, SoapySDR::Kwargs ());
	if (m_stream == nullptr)
	   throw  device_exception ("cannot open stream");

	statusLabel -> setText ("running");
	m_thread	= std::thread (&soapyHandler::workerthread, this);
}

bool	soapyHandler::restartReader (int32_t freq, int skipped) {
	if (m_device != nullptr)
	   m_device -> setFrequency (SOAPY_SDR_RX, 0, freq);
	theConverter. reset ();
	m_sampleBuffer. FlushRingBuffer ();
	m_freq		= freq;
	toSkip. store (skipped);
	return true;
}

void	soapyHandler::stopReader () {
	theConverter. reset ();
}

int32_t	soapyHandler::getVFOFrequency	() {
	if (m_device == nullptr)
	   return 0;
	return (int32_t)(m_device -> getFrequency (SOAPY_SDR_RX, 0));
}

int32_t soapyHandler::getSamples (std::complex<float> *buffer,
	                                               int32_t amount) {
	if (m_device == nullptr)
	   return 0;
	int real_amount = m_sampleBuffer. getDataFromBuffer (buffer,
                                                            (uint32_t)amount);
	if (m_dumping. load ())
	   xmlWriter    -> add (buffer, real_amount);
        return real_amount;
}

int32_t soapyHandler::Samples () {
	if (m_device == nullptr)
	   return 0;
	return m_sampleBuffer. GetRingBufferReadAvailable ();
}

void soapyHandler::reset	() {
	theConverter. reset ();
	m_sampleBuffer. FlushRingBuffer ();
}

int16_t	soapyHandler::bitDepth	() {
	if (streamFormat == std::string ("CS8"))
	   return 8;
	return 12;
}

void	soapyHandler::setGain (uint8_t selector, int32_t gain) {
	if (m_device == nullptr)
	   return;
	m_device -> setGain (SOAPY_SDR_RX, 0, gainsList [selector],
	                                              (float)gain);
}

void	soapyHandler::setGain_0 (int32_t gain) {
	setGain (0, gain);
}

void	soapyHandler::setGain_1 (int32_t gain) {
	setGain (1, gain);
}

void	soapyHandler::setGain_2 (int32_t gain) {
	setGain (2, gain);
}

void	soapyHandler::set_agcControl (int32_t agc) {
	(void)agc;
	if (m_device == nullptr)
	   return;
	m_device -> setGainMode (SOAPY_SDR_RX, 0,
	                          agcControl -> isChecked () ? 1 : 0);
}

void	soapyHandler::handleAntenna (const QString & name) {
	if (m_device == nullptr)
	   return;
	m_device -> setAntenna (SOAPY_SDR_RX, 0, name. toLatin1 (). data ());
}

bool	soapyHandler::isFileInput () {
	return false;
}

int32_t soapyHandler::findDesiredSamplerate (const SoapySDR::RangeList &range) {

	for (size_t i = 0; i < range. size (); i++) {
	   if ((range [i]. minimum () <= SAMPLERATE) &&
	       (SAMPLERATE <= range [i]. maximum ()))
	      return SAMPLERATE;
	}

//	No exact match, do try something
	for (size_t i = 0; i < range. size (); i++)
	   if ((SAMPLERATE < range [i]. minimum ()) &&
	       (range [i]. minimum () - SAMPLERATE < 5000000))
	      return range[i].minimum();

	for (size_t i = 0; i < range. size (); i++)
	   if ((SAMPLERATE > range [i]. maximum ()) &&
	       (SAMPLERATE - range [i]. maximum () < 100000))
	      return range[i].minimum();
	return -1;
}

int32_t	soapyHandler::findDesiredBandwidth (const SoapySDR::RangeList &range) {
	for (size_t i = 0; i < range. size (); i++) {
	   if ((range [i]. minimum () <= 1536000) &&
	       (1536000 <= range [i]. maximum ()))
	      return 1536000;
	}

//	No exact match, do try something
	for (size_t i = 0; i < range.size(); i++)
	   if (range [i]. minimum () >= 1500000)
	      return range [i]. minimum ();
	return -1;
}

void	soapyHandler::workerthread () {
int32_t flag = 0;
long long timeNS;
std::complex<float> buffer[4096];
void * const buffs [] = {buffer};

	m_running. store (true);

	m_device -> activateStream (m_stream);
	while (m_running. load ()) {
	   int32_t numRead =
	               m_device -> readStream (m_stream, buffs,
	                                        4096, flag, timeNS, 10000);
	   int skipRemaining = toSkip. load ();
	   if (skipRemaining > 0) {
	      int newSkip = skipRemaining - numRead;
	      toSkip. store (newSkip > 0 ? newSkip : 0);
	      continue;
	   }

	   if (numRead > 0) {
	      if (selectedRate == SAMPLERATE) {
	         m_sampleBuffer. putDataIntoBuffer (buffer, numRead);
	         continue;
	      }
	      theConverter. add (buffer, numRead);
	   }
	}
}

void	soapyHandler::handle_balanceIndicator (int b) {
	(void)b;
	if (m_device == nullptr)
	   return;
	m_device -> setIQBalanceMode (SOAPY_SDR_RX, 0,
	                                 balanceIndicator -> isChecked ());
}

void	soapyHandler::handle_DCModeIndicator	(int b) {
	(void)b;
	if (m_device == nullptr)
	   return;
	m_device -> setDCOffsetMode (SOAPY_SDR_RX, 0,
	                                 DCModeIndicator -> isChecked ());
}

void	soapyHandler::handle_ppmIndicator	(double v) {
	if (m_device == nullptr)
	   return;
	m_device -> setFrequencyCorrection (SOAPY_SDR_RX, 0, v);
}

void    soapyHandler::reportStatus      (const QString &s) {
        statusLabel     -> setText (s);
}

bool	soapyHandler::setup_xmlDump (bool direct) {
QString channel		= value_s (soapySettings, DAB_GENERAL,
	                                             "channel", "xx");
	try {
	   xmlWriter	= new xml_fileWriter (soapySettings,
	                                      channel,
	                                      sizeof (float) * 8,
	                                      "float32",
	                                      selectedRate,
	                                      m_freq,
	                                      -1,
	                                      selectedString,
	                                      selectedSerial,
	                                      "qt-dab",
	                                      direct);
	} catch (...) {
	   return false;
	}

	dumpButton	-> setText ("writing");
	m_dumping. store (true);
	return true;
}
	
void	soapyHandler::close_xmlDump () {
	if (xmlWriter == nullptr)	// this can happen !!
	   return;
	m_dumping. store (false);
	usleep (1000);
	xmlWriter	-> computeHeader ();
	dumpButton	-> setText ("Dump");
	delete xmlWriter;
	xmlWriter	= nullptr;
}

void	soapyHandler::handle_xmlDump () {
        if (xmlWriter == nullptr) {
           setup_xmlDump (false);
        }
        else {
           close_xmlDump ();
        }  
}

