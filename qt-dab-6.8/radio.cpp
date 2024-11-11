#
/*
 *    Copyright (C)  2015 .. 2024
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
#include	<QDebug>
#include	<QDateTime>
#include	<QFile>
#include	<QStringList>
#include	<QMouseEvent>
#include	<QDir>
#include	<QColorDialog>
#include	<fstream>
#include	"dab-constants.h"
#include	"mot-content-types.h"
#include	<iostream>
#include	<numeric>
#include	<vector>
#include	"radio.h"
#include	"config-handler.h"
#include	"ofdm-handler.h"
#include	"schedule-selector.h"
#include	"element-selector.h"
#include	"dab-tables.h"
#include	"dab-params.h"
#include	"ITU_Region_1.h"
#include	"coordinates.h"
#include	"mapport.h"
#include	"techdata.h"
#include	"aboutdialog.h"
#include	"db-loader.h"
#include	"cacheElement.h"
#include	"distances.h"
#include	"position-handler.h"
#include	"settings-handler.h"
#ifdef	TCP_STREAMER
#include	"tcp-streamer.h"
#else
#include	"Qt-audio.h"
#include	"audiosink.h"
#endif
#include	"time-table.h"

#include	"device-exceptions.h"
#include	"settingNames.h"
#include	"uploader.h"

#if defined (__MINGW32__) || defined (_WIN32)
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
	total_time = std::accumulate (cpu_times. begin(),
	                            cpu_times. end(), (size_t)0);
	return true;
}
#include	<unistd.h>
#endif


#define	WHITE	"#ffffff"
#define	BLACK	"#000000"
#define	GREEN	"#8ff0a4"
#define	BLUE	"#00ffff"
#define	RED	"#ff007f"
#define	YELLOW	"#f9f06b"

static inline
QString ids_to_string (int mainId, int subId) {
	return  "(" + QString::number (mainId) + "-"
	            + QString::number (subId)  + ")";
}

static inline
QStringList splitter (const QString &s) {
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 2)
	QStringList list = s.split (":", Qt::SkipEmptyParts);
#else
	QStringList list = s.split (":", QString::SkipEmptyParts);
#endif
	return list;
}

#define	D_PRIV	1
#define	D_NIX	0

static const
char	LABEL_STYLE [] = "color:lightgreen";

	RadioInterface::RadioInterface (QSettings	*Si,
	                                const QString	&scanListFile,
	                                const QString	&presetFile,
	                                const QString	&freqExtension,
	                                const QString	&schedule,
	                                bool		error_report,
	                                int32_t		dataPort,
	                                int32_t		clockPort,
	                                int		fmFrequency,
	                                QWidget		*parent):
	                                        QWidget (parent),
	                                        theSpectrumBuffer (16 * 32768),
	                                        theIQBuffer (2 * 1536),
	                                        theTIIBuffer (32768),
	                                        theNULLBuffer (32768),
	                                        theChannelBuffer (4096),
	                                        theSNRBuffer (512),
	                                        theResponseBuffer (32768),
	                                        theFrameBuffer (2 * 32768),
		                                theDataBuffer (32768),
	                                        theAudioBuffer (8 * 32768),
	                                        stdDevBuffer (2 * 1536),
	                                        theNewDisplay (this, Si),
	                                        theSNRViewer (this, Si),
	                                        theDLCache (10),
	                                        theTIIProcessor (Si),
	                                        theFilenameFinder (Si),
	                                        theScheduler (this, schedule),
	                                        theTechData (16 * 32768),
	                                        theAudioConverter (this),
	                                        theScanlistHandler (this,
	                                                        scanListFile),
	                                        theDeviceChoser (Si),
	                                        theDXDisplay (this, Si),
	                                        theLogger	(Si),
	                                        theSCANHandler (this, Si, freqExtension) {
int16_t k;
QString h;

	dabSettings_p			= Si;
	this	-> error_report		= error_report;
	this	-> fmFrequency		= fmFrequency;
	this	-> dlTextFile		= nullptr;
	this	-> ficDumpPointer	= nullptr;
	this	-> the_aboutLabel	= nullptr;
	running. 		store (false);
	theOFDMHandler		= nullptr;
	stereoSetting		= false;
	maxDistance		= -1;
	contentTable_p		= nullptr;
	scanTable_p		= nullptr;
	mapHandler		= nullptr;
	theDXDisplay. hide ();
//	"globals" is introduced to reduce the number of parameters
//	for the ofdmHandler
	globals. spectrumBuffer	= &theSpectrumBuffer;
	globals. iqBuffer	= &theIQBuffer;
	globals. responseBuffer	= &theResponseBuffer;
	globals. tiiBuffer	= &theTIIBuffer;
	globals. nullBuffer	= &theNULLBuffer;
	globals. channelBuffer	= &theChannelBuffer;
	globals. snrBuffer	= &theSNRBuffer;
	globals. frameBuffer	= &theFrameBuffer;
	globals. stdDevBuffer	= &stdDevBuffer;

	globals. dabMode         =
	          value_i (dabSettings_p, DAB_GENERAL, "dabMode", 1);
	globals. threshold		=
	          value_i (dabSettings_p, DAB_GENERAL, "threshold", 3);
	globals. diff_length	=
	          value_i (dabSettings_p, DAB_GENERAL, "diff_length", DIFF_LENGTH);
	globals. tii_delay   =
	          value_i (dabSettings_p, DAB_GENERAL, "tii_delay", 5);
	if (globals. tii_delay < 2)
	   globals. tii_delay	= 2;
	globals. tii_depth      =
	          value_i (dabSettings_p, DAB_GENERAL, "tii_depth", 4);
	globals. echo_depth     =
	          value_i (dabSettings_p, DAB_GENERAL, "echo_depth", 1);

#ifdef	_SEND_DATAGRAM_
	ipAddress	=  value_s (dabSettings_p, "DATAGRAM",
	                              "ipAddress", "127.0.0.1");
	port		=  value_i (dabSettings_p, "DATAGRAM",
	                              "port" 8888);
#endif
//	set on top or not? checked at start up
	if (value_i (dabSettings_p, DAB_GENERAL, "onTop", 0) == 1) 
	   setWindowFlags (windowFlags () | Qt::WindowStaysOnTopHint);
	for (int i = 0; i < 4; i ++) {
	   QPixmap p;
	   QString labelName	=
	        QString (":res/radio-pictures/signal%1.png"). arg (i, 1, 10, QChar ('0'));
	   p. load (labelName, "png");
	   strengthLabels. push_back (p);
	}

//	The settings are done, now creation of the GUI parts
	setupUi (this);
//	and init the up and down button
	{  QPixmap p;
	   if (p. load (":res/radio-pictures/up-arrow.png", "png"))
	      prevChannelButton -> setPixmap (p. scaled (30, 30, Qt::KeepAspectRatio));
	   if (p. load (":res/radio-pictures/down-arrow.png", "png"))
	      nextChannelButton -> setPixmap (p. scaled (30, 30, Qt::KeepAspectRatio));
	   if (p. load (":res/radio-pictures/details24.png", "png"))
	      serviceButton -> setPixmap (p. scaled (30, 30, Qt::KeepAspectRatio));
	   else
	      fprintf (stderr, "Loading details button failed\n");

	   if (p. load (":res/radio-pictures/folder_button.png", "png"))
	      folder_shower -> setPixmap (p. scaled (30, 30, Qt::KeepAspectRatio));
	}

	connect (folder_shower, SIGNAL (clicked ()),
	         this, SLOT (handle_folderButton ()));

//	put the widgets in the right place and create the workers
	set_position_and_size	(dabSettings_p, this, S_MAIN_WIDGET);
	configHandler_p		= new configHandler (this, dabSettings_p);
	the_ensembleHandler	= new ensembleHandler (this, dabSettings_p,
	                                                       presetFile);
//	we have the configuration handler and the ensemble handler,
//	connect some signals directly
	configHandler_p		-> set_connections ();
	configHandler_p		-> setDeviceList (theDeviceChoser.
	                                            getDeviceList ());

	connect (configHandler_p, &configHandler::frameClosed,
	         this, &RadioInterface::handle_configFrame_closed);
	connect (configHandler_p, &configHandler::handle_fontSelect,
	         the_ensembleHandler, &ensembleHandler::handle_fontSelect);

	connect (configHandler_p, &configHandler::handle_fontSizeSelect,
	         the_ensembleHandler, &ensembleHandler::handle_fontSizeSelect);
	connect (configHandler_p, &configHandler::handle_fontColorSelect,
	         the_ensembleHandler,& ensembleHandler::handle_fontColorSelect);
	connect (configHandler_p, &configHandler::set_serviceOrder,
	         the_ensembleHandler, &ensembleHandler::set_serviceOrder);
	connect (&theNewDisplay, &displayWidget::frameClosed,
	         this, &RadioInterface::handle_newDisplayFrame_closed);
#ifdef HAVE_RTLSDR_V3
	SystemVersion	= QString ("8") + " with RTLSDR-V3";
#elif HAVE_RTLSDR_V4
	SystemVersion	= QString ("8") + " with RTLSDR-V4";
#else
	SystemVersion	= QString ("8");
#endif
	setWindowTitle ("Qt-DAB-6." +SystemVersion);
	version		= "Qt-DAB-6." + SystemVersion;

	ensembleWidget -> setWidget (the_ensembleHandler);
	connect (the_ensembleHandler, &ensembleHandler::selectService,
	         this, &RadioInterface::localSelect);
	connect (the_ensembleHandler,
	               &ensembleHandler::start_background_task,
	         this, &RadioInterface::start_background_task);
	   
	techWindow_p	= new techData (this, dabSettings_p, &theTechData);
	
	connect (techWindow_p, &techData::frameClosed,
	         this, &RadioInterface::handle_techFrame_closed);

	if (value_i (dabSettings_p, DAB_GENERAL, NEW_DISPLAY_VISIBLE, 0) != 0)
	   theNewDisplay. show ();
	else
	   theNewDisplay. hide ();

	labelStyle	= value_s (dabSettings_p, DAB_GENERAL, LABEL_COLOR,
	                                                     LABEL_STYLE);
	QFont font	= serviceLabel -> font ();
	font. setPointSize (16);
	font. setBold (true);
	serviceLabel	-> setStyleSheet (labelStyle);
	serviceLabel	-> setFont (font);
	programTypeLabel	-> setStyleSheet (labelStyle);
	font      = ensembleId -> font ();
	font. setPointSize (14);
	ensembleId      -> setFont (font);

	channel. currentService. valid	= false;
	channel. serviceCount		= -1;

	channel. targetPos	= position {0, 0};
	
	localPos. latitude 		=
	             value_f (dabSettings_p, MAP_HANDLING, HOME_LATITUDE, 0.0);
	localPos. longitude 		=
	             value_f (dabSettings_p, MAP_HANDLING, HOME_LONGITUDE, 0.0);
	techWindow_p 		-> hide ();	// until shown otherwise
	stillMuting		-> hide ();

#ifdef	DATA_STREAMER
	dataStreamer_p		= new tcpServer (dataPort);
#else
	(void)dataPort;
#endif
#ifdef	CLOCK_STREAMER
	clockStreamer_p		= new tcpServer (clockPort);
#else
	(void)clockPort;
#endif

	volumeSlider	-> hide ();
//	Where do we leave the audio out?
	configHandler_p	-> show_streamSelector (false);
	int latency	= value_i (dabSettings_p, SOUND_HANDLING, "latency", 5);
	soundOut_p		= nullptr;
//
//	If we do not have a TCP streamer, we go for one of the
//	Portaudio and the Qt_audio alternatives.
//	Default - and if Qt_Audio fails, we go for Portaudio
#ifndef	TCP_STREAMER
	QStringList streams;
	QString	temp;
	
	QString s = value_s (dabSettings_p, SOUND_HANDLING, SOUND_HANDLER,
	                                                   S_PORT_AUDIO);
	if (s != S_PORT_AUDIO) {	// try Qt_Audio
	   try {
	      soundOut_p	= new Qt_Audio (dabSettings_p);
	      streams		= ((Qt_Audio *)soundOut_p) -> streams ();
	      temp		=
	          value_s (dabSettings_p, SOUND_HANDLING,
	                                  QT_AUDIO_STREAM_NAME, "default");
	      volumeSlider	-> show ();
	      int volume	=
	          value_i (dabSettings_p, SOUND_HANDLING, QT_AUDIO_VOLUME, 50);
	      volumeSlider		-> setValue (volume);
	      ((Qt_Audio *)soundOut_p)	-> setVolume (volume);
	      connect (volumeSlider, &QSlider::valueChanged,
	               this, &RadioInterface::setVolume);
	      techWindow_p	-> hide_missedLabel ();
	   } catch (...) {
//	      fprintf (stderr, "QT_AUDIO does not find streams\n");
	      soundOut_p = nullptr;
	   }
	}
//
//	we end up here if selection was PORT_AUDIO or using Qt_Audio failed
//	as it does on U20
	if (soundOut_p == nullptr) {
	   soundOut_p		= new audioSink		(latency);
	   streams	= ((audioSink *)soundOut_p) -> streams ();
	   temp		=
	          value_s (dabSettings_p, SOUND_HANDLING, AUDIO_STREAM_NAME,
	                                                  "default");
	}

	if (streams. size () > 0) {
//	   for (auto s: streams)
//	      fprintf (stderr, "%s\n", s. toLatin1 (). data ());
	   configHandler_p -> fill_streamTable (streams);
	   configHandler_p -> show_streamSelector (true);
	   k	= configHandler_p -> init_streamTable (temp);
	   if (k >= 0)
	      soundOut_p -> selectDevice (k);
	   configHandler_p	-> connect_streamTable	();
	}
	else {
	   delete soundOut_p;
	   soundOut_p = new audioPlayer ();
	}
#else
	soundOut_p		= new tcpStreamer	(20040);
	techWindow_p		-> hide		();
#endif
//
//	some MOT, tetx and other data is stored in the Qt-DAB-files directory
//	in home or tmp dir
	QString tempPath	= theFilenameFinder. basicPath ();
	path_for_tiiFile	=
	                      value_s (dabSettings_p, DAB_GENERAL,
	                                             S_TII_PATH, tempPath);
	path_for_tiiFile	= checkDir (path_for_tiiFile);
	
	path_for_pictures	=
	                      value_s (dabSettings_p, DAB_GENERAL,
	                                       S_PICTURES_PATH, tempPath);
	path_for_pictures	= checkDir (path_for_pictures)
;
	path_for_files		=
	                      value_s (dabSettings_p, DAB_GENERAL,
	                                        S_FILE_PATH, tempPath);
	epgPath			=
	                      value_s (dabSettings_p, DAB_GENERAL,
	                                        S_EPG_PATH, tempPath);
	epgPath			= checkDir (epgPath);

	connect (&epgProcessor, &epgDecoder::set_epgData,
	         this, &RadioInterface::set_epgData);
//	timer for autostart epg service
	epgTimer. setSingleShot (true);
	connect (&epgTimer, &QTimer::timeout,
	         this, &RadioInterface::epgTimer_timeOut);
	pauzeTimer. setSingleShot (true);
	connect (&pauzeTimer, &QTimer::timeout,
	         this, &RadioInterface::show_pauzeSlide);

	my_timeTable		= new timeTableHandler (this);
	my_timeTable		-> hide ();

	connect (&theScanlistHandler, &scanListHandler::handle_scanListSelect,
	         this, &RadioInterface::handle_scanListSelect);

//	extract the channelnames and fill the combobox
	QStringList res = theSCANHandler. getChannelNames ();
	for (auto &s: res)
	  channelSelector -> addItem (s);

	QPalette p	= theNewDisplay. ficError_display -> palette();
	p. setColor (QPalette::Highlight, Qt::red);
	theNewDisplay. ficError_display	-> setPalette (p);
	p. setColor (QPalette::Highlight, Qt::green);
//
	audioDumping		= false;
	sourceDumping		= false;
	ficBlocks		= 0;
	ficSuccess		= 0;
	total_ficError		= 0;
	total_fics		= 0;
      
	previous_idle_time	= 0;
	previous_total_time	= 0; 

//	Connect the buttons for the color_settings
	connect	(scanButton, &smallPushButton::rightClicked,
	         this, &RadioInterface::color_scanButton);
	connect (scanListButton, &newPushButton::rightClicked,
	         this, &RadioInterface::color_scanListButton);
	connect (presetButton, &newPushButton::rightClicked,
	         this, &RadioInterface::color_presetButton);
	connect (configButton, &smallPushButton::rightClicked,
	         this, &RadioInterface::color_configButton);
	connect (httpButton, &smallPushButton::rightClicked,
	         this, &RadioInterface::color_httpButton);
	connect (prevServiceButton, &smallPushButton::rightClicked,
	         this, &RadioInterface::color_prevServiceButton);
	connect (nextServiceButton, &smallPushButton::rightClicked,
	         this, &RadioInterface::color_nextServiceButton);
	connect (spectrumButton, &smallPushButton::rightClicked,
	         this, &RadioInterface::color_spectrumButton);
//
//	
	connect (techWindow_p, &techData::handle_timeTable,
	         this, &RadioInterface::handle_timeTable);
	connect (&theNewDisplay, &displayWidget::mouseClick,
	         this, &RadioInterface::handle_iqSelector);

	connect (aboutLabel, &clickablelabel::clicked,
	         this, &RadioInterface::handle_aboutLabel);

	connect (soundLabel, &clickablelabel::clicked,
	         this, &RadioInterface::handle_muteButton);
	connect (snrLabel, &clickablelabel::clicked,
	         this, &RadioInterface::handle_snrLabel);

//	if (theTIIProcessor. has_tiiFile ())
	   configHandler_p -> enable_loadLib ();
//	else
//	   httpButton	-> setEnabled (false);

	channel. etiActive	= false;

	show_pauzeSlide ();

//	and start the timer(s)
//	The displaytimer is there to show the number of
//	seconds running and handle - if available - the tii data
	displayTimer. setInterval (1000);
	connect (&displayTimer, &QTimer::timeout,
	         this, &RadioInterface::updateTimeDisplay);
	displayTimer. start (1000);
	numberofSeconds		= 0;

//	timer for scanning
	channelTimer. setSingleShot (true);
	channelTimer. setInterval (10000);
	connect (&channelTimer, &QTimer::timeout,
	         this, &RadioInterface::channel_timeOut);
//
//	presetTimer
	presetTimer. setSingleShot (true);
	connect (&presetTimer, &QTimer::timeout,
	         this, &RadioInterface::setPresetService);
//
//	timer for muting
	muteTimer. setSingleShot (true);
	set_Colors ();
//
//	do we have a known device from previous invocations?
	inputDevice_p	= nullptr;
	h               =
	           value_s (dabSettings_p, DAB_GENERAL, 
	                                      SELECTED_DEVICE, "no device");
	bool b = configHandler_p -> findDevice (h);
	if (b) {
	   inputDevice_p = create_device (h, &theLogger);
	}
//
//	do we show controls?
	bool visible	=
	            value_i (dabSettings_p, DAB_GENERAL, 
	                                     CONFIG_WIDGET_VISIBLE, 0) != 0;
	if (visible) {
	   configHandler_p	-> show ();
	}
	connect (configButton, &QPushButton::clicked,
	         this, &RadioInterface::handle_configButton);

	if (value_i (dabSettings_p, DAB_GENERAL, SNR_WIDGET_VISIBLE, 0) != 0)
	   theSNRViewer. show ();
	else
	   theSNRViewer. hide ();
	if (value_i (dabSettings_p, DAB_GENERAL, TECHDATA_VISIBLE, 0) != 0)
	   techWindow_p -> show ();

	dynamicLabel	-> setTextInteractionFlags(Qt::TextSelectableByMouse);
	dynamicLabel    -> setToolTip ("The text (or parts of it) of the dynamic label can be copied. Selecting the text with the mouse and clicking the right hand mouse button shows a small menu with which the text can be put into the clipboard");
//
//	Until the opposite is known to be true:
	nextService. channel = "";
	nextService. serviceName = "";
//	if a device was selected, we just start, otherwise
//	we wait until one is selected
	connectGUI ();
//
//	Just check whether the ini file is used before
	bool iniExists	=
	           value_i (dabSettings_p, DAB_GENERAL, "EXISTS", 0) != 0;
	if (!iniExists) {
	   store (dabSettings_p, DAB_GENERAL, "EXISTS", 1);
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("The ini file is new and no home location is known yet"));
	}
	if (inputDevice_p != nullptr) {
	   doStart_direct ();
	   qApp	-> installEventFilter (this);
	   return;
	}
	if (!visible) { 	// make it visible
	   store (dabSettings_p, DAB_GENERAL, CONFIG_WIDGET_VISIBLE, 1);
	   store (dabSettings_p, DAB_GENERAL, DEVICE_WIDGET_VISIBLE, 1);
	}
	configHandler_p		-> show ();
	configHandler_p		-> connectDevices ();
	qApp	-> installEventFilter (this);
//
//	and just wait to see what device is selected
}
//
//	doStart (QString) is called when - on startup - NO device
//	was registered to be used, and the user presses the
//	selectDevice comboBox
void	RadioInterface::doStart (const QString &dev) {
	inputDevice_p	= create_device	(dev, &theLogger);
//	Some buttons should not be touched before we have a device
	if (inputDevice_p == nullptr) {
	   return;
	}
	doStart_direct ();
}
//
//	we (re)start a device, if it happens to be a regular
//	device, check for a preset name
void	RadioInterface::doStart_direct	() {
	disconnect (channelSelector,
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 2)
	            qOverload<const QString &> (&QComboBox::textActivated),
#else
	            qOverload<const QString &> (&QComboBox::activated),
#endif
	            this, &RadioInterface::handle_channelSelector);
	QString startingChannel = 
	      value_s (dabSettings_p, DAB_GENERAL, CHANNEL_NAME, "5A");
	int k = channelSelector -> findText (startingChannel);
	if (k != -1) 	
	   channelSelector -> setCurrentIndex (k);
	theLogger. log (logger::LOG_RADIO_STARTS, inputDevice_p -> deviceName (),
	                                 channelSelector -> currentText ());
	theOFDMHandler	= new ofdmHandler  (this,
	                                    inputDevice_p,
	                                    &globals, dabSettings_p, &theLogger);
	if (value_i (dabSettings_p, CONFIG_HANDLER, "dcRemoval", 0) != 0) {
	   theOFDMHandler	-> set_dcRemoval  (true);
	   theNewDisplay. set_dcRemoval (true);
	}

	channel. cleanChannel ();
	the_ensembleHandler	-> reset	();
	the_ensembleHandler	-> setMode (!inputDevice_p -> isFileInput ());

	if (value_i (dabSettings_p, DAB_GENERAL, DEVICE_WIDGET_VISIBLE, 0) != 0)
	   inputDevice_p -> setVisibility (true);

//	Just to be sure we disconnect here.
//	It would have been helpful to have a function
//	testing whether or not a connection exists, we need a kind
//	of "reset"
	configHandler_p -> disconnectDevices ();
	configHandler_p	-> reconnectDevices ();
	
	connect (channelSelector,
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 2)
	         qOverload<const QString &> (&QComboBox::textActivated),
#else
	         qOverload<const QString &> (&QComboBox::activated),
#endif
	         this, &RadioInterface::handle_channelSelector);

	if (configHandler_p -> tii_detector_active ())
	   theOFDMHandler -> set_tiiDetectorMode (true);

	startChannel (channelSelector -> currentText ());
	int auto_http	= value_i (dabSettings_p, DAB_GENERAL, "auto_http", 0);
	if ((auto_http != 0) && (localPos. latitude != 0)) {
	   bool succ = autoStart_http ();
	   if (succ)
	      httpButton	-> setText ("http-on");
	} 
	running. store (true);
}

	RadioInterface::~RadioInterface () {
	fprintf (stderr, "radioInterface is deleted\n");
}

//	
//	no_signal_found is called when scanning is on by the
//	ofdm handler
void	RadioInterface::no_signal_found () {
	channel_timeOut ();
}

///////////////////////////////////////////////////////////////////////////
//
//	a slot, called by the fic/fib handlers
void	RadioInterface::add_to_ensemble (const QString &serviceName,
	                                           int32_t SId, int  subChId) {
	if (!running. load())
	   return;

	serviceId ed;
	ed. name	= serviceName;
	ed. SId		= SId;
	ed. subChId	= subChId;
	ed. channel	= channel. channelName;

	bool added	= the_ensembleHandler -> add_to_ensemble (ed);
	if (added) {
	   channel. nrServices ++;
	   if (theSCANHandler. active ())
	      theSCANHandler. addService (channel. channelName);
	   if (theSCANHandler. active () && !theSCANHandler. scan_to_data ()) {
	      theScanlistHandler. addElement (channel. channelName,
	                                              serviceName);
	   }
	}

	if ((channel. serviceCount == channel. nrServices)&& 
	                     !theSCANHandler. active ()) {
//	   presetTimer. stop ();
	   setPresetService ();
	}
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

//	a slot, called by the fib processor
void	RadioInterface::name_of_ensemble (int id, const QString &v) {
QString s;
	if (!running. load())
	   return;

	ensembleId	-> setText (v + QString ("(") + hextoString (id) + QString (")"));

	transmitter_country	-> setText (channel. countryName);
	channel. ensembleName	= v;
	channel. Eid		= id;
//
//	id we are scanning "to data", we reached the end
	if (theSCANHandler. scan_to_data ())
	   stopScanning ();
	else
	if (theSCANHandler. active () && theSCANHandler. scan_single ())
	   theSCANHandler. addEnsemble (channelSelector -> currentText (), v);
	                           
//	... and is we are not scanning, clicking the ensembleName
//	has effect
	if (!theSCANHandler. active ())
	   connect (ensembleId, &clickablelabel::clicked,
	            this, &RadioInterface::handle_contentButton);
}
//
///////////////////////////////////////////////////////////////////////////

void	RadioInterface::handle_contentButton	() {
QStringList s	= theOFDMHandler -> basicPrint ();

	if (contentTable_p != nullptr) {
	   contentTable_p -> hide ();
	   delete contentTable_p;
	   contentTable_p = nullptr;
	   return;
	}
	QString headLine = build_headLine ();
	contentTable_p		= new contentTable (this, dabSettings_p,
	                                            channel. channelName,
	                                            theOFDMHandler -> scanWidth ());
	connect (contentTable_p, &contentTable::goService,
	         this, &RadioInterface::handle_contentSelector);

	contentTable_p		-> addLine (headLine);
//	contentTable_p		-> addLine ("\n");
	for (auto &ss : s)
	   contentTable_p -> addLine (ss);
	contentTable_p -> show ();
	if (configHandler_p -> upload_selector_active ()) {
	   try {
	      uploader the_uploader;
	      QMessageBox::StandardButton reply =
	              QMessageBox::question (this, 
	                                     "upload content to fmlist.org?", "",
	                              QMessageBox::Yes | QMessageBox::No);
	      if (reply == QMessageBox::Yes)
	         the_uploader. loadUp (channel. ensembleName,
	                               channel. Eid,
	                               channel. channelName, 
	                               contentTable_p -> upload ());
	   } catch (...) {}
	}
}

QString	RadioInterface::checkDir (const QString &s) {
QString	dir = s;

	if (!dir. endsWith (QChar ('/')))
	   dir += QChar ('/');

	if (QDir (dir). exists())
	   return dir;
	QDir (). mkpath (dir);
	return dir;
}

void	RadioInterface::handle_motObject (QByteArray result,
	                                  QString  objectName,
	                                  int contentType,
	                                  bool dirElement,
	                                  bool backgroundFlag) {
QString realName;

	switch (getContentBaseType ((MOTContentType)contentType)) {
	   case MOTBaseTypeGeneralData:
	      break;

	   case MOTBaseTypeText:
	      save_MOTtext (result, contentType, objectName);
	      break;

	   case MOTBaseTypeImage:
	      show_MOTlabel (result, contentType,
	                       objectName, dirElement, backgroundFlag);
	      break;

	   case MOTBaseTypeAudio:
	      break;

	   case MOTBaseTypeVideo:
	      break;

	   case MOTBaseTypeTransport:
	      save_MOTObject (result, objectName);
	      break;

	   case MOTBaseTypeSystem:
	      break;

	   case  MOTBaseTypeApplication: 	// epg data
	      if (epgPath == "")
	         return;

	      if (objectName == QString (""))
	         objectName = "epg file";
	      objectName  = epgPath + objectName;
	      {  QString temp = objectName;
	         temp = temp. left (temp. lastIndexOf (QChar ('/')));
	         if (!QDir (temp). exists ())
	            QDir (). mkpath (temp);	

	         std::vector<uint8_t> epgData (result. begin(),
	                                                  result. end());
//	         uint32_t ensembleId =
//	                     theOFDMHandler -> get_ensembleId ();
	         uint32_t currentSId =
	                     the_ensembleHandler -> extract_SId  (objectName);
	         uint32_t julianDate	=
	                     theOFDMHandler -> julianDate ();
	         int subType = 
	                  getContentSubType ((MOTContentType)contentType);
	         epgProcessor. process_epg (epgData. data (), 
	                                    epgData. size (), currentSId,
	                                    subType,
	                                    julianDate);
	         if (configHandler_p -> epg2_active ()) {
	            epgHandler. decode (epgData,
	                      QDir::toNativeSeparators (objectName));
	         }
	      }
	      return;

	   case MOTBaseTypeProprietary:
	      break;
	}
}

void	RadioInterface::save_MOTtext (QByteArray &result,
	                              int contentType,
	                              const QString &name) {
	(void)contentType;
	if (path_for_files == "")
	   return;

	QString textName = QDir::toNativeSeparators (path_for_files + name);

	FILE *x = fopen (textName. toUtf8 (). data (), "w+b");
	if (x == nullptr)
	   fprintf (stderr, "cannot write file %s\n",
	                            textName. toUtf8 (). data ());
	else {
//	   fprintf (stderr, "going to write file %s\n",
//	                            textName. toUtf8(). data());
	   (void)fwrite (result. data (), 1, result.length(), x);
	   fclose (x);
	}
}

void	RadioInterface::save_MOTObject (QByteArray  &result,
	                                QString name) {
	if (path_for_files == "")
	   return;

	if (name == "") {
	   static int counter	= 0;
	   name = "motObject_" + QString::number (counter);
	   counter ++;
	}
	save_MOTtext (result, 5, name);
}

//	MOT slide, to show
void	RadioInterface::show_MOTlabel	(QByteArray  &data,
	                                 int contentType,
	                                 const QString  &pictureName,
	                                 int dirs,
	                                 bool backgroundFlag) {
const char *type;
	if (!running. load() || (pictureName == QString ("")))
	   return;

	(void)dirs;
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

	if ((value_i (dabSettings_p, CONFIG_HANDLER,
	                           SAVE_SLIDES_SETTING, 0) != 0) &&
	                                         (path_for_pictures != "")) {
	   QString pict = path_for_pictures + pictureName;
	   QString temp = pict;
	   temp = temp. left (temp. lastIndexOf (QChar ('/')));
	   if (!QDir (temp). exists())
	      QDir (). mkpath (temp);	
	   pict		= QDir::toNativeSeparators (pict);
	   FILE *x = fopen (pict. toUtf8 (). data (), "w+b");

	   if (x == nullptr)
	      fprintf (stderr, "cannot write file %s\n",
	                            pict. toUtf8 (). data ());
	   else {
	      theLogger. log (logger::LOG_SLIDE_WRITTEN, pict);
	      (void)fwrite (data. data(), 1, data.length(), x);
	      fclose (x);
	   }
	}

	if (backgroundFlag)
	   return;

	if (channel. currentService. valid &&  
	                     channel. currentService. announcement_going)
	   return;
	QPixmap p;
	if (p. loadFromData (data, type))
	   displaySlide (p);
}
//
//	sendDatagram is triggered by the ip handler,
void	RadioInterface::sendDatagram	(int length) {
uint8_t *localBuffer = dynVec (uint8_t, length);

	if (theDataBuffer. GetRingBufferReadAvailable() < length) {
	   fprintf (stderr, "Something went wrong\n");
	   return;
	}

	theDataBuffer. getDataFromBuffer (localBuffer, length);
#ifdef	_SEND_DATAGRAM_
	if (running. load ()) {
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
uint8_t *localBuffer = dynVec (uint8_t, length + 8);
#endif
	(void)frametype;
	if (!running. load())
	   return;
	if (theDataBuffer. GetRingBufferReadAvailable() < length) {
	   fprintf (stderr, "Something went wrong\n");
	   return;
	}
#ifdef	DATA_STREAMER
//	fprintf (stderr, "%d\n", length);
	theDataBuffer. getDataFromBuffer (&localBuffer [8], length);
	localBuffer [0] = 0xFF;
	localBuffer [1] = 0x00;
	localBuffer [2] = 0xFF;
	localBuffer [3] = 0x00;
	localBuffer [4] = (length & 0xFF) >> 8;
	localBuffer [5] = length & 0xFF;
	localBuffer [6] = 0x00;
	localBuffer [7] = frametype == 0 ? 0 : 0xFF;
	if (running. load())
	   dataStreamer_p -> sendData (localBuffer, length + 8);
#endif
}
/**
  *	If a "change in configuration" is detected, we have to
  *	restart the selected service - if any.s
  *	If the service is a secondary service, it might be the case
  *	that we have to start the main service
  *	how do we find that?
  *
  *	Response to a signal, so we presume that the signaling body exists
  *	signal may be pending though
  */
