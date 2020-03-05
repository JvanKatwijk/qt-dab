#
/*
 *    Copyright (C) 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
#include	<QSettings>
#include	<QMessageBox>
#include	<QFileDialog>
#include	<QDebug>
#include	<QDateTime>
#include	<QFile>
#include	<QStringList>
#include	<QStringListModel>
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
#include	"xml-filereader.h"
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
#ifdef	HAVE_SDRPLAY_V2
#include	"sdrplay-handler-v2.h"
#endif
#ifdef	HAVE_SDRPLAY_V3
#include	"sdrplay-handler-v3.h"
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
#include	"history-handler.h"

#ifdef	__MINGW32__
#include <windows.h>

__int64 FileTimeToInt64 (FILETIME & ft) {
	ULARGE_INTEGER foo;

	foo.LowPart	= ft.dwLowDateTime;
	foo.HighPart	= ft.dwHighDateTime;
	return (foo.QuadPart);
}

bool get_cpu_times (size_t &idle_time, size_t &total_time) {
FILETIME IdleTime, KernelTime, UserTime;
size_t	thisIdle, thisKernel, thisUser;

	GetSystemTimes (&IdleTime, &KernelTime, &UserTime);
	
	thisIdle	= FileTimeToInt64 (IdleTime);
	thisKernel	= FileTimeToInt64 (KernelTime);
	thisUser	= FileTimeToInt64 (UserTime);
	idle_time	= (size_t) thisIdle;
	total_time	= (size_t)(thisKernel + thisUser);
	return true;
}
#else
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
#endif

static
uint8_t convert (QString s) {
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

	RadioInterface::RadioInterface (QSettings	*Si,
	                                const QString	&presetFile,
	                                const QString	&freqExtension,
	                                int32_t		dataPort,
	                                QWidget		*parent):
	                                        QWidget (parent),
	                                        my_presetHandler (this),
	                                        theBand (freqExtension),
	                                        theTable (this) {
int16_t	latency;
int16_t k;
QString h;
QString	presetName;
uint8_t	dabBand;

	dabSettings		= Si;
	running. 		store (false);
	scanning. 		store (false);
	isSynced		= false;
	runningServices. 	resize (0);
	spectrumBuffer          =
	                  new RingBuffer<std::complex<float>> (2 * 32768);
	iqBuffer		=
	                  new RingBuffer<std::complex<float>> (2 * 1536);
	responseBuffer		=
	                  new RingBuffer<float> (32768);
	tiiBuffer		=
	                  new RingBuffer<std::complex<float>> (32768);
	audioBuffer		=
	                  new RingBuffer<int16_t>(16 * 32768);
	frameBuffer		=
	                  new RingBuffer<uint8_t> (2 * 32768);

	dataBuffer		= new RingBuffer<uint8_t>(32768);
	
	switchTime		=
	                  dabSettings -> value ("switchTime", 6000). toInt ();
	latency			=
	                  dabSettings -> value ("latency", 5). toInt();

	bool has_presetName	=
	              dabSettings -> value ("has-presetName", 1). toInt() != 0;
	if (has_presetName) {
	   presetName		=
	              dabSettings -> value ("presetname", ""). toString();
	   dabService s;
	   s. serviceName = presetName;
	   s. SId	  = 0;
	   s. SCIds	  = 0;
	   runningServices. push_back (s);
	}
#ifdef	_SEND_DATAGRAM_
	ipAddress		= dabSettings -> value ("ipAddress", "127.0.0.1"). toString();
	port			= dabSettings -> value ("port", 8888). toInt();
#endif
//
	saveSlides	= dabSettings -> value ("saveSlides", 1). toInt();
	if (saveSlides != 0)
	   set_picturePath ();
/*
 * Experimental:
 *	lots of people seem to want the scan to continue, rather than
 *	stop whever a channel with data is found.
 */

	noSort		= dabSettings -> value ("noSort", 0). toInt () == 1;
	normalScan	= dabSettings -> value ("normalScan", 0). toInt () == 1;
//	The settings are done, now creation of the GUI parts
	setupUi (this);
//
	dataDisplay	= new QFrame (nullptr);
	techData. setupUi (dataDisplay);

	dataDisplay		->  hide();
	Services                = QStringList ();
        model . clear ();
        ensembleDisplay         -> setModel (&model);

	bool showWidget         =
	                 dabSettings -> value ("showDeviceWidget", 0).
                                                              toInt () != 0;
	devicewidgetButton	-> setText (showWidget ? "show" : "hide");

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
	soundOut		= new Qt_Audio();
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

	QString historyFile     = QDir::homePath () + "/.qt-history.xml";
        historyFile             = dabSettings -> value ("history",
                                                    historyFile). toString ();
        historyFile             = QDir::toNativeSeparators (historyFile);
        my_history              = new historyHandler (this, historyFile);

        connect (my_history, SIGNAL (handle_historySelect (const QString &)),
                 this, SLOT (handle_historySelect (const QString &)));
        connect (historyButton, SIGNAL (clicked ()),
                 this, SLOT (handle_historyButton ()));

//	restore some settings from previous incarnations
	QString t       =
	        dabSettings     -> value ("dabBand", "VHF Band III"). toString();
	dabBand         = t == "VHF Band III" ?  BAND_III : L_BAND;

	theBand. setupChannels  (channelSelector, dabBand);

//	QPalette p	= techData. ficError_display -> palette();
	QPalette p	= ficError_display -> palette();
	p. setColor (QPalette::Highlight, Qt::green);
	ficError_display		-> setPalette (p);
//	techData. ficError_display	-> setPalette (p);
	techData. frameError_display	-> setPalette (p);
	techData. rsError_display	-> setPalette (p);
	techData. aacError_display	-> setPalette (p);
	techData. rsError_display	-> hide();
	techData. aacError_display	-> hide();
	techData. motAvailable		-> 
	                   setStyleSheet ("QLabel {background-color : red}");
	techData. fmLabel		-> hide();
	techData. fmFrequency		-> hide();
//
//
	audioDumper		= nullptr;
	rawDumper		= nullptr;
	frameDumper		= nullptr;
	ficBlocks		= 0;
	ficSuccess		= 0;
	syncedLabel		->
	               setStyleSheet ("QLabel {background-color : red; color: white}");
//	stereoLabel		->
//	               setStyleSheet ("QLabel {background-color : red}");

//
	connect (streamoutSelector, SIGNAL (activated (int)),
	         this,  SLOT (set_streamSelector (int)));
	my_presetHandler. loadPresets (presetFile, presetSelector);
//	
//	display the version
	copyrightLabel	-> setToolTip (footText ());

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
//
//	presetTimer
	presetTimer. setSingleShot (true);
	presetTimer. setInterval (switchTime);
	connect (&presetTimer, SIGNAL (timeout (void)),
	            this, SLOT (setPresetStation (void)));

#ifdef	HAVE_SDRPLAY_V2
	deviceSelector	-> addItem ("sdrplay");
