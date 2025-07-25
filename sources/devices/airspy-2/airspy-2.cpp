#
/**
 *  IW0HDV Extio
 *
 *  Copyright 2015 by Andrea Montefusco IW0HDV
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *	recoding, taking parts and extending for the airspy_2 interface
 *	for the Qt-DAB program
 *	jan van Katwijk
 *	Lazy Chair Computing
 */

#include	<QPoint>
#include	<QFileDialog>
#include	<QTime>
#include	<QDate>
#include	"dab-constants.h"
#include	"airspy-2.h"
#include	"position-handler.h"
#include	"airspyselect.h"
#include	"device-exceptions.h"
#include	"logger.h"
#include	"settingNames.h"
#include	"settings-handler.h"

#define	AIRSPY_SETTINGS	"airspySettings"
#define	TAB_SETTINGS	"tabSettings"
#define	CONV_QUALITY	"convQuality"

static
const	int	EXTIO_NS	=  8192;
static
const	int	EXTIO_BASE_TYPE_SIZE = sizeof (float);

	airspy_2::airspy_2 (QSettings *s,
	                    const QString  &recorderVersion,
	                    logger	*theLogger) :	// dummy for now
                                         _I_Buffer (4 * 1024 * 1024) {
int	result;
int	distance	= 1000000;
std::vector <uint32_t> sampleRates;
uint32_t samplerateCount;

	(void)theLogger;
	this	-> airspySettings	= s;
	this	-> recorderVersion	= recorderVersion;
	setupUi (&myFrame);
	setPositionAndSize (s, &myFrame, AIRSPY_SETTINGS);
	myFrame. show		();
//
//	Since we have different tabs, with different sliders for
//	gain setting, restoring the settings is a tedious task
	int tab	= value_i (airspySettings, AIRSPY_SETTINGS, TAB_SETTINGS, 2);
        tabWidget	-> setCurrentIndex (tab);
	restore_gainSliders  (200, tab);

	device			= nullptr;
	serialNumber		= 0;

#ifdef	__MINGW32__
	const char *libraryString = "airspy.dll";
#else
	const char *libraryString = "libairspy.so";
#endif
	
        library_p = new QLibrary (libraryString);
        library_p -> load();

        if (!library_p -> isLoaded ()) {
           throw (device_exception ("failed to open " +
                                        std::string (libraryString)));
        }

	if (!load_airspyFunctions ()) {
	   delete library_p;
	   throw (device_exception ("one or more library functions could not be loaded"));
	}
//
	strcpy (serial,"");
	if (this -> my_airspy_init () != AIRSPY_SUCCESS) {
	   delete library_p;
	   throw (device_exception ("could not init"));
	}

	uint64_t deviceList [4];
	int	deviceIndex;
	int numofDevs = my_airspy_list_devices (deviceList, 4)
;
	if (numofDevs == 0) {
	   delete library_p;
	   throw (device_exception ("No airspy device was detected"));
	}

	if (numofDevs > 1) {
           airspySelect deviceSelector;
           for (deviceIndex = 0; deviceIndex < (int)numofDevs; deviceIndex ++) {
              deviceSelector.
                   addtoList (QString::number (deviceList [deviceIndex]));
           }
           deviceIndex = deviceSelector. QDialog::exec();
        }
	else
	   deviceIndex = 0;
	
	result = my_airspy_open (&device, deviceList [deviceIndex]);
	if (result != AIRSPY_SUCCESS) {
	   delete library_p;
	   throw (device_exception
	                      (my_airspy_error_name ((airspy_error)result)));
	}

	(void) my_airspy_set_sample_type (device, AIRSPY_SAMPLE_INT16_IQ);
	(void) my_airspy_get_samplerates (device, &samplerateCount, 0);
	fprintf (stderr, "%d samplerates are supported\n", samplerateCount); 
	sampleRates. resize (samplerateCount);
	my_airspy_get_samplerates (device,
	                            sampleRates. data(), samplerateCount);

	selectedRate	= 0;
	for (int i = 0; i < (int)samplerateCount; i ++) {
	   fprintf (stderr, "%d \n", sampleRates [i]);
	   if (abs ((int)sampleRates [i] - SAMPLERATE) < distance) {
	      distance	= abs ((int)sampleRates [i] - SAMPLERATE);
	      selectedRate = sampleRates [i];
	   }
	}

	if (selectedRate == 0) {
	   delete library_p;
	   throw (device_exception ("Cannot handle the samplerates"));
	}

	fprintf (stderr, "selected samplerate = %d\n", selectedRate);
	result = my_airspy_set_samplerate (device, selectedRate);

	if (result != AIRSPY_SUCCESS) {
	   delete library_p;
	   throw (device_exception
	             (my_airspy_error_name ((enum airspy_error)result)));
	}
	filterDepth		= value_i (airspySettings, AIRSPY_SETTINGS,
	                                     CONV_QUALITY, 7);
	convQuality_setter	-> setValue (filterDepth);
        theFilter. reset (new LowPassFIR (filterDepth,
	                                  1536000 / 2, selectedRate));

//      The sizes of the mapTables follow from the input and output rate
//      (selectedRate / 1000) vs (SAMPLERATE / 1000)
//      so we end up with buffers with 1 msec content
        convBufferSize          = selectedRate / 1000;
	float samplesPerMsec	= SAMPLERATE / 1000.0;
        for (int i = 0; i < SAMPLERATE / 1000; i ++) {
           float inVal  = float (selectedRate / 1000);
           mapTable_int [i]     = int (floor (i * (inVal / samplesPerMsec)));
           mapTable_float [i]   = i * (inVal / samplesPerMsec) - mapTable_int [i];
        }
        convIndex       = 0;
        convBuffer. resize (convBufferSize + 1);

//
	restore_gainSettings (tab);
	connect (linearitySlider, &QSlider::valueChanged,
	         this, &airspy_2::set_linearity);
	connect (sensitivitySlider, &QSlider::valueChanged,
	         this, &airspy_2::set_sensitivity);
	connect (lnaSlider, &QSlider::valueChanged,
	         this, &airspy_2::set_lna_gain);
	connect (vgaSlider, &QSlider::valueChanged,
	         this, &airspy_2::set_vga_gain);
	connect (mixerSlider, &QSlider::valueChanged,
	         this, &airspy_2::set_mixer_gain);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (lnaButton, &QCheckBox::checkStateChanged,
#else
	connect (lnaButton, &QCheckBox::stateChanged,
#endif
	         this, &airspy_2::set_lna_agc);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (mixerButton, &QCheckBox::checkStateChanged,
#else
	connect (mixerButton, &QCheckBox::stateChanged,
#endif
	         this, &airspy_2::set_mixer_agc);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (biasButton, &QCheckBox::checkStateChanged,
#else
	connect (biasButton, &QCheckBox::stateChanged,
#endif
	         this, &airspy_2::set_rf_bias);
	connect (tabWidget, &QTabWidget::currentChanged,
	         this, &airspy_2::switch_tab);
	connect (dumpButton, &QPushButton::clicked,
	         this, &airspy_2::set_xmlDump);
	connect (this, &airspy_2::new_tabSetting,
	         tabWidget, &QTabWidget::setCurrentIndex);
//
	displaySerial	-> setText (getSerial());
	running. store (false);
	my_airspy_set_rf_bias (device, rf_bias ? 1 : 0);

	dumping. store (false);
}

	airspy_2::~airspy_2 () {
	stopReader ();
	myFrame. hide ();
	storeWidgetPosition (airspySettings, &myFrame, AIRSPY_SETTINGS);
	store (airspySettings, AIRSPY_SETTINGS, TAB_SETTINGS,
	                                   tabWidget -> currentIndex ());
	if (device != nullptr) {
	   int result = my_airspy_stop_rx (device);
	   if (result != AIRSPY_SUCCESS) {
	      printf ("my_airspy_stop_rx() failed: %s (%d)\n",
	             my_airspy_error_name((airspy_error)result), result);
	   }

	   result = my_airspy_close (device);
	   if (result != AIRSPY_SUCCESS) {
	      printf ("airspy_close() failed: %s (%d)\n",
	             my_airspy_error_name((airspy_error)result), result);
	   }
	}
	my_airspy_exit();
	theFilter. reset ();
	if (!xmlWriter. isNull ())
	   xmlWriter. reset ();
	delete library_p;
}

bool	airspy_2::restartReader	(int32_t freq, int samplesSkipped) {
int	result;
//int32_t	bufSize	= EXTIO_NS * EXTIO_BASE_TYPE_SIZE * 2;

	if (running. load())
	   return true;

	lastFrequency	= freq;
	this	-> toSkip = samplesSkipped;
	QString key	= QString (TAB_SETTINGS) + "-"
	                             + QString::number (freq / MHz (1));
	int tab		= value_i (airspySettings, AIRSPY_SETTINGS, key, 0);
	tabWidget       -> blockSignals (true);
        new_tabSetting  (tab);
        while (tabWidget -> currentIndex () != tab) 
           usleep (1000);
        tabWidget       -> blockSignals (false);
//
//	sliders are now set,
	result = my_airspy_set_freq (device, freq);

	if (result != AIRSPY_SUCCESS) {
	   showStatus (my_airspy_error_name ((airspy_error)result));
	   printf ("my_airspy_set_freq() failed: %s (%d)\n",
	            my_airspy_error_name((airspy_error)result), result);
	}
	result = my_airspy_set_sample_type (device, AIRSPY_SAMPLE_INT16_IQ);
	if (result != AIRSPY_SUCCESS) {
	   showStatus (my_airspy_error_name ((airspy_error)result));
	   printf ("my_airspy_set_sample_type() failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	   return false;
	}

	result = my_airspy_start_rx (device,
	            (airspy_sample_block_cb_fn)callback, this);
	if (result != AIRSPY_SUCCESS) {
	   showStatus (my_airspy_error_name ((airspy_error)result));
	   printf ("my_airspy_start_rx() failed: %s (%d)\n",
	         my_airspy_error_name((airspy_error)result), result);
	   return false;
	}

//
//	Hier moeten we de tab en gain weer zetten
	restore_gainSliders (freq / MHz (1), tab);
	restore_gainSettings (tab);
	showStatus (QString ("Restart at ") +
	            QString::number (freq / 1000) + QString ("Khz"));
	_I_Buffer. FlushRingBuffer ();
//
	running. store (true);
	return true;
}

void	airspy_2::stopReader() {
int	result;

	if (!running. load())
	   return;

	close_xmlDump ();
	QString key = QString (TAB_SETTINGS) + "-" 
	                        + QString::number (lastFrequency / MHz (1));
	store (airspySettings, AIRSPY_SETTINGS, key, tabWidget -> currentIndex ());
	record_gainSettings (lastFrequency / MHz (1), 
	                           tabWidget -> currentIndex ());
	result = my_airspy_stop_rx (device);

	if (result != AIRSPY_SUCCESS ) {
	   showStatus (my_airspy_error_name ((airspy_error)result));
	   printf ("my_airspy_stop_rx() failed: %s (%d)\n",
	          my_airspy_error_name ((airspy_error)result), result);
	}
	running. store (false);
	resetBuffer ();
}
//
//	Directly copied from the airspy extio dll from Andrea Montefusco
int airspy_2::callback (airspy_transfer* transfer) {
airspy_2 *p;

	if (!transfer)
	   return 0;		// should not happen
	p = static_cast<airspy_2 *> (transfer -> ctx);

// we read  AIRSPY_SAMPLE_INT16_IQ:
	int32_t bytes_to_write = transfer -> sample_count * sizeof (int16_t) * 2; 
	uint8_t *pt_rx_buffer   = (uint8_t *)transfer->samples;
	p -> data_available (pt_rx_buffer, bytes_to_write);
	return 0;
}

//	called from AIRSPY data callback
//	2*2 = 4 bytes for sample, as per AirSpy USB data stream format
//	Rate conversion is done using the samplerate library
//	with a selectable "quality level"
int 	airspy_2::data_available (void *buf, int buf_size) {	
int16_t	*sbuf	= (int16_t *)buf;
int nSamples	= buf_size / (sizeof (int16_t) * 2);
std::complex<float> temp [SAMPLERATE / 1000];

	if (dumping. load ())
	   xmlWriter -> add ((std::complex<int16_t> *)sbuf, nSamples);
	if (convQuality_setter -> value () != filterDepth) {
	   filterDepth = convQuality_setter -> value ();
	   theFilter -> resize (filterDepth);
	   store  (airspySettings, AIRSPY_SETTINGS,
                                             CONV_QUALITY, filterDepth);
	}

	for (int i = 0; i < nSamples; i ++) {
	    convBuffer [convIndex ++] = theFilter -> Pass (
                                             std::complex<float> (
                                                sbuf [2 * i] / (float)2048,
                                                sbuf [2 * i + 1] / (float)2048));

	   if (convIndex > convBufferSize) {
	      for (int j = 0; j < SAMPLERATE / 1000; j ++) {
	         int16_t  inpBase    = mapTable_int [j];
	         float    inpRatio   = mapTable_float [j];
	         temp [j]    = convBuffer [inpBase + 1] * inpRatio +
	                       convBuffer [inpBase] * (1 - inpRatio);
	      }
	      if (toSkip > 0)
	         toSkip -= SAMPLERATE / 1000;
	      else
	         _I_Buffer. putDataIntoBuffer (temp, SAMPLERATE / 1000);
//	shift the sample at the end to the beginning, it is needed
//	as the starting sample for the next time
	      convBuffer [0] = convBuffer [convBufferSize];
	      convIndex = 1;
           }
	}
	return 0;
}
//
const char *airspy_2::getSerial () {
airspy_read_partid_serialno_t read_partid_serialno;
int result = my_airspy_board_partid_serialno_read (device,
	                                          &read_partid_serialno);
	if (result != AIRSPY_SUCCESS) {
	   printf ("failed: %s (%d)\n",
	         my_airspy_error_name ((airspy_error)result), result);
	   return "UNKNOWN";
	} else {
	   snprintf (serial, sizeof(serial), "%08X%08X", 
	             read_partid_serialno. serial_no [2],
	             read_partid_serialno. serial_no [3]);
	}
	return serial;
}
//
//	not used here
int	airspy_2::open() {
//int result = my_airspy_open (&device);
//
//	if (result != AIRSPY_SUCCESS) {
//	   printf ("airspy_open() failed: %s (%d)\n",
//	          my_airspy_error_name((airspy_error)result), result);
//	   return -1;
//	} else {
//	   return 0;
//	}
	return 0;
}

//
//	These functions are added for the SDR-J interface
void	airspy_2::resetBuffer	() {
	_I_Buffer. FlushRingBuffer	();
}

int16_t	airspy_2::bitDepth		() {
	return 13;
}

int32_t	airspy_2::getSamples (std::complex<float> *v, int32_t size) {
	return _I_Buffer. getDataFromBuffer (v, size);
}

int32_t	airspy_2::Samples		() {
	return _I_Buffer. GetRingBufferReadAvailable();
}
//
const char* airspy_2::board_id_name() {
uint8_t bid;

	if (my_airspy_board_id_read (device, &bid) == AIRSPY_SUCCESS)
	   return my_airspy_board_id_name ((airspy_board_id)bid);
	else
	   return "UNKNOWN";
}
//
bool	airspy_2::load_airspyFunctions() {
//
//	link the required procedures
	my_airspy_init	= (pfn_airspy_init)
	                       library_p -> resolve ("airspy_init");
	if (my_airspy_init == nullptr) {
	   fprintf (stderr, "Could not find airspy_init\n");
	   return false;
	}

	my_airspy_exit	= (pfn_airspy_exit)
	                       library_p -> resolve ("airspy_exit");
	if (my_airspy_exit == nullptr) {
	   fprintf (stderr, "Could not find airspy_exit\n");
	   return false;
	}

	my_airspy_list_devices	= (pfn_airspy_list_devices)
	                       library_p -> resolve ("airspy_list_devices");
	if (my_airspy_list_devices == nullptr) {
	   fprintf (stderr, "Could not find airspy_list_devices\n");
	   return false;
	}
	
	my_airspy_open	= (pfn_airspy_open)
	                       library_p -> resolve ("airspy_open");
	if (my_airspy_open == nullptr) {
	   fprintf (stderr, "Could not find airspy_open\n");
	   return false;
	}

	my_airspy_close	= (pfn_airspy_close)
	                       library_p -> resolve ("airspy_close");
	if (my_airspy_close == nullptr) {
	   fprintf (stderr, "Could not find airspy_close\n");
	   return false;
	}

	my_airspy_get_samplerates	= (pfn_airspy_get_samplerates)
	                       library_p -> resolve ("airspy_get_samplerates");
	if (my_airspy_get_samplerates == nullptr) {
	   fprintf (stderr, "Could not find airspy_get_samplerates\n");
	   return false;
	}

	my_airspy_set_samplerate	= (pfn_airspy_set_samplerate)
	                       library_p -> resolve ("airspy_set_samplerate");
	if (my_airspy_set_samplerate == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_samplerate\n");
	   return false;
	}

	my_airspy_start_rx	= (pfn_airspy_start_rx)
	                       library_p -> resolve ("airspy_start_rx");
	if (my_airspy_start_rx == nullptr) {
	   fprintf (stderr, "Could not find airspy_start_rx\n");
	   return false;
	}

	my_airspy_stop_rx	= (pfn_airspy_stop_rx)
	                       library_p -> resolve ("airspy_stop_rx");
	if (my_airspy_stop_rx == nullptr) {
	   fprintf (stderr, "Could not find airspy_stop_rx\n");
	   return false;
	}

	my_airspy_set_sample_type	= (pfn_airspy_set_sample_type)
	                       library_p -> resolve ("airspy_set_sample_type");
	if (my_airspy_set_sample_type == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_sample_type\n");
	   return false;
	}

	my_airspy_set_freq	= (pfn_airspy_set_freq)
	                       library_p -> resolve ("airspy_set_freq");
	if (my_airspy_set_freq == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_freq\n");
	   return false;
	}

	my_airspy_set_lna_gain	= (pfn_airspy_set_lna_gain)
	                       library_p -> resolve ("airspy_set_lna_gain");
	if (my_airspy_set_lna_gain == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_lna_gain\n");
	   return false;
	}

	my_airspy_set_mixer_gain	= (pfn_airspy_set_mixer_gain)
	                       library_p -> resolve ("airspy_set_mixer_gain");
	if (my_airspy_set_mixer_gain == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_mixer_gain\n");
	   return false;
	}

	my_airspy_set_vga_gain	= (pfn_airspy_set_vga_gain)
	                       library_p -> resolve ("airspy_set_vga_gain");
	if (my_airspy_set_vga_gain == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_vga_gain\n");
	   return false;
	}
	
	my_airspy_set_linearity_gain = (pfn_airspy_set_linearity_gain)
	                       library_p -> resolve ("airspy_set_linearity_gain");
	if (my_airspy_set_linearity_gain == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_linearity_gain\n");
	   fprintf (stderr, "You probably did install an old library\n");
	   return false;
	}

	my_airspy_set_sensitivity_gain = (pfn_airspy_set_sensitivity_gain)
	                       library_p -> resolve ("airspy_set_sensitivity_gain");
	if (my_airspy_set_sensitivity_gain == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_sensitivity_gain\n");
	   fprintf (stderr, "You probably did install an old library\n");
	   return false;
	}

	my_airspy_set_lna_agc	= (pfn_airspy_set_lna_agc)
	                       library_p -> resolve ("airspy_set_lna_agc");
	if (my_airspy_set_lna_agc == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_lna_agc\n");
	   return false;
	}

	my_airspy_set_mixer_agc	= (pfn_airspy_set_mixer_agc)
	                       library_p -> resolve ("airspy_set_mixer_agc");
	if (my_airspy_set_mixer_agc == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_mixer_agc\n");
	   return false;
	}

	my_airspy_set_rf_bias	= (pfn_airspy_set_rf_bias)
	                       library_p -> resolve ("airspy_set_rf_bias");
	if (my_airspy_set_rf_bias == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_rf_bias\n");
	   return false;
	}

	my_airspy_error_name	= (pfn_airspy_error_name)
	                       library_p -> resolve ("airspy_error_name");
	if (my_airspy_error_name == nullptr) {
	   fprintf (stderr, "Could not find airspy_error_name\n");
	   return false;
	}

	my_airspy_board_id_read	= (pfn_airspy_board_id_read)
	                       library_p -> resolve ("airspy_board_id_read");
	if (my_airspy_board_id_read == nullptr) {
	   fprintf (stderr, "Could not find airspy_board_id_read\n");
	   return false;
	}

	my_airspy_board_id_name	= (pfn_airspy_board_id_name)
	                       library_p -> resolve ("airspy_board_id_name");
	if (my_airspy_board_id_name == nullptr) {
	   fprintf (stderr, "Could not find airspy_board_id_name\n");
	   return false;
	}

	my_airspy_board_partid_serialno_read	=
	                (pfn_airspy_board_partid_serialno_read)
	                       library_p -> resolve ("airspy_board_partid_serialno_read");
	if (my_airspy_board_partid_serialno_read == nullptr) {
	   fprintf (stderr, "Could not find airspy_board_partid_serialno_read\n");
	   return false;
	}

	return true;
}

int	airspy_2::getBufferSpace	() {
	return _I_Buffer. GetRingBufferWriteAvailable ();
}

QString	airspy_2::deviceName	() {
	return QString ("AIRspy :") + QString (getSerial ());
}

void	airspy_2::set_xmlDump () {
	if (xmlWriter. isNull ()) {
	   setup_xmlDump (false);
	}
	else {
	   close_xmlDump ();
	}
}

void	airspy_2::startDump	() {
	setup_xmlDump (true);
}

void	airspy_2::stopDump	() {
	close_xmlDump	();
}

bool	airspy_2::setup_xmlDump (bool direct) {
QString channel		= value_s (airspySettings, DAB_GENERAL,
	                                            "channel", "xx");
	try {
	   xmlWriter. reset (new xml_fileWriter (airspySettings,
	                                      channel,
	                                      12,
	                                      "int16",
	                                      selectedRate,
	                                      lastFrequency,
	                                      -1,
	                                      "Airspy",
	                                      getSerial (),
	                                      recorderVersion,
	                                      direct));
	} catch (...) {
	   return false;
	}
	dumping. store (true);
	dumpButton	-> setText ("writing");
	return true;
}

void	airspy_2::close_xmlDump () {
	if (xmlWriter. isNull ())	// this can happen !!
	   return;
	dumping. store (false);
	usleep (1000);
	xmlWriter	-> computeHeader ();
	dumping. store (false);
	dumpButton	-> setText ("Dump");
	xmlWriter. reset ();
}
//
//	gain settings are maintained on a per-channel and per tab base,
//	Values are recorded on both switching tabs and changing channels
void	airspy_2::record_gainSettings	(int freq, int tab) {
QString	res;
QString key;

	key		= QString::number (freq) + "-"
	                                 + QString::number (tab);
	switch (tab) {
	   case 0:	// sensitity screen is on
	      res = QString::number (sensitivitySlider -> value ());
	      break;
	   case 1:	// linearity screen is on
	      res = QString::number (linearitySlider -> value ());
	      break;
	   case 3:	// classic screen
	   default:
	      res = QString::number (vgaSlider -> value ());
	      res = res + ":" + QString::number (mixerSlider -> value ());
	      res = res + ":" + QString::number (lnaSlider -> value ());
	      break;
	}

	res	= res + ":" + QString::number (lnaButton -> isChecked ());
	res	= res + ":" + QString::number (mixerButton -> isChecked ());
	res	= res + ":" + QString::number (biasButton -> isChecked ());

	store (airspySettings, AIRSPY_SETTINGS, key, res);
}
//
//	When starting a channel, the gain sliders from the previous
//	time that channel was the current channel, are restored
//	Note that the device settings are NOT yet updated
void	airspy_2::restore_gainSliders	(int freq, int tab) {
int	lna	= 0;
int	mixer	= 0;
int	bias	= 0;
QString key	= QString::number (freq) + "-" + QString::number (tab);
QString	def	= "";

	QString gainValues =
	       value_s (airspySettings, AIRSPY_SETTINGS, key, def);
	if (gainValues == "") { // we create default values
	   if ((tab == 0) || (tab == 1)) {
	      gainValues = "10:0:0:0";
	   }
	   else
	      gainValues = "10:10:10:0:0:0";	
	}
	QStringList list = gainValues. split (":");
	switch (tab) {
	   case 0:
	      disconnect (sensitivitySlider, &QSlider::valueChanged,
	                  this, &airspy_2::set_sensitivity);
	      sensitivitySlider -> setValue (list. at (0). toInt ());
	      connect (sensitivitySlider, &QSlider::valueChanged,
	               this, &airspy_2::set_sensitivity);
	      lna	= list. at (1). toInt ();
	      mixer	= list. at (2). toInt ();
	      bias	= list. at (3). toInt ();
	      break;
	   case 1:
	      disconnect (linearitySlider, &QSlider::valueChanged,
	                  this, &airspy_2::set_linearity);
	      linearitySlider -> setValue (list. at (0). toInt ());
	      connect (linearitySlider, &QSlider::valueChanged,
	               this, &airspy_2::set_linearity);
	      lna	= list. at (1). toInt ();
	      mixer	= list. at (2). toInt ();
	      bias	= list. at (3). toInt ();
	      break;

	   default:	// classic view
	      disconnect (vgaSlider, &QSlider::valueChanged,
	                  this, &airspy_2::set_vga_gain);
	      disconnect (mixerSlider, &QSlider::valueChanged,
	                  this, &airspy_2::set_mixer_gain);
	      disconnect (lnaSlider, &QSlider::valueChanged,
	                  this, &airspy_2::set_lna_gain);
	      vgaSlider		-> setValue (list. at (0). toInt ());
	      mixerSlider	-> setValue (list. at (1). toInt ());
	      lnaSlider		-> setValue (list. at (2). toInt ());
	      connect (vgaSlider, &QSlider::valueChanged,
	               this, &airspy_2::set_vga_gain);
	      connect (mixerSlider, &QSlider::valueChanged,
	               this, &airspy_2::set_mixer_gain);
	      connect (lnaSlider, &QSlider::valueChanged,
	               this, &airspy_2::set_lna_gain);
	      lna	= list. at (3). toInt ();
	      mixer	= list. at (4). toInt ();
	      bias	= list. at (5). toInt ();
	}
//
//	Now the agc settings
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	disconnect (lnaButton, &QCheckBox::checkStateChanged,
#else
	disconnect (lnaButton, &QCheckBox::stateChanged,
#endif
	            this, &airspy_2::set_lna_agc);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	disconnect (mixerButton, &QCheckBox::checkStateChanged,
#else
	disconnect (mixerButton, &QCheckBox::stateChanged,
#endif
	            this, &airspy_2::set_mixer_agc);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	disconnect (biasButton, &QCheckBox::checkStateChanged,
#else
	disconnect (biasButton, &QCheckBox::stateChanged,
#endif
	            this, &airspy_2::set_rf_bias);
	if (lna != 0)
	   lnaButton	-> setChecked (true);
	if (mixer != 0)
	   mixerButton	-> setChecked (true);
	if (bias != 0)
	   biasButton	-> setChecked (true);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (lnaButton, &QCheckBox::checkStateChanged,
#else
	connect (lnaButton, &QCheckBox::stateChanged,
#endif
	         this, &airspy_2::set_lna_agc);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (mixerButton, &QCheckBox::checkStateChanged,
#else
	connect (mixerButton, &QCheckBox::stateChanged,
#endif
	         this, &airspy_2::set_mixer_agc);
#if QT_VERSION >= QT_VERSION_CHECK (6, 0, 2)
	connect (biasButton, &QCheckBox::checkStateChanged,
#else
	connect (biasButton, &QCheckBox::stateChanged,
#endif
	         this, &airspy_2::set_rf_bias);
}

void	airspy_2::restore_gainSettings	(int tab) {
	switch (tab) {
	   case 0:	//sensitivity
	      set_sensitivity (sensitivitySlider -> value ());
	      break;
	   case 1:	// linearity
	      set_linearity (linearitySlider -> value ());
	      break;
	   case 2:	// classic view
	   default:
	      set_lna_gain	(lnaSlider	-> value ());
	      set_mixer_gain	(mixerSlider	-> value ());
	      set_vga_gain	(vgaSlider	-> value ());
	      break;
	}
	if (lnaButton -> isChecked ()) 
	   set_lna_agc (1);
	if (mixerButton -> isChecked ())
	   set_mixer_agc (1);
	if (biasButton -> isChecked ())
	   set_rf_bias (1);
}

void	airspy_2::switch_tab (int t) {
	record_gainSettings (lastFrequency / MHz (1),
	                           tabWidget -> currentIndex ());
	tabWidget       -> blockSignals (true);
	new_tabSetting	(t);
        while (tabWidget -> currentIndex () != t)
           usleep (1000);
        tabWidget       -> blockSignals (false);
	store (airspySettings, AIRSPY_SETTINGS, TAB_SETTINGS, t);
	restore_gainSliders (lastFrequency / MHz (1), t);
	restore_gainSettings (t);
}

#define GAIN_COUNT (22)

uint8_t airspy_linearity_vga_gains[GAIN_COUNT] = { 13, 12, 11, 11, 11, 11, 11, 10, 10, 10, 10, 10, 10, 10, 10, 10, 9, 8, 7, 6, 5, 4 };
uint8_t airspy_linearity_mixer_gains[GAIN_COUNT] = { 12, 12, 11, 9, 8, 7, 6, 6, 5, 0, 0, 1, 0, 0, 2, 2, 1, 1, 1, 1, 0, 0 };
uint8_t airspy_linearity_lna_gains[GAIN_COUNT] = { 14, 14, 14, 13, 12, 10, 9, 9, 8, 9, 8, 6, 5, 3, 1, 0, 0, 0, 0, 0, 0, 0 };
uint8_t airspy_sensitivity_vga_gains[GAIN_COUNT] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4 };
uint8_t airspy_sensitivity_mixer_gains[GAIN_COUNT] = { 12, 12, 12, 12, 11, 10, 10, 9, 9, 8, 7, 4, 4, 4, 3, 2, 2, 1, 0, 0, 0, 0 };
uint8_t airspy_sensitivity_lna_gains[GAIN_COUNT] = { 14, 14, 14, 14, 14, 14, 14, 14, 14, 13, 12, 12, 9, 9, 8, 7, 6, 5, 3, 2, 1, 0 };

void	airspy_2::set_linearity (int value) {
int	result = my_airspy_set_linearity_gain (device, value);
int	temp;
	if (result != AIRSPY_SUCCESS) {
	   showStatus (my_airspy_error_name ((airspy_error)result));
	   printf ("airspy_set_lna_gain() failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	   return;
	}
	linearityDisplay	-> display (value);
	temp	= airspy_linearity_lna_gains [GAIN_COUNT - 1 - value];
	linearity_lnaDisplay	-> display (temp);
	temp	= airspy_linearity_mixer_gains [GAIN_COUNT - 1 - value];
	linearity_mixerDisplay	-> display (temp);
	temp	= airspy_linearity_vga_gains [GAIN_COUNT - 1 - value];
	linearity_vgaDisplay	-> display (temp);
}

void	airspy_2::set_sensitivity (int value) {
int	result = my_airspy_set_sensitivity_gain (device, value);
int	temp;
	if (result != AIRSPY_SUCCESS) {
	   showStatus (my_airspy_error_name ((airspy_error)result));
	   printf ("airspy_set_mixer_gain() failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	   return;
	}
	sensitivityDisplay	-> display (value);
	temp	= airspy_sensitivity_lna_gains [GAIN_COUNT - 1 - value];
	sensitivity_lnaDisplay	-> display (temp);
	temp	= airspy_sensitivity_mixer_gains [GAIN_COUNT - 1 - value];
	sensitivity_mixerDisplay	-> display (temp);
	temp	= airspy_sensitivity_vga_gains [GAIN_COUNT - 1 - value];
	sensitivity_vgaDisplay	-> display (temp);
}

//	Original functions from the airspy extio dll
/* Parameter value shall be between 0 and 15 */
void	airspy_2::set_lna_gain (int value) {
int result = my_airspy_set_lna_gain (device, lnaGain = value);

	if (result != AIRSPY_SUCCESS) {
	   showStatus (my_airspy_error_name ((airspy_error)result));
	   printf ("airspy_set_lna_gain() failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	}
	else
	   lnaDisplay	-> display (value);
}

/* Parameter value shall be between 0 and 15 */
void	airspy_2::set_mixer_gain (int value) {
int result = my_airspy_set_mixer_gain (device, mixerGain = value);

	if (result != AIRSPY_SUCCESS) {
	   showStatus (my_airspy_error_name ((airspy_error)result));
	   printf ("airspy_set_mixer_gain() failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	}
	else
	   mixerDisplay	-> display (value);
}

/* Parameter value shall be between 0 and 15 */
void	airspy_2::set_vga_gain (int value) {
int result = my_airspy_set_vga_gain (device, vgaGain = value);

	if (result != AIRSPY_SUCCESS) {
	   showStatus (my_airspy_error_name ((airspy_error)result));
	   printf ("airspy_set_vga_gain() failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	}
	else
	   vgaDisplay	-> display (value);
}
//
//	agc's
/* Parameter value:
	0=Disable LNA Automatic Gain Control
	1=Enable LNA Automatic Gain Control
*/
void	airspy_2::set_lna_agc	(int dummy) {
	(void)dummy;
	lna_agc	= lnaButton	-> isChecked ();
	int result = my_airspy_set_lna_agc (device, lna_agc ? 1 : 0);

	if (result != AIRSPY_SUCCESS) {
	   showStatus (my_airspy_error_name ((airspy_error)result));
	   printf ("airspy_set_lna_agc() failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	}
}

/* Parameter value:
	0=Disable MIXER Automatic Gain Control
	1=Enable MIXER Automatic Gain Control
*/
void	airspy_2::set_mixer_agc	(int dummy) {
	(void)dummy;
	mixer_agc	= mixerButton -> isChecked ();

int result = my_airspy_set_mixer_agc (device, mixer_agc ? 1 : 0);

	if (result != AIRSPY_SUCCESS) {
	   showStatus (my_airspy_error_name ((airspy_error)result));
	   printf ("airspy_set_mixer_agc() failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	}
}

/* Parameter value shall be 0=Disable BiasT or 1=Enable BiasT */
void	airspy_2::set_rf_bias (int dummy) {
	(void)dummy;
	rf_bias	= biasButton -> isChecked ();
int result = my_airspy_set_rf_bias (device, rf_bias ? 1 : 0);

	if (result != AIRSPY_SUCCESS) {
	   showStatus (my_airspy_error_name ((airspy_error)result));
	   printf("airspy_set_rf_bias() failed: %s (%d)\n",
	           my_airspy_error_name ((airspy_error)result), result);
	}
}

void	airspy_2::showStatus	(const QString s) {
	statusLabel -> setText (s);
}

