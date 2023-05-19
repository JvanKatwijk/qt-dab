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
#include	"device-exceptions.h"

//	Description for the fir-filter is here:
#include	"dabFilter.h"

/* static scratch mem for strings */
static char tmpstr[64];

/* helper function generating channel names */
static
char*	get_ch_name (const char* type, int id) {
        snprintf (tmpstr, sizeof(tmpstr), "%s%d", type, id);
        return tmpstr;
}

int	plutoHandler::
	ad9361_set_trx_fir_enable (struct iio_device *dev, int enable) {
int ret = iio_device_attr_write_bool (dev,
	                              "in_out_voltage_filter_fir_en",
	                              !!enable);
	if (ret < 0)
	   ret = iio_channel_attr_write_bool (
	                        iio_device_find_channel(dev, "out", false),
	                        "voltage_filter_fir_en", !!enable);
	return ret;
}

int	plutoHandler::
	ad9361_get_trx_fir_enable (struct iio_device *dev, int *enable) {
bool value;

	int ret = iio_device_attr_read_bool (dev,
	                                     "in_out_voltage_filter_fir_en",
	                                     &value);

	if (ret < 0)
	   ret = iio_channel_attr_read_bool (
	                        iio_device_find_channel (dev, "out", false),
	                        "voltage_filter_fir_en", &value);
	if (!ret)
	   *enable	= value;

	return ret;
}

/* returns ad9361 phy device */
struct iio_device* plutoHandler::get_ad9361_phy (struct iio_context *ctx) {
struct iio_device *dev = iio_context_find_device (ctx, "ad9361-phy");
	return dev;
}

/* finds AD9361 streaming IIO devices */
bool 	plutoHandler::get_ad9361_stream_dev (struct iio_context *ctx,
	                    enum iodev d, struct iio_device **dev) {
	switch (d) {
	case TX:
	   *dev = iio_context_find_device (ctx, "cf-ad9361-dds-core-lpc");
	   return *dev != NULL;

	case RX:
	   *dev = iio_context_find_device (ctx, "cf-ad9361-lpc");
	   return *dev != NULL;

	default: 
	   return false;
	}
}

/* finds AD9361 streaming IIO channels */
bool	plutoHandler::get_ad9361_stream_ch (__notused struct iio_context *ctx,
	                                    enum iodev d,
	                                    struct iio_device *dev,
	                                    int chid,
	                                    struct iio_channel **chn) {
	*chn = iio_device_find_channel (dev,
	                                get_ch_name ("voltage", chid),
	                                d == TX);
	if (!*chn)
	   *chn = iio_device_find_channel (dev,
	                                   get_ch_name ("altvoltage", chid),
	                                   d == TX);
	return *chn != NULL;
}

/* finds AD9361 phy IIO configuration channel with id chid */
bool	plutoHandler::get_phy_chan (struct iio_context *ctx,
	              enum iodev d, int chid, struct iio_channel **chn) {
	switch (d) {
	   case RX:
	      *chn = iio_device_find_channel (get_ad9361_phy (ctx),
	                                      get_ch_name ("voltage", chid),
	                                      false);
	      return *chn != NULL;

	   case TX:
	      *chn = iio_device_find_channel (get_ad9361_phy (ctx),
	                                      get_ch_name ("voltage", chid),
	                                      true);
	      return *chn != NULL;

	   default: 
	      return false;
	}
}

/* finds AD9361 local oscillator IIO configuration channels */
bool	plutoHandler::get_lo_chan (struct iio_context *ctx,
	             enum iodev d, struct iio_channel **chn) {
// LO chan is always output, i.e. true
	switch (d) {
	   case RX:
	      *chn = iio_device_find_channel (get_ad9361_phy (ctx),
	                                      get_ch_name ("altvoltage", 0),
	                                      true);
	      return *chn != NULL;

	   case TX:
	      *chn = iio_device_find_channel (get_ad9361_phy (ctx),
	                                      get_ch_name ("altvoltage", 1),
	                                      true);
	      return *chn != NULL;

	   default: 
	      return false;
	}
}

