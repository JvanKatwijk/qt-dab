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
#include	"errorlog.h"
#include	"settingNames.h"
#include	"settings-handler.h"

#define	FIFO_SIZE	32768
#define	LIME_SETTINGS	"LIME_SETTINGS"

static
int16_t localBuffer [4 * FIFO_SIZE];
lms_info_str_t limedevices [10];

	limeHandler::limeHandler (QSettings *s,
	                          const QString &recorderVersion,
	                          errorLogger	*theLogger):
	                             _I_Buffer (4 * 1024 * 1024),
	                             theFilter (5, 1560000 / 2, 2048000) {
	this	-> limeSettings		= s;
	this	-> recorderVersion	= recorderVersion;
	this	-> theErrorLogger	= theLogger;
	setupUi (&myFrame);
	setPositionAndSize (s, &myFrame, LIME_SETTINGS);
	myFrame. show	();

	myFrame. setWindowTitle ("LimeSDR control");
	filtering	= false;

	currentDepth	= value_i (limeSettings, LIME_SETTINGS,
	                                                "filterDepth", 5);
	filterDepth	-> setValue (currentDepth);
	theFilter. resize (currentDepth);
#ifdef  __MINGW32__
        const char *libraryString = "LimeSuite.dll";
#elif  __clang__
        const char *libraryString = "/opt/local/lib/libLimeSuite.dylib";
#else
        const char *libraryString = "libLimeSuite.so";
#endif
	
        library_p	= new QLibrary (libraryString);
        library_p	-> load ();
        if (!library_p -> isLoaded ()) {
           throw (device_exception ("failed to open " +
                                        std::string (libraryString)));
        }

        if (!load_limeFunctions ()) {
           delete library_p;
           throw (device_exception ("could not find one or more library functions"));
        }

	libraryLoaded	= true;
//      From here we have a library available
	int ndevs	= LMS_GetDeviceList (limedevices);
	if (ndevs == 0) 	// no devices found
	   throw (device_exception ("No lime device found"));
	else
	if (ndevs < 0) {
	   const char *error = LMS_GetLastErrorMessage ();
	   throw (device_exception (error));
	}

	for (int i = 0; i < ndevs; i ++)
	   fprintf (stderr, "device %s\n", limedevices [i]);

	int res		= LMS_Open (&theDevice, nullptr, nullptr);
	if (res < 0) {	// some error
	   const char * error = LMS_GetLastErrorMessage ();
	   throw (device_exception (error));
	}

	nameOfDevice	-> setText (limedevices [0]);
	res		= LMS_Init (theDevice);
	if (res < 0) {	// some error
	   const char * error = LMS_GetLastErrorMessage ();
	   LMS_Close (&theDevice);
	   throw (device_exception (error));
	}

	res		= LMS_GetNumChannels (theDevice, LMS_CH_RX);
	if (res == 0) {	// no channels
	   throw (device_exception ("could not set number of channels"));
	   LMS_Close (&theDevice);
	}
	if (res < 0) {	// some error
	   const char * error = LMS_GetLastErrorMessage ();
	   LMS_Close (&theDevice);
	   throw (device_exception (error));
	}

	fprintf (stderr, "device %s supports %d channels\n",
	                            limedevices [0], res);
	res		= LMS_EnableChannel (theDevice, LMS_CH_RX, 0, true);
	if (res != 0) {	// some error
	   const char * error = LMS_GetLastErrorMessage ();
	   LMS_Close (theDevice);
	   throw (device_exception (error));
	}

	res	= LMS_SetSampleRate (theDevice, 2048000.0, 1);
	if (res != 0) {
	   const char * error = LMS_GetLastErrorMessage ();
	   LMS_Close (theDevice);
	   throw (device_exception (error));
	}

	float_type host_Hz, rf_Hz;
	res	= LMS_GetSampleRate (theDevice, LMS_CH_RX, 0,
	                                            &host_Hz, &rf_Hz);
	if (res != 0) {
	   const char * error = LMS_GetLastErrorMessage ();
	   LMS_Close (theDevice);
	   throw (device_exception (error));
	}

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
	                           antennaList -> currentIndex ());
	if (res != 0) {
	   const char * error = LMS_GetLastErrorMessage ();
	   LMS_Close (theDevice);
	   throw (device_exception (error));
	}

