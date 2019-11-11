#
/*
 *    Copyright (C) 2013, 2014, 2015, 2016, 2017, 2018, 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB (formerly SDR-J, JSDR).
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
#include	<QSettings>
#include	<QMessageBox>
#include	<QFileDialog>
#include	<QDebug>
#include	<QDateTime>
#include	<QFile>
#include	<QStringList>
#include	<QStringListModel>
#include	<QStandardItem>
#include	<QMouseEvent>
#include	<QDir>
#include	<fstream>
#include	"dab-constants.h"
#include	<iostream>
#include	<numeric>
#include	<unistd.h>
#include	<vector>
#include	"radio.h"
#include	"band-handler.h"
#include	"ensemble-printer.h"
#include	"audio-descriptor.h"
#include	"data-descriptor.h"
#include	"rawfiles.h"
#include	"wavfiles.h"
#include	"dab_tables.h"
#ifdef	TCP_STREAMER
#include	"tcp-streamer.h"
#elif	QT_AUDIO
#include	"Qt-audio.h"
#else
#include	"audiosink.h"
#endif
#ifdef	HAVE_RTLSDR
#include	"rtlsdr-handler.h"
#endif
#ifdef	HAVE_SDRPLAY
#include	"sdrplay-handler.h"
#endif
#ifdef	HAVE_ELAD_S1
#include	"elad-handler.h"
#endif
#ifdef	__MINGW32__
#ifdef	HAVE_EXTIO
#include	"extio-handler.h"
#endif
#endif
#ifdef	HAVE_RTL_TCP
#include	"rtl_tcp_client.h"
#endif
#ifdef	HAVE_AIRSPY
#include	"airspy-handler.h"
#endif
#ifdef	HAVE_HACKRF
#include	"hackrf-handler.h"
#endif
#ifdef	HAVE_SOAPY
#include	"soapy-handler.h"
#endif
#ifdef	HAVE_LIME
#include	"lime-handler.h"
#endif
#include	"ui_technical_data.h"
#include	"spectrum-viewer.h"
#include	"correlation-viewer.h"
#include	"tii-viewer.h"

std::vector<size_t> get_cpu_times() {
	std::ifstream proc_stat ("/proc/stat");
	proc_stat. ignore (5, ' ');    // Skip the 'cpu' prefix.
	std::vector<size_t> times;
	for (size_t time; proc_stat >> time; times. push_back (time));
	return times;
}
 
bool get_cpu_times (size_t &idle_time, size_t &total_time) {
	const std::vector <size_t> cpu_times = get_cpu_times();
	if (cpu_times. size() < 4)
	   return false;
	idle_time  = cpu_times [3];
	total_time = std::accumulate (cpu_times. begin(), cpu_times. end(), 0);
	return true;
}

static
uint8_t	convert (QString s) {
	if (s == "Mode 1")
	   return 1;
	if (s == "Mode 2")
	   return 2;
	if (s == "Mode 3")
	   return 3;
	if (s == "Mode 4")
	   return 4;
	return 1;
}
/**
  *	We use the creation function merely to set up the
  *	user interface and make the connections between the
  *	gui elements and the handling agents. All real action
  *	is initiated by gui buttons
  */

	RadioInterface::RadioInterface (QSettings	*Si,
	                                QString		presetFile,
	                                int32_t		dataPort,
	                                QWidget		*parent):
	                                        QWidget (parent),
	                                        my_presetHandler (this) {
int16_t	latency;
int16_t k;
QString h;

	dabSettings		= Si;
	running. store (false);
	scanning. store (false);
	tiiSwitch		= false;
	isSynced		= UNSYNCED;
	spectrumBuffer          = new RingBuffer<DSPCOMPLEX> (2 * 32768);
	iqBuffer		= new RingBuffer<std::complex<float>> (2 * 1536);
	responseBuffer		= new RingBuffer<float> (32768);
	tiiBuffer		= new RingBuffer<DSPCOMPLEX> (32768);
	audioBuffer		= new RingBuffer<int16_t>(16 * 32768);
	frameBuffer		= new RingBuffer<uint8_t> (32768);
//
//
//	latency is used to allow different settings for different
//	situations wrt the output buffering. For windows and the RPI
//	this need to be a pretty large value (e.g. 5 to 10)
	latency		=
	           dabSettings -> value ("latency", 5). toInt();



	switchTime	=
	           dabSettings -> value ("switch_time", 10000). toInt ();

	dataBuffer		= new RingBuffer<uint8_t>(32768);
///	The default, most likely to be overruled
#ifdef	_SEND_DATAGRAM_
	ipAddress		= dabSettings -> value ("ipAddress", "127.0.0.1"). toString();
	port			= dabSettings -> value ("port", 8888). toInt();
#endif
//
	has_presetName		= dabSettings -> value ("has-presetName", 1). toInt() != 0;
	if (has_presetName) 
	   presetName		= dabSettings -> value ("presetname", ""). toString();

	saveSlides	= dabSettings -> value ("saveSlides", 1). toInt();
	if (saveSlides != 0)
	   set_picturePath ();

///////////////////////////////////////////////////////////////////////////

//	The settings are done, now creation of the GUI parts
	setupUi (this);
//
	dataDisplay	= new QFrame (nullptr);
	techData. setupUi (dataDisplay);
	dataDisplay		->  hide();
#ifdef	__MINGW32__
	cpuMonitor	-> hide();
#endif
	
	bool showWidget		= dabSettings -> value ("showDeviceWidget", 0).
	                                                      toInt () != 0;
	showDeviceWidget	-> setText (showWidget ? "show" : "hide");
//
#ifdef	DATA_STREAMER
	dataStreamer		= new tcpServer (dataPort);
#else
	(void)dataPort;
#endif

//	Where do we leave the audio out?
	streamoutSelector	-> hide();
#ifdef	TCP_STREAMER
	soundOut		= new tcpStreamer	(20040);
#elif	QT_AUDIO
	soundOut		= new Qt_Audio		();
#else
// just sound out
	soundOut		= new audioSink		(latency);

	((audioSink *)soundOut)	-> setupChannels (streamoutSelector);
	streamoutSelector	-> show();
	bool err;
	h	= dabSettings -> value ("soundchannel", "default"). toString();
	k	= streamoutSelector -> findText (h);
	if (k != -1) {
	   streamoutSelector -> setCurrentIndex (k);
	   err = !((audioSink *)soundOut) -> selectDevice (k);
	}

	if ((k == -1) || err)
	   ((audioSink *)soundOut)	-> selectDefaultDevice();
#endif
//
	my_spectrumViewer	= new spectrumViewer (this, dabSettings,
	                                              spectrumBuffer,
	                                              iqBuffer);

	my_correlationViewer	= new correlationViewer (this,
	                                                 responseBuffer);

	my_tiiViewer		= new tiiViewer (this, tiiBuffer);
//
	QString t       =
	        dabSettings     -> value ("dabBand", "VHF Band III"). toString();
	uint8_t dabBand		= t == "VHF Band III" ?  BAND_III : L_BAND;

	theBand. setupChannels  (channelSelector, dabBand);

//
	QPalette p	= techData. ficError_display -> palette();
	p. setColor (QPalette::Highlight, Qt::green);
	techData. ficErrorLabel		-> setAlignment (Qt::AlignRight);
	techData. ficError_display	-> setPalette (p);
	techData. frameErrorLabel	-> setAlignment (Qt::AlignRight);
	techData. frameError_display	-> setPalette (p);
	techData. rsErrorLabel		-> setAlignment (Qt::AlignRight);
	techData. rsError_display	-> setPalette (p);
	techData. aacErrorLabel		-> setAlignment (Qt::AlignRight);
	techData. aacError_display	-> setPalette (p);
	techData. rsError_display	-> hide();
	techData. aacError_display	-> hide();
	techData. motAvailable		-> 
	                   setStyleSheet ("QLabel {background-color : red}");
	techData. fmLabel		-> hide();
//
//
	rawDumper               = nullptr;
	frameDumper             = nullptr;
	audioDumper             = nullptr;

	ficBlocks		= 0;
	ficSuccess		= 0;
	syncedLabel		->
	               setStyleSheet ("QLabel {background-color : red; color: white}");

	Services		= QStringList ();
	model . clear ();
	ensembleDisplay	-> setModel (&model);
//
	connect (streamoutSelector, SIGNAL (activated (int)),
	         this,  SLOT (set_streamSelector (int)));
	my_presetHandler. loadPresets (presetFile, presetSelector);
//	
//	display the version
	QString v = "Qt-DAB -" + QString (CURRENT_VERSION);
	QString versionText = "qt-dab version: " + QString(CURRENT_VERSION);
	versionText += "Copyright J van Katwijk, J. vanKatwijk@gmail.com\n";
	versionText += "Rights of Qt, fftw, portaudio, libsamplerate and libsndfile gratefully acknowledged";
	versionText += "Rights of other contribuants gratefully acknowledged\n";
	versionText += " Build on: " + QString(__TIMESTAMP__) + QString (" ") + QString (GITHASH);
	copyrightLabel	-> setToolTip (versionText);

	copyrightLabel	-> setAlignment (Qt::AlignRight);
	techData. programName -> setAlignment (Qt::AlignCenter);
//	and start the timer(s)
//	The displaytimer is there to show the number of
//	seconds running and handle - if available - the tii data
	displayTimer. setInterval (1000);
	connect (&displayTimer, SIGNAL (timeout (void)),
	         this, SLOT (updateTimeDisplay (void)));
	displayTimer. start (1000);
	numberofSeconds		= 0;
//
//	timer for scanning
	signalTimer. setSingleShot (true);
	signalTimer. setInterval (10000);
	connect (&signalTimer, SIGNAL (timeout (void)),
	         this, SLOT (No_Signal_Found (void)));

#ifdef	HAVE_SDRPLAY
	deviceSelector	-> addItem ("sdrplay");
#endif
#ifdef	HAVE_RTLSDR
	deviceSelector	-> addItem ("dabstick");
#endif
#ifdef	HAVE_AIRSPY
	deviceSelector	-> addItem ("airspy");
#endif
#ifdef	HAVE_ELAD_S1
	deviceSelector	-> addItem ("elad-s1");
#endif
#ifdef	HAVE_HACKRF
	deviceSelector	-> addItem ("hackrf");
#endif
#ifdef	HAVE_SOAPY
	deviceSelector	-> addItem ("soapy");
#endif
#ifdef	HAVE_LIME
	deviceSelector	-> addItem ("limeSDR");
#endif
#ifdef  HAVE_EXTIO
	deviceSelector	-> addItem ("extio");
#endif
#ifdef	HAVE_RTL_TCP
	deviceSelector	-> addItem ("rtl_tcp");
#endif

	inputDevice	= nullptr;
	h               =
	           dabSettings -> value ("device", "no device"). toString();
	k               = deviceSelector -> findText (h);
	if (k != -1) {
	   deviceSelector       -> setCurrentIndex (k);
	   inputDevice	= setDevice (deviceSelector -> currentText());
	}
	
	my_dabProcessor	= nullptr;
//	if a device was selected, we just start, otherwise
//	we wait until one is selected
	if (inputDevice != nullptr) 
	   emit doStart ();
	else 
	   connect (deviceSelector, SIGNAL (activated (QString)),
	            this,  SLOT (doStart (QString)));
	qApp	-> installEventFilter (this);
	currentService	= nullptr;
}

