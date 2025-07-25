#
/*
 *    Copyright (C) 2013 .. 2025
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
 *    along with Qt-SDR; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * 	This particular driver is a very simple wrapper around the
 * 	librtlsdr.  In order to keep things simple, we dynamically
 * 	load the dll (or .so). The librtlsdr is osmocom software and all rights
 * 	are greatly acknowledged
 */

#include	<QThread>
#include	"dab-constants.h"
#include	"rtlsdr-handler.h"
#include	"position-handler.h"
#include	"rtl-dongleselect.h"
#include	"rtl-sdr.h"
#include	"device-exceptions.h"
#include	"logger.h"
#include	"settings-handler.h"

#define	CORRF	0.005
#ifdef	__MINGW32__
#define	GETPROCADDRESS	GetProcAddress
#else
#define	GETPROCADDRESS	dlsym
#endif

//
//	Our wrapper is a simple classs
	rtlsdrHandler::rtlsdrHandler (QSettings *s,
	                              const QString &recorderVersion,
	                              logger	*theLogger):	// dummy for now
	                                 _I_Buffer (8 * 1024 * 1024),
	                                 theFilter (5, 1560000 / 2, SAMPLERATE) {
int16_t	deviceCount;
int32_t	r;
int16_t	deviceIndex;
QString	temp;
int	k;
char	manufac [256], product [256], serial [256];

	(void)theLogger;
	rtlsdrSettings			= s;
	this	-> recorderVersion	= recorderVersion;
	setupUi (&myFrame);
	setPositionAndSize (s, &myFrame, "rtlsdrSettings");
	myFrame. show ();
	filtering			= false;

	currentDepth	=  value_i (rtlsdrSettings, "rtlsdrSettings",
	                                               "filterDepth", 5);
	filterDepth	-> setValue (currentDepth);
	theFilter. resize (currentDepth);

	isActive. store (false);
#ifdef	__MINGW32__
	const char *libraryString	= "librtlsdr.dll";
#elif __linux__
//	const char *libraryString	= "/usr/local/lib64/librtlsdr.so";
	const char *libraryString	= "librtlsdr.so";
#elif __APPLE__
	const char *libraryString	= "librtlsdr.dylib";
#endif
	phandle = new QLibrary (libraryString);
	phandle -> load ();

	if (!phandle -> isLoaded ()) {
	   throw (device_exception (std::string ("failed to open ") + std::string (libraryString)));
	}

	if (!load_rtlFunctions ()) {
	   delete (phandle);
	   throw (device_exception ("could not load one or more library functions"));
	}

//	Ok, from here we have the library functions accessible
	deviceCount 		= this -> rtlsdr_get_device_count ();
	if (deviceCount == 0) {
	   delete (phandle);
	   throw (device_exception ("No rtlsdr device found"));
	}

	deviceIndex = 0;	// default
	if (deviceCount > 1) {
	   rtl_dongleSelect dongleSelector;
	   for (deviceIndex = 0; deviceIndex < deviceCount; deviceIndex ++) {
	      dongleSelector.
	           addtoDongleList (rtlsdr_get_device_name (deviceIndex));
	   }
	   deviceIndex = dongleSelector. QDialog::exec();
	}
//
//	OK, now open the hardware
	r		= this -> rtlsdr_open (&theDevice, deviceIndex);
	if (r < 0) {
	   delete phandle;
	   throw (device_exception ("Opening rtlsdr device failed"));
	}

	deviceModel	= rtlsdr_get_device_name (deviceIndex);
	deviceVersion	-> setText (deviceModel);
	QString tunerType	= get_tunerType (rtlsdr_get_tuner_type (theDevice));
	product_display	-> setText (tunerType);

	r = rtlsdr_set_sample_rate (theDevice, SAMPLERATE);
	if (r < 0) {
	   delete phandle;
	   throw (device_exception ("Setting samplerate for rtlsdr failed"));
	}

	gainsCount = rtlsdr_get_tuner_gains (theDevice, nullptr);
	fprintf (stderr, "Supported gain values (%d): ", gainsCount);
	{  int gains [gainsCount];
	   gainsCount	= rtlsdr_get_tuner_gains (theDevice, gains);
	   for (int i = gainsCount; i > 0; i--) {
	      fprintf (stderr, "%.1f ", gains [i - 1] / 10.0);
	      gainControl -> addItem (QString::number (gains [i - 1]));
	   }
	   fprintf (stderr, "\n");
	}

	if (rtlsdr_set_tuner_bandwidth != nullptr)
	   rtlsdr_set_tuner_bandwidth (theDevice, KHz (1575));
	rtlsdr_set_tuner_gain_mode (theDevice, 1);
//
//	See what the saved values are and restore the GUI settings
	temp =  value_s (rtlsdrSettings, "rtlsdrSettings",
	                              "externalGain", "10");
	k	= gainControl -> findText (temp);
	gainControl	-> setCurrentIndex (k != -1 ? k : gainsCount / 2);

	temp		= value_s (rtlsdrSettings, "rtlsdrSettings",
	                                        "autogain", "autogain_on");
	agcControl	-> setChecked (temp == "autogain_on");
	
	ppm_correction	->  
	     setValue (value_i (rtlsdrSettings, "rtlsdrSettings",
	                                 "ppm_correction", 0));
	save_gainSettings	=
	     value_i (rtlsdrSettings, "rtlsdrSettings",
	                             "save_gainSettings", 1) != 0;

	(void)this -> rtlsdr_set_center_freq (theDevice, 220000000);

	rtlsdr_get_usb_strings (theDevice, manufac, product, serial);
	fprintf (stderr, "%s %s %s\n",
	            manufac, product, serial);

//	all sliders/values are set to previous values, now do the settings
//	based on these slider values
	if (agcControl -> isChecked ())
	   rtlsdr_set_agc_mode (theDevice, 1);
	else
	   rtlsdr_set_agc_mode (theDevice, 0);
	rtlsdr_set_tuner_gain	(theDevice, 
	                         gainControl -> currentText (). toInt ());
	set_ppmCorrection	(ppm_correction -> value());

	for (int i = 0; i < 256; i ++)
	   convTable [i] = (-128.0 + i) / 128.0;
//	and attach the buttons/sliders to the actions
	connect (gainControl,
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 2)
	         qOverload<const QString &>(&QComboBox::textActivated),
#else
	         qOverload<const QString &>(&QComboBox::activated),
#endif
	         this, &rtlsdrHandler::set_ExternalGain);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (agcControl, &QCheckBox::checkStateChanged,
#else
	connect (agcControl, &QCheckBox::stateChanged,
#endif
	         this, &rtlsdrHandler::set_autogain);
	connect (ppm_correction, qOverload<int>(&QSpinBox::valueChanged),
	         this, &rtlsdrHandler::set_ppmCorrection);
	connect (xml_dumpButton, &QPushButton::clicked,
	         this, &rtlsdrHandler::set_xmlDump);
	connect (iq_dumpButton, &QPushButton::clicked,
	         this, &rtlsdrHandler::set_iqDump);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (biasControl, &QCheckBox::checkStateChanged,
#else
	connect (biasControl, &QCheckBox::stateChanged,
#endif
	         this, &rtlsdrHandler::set_biasControl);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (filterSelector, &QCheckBox::checkStateChanged,
#else
	connect (filterSelector, &QCheckBox::stateChanged,
#endif
	         this, &rtlsdrHandler::set_filter);
//
//	and for saving/restoring the gain setting:
	connect (this, &rtlsdrHandler::new_gainIndex,
	         gainControl, &QComboBox::setCurrentIndex);
	connect (this, &rtlsdrHandler::new_agcSetting,
	         agcControl, &QCheckBox::setChecked);
	iqDumper	= nullptr;
	iq_dumping. store (false);
	xml_dumping. store (false);
}

	rtlsdrHandler::~rtlsdrHandler () {
	stopReader	();
	if (!workerHandle. isNull ()) {
	   rtlsdr_cancel_async (theDevice);
	   while (!workerHandle -> isFinished())
	      usleep (200); 
	   _I_Buffer. FlushRingBuffer();
	   workerHandle. reset ();
	}
	storeWidgetPosition (rtlsdrSettings, &myFrame, "rtlsdrSettings");
	QString gainText	= gainControl -> currentText ();
	store (rtlsdrSettings, "er;sdrSettings",
	                       "externalGain", gainText);
	store (rtlsdrSettings, "rtlsdrSettings",
	                   "autogain", agcControl -> isChecked () ? 1 : 0);
	store (rtlsdrSettings, "rtlsdrSettings",
	                    "ppm_correction", ppm_correction -> value());
	store (rtlsdrSettings, "rtlsdrSettings",
	                    "filterDepth", filterDepth -> value ());
	rtlsdrSettings	-> sync ();
	if (!xmlWriter. isNull ())
	   xmlWriter. reset ();
	myFrame. hide ();
	usleep (1000);
	rtlsdr_close (theDevice);
	delete phandle;
}

//
void	rtlsdrHandler::set_filter	(int c) {
	(void)c;
	filtering       = filterSelector -> isChecked ();
}

bool	rtlsdrHandler::restartReader	(int32_t freq, int skipped) {
	(void)(this -> rtlsdr_set_center_freq (theDevice, freq));
	if (save_gainSettings)
	   update_gainSettings (freq / MHz (1));

	lastFrequency	= freq;
	this -> toSkip = skipped;
	set_autogain (agcControl -> isChecked ());
	set_ExternalGain (gainControl -> currentText ());
	if (workerHandle. isNull ()) {
//	reset endpoint
	   (void)this -> rtlsdr_reset_buffer (theDevice);
	   workerHandle. reset (new dll_driver (this));
	}
	_I_Buffer. FlushRingBuffer();
	isActive. store (true);
	return true;
}

void	rtlsdrHandler::stopReader () {
	if (workerHandle. isNull ())
	   return;
	isActive. store (false);
	this -> rtlsdr_cancel_async (theDevice);
        this -> rtlsdr_reset_buffer (theDevice);
        if (!workerHandle. isNull ()) {
            while (!workerHandle -> isFinished())
                usleep(100);
            _I_Buffer.FlushRingBuffer ();
            workerHandle. reset ();
        }
        close_xmlDump();

	close_xmlDump ();
	if (save_gainSettings)
	   record_gainSettings	((int32_t)(this -> rtlsdr_get_center_freq (theDevice)) / MHz (1));
}
//
//	when selecting  the gain from a table, use the table value
void	rtlsdrHandler::set_ExternalGain	(const QString &gain) {
	rtlsdr_set_tuner_gain (theDevice, gain. toInt());
}
//
void	rtlsdrHandler::set_autogain	(int dummy) {
	(void)dummy;
	rtlsdr_set_agc_mode (theDevice, agcControl -> isChecked () ? 1 : 0);
	rtlsdr_set_tuner_gain (theDevice, 
	                       gainControl -> currentText (). toInt ());
}
//
void	rtlsdrHandler::set_biasControl	(int dummy) {
	(void)dummy;
	if (rtlsdr_set_bias_tee != nullptr)
	   rtlsdr_set_bias_tee (theDevice, biasControl -> isChecked () ? 1 : 0);
}
//	correction is in Hz
void	rtlsdrHandler::set_ppmCorrection	(int32_t ppm) {
	this -> rtlsdr_set_freq_correction (theDevice, ppm);
}

int32_t	rtlsdrHandler::getSamples (std::complex<float> *V, int32_t size) { 
	if (!isActive. load ())
	   return 0;
	return _I_Buffer. getDataFromBuffer (V, size);
}

int32_t	rtlsdrHandler::Samples () {
	if (!isActive. load ())
	   return 0;
	return _I_Buffer. GetRingBufferReadAvailable ();
}
//

bool	rtlsdrHandler::load_rtlFunctions() {
//
//	link the required procedures
	rtlsdr_open	= (pfnrtlsdr_open) phandle -> resolve ("rtlsdr_open");
	if (rtlsdr_open == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_open\n");
	   return false;
	}

	rtlsdr_close	= (pfnrtlsdr_close) phandle -> resolve ("rtlsdr_close");
	if (rtlsdr_close == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_close\n");
	   return false;
	}

	rtlsdr_get_usb_strings =
	               (pfnrtlsdr_get_usb_strings)
	                   phandle -> resolve ("rtlsdr_get_usb_strings");
	if (rtlsdr_get_usb_strings == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_get_usb_strings\n");
	   return false;
	}

	rtlsdr_set_sample_rate =
	              (pfnrtlsdr_set_sample_rate)
	                  phandle -> resolve ("rtlsdr_set_sample_rate");
	if (rtlsdr_set_sample_rate == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_set_sample_rate\n");
	   return false;
	}

	rtlsdr_get_sample_rate	=
	             (pfnrtlsdr_get_sample_rate)
	                  phandle -> resolve ("rtlsdr_get_sample_rate");
	if (rtlsdr_get_sample_rate == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_get_sample_rate\n");
	   return false;
	}

	rtlsdr_get_tuner_gains	=
	             (pfnrtlsdr_get_tuner_gains)
	                 phandle -> resolve ("rtlsdr_get_tuner_gains");
	if (rtlsdr_get_tuner_gains == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_get_tuner_gains\n");
	   return false;
	}

	rtlsdr_get_tuner_type	=
	             (pfnrtlsdr_get_tuner_type)
	                 phandle -> resolve ("rtlsdr_get_tuner_type");
	if (rtlsdr_get_tuner_type == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_get_tuner_type\n");
	   return false;
	}

	rtlsdr_set_tuner_gain_mode	=
	             (pfnrtlsdr_set_tuner_gain_mode)
	                 phandle -> resolve ("rtlsdr_set_tuner_gain_mode");
	if (rtlsdr_set_tuner_gain_mode == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_set_tuner_gain_mode\n");
	   return false;
	}

	rtlsdr_set_agc_mode	=
	             (pfnrtlsdr_set_agc_mode)
	                 phandle -> resolve ("rtlsdr_set_agc_mode");
	if (rtlsdr_set_agc_mode == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_set_agc_mode\n");
	   return false;
	}

	rtlsdr_set_tuner_gain	=
	            (pfnrtlsdr_set_tuner_gain)
	                phandle -> resolve ("rtlsdr_set_tuner_gain");
	if (rtlsdr_set_tuner_gain == nullptr) {
	   fprintf (stderr, "Cound not find rtlsdr_set_tuner_gain\n");
	   return false;
	}

	rtlsdr_get_tuner_gain	=
	            (pfnrtlsdr_get_tuner_gain)
	                 phandle -> resolve ("rtlsdr_get_tuner_gain");
	if (rtlsdr_get_tuner_gain == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_get_tuner_gain\n");
	   return false;
	}

	rtlsdr_set_center_freq	=
	            (pfnrtlsdr_set_center_freq)
	                phandle -> resolve ("rtlsdr_set_center_freq");
	if (rtlsdr_set_center_freq == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_set_center_freq\n");
	   return false;
	}

	rtlsdr_get_center_freq	=
	            (pfnrtlsdr_get_center_freq)
	                phandle -> resolve ("rtlsdr_get_center_freq");
	if (rtlsdr_get_center_freq == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_get_center_freq\n");
	   return false;
	}

	rtlsdr_reset_buffer	=
	            (pfnrtlsdr_reset_buffer)
	                phandle -> resolve ("rtlsdr_reset_buffer");
	if (rtlsdr_reset_buffer == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_reset_buffer\n");
	   return false;
	}

	rtlsdr_read_async	=
	            (pfnrtlsdr_read_async)
	                phandle -> resolve ("rtlsdr_read_async");
	if (rtlsdr_read_async == nullptr) {
	   fprintf (stderr, "Cound not find rtlsdr_read_async\n");
	   return false;
	}

	rtlsdr_get_device_count	=
	           (pfnrtlsdr_get_device_count)
	               phandle -> resolve ("rtlsdr_get_device_count");
	if (rtlsdr_get_device_count == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_get_device_count\n");
	   return false;
	}

	rtlsdr_cancel_async	=
	           (pfnrtlsdr_cancel_async)
	               phandle -> resolve ("rtlsdr_cancel_async");
	if (rtlsdr_cancel_async == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_cancel_async\n");
	   return false;
	}

	rtlsdr_set_direct_sampling =
	           (pfnrtlsdr_set_direct_sampling)
	               phandle -> resolve ("rtlsdr_set_direct_sampling");
	if (rtlsdr_set_direct_sampling == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_set_direct_sampling\n");
	   return false;
	}

	rtlsdr_set_freq_correction =
	           (pfnrtlsdr_set_freq_correction)
	               phandle -> resolve ("rtlsdr_set_freq_correction");
	if (rtlsdr_set_freq_correction == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_set_freq_correction\n");
	   return false;
	}
	
	rtlsdr_get_device_name =
	           (pfnrtlsdr_get_device_name)
	               phandle -> resolve ("rtlsdr_get_device_name");
	if (rtlsdr_get_device_name == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_get_device_name\n");
	   return false;
	}

	rtlsdr_set_tuner_bandwidth	=
	            (pfnrtlsdr_set_center_freq)
	                phandle -> resolve ("rtlsdr_set_tuner_bandwidth");
	if (rtlsdr_set_tuner_bandwidth == nullptr) {
	   fprintf (stderr, "no support for tuner bandwidth\n");
//	   nullpointer - if function is not available - is handled
	}

	rtlsdr_set_bias_tee =
	           (pfnrtlsdr_set_bias_tee)
	               phandle -> resolve ("rtlsdr_set_bias_tee");
	if (rtlsdr_set_bias_tee == nullptr)
	   fprintf (stderr, "biasControl will not work\n");
//	nullpointer - if function is not available - is handled
//
	fprintf (stderr, "OK, functions seem to be loaded\n");
	return true;
}

void	rtlsdrHandler::resetBuffer() {
	_I_Buffer. FlushRingBuffer();
}

int16_t	rtlsdrHandler::maxGain() {
	return gainsCount;
}

int16_t	rtlsdrHandler::bitDepth() {
	return 8;
}

QString	rtlsdrHandler::deviceName	() {
	return deviceModel;
}

void	rtlsdrHandler::set_iqDump	() {
	if (xmlWriter != nullptr)
	   return;
	if (iqDumper == nullptr) {
	   if (setup_iqDump ()) {
	      xml_dumpButton	-> hide ();
	   }
	}
	else {
	   close_iqDump ();
	   xml_dumpButton	-> show ();
	}
}

bool	rtlsdrHandler::setup_iqDump () {
	QString fileName = QFileDialog::getSaveFileName (nullptr,
	                                         tr ("Save file ..."),
	                                         QDir::homePath(),
	fileName        = QDir::toNativeSeparators (fileName));
	iqDumper	= fopen (fileName. toUtf8 (). data (), "wb");
	if (iqDumper == nullptr)
	   return false;
	iq_dumpButton	-> setText ("writing raw file");
	iq_dumping. store (true);
	return true;
}

void	rtlsdrHandler::close_iqDump () {
	if (iqDumper == nullptr)	// this can happen !!
	   return;
	iq_dumping. store (false);
	fclose (iqDumper);
	iq_dumpButton	-> setText ("Dump to raw");
	iqDumper	= nullptr;
}
	   
void	rtlsdrHandler::set_xmlDump () {
	if (iqDumper != nullptr)
	   return;
	if (!xml_dumping. load ()) {
	   if (setup_xmlDump (false)) 
	      iq_dumpButton	-> hide	();
	}
	else {
	   close_xmlDump ();
	   iq_dumpButton	-> show	();
	}
}

void	rtlsdrHandler::startDump	() {
	setup_xmlDump (true);
	iq_dumpButton	-> hide ();
}

void	rtlsdrHandler::stopDump		() {
	close_xmlDump	();
	iq_dumpButton	-> show ();
}

static inline
bool	isValid (QChar c) {
	return c. isLetterOrNumber () || (c == '-');
}

bool	rtlsdrHandler::setup_xmlDump (bool direct) {
QString channel		= rtlsdrSettings -> value ("channel", "xx").
	                                                      toString ();
	try {
	   xmlWriter. reset (new xml_fileWriter (rtlsdrSettings,
	                                      channel,
	                                      8,
	                                      "uint8",
	                                      SAMPLERATE,
	                                      lastFrequency,
	                                      rtlsdr_get_tuner_gain (theDevice),
	                                      "RTLSDR",
	                                      deviceModel,
	                                      recorderVersion,
	                                      direct));
	} catch (...) {
	   return false;
	}
	xml_dumpButton	-> setText ("writing xml file");
	xml_dumping. store (true);
	return true;
}
	
void	rtlsdrHandler::close_xmlDump () {
	if (xmlWriter == nullptr)
	   return;
	usleep (1000);
	xmlWriter	-> computeHeader ();
	xmlWriter. reset ();
	xml_dumpButton	-> setText ("Dump to xml");
	xml_dumping. store (false);
}

////////////////////////////////////////////////////////////////////////
//
//      the frequency (the MHz component) is used as key
//
void    rtlsdrHandler::record_gainSettings (int freq) {
QString	gain	= gainControl	-> currentText ();
int	agc	= agcControl	-> isChecked () ? 1 : 0;
QString theValue        = gain + ":" + QString::number (agc);
QString freqVal		= QString::number (freq);
	store (rtlsdrSettings, "rtlsdrSettings", freqVal, theValue);
}

void	rtlsdrHandler::update_gainSettings (int freq) {
int	agc;
QString	theValue	= "";
QString freqVal		= QString::number (freq);
	theValue	= value_s (rtlsdrSettings, "rtlsdrSettings",
	                                             freqVal, theValue);

	if (theValue == QString (""))
	   return;		// or set some defaults here

	QStringList result	= theValue. split (":");
	if (result. size () != 2) 	// should not happen
	   return;

	QString temp = result. at (0);
	agc	= result. at (1). toInt ();

	int k	= gainControl -> findText (temp);
	if (k != -1) {
	   gainControl	-> blockSignals (true);
	   new_gainIndex (k);
	   while (gainControl -> currentText () != temp)
	      usleep (1000);
	   gainControl	-> blockSignals (false);
	}

	agcControl	-> blockSignals (true);
	new_agcSetting (agc == 1);
	while (agcControl -> isChecked () != (agc == 1))
	   usleep (1000);
	set_autogain (agcControl -> isChecked ());
	agcControl	-> blockSignals (false);
}

#define	IQ_BUFSIZE	4096
void	rtlsdrHandler::processBuffer (uint8_t *buf, uint32_t len) {
float	sumI	= 0;
float	sumQ	= 0;
auto	*tempBuf 	= dynVec (std::complex<float>, len / 2);
static uint8_t dumpBuffer [2 * IQ_BUFSIZE];
static int iqTeller	= 0;

	if (!isActive. load ()) 
	   return;

	if (toSkip > 0) {
	   toSkip -= len / 2;
	   return;
	}
	if (xml_dumping. load ())
	   xmlWriter -> add ((std::complex<uint8_t> *)buf, len / 2);

	if (iq_dumping. load ()) {
	   for (uint32_t i = 0; i < len / 2; i ++) {
	      dumpBuffer [2 * iqTeller]	= buf [2 * i];
	      dumpBuffer [2 * iqTeller + 1] = buf [2 * i + 1];
	      iqTeller ++;
	      if (iqTeller >= IQ_BUFSIZE) {
	         fwrite (dumpBuffer, 2, IQ_BUFSIZE, iqDumper);
	         iqTeller = 0;
	      }
	   }
	}
	if ((filtering) && (filterDepth -> value () != currentDepth)) {
	   currentDepth = filterDepth -> value ();
	   theFilter. resize (currentDepth);
	}
	float dcI	= m_dcI;
	float dcQ	= m_dcQ;
	for (uint32_t i = 0; i < len / 2; i ++) {
	   float tempI	= convTable [buf [2 * i]];
	   float tempQ	= convTable [buf [2 * i + 1]];
	   sumI		+= tempI;
	   sumQ		+= tempQ;
	   tempBuf [i] = std::complex<float> (tempI, tempQ);
	   if (filtering)
	      tempBuf [i] = theFilter. Pass (tempBuf [i]);
	}
// calculate correction values for next input buffer
	m_dcI = sumI / (len / 2) * CORRF + (1 - CORRF) * dcI;
	m_dcQ = sumQ / (len / 2) * CORRF + (1 - CORRF) * dcQ;
	int ovf	= _I_Buffer. GetRingBufferWriteAvailable () - len / 2;
	if (ovf < 0)
	   (void)_I_Buffer. putDataIntoBuffer (tempBuf, len / 2 + ovf);
	else
	   (void)_I_Buffer. putDataIntoBuffer (tempBuf, len / 2);
	reportOverflow (ovf < 0);
}

QString	rtlsdrHandler::get_tunerType	(int tunerType) {
	switch (tunerType) {
	   case RTLSDR_TUNER_E4000:
	      return "E4000";
	   case RTLSDR_TUNER_FC0012:
	      return "FC0012";
	   case RTLSDR_TUNER_FC0013:
	      return "FC0013";
	   case RTLSDR_TUNER_FC2580:
	      return "FC2580";
	   case RTLSDR_TUNER_R820T:
	      return "R820T";
	      break;
	   case RTLSDR_TUNER_R828D:
	      return "R828D";
	   default:
	      return "unknown";
	}
}

void	rtlsdrHandler::reportOverflow (bool ovf) {
static bool theOvf	= true;
	if (ovf && !theOvf){
	   overflowLabel -> setText ("Overload");
           overflowLabel -> setStyleSheet ("QLabel {background-color : red; \
                                                   color: white}");
	   theOvf	= true;
	}
	else
	if (!ovf && theOvf) {		// space in the buffer is sufficient
	   overflowLabel -> setStyleSheet("QLabel {background-color : green; \
                                                   color: white}");
	   theOvf	= false;
	}
	else
	   theOvf = ovf;
}


