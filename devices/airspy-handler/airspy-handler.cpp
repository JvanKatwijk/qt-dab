
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
 *	recoding, taking parts and extending for the airspyHandler interface
 *	for the Qt-DAB program
 *	jan van Katwijk
 *	Lazy Chair Computing
 */

#ifdef	__MINGW32__
#define	GETPROCADDRESS	GetProcAddress
#else
#define	GETPROCADDRESS	dlsym
#endif

#include	"airspy-handler.h"
#include	"airspyfilter.h"

static
const	int	EXTIO_NS	=  8192;
static
const	int	EXTIO_BASE_TYPE_SIZE = sizeof (float);

	airspyHandler::airspyHandler (QSettings *s) {
int	result, i;
int	distance	= 1000000;
std::vector <uint32_t> sampleRates;
uint32_t samplerateCount;

	this	-> airspySettings	= s;
	myFrame		= new QFrame (nullptr);
	setupUi (this -> myFrame);
	this	-> myFrame	-> show();

	filter			= nullptr;
	airspySettings	-> beginGroup ("airspyHandler");
	int16_t temp 		= airspySettings -> value ("linearity", 10).
	                                                          toInt();
	linearitySlider		-> setValue (temp);
	linearityDisplay	-> display  (temp);
	temp			= airspySettings -> value ("sensitivity", 10).
	                                                          toInt();
	sensitivitySlider	-> setValue (temp);
	sensitivityDisplay	-> display (temp);

	vgaGain			= airspySettings -> value ("vga", 5).toInt();
	vgaSlider		-> setValue (vgaGain);
	vgaDisplay		-> display (vgaGain);
	mixerGain		= airspySettings -> value ("mixer", 10). toInt();
	mixerSlider		-> setValue (mixerGain);
	mixerDisplay		-> display (mixerGain);
	mixer_agc		= false;
	lnaGain			= airspySettings -> value ("lna", 5). toInt();
	lnaSlider		-> setValue (lnaGain);
	lnaDisplay		-> display  (lnaGain);
	mixer_agc		= false;
	lna_agc			= false;
	rf_bias			= false;
	coarseOffset		= airspySettings -> value ("airspyOffset", 0). toInt();
	airspySettings	-> endGroup();

	device			= nullptr;
	serialNumber		= 0;
	theBuffer		= nullptr;
#ifdef	__MINGW32__
	const char *libraryString = "airspy.dll";
	Handle		= LoadLibrary ((wchar_t *)L"airspy.dll");
#else
	const char *libraryString = "libairspy.so";
	Handle		= dlopen ("libairspy.so", RTLD_LAZY);
#endif

	if (Handle == nullptr) {
	   fprintf (stderr, "failed to open %s\n", libraryString);
#ifndef	__MINGW32__
	   fprintf (stderr, "Error = %s\n", dlerror());
#endif
	   delete myFrame;
	   throw (20);
	}

	libraryLoaded	= true;

	if (!load_airspyFunctions()) {
	   fprintf (stderr, "problem in loading functions\n");
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
	
#endif
	   delete myFrame;
	}
//
	strcpy (serial,"");
	result = this -> my_airspy_init();
	if (result != AIRSPY_SUCCESS) {
	   printf ("my_airspy_init() failed: %s (%d)\n",
	             my_airspy_error_name((airspy_error)result), result);
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
	   delete myFrame;
	   throw (21);
	}

	result = my_airspy_open (&device);
	if (result != AIRSPY_SUCCESS) {
	   printf ("my_airpsy_open() failed: %s (%d)\n",
	             my_airspy_error_name ((airspy_error)result), result);
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
	   delete myFrame;
	   throw (22);
	}

	(void) my_airspy_set_sample_type (device, AIRSPY_SAMPLE_INT16_IQ);
	(void) my_airspy_get_samplerates (device, &samplerateCount, 0);
	fprintf (stderr, "%d samplerates are supported\n", samplerateCount); 
	sampleRates. resize (samplerateCount);
	my_airspy_get_samplerates (device,
	                            sampleRates. data(), samplerateCount);

	selectedRate	= 0;
	for (i = 0; i < (int)samplerateCount; i ++) {
	   fprintf (stderr, "%d \n", sampleRates [i]);
	   if (abs ((int)sampleRates [i] - 2048000) < distance) {
	      distance	= abs ((int)sampleRates [i] - 2048000);
	      selectedRate = sampleRates [i];
	   }
	}

	if (selectedRate == 0) {
	   fprintf (stderr, "Sorry. cannot help you\n");
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
	   delete myFrame;
	   throw (23);
	}
	else
	   fprintf (stderr, "selected samplerate = %d\n", selectedRate);

	result = my_airspy_set_samplerate (device, selectedRate);
	if (result != AIRSPY_SUCCESS) {
           printf("airspy_set_samplerate() failed: %s (%d)\n",
	             my_airspy_error_name ((enum airspy_error)result), result);
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
	   delete myFrame;
	   throw (24);
	}

	airspySettings	-> beginGroup ("airspyHandler");
	filtering 	= airspySettings -> value ("filtering", 0). toInt();
	int filterDegree = airspySettings -> value ("filterdegree", 9). toInt();
	filterDegree	= (filterDegree & ~01) + 1;
//
//	if we apply filtering it is using a symmetric lowpass filter
	filter		= new airspyFilter (filterDegree,
	                                        1024000, selectedRate);
	airspySettings	-> endGroup();

//	The sizes of the mapTables follow from the input and output rate
//	(selectedRate / 1000) vs (2048000 / 1000)
//	so we end up with buffers with 1 msec content
	convBufferSize		= selectedRate / 1000;
	for (i = 0; i < 2048; i ++) {
	   float inVal	= float (selectedRate / 1000);
	   mapTable_int [i]	=  int (floor (i * (inVal / 2048.0)));
	   mapTable_float [i]	= i * (inVal / 2048.0) - mapTable_int [i];
	}
	convIndex	= 0;
	convBuffer. resize (convBufferSize + 1);

	theBuffer	= new RingBuffer<std::complex<float>>
	                                                    (1024 * 1024 * 8);
	tabWidget	-> setCurrentIndex (0);
	connect (linearitySlider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_linearity (int)));
	connect (sensitivitySlider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_sensitivity (int)));
	connect (lnaSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_lna_gain (int)));
	connect (vgaSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_vga_gain (int)));
	connect (mixerSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_mixer_gain (int)));
	connect (lnaButton, SIGNAL (clicked (void)),
	         this, SLOT (set_lna_agc (void)));
	connect (mixerButton, SIGNAL (clicked (void)),
	         this, SLOT (set_mixer_agc (void)));
	connect (biasButton, SIGNAL (clicked (void)),
	         this, SLOT (set_rf_bias (void)));
	connect (tabWidget, SIGNAL (currentChanged (int)),
	         this, SLOT (show_tab (int)));
	displaySerial	-> setText (getSerial());
	running. store (false);
