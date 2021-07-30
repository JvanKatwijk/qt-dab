#
/*
 *    Copyright (C)  2015, 2016, 2017, 2018, 2019, 2020, 2021
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
#include	<QCoreApplication>
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
#include	"rawfiles.h"
#include	"wavfiles.h"
#include	"xml-filereader.h"
#include	"color-selector.h"
#include	"alarm-selector.h"
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
#include	"sdrplay-handler-v2.h"
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
#ifdef	HAVE_LIME
#include	"lime-handler.h"
#endif
#ifdef	HAVE_PLUTO
#include	"pluto-handler.h"
#elif	HAVE_PLUTO_2
#include	"pluto-handler-2.h"
#elif	HAVE_PLUTO_RXTX
#include	"pluto-rxtx-handler.h"
#include	"dab-streamer.h"
#endif
#ifdef	HAVE_SOAPY
#include	"soapy-handler.h"
#endif
#ifdef	HAVE_COLIBRI
#include	"colibri-handler.h"
#endif
#ifdef	HAVE_ELAD
#include	"elad-handler.h"
#endif
#include	"spectrum-viewer.h"
#include	"correlation-viewer.h"
#include	"tii-viewer.h"
#include	"history-handler.h"
#include	"time-table.h"

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

#define	SINGLE_SCAN		0
#define	SCAN_TO_DATA		1
#define	SCAN_CONTINUOUSLY	2

const
char	*scanTextTable [3] = {
	"single scan",
	"scan to data",
	"scan continuously"
};

static inline
QString scanmodeText (int e) {
	return QString (scanTextTable [e]);
}

#define	CONTENT_BUTTON		QString ("contentButton")
#define DETAIL_BUTTON		QString ("detailButton")
#define	RESET_BUTTON		QString ("resetButton")
#define SCAN_BUTTON		QString ("scanButton")
#define	TII_BUTTON		QString ("tiiButton")
#define	CORRELATION_BUTTON	QString ("correlationButton")
#define	SPECTRUM_BUTTON		QString ("spectrumButton")
#define	SNR_BUTTON		QString ("snrButton")
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
#define	CONFIG_BUTTON		QString	("configButton")
#define	DLTEXT_BUTTON		QString	("dlTextButton")

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

#ifdef	__LOGGING__
void	RadioInterface::LOG	(const QString &a1, const QString &a2) {
QString theTime	= QDateTime::currentDateTime (). toString ();
	if (logFile == nullptr)
	   return;
	fprintf (logFile, "at %s: %s %s\n",
	              theTime. toUtf8 (). data (),
	              a1. toUtf8 (). data (), a2. toUtf8 (). data ());
}
#else
void	RadioInterface::LOG	(const QString &a1, const QString &a2) {
	(void)a1; (void)a2;
}

#endif
	RadioInterface::RadioInterface (QSettings	*Si,
	                                const QString	&presetFile,
	                                const QString	&freqExtension,
	                                bool		error_report,
	                                int32_t		dataPort,
	                                int32_t		clockPort,
	                                int		fmFrequency,
	                                QWidget		*parent):
	                                        QWidget (parent),
	                                        spectrumBuffer (2 * 32768),
	                                        iqBuffer (2 * 1536),
	                                        tiiBuffer (32768),
	                                        snrBuffer (512),
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
	                                        my_snrViewer (
	                                                  this, Si),
	                                        my_presetHandler (this),
	                                        theBand (freqExtension, Si),
	                                        theTable (this),
	                                        filenameFinder (Si),
	                                        dataDisplay (nullptr),
	                                        configDisplay (nullptr),
	                                        the_dlCache (10) {
int16_t	latency;
int16_t k;
QString h;
uint8_t	dabBand;

	dabSettings		= Si;
	this	-> error_report	= error_report;
	this	-> fmFrequency	= fmFrequency;
	this	-> dlTextFile	= nullptr;
	running. 		store (false);
	scanning. 		store (false);
	my_dabProcessor		= nullptr;
	isSynced		= false;
	stereoSetting		= false;
//
//	"globals" is introduced to reduce the number of parameters
//	for the dabProcessor
	globals. spectrumBuffer	= &spectrumBuffer;
	globals. iqBuffer	= &iqBuffer;
	globals. responseBuffer	= &responseBuffer;
	globals. tiiBuffer	= &tiiBuffer;
	globals. snrBuffer	= &snrBuffer;
	globals. frameBuffer	= &frameBuffer;

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
	if (globals. tii_delay < 2)
	   globals. tii_delay	= 2;
	globals. tii_depth      =
	          dabSettings -> value ("tii_depth", 4). toInt();
	globals. echo_depth     =
	          dabSettings -> value ("echo_depth", 1). toInt();

	theFont			=
	          dabSettings -> value ("theFont", "Times"). toString ();
	fontSize		=
	          dabSettings -> value ("fontSize", 12). toInt ();

#ifdef	_SEND_DATAGRAM_
	ipAddress		= dabSettings -> value ("ipAddress", "127.0.0.1"). toString();
	port			= dabSettings -> value ("port", 8888). toInt();
#endif
//
	saveSlides	= dabSettings -> value ("saveSlides", 1). toInt();
	if (saveSlides != 0)
	   set_picturePath ();

	filePath	= dabSettings -> value ("filePath", ""). toString ();
	if ((filePath != "") && (!filePath. endsWith ("/")))
	   filePath = filePath + "/";

//	The settings are done, now creation of the GUI parts
	setupUi (this);
//
//	dataDisplay	= new QFrame (nullptr);
	techData. setupUi (&dataDisplay);
	techData. timeTable_button -> hide ();

	epgLabel	-> hide ();
	epgLabel	-> setStyleSheet ("QLabel {background-color : yellow}");
//	configDisplay	= new QFrame (nullptr);
	configWidget. setupUi (&configDisplay);

	int x = dabSettings -> value ("muteTime", 2). toInt ();
	configWidget. muteTimeSetting -> setValue (x);

	x = dabSettings -> value ("switchDelay", 8). toInt ();
	configWidget. switchDelaySetting -> setValue (x);

	int snrDelay = dabSettings -> value ("snrDelay", 5). toInt ();
	configWidget. snrDelaySetting -> setValue (snrDelay);

	currentService. valid	= false;
	nextService. valid	= false;

	if (dabSettings -> value ("has-presetName", 0). toInt () != 0) {
	   configWidget. saveServiceSelector -> setChecked (true);
	   QString presetName		=
	              dabSettings -> value ("presetname", ""). toString();
	   if (presetName != "") {
	      nextService. serviceName = presetName;
	      nextService. SId		= 0;
	      nextService. SCIds	= 0;
	      nextService. valid	= true;
	   }
	}

	dabSettings	-> beginGroup ("snrViewer");
	configWidget. snrHeightSelector -> setValue (dabSettings -> value ("snrHeight", 15). toInt ());
	configWidget. snrLengthSelector -> setValue (dabSettings -> value ("snrLength", 312). toInt ());
	dabSettings	-> endGroup ();
//
#ifdef	__LOGGING__
	logFile		= nullptr;
	QString abc	= dabSettings	-> value ("logFile", ""). toString ();
	if (abc != "")
	   logFile	= fopen (abc. toUtf8 (). data (), "a");
#endif
	int scanMode	=
	           dabSettings -> value ("scanMode", SINGLE_SCAN). toInt ();
	configWidget. scanmodeSelector -> setCurrentIndex (scanMode);

	bool motselectionMode =
	           dabSettings -> value ("motSlides", 0). toInt () == 1;
	if (motselectionMode)
	   configWidget. motslideSelector -> setChecked (true);

	x = dabSettings -> value ("closeDirect", 0). toInt ();
	if (x != 0)
	   configWidget. closeDirect -> setChecked (true);
	x = dabSettings -> value ("serviceOrder", ALPHA_BASED). toInt ();
	if (x == ALPHA_BASED)
	   configWidget. orderAlfabetical -> setChecked (true);
	else
	if (x == ID_BASED)
	   configWidget. orderServiceIds -> setChecked (true);
	else
	   configWidget. ordersubChannelIds -> setChecked (true);

	motSlides		= nullptr;
	dataDisplay. hide ();
	stillMuting		-> hide ();
	serviceList. clear ();
        model . clear ();
        ensembleDisplay         -> setModel (&model);
	alarmLabel		->
                         setStyleSheet ("QLabel {background-color : red}");
	alarmLabel		-> setText ("Alarm");
	alarmLabel		-> hide ();
/*
 */
#ifdef	DATA_STREAMER
	dataStreamer		= new tcpServer (dataPort);
#else
	(void)dataPort;
#endif
#ifdef	CLOCK_STREAMER
	clockStreamer		= new tcpServer (clockPort);
#else
	(void)clockPort;
#endif

//	Where do we leave the audio out?
	streamoutSelector	-> hide();
#ifdef	TCP_STREAMER
	soundOut		= new tcpStreamer	(20040);
#elif	QT_AUDIO
	soundOut		= new Qt_Audio();
#else
//	just sound out
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
#ifdef	TRY_EPG
	epgPath		= dabSettings -> value ("epgPath", "/tmp"). toString ();
	connect (&epgProcessor,
	             SIGNAL (set_epgData (int, int, const QString &)),
	         this, SLOT (set_epgData (int, int, const QString &)));
	if ((epgPath != "") && (!epgPath. endsWith ("/")))
	   epgPath = epgPath + "/";
//	timer for autostart epg service
        epgTimer. setSingleShot (true);
        connect (&epgTimer, SIGNAL (timeout ()),
                 this, SLOT (epgTimer_timeOut ()));

#endif
	QString historyFile     = QDir::homePath () + "/.qt-history.xml";
        historyFile             =
	             dabSettings -> value ("history", historyFile). toString ();
        historyFile             = QDir::toNativeSeparators (historyFile);
        my_history              = new historyHandler (this, historyFile);
	my_timeTable		= new timeTableHandler (this);
	my_timeTable	-> hide ();
        connect (my_history, SIGNAL (handle_historySelect (const QString &)),
                 this, SLOT (handle_historySelect (const QString &)));
	connect (this, SIGNAL (set_newChannel (int)),
                 channelSelector, SLOT (setCurrentIndex (int)));
        connect (this, SIGNAL (set_newPresetIndex (int)),
                 presetSelector, SLOT (setCurrentIndex (int)));

//	restore some settings from previous incarnations
	QString t       =
	        dabSettings     -> value ("dabBand", "VHF Band III"). toString();
	dabBand         = t == "VHF Band III" ?  BAND_III : L_BAND;

	theBand. setupChannels  (channelSelector, dabBand);
	QString skipfileName	= 
	               dabSettings	-> value ("skipFile", ""). toString ();
	theBand. setup_skipList (skipfileName);

	QPalette p	= ficError_display -> palette();
	p. setColor (QPalette::Highlight, Qt::red);
	ficError_display		-> setPalette (p);
	techData. stereoLabel		->
	                   setStyleSheet ("QLabel {background-color : red}");
	p. setColor (QPalette::Highlight, Qt::green);
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
	total_ficError		= 0;
	total_fics		= 0;
	syncedLabel		->
	        setStyleSheet ("QLabel {background-color : red; color: white}");
	techData. stereoLabel		->
	        setStyleSheet ("QLabel {background-color : red}");
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
	connect (snrButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_snrButton ()));
        connect (devicewidgetButton, SIGNAL (rightClicked ()),
                 this, SLOT (color_devicewidgetButton ()));
        connect (historyButton, SIGNAL (rightClicked ()),
                 this, SLOT (color_historyButton ()));
	connect (dumpButton, SIGNAL (rightClicked (void)),
	         this, SLOT (color_sourcedumpButton (void)));
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
	connect (techData. timeTable_button, SIGNAL (clicked ()),
	         this, SLOT (handle_timeTable ()));