//	default frequency
	res		= LMS_SetLOFrequency (theDevice, LMS_CH_RX,
	                                                 0, 220000000.0);
	if (res < 0) {
	   const char * error = LMS_GetLastErrorMessage ();
	   LMS_Close (theDevice);
	   throw (device_exception (error));
	}
//
	lms_range_t	range;
	res		= LMS_GetLPFBWRange (theDevice, LMS_CH_RX, &range);
	if (res < 0) {
	   const char * error = LMS_GetLastErrorMessage ();
	   LMS_Close (theDevice);
	   throw (device_exception (error));
	}

	double	bandWidth	= 1536000;
	fprintf (stderr, "range %f - %f\n",
	                   (float)range. min, (float)range. max);
	if (bandWidth < range. min)
	   bandWidth = range. min + 100;
	res		= LMS_SetLPFBW (theDevice, LMS_CH_RX,
	                                0, bandWidth);
	if (res < 0) {
	   const char * error = LMS_GetLastErrorMessage ();
	   LMS_Close (theDevice);
	   throw (device_exception (error));
	}

	res		= LMS_SetGFIRLPF (theDevice, LMS_CH_RX,
	                                  0, true,  bandWidth);
	if (res < 0) {
	   const char * error = LMS_GetLastErrorMessage ();
	   LMS_Close (theDevice);
	   throw (device_exception (error));
	}
	res		= LMS_GetLPFBW (theDevice, LMS_CH_RX, 0, &bandWidth);
	fprintf (stderr, "BW set to %f\n", (float)bandWidth);

	int gaindB	= 73;
	gainSelector -> setMaximum (gaindB);
	bandWidth	= 2500000;;
//	Calibrating
	LMS_Calibrate (theDevice, LMS_CH_RX, 0, bandWidth, 0);
	if (res < 0) {
	   const char * error = LMS_GetLastErrorMessage ();
	   LMS_Close (theDevice);
	   throw (device_exception (error));
	}
	
	k	=  value_i (limeSettings, LIME_SETTINGS, "gain", 50);
	gainSelector -> setValue (k);
	setGain (k);
	connect (gainSelector, qOverload<int>(&QSpinBox::valueChanged),
	         this, &limeHandler::setGain);
	connect (dumpButton, &QPushButton::clicked,
	         this, &limeHandler::set_xmlDump);
	connect (this, &limeHandler::new_gainValue,
	         gainSelector, &QSpinBox::setValue);
