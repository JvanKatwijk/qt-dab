#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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
#include	<QFileDialog>
#include	<QTime>
#include	<QDate>
#include	<QLabel>
#include	<QDebug>
#include	<QFileDialog>
#include	"pluto-handler.h"
#include	"xml-filewriter.h"

#ifdef  __MINGW32__
#define GETPROCADDRESS  GetProcAddress
#else
#define GETPROCADDRESS  dlsym
#endif

/* static scratch mem for strings */
static char tmpstr[64];

/* helper function generating channel names */
static
char*	get_ch_name (const char* type, int id) {
        snprintf (tmpstr, sizeof(tmpstr), "%s%d", type, id);
        return tmpstr;
}

static 
QString get_ch_name (QString type, int id) {
QString result = type;
	result. append (QString::number (id));
	return result;
}

/* finds AD9361 streaming IIO channels */
bool	plutoHandler::
	      get_ad9361_stream_ch (struct iio_context *ctx,
	                            struct iio_device *dev,
	                            int chid, struct iio_channel **chn) {
	(void)ctx;
        *chn = this -> iio_device_find_channel (dev,
	                                       get_ch_name ("voltage", chid),
	                                       false);
        if (*chn == nullptr)
	   *chn = this ->  iio_device_find_channel (dev,
	                                   get_ch_name ("altvoltage", chid),
	                                   false);
        return *chn != nullptr;
}

	plutoHandler::plutoHandler  (QSettings *s,
	                             QString &recorderVersion):
	                                  myFrame (nullptr),
	                                  _I_Buffer (4 * 1024 * 1024) {
struct iio_channel *chn		= nullptr;
struct iio_device *phys_dev	= nullptr;

	plutoSettings			= s;
	this	-> recorderVersion	= recorderVersion;
	setupUi (&myFrame);
	myFrame. show	();

#ifdef	__MINGW32__
	const char *libraryString = "libiio.dll";
	Handle		= LoadLibrary ((wchar_t *)L"libiio.dll");
	if (Handle == nullptr) {
	   fprintf (stderr, "failed to open %s (%d)\n", libraryString, GetLastError());
//	   delete myFrame;
	   throw (20);
	}
#else
	const char *libraryString = "libiio.so";
	Handle		= dlopen ("libiio.so", RTLD_NOW);

	if (Handle == nullptr) {
	   fprintf (stderr, "failed to open %s (%s)\n", libraryString, dlerror());
//	   delete myFrame;
	   throw (20);
	}
#endif
	if (!load_plutoFunctions()) {
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
//	   delete myFrame;
	   throw (21);
	}

	fprintf (stderr, "functions are loaded\n");
	this	-> ctx			= nullptr;
	this	-> rxbuf		= nullptr;
	this	-> rx0_i		= nullptr;
	this	-> rx0_q		= nullptr;

	this	-> bw_hz		= PLUTO_RATE;
	this	-> fs_hz		= PLUTO_RATE;
	this	-> lo_hz		= 220000000;
	this	-> rfport		= "A_BALANCED";

	plutoSettings	-> beginGroup ("plutoSettings");
	bool agcMode	=
	             plutoSettings -> value ("pluto-agc", 0). toInt () == 1;
	int  gainValue	=
	             plutoSettings -> value ("pluto-gain", 50). toInt ();
	debugFlag	=
	             plutoSettings -> value ("pluto-debug", 0). toInt () == 1;
	plutoSettings	-> endGroup ();

	if (debugFlag)
	   debugButton	-> setText ("debug on");
	if (agcMode) {
	   agcControl	-> setChecked (true);	
	   gainControl	-> hide ();
	}
	gainControl	-> setValue (gainValue);
	   
	state	-> setText ("Looking for context");
	ctx	= this -> iio_create_default_context ();
	if (ctx == nullptr) {
	   if (debugFlag)
	      fprintf (stderr, "default context failed\n");
	   ctx = this -> iio_create_local_context ();
	}

	if (ctx == nullptr) {
	   if (debugFlag)
	      fprintf (stderr, "creating local context failed\n");
	   ctx = this -> iio_create_network_context ("pluto.local");
	}

	if (ctx == nullptr) {
	   if (debugFlag)
	      fprintf (stderr, "creating network context with pluto.local failed\n");
	   ctx = this -> iio_create_network_context ("192.168.2.1");
	}

	if (ctx == nullptr) {
	   fprintf (stderr, "No pluto found, fatal\n");
	   throw (24);
	}

	if (debugFlag)
	   fprintf (stderr, "context name found %s\n",
	                            this -> iio_context_get_name (ctx));

	state	-> setText ("Context created,\n looking for devices");
	if (this -> iio_context_get_devices_count (ctx) <= 0) {
	   state -> setText ("no devices found, fatal");
	   this -> iio_context_destroy	(ctx);
	   throw (25);
	}

	rx = this -> iio_context_find_device (ctx, "cf-ad9361-lpc");
	if (rx == nullptr) {
	   state -> setText ("No device found");
	   this -> iio_context_destroy (ctx);
	   throw (26);
	}

        state -> setText ("* Configuring AD9361 for streaming\n");

// Configure phy and lo channels
	if (debugFlag)
	   fprintf (stderr, "* Acquiring AD9361 phy channel %d\n", 0);
	phys_dev = this -> iio_context_find_device (ctx, "ad9361-phy");
	if (phys_dev == nullptr) {
	   if (debugFlag) 
	      fprintf (stderr, "no ad9361 found\n");
	   this -> iio_context_destroy (ctx);
	   throw (27);
	}

	chn = this -> iio_device_find_channel (phys_dev,
                                       get_ch_name (QString ("voltage"), 0).
	                                                  toLatin1 (). data (),
                                       false);
	if (chn == nullptr) {
	   if (debugFlag)
	      fprintf (stderr, "cannot acquire phy channel %d\n", 0);
	   this -> iio_context_destroy (ctx);
	   throw (27);
	}

	int res = this -> iio_channel_attr_write (chn, "rf_port_select",
	                                               this -> rfport);
	if (res < 0) {
	   if (debugFlag) {
	      char error [255];
	      this -> iio_strerror (res, error, 255); 
	      fprintf (stderr, "error in port selection %s\n", error);
	   }
	   this -> iio_context_destroy (ctx);
	   throw (28);
	}

	res = this -> iio_channel_attr_write_longlong (chn,
	                                               "rf_bandwidth",
	                                               this -> bw_hz);
	if (res < 0) {
	   if (debugFlag) {
	      char errorText [255];
	      this -> iio_strerror (res, errorText, 255); 
	      fprintf (stderr, "cannot select bandwidth %s\n", errorText);
	   }
	   this -> iio_context_destroy (ctx);
	   throw (29);
	}

	res = this -> iio_channel_attr_write_longlong (chn,
	                                               "sampling_frequency",
	                                               this -> fs_hz);
	if (res < 0) {
	   if (debugFlag) {
	      char errorText [255];
	      this -> iio_strerror (res, errorText, 255); 
	      fprintf (stderr, "cannot set sampling frequency %s\n", errorText);
	   }
	   this -> iio_context_destroy (ctx);
	   throw (30);
	}

	this	-> gain_channel = chn;

// Configure LO channel
	if (debugFlag)
	   fprintf (stderr, "* Acquiring AD9361 %s lo channel\n", "RX");
	phys_dev = this -> iio_context_find_device (ctx, "ad9361-phy");
//
	this -> lo_channel =
	             this -> iio_device_find_channel (phys_dev,
                                              get_ch_name ("altvoltage", 0),
                                              true);
	if (this -> lo_channel == nullptr) {
	   if (debugFlag)
	      fprintf (stderr, "cannot find lo for channel\n");
	   this -> iio_context_destroy (ctx);
	   throw (31);
	}

	res = this -> iio_channel_attr_write_longlong (this -> lo_channel,
	                                               "frequency",
	                                               this -> lo_hz);
	if (res < 0 ) {
	   if (debugFlag) {
	      char error [255];
	      this -> iio_strerror (res, error, 255); 
	      fprintf (stderr, "cannot set local oscillator frequency %s\n",
	                                                           error);
	   }
	   this -> iio_context_destroy (ctx);
	   throw (32);
	}

	state -> setText ("* Initializing AD9361 IIO streaming channels\n");
        if (!get_ad9361_stream_ch (ctx, rx, 0, &rx0_i)) {
	   if (debugFlag)
	      fprintf (stderr, "Rx chan i not found\n");
	   this -> iio_context_destroy (ctx);
	   throw (33);
	}

        if (!get_ad9361_stream_ch (ctx, rx, 1, &rx0_q)) {
	   if (debugFlag)
              fprintf (stderr, "Rx chan i not found\n");
           this -> iio_context_destroy (ctx);
           throw (34);
	}

        state -> setText ("* Enabling IIO streaming channels");
        this -> iio_channel_enable (rx0_i);
        this -> iio_channel_enable (rx0_q);

        state -> setText ("* Creating non-cyclic IIO buffers with 1 MiS\n");
        rxbuf = this -> iio_device_create_buffer (rx, 1024*1024, false);
	if (rxbuf == nullptr) {
	   if (debugFlag) 
	      fprintf (stderr, "could not create RX buffer, fatal\n");
	   this -> iio_context_destroy (ctx);
	   throw (35);
	}
//
	this -> iio_buffer_set_blocking_mode (rxbuf, true);
	if (!agcMode) {
	   int ret = this -> iio_channel_attr_write (this -> gain_channel,
	                                             "gain_control_mode",
	                                             "manual");
	   ret = this -> iio_channel_attr_write_longlong (this -> gain_channel,
	                                                  "hardwaregain",
	                                                  gainValue);
	   if (ret < 0) 
	      state -> setText ("error in initial gain setting");
	}
	else {
	   int ret = this -> iio_channel_attr_write (this -> gain_channel,
	                                             "gain_control_mode",
	                                             "slow_attack");
	   if (ret < 0)
	      state -> setText ("error in initial gain setting");
	}
	
//	and be prepared for future changes in the settings
	connect (gainControl, SIGNAL (valueChanged (int)),
	         this, SLOT (set_gainControl (int)));
	connect (agcControl, SIGNAL (stateChanged (int)),
	         this, SLOT (set_agcControl (int)));
	connect (debugButton, SIGNAL (clicked ()),
	         this, SLOT (toggle_debugButton ()));
	connect (dumpButton, SIGNAL (clicked ()),
	         this, SLOT (set_xmlDump ()));

	connect (this, SIGNAL (new_gainValue (int)),
	         gainControl, SLOT (setValue (int)));
	connect (this, SIGNAL (new_agcValue (bool)),
	         agcControl, SLOT (setChecked (bool)));
//	set up for interpolator
	float	denominator	= float (DAB_RATE) / DIVIDER;
        float inVal		= float (PLUTO_RATE) / DIVIDER;
	for (int i = 0; i < DAB_RATE / DIVIDER; i ++) {
           mapTable_int [i]	= int (floor (i * (inVal / denominator)));
	   mapTable_float [i] =
	                     i * (inVal / denominator) - mapTable_int [i];
        }
        convIndex       = 0;
	dumping. store	(false);
	xmlDumper	= nullptr;
	running. store (false);
	connected	= true;
	state -> setText ("ready to go");
}

	plutoHandler::~plutoHandler() {
	myFrame. hide ();
	plutoSettings	-> beginGroup ("plutoSettings");
	plutoSettings	-> setValue ("pluto-agcMode",
	                              agcControl -> isChecked () ? 1 : 0);
	plutoSettings	-> setValue ("pluto-gain", 
	                              gainControl -> value ());
	plutoSettings	-> setValue ("pluto-debug", debugFlag ? 1 : 0);
	plutoSettings	-> endGroup ();
	if (!connected)		// should not happen
	   return;
	stopReader();
	iio_buffer_destroy (rxbuf);
	iio_context_destroy (ctx);
}
//