void	RadioInterface::doStart (QString dev) {
	(void)dev;
	inputDevice = setDevice	(deviceSelector -> currentText());
	if (inputDevice == nullptr) {
//	just in case someone wants to push all those nice buttons that
//	are now connected to erroneous constructs
//	Some buttons should not be touched before we have a device
	   disconnectGUI();
	   return;
	}
	emit doStart();
}
//
//	when doStart is called, a device is available and selected
void	RadioInterface::doStart() {
bool	r = false;
int32_t	frequency;

	QString h       = dabSettings -> value ("channel", "12C"). toString();
	int k           = channelSelector -> findText (h);
	if (k != -1) {
	   channelSelector -> setCurrentIndex (k);
	}
	
	frequency	= theBand. Frequency (
	                                      channelSelector -> currentText());
	inputDevice     -> setVFOFrequency (frequency);
	r = inputDevice		-> restartReader();
	qDebug ("Starting %d\n", r);
	if (!r) {
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("Opening  input stream failed\n"));
	   return;	// give it another try
	}
	frequencyDisplay	-> display (frequency / 1000000.0);

//	Some buttons should not be touched before we have a device
	connectGUI();

	int tii_delay	=
	           dabSettings	-> value ("tii_delay", 5). toInt();
	if (tii_delay < 5)
	   tii_delay = 5;
	int	tii_depth	=
	               dabSettings -> value ("tii_depth", 1). toInt();
	int	echo_depth	=
	               dabSettings -> value ("echo_depth", 1). toInt();
	QString dabMode		=
	               dabSettings   -> value ("dabMode", "Mode 1"). toString();
//	Performance parameters: read the README
	uint16_t threshold	=
	           dabSettings -> value ("threshold", 3). toInt();
	uint16_t diff_length	=
	           dabSettings	-> value ("diff_length", 40). toInt();
	
//	we avoided up till now connecting the channel selector
//	to the slot since that function does a lot more that we
//	do not want here
	connect (presetSelector, SIGNAL (activated (QString)),
	         this, SLOT (handle_presetSelector (QString)));
	connect (channelSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (selectChannel_usingSelector (const QString &)));
//	and we connect the deviceSelector to a handler
	disconnect (deviceSelector, SIGNAL (activated (QString)),
	            this,  SLOT (doStart (QString)));
//
//	Just to be sure we disconnect here.
//	It would have been helpful to have a function
//	testing whether or not a connection exists, we need a kind
//	of "reset"
	disconnect (deviceSelector, SIGNAL (activated (QString)),
	            this,  SLOT (newDevice (QString)));

	connect (deviceSelector, SIGNAL (activated (QString)),
	         this, SLOT (newDevice (QString)));
//
//	It is time for some action
	my_dabProcessor = new dabProcessor   (this,
	                                      inputDevice,
	                                      convert (dabMode),
	                                      threshold,
	                                      diff_length,
	                                      tii_delay,
	                                      tii_depth,
	                                      echo_depth,
	                                      picturesPath,
	                                      responseBuffer,
	                                      spectrumBuffer,
	                                      iqBuffer,
	                                      tiiBuffer,
	                                      frameBuffer
	                                      );

	if (has_presetName && (presetName != QString (""))) {
	   presetTimer. setSingleShot (true);
	   presetTimer. setInterval (switchTime);
	   connect (&presetTimer, SIGNAL (timeout (void)),
	            this, SLOT (setPresetStation (void)));
	   presetTimer. start (switchTime);
	}

	clearEnsemble();		// the display
	secondariesVector. resize (0);
	my_dabProcessor	-> start();
	running. store (true);
}

	RadioInterface::~RadioInterface() {
	fprintf (stderr, "radioInterface is deleted\n");
}
//
/**
  *	\brief At the end, we might save some GUI values
  *	The QSettings could have been the class variable as well
  *	as the parameter
  */
void	RadioInterface::dumpControlState (QSettings *s) {
	if (s == nullptr)	// cannot happen
	   return;

	if (has_presetName)
	   s	-> setValue ("presetname", serviceLabel -> text ());
	s	-> setValue ("device",
	                      deviceSelector -> currentText());
	s	-> setValue ("channel",
	                      channelSelector -> currentText());
	s	-> setValue ("soundchannel",
	                      streamoutSelector -> currentText());
	if (inputDevice != nullptr)
	   s	-> setValue ("showDeviceWidget",
	                          inputDevice -> isHidden () != 0);
	                     
	s	-> sync();
}
//
///////////////////////////////////////////////////////////////////////////////
//	
//	The public slots are called from other places within the dab software
//	so please provide some implementation, perhaps an empty one
//
//	a slot called by the ofdmprocessor
void	RadioInterface::set_CorrectorDisplay (int v) {
	if (running. load())
	   correctorDisplay	-> display (v);
}

//	If the ofdm processor has waited for a period of N frames
//	to get a start of a synchronization,
//	it sends a signal to the GUI handler
//	If "scanning" is "on" we hop to the next frequency on
//	the list

void	RadioInterface::signalTimer_out() {
	No_Signal_Found();
}

void	RadioInterface::No_Signal_Found() {
	signalTimer. stop();
	if (!running. load ())
	   return;
	disconnect (&presetTimer, SIGNAL (timeout ()),
	            this, SLOT (No_Signal_Found ()));
	disconnect (&signalTimer, SIGNAL (timeout ()),
	            this, SLOT (No_Signal_Found ()));
//	so, apparently we are scanning
//	Increment the frequency
//	and restart
	my_dabProcessor -> stop();
	while (!my_dabProcessor -> isFinished())
	   usleep (10000);
	if (!scanning. load ())
	   return;

	increment_Channel();
	clearEnsemble();
	my_dabProcessor	-> start();
	connect (&signalTimer, SIGNAL (timeout ()),
	         this, SLOT (No_Signal_Found ()));
	signalTimer. start (switchTime);
}
//
//	In case the scanning button was pressed, we
//	set it off as soon as we have a signal found
void	RadioInterface::Yes_Signal_Found() {
	if (!running. load())
	   return;
	disconnect (&signalTimer, SIGNAL (timeout ()),
	            this, SLOT (No_Signal_Found ()));
	signalTimer. stop();
	stopScanning();
}

