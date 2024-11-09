#
/*
 *    Copyright (C) 2014 .. 2023
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
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	<QTime>
#include	"lime-handler.h"
#include	"position-handler.h"
#include	"xml-filewriter.h"
#include	"device-exceptions.h"
#include	"logger.h"
#include	"settingNames.h"
#include	"settings-handler.h"

#define	FIFO_SIZE	32768
#define	LIME_SETTINGS	"LIME_SETTINGS"

static
int16_t localBuffer [4 * FIFO_SIZE];
lms_info_str_t limedevices [10];

	limeHandler::limeHandler (QSettings *s,
	                          const QString &recorderVersion,
	                          logger	*theLogger): // dummy for now
	                             _I_Buffer (4* 1024 * 1024),
	                             theFilter (5, 1560000 / 2, 2048000) {
	this	-> limeSettings		= s;
	this	-> recorderVersion	= recorderVersion;
	setupUi (&myFrame);
	set_position_and_size (s, &myFrame, LIME_SETTINGS);
	myFrame. show	();

	filtering	= false;

	currentDepth	= value_i (limeSettings, LIME_SETTINGS,
	                                                "filterDepth", 5);
	filterDepth	-> setValue (currentDepth);
	theFilter. resize (currentDepth);
#ifdef  __MINGW32__
        const char *libraryString = "LimeSuite.dll";
        Handle          = LoadLibrary ((wchar_t *)L"LimeSuite.dll");
#elif  __clang__
        const char *libraryString = "/opt/local/lib/libLimeSuite.dylib";
        Handle		= dlopen (libraryString, RTLD_NOW);
#else
        const char *libraryString = "libLimeSuite.so";
        Handle          = dlopen (libraryString, RTLD_NOW);
#endif

        if (Handle == nullptr) {
           throw (device_exception ("failed to open " +
	                             std::string (libraryString)));
        }

        libraryLoaded   = true;
	if (!load_limeFunctions()) {
#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
           dlclose (Handle);
#endif
           throw (device_exception ("could not load all required lib functions"));
        }
//
//      From here we have a library available
	int ndevs	= LMS_GetDeviceList (limedevices);
	if (ndevs == 0) {	// no devices found
	   throw (device_exception ("No lime device found"));
	}

	for (int i = 0; i < ndevs; i ++)
	   fprintf (stderr, "device %s\n", limedevices [i]);

	int res		= LMS_Open (&theDevice, nullptr, nullptr);
	if (res < 0) {	// some error
	   throw (device_exception ("failed to open device"));
	}

	res		= LMS_Init (theDevice);
	if (res < 0) {	// some error
	   LMS_Close (&theDevice);
	   throw (device_exception ("failed to initialize device"));
	}

	res		= LMS_GetNumChannels (theDevice, LMS_CH_RX);
	if (res < 0) {	// some error
	   LMS_Close (&theDevice);
	   throw (device_exception ("could not set number of channels"));
	}

	fprintf (stderr, "device %s supports %d channels\n",
	                            limedevices [0], res);
	res		= LMS_EnableChannel (theDevice, LMS_CH_RX, 0, true);
	if (res < 0) {	// some error
	   LMS_Close (theDevice);
	   throw (device_exception ("could not enable channels"));
	}

	res	= LMS_SetSampleRate (theDevice, 2048000.0, 0);
	if (res < 0) {
	   LMS_Close (theDevice);
	   throw (device_exception ("could not set samplerate"));
	}

	float_type host_Hz, rf_Hz;
	res	= LMS_GetSampleRate (theDevice, LMS_CH_RX, 0,
	                                            &host_Hz, &rf_Hz);

	fprintf (stderr, "samplerate = %f %f\n", (float)host_Hz, (float)rf_Hz);
	
	res		= LMS_GetAntennaList (theDevice, LMS_CH_RX, 0, antennas);
	for (int i = 0; i < res; i ++) 	
	   antennaList	-> addItem (QString (antennas [i]));

	QString antenne	=
	           value_s (limeSettings, LIME_SETTINGS,
	                                "antenna", "default");
	save_gainSettings	=
	          value_i (limeSettings, LIME_SETTINGS, 
	                                 "save_gainSettings", 1) != 0;

	int k       = antennaList -> findText (antenne);
        if (k != -1) 
           antennaList -> setCurrentIndex (k);
	connect (antennaList, qOverload<int>(&QComboBox::activated),
	         this, &limeHandler::setAntenna);

//	default antenna setting
	res		= LMS_SetAntenna (theDevice, LMS_CH_RX, 0, 
	                           antennaList -> currentIndex());

//	default frequency
	res		= LMS_SetLOFrequency (theDevice, LMS_CH_RX,
	                                                 0, 220000000.0);
	if (res < 0) {
	   LMS_Close (theDevice);
	   throw (device_exception ("could not set LO frequency"));
	}

	res		= LMS_SetLPFBW (theDevice, LMS_CH_RX,
	                                               0, 1536000.0);
	if (res < 0) {
	   LMS_Close (theDevice);
	   throw (device_exception ("could not set bandwidth"));
	}

	LMS_SetGaindB (theDevice, LMS_CH_RX, 0, 50);

	LMS_Calibrate (theDevice, LMS_CH_RX, 0, 2500000.0, 0);
	
	
	k	=  value_i (limeSettings, LIME_SETTINGS, "gain", 50);
	gainSelector -> setValue (k);
	setGain (k);
	connect (gainSelector, qOverload<int>(&QSpinBox::valueChanged),
	         this, &limeHandler::setGain);
	connect (dumpButton, &QPushButton::clicked,
	         this, &limeHandler::set_xmlDump);
	connect (this, &limeHandler::new_gainValue,
	         gainSelector, &QSpinBox::setValue);
	connect (filterSelector, &QCheckBox::stateChanged,
	         this, &limeHandler::set_filter);
	dumping. store (false);
	running. store (false);
	xmlWriter	= nullptr;
}

	limeHandler::~limeHandler() {
	stopReader ();
	running. store (false);
	while (isRunning())
	   usleep (100);
	myFrame. hide ();
	store_widget_position (limeSettings, &myFrame, LIME_SETTINGS);
	QString currentText = antennaList -> currentText ();
	store (limeSettings, LIME_SETTINGS, "antenna", currentText);
	store (limeSettings, LIME_SETTINGS, "gain", gainSelector -> value());
	store (limeSettings, LIME_SETTINGS,
	                      "filterDepth", filterDepth -> value ());
	LMS_Close (theDevice);
}

void	limeHandler::setGain		(int g) {
float_type gg;
	LMS_SetGaindB (theDevice, LMS_CH_RX, 0, g);
	LMS_GetNormalizedGain (theDevice, LMS_CH_RX, 0, &gg);
	actualGain	-> display (gg);
}

void	limeHandler::setAntenna		(int ind) {
	(void)LMS_SetAntenna (theDevice, LMS_CH_RX, 0, ind);
}

void	limeHandler::set_filter		(int c) {
	(void)c;
	filtering	= filterSelector -> isChecked ();
	fprintf (stderr, "filter set %s\n", filtering ? "on" : "off");
}

bool	limeHandler::restartReader	(int32_t freq) {
int	res;

	if (isRunning())
	   return true;

	lastFrequency	= freq;
	if (save_gainSettings) {
	   update_gainSettings	(freq / MHz (1));
	   setGain (gainSelector -> value ());
	}
	LMS_SetLOFrequency (theDevice, LMS_CH_RX, 0, freq);
	stream. isTx            = false;
        stream. channel         = 0;
        stream. fifoSize        = FIFO_SIZE;
        stream. throughputVsLatency     = 0.1;  // ???
        stream. dataFmt         = lms_stream_t::LMS_FMT_I12;    // 12 bit ints

	res     = LMS_SetupStream (theDevice, &stream);
        if (res < 0)
           return false;
        res     = LMS_StartStream (&stream);
        if (res < 0)
           return false;

	start ();
	return true;
}
	
void	limeHandler::stopReader	() {
	close_xmlDump ();
	if (!isRunning())
	   return;
	if (save_gainSettings)
	   record_gainSettings (lastFrequency);

	running. store (false);
	while (isRunning())
	   usleep (200);
	(void)LMS_StopStream	(&stream);	
	(void)LMS_DestroyStream	(theDevice, &stream);
}

int	limeHandler::getSamples	(std::complex<float> *V, int32_t size) {
auto *temp	= dynVec (std::complex<int16_t>, size);

        int amount      = _I_Buffer. getDataFromBuffer (temp, size);
	if (filtering) {
	   if (filterDepth -> value () != currentDepth) {
	      currentDepth = filterDepth -> value ();
	      theFilter. resize (currentDepth);
	   }
           for (int i = 0; i < amount; i ++) 
	      V [i] = theFilter. Pass (std::complex<float> (
	                                         real (temp [i]) / 2048.0,
	                                         imag (temp [i]) / 2048.0));
	}
	else
           for (int i = 0; i < amount; i ++)
              V [i] = std::complex<float> (real (temp [i]) / 2048.0,
                                           imag (temp [i]) / 2048.0);
        if (dumping. load ())
           xmlWriter -> add (temp, amount);
        return amount;
}

int	limeHandler::Samples() {
	return _I_Buffer. GetRingBufferReadAvailable();
}

void	limeHandler::resetBuffer() {
	_I_Buffer. FlushRingBuffer();
}

int16_t	limeHandler::bitDepth() {
	return 12;
}

QString	limeHandler::deviceName	() {
	return "limeSDR";
}

void	limeHandler::showErrors		(int underrun, int overrun) {
	underrunDisplay	-> display (underrun);
	overrunDisplay	-> display (overrun);
}


void	limeHandler::run() {
int	res;
lms_stream_status_t streamStatus;
int	underruns	= 0;
int	overruns	= 0;
int	amountRead	= 0;

	running. store (true);
	while (running. load()) {
	   res = LMS_RecvStream (&stream, localBuffer,
	                                     FIFO_SIZE,  &meta, 1000);
	   if (res > 0) {
	      _I_Buffer. putDataIntoBuffer (localBuffer, res);
	      amountRead	+= res;
	      res	= LMS_GetStreamStatus (&stream, &streamStatus);
	      underruns	+= streamStatus. underrun;
	      overruns	+= streamStatus. overrun;
	   }
	   if (amountRead > 4 * 2048000) {
	      amountRead = 0;
	      showErrors (underruns, overruns);
	      underruns	= 0;
	      overruns	= 0;
	   }
	}
}

bool	limeHandler::load_limeFunctions() {

	this	-> LMS_GetDeviceList = (pfn_LMS_GetDeviceList)
	                    GETPROCADDRESS (Handle, "LMS_GetDeviceList");
	if (this -> LMS_GetDeviceList == nullptr) {
	   fprintf (stderr, "could not find LMS_GetdeviceList\n");
	   return false;
	}
	this	-> LMS_Open = (pfn_LMS_Open)
	                    GETPROCADDRESS (Handle, "LMS_Open");
	if (this -> LMS_Open == nullptr) {
	   fprintf (stderr, "could not find LMS_Open\n");
	   return false;
	}
	this	-> LMS_Close = (pfn_LMS_Close)
	                    GETPROCADDRESS (Handle, "LMS_Close");
	if (this -> LMS_Close == nullptr) {
	   fprintf (stderr, "could not find LMS_Close\n");
	   return false;
	}
	this	-> LMS_Init = (pfn_LMS_Init)
	                    GETPROCADDRESS (Handle, "LMS_Init");
	if (this -> LMS_Init == nullptr) {
	   fprintf (stderr, "could not find LMS_Init\n");
	   return false;
	}
	this	-> LMS_GetNumChannels = (pfn_LMS_GetNumChannels)
	                    GETPROCADDRESS (Handle, "LMS_GetNumChannels");
	if (this -> LMS_GetNumChannels == nullptr) {
	   fprintf (stderr, "could not find LMS_GetNumChannels\n");
	   return false;
	}
	this	-> LMS_EnableChannel = (pfn_LMS_EnableChannel)
	                    GETPROCADDRESS (Handle, "LMS_EnableChannel");
	if (this -> LMS_EnableChannel == nullptr) {
	   fprintf (stderr, "could not find LMS_EnableChannel\n");
	   return false;
	}
	this	-> LMS_SetSampleRate = (pfn_LMS_SetSampleRate)
	                    GETPROCADDRESS (Handle, "LMS_SetSampleRate");
	if (this -> LMS_SetSampleRate == nullptr) {
	   fprintf (stderr, "could not find LMS_SetSampleRate\n");
	   return false;
	}
	this	-> LMS_GetSampleRate = (pfn_LMS_GetSampleRate)
	                    GETPROCADDRESS (Handle, "LMS_GetSampleRate");
	if (this -> LMS_GetSampleRate == nullptr) {
	   fprintf (stderr, "could not find LMS_GetSampleRate\n");
	   return false;
	}
	this	-> LMS_SetLOFrequency = (pfn_LMS_SetLOFrequency)
	                    GETPROCADDRESS (Handle, "LMS_SetLOFrequency");
	if (this -> LMS_SetLOFrequency == nullptr) {
	   fprintf (stderr, "could not find LMS_SetLOFrequency\n");
	   return false;
	}
	this	-> LMS_GetLOFrequency = (pfn_LMS_GetLOFrequency)
	                    GETPROCADDRESS (Handle, "LMS_GetLOFrequency");
	if (this -> LMS_GetLOFrequency == nullptr) {
	   fprintf (stderr, "could not find LMS_GetLOFrequency\n");
	   return false;
	}
	this	-> LMS_GetAntennaList = (pfn_LMS_GetAntennaList)
	                    GETPROCADDRESS (Handle, "LMS_GetAntennaList");
	if (this -> LMS_GetAntennaList == nullptr) {
	   fprintf (stderr, "could not find LMS_GetAntennaList\n");
	   return false;
	}
	this	-> LMS_SetAntenna = (pfn_LMS_SetAntenna)
	                    GETPROCADDRESS (Handle, "LMS_SetAntenna");
	if (this -> LMS_SetAntenna == nullptr) {
	   fprintf (stderr, "could not find LMS_SetAntenna\n");
	   return false;
	}
	this	-> LMS_GetAntenna = (pfn_LMS_GetAntenna)
	                    GETPROCADDRESS (Handle, "LMS_GetAntenna");
	if (this -> LMS_GetAntenna == nullptr) {
	   fprintf (stderr, "could not find LMS_GetAntenna\n");
	   return false;
	}
	this	-> LMS_GetAntennaBW = (pfn_LMS_GetAntennaBW)
	                    GETPROCADDRESS (Handle, "LMS_GetAntennaBW");
	if (this -> LMS_GetAntennaBW == nullptr) {
	   fprintf (stderr, "could not find LMS_GetAntennaBW\n");
	   return false;
	}
	this	-> LMS_SetNormalizedGain = (pfn_LMS_SetNormalizedGain)
	                    GETPROCADDRESS (Handle, "LMS_SetNormalizedGain");
	if (this -> LMS_SetNormalizedGain == nullptr) {
	   fprintf (stderr, "could not find LMS_SetNormalizedGain\n");
	   return false;
	}
	this	-> LMS_GetNormalizedGain = (pfn_LMS_GetNormalizedGain)
	                    GETPROCADDRESS (Handle, "LMS_GetNormalizedGain");
	if (this -> LMS_GetNormalizedGain == nullptr) {
	   fprintf (stderr, "could not find LMS_GetNormalizedGain\n");
	   return false;
	}
	this	-> LMS_SetGaindB = (pfn_LMS_SetGaindB)
	                    GETPROCADDRESS (Handle, "LMS_SetGaindB");
	if (this -> LMS_SetGaindB == nullptr) {
	   fprintf (stderr, "could not find LMS_SetGaindB\n");
	   return false;
	}
	this	-> LMS_GetGaindB = (pfn_LMS_GetGaindB)
	                    GETPROCADDRESS (Handle, "LMS_GetGaindB");
	if (this -> LMS_GetGaindB == nullptr) {
	   fprintf (stderr, "could not find LMS_GetGaindB\n");
	   return false;
	}
	this	-> LMS_SetLPFBW = (pfn_LMS_SetLPFBW)
	                    GETPROCADDRESS (Handle, "LMS_SetLPFBW");
	if (this -> LMS_SetLPFBW == nullptr) {
	   fprintf (stderr, "could not find LMS_SetLPFBW\n");
	   return false;
	}
	this	-> LMS_GetLPFBW = (pfn_LMS_GetLPFBW)
	                    GETPROCADDRESS (Handle, "LMS_GetLPFBW");
	if (this -> LMS_GetLPFBW == nullptr) {
	   fprintf (stderr, "could not find LMS_GetLPFBW\n");
	   return false;
	}
	this	-> LMS_Calibrate = (pfn_LMS_Calibrate)
	                    GETPROCADDRESS (Handle, "LMS_Calibrate");
	if (this -> LMS_Calibrate == nullptr) {
	   fprintf (stderr, "could not find LMS_Calibrate\n");
	   return false;
	}
	this	-> LMS_SetupStream = (pfn_LMS_SetupStream)
	                    GETPROCADDRESS (Handle, "LMS_SetupStream");
	if (this -> LMS_SetupStream == nullptr) {
	   fprintf (stderr, "could not find LMS_SetupStream\n");
	   return false;
	}
	this	-> LMS_DestroyStream = (pfn_LMS_DestroyStream)
	                    GETPROCADDRESS (Handle, "LMS_DestroyStream");
	if (this -> LMS_DestroyStream == nullptr) {
	   fprintf (stderr, "could not find LMS_DestroyStream\n");
	   return false;
	}
	this	-> LMS_StartStream = (pfn_LMS_StartStream)
	                    GETPROCADDRESS (Handle, "LMS_StartStream");
	if (this -> LMS_StartStream == nullptr) {
	   fprintf (stderr, "could not find LMS_StartStream\n");
	   return false;
	}
	this	-> LMS_StopStream = (pfn_LMS_StopStream)
	                    GETPROCADDRESS (Handle, "LMS_StopStream");
	if (this -> LMS_StopStream == nullptr) {
	   fprintf (stderr, "could not find LMS_StopStream\n");
	   return false;
	}
	this	-> LMS_RecvStream = (pfn_LMS_RecvStream)
	                    GETPROCADDRESS (Handle, "LMS_RecvStream");
	if (this -> LMS_RecvStream == nullptr) {
	   fprintf (stderr, "could not find LMS_RecvStream\n");
	   return false;
	}
	this	-> LMS_GetStreamStatus = (pfn_LMS_GetStreamStatus)
	                    GETPROCADDRESS (Handle, "LMS_GetStreamStatus");
	if (this -> LMS_GetStreamStatus == nullptr) {
	   fprintf (stderr, "could not find LMS_GetStreamStatus\n");
	   return false;
	}

	return true;
}

void	limeHandler::set_xmlDump () {
	if (xmlWriter == nullptr) {
	   setup_xmlDump ();
	}
	else {
	   close_xmlDump ();
	}
}

static inline
bool	isValid (QChar c) {
	return c. isLetterOrNumber () || (c == '-');
}

bool	limeHandler::setup_xmlDump () {
QString channel		= value_s (limeSettings, DAB_GENERAL,
	                                       "channel", "xx");
	xmlWriter	= nullptr;
	try {
	   xmlWriter	= new xml_fileWriter (limeSettings,
	                                      channel,
	                                      bitDepth (),
	                                      "int16",
	                                      2048000,
	                                      lastFrequency,
	                                      theGain,
	                                      "LimeSDR",
	                                      "???",
	                                      recorderVersion);
	} catch (...) {
	   return false;
	}
	dumpButton	-> setText ("writing");
	dumping. store (true);
	return true;
}
	
void	limeHandler::close_xmlDump () {
	if (xmlWriter == nullptr)	// this can happen !!
	   return;
	usleep (1000);
	xmlWriter	-> computeHeader ();
	delete xmlWriter;
	dumping. store (false);
	dumpButton	-> setText ("Dump");
	xmlWriter	= nullptr;
}

void	limeHandler::record_gainSettings	(int key) {
int gainValue	= gainSelector -> value ();
QString theValue	= QString::number (gainValue);

        store (limeSettings, LIME_SETTINGS, QString::number (key), theValue);
}

void	limeHandler::update_gainSettings	(int key) {
int	gainValue;

        gainValue	= value_i (limeSettings, LIME_SETTINGS, 
	                            QString::number (key), -1);

	if (gainValue == -1)
	   return;

	gainSelector	-> blockSignals (true);
	new_gainValue (gainValue);
	while (gainSelector -> value () != gainValue)
	   usleep (1000);
	actualGain	-> display (gainValue);
	gainSelector	-> blockSignals (false);
}

