#
/*
 *    Copyright (C) 201 .. 2024
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
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	<QThread>
#include	"rtlsdr-handler-win.h"
#include	"rtl-dongleselect.h"
#include	"position-handler.h"
#include	"rtl-sdr.h"
#include	"xml-filewriter.h"
#include	"device-exceptions.h"
#include	"errorlog.h"
#include	"settings-handler.h"

#define	CORRF		0.005
#define	READLEN_DEFAULT	(4 * 8192)
//
//	For the callback, we do need some environment which
//	is passed through the ctx parameter
//
//	This is the user-side call back function
//	ctx is the calling task

static
void	RTLSDRCallBack (uint8_t *buf, uint32_t len, void *ctx) {
rtlsdrHandler_win	*theStick	= (rtlsdrHandler_win *)ctx;

	if ((theStick == nullptr) || (len != READLEN_DEFAULT)) {
	   fprintf (stderr, "%d \n", len);
	   return;
	}

	if (theStick -> isActive. load ()) 	
	   static_cast<rtlsdrHandler_win *>(ctx) -> processBuffer (buf, len);
}
//
//	for handling the events in libusb, we need a controlthread
//	whose sole purpose is to process the rtlsdr_read_async function
//	from the lib.
class	dll_driver_win : public QThread {
private:
	rtlsdrHandler_win	*theStick;
public:

	dll_driver_win (rtlsdrHandler_win *d) {
	theStick	= d;
	start		();
}

	~dll_driver_win	() {
}

private:
void	run () {
	rtlsdr_read_async (theStick -> theDevice,
	                   (rtlsdr_read_async_cb_t)&RTLSDRCallBack,
	                   (void *)theStick,
	                   0,
	                   READLEN_DEFAULT);
	fprintf (stderr, "dll_task terminates\n");
	}
};
//
//	Our wrapper is a simple classs
//
//	The Windows implementation differs from the Linux one. It
//	turns out that windows has some (well, a lot) problems with
//	enthousiastic killing and restarting the usb-interfacing thread
//	So, here we have the solution that the support thread remains
//	active during stop/restart sequences
	rtlsdrHandler_win::rtlsdrHandler_win (QSettings *s,
	                                      const QString &recorderVersion,
	                                      errorLogger *theLogger):
	                                 _I_Buffer (8 * 1024 * 1024),
	                                 theFilter (5, 1560000 / 2, 2048000) {
int16_t	deviceCount;
int32_t	r;
int16_t	deviceIndex;
QString	temp;
int	k;
char	manufac [256], product [256], serial [256];
	rtlsdrSettings			= s;
	this	-> recorderVersion	= recorderVersion;
	this	-> theErrorLogger	= theLogger;
        setupUi (&myFrame);
//	setPositionAndSize (s, &myFrame, "rtlsdrSettings");
	myFrame. show();
	filtering			= false;

	currentDepth	=  value_i (rtlsdrSettings, "rtlsdrSettings",
	                                                 "filterDepth", 5);
	filterDepth	-> setValue (currentDepth);
	theFilter. resize (currentDepth);

	inputRate		= 2048000;
	workerHandle		= nullptr;
	isActive. store (false);

//	Ok, from here we have the library functions accessible
	deviceCount 		= rtlsdr_get_device_count ();
	if (deviceCount == 0) {
	   throw (device_exception ("No device found\n"));
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
	r		=  rtlsdr_open (&theDevice, deviceIndex);
	if (r < 0) {
	   throw (device_exception ("Opening rtlsdr device failed"));
	}

	deviceModel	= rtlsdr_get_device_name (deviceIndex);
	deviceVersion	-> setText (deviceModel);
	QString	tunerType	= get_tunerType (rtlsdr_get_tuner_type (theDevice));
	product_display	-> setText (tunerType);

	r		= rtlsdr_set_sample_rate (theDevice, inputRate);
	if (r < 0) {
	   throw (device_exception ("Setting samplerate failed\n"));
	}

	gainsCount = rtlsdr_get_tuner_gains (theDevice, nullptr);
	fprintf (stderr, "Supported gain values (%d): ", gainsCount);
	{  int gains [gainsCount];
	   gainsCount	= rtlsdr_get_tuner_gains (theDevice, gains);
	   for (int i = gainsCount; i > 0; i--) {
	      fprintf (stderr, "%.1f ", gains [i - 1] / 10.0);
	      gainControl -> addItem (QString::number (gains [i - 1]));
	   }
	   fprintf (stderr, "\n");
	}

	rtlsdr_set_center_freq (theDevice, 220000000);
	rtlsdr_set_tuner_bandwidth (theDevice, KHz (1575));
	rtlsdr_set_tuner_gain_mode (theDevice, 1);
//
//	See what the saved values are and restore the GUI settings
	temp	= value_s (rtlsdrSettings, "rtlsdrSettings",
	                                   "externalGain", "10");
	k	= gainControl -> findText (temp);
	gainControl	-> setCurrentIndex (k != -1 ? k : gainsCount / 2);

	temp		= value_s (rtlsdrSettings, "rtlsdrSettings",
	                                      "autogain", "autogain_on");
	agcControl	-> setChecked (temp == "autogain_on");
	
	ppm_correction	->
	     setValue (value_i (rtlsdrSettings, "rtlsdrSettings",
	                                               "ppm_correction", 0));
	save_gainSettings	=
	     value_i (rtlsdrSettings, "rtlsdrSettings",
	                               "save_gainSettings", 1) != 0;

	rtlsdr_get_usb_strings (theDevice, manufac, product, serial);
	fprintf (stderr, "%s %s %s\n",
	            manufac, product, serial);

//	all sliders/values are set to previous values, now do the settings
//	based on these slider values
	if (agcControl -> isChecked ())
	   rtlsdr_set_agc_mode (theDevice, 1);
	else
	   rtlsdr_set_agc_mode (theDevice, 0);
	rtlsdr_set_tuner_gain	(theDevice, 
	                         gainControl -> currentText (). toInt ());
	set_ppmCorrection	(ppm_correction -> value());

	for (int i = 0; i < 256; i ++)
	   convTable [i] = ((float)i - 127.38) / 128.0;

//	and attach the buttons/sliders to the actions
	connect (gainControl,
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 2)
	         qOverload<const QString &>(&QComboBox::textActivated),
#else
	         qOverload<const QString &>(&QComboBox::activated),
#endif
	         this, &rtlsdrHandler_win::set_ExternalGain);
#if QT_VERSION >= QT_VERSION_CHECK (6, 7, 0)
	connect (agcControl, &QCheckBox::checkStateChanged,
#else
	connect (agcControl, &QCheckBox::stateChanged,
#endif
	         this, &rtlsdrHandler_win::set_autogain);
	connect (ppm_correction, qOverload<int>(&QSpinBox::valueChanged),
	         this, &rtlsdrHandler_win::set_ppmCorrection);
	connect (xml_dumpButton, &QPushButton::clicked,
	         this, &rtlsdrHandler_win::set_xmlDump);
	connect (iq_dumpButton, &QPushButton::clicked,
	         this, &rtlsdrHandler_win::set_iqDump);
#if QT_VERSION >= QT_VERSION_CHECK (6, 7, 0)
	connect (biasControl, &QCheckBox::checkStateChanged,
#else
	connect (biasControl, &QCheckBox::stateChanged,
#endif
	         this, &rtlsdrHandler_win::set_biasControl);
#if QT_VERSION >= QT_VERSION_CHECK (6, 7, 0)
	connect (filterSelector, &QCheckBox::checkStateChanged,
#else
	connect (filterSelector, &QCheckBox::stateChanged,
#endif
	         this, &rtlsdrHandler_win::set_filter);
//
//	and for saving/restoring the gain setting:
	connect (this, &rtlsdrHandler_win::new_gainIndex,
	         gainControl, &QComboBox::setCurrentIndex);
	connect (this, &rtlsdrHandler_win::new_agcSetting,
	         agcControl, &QCheckBox::setChecked);
	iqDumper	= nullptr;
	xmlWriter	= nullptr;
	iq_dumping. store (false);
	xml_dumping. store (false);
}

	rtlsdrHandler_win::~rtlsdrHandler_win	() {
	stopReader	();
	if (workerHandle != nullptr) {
	   rtlsdr_cancel_async (theDevice);
	   while (!workerHandle -> isFinished()) 
	      usleep (200);
	   _I_Buffer. FlushRingBuffer();
	   delete	workerHandle;
	   workerHandle	= nullptr;
//	   rtlsdr_close (theDevice);	// will crash if activated
	}

	QString gainText	= gainControl -> currentText ();
	storeWidgetPosition (rtlsdrSettings, &myFrame, "rtlsdrSettings");
	store (rtlsdrSettings, "rtlsdrSettings",
	                "externalGain", gainText);
	store (rtlsdrSettings, "rtlsdrSettings",
	                "autogain", agcControl -> isChecked () ? 1 : 0);
	store (rtlsdrSettings, "rtlsdrSettings",
	                "ppm_correction", ppm_correction -> value());
	store (rtlsdrSettings, "rtlsdrSettings", 
	                "filterDepth", filterDepth -> value ());
	rtlsdrSettings	-> sync ();
	usleep (1000);
	myFrame. hide ();
}
//
void	rtlsdrHandler_win::set_filter	(int c) {
	(void)c;
	filtering       = filterSelector -> isChecked ();
}

bool	rtlsdrHandler_win::restartReader	(int32_t freq, int skipped) {
	_I_Buffer. FlushRingBuffer();

	(void)(rtlsdr_set_center_freq (theDevice, freq));
	if (save_gainSettings)
	   update_gainSettings (freq / MHz (1));

	lastFrequency	= freq;
	this	-> toSkip	= skipped;
	set_autogain (agcControl -> isChecked ());
	set_ExternalGain (gainControl -> currentText ());
	if (workerHandle == nullptr) {
	   (void)rtlsdr_reset_buffer (theDevice);
	   workerHandle	= new dll_driver_win (this);
	}
	isActive. store (true);
	return true;
}

void	rtlsdrHandler_win::stopReader () {
	isActive. store (false);
	_I_Buffer. FlushRingBuffer();
	close_xmlDump ();
	if (save_gainSettings)
	   record_gainSettings	((int32_t)(rtlsdr_get_center_freq (theDevice)) / MHz (1));
}
//
//	when selecting  the gain from a table, use the table value
void	rtlsdrHandler_win::set_ExternalGain	(const QString &gain) {
	int res = rtlsdr_set_tuner_gain (theDevice, gain. toInt());
	if (res != 0) { 
	   QString t = QString ("cannot set gain to ") + gain;
	   theErrorLogger -> add ("RTLSDR", t);	
	}
}
//
void	rtlsdrHandler_win::set_autogain	(int dummy) {
	(void)dummy;
	int res = rtlsdr_set_agc_mode (theDevice,
	                               agcControl -> isChecked () ? 1 : 0);
	if (res != 0) {
	   QString t = "Problem with agcControl to " +
	                 QString::number (agcControl -> isChecked () ? 1 : 0);
	   theErrorLogger -> add ("RTLSDR", t);
	}
	res = rtlsdr_set_tuner_gain (theDevice, 
	                       gainControl -> currentText (). toInt ());
	if (res != 0) {
	   QString t = "Problem with gaincontrol to " +
	                 QString::number (agcControl -> isChecked () ? 1 : 0);
	   theErrorLogger -> add ("RTLSDR", t);
	}
}
//
void	rtlsdrHandler_win::set_biasControl	(int dummy) {
	(void)dummy;
	int res = rtlsdr_set_bias_tee (theDevice,
	                 biasControl -> isChecked () ? 1 : 0);
	if (res != 0) {
	   QString t = "Problem with biascontrol to " +
	                 QString::number (biasControl -> isChecked () ? 1 : 0);
	   theErrorLogger -> add ("RTLSDR", t);
	}
}
//	correction is in Hz
void	rtlsdrHandler_win::set_ppmCorrection	(int32_t ppm) {
	int res = rtlsdr_set_freq_correction (theDevice, ppm);
	if (res != 0) {
	   QString t = "Problem with ppm correction to " +
	                 QString::number (ppm);
	   theErrorLogger -> add ("RTLSDR", t);
	}
}

int32_t	rtlsdrHandler_win::getSamples (std::complex<float> *V, int32_t size) { 
	if (!isActive. load ())
	   return 0;
	return _I_Buffer. getDataFromBuffer (V, size);
}

int32_t	rtlsdrHandler_win::Samples () {
	if (!isActive. load ())
	   return 0;
	return _I_Buffer. GetRingBufferReadAvailable ();
}

void	rtlsdrHandler_win::resetBuffer() {
	_I_Buffer. FlushRingBuffer();
}

int16_t	rtlsdrHandler_win::maxGain() {
	return gainsCount;
}

int16_t	rtlsdrHandler_win::bitDepth() {
	return 8;
}

QString	rtlsdrHandler_win::deviceName	() {
	return deviceModel;
}

void	rtlsdrHandler_win::set_iqDump	() {
	if (iqDumper == nullptr) {
	   if (setup_iqDump ()) {
	      xml_dumpButton	-> hide ();
	   }
	}
	else {
	   close_iqDump ();
	   xml_dumpButton	-> show ();
	}
}

bool	rtlsdrHandler_win::setup_iqDump () {
	QString fileName = QFileDialog::getSaveFileName (nullptr,
	                                         tr ("Save file ..."),
	                                         QDir::homePath(),
	                                         tr ("raw (*.raw)"));
	fileName        = QDir::toNativeSeparators (fileName);
	iqDumper	= fopen (fileName. toUtf8 (). data (), "w");
	if (iqDumper == nullptr)
	   return false;
	iq_dumpButton	-> setText ("writing raw file");
	iq_dumping. store (true);
	return true;
}

void	rtlsdrHandler_win::close_iqDump () {
	if (iqDumper == nullptr)	// this can happen !!
	   return;
	iq_dumping. store (false);
	fclose (iqDumper);
	iq_dumpButton	-> setText ("Dump to raw");
	iqDumper	= nullptr;
}
	   
void	rtlsdrHandler_win::set_xmlDump () {
	if (!xml_dumping. load ()) {
	   if (setup_xmlDump ()) 
	      iq_dumpButton	-> hide	();
	}
	else {
	   close_xmlDump ();
	   iq_dumpButton	-> show	();
	}
}

bool	rtlsdrHandler_win::setup_xmlDump () {
QString channel		= rtlsdrSettings -> value ("channel", "xx").
	                                                      toString ();
	xmlWriter	= nullptr;
	try {
	   xmlWriter	= new xml_fileWriter (rtlsdrSettings,
	                                      channel,
	                                      8,
	                                      "uint8",
	                                      2048000,
	                                      lastFrequency,
	                                      rtlsdr_get_tuner_gain (theDevice),
	                                      "RTLSDR",
	                                      deviceModel,
	                                      recorderVersion);
	} catch (...) {
	   return false;
	}
	xml_dumping. store (true);
	xml_dumpButton	-> setText ("writing xml file");
	return true;
}
	
void	rtlsdrHandler_win::close_xmlDump () {
	if (xmlWriter == nullptr)
	   return;
	usleep (1000);
	xmlWriter	-> computeHeader ();
	delete xmlWriter;
	xmlWriter	= nullptr;
	xml_dumping. store (false);
	xml_dumpButton	-> setText ("Dump to xml");
}

////////////////////////////////////////////////////////////////////////
//
//      the frequency (the MHz component) is used as key
//
void    rtlsdrHandler_win::record_gainSettings (int freq) {
QString	gain	= gainControl	-> currentText ();
int	agc	= agcControl	-> isChecked () ? 1 : 0;
QString theValue        = gain + ":" + QString::number (agc);
QString key		= QString::number (freq);
	store (rtlsdrSettings, "rtlsdrSettings", key, theValue);
}

void	rtlsdrHandler_win::update_gainSettings (int freq) {
int	agc;
QString	theValue	= "";
QString freqS		= QString::number (freq);
	theValue	=  value_s (rtlsdrSettings, "rtlsdrSettings",
	                                              freqS, theValue);
	if (theValue == QString (""))
	   return;		// or set some defaults here

	QStringList result	= theValue. split (":");
	if (result. size () != 2) 	// should not happen
	   return;

	QString temp = result. at (0);
	agc	= result. at (1). toInt ();

	int k	= gainControl -> findText (temp);
	if (k != -1) {
	   gainControl	-> blockSignals (true);
	   new_gainIndex (k);
	   while (gainControl -> currentText () != temp)
	      usleep (1000);
	   gainControl	-> blockSignals (false);
	}

	agcControl	-> blockSignals (true);
	new_agcSetting (agc == 1);
	while (agcControl -> isChecked () != (agc == 1))
	   usleep (1000);
	set_autogain (agcControl -> isChecked ());
	agcControl	-> blockSignals (false);
}

#define	IQ_BUFSIZE	4096
void	rtlsdrHandler_win::processBuffer (uint8_t *buf, uint32_t len) {
float	sumI	= 0;
float	sumQ	= 0;
static
float	m_dcI	= 0;
static
float	m_dcQ	= 0;
uint32_t	nrSamples = len / 2;
auto	*tempBuf	= dynVec (std::complex<float>, nrSamples);
static uint8_t dumpBuffer [2 * IQ_BUFSIZE];
static int iqTeller	= 0;

	if (!isActive. load ()) 
	   return;

	if (toSkip > 0) {
	   toSkip -= nrSamples;
	   return;
	}
	if (xml_dumping. load ())
	   xmlWriter -> add ((std::complex<uint8_t> *)buf, nrSamples);

	if (iq_dumping. load ()) {
	   for (uint32_t i = 0; i < nrSamples; i ++) {
	      dumpBuffer [2 * iqTeller]	= buf [2 * i];
	      dumpBuffer [2 * iqTeller + 1] = buf [2 * i + 1];
	      iqTeller ++;
	      if (iqTeller >= IQ_BUFSIZE) {
	         fwrite (dumpBuffer, 2, IQ_BUFSIZE, iqDumper);
	         iqTeller = 0;
	      }
	   }
	}

	if ((filtering) && (filterDepth -> value () != currentDepth)) {
	   currentDepth = filterDepth -> value ();
	   theFilter. resize (currentDepth);
	}
	float dcI	= m_dcI;
	float dcQ	= m_dcQ;
	for (uint32_t i = 0; i < nrSamples; i ++) {
	   float tempI	= convTable [buf [2 * i]];
	   float tempQ	= convTable [buf [2 * i + 1]];
	   sumI		+= tempI;
	   sumQ		+= tempQ;
	   tempBuf [i] = std::complex<float> (tempI, tempQ);
	   if (filtering)
	      tempBuf [i] = theFilter. Pass (tempBuf [i]);
	}
// calculate correction values for next input buffer
	m_dcI = sumI / nrSamples * CORRF + (1 - CORRF) * dcI;
	m_dcQ = sumQ / nrSamples * CORRF + (1 - CORRF) * dcQ;
	int ovf	= _I_Buffer. GetRingBufferWriteAvailable () - nrSamples;
	if (ovf < 0)
	   (void)_I_Buffer. putDataIntoBuffer (tempBuf, nrSamples + ovf);
	else
	   (void)_I_Buffer. putDataIntoBuffer (tempBuf, len / 2);
	reportOverflow (ovf < 0);
}

QString	rtlsdrHandler_win::get_tunerType	(int tunerType) {
	switch (tunerType) {
	   case RTLSDR_TUNER_E4000:
	      return "E4000";
	   case RTLSDR_TUNER_FC0012:
	      return "FC0012";
	   case RTLSDR_TUNER_FC0013:
	      return "FC0013";
	   case RTLSDR_TUNER_FC2580:
	      return "FC2580";
	   case RTLSDR_TUNER_R820T:
	      return "R820T";
	      break;
	   case RTLSDR_TUNER_R828D:
	      return "R828D";
	   default:
	      return "unknown";
	}
}

void	rtlsdrHandler_win::reportOverflow (bool ovf) {
static bool theOvf	= true;
	if (ovf && !theOvf){
	   overflowLabel -> setText ("Overload");
           overflowLabel -> setStyleSheet("QLabel {background-color : red;\
                                                   color: white}");
	   theOvf	= true;
	}
	else
	if (!ovf && theOvf) {		// space in the buffer is sufficient
	   overflowLabel -> setStyleSheet("QLabel {background-color : green;\
                                                   color: white}");
	   theOvf	= false;
	}
	else
	   theOvf = ovf;
}