//	connect (techData. muteButton, SIGNAL (rightClicked (void)),
//	         this, SLOT (color_muteButton (void)));
	connect (muteButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_muteButton ()));
	connect (dlTextButton, SIGNAL (clicked ()),	
	         this, SLOT (handle_dlTextButton ()));
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
	         this, SLOT (channel_timeOut (void)));
//
//	presetTimer
	presetTimer. setSingleShot (true);
	connect (&presetTimer, SIGNAL (timeout (void)),
	            this, SLOT (setPresetStation (void)));
//
//	timer for muting
	muteTimer. setSingleShot (true);
	muting		= false;
//
	alarmTimer. setSingleShot	(true);
	connect (&alarmTimer, SIGNAL (timeout ()),
	         this, SLOT (alarmTimer_timeOut ()));
	QPalette lcdPalette;
	lcdPalette. setColor (QPalette::Background, Qt::white);
	lcdPalette. setColor (QPalette::Base, Qt::black);
	snrDisplay		-> setPalette (lcdPalette);
	snrDisplay		-> setAutoFillBackground (true);
	frequencyDisplay	-> setPalette (lcdPalette);
	frequencyDisplay	-> setAutoFillBackground (true);
	cpuMonitor		-> setPalette (lcdPalette);
	cpuMonitor		-> setAutoFillBackground (true);
	correctorDisplay	-> setPalette (lcdPalette);
	correctorDisplay	-> setAutoFillBackground (true);
	set_Colors ();
	localTimeDisplay -> setStyleSheet ("QLabel {background-color : gray; color: white}");
	runtimeDisplay	-> setStyleSheet ("QLabel {background-color : gray; color: white}");

//
//	add devices to the list
	deviceSelector	-> addItem ("file input(.raw)");
	deviceSelector	-> addItem ("file input(.iq)");
	deviceSelector	-> addItem ("file input(.sdr)");
	deviceSelector	-> addItem ("xml files");
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
#ifdef	HAVE_HACKRF
	deviceSelector	-> addItem ("hackrf");
#endif
#ifdef	HAVE_SOAPY
	deviceSelector	-> addItem ("soapy");
#endif
#ifdef	HAVE_LIME
	deviceSelector	-> addItem ("limeSDR");
#endif
#ifdef	HAVE_PLUTO_RXTX
	deviceSelector	-> addItem ("pluto-rxtx");
	streamerOut	= nullptr;
#endif
#ifdef	HAVE_PLUTO
	deviceSelector	-> addItem ("pluto");
#endif
#ifdef	HAVE_COLIBRI
	deviceSelector	-> addItem ("colibri");
#endif
#ifdef  HAVE_EXTIO
	deviceSelector	-> addItem ("extio");
#endif
#ifdef	HAVE_RTL_TCP
	deviceSelector	-> addItem ("rtl_tcp");
#endif
#ifdef	HAVE_ELAD
	deviceSelector	-> addItem ("elad-s1");
#endif
	inputDevice	= nullptr;
	h               =
	           dabSettings -> value ("device", "no device"). toString();
	k               = deviceSelector -> findText (h);
//	fprintf (stderr, "%d %s\n", k, h. toUtf8(). data());
	if (k != -1) {
	   deviceSelector       -> setCurrentIndex (k);
	   inputDevice	= setDevice (deviceSelector -> currentText());
	}

	if (inputDevice != nullptr) {
	   if (dabSettings -> value ("deviceVisible", 1). toInt () != 0)
	      inputDevice -> show ();
	   else
	      inputDevice -> hide ();
	}

	if (dabSettings	-> value ("spectrumVisible", 0). toInt () == 1) 
	   my_spectrumViewer. show ();
	if (dabSettings -> value ("tiiVisible", 0). toInt () == 1) 
	   my_tiiViewer. show ();
	if (dabSettings -> value ("correlationVisible", 0). toInt () == 1)
	   my_correlationViewer. show ();
	if (dabSettings -> value ("snrVisible", 0). toInt () == 1)
	   my_snrViewer. show ();

//	if a device was selected, we just start, otherwise
//	we wait until one is selected
	currentServiceDescriptor	= nullptr;

	if (inputDevice != nullptr) {
	   LOG ("start with ",
	            inputDevice -> deviceName (). toUtf8 (). data ());
	   if (doStart ()) {
	      qApp	-> installEventFilter (this);
	      return;
	   }
	   else {
	      delete inputDevice;
	      inputDevice	= nullptr;
	   }
	}

	LOG ("starting without device ", "");
	connect (deviceSelector, SIGNAL (activated (const QString &)),
	         this,  SLOT (doStart (const QString &)));
	qApp	-> installEventFilter (this);
}

QString RadioInterface::footText () {
	version			= QString (CURRENT_VERSION);
        QString versionText = "Qt-DAB-version: " + QString(CURRENT_VERSION) + "\n";
        versionText += "Built on " + QString(__TIMESTAMP__) + QString (", Commit ") + QString (GITHASH) + "\n";
        versionText += "Copyright Jan van Katwijk, mailto:J.vanKatwijk@gmail.com\n";
        versionText += "Rights of Qt, fftw, portaudio, libsamplerate and libsndfile gratefully acknowledged\n";
        versionText += "Rights of other contributors gratefully acknowledged";
       return versionText;
}
//
//	doStart (QString) is called when - on startup - NO device
//	was registered to be used, and the user presses the
//	selectDevice comboBox
void	RadioInterface::doStart (const QString &dev) {
	(void)dev;
	inputDevice	= setDevice	(dev);
//	just in case someone wants to push all those nice buttons that
//	are now connected to erroneous constructs
//	Some buttons should not be touched before we have a device
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

	my_dabProcessor	= new dabProcessor  (this, inputDevice, &globals);

	int snrDelay = dabSettings -> value ("snrDelay", 5). toInt ();
	my_snrViewer. set_snrDelay (snrDelay);
//	Some buttons should not be touched before we have a device
	connectGUI ();

	if (dabSettings -> value ("showDeviceWidget", 0).  toInt () != 0)
	   inputDevice -> show ();

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
	            this, SLOT (doStart (const QString &)));
	disconnect (deviceSelector, SIGNAL (activated (const QString &)),
	            this,  SLOT (newDevice (const QString &)));
	connect (deviceSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (newDevice (const QString &)));
//
//	secondariesVector. resize (0);	
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

	RadioInterface::~RadioInterface() {
	fprintf (stderr, "radioInterface is deleted\n");
}
//
/**
  */
void	RadioInterface::dumpControlState (QSettings *s) {
	if (s == nullptr)	// cannot happen
	   return;

	s	-> setValue ("channel", channelSelector -> currentText ());
	s	-> setValue ("device",
	                      deviceSelector -> currentText());
	s	-> setValue ("soundchannel",
	                               streamoutSelector -> currentText());
	if (inputDevice != nullptr)
           s    -> setValue ("deviceVisible",
                                  inputDevice -> isHidden () ? 0 : 1);
	s	-> setValue ("spectrumVisible",
	                          my_spectrumViewer. isHidden () ? 0 : 1);
	s	-> setValue ("tiiVisible",
	                          my_tiiViewer. isHidden () ? 0 : 1);
	s	-> setValue ("correlationVisible",
	                          my_correlationViewer. isHidden () ? 0 : 1);
	s	-> setValue ("snrVisible",
	                          my_snrViewer. isHidden () ? 0 : 1);
	s	-> sync();
}
//
///////////////////////////////////////////////////////////////////////////////
//	
//	a slot called by the ofdmprocessor
void	RadioInterface::set_CorrectorDisplay (int v) {
	correctorDisplay	-> display (v);
}
//
//	might be called when scanning only
void	RadioInterface::channel_timeOut() {
	No_Signal_Found();
}

