#
/*
 *    Copyright (C) 2014 .. 2025
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
#include	"errorlog.h"
#include	"settings-handler.h"

#define	READLEN_DEFAULT	(4 * 8192)

//
//	For the callback, we do need some environment which
//	is passed through the ctx parameter
//
//	This is the user-side call back function
//	ctx is the calling task

static
void	RTLSDRCallBack (uint8_t *buf, uint32_t len, void *ctx) {
rtlsdrHandler	*theStick	= (rtlsdrHandler *)ctx;

	if ((theStick == nullptr) || (len != READLEN_DEFAULT)) {
	   fprintf (stderr, "%d \n", len);
	   return;
	}

	if (!theStick -> isActive. load ())
	   return;
	static_cast<rtlsdrHandler *>(ctx) -> processBuffer (buf, len);
}
//
//	for handling the events in libusb, we need a controlthread
//	whose sole purpose is to process the rtlsdr_read_async function
//	from the lib.
class	dll_driver: public QThread {
private:
	rtlsdrHandler	*theStick;
public:

	dll_driver (rtlsdrHandler *d) {
	theStick	= d;
	start		();
}

	~dll_driver	() {
}

private:
void	run () {
	theStick -> rtlsdr_read_async (theStick -> theDevice,
	                   (rtlsdr_read_async_cb_t)&RTLSDRCallBack,
	                   (void *)theStick,
	                   0,
	                   READLEN_DEFAULT);
//	fprintf (stderr, "dll_task terminates\n");
	}
};

//
//	The driver, a - reasonably simple - class is used for 
//	both windows and linux. For Windows even two versions.
//	The driver is therefore parameterized on the library
//	it uses. The instantiator "knows" (we hope) the library
//	to be used.
//
	rtlsdrHandler::rtlsdrHandler (QSettings *s,
	                              const QString libraryString,
	                              int theVersion,
	                              const QString &recorderVersion,
	                              errorLogger	*theLogger):
	                                 _I_Buffer (8 * 1024 * 1024) {
int16_t	deviceCount;
int32_t	r;
int16_t	deviceIndex;
QString	temp;
int	k;
char	manufac [256], product [256], serial [256];

	rtlsdrSettings			= s;
	this	-> libraryString	= libraryString;
	storageName			= QString ("rtlsdrSettings-V") +
	                                       QString::number(theVersion);
	this	-> recorderVersion	= recorderVersion;
	this	-> theErrorLogger	= theLogger;
	setupUi (&myFrame);
//	setPositionAndSize (s, &myFrame, "rtlsdrSettings");
	myFrame. show ();
	isActive. store (false);
	previousGain		= 0;
	phandle = new QLibrary (libraryString);
	phandle -> load ();

	if (!phandle -> isLoaded ()) {
	   throw (device_exception (std::string ("failed to open ") + libraryString. toStdString ()));
	}

	if (!load_rtlFunctions ()) {
	   delete (phandle);
	   throw (device_exception ("could not load one or more essential library functions"));
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
	if (rtlsdr_get_tuner_i2c_register == nullptr) {
	   gainLabelText	-> hide ();
	   tunerGainLabel	-> hide ();
	}
 
//	OK, now open the hardware
	r		= this -> rtlsdr_open (&theDevice, deviceIndex);
	if (r < 0) {
	   delete phandle;
	   throw (device_exception ("Opening rtlsdr device failed"));
	}

	if (rtlsdr_get_version != nullptr) {
           uint32_t V = rtlsdr_get_version ();
           uint16_t major       = V >> 24;
           uint16_t minor       = (V & 0XFF0000) >> 16;
           uint8_t micro        = (V & 0xFF00) >> 8;
           uint8_t nano         = (V & 0XFF);
	   libVersion   -> setText (QString ("Library ") +
	                            QString::number (major) +
                                    ":" + QString::number (minor) +
                                    "-" + QString::number (micro) +
                                    ":" + QString::number (nano) +
	                            ":" + libraryString);
        }
        else
	   libVersion	-> setText (libraryString);

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

	QString tunerType	= get_tunerType (rtlsdr_get_tuner_type (theDevice));
	deviceModel	= rtlsdr_get_device_name (deviceIndex);
	deviceVersion	-> setText (deviceModel);
	product_display	-> setText (tunerType);

	int currentBandWidth	=
	           value_i (rtlsdrSettings, storageName, "bandwidth", 1750);
	bandwidth_selector	-> setValue (currentBandWidth);
	
	if (rtlsdr_set_tuner_bandwidth != nullptr) {
	   r = rtlsdr_set_tuner_bandwidth (theDevice, KHz (currentBandWidth));
	   if (r != 0) {
	      QString t = QString ("cannot set frequency ") +
	                               QString::number (currentBandWidth);
	      theErrorLogger -> add ("RTLSDR", t);
	   }
	}
//
//	we use the tickSignal as time counter (to update some info)
	connect (this, &rtlsdrHandler::tickSignal,
	         this, &rtlsdrHandler::handle_tickSignal);
//	See what the saved values are and restore the GUI settings
	temp =  value_s (rtlsdrSettings, storageName,
	                                         "externalGain", "10");
	k	= gainControl -> findText (temp);
	gainControl	-> setCurrentIndex (k != -1 ? k : gainsCount / 2);

	int agc = value_i (rtlsdrSettings, storageName, "agcMode", 1);
        init_autogain (agc);
	
	ppm_correction	->  
	     setValue (value_i (rtlsdrSettings, storageName,
	                                 "ppm_correction", 0));
	save_gainSettings	=
	     value_i (rtlsdrSettings, storageName,
	                             "save_gainSettings", 1) != 0;

	int res = this -> rtlsdr_set_center_freq (theDevice, 220000000);
	if (res != 0) {
	   QString t = QString ("cannot set frequency to ") +
	                                   QString::number (220000000);
	   theErrorLogger -> add ("RTLSDR", t);
	}

	rtlsdr_get_usb_strings (theDevice, manufac, product, serial);
	fprintf (stderr, "%s %s %s\n",
	            manufac, product, serial);

//	all sliders/values are set to previous values, now do the settings
//	based on these slider values

	res = rtlsdr_set_tuner_gain	(theDevice, 
	                         gainControl -> currentText (). toInt ());
	if (res != 0) {
	   QString t = QString ("cannot set tuner gain to ") +
	                                         gainControl -> currentText ();
	   theErrorLogger -> add ("RTLSDR", t);
	}
	set_ppmCorrection	(ppm_correction -> value());

	for (int i = 0; i < 256; i ++)
	   convTable [i] = (i - 127.38) / 128.0;

//	and attach the buttons/sliders to the actions
	connect (gainControl,
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 2)
	         qOverload<const QString &>(&QComboBox::textActivated),
#else
	         qOverload<const QString &>(&QComboBox::activated),
#endif
	         this, &rtlsdrHandler::set_ExternalGain);
	connect (ppm_correction, qOverload<double>(&QDoubleSpinBox::valueChanged),
	         this, &rtlsdrHandler::set_ppmCorrection);
	connect (agc_hw, &QRadioButton::toggled,
                 this, &rtlsdrHandler::set_hw_agc);
        connect (agc_sw, &QRadioButton::toggled,
                 this, &rtlsdrHandler::set_sw_agc);
        connect (agc_off, &QRadioButton::toggled,
                 this, &rtlsdrHandler::set_off_agc);
	connect (xml_dumpButton, &QPushButton::clicked,
	         this, &rtlsdrHandler::set_xmlDump);
	connect (iq_dumpButton, &QPushButton::clicked,
	         this, &rtlsdrHandler::set_iqDump);
#if QT_VERSION >= QT_VERSION_CHECK (6, 7, 0)
	connect (biasControl, &QCheckBox::checkStateChanged,
#else
	connect (biasControl, &QCheckBox::stateChanged,
#endif
	         this, &rtlsdrHandler::set_biasControl);
//
//	and for saving/restoring the gain setting:
	connect (this, &rtlsdrHandler::new_gainIndex,
	         gainControl, &QComboBox::setCurrentIndex);
	connect (bandwidth_selector, &QSpinBox::valueChanged,
	         this, &rtlsdrHandler::set_bandWidth);
	iqDumper	= nullptr;
	xmlWriter	= nullptr;
	iq_dumping. store (false);
	xml_dumping. store (false);
	workerHandle	= nullptr;
}

	rtlsdrHandler::~rtlsdrHandler () {
	stopReader	();

	if (workerHandle != nullptr) {
	   rtlsdr_cancel_async (theDevice);
	   while (!workerHandle -> isFinished ())
	      usleep (200);
	    _I_Buffer. FlushRingBuffer();
           delete       workerHandle;
           workerHandle = nullptr;
//         rtlsdr_close (theDevice);    // will crash if activated
        }

	storeWidgetPosition (rtlsdrSettings, &myFrame, storageName);
	QString gainText	= gainControl -> currentText ();
	store (rtlsdrSettings, storageName, "externalGain", gainText);
	int     agc     = agc_off -> isChecked () ? 1 :
                                  agc_sw  -> isChecked () ? 2 : 0;
        store (rtlsdrSettings, storageName, "agcMode", agc);
	store (rtlsdrSettings, storageName,
	                    "ppm_correction", ppm_correction -> value());
	rtlsdrSettings	-> sync ();
	if (xmlWriter != nullptr) {
	   delete xmlWriter;
	   xmlWriter = nullptr;
	}
	myFrame. hide ();
	usleep (1000);
	rtlsdr_close (theDevice);
	delete phandle;
}

//
bool	rtlsdrHandler::restartReader	(int32_t freq, int skipped) {
	int res =  this -> rtlsdr_set_center_freq (theDevice, freq);
	if (res != 0) {
           QString t = QString ("Cannot set frequency to ") +
                                            QString::number (freq);
           theErrorLogger -> add ("RTLSDR", t);
           return false;
        }

	if (save_gainSettings)
	   update_gainSettings (freq / MHz (1));

	lastFrequency	= freq;
	this -> toSkip = skipped;
	set_ExternalGain (gainControl -> currentText ());
	tunedFrequency	-> setText ("Tuned to " + QString::number(freq / KHz (1)) + " kHz");
	if (workerHandle == nullptr) {	// usualy it will be non zero
//	reset endpoint
	   (void)this -> rtlsdr_reset_buffer (theDevice);
	   workerHandle	= new dll_driver (this);
	}
	_I_Buffer. FlushRingBuffer ();
	isActive. store (true);
	return true;
}

void	rtlsdrHandler::stopReader () {
	isActive. store (false);
         _I_Buffer.FlushRingBuffer ();
        close_xmlDump();
	if (save_gainSettings)
	   record_gainSettings	((int32_t)(this -> rtlsdr_get_center_freq (theDevice)) / MHz (1));
}
//
//	when selecting  the gain from a table, use the table value
void	rtlsdrHandler::set_ExternalGain	(const QString &gain) {
	int res = rtlsdr_set_tuner_gain (theDevice, gain. toInt());
	if (res != 0) {
	   QString t = QString ("Cannot set gain to ") +
	                                    QString::number (gain. toInt ());
	   theErrorLogger -> add ("RTLSDR", t);
	}
}
//
void	rtlsdrHandler::init_autogain	(int level) {
	if (level == 1)
           agc_off      -> setChecked (true);
        else
        if (level == 2)
           agc_sw       -> setChecked (true);
        else    // level == 0
           agc_hw       -> setChecked (true);

        set_autogain (level);
}

void	rtlsdrHandler::set_hw_agc	() {
	set_autogain (0);
}

void	rtlsdrHandler::set_sw_agc	() {
	set_autogain (2);
}

void	rtlsdrHandler::set_off_agc	() {
	set_autogain (1);
}

void	rtlsdrHandler::set_autogain (int level) {
	rtlsdr_set_agc_mode (theDevice, level == 0);
	rtlsdr_set_tuner_gain_mode (theDevice, level);
	if (level == 1) {
	   gainControl -> setEnabled (true);
	   set_ExternalGain (gainControl -> currentText ());
	}
	else
	   gainControl -> setEnabled (false);
}
//
void	rtlsdrHandler::set_biasControl	(int dummy) {
	(void)dummy;
	if (rtlsdr_set_bias_tee != nullptr) {
	   int res =  rtlsdr_set_bias_tee (theDevice,
	                             biasControl -> isChecked () ? 1 : 0);
	   if (res != 0) {
	      QString t = QString ("Cannot set bias tee to ") +
	                             (biasControl -> isChecked () ? "1" : "0");
	      theErrorLogger -> add ("RTLSDR", t);
	   }
	}
}
//	correction is in Hz
void	rtlsdrHandler::set_ppmCorrection	(double ppm) {
int res;
        if (rtlsdr_set_freq_correction_ppb != nullptr)
           res = this -> rtlsdr_set_freq_correction_ppb (theDevice, ppm * 1000);
        else
           res = this -> rtlsdr_set_freq_correction (theDevice, ppm);

	if (res != 0) {
	   QString t = QString ("Cannot set ppm to ") +
	                                    QString::number (ppm);
	   theErrorLogger -> add ("RTLSDR", t);
	}
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
//	if (rtlsdr_set_freq_correction == nullptr) {
//	   fprintf (stderr, "Could not find rtlsdr_set_freq_correction\n");
//	   return false;
//	}

	rtlsdr_get_tuner_i2c_register =
	           (pfnrtlsdr_get_tuner_i2c_register)
	               phandle -> resolve ("rtlsdr_get_tuner_i2c_register");
	if (rtlsdr_get_tuner_i2c_register == nullptr) {
//	nullpointer - if function is not available - is handled
	}
	
	rtlsdr_get_device_name =
	           (pfnrtlsdr_get_device_name)
	               phandle -> resolve ("rtlsdr_get_device_name");
	if (rtlsdr_get_device_name == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_get_device_name\n");
	   return false;
	}

//	Optional function, null pointer is handled
	rtlsdr_set_tuner_bandwidth	=
	            (pfnrtlsdr_set_tuner_bandwidth)
	                phandle -> resolve ("rtlsdr_set_tuner_bandwidth");
	if (rtlsdr_set_tuner_bandwidth == nullptr) {
	   fprintf (stderr, "No tuner bandwidth selector\n");
//	   nullpointer - if function is not available - is handled
	}

//	Optional function, null pointer is handled
	rtlsdr_set_bias_tee =
	           (pfnrtlsdr_set_bias_tee)
	               phandle -> resolve ("rtlsdr_set_bias_tee");
//	if (rtlsdr_set_bias_tee == nullptr)
//	   fprintf (stderr, "biasControl will not work\n");
//	nullpointer - if function is not available - is handled
//
	rtlsdr_set_freq_correction_ppb =
	           (pfnrtlsdr_set_freq_correction_ppb)
	               phandle -> resolve ("rtlsdr_set_freq_correction_ppb");

//	Optional function, null pointer is handled
	rtlsdr_get_version =
	           (pfnrtlsdr_get_version)
	               phandle -> resolve ("rtlsdr_get_version");

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
	                                         QDir::homePath (),
	                                         "raw (*.raw)");
	fileName        = QDir::toNativeSeparators (fileName);
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

bool	rtlsdrHandler::setup_xmlDump (bool direct) {
QString channel		= value_s (rtlsdrSettings, "dab-general",
	                                   "channel", "xx");

	if (xmlWriter != nullptr)
	   return false;
	try {
	   xmlWriter	= new xml_fileWriter (rtlsdrSettings,
	                                      channel,
	                                      8,
	                                      "uint8",
	                                      SAMPLERATE,
	                                      lastFrequency,
	                                      rtlsdr_get_tuner_gain (theDevice),
	                                      "RTLSDR",
	                                      deviceModel,
	                                      recorderVersion,
	                                      direct);
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
	delete xmlWriter;
	xmlWriter	= nullptr;
	xml_dumpButton	-> setText ("Dump to xml");
	xml_dumping. store (false);
}

////////////////////////////////////////////////////////////////////////
//
//      the frequency (the MHz component) is used as key
//
void    rtlsdrHandler::record_gainSettings (int freq) {
QString	gain	= gainControl	-> currentText ();
int     agc     = agc_off -> isChecked () ? 1 :
                  agc_sw  -> isChecked () ? 2 : 0;
QString theValue        = gain + ":" + QString::number (agc);
QString freqVal		= QString::number (freq);
	store (rtlsdrSettings, storageName, freqVal, theValue);
}

void	rtlsdrHandler::update_gainSettings (int freq) {
int	agc;
QString	theValue	= "";
QString freqVal		= QString::number (freq);
	theValue	= value_s (rtlsdrSettings, storageName,
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
	   gainControl	-> setEnabled (false);
	   new_gainIndex (k);
	   while (gainControl -> currentText () != temp)
	      usleep (1000);
	}
	if (agc == 1) {
	   gainControl -> setEnabled (true);
	   agc_off	-> setEnabled (false);
	   agc_off	-> setChecked (true);
	   agc_off	-> setEnabled (true);
	}
	else
	if (agc == 0) {
	   agc_hw	-> setEnabled (false);
	   agc_hw	-> setChecked (true);
	   agc_hw	-> setEnabled (true);
	}
	else {	// agc == 2
	   agc_sw	-> setEnabled (false);
	   agc_sw	-> setChecked (true);
	   agc_sw	-> setEnabled (true);
	}
}

#define	IQ_BUFSIZE	4096
void	rtlsdrHandler::processBuffer (uint8_t *buf, uint32_t len) {
float	sumI	= 0;
float	sumQ	= 0;
int	nrSamples	= len / 2;
auto	*tempBuf 	= dynVec (std::complex<float>, nrSamples);
static uint8_t dumpBuffer [2 * IQ_BUFSIZE];
static int iqTeller	= 0;
static int teller	= 0;
	if (!isActive. load ()) 
	   return;

	if (toSkip > 0) {
	   toSkip -= len / 2;
	   return;
	}
	if (xml_dumping. load ())
	   xmlWriter -> add ((std::complex<uint8_t> *)buf, nrSamples);

	if (iq_dumping. load ()) {
	   for (int32_t i = 0; i < nrSamples; i ++) {
	      dumpBuffer [2 * iqTeller]	= buf [2 * i];
	      dumpBuffer [2 * iqTeller + 1] = buf [2 * i + 1];
	      iqTeller ++;
	      if (iqTeller >= IQ_BUFSIZE) {
	         fwrite (dumpBuffer, 2, IQ_BUFSIZE, iqDumper);
	         iqTeller = 0;
	      }
	   }
	}
	for (int32_t i = 0; i < nrSamples; i ++) {
	   teller ++;
	   float tempI	= convTable [buf [2 * i]];
	   float tempQ	= convTable [buf [2 * i + 1]];
	   sumI		+= tempI;
	   sumQ		+= tempQ;
	   tempBuf [i] = std::complex<float> (tempI, tempQ);
	}
// calculate correction values for next input buffer
	int ovf	= _I_Buffer. GetRingBufferWriteAvailable () - nrSamples;
	if (ovf < 0)
	   (void)_I_Buffer. putDataIntoBuffer (tempBuf, nrSamples + ovf);
	else
	   (void)_I_Buffer. putDataIntoBuffer (tempBuf, nrSamples);

	if (++teller >= 2048000) {
	   emit tickSignal  (ovf < 0);
	   teller = 0;
	}
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

void	rtlsdrHandler::handle_tickSignal (bool ovf) {
	reportOverflow (ovf);
	
	if (rtlsdr_get_tuner_i2c_register == nullptr) 
	   return;
	reportGain ();
}

void	rtlsdrHandler::reportOverflow (bool ovf) {
static bool theOvf	= true;
	if (ovf && !theOvf){
	   overflowLabel -> setText ("Overload");
           overflowLabel -> setStyleSheet (
	                 "QLabel {background-color : red; color: white}");
	   theOvf	= true;
	}
	else
	if (!ovf && theOvf) {		
	   overflowLabel -> setText (" ");
	   overflowLabel -> setStyleSheet("QLabel {background-color : green; \
                                                   color: white}");
	   theOvf	= false;
	}
	else
	   theOvf = ovf;
}

void	rtlsdrHandler::reportGain () {
	uint8_t  data [192];
	int32_t	reg_len;
	int32_t tuner_gain;
	int32_t result;

	result = rtlsdr_get_tuner_i2c_register (theDevice, data,
	                                            &reg_len, &tuner_gain);
	if (result != 0) 
	   return;

	tuner_gain = (tuner_gain + 5) / 10;
	if (previousGain != tuner_gain) {
//	   if (agcControl)
	      tunerGainLabel -> setText(QString::number(tuner_gain) + " dB");
//	   else
//	      tunerGain->setText("unknown");
	   previousGain = tuner_gain;
	}
}

void	rtlsdrHandler::set_bandWidth (int bW) { 
	if (rtlsdr_set_tuner_bandwidth == nullptr)
	   return;
	store (rtlsdrSettings, storageName, "bandwidth", bW);
	rtlsdr_set_tuner_bandwidth (theDevice, bW * 1000);
} 

