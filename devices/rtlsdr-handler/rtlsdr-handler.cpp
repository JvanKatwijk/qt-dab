#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB
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
#include	<QDir>
#include	"rtlsdr-handler.h"
#include	"rtl-dongleselect.h"
#include	"rtl-sdr.h"

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
void	RTLSDRCallBack (uint8_t *buf, uint32_t len, void *ctx) {
rtlsdrHandler	*theStick	= (rtlsdrHandler *)ctx;

	if ((theStick == NULL) || (len != READLEN_DEFAULT))
	   return;

	(void)theStick -> _I_Buffer -> putDataIntoBuffer (buf, len);
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
	start ();
	}

	~dll_driver (void) {
	}

private:
virtual void	run (void) {
	(theStick -> rtlsdr_read_async) (theStick -> device,
	                          (rtlsdr_read_async_cb_t)&RTLSDRCallBack,
	                          (void *)theStick,
	                          0,
	                          READLEN_DEFAULT);
	}
};
//
//	Our wrapper is a simple classs
	rtlsdrHandler::rtlsdrHandler (QSettings *s) {
int16_t	deviceCount;
int32_t	r;
int16_t	deviceIndex;
int16_t	i;
QString	temp;
int	k;

	rtlsdrSettings		= s;
	this	-> myFrame	= new QFrame (NULL);
	setupUi (this -> myFrame);
	this	-> myFrame	-> show ();
	inputRate		= 2048000;
	libraryLoaded		= false;
	open			= false;
	_I_Buffer		= NULL;
	workerHandle		= NULL;
	lastFrequency		= KHz (22000);	// just a dummy
	this	-> vfoOffset	= 0;
	gains			= NULL;

#ifdef	__MINGW32__
	const char *libraryString = "rtlsdr.dll";
	Handle		= LoadLibrary ((wchar_t *)L"rtlsdr.dll");
#else
	const char *libraryString = "librtlsdr.so";
	Handle		= dlopen ("librtlsdr.so", RTLD_NOW);
#endif

	if (Handle == NULL) {
	   fprintf (stderr, "failed to open %s\n", libraryString);
	   delete myFrame;
	   throw (20);
	}

	libraryLoaded	= true;
	if (!load_rtlFunctions ()) {
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
	   delete myFrame;
	   throw (21);
	}
//
//	Ok, from here we have the library functions accessible
	deviceCount 		= this -> rtlsdr_get_device_count ();
	if (deviceCount == 0) {
	   fprintf (stderr, "No devices found\n");
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
	   delete myFrame;
	   throw (22);
	}

	deviceIndex = 0;	// default
	if (deviceCount > 1) {
	   rtl_dongleSelect dongleSelector;
	   for (deviceIndex = 0; deviceIndex < deviceCount; deviceIndex ++) {
	      dongleSelector.
	           addtoDongleList (rtlsdr_get_device_name (deviceIndex));
	   }
	   deviceIndex = dongleSelector. QDialog::exec ();
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
	   delete myFrame;
	   throw (23);
	}

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
	   delete myFrame;
	   throw (24);
	}

	r			= this -> rtlsdr_get_sample_rate (device);
	fprintf (stderr, "samplerate set to %d\n", r);

	gainsCount = rtlsdr_get_tuner_gains (device, NULL);
	fprintf(stderr, "Supported gain values (%d): ", gainsCount);
	gains		= new int [gainsCount];
	gainsCount = rtlsdr_get_tuner_gains (device, gains);
	for (i = gainsCount; i > 0; i--) {
	   fprintf(stderr, "%.1f ", gains [i - 1] / 10.0);
	   combo_gain -> addItem (QString::number (gains [i - 1]));
	}
	fprintf(stderr, "\n");

	rtlsdr_set_tuner_gain_mode (device, 1);

	_I_Buffer		= new RingBuffer<uint8_t>(8 * 1024 * 1024);

	theGain		= gains [gainsCount / 2];	// default
