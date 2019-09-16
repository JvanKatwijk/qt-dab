#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    Copyright (C) 2019 Amplifier, antenna and ppm correctors
 *    Fabio Capozzi
 *
 *    This file is part of Qt-DAB
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation version 2 of the License.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	<QThread>
#include	<QSettings>
#include	<QHBoxLayout>
#include	<QLabel>
#include	<QDebug>
#include	"hackrf-handler.h"

#define	DEFAULT_GAIN	30

	hackrfHandler::hackrfHandler  (QSettings *s) {
int	res;
	hackrfSettings			= s;
	this	-> myFrame		= new QFrame (nullptr);
	setupUi (this -> myFrame);
	this	-> myFrame	-> show();
	this	-> inputRate		= Khz (2048);
	_I_Buffer			= nullptr;

#ifdef  __MINGW32__
        const char *libraryString = "libhackrf.dll";
        Handle          = LoadLibrary ((wchar_t *)L"libhackrf.dll");
#elif  __clang__
        const char *libraryString = "/opt/local/lib/libhackrf.dylib";
        Handle = dlopen(libraryString,RTLD_NOW);
#else
        const char *libraryString = "libhackrf.so.0";
        Handle          = dlopen (libraryString, RTLD_NOW);
#endif

	if (Handle == nullptr) {
	   fprintf (stderr, "failed to open %s\n", libraryString);
	   delete myFrame;
	   throw (20);
	}

        libraryLoaded   = true;
        if (!load_hackrfFunctions()) {
#ifdef __MINGW32__
           FreeLibrary (Handle);
#else
           dlclose (Handle);
#endif
           delete myFrame;
           throw (21);
        }
//
//	From here we have a library available

	_I_Buffer	= new RingBuffer<std::complex<float>>(1024 * 1024);
	vfoFrequency	= Khz (220000);
//
//	See if there are settings from previous incarnations
	hackrfSettings		-> beginGroup ("hackrfSettings");
	lnagainSlider 		-> setValue (
	            hackrfSettings -> value ("hack_lnaGain", DEFAULT_GAIN). toInt());
	vgagainSlider 		-> setValue (
	            hackrfSettings -> value ("hack_vgaGain", DEFAULT_GAIN). toInt());
//	contributed by Fabio
	bool isChecked =
	    hackrfSettings -> value ("hack_AntEnable", false). toBool();
	AntEnableButton -> setCheckState (isChecked ? Qt::Checked :
	                                              Qt::Unchecked);
	isChecked	=
	   hackrfSettings -> value ("hack_AmpEnable", false). toBool();
	AmpEnableButton	-> setCheckState (isChecked ? Qt::Checked : 
                                                      Qt::Unchecked);
	ppm_correction      -> setValue (
	          hackrfSettings -> value ("hack_ppmCorrection", 0). toInt());
//	end

	hackrfSettings	-> endGroup();

//
	res	= this -> hackrf_init();
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_init:");
	   fprintf (stderr, "%s \n",
	                 this -> hackrf_error_name (hackrf_error (res)));
	   delete myFrame;
	   throw (21);
	}

	res	= this	-> hackrf_open (&theDevice);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_open:");
	   fprintf (stderr, "%s \n",
	                 this -> hackrf_error_name (hackrf_error (res)));
	   delete myFrame;
	   throw (22);
	}

	res	= this -> hackrf_set_sample_rate (theDevice, 2048000.0);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_set_samplerate:");
	   fprintf (stderr, "%s \n",
	                 this -> hackrf_error_name (hackrf_error (res)));
	   delete myFrame;
	   throw (23);
	}

	res	= this -> hackrf_set_baseband_filter_bandwidth (theDevice,
	                                                        1750000);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_set_bw:");
	   fprintf (stderr, "%s \n",
	                 this -> hackrf_error_name (hackrf_error (res)));
	   delete myFrame;
	   throw (24);
	}

	res	= this -> hackrf_set_freq (theDevice, 220000000);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_set_freq: ");
	   fprintf (stderr, "%s \n",
	                 this -> hackrf_error_name (hackrf_error (res)));
	   delete myFrame;
	   throw (25);
	}


	res = this -> hackrf_set_antenna_enable (theDevice, 1);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_set_antenna_enable: ");
	   fprintf (stderr, "%s \n",
                        this -> hackrf_error_name (hackrf_error (res)));
	   delete myFrame;
	   throw (26);
	}

	res = this -> hackrf_set_amp_enable (theDevice, 1);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_set_antenna_enable: ");
	   fprintf (stderr, "%s \n",
	                this -> hackrf_error_name (hackrf_error (res)));
	   delete myFrame;
	   throw (27);
	}

	uint16_t regValue;
	res = this -> hackrf_si5351c_read (theDevice, 162, &regValue);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_si5351c_read: ");
	   fprintf (stderr, "%s \n",
                     this -> hackrf_error_name (hackrf_error (res)));
	   delete myFrame;
	   throw (28);
	}

	res = this -> hackrf_si5351c_write (theDevice, 162, regValue);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_si5351c_write: ");
	   fprintf (stderr, "%s \n",
	              this -> hackrf_error_name (hackrf_error (res)));
	   delete myFrame;
	   throw (29);
	}

	setLNAGain	(lnagainSlider		-> value());
	setVGAGain	(vgagainSlider		-> value());
	EnableAntenna	(1);		// value is a dummy really
	EnableAmpli	(1);		// value is a dummy, really
	set_ppmCorrection (ppm_correction	-> value());