void	RadioInterface::set_Scanning () {
	disconnect (scanButton, SIGNAL (clicked ()),
	            this, SLOT (set_Scanning ()));
	if (!running. load ())
	   return;
	if (scanning. load ())
	   stopScanning ();
	else
	   startScanning ();
	connect (scanButton, SIGNAL (clicked ()),
	         this, SLOT (set_Scanning ()));
}

void	RadioInterface::startScanning () {
	if (!running. load ())
	   return;

	presetTimer. stop ();
	disconnect (&presetTimer, SIGNAL (timeout (void)),
	            this, SLOT (setPresetStation (void)));
	disconnect (&signalTimer, SIGNAL (timeout (void)),
	            this, SLOT (setPresetStation (void)));
	setStereo (false);
	scanning. store (true);
	fprintf (stderr, "scanning is op %d gezet\n", scanning. load ());
	my_dabProcessor -> set_scanMode (scanning);
//	scanButton -> setText ("scanning");
	increment_Channel();
	fprintf (stderr, "en nu is scanning %d\n", scanning. load ());
	connect (&signalTimer, SIGNAL (timeout (void)),
	         this, SLOT (No_Signal_Found ()));
	signalTimer. start (10000);
}

void	RadioInterface::stopScanning() {
	if (!running. load())
	   return;
	disconnect (&signalTimer, SIGNAL (timeout ()),
	            this, SLOT (No_Signal_Found ()));
	scanning. store (false);
	my_dabProcessor -> set_scanMode (!scanning. load ());
	scanButton -> setText ("scan");
}
//
void	RadioInterface::increment_Channel() {
int32_t	tunedFrequency;
	if (!running. load())
	   return;

int	cc	= channelSelector -> currentIndex();

	cc	+= 1;
	if (cc >= channelSelector -> count())
	   cc = 0;
//
//	To avoid reaction of the system on setting a different value
	disconnect (channelSelector, SIGNAL (activated (const QString &)),
	              this, SLOT (selectChannel_usingSelector (const QString &)));
	channelSelector -> setCurrentIndex (cc);
	tunedFrequency	=
	         theBand. Frequency (channelSelector -> currentText());
	inputDevice	-> setVFOFrequency (tunedFrequency);
	frequencyDisplay	-> display (tunedFrequency / 1000000.0);
	connect    (channelSelector, SIGNAL (activated (const QString &)),
	             this, SLOT (selectChannel_usingSelector (const QString &)));
}

void	RadioInterface::decrement_Channel() {
int32_t	tunedFrequency;
	if (!running. load())
	   return;

int	cc	= channelSelector -> currentIndex();

	cc	-= 1;
	if (cc < 0)
	   cc = channelSelector -> count() - 1;
//
//	To avoid reaction of the system on setting a different value
	disconnect (channelSelector, SIGNAL (activated (const QString &)),
	              this, SLOT (selectChannel_usingSelector (const QString &)));
	channelSelector -> setCurrentIndex (cc);
	tunedFrequency	=
	         theBand. Frequency (channelSelector -> currentText());
	inputDevice	-> setVFOFrequency (tunedFrequency);
	frequencyDisplay	-> display (tunedFrequency / 1000000.0);

	connect    (channelSelector, SIGNAL (activated (const QString &)),
	             this, SLOT (selectChannel_usingSelector (const QString &)));
}

///////////////////////////////////////////////////////////////////////////
/**
  *	clearEnsemble
  *	on changing settings, we clear all things in the gui
  *	related to the ensemble.
  *	The function is called from "deep" within the handling code
  *	Potentially a dangerous approach, since the fic handler
  *	might run in a separate thread and generate data to be displayed
  */
void	RadioInterface::clearEnsemble() {

	clear_showElements();
	if (!running. load())
	   return;

//	it obviously means: stop processing
	my_dabProcessor	-> clearEnsemble();
	my_dabProcessor	-> reset();
}

//
//	a slot, called by the fic/fib handlers
void	RadioInterface::addtoEnsemble (const QString &s) {
	if (!running. load())
	   return;

	Services << s;
	Services. removeDuplicates();
	Services. sort();
	model. clear ();
	for (const auto serv : Services) 
	   model. appendRow (new QStandardItem (serv));
	int row	= model. rowCount ();
	for (int i = 0; i < row; i ++) {
	   model. setData (model. index (i, 0),
	              QFont ("Cantarell", 11), Qt::FontRole);
	}
	ensembleDisplay	-> setModel (&model);
}

QString hextoString (int v) {
char t [4];
QString res;
int	i;
	for (i = 0; i < 4; i ++) {
	   t [3 - i] = v & 0xF;
	   v >>= 4;
	}
	for (i = 0; i < 4; i ++) {
	   QChar c = t [i] <= 9 ? (char) ('0' + t [i]) : (char)('A'+ t [i] - 10);
	   res. append (c);
	}
	return res;
}
//
//	a slot, called by the fib processor
void	RadioInterface::nameofEnsemble (int id, const QString &v) {
QString s;
	if (!running. load())
	   return;

	ensembleId	-> setText (v + QString (": ")+ hextoString (id));
	my_dabProcessor	-> coarseCorrectorOff();
	Yes_Signal_Found();
}

///////////////////////////////////////////////////////////////////////
/**
  *	\brief show_successRate
  *	a slot, called by the MSC handler to show the
  *	percentage of frames that could be handled
  */
void	RadioInterface::show_frameErrors (int s) {
	if (running. load())
	   techData. frameError_display	-> setValue (100 - 4 * s);
}

void	RadioInterface::show_rsErrors (int s) {
	if (running. load())
	   techData. rsError_display	-> setValue (100 - 4 * s);
}
	
void	RadioInterface::show_aacErrors (int s) {
	if (running. load())
	   techData. aacError_display	-> setValue (100 - 4 * s);
}

void	RadioInterface::show_ficSuccess (bool b) {
	if (!running. load())	
	   return;
	if (b) {
	   ficSuccess ++;
	}

	if (++ficBlocks >= 100) {
	   techData. ficError_display	-> setValue (ficSuccess);
	   ficSuccess	= 0;
	   ficBlocks	= 0;
	}
}

void	RadioInterface::show_motHandling (bool b) {
	if (!running. load())
	   return;
	if (b) {
	   techData. motAvailable -> 
	               setStyleSheet ("QLabel {background-color : green; color: white}");
	}
	else {
	   techData. motAvailable ->
	               setStyleSheet ("QLabel {background-color : red; color : white}");
	}
}
	
///	called from the ofdmDecoder, which computed this for each frame
void	RadioInterface::show_snr (int s) {
	if (running. load())
	   snrDisplay	-> display (s);
}

///	just switch a color, obviously GUI dependent, but called
//	from the ofdmprocessor
void	RadioInterface::setSynced	(char b) {
	if (!running. load())
	   return;
	if (isSynced == b)
	   return;

	isSynced = b;
	switch (isSynced) {
	   case SYNCED:
	      syncedLabel -> 
	               setStyleSheet ("QLabel {background-color : green; color: white}");
	      break;

	   default:
	      syncedLabel ->
	               setStyleSheet ("QLabel {background-color : red}");
	      break;
	}
}

//	showLabel is triggered by the message handler
//	the GUI may decide to ignore this
void	RadioInterface::showLabel	(QString s) {
	if (running. load())
	   dynamicLabel	-> setText (s);
}