//
//	See what the saved values are and restore the GUI settings
	rtlsdrSettings	-> beginGroup ("rtlsdrSettings");
	coarseOffset	= rtlsdrSettings -> value ("rtlsdrOffset", 0). toInt ();
	temp = rtlsdrSettings -> value ("externalGain", "10"). toString ();
	k	= combo_gain -> findText (temp);
	if (k != -1) {
	   combo_gain	-> setCurrentIndex (k);
	   theGain	= temp. toInt ();
	}

	temp	= rtlsdrSettings -> value ("autogain",
	                                      "autogain_on"). toString ();
	k	= combo_autogain -> findText (temp);
	if (k != -1) 
	   combo_autogain	-> setCurrentIndex (k);
	
	
	ppm_correction	-> setValue (rtlsdrSettings -> value ("ppm_correction", 0). toInt ());
	KhzOffset	-> setValue (rtlsdrSettings -> value ("KhzOffset", 0). toInt ());
	rtlsdrSettings	-> endGroup ();
//
//	all sliders/values are set to previous values, now do the settings
//	based on these slider values
	rtlsdr_set_tuner_gain_mode (device,
	                   combo_autogain -> currentText () == "autogain_on");
	if (combo_autogain -> currentText () == "autogain_on")
	   rtlsdr_set_agc_mode (device, 1);
	else
	   rtlsdr_set_agc_mode (device, 0);
	rtlsdr_set_tuner_gain	(device, theGain);
	set_ppmCorrection	(ppm_correction -> value ());
	set_KhzOffset		(KhzOffset -> value ());
//
	dumping			= false;