//	and be prepared for future changes in the settings
	connect (lnagainSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (setLNAGain (int)));
	connect (vgagainSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (setVGAGain (int)));
	connect (AntEnableButton, SIGNAL (stateChanged (int)),
	         this, SLOT (EnableAntenna (int)));
	connect (AmpEnableButton, SIGNAL (stateChanged (int)),
	         this, SLOT (EnableAmpli (int)));
	connect (ppm_correction, SIGNAL (valueChanged (int)),
	         this, SLOT (set_ppmCorrection  (int)));

	hackrf_device_list_t *deviceList = this -> hackrf_device_list();
	if (deviceList != nullptr) {	// well, it should be
	   char *serial = deviceList -> serial_numbers [0];
	   serial_number_display -> setText (serial);
	   enum hackrf_usb_board_id board_id =
	                 deviceList -> usb_board_ids [0];
	   usb_board_id_display ->
	                setText (this -> hackrf_usb_board_id_name (board_id));
	}

	running. store (false);
}

	hackrfHandler::~hackrfHandler() {
	stopReader();
	if (_I_Buffer != nullptr)
	   delete _I_Buffer;
	hackrfSettings	-> beginGroup ("hackrfSettings");
	hackrfSettings	-> setValue ("hack_lnaGain",
	                                 lnagainSlider -> value());
	hackrfSettings -> setValue ("hack_vgaGain",
	                                 vgagainSlider	-> value());
	hackrfSettings -> setValue ("hack_AntEnable",
	                              AntEnableButton -> checkState() == Qt::Checked);
	hackrfSettings -> setValue ("hack_AmpEnable",
	                              AmpEnableButton -> checkState() == Qt::Checked);
	hackrfSettings	-> setValue ("hack_ppmCorrection",
	                              ppm_correction -> value());
	hackrfSettings	-> endGroup();
	this	-> hackrf_close (theDevice);
	this	-> hackrf_exit();
	delete myFrame;
}
//

void	hackrfHandler::setVFOFrequency	(int32_t newFrequency) {
int	res;
	res	= this -> hackrf_set_freq (theDevice, newFrequency);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_set_freq: \n");
	   fprintf (stderr, "%s \n",
	                 this -> hackrf_error_name (hackrf_error (res)));
	   return;
	}

//
//	It seems that after changing the frequency, the preamp is switched off
	if (AmpEnableButton -> checkState() == Qt::Checked) 
	   EnableAmpli (1);
	vfoFrequency = newFrequency;
}

int32_t	hackrfHandler::getVFOFrequency() {
	return vfoFrequency;
}

void	hackrfHandler::setLNAGain	(int newGain) {
int	res;
	if ((newGain <= 40) && (newGain >= 0)) {
	   res	= this -> hackrf_set_lna_gain (theDevice, newGain);
	   if (res != HACKRF_SUCCESS) {
	      fprintf (stderr, "Problem with hackrf_lna_gain :\n");
	      fprintf (stderr, "%s \n",
	                 this -> hackrf_error_name (hackrf_error (res)));
	      return;
	   }
	   lnagainDisplay	-> display (newGain);
	}
}

void	hackrfHandler::setVGAGain	(int newGain) {
int	res;
	if ((newGain <= 62) && (newGain >= 0)) {
	   res	= this -> hackrf_set_vga_gain (theDevice, newGain);
	   if (res != HACKRF_SUCCESS) {
	      fprintf (stderr, "Problem with hackrf_vga_gain :\n");
	      fprintf (stderr, "%s \n",
	                 this -> hackrf_error_name (hackrf_error (res)));
	      return;
	   }
	   vgagainDisplay	-> display (newGain);
	}
}

void	hackrfHandler::EnableAntenna (int d) {
int res;
bool	b;

	(void)d;
	b = AntEnableButton	-> checkState() == Qt::Checked;
	res = this -> hackrf_set_antenna_enable (theDevice, b);
//	fprintf(stderr,"Passed %d\n",(int)b);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_set_antenna_enable :\n");
	   fprintf (stderr, "%s \n",
	                    this -> hackrf_error_name (hackrf_error (res)));
	   return;
	}