void	RadioInterface::changeinConfiguration (const QStringList &notInOld,
	                                       const QStringList &notInNew) {
	if (!running. load () || theOFDMHandler == nullptr)
	   return;
	
	dabService s;
	if (channel. currentService. valid) {
	   s = channel. currentService;
	   s. serviceName	= channel. currentService. serviceName;
	   s. SId	= channel. currentService. SId;
	   s. SCIds	= channel. currentService. SCIds;
	   s. channel	= channel. channelName;
	   s. valid	= true;
	}

	stopService	(channel. currentService);
	stopScanning ();
//
//	we stop all secondary services as well, but we maintain theer
//	description, file descriptors remain of course
//
	if (channel. etiActive)
	   theOFDMHandler -> reset_etiGenerator ();
	for (uint16_t i = 0; i < channel. backgroundServices. size (); i ++)
	   theOFDMHandler -> stop_service (channel. backgroundServices. at (i). subChId,
	                                   BACK_GROUND);

//	we rebuild the services list from the fib and
//	then we (try to) restart the service
	int	serviceOrder	= configHandler_p -> get_serviceOrder ();
	std::vector<serviceId> serviceList	=
	          theOFDMHandler -> get_services (serviceOrder);
	the_ensembleHandler -> reset	();
	channel. nrServices = 0;
	for (auto &serv: serviceList) {
	   serv . channel = channel. channelName;
	   if (the_ensembleHandler -> add_to_ensemble (serv))
	      channel. nrServices  ++;
	}

//	Of course, the (sub)service may have disappeared
	if (s. valid) {
	   QString ss = theOFDMHandler -> find_service (s. SId, s. SCIds);
	   if (ss != "") {
	      startService (s);
	      return;
	   }
//
	show_changeLabel (notInOld, notInNew);
//	The service is gone, it may be the subservice of another one
	   s. SCIds = 0;
	   s. serviceName =
	               theOFDMHandler -> find_service (s. SId, 0);
	   if (s. serviceName != "") {
	      startService (s);
	   }
	}
//
//	we also have to restart all background services,
	for (uint16_t i = 0; i < channel. backgroundServices. size (); i ++) {
	   QString ss = theOFDMHandler -> find_service (s. SId, s. SCIds);
	   if (ss == "") {	// it is gone, close the file if any
	      if (channel. backgroundServices. at (i). fd != nullptr)
	         fclose (channel. backgroundServices. at (i). fd);
	      channel. backgroundServices. erase
	                        (channel. backgroundServices. begin () + i);
	   }
	   else {	// (re)start the service
	      audiodata ad;
	      theOFDMHandler -> data_for_audioservice (ss, ad);
	      if (ad. defined) {
	         FILE *f = channel. backgroundServices. at (i). fd;
	         theOFDMHandler -> 
	                   set_audioChannel (ad, &theAudioBuffer, f, BACK_GROUND);	       
	         channel. backgroundServices. at (i). subChId  = ad. subchId;
	      }
	      else {
	         packetdata pd;
	         theOFDMHandler -> data_for_packetservice (ss, pd, 0);
	         theOFDMHandler -> 
	                   set_dataChannel (pd, &theDataBuffer, BACK_GROUND);	       
	         channel. backgroundServices. at (i). subChId     = pd. subchId;
	      }
	   }
	}
}
//
//	In order to not overload with an enormous amount of
//	signals, we trigger this function at most 10 times a second
void	RadioInterface::newAudio	(int amount, int rate,
	                                          bool ps, bool sbr) {
	if (!running. load ())
	   return;

static int teller	= 0;
//	if (!techWindow_p -> isHidden ()) {
	teller ++;
	if (teller > 10) {
	   teller = 0;
	   if (!techWindow_p -> isHidden ())
	      techWindow_p	-> show_rate (rate, ps, sbr);
	   audiorateLabel	-> setStyleSheet (labelStyle);
	   audiorateLabel	-> setText (QString::number (rate));
	   if (!ps)
	      psLabel -> setText (" ");
	   else {
	      psLabel -> setStyleSheet (labelStyle); 
	      psLabel -> setText ("ps");
	   }
	   if (!sbr)
	      sbrLabel -> setText ("  "); 
	   else {
	      sbrLabel -> setStyleSheet (labelStyle);
	      sbrLabel -> setText ("sbr");
	   }
	}
	std::complex<int16_t> vec [amount];
	while (theAudioBuffer. GetRingBufferReadAvailable () > amount) {
	   theAudioBuffer. getDataFromBuffer (vec, amount);
#ifdef	HAVE_PLUTO_RXTX
	   if (streamerOut_p != nullptr)
	      streamerOut_p	-> audioOut (vec, amount, rate);
#endif
//
	   std::vector<float> tmpBuffer;
	   int size = theAudioConverter. convert (vec, amount, rate, tmpBuffer);
	   if (!muteTimer. isActive ())
	      soundOut_p -> audioOutput (tmpBuffer. data (), size);

	   if (!techWindow_p -> isHidden ()) {
	      theTechData. putDataIntoBuffer (vec, amount);
	      techWindow_p	-> audioDataAvailable (amount, rate);
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
	running. store	(false);
	theSCANHandler. hide ();
	stopScanning ();
	while (theSCANHandler. active ())
	   usleep (1000);
	hideButtons	();

	theDXDisplay. hide ();
	store_widget_position (dabSettings_p, this, S_MAIN_WIDGET);
	theNewDisplay. hide ();
//
#ifdef	DATA_STREAMER
	fprintf (stderr, "going to close the dataStreamer\n");
	delete		dataStreamer_p;
#endif
#ifdef	CLOCK_STREAMER
	fprintf (stderr, "going to close the clockstreamer\n");
	delete	clockStreamer_p;
#endif
	displayTimer.	stop	();
	channelTimer.	stop	();
	presetTimer.	stop	();
	epgTimer.	stop	();
	soundOut_p	-> stop ();
	if (dlTextFile != nullptr)
	   fclose (dlTextFile);
#ifdef	HAVE_PLUTO_RXTX
	if (streamerOut_p != nullptr)
	   streamerOut_p	-> stop ();
#endif
	if (theOFDMHandler != nullptr)
	   theOFDMHandler -> stop ();
	the_ensembleHandler	-> hide ();
//	delete	the_ensembleHandler;
	delete	configHandler_p;
//	techWindow_p	-> hide ();
	delete techWindow_p;
	if (contentTable_p != nullptr) {
	   contentTable_p -> clearTable ();
	   contentTable_p -> hide ();
	   delete contentTable_p;
	}
	if (mapHandler != nullptr)
	   mapHandler ->  stop ();
//	just save a few checkbox settings that are not

	if (scanTable_p != nullptr) {
	   scanTable_p	-> clearTable ();
	   scanTable_p	-> hide ();
	   delete scanTable_p;
	}
	theSCANHandler. hide ();

	stopFramedumping	();
	stop_sourcedumping	();
	stopAudiodumping	();
	theScheduler. hide	();
	theLogger. log (logger::LOG_RADIO_STOPS);
	usleep (1000);		// pending signals
//	everything should be halted by now
	dabSettings_p	-> sync ();
	theSNRViewer. hide ();
	if (theOFDMHandler != nullptr)
	   delete	theOFDMHandler;
	if (inputDevice_p != nullptr)
	   delete	inputDevice_p;
	delete		soundOut_p;
	theScanlistHandler. hide ();
	delete	my_timeTable;
//	close();
	fprintf (stderr, ".. end the radio silences\n");
}
//
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
	   size_t idle_time	= 0, total_time;
	   get_cpu_times (idle_time, total_time);
	   const float idle_time_delta =
	                 static_cast<float>(idle_time - previous_idle_time);
	   const float total_time_delta =
	                 static_cast<float> (total_time - previous_total_time);
	   const float utilization = 100.0 * (1.0 - idle_time_delta / total_time_delta);
	   configHandler_p -> showLoad (utilization);
	   previous_idle_time = idle_time;
	   previous_total_time = total_time;
	}
//
//	The timer runs autonomously, so it might happen
//	that it rings when there is no processor running
	if (theOFDMHandler == nullptr)
	   return;
	if (!techWindow_p -> isHidden ())  {
	   if (soundOut_p -> hasMissed ()) {
	      int xxx = ((audioSink *)soundOut_p) -> missed ();
	      techWindow_p -> showMissed (xxx);
	   }
	}
}
//
//	precondition: everything is quiet
deviceHandler	*RadioInterface::create_device (const QString &s,
	                                        logger *theLogger) {
deviceHandler	*inputDevice = theDeviceChoser.
	                               createDevice  (s, version, theLogger);

	if (inputDevice	== nullptr)
	   return nullptr;

	channel. realChannel	= !inputDevice -> isFileInput ();
	if (channel. realChannel)
	   showButtons ();
	else 
	   hideButtons ();
	the_ensembleHandler -> setMode (channel. realChannel);
	connect (inputDevice, &deviceHandler::frameClosed,
	         this, &RadioInterface::handle_deviceFrame_closed);
	QString ss = s;
	store (dabSettings_p, DAB_GENERAL, SELECTED_DEVICE, ss);
	if (value_i (dabSettings_p, DAB_GENERAL, DEVICE_WIDGET_VISIBLE, 1) != 0)
	   inputDevice -> setVisibility (true);
	else
	   inputDevice -> setVisibility (false);
	theNewDisplay. set_bitDepth (inputDevice -> bitDepth ());
	return inputDevice;
}
//
//	newDevice is called from the GUI when selecting a device
//	with the selector
void	RadioInterface::newDevice (const QString &deviceName) {
//	Part I : stopping all activities
	running. store (false);
	stopScanning	();
	stopChannel	();
	fprintf (stderr, "disconnecting\n");
	if (inputDevice_p != nullptr) {
	   delete inputDevice_p;
	   inputDevice_p = nullptr;
	   fprintf (stderr, "device is deleted\n");
	}

	theLogger. log (logger::LOG_NEWDEVICE, deviceName, 
	                                channelSelector -> currentText ());
	inputDevice_p		= create_device (deviceName, &theLogger);
	if (inputDevice_p == nullptr) {
	   inputDevice_p = new deviceHandler ();
	   return;		// nothing will happen
	}
	doStart_direct ();		// will set running
}

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
	   clockStreamer_p -> sendData (localBuffer, 10);