#endif
#ifdef	HAVE_SDRPLAY_V3
	deviceSelector	-> addItem ("sdrplay-v3");
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
	deviceSelector	-> addItem ("xml-files");
	inputDevice	= nullptr;
	h               =
	           dabSettings -> value ("device", "no device"). toString();
	k               = deviceSelector -> findText (h);
//	fprintf (stderr, "%d %s\n", k, h. toUtf8(). data());
	if (k != -1) {
	   deviceSelector       -> setCurrentIndex (k);
	   inputDevice	= setDevice (deviceSelector -> currentText());
	}
	
	my_dabProcessor	= nullptr;
//	if a device was selected, we just start, otherwise
//	we wait until one is selected
	if (inputDevice != nullptr) 
	   emit doStart();
	else 
	   connect (deviceSelector, SIGNAL (activated (const QString &)),
	            this,  SLOT (doStart (const QString &)));
	qApp	-> installEventFilter (this);
	currentService	= nullptr;
}

QString RadioInterface::footText () {
	version			= QString (CURRENT_VERSION);
        QString versionText = "Qt-DAB- version: " + QString(CURRENT_VERSION);
        versionText += "Copyright J van Katwijk, J. vanKatwijk@gmail.com\n";
        versionText += "Rights of Qt, fftw, portaudio, libsamplerate and libsndfile gratefully acknowledged";
        versionText += "Rights of other contribuants gratefully acknowledged\n";
        versionText += " Build on: " + QString(__TIMESTAMP__) + QString (" ") + QString (GITHASH);
        return versionText;
}
//
//	doStart (QString) is called when - on startup - no device
//	was registered to be used, and the user presses the
//	selectDevice comboBox
void	RadioInterface::doStart (const QString &dev) {
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
	if (k != -1) 
	   channelSelector -> setCurrentIndex (k);

	frequency		= theBand. Frequency (
	                                 channelSelector -> currentText());
	r = inputDevice		-> restartReader (frequency);
	qDebug ("Starting %d\n", r);
	if (!r) {
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("Opening  input stream failed\n"));
	   return;	// give it another try
	}
	frequencyDisplay	-> display (frequency / 1000000.0);
//	Some buttons should not be touched before we have a device
	connectGUI();

	int threshold		=
	                  dabSettings -> value ("threshold", 3). toInt();
	int diff_length	=
	           dabSettings	-> value ("diff_length", DIFF_LENGTH). toInt();
	int tii_delay   =
	           dabSettings  -> value ("tii_delay", 5). toInt();
	if (tii_delay < 5)
	   tii_delay = 5;
	int     tii_depth       =
	               dabSettings -> value ("tii_depth", 1). toInt();
	int     echo_depth      =
	               dabSettings -> value ("echo_depth", 1). toInt();
	QString dabMode         =
	               dabSettings   -> value ("dabMode", "Mode 1"). toString();
	
//	we avoided up till now connecting the channel selector
//	to the slot since that function does a lot more that we
//	do not want here
	connect (presetSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (handle_presetSelector (const QString &)));
	connect (channelSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (selectChannel (const QString &)));
//	and we connect the deviceSelector to a handler
	disconnect (deviceSelector, SIGNAL (activated (QString)),
	            this,  SLOT (doStart (QString)));
//
//	Just to be sure we disconnect here.
//	It would have been helpful to have a function
//	testing whether or not a connection exists, we need a kind
//	of "reset"
	disconnect (deviceSelector, SIGNAL (activated (const QString &)),
	            this,  SLOT (newDevice (const QString &)));

	connect (deviceSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (newDevice (const QString &)));
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

	if (runningServices. size () > 0) {
	   presetTimer. setSingleShot	(true);
	   presetTimer. setInterval 	(switchTime);
	   presetTimer. start 		(switchTime);
	}

	secondariesVector. resize (0);
	startChannel (channelSelector -> currentText ());
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
	QString presetName;
	
	if (runningServices. size () > 0) {
	   dabService serv = runningServices. back ();
	   s	-> setValue ("presetname", serv. serviceName);
	}

	s	-> setValue ("device",
	                      deviceSelector -> currentText());
	s	-> setValue ("channel",
	                      channelSelector -> currentText());
	s	-> setValue ("soundchannel",
	                               streamoutSelector -> currentText());
	if (inputDevice != nullptr)
           s    -> setValue ("devicewidgetButton",
                                  inputDevice -> isHidden () != 0);
	s	-> sync();
}

//
//
///////////////////////////////////////////////////////////////////////////////
//	
//	a slot called by the ofdmprocessor
void	RadioInterface::set_CorrectorDisplay (int v) {
	if (running. load())
	   correctorDisplay	-> display (v);
}
//
//	might be called when scanning only
void	RadioInterface::signalTimer_out() {
	No_Signal_Found();
}