void	plutoHandler::setVFOFrequency	(int32_t newFrequency) {
int	ret;
	this	-> lo_hz = newFrequency;
	ret = this -> iio_channel_attr_write_longlong (this -> lo_channel,
	                                               "frequency",
	                                               this -> lo_hz);
	if (ret < 0) {
	   fprintf (stderr, "cannot set local oscillator frequency\n");
	}
	if (debugFlag)
	   fprintf (stderr, "frequency set to %d\n",
	                                 (int)(this -> lo_hz));
}

int32_t	plutoHandler::getVFOFrequency () {
	return this -> lo_hz;
}
//
//	If the agc is set, but someone touches the gain button
//	the agc is switched off. Btw, this is hypothetically
//	since the gain control is made invisible when the
//	agc is set
void	plutoHandler::set_gainControl	(int newGain) {
int ret;

	if (agcControl -> isChecked ()) {
	   ret = this -> iio_channel_attr_write (this -> gain_channel,
	                                         "gain_control_mode",
	                                         "manual");
	   if (ret < 0) {
	      state -> setText ("error in gain setting");
	      if (debugFlag)
	         fprintf (stderr, "could not change gain control to manual");
	      return;
	   }
	   
	   disconnect (agcControl, SIGNAL (stateChanged (int)),
	         this, SLOT (set_agcControl (int)));
	   agcControl -> setChecked (false);
	   connect (agcControl, SIGNAL (stateChanged (int)),
	         this, SLOT (set_agcControl (int)));
	}

	ret = this -> iio_channel_attr_write_longlong (this -> gain_channel,
	                                               "hardwaregain",
	                                               newGain);
	if (ret < 0) {
	   state -> setText ("error in gain setting");
	   if (debugFlag) 
	      fprintf (stderr,
	               "could not set hardware gain to %d\n", newGain);
	}
}

