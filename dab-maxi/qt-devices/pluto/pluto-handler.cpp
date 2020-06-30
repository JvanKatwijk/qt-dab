#
/*
 *    Copyright (C) 2014 .. 2020
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
#include	<QTime>
#include	<QDate>
#include	<QLabel>
#include	<QDebug>
#include	<QFileDialog>
#include	"pluto-handler.h"

/* static scratch mem for strings */
static char tmpstr[64];

/* helper function generating channel names */
static
char*	get_ch_name(const char* type, int id) {
        snprintf (tmpstr, sizeof(tmpstr), "%s%d", type, id);
        return tmpstr;
}

/* returns ad9361 phy device */
static
struct	iio_device* get_ad9361_phy (struct iio_context *ctx) {
struct iio_device *dev =  iio_context_find_device(ctx, "ad9361-phy");
	if (dev == nullptr) {
	   fprintf (stderr, "No ad9361-phy found, fatal\n");
	   throw (21);
	}
        return dev;
}

///* finds AD9361 streaming IIO devices */
//static
//bool	get_ad9361_stream_dev (struct iio_context *ctx,
//	                       struct iio_device **dev) {
//	*dev = iio_context_find_device (ctx, "cf-ad9361-lpc");
//	return *dev != nullptr;
//}

//static
//bool	get_phy_chan (struct iio_context *ctx,
//                      int chid, struct iio_channel **chn) {
//	*chn = iio_device_find_channel (get_ad9361_phy (ctx),
//                                        get_ch_name ("voltage", chid),
//                                        false);
//	return *chn != nullptr;
//}

///* finds AD9361 local oscillator IIO configuration channels */
//static
//bool    get_lo_chan (struct iio_context *ctx, struct iio_channel **chn) {
//	*chn = iio_device_find_channel (get_ad9361_phy (ctx),
//                                        get_ch_name ("altvoltage", 0),
//                                        true);
//	return *chn != nullptr;
//}

/* applies streaming configuration through IIO */
static
bool	cfg_ad9361_streaming_ch (struct iio_context *ctx,
	                         struct stream_cfg *cfg, int chid) {
struct iio_channel *chn = nullptr;

// Configure phy and lo channels
//	fprintf (stderr, "* Acquiring AD9361 phy channel %d\n", chid);
	chn = iio_device_find_channel (get_ad9361_phy (ctx),
                                        get_ch_name ("voltage", chid),
                                        false);
	if (chn == nullptr) {
	   fprintf (stderr, "cannot acquire phy channel %d\n", chid);
	   return false;
	}

	if (iio_channel_attr_write (chn, "rf_port_select",
	                                                cfg -> rfport) < 0) {
	   fprintf (stderr, "cannot select port\n");
	   return false;
	}

	if (iio_channel_attr_write_longlong (chn, "rf_bandwidth",
	                                                 cfg -> bw_hz) < 0) {
	   fprintf (stderr, "cannot select bandwidth\n");
	   return false;
	}

	if (iio_channel_attr_write_longlong (chn, "sampling_frequency",
	                                                 cfg -> fs_hz) < 0) {
	   fprintf (stderr, "cannot set sampling frequency\n");
	   return false;
	}

	cfg	-> gain_channel = chn;
// Configure LO channel
//	fprintf (stderr, "* Acquiring AD9361 %s lo channel\n", "RX");
	cfg -> lo_channel = iio_device_find_channel (get_ad9361_phy (ctx),
                                              get_ch_name ("altvoltage", 0),
                                              true);
	if (cfg -> lo_channel == nullptr) {
	   fprintf (stderr, "cannot find lo for channel\n");
	   return false;
	}

	if (iio_channel_attr_write_longlong (cfg -> lo_channel, "frequency",
	                                                 cfg -> lo_hz) < 0) {
	   fprintf (stderr, "cannot set local oscillator frequency\n");
	   return false;
	}
	return true;
}