void	RadioInterface::setStereo	(bool s) {
	if (!running. load())
	   return;
	if (s) { 
	   stereoLabel -> 
	               setStyleSheet ("QLabel {background-color : green; color: white}");
	   stereoLabel -> setText ("stereo");
	}
	else {
	   stereoLabel ->
	               setStyleSheet ("QLabel {background-color : red}");
	   stereoLabel -> setText ("");
	}
}
//
//////////////////////////////////////////////////////////////////////////
//
void	checkDir (QString &s) {
int16_t	ind	= s. lastIndexOf (QChar ('/'));
int16_t	i;
QString	dir;
	if (ind == -1)		// no slash, no directory
	   return;

	for (i = 0; i < ind; i ++)
	   dir. append (s [i]);

	if (QDir (dir). exists())
	   return;
	QDir(). mkpath (dir);
}
//	showMOT is triggered by the MOT handler,
//	the GUI may decide to ignore the data sent
//	since data is only sent whenever a data channel is selected
void	RadioInterface::showMOT		(QByteArray data,
	                                 int subtype, QString pictureName) {
const char *type;
	if (!running. load())
	   return;

	type = subtype == 0 ? "GIF" :
	       subtype == 1 ? "JPG" :
//	       subtype == 1 ? "JPEG" :
	       subtype == 2 ? "BMP" : "PNG";

	QPixmap p;
	p. loadFromData (data, type);
	if (saveSlides && (pictureName != QString (""))) {
	   pictureName	= QDir::toNativeSeparators (pictureName);
	   FILE *x = fopen (pictureName. toLatin1(). data(), "w+b");
	   if (x == nullptr)
	      fprintf (stderr, "cannot write file %s\n",
	                            pictureName. toLatin1(). data());
	   else {
	      fprintf (stderr, "going to write file %s\n",
	                            pictureName. toLatin1(). data());
	      (void)fwrite (data. data(), 1, data.length(), x);
	      fclose (x);
	   }
	}

	int w	= techData. pictureLabel -> width ();
	int h	= 2 * w / 3;
	techData. pictureLabel ->
	          setPixmap (p. scaled (w, h, Qt::KeepAspectRatio));
	techData. pictureLabel -> show ();
}
//
//	sendDatagram is triggered by the ip handler,
//
void	RadioInterface::sendDatagram	(int length) {
uint8_t localBuffer [length];
	if (dataBuffer -> GetRingBufferReadAvailable() < length) {
	   fprintf (stderr, "Something went wrong\n");
	   return;
	}
	dataBuffer -> getDataFromBuffer (localBuffer, length);
#ifdef	_SEND_DATAGRAM_
	if (running. load()) {
	   dataOut_socket. writeDatagram ((const char *)localBuffer, length,
	                                   QHostAddress (ipAddress),
	                                   port);
	}
	
#endif
}
//
//
void	RadioInterface::handle_tdcdata (int frametype, int length) {
#ifdef DATA_STREAMER
uint8_t localBuffer [length + 8];
#endif
	(void)frametype;
	if (!running. load())
	   return;
	if (dataBuffer -> GetRingBufferReadAvailable() < length) {
	   fprintf (stderr, "Something went wrong\n");
	   return;
	}
#ifdef	DATA_STREAMER
	dataBuffer -> getDataFromBuffer (&localBuffer [8], length);
	localBuffer [0] = 0xFF;
	localBuffer [1] = 0x00;
	localBuffer [2] = 0xFF;
	localBuffer [3] = 0x00;
	localBuffer [4] = (length & 0xFF) >> 8;
	localBuffer [5] = length & 0xFF;
	localBuffer [6] = 0x00;
	localBuffer [7] = frametype == 0 ? 0 : 0xFF;
	if (running. load())
	   dataStreamer -> sendData (localBuffer, length + 8);
#endif
}

/**
  *	\brief changeinConfiguration
  *	No idea yet what to do, so just give up
  *	with what we were doing. The user will -eventually -
  *	see the new configuration from which he can select
  */
void	RadioInterface::changeinConfiguration() {
	if (running. load()) {
	   soundOut		-> stop();
	   inputDevice		-> stopReader();
	   inputDevice		-> resetBuffer();
	   my_dabProcessor	-> reset();
	   clear_showElements();
	}
}
//
//	In order to not overload with an enormous amount of
//	signals, we trigger this function at most 10 times a second
//
void	RadioInterface::newAudio	(int amount, int rate) {
	if (running. load()) {
	   int16_t vec [amount];
	   while (audioBuffer -> GetRingBufferReadAvailable() > amount) {
	      audioBuffer -> getDataFromBuffer (vec, amount);
	      soundOut	-> audioOut (vec, amount, rate);
	   }
	}
}
//
//	This function is only used in the Gui to clear
//	the details of a selection
void	RadioInterface::clear_showElements() {
//	if (!running. load())
//	   return;
	Services		= QStringList();
	model.	clear ();

	my_dabProcessor		-> clearEnsemble();
	ensembleId		-> setText ("");
	dynamicLabel		-> setText ("");
	serviceLabel		-> setText ("");
	
//	Then the various displayed items
	techData. frameError_display	-> setValue (0);
	techData. rsError_display	-> setValue (0);
	techData. aacError_display	-> setValue (0);
	techData. ficError_display	-> setValue (0);
	techData. programName		-> setText (QString (""));
	techData. bitrateDisplay	-> display (0);
	techData. startAddressDisplay	-> display (0);
	techData. lengthDisplay		-> display (0);
	techData. subChIdDisplay	-> display (0);
//	techData. protectionlevelDisplay -> display (0);
	techData. uepField		-> setText (QString (""));
	techData. ASCTy			-> setText (QString (""));
	techData. language		-> setText (QString (""));
	techData. programType		-> setText (QString (""));
	techData. motAvailable		-> 
	               setStyleSheet ("QLabel {background-color : red}");
	transmitter_coordinates	-> setAlignment (Qt::AlignRight);
	transmitter_coordinates -> setText ("");
	my_tiiViewer	-> clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	

/**
  *	\brief TerminateProcess
  *	Pretty critical, since there are many threads involved
  *	A clean termination is what is needed, regardless of the GUI
  */
void	RadioInterface::TerminateProcess() {
	running. store (false);
#ifdef	DATA_STREAMER
	fprintf (stderr, "going to close the dataStreamer\n");
	delete		dataStreamer;
#endif
	displayTimer. stop();
	signalTimer.  stop();
	presetTimer.  stop();

	my_presetHandler. savePresets (presetSelector);
	if (audioDumper != nullptr) {
	   soundOut	-> stopDumping();
	   sf_close (audioDumper);
	}

	dumpControlState (dabSettings);
	if (inputDevice != nullptr) 
	   inputDevice		-> stopReader();	// might be concurrent
	if (my_dabProcessor != nullptr)
	   my_dabProcessor	-> stop();		// definitely concurrent

	if (currentService != nullptr)
	   delete currentService;
	currentService	= nullptr;
	soundOut		-> stop();
	dataDisplay		->  hide();
//	everything should be halted by now
	delete		soundOut;
	if (inputDevice != nullptr)
	   delete		inputDevice;
	fprintf (stderr, "going to delete dabProcessor\n");
	if (my_dabProcessor != nullptr)
	   delete		my_dabProcessor;
	fprintf (stderr, "deleted dabProcessor\n");
	delete	dataDisplay;
	delete	my_spectrumViewer;
	delete	my_tiiViewer;
	delete	my_correlationViewer;
	delete	iqBuffer;
	delete	spectrumBuffer;
	delete	responseBuffer;
	delete	tiiBuffer;
//	close();
	fprintf (stderr, ".. end the radio silences\n");
}

//
/**
  *	\brief selectChannel
  *	Depending on the GUI the user might select a channel
  *	or some magic will cause a channel to be selected
  */
void	RadioInterface::selectChannel_usingSelector (QString s) {
	presetSelector	-> setCurrentIndex (0);
	presetTimer. stop ();
	disconnect (&presetTimer, SIGNAL (timeout ()),
	            this, SLOT (setPresetStation (void)));
	serviceLabel	-> setText ("");
	selectChannel (s);
}

void	RadioInterface::selectChannel (QString s) {
int32_t	tunedFrequency;
bool	localRunning	= running. load();
	if (currentService != nullptr)
	   delete currentService;
	currentService	= nullptr;
	stopServices ();	// or reset_msc?
	setStereo (false);
	if (tiiSwitch)
	   set_tiiSwitch ();	// switch it off
	if (localRunning) {
	   soundOut		-> stop();
	   inputDevice		-> stopReader();
	}

	clear_showElements	();
	secondariesVector. resize (0);
	tunedFrequency	= theBand. Frequency (s);
	inputDevice	-> setVFOFrequency (tunedFrequency);
	frequencyDisplay	-> display (tunedFrequency / 1000000.0);

	if (localRunning) {
	   if (!inputDevice	-> restartReader()) {
	      QMessageBox::warning (this, tr ("Warning"),
	                           tr ("Device will not restart\n"));
	      return;
	   }
	   my_dabProcessor	-> reset();
	   running. store (true);
	}
	dabSettings	-> setValue ("channel", s);
}

static size_t previous_idle_time	= 0;
static size_t previous_total_time	= 0;

void	RadioInterface::updateTimeDisplay() {
	if (!running. load())
	   return;

	numberofSeconds ++;
	int16_t	numberHours	= numberofSeconds / 3600;
	int16_t	numberMinutes	= (numberofSeconds / 60) % 60;
	QString text = QString ("runs ");
	text. append (QString::number (numberHours));
	text. append (" hr, ");
	text. append (QString::number (numberMinutes));
	text. append (" min");
	timeDisplay	-> setText (text);
#ifndef	__MINGW32__
	if ((numberofSeconds % 2) == 0) {
	   size_t idle_time, total_time;
	   get_cpu_times (idle_time, total_time);
	   const float idle_time_delta = idle_time - previous_idle_time;
	   const float total_time_delta = total_time - previous_total_time;
	   const float utilization = 100.0 * (1.0 - idle_time_delta / total_time_delta);
	   cpuMonitor -> display (utilization);
	   previous_idle_time = idle_time;
	   previous_total_time = total_time;
	}
#endif
#ifdef	SHOW_MISSING
	if ((numberofSeconds % 10) == 0) {
	   int xxx = ((audioSink *)soundOut)	-> missed();
	   fprintf (stderr, "missed %d\n", xxx);
	}
#endif
}

void	RadioInterface::autoCorrector_on() {
//	first the real stuff
	if (!running. load())
	   return;

	clear_showElements();
	my_dabProcessor		-> clearEnsemble();
	my_dabProcessor		-> coarseCorrectorOn();
	my_dabProcessor		-> reset();
}

/**
  *	\brief setDevice
  *	setDevice is called during the start up phase
  *	of the dab program, and from within newDevice,
  *	that would/should have taken care of proper
  *	stopping service handling
  */
virtualInput	*RadioInterface::setDevice (QString s) {
QString	file;
virtualInput	*inputDevice	= nullptr;
///	OK, everything quiet, now let us see what to do
#ifdef	HAVE_AIRSPY
	if (s == "airspy") {
	   try {
	      inputDevice	= new airspyHandler (dabSettings);
	      showButtons();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                           tr ("Airspy or Airspy mini not found\n"));
	      return nullptr;
	   }
	}
	else
#endif
#ifdef	HAVE_HACKRF
	if (s == "hackrf") {
	   try {
	      inputDevice	= new hackrfHandler (dabSettings);
	      showButtons();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                           tr ("hackrf not found\n"));
	      return nullptr;
	   }
	}
	else
