#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of dab-mini
 *
 *    dab-mini is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation version 2 of the License.
 *
 *    dab-mini is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dab-mini if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	<QThread>
#include	<QSettings>
#include	<QHBoxLayout>
#include	<QLabel>
#include	"hackrf-handler.h"

#define	DEFAULT_GAIN	30

	hackrfHandler::hackrfHandler  (QSettings *s,
	                               QSpinBox	*vgaSelector,
	                               QSpinBox	*lnaSelector):
	                                 _I_Buffer (4 * 1024 * 1024) {
int	err;
int	res;
	hackrfSettings			= s;
	this	-> vgaSelector	= vgaSelector;
	vgaSelector	-> setToolTip ("vga gain, range 1 .. 62");
	vgaSelector	-> setRange   (1, 62);
	this	-> lnaSelector		= lnaSelector;
	lnaSelector	-> setToolTip ("lna gain, range 1 .. 40");
	lnaSelector	-> setRange (1, 40);
	this	-> inputRate		= Khz (2048);

#ifdef  __MINGW32__
        const char *libraryString = "libhackrf.dll";
        Handle          = LoadLibrary ((wchar_t *)L"libhackrf.dll");
#else
        const char *libraryString = "libhackrf.so";
        Handle          = dlopen (libraryString, RTLD_NOW);
#endif

	if (Handle == nullptr) {
	   fprintf (stderr, "failed to open %s\n", libraryString);
	   throw (20);
	}

        libraryLoaded   = true;
        if (!load_hackrfFunctions ()) {
#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
           dlclose (Handle);
#endif
           throw (21);
        }
//
//	See if there are settings from previous incarnations
	hackrfSettings		-> beginGroup ("hackrfSettings");
	lnaSelector 		-> setValue (
	            hackrfSettings -> value ("hack_lnaGain", 20). toInt ());
	vgaSelector 		-> setValue (
	            hackrfSettings -> value ("hack_vgaGain", 20). toInt ());

	hackrfSettings	-> endGroup ();

//
	res	= hackrf_init ();
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_init:");
	   fprintf (stderr, "%s \n", hackrf_error_name (hackrf_error (res)));
	   throw (21);
	}

	res	= hackrf_open (&theDevice);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_open:");
	   fprintf (stderr, "%s \n", hackrf_error_name (hackrf_error (res)));
	   throw (22);
	}

	res	= hackrf_set_sample_rate (theDevice, 2048000.0);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_set_samplerate:");
	   fprintf (stderr, "%s \n", hackrf_error_name (hackrf_error (res)));
	   throw (23);
	}

	res	= hackrf_set_baseband_filter_bandwidth (theDevice, 2000000);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_set_bw:");
	   fprintf (stderr, "%s \n", hackrf_error_name (hackrf_error (res)));
	   throw (24);
	}

	res	= hackrf_set_freq (theDevice, 220000000);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_set_freq: ");
	   fprintf (stderr, "%s \n", hackrf_error_name (hackrf_error (res)));
	   throw (25);
	}

	hackrf_set_lna_gain (theDevice, lnaSelector -> value ());
	hackrf_set_vga_gain (theDevice, vgaSelector -> value ());

//	and be prepared for future changes in the settings
	connect (lnaSelector, SIGNAL (valueChanged (int)),
	         this, SLOT (setLNAGain (int)));
	connect (vgaSelector, SIGNAL (valueChanged (int)),
	         this, SLOT (setVGAGain (int)));

	hackrf_device_list_t *deviceList = hackrf_device_list ();
	if (deviceList != NULL) {
	   char *serial = deviceList -> serial_numbers [0];
//	   serial_number_display -> setText (serial);
	   enum hackrf_usb_board_id board_id =
	                 deviceList -> usb_board_ids [0];
//	   usb_board_id_display -> setText (hackrf_usb_board_id_name (board_id));
	}

	connect	(this, SIGNAL (new_lnaGainValue (int)),
	         lnaSelector, SLOT (setValue (int)));
	connect (this, SIGNAL (new_vgaGainValue (int)),
	         vgaSelector, SLOT (setValue (int)));
	running. store (false);
}

	hackrfHandler::~hackrfHandler	(void) {
	stopReader ();
	hackrfSettings	-> beginGroup ("hackrfSettings");
	hackrfSettings	-> setValue ("hack_lnaGain",
	                                 lnaSelector -> value ());
	hackrfSettings -> setValue ("hack_vgaGain",
	                                 vgaSelector	-> value ());
	hackrfSettings	-> endGroup ();
	this    -> hackrf_close (theDevice);
	this    -> hackrf_exit ();

}
//