/* finds AD9361 streaming IIO channels */
static
bool get_ad9361_stream_ch (__notused struct iio_context *ctx,
	                   struct iio_device *dev,
	                   int chid, struct iio_channel **chn) {
        *chn = iio_device_find_channel (dev,
	                                get_ch_name ("voltage", chid), false);
        if (*chn == nullptr)
	   *chn = iio_device_find_channel (dev,
	                                   get_ch_name ("altvoltage", chid),
	                                   false);
        return *chn != nullptr;
}

	plutoHandler::plutoHandler  (QSettings *s,
	                             QString &recorderVersion):
	                                  myFrame (nullptr),
	                                  _I_Buffer (4 * 1024 * 1024) {
	plutoSettings			= s;
	this	-> recorderVersion	= recorderVersion;
	setupUi (&myFrame);
	myFrame. show	();

	ctx				= nullptr;
	rxbuf				= nullptr;
	rx0_i				= nullptr;
	rx0_q				= nullptr;

	rxcfg. bw_hz			= PLUTO_RATE;
	rxcfg. fs_hz			= PLUTO_RATE;
	rxcfg. lo_hz			= 220000000;
	rxcfg. rfport			= "A_BALANCED";

	plutoSettings	-> beginGroup ("plutoSettings");
	bool agcMode	=
	             plutoSettings -> value ("pluto-agc", 0). toInt () == 1;
	int  gainValue	=
	             plutoSettings -> value ("pluto-gain", 33). toInt ();
	plutoSettings	-> endGroup ();
	if (agcMode) {
	   agcControl	-> setChecked (true);	
	   gainControl	-> hide ();
	}
	gainControl	-> setValue (gainValue);
	   
	state	-> setText ("Looking for context");
	ctx	= iio_create_default_context ();
	if (ctx == nullptr) {
//	   fprintf (stderr, "default context failed\n");
	   ctx = iio_create_local_context ();
	}
	else
	   state -> setText ("default context found");
	if (ctx == nullptr) {
//	   fprintf (stderr, "creating local context failed\n");
	   ctx = iio_create_network_context ("pluto.local");
	}
	else
	   state -> setText ("local context created");
	if (ctx == nullptr) {
	   fprintf (stderr, "creating network context with pluto.local failed\n");
	   ctx = iio_create_network_context ("192.168.2.1");
//	   ctx = iio_create_network_context ("qra.f5oeo.fr");
	}
	else
	   state -> setText ("network with pluto.local failed");

	if (ctx == nullptr) {
	   fprintf (stderr, "No pluto found, fatal\n");
	   throw (24);
	}

	state	-> setText ("Context created,\n counting devices");
//	fprintf (stderr, "context created, now counting devices\n");
	if (iio_context_get_devices_count (ctx) <= 0) {
	   state -> setText ("no devices found, fatal");
	   return;
	}

//	state	-> setText ("we have devices\n, Acquiring AD9361 streaming devices");
	rx = iio_context_find_device (ctx, "cf-ad9361-lpc");
	if (rx == nullptr) {
	   state -> setText ("No device found");
	   return;
	}

        state -> setText ("* Configuring AD9361 for streaming\n");
        if (!cfg_ad9361_streaming_ch (ctx, &rxcfg, 0)) {
	   state -> setText ("RX port 0 not found");
	   return;
	}

	state -> setText ("* Initializing AD9361 IIO streaming channels\n");
        if (!get_ad9361_stream_ch (ctx, rx, 0, &rx0_i)) {
	   state -> setText ("RX chan i not found");
	   return;
	}

        if (!get_ad9361_stream_ch (ctx, rx, 1, &rx0_q)) {
	   state -> setText ("RX chan q not found");
	   return;
	}

        state -> setText ("* Enabling IIO streaming channels");
        iio_channel_enable (rx0_i);
        iio_channel_enable (rx0_q);

        state -> setText ("* Creating non-cyclic IIO buffers with 1 MiS\n");
        rxbuf = iio_device_create_buffer (rx, 1024*1024, false);
	if (rxbuf == nullptr) {
	   state -> setText ("could not create RX buffer, fatal");
	   return;
	}
//
	iio_buffer_set_blocking_mode (rxbuf, true);
	if (!agcMode) {
	   int ret = iio_channel_attr_write (rxcfg. gain_channel,
	                                     "gain_control_mode", "manual");
	   ret = iio_channel_attr_write_longlong (rxcfg. gain_channel,
	                                          "hardwaregain", gainValue);
	   if (ret < 0) 
	      state -> setText ("error in initial gain setting");
	}
	else {
	   int ret = iio_channel_attr_write (rxcfg. gain_channel,
	                                     "gain_control_mode", "manual");
	   if (ret < 0)
	      state -> setText ("error in initial gain setting");
	}
	
//	and be prepared for future changes in the settings
	connect (gainControl, SIGNAL (valueChanged (int)),
	         this, SLOT (set_gainControl (int)));
	connect (agcControl, SIGNAL (stateChanged (int)),
	         this, SLOT (set_agcControl (int)));

	float	divider		= (float)DIVIDER;
	float	denominator	= DAB_RATE / divider;
	for (int i = 0; i < DAB_RATE / DIVIDER; i ++) {
           float inVal  = float (PLUTO_RATE / divider);
           mapTable_int [i]     =  int (floor (i * (inVal / denominator)));
           mapTable_float [i]   = i * (inVal / denominator) - mapTable_int [i];
        }
        convIndex       = 0;

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
	plutoSettings	-> endGroup ();
	if (!connected)
	   return;
	stopReader();
	iio_buffer_destroy (rxbuf);
	iio_context_destroy (ctx);
}
//