#endif
	this	-> UTC. year		= year;
	this	-> UTC. month		= month;
	this	-> UTC. day		= d2;
	this	-> UTC. hour		= h2;
	this	-> UTC. minute		= m2;
	QString result;
	if (configHandler_p -> utcSelector_active ())
	   result	= convertTime (year, month, day, h2, m2);
	else
	   result	= convertTime (year, month, day,
	                                     hours, minutes);
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

QString	RadioInterface::convertTime (struct theTime &t) {
char dayString [3];
char hourString [3];
char minuteString [3];
	sprintf (dayString, "%.2d", t. day);
	sprintf (hourString, "%.2d", t. hour);
	sprintf (minuteString, "%.2d", t. minute);
	QString result = QString::number (t. year) + "-" +
	                       monthTable [t. month - 1] + "-" +
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
	if (!techWindow_p -> isHidden ())
	   techWindow_p -> show_frameErrors (s);
}
//
//	called from the MP4 decoder
void	RadioInterface::show_rsErrors (int s) {
	if (!running. load ())		// should not happen
	   return;
	if (!techWindow_p -> isHidden ())
	   techWindow_p	-> show_rsErrors (s);
}
//
//	called from the NP4 decoder
void	RadioInterface::show_aacErrors (int s) {
	if (!running. load ())
	   return;
	if (!techWindow_p -> isHidden ())
	   techWindow_p	-> show_aacErrors (s);
}
//
//	called from the ficHandler
void	RadioInterface::show_ficSuccess (bool b) {
	if (!running. load ())	
	   return;
	if (b) 
	   ficSuccess ++;

	if (++ficBlocks >= 100) {
	   QPalette p      = theNewDisplay. ficError_display -> palette();
	   if (ficSuccess < 85)
	      p. setColor (QPalette::Highlight, Qt::red);
	   else
	      p. setColor (QPalette::Highlight, Qt::green);

	   theNewDisplay. ficError_display	-> setPalette (p);
	   theNewDisplay. ficError_display	-> setValue (ficSuccess);
	   total_ficError	+= 100 - ficSuccess;
	   total_fics		+= 100;
	   ficSuccess		= 0;
	   ficBlocks		= 0;
	}
}
//
//	called from the PAD handler
void	RadioInterface::show_mothandling (bool b) {
	if (!running. load () || !b)
	   return;
	techWindow_p	-> show_motHandling (b);
}
	
//	just switch a color, called from the dabprocessor
void	RadioInterface::set_synced	(bool b) {
	theNewDisplay. set_syncLabel (b);
}
//
//	called from the PAD handler

void	RadioInterface::show_label	(const QString &s) {
#ifdef	HAVE_PLUTO_RXTX
	if ((streamerOut_p != nullptr) && (s != ""))
	   streamerOut_p -> addRds (std::string (s. toUtf8 (). data ()));
#endif
	if (running. load()) {
	   dynamicLabel	-> setStyleSheet (labelStyle);
	   dynamicLabel	-> setText (s);
	}
	if ((s == "") || (dlTextFile == nullptr) ||
	                                (theDLCache. addifNew (s)))
	   return;

	QString currentChannel = channel. channelName;
	QDateTime theDateTime	= QDateTime::currentDateTime ();
	fprintf (dlTextFile, "%s.%s %4d-%02d-%02d %02d:%02d:%02d  %s\n",
	                          currentChannel. toUtf8 (). data (),
	                          channel. currentService. serviceName.
	                                          toUtf8 (). data (),
	                          localTime. year,
	                          localTime. month,
	                          localTime. day,
	                          localTime. hour,
	                          localTime. minute,
	                          localTime. second,
	                          s. toUtf8 (). data ());
}

void	RadioInterface::setStereo	(bool b) {
	if (!running. load () || (stereoSetting == b))
	   return;
	if (b) {
	   stereoLabel	-> setStyleSheet (labelStyle);
	   stereoLabel	-> setText ("<i>stereo</i>");
	}
	else
	   stereoLabel	-> setText ("      ");
	   
	stereoSetting = b;
}

void	RadioInterface::handle_detailButton	() {
	if (!running. load ())
	   return;
	if (techWindow_p -> isHidden ())
	   techWindow_p -> show ();
	else
	   techWindow_p -> hide ();
	store (dabSettings_p, DAB_GENERAL, TECHDATA_VISIBLE,
	                            techWindow_p -> isHidden () ? 0 : 1);
}
//
//	Whenever the input device is a file, some functions,
//	e.g. selecting a channel, setting an alarm, are not
//	meaningful
void	RadioInterface::showButtons		() {
	scanButton		-> setEnabled (true);;
	channelSelector		-> setEnabled (true);
	nextChannelButton	-> setEnabled (true);
	prevChannelButton	-> setEnabled (true);
	scanListButton		-> setEnabled (true);
}

void	RadioInterface::hideButtons		() {
	scanButton		-> setEnabled (false);
	channelSelector		-> setEnabled (false);
	nextChannelButton	-> setEnabled (false);
	prevChannelButton	-> setEnabled (false);
	scanListButton		-> setEnabled (false);
}

void	RadioInterface::set_sync_lost	() {
}
//
//	dump handling
//
/////////////////////////////////////////////////////////////////////////

static inline
void	setButtonFont (QPushButton *b, QString text, int size) {
	QFont font	= b -> font ();
	font. setPointSize (size);
	b		-> setFont (font);
	b		-> setText (text);
	b		-> update ();
}

void	RadioInterface::handle_audiodumpButton () {
	if (!running. load () || theSCANHandler. active ())
	   return;

	if (audioDumping) 
	   stopAudiodumping ();	
	else
	   startAudiodumping ();
}

void	RadioInterface::stopAudiodumping	() {
	if (!audioDumping)
	   return;

	theLogger. log (logger::LOG_AUDIODUMP_STOPS);
	theAudioConverter. stop_audioDump ();
	audioDumping	= false;
	techWindow_p	-> audiodumpButton_text ("audio dump", 10);
}

void	RadioInterface::startAudiodumping () {
	if (audioDumping)	// should not happen
	   return;

	QString audioDumpName	=
	      theFilenameFinder.
	           findAudioDump_fileName  (channel. currentService. serviceName, true);
	if (audioDumpName == "")
	   return;

	theLogger. log (logger::LOG_AUDIODUMP_STARTS,
	                        channelSelector -> currentText (),
	                        channel. currentService. serviceName);
	
	techWindow_p	-> audiodumpButton_text ("writing", 12);
	theAudioConverter. start_audioDump (audioDumpName);
	audioDumping	= true;
}