/* applies streaming configuration through IIO */
bool	plutoHandler::cfg_ad9361_streaming_ch (struct iio_context *ctx,
	                                       struct stream_cfg *cfg,
	                                       enum iodev type, int chid) {
struct iio_channel *chn = NULL;
int	ret;

// Configure phy and lo channels
	printf("* Acquiring AD9361 phy channel %d\n", chid);
	if (!get_phy_chan (ctx, type, chid, &chn)) {
	   return false;
	}
	ret = iio_channel_attr_write (chn,
	                              "rf_port_select", cfg -> rfport);
	if (ret < 0)
	   return false;
	ret = iio_channel_attr_write_longlong (chn,
	                                       "rf_bandwidth", cfg -> bw_hz);
	ret = iio_channel_attr_write_longlong (chn,
	                                       "sampling_frequency",
	                                       cfg -> fs_hz);

// Configure LO channel
	printf("* Acquiring AD9361 %s lo channel\n", type == TX ? "TX" : "RX");
	if (!get_lo_chan (ctx, type, &chn)) {
	   return false;
	}
	ret = iio_channel_attr_write_longlong (chn,
	                                       "frequency", cfg -> lo_hz);
	return true;
}

	plutoHandler::plutoHandler  (QSettings *s,
	                             QString &recorderVersion):
	                                  myFrame (nullptr),
	                                  _I_Buffer (4 * 1024 * 1024) {
	plutoSettings			= s;
	this	-> recorderVersion	= recorderVersion;
	setupUi (&myFrame);
	myFrame. show	();

#ifdef	__MINGW32__
	const char * libName = "libiio.dll";
#else
	const char * libName= "libiio.so";
#endif

	pHandle		= new QLibrary (libName);
	if (pHandle == nullptr) {
	   throw (new pluto_exception ("could not load " + std::string (libName));
	}

	pHandle -> load ();
	if (!pHandle -> isLoaded ()) {
	   throw (new pluto_exception ("Failed to open " + std::string (libName));
	}

	bool success			= loadFunctions ();
	if (!success) {
	   delete pHandle;
	   throw (new pluto_exception ("could not load all required lib functions"));
	}

	this	-> ctx			= nullptr;
	this	-> rxbuf		= nullptr;
	this	-> rx0_i		= nullptr;
	this	-> rx0_q		= nullptr;

	rx_cfg. bw_hz			= 1536000;
	rx_cfg. fs_hz			= PLUTO_RATE;
	rx_cfg. lo_hz			= 220000000;
	rx_cfg. rfport			= "A_BALANCED";

	plutoSettings	-> beginGroup ("plutoSettings");
	bool agcMode	=
	             plutoSettings -> value ("pluto-agc", 0). toInt () == 1;
	int  gainValue	=
	             plutoSettings -> value ("pluto-gain", 50). toInt ();
	debugFlag	=
	             plutoSettings -> value ("pluto-debug", 0). toInt () == 1;
	save_gainSettings =
	             plutoSettings -> value ("save_gainSettings", 1). toInt () != 0;
	filterOn	= true;
	plutoSettings	-> endGroup ();

	if (debugFlag)
	   debugButton	-> setText ("debug on");
	if (agcMode) {
	   agcControl	-> setChecked (true);	
	   gainControl	-> hide ();
	}
	gainControl	-> setValue (gainValue);
//
//	step 1: establish a context
//
	ctx	= iio_create_default_context ();
	if (ctx == nullptr) {
	   ctx = iio_create_local_context ();
	}

	if (ctx == nullptr) {
	   ctx = iio_create_network_context ("pluto.local");
	}

	if (ctx == nullptr) {
	   ctx = iio_create_network_context ("192.168.2.1");
	}

	if (ctx == nullptr) {
	   fprintf (stderr, "No pluto found, fatal\n");
	   throw (new pluto_exception ("No pluto device detected"));
	}
//
	
	if (iio_context_get_devices_count (ctx) <= 0) {
	   throw (new pluto_exception ("no pluto device detected"));
	}
//	
	fprintf (stderr, "* Acquiring AD9361 streaming devices\n");
	if (!get_ad9361_stream_dev (ctx, RX, &rx)) {
	   throw (new pluto_exception ("No RX device found"));
	}

	struct iio_channel *chn;
	fprintf (stderr, "switching off TX\n");
	if (get_phy_chan (ctx, TX, 0, &chn)) 
	   iio_channel_attr_write_longlong (chn, "powerdown", 1);

	fprintf (stderr, "* Configuring AD9361 for streaming\n");
	if (!cfg_ad9361_streaming_ch (ctx, &rx_cfg, RX, 0)) {
	   throw (new pluto_exception ("RX port 0 not found"));
	}

	if (get_phy_chan (ctx, RX, 0, &chn)) {
	   int ret;
	   if (agcMode)
	      ret = iio_channel_attr_write (chn,
	                                    "gain_control_mode",
	                                    "slow_attack");
	   else {
	      ret = iio_channel_attr_write (chn,
	                                    "gain_control_mode",
	                                    "manual");
	      ret = iio_channel_attr_write_longlong (chn,
	                                             "hardwaregain",
	                                             gainValue);
	   }

	   if (ret < 0)
	      throw (new pluto_exception ("setting agc/gain did not work"));
	}

	fprintf (stderr, "* Initializing AD9361 IIO streaming channels\n");
	if (!get_ad9361_stream_ch (ctx, RX, rx, 0, &rx0_i)) {
	   throw (new pluto_exception ("RX I channel not found"));
	}
	
	if (!get_ad9361_stream_ch (ctx, RX, rx, 1, &rx0_q)) {
	   throw (new pluto_exception ("RX  Q channel not found"));
	}
	
        iio_channel_enable (rx0_i);
        iio_channel_enable (rx0_q);

        rxbuf = iio_device_create_buffer (rx, 256*1024, false);
	if (rxbuf == nullptr) {
	   iio_context_destroy (ctx);
	   throw (new pluto_exception ("could not create RX buffer"));
	}

	iio_buffer_set_blocking_mode (rxbuf, true);
//	and be prepared for future changes in the settings
	connect (gainControl, SIGNAL (valueChanged (int)),
	         this, SLOT (set_gainControl (int)));
	connect (agcControl, SIGNAL (stateChanged (int)),
	         this, SLOT (set_agcControl (int)));
	connect (debugButton, SIGNAL (clicked ()),
	         this, SLOT (toggle_debugButton ()));
	connect (dumpButton, SIGNAL (clicked ()),
	         this, SLOT (set_xmlDump ()));
	connect (filterButton, SIGNAL (clicked ()),
	         this, SLOT (set_filter ()));

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
	int enabled;
//
//	go for the filter
	ad9361_get_trx_fir_enable (get_ad9361_phy (ctx), &enabled);
	if (enabled)
	   ad9361_set_trx_fir_enable (get_ad9361_phy (ctx), 0);
	int ret = iio_device_attr_write_raw (get_ad9361_phy (ctx),
	                                     "filter_fir_config",
	                                     dabFilter, strlen (dabFilter));
	if (ret < 0)
	   fprintf (stderr, "filter mislukt");
//	and enable it
	filterButton	-> setText ("filter off");
	ret = ad9361_set_trx_fir_enable (get_ad9361_phy (ctx), 1);
	if (ret < 0)
	   fprintf (stderr, "enabling filter failed\n");
	connected	= true;
	state -> setText ("ready to go");
}

	plutoHandler::~plutoHandler() {
	myFrame. hide ();
	stopReader();
	plutoSettings	-> beginGroup ("plutoSettings");
	plutoSettings	-> setValue ("pluto-agcMode",
	                              agcControl -> isChecked () ? 1 : 0);
	plutoSettings	-> setValue ("pluto-gain", 
	                              gainControl -> value ());
	plutoSettings	-> setValue ("pluto-debug", debugFlag ? 1 : 0);
	plutoSettings	-> endGroup ();
	if (!connected)		// should not happen
	   return;
	ad9361_set_trx_fir_enable (get_ad9361_phy (ctx), 0);
	iio_buffer_destroy (rxbuf);
	iio_context_destroy (ctx);
	delete pHandle;
}
//

void	plutoHandler::setVFOFrequency	(int32_t newFrequency) {
int	ret;
struct iio_channel *lo_channel;

	rx_cfg. lo_hz = newFrequency;
	ret	= get_lo_chan (ctx, RX, &lo_channel);
	ret	= iio_channel_attr_write_longlong (lo_channel,
	                                           "frequency",
	                                           rx_cfg. lo_hz);
	if (ret < 0) {
	   fprintf (stderr, "cannot set local oscillator frequency\n");
	}
	if (debugFlag)
	   fprintf (stderr, "frequency set to %d\n",
	                                 (int)(rx_cfg. lo_hz));
}

int32_t	plutoHandler::getVFOFrequency () {
	return rx_cfg. lo_hz;
}
//
//	If the agc is set, but someone touches the gain button
//	the agc is switched off. Btw, this is hypothetically
//	since the gain control is made invisible when the
//	agc is set
void	plutoHandler::set_gainControl	(int newGain) {
int ret;
struct iio_channel *chn;
	ret = get_phy_chan (ctx, RX, 0, &chn);
	if (ret < 0)
	   return;
	if (agcControl -> isChecked ()) {
	   ret = iio_channel_attr_write (chn, "gain_control_mode", "manual");
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

	ret = iio_channel_attr_write_longlong (chn, "hardwaregain", newGain);
	if (ret < 0) {
	   state -> setText ("error in gain setting");
	   if (debugFlag) 
	      fprintf (stderr,
	               "could not set hardware gain to %d\n", newGain);
	}
}

void	plutoHandler::set_agcControl	(int dummy) {
int ret;
struct iio_channel *gain_channel;

	get_phy_chan (ctx, RX, 0, &gain_channel);
	(void)dummy;
	if (agcControl -> isChecked ()) {
	   ret = iio_channel_attr_write (gain_channel,
	                                 "gain_control_mode", "slow_attack");
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
	   ret = iio_channel_attr_write (gain_channel,
	                                 "gain_control_mode", "manual");
	   if (ret < 0) {
	      state -> setText ("error in gain setting");
	      if (debugFlag)
	         fprintf (stderr, "error in gain setting\n");
	      return;
	   }
	   gainControl	-> show ();

	   ret = iio_channel_attr_write_longlong (gain_channel,
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
int ret;
iio_channel *lo_channel;
iio_channel *gain_channel;
	if (debugFlag)
	   fprintf (stderr, "restart called with %d\n", freq);
	if (!connected)		// should not happen
	   return false;
	if (running. load())
	   return true;		// should not happen
	if (save_gainSettings)
	   update_gainSettings (freq /MHz (1));

	get_phy_chan (ctx, RX, 0, &gain_channel);
//	settings are restored, now handle them
	ret = iio_channel_attr_write (gain_channel,
	                              "gain_control_mode",
	                              agcControl -> isChecked () ?
	                                       "slow_attack" : "manual");
	if (ret < 0) {
	   if (debugFlag)
	      fprintf (stderr, "error in setting agc\n");
	}
	else
	   state -> setText (agcControl -> isChecked ()? "agc set" : "agc off");

	if (!agcControl -> isChecked ()) {
	   ret = iio_channel_attr_write_longlong (gain_channel,
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
	if (agcControl -> isChecked ())
	   gainControl -> hide ();
	else
	   gainControl	-> show ();

	get_lo_chan (ctx, RX, &lo_channel);
	rx_cfg. lo_hz = freq;
	ret = iio_channel_attr_write_longlong (lo_channel,
	                                       "frequency",
	                                       rx_cfg. lo_hz);
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
	if (save_gainSettings)
	   record_gainSettings (rx_cfg. lo_hz/ MHz (1));
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
	   nbytes_rx	= iio_buffer_refill	(rxbuf);
	   p_inc	= iio_buffer_step	(rxbuf);
	   p_end	= (char *) iio_buffer_end  (rxbuf);

	   for (p_dat = (char *)iio_buffer_first (rxbuf, rx0_i);
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
//
//	we know that the coefficients are loaded
void	plutoHandler::set_filter () {
int ret;
	if (filterOn) {
           ad9361_set_trx_fir_enable (get_ad9361_phy (ctx), 0);
	   filterButton -> setText ("filter on");
	}
	else {
           ret = ad9361_set_trx_fir_enable (get_ad9361_phy (ctx), 1);
	   filterButton -> setText ("filter off");
	   fprintf (stderr, "setting filter went %s\n", ret == 0 ? "ok" : "wrong");
	}
	filterOn = !filterOn;
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
	set_gainControl (gainControl -> value ());
	gainControl	-> blockSignals (false);

	agcControl	-> blockSignals (true);
	new_agcValue (agc == 1);
	while (agcControl -> isChecked () != (agc == 1))
	   usleep (1000);
	set_agcControl (agcControl -> isChecked ());
	agcControl	-> blockSignals (false);
}

bool	plutoHandler::loadFunctions	() {

	iio_device_find_channel =
	           (pfn_iio_device_find_channel)
	                    pHandle -> resolve ("iio_device_find_channel");
	if (iio_device_find_channel == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_device_find_channel");
	   return false;
	}

	iio_create_default_context =
	            (pfn_iio_create_default_context)
	                    pHandle -> resolve ("iio_create_default_context");
	if (iio_create_default_context == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_create_default_context");
	   return false;
	}

	iio_create_local_context =
	            (pfn_iio_create_local_context)
	                    pHandle -> resolve ("iio_create_local_context");
	if (iio_create_local_context == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_create_local_context");
	   return false;
	}

	iio_create_network_context =
	            (pfn_iio_create_network_context)
	                    pHandle -> resolve ("iio_create_network_context");
	if (iio_create_network_context == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_create_network_context");
	   return false;
	}

	iio_context_get_name =
	            (pfn_iio_context_get_name)
	                    pHandle -> resolve ("iio_context_get_name");
	if (iio_context_get_name == nullptr) {
	   fprintf (stderr, "could not load %s\n", iio_context_get_name);
	   return false;
	}

	iio_context_get_devices_count =
	            (pfn_iio_context_get_devices_count)
	                    pHandle -> resolve ("iio_context_get_devices_count");
	if (iio_context_get_devices_count == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_context_get_devices_count");
	   return false;
	}

	iio_context_find_device =
	             (pfn_iio_context_find_device)
	                    pHandle -> resolve ("iio_context_find_device");
	if (iio_context_find_device == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_context_find_device");
	   return false;
	}

	iio_device_attr_read_bool =
	             (pfn_iio_device_attr_read_bool)
	                      pHandle -> resolve ("iio_device_attr_read_bool");
	if (iio_device_attr_read_bool == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_device_attr_read_bool");
	   return false;
	}

	iio_device_attr_write_bool =
	             (pfn_iio_device_attr_write_bool)
	                      pHandle -> resolve ("iio_device_attr_write_bool");
	if (iio_device_attr_write_bool == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_device_attr_write_bool");
	   return false;
	}

	iio_channel_attr_read_bool =
	             (pfn_iio_channel_attr_read_bool)
	                      pHandle -> resolve ("iio_channel_attr_read_bool");
	if (iio_channel_attr_read_bool == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_channel_attr_read_bool");
	   return false;
	}

	iio_channel_attr_write_bool =
	             (pfn_iio_channel_attr_write_bool)
	                      pHandle -> resolve ("iio_channel_attr_write_bool");
	if (iio_channel_attr_write_bool == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_channel_attr_write_bool");
	   return false;
	}

	iio_channel_enable =
	             (pfn_iio_channel_enable)
	                      pHandle -> resolve ("iio_channel_enable");
	if (iio_channel_enable == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_channel_enable");
	   return false;
	}

	iio_channel_attr_write =
	              (pfn_iio_channel_attr_write)
	                      pHandle -> resolve ("iio_channel_attr_write");
	if (iio_channel_attr_write == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_channel_attr_write");
	   return false;
	}

	iio_channel_attr_write_longlong =
	              (pfn_iio_channel_attr_write_longlong)
	                       pHandle -> resolve ("iio_channel_attr_write_longlong");
	if (iio_channel_attr_write_longlong == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_channel_attr_write_longlong");
	   return false;
	}

	iio_device_attr_write_longlong =
	               (pfn_iio_device_attr_write_longlong)
	                        pHandle -> resolve ("iio_device_attr_write_longlong");
	if (iio_device_attr_write_longlong == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_device_attr_write_longlong");
	   return false;
	}

	iio_device_attr_write_raw =
	               (pfn_iio_device_attr_write_raw)
	                         pHandle -> resolve ("iio_device_attr_write_raw");
	if (iio_device_attr_write_raw == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_device_attr_write_raw");
	   return false;
	}

	iio_device_create_buffer =
	               (pfn_iio_device_create_buffer)
	                         pHandle -> resolve ("iio_device_create_buffer");
	if (iio_device_create_buffer == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_device_create_buffer");
	   return false;
	}

	iio_buffer_set_blocking_mode =
	               (pfn_iio_buffer_set_blocking_mode)
	                         pHandle -> resolve ("iio_buffer_set_blocking_mode");
	if (iio_buffer_set_blocking_mode == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_buffer_set_blocking_mode");
	   return false;
	}

	iio_buffer_destroy =
	               (pfn_iio_buffer_destroy)
	                         pHandle -> resolve ("iio_buffer_destroy");
	if (iio_buffer_destroy == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_buffer_destroy");
	   return false;
	}

	iio_context_destroy =
	               (pfn_iio_context_destroy)
	                         pHandle -> resolve ("iio_context_destroy");
	if (iio_context_destroy == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_context_destroy");
	   return false;
	}

	iio_buffer_refill =
	               (pfn_iio_buffer_refill)
	                         pHandle -> resolve ("iio_buffer_refill");
	if (iio_buffer_refill == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_buffer_refill");
	   return false;
	}

	iio_buffer_step =
	               (pfn_iio_buffer_step)
	                         pHandle -> resolve ("iio_buffer_step");
	if (iio_buffer_step == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_buffer_step");
	   return false;
	}

	iio_buffer_end =
	               (pfn_iio_buffer_end)
	                          pHandle -> resolve ("iio_buffer_end");
	if (iio_buffer_end == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_buffer_end");
	   return false;
	}

	iio_buffer_first =
	               (pfn_iio_buffer_first)
	                          pHandle -> resolve ("iio_buffer_first");
	if (iio_buffer_first == nullptr) {
	   fprintf (stderr, "could not load %s\n", "iio_buffer_first");
	   return false;
	}
	return true;
}