//	and attach the buttons/sliders to the actions
	connect (combo_gain, SIGNAL (activated (const QString &)),
	         this, SLOT (set_ExternalGain (const QString &)));
	connect (combo_autogain, SIGNAL (activated (const QString &)),
	         this, SLOT (set_autogain (const QString &)));
	connect (ppm_correction, SIGNAL (valueChanged (int)),
	         this, SLOT (set_ppmCorrection  (int)));
	connect (KhzOffset, SIGNAL (valueChanged (int)),
	         this, SLOT (set_KhzOffset (int)));
	connect (dumpButton, SIGNAL (clicked (void)),
	         this, SLOT (dumpButton_pressed (void)));
}

	rtlsdrHandler::~rtlsdrHandler	(void) {
	if (Handle == NULL) {	// nothing achieved earlier on
	   delete myFrame;
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

	stopReader ();
	rtlsdrSettings	-> beginGroup ("rtlsdrSettings");
	rtlsdrSettings	-> setValue ("rtlsdrOffset", coarseOffset);
	rtlsdrSettings	-> setValue ("externalGain",
	                                      combo_gain -> currentText ());
	rtlsdrSettings	-> setValue ("autogain",
	                                      combo_autogain -> currentText ());
	rtlsdrSettings	-> setValue ("ppm_correction",
	                                      ppm_correction -> value ());
	rtlsdrSettings	-> setValue ("KhzOffset",
	                                      KhzOffset	-> value ());
	rtlsdrSettings	-> sync ();
	rtlsdrSettings	-> endGroup ();
	
	this -> rtlsdr_close (device);
#ifdef __MINGW32__
	FreeLibrary (Handle);
#else
	dlclose (Handle);
#endif
	if (_I_Buffer != NULL)
	   delete _I_Buffer;
	if (gains != NULL)
	   delete[] gains;
	delete	myFrame;
}

void	rtlsdrHandler::setVFOFrequency	(int32_t f) {
	lastFrequency	= f;
	(void)(this -> rtlsdr_set_center_freq (device, f + vfoOffset));
}

int32_t	rtlsdrHandler::getVFOFrequency	(void) {
	return (int32_t)(this -> rtlsdr_get_center_freq (device)) - vfoOffset;
}
//
//
bool	rtlsdrHandler::restartReader	(void) {
int32_t	r;

	if (workerHandle != NULL)
	   return true;

	_I_Buffer	-> FlushRingBuffer ();
	r = this -> rtlsdr_reset_buffer (device);
	if (r < 0)
	   return false;

	this -> rtlsdr_set_center_freq (device, lastFrequency + vfoOffset);
	workerHandle	= new dll_driver (this);
	rtlsdr_set_agc_mode (device,
                combo_autogain -> currentText () == "autogain_on" ? 1 : 0);
	rtlsdr_set_tuner_gain (device, theGain);
	return true;
}

void	rtlsdrHandler::stopReader	(void) {
	if (workerHandle == NULL)
	   return;
	if (workerHandle != NULL) { // we are running
	   this -> rtlsdr_cancel_async (device);
	   if (workerHandle != NULL) {
	      while (!workerHandle -> isFinished ()) 
	         usleep (100);
	      delete	workerHandle;
	   }
	}
	workerHandle = NULL;
}
//
//	when selecting  the gain from a table, use the table value
void	rtlsdrHandler::set_ExternalGain	(const QString &gain) {
	theGain		= gain. toInt ();
	rtlsdr_set_tuner_gain (device, gain. toInt ());
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

void	rtlsdrHandler::set_KhzOffset	(int32_t o) {
	vfoOffset	= Khz (o);
	(void)(this -> rtlsdr_set_center_freq (device, lastFrequency + vfoOffset));
}

//
//	The brave old getSamples. For the dab stick, we get
//	size samples: still in I/Q pairs, but we have to convert the data from
//	uint8_t to DSPCOMPLEX *
int32_t	rtlsdrHandler::getSamples (std::complex<float> *V, int32_t size) { 
int32_t	amount, i;
uint8_t	*tempBuffer = (uint8_t *)alloca (2 * size * sizeof (uint8_t));
//
	amount = _I_Buffer	-> getDataFromBuffer (tempBuffer, 2 * size);
	if (dumping)
 	   fwrite (tempBuffer, amount, 1, dumpfilePointer);

	for (i = 0; i < amount / 2; i ++)
	    V [i] = std::complex<float>
	                       ((float (tempBuffer [2 * i] - 128)) / 128.0,
	                        (float (tempBuffer [2 * i + 1] - 128)) / 128.0);
	return amount / 2;
}

int32_t	rtlsdrHandler::Samples	(void) {
	return _I_Buffer	-> GetRingBufferReadAvailable () / 2;
}
//

bool	rtlsdrHandler::load_rtlFunctions (void) {
//
//	link the required procedures
	rtlsdr_open	= (pfnrtlsdr_open)
	                       GETPROCADDRESS (Handle, "rtlsdr_open");
	if (rtlsdr_open == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_open\n");
	   return false;
	}
	rtlsdr_close	= (pfnrtlsdr_close)
	                     GETPROCADDRESS (Handle, "rtlsdr_close");
	if (rtlsdr_close == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_close\n");
	   return false;
	}

	rtlsdr_set_sample_rate =
	    (pfnrtlsdr_set_sample_rate)GETPROCADDRESS (Handle, "rtlsdr_set_sample_rate");
	if (rtlsdr_set_sample_rate == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_sample_rate\n");
	   return false;
	}

	rtlsdr_get_sample_rate	=
	    (pfnrtlsdr_get_sample_rate)GETPROCADDRESS (Handle, "rtlsdr_get_sample_rate");
	if (rtlsdr_get_sample_rate == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_sample_rate\n");
	   return false;
	}

	rtlsdr_get_tuner_gains		= (pfnrtlsdr_get_tuner_gains)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_tuner_gains");
	if (rtlsdr_get_tuner_gains == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_tuner_gains\n");
	   return false;
	}


	rtlsdr_set_tuner_gain_mode	= (pfnrtlsdr_set_tuner_gain_mode)
	                     GETPROCADDRESS (Handle, "rtlsdr_set_tuner_gain_mode");
	if (rtlsdr_set_tuner_gain_mode == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_tuner_gain_mode\n");
	   return false;
	}

	rtlsdr_set_agc_mode	= (pfnrtlsdr_set_agc_mode)
	                     GETPROCADDRESS (Handle, "rtlsdr_set_agc_mode");
	if (rtlsdr_set_agc_mode == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_agc_mode\n");
	   return false;
	}

	rtlsdr_set_tuner_gain	= (pfnrtlsdr_set_tuner_gain)
	                     GETPROCADDRESS (Handle, "rtlsdr_set_tuner_gain");
	if (rtlsdr_set_tuner_gain == NULL) {
	   fprintf (stderr, "Cound not find rtlsdr_set_tuner_gain\n");
	   return false;
	}

	rtlsdr_get_tuner_gain	= (pfnrtlsdr_get_tuner_gain)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_tuner_gain");
	if (rtlsdr_get_tuner_gain == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_tuner_gain\n");
	   return false;
	}
	rtlsdr_set_center_freq	= (pfnrtlsdr_set_center_freq)
	                     GETPROCADDRESS (Handle, "rtlsdr_set_center_freq");
	if (rtlsdr_set_center_freq == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_center_freq\n");
	   return false;
	}

	rtlsdr_get_center_freq	= (pfnrtlsdr_get_center_freq)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_center_freq");
	if (rtlsdr_get_center_freq == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_center_freq\n");
	   return false;
	}

	rtlsdr_reset_buffer	= (pfnrtlsdr_reset_buffer)
	                     GETPROCADDRESS (Handle, "rtlsdr_reset_buffer");
	if (rtlsdr_reset_buffer == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_reset_buffer\n");
	   return false;
	}

	rtlsdr_read_async	= (pfnrtlsdr_read_async)
	                     GETPROCADDRESS (Handle, "rtlsdr_read_async");
	if (rtlsdr_read_async == NULL) {
	   fprintf (stderr, "Cound not find rtlsdr_read_async\n");
	   return false;
	}

	rtlsdr_get_device_count	= (pfnrtlsdr_get_device_count)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_device_count");
	if (rtlsdr_get_device_count == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_device_count\n");
	   return false;
	}

	rtlsdr_cancel_async	= (pfnrtlsdr_cancel_async)
	                     GETPROCADDRESS (Handle, "rtlsdr_cancel_async");
	if (rtlsdr_cancel_async == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_cancel_async\n");
	   return false;
	}

	rtlsdr_set_direct_sampling = (pfnrtlsdr_set_direct_sampling)
	                  GETPROCADDRESS (Handle, "rtlsdr_set_direct_sampling");
	if (rtlsdr_set_direct_sampling == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_direct_sampling\n");
	   return false;
	}

	rtlsdr_set_freq_correction = (pfnrtlsdr_set_freq_correction)
	                  GETPROCADDRESS (Handle, "rtlsdr_set_freq_correction");
	if (rtlsdr_set_freq_correction == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_freq_correction\n");
	   return false;
	}
	
	rtlsdr_get_device_name = (pfnrtlsdr_get_device_name)
	                  GETPROCADDRESS (Handle, "rtlsdr_get_device_name");
	if (rtlsdr_get_device_name == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_device_name\n");
	   return false;
	}

	fprintf (stderr, "OK, functions seem to be loaded\n");
	return true;
}

void	rtlsdrHandler::resetBuffer (void) {
	_I_Buffer -> FlushRingBuffer ();
}

int16_t	rtlsdrHandler::maxGain	(void) {
	return gainsCount;
}

int16_t	rtlsdrHandler::bitDepth	(void) {
	return 8;
}

void	rtlsdrHandler::dumpButton_pressed (void) {
	if (!dumping) {
	   QString file = QFileDialog::getSaveFileName (NULL,
	                                                tr ("Save file ..."),
	                                                QDir::homePath (),
	                                                tr ("iq file (*.iq)"));
	   if (file == QString (""))
	      return;
	   file		= QDir::toNativeSeparators (file);
	   if (!file.endsWith (".iq", Qt::CaseInsensitive))
	      file.append (".iq");
	   dumpfilePointer = fopen (file. toLatin1 (). data (), "w+b");
	   if (dumpfilePointer == NULL)
	      return;
	   dumpButton -> setText ("WRITING");
	   dumping = true;
	}
	else {
	   dumping = false;
	   fclose (dumpfilePointer);
	   dumpfilePointer = NULL;
	   dumpButton -> setText ("write raw bytes");
	}
}