#if QT_VERSION >= QT_VERSION_CHECK (6, 7, 0)
	connect (filterSelector, &QCheckBox::checkStateChanged,
#else
	connect (filterSelector, &QCheckBox::stateChanged,
#endif
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
	storeWidgetPosition (limeSettings, &myFrame, LIME_SETTINGS);
	QString currentText = antennaList -> currentText ();
	store (limeSettings, LIME_SETTINGS, "antenna", currentText);
	store (limeSettings, LIME_SETTINGS, "gain", gainSelector -> value());
	store (limeSettings, LIME_SETTINGS,
	                      "filterDepth", filterDepth -> value ());
	LMS_Close (theDevice);
	if (libraryLoaded)
	   delete (library_p);
}

void	limeHandler::setGain		(int gaindB) {
float_type gg;
	int errorCode = LMS_SetGaindB (theDevice, LMS_CH_RX, 0, gaindB);
	if (errorCode < 0) {
	   const char *error = LMS_GetLastErrorMessage ();
	   theErrorLogger -> add ("Lime", QString (error));
	}
	errorCode = LMS_GetNormalizedGain (theDevice, LMS_CH_RX, 0, &gg);
	if (errorCode < 0) {
	   const char *error = LMS_GetLastErrorMessage ();
	   theErrorLogger -> add ("Lime", QString (error));
	}	
	else {
	   store (limeSettings, LIME_SETTINGS, "gain", gaindB);
	   actualGain	-> display (gg);
	}
}

void	limeHandler::setAntenna		(int ind) {
	int errorCode = LMS_SetAntenna (theDevice, LMS_CH_RX, 0, ind);
	if (errorCode < 0) {
	   const char *error = LMS_GetLastErrorMessage ();
	   theErrorLogger -> add ("Lime", QString (error));
	}	
}

void	limeHandler::set_filter		(int c) {
	(void)c;
	filtering	= filterSelector -> isChecked ();
	fprintf (stderr, "filter set %s\n", filtering ? "on" : "off");
}

bool	limeHandler::restartReader	(int32_t freq, int samplesSkipped) {
int	errorCode;

	(void)samplesSkipped;
	if (isRunning())
	   return true;

	errorCode = LMS_SetLOFrequency (theDevice, LMS_CH_RX, 0, freq);
	if (errorCode < 0) {
	   const char *error = LMS_GetLastErrorMessage ();
	   theErrorLogger -> add ("Lime", QString (error));
	   return false;
	}

	double actualFreq;
	errorCode = LMS_GetLOFrequency (theDevice, LMS_CH_RX, 0, &actualFreq);
	if (errorCode < 0) {
	   const char *error = LMS_GetLastErrorMessage ();
	   theErrorLogger -> add ("Lime", QString (error));
	   return false;
	}
	frequencyLabel	-> setText (QString::number ((int)actualFreq / 1000) + " kHz");
	lastFrequency	= actualFreq;
	if (save_gainSettings) {
	   update_gainSettings	(actualFreq / MHz (1));
	   setGain (gainSelector -> value ());
	}

	stream. isTx            = false;
        stream. channel         = 0;
        stream. fifoSize        = FIFO_SIZE;
        stream. throughputVsLatency     = 0.1;  // ???
        stream. dataFmt         = lms_stream_t::LMS_FMT_I12;    // 12 bit ints

	errorCode     = LMS_SetupStream (theDevice, &stream);
	if (errorCode < 0) {
	   const char *error = LMS_GetLastErrorMessage ();
	   theErrorLogger -> add ("Lime", QString (error));
	   return false;
	}
        errorCode     = LMS_StartStream (&stream);
	if (errorCode < 0) {
	   const char *error = LMS_GetLastErrorMessage ();
	   theErrorLogger -> add ("Lime", QString (error));
	   return false;
	}
	start ();
	return true;
}
	
void	limeHandler::stopReader	() {
int errorCode;
	close_xmlDump ();
	if (!isRunning())
	   return;
	if (save_gainSettings)
	   record_gainSettings (lastFrequency);

	running. store (false);
	while (isRunning())
	   usleep (200);
	errorCode = LMS_StopStream	(&stream);	
	if (errorCode < 0) {
	   const char *error = LMS_GetLastErrorMessage ();
	   theErrorLogger -> add ("Lime", QString (error));
	   return;
	}
	errorCode = LMS_DestroyStream	(theDevice, &stream);
	if (errorCode < 0) {
	   const char *error = LMS_GetLastErrorMessage ();
	   theErrorLogger -> add ("Lime", QString (error));
	   return;
	}
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
int	samples;
lms_stream_status_t streamStatus;
int	underruns	= 0;
int	overruns	= 0;
int	amountRead	= 0;

	running. store (true);
	while (running. load()) {
	   samples = LMS_RecvStream (&stream, localBuffer,
	                                     FIFO_SIZE,  &meta, 1000);
	   if (samples < 0) {
	      const char *error = LMS_GetLastErrorMessage ();
	      theErrorLogger -> add ("Lime", QString (error));
	      continue;
	   }

	   _I_Buffer. putDataIntoBuffer (localBuffer, samples);
	   amountRead	+= samples;
	   int errorCode = LMS_GetStreamStatus (&stream, &streamStatus);
	   if (errorCode < 0) {
	      const char *error = LMS_GetLastErrorMessage ();
	      theErrorLogger -> add ("Lime", QString (error));
	      continue;
	   }
	   underruns	+= streamStatus. underrun;
	   overruns	+= streamStatus. overrun;
	   if (amountRead > 2048000) {
	      amountRead = 0;
	      underrunDisplay	-> display (underruns);
	      overrunDisplay	-> display (overruns);
	      fifoDisplay	-> display ((int)streamStatus. fifoFilledCount);
	      underruns	= 0;
	      overruns	= 0;
	   }
	}
}

bool	limeHandler::load_limeFunctions() {

	this	-> LMS_GetDeviceList = (pfn_LMS_GetDeviceList)
	                    library_p -> resolve ("LMS_GetDeviceList");
	if (this -> LMS_GetDeviceList == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_GetdeviceList\n");
	   return false;
	}
	this	-> LMS_Open = (pfn_LMS_Open)
	                    library_p -> resolve ("LMS_Open");
	if (this -> LMS_Open == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_Open\n");
	   return false;
	}
	this	-> LMS_Close = (pfn_LMS_Close)
	                    library_p -> resolve ("LMS_Close");
	if (this -> LMS_Close == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_Close\n");
	   return false;
	}
	this	-> LMS_Init = (pfn_LMS_Init)
	                    library_p -> resolve ("LMS_Init");
	if (this -> LMS_Init == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_Init\n");
	   return false;
	}
	this	-> LMS_GetNumChannels = (pfn_LMS_GetNumChannels)
	                    library_p -> resolve ("LMS_GetNumChannels");
	if (this -> LMS_GetNumChannels == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_GetNumChannels\n");
	   return false;
	}
	this	-> LMS_EnableChannel = (pfn_LMS_EnableChannel)
	                    library_p -> resolve ("LMS_EnableChannel");
	if (this -> LMS_EnableChannel == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_EnableChannel\n");
	   return false;
	}
	this	-> LMS_SetSampleRate = (pfn_LMS_SetSampleRate)
	                    library_p -> resolve ("LMS_SetSampleRate");
	if (this -> LMS_SetSampleRate == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_SetSampleRate\n");
	   return false;
	}
	this	-> LMS_GetSampleRate = (pfn_LMS_GetSampleRate)
	                    library_p -> resolve ("LMS_GetSampleRate");
	if (this -> LMS_GetSampleRate == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_GetSampleRate\n");
	   return false;
	}
	this	-> LMS_SetLOFrequency = (pfn_LMS_SetLOFrequency)
	                    library_p -> resolve ("LMS_SetLOFrequency");
	if (this -> LMS_SetLOFrequency == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_SetLOFrequency\n");
	   return false;
	}
	this	-> LMS_GetLOFrequency = (pfn_LMS_GetLOFrequency)
	                    library_p -> resolve ("LMS_GetLOFrequency");
	if (this -> LMS_GetLOFrequency == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_GetLOFrequency\n");
	   return false;
	}
	this	-> LMS_GetAntennaList = (pfn_LMS_GetAntennaList)
	                    library_p -> resolve ("LMS_GetAntennaList");
	if (this -> LMS_GetAntennaList == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_GetAntennaList\n");
	   return false;
	}
	this	-> LMS_SetAntenna = (pfn_LMS_SetAntenna)
	                    library_p -> resolve ("LMS_SetAntenna");
	if (this -> LMS_SetAntenna == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_SetAntenna\n");
	   return false;
	}
	this	-> LMS_GetAntenna = (pfn_LMS_GetAntenna)
	                    library_p -> resolve ("LMS_GetAntenna");
	if (this -> LMS_GetAntenna == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_GetAntenna\n");
	   return false;
	}
	this	-> LMS_GetAntennaBW = (pfn_LMS_GetAntennaBW)
	                    library_p -> resolve ("LMS_GetAntennaBW");
	if (this -> LMS_GetAntennaBW == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_GetAntennaBW\n");
	   return false;
	}
	this	-> LMS_SetNormalizedGain = (pfn_LMS_SetNormalizedGain)
	                    library_p -> resolve ("LMS_SetNormalizedGain");
	if (this -> LMS_SetNormalizedGain == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_SetNormalizedGain\n");
	   return false;
	}
	this	-> LMS_GetNormalizedGain = (pfn_LMS_GetNormalizedGain)
	                    library_p -> resolve ("LMS_GetNormalizedGain");
	if (this -> LMS_GetNormalizedGain == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_GetNormalizedGain\n");
	   return false;
	}
	this	-> LMS_SetGaindB = (pfn_LMS_SetGaindB)
	                    library_p -> resolve ("LMS_SetGaindB");
	if (this -> LMS_SetGaindB == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_SetGaindB\n");
	   return false;
	}
	this	-> LMS_GetGaindB = (pfn_LMS_GetGaindB)
	                    library_p -> resolve ("LMS_GetGaindB");
	if (this -> LMS_GetGaindB == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_GetGaindB\n");
	   return false;
	}
	this	-> LMS_GetLPFBWRange = (pfn_LMS_GetLPFBWRange)
	                    library_p -> resolve ("LMS_GetLPFBWRange");
	if (this -> LMS_GetLPFBWRange == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_GetLPFBWRange\n");
	   return false;
	}
	this	-> LMS_SetLPFBW = (pfn_LMS_SetLPFBW)
	                    library_p -> resolve ("LMS_SetLPFBW");
	if (this -> LMS_SetLPFBW == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_SetLPFBW\n");
	   return false;
	}
	this	-> LMS_GetLPFBW = (pfn_LMS_GetLPFBW)
	                    library_p -> resolve ("LMS_GetLPFBW");
	if (this -> LMS_GetLPFBW == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_GetLPFBW\n");
	   return false;
	}
	this	-> LMS_SetGFIRLPF = (pfn_LMS_SetGFIRLPF)
	                    library_p -> resolve ("LMS_SetGFIRLPF");
	if (this -> LMS_SetGFIRLPF == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_SetGFIRLPF\n");
	   return false;
	}
	this	-> LMS_Calibrate = (pfn_LMS_Calibrate)
	                    library_p -> resolve ("LMS_Calibrate");
	if (this -> LMS_Calibrate == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_Calibrate\n");
	   return false;
	}
	this	-> LMS_SetupStream = (pfn_LMS_SetupStream)
	                    library_p -> resolve ("LMS_SetupStream");
	if (this -> LMS_SetupStream == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_SetupStream\n");
	   return false;
	}
	this	-> LMS_DestroyStream = (pfn_LMS_DestroyStream)
	                    library_p -> resolve ("LMS_DestroyStream");
	if (this -> LMS_DestroyStream == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_DestroyStream\n");
	   return false;
	}
	this	-> LMS_StartStream = (pfn_LMS_StartStream)
	                    library_p -> resolve ("LMS_StartStream");
	if (this -> LMS_StartStream == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_StartStream\n");
	   return false;
	}
	this	-> LMS_StopStream = (pfn_LMS_StopStream)
	                    library_p -> resolve ("LMS_StopStream");
	if (this -> LMS_StopStream == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_StopStream\n");
	   return false;
	}
	this	-> LMS_RecvStream = (pfn_LMS_RecvStream)
	                    library_p -> resolve ("LMS_RecvStream");
	if (this -> LMS_RecvStream == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_RecvStream\n");
	   return false;
	}
	this	-> LMS_GetStreamStatus = (pfn_LMS_GetStreamStatus)
	                    library_p -> resolve ("LMS_GetStreamStatus");
	if (this -> LMS_GetStreamStatus == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_GetStreamStatus\n");
	   return false;
	}
	this	-> LMS_GetLastErrorMessage = (pfn_LMS_GetLastErrorMessage)
	                    library_p -> resolve ("LMS_GetLastErrorMessage");
	if (this -> LMS_GetLastErrorMessage == nullptr) {
	   theErrorLogger -> add ("Lime",
	                     "could not find LMS_GetLastErrorMessage\n");
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

bool	limeHandler::setup_xmlDump () {
QString channel		= value_s (limeSettings, DAB_GENERAL,
	                                       "channel", "xx");
	xmlWriter	= nullptr;
	try {
	   int gaindB	=  value_i (limeSettings, LIME_SETTINGS, "gain", 50);
	   xmlWriter	= new xml_fileWriter (limeSettings,
	                                      channel,
	                                      bitDepth (),
	                                      "int16",
	                                      2048000,
	                                      lastFrequency,
	                                      gaindB,
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