void	hackrfHandler::setLNAGain	(int newGain) {
int	res;
	if ((newGain <= 60) && (newGain >= 0)) {
	   res	= hackrf_set_lna_gain (theDevice, newGain);
	   if (res != HACKRF_SUCCESS) {
	      fprintf (stderr, "Problem with hackrf_lna_gain %d\n", res);
	      return;
	   }
	}
}

void	hackrfHandler::setVGAGain	(int newGain) {
int	res;
	if ((newGain <= 62) && (newGain >= 0)) {
	   res	= hackrf_set_vga_gain (theDevice, newGain);
	   if (res != HACKRF_SUCCESS) {
	      fprintf (stderr, "Problem with hackrf_vga_gain %d\n", res);
	      return;
	   }
	}
}

static std::complex<float>buffer [32 * 32768];
static
int	callback (hackrf_transfer *transfer) {
hackrfHandler *ctx = static_cast <hackrfHandler *>(transfer -> rx_ctx);
int	i;
//std::complex<float> buffer [transfer -> buffer_length / 2];
uint8_t *p	= transfer -> buffer;
RingBuffer<std::complex<float> > * q = &(ctx -> _I_Buffer);

	for (i = 0; i < transfer -> valid_length / 2; i ++) {
	   float re	= (int8_t)(p [2 * i]) / 128.0;
	   float im	= (int8_t)(p [2 * i + 1]) / 128.0;
	   buffer [i]	= std::complex<float> (re, im);
	}
	q	-> putDataIntoBuffer (buffer, transfer -> valid_length / 2);
	return 0;
}

bool	hackrfHandler::restartReader	(int32_t frequency) {
int	res;

//	if (hackrf_is_streaming (theDevice))
//	   return true;

	vfoFrequency	= frequency;
	update_gainSettings (frequency / MHz (1));
	hackrf_set_lna_gain (theDevice, lnaSelector -> value ());
        hackrf_set_vga_gain (theDevice, vgaSelector -> value ());

	res	= hackrf_set_freq (theDevice, frequency);
	res	= hackrf_start_rx (theDevice, callback, this);	
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_start_rx %d\n", res);
	   return false;
	}
	running. store (hackrf_is_streaming (theDevice));
	return running. load ();
}

void	hackrfHandler::stopReader	(void) {
int	res;

	if (!running. load ())
	   return;

	res	= hackrf_stop_rx (theDevice);
	record_gainSettings	(vfoFrequency / MHz (1));

	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_stop_rx %d\n", res);
	   return;
	}
	running. store (false);
}

//
//	The brave old getSamples. For the hackrf, we get
//	size still in I/Q pairs
int32_t	hackrfHandler::getSamples (std::complex<float> *V, int32_t size) { 
	return _I_Buffer. getDataFromBuffer (V, size);
}

int32_t	hackrfHandler::Samples	(void) {
	return _I_Buffer. GetRingBufferReadAvailable ();
}

void	hackrfHandler::resetBuffer	(void) {
	_I_Buffer. FlushRingBuffer ();
}

int16_t	hackrfHandler::bitDepth	(void) {
	return 8;
}