#endif
#ifdef	HAVE_SOAPY
	if (s == "soapy") {
	   try {
	      inputDevice	= new soapyHandler (dabSettings);
	      showButtons();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                                  tr ("no soapy device found\n"));
	      return nullptr;
	   }
	}
	else
#endif
#ifdef	HAVE_LIME
	if (s == "limeSDR") {
	   try {
	      inputDevice = new limeHandler (dabSettings);
	      showButtons();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                                  tr ("no lime device found\n"));
	      return nullptr;
	   }
	}
	else
#endif
#ifdef HAVE_EXTIO
//	extio is - in its current settings - for Windows, it is a
//	wrap around the dll
	if (s == "extio") {
	   try {
	      inputDevice = new extioHandler (dabSettings);
	      showButtons();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                            tr ("extio: no luck\n") );
	      return nullptr;
	   }
	}
	else
#endif
#ifdef HAVE_RTL_TCP
//	RTL_TCP might be working. 
	if (s == "rtl_tcp") {
	   try {
	      inputDevice = new rtl_tcp_client (dabSettings);
	      showButtons();
	      presetSelector -> hide ();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                           tr ("rtl_tcp: no luck\n") );
	      return nullptr;
	   }
	}
	else
#endif
#ifdef	HAVE_SDRPLAY
	if (s == "sdrplay") {
	   try {
	      inputDevice	= new sdrplayHandler (dabSettings);
	      showButtons();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                               tr ("SDRplay: no library or device\n"));
	      return nullptr;
	   }
	}
	else
#endif
#ifdef	HAVE_ELAD_S1
	if (s == "elad-s1") {
	   try {
	      inputDevice	= new eladHandler (dabSettings);
	      showButtons();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                               tr ("elad-s1: no library or device\n"));
	      return nullptr;
	   }
	}
	else
#endif
#ifdef	HAVE_RTLSDR
	if (s == "dabstick") {
	   try {
	      inputDevice	= new rtlsdrHandler (dabSettings);
	      showButtons();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                           tr ("DAB stick not found! Please use one with RTL2832U or similar chipset!\n"));
	      fprintf (stderr, "error = %d\n", e);
	      return nullptr;
	   }
	}
	else
#endif
	if (s == "file input (.iq)") {
	   file		= QFileDialog::getOpenFileName (this,
	                                                tr ("Open file ..."),
	                                                QDir::homePath(),
	                                                tr ("iq data (*.iq)"));
	   if (file == QString (""))
	      return nullptr;
	   file		= QDir::toNativeSeparators (file);
	   try {
	      inputDevice	= new rawFiles (file);
	      hideButtons();
	      presetName	= "";
	      presetSelector -> hide ();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                               tr ("file not found"));
	      return nullptr;
	   }
	}
	else
	if (s == "file input (.raw)") {
	   file		= QFileDialog::getOpenFileName (this,
	                                                tr ("Open file ..."),
	                                                QDir::homePath(),
	                                                tr ("raw data (*.raw)"));
//	      return nullptr;
	   file		= QDir::toNativeSeparators (file);
	   try {
	      inputDevice	= new rawFiles (file);
	      hideButtons();
	      presetName	= "";
	      presetSelector -> hide ();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                               tr ("file not found"));
	      return nullptr;
	   }
	}
	else
	if (s == "file input (.sdr)") {
	   file		= QFileDialog::getOpenFileName (this,
	                                                tr ("Open file ..."),
	                                                QDir::homePath(),
	                                                tr ("raw data (*.sdr)"));
	   if (file == QString (""))
	      return nullptr;

	   file		= QDir::toNativeSeparators (file);
	   try {
	      inputDevice	= new wavFiles (file);
	      hideButtons();	
	      presetName	= "";
	      presetSelector -> hide ();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                               tr ("file not found"));
	      return nullptr;
	   }
	}
	else {
	   fprintf (stderr, "unknown device, failing\n");
	   return nullptr;
	}

	my_spectrumViewer	-> setBitDepth (inputDevice -> bitDepth());
	if (showDeviceWidget -> text () == "hide") {
	   inputDevice	-> show ();
	}
	else {
	   inputDevice -> hide ();
	}
	return inputDevice;
}
//
//	newDevice is called from the GUI when selecting a device
void	RadioInterface::newDevice (QString deviceName) {

//	if (!running. load())		// should not happen
//	   return;

	disconnectGUI();
	presetTimer. stop();
//	connect (&presetTimer, SIGNAL (timeout (void)),
//	         this, SLOT (setPresetStation (void)));
	running. store (false);
	inputDevice	-> stopReader();

	scanning. store (false);
	tiiSwitch		= false;
	isSynced		= UNSYNCED;
	if (my_dabProcessor == nullptr)
	   fprintf (stderr, "SCREAM\n");
	else {
	   my_dabProcessor	-> stop();
	   delete my_dabProcessor;
	}
	delete inputDevice;
	my_dabProcessor		= nullptr;
	inputDevice		= nullptr;
	inputDevice		= setDevice (deviceName);
	if (inputDevice == nullptr) {
	   inputDevice = new virtualInput();
//	Note that the device handler itself will do the complaining
	   return;
	}
	doStart();		// will set running
}
//
//	aiming at a future where we will be selective in
//	stopping services
void    RadioInterface::stopServices () {
        while (activeServices. size () > 0) {
           int top = activeServices. size () - 1;
           const QString s = activeServices. at (top);
           my_dabProcessor -> unset_Channel (s);
//         fprintf (stderr, "removing %s\n", s. toLatin1 (). data ());
           activeServices. pop_back ();
        }
//      my_dabProcessor -> reset_msc();
}

//
//      a service might be called from the GUI as well as from an
//	internal call, in the latter case we first compute the index
void    RadioInterface::selectService (QString s) {
int row = model. rowCount ();
	for (int i = 0; i < row; i ++) {
	   QString itemText =
	          model. index (i, 0). data (Qt::DisplayRole). toString ();
	   if (itemText == s) {
	      selectService (model. index (i, 0));
	      return;
	   }
	}
}

