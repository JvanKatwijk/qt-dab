#
/*
 *    Copyright (C) 2014 .. 2017
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
#include	"dab-constants.h"
#include	"unistd.h"

#include	"device-exceptions.h"
#include	"radio.h"

using namespace std;

	soapyHandler::soapyHandler (QSettings *s) :
	                                     m_sampleBuffer (8 * 1024 * 1024),
	                                     theConverter (&m_sampleBuffer) {
size_t length;

QString	deviceString;
QString	serial;
	setupUi (&myFrame);
	myFrame. show ();
//	enumerate devices
	SoapySDRKwargs *results = SoapySDRDevice_enumerate (NULL, &length);
	for (size_t i = 0; i < length; i++) {
	   fprintf (stderr, "Found device #%d: ", (int)i);
	   for (size_t j = 0; j < results[i].size; j++) {
	      fprintf (stderr, "%s = %s\n",
	                        results [i]. keys [j], results [i]. vals [j]);
	      if (QString (results [i]. keys [j]) == "driver") {
	         deviceString = results [i]. vals [j];
	         deviceNameLabel -> setText (deviceString);
	      }
	   }
	   for (size_t j = 0; j < results[i].size; j++) {
	      if (QString (results [i]. keys [j]) == "serial") {
	         serial = results [i]. vals [j];
	      }
	   }
	   break;
	}

	SoapySDRKwargsList_clear (results, length);
	if (length == 0)
	   throw device_exception ("No devices found\n");
	deviceReady. store (false);
	m_running. store (false);
	deviceNameLabel	->  setText (deviceString);
	serialNumber	->  setText (serial);
	createDevice (deviceString);
}

	soapyHandler::~soapyHandler	() {
	m_running. store (false);
	if (m_thread. joinable ()) {
	   m_thread. join();
	}

	if (m_device != nullptr) {
	   SoapySDRDevice_unmake (m_device);
	   m_device = nullptr;
	}
}

void	soapyHandler::createDevice (const QString &deviceString) {
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
	if (resultRate < 0) 
	   throw (device_exception ("no suitable samplerate found"));

	samplerateLabel	-> setText (QString::number (resultRate));
	fprintf (stderr, "resultRate = %d\n", resultRate);
	theConverter. setup (resultRate, 2048000);
	if (SoapySDRDevice_setSampleRate (m_device,
	                                  SOAPY_SDR_RX, 0, resultRate) != 0) {
	   fprintf (stderr, "setSampleRate fail: %s\n",
	                            SoapySDRDevice_lastError());
	}
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
	m_thread = std::thread(&soapyHandler::workerthread, this);
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
}

void	soapyHandler::reset () {
	m_sampleBuffer.FlushRingBuffer();
}

int32_t soapyHandler::getSamples (std::complex<float> *Buffer, int32_t Size) {
	int32_t amount = m_sampleBuffer. getDataFromBuffer (Buffer, Size);
	return amount;
}

int32_t	soapyHandler::Samples	() {
	return m_sampleBuffer. GetRingBufferReadAvailable ();
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
	return true;
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
    return m_gains.size();
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
channels.push_back(0);

	const size_t mtu	= SoapySDRDevice_getStreamMTU (m_device,
	                                                       rxStream);
	const size_t samplesToRead = mtu;
	std::vector<std::complex<float>> buf (samplesToRead);

	int frames	= 0;
	int amount	= 0;
	while (m_running. load ()) {
	   void *buffs [1];
	   buffs [0] = buf. data ();
           int flags;                   //flags set by receive operation
           long long timeNs;            //timestamp for receive buffer
           int ret = SoapySDRDevice_readStream (m_device, rxStream,
	                                        buffs, samplesToRead, &flags,
	                                        &timeNs, 100000);

	   if (ret <= 0)
	      continue;
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

//stop streaming
	SoapySDRDevice_deactivateStream (m_device, rxStream, 0, 0);
        SoapySDRDevice_closeStream (m_device, rxStream);

	m_running. store (false);
}

int	soapyHandler::findDesiredRange (SoapySDRRange * theRanges,
	                                                  int length) {
int resultrate	= -1;
	for (int i = 0; i < length; i ++) {
	   if ((theRanges [i]. minimum <= 2048000) &&
	       (2048000 <= theRanges [i]. maximum))
	      return 2048000;
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