void	RadioInterface::scheduled_audioDumping () {
	if (audioDumping) {
	   theAudioConverter. stop_audioDump	();
	   audioDumping		= false;
	   techWindow_p	-> audiodumpButton_text ("audio dump", 10);
	   return;
	}

	QString audioDumpName	=
	      theFilenameFinder.
	            findAudioDump_fileName  (serviceLabel -> text (), false);
	if (audioDumpName == "")
	   return;

	techWindow_p	-> audiodumpButton_text ("writing", 12);
	theAudioConverter. start_audioDump (audioDumpName);
	audioDumping		= true;
}

void	RadioInterface::handle_framedumpButton () {
	if (!running. load () || theSCANHandler. active ())
	   return;

	if (channel. currentService. frameDumper != nullptr) 
	   stopFramedumping ();
	else
	   startFramedumping ();
}

void	RadioInterface::stopFramedumping () {
	if (channel. currentService. frameDumper == nullptr)
	   return;

	theLogger. log (logger::LOG_FRAMEDUMP_STOPS);
	fclose (channel. currentService. frameDumper);
	techWindow_p ->  framedumpButton_text ("frame dump", 10);
	channel. currentService. frameDumper	= nullptr;
}

void	RadioInterface::startFramedumping () {
	channel. currentService. frameDumper	=
	     theFilenameFinder. findFrameDump_fileName (channel. currentService. serviceName,
	                                                              true);
	if (channel. currentService. frameDumper == nullptr)
	   return;
	theLogger. log (logger::LOG_FRAMEDUMP_STARTS, 
	                                         channel. channelName,
	                                         channel. currentService. serviceName);
	techWindow_p ->  framedumpButton_text ("recording", 12);
}

void	RadioInterface::scheduled_frameDumping (const QString &s) {
	if (channel. currentService. frameDumper != nullptr) {
	   fclose (channel. currentService. frameDumper);
	   techWindow_p ->  framedumpButton_text ("frame dump", 10);
	   channel. currentService. frameDumper	= nullptr;
	   return;
	}
	   
	channel. currentService. frameDumper	=
	     theFilenameFinder. findFrameDump_fileName (s, false);
	if (channel. currentService. frameDumper == nullptr)
	   return;
	techWindow_p ->  framedumpButton_text ("recording", 12);
}
//----------------------------------------------------------------------
//	End of section on dumping
//----------------------------------------------------------------------
//
//	called from the mp4 handler, using a signal
void	RadioInterface::newFrame        (int amount) {
uint8_t	*buffer  = (uint8_t *) alloca (amount * sizeof (uint8_t));

	if (!running. load ())
	   return;

	if (channel. currentService. frameDumper == nullptr) 
	   theFrameBuffer. FlushRingBuffer ();
	else
	while (theFrameBuffer. GetRingBufferReadAvailable () >= amount) {
	   theFrameBuffer. getDataFromBuffer (buffer, amount);
	   if (channel. currentService. frameDumper != nullptr)
	      fwrite (buffer, amount, 1, channel. currentService. frameDumper);
	}
}

void	RadioInterface::handle_spectrumButton	() {
	if (!running. load ())
	   return;
	if (theNewDisplay. isHidden ())
	   theNewDisplay. show ();
	else
	   theNewDisplay. hide ();
	store (dabSettings_p, DAB_GENERAL, NEW_DISPLAY_VISIBLE,
	                              theNewDisplay. isHidden () ? 0 : 1);
}

void	RadioInterface::handle_scanListButton    () {
	if (!running. load ())
	   return;

	if (theScanlistHandler. isHidden ())
	   theScanlistHandler. show ();
	else
	   theScanlistHandler. hide ();
}
//
//	When changing (or setting) a device, we do not want anybody
//	to have the buttons on the GUI touched, so
//	we just disconnet them and (re)connect them as soon as
//	a device is operational
void	RadioInterface::connectGUI	() {
//	connections from the main widget
	connect (prevServiceButton, &QPushButton::clicked, 
	         this, &RadioInterface::handle_prevServiceButton);
	connect (nextServiceButton, &QPushButton::clicked, 
	         this, &RadioInterface::handle_nextServiceButton);

//	channelButton handled elsewhere
	connect	(prevChannelButton, &clickablelabel::clicked,
	         this, &RadioInterface::handle_prevChannelButton);
	connect (nextChannelButton, &clickablelabel::clicked,
	         this, &RadioInterface::handle_nextChannelButton);

	connect (scanListButton, &QPushButton::clicked,
	         this, &RadioInterface::handle_scanListButton);
	               
	connect (presetButton, &QPushButton::clicked,
	         this, &RadioInterface::handle_presetButton);

	connect (spectrumButton, &QPushButton::clicked,
	         this, &RadioInterface::handle_spectrumButton);
	connect (serviceLabel, &clickablelabel::clicked,
	         this, &RadioInterface::handle_labelColor);
	connect (serviceButton, &clickablelabel::clicked,
	         this, &RadioInterface::handle_detailButton);
//
	connect (httpButton, &QPushButton::clicked,
	         this, &RadioInterface::handle_httpButton);
	connect (scanButton, &QPushButton::clicked,
	         this, &RadioInterface::handle_scanButton);
//
//	and for the techWindow
	connect (techWindow_p, &techData::handle_audioDumping,
	         this, &RadioInterface::handle_audiodumpButton);
	connect (techWindow_p, &techData::handle_frameDumping,
	         this, &RadioInterface::handle_framedumpButton);
}

void	RadioInterface::disconnectGUI () {
}
//
#include <QCloseEvent>
void RadioInterface::closeEvent (QCloseEvent *event) {
	
	int x = configHandler_p -> closeDirect_active ();
	store (dabSettings_p, CONFIG_HANDLER, CLOSE_DIRECT_SETTING, x);
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

	if (theOFDMHandler == nullptr) {
	   fprintf (stderr, "expert error 5\n");
	   return true;
	}
	else
	if ((obj == this -> the_ensembleHandler -> viewport ()) &&
	    (event -> type () == QEvent::MouseButtonPress)) {
	   QMouseEvent *ev = static_cast<QMouseEvent *>(event);
	   if (ev -> buttons () & Qt::RightButton) {
	      QTableWidgetItem *x =
	              the_ensembleHandler -> itemAt (ev -> pos ());
	      if (x != nullptr)
	         the_ensembleHandler -> handle_rightMouseClick (x -> text ());
	   }
	   else {
	      return QWidget::eventFilter (obj, event);
	   }
	}
	else
//	An option is to click - right hand mouse button - on a
//	service in the scanlist in order to add it to the
//	list of favorites
	if ((obj == this -> theScanlistHandler. viewport ()) &&
	    (event -> type () == QEvent::MouseButtonPress)) {
	   QMouseEvent *ev = static_cast<QMouseEvent *>(event);
	   if (ev -> buttons () & Qt::RightButton) {
	      QString service =
	           this -> theScanlistHandler. indexAt (ev -> pos()). data ().toString();
	     the_ensembleHandler -> add_favorite_from_scanList (service);
	   }
	}

	return QWidget::eventFilter (obj, event);
}

QPixmap RadioInterface::fetch_announcement (int id) {
QPixmap p;
QString pictureName	= QString (":res/radio-pictures/announcement%1.png").
	                                      arg (id, 2, 10, QChar ('0'));
	if (!p.load (pictureName, "png"))
	   p. load (":res/radio-pictures/announcement-d.png", "png");
	return p;
}

void	RadioInterface::start_announcement (const QString &name,
	                                    int subChId, int announcementId ) {
	if (!running. load ())
	   return;

	(void)subChId;
	if (name == serviceLabel -> text ()) {
	   if (!channel. currentService. announcement_going) {
	      serviceLabel	-> setStyleSheet ("QLabel {color : red}");
	      channel. currentService. announcement_going = true;
	      QPixmap p = fetch_announcement (announcementId);
	      displaySlide (p);
	   }
	}
}

void	RadioInterface::stop_announcement (const QString &name, int subChId) {
	(void)subChId;
	if (!running. load ())
	   return;

	if (name == channel. currentService. serviceName) {
	   if (channel. currentService. announcement_going) {
	      serviceLabel	-> setStyleSheet (labelStyle);
	      channel. currentService. announcement_going = false;
	      show_pauzeSlide ();
	   }
	}
}
//
//	selection, either direct, from presets,  from scanlist or schedule
////////////////////////////////////////////////////////////////////////
//
//	Selecting from the list of services,
//	record the <channel:servicePair>
void	RadioInterface::localSelect (const QString &service,
	                                    const QString &theChannel) {
	localSelect_SS (service, theChannel);
}
	
//	selecting from the preset list and handling delayed services
void	RadioInterface::handle_presetSelect (const QString &channel,
	                                     const QString &service) {
	if (!inputDevice_p -> isFileInput ())
	   localSelect_SS (service, channel);
	else
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("Selection not possible"));
}
//
//	selecting from the scan list, which is essential
//	the same as handling form the preset list
void	RadioInterface::handle_scanListSelect (const QString &s) {
	if (!inputDevice_p -> isFileInput ()) {
	   QStringList list        = splitter (s);
	   if (list. length () != 2)
	      return;
	   localSelect_SS (list. at (1), list. at (0));
	}
	else
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("Selection not possible"));
}
//
//	selecting from a content description
void	RadioInterface::handle_contentSelector (const QString &s) {
	localSelect_SS (s, channel. channelName);
}
//
//	From a predefined schedule list, the service names most
//	likely are less than 16 characters
//
void	RadioInterface::scheduleSelect (const QString &s) {
QStringList list	= splitter (s);
	if (list. length () != 2)
	   return;
	fprintf (stderr, "we found %s %s\n",
	                   list. at (1). toLatin1 (). data (),
	                   list. at (0). toLatin1 (). data ());
	localSelect_SS (list. at (1), list. at (0));
}
//
//	The function localSelect is called from different
//	places, making it complex
void	RadioInterface::localSelect_SS (const QString &service,
	                                const QString &theChannel) {

QString serviceName	= service;

	if (theOFDMHandler == nullptr)	// should not happen
	   return;

	channelTimer. stop ();
	presetTimer. stop  ();
	stopService (channel. currentService);

	for (int i = service. size (); i < 16; i ++)
	   serviceName. push_back (' ');

	if (!inputDevice_p -> isFileInput () &&
	                        (value_i (dabSettings_p, "channelPresets",
	                               "davidMode", D_NIX) != D_PRIV)) {
	   QString theService = service;
	   QString channelName = theChannel;
	   store (dabSettings_p, "channelPresets", channelName, theService);
	}
	if (theChannel == channel. channelName) {
	   channel. currentService. valid = false;
	   dabService s;
	   theOFDMHandler -> get_parameters (service, &s. SId, &s. SCIds);
	   if (s. SId == 0) {
	      dynamicLabel -> setText ("cannot run " +
	                       s. serviceName + " yet");
	      return;
	   }
	   s. serviceName = service;
	   startService (s);
	}
	else {
	   stopChannel ();
	   int k           = channelSelector -> findText (theChannel);
	   if (k != -1) {
	     new_channelIndex (k);
	   }
	   else {
	      QMessageBox::warning (this, tr ("Warning"),
	                            tr ("Incorrect service specification\n"));
	      return;
	   }
	   startChannel  (theChannel, serviceName);
	}
}

///////////////////////////////////////////////////////////////////////////

void	RadioInterface::stopService	(dabService &s) {
	if (!s. valid)
	   return;
	presetTimer. stop ();
	channelTimer. stop ();
	stop_muting	();
	set_soundLabel (false);
	channel. audioActive	= false;
	if (theOFDMHandler == nullptr) {
	   fprintf (stderr, "Expert error 22\n");
	   return;
	}

//	stop "dumpers"
	if (channel. currentService. frameDumper != nullptr) {
	   stopFramedumping ();
	   channel. currentService. frameDumper = nullptr;
	}

	if (audioDumping) {
	   stopAudiodumping ();
	}

//	and clean up the technical widget
	techWindow_p	-> cleanUp ();

//	stop "secondary services" - if any - as well
	if (s. valid) {
	   theOFDMHandler -> stop_service (s. subChId, FORE_GROUND);
	   if (s. is_audio) {
	      soundOut_p -> suspend ();
	      for (int i = 0; i < 5; i ++) {
	         packetdata pd;
	         theOFDMHandler -> data_for_packetservice (s. serviceName,
	                                                        pd, i);
	         if (pd. defined) {
	            theOFDMHandler -> stop_service (pd. subchId, FORE_GROUND);
	            break;
	         }
	      }
	   }
	   s. valid = false;
	}
	show_pauzeSlide ();
	cleanScreen	();
}
//
//
void	RadioInterface::startService (dabService &s) {
QString serviceName	= s. serviceName;

	channel. currentService		= s;
	channel. currentService. frameDumper	= nullptr;
	channel. currentService. valid	= false;
	theLogger. log (logger::LOG_NEW_SERVICE,
	                             channelSelector -> currentText (),
	                                         serviceName);

//	mark the selected service in the service list
//
//	and display the servicename on the serviceLabel
	serviceLabel	-> setText (serviceName);
	dynamicLabel	-> setText ("");
	the_ensembleHandler -> reportStart (serviceName);
	audiodata ad;
	theOFDMHandler -> data_for_audioservice (serviceName, ad);
	if (ad. defined) {
	   channel. currentService. valid	= true;
	   channel. currentService. is_audio	= true;
	   channel. currentService. subChId	= ad. subchId;
	   if (theOFDMHandler -> has_timeTable (ad. SId))
	      techWindow_p -> show_timetableButton (true);
	   startAudioservice (ad);
	   techWindow_p	-> is_DAB_plus  (ad. ASCTy == 077);

#ifdef	HAVE_PLUTO_RXTX
	   if (streamerOut_p != nullptr)
	      streamerOut_p -> addRds (std::string (serviceName. toUtf8 (). data ()));
#endif
	}
	else {
	   packetdata pd;
	   theOFDMHandler -> data_for_packetservice (serviceName, pd, 0);
	   if (pd. defined) {
	      channel. currentService. valid	= true;
	      channel. currentService. is_audio	= false;
	      channel. currentService. subChId	= pd. subchId;
	      startPacketservice (serviceName);
	   }
	   else {
	      QMessageBox::warning (this, tr ("Warning"),
 	                           tr ("insufficient data for this program\n"));
	      QString s = "";
	      store (dabSettings_p, DAB_GENERAL, PRESET_NAME, s);
	   }
	}
}
//
void	RadioInterface::startAudioservice (audiodata &ad) {
//	channel. currentService. valid	= true;
	(void)theOFDMHandler -> set_audioChannel (ad, &theAudioBuffer,
	                                            nullptr, FORE_GROUND);
//
//	check the other components for this service (if any)
	int nrComps	=
	     theOFDMHandler -> get_nrComps (channel. currentService. SId);
	for (int i = 1; i < nrComps; i ++) {
	   packetdata pd;
	   theOFDMHandler -> data_for_packetservice (ad. serviceName, pd, i);
	   if (pd. defined) {
	      theOFDMHandler -> set_dataChannel (pd, &theDataBuffer, FORE_GROUND);
	      break;
	   }
	}
//	activate sound
	soundOut_p -> resume ();
	channel. audioActive	= true;
	set_soundLabel (true);
	programTypeLabel	-> setText (getProgramType (ad. programType));
	rateLabel		-> setStyleSheet (labelStyle);
	rateLabel		-> setText (QString::number (ad. bitRate) + "kbit");
//	show service related data
	techWindow_p	-> show_serviceData 	(&ad);
}

void	RadioInterface::startPacketservice (const QString &s) {
packetdata pd;

	theOFDMHandler -> data_for_packetservice (s, pd, 0);
	if ((!pd. defined) ||
	            (pd.  DSCTy == 0) || (pd. bitRate == 0)) {
	   QMessageBox::warning (this, tr ("sdr"),
 	                         tr ("still insufficient data for this service\n"));
	   return;
	}

	if (!theOFDMHandler -> set_dataChannel (pd,
	                                         &theDataBuffer, FORE_GROUND)) {
	   QMessageBox::warning (this, tr ("sdr"),
 	                         tr ("could not start this service\n"));
	   return;
	}

	int nrComps	= 
	     theOFDMHandler -> get_nrComps (channel. currentService. SId);
	if (nrComps > 1)
//	   fprintf (stderr,  "%s has %d components\b",
//	                     channel. currentService. serviceName. toLatin1 (). data (), nrComps);
	for (int i = 1; i < nrComps; i ++) {
	   packetdata lpd;
	   theOFDMHandler -> data_for_packetservice (pd. serviceName, lpd, i);
	   if (lpd. defined) {
	      theOFDMHandler -> set_dataChannel (lpd, &theDataBuffer, FORE_GROUND);
//	      fprintf (stderr, "adding %s (%d) as subservice\n",
//	                            lpd. serviceName. toUtf8 (). data (),
//	                            lpd. subchId);
	      break;
	   }
	}

	switch (pd. DSCTy) {
	   default:
	      show_label (QString ("unimplemented Data"));
	      break;
	   case 5:
//	      fprintf (stderr, "selected apptype %d\n", pd. appType);
	      show_label (QString ("Transp. Channel partially implemented"));
	      break;
	   case 60:
	      show_label (QString ("MOT"));
	      break;
	   case 59: {
#ifdef	_SEND_DATAGRAM_
	      QString text = QString ("Embedded IP: UDP data to ");
	      text. append (ipAddress);
	      text. append (" ");
	      QString n = QString::number (port);
	      text. append (n);
	      show_label (text);
#else
	      show_label ("Embedded IP not supported ");
#endif
	   }
	      break;
	   case 44:
	      show_label (QString ("Journaline"));
	      break;
	}
}