void	RadioInterface::selectService (QModelIndex ind) {
QString serviceName;
	presetTimer. stop();
	disconnect (&presetTimer, SIGNAL (timeout (void)),
	            this, SLOT (setPresetStation (void)));
	presetSelector -> setCurrentIndex (0);

//	first ensure that the text of the elements is black
	int row	= model. rowCount ();
	for (int i = 0; i < row; i ++) {
	   QMap <int, QVariant> vMap = model. itemData (model. index (i, 0));
	   vMap. insert (Qt::ForegroundRole, QVariant (QBrush (Qt::black)));
	   model. setItemData (model . index (i, 0), vMap);
	   model. setData (model. index (i, 0),
	                     QFont ("Cantarell", 11), Qt::FontRole);
	}

//	then highlight the selected item
	serviceName = ind. data (Qt::DisplayRole). toString ();
	QMap<int, QVariant> vMap = model. itemData (ind);
	vMap. insert (Qt::ForegroundRole, QVariant (QBrush (Qt::red)));
	model. setItemData (ind, vMap);
	model. setData (ind, QFont ("Cantarell", 13), Qt::FontRole);

//	here it really starts
	serviceLabel -> setText (" ");
	if (!my_dabProcessor -> is_audioService (serviceName) &&
	    !my_dabProcessor -> is_packetService (serviceName))
	   return;

	serviceLabel -> setStyleSheet ("QLabel {color : black}");
	serviceLabel	-> setText (serviceName);
	stopServices ();
	setStereo (false);

	techData. rsError_display	-> hide();
	techData. aacError_display	-> hide();
	techData. motAvailable		-> 
	               setStyleSheet ("QLabel {background-color : red}");

	if (my_dabProcessor -> is_audioService (serviceName)) {
	   audiodata d;
	   my_dabProcessor -> dataforAudioService (serviceName, &d, 0);
	   if (!d. defined) {
	      QMessageBox::warning (this, tr ("Warning"),
 	                            tr ("unknown bitrate for this program\n"));
 	      return;
 	    }

	   activeServices. push_back (serviceName);
	   show_techData (serviceName,
	                   inputDevice -> getVFOFrequency() / 1000000.0,
	                   &d);
	    my_dabProcessor -> set_audioChannel (&d, audioBuffer);
	    for (int i = 1; i < 10; i ++) {
	       packetdata pd;
	       my_dabProcessor -> dataforPacketService (serviceName, &pd, i);
	       if (pd. defined) {
	          QString s = QString::number (i) + "%%" + serviceName;
	          pd. serviceName = s;
	          my_dabProcessor -> set_dataChannel (&pd, dataBuffer);
	          activeServices. push_back (s);
	          break;
	       }
	    }

	    soundOut	-> restart();
	    showLabel (QString (" "));
	}
	else
	if (my_dabProcessor -> is_packetService (serviceName)) {
	   packetdata pd;
	   my_dabProcessor -> dataforPacketService (serviceName, &pd, 0);
	   if ((!pd. defined) ||
	            (pd.  DSCTy == 0) || (pd. bitRate == 0)) {
	      fprintf (stderr, "d. DSCTy = %d, d. bitRate = %d\n",
	                               pd. DSCTy, pd. bitRate);
	      QMessageBox::warning (this, tr ("sdr"),
 	                            tr ("still insufficient data for this service\n"));
	      return;
	   }

	   activeServices. push_back (serviceName);
	   my_dabProcessor -> set_dataChannel (&pd, dataBuffer);
	   switch (pd. DSCTy) {
	      default:
	         showLabel (QString ("unimplemented Data"));
	         break;
	      case 5:
	         fprintf (stderr, "selected apptype %d\n", pd. appType);
	         showLabel (QString ("Transp. Channel not implemented"));
	         break;
	      case 60:
	         showLabel (QString ("MOT partially implemented"));
	         break;
	      case 59: {
#ifdef	_SEND_DATAGRAM_
	         QString text = QString ("Embedded IP: UDP data to ");
	         text. append (ipAddress);
	         text. append (" ");
	         QString n = QString::number (port);
	         text. append (n);
	         showLabel (text);
#else
	         showLabel ("Embedded IP not supported ");
#endif
	         }
	         break;
	      case 44:
	         showLabel (QString ("Journaline"));
	         break;
	   }
	}
	else
	   return;		// should not happen

	techData. pictureLabel -> setText ("");
}
//

//	signal, received from ofdm_decoder that a buffer is filled
//	with amount values ready for display
void	RadioInterface::showIQ	(int amount) {
	if (running. load())
	   my_spectrumViewer	-> showIQ (amount);
}

void	RadioInterface::showSpectrum	(int32_t amount) {
	if (running. load())
	   my_spectrumViewer -> showSpectrum (amount,
				            inputDevice -> getVFOFrequency());
}

void	RadioInterface::showQuality	(float q) {
	if (running. load())
	   my_spectrumViewer	-> showQuality (q);
}

void	RadioInterface::showCorrelation (int amount) {
	if (running. load())
	   my_correlationViewer -> showCorrelation (amount);
}

void	RadioInterface::showIndex (int ind) {
	if (!running. load())
	   return;

	my_correlationViewer -> showIndex (ind);
}

//
void	RadioInterface::show_tii (int amount) {
	if (!running. load())
	   return;
	my_tiiViewer	-> showSpectrum (amount);
	my_tiiViewer	-> showSecondaries (secondariesVector);
}
//
void	RadioInterface:: set_streamSelector (int k) {
#if	not defined (TCP_STREAMER) &&  not defined (QT_AUDIO)
	((audioSink *)(soundOut)) -> selectDevice (k);
#else
	(void)k;
#endif
}

void	RadioInterface::toggle_show_data() {
	if (dataDisplay -> isHidden())
	   dataDisplay -> show();
	else
	   dataDisplay -> hide();
}


void	RadioInterface::showButtons() {
//	if (!running. load())
//	   return;
	connect (scanButton, SIGNAL (clicked (void)),
                 this, SLOT (set_Scanning (void)));
	scanButton	-> setText ("scan");
	connect (channelSelector, SIGNAL (activated (const QString &)),
                 this, SLOT (selectChannel_usingSelector (const QString &)));
	channelSelector	-> show ();
	connect (dumpButton, SIGNAL (clicked (void)),
                 this, SLOT (set_sourceDump (void)));
	dumpButton	-> setText ("raw dump");
	connect (nextChannelButton, SIGNAL (clicked ()),
                 this, SLOT (set_nextChannel ()));
	nextChannelButton	-> setText ("+");
        connect (prevChannelButton, SIGNAL (clicked ()),
                 this, SLOT (set_prevChannel ()));
	prevChannelButton	-> setText ("-");
	presetSelector	-> show ();
}

void	RadioInterface::hideButtons() {
//	if (!running. load())
//	   return;
	disconnect (scanButton, SIGNAL (clicked (void)),
                    this, SLOT (set_Scanning (void)));
	scanButton	-> setText ("X");
	channelSelector	-> hide ();
	disconnect (channelSelector, SIGNAL (activated (const QString &)),
                    this, SLOT (selectChannel_usingSelector (const QString &)));
	disconnect (dumpButton, SIGNAL (clicked (void)),
                    this, SLOT (set_sourceDump (void)));
	dumpButton	-> setText ("X");
	disconnect (nextChannelButton, SIGNAL (clicked ()),
                    this, SLOT (set_nextChannel ()));
	nextChannelButton	-> setText ("X");
        disconnect (prevChannelButton, SIGNAL (clicked ()),
                    this, SLOT (set_prevChannel ()));
	prevChannelButton	-> setText ("X");
	presetSelector	-> hide ();
}

void	RadioInterface::setSyncLost() {
}

void	RadioInterface::showCoordinates (int data) {
int	mainId	= data >> 8;
int	subId	= data & 0xFF;
QString a = "Estimate: ";
QString b = "  ";
QString c = "  ";

	if (!running. load())
	   return;

	a	.append (QString::number (mainId));
	b	.append (QString::number (subId));
	a. append (b);
	transmitter_coordinates	-> setAlignment (Qt::AlignRight);
	transmitter_coordinates -> setText (a);
}

void	RadioInterface::showSecondaries (int data) {
	if (!running. load())
	   return;

	if (data == -1)
	   secondariesVector. resize (0);
	else
	   secondariesVector. push_back (data);
}
	
void	RadioInterface::showEnsembleData() {
QString currentChannel	= channelSelector -> currentText();
int32_t	frequency	= inputDevice -> getVFOFrequency();
ensemblePrinter	my_Printer;

	if (!running. load() || (ensembleId -> text () == QString ("")))
	   return;

	QString theTime		= localTimeDisplay -> text ();
	theTime. replace (":", "-");
	QString suggestedFileName = QDir::homePath ();
	suggestedFileName. append ("/Qt-DAB-");
	suggestedFileName. append (theTime);
	suggestedFileName. append (".txt");
	fprintf (stderr, "suggested filename = %s\n",
	          suggestedFileName. toLatin1 (). data ());
	QString fileName = QFileDialog::getSaveFileName (this,
	                                        tr ("Save file ..."),
	                                        suggestedFileName,
//	                                        QDir::homePath(),
	                                        tr ("Text (*.txt)"));
	fileName	= QDir::toNativeSeparators (fileName);
	FILE *file_P	= fopen (fileName. toUtf8(). data(), "w");

	if (file_P == nullptr) {
	   fprintf (stderr, "Could not open file %s\n",
	                              fileName. toUtf8(). data());
	   return;
	}
	my_Printer. showEnsembleData (currentChannel, frequency, theTime,
	                              Services, my_dabProcessor, file_P);

	fclose (file_P);
}

void	RadioInterface::setPresetStation() {
	disconnect (&presetTimer, SIGNAL (timeout (void)),
	            this, SLOT (setPresetStation (void)));
	if (ensembleId -> text () == QString ("")) {
	   QMessageBox::warning (this, tr ("Warning"),
                                  tr ("Oops, ensemble not yet recognized\nselect service manually\n"));
	   return;
	}

	if (presetName == QString (""))		// should not happen
	   return;

	for (const auto& service: Services)
	   if (service. contains (presetName)) {
	      fprintf (stderr, "going to select %s\n", presetName. toLatin1 (). data ());
	      selectService (presetName);
	      return;
	   }
	fprintf (stderr, "presetName %s not found\n", presetName. toLatin1 (). data ());
}