///////////////////////////////////////////////////////////////////////////
//
//	a slot, called by the fic/fib handlers
void	RadioInterface::addtoEnsemble (const QString &serviceName,
	                                             int32_t serviceId) {
	if (!running. load())
	   return;

	(void)serviceId;
	if (Services. contains (serviceName))
	   return;
	Services << serviceName;
#if 0
	fprintf (stderr, "adding %s serviceId %x subchId %d\n",
	                          serviceName. toLatin1 (). data (),
	                          serviceId, subChId);
#endif
	my_history -> addElement (channelSelector -> currentText (),
	                                                        serviceName);

	if (!noSort)
	   Services. sort ();
	model. clear ();
        for (const auto serv : Services)
           model. appendRow (new QStandardItem (serv));
        int row = model. rowCount ();
        for (int i = 0; i < row; i ++) {
           model. setData (model. index (i, 0),
                      QFont ("Cantarell", 11), Qt::FontRole);
        }

        ensembleDisplay -> setModel (&model);
}
//
//	The ensembleId is written as hexadecimal, however, the 
//	number display of Qt is only 7 segments ...
static
QString hextoString (int v) {
char t [4];
QString res;
int     i;
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

///	a slot, called by the fib processor
void	RadioInterface::nameofEnsemble (int id, const QString &v) {
QString s;
	if (!running. load())
	   return;

	ensembleId	-> setAlignment(Qt::AlignCenter);
	ensembleId	-> setText (v + QString (":") + hextoString (id));
	my_dabProcessor	-> coarseCorrectorOff();
	if (normalScan)
	   stopScanning ();	// if scanning, we are done
}

//////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////
QString	RadioInterface::filenameSuggestion (QString theTime) {
QString	name	= QDir::homePath () + "/Qt-DAB-";

	theTime. replace (":", "-");
	theTime. replace (" ", "-");
	name. append (theTime);
	name. append (".txt");
	return name;
}

void	RadioInterface::handle_contentButton	() {
QString currentChannel	= channelSelector -> currentText();
int32_t	frequency	= inputDevice -> getVFOFrequency();
QString theTime		= localTimeDisplay -> text ();
ensemblePrinter	my_Printer;

	if (!running. load() || (ensembleId -> text () == QString ("")))
	   return;

	QString suggestedFileName	= filenameSuggestion (theTime);
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

//	showMOT is triggered by the MOT handler,
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

	int w   = techData. pictureLabel -> width ();
        int h   = 2 * w / 3;
        techData. pictureLabel ->
                  setPixmap (p. scaled (w, h, Qt::KeepAspectRatio));
        techData. pictureLabel -> show ();
}
//
//	sendDatagram is triggered by the ip handler,
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
//	tdcData is triggered by the backend.
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
  *	If a change is detected, we have to restart the selected
  *	service - if any. If the service is a secondary service,
  *	it might be the case that we have to start the main service
  *	how do we find that?
  */
void	RadioInterface::changeinConfiguration() {
	if (running. load ()) {
	   dabService s;
	   if (runningServices. size () > 0) 
	      s = runningServices. at (runningServices. size () - 1);
	   stopScanning    ();
	   stopService     ();
	   fprintf (stderr, "change detected\n");
//
//	we rebuild the services list from the fib and
//	then we (try to) restart the service
	   Services	= my_dabProcessor -> getServices ();
	   if (!noSort) {
	      Services. sort	();
	      model. clear	();
	      for (const auto serv : Services)
	         model. appendRow (new QStandardItem (serv));
	      int row = model. rowCount ();
	      for (int i = 0; i < row; i ++) {
	         model. setData (model. index (i, 0),
	         QFont ("Cantarell", 11), Qt::FontRole);
	      }
	   }
	   ensembleDisplay -> setModel (&model);
//
//	and restart the one that was running
	   if (s. serviceName != "") {
	      if (s. SCIds != 0) { // secondary service may be gone
	         if (my_dabProcessor -> findService (s. SId, s. SCIds) ==
	                                                   s. serviceName) {
	            runningServices. push_back (s);
	            startService (&s);
	            return;
	         }
	         else {
	            s. SCIds = 0;
	            s. serviceName =
	                  my_dabProcessor -> findService (s. SId, s. SCIds);
	         }
	      }
//	checking for the main service
	      if (s. serviceName != 
	                 my_dabProcessor -> findService (s. SId, s. SCIds)) {
	         QMessageBox::warning (this, tr ("Warning"),
                                tr ("insufficient data for this program\n"));
                 return;
              }
	      runningServices. push_back (s);
	      startService (&s);
	   }
	}
}
//
//	In order to not overload with an enormous amount of
//	signals, we trigger this function at most 10 times a second
//
void	RadioInterface::newAudio	(int amount, int rate) {
	if (running. load ()) {
	   int16_t vec [amount];
	   while (audioBuffer -> GetRingBufferReadAvailable() > amount) {
	      audioBuffer -> getDataFromBuffer (vec, amount);
	      soundOut	-> audioOut (vec, amount, rate);
	   }
	}
}
//

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

	theTable. hide ();
	my_presetHandler. savePresets (presetSelector);
	if (audioDumper != nullptr) {
	   soundOut	-> stopDumping();
	   sf_close (audioDumper);
	}

	if (inputDevice != nullptr) 
	   inputDevice		-> stopReader();	// might be concurrent
	if (my_dabProcessor != nullptr)
	   my_dabProcessor	-> stop();		// definitely concurrent

	delete my_history;
	if (currentService != nullptr)
	   delete currentService;
	currentService	= nullptr;
	soundOut		-> stop();
	dataDisplay		->  hide();
//	everything should be halted by now
	dumpControlState (dabSettings);
	delete		soundOut;
	if (inputDevice != nullptr)
	   delete		inputDevice;
	fprintf (stderr, "going to delete dabProcessor\n");

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
	delete	frameBuffer;
//	close();
	fprintf (stderr, ".. end the radio silences\n");
}

//
static size_t previous_idle_time	= 0;
static size_t previous_total_time	= 0;

void	RadioInterface::updateTimeDisplay() {
	if (!running. load())
	   return;
	numberofSeconds ++;
	int16_t	numberHours	= numberofSeconds / 3600;
	int16_t	numberMinutes	= (numberofSeconds / 60) % 60;
	QString text = QString ("runtime ");
	text. append (QString::number (numberHours));
	text. append (" hr, ");
	text. append (QString::number (numberMinutes));
	text. append (" min");
	runtimeDisplay	-> setText (text);
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
#ifdef	SHOW_MISSING
	if ((numberofSeconds % 10) == 0) {
	   int xxx = ((audioSink *)soundOut)	-> missed();
	   fprintf (stderr, "missed %d\n", xxx);
	}
#endif
}
//
//	precondition: everything is quiet
virtualInput	*RadioInterface::setDevice (const QString &s) {
QString	file;
virtualInput	*inputDevice	= nullptr;
///	OK, everything quiet, now let us see what to do
#ifdef	HAVE_AIRSPY
	if (s == "airspy") {
	   try {
	      inputDevice	= new airspyHandler (dabSettings, version);
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
	      inputDevice	= new hackrfHandler (dabSettings, version);
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
	      inputDevice = new limeHandler (dabSettings, version);
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
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                           tr ("rtl_tcp: no luck\n") );
	      return nullptr;
	   }
	}
	else