void	plutoHandler::set_agcControl	(int dummy) {
int ret;

	(void)dummy;
	if (agcControl -> isChecked ()) {
	   ret = this -> iio_channel_attr_write (this -> gain_channel,
	                                         "gain_control_mode",
	                                         "slow_attack");
	   if (ret < 0) {
	      if (debugFlag)
	         fprintf (stderr, "error in setting agc\n");
	      return;
	   }
	   else
	      state -> setText ("agc set");
	   gainControl -> hide ();
	}
	else {	// switch agc off
	   ret = this -> iio_channel_attr_write (this -> gain_channel,
	                                         "gain_control_mode",
	                                         "manual");
	   if (ret < 0) {
	      state -> setText ("error in gain setting");
	      if (debugFlag)
	         fprintf (stderr, "error in gain setting\n");
	      return;
	   }
	   gainControl	-> show ();

	   ret = this -> iio_channel_attr_write_longlong (this -> gain_channel,
	                                                  "hardwaregain", 
	                                             gainControl -> value ());
	   if (ret < 0) {
	      state -> setText ("error in gain setting");
	      if (debugFlag)
	         fprintf (stderr,
	                  "could not set hardware gain to %d\n",
	                                          gainControl -> value ());
	   }
	}
}

bool	plutoHandler::restartReader	(int32_t freq) {
	if (debugFlag)
	   fprintf (stderr, "restart called with %d\n", freq);
	if (!connected)		// should not happen
	   return false;
	if (running. load())
	   return true;		// should not happen
#ifdef	__KEEP_GAIN_SETTINGS__
	update_gainSettings (freq /MHz (1));
#endif
	this -> lo_hz = freq;
	int ret = this -> iio_channel_attr_write_longlong
	                                              (this -> lo_channel,
	                                               "frequency",
	                                               this -> lo_hz);
	if (ret < 0) {
	   if (debugFlag)
	      fprintf (stderr, "cannot set local oscillator frequency\n");
	   return false;
	}
	else
	   start ();
	return true;
}