//	my_airspy_set_rf_bias (device, rf_bias ? 1 : 0);

	show_tab (0);			// will set currentTab
}

	airspyHandler::~airspyHandler() {
	airspySettings	-> beginGroup ("airspyHandler");
	airspySettings -> setValue ("linearity", linearitySlider -> value());
	airspySettings -> setValue ("sensitivity", sensitivitySlider -> value());
	airspySettings -> setValue ("vga", vgaGain);
	airspySettings -> setValue ("mixer", mixerGain);
	airspySettings -> setValue ("lna", lnaGain);
	airspySettings	-> setValue ("airspyOffset", coarseOffset);
	airspySettings	-> endGroup();
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
	if (filter	!= nullptr)
	   delete filter;
	delete myFrame;
	if (Handle == nullptr) {
	   return;	// nothing achieved earlier
	}
	my_airspy_exit();
#ifdef __MINGW32__
	FreeLibrary (Handle);
#else
	dlclose (Handle);
#endif
err:
	if (theBuffer != nullptr)
	   delete theBuffer;
}

void	airspyHandler::setVFOFrequency (int32_t nf) {
int result = my_airspy_set_freq (device, lastFrequency = nf);

	if (result != AIRSPY_SUCCESS) {
	   printf ("my_airspy_set_freq() failed: %s (%d)\n",
	            my_airspy_error_name((airspy_error)result), result);
	}
}

int32_t	airspyHandler::getVFOFrequency() {
	return lastFrequency;
}