bool	hackrfHandler::load_hackrfFunctions (void) {
//
//	link the required procedures
	this -> hackrf_init	= (pfn_hackrf_init)
	                       GETPROCADDRESS (Handle, "hackrf_init");
	if (this -> hackrf_init == nullptr) {
	   fprintf (stderr, "Could not find hackrf_init\n");
	   return false;
	}

	this -> hackrf_open	= (pfn_hackrf_open)
	                       GETPROCADDRESS (Handle, "hackrf_open");
	if (this -> hackrf_open == nullptr) {
	   fprintf (stderr, "Could not find hackrf_open\n");
	   return false;
	}

	this -> hackrf_close	= (pfn_hackrf_close)
	                       GETPROCADDRESS (Handle, "hackrf_close");
	if (this -> hackrf_close == nullptr) {
	   fprintf (stderr, "Could not find hackrf_close\n");
	   return false;
	}

	this -> hackrf_exit	= (pfn_hackrf_exit)
	                       GETPROCADDRESS (Handle, "hackrf_exit");
	if (this -> hackrf_exit == nullptr) {
	   fprintf (stderr, "Could not find hackrf_exit\n");
	   return false;
	}

	this -> hackrf_start_rx	= (pfn_hackrf_start_rx)
	                       GETPROCADDRESS (Handle, "hackrf_start_rx");
	if (this -> hackrf_start_rx == nullptr) {
	   fprintf (stderr, "Could not find hackrf_start_rx\n");
	   return false;
	}

	this -> hackrf_stop_rx	= (pfn_hackrf_stop_rx)
	                       GETPROCADDRESS (Handle, "hackrf_stop_rx");
	if (this -> hackrf_stop_rx == nullptr) {
	   fprintf (stderr, "Could not find hackrf_stop_rx\n");
	   return false;
	}

	this -> hackrf_device_list	= (pfn_hackrf_device_list)
	                       GETPROCADDRESS (Handle, "hackrf_device_list");
	if (this -> hackrf_device_list == nullptr) {
	   fprintf (stderr, "Could not find hackrf_device_list\n");
	   return false;
	}

	this -> hackrf_set_baseband_filter_bandwidth	=
	                      (pfn_hackrf_set_baseband_filter_bandwidth)
	                      GETPROCADDRESS (Handle,
	                         "hackrf_set_baseband_filter_bandwidth");
	if (this -> hackrf_set_baseband_filter_bandwidth == nullptr) {
	   fprintf (stderr, "Could not find hackrf_set_baseband_filter_bandwidth\n");
	   return false;
	}

	this -> hackrf_set_lna_gain	= (pfn_hackrf_set_lna_gain)
	                       GETPROCADDRESS (Handle, "hackrf_set_lna_gain");
	if (this -> hackrf_set_lna_gain == nullptr) {
	   fprintf (stderr, "Could not find hackrf_set_lna_gain\n");
	   return false;
	}

	this -> hackrf_set_vga_gain	= (pfn_hackrf_set_vga_gain)
	                       GETPROCADDRESS (Handle, "hackrf_set_vga_gain");
	if (this -> hackrf_set_vga_gain == nullptr) {
	   fprintf (stderr, "Could not find hackrf_set_vga_gain\n");
	   return false;
	}

	this -> hackrf_set_freq	= (pfn_hackrf_set_freq)
	                       GETPROCADDRESS (Handle, "hackrf_set_freq");
	if (this -> hackrf_set_freq == nullptr) {
	   fprintf (stderr, "Could not find hackrf_set_freq\n");
	   return false;
	}

	this -> hackrf_set_sample_rate	= (pfn_hackrf_set_sample_rate)
	                       GETPROCADDRESS (Handle, "hackrf_set_sample_rate");
	if (this -> hackrf_set_sample_rate == nullptr) {
	   fprintf (stderr, "Could not find hackrf_set_sample_rate\n");
	   return false;
	}

	this -> hackrf_is_streaming	= (pfn_hackrf_is_streaming)
	                       GETPROCADDRESS (Handle, "hackrf_is_streaming");
	if (this -> hackrf_is_streaming == nullptr) {
	   fprintf (stderr, "Could not find hackrf_is_streaming\n");
	   return false;
	}

	this -> hackrf_error_name	= (pfn_hackrf_error_name)
	                       GETPROCADDRESS (Handle, "hackrf_error_name");
	if (this -> hackrf_error_name == nullptr) {
	   fprintf (stderr, "Could not find hackrf_error_name\n");
	   return false;
	}

	this -> hackrf_usb_board_id_name = (pfn_hackrf_usb_board_id_name)
	                       GETPROCADDRESS (Handle, "hackrf_usb_board_id_name");
	if (this -> hackrf_usb_board_id_name == nullptr) {
	   fprintf (stderr, "Could not find hackrf_usb_board_id_name\n");
	   return false;
	}

	fprintf (stderr, "OK, functions seem to be loaded\n");
	return true;
}


void	hackrfHandler::record_gainSettings	(int freq) {
int	vgaValue;
int	lnaValue;
int	ampEnable;
QString theValue;

	vgaValue	= vgaSelector	-> value ();
	lnaValue	= lnaSelector	-> value ();
	theValue	= QString::number (vgaValue) + ":";
	theValue	+= QString::number (lnaValue);
	hackrfSettings  -> beginGroup ("hackrfSettings");
        hackrfSettings	-> setValue (QString::number (freq), theValue);
        hackrfSettings -> endGroup ();
}

void	hackrfHandler::update_gainSettings	(int freq) {
int	vgaValue;
int	lnaValue;
QString	theValue	= "";

	hackrfSettings	-> beginGroup ("hackrfSettings");
	theValue	= hackrfSettings -> value (QString::number (freq), ""). toString ();
	hackrfSettings	-> endGroup ();

	if (theValue == QString (""))
	   return;		// or set some defaults here

	QStringList result	= theValue. split (":");
	if (result. size () != 2) 	// should not happen
	   return;

	vgaValue	= result. at (0). toInt ();
	lnaValue	= result. at (1). toInt ();

	vgaSelector	-> blockSignals (true);
	new_vgaGainValue (vgaValue);
	while (vgaSelector -> value () != vgaValue)
	   usleep (1000);
	vgaSelector	-> blockSignals (false);

	lnaSelector	-> blockSignals (true);
	new_lnaGainValue (lnaValue);
	while (lnaSelector -> value () != lnaValue)
	   usleep (1000);
	lnaSelector	-> blockSignals (false);
}