//	This function is only used in the Gui to clear
//	the details of a selected service
void	RadioInterface::cleanScreen	() {
	serviceLabel			-> setText ("");
	dynamicLabel			-> setText ("");
	techWindow_p			-> cleanUp ();
	stereoLabel			-> setText ("");
	programTypeLabel 		-> setText ("");
	psLabel				-> setText ("");
	sbrLabel			-> setText ("");
	audiorateLabel			-> setText ("");
	rateLabel			-> setText ("");
	stereoLabel			-> setText ("");

	stereoSetting			= false;
	techWindow_p			-> cleanUp ();
	setStereo	(false);
	distanceLabel			-> setText ("");
	transmitter_country		-> setText ("");
	theNewDisplay. ficError_display	-> setValue (0);
}

////////////////////////////////////////////////////////////////////////////
//
void	RadioInterface::handle_prevServiceButton        () {
	the_ensembleHandler -> selectPrevService ();
}

void	RadioInterface::handle_nextServiceButton        () {
	the_ensembleHandler -> selectNextService ();
}
//
//	The user(s)
///////////////////////////////////////////////////////////////////////////
//	setPresetService () is called after a time out to 
//	actually start the service that we were waiting for
void	RadioInterface::setPresetService () {
	if (!running. load ())
	   return;
	stopScanning ();	// should not run
	presetTimer. stop ();

	if (nextService. channel != channel. channelName)
	   return;

	if (channel. Eid == 0) {
	   dynamicLabel -> setText ("ensemblename not yet found\n");
	   return;
	}
	QString presetName	= nextService. serviceName;
	for (int i = presetName. length (); i < 16; i ++)
	   presetName. push_back (' ');
	dabService s = nextService;
	theOFDMHandler	-> get_parameters (presetName, &s. SId, &s. SCIds);
	if (s. SId == 0) {
	   dynamicLabel -> setText (QString ("not all data for ") +
	                            s. serviceName +
	                             " on board");
	   return;
	}
	startService (s);
}
//
//	Channel basics
///////////////////////////////////////////////////////////////////////////
//	Precondition: no channel should be active
//
//	start channel gets as second parameter a servicename (or
//	an empty string. If it is a serviceName, then the
//	function will initiate a start of the service of that name,
//	otherwise it will start the service found associated to the
//	channel (if any)
void	RadioInterface::startChannel (const QString &theChannel,
	                                 QString firstService) {
int	tunedFrequency	=
	         theSCANHandler. Frequency (theChannel);
	the_ensembleHandler	-> reset ();
	theNewDisplay. showFrequency (theChannel, tunedFrequency);
	usleep (1000);
	inputDevice_p		-> restartReader (tunedFrequency);

	presetTimer. stop	();		// should not run
	channel. cleanChannel	();
	channel. channelName	= theChannel;
	channel. tunedFrequency	= tunedFrequency;
	channel. countryName	= "";
	theLogger. log (logger::LOG_NEW_CHANNEL, theChannel, channel. snr);
	channel. realChannel	= !inputDevice_p -> isFileInput ();
	if (channel. realChannel) {
	   QString sChannel = theChannel;
	   store (dabSettings_p, DAB_GENERAL, CHANNEL_NAME, sChannel);
	}
//
//	The ".sdr" and ".uff" files - when built by us - carry
//	the channel frequency in their data
	if (inputDevice_p -> isFileInput ()) {
	   channelSelector		-> setEnabled (false);
	   int freq	= inputDevice_p -> getVFOFrequency ();
	   QString realChannel = theSCANHandler. getChannel (freq);
	   if (realChannel != "") {
	      int k = channelSelector -> findText (realChannel);
	      channelSelector -> setCurrentIndex (k);
	      channel. channelName	= realChannel;
	      channel. tunedFrequency	= freq;
	      theNewDisplay. showFrequency (realChannel,
	                                    channel. tunedFrequency);
	   }
	   else {
	      channel. channelName	= "";
	      channel. tunedFrequency	= -1;
	   }
	}

	distanceLabel		-> setText ("");
	theDXDisplay. cleanUp ();
	theNewDisplay. show_transmitters (channel. transmitters);
	bool localTransmitters =
	             configHandler_p -> localTransmitterSelector_active ();
	if (localTransmitters  && (mapHandler != nullptr))
	   mapHandler -> putData (MAP_RESET, channel. targetPos);
	else
	if (mapHandler != nullptr)
	   mapHandler -> putData (MAP_FRAME, position {-1, -1});
//
	int switchDelay		=
	             configHandler_p -> switchDelayValue ();
//	if no preset is started, we look in the tables what the servicename
//	was the last time the channel was active
	if (!inputDevice_p -> isFileInput () &&
	                   !theSCANHandler. active ()) {
	   if (firstService == "")
	      firstService =
	            value_s (dabSettings_p, "channelPresets", theChannel, "");
//
//	at this point we do not know whether or not a preset is 
//	set, so if this mode is "on" and there is a service name
//	associated with the channel, we set the [reset handling on
//	but only if ....
	   if (firstService != "") {
	      nextService. channel	= theChannel;
	      nextService. serviceName	= firstService;
	      nextService. SId		= 0;
	      nextService. SCIds	= 0;
	      presetTimer. setSingleShot (true);
	      presetTimer. setInterval (switchDelay);
	      presetTimer. start (switchDelay);
	   }
	}
	if (!theSCANHandler. active ())
	   epgTimer. start (switchDelay);
//
//	all set, go for it
	theOFDMHandler		-> start ();
}
//
//	apart from stopping the reader, a lot of administration
//	is to be done.
void	RadioInterface::stopChannel	() {
	if (inputDevice_p == nullptr)		// should not happen
	   return;

	epgTimer. stop		();		// if running
	presetTimer. stop 	();		// if running
	channelTimer. stop	();		// if running
	inputDevice_p		-> stopReader ();
	disconnect (ensembleId, &clickablelabel::clicked,
	            this, &RadioInterface::handle_contentButton);
	ensembleId	-> setText ("");
	stop_sourcedumping	();
	stop_etiHandler	();	// if any
	theLogger. log (logger::LOG_CHANNEL_STOPS, channel. channelName);
	transmitter_country	-> setText ("");
//
//	first, stop services in fore and background
	if (channel. currentService. valid)
	   stopService (channel. currentService);
	soundOut_p	-> suspend ();

	for (auto s : channel. backgroundServices) {
	   theOFDMHandler -> stop_service (s. subChId, BACK_GROUND);
	   if (s. fd != nullptr)
	      fclose (s. fd);
	}
	channel. backgroundServices. clear ();

	if (contentTable_p != nullptr) {
	   contentTable_p -> hide ();
	   delete contentTable_p;
	   contentTable_p = nullptr;
	}
//	note framedumping - if any - was already stopped
//	ficDumping - if on - is stopped here
	if (ficDumpPointer != nullptr) {
	   theOFDMHandler -> stop_ficDump ();
	   ficDumpPointer = nullptr;
	}
	theOFDMHandler		-> stop ();
	theDXDisplay. cleanUp ();
	usleep (1000);
	techWindow_p	-> cleanUp ();

	show_pauzeSlide ();
	channel. cleanChannel	();
	bool localTransmitters	=
	             configHandler_p -> localTransmitterSelector_active ();
	if (localTransmitters && (mapHandler != nullptr))
	   mapHandler -> putData (MAP_RESET, channel. targetPos);
	QCoreApplication::processEvents ();
//
//	no processing left at this time
	usleep (1000);		// may be handling pending signals?
//	all stopped, now look at the GUI elements
//	the visual elements related to service and channel
	set_synced	(false);
	cleanScreen	();
}
//
//	next- and previous channel buttons
/////////////////////////////////////////////////////////////////////////

void	RadioInterface::handle_channelSelector (const QString &channel) {
	if (!running. load ())
	   return;

//	LOG select channel
	presetTimer. stop ();
	stopScanning	();
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
	if (theOFDMHandler == nullptr) {
	   fprintf (stderr, "Expert error 23\n");
	   abort ();
	}

	stopScanning	();
	stopChannel	();
	new_channelIndex (currentChannel);
	startChannel (channelSelector -> currentText ());
}
//
//	scanning
//	The scan function covers three scan strategies. In order to make things
//	manageable, we implement the streams  in different functions and procedures
void	RadioInterface::handle_scanButton () {
	if (!running. load ())
	   return;
	if (theSCANHandler. isVisible ())
	   theSCANHandler. hide ();
	else
	   theSCANHandler. show ();
}

void	RadioInterface::startScanning	() {
	the_ensembleHandler	-> set_showMode (SHOW_ENSEMBLE);
	presetButton		-> setText ("not in use");
	presetButton	-> setEnabled (false);
	stopChannel     ();
	presetTimer. stop ();
	channelTimer. stop ();
	if (inputDevice_p -> isFileInput ())
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("Scanning not useful with file input"));

	epgTimer. stop ();
	connect (theOFDMHandler, &ofdmHandler::no_signal_found,
	         this, &RadioInterface::no_signal_found);

	if (theSCANHandler. scan_to_data ())
	   start_scan_to_data ();
	else
	if (theSCANHandler. scan_single ())
	   start_scan_single ();
	else
	   start_scan_continuous ();
}

void	RadioInterface::start_scan_to_data () {
//	when running scan to data, we look at all channels, whether
//	on the skiplist or not
	QString cs = theSCANHandler. getNextChannel (channelSelector -> currentText ());
	int cc = channelSelector -> findText (cs);
//	LOG scanning starts
	new_channelIndex (cc);
//	theSCANHandler. addText (" scanning channel " +
//	                            channelSelector -> currentText ());
	int switchDelay		=
	              configHandler_p -> switchDelayValue ();
	channelTimer. start (switchDelay);
	theOFDMHandler	-> set_scanMode (true);
	startChannel    (channelSelector -> currentText ());
}

void	RadioInterface::start_scan_single () {
	if (configHandler_p -> clearScan_Selector_active ())
	   theScanlistHandler. clear_scanList ();

	if (scanTable_p == nullptr) 
	   scanTable_p = new contentTable (this, dabSettings_p, "scan", 
	                                       theOFDMHandler -> scanWidth ());
	else					// should not happen
	   scanTable_p -> clearTable ();

	QString topLine = QString ("ensemble") + ";"  +
	                           "channelName" + ";" +
	                           "frequency (KHz)" + ";" +
	                           "Eid" + ";" +
	                           "tii" + ";" +
	                           "time" + ";" +
	                           "SNR" + ";" +
	                           "nr services" + ";";
	scanTable_p	-> addLine (topLine);
	scanTable_p	-> addLine ("\n");

	theOFDMHandler	-> set_scanMode (true);
	QString fs	= theSCANHandler. getFirstChannel ();
	int k = channelSelector ->  findText (fs);
	if (k != -1)
	   new_channelIndex (k);
	theSCANHandler. addText (" scanning channel " +
	                            channelSelector -> currentText ());
	int switchDelay		=
	             configHandler_p -> switchDelayValue ();
	channelTimer. start (switchDelay);
	startChannel    (channelSelector -> currentText ());
}

void	RadioInterface::start_scan_continuous () {
	if (scanTable_p == nullptr) 
	   scanTable_p = new contentTable (this, dabSettings_p, "scan", 
	                                     theOFDMHandler -> scanWidth ());
	else					// should not happen
	   scanTable_p -> clearTable ();

	QString topLine = QString ("ensemble") + ";"  +
	                           "channelName" + ";" +
	                           "frequency (KHz)" + ";" +
	                           "Eid" + ";" +
	                           "tii" + ";" +
	                           "time" + ";" +
	                           "SNR" + ";" +
	                           "nr services" + ";";
	scanTable_p -> addLine (topLine);
	scanTable_p	-> addLine ("\n");

	theOFDMHandler	-> set_scanMode (true);
//      To avoid reaction of the system on setting a different value:
	QString fs = theSCANHandler. getFirstChannel ();
	int k = channelSelector -> findText (fs);
	new_channelIndex (k);
	int switchDelay		=
	             configHandler_p -> switchDelayValue ();
	channelTimer. start (2 * switchDelay);
	startChannel    (channelSelector -> currentText ());
}
//
//	stop_scanning is called
//	1. when the scanbutton is touched during scanning
//	2. on user selection of a service or a channel select
//	3. on device selection
//	4. on handling a reset
void	RadioInterface::stopScanning	() {
	disconnect (theOFDMHandler, &ofdmHandler::no_signal_found,
	            this, &RadioInterface::no_signal_found);
	if (!theSCANHandler. active ())
	   return;
	presetButton	-> setText ("favorites");
	presetButton	-> setEnabled (true);
//	LOG scanning stops
	channelTimer. stop ();

	if (theSCANHandler. scan_to_data ())
	   stop_scan_to_data ();
	else
	if (theSCANHandler. scan_single ())
	   stop_scan_single ();
	else
	   stop_scan_continuous ();
	theSCANHandler. setStop ();
//	presetButton	-> setEnabled (true);
}

void	RadioInterface::stop_scan_to_data () {
	theOFDMHandler	-> set_scanMode (false);
	channelTimer. stop ();
}

void	RadioInterface::stop_scan_single () {
	theOFDMHandler	-> set_scanMode (false);
	channelTimer. stop ();

	if (scanTable_p == nullptr)
	   return;		// should not happen

	if (configHandler_p -> upload_selector_active ()) {
	   try {
	      uploader the_uploader;
	      QMessageBox::StandardButton reply =
	              QMessageBox::question (this,
	                                     "upload content to fmlist.org?", "",
	                              QMessageBox::Yes | QMessageBox::No);
	      if (reply == QMessageBox::Yes) {
	         the_uploader. loadUp ("Scan",
	                               0,
	                               "result table",
	                               scanTable_p -> upload ());
	      }
	   } catch (...) {}
	}

	FILE *scanDumper_p	= theSCANHandler. askFileName ();
	if (scanDumper_p != nullptr) {
	   scanTable_p -> dump (scanDumper_p);
	   fclose (scanDumper_p);
	   scanDumper_p = nullptr;
	}
	delete scanTable_p;
	scanTable_p	= nullptr;
}

void	RadioInterface::stop_scan_continuous () {
	theOFDMHandler	-> set_scanMode (false);
	channelTimer. stop ();

	if (scanTable_p == nullptr)
	   return;		// should not happen

	FILE *scanDumper_p	= theSCANHandler. askFileName ();
	if (scanDumper_p != nullptr) {
	   scanTable_p -> dump (scanDumper_p);
	   fclose (scanDumper_p);
	   scanDumper_p = nullptr;
	}
	delete scanTable_p;
	scanTable_p	= nullptr;
}

//	If the ofdm processor has waited - without success -
//	for a period of N frames to get a start of a synchronization,
//	it sends a signal to the GUI handler
//	If "scanning" is "on" we hop to the next frequency on
//	the list.
//	Also called as a result of time out on channelTimer

void	RadioInterface::channel_timeOut () {
	channelTimer. stop ();
	if (!theSCANHandler. active ())
	   return;

	if (theSCANHandler. scan_to_data ())
	   next_for_scan_to_data ();
	else	
	if (theSCANHandler. scan_single ())
	   next_for_scan_single ();
	else
	   next_for_scan_continuous ();
}

void	RadioInterface::next_for_scan_to_data () {
	if  (channel. nrServices > 0) {
	   stopScanning ();
	   return;
	}
	stopChannel ();
	QString ns	= theSCANHandler. getNextChannel ();
	int cc = channelSelector -> findText (ns);
	new_channelIndex (cc);
//
//	and restart for the next run
	theSCANHandler. addText ("scanning channel " +
	                             channelSelector -> currentText ());
	int switchDelay	= 
	         configHandler_p -> switchDelayValue ();
	channelTimer. start (switchDelay);
	startChannel (channelSelector -> currentText ());
}

void	RadioInterface::next_for_scan_single () {
	if (channel. nrServices > 0)
	   show_for_single_scan ();
	stopChannel ();
	try {
	   QString cs	= theSCANHandler. getNextChannel ();
	   int	cc	= channelSelector -> findText (cs);
	   new_channelIndex (cc);
	} catch (...) {
	   stopScanning ();
	   return;
	}

	theSCANHandler. addText ("scanning channel " +
	                             channelSelector -> currentText ());
	int switchDelay	= 
	         configHandler_p -> switchDelayValue ();
	channelTimer. start (switchDelay);
	startChannel (channelSelector -> currentText ());
}