void	RadioInterface::set_picturePath() {
QString defaultPath	= QDir::tempPath();

	if (defaultPath. endsWith ("/"))
	   defaultPath. append ("qt-pictures/");
	else
	   defaultPath. append ("/qt-pictures/");

	picturesPath	=
	        dabSettings	-> value ("pictures", defaultPath). toString();

	if ((picturesPath != "") && (!picturesPath. endsWith ("/")))
	   picturesPath. append ("/");
	QDir testdir (picturesPath);

	if (!testdir. exists())
	   testdir. mkdir (picturesPath);
}

void	RadioInterface::set_nextChannel() {
	disconnect (nextChannelButton, SIGNAL (clicked ()),
	            this, SLOT (set_nextChannel ()));
	if (scanning. load ())
	   stopScanning ();
	my_dabProcessor -> stop();
	while (!my_dabProcessor -> isFinished())
	   usleep (10000);
	increment_Channel();
	clearEnsemble();
	my_dabProcessor	-> start();
	connect (nextChannelButton, SIGNAL (clicked ()),
	         this, SLOT (set_nextChannel ()));
}

void	RadioInterface::set_prevChannel () {
	disconnect (prevChannelButton, SIGNAL (clicked ()),
	            this, SLOT (set_prevChannel ()));
	if (scanning. load ())
	   stopScanning ();
	my_dabProcessor -> stop();
	while (!my_dabProcessor -> isFinished())
	   usleep (10000);
	decrement_Channel();
	clearEnsemble();
	my_dabProcessor	-> start ();
	connect (prevChannelButton, SIGNAL (clicked ()),
	         this, SLOT (set_prevChannel ()));
}

void	RadioInterface::set_tiiSwitch() {
	if (my_tiiViewer -> isHidden())
	   my_tiiViewer -> show();
	else
	   my_tiiViewer -> hide();
}

void	RadioInterface::set_correlationSwitch() {
	if (my_correlationViewer -> isHidden())
	   my_correlationViewer	-> show();
	else
	   my_correlationViewer	-> hide();
}

void	RadioInterface::set_spectrumSwitch() {
	if (my_spectrumViewer -> isHidden())
	   my_spectrumViewer -> show();
	else
	   my_spectrumViewer -> hide();
}

//
//	When changing (or setting) a device, we do not want anybody
//	to have the buttons on the GUI touched, so
//	we just disconnet them and (re)connect them as soon as
//	a device is operational
void	RadioInterface::connectGUI() {
	connect (showProgramData, SIGNAL (clicked (void)),
	         this, SLOT (toggle_show_data (void)));
	connect (previousService, SIGNAL (clicked ()),
                 this, SLOT (handle_setprevious ()));
        connect (nextService, SIGNAL (clicked ()),
                 this, SLOT (handle_setnext ()));
	connect (showDeviceWidget, SIGNAL (clicked ()),
                 this, SLOT (handle_showDeviceWidget ()));
	connect (saveEnsembleData, SIGNAL (clicked (void)),
	         this, SLOT (showEnsembleData (void)));
	connect (ensembleDisplay, SIGNAL (clicked (QModelIndex)),
	         this, SLOT (selectService (QModelIndex)));
	connect (resetButton, SIGNAL (clicked (void)),
	         this, SLOT (autoCorrector_on (void)));
	connect	(scanButton, SIGNAL (clicked (void)),
	         this, SLOT (set_Scanning (void)));
	connect (nextChannelButton, SIGNAL (clicked (void)),
	         this, SLOT (set_nextChannel (void)));
	connect (prevChannelButton, SIGNAL (clicked (void)),
	         this, SLOT (set_prevChannel (void)));
	connect (dumpButton, SIGNAL (clicked (void)),
	         this, SLOT (set_sourceDump (void)));
	connect (audioDumpButton, SIGNAL (clicked (void)),
	         this, SLOT (set_audioDump (void)));
	connect (frameDumpButton, SIGNAL (clicked (void)),
	         this, SLOT (set_frameDump (void)));
	connect (tiiButton, SIGNAL (clicked (void)),
	         this, SLOT (set_tiiSwitch (void)));
	connect (show_correlationButton, SIGNAL (clicked (void)),
	         this, SLOT (set_correlationSwitch (void)));
	connect (show_spectrumButton, SIGNAL (clicked (void)),
	         this, SLOT (set_spectrumSwitch (void)));
}

void	RadioInterface::disconnectGUI() {
	   disconnect (showProgramData, SIGNAL (clicked (void)),
	               this, SLOT (toggle_show_data (void)));
	   disconnect (previousService, SIGNAL (clicked ()),
	               this, SLOT (handle_setprevious ()));
	   disconnect (nextService, SIGNAL (clicked ()),
	               this, SLOT (handle_setnext ()));
	   disconnect (saveEnsembleData, SIGNAL (clicked (void)),
	               this, SLOT (showEnsembleData (void)));
	   disconnect (ensembleDisplay, SIGNAL (clicked (QModelIndex)),
	               this, SLOT (selectService (QModelIndex)));
	   disconnect (resetButton, SIGNAL (clicked (void)),
	               this, SLOT (autoCorrector_on (void)));
	   disconnect (scanButton, SIGNAL (clicked (void)),
	               this, SLOT (set_Scanning (void)));
	   disconnect (nextChannelButton, SIGNAL (clicked (void)),
	               this, SLOT (set_nextChannel (void)));
	   disconnect (dumpButton, SIGNAL (clicked (void)),
	               this, SLOT (set_sourceDump (void)));
	   disconnect (frameDumpButton, SIGNAL (clicked (void)),
	               this, SLOT (set_frameDump (void)));
	   disconnect (audioDumpButton, SIGNAL (clicked (void)),
	               this, SLOT (set_audioDump (void)));
	   disconnect (tiiButton, SIGNAL (clicked (void)),
	               this, SLOT (set_tiiSwitch (void)));
	   disconnect (show_correlationButton, SIGNAL (clicked (void)),
	               this, SLOT (set_correlationSwitch (void)));
}

void	RadioInterface::show_techData (QString 		serviceName, 
	                               float 		Frequency,
	                               audiodata	*d) {
	techData. programName	-> setText (serviceName);
	techData. bitrateDisplay -> display (d -> bitRate);
	techData. startAddressDisplay -> display (d -> startAddr);
	techData. lengthDisplay	-> display (d -> length);
	techData. subChIdDisplay -> display (d -> subchId);
	QString protL	= getProtectionLevel (d -> shortForm,
	                                      d -> protLevel);
	techData. uepField	-> setText (protL);
	techData. ASCTy		-> setText (d -> ASCTy == 077 ? "DAB+" : "DAB");
	if (d -> ASCTy == 077) {
	   techData. rsError_display -> show();
	   techData. aacError_display -> show();
	}
	techData. language ->
	   setText (getLanguage (d -> language));
	techData. programType ->
	   setText (the_textMapper.
	               get_programm_type_string (d -> programType));
	if (d -> fmFrequency == -1) {
	   techData. fmFrequency -> hide();
	   techData. fmLabel	-> hide();
	}
	else {
	   techData. fmLabel -> show();
	   techData. fmFrequency -> show();
	   QString f = QString::number (d -> fmFrequency);
	   f. append (" Khz");
	   techData. fmFrequency -> setText (f);
	}
}

#include <QCloseEvent>
void RadioInterface::closeEvent (QCloseEvent *event) {

	QMessageBox::StandardButton resultButton =
	                QMessageBox::question (this, "dabRadio",
	                                       tr("Are you sure?\n"),
	                                       QMessageBox::No | QMessageBox::Yes,
	                                       QMessageBox::Yes);
	if (resultButton != QMessageBox::Yes) {
	   event -> ignore();
	} else {
	   TerminateProcess();
	   event -> accept();
	}
}

bool	RadioInterface::eventFilter (QObject *obj, QEvent *event) {
	if ((obj == this -> ensembleDisplay -> viewport()) &&
	    (event -> type() == QEvent::MouseButtonPress )) {
	   QMouseEvent *ev = static_cast<QMouseEvent *>(event);
	   if (ev -> buttons() & Qt::RightButton) {
	      audiodata ad;
	      packetdata pd;
	      QString serviceName =
	           this -> ensembleDisplay -> indexAt (ev -> pos()). data().toString();
	      my_dabProcessor -> dataforAudioService (serviceName, &ad, 0);
	      if (ad. defined && (serviceName == serviceLabel -> text ())) {
	         presetData pd;
	         pd. serviceName	= serviceName;
	         pd. channel		= channelSelector -> currentText ();
	         QString itemText	= pd. channel + ":" + pd. serviceName;
	         for (int i = 0; i < presetSelector -> count (); i ++)
	            if (presetSelector -> itemText (i) == itemText)
	               return true;
	         presetSelector -> addItem (itemText);
	         return true;
	      }
	      
	      if (ad. defined) {
	         if (currentService != nullptr) 
	            delete currentService;
	         currentService	= new audioDescriptor (&ad);
	         fprintf (stderr, "new audioDescriptor\n");
	         return true;
	      }

	      my_dabProcessor -> dataforPacketService (serviceName, &pd, 0);
	      if (pd. defined) {
	         if (currentService != nullptr)
	            delete currentService;
	         currentService	= new dataDescriptor (&pd);
	         return true;
	      }
	   }
	}
	return QWidget::eventFilter (obj, event);
}