//	AntEnableButton -> setChecked (b);
}

void	hackrfHandler::EnableAmpli (int a) {
int res;
bool	b;

	(void)a;
	b = AmpEnableButton	-> checkState() == Qt::Checked;
	res = this -> hackrf_set_amp_enable (theDevice, b);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_set_amp_enable :\n");
	   fprintf (stderr, "%s \n",
	                   this -> hackrf_error_name (hackrf_error (res)));
	   return;
	}
//	AmpEnableButton->setChecked (b);
}

//      correction is in Hz
// This function has to be modified to implement ppm correction
// writing in the si5351 register does not seem to work yet
// To be completed

void	hackrfHandler::set_ppmCorrection	(int32_t ppm) {
int res;
uint16_t value;

	res = this -> hackrf_si5351c_write (theDevice,
	                                    162,
	                                    static_cast<uint16_t>(ppm));
	res = this -> hackrf_si5351c_read (theDevice,
	                                   162, &value);
	(void) res;
	qDebug() << "Read si5351c register 162 : " << value <<"\n";
}

//
//	we use a static large buffer, rather than trying to allocate
//	a buffer on the stack
static std::complex<float>buffer [32 * 32768];
static
int	callback (hackrf_transfer *transfer) {
hackrfHandler *ctx = static_cast <hackrfHandler *>(transfer -> rx_ctx);
int	i;
uint8_t *p	= transfer -> buffer;
RingBuffer<std::complex<float> > * q = ctx -> _I_Buffer;
int	bufferIndex	= 0;

	for (i = 0; i < transfer -> valid_length / 2; i += 1) {
	   float re	= (((int8_t *)p) [2 * i]) / 128.0;
	   float im	= (((int8_t *)p) [2 * i + 1]) / 128.0;
	   buffer [bufferIndex ++]	= std::complex<float> (re, im);
	}
	q -> putDataIntoBuffer (buffer, bufferIndex);
	return 0;
}

bool	hackrfHandler::restartReader() {
int	res;

	if (running. load())
	   return true;

	res	= this -> hackrf_start_rx (theDevice, callback, this);	
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_start_rx :\n");
	   fprintf (stderr, "%s \n",
	                 this -> hackrf_error_name (hackrf_error (res)));
	   return false;
	}
	running. store (this -> hackrf_is_streaming (theDevice));
	return running. load();
}

void	hackrfHandler::stopReader() {
int	res;

	if (!running. load())
	   return;

	res	= this -> hackrf_stop_rx (theDevice);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_stop_rx :\n");
	   fprintf (stderr, "%s \n",
	                 this -> hackrf_error_name (hackrf_error (res)));
	   return;
	}
	running. store (false);
}

//
//	The brave old getSamples. For the hackrf, we get
//	size still in I/Q pairs
int32_t	hackrfHandler::getSamples (std::complex<float> *V, int32_t size) { 
	return _I_Buffer	-> getDataFromBuffer (V, size);
}

int32_t	hackrfHandler::Samples() {
	return _I_Buffer	-> GetRingBufferReadAvailable();
}

void	hackrfHandler::resetBuffer() {
	_I_Buffer	-> FlushRingBuffer();
}

int16_t	hackrfHandler::bitDepth() {
	return 8;
}

bool	hackrfHandler::load_hackrfFunctions() {
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
// Aggiunta Fabio
	this -> hackrf_set_antenna_enable = (pfn_hackrf_set_antenna_enable)
	                  GETPROCADDRESS (Handle, "hackrf_set_antenna_enable");
	if (this -> hackrf_set_antenna_enable == nullptr) {
	   fprintf (stderr, "Could not find hackrf_set_antenna_enable\n");
	   return false;
	}

	this -> hackrf_set_amp_enable = (pfn_hackrf_set_amp_enable)
	                  GETPROCADDRESS (Handle, "hackrf_set_amp_enable");
	if (this -> hackrf_set_amp_enable == nullptr) {
	   fprintf (stderr, "Could not find hackrf_set_amp_enable\n");
	   return false;
	}

	this -> hackrf_si5351c_read = (pfn_hackrf_si5351c_read)
	                 GETPROCADDRESS (Handle, "hackrf_si5351c_read");
	if (this -> hackrf_si5351c_read == nullptr) {
	   fprintf (stderr, "Could not find hackrf_si5351c_read\n");
	   return false;
	}

	this -> hackrf_si5351c_write = (pfn_hackrf_si5351c_write)
	                 GETPROCADDRESS (Handle, "hackrf_si5351c_write");
	if (this -> hackrf_si5351c_write == nullptr) {
	   fprintf (stderr, "Could not find hackrf_si5351c_write\n");
	   return false;
	}

	fprintf (stderr, "OK, functions seem to be loaded\n");
	return true;
}
