#
/*
 *    Copyright (C) 2014 .. 2023
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
#include	<QTime>
#include	<QDate>
#include	<QLabel>
#include	<QDebug>
#include	<QFileDialog>
#include	"hackrf-handler.h"
#include	"xml-filewriter.h"
#include	"device-exceptions.h"

#define	DEFAULT_VGA_GAIN	30
#define	DEFAULT_LNA_GAIN	30

	hackrfHandler::hackrfHandler  (QSettings *s,
	                               const QString &recVersion):
	                                  hackrfSettings (s),
	                                  recorderVersion (recVersion),
	                                  _I_Buffer (4 * 1024 * 1024) {

	hackrfSettings -> beginGroup ("hackrfSettings");
        int x   = hackrfSettings -> value ("position-x", 100). toInt ();
        int y   = hackrfSettings -> value ("position-y", 100). toInt ();
        hackrfSettings -> endGroup ();
        setupUi (&myFrame);
        myFrame. move (QPoint (x, y));
	this	-> inputRate		= Khz (2048);

#ifdef  __MINGW32__
	const char *libraryString = "libhackrf.dll";
#elif __linux__
	const char *libraryString = "libhackrf.so.0";
#elif __APPLE__
	const char *libraryString = "libhackrf.dylib";
#endif
	library_p = new QLibrary (libraryString);
	library_p -> load();

	if (!library_p -> isLoaded ()) {
	   throw (device_exception ("failed to open " +
	                                std::string (libraryString)));
	}

	if (!load_hackrfFunctions ()) {
	   delete library_p;
	   throw (device_exception ("could not find one or more library functions"));
	}
//
//	From here we have a library available

	lastFrequency	= Khz (220000);
//
//	See if there are settings from previous incarnations
	hackrfSettings		-> beginGroup ("hackrfSettings");
	lnaGainSlider 		-> setValue (
	       hackrfSettings -> value ("hack_lnaGain",
	                                         DEFAULT_LNA_GAIN). toInt());
	vgaGainSlider 		-> setValue (
	       hackrfSettings -> value ("hack_vgaGain",
	                                         DEFAULT_VGA_GAIN). toInt());
	bool isChecked =
	       hackrfSettings -> value ("hack_AntEnable", 0). toBool();
	biasT_button -> setCheckState (isChecked ? Qt::Checked :
	                                              Qt::Unchecked);
	isChecked	=
	       hackrfSettings -> value ("hack_AmpEnable", false). toBool();
	AmpEnableButton	-> setCheckState (isChecked ? Qt::Checked : 
	                                              Qt::Unchecked);
	ppm_correction      -> setValue (
	       hackrfSettings -> value ("hack_ppmCorrection", 0). toInt());
	save_gainSettings	=
	       hackrfSettings -> value ("save_gainSettings", 1). toInt () != 0;
	hackrfSettings	-> endGroup();
//
	if (hackrf_init () != HACKRF_SUCCESS) {
	   delete  library_p;
	   throw (device_exception ("init failed"));
	}

	if (hackrf_open (&theDevice) != HACKRF_SUCCESS) {
	   delete library_p;
	   throw (device_exception ("open failure"));
	}

	if (hackrf_set_sample_rate (theDevice, 2048000.0) != HACKRF_SUCCESS) {
	   delete library_p;
	   throw (device_exception ("error setting samplerate"));
	}

	int test = hackrf_set_baseband_filter_bandwidth (theDevice, 1750000);
	if (test != HACKRF_SUCCESS) {
	   delete library_p;
	   throw (device_exception ("failure setting bandwidth"));
	}

	if (hackrf_set_freq (theDevice, 220000000) != HACKRF_SUCCESS) {
	   delete library_p;
	   throw (device_exception ("failure setting frequency"));
	}

	uint16_t regValue;
	test = hackrf_si5351c_read (theDevice, 162, &regValue);
	if (test != HACKRF_SUCCESS) {
	   delete library_p;
	   throw (device_exception ("failure reading board name"));
	}

	int res = this -> hackrf_si5351c_write (theDevice, 162, regValue);
	if (res != HACKRF_SUCCESS) {
	   delete library_p;
	   throw (device_exception (this -> hackrf_error_name (hackrf_error (res))));
	}

	handle_LNAGain	(lnaGainSlider		-> value());
	handle_VGAGain	(vgaGainSlider		-> value());
	handle_biasT	(1);		// value is a dummy really
	handle_Ampli	(1);		// value is a dummy, really
	handle_ppmCorrection (ppm_correction	-> value());

//	and be prepared for future changes in the settings
	connect (lnaGainSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_LNAGain (int)));
	connect (vgaGainSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_VGAGain (int)));
	connect (biasT_button, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_biasT (int)));
	connect (AmpEnableButton, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_Ampli (int)));
	connect (ppm_correction, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_ppmCorrection  (int)));
	connect (dumpButton, SIGNAL (clicked ()),
	         this, SLOT (handle_xmlDump ()));

	hackrf_device_list_t *deviceList = this -> hackrf_device_list();
	if (deviceList != nullptr) {	// well, it should be
	   char *serial = deviceList -> serial_numbers [0];
	   serial_number_display -> setText (serial);
	   enum hackrf_usb_board_id board_id =
	                 deviceList -> usb_board_ids [0];
	   usb_board_id_display ->
	                setText (this -> hackrf_usb_board_id_name (board_id));
	}
	connect (this, SIGNAL (signal_antEnable (bool)),
	         biasT_button, SLOT (setChecked (bool)));
	connect (this, SIGNAL (signal_ampEnable (bool)),
	         biasT_button, SLOT (setChecked (bool)));
	connect (this, SIGNAL (signal_vgaValue (int)),
		 vgaGainSlider, SLOT (setValue (int)));
	connect (this, SIGNAL (signal_vgaValue (int)),
		 vgagainDisplay, SLOT (display (int)));
	connect (this, SIGNAL (signal_lnaValue (int)),
	         lnaGainSlider, SLOT (setValue (int)));
	connect (this, SIGNAL (signal_lnaValue (int)),
	         lnagainDisplay, SLOT (display (int)));
	xmlDumper	= nullptr;
	dumping. store (false);
	running. store (false);
}

	hackrfHandler::~hackrfHandler() {
	stopReader();
	myFrame. hide ();
	hackrfSettings	-> beginGroup ("hackrfSettings");
        hackrfSettings -> setValue ("position-x", myFrame. pos (). x ());
        hackrfSettings -> setValue ("position-y", myFrame. pos (). y ());
	hackrfSettings	-> setValue ("hack_lnaGain",
	                                 lnaGainSlider -> value());
	hackrfSettings -> setValue ("hack_vgaGain",
	                                 vgaGainSlider	-> value());
	hackrfSettings -> setValue ("hack_AntEnable",
	                             biasT_button -> checkState() == Qt::Checked ? 1 : 0);
	hackrfSettings -> setValue ("hack_AmpEnable",
	                              AmpEnableButton -> checkState() == Qt::Checked);
	hackrfSettings	-> setValue ("hack_ppmCorrection",
	                              ppm_correction -> value());
	hackrfSettings	-> endGroup();
	this	-> hackrf_close (theDevice);
	this	-> hackrf_exit();
}
//

void	hackrfHandler::handle_LNAGain	(int newGain) {
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

void	hackrfHandler::handle_VGAGain	(int newGain) {
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

void	hackrfHandler::handle_biasT (int d) {
int res;
bool	b;

	(void)d;
	b = biasT_button	-> checkState() == Qt::Checked;
	res = this -> hackrf_set_antenna_enable (theDevice, b);
//	fprintf(stderr,"Passed %d\n",(int)b);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_set_antenna_enable :\n");
	   fprintf (stderr, "%s \n",
	                    this -> hackrf_error_name (hackrf_error (res)));
	   return;
	}
//	biasT_button -> setChecked (b);
}

void	hackrfHandler::handle_Ampli (int a) {
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

void	hackrfHandler::handle_ppmCorrection	(int32_t ppm) {
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
static std::complex<int8_t>buffer [32 * 32768];
static
int	callback (hackrf_transfer *transfer) {
hackrfHandler *ctx = static_cast <hackrfHandler *>(transfer -> rx_ctx);
int8_t *p	= reinterpret_cast<int8_t * const>(transfer -> buffer);
RingBuffer<std::complex<int8_t> > * q = & (ctx -> _I_Buffer);
int	bufferIndex	= 0;

	for (int i = 0; i < transfer -> valid_length / 2; i ++) {
	   int8_t re	= ((int8_t *)p) [2 * i];
	   int8_t im	= ((int8_t *)p) [2 * i + 1];
	   buffer [bufferIndex]	= std::complex<int8_t> (re, im);
	   bufferIndex ++;
	}
	q -> putDataIntoBuffer (buffer, bufferIndex);
	return 0;
}

bool	hackrfHandler::restartReader	(int32_t freq) {
int	res;

	if (running. load())
	   return true;

	lastFrequency	= freq;
	if (save_gainSettings)
	   update_gainSettings (freq / MHz (1));
	this -> hackrf_set_lna_gain (theDevice, lnaGainSlider -> value ());
	this -> hackrf_set_vga_gain (theDevice, vgaGainSlider -> value ());
	this -> hackrf_set_amp_enable (theDevice, 
	                               AmpEnableButton -> isChecked () ? 1 : 0);
	this -> hackrf_set_antenna_enable (theDevice, 
	                            biasT_button -> isChecked () ? 1 : 0);

	res	= this -> hackrf_set_freq (theDevice, freq);
	if (res != HACKRF_SUCCESS) {
	   fprintf (stderr, "Problem with hackrf_set_freq: \n");
	   fprintf (stderr, "%s \n",
	                 this -> hackrf_error_name (hackrf_error (res)));
	   return false;
	}
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

void	hackrfHandler::stopReader () {
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

	if (save_gainSettings)
	   record_gainSettings	(lastFrequency / MHz (1));
	running. store (false);
}

//
//	The brave old getSamples. For the hackrf, we get
//	size still in I/Q pairs
int32_t	hackrfHandler::getSamples (std::complex<float> *V, int32_t size) { 
std::complex<int8_t> temp [size];
	int amount      = _I_Buffer. getDataFromBuffer (temp, size);
	for (int i = 0; i < amount; i ++)
	   V [i] = std::complex<float> (real (temp [i]) / 127.0f,
	                                      imag (temp [i]) / 127.0f);
	if (dumping. load ())
	   xmlWriter -> add (temp, amount);
	return amount;
}

int32_t	hackrfHandler::Samples () {
	return _I_Buffer. GetRingBufferReadAvailable();
}

void	hackrfHandler::resetBuffer() {
	_I_Buffer. FlushRingBuffer();
}

int16_t	hackrfHandler::bitDepth() {
	return 8;
}

QString	hackrfHandler::deviceName	() {
	return "hackRF";
}

bool	hackrfHandler::load_hackrfFunctions () {
//
//	link the required procedures
	this -> hackrf_init	=
	                (pfn_hackrf_init) library_p -> resolve ("hackrf_init");
	if (this -> hackrf_init == nullptr) {
	   fprintf (stderr, "Could not find hackrf_init\n");
	   return false;
	}

	this -> hackrf_open	=
	                (pfn_hackrf_open) library_p -> resolve  ("hackrf_open");
	if (this -> hackrf_open == nullptr) {
	   fprintf (stderr, "Could not find hackrf_open\n");
	   return false;
	}

	this -> hackrf_close	=
	                (pfn_hackrf_close) library_p -> resolve ("hackrf_close");
	if (this -> hackrf_close == nullptr) {
	   fprintf (stderr, "Could not find hackrf_close\n");
	   return false;
	}

	this -> hackrf_exit	=
	                (pfn_hackrf_exit) library_p -> resolve ("hackrf_exit");
	if (this -> hackrf_exit == nullptr) {
	   fprintf (stderr, "Could not find hackrf_exit\n");
	   return false;
	}

	this -> hackrf_start_rx	=
	                (pfn_hackrf_start_rx) library_p -> resolve ("hackrf_start_rx");
	if (this -> hackrf_start_rx == nullptr) {
	   fprintf (stderr, "Could not find hackrf_start_rx\n");
	   return false;
	}

	this -> hackrf_stop_rx	=
	                (pfn_hackrf_stop_rx) library_p -> resolve ("hackrf_stop_rx");
	if (this -> hackrf_stop_rx == nullptr) {
	   fprintf (stderr, "Could not find hackrf_stop_rx\n");
	   return false;
	}

	this -> hackrf_device_list	=
	                (pfn_hackrf_device_list) library_p -> resolve ("hackrf_device_list");
	if (this -> hackrf_device_list == nullptr) {
	   fprintf (stderr, "Could not find hackrf_device_list\n");
	   return false;
	}

	this -> hackrf_set_baseband_filter_bandwidth	=
	               (pfn_hackrf_set_baseband_filter_bandwidth)
	                  library_p -> resolve ("hackrf_set_baseband_filter_bandwidth");
	if (this -> hackrf_set_baseband_filter_bandwidth == nullptr) {
	   fprintf (stderr, "Could not find hackrf_set_baseband_filter_bandwidth\n");
	   return false;
	}

	this -> hackrf_set_lna_gain	=
	               (pfn_hackrf_set_lna_gain)
	                   library_p -> resolve ("hackrf_set_lna_gain");
	if (this -> hackrf_set_lna_gain == nullptr) {
	   fprintf (stderr, "Could not find hackrf_set_lna_gain\n");
	   return false;
	}

	this -> hackrf_set_vga_gain	=
	               (pfn_hackrf_set_vga_gain)
	                   library_p -> resolve ("hackrf_set_vga_gain");
	if (this -> hackrf_set_vga_gain == nullptr) {
	   fprintf (stderr, "Could not find hackrf_set_vga_gain\n");
	   return false;
	}

	this -> hackrf_set_freq	=
	               (pfn_hackrf_set_freq) library_p -> resolve ("hackrf_set_freq");
	if (this -> hackrf_set_freq == nullptr) {
	   fprintf (stderr, "Could not find hackrf_set_freq\n");
	   return false;
	}

	this -> hackrf_set_sample_rate	=
	               (pfn_hackrf_set_sample_rate)
	                   library_p -> resolve ("hackrf_set_sample_rate");
	if (this -> hackrf_set_sample_rate == nullptr) {
	   fprintf (stderr, "Could not find hackrf_set_sample_rate\n");
	   return false;
	}

	this -> hackrf_is_streaming	=
	               (pfn_hackrf_is_streaming)
	                   library_p -> resolve ("hackrf_is_streaming");
	if (this -> hackrf_is_streaming == nullptr) {
	   fprintf (stderr, "Could not find hackrf_is_streaming\n");
	   return false;
	}

	this -> hackrf_error_name	=
	               (pfn_hackrf_error_name)
	                   library_p -> resolve ("hackrf_error_name");
	if (this -> hackrf_error_name == nullptr) {
	   fprintf (stderr, "Could not find hackrf_error_name\n");
	   return false;
	}

	this -> hackrf_usb_board_id_name =
	               (pfn_hackrf_usb_board_id_name)
	                   library_p -> resolve ("hackrf_usb_board_id_name");
	if (this -> hackrf_usb_board_id_name == nullptr) {
	   fprintf (stderr, "Could not find hackrf_usb_board_id_name\n");
	   return false;
	}

	this -> hackrf_set_antenna_enable =
	              (pfn_hackrf_set_antenna_enable)
	                     library_p -> resolve ("hackrf_set_antenna_enable");
	if (this -> hackrf_set_antenna_enable == nullptr) {
	   fprintf (stderr, "Could not find hackrf_set_antenna_enable\n");
	   return false;
	}

	this -> hackrf_set_amp_enable =
	              (pfn_hackrf_set_amp_enable)
	                    library_p -> resolve ("hackrf_set_amp_enable");
	if (this -> hackrf_set_amp_enable == nullptr) {
	   fprintf (stderr, "Could not find hackrf_set_amp_enable\n");
	   return false;
	}

	this -> hackrf_si5351c_read =
	              (pfn_hackrf_si5351c_read)
	                      library_p -> resolve ("hackrf_si5351c_read");
	if (this -> hackrf_si5351c_read == nullptr) {
	   fprintf (stderr, "Could not find hackrf_si5351c_read\n");
	   return false;
	}

	this -> hackrf_si5351c_write =
	              (pfn_hackrf_si5351c_write)
	                      library_p -> resolve("hackrf_si5351c_write");
	if (this -> hackrf_si5351c_write == nullptr) {
	   fprintf (stderr, "Could not find hackrf_si5351c_write\n");
	   return false;
	}

//	this	-> hackrf_board_rev_read =
//	              (pfn_hackrf_board_rev_read)
//	                      library_p ->resolve ("hackrf_board_rev_read");
//	if (hackrf_board_rev_read == nullptr) {
//	   fprintf (stderr, "Could not find hackrf_board_rev_read\n");
//	   return false;
//	}

	fprintf (stderr, "OK, functions seem to be loaded\n");
	return true;
}

void	hackrfHandler::handle_xmlDump () {
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

bool	hackrfHandler::setup_xmlDump () {
QTime	theTime;
QDate	theDate;
QString saveDir = hackrfSettings -> value (SAVEDIR_XML,
	                                   QDir::homePath ()). toString ();
	if ((saveDir != "") && (!saveDir. endsWith ("/")))
	   saveDir += "/";

	QString channel		= hackrfSettings -> value ("channel", "xx").
	                                                   toString ();
	QString timeString      = theDate. currentDate (). toString () + "-" +
	                          theTime. currentTime (). toString ();
	for (int i = 0; i < timeString. length (); i ++)
	   if (!isValid (timeString. at (i)))
	      timeString. replace (i, 1, '-');
	QString suggestedFileName =
	            saveDir + "hackrf" + "-" + channel +  "-" + timeString;
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
	                                      "int8",
	                                      2048000,
	                                      lastFrequency,
	                                      "Hackrf",
	                                      "--",
	                                      recorderVersion);
	dumping. store (true);

	QString	dumper	= QDir::fromNativeSeparators (fileName);
	int x		= dumper. lastIndexOf ("/");
	saveDir		= dumper. remove (x, dumper. count () - x);
	hackrfSettings	-> setValue ("saveDir_xmlDump", saveDir);

	return true;
}

void	hackrfHandler::close_xmlDump () {
	if (xmlDumper == nullptr)	// this can happen !!
	   return;
	dumping. store (false);
	usleep (1000);
	xmlWriter	-> computeHeader ();
	delete xmlWriter;
	fclose (xmlDumper);
	xmlDumper	= nullptr;
}

void	hackrfHandler::record_gainSettings	(int freq) {
int	vgaValue;
int	lnaValue;
int	ampEnable;
QString theValue;

	vgaValue	= vgaGainSlider	-> value ();
	lnaValue	= lnaGainSlider	-> value ();
	ampEnable	= AmpEnableButton -> isChecked () ? 1 : 0;
	theValue	= QString::number (vgaValue) + ":";
	theValue	+= QString::number (lnaValue) + ":";
	theValue	+= QString::number (ampEnable);
	hackrfSettings  -> beginGroup ("hackrfSettings");
	hackrfSettings	-> setValue (QString::number (freq), theValue);
	hackrfSettings -> endGroup ();
}

void	hackrfHandler::update_gainSettings	(int freq) {
int	vgaValue;
int	lnaValue;
int	ampEnable;
QString	theValue	= "";

	hackrfSettings	-> beginGroup ("hackrfSettings");
	theValue	= hackrfSettings -> value (QString::number (freq), ""). toString ();
	hackrfSettings	-> endGroup ();

	if (theValue == QString (""))
	   return;		// or set some defaults here
	QStringList result	= theValue. split (":");
	if (result. size () != 3) 	// should not happen
	   return;

	vgaValue	= result. at (0). toInt ();
	lnaValue	= result. at (1). toInt ();
	ampEnable	= result. at (2). toInt ();

	vgaGainSlider	-> blockSignals (true);
	signal_vgaValue (vgaValue);
	while (vgaGainSlider -> value () != vgaValue)
	   usleep (1000);
	vgagainDisplay	-> display (vgaValue);
	vgaGainSlider	-> blockSignals (false);

	lnaGainSlider	-> blockSignals (true);
	signal_lnaValue (lnaValue);
	while (lnaGainSlider -> value () != lnaValue)
	   usleep (1000);
	lnagainDisplay	-> display (lnaValue);
	lnaGainSlider	-> blockSignals (false);

	AmpEnableButton	-> blockSignals (true);
	signal_ampEnable (ampEnable == 1);
//	while (AmpEnableButton -> isChecked () != (ampEnable == 1))
//	   usleep (1000);
	AmpEnableButton	-> blockSignals (false);
}