void	plutoHandler::setVFOFrequency	(int32_t newFrequency) {
int	ret;
	rxcfg. lo_hz = newFrequency;
	ret = iio_channel_attr_write_longlong
	                             (rxcfg. lo_channel,
	                                   "frequency", rxcfg. lo_hz);
	if (ret < 0) {
	   fprintf (stderr, "cannot set local oscillator frequency\n");
	}
//	fprintf (stderr, "frequency set to %d\n", rxcfg. lo_hz);
}

int32_t	plutoHandler::getVFOFrequency() {
	return rxcfg. lo_hz;
}
//
//	If the agc is set, but someone touches the gain button
//	the agc is switched off. Btw, this is hypothetically
//	since the gain control is made invisible when the
//	agc is set
void	plutoHandler::set_gainControl	(int newGain) {
int ret;

	if (agcControl -> isChecked ()) {
	   ret = iio_channel_attr_write (rxcfg. gain_channel,
	                                    "gain_control_mode", "manual");
	   if (ret < 0) {
	      state -> setText ("error in gain setting");
//	      fprintf (stderr, "could not change gain control to manual");
	      return;
	   }
	   
	   disconnect (agcControl, SIGNAL (stateChanged (int)),
	         this, SLOT (set_agcControl (int)));
	   agcControl -> setChecked (false);
	   connect (agcControl, SIGNAL (stateChanged (int)),
	         this, SLOT (set_agcControl (int)));
	}

	ret = iio_channel_attr_write_longlong (rxcfg. gain_channel,
	                                          "hardwaregain", newGain);
	if (ret < 0) {
	   state -> setText ("error in gain setting");
//	   fprintf (stderr,
//	           "could not set hardware gain to %d\n", newGain);
	}
}

void	plutoHandler::set_agcControl	(int dummy) {
int ret;

	(void)dummy;
	if (agcControl -> isChecked ()) {
	   ret = iio_channel_attr_write (rxcfg. gain_channel,
	                               "gain_control_mode", "slow_attack");
	   if (ret < 0) 
	      fprintf (stderr, "error in setting agc\n");
	   else
	      state -> setText ("agc set");
	   gainControl -> hide ();
	}
	else {	// switch agc off
	   ret = iio_channel_attr_write (rxcfg. gain_channel,
	                                    "gain_control_mode", "manual");
	   if (ret < 0) {
	      state -> setText ("error in gain setting");
	      return;
	   }
	   gainControl	-> show ();

	   ret = iio_channel_attr_write_longlong (rxcfg. gain_channel,
	                                          "hardwaregain", 
	                                          gainControl -> value ());
	   if (ret < 0) {
	      state -> setText ("error in gain setting");
	      fprintf (stderr,
	               "could not set hardware gain to %d\n",
	                                          gainControl -> value ());
	   }
	}
}

bool	plutoHandler::restartReader	(int32_t freq) {
//	fprintf (stderr, "restart called with %d\n", freq);
	if (!connected)
	   return false;
	if (running. load())
	   return true;		// should not happen

	setVFOFrequency (freq);
	start ();
	return true;
}

void	plutoHandler::stopReader() {
	if (!running. load())
	   return;
	running. store (false);
	while (isRunning())
	   usleep (500);
}

void	plutoHandler::run	() {
char	*p_end, *p_dat;
int	p_inc;
int	nbytes_rx;
std::complex<float> localBuf [DAB_RATE / DIVIDER];

	state -> setText ("running");
	running. store (true);
	while (running. load ()) {
	   nbytes_rx	= iio_buffer_refill	(rxbuf);
	   p_inc	= iio_buffer_step	(rxbuf);
	   p_end	= (char *)(iio_buffer_end  (rxbuf));

	   for (p_dat = (char *)iio_buffer_first (rxbuf, rx0_i);
	        p_dat < p_end; p_dat += p_inc) {
	      const int16_t i_p = ((int16_t *)p_dat) [0];
	      const int16_t q_p = ((int16_t *)p_dat) [1];
	      std::complex<float>sample = std::complex<float> (i_p / 2048.0,
	                                                       q_p / 2048.0);
	      convBuffer [convIndex ++] = sample;
	      if (convIndex > CONV_SIZE) {
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

int16_t	plutoHandler::bitDepth() {
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