///////////////////////////////////////////////////////////////////////////
//
//	a slot, called by the fic/fib handlers
void	RadioInterface::addtoEnsemble (const QString &serviceName,
	                                             int32_t SId) {
int	serviceOrder;
	if (!running. load())
	   return;

	(void)SId;
	serviceId ed;
	ed. name	= serviceName;
	ed. SId		= SId;
	if (isMember (serviceList, ed))
	   return;

	ed. subChId	= my_dabProcessor -> getSubChId (serviceName, SId);
	serviceOrder	=
	    dabSettings -> value ("serviceOrder", ALPHA_BASED). toInt ();

	serviceList = insert (serviceList, ed, serviceOrder);
	my_history -> addElement (channelSelector -> currentText (),
	                                                        serviceName);
	model. clear ();
	for (const auto serv : serviceList)
	   model. appendRow (new QStandardItem (serv. name));
        for (int i = 0; i < model. rowCount (); i ++) {
           model. setData (model. index (i, 0),
                      QFont (theFont, fontSize), Qt::FontRole);
        }

        ensembleDisplay -> setModel (&model);
}
//
//	The ensembleId is written as hexadecimal, however, the 
//	number display of Qt is only 7 segments ...
static
QString hextoString (int v) {
QString res;
	for (int i = 0; i < 4; i ++) {
	   uint8_t t = (v & 0xF000) >> 12;
	   QChar c = t <= 9 ? (char)('0' + t) : (char) ('A' + t - 10);
	   res. append (c);
	   v <<= 4;
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
	if (configWidget. scanmodeSelector -> currentIndex () == SCAN_TO_DATA)
	   stopScanning (false);
}
//
///////////////////////////////////////////////////////////////////////////

void	RadioInterface::handle_contentButton	() {
ensemblePrinter	my_Printer;
QString	utcTime		= convertTime (UTC. year, UTC. month, UTC. day,
	                               UTC. hour, UTC. minute);
QString	currentChannel	= channelSelector -> currentText ();
int	frequency	= inputDevice	-> getVFOFrequency ();

	if (!running. load() || (ensembleId -> text () == QString ("")))
	   return;
	if (scanning. load ())
	   return;

	FILE *fileP	=
	       filenameFinder. findContentDump_fileName (currentChannel);
	if (fileP	== nullptr)
	   return;
//
//	we asserted that my_dabProcessor exists
	my_Printer. showEnsembleData (currentChannel,
	                              frequency,
	                              utcTime,
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

//	fprintf (stderr, "handle_MOT: type %x (%x), name %s dir = %d\n",
//	                           contentType,
//	                           getContentBaseType ((MOTContentType)contentType),
//	                           name. toUtf8 (). data (), dirElement);
	switch (getContentBaseType ((MOTContentType)contentType)) {
	   case MOTBaseTypeGeneralData:
	      break;

	   case MOTBaseTypeText:
	      save_MOTtext (result, contentType, name);
	      break;

	   case MOTBaseTypeImage:
	      if (dirElement == 0)
	         show_MOTlabel (result, contentType, name, dirElement);
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
	      name  = QDir::toNativeSeparators (epgPath + name);
	      checkDir (name);
	      {  std::vector<uint8_t> epgData (result. begin(),
	                                                  result. end());
	         uint32_t ensembleId =
	                     my_dabProcessor -> get_ensembleId ();
	         uint32_t currentSId =
	                     extract_epg (name, serviceList, ensembleId);
	         fprintf (stderr, "currentSID = %X\n", currentSId);
	         if (currentSId != 0) {
	            FILE *f = fopen (name. toUtf8 (). data (), "w+b");
	            if (f == nullptr)
	               fprintf (stderr, "Opening %s failed\n",
	                                      name. toUtf8 (). data ());
	
	            fwrite (epgData. data (), 1, epgData. size (), f);
	            fclose (f);
	            epgProcessor. process_epg (epgData. data (), 
	                                       epgData. size (), currentSId);
	         }
//	         epgHandler. decode (epgData, realName);
	      }
//	      fprintf (stderr, "epg file %s\n",
//	                            realName. toUtf8 (). data ());
#endif
	      return;

	   case MOTBaseTypeProprietary:
	      break;
	}
}

void	RadioInterface::save_MOTtext (QByteArray result,
	                              int contentType,  QString name) {
	(void)contentType;
	if (filePath == "")
	   return;

	QString textName = QDir::toNativeSeparators (filePath + name);
	checkDir (textName);

	FILE *x = fopen (textName. toUtf8 (). data (), "w+b");
	if (x == nullptr)
	   fprintf (stderr, "cannot write file %s\n",
	                            textName. toUtf8 (). data ());
	else {
	   fprintf (stderr, "going to write file %s\n",
	                            textName. toUtf8(). data());
	   (void)fwrite (result. data (), 1, result.length(), x);
	   fclose (x);
	}
}

//	MOT slide, to show
void	RadioInterface::show_MOTlabel	(QByteArray data,
	                                 int contentType,
	                                 QString pictureName,
	                                 int dirs) {
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


	if (saveSlides) {
	   QString pict = QDir::toNativeSeparators (picturesPath + pictureName);
	   checkDir (pict);
	   FILE *x = fopen (pict. toUtf8 (). data (), "w+b");
	   if (x == nullptr)
	      fprintf (stderr, "cannot write file %s\n",
	                            pict. toUtf8 (). data ());
	   else {
	      fprintf (stderr, "going to write file %s\n",
	                            pict. toUtf8(). data());
	      (void)fwrite (data. data(), 1, data.length(), x);
	      fclose (x);
	   }
	}
	
	if (!currentService. is_audio)
	   return;

	if (dirs != 0)
	   return;

	QPixmap p;
	p. loadFromData (data, type);
	bool b	= dabSettings -> value ("motSlides", 0). toInt () == 1;
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
  *
  *	Response to a signal, so we presume that the signaling body exists
  *	signal may be pending though
  */
void	RadioInterface::changeinConfiguration () {
int	serviceOrder;
	if (!running. load () || my_dabProcessor == nullptr)
	   return;
	dabService s;
	if (currentService. valid) 
	   s = currentService;
	stopScanning    (false);
	stopService     ();
	fprintf (stderr, "change detected\n");
	serviceOrder	= 
	        dabSettings -> value ("serviceOrder", ALPHA_BASED). toInt ();
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
	   QFont (theFont, fontSize), Qt::FontRole);
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
//
//	In order to not overload with an enormous amount of
//	signals, we trigger this function at most 10 times a second
//
void	RadioInterface::newAudio	(int amount, int rate) {
	if (running. load ()) {
	   int16_t vec [amount];
	   while (audioBuffer. GetRingBufferReadAvailable() > amount) {
	      audioBuffer. getDataFromBuffer (vec, amount);
#ifdef	HAVE_PLUTO_RXTX
	      if (streamerOut != nullptr)
	         streamerOut	-> audioOut (vec, amount, rate);
#endif
	      if (!muting)
	         soundOut	-> audioOut (vec, amount, rate);
	   }
	}
}
//

/////////////////////////////////////////////////////////////////////////////
//	
/**
  *	\brief TerminateProcess
  *	Pretty critical, since there are many threads involved
  *	A clean termination is what is needed, regardless of the GUI
  */
void	RadioInterface::TerminateProcess () {
	if (scanning. load ())
	   stopScanning (false);
	running. store	(false);
	dumpControlState (dabSettings);
	hideButtons	();
#ifdef	DATA_STREAMER
	fprintf (stderr, "going to close the dataStreamer\n");
	delete		dataStreamer;
#endif
#ifdef	CLOCK_STREAMER
	fprintf (stderr, "going to close the clockstreamer\n");
	delete	clockStreamer;
#endif
	displayTimer.	stop	();
	channelTimer.	stop	();
	presetTimer.	stop	();
#ifdef	TRY_EPG
	epgTimer.	stop	();
#endif
	alarmTimer.	stop	();
	soundOut	-> stop ();
	if (dlTextFile != nullptr)
	   fclose (dlTextFile);
#ifdef	HAVE_PLUTO_RXTX
	if (streamerOut != nullptr)
	   streamerOut	-> stop ();
#endif
	if (my_dabProcessor != nullptr)
	   my_dabProcessor -> stop ();
	my_presetHandler. savePresets (presetSelector);
	theBand. saveSettings	();
	stop_frameDumping	();
	stop_sourceDumping	();
	stop_audioDumping	();
	theTable. hide		();
	theBand. hide		();
	dataDisplay. hide();
	if (motSlides != nullptr)
	   motSlides	-> hide ();
	LOG ("terminating ", "");
	usleep (1000);		// pending signals
#ifdef	__LOGGING__
	if (logFile != nullptr)
	   fclose (logFile);
	logFile	= nullptr;
#endif
//	everything should be halted by now

	dabSettings	-> sync ();
	my_spectrumViewer. hide ();
	my_correlationViewer. hide ();
	my_tiiViewer. hide ();
	my_snrViewer. hide ();
	if (my_dabProcessor != nullptr)
	   delete	my_dabProcessor;
	if (inputDevice != nullptr)
	   delete	inputDevice;

	delete		soundOut;
	if (motSlides != nullptr)
	   delete	motSlides;
	if (currentServiceDescriptor != nullptr)
	   delete currentServiceDescriptor;
//	delete	dataDisplay;
//	delete	configDisplay;
	delete	my_history;
	delete	my_timeTable;
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
	QString text = QString ("runtime ") + 
	               QString::number (numberHours) + " hr, "
	               + QString::number (numberMinutes) + " min";
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
//
//	The timer runs autonomously, so it might happen
//	that it rings when there is no processor running
	if (my_dabProcessor == nullptr)
	   return;

	if (error_report && (numberofSeconds % 10) == 0) {
	   int	totalFrames;
	   int	goodFrames;
	   int	badFrames;
	   my_dabProcessor	-> getFrameQuality (&totalFrames,
	                                            &goodFrames,
	                                            &badFrames);
	   fprintf (stderr, "total %d, good %d bad %d ficRatio %f\n",
	                     totalFrames, goodFrames, badFrames,
	                                            total_ficError * 100.0 / total_fics);
	   total_ficError	= 0;
	   total_fics		= 0;
#ifndef TCP_STREAMER 
#ifndef	QT_AUDIO
	   if (streamoutSelector -> isVisible ()) {
	      int xxx = ((audioSink *)soundOut)	-> missed();
	      fprintf (stderr, "missed %d\n", xxx);
	   }
#endif
#endif
	}
}
//
//	precondition: everything is quiet
deviceHandler	*RadioInterface::setDevice (const QString &s) {
QString	file;
deviceHandler	*inputDevice	= nullptr;
//	OK, everything quiet, now let us see what to do

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
#ifdef	HAVE_PLUTO
	if (s == "pluto") {
	   try {
	      inputDevice = new plutoHandler (dabSettings, version);
	      showButtons();
	   }
	   catch (int e) {
	      switch (e) {
	         case 22:
	            QMessageBox::warning (this, tr ("warning"),
	                                  tr ("failed to open libiio.dll\n"));
	            return nullptr;
	         case 23:
	            QMessageBox::warning (this, tr ("warning"),
	                                 tr ("error in loading functions\n"));
	            return nullptr;
	         case 24:
	            QMessageBox::warning (this, tr ("Warning"),
	                                  tr ("no pluto device found\n"));
	            return nullptr;
	         default:
	            QMessageBox::warning (this, tr ("Warning"),
	                                  tr ("some else with pluto\n"));
	            return nullptr;
	      }
	   }
	}
	else
#endif
#ifdef	HAVE_PLUTO_RXTX
	if (s == "pluto-rxtx") {
	   try {
	      inputDevice = new plutoHandler (dabSettings,
	                                      version, fmFrequency);
	      showButtons();
	      streamerOut = new dabStreamer (48000, 192000,
	                                       (plutoHandler *)inputDevice);
	      ((plutoHandler *)inputDevice)	-> startTransmitter (
	                                               fmFrequency);
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                                  tr ("no pluto device found\n"));
	      return nullptr;
	   }
	}
	else
#endif
#ifdef	HAVE_COLIBRI
	if (s == "colibri") {
	   try {
	      inputDevice = new colibriHandler (dabSettings, false);
	      showButtons();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                                  tr ("no colibri device found\n"));
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
#ifdef	HAVE_ELAD
	if (s == "elad-s1") {
	   try {
	      inputDevice = new eladHandler (dabSettings);
	      showButtons();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                                  tr ("no elad device found\n"));
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
	if (s == "xml files") {
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
	if ((s == "file input(.iq)") || (s == "file input(.raw)")) {
	   const char *p;
	   if (s == "file input (.iq)")
	      p = "iq data (*iq)";
	   else
	      p = "raw data (*raw)";
	   
	   file		= QFileDialog::getOpenFileName (this,
	                                                tr ("Open file ..."),
	                                                QDir::homePath(),
	                                                tr (p));
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
	if (s == "file input(.sdr)") {
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
//
//	It took some code, but it seems we have a device
	my_spectrumViewer. setBitDepth (inputDevice -> bitDepth());
//
//	do we want to see the widget for device control?
	if (dabSettings -> value ("deviceVisible", 0). toInt ()) {
	   inputDevice  -> show ();
	}
        else 
           inputDevice -> hide ();

	return inputDevice;
}
//
//	newDevice is called from the GUI when selecting a device
//	with the selector
void	RadioInterface::newDevice (const QString &deviceName) {
//	Part I : stopping all activities
	running. store (false);
	stopChannel	();
	disconnectGUI	();
	if (inputDevice != nullptr) {
	   delete inputDevice;
	   fprintf (stderr, "device is deleted\n");
	   inputDevice = nullptr;
	}
	LOG ("selecting ", 
	            deviceName. toUtf8 (). data ());
	fprintf (stderr, "going for a device %s\n", deviceName. toUtf8 (). data ());
	inputDevice		= setDevice (deviceName);
	if (inputDevice == nullptr) {
	   inputDevice = new deviceHandler ();
	   return;		// nothing will happen
	}
	if (dabSettings -> value ("deviceVisible", 1). toInt () != 0)
	   inputDevice -> show ();
	else
	   inputDevice -> hide ();
	doStart();		// will set running
}

void	RadioInterface::handle_devicewidgetButton	() {
	if (inputDevice == nullptr)
           return;

        if (inputDevice -> isHidden ()) {
           inputDevice -> show ();
        }
        else {
           inputDevice -> hide ();
        }

	dabSettings -> setValue ("deviceVisible",
	                      inputDevice -> isHidden () ? 0 : 1);
}

///////////////////////////////////////////////////////////////////////////
//	signals, received from ofdm_decoder for which that data is
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

#ifdef	CLOCK_STREAMER
	uint8_t	localBuffer [10];
	localBuffer [0] = 0xFF;
	localBuffer [1] = 0x00;
	localBuffer [2] = 0xFF;
	localBuffer [3] = 0x00;
	localBuffer [4] = (year & 0xFF00) >> 8;
	localBuffer [5] = year & 0xFF;
	localBuffer [6] = month;
	localBuffer [7] = day;
	localBuffer [8] = minutes;
	localBuffer [9] = seconds;
	if (running. load())
	   clockStreamer -> sendData (localBuffer, 10);
#endif
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
//
//	called from the MP4 decoder
void	RadioInterface::show_frameErrors (int s) {
	if (!running. load ()) 
	   return;
	QPalette p      = techData. frameError_display -> palette();
	if (100 - 4 * s < 80)
           p. setColor (QPalette::Highlight, Qt::red);
	else
           p. setColor (QPalette::Highlight, Qt::green);

	techData. frameError_display	-> setPalette (p);
	techData. frameError_display	-> setValue (100 - 4 * s);
}
//
//	called from the MP4 decoder
void	RadioInterface::show_rsErrors (int s) {
	if (!running. load ())		// should not happen
	   return;
	QPalette p      = techData. rsError_display -> palette();
	if (100 - 4 * s < 80)
           p. setColor (QPalette::Highlight, Qt::red);
	else
           p. setColor (QPalette::Highlight, Qt::green);
	techData. rsError_display	-> setPalette (p);
	techData. rsError_display	-> setValue (100 - 4 * s);
}
//
//	called from the aac decoder
void	RadioInterface::show_aacErrors (int s) {
	if (!running. load ())
	   return;

	QPalette p      = techData. aacError_display -> palette();
	if (100 - 4 * s < 80)
	   p. setColor (QPalette::Highlight, Qt::red);
	else
	   p. setColor (QPalette::Highlight, Qt::green);
	techData. aacError_display	-> setPalette (p);
	techData. aacError_display	-> setValue (100 - 4 * s);
}
//
//	called from the ficHandler
void	RadioInterface::show_ficSuccess (bool b) {
	if (!running. load ())	
	   return;

	if (b) 
	   ficSuccess ++;

	if (++ficBlocks >= 100) {
	   QPalette p      = ficError_display -> palette();
	   if (ficSuccess < 85)
              p. setColor (QPalette::Highlight, Qt::red);
	   else
	      p. setColor (QPalette::Highlight, Qt::green);

	   ficError_display	-> setPalette (p);
	   ficError_display	-> setValue (ficSuccess);
	   total_ficError	+= 100 - ficSuccess;
	   total_fics		+= 100;
	   ficSuccess		= 0;
	   ficBlocks		= 0;
	}
}
//
//	called from the PAD handler
void	RadioInterface::show_motHandling (bool b) {
	if (!running. load () || !b)
	   return;
	techData. motAvailable -> 
	            setStyleSheet (b ?
	                   "QLabel {background-color : green; color: white}":
	                   "QLabel {background-color : red; color : white}");
}
	
//	called from the dabProcessor
void	RadioInterface::show_snr (int s) {
	if (!running. load ())
	   return;
	snrDisplay	-> display (s);
	
	if (my_snrViewer. isHidden ()) {
	   snrBuffer. FlushRingBuffer ();
	   return;
	}

	int amount = snrBuffer. GetRingBufferReadAvailable ();
	if (amount <= 0)
	   return;

	float ss [amount];
	snrBuffer. getDataFromBuffer (ss, amount);
	for (int i = 0; i < amount; i ++) {
	   my_snrViewer. add_snr (ss [i]);
	}
	my_snrViewer. show_snr ();
}

//	just switch a color, called from the dabprocessor
void	RadioInterface::setSynced	(bool b) {
	if (!running. load() || (isSynced == b))
	   return;

	isSynced = b;
	syncedLabel	-> setStyleSheet (b ?
	   	              "QLabel {background-color: green; color : white}":
	   	              "QLabel {background-color: red; color : white}");
}
//
//	called from the PAD handler

void	RadioInterface::showLabel	(QString s) {
#ifdef	HAVE_PLUTO_RXTX
	if (streamerOut != nullptr)
	   streamerOut -> addRds (std::string (s. toUtf8 (). data ()));
#endif
	if (running. load())
	   dynamicLabel	-> setText (s);
	if (dlTextFile == nullptr)
	   return;
	if (the_dlCache. isMember (s))
	   return;
	the_dlCache. add (s);
	QString currentChannel = channelSelector -> currentText ();
	QDateTime theDateTime	= QDateTime::currentDateTime ();
	QTime theTime		= theDateTime. time ();
	fprintf (dlTextFile, "%s.%s %4d-%02d-%02d %02d:%02d:%02d  %s\n",
	                          currentChannel. toUtf8 (). data (),
	                          currentService. serviceName.
	                                          toUtf8 (). data (),
//	                          theTime. hour (), theTime. minute (),
	                          localTime. year,
	                          localTime. month,
	                          localTime. day,
	                          localTime. hour,
	                          localTime. minute,
	                          localTime. second,
	                                     s. toUtf8 (). data ());
}

void	RadioInterface::setStereo	(bool b) {
	if (!running. load ())
	   return;
	if (stereoSetting == b)
	   return;
	
	techData. stereoLabel	-> setStyleSheet (b ?
	   	         "QLabel {background-color: green; color : white}":
	   	         "QLabel {background-color: red; color : white}");
	techData. stereoLabel	-> setText (b ? "stereo" : "mono");
	stereoSetting = b;
}
//
//	In this version we show the spectrum even when the
//	mainId and subId did not change

static
QString	tiiNumber (int n) {
	if (n >= 10)
	   return QString::number (n);
	return QString ("0") + QString::number (n);
}

void	RadioInterface::show_tii_spectrum	() {
	my_tiiViewer. showSpectrum (1);
}

void	RadioInterface::show_tii	(int mainId, int subId) {
QString a = "Est: ";
bool	found	= false;
	if (mainId == 0xFF) 
	   return;
	for (int i = 0; i < transmitters. size (); i += 2) {
	   if ((transmitters. at (i) == (mainId & 0x7F)) &&
	       (transmitters. at (i + 1) == subId)) {
	      found = true;
	      break;
	   }
	}

	if (!found) {
	   transmitters. append (mainId & 0x7F);
	   transmitters. append (subId);
	}
        if (!running. load())
           return;

	a = a + " " +  tiiNumber (mainId) + " " + tiiNumber (subId);

	transmitter_coordinates	-> setAlignment (Qt::AlignRight);
	transmitter_coordinates	-> setText (a);
	my_tiiViewer. showTransmitters (transmitters);
}

void	RadioInterface::showSpectrum	(int32_t amount) {
	if (!running. load())
	   return;

	my_spectrumViewer. showSpectrum (amount,
				         inputDevice -> getVFOFrequency());
}

void	RadioInterface::showIQ		(int amount) {
	if (!running. load())
	   return;

	my_spectrumViewer. showIQ (amount);
}

void	RadioInterface::showQuality	(float q) {
	if (!running. load())
	   return;

	my_spectrumViewer. showQuality (q);
}
//
//	called from the MP4 decoder
void	RadioInterface::show_rsCorrections	(int c) {
	if (!running)
	   return;

	techData. rsCorrections	-> display (c);
}
//
//	called from the DAB processor
void	RadioInterface::show_clockError	(int e) {
	if (!running. load ())
	   return;

	my_spectrumViewer. show_clockErr (e);
}
//
//	called from the phasesynchronizer
void	RadioInterface::showCorrelation	(int amount, int marker) {
	if (!running. load())
	   return;
	my_correlationViewer. showCorrelation (amount, marker);
}

////////////////////////////////////////////////////////////////////////////

void	RadioInterface:: set_streamSelector (int k) {
	if (!running. load ())
	   return;
#if	not defined (TCP_STREAMER) &&  not defined (QT_AUDIO)
	((audioSink *)(soundOut)) -> selectDevice (k);
#else
	(void)k;
#endif
}

void	RadioInterface::switchVisibility (QWidget *w) {
	if (w -> isHidden ())
	   w  -> show ();
	else
	   w -> hide ();
}

void	RadioInterface::handle_detailButton	() {
	if (!running. load ())
	   return;
	switchVisibility (&dataDisplay);
	if (dataDisplay. isHidden ())
	   my_timeTable -> hide ();
}
//
//	Whenever the input device is a file, some functions,
//	e.g. selecting a channel, setting an alarm, are not
//	meaningful
void	RadioInterface::showButtons		() {
	scanButton		-> show ();
	channelSelector		-> show ();
	dumpButton		-> show	();
	nextChannelButton	-> show ();
	prevChannelButton	-> show ();
	frequencyDisplay	-> show ();
	presetSelector		-> show ();
}

void	RadioInterface::hideButtons		() {
	scanButton		-> hide ();
	channelSelector		-> hide ();
	dumpButton		-> hide	();
	nextChannelButton	-> hide ();
	prevChannelButton	-> hide ();
	frequencyDisplay	-> hide ();
	presetSelector		-> hide ();
}

void	RadioInterface::setSyncLost	() {
}

void	RadioInterface::set_picturePath		() {
QString defaultPath	= QDir::tempPath ();

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

void	setButtonFont (QPushButton *b, QString text, int size) {
	QFont font	= b -> font ();
	font. setPointSize (size);
	b		-> setFont (font);
	b		-> setText (text);
	b		-> update ();
}

void	RadioInterface::stop_sourceDumping	() {
	if (rawDumper == nullptr) 
	   return;

	my_dabProcessor	-> stopDumping();
	sf_close (rawDumper);
	rawDumper	= nullptr;
	setButtonFont (dumpButton, "Raw dump", 10);
}
//
void	RadioInterface::start_sourceDumping () {
QString deviceName	= inputDevice -> deviceName ();
QString channelName	= channelSelector -> currentText ();
	
	if (scanning. load ())
	   return;

	rawDumper	=
	         filenameFinder. findRawDump_fileName (deviceName, channelName);
	if (rawDumper == nullptr)
	   return;

	setButtonFont (dumpButton, "writing", 12);
	my_dabProcessor -> startDumping (rawDumper);
}

void	RadioInterface::handle_sourcedumpButton () {
	if (!running. load () || scanning. load ())
	   return;

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
	audioDumper	= nullptr;
	setButtonFont (techData. audiodumpButton, "audio dump", 10);
}

void	RadioInterface::start_audioDumping () {
	audioDumper	=
	      filenameFinder. findAudioDump_fileName  (serviceLabel -> text ());
	if (audioDumper == nullptr)
	   return;

	setButtonFont (techData. audiodumpButton, "writing", 12);
	soundOut	-> startDumping (audioDumper);
}

void	RadioInterface::handle_audiodumpButton () {
	if (!running. load () || scanning. load ())
	   return;

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
	frameDumper	=
	     filenameFinder. findFrameDump_fileName (serviceLabel -> text ());
	if (frameDumper == nullptr)
	   return;
	setButtonFont (techData. framedumpButton, "recording", 12);
}

void	RadioInterface::handle_framedumpButton () {
	if (!running. load () || scanning. load ())
	   return;

	if (frameDumper != nullptr) 
	   stop_frameDumping ();
	else
	   start_frameDumping ();
}
//
//	called from the mp4 handler, using a signal
void    RadioInterface::newFrame        (int amount) {
uint8_t buffer [amount];
	if (!running. load ())
	   return;

	if (frameDumper == nullptr) 
	   frameBuffer. FlushRingBuffer ();
	else
	while (frameBuffer. GetRingBufferReadAvailable () >= amount) {
	   frameBuffer. getDataFromBuffer (buffer, amount);
	   if (frameDumper != nullptr)
	      fwrite (buffer, amount, 1, frameDumper);
	}
}

void	RadioInterface::handle_tiiButton	() {
	if (!running. load ())
	   return;

	if (my_tiiViewer. isHidden())
	   my_tiiViewer. show();
	else
	   my_tiiViewer. hide();
}

void	RadioInterface::handle_correlationButton	() {
	if (!running. load ())
	   return;

	if (my_correlationViewer. isHidden())
	   my_correlationViewer. show();
	else
	   my_correlationViewer. hide();
}

void	RadioInterface::handle_spectrumButton	() {
	if (!running. load ())
	   return;

	if (my_spectrumViewer. isHidden())
	   my_spectrumViewer. show ();
	else
	   my_spectrumViewer. hide ();
}

void	RadioInterface::handle_snrButton	() {
	if (!running. load ())
	   return;

	if (my_snrViewer. isHidden ())
	   my_snrViewer. show ();
	else
	   my_snrViewer. hide ();
}

void    RadioInterface::handle_historyButton    () {
	if (!running. load ())
	   return;

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
void	RadioInterface::connectGUI	() {
	connect (contentButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_contentButton (void)));
	connect (detailButton, SIGNAL (clicked ()),
	         this, SLOT (handle_detailButton ()));
	connect (resetButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_resetButton (void)));
	connect	(scanButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_scanButton (void)));
	connect (show_tiiButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_tiiButton (void)));
	connect (show_correlationButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_correlationButton (void)));
	connect (show_spectrumButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_spectrumButton (void)));
	connect (snrButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_snrButton (void)));
        connect (devicewidgetButton, SIGNAL (clicked ()),
                 this, SLOT (handle_devicewidgetButton ()));
        connect (historyButton, SIGNAL (clicked ()),
                 this, SLOT (handle_historyButton ()));
	connect (dumpButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_sourcedumpButton (void)));

	connect (nextChannelButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_nextChannelButton (void)));
	connect	(prevChannelButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_prevChannelButton (void)));
	connect (prevServiceButton, SIGNAL (clicked ()),
                 this, SLOT (handle_prevServiceButton ()));
        connect (nextServiceButton, SIGNAL (clicked ()),
                 this, SLOT (handle_nextServiceButton ()));

	connect (techData. audiodumpButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_audiodumpButton (void)));
	connect (techData. framedumpButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_framedumpButton (void)));
