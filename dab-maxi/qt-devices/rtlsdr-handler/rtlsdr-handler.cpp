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
#include	<QFileDialog>
#include	<QTime>
#include	<QDate>
#include	<QDir>
#include	"rtlsdr-handler.h"
#include	"rtl-dongleselect.h"
#include	"rtl-sdr.h"
#include	"xml-filewriter.h"

#ifdef	__MINGW32__
#define	GETPROCADDRESS	GetProcAddress
#else
#define	GETPROCADDRESS	dlsym
#endif

#define	READLEN_DEFAULT	8192
//
//	For the callback, we do need some environment which
//	is passed through the ctx parameter
//
//	This is the user-side call back function
//	ctx is the calling task

static 
float mapTable [] = {
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

static
void	RTLSDRCallBack (uint8_t *buf, uint32_t len, void *ctx) {
rtlsdrHandler	*theStick	= (rtlsdrHandler *)ctx;

	if ((theStick == nullptr) || (len != READLEN_DEFAULT))
	   return;

	(void)theStick -> _I_Buffer ->
	             putDataIntoBuffer ((std::complex<uint8_t> *)buf, len / 2);
}
//
//	for handling the events in libusb, we need a controlthread
//	whose sole purpose is to process the rtlsdr_read_async function
//	from the lib.
class	dll_driver : public QThread {
private:
	rtlsdrHandler	*theStick;
public:

	dll_driver (rtlsdrHandler *d) {
	theStick	= d;
	start();
	}

	~dll_driver() {
	}

private:
virtual void	run () {
	(theStick -> rtlsdr_read_async) (theStick -> device,
	                          (rtlsdr_read_async_cb_t)&RTLSDRCallBack,
	                          (void *)theStick,
	                          0,
	                          READLEN_DEFAULT);
	}
};
//
//	Our wrapper is a simple classs
	rtlsdrHandler::rtlsdrHandler (QSettings *s,
	                                  QString &recorderVersion):
	                                         myFrame (nullptr) {
int16_t	deviceCount;
int32_t	r;
int16_t	deviceIndex;
int16_t	i;
QString	temp;
int	k;
char	manufac [256], product [256], serial [256];

	rtlsdrSettings			= s;
	this	-> recorderVersion	= recorderVersion;
	setupUi (&myFrame);
	myFrame. show();
	inputRate			= 2048000;
	libraryLoaded			= false;
	open			= false;
	_I_Buffer		= nullptr;
	workerHandle		= nullptr;
	lastFrequency		= KHz (22000);	// just a dummy
	gains			= nullptr;

#ifdef	__MINGW32__
	const char *libraryString = "rtlsdr.dll";
	Handle		= LoadLibrary ((wchar_t *)L"rtlsdr.dll");
	if (Handle == nullptr) {
	   fprintf (stderr, "failed to open %s (%d)\n", libraryString, GetLastError());
//	   delete myFrame;
	   throw (20);
	}
#else
	const char *libraryString = "librtlsdr.so";
	Handle		= dlopen ("librtlsdr.so", RTLD_NOW);

	if (Handle == nullptr) {
	   fprintf (stderr, "failed to open %s (%s)\n", libraryString, dlerror());
//	   delete myFrame;
	   throw (20);
	}
#endif
	libraryLoaded	= true;
	if (!load_rtlFunctions()) {
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
//	   delete myFrame;
	   throw (21);
	}
//
//	Ok, from here we have the library functions accessible
	deviceCount 		= this -> rtlsdr_get_device_count();
	if (deviceCount == 0) {
	   fprintf (stderr, "No devices found\n");
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
//	   delete myFrame;
	   throw (22);
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
	r			= this -> rtlsdr_open (&device, deviceIndex);
	if (r < 0) {
	   fprintf (stderr, "Opening rtlsdr device failed\n");
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
//	   delete myFrame;
	   throw (23);
	}
	deviceModel	= rtlsdr_get_device_name (deviceIndex);
	deviceVersion	-> setText (deviceModel);

	open			= true;
	r			= this -> rtlsdr_set_sample_rate (device,
	                                                          inputRate);
	if (r < 0) {
	   fprintf (stderr, "Setting samplerate failed\n");
	   rtlsdr_close (device);
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
//	   delete myFrame;
	   throw (24);
	}

	r			= this -> rtlsdr_get_sample_rate (device);
	
	gainsCount = rtlsdr_get_tuner_gains (device, nullptr);
	fprintf(stderr, "Supported gain values (%d): ", gainsCount);
	gains		= new int [gainsCount];
	gainsCount	= rtlsdr_get_tuner_gains (device, gains);
	for (i = gainsCount; i > 0; i--) {
	   fprintf(stderr, "%.1f ", gains [i - 1] / 10.0);
	   combo_gain -> addItem (QString::number (gains [i - 1]));
	}
	fprintf(stderr, "\n");

	rtlsdr_set_tuner_gain_mode (device, 1);
	rtlsdr_set_agc_mode (device, 0);

	_I_Buffer	= new RingBuffer<std::complex<uint8_t>>(4 * 1024 * 1024);
	theGain		= gains [gainsCount / 2];	// default
//
//	See what the saved values are and restore the GUI settings
	rtlsdrSettings	-> beginGroup ("rtlsdrSettings");
	coarseOffset	= rtlsdrSettings -> value ("rtlsdrOffset", 0). toInt();
	temp = rtlsdrSettings -> value ("externalGain", "10"). toString();
	k	= combo_gain -> findText (temp);
	if (k != -1) {
	   combo_gain	-> setCurrentIndex (k);
	   theGain	= temp. toInt();
	}

	temp	= rtlsdrSettings -> value ("autogain",
	                                      "autogain_on"). toString();
	k	= combo_autogain -> findText (temp);
	if (k != -1) 
	   combo_autogain	-> setCurrentIndex (k);
	
	
	ppm_correction	-> setValue (rtlsdrSettings -> value ("ppm_correction", 0). toInt());
	rtlsdrSettings	-> endGroup();

	rtlsdr_get_usb_strings (device, manufac, product, serial);
	fprintf (stderr, "%s %s %s\n",
	            manufac, product, serial);

//	all sliders/values are set to previous values, now do the settings
//	based on these slider values
	rtlsdr_set_tuner_gain_mode (device,
	                   combo_autogain -> currentText() == "autogain_on");
	if (combo_autogain -> currentText() == "autogain_on")
	   rtlsdr_set_agc_mode (device, 1);
	else
	   rtlsdr_set_agc_mode (device, 0);
	rtlsdr_set_tuner_gain	(device, theGain);
	set_ppmCorrection	(ppm_correction -> value());

//	and attach the buttons/sliders to the actions
	connect (combo_gain, SIGNAL (activated (const QString &)),
	         this, SLOT (set_ExternalGain (const QString &)));
	connect (combo_autogain, SIGNAL (activated (const QString &)),
	         this, SLOT (set_autogain (const QString &)));
	connect (ppm_correction, SIGNAL (valueChanged (int)),
	         this, SLOT (set_ppmCorrection  (int)));
	connect (xml_dumpButton, SIGNAL (clicked ()),
	         this, SLOT (set_xmlDump ()));
	connect (iq_dumpButton, SIGNAL (clicked ()),
	         this, SLOT (set_iqDump ()));
	xmlDumper       = nullptr;
	iqDumper	= nullptr;
	iq_dumping. store (false);
	xml_dumping. store (false);

}

	rtlsdrHandler::~rtlsdrHandler() {
	if (Handle == nullptr) {	// nothing achieved earlier on
//	   delete myFrame;
	   return;
	}
	
	if (!open) {	
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
	   return;
	}

	stopReader();
	rtlsdrSettings	-> beginGroup ("rtlsdrSettings");
	rtlsdrSettings	-> setValue ("rtlsdrOffset", coarseOffset);
	rtlsdrSettings	-> setValue ("externalGain",
	                                      combo_gain -> currentText());
	rtlsdrSettings	-> setValue ("autogain",
	                                      combo_autogain -> currentText());
	rtlsdrSettings	-> setValue ("ppm_correction",
	                                      ppm_correction -> value());
	rtlsdrSettings	-> sync();
	rtlsdrSettings	-> endGroup();
	
	this -> rtlsdr_close (device);
#ifdef __MINGW32__
	FreeLibrary (Handle);
#else
	dlclose (Handle);
#endif
	if (_I_Buffer != nullptr)
	   delete _I_Buffer;
	if (gains != nullptr)
	   delete[] gains;
//	delete	myFrame;
}

void	rtlsdrHandler::setVFOFrequency	(int32_t f) {
	lastFrequency	= f;
	(void)(this -> rtlsdr_set_center_freq (device, f));
}

int32_t	rtlsdrHandler::getVFOFrequency() {
	return (int32_t)(this -> rtlsdr_get_center_freq (device));
}
//
//
bool	rtlsdrHandler::restartReader	(int32_t freq) {
int32_t	r;

	lastFrequency	= freq;
	(void)(this -> rtlsdr_set_center_freq (device, freq));

	if (workerHandle != nullptr)
	   return true;

	_I_Buffer	-> FlushRingBuffer();
	r = this -> rtlsdr_reset_buffer (device);
	if (r < 0)
	   return false;

	this -> rtlsdr_set_center_freq (device, lastFrequency);
	rtlsdr_set_agc_mode (device,
                combo_autogain -> currentText() == "autogain_on" ? 1 : 0);
	rtlsdr_set_tuner_gain (device, theGain);
	workerHandle	= new dll_driver (this);
	return true;
}

void	rtlsdrHandler::stopReader () {
	if (workerHandle == nullptr)
	   return;
	close_xmlDump ();
	this -> rtlsdr_cancel_async (device);
	while (!workerHandle -> isFinished()) 
	   usleep (100);
	_I_Buffer -> FlushRingBuffer();
	delete	workerHandle;
	workerHandle = nullptr;
}
//
//	when selecting  the gain from a table, use the table value
void	rtlsdrHandler::set_ExternalGain	(const QString &gain) {
	theGain		= gain. toInt();
	rtlsdr_set_tuner_gain (device, gain. toInt());
}
//
void	rtlsdrHandler::set_autogain	(const QString &autogain) {
	rtlsdr_set_agc_mode (device, autogain == "autogain_off" ? 0 : 1);
	rtlsdr_set_tuner_gain (device, theGain);
}
//
//	correction is in Hz
void	rtlsdrHandler::set_ppmCorrection	(int32_t ppm) {
	this -> rtlsdr_set_freq_correction (device, ppm);
}

int32_t	rtlsdrHandler::getSamples (std::complex<float> *V, int32_t size) { 
std::complex<uint8_t> temp [size];
int	amount;

static uint8_t dumpBuffer [4096];
static int iqTeller	= 0;
	amount = _I_Buffer	-> getDataFromBuffer (temp, size);
	for (int i = 0; i < amount; i ++) 
	   V [i] = std::complex<float> (mapTable [real (temp [i])],
	                                mapTable [imag (temp [i])]);
	if (xml_dumping. load ())
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

int32_t	rtlsdrHandler::Samples() {
	return _I_Buffer	-> GetRingBufferReadAvailable ();
}
//

bool	rtlsdrHandler::load_rtlFunctions() {
//
//	link the required procedures
	rtlsdr_open	= (pfnrtlsdr_open)
	                       GETPROCADDRESS (Handle, "rtlsdr_open");
	if (rtlsdr_open == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_open\n");
	   return false;
	}

	rtlsdr_close	= (pfnrtlsdr_close)
	                     GETPROCADDRESS (Handle, "rtlsdr_close");
	if (rtlsdr_close == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_close\n");
	   return false;
	}

	rtlsdr_get_usb_strings =
	    (pfnrtlsdr_get_usb_strings)
	    GETPROCADDRESS (Handle, "rtlsdr_get_usb_strings");
	if (rtlsdr_get_usb_strings == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_get_usb_strings\n");
	   return false;
	}

	rtlsdr_set_sample_rate =
	    (pfnrtlsdr_set_sample_rate)GETPROCADDRESS (Handle, "rtlsdr_set_sample_rate");
	if (rtlsdr_set_sample_rate == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_set_sample_rate\n");
	   return false;
	}

	rtlsdr_get_sample_rate	=
	    (pfnrtlsdr_get_sample_rate)GETPROCADDRESS (Handle, "rtlsdr_get_sample_rate");
	if (rtlsdr_get_sample_rate == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_get_sample_rate\n");
	   return false;
	}

	rtlsdr_get_tuner_gains		= (pfnrtlsdr_get_tuner_gains)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_tuner_gains");
	if (rtlsdr_get_tuner_gains == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_get_tuner_gains\n");
	   return false;
	}


	rtlsdr_set_tuner_gain_mode	= (pfnrtlsdr_set_tuner_gain_mode)
	                     GETPROCADDRESS (Handle, "rtlsdr_set_tuner_gain_mode");
	if (rtlsdr_set_tuner_gain_mode == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_set_tuner_gain_mode\n");
	   return false;
	}

	rtlsdr_set_agc_mode	= (pfnrtlsdr_set_agc_mode)
	                     GETPROCADDRESS (Handle, "rtlsdr_set_agc_mode");
	if (rtlsdr_set_agc_mode == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_set_agc_mode\n");
	   return false;
	}

	rtlsdr_set_tuner_gain	= (pfnrtlsdr_set_tuner_gain)
	                     GETPROCADDRESS (Handle, "rtlsdr_set_tuner_gain");
	if (rtlsdr_set_tuner_gain == nullptr) {
	   fprintf (stderr, "Cound not find rtlsdr_set_tuner_gain\n");
	   return false;
	}

	rtlsdr_get_tuner_gain	= (pfnrtlsdr_get_tuner_gain)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_tuner_gain");
	if (rtlsdr_get_tuner_gain == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_get_tuner_gain\n");
	   return false;
	}
	rtlsdr_set_center_freq	= (pfnrtlsdr_set_center_freq)
	                     GETPROCADDRESS (Handle, "rtlsdr_set_center_freq");
	if (rtlsdr_set_center_freq == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_set_center_freq\n");
	   return false;
	}

	rtlsdr_get_center_freq	= (pfnrtlsdr_get_center_freq)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_center_freq");
	if (rtlsdr_get_center_freq == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_get_center_freq\n");
	   return false;
	}

	rtlsdr_reset_buffer	= (pfnrtlsdr_reset_buffer)
	                     GETPROCADDRESS (Handle, "rtlsdr_reset_buffer");
	if (rtlsdr_reset_buffer == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_reset_buffer\n");
	   return false;
	}

	rtlsdr_read_async	= (pfnrtlsdr_read_async)
	                     GETPROCADDRESS (Handle, "rtlsdr_read_async");
	if (rtlsdr_read_async == nullptr) {
	   fprintf (stderr, "Cound not find rtlsdr_read_async\n");
	   return false;
	}

	rtlsdr_get_device_count	= (pfnrtlsdr_get_device_count)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_device_count");
	if (rtlsdr_get_device_count == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_get_device_count\n");
	   return false;
	}

	rtlsdr_cancel_async	= (pfnrtlsdr_cancel_async)
	                     GETPROCADDRESS (Handle, "rtlsdr_cancel_async");
	if (rtlsdr_cancel_async == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_cancel_async\n");
	   return false;
	}

	rtlsdr_set_direct_sampling = (pfnrtlsdr_set_direct_sampling)
	                  GETPROCADDRESS (Handle, "rtlsdr_set_direct_sampling");
	if (rtlsdr_set_direct_sampling == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_set_direct_sampling\n");
	   return false;
	}

	rtlsdr_set_freq_correction = (pfnrtlsdr_set_freq_correction)
	                  GETPROCADDRESS (Handle, "rtlsdr_set_freq_correction");
	if (rtlsdr_set_freq_correction == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_set_freq_correction\n");
	   return false;
	}
	
	rtlsdr_get_device_name = (pfnrtlsdr_get_device_name)
	                  GETPROCADDRESS (Handle, "rtlsdr_get_device_name");
	if (rtlsdr_get_device_name == nullptr) {
	   fprintf (stderr, "Could not find rtlsdr_get_device_name\n");
	   return false;
	}

	fprintf (stderr, "OK, functions seem to be loaded\n");
	return true;
}

void	rtlsdrHandler::resetBuffer() {
	_I_Buffer -> FlushRingBuffer();
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

void	rtlsdrHandler::show		() {
	myFrame. show ();
}

void	rtlsdrHandler::hide		() {
	myFrame. hide ();
}

bool	rtlsdrHandler::isHidden		() {
	return myFrame. isHidden ();
}

void	rtlsdrHandler::set_iqDump	() {
	if (iqDumper == nullptr) {
	   if (setup_iqDump ()) {
              iq_dumpButton	-> setText ("writing raw file");
	      xml_dumpButton	-> hide ();
	   }
        }
        else {
           close_iqDump ();
           iq_dumpButton	-> setText ("Dump to raw");
	   xml_dumpButton	-> show ();
        }
}

bool	rtlsdrHandler::setup_iqDump () {
	QString fileName = QFileDialog::getSaveFileName (nullptr,
	                                         tr ("Save file ..."),
	                                         QDir::homePath(),
	                                         tr ("raw (*.raw)"));
        fileName        = QDir::toNativeSeparators (fileName);
        iqDumper	= fopen (fileName. toUtf8 (). data (), "w");
	if (iqDumper == nullptr)
	   return false;
	
	iq_dumping. store (true);
	return true;
}

void	rtlsdrHandler::close_iqDump () {
	if (iqDumper == nullptr)	// this can happen !!
	   return;
	iq_dumping. store (false);
	fclose (iqDumper);
	iqDumper	= nullptr;
}
	   
void	rtlsdrHandler::set_xmlDump () {
	if (!xml_dumping. load ()) {
	  if (setup_xmlDump ())
	      xml_dumpButton	-> setText ("writing xml file");
	  iq_dumpButton	-> hide	();
	}
	else {
	   close_xmlDump ();
	   xml_dumpButton	-> setText ("Dump to xml");
	   iq_dumpButton	-> show	();
	}
}

static inline
bool	isValid (QChar c) {
	return c. isLetterOrNumber () || (c == '-');
}

bool	rtlsdrHandler::setup_xmlDump () {
QTime	theTime;
QDate	theDate;
QString saveDir = rtlsdrSettings -> value ("saveDir_xmlDump",
                                           QDir::homePath ()). toString ();

	if (xml_dumping. load ())
	   return false;

        if ((saveDir != "") && (!saveDir. endsWith ("/")))
           saveDir += "/";

	QString channel		= rtlsdrSettings -> value ("channel", "xx").
	                                                      toString ();
	QString timeString      = theDate. currentDate (). toString () + "-" +
	                          theTime. currentTime(). toString ();
	for (int i = 0; i < timeString. length (); i ++)
	   if (!isValid (timeString. at (i)))
	      timeString. replace (i, 1, '-');
        QString suggestedFileName =
                    saveDir + deviceModel + "-" + channel + "-" + timeString;
	QString fileName =
	           QFileDialog::getSaveFileName (nullptr,
	                                         tr ("Save file ..."),
	                                         suggestedFileName + ".uff",
	                                         tr ("Xml (*.uff)"));
        fileName        = QDir::toNativeSeparators (fileName);
        xmlDumper	= fopen (fileName. toUtf8(). data(), "w");
	if (xmlDumper == nullptr)
	   return false;
	
	xmlWriter	= new xml_fileWriter (xmlDumper,
	                                      8,
	                                      "uint8",
	                                      2048000,
	                                      getVFOFrequency (),
	                                      "rtlsdr",
	                                      deviceModel,
	                                      recorderVersion);
	xml_dumping. store (true);

	QString	dumper	= QDir::fromNativeSeparators (fileName);
	int x		= dumper. lastIndexOf ("/");
	saveDir		= dumper. remove (x, dumper. count () - x);
	rtlsdrSettings	-> setValue ("saveDir_xmlDump", saveDir);

	return true;
}

void	rtlsdrHandler::close_xmlDump () {
	if (!xml_dumping. load ())
	   return;
	if (xmlDumper == nullptr)	// cannot happen
	   return;
	xml_dumping. store (false);
	usleep (1000);
	xmlWriter	-> computeHeader ();
	delete xmlWriter;
	fclose (xmlDumper);
	xmlDumper	= nullptr;
}

