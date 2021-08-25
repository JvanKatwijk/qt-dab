#
/*
 *    Copyright (C) 2018 .. 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the dab-2
 *
 *    dab-2 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    dab-2 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *    You should have received a copy of the GNU General Public License
 *    along with dab-2; if not, write to the Free Software
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
#include	"mot-content-types.h"
#include	<iostream>
#include	<numeric>
#include	<unistd.h>
#include	<vector>
#include	"radio.h"
#include	"band-handler.h"
#include	"ensemble-printer.h"
#include	"audio-descriptor.h"
#include	"data-descriptor.h"
#include	"device-handler.h"
#include	"wavfiles.h"
#include	"rawfiles.h"
#include	"xml-filereader.h"
#include	"color-selector.h"
#include        "schedule-selector.h"
#include        "element-selector.h"
#include	"dab-tables.h"
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
#include	"sdrplay-handler.h"
#endif
#ifdef	HAVE_SDRPLAY_V3
#include	"sdrplay-handler-v3.h"
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
#ifdef	HAVE_PLUTO
#include	"pluto-handler.h"
#endif
#include	"ui_technical_data.h"
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
#define SINGLE_SCAN		0
#define SCAN_TO_DATA		1
#define SCAN_CONTINUOUSLY	2

static inline
QString scanmodeText (int e) {
	if (e == SINGLE_SCAN)
	   return QString ("single scan");
	if (e == SCAN_TO_DATA)
	   return QString ("scan to data");
	if (e == SCAN_CONTINUOUSLY)
	   return QString ("scan continuously");
	return QString ("???");
}


#define	CONTENT_BUTTON		QString ("contentButton")
#define DETAIL_BUTTON		QString ("detailButton")
#define	RESET_BUTTON		QString ("resetButton")
#define SCAN_BUTTON		QString ("scanButton")
#define	TII_BUTTON		QString ("tiiButton")
#define	CORRELATION_BUTTON	QString ("correlationButton")
#define	SPECTRUM_BUTTON		QString ("spectrumButton")
#define	DEVICEWIDGET_BUTTON	QString ("devicewidgetButton")
#define	HISTORY_BUTTON		QString ("historyButton")
#define	DUMP_BUTTON		QString ("dumpButton")
#define	MUTE_BUTTON		QString ("muteButton")
#define PREVCHANNEL_BUTTON	QString ("prevChannelButton")
#define NEXTCHANNEL_BUTTON	QString ("nextChannelButton")
#define PREVSERVICE_BUTTON	QString ("prevServiceButton")
#define NEXTSERVICE_BUTTON	QString ("nextServiceButton")
#define	FRAMEDUMP_BUTTON	QString ("framedumpButton")
#define	AUDIODUMP_BUTTON	QString ("audiodumpButton")
#define	CONFIG_BUTTON		QString ("configButton")
#define DLTEXT_BUTTON           QString ("dlTextButton")


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
	                                        spectrumBuffer (2 * 32768),
	                                        iqBuffer (2 * 1536),
	                                        tiiBuffer (32768),
	                                        responseBuffer (32768),
	                                        frameBuffer (2 * 32768),
		                                dataBuffer (32768),
	                                        audioBuffer (8 * 32768),
	                                        my_spectrumViewer (
	                                                 this, Si,
	                                                 &spectrumBuffer,
	                                                 &iqBuffer),
	                                        my_correlationViewer (
	                                                 this, Si,
	                                                 &responseBuffer),
	                                        my_tiiViewer (
	                                                 this, Si,
		                                         &tiiBuffer),
	                                        my_presetHandler (this),
	                                        theBand (freqExtension, Si),
	                                        filenameFinder (Si),
	                                        the_dlCache (10),
	                                        theTable (this),
	                                        theScheduler (this) {
int16_t	latency;
int16_t k;
QString h;
QString	presetName;
uint8_t	dabBand;

	dabSettings		= Si;
	running. 		store (false);
	scanning. 		store (false);
	isSynced		= false;
	stereoSetting		= false;
	dlTextFile		= nullptr;

//	"globals" is introduced to reduce the number of parameters
//	for the dabProcessor
	globals. spectrumBuffer		= &spectrumBuffer;
	globals. iqBuffer		= &iqBuffer;
	globals. responseBuffer		= &responseBuffer;
	globals. tiiBuffer		= &tiiBuffer;
	globals. frameBuffer		= &frameBuffer;
	latency			=
	                  dabSettings -> value ("latency", 5). toInt();

	QString dabMode         =
	               dabSettings   -> value ("dabMode", "Mode 1"). toString();
	globals. dabMode	= convert (dabMode);
	globals. threshold		=
	                  dabSettings -> value ("threshold", 3). toInt();
	globals. diff_length	=
	           dabSettings	-> value ("diff_length", DIFF_LENGTH). toInt();
	globals. tii_delay   =
	           dabSettings  -> value ("tii_delay", 5). toInt();
	if (globals. tii_delay < 5)
	   globals. tii_delay = 5;
	globals. tii_depth       =
	               dabSettings -> value ("tii_depth", 1). toInt();
	globals. echo_depth      =
	               dabSettings -> value ("echo_depth", 1). toInt();

	currentService. valid	= false;
	nextService. valid	= false;
	bool has_presetName	=
	              dabSettings -> value ("has-presetName", 1). toInt() != 0;
	if (has_presetName) {
	   presetName		=
	              dabSettings -> value ("presetname", ""). toString();
	   if (presetName != "") {
	      nextService. serviceName	= presetName;
	      nextService. SId		= 0;
	      nextService. SCIds	= 0;
	      nextService. valid	= true;
	   }
	}
#ifdef	_SEND_DATAGRAM_
	ipAddress		= dabSettings -> value ("ipAddress", "127.0.0.1"). toString();
	port			= dabSettings -> value ("port", 8888). toInt();
#endif
//
	saveSlides	= dabSettings -> value ("saveSlides", 1). toInt();
	if (saveSlides != 0)
	   set_picturePath ();

	epgPath		= dabSettings -> value ("epgPath", ""). toString ();
	filePath	= dabSettings -> value ("filePath", ""). toString ();
	if ((epgPath != "") && (!epgPath. endsWith ("/")))
	   epgPath = epgPath + "/";
	if ((filePath != "") && (!filePath. endsWith ("/")))
	   filePath = filePath + "/";
/*
 * Experimental:
 *	lots of people seem to want the scan to continue, rather than
 *	stop whever a channel with data is found.
 */
	setupUi (this);

	dataDisplay	= new QFrame (nullptr);
	techData. setupUi (dataDisplay);
	techData. timeTable_button	-> hide ();

	configDisplay	= new QFrame (nullptr);
	configWidget. setupUi (configDisplay);

	int x = dabSettings -> value ("switchDelay", 8). toInt ();
	configWidget. switchDelaySetting -> setValue (x);

	x = dabSettings -> value ("muteTime", 2). toInt ();
	configWidget. muteTimeSetting -> setValue (x);

	x = dabSettings -> value ("serviceOrder", ALPHA_BASED). toInt ();
	if (x == ALPHA_BASED)
	   configWidget. orderAlfabetical -> setChecked (true);
	else
	if (x == ID_BASED)
	   configWidget. orderServiceIds -> setChecked (true);
	else
	   configWidget. ordersubChannelIds -> setChecked (true);
	motSlides		= nullptr;
	dataDisplay		-> hide ();
	stillMuting		-> hide ();
	serviceList. clear ();
	model . clear ();
	ensembleDisplay         -> setModel (&model);

/*
 */
//	scanMode is - unfortunately - global
	scanMode        =
	           dabSettings -> value ("scanMode", SINGLE_SCAN). toInt ();
	configWidget. scanmodeSelector -> setCurrentIndex (scanMode);

	bool motselectionMode =
	           dabSettings -> value ("motSlides", 0). toInt () == 1;
	if (motselectionMode)
	   configWidget. motslideSelector -> setChecked (true);

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
	QString historyFile     = QDir::homePath () + "/.qt-history.xml";
	historyFile             = dabSettings -> value ("history",
	                                            historyFile). toString ();
	historyFile             = QDir::toNativeSeparators (historyFile);
	my_history              = new historyHandler (this, historyFile);

	set_Colors ();
	connect (my_history, SIGNAL (handle_historySelect (const QString &)),
	         this, SLOT (handle_historySelect (const QString &)));
	connect (historyButton, SIGNAL (clicked ()),
	         this, SLOT (handle_historyButton ()));
	connect (this, SIGNAL (set_newChannel (int)),
	         channelSelector, SLOT (setCurrentIndex (int)));
	connect (this, SIGNAL (set_newPresetIndex (int)),
	         presetSelector, SLOT (setCurrentIndex (int)));


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
	frameDumper		= nullptr;
	ficBlocks		= 0;
	ficSuccess		= 0;
	syncedLabel		->
	               setStyleSheet ("QLabel {background-color : red; color: white}");
//
	connect (streamoutSelector, SIGNAL (activated (int)),
	         this,  SLOT (set_streamSelector (int)));
	my_presetHandler. loadPresets (presetFile, presetSelector);
//	
//	Connect the buttons for the color_settings
	connect (contentButton, SIGNAL (rightClicked (void)),
	         this, SLOT (color_contentButton (void)));
	connect (detailButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_detailButton ()));
	connect (resetButton, SIGNAL (rightClicked (void)),
	         this, SLOT (color_resetButton (void)));
	connect	(scanButton, SIGNAL (rightClicked (void)),
	         this, SLOT (color_scanButton (void)));
	connect (show_tiiButton, SIGNAL (rightClicked (void)),
	         this, SLOT (color_tiiButton (void)));
	connect (show_correlationButton, SIGNAL (rightClicked (void)),
	         this, SLOT (color_correlationButton (void)));
	connect (show_spectrumButton, SIGNAL (rightClicked (void)),
	         this, SLOT (color_spectrumButton (void)));
	connect (devicewidgetButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_devicewidgetButton ()));
	connect (historyButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_historyButton ()));