//	connect (techData. muteButton, SIGNAL (clicked (void)),
//	         this, SLOT (handle_muteButton (void)));
	connect (muteButton, SIGNAL (clicked ()),
	         this, SLOT (handle_muteButton ()));

	connect (ensembleDisplay, SIGNAL (clicked (QModelIndex)),
	         this, SLOT (selectService (QModelIndex)));

	connect (configButton, SIGNAL (clicked ()),
	         this, SLOT (handle_configSetting ()));
	connect (configWidget. muteTimeSetting, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_muteTimeSetting (int)));
	connect (configWidget. snrDelaySetting, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_snrDelaySetting (int)));
	connect (configWidget. switchDelaySetting,
	                                 SIGNAL (valueChanged (int)),
	         this, SLOT (handle_switchDelaySetting (int)));
	connect (configWidget. orderAlfabetical, SIGNAL (clicked ()),
	         this, SLOT (handle_orderAlfabetical ()));
	connect (configWidget. orderServiceIds, SIGNAL (clicked ()),
	         this, SLOT (handle_orderServiceIds ()));
	connect (configWidget. ordersubChannelIds, SIGNAL (clicked ()),
	         this, SLOT (handle_ordersubChannelIds ()));
	connect (configWidget. alarmSelector,
	                           SIGNAL (activated (const QString &)),
	         this, SLOT (handle_alarmSelector (const QString &)));
	connect (configWidget. setTime_button, SIGNAL (clicked ()),
	         this, SLOT (handle_setTime_button ()));
	connect (configWidget. plotLengthSetting, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_plotLengthSetting (int)));
	connect (configWidget. scanmodeSelector,
	                            SIGNAL (currentIndexChanged (int)),
	         this, SLOT (handle_scanmodeSelector (int)));
	connect (configWidget. motslideSelector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_motslideSelector (int)));
	connect (configWidget. saveServiceSelector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_saveServiceSelector (int)));
	connect (configWidget. snrHeightSelector, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_snrHeightSelector (int)));
	connect (configWidget. snrLengthSelector, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_snrLengthSelector (int)));
	connect (configWidget. skipList_button, SIGNAL (clicked ()),
	         this, SLOT (handle_skipList_button ()));
	connect (configWidget. skipFile_button, SIGNAL (clicked ()),
	         this, SLOT (handle_skipFile_button ()));
}