void	plutoHandler::stopReader() {
	if (!running. load())
	   return;
	close_xmlDump	();
#ifdef	__KEEP_GAIN_SETTINGS__
	record_gainSettings (this -> lo_hz/ MHz (1));
#endif
	running. store (false);
	while (isRunning())
	   usleep (500);
}

void	plutoHandler::run	() {
char	*p_end, *p_dat;
int	p_inc;
int	nbytes_rx;
std::complex<float> localBuf [DAB_RATE / DIVIDER];
std::complex<int16_t> dumpBuf [DAB_RATE / DIVIDER];

	state -> setText ("running");
	running. store (true);
	while (running. load ()) {
	   nbytes_rx	= this -> iio_buffer_refill	(rxbuf);
	   p_inc	= this -> iio_buffer_step	(rxbuf);
	   p_end	= (char *)this -> iio_buffer_end  (rxbuf);

	   for (p_dat = (char *)this -> iio_buffer_first (rxbuf, rx0_i);
	        p_dat < p_end; p_dat += p_inc) {
	      const int16_t i_p = ((int16_t *)p_dat) [0];
	      const int16_t q_p = ((int16_t *)p_dat) [1];
	      std::complex<int16_t>dumpS = std::complex<int16_t> (i_p, q_p);
	      dumpBuf [convIndex] = dumpS;
	      std::complex<float>sample = std::complex<float> (i_p / 2048.0,
	                                                       q_p / 2048.0);
	      convBuffer [convIndex ++] = sample;
	      if (convIndex > CONV_SIZE) {
	         if (dumping. load ())
	            xmlWriter -> add (dumpBuf, CONV_SIZE);
	         for (int j = 0; j < DAB_RATE / DIVIDER; j ++) {
	            int16_t inpBase	= mapTable_int [j];
	            float   inpRatio	= mapTable_float [j];
	            localBuf [j]	= cmul (convBuffer [inpBase + 1],
	                                                          inpRatio) +
                                     cmul (convBuffer [inpBase], 1 - inpRatio);
                 }
	         _I_Buffer. putDataIntoBuffer (localBuf,
	                                        DAB_RATE / DIVIDER);
	         convBuffer [0] = convBuffer [CONV_SIZE];
	         convIndex = 1;
	      }
	   }
	}
}