void	RadioInterface::next_for_scan_continuous () {
	if (channel. nrServices > 0)
	   show_for_continuous ();
	stopChannel ();

	QString cs	= theSCANHandler. getNextChannel ();
	int cc	= channelSelector -> findText (cs);
	new_channelIndex (cc);

	int switchDelay	= 
	         configHandler_p -> switchDelayValue ();
	channelTimer. start (2 * switchDelay);
	startChannel (channelSelector -> currentText ());
}

////////////////////////////////////////////////////////////////////////////
QString RadioInterface::build_headLine () {
QString SNR 		= "SNR " + QString::number (channel. snr);
QString	tii;
QString	theName;
QString theDistance;
QString	theCorner;
QString	theHeight;

	if (theOFDMHandler == nullptr) {	// cannot happen
	   fprintf (stderr, "Expert error 26\n");
	   return "";
	}

	if (channel. mainId != -1) 
	   tii		= ids_to_string (channel. mainId,
	                                     channel. subId) + ";" ;
	else
	   tii		= "?,?;";
	if (channel. transmitterName != "")
	   theName	= channel. transmitterName + ";";
	else
	   theName 	= ";";
	if (channel. distance > 0) {
	   theDistance	= QString::number (channel. distance, 'f', 1) + " km ";
	   theCorner	= QString::number (channel. corner, 'f', 1)
	                      + QString::fromLatin1 (" \xb0 ");
	   theHeight	= " (" + QString::number (channel. height, 'f', 1) +  "m)" + "\n";
	}
	else {
	   theDistance	= "unknown";
	   theCorner	= "";
	   theHeight	= "\n";
	}
	   
	QString utcTime	= convertTime (UTC. year, UTC.month,
	                               UTC. day, UTC. hour, 
	                               UTC. minute);
	QString headLine = channel. ensembleName + ";" +
	                      channel. channelName  + ";" +
	                      QString::number (channel. tunedFrequency) + ";" +
	                      hextoString (channel. Eid) + ";" +
	                      tii +
	                      utcTime + ";" +
	                      SNR + ";" +
	                      QString::number (channel. nrServices) +";" +
	                      theName + theDistance + theCorner + theHeight;
	return headLine;
}

void	RadioInterface::show_for_single_scan () {
QString	headLine = build_headLine ();
	QStringList s = theOFDMHandler -> basicPrint ();
	scanTable_p -> addLine (headLine);
	scanTable_p -> addLine ("\n;\n");
	for (const auto &l : s)
	   scanTable_p -> addLine (l);
	scanTable_p -> addLine ("\n;\n;\n");
	scanTable_p -> show ();
}

void	RadioInterface::show_for_continuous () {
	QString headLine = build_headLine ();
	scanTable_p -> addLine (headLine);

	for (auto &tr: channel. transmitters) {
	   if (!tr. isStrongest) {
	      QString line = build_cont_addLine (tr);
	      if (line == "")
	         continue;
	      scanTable_p -> addLine (line);
	   }
	}
	scanTable_p -> show ();
}

QString RadioInterface::build_cont_addLine (transmitterDesc &tr) {
QString tii;
QString theName;
QString theDistance;
QString theCorner;
QString theHeight;

	tii		= ids_to_string (tr. theTransmitter. mainId,
	                                 tr. theTransmitter. subId) + ";" ;
	if (tr. theTransmitter. transmitterName != "")
	   theName	= tr. theTransmitter. transmitterName + ";";
	else
	   theName 	= ";";
	
	if (tr. distance > 0) {
	   theDistance	= QString::number (tr. distance, 'f', 1) + " km ";
	   theCorner	= QString::number (tr. corner, 'f', 1)
	                      + QString::fromLatin1 (" \xb0 ");
	   theHeight	= " (" + QString::number (tr. theTransmitter. height, 'f', 1) +  "m)" + "\n";
	}
	else {
	   theDistance	= "unknown";
	   theCorner	= "?";
	   theHeight	= "?";
	}
	return  QString (";") +
	                 ";" +
	                 ";" +
	                 ";" +
	                 tii +
	                 ";" +
	                 ";" +
	                 ";" +
	                  theName + theDistance + theCorner + theHeight;
}

/////////////////////////////////////////////////////////////////////
//
//	Handling the Mute button
void	RadioInterface::handle_muteButton	() {
	if (muteTimer. isActive ()) {
	   stop_muting ();
	   return;
	}
	if (!channel. audioActive)
	   return;
	set_soundLabel (false);
	connect (&muteTimer, &QTimer::timeout,
	         this, &RadioInterface::muteButton_timeOut);
	muteDelay	= configHandler_p -> muteValue ();
	muteDelay	*= 60;	// seconds
	muteTimer. start (1000);
	stillMuting	-> show ();
	stillMuting	-> display (muteDelay);
}

void	RadioInterface::muteButton_timeOut	() {
	muteDelay --;
	if (muteDelay > 0) {
	   stillMuting -> display (muteDelay);
	   muteTimer. start (1000);
	   return;
	}
	else {
	   disconnect (&muteTimer, &QTimer::timeout,
	               this, &RadioInterface::muteButton_timeOut);
	   stillMuting	-> hide ();
	   if (channel. audioActive)
	      set_soundLabel (true);
	}
}

void	RadioInterface::stop_muting		() {
	if (!muteTimer. isActive ()) 
	   return;
	set_soundLabel (true);
	muteTimer. stop ();
	disconnect (&muteTimer, &QTimer::timeout,
	               this, &RadioInterface::muteButton_timeOut);
	stillMuting	-> hide ();
}
//
//	End of handling mute button

//	new_channelIndex is called whenever we are sure that
//	the channel selector is "connected", and we programamtically
//	change the setting, which obviously would lead to a signal
//	that we do not wwant right now

void	RadioInterface::new_channelIndex (int index) {
	if (channelSelector -> currentIndex () == index)
	   return;
	channelSelector	-> setEnabled (false);
	channelSelector	-> setCurrentIndex (index);
	channelSelector	-> setEnabled (true);
}
//
/////////////////////////////////////////////////////////////////////////
//	merely as a gadget, for each button the color can be set
//	Lots of code, about 400 lines, just for a gadget
//	
void	RadioInterface::set_Colors () {

QString scanButton_color =
	   value_s (dabSettings_p, COLOR_SETTINGS, SCAN_BUTTON + "_color",
	                                             GREEN);
QString scanButton_font =
	   value_s (dabSettings_p, COLOR_SETTINGS, SCAN_BUTTON + "_font",
	                                              BLACK);

QString spectrumButton_color =
	   value_s (dabSettings_p, COLOR_SETTINGS, SPECTRUM_BUTTON + "_color",
	                                              BLUE);
QString spectrumButton_font =
	   value_s (dabSettings_p, COLOR_SETTINGS, SPECTRUM_BUTTON + "_font",
	                                              BLACK);
QString scanListButton_color =
	   value_s (dabSettings_p, COLOR_SETTINGS, SCANLIST_BUTTON + "_color",
	                                              GREEN);
QString scanListButton_font =
	   value_s (dabSettings_p, COLOR_SETTINGS, SCANLIST_BUTTON + "_font",
	                                              BLACK);
QString presetButton_color =
	   value_s (dabSettings_p, COLOR_SETTINGS, PRESET_BUTTON + "_color",
	                                              GREEN);
QString presetButton_font =
	   value_s (dabSettings_p, COLOR_SETTINGS, PRESET_BUTTON + "_font",
	                                              BLACK);
QString prevServiceButton_color =
	   value_s (dabSettings_p, COLOR_SETTINGS,
	                            PREVSERVICE_BUTTON + "_color", YELLOW);
QString prevServiceButton_font =
	   value_s (dabSettings_p, COLOR_SETTINGS,
	                    PREVSERVICE_BUTTON + "_font", BLACK);
QString nextServiceButton_color =
	   value_s (dabSettings_p, COLOR_SETTINGS,
	                     NEXTSERVICE_BUTTON + "_color", YELLOW);
QString nextServiceButton_font =
	   value_s (dabSettings_p, COLOR_SETTINGS,
	                     NEXTSERVICE_BUTTON + "_font", BLACK);

QString	configButton_color =
	   value_s (dabSettings_p, COLOR_SETTINGS, CONFIG_BUTTON + "_color",
	                                              YELLOW);
QString configButton_font	=
	   value_s (dabSettings_p, COLOR_SETTINGS, CONFIG_BUTTON + "_font",
	                                              BLACK);
QString	httpButton_color =
	   value_s (dabSettings_p, COLOR_SETTINGS, HTTP_BUTTON + "_color",
	                                              YELLOW);
QString httpButton_font	=
	   value_s (dabSettings_p, COLOR_SETTINGS, HTTP_BUTTON + "_font",
	                                              BLACK);

	QString temp = "QPushButton {background-color: %1; color: %2}";
	spectrumButton ->
	              setStyleSheet (temp. arg (spectrumButton_color,
	                                        spectrumButton_font));
	scanListButton	->
	              setStyleSheet (temp. arg (scanListButton_color,
	                                        scanListButton_font));
	presetButton    -> 
	              setStyleSheet (temp. arg (presetButton_color,
	                                        presetButton_font));
	scanButton	->
	              setStyleSheet (temp. arg (scanButton_color,
	                                        scanButton_font));
	configButton	->
	              setStyleSheet (temp. arg (configButton_color,
	                                        configButton_font));
	httpButton	->
	              setStyleSheet (temp. arg (httpButton_color,
	                                        httpButton_font));
	prevServiceButton ->
	              setStyleSheet (temp. arg (prevServiceButton_color,
	                                        prevServiceButton_font));
	nextServiceButton ->
	              setStyleSheet (temp. arg (nextServiceButton_color, 
	                                        nextServiceButton_font));
}

void	RadioInterface::color_scanButton	() {
	set_buttonColors (scanButton, SCAN_BUTTON);
}

void	RadioInterface::color_spectrumButton	()	{
	set_buttonColors (spectrumButton, SPECTRUM_BUTTON);
}

void	RadioInterface::color_scanListButton	()	{
	set_buttonColors (scanListButton, SCANLIST_BUTTON);
}
	                                        
void    RadioInterface::color_presetButton      ()      {
	set_buttonColors (presetButton, PRESET_BUTTON);
}  

void	RadioInterface::color_prevServiceButton ()      {
	set_buttonColors (prevServiceButton, PREVSERVICE_BUTTON);
}                                               

void	RadioInterface::color_nextServiceButton ()      {
	set_buttonColors (nextServiceButton, NEXTSERVICE_BUTTON);
}                               
	   
void	RadioInterface::color_configButton	() 	{
	set_buttonColors (configButton, CONFIG_BUTTON);
}

void	RadioInterface::color_httpButton	() 	{
	set_buttonColors (httpButton, HTTP_BUTTON);
}

void	RadioInterface::set_buttonColors	(QPushButton *b,
	                                         const QString &buttonName) {
QColor	baseColor, textColor;

	QColor color = QColorDialog::getColor (baseColor, nullptr, "baseColor");
	if (!color. isValid ())
	   return;
	baseColor	= color;
	color = QColorDialog::getColor (textColor, nullptr, "textColor");
	if (!color. isValid ())
	   return;
	textColor	= color;
	QString temp = "QPushButton {background-color: %1; color: %2}";
	b	-> setStyleSheet (temp. arg (baseColor. name (),
	                                     textColor. name ()));

	QString buttonColor	= buttonName + "_color";
	QString buttonFont	= buttonName + "_font";
	QString baseColor_name	= baseColor. name ();
	QString textColor_name	= textColor. name ();
	store (dabSettings_p, COLOR_SETTINGS, buttonColor, baseColor_name);
	store (dabSettings_p, COLOR_SETTINGS, buttonFont, textColor_name);
}

///////////////////////////////////////////////////////////////////////////
//	Handling schedule

static
const char	*scheduleList[] = {"nothing", "exit",
	                           "framedump", "dltext", "ficDump"};

void	RadioInterface::handle_scheduleButton	() {
QStringList candidates;
scheduleSelector theSelector;
QString		scheduleService;

	for (int i = 0; i < 5; i ++) {
	   theSelector. addtoList (QString (scheduleList [i]));
	   candidates += QString (scheduleList [i]);
	}

	QStringList selectables	= the_ensembleHandler -> getSelectables ();
	for (auto &candidate: selectables) {
	   if (!candidates. contains (candidate)) {
	      theSelector.
	              addtoList (candidate);
	      candidates += candidate;
	   }
	}

	int selected		= theSelector. QDialog::exec ();
	scheduleService		= candidates. at (selected);
	{  elementSelector	theElementSelector (scheduleService);
	   int	targetTime	= theElementSelector. QDialog::exec ();
	   int delayDays	= (targetTime & 0XF0000) >> 16;
	   targetTime		= targetTime & 0xFFFF;
	   theScheduler. addRow (scheduleService,
	                         delayDays,
	                         targetTime / 60, 
	                         targetTime % 60);
	}
	theScheduler. show ();
}

void	RadioInterface::scheduler_timeOut	(const QString &s) {
	if (!running. load ())
	   return;

	if (s == "nothing")
	   return;

	if (s == "exit") {
	   configHandler_p -> set_closeDirect (true);
	   QWidget::close ();
	   return;
	}

	if (s ==  "framedump") {
	   scheduled_frameDumping (channel.currentService. serviceName);
	   return;
	}

	if (s ==  "audiodump") {
	   scheduled_audioDumping ();
	   return;
	}

	if (s == "dlText") {
	   scheduled_dlTextDumping ();
	   return;
	}

	if (s == "ficDump") {
	   scheduled_ficDumping ();
	   return;
	}

	presetTimer. stop ();
	stopScanning ();
	scheduleSelect (s);
}

void	RadioInterface::scheduled_ficDumping () {
	if (ficDumpPointer == nullptr) {
	   ficDumpPointer     =
	     theFilenameFinder. find_ficDump_file (channel. channelName);
	   if (ficDumpPointer == nullptr)
	      return;
	   theOFDMHandler -> start_ficDump (ficDumpPointer);
	   return;
	}
	theOFDMHandler	-> stop_ficDump ();
	ficDumpPointer = nullptr;
}

//------------------------------------------------------------------------
//
//	if configured, the interpreation of the EPG data starts automatically,
//	the servicenames of an SPI/EPG service may differ from one country
//	to another
void	RadioInterface::epgTimer_timeOut	() {
	epgTimer. stop ();
	
	if (value_i (dabSettings_p, CONFIG_HANDLER, "epgFlag", 0) != 1)
	   return;
	if (theSCANHandler. active ())
	   return;
	QStringList epgList = the_ensembleHandler -> get_epgServices ();
	for (auto serv : epgList) {
	   packetdata pd;
	   fprintf (stderr, "Looking to %s\n", serv. toLatin1 (). data ());
	   theOFDMHandler -> data_for_packetservice (serv, pd, 0);
	   if ((!pd. defined) ||
	            (pd.  DSCTy == 0) || (pd. bitRate == 0)) 
	      continue;
	   fprintf (stderr, "YES\n");
	   if (pd. DSCTy == 60) {
//	LOG hidden service starts
	      fprintf (stderr, "Starting hidden service %s\n",
	                                serv. toUtf8 (). data ());
	      theOFDMHandler -> set_dataChannel (pd, &theDataBuffer, BACK_GROUND);
	      dabService s;
	      s. channel     = pd. channel;
	      s. serviceName = pd. serviceName;
	      s. SId         = pd. SId;
	      s. SCIds	     = pd. SCIds;
	      s. subChId     = pd. subchId;
	      s. fd          = nullptr;
	      channel. backgroundServices. push_back (s);
	   }
	}
}

void	RadioInterface::set_epgData (int SId, int theTime,
	                             const QString &theText,
	                             const QString &theDescr) {
	if (theOFDMHandler != nullptr)
	   theOFDMHandler -> set_epgData (SId, theTime,
	                                   theText, theDescr);
}

void	RadioInterface::handle_timeTable	() {
int	epgWidth;
	if (!my_timeTable -> isHidden ()) {
	   my_timeTable -> hide ();
	   return;
	}
	if (!channel. currentService. valid ||
	                     !channel. currentService. is_audio)
	   return;

	my_timeTable	-> clear ();
	epgWidth	= value_i (dabSettings_p, DAB_GENERAL, "epgWidth", 70);
	if (epgWidth < 50)
	   epgWidth = 50;
	std::vector<epgElement> res =
	           theOFDMHandler -> find_epgData (channel. currentService. SId);
	for (const auto& element: res)
	   my_timeTable -> addElement (element. theTime,
	                               epgWidth,
	                               element. theText,
	                               element. theDescr);
	my_timeTable	-> show ();
}