void	RadioInterface::showTime	(const QString &s) {
	localTimeDisplay	-> setText (s);
}

void	RadioInterface::startAnnouncement (const QString &name, int subChId) {
//	fprintf (stderr, "announcement for %s\n", name. toLatin1 (). data ());
	if (name == serviceLabel -> text ()) {
	   serviceLabel ->
	              setStyleSheet ("QLabel {color : red}");
	}
}

void	RadioInterface::stopAnnouncement (const QString &name, int subChId) {
	(void)subChId;
	if (name == serviceLabel -> text ()) {
	   serviceLabel ->
	              setStyleSheet ("QLabel {color : black}");
	}
}

void	RadioInterface::select_presetService (QString  channel,
	                                      QString serviceName) {
presetData pd;
bool	res;
//
//	just to be certain that we are the only one
	presetTimer. stop ();
	disconnect (&presetTimer, SIGNAL (timeout (void)),
	            this, SLOT (setPresetStation (void)));

	if (channel == channelSelector -> currentText ()) {
	   emit selectService (serviceName);
	   return;
	}
//
//	The hard part is stopping the current service,
//	selecting a new channel,
//	waiting a while
//	trying to start the selected service
	int k           = channelSelector -> findText (channel);
	if (k != -1) {
	   channelSelector -> setCurrentIndex (k);
	}
	else {
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("Incorrect preset\n"));

	   return;
	}
	emit selectChannel (channel);
	serviceLabel	-> setText ("");
	dynamicLabel	-> setText ("");
	presetName	= serviceName;
	presetTimer. setSingleShot (true);
	connect (&presetTimer, SIGNAL (timeout (void)),
	            this, SLOT (setPresetStation (void)));
	presetTimer. start (switchTime);
}

void	RadioInterface::handle_presetSelector (QString s) {
	stopScanning ();
	QStringList list = s.split (":", QString::SkipEmptyParts);
	if (list. length () != 2)
	   return;
	QString channel = list. at (0);
	QString service	= list. at (1);
	select_presetService (channel, service);
}

static
void	colorButton (QPushButton *pb, QColor c, int p) {
QPalette pal = pb	-> palette ();
	pal. setColor (QPalette::Button, c);
	pb		-> setAutoFillBackground (true);
	pb		-> setPalette (pal);
	QFont font	= pb -> font ();
	font. setPointSize (p);
	pb		-> setFont (font);
}

void	RadioInterface::set_sourceDump () {
SF_INFO *sf_info        = (SF_INFO *)alloca (sizeof (SF_INFO));

	if (rawDumper != nullptr) {
	   my_dabProcessor	-> stopDumping();
	   sf_close (rawDumper);
	   rawDumper	= nullptr;
	   colorButton (dumpButton, Qt::white, 10);
	   dumpButton	-> setText ("Raw dump");
           dumpButton	-> update ();
	   return;
	}

	QString file = QFileDialog::getSaveFileName (this,
	                                     tr ("Save file ..."),
	                                     QDir::homePath(),
	                                     tr ("raw data (*.sdr)"));
	if (file == QString (""))       // apparently cancelled
	   return;
	file    = QDir::toNativeSeparators (file);
	if (!file.endsWith (".sdr", Qt::CaseInsensitive))
	   file.append (".sdr");
	sf_info -> samplerate   = INPUT_RATE;
	sf_info -> channels     = 2;
	sf_info -> format       = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	rawDumper = sf_open (file. toUtf8(). data(),
	                                   SFM_WRITE, sf_info);
	if (rawDumper == nullptr) {
	   qDebug() << "cannot open " << file. toUtf8(). data();
	   return;
	}

	colorButton (dumpButton, Qt::red, 12);
	dumpButton	-> setText ("writing");
        dumpButton	-> update ();
	my_dabProcessor -> startDumping (rawDumper);
}

void	RadioInterface::set_audioDump() {
SF_INFO	*sf_info	= (SF_INFO *)alloca (sizeof (SF_INFO));

	if (audioDumper != nullptr) {
	   soundOut	-> stopDumping();
	   sf_close (audioDumper);
	   audioDumper = nullptr;
	   colorButton (audioDumpButton, Qt::white, 10);
	   audioDumpButton	-> setText ("audio dump");
	   audioDumpButton	-> update ();
	   return;
	}

	QString file = QFileDialog::getSaveFileName (this,
	                                        tr ("Save file ..."),
	                                        QDir::homePath(),
	                                        tr ("PCM wave file (*.wav)"));
	if (file == QString (""))
	   return;
	file		= QDir::toNativeSeparators (file);
	if (!file.endsWith (".wav", Qt::CaseInsensitive))
	   file.append (".wav");
	sf_info		-> samplerate	= 48000;
	sf_info		-> channels	= 2;
	sf_info		-> format	= SF_FORMAT_WAV | SF_FORMAT_PCM_16;

	audioDumper	= sf_open (file. toUtf8(). data(),
	                                   SFM_WRITE, sf_info);
	if (audioDumper == nullptr) {
	   qDebug() << "Cannot open " << file. toUtf8(). data();
	   return;
	}

	colorButton (audioDumpButton, Qt::red, 12);
	audioDumpButton		-> setText ("WRITING");
	audioDumpButton		-> update ();
	soundOut		-> startDumping (audioDumper);
}

void	RadioInterface::set_frameDump () {
	if (frameDumper != nullptr) {
	   fclose (frameDumper);
	   colorButton (frameDumpButton, Qt::white, 10);
	   frameDumpButton	-> setText ("frame dump");
	   frameDumpButton	-> update ();
	   frameDumper	= nullptr;
	   return;
	}
        QString file = QFileDialog::getSaveFileName (this,
                                             tr ("Save file ..."),
                                             QDir::homePath(),
                                             tr ("aac data (*.aac)"));
        if (file == QString (""))       // apparently cancelled
           return;
        file    = QDir::toNativeSeparators (file);
	frameDumper = fopen (file. toLatin1 (). data (), "w+b");
        if (frameDumper == nullptr) {
	   QString s = QString ("cannot open ") + file;
	   QMessageBox::warning (this, tr ("Warning"),
                                       tr (s. toLatin1 (). data ()));
           return;
        }
	colorButton (frameDumpButton, Qt::red, 12);
        frameDumpButton		-> setText ("recording");
        frameDumpButton		-> update ();
}
//
//	newFrame is called from somewhere in the mp4processor,
//	the data is already put in the databuffer
void	RadioInterface::newFrame	(int amount) {
uint8_t	buffer [amount];

	frameBuffer	-> getDataFromBuffer (buffer, amount);
	if (frameDumper != nullptr)
	   fwrite (buffer, amount, 1, frameDumper);
}

////////////////////////////////////////////////////////////////////////////

void	RadioInterface::handle_setprevious	() {
	disconnect (previousService, SIGNAL (clicked ()),
	         this, SLOT (handle_setprevious ()));
	QString currentService = serviceLabel -> text ();
	if ((Services. length () != 0) &&
	          (serviceLabel -> text () != "")) {
	   for (int i = 0; i < Services. length (); i ++) {
	      if (Services. at (i) == currentService) {
	         i = i - 1;
	         if (i < 0) i =
	            Services. length () - 1;
	         selectService (Services. at (i));
	         break;
	      }
	   }
	}
	connect (previousService, SIGNAL (clicked ()),
	         this, SLOT (handle_setprevious ()));
}

void	RadioInterface::handle_setnext		() {
	connect (previousService, SIGNAL (clicked ()),
	         this, SLOT (handle_setprevious ()));
	QString currentService = serviceLabel -> text ();
	if ((Services. length () != 0) &&
	          (serviceLabel -> text () != "")) {
	   for (int i = 0; i < Services. length (); i ++) {
	      if (Services. at (i) == currentService) {
	         i = i + 1;
	         if (i >= Services. length ())
	            i = 0;
	         selectService (Services. at (i));
	         break;
	      }
	   }
	}
}

void    RadioInterface::handle_showDeviceWidget () {
        if (inputDevice == nullptr)
           return;
	disconnect (showDeviceWidget, SIGNAL (clicked ()),
	            this, SLOT (handle_showDeviceWidget ()));

        if (inputDevice -> isHidden ()) {
           inputDevice -> show ();
           showDeviceWidget -> setText ("hide");
	   dabSettings -> setValue ("showDeviceWidget", 1);
        }
        else {
           inputDevice -> hide ();
           showDeviceWidget -> setText ("show");
	   dabSettings -> setValue ("showDeviceWidget", 0);
	}
	connect (showDeviceWidget, SIGNAL (clicked ()),
	         this, SLOT (handle_showDeviceWidget ()));
}