#endif
#ifdef	HAVE_SDRPLAY_V2
	if (s == "sdrplay") {
	   try {
	      inputDevice	= new sdrplayHandler (dabSettings, version);
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
#ifdef	HAVE_SDRPLAY_V3
	if (s == "sdrplay-v3") {
	   try {
	      inputDevice	= new sdrplayHandler_v3 (dabSettings, version);
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
	      inputDevice	= new rtlsdrHandler (dabSettings, version);
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
	if (s == "xml-files") {
	   file		= QFileDialog::getOpenFileName (this,
	                                                tr ("Open file ..."),
	                                                QDir::homePath(),
	                                                tr ("xml data (*.*)"));
	   if (file == QString (""))
	      return nullptr;
	   file		= QDir::toNativeSeparators (file);
	   try {
	      inputDevice	= new xml_fileReader (file);
	      hideButtons();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                               tr ("file not found"));
	      return nullptr;
	   }
	}
	else
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

	if (devicewidgetButton -> text () == "hide") {
	   inputDevice  -> show ();
        }
        else {
           inputDevice -> hide ();
        }

	return inputDevice;
}
//
//	newDevice is called from the GUI when selecting a device
void	RadioInterface::newDevice (const QString &deviceName) {
//
//	Part I : stopping all activities
	running. store (false);
	if (inputDevice != nullptr) {
	   stopScanning	();
	   stopService ();
	   stopChannel ();	// will also stop the reader
	   disconnectGUI();
	   if (my_dabProcessor != nullptr)
	      delete my_dabProcessor;
	   delete inputDevice;
	   fprintf (stderr, "device is deleted\n");
	}
	my_dabProcessor		= nullptr;
	inputDevice		= nullptr;
	fprintf (stderr, "going for a device %s\n", deviceName. toLatin1 (). data ());
	inputDevice		= setDevice (deviceName);
	if (inputDevice == nullptr) {
	   inputDevice = new virtualInput();
	   return;		// nothing will happen
	}
	doStart();		// will set running
}

void	RadioInterface::handle_devicewidgetButton	() {
	if (inputDevice == nullptr)
           return;
        disconnect (devicewidgetButton, SIGNAL (clicked ()),
                    this, SLOT (handle_devicewidgetButton ()));

        if (inputDevice -> isHidden ()) {
           inputDevice -> show ();
           devicewidgetButton -> setText ("hide");
           dabSettings -> setValue ("devicewidgetButton", 1);
        }
        else {
           inputDevice -> hide ();
           devicewidgetButton -> setText ("show");
           dabSettings -> setValue ("devicewidgetButton", 0);
        }

        connect (devicewidgetButton, SIGNAL (clicked ()),
                 this, SLOT (handle_devicewidgetButton ()));
}

///////////////////////////////////////////////////////////////////////////
//	signals, received from ofdm_decoder that data is
//	to be displayed
///////////////////////////////////////////////////////////////////////////


void	RadioInterface::showTime	(const QString &s) {
	localTimeDisplay	-> setText (s);
}

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
	if (b) 
	   ficSuccess ++;

	if (++ficBlocks >= 100) {
	   ficError_display	-> setValue (ficSuccess);
//	   techData. ficError_display	-> setValue (ficSuccess);
	   ficSuccess	= 0;
	   ficBlocks	= 0;
	}
}

void	RadioInterface::show_motHandling (bool b) {
	if (!running. load())
	   return;
	if (b) 
	   techData. motAvailable -> 
	               setStyleSheet ("QLabel {background-color : green; color: white}");
	else 
	   techData. motAvailable ->
	               setStyleSheet ("QLabel {background-color : red; color : white}");
}
	
//	called from the ofdmDecoder, it is computed for each frame
void	RadioInterface::show_snr (int s) {
	if (running. load())
	   snrDisplay	-> display (s);
}

//	just switch a color, called from the ofdmprocessor
void	RadioInterface::setSynced	(bool b) {
	if (!running. load() || (isSynced == b))
	   return;

	isSynced = b;
	if (isSynced) 
	      syncedLabel -> 
	               setStyleSheet ("QLabel {background-color : green; color: white}");
	else
	      syncedLabel ->
	               setStyleSheet ("QLabel {background-color : red}");
}

void	RadioInterface::showLabel	(QString s) {
	if (running. load())
	   dynamicLabel	-> setText (s);
}

void	RadioInterface::setStereo	(bool s) {
	(void)s;
//	if (!running. load())
//	   return;
//	if (s) { 
//	   stereoLabel -> 
//	               setStyleSheet ("QLabel {background-color : green; color: white}");
//	   stereoLabel -> setText ("stereo");
//	}
//	else {
//	   stereoLabel ->
//	               setStyleSheet ("QLabel {background-color : red}");
//	   stereoLabel -> setText ("");
//	}
}

void	RadioInterface::show_tii (QByteArray data) {
int8_t  mainId, subId;
QString a = "Est: ";

        if (!running. load())
           return;

        if (data. size () > 1) {
           mainId       = data. at (0);
           subId        = data. at (1);
           a. append (QString::number (mainId) + " " + QString::number (subId));
           transmitter_coordinates      -> setAlignment (Qt::AlignRight);
           transmitter_coordinates	-> setText (a);
           my_tiiViewer -> showSecondaries (data);
        }
        my_tiiViewer    -> showSpectrum (1);

}

void	RadioInterface::showSpectrum	(int32_t amount) {
	if (running. load())
	   my_spectrumViewer -> showSpectrum (amount,
				              inputDevice -> getVFOFrequency());
}

void	RadioInterface::showIQ	(int amount) {
	if (running. load())
	   my_spectrumViewer	-> showIQ (amount);
}

void	RadioInterface::showQuality	(float q) {
	if (running. load())
	   my_spectrumViewer	-> showQuality (q);
}

void	RadioInterface::showImpulse (int amount) {
	if (running. load())
	   my_correlationViewer -> showCorrelation (amount);
}

void	RadioInterface::showIndex (int ind) {
	if (!running. load())
	   return;

	my_correlationViewer -> showIndex (ind);
}

//
////////////////////////////////////////////////////////////////////////////

void	RadioInterface:: set_streamSelector (int k) {
#if	not defined (TCP_STREAMER) &&  not defined (QT_AUDIO)
	((audioSink *)(soundOut)) -> selectDevice (k);
#else
	(void)k;
#endif
}

void	RadioInterface::handle_detailButton	() {
	if (dataDisplay -> isHidden())
	   dataDisplay -> show();
	else
	   dataDisplay -> hide();
}

void	RadioInterface::showButtons() {
	scanButton		-> show ();
	channelSelector		-> show ();
	dumpButton		-> show	();
	nextChannelButton	-> show ();
	prevChannelButton	-> show ();
	frequencyDisplay	-> show ();
	presetSelector		-> show ();
}

void	RadioInterface::hideButtons() {
	scanButton		-> hide ();
	channelSelector		-> hide ();
	dumpButton		-> hide	();
	nextChannelButton	-> hide ();
	prevChannelButton	-> hide ();
	frequencyDisplay	-> hide ();
	presetSelector		-> hide ();
}

void	RadioInterface::setSyncLost() {
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
	if (picturesPath != "") {
	   QDir testdir (picturesPath);

	   if (!testdir. exists())
	      testdir. mkdir (picturesPath);
	}
}

void	RadioInterface::handle_resetButton	() {
	if (!running. load())
	   return;
	stopChannel ();
	startChannel	(channelSelector -> currentText ());
}
//
////////////////////////////////////////////////////////////////////////
//
//	dump handling
//
/////////////////////////////////////////////////////////////////////////
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

void	RadioInterface::stop_sourceDumping	() {
	if (rawDumper == nullptr) 
	   return;
	my_dabProcessor	-> stopDumping();
	sf_close (rawDumper);
	rawDumper	= nullptr;
	colorButton (dumpButton, Qt::white, 10);
	dumpButton	-> setText ("Raw dump");
	dumpButton	-> update ();
}

void	RadioInterface::start_sourceDumping () {
SF_INFO *sf_info        = (SF_INFO *)alloca (sizeof (SF_INFO));
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

void	RadioInterface::handle_sourcedumpButton () {

	if (rawDumper != nullptr)
	   stop_sourceDumping ();
	else
	   start_sourceDumping ();
}

void	RadioInterface::stop_audioDumping	() {
	if (audioDumper == nullptr)
	   return;
	soundOut	-> stopDumping();
	sf_close (audioDumper);
	audioDumper = nullptr;
	colorButton (audioDumpButton, Qt::white, 10);
	audioDumpButton	-> setText ("audio dump");
	audioDumpButton	-> update ();
}

void	RadioInterface::start_audioDumping () {
SF_INFO	*sf_info	= (SF_INFO *)alloca (sizeof (SF_INFO));
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

void	RadioInterface::handle_audiodumpButton () {

	if (audioDumper != nullptr) 
	   stop_audioDumping ();	
	else
	   start_audioDumping ();
}

void	RadioInterface::stop_frameDumping () {
	if (frameDumper == nullptr)
	   return;
	fclose (frameDumper);
	colorButton (frameDumpButton, Qt::white, 10);
	frameDumpButton	-> setText ("frame dump");
	frameDumpButton	-> update ();
	frameDumper	= nullptr;
}

void	RadioInterface::start_frameDumping () {
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

void	RadioInterface::handle_framedumpButton () {
	if (frameDumper != nullptr) 
	   stop_frameDumping ();
	else
	   start_frameDumping ();
}

void    RadioInterface::newFrame        (int amount) {
uint8_t buffer [amount];

	if (frameDumper == nullptr) 
	   frameBuffer	-> FlushRingBuffer ();
	else
	while (frameBuffer -> GetRingBufferReadAvailable () >= amount) {
	   frameBuffer     -> getDataFromBuffer (buffer, amount);
	   if (frameDumper != nullptr)
	      fwrite (buffer, amount, 1, frameDumper);
	}
}

void	RadioInterface::handle_tiiButton	() {
	if (my_tiiViewer -> isHidden())
	   my_tiiViewer -> show();
	else
	   my_tiiViewer -> hide();
}

void	RadioInterface::handle_correlationButton	() {
	if (my_correlationViewer -> isHidden())
	   my_correlationViewer	-> show();
	else
	   my_correlationViewer	-> hide();
}

void	RadioInterface::handle_spectrumButton	() {
	if (my_spectrumViewer -> isHidden())
	   my_spectrumViewer -> show();
	else
	   my_spectrumViewer -> hide();
}

void    RadioInterface::handle_historyButton    () {
        if (my_history  -> isHidden ())
           my_history   -> show ();
        else
           my_history   -> hide ();
}
//
//	When changing (or setting) a device, we do not want anybody
//	to have the buttons on the GUI touched, so
//	we just disconnet them and (re)connect them as soon as
//	a device is operational
void	RadioInterface::connectGUI() {
	connect (detailButton, SIGNAL (clicked ()),
	         this, SLOT (handle_detailButton ()));
	connect (prevServiceButton, SIGNAL (clicked ()),
                 this, SLOT (handle_prevServiceButton ()));
        connect (nextServiceButton, SIGNAL (clicked ()),
                 this, SLOT (handle_nextServiceButton ()));
        connect (devicewidgetButton, SIGNAL (clicked ()),
                 this, SLOT (handle_devicewidgetButton ()));
	connect (contentButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_contentButton (void)));
	connect (ensembleDisplay, SIGNAL (clicked (QModelIndex)),
	         this, SLOT (selectService (QModelIndex)));
	connect (resetButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_resetButton (void)));
	connect	(scanButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_scanButton (void)));
	connect (nextChannelButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_nextChannelButton (void)));
	connect	(prevChannelButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_prevChannelButton (void)));
	connect (dumpButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_sourcedumpButton (void)));
	connect (audioDumpButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_audiodumpButton (void)));
	connect (frameDumpButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_framedumpButton (void)));
	connect (show_tiiButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_tiiButton (void)));
	connect (show_correlationButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_correlationButton (void)));
	connect (show_spectrumButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_spectrumButton (void)));
}