int32_t	airspyHandler::defaultFrequency() {
	return Khz (94700);
}

bool	airspyHandler::restartReader() {
int	result;
int32_t	bufSize	= EXTIO_NS * EXTIO_BASE_TYPE_SIZE * 2;
	if (running. load())
	   return true;

	theBuffer	-> FlushRingBuffer();
	result = my_airspy_set_sample_type (device, AIRSPY_SAMPLE_INT16_IQ);
//	result = my_airspy_set_sample_type (device, AIRSPY_SAMPLE_FLOAT32_IQ);
	if (result != AIRSPY_SUCCESS) {
	   printf ("my_airspy_set_sample_type() failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	   return false;
	}

	if (currentTab == 0)
	   set_sensitivity	(sensitivitySlider -> value());
	else 
	if (currentTab	== 1)
	   set_linearity	(linearitySlider -> value());
	else {
	   set_vga_gain		(vgaGain);
	   set_mixer_gain	(mixerGain);
	   set_lna_gain		(lnaGain);
	}
	
	result = my_airspy_start_rx (device,
	            (airspy_sample_block_cb_fn)callback, this);
	if (result != AIRSPY_SUCCESS) {
	   printf ("my_airspy_start_rx() failed: %s (%d)\n",
	         my_airspy_error_name((airspy_error)result), result);
	   return false;
	}
//
	running. store (true);
	return true;
}

void	airspyHandler::stopReader() {
int	result;

	if (!running. load())
	   return;
	result = my_airspy_stop_rx (device);

	if (result != AIRSPY_SUCCESS ) 
	   printf ("my_airspy_stop_rx() failed: %s (%d)\n",
	          my_airspy_error_name ((airspy_error)result), result);

	running. store (false);
}
//
//	Directly copied from the airspy extio dll from Andrea Montefusco
int airspyHandler::callback (airspy_transfer* transfer) {
airspyHandler *p;

	if (!transfer)
	   return 0;		// should not happen
	p = static_cast<airspyHandler *> (transfer -> ctx);

// we read  AIRSPY_SAMPLE_INT16_IQ:
	int32_t bytes_to_write = transfer -> sample_count * sizeof (int16_t) * 2; 
	uint8_t *pt_rx_buffer   = (uint8_t *)transfer->samples;
	p -> data_available (pt_rx_buffer, bytes_to_write);
	return 0;
}

//	called from AIRSPY data callback
//	2*2 = 4 bytes for sample, as per AirSpy USB data stream format
//	we do the rate conversion here, read in groups of 2 * xxx samples
//	and transform them into groups of 2 * 512 samples
int 	airspyHandler::data_available (void *buf, int buf_size) {	
int16_t	*sbuf	= (int16_t *)buf;
int nSamples	= buf_size / (sizeof (int16_t) * 2);
std::complex<float> temp [2048];
int32_t  i, j;

//	if (filtering) {
//	   for (i = 0; i < nSamples; i ++) {
//	      convBuffer [convIndex ++] = filter -> Pass (
//	                                        sbuf [2 * i] / (float)2048,
//	                                        sbuf [2 * i + 1] / (float)2048);
//	      if (convIndex > convBufferSize) {
//	         for (j = 0; j < 2048; j ++) {
//	            int16_t  inpBase	= mapTable_int [j];
//	            float    inpRatio	= mapTable_float [j];
//	            temp [j]	= cmul (convBuffer [inpBase + 1], inpRatio) + 
//	                             cmul (convBuffer [inpBase], 1 - inpRatio);
//	         }
//
//	         theBuffer	-> putDataIntoBuffer (temp, 2048);
////
////	shift the sample at the end to the beginning, it is needed
////	as the starting sample for the next time
//	         convBuffer [0] = convBuffer [convBufferSize];
//	         convIndex = 1;
//	      }
//	   }
//	}
//	else
	for (i = 0; i < nSamples; i ++) {
	   convBuffer [convIndex ++] = std::complex<float> (
	                                     sbuf [2 * i] / (float)2048,
	                                     sbuf [2 * i + 1] / (float)2048);
	   if (convIndex > convBufferSize) {
	      for (j = 0; j < 2048; j ++) {
	         int16_t  inpBase	= mapTable_int [j];
	         float    inpRatio	= mapTable_float [j];
	         temp [j]	= cmul (convBuffer [inpBase + 1], inpRatio) + 
	                          cmul (convBuffer [inpBase], 1 - inpRatio);
	      }

	      theBuffer	-> putDataIntoBuffer (temp, 2048);
//
//	shift the sample at the end to the beginning, it is needed
//	as the starting sample for the next time
	      convBuffer [0] = convBuffer [convBufferSize];
	      convIndex = 1;
	   }
	}
	return 0;
}
//
const char *airspyHandler::getSerial() {
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
int	airspyHandler::open() {
int result = my_airspy_open (&device);

	if (result != AIRSPY_SUCCESS) {
	   printf ("airspy_open() failed: %s (%d)\n",
	          my_airspy_error_name((airspy_error)result), result);
	   return -1;
	} else {
	   return 0;
	}
}

//
//	These functions are added for the SDR-J interface
void	airspyHandler::resetBuffer() {
	theBuffer	-> FlushRingBuffer();
}

int16_t	airspyHandler::bitDepth() {
	return 13;
}

int32_t	airspyHandler::getSamples (std::complex<float> *v, int32_t size) {

	return theBuffer	-> getDataFromBuffer (v, size);
}

int32_t	airspyHandler::Samples() {
	return theBuffer	-> GetRingBufferReadAvailable();
}
//
#define GAIN_COUNT (22)

uint8_t airspy_linearity_vga_gains[GAIN_COUNT] = { 13, 12, 11, 11, 11, 11, 11, 10, 10, 10, 10, 10, 10, 10, 10, 10, 9, 8, 7, 6, 5, 4 };
uint8_t airspy_linearity_mixer_gains[GAIN_COUNT] = { 12, 12, 11, 9, 8, 7, 6, 6, 5, 0, 0, 1, 0, 0, 2, 2, 1, 1, 1, 1, 0, 0 };
uint8_t airspy_linearity_lna_gains[GAIN_COUNT] = { 14, 14, 14, 13, 12, 10, 9, 9, 8, 9, 8, 6, 5, 3, 1, 0, 0, 0, 0, 0, 0, 0 };
uint8_t airspy_sensitivity_vga_gains[GAIN_COUNT] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4 };
uint8_t airspy_sensitivity_mixer_gains[GAIN_COUNT] = { 12, 12, 12, 12, 11, 10, 10, 9, 9, 8, 7, 4, 4, 4, 3, 2, 2, 1, 0, 0, 0, 0 };
uint8_t airspy_sensitivity_lna_gains[GAIN_COUNT] = { 14, 14, 14, 14, 14, 14, 14, 14, 14, 13, 12, 12, 9, 9, 8, 7, 6, 5, 3, 2, 1, 0 };

void	airspyHandler::set_linearity (int value) {
int	result = my_airspy_set_linearity_gain (device, value);
int	temp;
	if (result != AIRSPY_SUCCESS) {
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

void	airspyHandler::set_sensitivity (int value) {
int	result = my_airspy_set_sensitivity_gain (device, value);
int	temp;
	if (result != AIRSPY_SUCCESS) {
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

//

//	Original functions from the airspy extio dll
/* Parameter value shall be between 0 and 15 */
void	airspyHandler::set_lna_gain (int value) {
int result = my_airspy_set_lna_gain (device, lnaGain = value);

	if (result != AIRSPY_SUCCESS) {
	   printf ("airspy_set_lna_gain() failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	}
	else
	   lnaDisplay	-> display (value);
}

/* Parameter value shall be between 0 and 15 */
void	airspyHandler::set_mixer_gain (int value) {
int result = my_airspy_set_mixer_gain (device, mixerGain = value);

	if (result != AIRSPY_SUCCESS) {
	   printf ("airspy_set_mixer_gain() failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	}
	else
	   mixerDisplay	-> display (value);
}

/* Parameter value shall be between 0 and 15 */
void	airspyHandler::set_vga_gain (int value) {
int result = my_airspy_set_vga_gain (device, vgaGain = value);

	if (result != AIRSPY_SUCCESS) {
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
void	airspyHandler::set_lna_agc() {
	lna_agc	= !lna_agc;
int result = my_airspy_set_lna_agc (device, lna_agc ? 1 : 0);

	if (result != AIRSPY_SUCCESS) {
	   printf ("airspy_set_lna_agc() failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	}
	if (lna_agc)
	   lnaButton	-> setText ("lna agc on");
	else
	   lnaButton	-> setText ("lna agc");
}

/* Parameter value:
	0=Disable MIXER Automatic Gain Control
	1=Enable MIXER Automatic Gain Control
*/
void	airspyHandler::set_mixer_agc() {
	mixer_agc	= !mixer_agc;

int result = my_airspy_set_mixer_agc (device, mixer_agc ? 1 : 0);

	if (result != AIRSPY_SUCCESS) {
	   printf ("airspy_set_mixer_agc() failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	}
	if (mixer_agc)
	   mixerButton	-> setText ("mixer agc on");
	else
	   mixerButton	-> setText ("mixer agc");
}


/* Parameter value shall be 0=Disable BiasT or 1=Enable BiasT */
void	airspyHandler::set_rf_bias() {
	rf_bias	= !rf_bias;
int result = my_airspy_set_rf_bias (device, rf_bias ? 1 : 0);

	if (result != AIRSPY_SUCCESS) {
	   printf("airspy_set_rf_bias() failed: %s (%d)\n",
	           my_airspy_error_name ((airspy_error)result), result);
	}
}


const char* airspyHandler::board_id_name() {
uint8_t bid;

	if (my_airspy_board_id_read (device, &bid) == AIRSPY_SUCCESS)
	   return my_airspy_board_id_name ((airspy_board_id)bid);
	else
	   return "UNKNOWN";
}
//
//
bool	airspyHandler::load_airspyFunctions() {
//
//	link the required procedures
	my_airspy_init	= (pfn_airspy_init)
	                       GETPROCADDRESS (Handle, "airspy_init");
	if (my_airspy_init == nullptr) {
	   fprintf (stderr, "Could not find airspy_init\n");
	   return false;
	}

	my_airspy_exit	= (pfn_airspy_exit)
	                       GETPROCADDRESS (Handle, "airspy_exit");
	if (my_airspy_exit == nullptr) {
	   fprintf (stderr, "Could not find airspy_exit\n");
	   return false;
	}

	my_airspy_open	= (pfn_airspy_open)
	                       GETPROCADDRESS (Handle, "airspy_open");
	if (my_airspy_open == nullptr) {
	   fprintf (stderr, "Could not find airspy_open\n");
	   return false;
	}

	my_airspy_close	= (pfn_airspy_close)
	                       GETPROCADDRESS (Handle, "airspy_close");
	if (my_airspy_close == nullptr) {
	   fprintf (stderr, "Could not find airspy_close\n");
	   return false;
	}

	my_airspy_get_samplerates	= (pfn_airspy_get_samplerates)
	                       GETPROCADDRESS (Handle, "airspy_get_samplerates");
	if (my_airspy_get_samplerates == nullptr) {
	   fprintf (stderr, "Could not find airspy_get_samplerates\n");
	   return false;
	}

	my_airspy_set_samplerate	= (pfn_airspy_set_samplerate)
	                       GETPROCADDRESS (Handle, "airspy_set_samplerate");
	if (my_airspy_set_samplerate == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_samplerate\n");
	   return false;
	}

	my_airspy_start_rx	= (pfn_airspy_start_rx)
	                       GETPROCADDRESS (Handle, "airspy_start_rx");
	if (my_airspy_start_rx == nullptr) {
	   fprintf (stderr, "Could not find airspy_start_rx\n");
	   return false;
	}

	my_airspy_stop_rx	= (pfn_airspy_stop_rx)
	                       GETPROCADDRESS (Handle, "airspy_stop_rx");
	if (my_airspy_stop_rx == nullptr) {
	   fprintf (stderr, "Could not find airspy_stop_rx\n");
	   return false;
	}

	my_airspy_set_sample_type	= (pfn_airspy_set_sample_type)
	                       GETPROCADDRESS (Handle, "airspy_set_sample_type");
	if (my_airspy_set_sample_type == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_sample_type\n");
	   return false;
	}

	my_airspy_set_freq	= (pfn_airspy_set_freq)
	                       GETPROCADDRESS (Handle, "airspy_set_freq");
	if (my_airspy_set_freq == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_freq\n");
	   return false;
	}

	my_airspy_set_lna_gain	= (pfn_airspy_set_lna_gain)
	                       GETPROCADDRESS (Handle, "airspy_set_lna_gain");
	if (my_airspy_set_lna_gain == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_lna_gain\n");
	   return false;
	}

	my_airspy_set_mixer_gain	= (pfn_airspy_set_mixer_gain)
	                       GETPROCADDRESS (Handle, "airspy_set_mixer_gain");
	if (my_airspy_set_mixer_gain == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_mixer_gain\n");
	   return false;
	}

	my_airspy_set_vga_gain	= (pfn_airspy_set_vga_gain)
	                       GETPROCADDRESS (Handle, "airspy_set_vga_gain");
	if (my_airspy_set_vga_gain == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_vga_gain\n");
	   return false;
	}
	
	my_airspy_set_linearity_gain = (pfn_airspy_set_linearity_gain)
	                       GETPROCADDRESS (Handle, "airspy_set_linearity_gain");
	if (my_airspy_set_linearity_gain == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_linearity_gain\n");
	   fprintf (stderr, "You probably did install an old library\n");
	   return false;
	}

	my_airspy_set_sensitivity_gain = (pfn_airspy_set_sensitivity_gain)
	                       GETPROCADDRESS (Handle, "airspy_set_sensitivity_gain");
	if (my_airspy_set_sensitivity_gain == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_sensitivity_gain\n");
	   fprintf (stderr, "You probably did install an old library\n");
	   return false;
	}

	my_airspy_set_lna_agc	= (pfn_airspy_set_lna_agc)
	                       GETPROCADDRESS (Handle, "airspy_set_lna_agc");
	if (my_airspy_set_lna_agc == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_lna_agc\n");
	   return false;
	}

	my_airspy_set_mixer_agc	= (pfn_airspy_set_mixer_agc)
	                       GETPROCADDRESS (Handle, "airspy_set_mixer_agc");
	if (my_airspy_set_mixer_agc == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_mixer_agc\n");
	   return false;
	}

	my_airspy_set_rf_bias	= (pfn_airspy_set_rf_bias)
	                       GETPROCADDRESS (Handle, "airspy_set_rf_bias");
	if (my_airspy_set_rf_bias == nullptr) {
	   fprintf (stderr, "Could not find airspy_set_rf_bias\n");
	   return false;
	}

	my_airspy_error_name	= (pfn_airspy_error_name)
	                       GETPROCADDRESS (Handle, "airspy_error_name");
	if (my_airspy_error_name == nullptr) {
	   fprintf (stderr, "Could not find airspy_error_name\n");
	   return false;
	}

	my_airspy_board_id_read	= (pfn_airspy_board_id_read)
	                       GETPROCADDRESS (Handle, "airspy_board_id_read");
	if (my_airspy_board_id_read == nullptr) {
	   fprintf (stderr, "Could not find airspy_board_id_read\n");
	   return false;
	}

	my_airspy_board_id_name	= (pfn_airspy_board_id_name)
	                       GETPROCADDRESS (Handle, "airspy_board_id_name");
	if (my_airspy_board_id_name == nullptr) {
	   fprintf (stderr, "Could not find airspy_board_id_name\n");
	   return false;
	}

	my_airspy_board_partid_serialno_read	=
	                (pfn_airspy_board_partid_serialno_read)
	                       GETPROCADDRESS (Handle, "airspy_board_partid_serialno_read");
	if (my_airspy_board_partid_serialno_read == nullptr) {
	   fprintf (stderr, "Could not find airspy_board_partid_serialno_read\n");
	   return false;
	}

	return true;
}

void	airspyHandler::show_tab (int t) {
	if (t == 0)		// sensitivity
	   set_sensitivity	(sensitivitySlider -> value());
	else
	if (t == 1)		// linearity
	   set_linearity	(linearitySlider -> value());
	else {			// classic view
	   set_vga_gain		(vgaGain);
	   set_mixer_gain	(mixerGain);
	   set_lna_gain		(lnaGain);
	}
	currentTab	= t;
}

int	airspyHandler::getBufferSpace	() {
	return theBuffer -> GetRingBufferWriteAvailable ();
}