//----------------------------------------------------------------------
//
void	RadioInterface::scheduled_dlTextDumping () {
	if (dlTextFile != nullptr) {
	   fclose (dlTextFile);
	   dlTextFile = nullptr;
	   configHandler_p	-> mark_dlTextButton (false);
	   return;
	}

	QString	fileName = theFilenameFinder. finddlText_fileName (false);
	dlTextFile	= fopen (fileName. toUtf8 (). data (), "w+");
	if (dlTextFile == nullptr)
	   return;
	configHandler_p -> mark_dlTextButton (true);
}
//
//---------------------------------------------------------------------
//
void	RadioInterface::handle_configButton	() {
	if (!configHandler_p -> isHidden ()) {
	   configHandler_p ->  hide ();	
	   store (dabSettings_p, DAB_GENERAL, CONFIG_WIDGET_VISIBLE, 0);
	}
	else {
	   configHandler_p -> show ();
	   store (dabSettings_p, DAB_GENERAL, CONFIG_WIDGET_VISIBLE, 1);
	}
}

void	RadioInterface::handle_devicewidgetButton	() {
	if (inputDevice_p == nullptr)
	   return;
	inputDevice_p	-> setVisibility (!inputDevice_p -> getVisibility ());

	store (dabSettings_p, DAB_GENERAL, DEVICE_WIDGET_VISIBLE,
	                      inputDevice_p -> getVisibility () ? 1 : 0);
}
//
//	called from the configHandler
void	RadioInterface::handle_dlTextButton	() {
	if (dlTextFile != nullptr) {
	   fclose (dlTextFile);
	   dlTextFile = nullptr;
	   configHandler_p	-> mark_dlTextButton (false);
	   return;
	}

	QString	fileName =theFilenameFinder. finddlText_fileName (true);
	dlTextFile	= fopen (fileName. toUtf8 (). data (), "w+");
	if (dlTextFile	== nullptr)
	   return;
	configHandler_p	-> mark_dlTextButton (true);
}
//
//	called from the config handler
void	RadioInterface::handle_resetButton	() {
	if (!running. load())
	   return;
	QString	channelName	= channel. channelName;
	stopScanning ();
	stopChannel ();
	startChannel	(channelName);
}
//
//	called from the config handler
void	RadioInterface::handle_snrButton	() {
	if (!running. load ())
	   return;

	if (theSNRViewer. isHidden ())
	   theSNRViewer. show ();
	else
	   theSNRViewer. hide ();
	store (dabSettings_p, DAB_GENERAL, SNR_WIDGET_VISIBLE,
	                          theSNRViewer. isHidden () ? 0 : 1);
}
//
//	called from the configHandler
void	RadioInterface::handle_set_coordinatesButton	() {
coordinates theCoordinator (dabSettings_p);
	(void)theCoordinator. QDialog::exec();
	localPos. latitude		=
	             value_f (dabSettings_p, MAP_HANDLING, HOME_LATITUDE, 0);
	localPos. longitude		=
	             value_f (dabSettings_p, MAP_HANDLING, HOME_LONGITUDE, 0);
}
//
//	called from the configHandler
void	RadioInterface::handle_loadTable	 () {
dbLoader theLoader (dabSettings_p);
	if (theLoader. load_db ()) {
	   QMessageBox::information (this, tr ("success"),
	                            tr ("Loading and installing database complete\n"));
	   theTIIProcessor. reload ();
	}
	else {
	   QMessageBox::information (this, tr ("fail"),
	                            tr ("Loading database failed\n"));
	}
}

void	RadioInterface::stop_sourcedumping	() {
	if (!sourceDumping)
	   return;
	theLogger. log (logger::LOG_SOURCEDUMP_STOPS);
	theOFDMHandler	-> stop_dumping();
	sourceDumping	= false;
	configHandler_p	-> mark_dumpButton (false);
}

void	RadioInterface::start_sourcedumping () {
QString deviceName	= inputDevice_p -> deviceName ();
QString channelName	= channel. channelName;
	if (theSCANHandler. active ())
	   return;

	QString rawDumpName	=
	         theFilenameFinder. findRawDump_fileName (deviceName, channelName);
	if (rawDumpName == "")
	   return;

	theLogger. log (logger::LOG_SOURCEDUMP_STARTS,
	                                     deviceName, channelName);
	configHandler_p	-> mark_dumpButton (true);
	theOFDMHandler -> start_dumping (rawDumpName, channel. tunedFrequency);
	sourceDumping = true;
}

void	RadioInterface::handle_sourcedumpButton () {
	if (!running. load () || theSCANHandler. active ())
	   return;
	if (sourceDumping)
	   stop_sourcedumping ();
	else
	   start_sourcedumping ();
}

void	RadioInterface::handle_LoggerButton (int s) {
	(void)s;
	if (configHandler_p -> logger_active ()) {
	   theLogger. logging_starts ();
	   store (dabSettings_p, DAB_GENERAL, LOG_MODE, 1);
	}
	else {
	   theLogger. logging_stops ();
	   store (dabSettings_p, DAB_GENERAL, LOG_MODE, 0);
	}
}

void	RadioInterface::set_tii_detectorMode (bool isChecked) {
	if (theOFDMHandler != nullptr) 
	   theOFDMHandler	-> set_tiiDetectorMode (isChecked);
}

void	RadioInterface::handle_dcRemovalSelector (bool b) {
	if (theOFDMHandler != nullptr)
	   theOFDMHandler	-> set_dcRemoval  (b);
	theNewDisplay. set_dcRemoval (b);
	store (dabSettings_p, CONFIG_HANDLER, "dcRemoval", b ? 1 : 0);
}

void	RadioInterface::set_transmitters_local  (bool isChecked) {
	maxDistance = -1;
	channel. targetPos	= position {0, 0};
	if ((isChecked) && (mapHandler != nullptr))
	   mapHandler -> putData (MAP_RESET, channel. targetPos);
}

void	RadioInterface::selectDecoder (int decoder) {
	theOFDMHandler	-> handle_decoderSelector (decoder);
}

void	RadioInterface:: set_streamSelector (int k) {
	if (!running. load ())
	   return;
	((audioSink *)(soundOut_p)) -> selectDevice (k);
	QString str = configHandler_p -> currentStream ();
	store (dabSettings_p, SOUND_HANDLING, AUDIO_STREAM_NAME, str);
}
//
//////////////////////////////////////////////////////////////////////////

void	RadioInterface::nrServices	(int n) {
	channel. serviceCount = n;
}

bool	RadioInterface::autoStart_http () {
	if (localPos. latitude == 0) 
	   return false;
	if (mapHandler != nullptr)  
	   return false;
	QString browserAddress	=
	            value_s (dabSettings_p, MAP_HANDLING, BROWSER_ADDRESS,
	                                    "http://localhost");
	QString mapPort		=
	            value_s (dabSettings_p, MAP_HANDLING, MAP_PORT_SETTING,
	                                             "8080");

	mapHandler = new httpHandler (this,
	                              mapPort,
	                              browserAddress,
	                              localPos,
	                              "",
	                              configHandler_p -> localBrowserSelector_active (), dabSettings_p);
	maxDistance = -1;
	return mapHandler != nullptr;
}


//	ensure that we only get a handler if we have a start location
void	RadioInterface::handle_httpButton	() {
	if (localPos. latitude == 0) {
	   QMessageBox::information (this, tr ("Warning"),
	                         tr ("Function not available, no coordinates were found"));
	   return;
	}

	if (mapHandler == nullptr)  {
	   QString browserAddress	=
	            value_s (dabSettings_p, MAP_HANDLING, BROWSER_ADDRESS,
	                                    "http://localhost");
	   QString mapPort		=
	            value_s (dabSettings_p, MAP_HANDLING, MAP_PORT_SETTING,
	                                             "8080");

	   QString mapFile;
	   if (configHandler_p -> transmitterNames_active ())
	      mapFile = theFilenameFinder. findMaps_fileName ();
	   else
	      mapFile = ""; 

	   mapHandler = new httpHandler (this,
	                                 mapPort,
	                                 browserAddress,
	                                 localPos,
	                                 mapFile,
	                                 configHandler_p -> localBrowserSelector_active (), dabSettings_p);
	   maxDistance = -1;
	   if (mapHandler != nullptr)
	      httpButton -> setText ("http-on");
	}
	else {
	   locker. lock ();
	   delete mapHandler;
	   mapHandler = nullptr;
	   locker. unlock ();
	   httpButton	-> setText ("http");
	}
}

void	RadioInterface::http_terminate	() {
	locker. lock ();
	if (mapHandler != nullptr) {
	   delete mapHandler;
	   mapHandler = nullptr;
	}
	locker. unlock ();
	httpButton -> setText ("http");
}

void	RadioInterface::displaySlide	(const QPixmap &p, const QString &t) {
int w   = 320;
//int h   = 3 * w / 4;
int h   = 200;
	pauzeTimer. stop ();
	pictureLabel	-> setAlignment(Qt::AlignCenter);
	pictureLabel ->
//	       setPixmap (p. scaled (w, h));
	       setPixmap (p. scaled (w, h, Qt::KeepAspectRatio));
	pictureLabel -> setToolTip (t);
	pictureLabel -> show ();
}

void	RadioInterface::show_pauzeSlide () {
QPixmap p;
static int teller	= 0;
QString slideName	= ":res/radio-pictures/pauze-slide-%1.png";
	pauzeTimer. stop ();
//	int nr		= rand () % 11;
	slideName	= slideName. arg (teller);
	if (p. load (slideName, "png")) {
	QString tooltipText;
	   switch (teller) {
	      case 2:
	         tooltipText = "homebrew 60-ies";
	         break;
	      case 3:
	         tooltipText = "homebrew 70-ies";
	         break;
	      case 4:
	         tooltipText = "Flee market in the 60-ies";
	         break;
	      case 5:
	         tooltipText = "The DEC PDP-11/60";
	         break;
	      case 6:
	         tooltipText = "the DEC PDP-1 mainframe";
	         break;
	      case 7:
	         tooltipText = "the DEC PDP-10 mainframe";
	         break;
	      case 8:
	         tooltipText = "the unforgettable DEC PDP-8";
	         break;
	      case 9:
	         tooltipText = "The DEC VAX 11/780";
	         break;
	      default:
	         tooltipText = "";
	   }
	   displaySlide (p, tooltipText);
	}
	  
	pauzeTimer. start (1 * 30 * 1000);
	teller = (teller + 1) % 11;
}
//////////////////////////////////////////////////////////////////////////
//	Experimental: handling eti
//	writing an eti file and scanning seems incompatible to me, so
//	that is why I use the button, originally named "scanButton"
//	for eti when eti is prepared.
//	Preparing eti is with a checkbox on the configuration widget
//
/////////////////////////////////////////////////////////////////////////

void	RadioInterface::handle_etiHandler	() {
	if (theOFDMHandler == nullptr)	// should not happen
	   return;

	if (channel. etiActive)
	   stop_etiHandler ();
	else
	   start_etiHandler ();
}

void	RadioInterface::stop_etiHandler () {
	if (!channel. etiActive) 
	   return;

	theLogger. log (logger::LOG_ETI_STOPS);
	theOFDMHandler -> stop_etiGenerator ();
	channel. etiActive = false;
	scanButton	-> setText ("eti");
}

void	RadioInterface::start_etiHandler () {
	if (channel. etiActive)
	   return;

	QString etiFile		=  theFilenameFinder.
	                                find_eti_fileName (channel. ensembleName, channel. channelName);
	if (etiFile == QString (""))
	   return;

	theLogger. log (logger::LOG_ETI_STARTS, 
	                     inputDevice_p -> deviceName (),
	                     channel. channelName);
	channel. etiActive = theOFDMHandler -> start_etiGenerator (etiFile);
	if (channel. etiActive) 
	   scanButton -> setText ("eti runs");
}

void	RadioInterface::handle_eti_activeSelector (int k) {
bool setting	= configHandler_p -> eti_active ();
	(void)k;
	if (inputDevice_p == nullptr)
	   return;

	if (setting) {
	   stopScanning ();	
	   disconnect (scanButton, &QPushButton::clicked,
	               this, &RadioInterface::handle_scanButton);
	   connect (scanButton, &QPushButton::clicked,
	            this, &RadioInterface::handle_etiHandler);
	   scanButton	-> setText ("eti");
	   if (!inputDevice_p -> isFileInput ())// restore the button' visibility
	      scanButton -> show ();
	   return;
	}
//	otherwise, disconnect the eti handling and reconnect scan
//	be careful, an ETI session may be going on
	stop_etiHandler ();		// just in case
	disconnect (scanButton, &QPushButton::clicked,
	            this, &RadioInterface::handle_etiHandler);
	connect (scanButton, &QPushButton::clicked,
	         this, &RadioInterface::handle_scanButton);
	scanButton      -> setText ("scan");
	if (inputDevice_p -> isFileInput ())	// hide the button now
	   scanButton -> hide ();
}

//
//	access functions to the display widget
void	RadioInterface::show_spectrum            (int amount) {
std::vector<Complex> inBuffer (2048);
	(void)amount;
	if (theSpectrumBuffer. GetRingBufferReadAvailable () < 2048)
	   return;
	theSpectrumBuffer. getDataFromBuffer (inBuffer. data (), 2048);
	theSpectrumBuffer. FlushRingBuffer ();
	if (!theNewDisplay. isHidden () &&
	           (theNewDisplay. get_tab () == SHOW_SPECTRUM))
	   theNewDisplay. show_spectrum (inBuffer, channel. tunedFrequency);
}

void	RadioInterface::show_tii_spectrum	() {
std::vector<Complex> inBuffer (2048);

	if (theTIIBuffer. GetRingBufferReadAvailable () < 2048)
	   return;
	theTIIBuffer. getDataFromBuffer (inBuffer. data (), 2048);
	theTIIBuffer. FlushRingBuffer ();
	if (!theNewDisplay. isHidden () &&
	           (theNewDisplay. get_tab () == SHOW_TII))
	   theNewDisplay. show_tii (inBuffer, channel. tunedFrequency);
}

void	RadioInterface::show_correlation	(int s, int g, QVector<int> r) {
std::vector<float> inBuffer (s);

	(void)g;
	theResponseBuffer. getDataFromBuffer (inBuffer. data (), s);
	theResponseBuffer. FlushRingBuffer ();
	if (!theNewDisplay. isHidden ()) {
	   if (theNewDisplay. get_tab () == SHOW_CORRELATION)
	      theNewDisplay. show_correlation (inBuffer, g, r, channel. distance);
	}
}

void	RadioInterface::show_null		(int amount) {
Complex	*inBuffer  = (Complex *)(alloca (amount * sizeof (Complex)));
	theNULLBuffer. getDataFromBuffer (inBuffer, amount);
	if (!theNewDisplay. isHidden ())
	   if (theNewDisplay. get_tab () ==  SHOW_NULL)
	      theNewDisplay. show_null (inBuffer, amount);
}