void	RadioInterface::disconnectGUI() {
	disconnect (contentButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_contentButton (void)));
	disconnect (detailButton, SIGNAL (clicked ()),
	         this, SLOT (handle_detailButton ()));
	disconnect (resetButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_resetButton (void)));
	disconnect	(scanButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_scanButton (void)));
	disconnect (show_tiiButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_tiiButton (void)));
	disconnect (show_correlationButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_correlationButton (void)));
	disconnect (show_spectrumButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_spectrumButton (void)));
	disconnect (snrButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_snrButton (void)));
        disconnect (devicewidgetButton, SIGNAL (clicked ()),
                 this, SLOT (handle_devicewidgetButton ()));
        disconnect (historyButton, SIGNAL (clicked ()),
                 this, SLOT (handle_historyButton ()));
	disconnect (dumpButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_sourcedumpButton (void)));
	disconnect (nextChannelButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_nextChannelButton (void)));
	disconnect	(prevChannelButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_prevChannelButton (void)));
	disconnect (prevServiceButton, SIGNAL (clicked ()),
                 this, SLOT (handle_prevServiceButton ()));
        disconnect (nextServiceButton, SIGNAL (clicked ()),
                 this, SLOT (handle_nextServiceButton ()));

	disconnect (techData. audiodumpButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_audiodumpButton (void)));
	disconnect (techData. framedumpButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_framedumpButton (void)));
//	disconnect (techData. muteButton, SIGNAL (clicked (void)),
//	         this, SLOT (handle_muteButton (void)));
	disconnect (muteButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_muteButton (void)));

	disconnect (ensembleDisplay, SIGNAL (clicked (QModelIndex)),
	         this, SLOT (selectService (QModelIndex)));

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
	disconnect (configWidget. alarmSelector,
	                           SIGNAL (activated (const QString &)),
	            this, SLOT (handle_alarmSelector (const QString &)));
	disconnect (configWidget. setTime_button, SIGNAL (clicked ()),
	            this, SLOT (handle_setTime_button ()));
	disconnect (configWidget. plotLengthSetting,
	                                         SIGNAL (valueChanged (int)),
	            this, SLOT (handle_plotLengthSetting (int)));
	disconnect (configWidget. scanmodeSelector, SIGNAL (currentIndexChanged (int)),
	            this, SLOT (handle_scanmodeSelector (int)));
	disconnect (configWidget. motslideSelector, SIGNAL (stateChanged (int)),
	            this, SLOT (handle_motslideSelector (int)));
	disconnect (configWidget. saveServiceSelector, SIGNAL (stateChanged (int)),
	            this, SLOT (handle_saveServiceSelector (int)));
	disconnect (configWidget. snrHeightSelector, SIGNAL (valueChanged (int)),
	            this, SLOT (handle_snrHeightSelector (int)));
	disconnect (configWidget. snrLengthSelector, SIGNAL (valueChanged (int)),
	            this, SLOT (handle_snrLengthSelector (int)));
	disconnect (configWidget. skipList_button, SIGNAL (clicked ()),
	            this, SLOT (handle_skipList_button ()));
	disconnect (configWidget. skipFile_button, SIGNAL (clicked ()),
	            this, SLOT (handle_skipFile_button ()));
}
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
	   TerminateProcess ();
	   event -> accept();
	}
}