//	connect (dumpButton, SIGNAL (rightClicked (void)),
//	         this, SLOT (color_sourcedumpButton (void)));
	connect (muteButton, SIGNAL (rightClicked (void)),
	         this, SLOT (color_muteButton (void)));
	connect (dlTextButton, SIGNAL (clicked ()),
                 this, SLOT (handle_dlTextButton ()));

	connect (configButton, SIGNAL (rightClicked (void)),
	         this, SLOT (color_configButton (void)));
	connect (dlTextButton, SIGNAL (rightClicked (void)),
                 this, SLOT (color_dlTextButton (void)));

	connect	(prevChannelButton, SIGNAL (rightClicked (void)),
	         this, SLOT (color_prevChannelButton (void)));
	connect (nextChannelButton, SIGNAL (rightClicked (void)),
	         this, SLOT (color_nextChannelButton (void)));
	connect (prevServiceButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_prevServiceButton ()));
	connect (nextServiceButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_nextServiceButton ()));

	connect (techData. framedumpButton, SIGNAL (rightClicked (void)),
	         this, SLOT (color_framedumpButton (void)));
	connect (techData. audiodumpButton, SIGNAL (rightClicked (void)),
	         this, SLOT (color_audiodumpButton (void)));
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
	channelTimer. setSingleShot (true);
	channelTimer. setInterval (10000);
	connect (&channelTimer, SIGNAL (timeout (void)),
	         this, SLOT (No_Signal_Found (void)));
//
//	presetTimer
	presetTimer. setSingleShot (true);
	connect (&presetTimer, SIGNAL (timeout (void)),
	         this, SLOT (setPresetStation (void)));

//      timer for muting
	muteTimer. setSingleShot (true);
	muting          = false;

	currentServiceDescriptor	= nullptr;

//	It is time for some action
	my_dabProcessor = new dabProcessor   (this, &globals);
//	The input device talks to the dabProcessor
	inputDevice     = nullptr;

	deviceSelector	-> addItem ("xml files");
#ifdef	HAVE_SDRPLAY_V2
	deviceSelector	-> addItem ("sdrplay");
#endif
#ifdef	HAVE_SDRPLAY_V3
	deviceSelector	-> addItem ("sdrplay-v3");
#endif
#ifdef	HAVE_PLUTO
	deviceSelector	-> addItem ("pluto");
#endif
#ifdef	HAVE_RTLSDR
	deviceSelector	-> addItem ("rtlsdr");
#endif
#ifdef	HAVE_AIRSPY
	deviceSelector	-> addItem ("airspy");
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

	h               =
	           dabSettings -> value ("device", "no device"). toString();
	k               = deviceSelector -> findText (h);
//	fprintf (stderr, "%d %s\n", k, h. toUtf8(). data());
	if (k != -1) {
	   deviceSelector       -> setCurrentIndex (k);
	   inputDevice	= setDevice (deviceSelector -> currentText());
	}

	if (dabSettings -> value ("spectrumVisible", 0). toInt () == 1)
	   my_spectrumViewer. show ();
	if (dabSettings -> value ("tiiVisible", 0). toInt () == 1)
	   my_tiiViewer. show ();
	if (dabSettings -> value ("correlationVisible", 0). toInt () == 1)
	   my_correlationViewer. show ();

//	if a device was selected, we just start, otherwise
//	we wait until one is selected
	if (inputDevice != nullptr) {
	   if (doStart ()) {
	      qApp	-> installEventFilter (this);
	      return;
	   }
	   else {
	      delete inputDevice;
	      inputDevice	= nullptr;
	   }
	}

	connect (deviceSelector, SIGNAL (activated (const QString &)),
	         this,  SLOT (doStart (const QString &)));
	qApp	-> installEventFilter (this);
}

//	doStart (QString) is called when - on startup - no device
//	was registered to be used, and the user presses the
//	selectDevice comboBox
void	RadioInterface::doStart (const QString &dev) {
	(void)dev;
	inputDevice = setDevice	(dev);
	if (inputDevice == nullptr) {
	   disconnectGUI ();
	   return;
	}
	doStart ();
}
//
//	when doStart is called, a device is available and selected
bool	RadioInterface::doStart	() {

	QString h       = dabSettings -> value ("channel", "12C"). toString();
	int k           = channelSelector -> findText (h);
	if (k != -1) 
	   channelSelector -> setCurrentIndex (k);

//	Some buttons should not be touched before we have a device
	connectGUI ();

//	we avoided up till now connecting the channel selector
//	to the slot since that function does a lot more that we
//	do not want here
	connect (presetSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (handle_presetSelector (const QString &)));
	connect (channelSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (selectChannel (const QString &)));
//
//	Just to be sure we disconnect here.
//	It would have been helpful to have a function
//	testing whether or not a connection exists, we need a kind
//	of "reset"
	disconnect (deviceSelector, SIGNAL (activated (const QString &)),
	            this,  SLOT (doStart (const QString &)));
	disconnect (deviceSelector, SIGNAL (activated (const QString &)),
	            this,  SLOT (newDevice (const QString &)));

	connect (deviceSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (newDevice (const QString &)));
//
	secondariesVector. resize (0);
	if (nextService. valid) {
	   int switchDelay		=
	              dabSettings -> value ("switchDelay", 8). toInt ();
	   presetTimer. setSingleShot	(true);
	   presetTimer. setInterval 	(switchDelay * 1000);
	   presetTimer. start 		(switchDelay * 1000);
	}

	bool dm = dabSettings -> value ("tii_detector", 0). toInt () == 1;
        if (dm)
           configWidget. tii_detectorMode -> setChecked (true);
        my_dabProcessor -> set_tiiDetectorMode (dm);
        connect (configWidget. tii_detectorMode, SIGNAL (stateChanged (int)),
                    this, SLOT (handle_tii_detectorMode (int)));

	startChannel (channelSelector -> currentText ());
	running. store (true);
	return true;
}

QString RadioInterface::footText () {
	version			= QString (CURRENT_VERSION);
	QString versionText = "dab-2- version: " + QString(CURRENT_VERSION);
	versionText += "Copyright J van Katwijk, J. vanKatwijk@gmail.com\n";
	versionText += "Rights of Qt, fftw, portaudio, libsamplerate and libsndfile gratefully acknowledged";
	versionText += "Rights of other contribuants gratefully acknowledged\n";
	versionText += " Build on: " + QString(__TIMESTAMP__) + QString (" ") + QString (GITHASH);
	return versionText;
}

	RadioInterface::~RadioInterface() {
	fprintf (stderr, "radioInterface is deleted\n");
}
//
void	RadioInterface::dumpControlState (QSettings *s) {
	if (s == nullptr)	// cannot happen
	   return;
	QString presetName;
	
	if (currentService. valid) {
	   s	-> setValue ("presetname", currentService. serviceName);
	}
	else
	   s	-> setValue ("presetname", "");


	s	-> setValue ("device",
	                     deviceSelector -> currentText ());
	s	-> setValue ("channel",
	                      channelSelector -> currentText());
	s	-> setValue ("soundchannel",
	                               streamoutSelector -> currentText());
	if (inputDevice != nullptr)
	   s    -> setValue ("devicewidgetButton",
	                          inputDevice -> isHidden () != 0);
	s       -> setValue ("spectrumVisible",
	                          my_spectrumViewer. isHidden () ? 0 : 1);
	s       -> setValue ("tiiVisible",
	                          my_tiiViewer. isHidden () ? 0 : 1);
	s       -> setValue ("correlationVisible",
	                          my_correlationViewer. isHidden () ? 0 : 1);
	s	-> sync();
}

//
//	precondition: everything is quiet
deviceHandler	*RadioInterface::setDevice (const QString &s) {
QString	file;
deviceHandler	*inputDevice	= nullptr;
//	OK, everything quiet, now let us see what to do

#ifdef	HAVE_AIRSPY
	if (s == "airspy") {
	   try {
	      inputDevice	= new airspyHandler (this,
	                                             dabSettings,
	                                             my_dabProcessor,
	                                             version);
	      showButtons();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                               tr ("airspy: no library or device\n"));
	      return nullptr;
	   }
	}
	else
#endif
#ifdef	HAVE_HACKRF
	if (s == "hackrf") {
	   try {
	      QString recorder = "dab-2";
	      inputDevice	= new hackrfHandler (this,
	                                             dabSettings,
	                                             my_dabProcessor,
	                                             recorder);
	      showButtons();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                               tr ("hackrf: no library or device\n"));
	      return nullptr;
	   }
	}
	else
#endif
#ifdef  HAVE_SOAPY
	if (s == "soapy") {
	   try {
	      inputDevice       = new soapyHandler (this,
	                                            dabSettings,
	                                            my_dabProcessor);
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
#ifdef	HAVE_LIMESDR
	if (s == "limeSDR") {
	   try {
	      inputDevice	= new limeHandler (this,
	                                           dabSettings,
	                                           my_dabProcessor,
	                                           version);
	      showButtons();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                               tr ("limeSDR: no library or device\n"));
	      return nullptr;
	   }
	}
	else
#endif
#ifdef	HAVE_PLUTO
	if (s == "pluto") {
	   try {
	      QString recorder = "dab-2";
	      inputDevice	= new plutoHandler (this,
	                                            dabSettings,
	                                            my_dabProcessor,
	                                            recorder);
	      showButtons ();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                               tr ("pluto: no library or device\n"));
	      return nullptr;
	   }
	}
	else