void	RadioInterface::disconnectGUI() {
	disconnect (detailButton, SIGNAL (clicked ()),
	            this, SLOT (handle_detailButton ()));
	disconnect (prevServiceButton, SIGNAL (clicked ()),
                    this, SLOT (handle_prevServiceButton ()));
        disconnect (nextServiceButton, SIGNAL (clicked ()),
                    this, SLOT (handle_nextServiceButton ()));
	disconnect (devicewidgetButton, SIGNAL (clicked ()),
                    this, SLOT (handle_devicewidgetButton ()));
	disconnect (contentButton, SIGNAL (clicked (void)),
	            this, SLOT (handle_contentButton (void)));
	disconnect (ensembleDisplay, SIGNAL (clicked (QModelIndex)),
	            this, SLOT (selectService (QModelIndex)));
	disconnect (resetButton, SIGNAL (clicked (void)),
	            this, SLOT (handle_resetButton (void)));
	disconnect (scanButton, SIGNAL (clicked (void)),
	            this, SLOT (handle_scanButton (void)));
	disconnect (nextChannelButton, SIGNAL (clicked (void)),
	            this, SLOT (handle_nextChannelButton (void)));
	disconnect (prevChannelButton, SIGNAL (clicked (void)),
	            this, SLOT (handle_prevChannelButton (void)));
	disconnect (dumpButton, SIGNAL (clicked (void)),
	            this, SLOT (handle_sourcedumpButton (void)));
	disconnect (audioDumpButton, SIGNAL (clicked (void)),
	            this, SLOT (handle_audiodumpButton (void)));
	disconnect (frameDumpButton, SIGNAL (clicked (void)),
	            this, SLOT (handle_framedumpButton (void)));
	disconnect (show_tiiButton, SIGNAL (clicked (void)),
	            this, SLOT (handle_tiiButton (void)));
	disconnect (show_correlationButton, SIGNAL (clicked (void)),
	            this, SLOT (handle_correlationButton (void)));
	disconnect (show_spectrumButton, SIGNAL (clicked (void)),
	            this, SLOT (handle_spectrumButton (void)));
}