int32_t	plutoHandler::getSamples (std::complex<float> *V, int32_t size) { 
	if (!isRunning ())
	   return 0;
	return _I_Buffer. getDataFromBuffer (V, size);
}

int32_t	plutoHandler::Samples () {
	return _I_Buffer. GetRingBufferReadAvailable();
}

void	plutoHandler::resetBuffer() {
	_I_Buffer. FlushRingBuffer();
}

int16_t	plutoHandler::bitDepth () {
	return 12;
}

QString	plutoHandler::deviceName	() {
	return "ADALM PLUTO";
}

void	plutoHandler::show	() {
	myFrame. show	();
}

void	plutoHandler::hide	() {
	myFrame. hide	();
}

bool	plutoHandler::isHidden	() {
	return myFrame. isHidden ();
}

void	plutoHandler::toggle_debugButton	() {
	debugFlag	= !debugFlag;
	debugButton -> setText (debugFlag ? "debug on" : "debug off");
}

bool	plutoHandler::load_plutoFunctions	() {
	this -> iio_device_find_channel = (p_iio_device_find_channel)
	               GETPROCADDRESS (Handle, "iio_device_find_channel");
	if (this -> iio_device_find_channel == nullptr) {
	   fprintf (stderr, "could not find iio_device_find_channel\n");
	   return false;
	}
	this -> iio_create_default_context = (p_iio_create_default_context)
	               GETPROCADDRESS (Handle, "iio_create_default_context");
	if (this -> iio_create_default_context == nullptr) {
	   fprintf (stderr, "could not find iio_create_default_context\n");
	   return false;
	}
	this -> iio_create_local_context = (p_iio_create_local_context)
	               GETPROCADDRESS (Handle, "iio_create_local_context");
	if (this -> iio_create_local_context == nullptr) {
	   fprintf (stderr, "could not find iio_create_local_context\n");
	   return false;
	}
	this -> iio_create_network_context = (p_iio_create_network_context)
	               GETPROCADDRESS (Handle, "iio_create_network_context");
	if (this -> iio_create_network_context == nullptr) {
	   fprintf (stderr, "could not find iio_create_network_context\n");
	   return false;
	}
	this -> iio_context_get_name = (p_iio_context_get_name)
	               GETPROCADDRESS (Handle, "iio_context_get_name");
	if (this -> iio_context_get_name == nullptr) {
	   fprintf (stderr, "could not find iio_context_get_name\n");
	   return false;
	}
	this -> iio_context_get_devices_count =
	            (p_iio_context_get_devices_count)
	               GETPROCADDRESS (Handle, "iio_context_get_devices_count");
	if (this -> iio_context_get_devices_count == nullptr) {
	   fprintf (stderr, "could not find iio_context_get_devices_count\n");
	   return false;
	}
	this -> iio_context_find_device = (p_iio_context_find_device)
	               GETPROCADDRESS (Handle, "iio_context_find_device");
	if (this -> iio_context_find_device == nullptr) {
	   fprintf (stderr, "could not find iio_context_find_device\n");
	   return false;
	}
	this -> iio_device_attr_write = (p_iio_device_attr_write)
	               GETPROCADDRESS (Handle, "iio_device_attr_write");
	if (this -> iio_device_attr_write == nullptr) {
	   fprintf (stderr, "could not find iio_device_attr_write\n");
	   return false;
	}
	this -> iio_device_attr_write_longlong =
	                            (p_iio_device_attr_write_longlong)
	               GETPROCADDRESS (Handle, "iio_device_attr_write_longlong");
	if (this -> iio_device_attr_write_longlong == nullptr) {
	   fprintf (stderr, "could not find iio_device_attr_write_longlong\n");
	   return false;
	}
	this -> iio_channel_attr_write = (p_iio_channel_attr_write)
	               GETPROCADDRESS (Handle, "iio_channel_attr_write");
	if (this -> iio_channel_attr_write == nullptr) {
	   fprintf (stderr, "could not find iio_channel_attr_write\n");
	   return false;
	}
	this -> iio_channel_attr_write_longlong =
	                               (p_iio_channel_attr_write_longlong)
	             GETPROCADDRESS (Handle, "iio_channel_attr_write_longlong");
	if (this -> iio_channel_attr_write_longlong == nullptr) {
	   fprintf (stderr, "could not find iio_channel_attr_write_longlong\n");
	   return false;
	}
	this -> iio_channel_enable = (p_iio_channel_enable)
	             GETPROCADDRESS (Handle, "iio_channel_enable");
	if (this -> iio_channel_enable == nullptr) {
	   fprintf (stderr, "could not find iio_channel_enable\n");
	   return false;
	}
	this -> iio_device_create_buffer = (p_iio_device_create_buffer)
	             GETPROCADDRESS (Handle, "iio_device_create_buffer");
	if (this -> iio_device_create_buffer == nullptr) {
	   fprintf (stderr, "could not find iio_device_create_buffer\n");
	   return false;
	}
	this -> iio_buffer_set_blocking_mode = (p_iio_buffer_set_blocking_mode)
	             GETPROCADDRESS (Handle, "iio_buffer_set_blocking_mode");
	if (this -> iio_buffer_set_blocking_mode == nullptr) {
	   fprintf (stderr, "could not find iio_buffer_set_blocking_mode\n");
	   return false;
	}
	this -> iio_buffer_destroy = (p_iio_buffer_destroy)
	             GETPROCADDRESS (Handle, "iio_buffer_destroy");
	if (this -> iio_buffer_destroy == nullptr) {
	   fprintf (stderr, "could not find iio_buffer_destroy\n");
	   return false;
	}
	this -> iio_context_destroy = (p_iio_context_destroy)
	             GETPROCADDRESS (Handle, "iio_context_destroy");
	if (this -> iio_context_destroy == nullptr) {
	   fprintf (stderr, "could not find iio_context_destroy\n");
	   return false;
	}
	this -> iio_buffer_refill = (p_iio_buffer_refill)
	             GETPROCADDRESS (Handle, "iio_buffer_refill");
	if (this -> iio_buffer_refill == nullptr) {
	   fprintf (stderr, "could not find iio_buffer_refill\n");
	   return false;
	}
	this -> iio_buffer_step = (p_iio_buffer_step)
	             GETPROCADDRESS (Handle, "iio_buffer_step");
	if (this -> iio_buffer_step == nullptr) {
	   fprintf (stderr, "could not find iio_buffer_step\n");
	   return false;
	}
	this -> iio_buffer_end = (p_iio_buffer_end)
	             GETPROCADDRESS (Handle, "iio_buffer_end");
	if (this -> iio_buffer_end == nullptr) {
	   fprintf (stderr, "could not find iio_buffer_end\n");
	   return false;
	}
	this -> iio_buffer_first = (p_iio_buffer_first)
	             GETPROCADDRESS (Handle, "iio_buffer_first");
	if (this -> iio_buffer_first == nullptr) {
	   fprintf (stderr, "could not find iio_buffer_first\n");
	   return false;
	}
	this -> iio_strerror = (p_iio_strerror)
	             GETPROCADDRESS (Handle, "iio_strerror");
	if (this -> iio_strerror == nullptr) {
	   fprintf (stderr, "could not find iio_strerror\n");
	   return false;
	}
	return true;
}