#endif
#ifdef HAVE_EXTIO
//      extio is - in its current settings - for Windows, it is a
//      wrap around the dll
	if (s == "extio") {
	   try {
	      inputDevice = new extioHandler (this,
	                                      dabSettings,
	                                      my_dabProcessor);
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
//      RTL_TCP might be working.
	if (s == "rtl_tcp") {
	   try {
	      inputDevice = new rtl_tcp_client (this,
	                                        dabSettings,
	                                        my_dabProcessor);
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
	      inputDevice	= new sdrplayHandler (this,
	                                              dabSettings,
	                                              my_dabProcessor,
	                                              version);
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
	      inputDevice	= new sdrplayHandler_v3 (this,
	                                                 dabSettings, 
	                                                 my_dabProcessor,
	                                                 version);
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
#ifdef	HAVE_RTLSDR
	if (s == "rtlsdr") {
	   try {
	      QString version = " ";
	      inputDevice	= new rtlsdrHandler (this,
	                                             dabSettings,
	                                             my_dabProcessor,
	                                             version);
	      showButtons();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                           tr ("DAB stick not found! Please use one with RTL2832U or similar chipset!\n"));
	      fprintf (stderr, "error = %d\n", e);
	      return nullptr;
	   }
	}
#endif
	if (s == "xml files") {
	   QString file	= QFileDialog::getOpenFileName (this,
	                                          tr ("Open file ..."),
	                                           QDir::homePath(),
	                                           tr ("xml data (*.*)"));
	   if (file == QString (""))
	      return nullptr;
	   file         = QDir::toNativeSeparators (file);

	   try {
	      inputDevice	= new xml_fileReader (this,
	                                              my_dabProcessor,
	                                              file);
	   } catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                            tr ("File input failed\n"));
	   }
	}
	if (s == "file input (.raw)") {
	   QString file	= QFileDialog::getOpenFileName (this,
	                                          tr ("Open file ..."),
	                                           QDir::homePath(),
	                                           tr ("raw data (*.raw)"));
	   if (file == QString (""))
	      return nullptr;
	   file         = QDir::toNativeSeparators (file);

	   try {
	      inputDevice	= new rawFiles (file, my_dabProcessor);
	   } catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                            tr ("File input failed\n"));
	   }
	}

	if (s == "file input (.iq)") {
	   QString file	= QFileDialog::getOpenFileName (this,
	                                          tr ("Open file ..."),
	                                           QDir::homePath(),
	                                           tr ("raw data (*.iq)"));
	   if (file == QString (""))
	      return nullptr;
	   file         = QDir::toNativeSeparators (file);

	   try {
	      inputDevice	= new rawFiles (file, my_dabProcessor);
	   } catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                            tr ("File input failed\n"));
	   }
	}

	if (s == "file input (.sdr)") {
	   QString file	= QFileDialog::getOpenFileName (this,
	                                          tr ("Open file ..."),
	                                           QDir::homePath(),
	                                           tr ("raw data (*.sdr)"));
	   if (file == QString (""))
	      return nullptr;
	   file         = QDir::toNativeSeparators (file);

	   try {
	      inputDevice	= new wavFiles (file, my_dabProcessor);
	   } catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                            tr ("File input failed\n"));
	   }
	}
	if (inputDevice == nullptr)
	   return nullptr;

	connect (my_dabProcessor, SIGNAL (set_Values (int, float, float)),
	         inputDevice, SLOT (set_Values (int, float, float)));

	my_spectrumViewer. setBitDepth (inputDevice -> bitDepth());

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
	stopChannel	();
	disconnectGUI	();
	delete inputDevice;
	fprintf (stderr, "device is deleted\n");
	fprintf (stderr, "going for a device %s\n", deviceName. toLatin1 (). data ());
	inputDevice		= setDevice (deviceName);
	if (inputDevice == nullptr) {
	   inputDevice = new deviceHandler ();
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
void	RadioInterface::channel_timeOut () {
	No_Signal_Found();
}

///////////////////////////////////////////////////////////////////////////
//
//	a slot, called by the fic/fib handlers
void	RadioInterface::addtoEnsemble (const QString &serviceName,
	                                             int32_t SId) {
	if (!running. load())
	   return;

	(void)SId;
	serviceId ed;
	ed. name = serviceName;
	ed. SId	= SId;

	if (isMember (serviceList, ed))
	   return;

	serviceOrder	=
	    dabSettings -> value ("serviceOrder", ALPHA_BASED). toInt ();
	if (serviceOrder	== SUBCH_BASED) {
	   audiodata ad;
	   my_dabProcessor	-> dataforAudioService (serviceName, &ad);
	   if (ad. defined)
	      ed. subChId	= ad. subchId;
	   else {
	      packetdata pd;
	      my_dabProcessor	-> dataforPacketService (serviceName, &pd, 0);
	      if (pd. defined)
	         ed. subChId	= pd. subchId;
	      else
	         ed. subChId	= 2000;
	   }
	}

	serviceList = insert (serviceList, ed, serviceOrder);
	my_history -> addElement (channelSelector -> currentText (),
	                                                       serviceName);
	model. clear ();
	for (const auto serv : serviceList)
	   model. appendRow (new QStandardItem (serv. name));
	int row = model. rowCount ();
	for (int i = 0; i < row; i ++) {
	   model. setData (model. index (i, 0),
	              QFont ("Cantarell", 11), Qt::FontRole);
	}

	ensembleDisplay -> setModel (&model);
}
//
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
	if (scanMode == SCAN_TO_DATA)
	   stopScanning (false);	// if scanning, we are done
}

//////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////

static inline
bool    isValid (QChar c) {
	return c. isLetter () || c. isDigit () || (c == '-');
}

void	RadioInterface::handle_contentButton	() {
ensemblePrinter	my_Printer;
QString	suggestedFileName;
QString currentChannel	= channelSelector -> currentText();
int32_t	frequency	= inputDevice -> getVFOFrequency();
QString saveDir         = dabSettings -> value ("contentDir",
	                                        QDir::homePath ()). toString ();

	if (!running. load() || (ensembleId -> text () == QString ("")))
	   return;

	if (scanning. load ())
	   return;
	FILE	*fileP	= findContentDump_fileName (currentChannel);
	if (fileP == nullptr)
	   return;

	my_Printer. showEnsembleData (currentChannel,
	                              frequency, 
	                              localTimeDisplay -> text (),
	                              transmitters,
	                              serviceList,
	                              my_dabProcessor, fileP);
	fclose (fileP);
}

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

void	RadioInterface::handle_motObject (QByteArray result,
	                                  QString name,
	                                  int contentType, bool dirElement) {
QString realName;

	fprintf (stderr, "handle_MOT: type %x, name %s dir = %d\n",
	                           contentType,
	                           name. toLatin1 (). data (), dirElement);
	switch (getContentBaseType ((MOTContentType)contentType)) {
	   case MOTBaseTypeGeneralData:
	      break;

	   case MOTBaseTypeText:
	      save_MOTtext (result, contentType, name);
	      break;

	   case MOTBaseTypeImage:
	      show_MOTlabel (result, contentType, name);
	      break;

	   case MOTBaseTypeAudio:
	      break;

	   case MOTBaseTypeVideo:
	      break;

	   case MOTBaseTypeTransport:
	      break;

	   case MOTBaseTypeSystem:
	      break;

	   case  MOTBaseTypeApplication: 	// epg data
#ifdef	TRY_EPG
	      if (epgPath == "")
	         return;
	      if (name == QString (""))
	         name = "epg file";
	      realName = epgPath + name;
	      realName  = QDir::toNativeSeparators (realName);
	      checkDir (realName);
	      {  std::vector<uint8_t> epgData (result. begin(),
	                                                  result. end());
	         epgHandler. decode (epgData, realName);
	      }
	      fprintf (stderr, "epg file %s\n", realName. toLatin1 (). data ());
#endif
	      return;

	   case MOTBaseTypeProprietary:
	      break;
	}
}

void	RadioInterface::save_MOTtext (QByteArray result,
	                              int contentType,  QString name) {
	if (filePath == "")
	   return;

QString textName = QDir::toNativeSeparators (filePath + name);

	(void)contentType;
	checkDir (textName);
	FILE *x = fopen (textName. toLatin1 (). data (), "w+b");
	if (x == nullptr)
	   fprintf (stderr, "cannot write file %s\n",
	                            textName. toLatin1 (). data ());
	else {
	   fprintf (stderr, "going to write file %s\n",
	                            textName. toLatin1(). data());
	   (void)fwrite (result. data (), 1, result.length(), x);
	   fclose (x);
	}
}


