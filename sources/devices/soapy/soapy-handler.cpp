#
/*
 *    Copyright (C) 2026
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
#include	<QSettings>
#include	<vector>
#include	<SoapySDR/Device.h>
#include	<SoapySDR/Formats.h>

#include	"soapy-handler.h"
#include	"soapy-converter.h"
#include	"soapy-select.h"
#include	"dab-constants.h"
#include	"unistd.h"

#include        "xml-filewriter.h"
#include	"device-exceptions.h"
#include	"radio.h"

using namespace std;

	soapyHandler::soapyHandler (QSettings *s) :
	                                     m_sampleBuffer (8 * 1024 * 1024),
	                                     theConverter (&m_sampleBuffer) {
size_t length;

std::vector<int> deviceIndexTable;

	this	-> soapySettings	= s;
	setupUi (&myFrame);
	myFrame. show ();

	deviceString	= "";
	serial		= "";
	connect (&theConverter, &soapyConverter::reportStatus,
	         this, &soapyHandler::reportStatus);
//	enumerate devices
	SoapySDRKwargs *results = SoapySDRDevice_enumerate (NULL, &length);
	for (size_t i = 0; i < length; i++) {
	   fprintf (stderr, "Found device #%d: ", (int)i);
	   bool isAudio = false;
	   QString currentDriver;
	   for (size_t j = 0; j < results [i]. size; j++) {
	      fprintf (stderr, "%s = %s\n",
	                        results [i]. keys [j], results [i]. vals [j]);
	      if (QString (results [i]. keys [j]) == "driver") {
	         currentDriver = results [i]. vals [j];
	         if (currentDriver == "audio") {
	            isAudio = true;
	            fprintf (stderr, "Skipping audio device\n");
	            break;	// from "j" loop
	         }
	      }
	   }

	   if (!isAudio && !currentDriver. isEmpty ()) {
	      deviceIndexTable. push_back (i);
	      deviceString = currentDriver;
	      deviceNameLabel -> setText (deviceString);
	   }
	}

	if (deviceIndexTable. size () == 0)
	   throw device_exception ("No devices found\n");
	int selectedTableIndex	= 0;
	if (deviceIndexTable. size () > 1) {
	   soapySelect soapySelector;
	   for (auto &devIndex : deviceIndexTable) {
	      for (size_t j = 0; j < results [devIndex]. size; j++) {
                 if (QString (results [devIndex]. keys [j]) == "driver") {
	            soapySelector. addtoList (results [devIndex]. vals [j]);
	            break;
	         }
	      }
	   }
	   selectedTableIndex = soapySelector. QDialog::exec ();
	   if ((selectedTableIndex < 0) ||
	       (selectedTableIndex >= (int)deviceIndexTable. size ()))
	      selectedTableIndex = 0;
	}

	// get the actual device index from the table
	int deviceIndex	= deviceIndexTable [selectedTableIndex];
	bool isUHDDevice = false;
	for  (size_t j = 0; j < results [deviceIndex]. size; j ++) {
	   if (QString (results [deviceIndex]. keys [j]) == "driver") {
	      deviceString = QString (results [deviceIndex]. vals [j]);
//	check for UHD devices, 
	      QString driverLower = deviceString. toLower ();
	      if (driverLower. contains ("uhd") ||
	          driverLower. contains ("usrp")) {
	         isUHDDevice = true;
	         fprintf (stderr, "Detected UHD/USRP device: %s\n",
                                    deviceString. toLatin1 (). data ());
	      }
	   }
	   if (QString (results [deviceIndex]. keys [j]) == "serial") 
	      serial = results [deviceIndex]. vals [j];
	}
	SoapySDRKwargsList_clear (results, length);
	deviceReady. store (false);
	m_running. store (false);
	m_dumping. store (false);
	deviceNameLabel	->  setText (deviceString);
	serialNumber	->  setText (serial);
	xmlWriter	= nullptr;
	connect (dumpButton, &QPushButton::clicked,
	         this, &soapyHandler::handle_xmlDump);
	toSkip. store (0);
	createDevice (deviceString, isUHDDevice);
}

	soapyHandler::~soapyHandler	() {
	m_running. store (false);
	close_xmlDump	();		// if open
	if (m_thread. joinable ()) {
	   m_thread. join ();
	}

	if (m_device != nullptr) {
	   SoapySDRDevice_unmake (m_device);
	   m_device = nullptr;
	}
}

void	soapyHandler::createDevice (const QString &deviceString,
	                                            bool isUHDDevice) {
	fprintf (stderr, "going to use %s\n",
	                                  deviceString. toLatin1 (). data ());
	SoapySDRKwargs args = {};
	SoapySDRKwargs_set (&args, "driver",
	                           deviceString. toLatin1 (). data ());
	m_device	 = SoapySDRDevice_make (&args);
	SoapySDRKwargs_clear (&args);

	if (m_device == NULL) {
	   std::string ss = std::string ("SoapySDRDevice_make fail: ") +
	                                   SoapySDRDevice_lastError ();
	   throw device_exception (ss);
	}
//	query device info
	size_t length;
	char** names =
	      SoapySDRDevice_listAntennas (m_device, SOAPY_SDR_RX, 0, &length);
	fprintf (stderr, "Rx antennas: ");
	for (size_t i = 0; i < length; i++)
	   fprintf (stderr, "%s, ", names[i]);
	printf ("\n");
	SoapySDRStrings_clear (&names, length);
//
//	about gain
	SoapySDRRange gainRange = SoapySDRDevice_getGainRange (m_device,
	                                               SOAPY_SDR_RX, 0);
	gainSelector -> setRange ((int)gainRange. minimum,
	                          (int)gainRange. maximum);
	double currentGain =
	             (int) (gainRange. maximum + gainRange. minimum) / 2;
	currentGain	= gainRange. minimum + currentGain;
	try {
	   SoapySDRDevice_setGain (m_device, SOAPY_SDR_RX, 0, currentGain);
	}
	catch (const out_of_range&) {
	   fprintf (stderr, "Soapy gain %d is out of range\n",
	                                              (int)currentGain);
	}
	gainSelector -> setValue ((int)currentGain);
	connect (gainSelector, qOverload<int>(&QSpinBox::valueChanged),
	         this, &soapyHandler::setGain);

	hasAgc	= SoapySDRDevice_hasGainMode (m_device, SOAPY_SDR_RX, 0);
	if (hasAgc) 
	   SoapySDRDevice_setGainMode (m_device, SOAPY_SDR_RX, 0, false); 
#if QT_VERSION >= QT_VERSION_CHECK (6, 7, 0)
	connect (agcControl, &QCheckBox::checkStateChanged,
#else
	connect (agcControl, &QCheckBox::stateChanged,
#endif
	         this, &soapyHandler::setAgc);

	SoapySDRRange *ranges =
	       SoapySDRDevice_getFrequencyRange (m_device, SOAPY_SDR_RX, 0, &length);
	fprintf (stderr, "Rx freq ranges: ");
	for (size_t i = 0; i < length; i++)
	   fprintf (stderr, "[%g Hz -> %g Hz], ",
	                        ranges[i].minimum, ranges[i].maximum);
	printf ("\n");
	free (ranges);

	SoapySDRRange *sampleRange =
	           SoapySDRDevice_getSampleRateRange (m_device, SOAPY_SDR_RX,
	                                              0, &length);
	int resultRate = findDesiredRange (sampleRange, length);
	
	if (isUHDDevice) {
	   fprintf (stderr, "UHD/USRP device detected, forcing sample rate to 2048000 Hz\n");
           resultRate = 2048000;
	}

	if (resultRate < 0) 
	   throw (device_exception ("no suitable samplerate found"));

	samplerateLabel	-> setText (QString::number (resultRate));
	fprintf (stderr, "Requesting samplerate  %d\n", resultRate);
//	
//	set the samplerate
	if (SoapySDRDevice_setSampleRate (m_device,
	                                  SOAPY_SDR_RX, 0, resultRate) != 0) {
	   std::string errorMsg = 
	              std::string ("Failed to set samplerate: ") +
	                                   SoapySDRDevice_lastError ();
	   fprintf (stderr, "%s\n", errorMsg. c_str ());
	   throw device_exception (errorMsg);
	}
//
//	  // Verify the actual sample rate that was set
        actualRate = SoapySDRDevice_getSampleRate (m_device,
	                                                  SOAPY_SDR_RX, 0);
        fprintf (stderr, "Actual sample rate set = %.0f Hz\n", actualRate);

//	For DAB, we need exactly 2048000 Hz (or very close)
        const int desiredRate = 2048000;
        const double tolerance = 500000.0;

        if (fabs (actualRate - desiredRate) > tolerance) {
           fprintf (stderr, "Warning: Sample rate %.0f Hz differs from desired %d Hz\n",
                    actualRate, desiredRate);
	   resultRate = (int)actualRate;
	   samplerateLabel -> setText (QString::number (resultRate));
        }

        theConverter. setup (resultRate, 2048000);

	const bool automatic = true;
	SoapySDRDevice_setFrequency (m_device, SOAPY_SDR_RX, 0,
	                                              220000000, NULL);

	rxStream =
              SoapySDRDevice_setupStream (m_device, SOAPY_SDR_RX,
                                          SOAPY_SDR_CF32, NULL, 0, NULL);
        if (rxStream == nullptr)
           throw (device_exception ("cannot open stream"));

        int xx = SoapySDRDevice_activateStream (m_device, rxStream, 0, 0, 0);
        if (xx != 0)
           throw (device_exception ("cannot activate stream"));

	m_running . store (true);
	m_sw_agc	= true;
	deviceReady	= true;
	statusLabel	-> setText ("running");
	m_thread	= std::thread (&soapyHandler::workerthread, this);
}

bool	soapyHandler::restartReader (int frequency, int skipped) {
	if (!deviceReady)
	   return false;
	m_sampleBuffer. FlushRingBuffer ();
	m_freq	= frequency;
	toSkip	= skipped;
	SoapySDRDevice_setFrequency (m_device, SOAPY_SDR_RX,
	                                          0, frequency, NULL);
	return true;
}

void	soapyHandler::stopReader	() {
	reset ();
}

void	soapyHandler::reset () {
	m_sampleBuffer.FlushRingBuffer();
	theConverter. reset ();
}

int32_t soapyHandler::getSamples (std::complex<float> *Buffer, int32_t size) {
int32_t available = m_sampleBuffer. GetRingBufferReadAvailable ();
	uint32_t amount = m_sampleBuffer. getDataFromBuffer (Buffer,
	                                                    (uint32_t)size);
	if (m_dumping. load ())
	   xmlWriter	-> add (Buffer, amount);
	return amount;
}

int32_t	soapyHandler::Samples	() {
	return  m_sampleBuffer. GetRingBufferReadAvailable ();
}

void	soapyHandler::setGain	(int32_t gainValue) {
	if ((m_device != nullptr)  && (!agcControl -> isChecked ())) {
           try {
               SoapySDRDevice_setGain (m_device, SOAPY_SDR_RX, 0, gainValue);
           }
           catch (const out_of_range&) {
              fprintf (stderr,  "Soapy gain %d is out of range\n", gainValue);
	   }
	}
}

bool	soapyHandler::isFileInput	() {
	return false;
}

void	soapyHandler::setAntenna (const std::string& antenna) {
//        try {
//            SoapySDRDevice_setAntenna (m_device, SOAPY_SDR_RX, 0, antenna);
//            m_antenna = antenna;
//        }
//        catch (...) {
//            fprintf (stderr, "Could not set antenna to %s\n",
//	                                             antenna. c_str ());
//        }
}

void	soapyHandler::increaseGain () {
	if (m_device != nullptr) {
        float current_gain = SoapySDRDevice_getGain (m_device, SOAPY_SDR_RX, 0);
        for (const float g : m_gains) {
            if (g > current_gain) {
                SoapySDRDevice_setGain (m_device, SOAPY_SDR_RX, 0, g);
                break;
            }
        }
    }
}

void	soapyHandler::decreaseGain () {
	if (m_device != nullptr) {
        float current_gain = SoapySDRDevice_getGain (m_device, SOAPY_SDR_RX, 0);
        for (auto it = m_gains.rbegin(); it != m_gains.rend(); ++it) {
            if (*it > current_gain) {
                SoapySDRDevice_setGain (m_device, SOAPY_SDR_RX, 0, *it);
                break;
            }
        }
    }
}

int32_t	soapyHandler::getGainCount () {
    return m_gains.size ();
}

void	soapyHandler::setAgc (int status) {
bool b	= agcControl -> isChecked ();
	if (hasAgc) {
	   SoapySDRDevice_setGainMode (m_device, SOAPY_SDR_RX, 0, b);
	   gainSelector -> setEnabled (!b);
	}
	else
	   m_sw_agc = b;
}

void	soapyHandler::workerthread () {
std::vector<size_t> channels;
channels. push_back (0);

	const size_t mtu	= SoapySDRDevice_getStreamMTU (m_device,
	                                                       rxStream);
	const size_t samplesToRead = mtu;
	std::vector<std::complex<float>> buf (samplesToRead);

	int frames	= 0;
	int amount	= 0;
	int totalSamplesRead = 0;
	int errorCount = 0;
        fprintf (stderr, "soapyHandler: Worker thread started, MTU = %zu samples\n", mtu);

	while (m_running. load ()) {
	   void *buffs [1];
	   buffs [0] = buf. data ();
           int flags;                   //flags set by receive operation
           long long timeNs;            //timestamp for receive buffer
           int ret = SoapySDRDevice_readStream (m_device, rxStream,
	                                        buffs, samplesToRead, &flags,
	                                        &timeNs, 100000);
	   if (ret <= 0) {
	      if (ret < 0) {
	         errorCount++;
	         if (errorCount % 100 == 0) {
	            fprintf (stderr,
	                  "soapyHandler: readStream errors: %d (ret=%d)\n", errorCount, ret);
	         }
	      }
              continue;
	   }

	   int skipRemaining = toSkip. load ();
	   if (skipRemaining > 0) {
	      int newSkip = skipRemaining - ret;
	      toSkip. store (newSkip > 0 ? newSkip : 0);
	      continue;
           }

	   theConverter. add (buf. data (), ret);
	   frames ++;
	   if (m_sw_agc and (frames >= 200)) {
	      frames = 0;
	      float maxnorm = 0;
	      for (auto z : buf) {
	         if (norm (z) > maxnorm) {
	            maxnorm = norm (z);
	         }
	      }
	      const float maxampl = sqrt (maxnorm);
	      if (maxampl > 0.5f) {
	         decreaseGain ();
	      }
	      else
	      if (maxampl < 0.1f) {
	         increaseGain ();
	      }
	   }
	}
	fprintf (stderr,
	         "soapyHandler: Worker thread stopping, total samples read: %d\n", totalSamplesRead);


//stop streaming
	SoapySDRDevice_deactivateStream (m_device, rxStream, 0, 0);
        SoapySDRDevice_closeStream (m_device, rxStream);

	m_running. store (false);
}

int	soapyHandler::findDesiredRange (SoapySDRRange *theRanges,
	                                                  int length) {
const int desiredRate	= 2048000;

	for (int i = 0; i < length; i ++) {
	   fprintf (stderr, "samplerate range (min-max) %g -> %g\n",
	                         theRanges [i]. minimum,
	                         theRanges [i]. maximum);
	   if ((theRanges [i]. minimum <= desiredRate) &&
	       (desiredRate <= theRanges [i]. maximum))
	      return desiredRate;
	}

//	No exact match, do try something
	for (int i = 0; i < length; i ++)
	   if ((2048000 < theRanges [i]. minimum) &&
	       (theRanges [i]. minimum - 2048000 < 5000000))
	      return theRanges [i]. minimum;

	for (int i = 0; i < length; i ++)
	   if ((2048000 > theRanges [i]. maximum) &&
	      (2048000 - theRanges [i]. maximum < 100000))
	      return theRanges [i]. minimum;
	return -1;
}

void	soapyHandler::reportStatus	(const QString &s) {
	statusLabel	-> setText (s);
}


bool	soapyHandler::setup_xmlDump (bool direct) {
QString channel		= value_s (soapySettings, DAB_GENERAL,
	                                             "channel", "xx");
	try {
	   xmlWriter	= new xml_fileWriter (soapySettings,
	                                      channel,
	                                      32,
	                                      "float32",
	                                      actualRate,
	                                      m_freq,
	                                      -1,
	                                      deviceString,
	                                      serial,
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