void	plutoHandler::set_xmlDump () {
	if (xmlDumper == nullptr) {
	  if (setup_xmlDump ())
	      dumpButton	-> setText ("writing");
	}
	else {
	   close_xmlDump ();
	   dumpButton	-> setText ("Dump");
	}
}

static inline
bool	isValid (QChar c) {
	return c. isLetterOrNumber () || (c == '-');
}

bool	plutoHandler::setup_xmlDump () {
QTime	theTime;
QDate	theDate;
QString saveDir = plutoSettings -> value ("saveDir_xmlDump",
                                           QDir::homePath ()). toString ();
        if ((saveDir != "") && (!saveDir. endsWith ("/")))
           saveDir += "/";
	QString channel		= plutoSettings -> value ("channel", "xx").
	                                                     toString ();
        QString timeString      = theDate. currentDate (). toString () + "-" +
	                          theTime. currentTime (). toString ();
	for (int i = 0; i < timeString. length (); i ++)
	   if (!isValid (timeString. at (i)))
	      timeString. replace (i, 1, "-");
        QString suggestedFileName =
                saveDir + "pluto" + "-" + channel + "-" + timeString;
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
	                                      12,
	                                      "int16",
	                                      PLUTO_RATE,
	                                      getVFOFrequency (),
	                                      "pluto",
	                                      "I",
	                                      recorderVersion);
	dumping. store (true);

	QString dumper	= QDir::fromNativeSeparators (fileName);
	int x		= dumper. lastIndexOf ("/");
        saveDir		= dumper. remove (x, dumper. count () - x);
        plutoSettings	-> setValue ("saveDir_xmlDump", saveDir);
	return true;
}