void	RadioInterface::show_tii	(int tiiValue, int index) {
QString	country	= "";
int	mainId	= tiiValue >> 8;
cacheElement	theTransmitter;
bool listChanged = false;

	bool dxMode	= configHandler_p -> get_dxSelector ();

	if (!dxMode) {
	   channel. transmitters. resize (0);
	   if (!theDXDisplay. isHidden ())
	      theDXDisplay. hide ();
	}

	if (!running. load () ||(mainId == 0xFF))	// shouldn't be
	   return;

	if (theOFDMHandler -> get_ecc () == 0) 
	   return;

	if (!channel. has_ecc) {
	   channel. ecc_byte	= theOFDMHandler -> get_ecc ();
	   country		= find_ITU_code (channel. ecc_byte,
	                                         (channel. Eid >> 12) &0xF);
	   channel. has_ecc	= true;
	   channel. countryName	= country;
	   channel. transmitterName = "";
	   transmitter_country	-> setText (country);
	}

	if ((localPos. latitude == 0) || (localPos. longitude == 0)) 
	   return;

	if (!theTIIProcessor. has_tiiFile ())
	   return;

//	OK, here we really start
//	first we check whether the item is already in the list, however
//	if it is but was not found in the database and now it is,
//	replace the item
	bool inList	= false;
	cacheElement * tr =
	      theTIIProcessor. get_transmitter (channel. realChannel?
	                                         channel. channelName :
	                                         "any",
	                                     channel. Eid,
	                                     tiiValue >> 8, tiiValue & 0xFF);
	for (uint16_t i = 0; i < channel. transmitters. size (); i ++) {
	   cacheElement t2 =
	           channel. transmitters. at (i). theTransmitter;
	   if ((channel. transmitters. at (i). tiiValue == tiiValue)) {
	      if (!t2. valid && tr -> valid) {
	         channel. transmitters. erase (channel. transmitters. begin () + i);
	         i -= 1;
//	         fprintf (stderr, "removing an element\n");
	      }
	      else {
	         inList = true;
	         break;
	      }
	   }
	}
//
//	If the item is not yet in the list add it and - in dxMode
//	add the data to the log
	if (!inList) {
	   theTransmitter = *tr;
	   theTransmitter. mainId	= tiiValue >> 8;
	   theTransmitter. subId	= tiiValue & 0xFF;
	   theTransmitter. channel	= channel. channelName;
	   transmitterDesc t = {tiiValue, false, false, theTransmitter, 0, 0};
	   channel. transmitters. push_back (t);	
	   if (dxMode) {
	      if (theDXDisplay. isHidden ())
	         theDXDisplay. show ();
	      FILE *tiiFile	=
	                 fopen (QString (path_for_tiiFile + "tii-files.txt" ).
	                                    toStdString (). c_str (), "a");
	      if (tiiFile != nullptr) {
	         QDateTime theTime;
	         QString tod = theTime. currentDateTime (). toString ();
	         fprintf (tiiFile , "%s -> %s %s %d %d %s\n",
	                            tod. toLatin1 (). data (),
	                            channel.  countryName. toLatin1 (). data (),
	                            channel. channelName. toLatin1 (). data (),
	                            tiiValue >> 8, tiiValue & 0xFF,
	                            theTransmitter. transmitterName. toLatin1 (). data ());
	         fclose (tiiFile);
	      }
	   }
	   listChanged	= true;
	}
//
//	Check which one is "strongest", 
	if (index == 0) {
	   int currentMax = -1;
	   for (uint16_t i = 0; i < channel. transmitters. size (); i ++) {
	      if (channel.transmitters. at (i). isStrongest)
	         currentMax = i;
	      channel. transmitters. at (i). isStrongest = false;
	   }
	   for (uint16_t i = 0; i < channel. transmitters. size (); i ++) {
	      if (channel. transmitters. at (i). tiiValue == tiiValue) {
	         channel. transmitters. at (i). isStrongest = true;
	         if (i != currentMax)
	            listChanged = true;
	      }
	   }
	}

//	display the transmitters on the scope widget
	if (!(theNewDisplay. isHidden () &&
	               (theNewDisplay. get_tab () == SHOW_TII))) 
	   theNewDisplay. show_transmitters (channel. transmitters);
//
//	and recompute distances etc etc
	if (!dxMode) {
	   QFont f ("Arial", 9);
	   distanceLabel ->  setFont (f);
	}
	else {
	   if (listChanged)	// adapt the list and display
	      theDXDisplay. cleanUp ();
	   theDXDisplay. show ();
	   theDXDisplay. setChannel (channel. channelName);
	}

	if (!listChanged && dxMode)
	   return;

	int	maxTrans	= -1;
//	The list was changed, so we rewrite the dxDisplay and 
//	the map
	QString labelText;
	for (auto &theTr : channel. transmitters) {
	   if (theTr. theTransmitter. valid) {
	      position thePosition;
	      thePosition. latitude	= theTr. theTransmitter. latitude;
	      thePosition. longitude	= theTr. theTransmitter. longitude;

	      if (theTr. theTransmitter. valid && theTr. isStrongest) {
	         channel. targetPos. latitude	= thePosition. latitude;
	         channel. targetPos. longitude	= thePosition. longitude;
	         channel. transmitterName		=
	                 theTr. theTransmitter. transmitterName;
	         channel. mainId		= theTr. tiiValue >> 8;
	         channel. subId			= theTr. tiiValue & 0xFF;
	      }

	      theTr. isFurthest	= false;
	
	      QString theName 	= theTr. theTransmitter. transmitterName;
	      float power	= theTr. theTransmitter. power;
	      float height	= theTr. theTransmitter. height;

//      if positions are known, we can compute distance and corner
	      theTr.  distance	= distance	(thePosition, localPos);
	      theTr.  corner	= corner	(thePosition, localPos);

	      if (theTr. isStrongest) {
	         channel. distance	= theTr. distance;
	         channel. corner	= theTr. corner;
	         channel. height	= height;
	      }

	      if (theTr. distance > maxDistance) {
	         maxTrans	= theTr. tiiValue;
	         maxDistance = theTr. distance;
	      }
	      int tiiValue_local	= theTr. tiiValue;
	      labelText = "(" + QString::number (tiiValue_local >> 8) + ","
	                       + QString::number (tiiValue_local & 0xFF) + ") ";
	      labelText += theName;
	      QString text2	= "";
	      if (theTr. theTransmitter. valid)
	             text2  =  " "
	                    + QString::number (theTr. distance, 'f', 1) + " km " 
	                    + QString::number (theTr. corner, 'f', 1) 
	                    + QString::fromLatin1 (" \xb0 ") 
	                    + " (" + QString::number (height, 'f', 1) +  "m)"
	                    + "  " + QString::number (power, 'f', 1) + "kW";
	   

	      if (dxMode) 
	         theDXDisplay. addRow (labelText, text2, 
	                                     theTr. isStrongest, theTr. corner);
	      else 
	         distanceLabel	-> setText (labelText + text2);
	   }
	   else {
	      int tiiValue_local	= theTr. tiiValue;
	      labelText = "(" + QString::number (tiiValue_local >> 8) + ","
	                       + QString::number (tiiValue_local & 0xFF) + ") ";
	      labelText += "not in database";
	      if (dxMode) 
	         theDXDisplay. addRow (labelText, "",  theTr. isStrongest, 0);
	      else 
	         distanceLabel	-> setText (labelText);
	   }
	}
//	now handling the map
//	see if we have a map
	if ((mapHandler == nullptr) || !listChanged) 
	      return;

	for (auto &theTr : channel. transmitters) {
	   if (!theTr. theTransmitter. valid)
	      continue;

	   uint8_t key	= MAP_NORM_TRANS;	// default value
	   bool localTransmitters	=
	            configHandler_p -> localTransmitterSelector_active ();
	   if ((!localTransmitters) && (theTr. tiiValue  == maxTrans)) { 
	      key = MAP_MAX_TRANS;
	   }
//
	   QDateTime theTime = 
	   configHandler_p -> utcSelector_active () ?
	                              QDateTime::currentDateTimeUtc () :
	                              QDateTime::currentDateTime ();
	   position thePosition; 
	   thePosition. latitude        = theTr. theTransmitter. latitude;
	   thePosition. longitude       = theTr. theTransmitter. longitude;
 

	   mapHandler -> putData (key,
	                          thePosition, 
	                          theTr. theTransmitter. transmitterName,
	                          channel. channelName,
	                          theTime. toString (Qt::TextDate),
	                          theTr. tiiValue,
	                          channel. snr,
	                          theTr. distance,
	                          theTr. corner,
	                          theTr. theTransmitter. power,
	                          theTr. theTransmitter. height);
	}
}

void	RadioInterface::showIQ			(int amount) {
std::vector<Complex> Values (amount);
	theIQBuffer. getDataFromBuffer (Values. data (), amount);
	theIQBuffer. FlushRingBuffer ();
	if (!theNewDisplay. isHidden ())
	   theNewDisplay. showIQ (Values);
}

void	RadioInterface::show_Corrector (int h, float l) {
	if (!theNewDisplay. isHidden ())
	   theNewDisplay. show_corrector (h, l);
}

void	RadioInterface::show_stdDev	(int amount) {
std::vector<float>Values (amount);
	stdDevBuffer. getDataFromBuffer (Values. data (), amount);
	if (!theNewDisplay. isHidden ())
	   theNewDisplay. show_stdDev (Values);
}

void	RadioInterface::show_snr		(float snr) {
QPixmap p;

	if (!theNewDisplay. isHidden ())
	   theNewDisplay. show_snr (snr);
	channel. snr	= snr;

	if (snr < 5) 
	   p = strengthLabels. at (0);
	else
	if (snr < 8)
	   p = strengthLabels. at (1);
	else
	if (snr < 12)
	   p = strengthLabels. at (2);
	else
	   p = strengthLabels. at (3);
	this -> snrLabel -> setAlignment(Qt::AlignRight);
	this -> snrLabel -> setPixmap (p. scaled (30, 30, Qt::KeepAspectRatio));

	if (theSNRViewer. isHidden ()) {
	   theSNRBuffer. FlushRingBuffer ();
	   return;
	}

	int amount = theSNRBuffer. GetRingBufferReadAvailable ();
	if (amount <= 0)
	   return;
	float *ss  = (float *) alloca (amount * sizeof (float));
	theSNRBuffer. getDataFromBuffer (ss, amount);
	for (int i = 0; i < amount; i ++) {
	   theSNRViewer. add_snr (ss [i]);
	}
	theSNRViewer. show_snr ();
}

void	RadioInterface::show_quality	(float q, 
	                                 float sco, float freqOffset) {
	if (!running. load () || theNewDisplay. isHidden ())
	   return;
	theNewDisplay. show_quality (q, sco, freqOffset); 
}
//
//	called from the MP4 decoder
void	RadioInterface::show_rsCorrections	(int c, int ec) {
	if (!running)
	   return;
	if (!techWindow_p -> isHidden ())
	   techWindow_p -> show_rsCorrections (c, ec);
}
//
//	called from the ofdm handler
void	RadioInterface::show_clock_error	(int d) {
	if (!running. load ())
	   return;
	if (!theNewDisplay. isHidden ()) {
	   theNewDisplay. show_clock_err (d);
	}
}

void	RadioInterface::show_channel	(int n) {
std::vector<Complex> v (n);
	theChannelBuffer. getDataFromBuffer (v. data (), n);
	theChannelBuffer. FlushRingBuffer ();
	if (!theNewDisplay. isHidden () &&
	           (theNewDisplay. get_tab () == SHOW_CHANNEL))
	   theNewDisplay. show_channel (v);
}

bool	RadioInterface::channelOn () {
	return (!theNewDisplay. isHidden () &&
	           (theNewDisplay. get_tab () == SHOW_CHANNEL));
}

bool	RadioInterface::devScopeOn () {
	return !theNewDisplay. isHidden () &&
	           (theNewDisplay. get_tab () == SHOW_STDDEV);
}

void	RadioInterface::handle_iqSelector () {
	if (theOFDMHandler != nullptr)
	   theOFDMHandler -> handle_iqSelector ();
}

void	RadioInterface::showPeakLevel (float iPeakLeft, float iPeakRight) {
	techWindow_p	-> showPeakLevel (iPeakLeft, iPeakRight);
}

void    RadioInterface::handle_presetButton     () {    
	int mode = the_ensembleHandler ->  get_showMode ();
	if (inputDevice_p -> isFileInput ()) {
	   mode = SHOW_ENSEMBLE;
	   presetButton -> setText ("favorites");
	   the_ensembleHandler -> set_showMode (mode);
	   return;
	}
	mode = mode == SHOW_ENSEMBLE ? SHOW_PRESETS : SHOW_ENSEMBLE;
	the_ensembleHandler -> set_showMode (mode);
	presetButton -> setText (mode == SHOW_ENSEMBLE ? "favorites" : "ensemble");
}     

void	RadioInterface::set_soundLabel  (bool f) {
QPixmap p;
	if (f) 
	    p. load (":res/radio-pictures/volume_on.png", "png");
	else
	    p. load (":res/radio-pictures/volume_off.png", "png");
	soundLabel	-> setAlignment(Qt::AlignRight);
	soundLabel -> 
	       setPixmap (p. scaled (30, 30, Qt::KeepAspectRatio));
}
	
void	RadioInterface::handle_aboutLabel   () { 
	if (the_aboutLabel == nullptr) {
	   the_aboutLabel = new AboutDialog (nullptr);
	   the_aboutLabel -> show ();
	   return;
	}
	delete the_aboutLabel;
	the_aboutLabel	= nullptr;
}

//
//	Starting a background task is by clicking with the right mouse button
//	on the servicename, swrvice is known to be in current ensemble
void	RadioInterface::start_background_task (const QString &service) {
audiodata ad;

	theOFDMHandler -> data_for_audioservice (service, ad);
	if ((!ad. defined) || (ad. ASCTy != 077))
	   return;

	for (uint16_t i = 0;
	     i < channel. backgroundServices. size (); i ++) {
	   if (channel. backgroundServices. at (i). serviceName ==
	                                                      service) {
	      theOFDMHandler -> stop_service (ad. subchId, BACK_GROUND);
	      if (channel. backgroundServices. at (i). fd != nullptr)
	         fclose (channel. backgroundServices. at (i). fd);
	      channel. backgroundServices. erase
	                        (channel. backgroundServices. begin () + i);
	      return;
	   }
	}

	FILE *f = theFilenameFinder. findFrameDump_fileName (service, true);
	if (f == nullptr)
	   return;

//	fprintf (stderr, "starting a background job %s\n",
//	                             ad. serviceName. toLatin1 (). data ());
	(void)theOFDMHandler ->
	                   set_audioChannel (ad, &theAudioBuffer, f, BACK_GROUND);
	dabService s;
	s. channel	= ad. channel;
	s. serviceName	= ad. serviceName;
	s. SId		= ad. SId;
	s. SCIds	= ad. SCIds;
	s. subChId	= ad. subchId;
	s. fd		= f;
	channel. backgroundServices. push_back (s);
}

void	RadioInterface::handle_labelColor () {
QColor	labelColor;
	QColor color	= QColorDialog::getColor (labelColor,
	                                          nullptr, "labelColor");
	if (!color. isValid ())
	   return;
	labelStyle		= "color:" + color. name ();
	store (dabSettings_p, COLOR_SETTINGS, LABEL_COLOR, labelStyle);
	QFont font	= serviceLabel -> font ();
	font. setPointSize (16);
	font. setBold (true);
	serviceLabel	-> setStyleSheet (labelStyle);
	serviceLabel	-> setFont (font);
	programTypeLabel	-> setStyleSheet (labelStyle);
}

void	RadioInterface::show_dcOffset (float dcOffset) {
	theNewDisplay. show_dcOffset (dcOffset);
}

void	RadioInterface::handle_techFrame_closed () {
	store (dabSettings_p, DAB_GENERAL, TECHDATA_VISIBLE, 0);
}

void	RadioInterface::handle_configFrame_closed () {
	store (dabSettings_p, DAB_GENERAL, CONFIG_WIDGET_VISIBLE, 0);
}

void	RadioInterface::handle_deviceFrame_closed () {
	store (dabSettings_p, "Heneral", DEVICE_WIDGET_VISIBLE, 0);
}

void	RadioInterface::handle_newDisplayFrame_closed () {
	store (dabSettings_p, DAB_GENERAL, NEW_DISPLAY_VISIBLE, 0);
}

void	RadioInterface::setVolume	(int n) {
	if (n == 0) 
	   set_soundLabel (false);
	else
	if (channel. audioActive)
	   set_soundLabel (true);
	((Qt_Audio *)soundOut_p) -> setVolume (n);
}

void	RadioInterface::handle_snrLabel	() {
	if (running. load()) {
	   dynamicLabel	-> setStyleSheet (labelStyle);
	   QString SNR	= "SNR " + QString::number (channel. snr);
	   dynamicLabel	-> setText (SNR);
	}
}

void	RadioInterface::handle_correlationSelector	(int d) {
	(void)d;
	bool b =  configHandler_p -> get_correlationSelector ();
	store (dabSettings_p, DAB_GENERAL, S_CORRELATION_ORDER, b ? 1 : 0);
	if (theOFDMHandler != nullptr)
	   theOFDMHandler -> set_correlationOrder (b);
}

void	RadioInterface::handle_dxSelector		(int d) {
	(void)d;
	bool b = configHandler_p -> get_dxSelector ();
	store (dabSettings_p, CONFIG_HANDLER, S_DX_MODE, b ? 1 : 0);
	if (!b)
	   theDXDisplay. hide ();
	if (theOFDMHandler != nullptr)
	   theOFDMHandler -> set_dxMode (b);
	if (b)
	   distanceLabel -> setText ("");
}

void	RadioInterface::channelSignal (const QString &channel) {
	stopChannel ();
	startChannel (channel);
}

void	RadioInterface::show_changeLabel (const QStringList notInOld,
	                                  const QStringList notInNew) {
	theLogger. log (logger::LOG_CONFIG_CHANGE);
	if (notInOld. size () > 0) {
	   fprintf (stderr, "New service:\n");
	   for (auto s: notInOld) 
	      fprintf (stderr, "\t%s\n", s. toUtf8 (). data ());
	}
	if (notInNew. size () > 0) {
	   fprintf (stderr, "removed service:\n");
	   for (auto s: notInNew)
	      fprintf (stderr, "\t%s\n", s. toUtf8 (). data ());
	}
}

void	RadioInterface::handle_folderButton	() {
	QString tempPath	= theFilenameFinder. basicPath ();
#ifdef __MINGW32__
	LPCWSTR temp = (const wchar_t *)tempPath. utf16 ();
	ShellExecute (nullptr, L"open", temp,
	                                   nullptr, nullptr, SW_SHOWDEFAULT);
#else
	std::string x = "xdg-open " + tempPath. toStdString ();
	fprintf (stderr, "we gaan voor %s\n", x. c_str ());
	(void)system (x. c_str ());
#endif
}