//	MOT slide, to show
void	RadioInterface::show_MOTlabel	(QByteArray data,
	                                 int contentType,
	                                 QString pictureName) {
const char *type;
	if (!running. load() || (pictureName == QString ("")))
	   return;

	switch (static_cast<MOTContentType>(contentType)) {
	   case MOTCTImageGIF:
	      type = "GIF";
	      break;

	   case MOTCTImageJFIF:
	      type = "JPG";
	      break;

	   case MOTCTImageBMP:
	      type = "BMP";
	      break;

	   case MOTCTImagePNG:
	      type = "PNG";
	      break;

	   default:
	        return;
	}

	QPixmap p;
	p. loadFromData (data, type);

	if (saveSlides) {
	   QString pict = QDir::toNativeSeparators (picturesPath + pictureName);
	   checkDir (pict);
	   FILE *x = fopen (pict. toLatin1 (). data (), "w+b");
	   if (x == nullptr)
	      fprintf (stderr, "cannot write file %s\n",
	                            pict. toLatin1 (). data ());
	   else {
	      fprintf (stderr, "going to write file %s\n",
	                            pict. toLatin1(). data());
	      (void)fwrite (data. data(), 1, data.length(), x);
	      fclose (x);
	   }
	}

	QString s = serviceLabel -> text ();
	if (!my_dabProcessor -> is_audioService (s))
	   return;

	bool b = dabSettings -> value ("motSlides", 0). toInt () == 1;
	if (!b) {
	   if (motSlides != nullptr) {
	      delete motSlides;
	      motSlides = nullptr;
	   }

	   int w   = techData. pictureLabel -> width ();
	   int h   = 2 * w / 3;
	   techData. pictureLabel ->
	          setPixmap (p. scaled (w, h, Qt::KeepAspectRatio));
	   techData. pictureLabel -> show ();
	}
	else {
	   if (motSlides == nullptr)
	      motSlides = new QLabel (nullptr);
	   motSlides	-> setPixmap (p);
	   motSlides	-> show ();
	}
}
//
//	sendDatagram is triggered by the ip handler,
void	RadioInterface::sendDatagram	(int length) {
uint8_t localBuffer [length];
	if (dataBuffer. GetRingBufferReadAvailable() < length) {
	   fprintf (stderr, "Something went wrong\n");
	   return;
	}
	dataBuffer. getDataFromBuffer (localBuffer, length);
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
	if (dataBuffer. GetRingBufferReadAvailable() < length) {
	   fprintf (stderr, "Something went wrong\n");
	   return;
	}
#ifdef	DATA_STREAMER
	dataBuffer. getDataFromBuffer (&localBuffer [8], length);
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
	   if (currentService. valid) 
	      s = currentService;
	   stopScanning    (false);
	   stopService     ();
	   fprintf (stderr, "change detected\n");
//
//	we rebuild the services list from the fib and
//	then we (try to) restart the service
	   serviceList	= my_dabProcessor -> getServices (serviceOrder);
	   model. clear	();
	   for (const auto serv : serviceList)
	      model. appendRow (new QStandardItem (serv. name));
	   int row = model. rowCount ();
	   for (int i = 0; i < row; i ++) {
	      model. setData (model. index (i, 0),
	      QFont ("Cantarell", 11), Qt::FontRole);
	   }
	   ensembleDisplay -> setModel (&model);
//
//	and restart the one that was running
	   if (s. valid) {
	      if (s. SCIds != 0) { // secondary service may be gone
	         if (my_dabProcessor -> findService (s. SId, s. SCIds) ==
	                                                   s. serviceName) {
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
	   while (audioBuffer. GetRingBufferReadAvailable() > amount) {
	      audioBuffer. getDataFromBuffer (vec, amount);
	      if (!muting)
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
	channelTimer.  stop();
	presetTimer.  stop();
	dumpControlState (dabSettings);
	soundOut		-> stop();
	if (inputDevice != nullptr) 
	   inputDevice		-> stopReader ();	// might be concurrent
	if (my_dabProcessor != nullptr)
	   my_dabProcessor	-> stop();		

	my_presetHandler. savePresets (presetSelector);
	stop_frameDumping		();
	stop_audioDumping		();
	theTable. hide ();
	dataDisplay	-> hide ();

	if (motSlides != nullptr)
	   motSlides	-> hide ();

	my_spectrumViewer. hide ();
	my_correlationViewer. hide ();
	my_tiiViewer. hide ();
	if (inputDevice != nullptr)
	   delete	inputDevice;
	fprintf (stderr, "going to delete dabProcessor\n");
	if (my_dabProcessor != nullptr)
	   delete	my_dabProcessor;

	delete		soundOut;
	if (motSlides != nullptr) {
	   delete motSlides;
	   motSlides	= nullptr;
	}
	if (currentServiceDescriptor != nullptr)
	   delete currentServiceDescriptor;
	delete		dataDisplay;
	delete		configDisplay;
	delete		my_history;
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

///////////////////////////////////////////////////////////////////////////
//	signals, received from ofdm_decoder that data is
//	to be displayed
///////////////////////////////////////////////////////////////////////////

static
const char *monthTable [] = {
	"jan", "feb", "mar", "apr", "may", "jun",
	"jul", "aug", "sep", "oct", "nov", "dec"
};
//
//	called from the fibDecoder
void	RadioInterface::clockTime (int year, int month, int day,
	                           int hours, int minutes,
	                               int d2, int h2, int m2, int seconds){
	this	-> localTime. year	= year;
	this	-> localTime. month	= month;
	this	-> localTime. day	= day;
	this	-> localTime. hour	= hours;
	this	-> localTime. minute	= minutes;
	this	-> localTime. second	= seconds;

	this	-> UTC. year		= year;
	this	-> UTC. month		= month;
	this	-> UTC. day		= d2;
	this	-> UTC. hour		= h2;
	this	-> UTC. minute		= m2;
	QString result	= convertTime (year, month, day, hours, minutes);
	localTimeDisplay -> setText (result);
}

QString	RadioInterface::convertTime (int year, int month,
	                             int day, int hours, int minutes) {
char dayString [3];
char hourString [3];
char minuteString [3];
	sprintf (dayString, "%.2d", day);
	sprintf (hourString, "%.2d", hours);
	sprintf (minuteString, "%.2d", minutes);
	QString result = QString::number (year) + "-" +
	                       monthTable [month - 1] + "-" +
	                       QString (dayString) + "  " +
	                       QString (hourString) + ":" +
	                       QString (minuteString);
	return result;
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
void	RadioInterface::show_snr (float snr) {
	if (running. load ()) {
	   snrDisplay	-> display (snr);
	}
}

//	just switch a color, called from the ofdmprocessor
void	RadioInterface::setSynced	(bool b) {
	if (!running. load () || (isSynced == b))
	   return;

	isSynced = b;
	if (isSynced) 
	      syncedLabel -> 
	               setStyleSheet ("QLabel {background-color : green; color: white}");
	else
	      syncedLabel ->
	               setStyleSheet ("QLabel {background-color : red}");
}

//	called from the PAD handler

void	RadioInterface::showLabel	(QString s) {
	if (running. load())
	   dynamicLabel	-> setText (s);
	if (dlTextFile == nullptr)
	   return;
	if (the_dlCache. addifNew (s))
	   return;
	QString currentChannel = channelSelector -> currentText ();
	QDateTime theDateTime	= QDateTime::currentDateTime ();
	fprintf (dlTextFile, "%s.%s %4d-%02d-%02d %02d:%02d:%02d  %s\n",
	                          currentChannel. toUtf8 (). data (),
	                          currentService. serviceName.
	                                          toUtf8 (). data (),
	                          localTime. year,
	                          localTime. month,
	                          localTime. day,
	                          localTime. hour,
	                          localTime. minute,
	                          localTime. second,
	                                     s. toUtf8 (). data ());
}

void	RadioInterface::setStereo	(bool s) {
	if (!running. load ())
	   return;
	if (stereoSetting == s)
	   return;

	techData. stereoLabel   -> setStyleSheet (s ?
	                 "QLabel {background-color: green; color : white}":
	                 "QLabel {background-color: red; color : white");
	techData. stereoLabel   -> setText (s ? "stereo" : "mono");
	stereoSetting = s;
}

static
QString tiiNumber (int n) {
	if (n >= 10)
	   return QString::number (n);
	return QString ("0") + QString::number (n);
}

void	RadioInterface::show_tii (int mainId, int subId) {
QString a = "Est: ";
bool	found	= false;

	for (int i = 0; i < transmitters. size (); i += 2) {
	   if ((transmitters. at (i) == mainId) &&
	       (transmitters. at (i + 1) == subId)) {
	      found = true;
	      break;
	   }
	}

	if (!found) {
	   transmitters. append (mainId);
	   transmitters. append (subId);
	}
	if (!running. load())
	   return;

	a = a + " " +  tiiNumber (mainId) + " " + tiiNumber (subId);

	transmitter_coordinates	-> setAlignment (Qt::AlignRight);
	transmitter_coordinates	-> setText (a);
	my_tiiViewer. showTransmitters (transmitters);
	my_tiiViewer. showSpectrum (1);
	if (!running. load())
	   return;
	
}

void	RadioInterface::showSpectrum	(int32_t amount) {
	if (running. load())
	   my_spectrumViewer. showSpectrum (amount,
				              inputDevice -> getVFOFrequency());
}

void	RadioInterface::showIQ	(int amount) {
	if (running. load())
	   my_spectrumViewer. showIQ (amount);
}

void	RadioInterface::showQuality	(float q) {
	if (running. load())
	   my_spectrumViewer. showQuality (q);
}

void	RadioInterface::show_rsCorrections (int e) {
	if (running. load())
	   techData. rsCorrections		-> display (e);
}

void    RadioInterface::show_clockError (int e) {
	if (!running. load ())
	   return;
	my_spectrumViewer. show_clockErr (e);
}

void	RadioInterface::showCorrelation	(int amount, int marker) {
	if (!running. load())
	   return;
	my_correlationViewer. showCorrelation (amount, marker);
}

void	RadioInterface::showIndex (int ind) {
	if (!running. load())
	   return;

	my_correlationViewer. showIndex (ind);
}
//
/////////////////////////////////////////////////////////////////////////
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
	nextChannelButton	-> show ();
	prevChannelButton	-> show ();
	frequencyDisplay	-> show ();
	presetSelector		-> show ();
}

void	RadioInterface::hideButtons() {
	scanButton		-> hide ();
	channelSelector		-> hide ();
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
//      dump handling
//
/////////////////////////////////////////////////////////////////////////

void    setButtonFont (QPushButton *b, QString text, int size) {
	QFont font      = b -> font ();
	font. setPointSize (size);
	b               -> setFont (font);
	b               -> setText (text);
	b               -> update ();
}


void	RadioInterface::handle_sourcedumpButton	() {
	QMessageBox::warning (this, tr ("Warning"),
	                            tr ("raw dump not implemented\nuse xml dump instead\n"));
}

void	RadioInterface::stop_audioDumping	() {
	if (audioDumper == nullptr)
	   return;

	soundOut	-> stopDumping();
	sf_close (audioDumper);
	audioDumper = nullptr;
	setButtonFont (techData. audiodumpButton, "audio dump", 10);
}

void	RadioInterface::start_audioDumping () {
audioDumper     = findAudioDump_fileName  (serviceLabel -> text (),
	                                           localTimeDisplay -> text ());
	if (audioDumper == nullptr)
	   return;

	setButtonFont (techData. audiodumpButton, "writing", 12);

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
	setButtonFont (techData. framedumpButton, "frame dump", 10);
	frameDumper	= nullptr;
}

void	RadioInterface::start_frameDumping () {
	frameDumper     = findFrameDump_fileName (serviceLabel -> text (),
	                                          localTimeDisplay -> text ());
	if (frameDumper == nullptr)
	   return;
	setButtonFont (techData. framedumpButton, "recording", 12);
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
	   globals. frameBuffer	-> FlushRingBuffer ();
	else
	while (globals. frameBuffer -> GetRingBufferReadAvailable () >= amount) {
	   globals. frameBuffer     -> getDataFromBuffer (buffer, amount);
	   if (frameDumper != nullptr)
	      fwrite (buffer, amount, 1, frameDumper);
	}
}


void	RadioInterface::handle_tiiButton	() {
	if (my_tiiViewer. isHidden())
	   my_tiiViewer. show();
	else
	   my_tiiViewer. hide();
}

void	RadioInterface::handle_correlationButton	() {
	if (my_correlationViewer. isHidden())
	   my_correlationViewer. show();
	else
	   my_correlationViewer. hide();
}

void	RadioInterface::handle_spectrumButton	() {
	if (my_spectrumViewer. isHidden())
	   my_spectrumViewer. show();
	else
	   my_spectrumViewer. hide();
}

void    RadioInterface::handle_historyButton    () {
	if (my_history -> isHidden ())
	   my_history -> show ();
	else
	   my_history -> hide ();
}
//
////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////
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
//	connect (dumpButton, SIGNAL (clicked (void)),
//	         this, SLOT (handle_sourcedumpButton (void)));
	connect (techData. audiodumpButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_audiodumpButton (void)));
	connect (techData. framedumpButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_framedumpButton (void)));
	connect (dumpButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_sourcedumpButton (void)));
	connect (show_tiiButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_tiiButton (void)));
	connect (show_correlationButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_correlationButton (void)));
	connect (show_spectrumButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_spectrumButton (void)));
	connect (muteButton, SIGNAL (clicked ()),
	         this, SLOT (handle_muteButton ()));
	connect (configButton, SIGNAL (clicked ()),
	         this, SLOT (handle_configSetting ()));
	connect (configWidget. muteTimeSetting, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_muteTimeSetting (int)));
	connect (configWidget. switchDelaySetting,
	                                 SIGNAL (valueChanged (int)),
	         this, SLOT (handle_switchDelaySetting (int)));
	connect (configWidget. orderAlfabetical, SIGNAL (clicked ()),
	         this, SLOT (handle_orderAlfabetical ()));
	connect (configWidget. orderServiceIds, SIGNAL (clicked ()),
	         this, SLOT (handle_orderServiceIds ()));
	connect (configWidget. ordersubChannelIds, SIGNAL (clicked ()),
	         this, SLOT (handle_ordersubChannelIds ()));
	connect (configWidget. scheduleSelector, SIGNAL (clicked ()),
	         this, SLOT (handle_scheduleSelector ()));
	connect (configWidget. plotLengthSetting, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_plotLengthSetting (int)));
	connect (configWidget. scanmodeSelector,
	                           SIGNAL (currentIndexChanged (int)),
	         this, SLOT (handle_scanmodeSelector (int)));
	connect (configWidget. motslideSelector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_motslideSelector (int)));
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
//	disconnect (dumpButton, SIGNAL (clicked (void)),
//	            this, SLOT (handle_sourcedumpButton (void)));
	disconnect (techData. audiodumpButton, SIGNAL (clicked (void)),
	            this, SLOT (handle_audiodumpButton (void)));
	disconnect (techData. framedumpButton, SIGNAL (clicked (void)),
	            this, SLOT (handle_framedumpButton (void)));
	disconnect (dumpButton, SIGNAL (clicked ()),
	            this, SLOT (handle_sourcedumpButton ()));
	disconnect (show_tiiButton, SIGNAL (clicked (void)),
	            this, SLOT (handle_tiiButton (void)));
	disconnect (show_correlationButton, SIGNAL (clicked (void)),
	            this, SLOT (handle_correlationButton (void)));
	disconnect (show_spectrumButton, SIGNAL (clicked (void)),
	            this, SLOT (handle_spectrumButton (void)));
	disconnect (muteButton, SIGNAL (clicked ()),
	            this, SLOT (handle_muteButton ()));
	disconnect (configButton, SIGNAL (clicked ()),
	            this, SLOT (handle_configSetting ()));
	disconnect (configWidget. muteTimeSetting,
	                                    SIGNAL (valueChanged (int)),
	            this, SLOT (handle_muteTimeSetting (int)));
	disconnect (configWidget. switchDelaySetting,
	                                    SIGNAL (valueChanged (int)),
	            this, SLOT (handle_switchDelaySetting (int)));
	disconnect (configWidget. orderAlfabetical, SIGNAL (clicked ()),
	            this, SLOT (handle_orderAlfabetical ()));
	disconnect (configWidget. orderServiceIds, SIGNAL (clicked ()),
	            this, SLOT (handle_orderServiceIds ()));
	disconnect (configWidget. ordersubChannelIds, SIGNAL (clicked ()),
	            this, SLOT (handle_ordersubChannelIds ()));
	disconnect (configWidget. scheduleSelector, SIGNAL (clicked ()),
	            this, SLOT (handle_scheduleSelector ()));

	disconnect (configWidget. plotLengthSetting,
	                           SIGNAL (valueChanged (int)),
	            this, SLOT (handle_plotLengthSetting (int)));
	disconnect (configWidget. scanmodeSelector,
	                           SIGNAL (currentIndexChanged (int)),
	            this, SLOT (handle_scanmodeSelector (int)));
	disconnect (configWidget. motslideSelector, SIGNAL (stateChanged (int)),
	            this, SLOT (handle_motslideSelector (int)));
}
//
//
#include <QCloseEvent>
void RadioInterface::closeEvent (QCloseEvent *event) {
	int x = configWidget. closeDirect -> isChecked () ? 1 : 0;
	dabSettings -> setValue ("closeDirect", x);
	if (x != 0) {
	   TerminateProcess ();
	   event -> accept ();
	   return;
	}



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
	if (!running. load ())
	   return QWidget::eventFilter (obj, event);
	if (event -> type () == QEvent::KeyPress) {
	   QKeyEvent *ke = static_cast <QKeyEvent *> (event);
	   if (ke -> key () == Qt::Key_Return) {
	      QString serviceName =
	         ensembleDisplay -> currentIndex ().
	                             data (Qt::DisplayRole). toString ();
	      if (currentService. serviceName != serviceName) {
	         fprintf (stderr, "currentservice = %s (%d)\n",
	                  currentService. serviceName. toLatin1 (). data (),
	                                currentService. valid);
	         stopService ();
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
	         if (currentServiceDescriptor != nullptr) 
	            delete currentServiceDescriptor;
	         currentServiceDescriptor	= new audioDescriptor (&ad);
	         return true;
	      }

	      my_dabProcessor -> dataforPacketService (serviceName, &pd, 0);
	      if (pd. defined) {
	         if (currentServiceDescriptor != nullptr)
	            delete currentServiceDescriptor;
	         currentServiceDescriptor	= new dataDescriptor (&pd);
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

void    RadioInterface::handle_presetSelector (const QString &s) {
	presetTimer. stop ();
	if ((s == "Presets") || (presetSelector -> currentIndex () == 0))
	   return;
	localSelect (s);
}

void	RadioInterface::localSelect (const QString &s) {
int	switchDelay;
	QStringList list = s.split (":", QString::SkipEmptyParts);
	if (list. length () != 2)
	   return;
	QString channel = list. at (0);
	QString service	= list. at (1);
	stopScanning (false);
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
	nextService. valid = true;
	nextService. serviceName        = service;
	nextService. SId                = 0;
	nextService. SCIds              = 0;
	switchDelay			=
	                 dabSettings -> value ("switchDelay", 8). toInt ();
	presetTimer. setSingleShot (true);
	presetTimer. setInterval (switchDelay * 1000);
	presetTimer. start (switchDelay * 1000);
	startChannel    (channelSelector -> currentText ());
}

////////////////////////////////////////////////////////////////////////////
//
//	handling services: stop and start
///////////////////////////////////////////////////////////////////////////

void	RadioInterface::stopService	() {
	presetTimer. stop ();
	presetSelector	-> setCurrentIndex (0);
	channelTimer. stop ();
	if (currentService. valid) {
	   fprintf (stderr, "%s wordt gestopt\n",
	                    currentService. serviceName. toLatin1 (). data ());
	   my_dabProcessor -> reset_Services ();
	   usleep (1000);
	   soundOut	-> stop ();
	   QString serviceName = currentService. serviceName;
	   for (int i = 0; i < model. rowCount (); i ++) {
	      QString itemText =
	          model. index (i, 0). data (Qt::DisplayRole). toString ();
	      if (itemText == serviceName) {
	         colorService (model. index (i, 0), Qt::black, 11);
	         break;
	      }
	   }
	}
	currentService. valid = false;
	cleanScreen	();
}
//
void	RadioInterface::selectService (QModelIndex ind) {
QString	currentProgram = ind. data (Qt::DisplayRole). toString();
	presetTimer.	stop();
	presetSelector -> setCurrentIndex (0);
	channelTimer.	stop ();
	stopScanning	(false);
	stopService 	();		// if any

	dabService s;
	my_dabProcessor -> getParameters (currentProgram, &s. SId, &s. SCIds);
	if (s. SId == 0) {
	   QMessageBox::warning (this, tr ("Warning"),
 	                         tr ("insufficient data for this program\n"));	
	   return;
	}

	s. serviceName = currentProgram;
	startService (&s);
}
//
void	RadioInterface::startService (dabService *s) {
QString serviceName	= s -> serviceName;

	if (currentService. valid) {
	   fprintf (stderr, "Niet verwacht, service %s still valid\n",
	                    currentService. serviceName. toLatin1 (). data ());
	   stopService ();
	}

	currentService          = *s;
	currentService. valid   = false;

	if (motSlides != nullptr) {
	   delete motSlides;
	   motSlides = nullptr;
	}
	techData. pictureLabel -> hide ();

	int rowCount	= model. rowCount ();
	for (int i = 0; i < rowCount; i ++) {
	   QString itemText =
	           model. index (i, 0). data (Qt::DisplayRole). toString ();
	   if (itemText == serviceName) {
	      colorService (model. index (i, 0), Qt::red, 15);
	      serviceLabel	-> setStyleSheet ("QLabel {color : black}");
	      serviceLabel	-> setText (serviceName);
	      if (my_dabProcessor -> is_audioService (serviceName)) {
	         start_audioService (serviceName);
	         currentService. valid = true;
	      }
	      else
	      if (my_dabProcessor -> is_packetService (serviceName)) {
	         start_packetService (serviceName);
	         currentService. valid = true;
	      }
	      else
	         fprintf (stderr, "%s: not clear what service is\n",
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
	if (motSlides != nullptr) {
	   delete motSlides;
	   motSlides = nullptr;
	}
	else
	   techData. pictureLabel -> hide ();

	new_presetIndex (0);
	techData. stereoLabel   -> setStyleSheet (
	                 "QLabel {background-color: white; color : black}");
	techData. stereoLabel		-> setText ("");
	stereoSetting                   = false;

	techData. rsCorrections		-> display (0);
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

	my_dabProcessor -> set_audioChannel (&ad, &audioBuffer);
	for (int i = 1; i < 10; i ++) {
	   packetdata pd;
	   my_dabProcessor -> dataforPacketService (serviceName, &pd, i);
	   if (pd. defined) {
	      my_dabProcessor -> set_dataChannel (&pd, &dataBuffer);
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

	my_dabProcessor -> set_dataChannel (&pd, &dataBuffer);
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
	presetTimer. stop ();
	nextService. valid	= false;
	if (!currentService. valid)
	   return;
	QString oldService	= currentService. serviceName;
	disconnect (prevServiceButton, SIGNAL (clicked ()),
	            this, SLOT (handle_prevServiceButton ()));
	stopScanning (false);
	stopService  ();
	if ((serviceList. size () != 0) && (oldService != "")) {
	   for (int i = 0; i < (int)(serviceList. size ()); i ++) {
	      if (serviceList. at (i). name == oldService) {
	         colorService (model. index (i, 0), Qt::black, 11);
	         i = i - 1;
	         if (i < 0)
	            i = serviceList. size () - 1;
	         dabService s;
	         my_dabProcessor -> getParameters (serviceList. at (i). name,
	                                           &s. SId, &s. SCIds);
	         if (s. SId == 0) {
	            QMessageBox::warning (this, tr ("Warning"),
	                         tr ("insufficient data for this program\n"));
	            break;
	         }
	         s. serviceName = serviceList. at (i). name;
	         startService (&s);
	         break;
	      }
	   }
	}
	connect (prevServiceButton, SIGNAL (clicked ()),
	         this, SLOT (handle_prevServiceButton ()));
}

void	RadioInterface::handle_nextServiceButton	() {
	presetTimer. stop ();
	nextService. valid	= false;
	if (!currentService. valid)
	   return;
	QString oldService	= currentService. serviceName;
	disconnect (nextServiceButton, SIGNAL (clicked ()),
	            this, SLOT (handle_nextServiceButton ()));
	QString currentService = serviceLabel -> text ();
	stopScanning (false);
	stopService ();
	if ((serviceList. size () != 0) && (oldService != "")) {
	   for (int i = 0; i < (int)(serviceList. size ()); i ++) {
	      if (serviceList. at (i). name == oldService) {
	         colorService (model. index (i, 0), Qt::black, 11);
	         i = i + 1;
	         if (i >= (int)(serviceList. size ()))
	            i = 0;
	         dabService s;
	         s. serviceName = serviceList. at (i). name;
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

void	RadioInterface::setPresetStation () {
	if (ensembleId -> text () == QString ("")) {
	   QMessageBox::warning (this, tr ("Warning"),
	                          tr ("Oops, ensemble not yet recognized\nselect service manually\n"));
	   return;
	}
	stopScanning (false);

	if (!nextService. valid)
	   return;

	QString presetName	= nextService. serviceName;
	for (const auto& service: serviceList) {
	   if (service. name. contains (presetName)) {
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
	nextService. valid = false;
	fprintf (stderr, "presetName %s not found\n", presetName. toLatin1 (). data ());
}

///////////////////////////////////////////////////////////////////////////
//
//	Channel basics
///////////////////////////////////////////////////////////////////////////
//	Precondition: no channel should be activated
//	
void	RadioInterface::startChannel (const QString &channel) {
int	tunedFrequency	=
	         theBand. Frequency (channel);
	frequencyDisplay	-> display (tunedFrequency / 1000000.0);
	dabSettings		-> setValue ("channel", channel);
	my_dabProcessor		-> start ();
	inputDevice		-> restartReader (tunedFrequency);
	show_for_safety	();
}
//
//	apart from stopping the reader, a lot of administration
//	is to be done.
//	The "stopService" (if any) clears the service related
//	elements on the screen(s)
void	RadioInterface::stopChannel	() {
	stop_audioDumping	();
	soundOut	-> stop ();
//	note framedumping - if any - was already stopped
	presetTimer. stop ();
	channelTimer. stop ();
//
//	The service - if any - is stopped by halting the dabProcessor
	hide_for_safety	();	// hide some buttons
	my_dabProcessor		-> stop ();
	inputDevice		-> stopReader ();
	my_tiiViewer. clear();
	QCoreApplication::processEvents ();
//
//	no processing left at this time
	usleep (1000);		// may be handling pensing signals?
	currentService. valid	= false;
	nextService. valid	= false;

//	all stopped, now look at the GUI elements
	ficError_display	-> setValue (0);
//	the visual elements related to service and channel
	setSynced	(false);
	ensembleId	-> setText ("");
	transmitter_coordinates	-> setText (" ");
	transmitters. clear ();
	if (serviceList. size () > 0) {
	   serviceList. clear ();
	   model. clear ();
	   ensembleDisplay	-> blockSignals (true);
	   ensembleDisplay	-> setModel (&model);
	   ensembleDisplay	-> blockSignals (false);
	   cleanScreen	();
	}
}

//
/////////////////////////////////////////////////////////////////////////
//
//      next- and previous channel buttons
/////////////////////////////////////////////////////////////////////////

void    RadioInterface::selectChannel (const QString &channel) {
	if (!running. load ())
	   return;

	presetTimer. stop ();
	new_presetIndex (0);
	stopScanning	(false);
	stopChannel	();
	startChannel	(channel);
}

void	RadioInterface::handle_nextChannelButton () {
int     currentChannel  = channelSelector -> currentIndex ();

	if (!running. load ())
	   return;

	presetTimer. stop ();
	presetSelector -> setCurrentIndex (0);
	if (my_dabProcessor == nullptr)
	   fprintf (stderr, "Expert error 24\n");
	else
	   stopScanning (false);
	stopChannel ();
	currentChannel ++;
	if (currentChannel >= channelSelector -> count ())
	   currentChannel = 0;
	new_channelIndex (currentChannel);
	startChannel (channelSelector -> currentText ());
}

void	RadioInterface::handle_prevChannelButton () {
int     currentChannel  = channelSelector -> currentIndex ();

	if (!running. load ())
	   return;

	presetTimer. stop ();
	if (my_dabProcessor == nullptr)
	   fprintf (stderr, "Expert error 25\n");
	else
	   stopScanning (false);
	stopChannel     ();
	currentChannel --;
	if (currentChannel < 0)
	   currentChannel =  channelSelector -> count () - 1;
	new_channelIndex (currentChannel);
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
	   stopScanning (false);
	else
	   startScanning ();
}

void	RadioInterface::startScanning	() {
int	switchDelay;

	scanMode	= dabSettings -> value ("scanMode", SINGLE_SCAN).
	                                                             toInt ();

	presetTimer. stop ();
	channelTimer. stop ();
	
	connect (my_dabProcessor, SIGNAL (No_Signal_Found ()),
	         this, SLOT (No_Signal_Found ()));
	new_presetIndex (0);
	stopChannel     ();
	int  cc      = channelSelector -> currentIndex ();
	if (scanMode == SCAN_TO_DATA) {
	   cc ++;
	   if (cc >= channelSelector -> count ())
	      cc = 0;
	}
	else {
	   cc = 0;
	}
	scanning. store (true);
	if (scanMode != SCAN_TO_DATA)
	   scanDumpFile	= findScanDump_FileName ();
	else
	   scanDumpFile = nullptr;

	my_dabProcessor	-> set_scanMode (true);
//      To avoid reaction of the system on setting a different value:
	new_channelIndex (cc);
	dynamicLabel    -> setText ("scan mode \"" +
	                            scanmodeText (scanMode) +
	                            "\" scanning channel " +
	                            channelSelector -> currentText ());
	scanButton      -> setText ("scanning");
	switchDelay		=
	                 dabSettings -> value ("switchDelay", 8). toInt ();
	channelTimer. start (switchDelay * 1000);

	startChannel    (channelSelector -> currentText ());
	if (scanMode != SCAN_TO_DATA) {
	   theTable. clear ();
	   theTable. show ();
	}
}

void	RadioInterface::stopScanning	(bool dump) {
	disconnect (my_dabProcessor, SIGNAL (No_Signal_Found ()),
	            this, SLOT (No_Signal_Found ()));
	(void)dump;
	dynamicLabel    -> setText ("Scan ended");
	scanButton      -> setText ("scan");

	my_dabProcessor -> set_scanMode (false);
	if (!running. load ())
	   return;
	if (!scanning. load ())
	   return;
	channelTimer. stop ();
	scanning. store (false);
	if (scanDumpFile != nullptr) {
	   fclose (scanDumpFile);
	   scanDumpFile = nullptr;
	}
}

//	If the ofdm processor has waited for a period of N frames
//	to get a start of a synchronization,
//	it sends a signal to the GUI handler
//	If "scanning" is "on" we hop to the next frequency on
//	the list

void	RadioInterface::No_Signal_Found () {
int	switchDelay;

	disconnect (my_dabProcessor, SIGNAL (No_Signal_Found (void)),
	            this, SLOT (No_Signal_Found (void)));
	channelTimer. stop ();
	disconnect (&channelTimer, SIGNAL (timeout (void)),
	            this, SLOT (channel_timeOut (void)));

	if (running. load () && scanning. load ()) {
	   int	cc	= channelSelector -> currentIndex ();
	   if ((scanMode != SCAN_TO_DATA) && (serviceList. size () > 0))
	      showServices ();
	   stopChannel ();
	   cc ++;
	   if ((cc >= channelSelector -> count ()) &&
	                                   (scanMode == SINGLE_SCAN)) {
	         stopScanning	(true);
	   }
	   else {  // we just continue
	      if (cc >= channelSelector -> count ()) 
	         cc = 0;
//	To avoid reaction of the system on setting a different value:
	      new_channelIndex (cc);

	      my_dabProcessor	-> set_scanMode (true);
	      connect (my_dabProcessor, SIGNAL (No_Signal_Found (void)),
	               this, SLOT (No_Signal_Found (void)));
	      connect (&channelTimer, SIGNAL (timeout (void)),
	               this, SLOT (channel_timeOut (void)));

	      dynamicLabel -> setText ("scan mode \"" +
	                               scanmodeText (scanMode) +
	                               "\" scanning channel " +
	                               channelSelector -> currentText ());
	      switchDelay	=
	                  dabSettings -> value ("switchDelay", 8). toInt ();
	      channelTimer. start (switchDelay * 1000);
	      startChannel (channelSelector -> currentText ());
	   }
	}
	else
	if (scanning. load ()) 
	   stopScanning	(false);
}

////////////////////////////////////////////////////////////////////////////
//
// showServices
////////////////////////////////////////////////////////////////////////////

void	RadioInterface::showServices () {
ensemblePrinter my_Printer;
QString SNR = "SNR " + QString::number (snrDisplay -> value ());
QString ensembleId	= hextoString (my_dabProcessor -> get_ensembleId ());
	theTable. newEnsemble (" ",
	                       channelSelector -> currentText (),
	                       my_dabProcessor	-> get_ensembleName (),
	                       ensembleId,
	                       SNR,
	                       transmitters);
	for (serviceId serv: serviceList) {
	   QString audioService = serv. name;
	   audiodata d;
	   my_dabProcessor -> dataforAudioService (audioService, &d);
	   if (!d. defined)
	      continue;

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
	if (scanDumpFile != nullptr)
	   my_Printer. showEnsembleData (channelSelector -> currentText (),
	                                 inputDevice -> getVFOFrequency (),
	                                 localTimeDisplay -> text (),
	                                 transmitters,
	                                 serviceList,
	                                 my_dabProcessor, scanDumpFile);

}

/////////////////////////////////////////////////////////////////////
//
bool	RadioInterface::isMember (std::vector<serviceId> a,
	                                                serviceId b) {
	for (const auto serv : a)
	   if (serv. name == b. name)
	      return true;
	return false;
}

std::vector<serviceId>
	RadioInterface::insert (std::vector<serviceId> l,
	                        serviceId n, int order) {
std::vector<serviceId> k;
	if (l . size () == 0) {
	   k. push_back (n);
	   return k;
	}
	uint32_t baseN		= 0;
	uint16_t baseSubCh	= 0;
	QString baseS		= "";

	bool	inserted	= false;
	for (const auto serv : l) {
	   if (!inserted) {
	      if (order == ID_BASED) {
	         if ((baseN <= n. SId) && (n. SId <= serv. SId)) {
	            k. push_back (n);
	            inserted = true;
	         }
	      }
	      else
	      if (order == SUBCH_BASED) {
	         if ((baseSubCh <= n. subChId) && (n. subChId <= serv. subChId)) {
	            k. push_back (n);
	            inserted = true;
	         }
	      }
	      else {
	         if ((baseS < n. name) && (n. name < serv. name)) {
	            k. push_back (n);
	            inserted = true;
	         }
	      }
	   }
	   baseS	= serv. name;
	   baseN	= serv. SId;
	   baseSubCh	= serv. subChId;
	   k. push_back (serv);
	}
	return k;
}
//
//	In those case we are sure not to have an operating
//	dabProcessor, we hide some buttons
void	RadioInterface::hide_for_safety () {
	dumpButton		->	hide ();
	techData. framedumpButton	->	hide ();
	techData. audiodumpButton	->	hide ();
	prevServiceButton	->	hide ();
	nextServiceButton	->	hide ();
	contentButton		->	hide ();
}

void	RadioInterface::show_for_safety () {
	dumpButton		->	show ();
	techData. framedumpButton	->	show ();
	techData. audiodumpButton	->	show ();
	prevServiceButton	->	show ();
	nextServiceButton	->	show ();
	contentButton		->	show ();
}

void	RadioInterface::muteButton_timeOut	() {
	muteDelay --;
	if (muteDelay > 0) {
	   stillMuting -> display (muteDelay);
	   muteTimer. start (1000);
	   return;
	}
	else {
	   disconnect (&muteTimer, SIGNAL (timeout ()),
	               this, SLOT (muteButton_timeOut ()));
	   setButtonFont (muteButton, "mute", 10);
	   stillMuting	-> hide ();
	   muting = false;
	}
}

void    RadioInterface::handle_muteButton       () {
	if (muting) {
	   muteTimer. stop ();
	   disconnect (&muteTimer, SIGNAL (timeout ()),
	               this, SLOT (handle_muteButton ()));
	   setButtonFont (muteButton, "mute", 10);
	   stillMuting	-> hide ();
	   muting = false;
	   return;
	}

	connect (&muteTimer, SIGNAL (timeout ()),
	         this, SLOT (muteButton_timeOut ()));
	muteDelay	= dabSettings -> value ("muteTime", 2). toInt ();
	muteDelay	*= 60;
	muteTimer. start (1000);
	setButtonFont (muteButton, "MUTING", 12);
	stillMuting	-> show ();
	stillMuting	-> display (muteDelay);
	muting = true;
}

//
//	Intermezzo: finding filenames
//
FILE	*RadioInterface::findContentDump_fileName (const QString &channel) {
QString suggestedFileName;
QString	saveDir		= dabSettings -> value ("contentDir",
	                                        QDir::homePath ()). toString ();
QString theTime	= localTimeDisplay -> text ();

	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	for (int i = 0; i < theTime. length (); i ++)
	   if (!isValid (theTime. at (i)))
	      theTime. replace (i, 1, '-');
	suggestedFileName = saveDir + "Qt-DAB-" + channel +
	                                          "-" + theTime;

	QString fileName = QFileDialog::getSaveFileName (this,
	                                        tr ("Save file ..."),
	                                        suggestedFileName + ".txt",
	                                        tr ("Text (*.txt)"));
	if (fileName == "")
	   return nullptr;

	fileName	= QDir::toNativeSeparators (fileName);
	FILE *fileP	= fopen (fileName. toUtf8(). data(), "w");

	if (fileP == nullptr) {
	   fprintf (stderr, "Could not open file %s\n",
	                              fileName. toUtf8(). data());
	   return nullptr;
	}

	QString	dumper	= QDir::fromNativeSeparators (fileName);
	int x           = dumper. lastIndexOf ("/");
	saveDir         = dumper. remove (x, dumper. count () - x);
	dabSettings     -> setValue ("contentDir", saveDir);
	return fileP;
}

//
FILE	*RadioInterface::findFrameDump_fileName (const QString &service,
	                                         const QString &time) {
QString	saveDir	= dabSettings -> value ("saveDir_frameDump",
	                                QDir::homePath ()).  toString ();
	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	QString tailS	= service + "-" + time + ".aac";
	for (int i = 0; i < tailS. length (); i ++)
	   if (!isValid (tailS. at (i)))
	      tailS. replace (i,1, '-');

	QString suggestedFileName = saveDir + tailS;
	QString file = QFileDialog::getSaveFileName (this,
	                                     tr ("Save file ..."),
	                                     suggestedFileName,
	                                     tr ("aac data (*.aac)"));
	if (file == QString (""))       // apparently cancelled
	   return nullptr;

	file		= QDir::toNativeSeparators (file);
	FILE *theFile	= fopen (file. toLatin1 (). data (), "w+b");
	if (theFile == nullptr) {
	   QString s = QString ("cannot open ") + file;
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr (s. toLatin1 (). data ()));
	   return nullptr;
	}

	QString dumper	= QDir::fromNativeSeparators (file);
	int x		= dumper. lastIndexOf ("/");
	saveDir		= dumper. remove (x, dumper. count () - x);
	dabSettings	-> setValue ("saveDir_frameDump", saveDir);
	
	return theFile;
}

SNDFILE	*RadioInterface::findAudioDump_fileName (const QString &service,
	                                         const QString &time) {
SF_INFO	*sf_info	= (SF_INFO *)alloca (sizeof (SF_INFO));
QString	saveDir	 = dabSettings -> value ("saveDir_audioDump",
	                                 QDir::homePath ()).  toString ();

	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	QString tailS = service + "-" + time;
	for (int i = 0; i < tailS. length (); i ++)
	   if (!isValid (tailS. at (i))) 
	      tailS. replace (i, 1, '-');

	QString suggestedFileName = saveDir + tailS + ".wav";
	QString file = QFileDialog::getSaveFileName (this,
	                                        tr ("Save file ..."),
	                                        suggestedFileName,
	                                        tr ("PCM wave file (*.wav)"));
	if (file == QString (""))
	   return nullptr;
	if (!file.endsWith (".wav", Qt::CaseInsensitive))
	   file.append (".wav");
	file		= QDir::toNativeSeparators (file);
	sf_info		-> samplerate	= 48000;
	sf_info		-> channels	= 2;
	sf_info		-> format	= SF_FORMAT_WAV | SF_FORMAT_PCM_16;

	SNDFILE *theFile	= sf_open (file. toUtf8(). data(),
	                                   SFM_WRITE, sf_info);
	if (theFile == nullptr) {
	   qDebug() << "Cannot open " << file. toUtf8(). data();
	   return nullptr;
	}

	QString	dumper	= QDir::fromNativeSeparators (file);
	int x		= dumper. lastIndexOf ("/");
	saveDir		= dumper. remove (x, dumper. count () - x);
	dabSettings	-> setValue ("saveDir_audioDump", saveDir);

	return theFile;
}

FILE	*RadioInterface::findScanDump_FileName		() {
	QMessageBox::StandardButton resultButton =
	             QMessageBox::question (this, "dabRadio",
	                                    tr ("save the scan?\n"),
	                                    QMessageBox::No | QMessageBox::Yes,
	                                    QMessageBox::Yes);
	if (resultButton != QMessageBox::Yes)
	   return nullptr;

	QString   saveDir = dabSettings -> value ("contentDir",
	                                        QDir::homePath ()). toString ();

	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	QString theTime   = localTimeDisplay -> text ();
	if (theTime == "localTime") 
	   theTime = QDateTime::currentDateTime (). toString ();
	for (int i = 0; i < theTime. length (); i ++)
	   if (!isValid (theTime. at (i)))
	      theTime. replace (i, 1, '-');
	QString suggestedFileName =
	                       saveDir + "Qt-DAB-scan" + "-" + theTime;

	fprintf (stderr, "suggested filename %s\n",
	                            suggestedFileName. toLatin1 (). data ());
	QString fileName = QFileDialog::getSaveFileName (this,
	                                        tr ("Save file ..."),
	                                        suggestedFileName + ".txt",
	                                        tr ("Text (*.txt)"));
	return  fopen (fileName. toUtf8 (). data (), "w");
}

void	RadioInterface::new_presetIndex (int index) {
	if (presetSelector -> currentIndex () == index)
	   return;
	presetSelector -> blockSignals (true);
	set_newPresetIndex (index);
	while (presetSelector -> currentIndex () != 0)
	   usleep (200);
	presetSelector	-> blockSignals (false);
//	presetSelector -> setCurrentIndex (index);
}

void	RadioInterface::new_channelIndex (int index) {
	if (channelSelector -> currentIndex () == index)
	   return;
	channelSelector	-> blockSignals (true);
	set_newChannel (index);
	while (channelSelector -> currentIndex () != index)
	   usleep (2000);
//	channelSelector -> setCurrentIndex (index);
	channelSelector	-> blockSignals (false);
}

//
//	merely as a gadget, for each button the color can be set
//	
void	RadioInterface::set_Colors () {
	dabSettings	-> beginGroup ("colorSettings");
QString contentButton_color =
	   dabSettings -> value (CONTENT_BUTTON + "_color",
	                                              "white"). toString ();
QString contentButton_font =
	   dabSettings -> value (CONTENT_BUTTON + "_font",
	                                              "black"). toString ();
QString detailButton_color =
	   dabSettings -> value (DETAIL_BUTTON + "_color",
	                                              "white"). toString ();
QString detailButton_font =
	   dabSettings -> value (DETAIL_BUTTON + "_font",
	                                              "black"). toString ();
QString resetButton_color =
	   dabSettings -> value (RESET_BUTTON + "_color",
	                                              "white"). toString ();
QString resetButton_font =
	   dabSettings -> value (RESET_BUTTON + "_font",
	                                              "black"). toString ();
QString scanButton_color =
	   dabSettings -> value (SCAN_BUTTON + "_color",
	                                              "white"). toString ();
QString scanButton_font =
	   dabSettings -> value (SCAN_BUTTON + "_font",
	                                              "black"). toString ();

QString tiiButton_color =
	   dabSettings -> value (TII_BUTTON + "_color",	
	                                              "white"). toString ();
QString tiiButton_font =
	   dabSettings -> value (TII_BUTTON + "_font",
	                                              "black"). toString ();
QString correlationButton_color =
	   dabSettings -> value (CORRELATION_BUTTON +"_color",
	                                              "white"). toString ();
QString correlationButton_font =
	   dabSettings -> value (CORRELATION_BUTTON + "_font",
	                                              "black"). toString ();
QString spectrumButton_color =
	   dabSettings -> value (SPECTRUM_BUTTON + "_color",
	                                              "white"). toString ();
QString spectrumButton_font =
	   dabSettings -> value (SPECTRUM_BUTTON + "_font",
	                                              "black"). toString ();
QString devicewidgetButton_color =
	   dabSettings -> value (DEVICEWIDGET_BUTTON + "_color",
	                                              "white"). toString ();
QString devicewidgetButton_font =
	   dabSettings -> value (DEVICEWIDGET_BUTTON + "_font",
	                                              "black"). toString ();

QString historyButton_color =
	   dabSettings -> value (HISTORY_BUTTON + "_color",
	                                              "white"). toString ();
QString historyButton_font =
	   dabSettings -> value (HISTORY_BUTTON + "_font",
	                                              "black"). toString ();
QString dumpButton_color =
	   dabSettings -> value (DUMP_BUTTON + "_color",
	                                              "white"). toString ();
QString dumpButton_font =
	   dabSettings -> value (DUMP_BUTTON + "_font",
	                                              "black"). toString ();
QString notUsedButton_color =
	   dabSettings -> value ("notUsedButton_color",
	                                              "black"). toString ();
QString notUsedButton_font =
	   dabSettings -> value ("notUsedButton_font",
	                                              "white"). toString ();
QString muteButton_color =
	   dabSettings -> value (MUTE_BUTTON + "_color",
	                                              "white"). toString ();
QString muteButton_font =
	   dabSettings -> value (MUTE_BUTTON + "_font",
	                                              "black"). toString ();

QString prevChannelButton_color =
	   dabSettings -> value (PREVCHANNEL_BUTTON + "_color",
	                                              "white"). toString ();
QString nextChannelButton_color =
	   dabSettings -> value (NEXTCHANNEL_BUTTON + "_color",
	                                              "white"). toString ();
QString prevServiceButton_color =
	   dabSettings -> value (PREVCHANNEL_BUTTON + "_color",
	                                              "white"). toString ();
QString nextServiceButton_color =
	   dabSettings -> value (NEXTCHANNEL_BUTTON + "_color",
	                                              "white"). toString ();

QString	framedumpButton_color =
	   dabSettings -> value (FRAMEDUMP_BUTTON + "_color",
	                                              "white"). toString ();
QString	framedumpButton_font =
	   dabSettings -> value (FRAMEDUMP_BUTTON + "_font",
		                                      "black"). toString ();
QString	audiodumpButton_color =
	   dabSettings -> value (AUDIODUMP_BUTTON + "_color",
	                                              "white"). toString ();
QString	audiodumpButton_font =
	   dabSettings -> value (AUDIODUMP_BUTTON + "_font",
	                                              "black"). toString ();

	dabSettings	-> endGroup ();

	QString temp = "QPushButton {background-color: %1; color: %2}";
	contentButton	-> setStyleSheet (temp. arg (contentButton_color,
	                                             contentButton_font));
	detailButton	-> setStyleSheet (temp. arg (detailButton_color,
	                                             detailButton_font));
	resetButton	-> setStyleSheet (temp. arg (resetButton_color,	
	                                             resetButton_font));
	scanButton	-> setStyleSheet (temp. arg (scanButton_color,
	                                             scanButton_font));

	show_tiiButton	-> setStyleSheet (temp. arg (tiiButton_color,
	                                             tiiButton_font));
	show_correlationButton
	                -> setStyleSheet (temp. arg (correlationButton_color,
	                                              correlationButton_font));
	show_spectrumButton
	                -> setStyleSheet (temp. arg (spectrumButton_color,
	                                             spectrumButton_font));
	devicewidgetButton -> setStyleSheet (temp. arg (devicewidgetButton_color,
	                                                devicewidgetButton_font));

	historyButton	-> setStyleSheet (temp. arg (historyButton_color,
	                                             historyButton_font));
	dumpButton	-> setStyleSheet (temp. arg (dumpButton_color,
	                                             dumpButton_font));
	configButton	-> setStyleSheet (temp. arg (notUsedButton_color,
	                                             notUsedButton_font));
	muteButton	-> setStyleSheet (temp. arg (muteButton_color,
	                                             muteButton_font));

	prevChannelButton -> setStyleSheet (temp. arg (prevChannelButton_color,
	                                               "red"));
	nextChannelButton -> setStyleSheet (temp. arg (nextChannelButton_color,
	                                               "black"));
	prevServiceButton -> setStyleSheet (temp. arg (prevServiceButton_color,
	                                               "red"));
	nextServiceButton -> setStyleSheet (temp. arg (nextServiceButton_color,
	                                               "black"));

	techData. framedumpButton ->
	                     setStyleSheet (temp. arg (framedumpButton_color,
	                                               framedumpButton_font));
	techData. audiodumpButton ->
	                     setStyleSheet (temp. arg (audiodumpButton_color,
	                                               audiodumpButton_font));
}

void	RadioInterface::color_contentButton	() {
	set_buttonColors (contentButton, CONTENT_BUTTON);
}

void	RadioInterface::color_detailButton	() {
	set_buttonColors (detailButton, DETAIL_BUTTON);
}

void	RadioInterface::color_resetButton	() {
	set_buttonColors (resetButton, RESET_BUTTON);
}

void	RadioInterface::color_scanButton	() {
	set_buttonColors (scanButton, SCAN_BUTTON);
}

void	RadioInterface::color_tiiButton		() {
	set_buttonColors (show_tiiButton, TII_BUTTON);
}

void	RadioInterface::color_correlationButton	()	{
	set_buttonColors (show_correlationButton, CORRELATION_BUTTON);
}

void	RadioInterface::color_spectrumButton	()	{
	set_buttonColors (show_spectrumButton, SPECTRUM_BUTTON);
}

void	RadioInterface::color_devicewidgetButton	() {
	set_buttonColors (devicewidgetButton, DEVICEWIDGET_BUTTON);
}

void	RadioInterface::color_historyButton	()	{
	set_buttonColors (historyButton, HISTORY_BUTTON);
}

void	RadioInterface::color_sourcedumpButton	()	{
//	set_buttonColors (dumpButton, DUMP_BUTTON);
}

void	RadioInterface::color_muteButton	()	{
	set_buttonColors (muteButton, MUTE_BUTTON);
}

void	RadioInterface::color_prevChannelButton	()	{
	set_buttonColors (prevChannelButton, PREVCHANNEL_BUTTON);
}

void	RadioInterface::color_nextChannelButton	()	{
	set_buttonColors (nextChannelButton, NEXTCHANNEL_BUTTON);
}

void	RadioInterface::color_prevServiceButton	()	{
	set_buttonColors (prevServiceButton, PREVSERVICE_BUTTON);
}

void	RadioInterface::color_nextServiceButton	()	{
	set_buttonColors (nextServiceButton, NEXTSERVICE_BUTTON);
}

void	RadioInterface::color_framedumpButton	()	{
	set_buttonColors (techData. framedumpButton, FRAMEDUMP_BUTTON);
}

void	RadioInterface::color_audiodumpButton	()	{
	set_buttonColors (techData. audiodumpButton, AUDIODUMP_BUTTON);
}

void    RadioInterface::color_configButton      ()      {
	set_buttonColors (configButton, CONFIG_BUTTON);
}

void    RadioInterface::color_dlTextButton      ()      {
        set_buttonColors (dlTextButton, DLTEXT_BUTTON);
}


void	RadioInterface::set_buttonColors	(QPushButton *b,
	                                         const QString &buttonName) {
colorSelector *selector;
int	index;

	selector		= new colorSelector ("button color");
	index			= selector -> QDialog::exec ();
	QString baseColor	= selector -> getColor (index);
	delete selector;
	if (index == 0)
	   return;
	selector		= new colorSelector ("text color");
	index			= selector	-> QDialog::exec ();
	QString textColor	= selector	-> getColor (index);
	delete selector;
	if (index == 0)
	   return;
	QString temp = "QPushButton {background-color: %1; color: %2}";
	b	-> setStyleSheet (temp. arg (baseColor, textColor));

	QString buttonColor	= buttonName + "_color";
	QString buttonFont	= buttonName + "_font";

	dabSettings	-> beginGroup ("colorSettings");
	dabSettings	-> setValue (buttonColor, baseColor);
	dabSettings	-> setValue (buttonFont, textColor);
	dabSettings	-> endGroup ();
}

/////////////////////////////////////////////////////////////////////////
//	External configuration items				//////

void	RadioInterface::handle_configSetting	() {
	if (configDisplay -> isHidden ()) 
	   configDisplay -> show ();
	else
	   configDisplay -> hide ();
}

void	RadioInterface::handle_muteTimeSetting	(int newV) {
	dabSettings	-> setValue ("muteTime", newV);
}

void	RadioInterface::handle_switchDelaySetting	(int newV) {
	dabSettings	-> setValue ("switchDelay", newV);
}

void	RadioInterface::handle_plotLengthSetting	(int l) {
	dabSettings -> setValue ("plotLength", l);
}

void	RadioInterface::handle_scanmodeSelector		(int d) {
	dabSettings -> setValue ("scanMode", d);
}

void	RadioInterface::handle_motslideSelector		(int d) {
	(void)d;
	dabSettings	-> setValue ("motSlides",
	                              configWidget. motslideSelector -> isChecked () ? 1 : 0);
}


void	RadioInterface::handle_orderAlfabetical		() {
	dabSettings -> setValue ("serviceOrder", ALPHA_BASED);
}

void	RadioInterface::handle_orderServiceIds		() {
	dabSettings -> setValue ("serviceOrder", ID_BASED);
}

void	RadioInterface::handle_ordersubChannelIds	() {
	dabSettings -> setValue ("serviceOrder", SUBCH_BASED);
}


///////////////////////////////////////////////////////////////////////////

//	Handling schedule

void    RadioInterface::handle_scheduleSelector	() {
QStringList candidates;
scheduleSelector theSelector;
QString		scheduleService;
	
	for (uint16_t i = 0; i < serviceList. size (); i ++) {
	   QString service = channelSelector -> currentText () +
	                           ":" + serviceList. at (i). name;
	   theSelector. addtoList (service);
	   candidates += service;
	}
	for (int i = 1; i < presetSelector -> count (); i ++) {
	   if (!candidates. contains (presetSelector -> itemText (i))) {
	      theSelector.
	              addtoList (presetSelector -> itemText (i));
	      candidates += presetSelector -> itemText (i);
	   }
	}

	int selected		= theSelector. QDialog::exec ();
	scheduleService		= candidates. at (selected);
	{  elementSelector	theElementSelector (scheduleService);
	   int	targetTime	= theElementSelector. QDialog::exec ();
	   theScheduler. addRow (scheduleService,
	                         targetTime / 60, 
	                         targetTime % 60);
	}
	theScheduler. show ();
}

void	RadioInterface::scheduler_timeOut	(const QString &s) {
	if (!running. load ())
	   return;

	presetTimer. stop ();
	localSelect (s);
}


void    RadioInterface::handle_tii_detectorMode (int d) {
bool    b = configWidget. tii_detectorMode -> isChecked ();
        my_dabProcessor -> set_tiiDetectorMode (b);
        dabSettings     -> setValue ("tii_detector", b ? 1 : 0);
}

void	RadioInterface::handle_dlTextButton	() {
	if (dlTextFile != nullptr) {
	   fclose (dlTextFile);
	   dlTextFile = nullptr;
	   dlTextButton	-> setText ("dlText");
	   return;
	}

	QString	fileName =filenameFinder. finddlText_fileName ();
	dlTextFile	= fopen (fileName. toUtf8 (). data (), "w+");
	if (dlTextFile	== nullptr)
	   return;
	dlTextButton		-> setText ("writing");
}