//
//
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
	if (event -> type () == QEvent::KeyPress) {
	   QKeyEvent *ke = static_cast <QKeyEvent *> (event);
	   if (ke -> key () == Qt::Key_Return) {
	      QString serviceName =
	         ensembleDisplay -> currentIndex ().
	                             data (Qt::DisplayRole). toString ();
	      if ((runningServices. size () > 0) &&
	             (runningServices. at (runningServices. size () - 1). serviceName !=
	                          serviceLabel -> text ())) {
	         selectService (ensembleDisplay -> currentIndex ());
              }
           }
        }
        else
	if ((obj == this -> my_history -> viewport ()) &&
            (event -> type () == QEvent::MouseButtonPress)) {
           QMouseEvent *ev = static_cast<QMouseEvent *>(event);
           if (ev -> buttons () & Qt::RightButton) {
              my_history -> clearHistory ();
           }
        }
        else
	if ((obj == this -> ensembleDisplay -> viewport()) &&
	    (event -> type() == QEvent::MouseButtonPress )) {
	   QMouseEvent *ev = static_cast<QMouseEvent *>(event);
	   if (ev -> buttons() & Qt::RightButton) {
	      audiodata ad;
	      packetdata pd;
	      QString serviceName =
	           this -> ensembleDisplay -> indexAt (ev -> pos()). data().toString();
	      if (serviceName. at (1) == ' ')
	         return true;
	      my_dabProcessor -> dataforAudioService (serviceName, &ad);
	      if (ad. defined && (serviceLabel -> text () == serviceName)) {
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

void	RadioInterface::startAnnouncement (const QString &name, int subChId) {
//	fprintf (stderr, "announcement for %s\n", name. toLatin1 (). data ());
	if (name == serviceLabel -> text ()) {
	   serviceLabel	-> setStyleSheet ("QLabel {color : red}");
	   fprintf (stderr, "announcement for %s (%d) starts\n",
	                             name. toLatin1 (). data (), subChId);
	}
}

void	RadioInterface::stopAnnouncement (const QString &name, int subChId) {
	(void)subChId;
	if (name == serviceLabel -> text ()) {
	   serviceLabel ->
	              setStyleSheet ("QLabel {color : black}");
	   fprintf (stderr, "end for announcement service %s\n",
	                              name. toLatin1 (). data ());
	}
}

////////////////////////////////////////////////////////////////////////
//
//	preset selection, either from presets or from history
////////////////////////////////////////////////////////////////////////

void    RadioInterface::handle_historySelect (const QString &s) {
        presetTimer. stop ();
        localSelect (s);
}

//
void    RadioInterface::handle_presetSelector (const QString &s) {
        presetTimer. stop ();
        if ((s == "Presets") || (presetSelector -> currentIndex () == 0))
           return;
        localSelect (s);
}

void	RadioInterface::localSelect (const QString &s) {
	QStringList list = s.split (":", QString::SkipEmptyParts);
	if (list. length () != 2)
	   return;
	QString channel = list. at (0);
	QString service	= list. at (1);
	stopScanning ();
	if (channel == channelSelector -> currentText ()) {
	   stopService ();
	   dabService s;
	   my_dabProcessor -> getParameters (service, &s. SId, &s. SCIds);
	   if (s. SId == 0) {
              QMessageBox::warning (this, tr ("Warning"),
                                 tr ("insufficient data for this program\n"));
              return;
           }
	   s. serviceName = service;
	   runningServices. push_back (s);
	   startService (&s);
	   return;
	}
//
//	The hard part is stopping the current service,
//      selecting a new channel,
//      waiting a while
//      trying to start the selected service
	disconnect (channelSelector, SIGNAL (activated (const QString &)),
	            this, SLOT (selectChannel (const QString &)));
	int k           = channelSelector -> findText (channel);
	if (k != -1) {
	   channelSelector -> setCurrentIndex (k);
	}
	else 
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("Incorrect preset\n"));
	connect (channelSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (selectChannel (const QString &)));
	if (k == -1)
	   return;

	stopChannel ();
	runningServices. resize (0);
	dabService serv;
	serv. serviceName = service;
	my_dabProcessor -> getParameters (service, &serv. SId, &serv. SCIds);
	if (serv. SId == 0) {
	   QMessageBox::warning (this, tr ("Warning"),
	                         tr ("insufficient data for this program\n"));
           return;
        }

	runningServices. push_back (serv);
	presetTimer. setSingleShot (true);
	presetTimer. setInterval (switchTime);
	presetTimer. start (switchTime);
	startChannel	(channelSelector -> currentText ());
}

////////////////////////////////////////////////////////////////////////////
//
//	handling services: stop and start
///////////////////////////////////////////////////////////////////////////

void	RadioInterface::stopService	() {
	presetTimer. stop ();
	signalTimer. stop ();
	if (runningServices. size () > 0) {
	   dabService s = runningServices. at (runningServices. size () - 1);
	   QString serviceName = s. serviceName;
	   soundOut	-> stop ();
	   for (int i = 0; i < model. rowCount (); i ++) {
	      QString itemText =
	          model. index (i, 0). data (Qt::DisplayRole). toString ();
	      if (itemText == serviceName) {
                 my_dabProcessor -> reset_msc ();
	         colorService (model. index (i, 0), Qt::black, 11);
	         break;
	      }
	   }
	}
	cleanScreen	();
}
//
void	RadioInterface::selectService (QModelIndex ind) {
QString	currentProgram = ind. data (Qt::DisplayRole). toString();
	presetTimer.	stop();
	presetSelector -> setCurrentIndex (0);
	signalTimer.	stop ();
	stopScanning	();
	stopService 	();		// if any

	dabService s;
	my_dabProcessor -> getParameters (currentProgram, &s. SId, &s. SCIds);
	if (s. SId == 0) {
	   QMessageBox::warning (this, tr ("Warning"),
 	                         tr ("insufficient data for this program\n"));	
	   return;
	}
	s. serviceName = currentProgram;
	runningServices. push_back (s);
	startService (&s);
}
//
void	RadioInterface::startService (dabService *s) {
QString serviceName	= s -> serviceName;

	if (runningServices. size () < 1) 
	   fprintf (stderr, "Sorry, no service planned\n");

	int rowCount	= model. rowCount ();
	for (int i = 0; i < rowCount; i ++) {
	   QString itemText =
	           model. index (i, 0). data (Qt::DisplayRole). toString ();
	   if (itemText == serviceName) {
	      colorService (model. index (i, 0), Qt::red, 15);
	      serviceLabel	-> setStyleSheet ("QLabel {color : black}");
	      serviceLabel	-> setText (serviceName);
	      if (my_dabProcessor -> is_audioService (serviceName))
	         start_audioService (serviceName);
	      else
	      if (my_dabProcessor -> is_packetService (serviceName))
	         start_packetService (serviceName);
	      else
	         fprintf (stderr, "%s is not clear\n",
	                            serviceName. toLatin1 (). data ());
	      return;
	   }
	}
}

void    RadioInterface::colorService (QModelIndex ind, QColor c, int pt) {
	QMap <int, QVariant> vMap = model. itemData (ind);
	vMap. insert (Qt::ForegroundRole, QVariant (QBrush (c)));
	model. setItemData (ind, vMap);
	model. setData (ind, QFont ("Cantarell", pt), Qt::FontRole);
}
//
//	This function is only used in the Gui to clear
//	the details of a selected service
void	RadioInterface::cleanScreen	() {
	serviceLabel			-> setText ("");
	dynamicLabel			-> setText ("");
	presetSelector			-> setCurrentIndex (0);
	techData. frameError_display	-> setValue (0);
	techData. rsError_display	-> setValue (0);
	techData. aacError_display	-> setValue (0);
	techData. programName		-> setText (QString (""));
	techData. bitrateDisplay	-> display (0);
	techData. startAddressDisplay	-> display (0);
	techData. lengthDisplay		-> display (0);
	techData. subChIdDisplay	-> display (0);
	techData. uepField		-> setText (QString (""));
	techData. ASCTy			-> setText (QString (""));
	techData. language		-> setText (QString (""));
	techData. programType		-> setText (QString (""));
	techData. motAvailable		-> 
	               setStyleSheet ("QLabel {background-color : red}");
	setStereo	(false);
}

void	RadioInterface::start_audioService (const QString &serviceName) {
audiodata ad;

	my_dabProcessor -> dataforAudioService (serviceName, &ad);
	if (!ad. defined) {
	   QMessageBox::warning (this, tr ("Warning"),
 	                         tr ("insufficient data for this program\n"));
	   return;
	}

	serviceLabel -> setAlignment(Qt::AlignCenter);
	serviceLabel -> setText (serviceName);

	my_dabProcessor -> set_audioChannel (&ad, audioBuffer);
	for (int i = 1; i < 10; i ++) {
	   packetdata pd;
	   my_dabProcessor -> dataforPacketService (serviceName, &pd, i);
	   if (pd. defined) {
	      my_dabProcessor -> set_dataChannel (&pd, dataBuffer);
	      break;
	   }
	}
//	activate sound
	soundOut -> restart ();
//	show service related data
	techData. programName		-> setText (serviceName);
	techData. serviceIdDisplay	-> display (ad. SId);
	techData. bitrateDisplay 	-> display (ad. bitRate);
	techData. startAddressDisplay 	-> display (ad. startAddr);
	techData. lengthDisplay		-> display (ad. length);
	techData. subChIdDisplay 	-> display (ad. subchId);
	QString protL	= getProtectionLevel (ad. shortForm, ad. protLevel);
	techData. uepField		-> setText (protL);
	techData. ASCTy			-> setText (ad. ASCTy == 077 ?
	                                                  "DAB+" : "DAB");
	if (ad. ASCTy == 077) {
	   techData. rsError_display	-> show ();
	   techData. aacError_display	-> show ();
	}
	else {
	   techData. rsError_display	-> hide	();
	   techData. aacError_display	-> hide ();
	}
	  
	techData. language ->
	   setText (getLanguage (ad. language));
	techData. programType ->
	   setText (the_textMapper.
	               get_programm_type_string (ad. programType));
	if (ad. fmFrequency == -1) {
	   techData. fmFrequency	-> hide ();
	   techData. fmLabel		-> hide	();
	}
	else {
	   techData. fmLabel		-> show ();
	   techData. fmFrequency	-> show ();
	   QString f = QString::number (ad. fmFrequency);
	   f. append (" Khz");
	   techData. fmFrequency	-> setText (f);
	}
}

void	RadioInterface::start_packetService (const QString &s) {
packetdata pd;

	my_dabProcessor -> dataforPacketService (s, &pd, 0);
	if ((!pd. defined) ||
	            (pd.  DSCTy == 0) || (pd. bitRate == 0)) {
	   QMessageBox::warning (this, tr ("sdr"),
 	                         tr ("still insufficient data for this service\n"));
	   return;
	}

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

////////////////////////////////////////////////////////////////////////////
//
//	next and previous service selection
////////////////////////////////////////////////////////////////////////////

//
//	Previous and next services. trivial implementation
void	RadioInterface::handle_prevServiceButton	() {
	disconnect (prevServiceButton, SIGNAL (clicked ()),
	            this, SLOT (handle_prevServiceButton ()));
	QString currentService = serviceLabel -> text ();
	stopScanning ();
	presetTimer. stop ();
	stopService  ();
	if ((Services. length () != 0) && (currentService != "")) {
	   for (int i = 0; i < Services. length (); i ++) {
	      if (Services. at (i) == currentService) {
	         i = i - 1;
	         if (i < 0) i =
	            Services. length () - 1;
	         dabService s;
	         my_dabProcessor -> getParameters (Services. at (i),
	                                           &s. SId, &s. SCIds);
	         if (s. SId == 0) {
                    QMessageBox::warning (this, tr ("Warning"),
                                 tr ("insufficient data for this program\n"));
	            break;
                 }
	         s. serviceName = Services. at (i);
	         runningServices. push_back (s);
	         startService (&s);
	         break;
	      }
	   }
	}
	connect (prevServiceButton, SIGNAL (clicked ()),
	         this, SLOT (handle_prevServiceButton ()));
}

void	RadioInterface::handle_nextServiceButton	() {
	disconnect (nextServiceButton, SIGNAL (clicked ()),
	            this, SLOT (handle_nextServiceButton ()));
	QString currentService = serviceLabel -> text ();
	stopScanning ();
	presetTimer. stop ();
	stopService ();
	if ((Services. length () != 0) && (currentService != "")) {
	   for (int i = 0; i < Services. length (); i ++) {
	      if (Services. at (i) == currentService) {
	         i = i + 1;
	         if (i >= Services. length ())
	            i = 0;
	         dabService s;
	         s. serviceName = Services. at (i);
	         my_dabProcessor -> getParameters (s. serviceName,
	                                           &s. SId, &s. SCIds);
	         if (s. SId == 0) {
	            QMessageBox::warning (this, tr ("Warning"),
	                      tr ("insufficient data for this program\n"));
	            break;
                 }

	         startService (&s);
	         break;
	      }
	   }
	}
	connect (nextServiceButton, SIGNAL (clicked ()),
	         this, SLOT (handle_nextServiceButton ()));
}

////////////////////////////////////////////////////////////////////////////
//
//	The user(s)
///////////////////////////////////////////////////////////////////////////

void	RadioInterface::setPresetStation() {
	if (ensembleId -> text () == QString ("")) {
	   QMessageBox::warning (this, tr ("Warning"),
	                          tr ("Oops, ensemble not yet recognized\nselect service manually\n"));
	   return;
	}
	stopScanning ();

	if (runningServices. size () == 0)
	   return;

	QString presetName	= runningServices. back (). serviceName;
	for (const auto& service: Services) {
	   if (service. contains (presetName)) {
	      fprintf (stderr, "going to select %s\n", presetName. toLatin1 (). data ());
	      dabService s;
	      s. serviceName = presetName;
	      my_dabProcessor	-> getParameters (presetName, &s. SId, &s. SCIds);
	      if (s. SId == 0) {
	         QMessageBox::warning (this, tr ("Warning"),
	                        tr ("insufficient data for this program\n"));
	         return;
	      }
	      s. serviceName = presetName;
	      startService (&s);
	      return;
	   }
	}
//
//	not found, no service selected
	runningServices. pop_back ();
	fprintf (stderr, "presetName %s not found\n", presetName. toLatin1 (). data ());
}

///////////////////////////////////////////////////////////////////////////
//
//	Channel basics
///////////////////////////////////////////////////////////////////////////
//	Precondition: no channel should be activated
//	
//	Others assume that the name of the service is
//	save in the runningServices
void	RadioInterface::startChannel (const QString &channel) {
int	tunedFrequency	=
	         theBand. Frequency (channel);
	frequencyDisplay	-> display (tunedFrequency / 1000000.0);
	inputDevice		-> restartReader (tunedFrequency);
	my_dabProcessor		-> start ();
}
//
//	apart from stopping the reader, a lot of administration
//	is to be done.
//	The "stopService" (if any) clears the service related
//	elements on the screen(s)
void	RadioInterface::stopChannel	() {
	if ((inputDevice == nullptr) || (my_dabProcessor == nullptr))
	   return;
	inputDevice			-> stopReader ();
	stopService ();
	if (!my_dabProcessor -> isRunning ())
	   return;		// do not stop twice
	my_dabProcessor			-> stop ();
	ficError_display		-> setValue (0);
//	techData. ficError_display	-> setValue (0);
	stop_sourceDumping	();
	stop_audioDumping	();
//	note framedumping - if any - was already stopped
	my_tiiViewer	-> clear();
//	the visual elements
	setSynced	(false);
	ensembleId			-> setText ("");
	snrDisplay			-> display (0);
	transmitter_coordinates		-> setText (" ");
	frequencyDisplay		-> display (0);
	Services 	= QStringList ();
	model. clear ();
	ensembleDisplay			-> setModel (&model);
}

void    RadioInterface::selectChannel (const QString &channel) {
	presetTimer. stop ();
	stopService ();
	stopScanning ();
	stopChannel ();
	startChannel (channel);
}
//
/////////////////////////////////////////////////////////////////////////
//
//	next- and previous channel buttons
/////////////////////////////////////////////////////////////////////////

void	RadioInterface::handle_nextChannelButton () {
int     currentChannel  = channelSelector -> currentIndex ();
	stopScanning ();
	presetTimer. stop ();
	presetSelector -> setCurrentIndex (0);
	stopService ();
	stopChannel ();
	currentChannel ++;
	if (currentChannel >= channelSelector -> count ())
	   currentChannel = 0;
	disconnect (channelSelector, SIGNAL (activated (const QString &)),
	            this, SLOT (selectChannel (const QString &)));
	channelSelector -> setCurrentIndex (currentChannel);
	connect (channelSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (selectChannel (const QString &)));
	startChannel (channelSelector -> currentText ());
}

void	RadioInterface::handle_prevChannelButton () {
int     currentChannel  = channelSelector -> currentIndex ();
	stopScanning ();
	presetTimer. stop ();
	stopChannel ();
	currentChannel --;
	if (currentChannel < 0)
	   currentChannel =  channelSelector -> count () - 1;
	disconnect (channelSelector, SIGNAL (activated (const QString &)),
	            this, SLOT (selectChannel (const QString &)));
	channelSelector -> setCurrentIndex (currentChannel);
	connect (channelSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (selectChannel (const QString &)));
	startChannel (channelSelector -> currentText ());
}

////////////////////////////////////////////////////////////////////////
//
//	scanning
/////////////////////////////////////////////////////////////////////////

void	RadioInterface::handle_scanButton () {
	if (!running. load ())
	   return;
	if (scanning. load ()) 
	   stopScanning ();
	else
	   startScanning ();
}

void	RadioInterface::startScanning	() {
	presetTimer. stop ();
        presetSelector -> setCurrentIndex (0);
        stopService     ();
        stopChannel     ();
	int  cc      = channelSelector -> currentIndex ();
	if (normalScan) {
	   cc ++;
	   if (cc >= channelSelector -> count ())
              cc = 0;
	}
	else {
	   cc = 0;
	}
        scanning. store (true);

//      To avoid reaction of the system on setting a different value:
        disconnect (channelSelector, SIGNAL (activated (const QString &)),
                    this, SLOT (selectChannel (const QString &)));
        channelSelector -> setCurrentIndex (cc);
        connect (channelSelector, SIGNAL (activated (const QString &)),
                 this, SLOT (selectChannel (const QString &)));
        my_dabProcessor -> set_scanMode (scanning. load ());
        scanButton      -> setText ("scanning");
        startChannel    (channelSelector -> currentText ());
        signalTimer. start (switchTime);
	if (!normalScan) {
	   theTable. clear ();
	   theTable. show ();
	}
}

void	RadioInterface::stopScanning	() {
      if (!running. load())
           return;
        if (!scanning. load ())
           return;
        signalTimer. stop ();
	theTable. hide ();
        scanning. store (false);
        my_dabProcessor -> set_scanMode (scanning. load ());
        scanButton      -> setText ("scan");
}

//	If the ofdm processor has waited for a period of N frames
//	to get a start of a synchronization,
//	it sends a signal to the GUI handler
//	If "scanning" is "on" we hop to the next frequency on
//	the list

void	RadioInterface::No_Signal_Found () {
	signalTimer. stop ();
	if (running. load () && scanning. load ()) {
	   int	cc	= channelSelector -> currentIndex ();
	   if ((!normalScan) && (Services != QStringList ()))
	      showServices ();
	   stopChannel ();
	   cc ++;
	   if ((cc >= channelSelector -> count ()) && !normalScan) {
//	if at the end we can't use "stopScanning", since that
//	hides the table, and we want to stay visible until ...
                 signalTimer. stop ();
//	      my_dabProcessor -> set_scanMode (scanning. load ());
	   }
	   else {  // we just continue
	      if (cc >= channelSelector -> count ())
	         cc = 0;
//	To avoid reaction of the system on setting a different value:
	      disconnect (channelSelector, SIGNAL (activated (const QString &)),
	                  this, SLOT (selectChannel (const QString &)));
	      channelSelector -> setCurrentIndex (cc);
	      connect (channelSelector, SIGNAL (activated (const QString &)),
	               this, SLOT (selectChannel (const QString &)));

	      my_dabProcessor	-> set_scanMode (true);
	      startChannel (channelSelector -> currentText ());
	      signalTimer. start (switchTime);
	   }
	}
	else
	if (scanning. load ()) 
	   stopScanning ();
}

////////////////////////////////////////////////////////////////////////////
//
// showServices
////////////////////////////////////////////////////////////////////////////

void	RadioInterface::showServices () {
QString SNR = "SNR " + QString::number (snrDisplay -> value ());
QString ensembleId	= hextoString (my_dabProcessor -> get_ensembleId ());
	theTable. newEnsemble (" ",
	                       channelSelector -> currentText (),
	                       my_dabProcessor	-> get_ensembleName (),
	                       ensembleId,
	                       SNR,
	                       transmitter_coordinates -> text ());
	for (QString& audioService: Services) {
	   audiodata d;
	   my_dabProcessor -> dataforAudioService (audioService, &d);
//	   if (!d. defined)
//	      continue;

	   QString serviceId = hextoString (d. SId);
	   QString bitRate   = QString::number (d. bitRate);
	   QString protL     = getProtectionLevel (d. shortForm,
                                                   d. protLevel);
	   QString codeRate  = getCodeRate (d. shortForm,
                                            d. protLevel);
	   theTable.
                  add_to_Ensemble (audioService, serviceId,
                                   d. ASCTy == 077 ? "DAB+" : "DAB",
                                   bitRate, protL, codeRate);
	}
}

