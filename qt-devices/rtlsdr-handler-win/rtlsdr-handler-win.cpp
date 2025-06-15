#
/*
 *    Copyright (C) 2013 .. 2017
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
#include	"rtlsdr-handler-win.h"
#include	"rtl-dongleselect.h"
#include	"position-handler.h"
#include	"rtl-sdr.h"
#include	"device-exceptions.h"
#include	"logger.h"
#include	"settings-handler.h"

#include	"dll-driver.h"

//
//	For the callback, we do need some environment which
//	is passed through the ctx parameter
//
//	This is the user-side call back function
//	ctx is the calling task

static 
float convTable [] = {
 -128 / 128.0 , -127 / 128.0 , -126 / 128.0 , -125 / 128.0 , -124 / 128.0 , -123 / 128.0 , -122 / 128.0 , -121 / 128.0 , -120 / 128.0 , -119 / 128.0 , -118 / 128.0 , -117 / 128.0 , -116 / 128.0 , -115 / 128.0 , -114 / 128.0 , -113 / 128.0 
, -112 / 128.0 , -111 / 128.0 , -110 / 128.0 , -109 / 128.0 , -108 / 128.0 , -107 / 128.0 , -106 / 128.0 , -105 / 128.0 , -104 / 128.0 , -103 / 128.0 , -102 / 128.0 , -101 / 128.0 , -100 / 128.0 , -99 / 128.0 , -98 / 128.0 , -97 / 128.0 
, -96 / 128.0 , -95 / 128.0 , -94 / 128.0 , -93 / 128.0 , -92 / 128.0 , -91 / 128.0 , -90 / 128.0 , -89 / 128.0 , -88 / 128.0 , -87 / 128.0 , -86 / 128.0 , -85 / 128.0 , -84 / 128.0 , -83 / 128.0 , -82 / 128.0 , -81 / 128.0 
, -80 / 128.0 , -79 / 128.0 , -78 / 128.0 , -77 / 128.0 , -76 / 128.0 , -75 / 128.0 , -74 / 128.0 , -73 / 128.0 , -72 / 128.0 , -71 / 128.0 , -70 / 128.0 , -69 / 128.0 , -68 / 128.0 , -67 / 128.0 , -66 / 128.0 , -65 / 128.0 
, -64 / 128.0 , -63 / 128.0 , -62 / 128.0 , -61 / 128.0 , -60 / 128.0 , -59 / 128.0 , -58 / 128.0 , -57 / 128.0 , -56 / 128.0 , -55 / 128.0 , -54 / 128.0 , -53 / 128.0 , -52 / 128.0 , -51 / 128.0 , -50 / 128.0 , -49 / 128.0 
, -48 / 128.0 , -47 / 128.0 , -46 / 128.0 , -45 / 128.0 , -44 / 128.0 , -43 / 128.0 , -42 / 128.0 , -41 / 128.0 , -40 / 128.0 , -39 / 128.0 , -38 / 128.0 , -37 / 128.0 , -36 / 128.0 , -35 / 128.0 , -34 / 128.0 , -33 / 128.0 
, -32 / 128.0 , -31 / 128.0 , -30 / 128.0 , -29 / 128.0 , -28 / 128.0 , -27 / 128.0 , -26 / 128.0 , -25 / 128.0 , -24 / 128.0 , -23 / 128.0 , -22 / 128.0 , -21 / 128.0 , -20 / 128.0 , -19 / 128.0 , -18 / 128.0 , -17 / 128.0 
, -16 / 128.0 , -15 / 128.0 , -14 / 128.0 , -13 / 128.0 , -12 / 128.0 , -11 / 128.0 , -10 / 128.0 , -9 / 128.0 , -8 / 128.0 , -7 / 128.0 , -6 / 128.0 , -5 / 128.0 , -4 / 128.0 , -3 / 128.0 , -2 / 128.0 , -1 / 128.0 
, 0 / 128.0 , 1 / 128.0 , 2 / 128.0 , 3 / 128.0 , 4 / 128.0 , 5 / 128.0 , 6 / 128.0 , 7 / 128.0 , 8 / 128.0 , 9 / 128.0 , 10 / 128.0 , 11 / 128.0 , 12 / 128.0 , 13 / 128.0 , 14 / 128.0 , 15 / 128.0 
, 16 / 128.0 , 17 / 128.0 , 18 / 128.0 , 19 / 128.0 , 20 / 128.0 , 21 / 128.0 , 22 / 128.0 , 23 / 128.0 , 24 / 128.0 , 25 / 128.0 , 26 / 128.0 , 27 / 128.0 , 28 / 128.0 , 29 / 128.0 , 30 / 128.0 , 31 / 128.0 
, 32 / 128.0 , 33 / 128.0 , 34 / 128.0 , 35 / 128.0 , 36 / 128.0 , 37 / 128.0 , 38 / 128.0 , 39 / 128.0 , 40 / 128.0 , 41 / 128.0 , 42 / 128.0 , 43 / 128.0 , 44 / 128.0 , 45 / 128.0 , 46 / 128.0 , 47 / 128.0 
, 48 / 128.0 , 49 / 128.0 , 50 / 128.0 , 51 / 128.0 , 52 / 128.0 , 53 / 128.0 , 54 / 128.0 , 55 / 128.0 , 56 / 128.0 , 57 / 128.0 , 58 / 128.0 , 59 / 128.0 , 60 / 128.0 , 61 / 128.0 , 62 / 128.0 , 63 / 128.0 
, 64 / 128.0 , 65 / 128.0 , 66 / 128.0 , 67 / 128.0 , 68 / 128.0 , 69 / 128.0 , 70 / 128.0 , 71 / 128.0 , 72 / 128.0 , 73 / 128.0 , 74 / 128.0 , 75 / 128.0 , 76 / 128.0 , 77 / 128.0 , 78 / 128.0 , 79 / 128.0 
, 80 / 128.0 , 81 / 128.0 , 82 / 128.0 , 83 / 128.0 , 84 / 128.0 , 85 / 128.0 , 86 / 128.0 , 87 / 128.0 , 88 / 128.0 , 89 / 128.0 , 90 / 128.0 , 91 / 128.0 , 92 / 128.0 , 93 / 128.0 , 94 / 128.0 , 95 / 128.0 
, 96 / 128.0 , 97 / 128.0 , 98 / 128.0 , 99 / 128.0 , 100 / 128.0 , 101 / 128.0 , 102 / 128.0 , 103 / 128.0 , 104 / 128.0 , 105 / 128.0 , 106 / 128.0 , 107 / 128.0 , 108 / 128.0 , 109 / 128.0 , 110 / 128.0 , 111 / 128.0 
, 112 / 128.0 , 113 / 128.0 , 114 / 128.0 , 115 / 128.0 , 116 / 128.0 , 117 / 128.0 , 118 / 128.0 , 119 / 128.0 , 120 / 128.0 , 121 / 128.0 , 122 / 128.0 , 123 / 128.0 , 124 / 128.0 , 125 / 128.0 , 126 / 128.0 , 127 / 128.0 };

//
//	Our wrapper is a simple classs
	rtlsdrHandler_win::rtlsdrHandler_win (QSettings *s,
	                                      const QString &recorderVersion,
	                                      logger *theLogger): // dummy now
	                                 _I_Buffer (8 * 1024 * 1024),
	                                 theFilter (5, 1560000 / 2, SAMPLERATE) {
int16_t	deviceCount;
int32_t	r;
int16_t	deviceIndex;
int16_t	i;
QString	temp;
int	k;
char	manufac [256], product [256], serial [256];
	(void)theLogger;
	rtlsdrSettings			= s;
	this	-> recorderVersion	= recorderVersion;
        setupUi (&myFrame);
	set_position_and_size (s, &myFrame, "rtlsdrSettings");
	myFrame. show();
	filtering			= false;

	currentDepth	=  value_i (rtlsdrSettings, "rtlsdrSettings",
	                                                 "filterDepth", 5);
	filterDepth	-> setValue (currentDepth);
	theFilter. resize (currentDepth);

	isActive. store (false);

//	Ok, from here we have the library functions accessible
	deviceCount 		= rtlsdr_get_device_count ();
	if (deviceCount == 0) {
	   throw (device_exception ("No device found\n"));
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
	r		=  rtlsdr_open (&theDevice, deviceIndex);
	if (r < 0) {
	   throw (device_exception ("Opening rtlsdr device failed"));
	}

	deviceModel	= rtlsdr_get_device_name (deviceIndex);
	deviceVersion	-> setText (deviceModel);
	QString	tunerType	= get_tunerType (rtlsdr_get_tuner_type (theDevice));
	product_display	-> setText (tunerType);

	r		= rtlsdr_set_sample_rate (theDevice, SAMPLERATE);
	if (r < 0) {
	   throw (device_exception ("Setting samplerate failed\n"));
	}

	gainsCount = rtlsdr_get_tuner_gains (theDevice, nullptr);
	fprintf (stderr, "Supported gain values (%d): ", gainsCount);
	{  int gains [gainsCount];
	   gainsCount	= rtlsdr_get_tuner_gains (theDevice, gains);
	   for (i = gainsCount; i > 0; i--) {
	      fprintf (stderr, "%.1f ", gains [i - 1] / 10.0);
	      gainControl -> addItem (QString::number (gains [i - 1]));
	   }
	   fprintf (stderr, "\n");
	}

	rtlsdr_set_center_freq (theDevice, 220000000);
	rtlsdr_set_tuner_bandwidth (theDevice, KHz (1575));
	rtlsdr_set_tuner_gain_mode (theDevice, 1);
//
//	See what the saved values are and restore the GUI settings
	temp	= value_s (rtlsdrSettings, "rtlsdrSettings",
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

//	and attach the buttons/sliders to the actions
	connect (gainControl,
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 2)
	         qOverload<const QString &>(&QComboBox::textActivated),
#else
	         qOverload<const QString &>(&QComboBox::activated),
#endif
	         this, &rtlsdrHandler_win::set_ExternalGain);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (agcControl, &QCheckBox::checkStateChanged,
#else
	connect (agcControl, &QCheckBox::stateChanged,
#endif
	         this, &rtlsdrHandler_win::set_autogain);
	connect (ppm_correction, qOverload<int>(&QSpinBox::valueChanged),
	         this, &rtlsdrHandler_win::set_ppmCorrection);
	connect (xml_dumpButton, &QPushButton::clicked,
	         this, &rtlsdrHandler_win::set_xmlDump);
	connect (iq_dumpButton, &QPushButton::clicked,
	         this, &rtlsdrHandler_win::set_iqDump);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (biasControl, &QCheckBox::checkStateChanged,
#else
	connect (biasControl, &QCheckBox::stateChanged,
#endif
	         this, &rtlsdrHandler_win::set_biasControl);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (filterSelector, &QCheckBox::checkStateChanged,
#else
	connect (filterSelector, &QCheckBox::stateChanged,
#endif
	         this, &rtlsdrHandler_win::set_filter);
//
//	and for saving/restoring the gain setting:
	connect (this, &rtlsdrHandler_win::new_gainIndex,
	         gainControl, &QComboBox::setCurrentIndex);
	connect (this, &rtlsdrHandler_win::new_agcSetting,
	         agcControl, &QCheckBox::setChecked);
	iqDumper	= nullptr;
	iq_dumping. store (false);
}

	rtlsdrHandler_win::~rtlsdrHandler_win	() {
	stopReader	();
	if (!workerHandle. isNull ()) {
	   rtlsdr_cancel_async (theDevice);
	   while (!workerHandle -> isFinished()) 
	      usleep (200);
	   _I_Buffer. FlushRingBuffer();
	   workerHandle. reset ();
//	   rtlsdr_close (theDevice);	// will crash if activated
	}

	QString gainText	= gainControl -> currentText ();
	store_widget_position (rtlsdrSettings, &myFrame, "rtlsdrSettings");
	store (rtlsdrSettings, "rtlsdrSettings",
	                "externalGain", gainText);
	store (rtlsdrSettings, "rtlsdrSettings",
	                "autogain", agcControl -> isChecked () ? 1 : 0);
	store (rtlsdrSettings, "rtlsdrSettings",
	                "ppm_correction", ppm_correction -> value());
	store (rtlsdrSettings, "rtlsdrSettings", 
	                "filterDepth", filterDepth -> value ());
	rtlsdrSettings	-> sync ();
	usleep (1000);
	myFrame. hide ();
}
//
void	rtlsdrHandler_win::set_filter	(int c) {
	(void)c;
	filtering       = filterSelector -> isChecked ();
}

bool	rtlsdrHandler_win::restartReader	(int32_t freq, int skipped) {
	_I_Buffer. FlushRingBuffer();

	(void)(rtlsdr_set_center_freq (theDevice, freq));
	if (save_gainSettings)
	   update_gainSettings (freq / MHz (1));

	lastFrequency	= freq;
	this -> toSkip = skipped;
	set_autogain (agcControl -> isChecked ());
	set_ExternalGain (gainControl -> currentText ());
	if (workerHandle. isNull ()) {
	   (void)rtlsdr_reset_buffer (theDevice);
	   workerHandle. reset (new dll_driver_win (this))
	}
	isActive. store (true);
	return true;
}

void	rtlsdrHandler_win::stopReader () {
	isActive. store (false);
	_I_Buffer. FlushRingBuffer();
	close_xmlDump ();
	if (save_gainSettings)
	   record_gainSettings	((int32_t)(rtlsdr_get_center_freq (theDevice)) / MHz (1));
}
//
//	when selecting  the gain from a table, use the table value
void	rtlsdrHandler_win::set_ExternalGain	(const QString &gain) {
	rtlsdr_set_tuner_gain (theDevice, gain. toInt());
}
//
void	rtlsdrHandler_win::set_autogain	(int dummy) {
	(void)dummy;
	rtlsdr_set_agc_mode (theDevice, agcControl -> isChecked () ? 1 : 0);
	rtlsdr_set_tuner_gain (theDevice, 
	                       gainControl -> currentText (). toInt ());
}
//
void	rtlsdrHandler_win::set_biasControl	(int dummy) {
	(void)dummy;
	rtlsdr_set_bias_tee (theDevice, biasControl -> isChecked () ? 1 : 0);
}
//	correction is in Hz
void	rtlsdrHandler_win::set_ppmCorrection	(int32_t ppm) {
	rtlsdr_set_freq_correction (theDevice, ppm);
}

int32_t	rtlsdrHandler_win::getSamples (std::complex<float> *V, int32_t size) { 
std::complex<uint8_t> temp [size];
int	amount;
static uint8_t dumpBuffer [4096];
static int iqTeller	= 0;

	if (!isActive. load ())
	   return 0;

	amount = _I_Buffer. getDataFromBuffer (temp, size);
	if (filtering) {
	   if (filterDepth -> value () != currentDepth) {
	      currentDepth = filterDepth -> value ();
	      theFilter. resize (currentDepth);
	   }
	   for (int i = 0; i < amount; i ++) 
	      V [i] = theFilter. Pass (
	               std::complex<float> (convTable [real (temp [i]) & 0xFF],
	                                    convTable [imag (temp [i]) & 0xFF]));
	}
	else
	   for (int i = 0; i < amount; i ++) 
	      V [i] = std::complex<float> (convTable [real (temp [i]) & 0xFF],
	                                   convTable [imag (temp [i]) & 0xFF]);
	if (!xml_writer. isNull ())
	   xmlWriter -> add (temp, amount);
	else
	if (iq_dumping. load ()) {
	   for (int i = 0; i < size; i ++) {
	      dumpBuffer [iqTeller]	= real (temp [i]);
	      dumpBuffer [iqTeller + 1]	= imag (temp [i]);
	      iqTeller += 2;
	      if (iqTeller >= 4096) {
	         fwrite (dumpBuffer, 1, 4096, iqDumper);
	         iqTeller = 0;
	      }
	   }
	}
	return amount;
}

int32_t	rtlsdrHandler_win::Samples () {
	if (!isActive. load ())
	   return 0;
	return _I_Buffer. GetRingBufferReadAvailable ();
}

void	rtlsdrHandler_win::resetBuffer() {
	_I_Buffer. FlushRingBuffer();
}

int16_t	rtlsdrHandler_win::maxGain() {
	return gainsCount;
}

int16_t	rtlsdrHandler_win::bitDepth() {
	return 8;
}

QString	rtlsdrHandler_win::deviceName	() {
	return deviceModel;
}

void	rtlsdrHandler_win::set_iqDump	() {
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

bool	rtlsdrHandler_win::setup_iqDump () {
	QString fileName = QFileDialog::getSaveFileName (nullptr,
	                                         tr ("Save file ..."),
	                                         QDir::homePath(),
	                                         tr ("raw (*.raw)"));
	fileName        = QDir::toNativeSeparators (fileName);
	iqDumper	= fopen (fileName. toUtf8 (). data (), "w");
	if (iqDumper == nullptr)
	   return false;
	iq_dumpButton	-> setText ("writing raw file");
	iq_dumping. store (true);
	return true;
}

void	rtlsdrHandler_win::close_iqDump () {
	if (iqDumper == nullptr)	// this can happen !!
	   return;
	iq_dumping. store (false);
	fclose (iqDumper);
	iq_dumpButton	-> setText ("Dump to raw");
	iqDumper	= nullptr;
}
	   
void	rtlsdrHandler_win::set_xmlDump () {
	if (xml_writer. isNull ())
	   if (setup_xmlDump (false)) 
	      iq_dumpButton	-> hide	();
	}
	else {
	   close_xmlDump ();
	   iq_dumpButton	-> show	();
	}
}

void	rtlsdrHandler_win::startDump	() {
	setup_xmlDump (true);
	iq_dumpButton	-> hide ();
}

void	rtlsdrHandler_win::stopDump	() {
	close_xmlDump ();
	iq_dumpButton		-> show ();
}

bool	rtlsdrHandler_win::setup_xmlDump (bool direct) {
QString channel		= rtlsdrSettings -> value ("channel", "xx").
	                                                      toString ();
	xmlWriter	= nullptr;
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
	return true;
}
	
void	rtlsdrHandler_win::close_xmlDump () {
	if (xmlWriter. isNull ())
	   return;
	usleep (1000);
	xmlWriter	-> computeHeader ();
	xmlWriter. reset ();
	xml_dumping. store (false);
	xml_dumpButton	-> setText ("Dump to xml");
}

////////////////////////////////////////////////////////////////////////
//
//      the frequency (the MHz component) is used as key
//
void    rtlsdrHandler_win::record_gainSettings (int freq) {
QString	gain	= gainControl	-> currentText ();
int	agc	= agcControl	-> isChecked () ? 1 : 0;
QString theValue        = gain + ":" + QString::number (agc);
QString key		= QString::number (freq);
	store (rtlsdrSettings, "rtlsdrSettings", key, theValue);
}

void	rtlsdrHandler_win::update_gainSettings (int freq) {
int	agc;
QString	theValue	= "";
QString freqS		= QString::number (freq);
	theValue	=  value_s (rtlsdrSettings, "rtlsdrSettings",
	                                              freqS, theValue);
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

QString	rtlsdrHandler_win::get_tunerType	(int tunerType) {
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

void	rtlsdrHandler_win::reportOverflow (bool ovf) {
static bool theOvf	= true;
	if (ovf && !theOvf){
	   overflowLabel -> setText ("Overload");
           overflowLabel -> setStyleSheet("QLabel {background-color : red;\
                                                   color: white}");
	   theOvf	= true;
	}
	else
	if (!ovf && theOvf) {		// space in the buffer is sufficient
	   overflowLabel -> setStyleSheet("QLabel {background-color : green;\
                                                   color: white}");
	   theOvf	= false;
	}
	else
	   theOvf = ovf;
}