bool	RadioInterface::eventFilter (QObject *obj, QEvent *event) {
	if (!running. load ())
	   return QWidget::eventFilter (obj, event);

	if (my_dabProcessor == nullptr) {
	   fprintf (stderr, "expert error 5\n");
	   return true;
	}

	if (event -> type () == QEvent::KeyPress) {
	   QKeyEvent *ke = static_cast <QKeyEvent *> (event);
	   if (ke -> key () == Qt::Key_Return) {
	      presetTimer. stop ();
	      nextService. valid = false;
	      QString serviceName =
	         ensembleDisplay -> currentIndex ().
	                             data (Qt::DisplayRole). toString ();
	      if (currentService. serviceName != serviceName) {
	         fprintf (stderr, "currentservice = %s (%d)\n",
	                  currentService. serviceName. toUtf8 (). data (),
                                        currentService. valid);
                 stopService ();
                 selectService (ensembleDisplay -> currentIndex ());
	         stopScanning (false);
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
	if (!running. load ())
	   return;

	if (name == serviceLabel -> text ()) {
	   serviceLabel	-> setStyleSheet ("QLabel {color : red}");
	   fprintf (stderr, "announcement for %s (%d) starts\n",
	                             name. toUtf8 (). data (), subChId);
	}
}

void	RadioInterface::stopAnnouncement (const QString &name, int subChId) {
	(void)subChId;
	if (!running. load ())
	   return;

	if (name == serviceLabel -> text ()) {
	   serviceLabel ->
	              setStyleSheet ("QLabel {color : black}");
	   fprintf (stderr, "end for announcement service %s\n",
	                              name. toUtf8 (). data ());
	}
}

////////////////////////////////////////////////////////////////////////
//
//	preset selection, either from presets or from history
////////////////////////////////////////////////////////////////////////

void    RadioInterface::handle_historySelect (const QString &s) {
	if (!running. load ())
	   return;

        presetTimer. stop ();
        localSelect (s);
}

void    RadioInterface::handle_presetSelector (const QString &s) {
	if (!running. load ())
	   return;

        presetTimer. stop ();
        if ((s == "Presets") || (presetSelector -> currentIndex () == 0))
           return;
	fprintf (stderr, "going for %s\n", s. toUtf8 (). data ());
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
	if (my_dabProcessor == nullptr) {
	   fprintf (stderr, "Expert error 21\n");
	   return;
	}

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
	stopChannel ();
//      trying to start the selected service
	int k           = channelSelector -> findText (channel);
	if (k != -1) {
	   new_channelIndex (k);
	}
	else {
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("Incorrect preset\n"));
	   return;
	}

	nextService. valid = true;
        nextService. serviceName        = service;
        nextService. SId                = 0;
        nextService. SCIds              = 0;
        presetTimer. setSingleShot (true);
	switchDelay			=
	                  dabSettings -> value ("switchDelay", 8). toInt ();
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
	channelTimer. stop ();
	stop_muting	();

	techData. timeTable_button -> hide ();
	my_timeTable	-> hide ();

	if (my_dabProcessor == nullptr) {
	   fprintf (stderr, "Expert error 22\n");
	   return;
	}

	if (currentService. valid) {
	   QString serviceName = currentService. serviceName;
	   if (my_dabProcessor -> is_audioService (serviceName)) {
	      audiodata ad;
	      my_dabProcessor -> dataforAudioService (serviceName, &ad);
	      my_dabProcessor -> stopService (&ad);
	      soundOut	-> stop ();
	      for (int i = 0; i < 10; i ++) {
	         packetdata pd;
                 my_dabProcessor -> dataforPacketService (ad. serviceName, &pd, i);
                 if (pd. defined) {
                    my_dabProcessor -> stopService (&pd);
                    break;
                 }
	      }
	   }
	   else {
	      packetdata pd;
	      my_dabProcessor -> dataforPacketService (serviceName, &pd, 0);
	      if (pd. defined)
	         my_dabProcessor -> stopService (&pd);
	   }

	   for (int i = 0; i < model. rowCount (); i ++) {
	      QString itemText =
	          model. index (i, 0). data (Qt::DisplayRole). toString ();
	      if (itemText == serviceName) {
	         colorService (model. index (i, 0), Qt::black, fontSize);
	         break;
	      }
	   }
	}
	cleanScreen	();
}
//
void	RadioInterface::selectService (QModelIndex ind) {
QString	currentProgram = ind. data (Qt::DisplayRole). toString();
	if (!running. load ())
	   return;

	if (my_dabProcessor == nullptr) {	// should/can not happen
	   fprintf (stderr, "Expert error 7\n");
	   fprintf (stderr, "no service should be visible\n");
	   return;
	}

	presetTimer.	stop	();
	channelTimer.	stop	();
	stopScanning	(false);

	stopService 	();		// if any

	dabService s;
	s. serviceName = currentProgram;
	my_dabProcessor -> getParameters (currentProgram, &s. SId, &s. SCIds);
	if (s. SId == 0) {
	   QMessageBox::warning (this, tr ("Warning"),
 	                         tr ("insufficient data for this program\n"));	
	   return;
	}

	startService (&s);
}
//
void	RadioInterface::startService (dabService *s) {
QString serviceName	= s -> serviceName;

	currentService		= *s;
	currentService. valid	= false;
	LOG ("start service ", serviceName. toUtf8 (). data ());
	LOG ("service has SNR ", QString::number (snrDisplay -> value ()));
	techData. timeTable_button -> hide ();
	int rowCount	= model. rowCount ();
	for (int i = 0; i < rowCount; i ++) {
	   QString itemText =
	           model. index (i, 0). data (Qt::DisplayRole). toString ();
	   if (itemText == serviceName) {
	      colorService (model. index (i, 0), Qt::red, fontSize + 4);
	      serviceLabel	-> setStyleSheet ("QLabel {color : black}");
	      serviceLabel	-> setText (serviceName);
	      audiodata ad;
	      
	      my_dabProcessor -> dataforAudioService (serviceName, &ad);
	      if (ad. defined) {
	         currentService. valid		= true;
	         currentService. is_audio	= true;
	         if (my_dabProcessor -> has_timeTable (ad. SId))
	            techData. timeTable_button -> show ();
	         start_audioService (&ad);
	         dabSettings	-> setValue ("presetname", serviceName);
	      }
	      else
	      if (my_dabProcessor -> is_packetService (serviceName)) {
	         currentService. valid		= true;
	         currentService. is_audio	= false;
	         start_packetService (serviceName);
	         dabSettings	-> setValue ("presetname", "");
	      }
	      else {
	         fprintf (stderr, "%s is not clear\n",
	                            serviceName. toUtf8 (). data ());
	         dabSettings	-> setValue ("presetname", "");
	      }
	      return;
	   }
	}
}

void    RadioInterface::colorService (QModelIndex ind, QColor c, int pt) {
	QMap <int, QVariant> vMap = model. itemData (ind);
	vMap. insert (Qt::ForegroundRole, QVariant (QBrush (c)));
	model. setItemData (ind, vMap);
	model. setData (ind, QFont (theFont, pt), Qt::FontRole);
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
        techData. pictureLabel -> hide ();

	new_presetIndex (0);
	techData. stereoLabel	-> setStyleSheet (
	   	         "QLabel {background-color: red; color : black}");
	techData. stereoLabel	-> setText ("");
	stereoSetting			= false;
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

void	RadioInterface::start_audioService (audiodata *ad) {
	if (!ad ->  defined) {
	   QMessageBox::warning (this, tr ("Warning"),
 	                         tr ("insufficient data for this program\n"));
	   return;
	}

	QDateTime theDateTime	= QDateTime::currentDateTime ();
	QTime theTime		= theDateTime. time ();
	fprintf (stderr, "we start %s at %.2d:%.2d\n",
	                        ad -> serviceName. toUtf8 (). data (),
	                        theTime. hour (), theTime. minute ());
	serviceLabel -> setAlignment(Qt::AlignCenter);
	serviceLabel -> setText (ad -> serviceName);
	currentService. valid	= true;

	(void)my_dabProcessor -> set_audioChannel (ad, &audioBuffer);
	for (int i = 1; i < 10; i ++) {
	   packetdata pd;
	   my_dabProcessor -> dataforPacketService (ad -> serviceName, &pd, i);
	   if (pd. defined) {
	      my_dabProcessor -> set_dataChannel (&pd, &dataBuffer);
	      fprintf (stderr, "adding %s (%d) as subservice\n",
	                            pd. serviceName. toUtf8 (). data (),
	                            pd. subchId);
	      break;
	   }
	}
//	activate sound
	soundOut -> restart ();
//	show service related data
	techData. programName		-> setText (ad -> serviceName);
	techData. serviceIdDisplay	-> display (ad -> SId);
	techData. bitrateDisplay 	-> display (ad -> bitRate);
	techData. startAddressDisplay 	-> display (ad -> startAddr);
	techData. lengthDisplay		-> display (ad -> length);
	techData. subChIdDisplay 	-> display (ad -> subchId);
	QString protL	= getProtectionLevel (ad -> shortForm,
	                                         ad -> protLevel);
	techData. uepField		-> setText (protL);
	techData. ASCTy			-> setText (ad -> ASCTy == 077 ?
	                                                  "DAB+" : "DAB");
	if (ad -> ASCTy == 077) {
	   techData. rsError_display	-> show ();
	   techData. aacError_display	-> show ();
	}
	else {
	   techData. rsError_display	-> hide	();
	   techData. aacError_display	-> hide ();
	}
	techData. codeRate ->
	             setText (getCodeRate (ad -> shortForm,
	                                              ad -> protLevel));
	techData. language ->
	        setText (getLanguage (ad -> language));
	techData. programType ->
	   setText (the_textMapper.
	               get_programm_type_string (ad -> programType));
	if (ad -> fmFrequency == -1) {
	   techData. fmFrequency	-> hide ();
	   techData. fmLabel		-> hide	();
	}
	else {
	   techData. fmLabel		-> show ();
	   techData. fmFrequency	-> show ();
	   QString f = QString::number (ad -> fmFrequency);
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

	if (!my_dabProcessor -> set_dataChannel (&pd, &dataBuffer)) {
	   QMessageBox::warning (this, tr ("sdr"),
 	                         tr ("could not start this service\n"));
	   return;
	}

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

void	RadioInterface::handle_prevServiceButton	() {
	disconnect (prevServiceButton, SIGNAL (clicked ()),
	            this, SLOT (handle_prevServiceButton ()));
	handle_serviceButton (BACKWARDS);
	connect (prevServiceButton, SIGNAL (clicked ()),
	         this, SLOT (handle_prevServiceButton ()));
}

void	RadioInterface::handle_nextServiceButton	() {
	disconnect (nextServiceButton, SIGNAL (clicked ()),
	            this, SLOT (handle_nextServiceButton ()));
	handle_serviceButton (FORWARD);
	connect (nextServiceButton, SIGNAL (clicked ()),
	         this, SLOT (handle_nextServiceButton ()));
}

//	Previous and next services. trivial implementation
void	RadioInterface::handle_serviceButton	(direction d) {
	if (!running. load ())
	   return;

	presetTimer. stop ();
	nextService. valid	= false;
	stopScanning (false);
	if (!currentService. valid)
	   return;

	QString oldService	= currentService. serviceName;
	
	stopService  ();

	if ((serviceList. size () != 0) && (oldService != "")) {
	   for (int i = 0; i < (int)(serviceList. size ()); i ++) {
	      if (serviceList. at (i). name == oldService) {
	         colorService (model. index (i, 0), Qt::black, fontSize);
	         if (d == FORWARD) {
	            i = (i + 1) % serviceList. size ();
	         }
	         else {
	            i = (i - 1 + serviceList. size ()) % serviceList. size ();
	         }
	         dabService s;
	         my_dabProcessor ->
	                  getParameters (serviceList. at (i). name,
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
}

////////////////////////////////////////////////////////////////////////////
//
//	The user(s)
///////////////////////////////////////////////////////////////////////////

void	RadioInterface::setPresetStation () {
	if (!running. load ())
	   return;

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
	      fprintf (stderr, "going to select %s\n", presetName. toUtf8 (). data ());
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
	nextService. valid = false;
//
//	not found, no service selected
	fprintf (stderr, "presetName %s not found\n", presetName. toUtf8 (). data ());
}

///////////////////////////////////////////////////////////////////////////
//
//	Channel basics
///////////////////////////////////////////////////////////////////////////
//	Precondition: no channel should be active
//	
void	RadioInterface::startChannel (const QString &channel) {
int	tunedFrequency	=
	         theBand. Frequency (channel);
	frequencyDisplay	-> display (tunedFrequency / 1000000.0);
	dabSettings		-> setValue ("channel", channel);
	inputDevice		-> resetBuffer ();
	serviceList. clear ();
	model. clear ();
	ensembleDisplay		-> setModel (&model);
	cleanScreen	();
	inputDevice		-> restartReader (tunedFrequency);
	my_dabProcessor		-> start (tunedFrequency);
	show_for_safety ();
	int	switchDelay	=
                          dabSettings -> value ("switchDelay", 8). toInt ();
#ifdef	TRY_EPG
	if (!scanning. load ())
           epgTimer. start (switchDelay * 1000);
#endif
}
//
//	apart from stopping the reader, a lot of administration
//	is to be done.
void	RadioInterface::stopChannel	() {
	if (inputDevice == nullptr)		// should not happen
	   return;
	stop_sourceDumping	();
	stop_audioDumping	();
        soundOut	-> stop ();
	stop_muting		();
	epgLabel	-> hide ();
//	note framedumping - if any - was already stopped
#ifdef	TRY_EPG
	epgTimer. stop		();
	techData. timeTable_button -> hide ();
	my_timeTable	-> hide ();
#endif
	presetTimer. stop 	();
        channelTimer. stop	();
//
//	The services - if any - need to be stopped
	hide_for_safety	();	// hide some buttons
	my_dabProcessor		-> stop ();
	inputDevice		-> stopReader ();
	my_tiiViewer. clear();
	QCoreApplication::processEvents ();
//
//	no processing left at this time
	usleep (1000);		// may be handling pending signals?
	currentService. valid	= false;
	nextService. valid	= false;

//	all stopped, now look at the GUI elements
	ficError_display	-> setValue (0);
//	the visual elements related to service and channel
	setSynced	(false);
	ensembleId	-> setText ("");
	transmitter_coordinates	-> setText (" ");
	transmitters. clear ();
//	if (serviceList. size () > 0) {
	   serviceList. clear ();
	   model. clear ();
//	   ensembleDisplay	-> blockSignals (true);
	   ensembleDisplay	-> setModel (&model);
//	   ensembleDisplay	-> blockSignals (false);
	   cleanScreen	();
//	}
}

//
/////////////////////////////////////////////////////////////////////////
//
//	next- and previous channel buttons
/////////////////////////////////////////////////////////////////////////

void    RadioInterface::selectChannel (const QString &channel) {
	if (!running. load ())
	   return;

	LOG ("select channel ", channel. toUtf8 (). data ());
	presetTimer. stop ();
	presetSelector		-> setCurrentIndex (0);
	stopScanning	(false);
	stopChannel	();
	startChannel	(channel);
}

void	RadioInterface::handle_nextChannelButton () {
int	nrChannels	= channelSelector -> count ();
int	newChannel	= channelSelector -> currentIndex () + 1;
	set_channelButton (newChannel % nrChannels);
}

void	RadioInterface::handle_prevChannelButton () {
int	nrChannels	= channelSelector -> count ();
	if (channelSelector -> currentIndex () == 0)
	   set_channelButton (nrChannels - 1);
	else
	   set_channelButton (channelSelector -> currentIndex () - 1);
}

void	RadioInterface::set_channelButton (int currentChannel) {
	if (!running. load ())
	   return;

	presetTimer. stop ();
	if (my_dabProcessor == nullptr) {
	   fprintf (stderr, "Expert error 23\n");
	   abort ();
	}

	stopScanning (false);
	stopChannel	();
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
int	scanMode	= configWidget. scanmodeSelector -> currentIndex ();
	presetTimer. stop ();
	channelTimer. stop ();
#ifdef	TRY_EPG
	epgTimer. stop ();
#endif
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
	   cc = theBand. firstChannel ();
	}
        scanning. store (true);
	if (scanMode == SINGLE_SCAN)
	   scanDumpFile	= filenameFinder. findScanDump_fileName ();
	else
	if (scanMode == SCAN_CONTINUOUSLY)
	   scanDumpFile	= filenameFinder. findSummary_fileName ();
	else
	   scanDumpFile = nullptr;

	my_dabProcessor	-> set_scanMode (true);
//      To avoid reaction of the system on setting a different value:
	new_channelIndex (cc);
	dynamicLabel	-> setText ("scan mode \"" +
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
        scanButton      -> setText ("scan");

	my_dabProcessor	-> set_scanMode (false);
	if (!running. load () || !scanning. load ())
           return;
	dynamicLabel	-> setText ("Scan ended");
        channelTimer. stop ();
	scanning. store (false);
	if (scanDumpFile != nullptr) {
	   fclose (scanDumpFile);
	   scanDumpFile = nullptr;
	}

//	theTable. hide ();
}

//	If the ofdm processor has waited - without success -
//	for a period of N frames to get a start of a synchronization,
//	it sends a signal to the GUI handler
//	If "scanning" is "on" we hop to the next frequency on
//	the list.
//	Also called as a result of time out on channelTimer

void	RadioInterface::No_Signal_Found () {
int	switchDelay;
int	scanMode	= configWidget. scanmodeSelector -> currentIndex ();

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
//	   if (scanMode != SCAN_TO_DATA)
	   cc = theBand. nextChannel (cc);
	   if ((cc >= channelSelector -> count ()) &&
	                               (scanMode == SINGLE_SCAN)) {
	      stopScanning (true);
	   }
	   else {  // we just continue
	      if (cc >= channelSelector -> count ())
	         cc = theBand. firstChannel ();
//	To avoid reaction of the system on setting a different value:
	      new_channelIndex (cc);

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
	   stopScanning (false);
}

////////////////////////////////////////////////////////////////////////////
//
// showServices
////////////////////////////////////////////////////////////////////////////
	
void	RadioInterface::showServices () {
ensemblePrinter	my_Printer;
int	scanMode	= configWidget. scanmodeSelector -> currentIndex ();
QString SNR 		= "SNR " + QString::number (snrDisplay -> value ());
QString ensembleId	= hextoString (my_dabProcessor -> get_ensembleId ());

	if (my_dabProcessor == nullptr) {	// cannot happen
	   fprintf (stderr, "Expert error 26\n");
	   return;
	}
	theTable. newEnsemble (" ",
	                       channelSelector -> currentText (),
	                       my_dabProcessor	-> get_ensembleName (),
	                       ensembleId,
	                       SNR,
	                       transmitters);
	if (scanMode == SINGLE_SCAN) {
	   theTable. new_headline ();
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
                                   d. ASCTy == 077 ? "DAB+ (plus)" : "DAB",
                                   bitRate, protL, codeRate);
	   }

	   for (serviceId serv: serviceList) {
	      QString packetService = serv. name;
	      packetdata d;
	      my_dabProcessor -> dataforPacketService (packetService, &d, 0);
	      if (!d. defined)
	         continue;

	      QString soort	= d. DSCTy == 60 ? "mot data" :
	                             d. DSCTy == 59 ? "ip data" :
	                             d. DSCTy == 44 ? "journaline data" :
	                             d. DSCTy ==  5 ? "tdc data" :
	                                                "unknow data";
	      QString serviceId;
	      serviceId. setNum (d. SId, 16);
	      QString bitRate   = QString::number (d. bitRate);
	      QString protL     = getProtectionLevel (d. shortForm,
                                                   d. protLevel);
	      QString codeRate  = getCodeRate (d. shortForm,
                                            d. protLevel);
	      theTable.
                  add_to_Ensemble (packetService, serviceId,
	                           soort,
                                   bitRate, protL, codeRate);
	   }
	}
	if (scanDumpFile != nullptr) {
	   QString utcTime	= convertTime (UTC. year, UTC.month,
	                                       UTC. day, UTC. hour, 
	                                       UTC. minute);
	   if (scanMode == SINGLE_SCAN)
	      my_Printer. showEnsembleData (channelSelector -> currentText (),
	                                    inputDevice -> getVFOFrequency (),
	                                    utcTime,
	                                    transmitters,
	                                    serviceList,
	                                    my_dabProcessor, scanDumpFile);
	   else
	   if (scanMode == SCAN_CONTINUOUSLY)
	      my_Printer. showSummaryData (channelSelector -> currentText (),
	                                   inputDevice -> getVFOFrequency (),
	                                   SNR,
	                                   utcTime,
	                                   transmitters,
	                                   serviceList,
	                                   my_dabProcessor,
	                                   scanDumpFile);
	}
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

	if (!inserted)
	   k. push_back (n);
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
//	   setButtonFont (techData. muteButton, "mute", 10);
	   setButtonFont (muteButton, "mute", 10);
	   stillMuting	-> hide ();
           muting = false;
	}
}

void	RadioInterface::stop_muting		() {
	if (!muting) 
	   return;

	muteTimer. stop ();
	disconnect (&muteTimer, SIGNAL (timeout ()),
	               this, SLOT (muteButton_timeOut ()));
//	setButtonFont (techData. muteButton, "mute", 10);
	setButtonFont (muteButton, "mute", 10);
	muting = false;
	stillMuting	-> hide ();
}

void	RadioInterface::handle_muteButton	() {
	if (muting) {
	   stop_muting ();
	   return;
	}

	connect (&muteTimer, SIGNAL (timeout (void)),
                 this, SLOT (muteButton_timeOut (void)));
	muteDelay	= dabSettings -> value ("muteTime", 2). toInt ();
	muteDelay	*= 60;
        muteTimer. start (1000);
//	setButtonFont (techData. muteButton, "MUTING", 12);
	setButtonFont (muteButton, "MUTING", 12);
	stillMuting	-> show ();
	stillMuting	-> display (muteDelay);
	muting = true;
}
//

void	RadioInterface::new_presetIndex (int index) {
	if (presetSelector -> currentIndex () == index)
	   return;
	presetSelector -> blockSignals (true);
	set_newPresetIndex (index);
        while (presetSelector -> currentIndex () != 0)
           usleep (200);
	presetSelector	-> blockSignals (false);
}

void	RadioInterface::new_channelIndex (int index) {
	if (channelSelector -> currentIndex () == index)
	   return;
	channelSelector	-> blockSignals (true);
	set_newChannel (index);
	while (channelSelector -> currentIndex () != index)
	   usleep (2000);
	channelSelector	-> blockSignals (false);
}
//
/////////////////////////////////////////////////////////////////////////
//	merely as a gadget, for each button the color can be set
//	Lots of code, just for a gadget
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
QString snrButton_color =
	   dabSettings -> value (SNR_BUTTON + "_color",
	                                              "white"). toString ();
QString snrButton_font =
	   dabSettings -> value (SNR_BUTTON + "_font",
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
QString muteButton_color =
	   dabSettings -> value (MUTE_BUTTON + "_color",
	                                              "white"). toString ();
QString muteButton_font =
	   dabSettings -> value (MUTE_BUTTON + "_font",
	                                              "black"). toString ();

QString prevChannelButton_color =
	   dabSettings -> value (PREVCHANNEL_BUTTON + "_color",
	                                              "black"). toString ();
QString prevChannelButton_font =
	   dabSettings -> value (PREVCHANNEL_BUTTON + "_font",
	                                              "white"). toString ();
QString nextChannelButton_color =
	   dabSettings -> value (NEXTCHANNEL_BUTTON + "_color",
	                                              "black"). toString ();
QString nextChannelButton_font =
	   dabSettings -> value (NEXTCHANNEL_BUTTON + "_font",
	                                              "white"). toString ();
QString prevServiceButton_color =
	   dabSettings -> value (PREVSERVICE_BUTTON + "_color",
	                                              "blaCK"). toString ();
QString prevServiceButton_font =
	   dabSettings -> value (PREVSERVICE_BUTTON + "_font",
	                                              "white"). toString ();
QString nextServiceButton_color =
	   dabSettings -> value (NEXTSERVICE_BUTTON + "_color",
	                                              "black"). toString ();
QString nextServiceButton_font =
	   dabSettings -> value (NEXTSERVICE_BUTTON + "_font",
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

QString	configButton_color =
	   dabSettings -> value (CONFIG_BUTTON + "_color",
	                                              "black"). toString ();
QString configButton_font	=
	   dabSettings -> value (CONFIG_BUTTON + "_font",
	                                              "white"). toString ();
QString	dlTextButton_color =
	   dabSettings -> value (DLTEXT_BUTTON + "_color",
	                                              "black"). toString ();
QString dlTextButton_font	=
	   dabSettings -> value (DLTEXT_BUTTON + "_font",
	                                              "white"). toString ();
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
	snrButton	-> setStyleSheet (temp. arg (snrButton_color,
	                                             snrButton_font));
	devicewidgetButton -> setStyleSheet (temp. arg (devicewidgetButton_color,
	                                                devicewidgetButton_font));

	historyButton	-> setStyleSheet (temp. arg (historyButton_color,
	                                             historyButton_font));
	dumpButton	-> setStyleSheet (temp. arg (dumpButton_color,
	                                             dumpButton_font));
	configButton	-> setStyleSheet (temp. arg (configButton_color,
	                                             configButton_font));

	prevChannelButton -> setStyleSheet (temp. arg (prevChannelButton_color,
	                                               prevChannelButton_font));
	nextChannelButton -> setStyleSheet (temp. arg (nextChannelButton_color,
	                                               nextChannelButton_font));
	prevServiceButton -> setStyleSheet (temp. arg (prevServiceButton_color,
	                                               prevServiceButton_font));
	nextServiceButton -> setStyleSheet (temp. arg (nextServiceButton_color,
	                                               nextServiceButton_font));

	techData. framedumpButton ->
	                     setStyleSheet (temp. arg (framedumpButton_color,
	                                               framedumpButton_font));
	techData. audiodumpButton ->
	                     setStyleSheet (temp. arg (audiodumpButton_color,
	                                               audiodumpButton_font));
//	techData. muteButton	-> setStyleSheet (temp. arg (muteButton_color,
	muteButton	-> setStyleSheet (temp. arg (muteButton_color,
	                                             muteButton_font));
	dlTextButton	-> setStyleSheet (temp. arg (dlTextButton_color,
	                                             dlTextButton_font));
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

void	RadioInterface::color_snrButton		() {
	set_buttonColors (snrButton, SNR_BUTTON);
}

void	RadioInterface::color_devicewidgetButton	() {
	set_buttonColors (devicewidgetButton, DEVICEWIDGET_BUTTON);
}

void	RadioInterface::color_historyButton	()	{
	set_buttonColors (historyButton, HISTORY_BUTTON);
}

void	RadioInterface::color_sourcedumpButton	()	{
	set_buttonColors (dumpButton, DUMP_BUTTON);
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

void	RadioInterface::color_muteButton	()	{
//	set_buttonColors (techData. muteButton, MUTE_BUTTON);
	set_buttonColors (muteButton, MUTE_BUTTON);
}

void	RadioInterface::color_configButton	()	{
	set_buttonColors (configButton, CONFIG_BUTTON);
}

void	RadioInterface::color_dlTextButton	()	{
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
	if (configDisplay . isHidden ()) 
	   configDisplay. show ();
	else {
	   theBand. hide ();
	   configDisplay. hide ();
	}
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
	dabSettings	-> setValue ("scanMode", d); 
}

void	RadioInterface::handle_motslideSelector		(int d) {
	(void)d;
	dabSettings	-> setValue ("motSlides",
	                              configWidget. motslideSelector -> isChecked () ? 1 : 0);
}

void	RadioInterface::handle_saveServiceSelector	(int d) {
	(void)d;
	dabSettings	-> setValue ("has-presetName",
	                             configWidget. saveServiceSelector -> isChecked () ? 1 : 0);
}

void	RadioInterface::handle_snrHeightSelector	(int d) {
	my_snrViewer. setHeight (d);
}

void	RadioInterface::handle_snrLengthSelector	(int d) {
	my_snrViewer. setLength (d);
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
//	Handling alarm
struct  {
QString		alarmService;
int		targetHour;
int		targetMinute;
} alarmData;

void	RadioInterface::handle_alarmSelector	(const QString &s) {
	alarmTimer. stop ();
	alarmLabel	-> hide ();

	if (s == "set alarm") {
	   QStringList candidates;
	   alarmSelector theSelector;
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

           int selected			= theSelector. QDialog::exec ();
//	   fprintf (stderr, "selected item %d\n", selected);
	   alarmData. alarmService	= candidates. at (selected);
	   alarmData. targetHour	= configWidget.
	                                       alarm_hours -> value ();
	   alarmData. targetMinute	= configWidget.
	                                       alarm_minutes -> value ();

	   QDateTime theDateTime	= QDateTime::currentDateTime ();
//	   QDate theDate		= theDateTime. date ();
	   QTime theTime		= theDateTime. time ();
	   if ((theTime. hour () > alarmData. targetHour) && (
	          alarmData. targetHour < 12))
	      alarmData. targetHour = alarmData. targetHour + 12;
	   if (alarmData. targetHour * 60 + alarmData. targetMinute <
	               theTime. hour () * 60 + theTime. minute () - 1) {
	      QMessageBox::warning (this, tr ("Warning"),
                             tr ("cannot set alarm in the past\n"));
	      return;
	   }
	   else {
	      alarmTimer. setSingleShot	(true);
	      alarmTimer. setInterval 	(60 * 1000);
	      alarmTimer. start		(60 * 1000);
	      fprintf (stderr, "alarm set for %s at %d %d\n",
	                       alarmData. alarmService. toUtf8 (). data (),
	                       alarmData. targetHour, alarmData. targetMinute);
	      alarmLabel	-> show ();
	   }
	}
}

void	RadioInterface::alarmTimer_timeOut	() {
QTime theTime = QTime::currentTime ();
int	actualTime	= (theTime. hour () * 60 + theTime. minute ()) * 60 +
	                                   theTime. second ();
int	targetTime	= (alarmData. targetHour * 60 +
	                                  alarmData. targetMinute) * 60;
int	theDelay	= 120;	// seconds

	alarmTimer. stop ();
	if (actualTime >= targetTime - 10) {
	   handle_historySelect (alarmData. alarmService);
	   fprintf (stderr, "het alarm gaat op %d %d %d\n",
	      theTime. hour (), theTime. minute (), theTime. second ());
	   alarmLabel	-> hide ();
	   return;
	}

	if (actualTime >= targetTime - 200)
	   theDelay = 10;
	if (actualTime >= targetTime - 50)
	   theDelay = 1;

	alarmTimer. setSingleShot (true);
	alarmTimer. setInterval   (theDelay * 1000);
	alarmTimer. start         (theDelay * 1000);
}

void	RadioInterface::handle_setTime_button	() {
QTime theTime = QTime::currentTime ();
	configWidget. alarm_hours -> setValue (theTime. hour ());
	configWidget. alarm_minutes -> setValue (theTime. minute ());
}

#ifdef	TRY_EPG
void	RadioInterface::epgTimer_timeOut	() {
	epgTimer. stop ();
	if (scanning. load ())
	   return;
	for (const auto serv : serviceList) {
	   if (serv. name. contains ("-EPG ", Qt::CaseInsensitive) ||
	       serv. name. contains (" EPG   ", Qt::CaseInsensitive) ||
               serv. name. startsWith ("EPG ", Qt::CaseInsensitive) ) {
	      packetdata pd;
              my_dabProcessor -> dataforPacketService (serv. name, &pd, 0);
              if ((!pd. defined) ||
                    (pd.  DSCTy == 0) || (pd. bitRate == 0)) 
	         continue;
	      if (pd. DSCTy == 60) {
	         LOG ("hidden service started ", serv. name);
	         epgLabel	-> show ();
	         fprintf (stderr, "Starting hidden service %s\n",
	                                serv. name. toUtf8 (). data ());
	         my_dabProcessor -> set_dataChannel (&pd, &dataBuffer);
	         break;
	      }
	   }
//#ifdef	__DABDATA__
	   else {
	      packetdata pd;
	      my_dabProcessor -> dataforPacketService (serv. name, &pd, 0);
	      if ((pd. defined)  && (pd. DSCTy == 59)) {
	         LOG ("hidden service started ", serv. name);
	         epgLabel  -> show ();
	         fprintf (stderr, "Starting hidden service %s\n",
                                        serv. name. toUtf8 (). data ());
	         my_dabProcessor -> set_dataChannel (&pd, &dataBuffer);
                 break;
	      }
	   }
//#endif
	}
}
#endif

//------------------------------------------------------------------------
//------------------------------------------------------------------------

#ifdef	TRY_EPG
uint32_t RadioInterface::extract_epg (QString name,
	                              std::vector<serviceId> serviceList,
	                              uint32_t ensembleId) {
	for (const auto serv : serviceList) {
	   if (name. contains (QString::number (serv. SId, 16),
	                          Qt::CaseInsensitive)) 
	   
	      return serv. SId;
	}
	return 0;
}

void	RadioInterface::set_epgData (int SId,
	                             int theTime, const QString &theText) {
	if (my_dabProcessor != nullptr)
	   my_dabProcessor -> set_epgData (SId, theTime, theText);
}

#endif

void	RadioInterface::handle_timeTable	() {
	if (!currentService. valid || !currentService. is_audio)
	   return;

	if  (my_timeTable -> isHidden ())
	   my_timeTable -> show ();
	else
	   my_timeTable -> hide ();

	my_timeTable	-> clear ();
	std::vector<epgElement> res =
	           my_dabProcessor -> find_epgData (currentService. SId);
	for (const auto& element: res)
	   my_timeTable -> addElement (element. theTime,
	                               element. theText);
}

void	RadioInterface::handle_skipList_button () {
	if (!theBand. isHidden ()) {
	   theBand. hide ();
	}
	else
	   theBand. show ();
}

void	RadioInterface::handle_skipFile_button	() {
const QString fileName	= filenameFinder. findskipFile_fileName ();
	theBand. saveSettings ();
	theBand. setup_skipList (fileName);
}

void	RadioInterface::handle_snrDelaySetting	(int del) {
	my_snrViewer. set_snrDelay (del);
	dabSettings	-> setValue ("snrDelay", del);
}

void	RadioInterface::handle_tii_detectorMode (int d) {
bool	b = configWidget. tii_detectorMode -> isChecked ();
	my_dabProcessor	-> set_tiiDetectorMode (b);
	dabSettings	-> setValue ("tii_detector", b ? 1 : 0);
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