void	plutoHandler::close_xmlDump () {
	if (xmlDumper == nullptr)	// this can happen !!
	   return;
	dumping. store (false);
	usleep (1000);
	xmlWriter	-> computeHeader ();
	delete xmlWriter;
	fclose (xmlDumper);
	xmlDumper	= nullptr;
}

void	plutoHandler::record_gainSettings (int freq) {
int	gainValue	= gainControl		-> value ();
int	agc		= agcControl		-> isChecked () ? 1 : 0;
QString theValue	= QString::number (gainValue) + ":" +
	                               QString::number (agc);

	plutoSettings	-> beginGroup ("plutoSettings");
	plutoSettings	-> setValue (QString::number (freq), theValue);
	plutoSettings	-> endGroup ();
}

void	plutoHandler::update_gainSettings (int freq) {
int	gainValue;
int	agc;
QString	theValue	= "";

	plutoSettings	-> beginGroup ("plutoSettings");
	theValue	= plutoSettings -> value (QString::number (freq), ""). toString ();
	plutoSettings	-> endGroup ();

	if (theValue == QString (""))
	   return;		// or set some defaults here

	QStringList result	= theValue. split (":");
	if (result. size () != 2) 	// should not happen
	   return;

	gainValue	= result. at (0). toInt ();
	agc		= result. at (1). toInt ();

	gainControl	-> blockSignals (true);
	new_gainValue (gainValue);
	while (gainControl -> value () != gainValue)
	   usleep (1000);
	gainControl	-> blockSignals (false);

	agcControl	-> blockSignals (true);
	new_agcValue (agc == 1);
	while (agcControl -> isChecked () != (agc == 1))
	   usleep (1000);
	agcControl	-> blockSignals (false);
}

