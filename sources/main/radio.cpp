#
/*
 *    Copyright (C)  2015 .. 2025
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
#include	<QToolTip>
#include	<QKeyEvent>
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
#include	"db-element.h"
#include	"distances.h"
#include	"position-handler.h"
#include	"settings-handler.h"
#ifdef	TCP_STREAMER
#include	"tcp-streamer.h"
#else
#include	"Qt-audio.h"
#include	"audiosink.h"
#endif

#include	"dab-tables.h"
#include	"device-exceptions.h"
#include	"settingNames.h"
#include	"uploader.h"

#include	<QScreen>
#include	<QDomElement>

#include	"dl2-handler.h"
#include	"basic-print.h"

#include	<filesystem>
#if defined (__MINGW32__) || defined (_WIN32)
#include <windows.h>
__int64 FileTimeToInt64 (FILETIME & ft) {
	ULARGE_INTEGER foo;

	foo.LowPart	= ft.dwLowDateTime;
	foo.HighPart	= ft.dwHighDateTime;
	return (foo.QuadPart);
}

bool getCpuTimes (size_t &idle_time, size_t &total_time) {
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
std::vector<size_t> getCpuTimes() {
	std::ifstream proc_stat ("/proc/stat");
	proc_stat. ignore (5, ' ');    // Skip the 'cpu' prefix.
	std::vector<size_t> times;
	for (size_t time; proc_stat >> time; times. push_back (time));
	return times;
}
 
bool getCpuTimes (size_t &idle_time, size_t &total_time) {
	const std::vector <size_t> cpu_times = getCpuTimes();
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
QString idsToString (int mainId, int subId) {
	return  "(" + QString::number (mainId) + "-"
	            + QString::number (subId)  + ")";
}

static inline
QStringList splitter (const QString &s) {
#if QT_VERSION > QT_VERSION_CHECK (5, 15, 2)
	QStringList list = s.split (":", Qt::SkipEmptyParts);
#else
	QStringList list = s.split (":", QString::SkipEmptyParts);
#endif
	return list;
}

static const
char	LABEL_STYLE [] = "color:lightgreen";

	RadioInterface::RadioInterface (QSettings	*Si,
	                                const QString	&scanListFile,
	                                const QString	&presetFile,
	                                const QString	&freqExtension,
	                                const QString	&schedule,
	                                const QString	&tiiFile,
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
	                                        theFilenameFinder (Si),
	                                        theScheduler (this, schedule),
	                                        theTechData (16 * 32768),
	                                        theAudioConverter (this),
	                                        theScanlistHandler (this,
	                                                        scanListFile),
	                                        theErrorLogger (Si),
	                                        theDeviceChoser (&theErrorLogger, Si),
	                                        theDXDisplay (this, Si),
	                                        theLogger	(Si),
	                                        theSCANHandler (this, Si,
	                                                        freqExtension),
	                                        theTIIProcessor (tiiFile),
	                                        myTimeTable (this, Si),
	                                        epgVertaler (&theErrorLogger) {
int16_t k;
QString h;

	dabSettings_p			= Si;
	this	-> error_report		= error_report;
	this	-> fmFrequency		= fmFrequency;
	this	-> dlTextFile		= nullptr;
	this	-> the_aboutLabel	= nullptr;
	running. 		store (false);
	stereoSetting			= false;
	contentTable_p			= nullptr;
	scanTable_p			= nullptr;
	mapHandler			= nullptr;
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
	          value_i (dabSettings_p, DAB_GENERAL, "tii_delay", 3);
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
//
//	signal strength icons
	for (int i = 0; i < 4; i ++) {
	   QPixmap p;
	   QString labelName	=
	        QString (":res/radio-pictures/signal%1.png"). arg (i, 1, 10, QChar ('0'));
	   p. load (labelName, "png");
	   strengthLabels. push_back (p);
	}

//	The settings are done, now creation of the GUI parts
	setupUi (this);
//	and init the up and down button, the select for details button ans
//	the button to show the directory with files 
	{  QPixmap p;
	   if (p. load (":res/radio-pictures/up-arrow.png", "png"))
	      prevChannelButton -> setPixmap (p. scaled (30, 30,
	                                                Qt::KeepAspectRatio));
	   if (p. load (":res/radio-pictures/down-arrow.png", "png"))
	      nextChannelButton -> setPixmap (p. scaled (30, 30,
	                                                Qt::KeepAspectRatio));
	   if (p. load (":res/radio-pictures/details24.png", "png"))
	      serviceButton -> setPixmap (p. scaled (30, 30,
	                                                Qt::KeepAspectRatio));
	   else 
	      theErrorLogger. add ("main", "Loading details button failed");
	   if (p. load (":res/radio-pictures/folder_button.png", "png"))
	      folder_shower -> setPixmap (p. scaled (30, 30, Qt::KeepAspectRatio));
	}
	aboutLabel -> setText (" © V6.9.4");
	runtimeDisplay	-> hide ();
//
	connect (folder_shower, &clickablelabel::clicked,
	         this, &RadioInterface::handle_folderButton);
	dxMode     = value_i (dabSettings_p, CONFIG_HANDLER, S_DX_MODE, 0) != 0;
	tiiButton -> setText (dxMode ? "tii local" : "dx display");
	connect (tiiButton, &QPushButton::clicked,
	         this, &RadioInterface::handle_tiiButton);

//	put the widgets in the right place and create the workers
	setPositionAndSize	(dabSettings_p, this, S_MAIN_WIDGET);

	configHandler_p. reset (new configHandler (this, dabSettings_p));
	theEnsembleHandler. reset (new ensembleHandler (this, dabSettings_p,
	                                                       presetFile));
//	we have the configuration handler and the ensemble handler,
//	connect some signals directly
	configHandler_p		-> set_activeServices (0);
	configHandler_p		-> set_connections ();
	configHandler_p		-> setDeviceList (theDeviceChoser.
	                                            getDeviceList ());
	connect (configHandler_p. data (), &configHandler::frameClosed,
	         this, &RadioInterface::handle_configFrame_closed);
	connect (configHandler_p. data (), &configHandler::handle_fontSelect,
	         theEnsembleHandler. data (),
	                               &ensembleHandler::handleFontSelect);
	connect (configHandler_p. data (),
	                         &configHandler::handle_fontSizeSelect,
	         theEnsembleHandler. data (),
	                         &ensembleHandler::handleFontSizeSelect);
	connect (configHandler_p. data (),
	                        &configHandler::handle_fontColorSelect,
	         theEnsembleHandler. data (),
	                        &ensembleHandler::handleFontColorSelect);
	connect (configHandler_p. data (), &configHandler::set_serviceOrder,
	         theEnsembleHandler. data (),
	                               &ensembleHandler::setServiceOrder);
	connect (&theNewDisplay, &displayWidget::frameClosed,
	         this, &RadioInterface::handle_newDisplayFrame_closed);

#ifdef HAVE_RTLSDR_V3
	SystemVersion	= QString ("9.4") + " with RTLSDR-V3";
#elif HAVE_RTLSDR_V4
	SystemVersion	= QString ("9.4") + " with RTLSDR-V4";
#else
	SystemVersion	= QString ("9.4");
#endif
#if QT_VERSION > QT_VERSION_CHECK (6, 0, 0)
	version		= "Qt6-DAB-6." + SystemVersion ;
#else
	version		= "Qt5-DAB-6." + SystemVersion;
#endif
	setWindowTitle (version);

	ensembleWidget -> setWidget (theEnsembleHandler. data ());
	connect (theEnsembleHandler. data (), &ensembleHandler::selectService,
	         this, &RadioInterface::localSelect);
	connect (theEnsembleHandler. data (),
	               &ensembleHandler::start_background_task,
	         this, &RadioInterface::handle_backgroundTask);
	   
	techWindow_p. reset (new techData (this, dabSettings_p, &theTechData));
	
	connect (techWindow_p. data (), &techData::frameClosed,
	         this, &RadioInterface::handle_techFrame_closed);

	if (value_i (dabSettings_p, DAB_GENERAL, NEW_DISPLAY_VISIBLE, 0) != 0)
	   theNewDisplay. show ();
	else
	   theNewDisplay. hide ();

	peakLeftDamped	= 0;
	peakRightDamped = 0;
	audioTeller	= 0;	// counting audio frames
	pauzeSlideTeller	= 0; // counting pause slides
	labelStyle	= value_s (dabSettings_p, DAB_GENERAL, LABEL_COLOR,
	                                                     LABEL_STYLE);
	QFont font	= serviceLabel -> font ();
	font. setPointSize (16);
	font. setBold (true);
	serviceLabel	-> setStyleSheet (labelStyle);
	serviceLabel	-> setFont (font);
	serviceLabel	-> setToolTip ("<font color=\"black\">the label displays the selected service. If a logo can be found, it will be displayed, otherwise the shortname is displayed");
	motLabel	-> setStyleSheet ("QLabel {color : red}");
	motLabel	-> setToolTip ("<font color=\"black\">the label colors green when MOT data, for the currently selected service, can be decoded");

	programTypeLabel	-> setStyleSheet (labelStyle);
	font      = ensembleId -> font ();
	font. setPointSize (14);
	ensembleId      -> setFont (font);
	channel. cleanChannel ();
	localPos. latitude 		=
	             value_f (dabSettings_p, MAP_HANDLING, HOME_LATITUDE, 0.0);
	localPos. longitude 		=
	             value_f (dabSettings_p, MAP_HANDLING, HOME_LONGITUDE, 0.0);
	if (localPos. latitude == 0)
	   localPos. latitude = 52.22;
	if (localPos. longitude == 0)
	   localPos. longitude = 4.54;

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
	      soundOut_p	= new Qt_Audio (this, dabSettings_p);
	      streams		= ((Qt_Audio *)soundOut_p) -> streams ();
	      temp		=
	          value_s (dabSettings_p, SOUND_HANDLING,
	                                  AUDIO_STREAM_NAME, "default");
	      volumeSlider	-> show ();
	      audioVolume	=
	          value_i (dabSettings_p, SOUND_HANDLING, QT_AUDIO_VOLUME, 50);
	      volumeSlider		-> setValue (audioVolume);
	      ((Qt_Audio *)soundOut_p)	-> setVolume (audioVolume);
	      connect (volumeSlider, &QSlider::valueChanged,
	               this, &RadioInterface::setVolume);
	   } catch (...) {
	      soundOut_p = nullptr;
	   }
	}
//
//	we end up here if selection was PORT_AUDIO or using Qt_Audio failed
//	as it does on U20
	if (soundOut_p == nullptr) {
	   soundOut_p	= new audioSink	(latency);
	   streams	= ((audioSink *)soundOut_p) -> streams ();
	   temp		=
	          value_s (dabSettings_p, SOUND_HANDLING,
	                                 AUDIO_STREAM_NAME, "default");
	}

	if (streams. size () > 0) {
	   configHandler_p -> fill_streamTable (streams);
	   configHandler_p -> show_streamSelector (true);
	   k	= configHandler_p -> init_streamTable (temp);
	   if (k >= 0) {
	      QString str = configHandler_p -> currentStream ();
	      soundOut_p -> selectDevice (k, str);
	   }
	   configHandler_p	-> connect_streamTable	();
	}
	else {
	   delete soundOut_p;
	   soundOut_p = new audioPlayer ();
	}
	if (!soundOut_p -> hasMissed ())
	   techWindow_p -> hideMissed ();
#else
	soundOut_p		= new tcpStreamer	(20040);
	techWindow_p		-> hide		();
#endif
//
//	some MOT, text and other data is stored in the Qt-DAB-files directory
	QString tempPath	= theFilenameFinder. basicPath ();	
	path_for_files		=
	                      value_s (dabSettings_p, DAB_GENERAL,
	                                        S_FILE_PATH, tempPath);
	if (path_for_files != "")
	   path_for_files		= checkDir (path_for_files);

//	timer for autostart epg service
	epgTimer. setSingleShot (true);
	connect (&epgTimer, &QTimer::timeout,
	         this, &RadioInterface::epgTimer_timeOut);
	pauzeTimer. setSingleShot (true);
	connect (&pauzeTimer, &QTimer::timeout,
	         this, &RadioInterface::show_pauzeSlide);
	stressTimer. setSingleShot (true);
	connect (&stressTimer, &QTimer::timeout,
	         this, &RadioInterface::handle_nextServiceButton);
	myTimeTable. hide ();

	connect (&theScanlistHandler, &scanListHandler::handleScanListSelect,
	         this, &RadioInterface::handleScanListSelect);

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
	connect (tiiButton, &smallPushButton::rightClicked,
	         this, &RadioInterface::color_tiiButton);
	connect (prevServiceButton, &smallPushButton::rightClicked,
	         this, &RadioInterface::color_prevServiceButton);
	connect (nextServiceButton, &smallPushButton::rightClicked,
	         this, &RadioInterface::color_nextServiceButton);
	connect (spectrumButton, &smallPushButton::rightClicked,
	         this, &RadioInterface::color_spectrumButton);
//
//	
	connect (techWindow_p. data (), &techData::handleTimeTable,
	         this, &RadioInterface::handle_timeTable);
	connect (&theNewDisplay, &displayWidget::mouseClick,
	         this, &RadioInterface::handle_iqSelector);

	connect (aboutLabel, &clickablelabel::clicked,
	         this, &RadioInterface::handle_aboutLabel);

	connect (soundLabel, &clickablelabel::clicked,
	         this, &RadioInterface::handle_muteButton);
	connect (snrLabel, &clickablelabel::clicked,
	         this, &RadioInterface::handle_snrLabel);

	if (std::filesystem::exists (tiiFile. toLatin1 (). data ())) 
	   configHandler_p -> enable_loadLib ();
	else
	   httpButton	-> setEnabled (false);

	channel. etiActive	= false;
	QPixmap epgP;
	epgP. load (":res/radio-pictures/epgLabel.png", "png");
	epgLabel	-> setPixmap (epgP. scaled (30, 30,
	                                         Qt::KeepAspectRatio));
	epgLabel	-> setToolTip ("this icon is visible when the EPG processor is active,  it will always run in the background");
	epgLabel	-> hide ();
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
	h               =
	           value_s (dabSettings_p, DAB_GENERAL, 
	                                      SELECTED_DEVICE, "no device");
	bool b = configHandler_p -> findDevice (h);
	if (b) {
	   inputDevice_p. reset (createDevice (h, &theLogger));
	}
//
	peakLeftDamped          = -100;
	peakRightDamped         = -100;
 
	leftAudio	-> setFillBrush	(QColor ("white"));
	rightAudio	-> setFillBrush (QColor ("white"));
	leftAudio	-> setBorderWidth	(0);
	rightAudio	-> setBorderWidth	(0);
	leftAudio	-> setValue (-30);
	rightAudio	-> setValue (-30);
	leftAudio	-> setAlarmBrush (Qt::red);
	rightAudio	-> setAlarmBrush (Qt::red);
	leftAudio	-> setAlarmEnabled (true);  
	rightAudio	-> setAlarmEnabled(true);

	journalineKey		= -1;
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
	dynamicLabel    -> setToolTip ("<font color=\"black\">The text (or parts of it) of the dynamic label can be copied. Selecting the text with the mouse and clicking the right hand mouse button shows a small menu with which the text can be put into the clipboard");
//
//	Until the opposite is known to be true:
	nextService. channel = "";
	nextService. serviceName = "";
//	if a device was selected, we just start, otherwise
//	we wait until one is selected
//	nrServicesLabel	-> display (QString::number (0));
	connectGUI ();
//
	this	-> cpuSupport	= 0;
#ifdef	__ARCH_X86__
	__builtin_cpu_init ();
	int has_avg2	=
	          __builtin_cpu_supports ("avx2") != 0 ? AVX_SUPPORT : 0;
	int has_sse4
	        = __builtin_cpu_supports ("sse4.1") != 0 ? SSE_SUPPORT : 0;
	cpuSupport	= has_avg2 + has_sse4;
#endif
//	Just check whether the ini file is used before
	bool iniExists	=
	           value_i (dabSettings_p, DAB_GENERAL, "EXISTS", 0) != 0;
	if (!iniExists) {
	   store (dabSettings_p, DAB_GENERAL, "EXISTS", 1);
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("The ini file is new and no home location is known yet"));
	}

	if (!inputDevice_p .isNull ()) {
	   startDirect ();
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
}
//
//	doStart (QString) is called when - on startup - NO device
//	was registered to be used, and the user presses the
//	selectDevice comboBox
void	RadioInterface::doStart (const QString &dev) {
	inputDevice_p. reset (createDevice	(dev, &theLogger));
//	Some buttons should not be touched before we have a device
	if (inputDevice_p. isNull ()) {
	   return;
	}
	startDirect ();
}
//
//	we (re)start a device, if it happens to be a regular
//	device, check for a preset name
void	RadioInterface::startDirect	() {
	disconnect (channelSelector,
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 2)
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
#ifdef	__ARCH_X86__
	if (cpuSupport & AVX_SUPPORT)
	   cpuLabel -> setText ("avx2");
	else
	if (cpuSupport & SSE_SUPPORT)
	   cpuLabel -> setText ("sse4.1");
	else
	   cpuLabel -> setText ("scalar");
#else
	cpuLabel -> setText ("");
#endif
	theLogger. log (logger::LOG_RADIO_STARTS, inputDevice_p -> deviceName (),
	                                 channelSelector -> currentText ());
	theOFDMHandler. reset (new ofdmHandler  (this,
	                                    inputDevice_p. data (),
	                                    &globals, dabSettings_p,
	                                    &theLogger, this -> cpuSupport));
	if (theOFDMHandler. isNull ()) {
	   QMessageBox::warning (this, tr ("Warning"),
                                       tr ("Fatal error, call expert 11"));
	   abort ();
	}

	theOFDMHandler	-> set_dcRemoval (configHandler_p -> get_dcRemoval ());
	theNewDisplay. set_dcRemoval (configHandler_p -> get_dcRemoval ());
	channel. cleanChannel ();
//
//	Note: this is NOT "theEnsembleHandler. reset ()" !!!!
	theEnsembleHandler	-> reset	();
	theEnsembleHandler	-> setMode (!inputDevice_p -> isFileInput ());

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


	startChannel (channelSelector -> currentText ());
	int auto_http	= value_i (dabSettings_p, CONFIG_HANDLER,
	                                       AUTO_HTTP, 0);
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
static
bool	seems_epg (const QString &name) {
	return  name. contains ("-EPG ", Qt::CaseInsensitive) ||
                name. contains (" EPG   ", Qt::CaseInsensitive) ||
                name. contains ("Spored", Qt::CaseInsensitive) ||
                name. contains ("NivaaEPG", Qt::CaseInsensitive) ||
                name. contains ("SPI", Qt::CaseSensitive) ||
                name. contains ("BBC Guide", Qt::CaseInsensitive) ||
                name. contains ("BBC  Guide", Qt::CaseInsensitive) ||
                name. contains ("EPG_", Qt::CaseInsensitive) ||
                name. contains ("EPG-", Qt::CaseInsensitive) ||
                name. startsWith ("EPG ", Qt::CaseInsensitive);
}
//
//	a slot, called by the fic/fib handlers
void	RadioInterface::addToEnsemble (const QString &serviceName,
	                                           int32_t SId, int  subChId) {
	if (!running. load())
	   return;

	serviceId ed;
	ed. name	= serviceName;
	ed. SId		= SId;
	ed. subChId	= subChId;
	ed. channel	= channel. channelName;

	if (theEnsembleHandler -> alreadyIn (ed))
	   return;
	if (((SId & 0xFFFF0000) != 0) &&
	     (configHandler_p -> get_audioServices_only ())) {
	      if (!seems_epg (serviceName))
	         return;
	}
	bool added	= theEnsembleHandler -> addToEnsemble (ed);
	if (added) {
	   channel. nrServices ++;
	   if (theSCANHandler. active ())
	      theSCANHandler. addService (channel. channelName);
	   if (theSCANHandler. active () && !theSCANHandler. scan_to_data ()) {
	      if ((SId & 0XF0000) == 0)	// only audio
	         theScanlistHandler. addElement (channel. channelName,
	                                              serviceName);
	   }
	}

	if ((channel. serviceCount == channel. nrServices)&& 
	                     !theSCANHandler. active ()) {
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
	   const uint8_t t = (v & 0xF000) >> 12;
	   QChar c = t <= 9 ? (char)('0' + t) : (char) ('A' + t - 10);
	   res. append (c);
	   v <<= 4;
	}
	return res;
}

//	a slot, called by the fib processor
void	RadioInterface::ensembleName (int id, const QString &v) {
QString s;
	if (!running. load())
	   return;

	ensembleId	-> setText (v + QString ("(") + hextoString (id) + QString (")"));

//	transmitter_country	-> setText (channel. countryName);
	channel. ensembleName	= v;
	channel. Eid		= id;
	dynamicLabel		-> setText ("");
//
//	id we are scanning "to data", we reached the end
	if (theSCANHandler. scan_to_data ()) {
	   stopScanning ();
	}
	else
	if (theSCANHandler. active ()) {
	   if (theSCANHandler. scan_single () ||
	       theSCANHandler. scan_continuous ()) {
	      theSCANHandler.
	                 addEnsemble (channelSelector -> currentText (), v);
	      channelTimer. stop ();
	      int switchStay		=
	              configHandler_p -> switchStayValue ();
	      if (theSCANHandler. dumpInFile ()) {
	         inputDevice_p	-> startDump	();
	      }
	      channelTimer. start (switchStay);
	   }
	}
	else
	if (!theSCANHandler. active ()) {
	   read_pictureMappings (id);
//	... and is we are not scanning, clicking the ensembleName
//	has effect
	   connect (ensembleId, &clickablelabel::clicked,
	            this, &RadioInterface::handle_contentButton);
	}
}
//
///////////////////////////////////////////////////////////////////////////

void	RadioInterface::handle_contentButton	() {
basicPrint thePrinter;
QStringList s	= thePrinter. print (theOFDMHandler -> contentPrint ());

	if (contentTable_p != nullptr) {
	   contentTable_p -> hide ();
	   delete contentTable_p;
	   contentTable_p = nullptr;
	   return;
	}
	QString headLine	= build_kop ();
	
	contentTable_p		= new contentTable (this, dabSettings_p,
	                                            channel. channelName,
	                                            thePrinter. scanWidth ());
	connect (contentTable_p, &contentTable::goService,
	         this, &RadioInterface::handle_contentSelector);

	contentTable_p		-> addLine (headLine);
	for (auto &tr : channel. transmitters) {
	   QString transmitterLine = build_transmitterLine (tr);
	   contentTable_p	-> addLine (transmitterLine);
	}
  
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
	      saveMOTtext (result, contentType, objectName);
	      break;

	   case MOTBaseTypeImage: 
	      showMOTlabel (result, contentType,
	                       objectName, dirElement, backgroundFlag);
	      break;

	   case MOTBaseTypeAudio:
	      break;

	   case MOTBaseTypeVideo:
	      break;

	   case MOTBaseTypeTransport:
	      saveMOTObject (result, objectName);
	      break;

	   case MOTBaseTypeSystem:
	      break;

	   case  MOTBaseTypeApplication: { 	// epg data
	      if ((path_for_files == "") || (theSCANHandler. active ()))
	         return;

	      if (objectName == QString (""))
	         objectName = "epg file";
	      std::vector<uint8_t> epgData (result. begin(), result. end());
	      QDomDocument epgDocument;
	      uint8_t docType = epgVertaler. process_epg (epgDocument,
	                                                  epgData, channel.lto);
	      if (docType == noType)		// should not happen
	         break;

	      if (docType == serviceInformationType) {
	         extractServiceInformation (epgDocument, channel. Eid, true);
	         return;
	      }
	   
	      QString theName = extractName (objectName);
	      if (theName == "") 
	         return;
	
	      QString temp = path_for_files +
	                          QString::number (channel. Eid, 16). toUpper () + "/";
	      if (!QDir (temp). exists ())
	         QDir (). mkpath (temp);	
	      theName  = temp + theName;
	      QFile file (QDir::toNativeSeparators (theName));
	      if (file. open (QIODevice::WriteOnly | QIODevice::Text)) { 
	         QTextStream stream (&file);
	         stream << epgDocument. toString ();
	         file. close ();
	      }
	   }
	   return;

	   case MOTBaseTypeProprietary:
	      break;
	}
}
//
//	In the MOT name, we look for a sequence of 8 digits with
//	some constraints, and then for a sequence of 4 characters
//	that match an Sid in this ensemble
QString RadioInterface::extractName (const QString &motName) {
QString dateString;
QString sidString;
int eos		= 0;
bool	ok	= false;
QString env;
QString realName;
int base	= motName. lastIndexOf (QChar ('/'));
	for (int i = 0; i <= base; i ++)
	   env. push_back (motName. at (i));
	for (int i = base + 1; i < motName. size (); i ++)
	   realName . push_back (motName. at (i));
	int dotat	= realName. lastIndexOf (".");
	if (dotat < 0)
	   return "";
	for (int i = 0; i < 4; i ++) {
	   dateString = "";
	   for (int j = 0; j < 4; j ++)
	      dateString . push_back (realName. at (i + j));
	   int y = dateString. toInt (&ok);
	   if ((2000 <= y) && (y <= 2030)) {
	      for (int j = 4; j < 8; j ++)
	         dateString. push_back (realName. at (i + j));
	      eos = i + 8;
	      break;
	   }
	}
	if (eos == 0)
	   return "";
	
	for (int i = eos; i < dotat - 3; i ++) {
	   sidString = "";
	   for (int j = i; j < i + 4; j ++)
	      sidString. push_back (realName. at (j));
	   uint32_t sid = sidString. toInt (&ok, 16);
	   if (ok) {
	      QString name = theEnsembleHandler -> extractName (sid);
	      if (name != "") {
	         return env + dateString + "_" + sidString. toUpper () + "_SI.xml";
	      }
	   }
	}
	return "";
}
	      
void	RadioInterface::saveMOTtext (QByteArray &result,
	                              int contentType,
	                              const QString &name) {
	(void)contentType;
	if (path_for_files == "")
	   return;

	QString textName = QDir::toNativeSeparators (path_for_files + name);

	FILE *x = fopen (textName. toUtf8 (). data (), "w+b");
	if (x == nullptr) {
	   QString t = QString ("problem to open file ") + textName;
	   theErrorLogger. add ("main", t);
	}
	else {
	   (void)fwrite (result. data (), 1, result.length(), x);
	   fclose (x);
	}
}

void	RadioInterface::saveMOTObject (QByteArray  &result,
	                                QString  &name) {
	if (path_for_files == "")
	   return;

	if (name == "") {	// should not happen
	   static int counter	= 0;
	   name = "motObject_" + QString::number (counter);
	   counter ++;
	}
	saveMOTtext (result, 5, name);
}

//	MOT slide, to show
void	RadioInterface::showMOTlabel	(QByteArray  &data,
	                                 int contentType,
	                                 const QString  &pictureName,
	                                 int dirs,
	                                 bool backgroundFlag) {
const char *type;
	if (!running. load () || (pictureName == QString ("")))
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

	if (dirs || ((value_i (dabSettings_p, CONFIG_HANDLER,
	                           SAVE_SLIDES_SETTING, 0) != 0) &&
	                                         (path_for_files != ""))) {
	   QString pict;
	   if (!dirs) 
	      pict = path_for_files + pictureName;
	   else
	      pict = path_for_files + QString::number (channel. Eid, 16). toUpper () + "/" + pictureName;
	   QString temp = pict;
	   temp = temp. left (temp. lastIndexOf (QChar ('/')));
	   if (!QDir (temp). exists())
	      QDir (). mkpath (temp);	
	   pict	= QDir::toNativeSeparators (pict);
	   FILE *x = fopen (pict. toUtf8 (). data (), "w+b");
	   if (x == nullptr) {
	      QString t = QString ("Problem opening picture file (writing) ") +
	                                    pict;
	      theErrorLogger. add ("main", t);
	   }
	   else {
	      theLogger. log (logger::LOG_SLIDE_WRITTEN, pict);
	      (void)fwrite (data. data(), 1, data.length(), x);
	      fclose (x);
	   }
	}

	if (backgroundFlag || dirs)
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
  *	we copy the tasklist and stop all services,
  *	then, using the data in the copy we (try to) start
  *	all services again (including the secondary services);
  */

void	RadioInterface::changeinConfiguration () {
std::vector<dabService> taskCopy = channel. runningTasks;

	if (theSCANHandler. active ()) {
	   stopScanning ();
	   return;
	}
	channelTimer. stop ();
	stopMuting	();
	setSoundLabel (false);
	if (channel. etiActive)
	   theOFDMHandler -> resetEtiGenerator ();
	myTimeTable. clear ();
	myTimeTable. hide ();
//	and stop the service
	for (auto &serv :channel. runningTasks) 
	   stopService (serv);
//	fprintf (stderr, "All services are halted, now start rebuilding\n");
	techWindow_p	-> cleanUp ();
	for (auto &serv : taskCopy) {
	   int index = theOFDMHandler -> getServiceComp (serv. serviceName);
	   if (index < 0)
	      theEnsembleHandler -> remove (serv. serviceName);
	// hier moet de ensemlelist nog worden aangepast
	      continue;
	   if (serv. runsBackground)
	      handle_backgroundTask (serv. serviceName);
	   else
	      startService (serv, index);
	}
}

//
//	In order to not overload with an enormous amount of
//	signals, we trigger this function at most 10 times a second
void	RadioInterface::newAudio	(int amount, int rate,
	                                          bool ps, bool sbr) {
	if (!running. load ())
	   return;

	audioTeller ++;
	if (audioTeller > 10) {
	   audioTeller = 0;
	   if (!techWindow_p -> isHidden ())
	      techWindow_p	-> showRate (rate, ps, sbr);
	   audiorateLabel	-> setStyleSheet ("color:cyan");
	   audiorateLabel	-> setText (QString::number (rate));
	   if (!ps)
	      psLabel -> setText (" ");
	   else {
	      psLabel -> setStyleSheet ("color:cyan"); 
	      psLabel -> setText ("ps");
	   }
	   if (!sbr)
	      sbrLabel -> setText ("  "); 
	   else {
	      sbrLabel -> setStyleSheet ("color:cyan");
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
	   setPeakLevel (tmpBuffer);
	}
}

void	RadioInterface::setPeakLevel (const std::vector<float> &samples) {
float	absPeakLeft	= 0;
float	absPeakRight	= 0;
	
	for (int i = 0; i < (int)(samples. size ()) / 2; i ++) {
	   const float absLeft  = std::abs (samples [2 * i]);
	   const float absRight = std::abs (samples [2 * i + 1]);
	   if (absLeft  > absPeakLeft)  
	      absPeakLeft  = absLeft;
	   if (absRight > absPeakRight)
	      absPeakRight = absRight;
	}

	float leftDb  = (absPeakLeft  > 0.0f ?
	                   20.0f * std::log10 (absPeakLeft)  : -40.0f);
	float rightDb = (absPeakRight > 0.0f ?
	                   20.0f * std::log10 (absPeakRight) : -40.0f);
	showPeakLevel (leftDb, rightDb);
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
	storeWidgetPosition (dabSettings_p, this, S_MAIN_WIDGET);
	stopScanning ();
	while (theSCANHandler. active ())
	   usleep (1000);
	theSCANHandler. hide ();
	myTimeTable. hide ();
	if (scanTable_p != nullptr) {
	   scanTable_p	-> clearTable ();
	   scanTable_p	-> hide ();
	   delete scanTable_p;
	}
	hideButtons	();

//	stop the timers
	displayTimer.	stop	();
	channelTimer.	stop	();
	presetTimer.	stop	();
	epgTimer.	stop	();
	pauzeTimer.	stop	();
	muteTimer.	stop	();
//
//	hide the windows
	theNewDisplay.		hide ();
	theDXDisplay.		hide ();
	theEnsembleHandler	-> hide ();
	if (the_aboutLabel != nullptr) {
	   the_aboutLabel -> hide ();
	   delete the_aboutLabel;
	}
	if (contentTable_p != nullptr) {
	   contentTable_p -> clearTable ();
	   contentTable_p -> hide ();
	   delete contentTable_p;
	}
	configHandler_p. reset ();
	techWindow_p. reset ();
	theSNRViewer.	hide ();
	theScheduler.	hide	();
	theScanlistHandler. hide ();
//
#ifdef	DATA_STREAMER
	fprintf (stderr, "going to close the dataStreamer\n");
	delete		dataStreamer_p;
#endif
#ifdef	CLOCK_STREAMER
	fprintf (stderr, "going to close the clockstreamer\n");
	delete	clockStreamer_p;
#endif
	if (dlTextFile != nullptr)
	   fclose (dlTextFile);
#ifdef	HAVE_PLUTO_RXTX
	if (streamerOut_p != nullptr)
	   streamerOut_p	-> stop ();
#endif
	if (mapHandler != nullptr)
	   mapHandler ->  stop ();
//	just save a few checkbox settings that are not

	stopFrameDumping	();
	stopSourceDumping	();
	stopAudioDumping	();
	if (!theOFDMHandler. isNull ())
	   theOFDMHandler		-> stop ();
	if (soundOut_p != nullptr)
	   delete soundOut_p;
	theLogger. log (logger::LOG_RADIO_STOPS);
	usleep (1000);		// pending signals
//	everything should be halted by now
	dabSettings_p	-> sync ();
	theOFDMHandler. reset ();
	inputDevice_p. reset ();
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
	   getCpuTimes (idle_time, total_time);
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
	if (!techWindow_p -> isHidden () && soundOut_p -> hasMissed ())  {
	   int totalSamples	= 0;
	   int totalMissed	= 0;
	   soundOut_p -> samplesMissed (totalSamples, totalMissed);
	   if (totalSamples != 0) {
	      float correct = (totalSamples - totalMissed) * 100 / totalSamples;
	      techWindow_p -> showMissed (correct);
	   }
	}
}
//
//	precondition: everything is quiet
deviceHandler	*RadioInterface::createDevice (const QString &s,
	                                        logger *theLogger) {
deviceHandler	*inputDevice = theDeviceChoser.
	                               createDevice  (s, version);
	
	(void)theLogger;		// for now
	if (inputDevice	== nullptr)
	   return nullptr;

	channel. realChannel	= !inputDevice -> isFileInput ();
	if (channel. realChannel)
	   showButtons ();
	else 
	   hideButtons ();
	theEnsembleHandler -> setMode (channel. realChannel);
	connect (inputDevice, &deviceHandler::frameClosed,
	         this, &RadioInterface::handle_deviceFrame_closed);
	QString ss = s;
	store (dabSettings_p, DAB_GENERAL, SELECTED_DEVICE, ss);
	inputDevice -> setVisibility (true);
//	if (value_i (dabSettings_p, DAB_GENERAL, DEVICE_WIDGET_VISIBLE, 1) == 0)
//	   inputDevice -> setVisibility (false);
	theNewDisplay. setBitDepth (inputDevice -> bitDepth ());
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

	theLogger. log (logger::LOG_NEWDEVICE, deviceName, 
	                                channelSelector -> currentText ());
	inputDevice_p. reset (createDevice (deviceName, &theLogger));
	if (inputDevice_p. isNull ()) {
	   inputDevice_p. reset (new deviceHandler ());
	   return;		// nothing will happen
	}

	startDirect ();		// will set running
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
	QDate theDate (year, month, day);
	channel. theDate = theDate;

	QFont font	= serviceLabel -> font ();
	font. setPointSize (16);
	font. setBold (true);
	localTimeDisplay	-> setStyleSheet (labelStyle);
        font. setPointSize (14);
	localTimeDisplay	-> setFont (font);
	localTimeDisplay	-> setText (result);
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
	return convertTime (t. year, t. month, t. day, t. hour, t. minute);
}
//
//	called from the MP4 decoder
void	RadioInterface::show_frameErrors (int s) {
	if (!running. load ()) 
	   return;
	if (!techWindow_p -> isHidden ())
	   techWindow_p -> showFrameErrors (s);
}
//
//	called from the MP4 decoder
void	RadioInterface::show_rsErrors (int s) {
	if (!running. load ())		// should not happen
	   return;
	if (!techWindow_p -> isHidden ())
	   techWindow_p	-> showRsErrors (s);
}
//
//	called from the NP4 decoder
void	RadioInterface::show_aacErrors (int s) {
	if (!running. load ())
	   return;
	if (!techWindow_p -> isHidden ())
	   techWindow_p	-> showAacErrors (s);
}
//
//	called from the ficHandler
void	RadioInterface::show_ficQuality (int val, int scaler) {
	if (!running. load ())	
	   return;
	
	QPalette p      = theNewDisplay. ficError_display -> palette();
	if (val * scaler < 85)
	   p. setColor (QPalette::Highlight, Qt::red);
	else
	   p. setColor (QPalette::Highlight, Qt::green);

	theNewDisplay. ficError_display	-> setPalette (p);
	theNewDisplay. ficError_display	-> setValue (val * scaler);
}

void	RadioInterface::show_ficBER	(float ber) {
	if (!running. load ())
	   return;
	if (!theNewDisplay. isHidden ())
	   theNewDisplay. showFICBER (ber);
}
//
//	called from the PAD handler
void	RadioInterface::show_mothandling (bool b) {
static bool old_mot = false;
	
	if (!running. load () || (old_mot == b))
	   return;
	if (b)
	   motLabel	-> setStyleSheet (labelStyle);
	else
	   motLabel	-> setStyleSheet ("QLabel {color : red}");
	old_mot = b;
}
	
//	just switch a color, called from the dabprocessor
void	RadioInterface::set_synced	(bool b) {
	theNewDisplay. setSyncLabel (b);
}
//
//	called from the PAD handler


void	RadioInterface::showLabel	(const QString &s, int charset) {
	(void)charset;
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
	techWindow_p	-> showStereo (b);
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

void	RadioInterface::handleAudiodumpButton () {
	if (!running. load () || theSCANHandler. active ())
	   return;

	if (audioDumping) 
	   stopAudioDumping ();	
	else
	   startAudioDumping ();
}

void	RadioInterface::stopAudioDumping	() {
	if (!audioDumping)
	   return;

	theLogger. log (logger::LOG_AUDIODUMP_STOPS);
	theAudioConverter. stop_audioDump ();
	audioDumping	= false;
	techWindow_p	-> audiodumpButton_text ("audio dump", 10);
}

void	RadioInterface::startAudioDumping () {
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

void	RadioInterface::scheduledAudioDumping () {
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

void	RadioInterface::handleFramedumpButton () {
	if (!running. load () || theSCANHandler. active ())
	   return;

	if (channel. currentService. frameDumper != nullptr) 
	   stopFrameDumping ();
	else
	   startFrameDumping ();
}

void	RadioInterface::stopFrameDumping () {
	if (channel. currentService. frameDumper == nullptr)
	   return;

	theLogger. log (logger::LOG_FRAMEDUMP_STOPS);
	fclose (channel. currentService. frameDumper);
	techWindow_p ->  framedumpButton_text ("save AAC/MP2", 10);
	channel. currentService. frameDumper	= nullptr;
}

void	RadioInterface::startFrameDumping () {
	if (!channel. currentService. isAudio)
	   return;
	channel. currentService. frameDumper	=
	     theFilenameFinder.
	      findFrameDump_fileName (channel. currentService. serviceName,
	                              channel. currentService. ASCTy, true);
	if (channel. currentService. frameDumper == nullptr)
	   return;
	theLogger. log (logger::LOG_FRAMEDUMP_STARTS, 
	                                         channel. channelName,
	                                         channel. currentService. serviceName);
	QString mode = channel. currentService. ASCTy == DAB_PLUS ?
	                                       "recording aac" : "recording mp2";
	techWindow_p ->  framedumpButton_text (mode, 12);
}

void	RadioInterface::scheduled_frameDumping (const QString &s) {
	if (channel. currentService. frameDumper != nullptr) {
	   fclose (channel. currentService. frameDumper);
	   techWindow_p ->  framedumpButton_text ("frame dump", 10);
	   channel. currentService. frameDumper	= nullptr;
	   return;
	}
	audiodata ad;
	int index = theOFDMHandler -> getServiceComp (s);
        if (index < 0)       
           return;
	
	theOFDMHandler -> audioData (index, ad);
	if (!ad. defined)
	   return;
	
	channel. currentService. frameDumper	=
	     theFilenameFinder. findFrameDump_fileName (s, ad. ASCTy, false);
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
	connect (this, &RadioInterface::call_scanButton,
	         this, &RadioInterface::handle_scanButton);
//
////	and for the techWindow
//	connect (techWindow_p. data (), &techData::handleAudioDumping,
//	         this, &RadioInterface::handleAudiodumpButton);
//	connect (techWindow_p. data (), &techData::handleFrameDumping,
//	         this, &RadioInterface::handleFramedumpButton);
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
	                                       tr("Quitting Qt-DAB\nAre you sure?\n"),
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

	if (obj == this -> theEnsembleHandler -> viewport ()) {
	    if (event -> type () == QEvent::MouseButtonPress) {
	      QMouseEvent *ev = static_cast<QMouseEvent *>(event);
	      if (ev -> buttons () & Qt::RightButton) {
	         QTableWidgetItem *x =
	              theEnsembleHandler -> itemAt (ev -> pos ());
	         if (x != nullptr)
	            theEnsembleHandler -> handleRightMouseClick (x -> text ());
	         return true;
	      }
	      else {
	         return QWidget::eventFilter (obj, event);
	      }
	   }
	}
	else 	
	if (event -> type () == QEvent::KeyPress) {
	   QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
	   if (keyEvent -> key () == Qt::Key_Return) {
	      theEnsembleHandler -> selectCurrentItem ();
	      return true;
	   }
	   else		// handling function keys 
	   if (QApplication::keyboardModifiers ().
                               testFlag (Qt::ControlModifier)) {
              return handle_keyEvent (keyEvent -> key ());
	   }
	}
//	An option is to click - right hand mouse button - on a
//	service in the scanlist in order to add it to the
//	list of favorites
	if ((obj == this -> theScanlistHandler. viewport ()) &&
	    (event -> type () == QEvent::MouseButtonPress)) {
	   QMouseEvent *ev = static_cast<QMouseEvent *>(event);
	   if (ev -> buttons () & Qt::RightButton) {
	      QString service =
	           this -> theScanlistHandler. indexAt (ev -> pos()). data ().toString();
	     theEnsembleHandler -> addFavoriteFromScanList (service);
	   }
	}
//	else
//	if (event -> type () == QEvent::MouseButtonPress) {
//	   QPixmap originalPixmap;
//	   QScreen *screen = QGuiApplication::primaryScreen();
//	   originalPixmap = screen -> grabWindow(this -> winId());
//	   QString format = "png";
//	   QString fileName = path_for_files + "main-widget";
//#ifdef	__MINGW32__
//	   fileName = fileName + ".png";
//	   originalPixmap. save (fileName);
//#else
//	   originalPixmap. save (fileName, format.toLatin1 (). data ());
//#endif
//	}
	return QWidget::eventFilter (obj, event);
}

QPixmap RadioInterface::fetchAnnouncement (int id) {
QPixmap p;
QString pictureName	= QString (":res/radio-pictures/announcement%1.png").
	                                      arg (id, 2, 10, QChar ('0'));
	if (!p.load (pictureName, "png"))
	   p. load (":res/radio-pictures/announcement-d.png", "png");
	return p;
}

void	RadioInterface::announcement	(int SId, int flags) {
	if (!running. load ())
	   return;

	if (channel. currentService. SId == (uint32_t)SId) {
	   if (flags != 0)
	      announcement_start (SId, flags);
	   else
	      announcement_stop ();
	}
}

static inline
int     bits (uint s) {
uint32_t startBit = 01;
        for (int i = 0; i < 15; i ++) {
           if ((s & startBit) != 0)
              return i;
           startBit <<= 1;
        }
        return 0;
}

//	do not mess with the order
void	RadioInterface::announcement_start (uint16_t SId,  uint16_t flags) {
	(void)SId;
	serviceLabel	-> setStyleSheet ("QLabel {color : red}");
	int pictureId	= bits (flags);
	QPixmap p = fetchAnnouncement (pictureId);
	displaySlide (p);
	channel. announcing = true;
}

void	RadioInterface::announcement_stop () {
	serviceLabel	-> setStyleSheet (labelStyle);
	channel. announcing = false;
	show_pauzeSlide ();
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
void	RadioInterface::handleScanListSelect (const QString &s) {
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
	presetTimer. stop ();
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
	
//	timers are stopped in the "stopService" function
	stopService (channel. currentService);

	for (int i = service. size (); i < 16; i ++)
	   serviceName. push_back (' ');

	if (!inputDevice_p -> isFileInput ()) {
	   QString theService = service;
	   QString channelName = theChannel;
	   store (dabSettings_p, "channelPresets", channelName, theService);
	}

	if (theChannel == channel. channelName) {
	   channel. currentService. isValid = false;
	   dabService s;
	   int index  = theOFDMHandler -> getServiceComp (service);
	   if (index < 0) {
	      dynamicLabel -> setText ("cannot run " + s. serviceName + " yet");
	      return;
	   }
	   s. serviceName = service;
	   startService (s, index);
	}
	else {		// selecting a service in a different channel
	   stopChannel ();
	   int k           = channelSelector -> findText (theChannel);
	   if (k != -1) {
	     newChannelIndex (k);
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

//void	RadioInterface::stopService	(dabService s) {
void	RadioInterface::stopService	(dabService &s) {
	if (!s. isValid)
	   return;
	presetTimer. stop ();
	channelTimer. stop ();
	stopMuting	();
	setSoundLabel (false);
	channel. audioActive	= false;

	announcement_stop ();
	if (s. isAudio) {
	   soundOut_p -> suspend ();
	   stopAudioDumping ();
	   stopFrameDumping ();
	   myTimeTable. clear ();

//	and clean up the technical widget
	   techWindow_p	-> cleanUp ();
//	and stop the service and erase it from the task list
	   theOFDMHandler -> stopService (s. serviceName,
	                                  s. subChId, FORE_GROUND);
	   for (int i = 0; i < (int)channel. runningTasks. size (); i ++) {
	      if (channel. runningTasks [i]. serviceName == s. serviceName)
	         if (channel. runningTasks [i]. runsBackground == false) {
	            channel. runningTasks. erase
	                        (channel. runningTasks. begin () + i);
	         }
	   }

//	stop "secondary services" - if any - as well
//	Note: they are not recorded on the tasklist
           int nrComps  =
                theOFDMHandler -> getNrComps (s. SId);
	   for (int i = 1; i < nrComps; i ++) {
	      int index =
                theOFDMHandler -> getServiceComp (s. SId, i);
              if ((index < 0) ||
                  (theOFDMHandler -> serviceType (index) != PACKET_SERVICE))
                 continue;
	      packetdata pd;
	      theOFDMHandler -> packetData (index, pd);
	      if (pd. defined) {
	         theOFDMHandler -> stopService (pd. serviceName,
	                                        pd. subchId, BACK_GROUND);
	      }
	   }
	}
	s. isValid = false;
	show_pauzeSlide ();
	cleanScreen	();
}
//
//	whether or not the epgService is triggered automatically or manually,
//	it will run in the background
void	RadioInterface::start_epgService (packetdata &pd) {
	theOFDMHandler -> setDataChannel (pd, &theDataBuffer, BACK_GROUND);
	dabService s;
	s. channel     = pd. channel;
	s. serviceName = pd. serviceName;
	s. SId         = pd. SId;
	s. subChId     = pd. subchId;
	s. fd          = nullptr;
	s. runsBackground = true;
	channel. runningTasks. push_back (s);
	epgLabel	-> show ();
}
//
void	RadioInterface::startService (dabService &s, int index) {
QString serviceName	= s. serviceName;
	s. SId		= theOFDMHandler -> getSId (index);
//	if the service is already running, ignore the call
	for (auto &serv : channel. runningTasks) {
	   if (serv. serviceName == serviceName) {
	      return;
	   }
	}
	presetTimer.	stop	();
	channel. currentService			= s;
	channel. currentService. isValid	= false;
	channel. currentService. frameDumper	= nullptr;
//	mark the selected service in the service list
//	and display the servicename on the serviceLabel
	theEnsembleHandler -> reportStart (serviceName);
	if (theOFDMHandler -> serviceType (index) == AUDIO_SERVICE) {
	   audiodata ad;
	   theOFDMHandler -> audioData (index, ad);
	   ad. channel = channel. channelName;
	   if (ad. defined) {
	      channel. currentService. isValid	= true;
	      channel. currentService. isAudio	= true;
	      channel. currentService. ASCTy	= ad. ASCTy;
	      channel. currentService. subChId	= ad. subchId;
	      techWindow_p -> showTimetableButton (true);
	      startAudioservice (ad);
//	   serviceLabel	-> setText (serviceName + "(" + ad. shortName + ")");
	      serviceLabel	-> setText (serviceName);
	      QPixmap p;
	      bool hasIcon = false;
	      if (get_serviceLogo (p, channel. currentService. SId)) {
	         hasIcon = true;
	         iconLabel ->
	            setPixmap (p. scaled (55, 55, Qt::KeepAspectRatio));
	      }
	      else
	         iconLabel -> setText (ad. shortName);
	      if (myTimeTable. isVisible ()) {
	          myTimeTable. setUp (channel. theDate, channel. Eid,
	                               channel. currentService. SId,
	                               serviceName);
	         if (hasIcon)
	            myTimeTable. addLogo (p);
	      }
	      techWindow_p	-> isDABPlus  (ad. ASCTy == DAB_PLUS);
	   }
	}
	else 
	if (theOFDMHandler -> serviceType (index) == PACKET_SERVICE) {
	   packetdata pd;
	   theOFDMHandler -> packetData (index, pd);
	   pd. channel = channel. channelName;
	   if (!pd. defined) {
	      QMessageBox::warning (this, tr ("Warning"),
 	                           tr ("insufficient data for this program\n"));
	      QString s = "";
	      store (dabSettings_p, DAB_GENERAL, PRESET_NAME, s);
	      return;;
	   }
	   if (pd. appType == 7) {
	      start_epgService (pd);
	      return;
	   }
	   serviceLabel	-> setText (pd. serviceName);
	   channel. currentService. isValid	= true;
	   channel. currentService. isAudio	= false;
	   channel. currentService. subChId	= pd. subchId;
	   startPacketservice (pd);
	}
}
//
void	RadioInterface::startAudioservice (audiodata &ad) {
	(void)theOFDMHandler -> setAudioChannel (ad, &theAudioBuffer,
	                                            nullptr, FORE_GROUND);
	uint16_t flags	= theOFDMHandler	-> getAnnouncing (ad. SId);
	if (flags != 0)
	   announcement_start (ad. SId, flags);
	else	
	   announcement_stop ();
	dabService s;
	s. channel	= ad. channel;
	s. serviceName	= ad. serviceName;
	s. SId		= ad. SId;
	s. subChId	= ad. subchId;
	s. fd		= nullptr;
	s. runsBackground	= false;
	channel. runningTasks. push_back (s);
//
//	check the other components for this service (if any)
	if (theOFDMHandler -> isPrimary (ad. serviceName)) {
	   int nrComps	=
	        theOFDMHandler -> getNrComps (ad. SId);
	   for (int i = 1; i < nrComps; i ++) {
	      int index =
	           theOFDMHandler -> getServiceComp (ad. SId, i);
	      if ((index < 0) ||
	             (theOFDMHandler -> serviceType (index) != PACKET_SERVICE))
	         continue;
	      packetdata pd;
	      theOFDMHandler -> packetData (index, pd);
	      if (pd. defined) {
	         theOFDMHandler -> setDataChannel (pd, &theDataBuffer,
	                                                BACK_GROUND);
	      }
	   }
	}
//	activate sound
	soundOut_p		-> resume ();
	channel. audioActive	= true;
	setSoundLabel (true);
	programTypeLabel	-> setText (getProgramType (ad. programType));
	rateLabel		-> setStyleSheet ("color:magenta");
	rateLabel		-> setText (QString::number (ad. bitRate) + "kbit");
//	show service related data
	techWindow_p	-> showServiceData 	(&ad);
}

void	RadioInterface::startPacketservice (packetdata &pd) {
	if ((pd.  DSCTy == 0) || (pd. bitRate == 0)) {
	   QMessageBox::warning (this, tr ("sdr"),
 	                         tr ("still insufficient data for this service\n"));
	   return;
	}

	if (!theOFDMHandler -> setDataChannel (pd, &theDataBuffer,
	                                              FORE_GROUND)) {
	   QMessageBox::warning (this, tr ("sdr"),
 	                         tr ("could not start this service\n"));
	   return;
	}

	switch (pd. DSCTy) {
	   default:
	      showLabel (QString ("unimplemented Data"), 1);
	      break;
	   case 5:
	      showLabel (QString ("Transp. Channel partially implemented"), 1);
	      break;
	   case 60:
	      showLabel (QString (" processing MOT data"), 1);
	      break;
	   case 59: {
#ifdef	_SEND_DATAGRAM_
	      QString text = QString ("Embedded IP: UDP data to ");
	      text. append (ipAddress);
	      text. append (" ");
	      QString n = QString::number (port);
	      text. append (n);
	      showLabel (text, 1);
#else
	      showLabel ("Embedded IP not supported ", 1);
#endif
	   }
	      break;
	   case 44:
	      showLabel (QString ("Journaline"), 1);
	      break;
	}
}

//	This function is only used in the Gui to clear
//	the details of a selected service
void	RadioInterface::cleanScreen	() {
	serviceLabel			-> setText ("");
	iconLabel			-> setPixmap (QPixmap ());
	dynamicLabel			-> setText ("");
	stereoLabel			-> setText ("");
	programTypeLabel 		-> setText ("");
	psLabel				-> setText ("");
	sbrLabel			-> setText ("");
	audiorateLabel			-> setText ("");
	rateLabel			-> setText ("");

	stereoSetting			= false;
	setStereo	(false);
	techWindow_p			-> cleanUp ();
	motLabel			-> setStyleSheet ("QLabel {color : red}");
	distanceLabel			-> setText ("");
	transmitter_country		-> setText ("");
	theNewDisplay. ficError_display	-> setValue (0);
}

////////////////////////////////////////////////////////////////////////////
//
void	RadioInterface::handle_prevServiceButton        () {
	theEnsembleHandler -> selectPrevService ();
}

void	RadioInterface::handle_nextServiceButton        () {
	theEnsembleHandler -> selectNextService ();
//	stressTimer. setSingleShot (true);
//	stressTimer. start (1000);
}
//
//	The user(s)
///////////////////////////////////////////////////////////////////////////
//	setPresetService () is called after a time out to 
//	actually start the service that we were waiting for
void	RadioInterface::setPresetService () {
	if (!running. load ())
	   return;
	if (theSCANHandler. active ())
	   return;
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
	int index = theOFDMHandler	-> getServiceComp (presetName);
	if (index < 0) {
	   dynamicLabel -> setText (QString ("not all data for ") +
	                            s. serviceName + " on board");
	   return;
	}
	startService (s, index);
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
	theEnsembleHandler	-> reset ();
	theNewDisplay. showFrequency (theChannel, tunedFrequency);
	presetTimer. stop	();		// should not run
	inputDevice_p		-> restartReader (tunedFrequency, 
	                                                 SAMPLERATE / 10);
	channel. cleanChannel	();
	channel. channelName	= theChannel;
	channel. tunedFrequency	= tunedFrequency;
	channel. countryName	= "";
	theLogger. log (logger::LOG_NEW_CHANNEL, theChannel, channel. snr);
	channel. realChannel	= !inputDevice_p -> isFileInput ();
	if (channel. realChannel) {
	   store (dabSettings_p, DAB_GENERAL, CHANNEL_NAME, theChannel);
	}

//	The ".sdr" and ".uff" files - when built by us - carry
//	the channel frequency in their data
	if (inputDevice_p -> isFileInput ()) {
	   channelSelector		-> setEnabled (false);
	   int freq			= inputDevice_p -> getVFOFrequency ();
	   QString realChannel = theSCANHandler. getChannel (freq);
	   if (realChannel != "") {
	      int k = channelSelector -> findText (realChannel);
	      channelSelector		-> setCurrentIndex (k);
	      channel. channelName	= realChannel;
	      channel. tunedFrequency	= freq;
	      theNewDisplay. showFrequency (realChannel, freq);
	   }
	   else {
	      channel. channelName	= "";
	      channel. tunedFrequency	= -1;
	   }
	}

	distanceLabel		-> setText ("");
	theDXDisplay. 		cleanUp ();
	theDXDisplay. setChannel (channel. channelName, channel. ensembleName);
	theNewDisplay. 		cleanTII	();
	theNewDisplay. 		showTransmitters (channel. transmitters);
	if (mapHandler != nullptr)
	   mapHandler -> putData (MAP_FRAME, position {-1, -1});

	if (theSCANHandler. active ()) {
	   theOFDMHandler	-> start ();
	   return;
	}
//
//	If we are scanning, we do not do delayed service start
	int switchDelay		=
	             configHandler_p -> switchDelayValue ();
//	if no preset is started, we look in the tables what the servicename
//	was the last time the channel was active
	if (!inputDevice_p -> isFileInput () &&
	                   !theSCANHandler. active ()) {

	   if (firstService == "") 	// no preset specified
	      firstService =
	            value_s (dabSettings_p, "channelPresets", theChannel, "");
//	at this point we do not know whether or not a preset is 
//	set, so if this mode is "on" and there is a service name
//	associated with the channel, we set the preset handling on
//	but only if ....
	   if (firstService != "") {
	      presetTimer. stop ();	// should not run here
	      nextService. channel	= theChannel;
	      nextService. serviceName	= firstService;
	      nextService. SId		= 0;
	      presetTimer. setSingleShot (true);
	      presetTimer. setInterval	(switchDelay);
	      presetTimer. start	(switchDelay);
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
	epgTimer. stop		();		// if running
	epgLabel	-> hide ();
	presetTimer. stop 	();		// if running
	channelTimer. stop	();		// if running
	if (myTimeTable. isVisible ()) { 
	   myTimeTable. clear ();
	   myTimeTable. hide ();
	}

	inputDevice_p		-> stopReader	();
	inputDevice_p		-> stopDump	();
	disconnect (ensembleId, &clickablelabel::clicked,
	            this, &RadioInterface::handle_contentButton);
	ensembleId	-> setText ("");
	stopSourceDumping	();
	if (channel. etiActive)
	   stop_etiHandler	();	// 
	theLogger. log (logger::LOG_CHANNEL_STOPS, channel. channelName);
	transmitter_country	-> setText	("");
	theNewDisplay. setSilent	();
//
	for (auto &serv : channel. runningTasks) {
	   if (!serv. runsBackground) 
	      theOFDMHandler -> stopService (serv. serviceName,
	                                     serv. subChId, FORE_GROUND);
	   else
	      theOFDMHandler -> stopService (serv. serviceName,
	                                     serv. subChId, BACK_GROUND);
	
	   if (serv. fd != nullptr)
	      fclose (serv. fd);
	}
	channel. runningTasks. resize (0);

	if (contentTable_p != nullptr) {
	   contentTable_p -> hide ();
	   delete contentTable_p;
	   contentTable_p = nullptr;
	}
//	note framedumping - if any - was already stopped
//	ficDumping - if on - is stopped here
	theOFDMHandler -> stopFicDump ();	// just in case ...
	theOFDMHandler		-> stop ();
	theDXDisplay. cleanUp ();
	usleep (1000);
	techWindow_p	-> cleanUp ();

	show_pauzeSlide ();
	channel. cleanChannel	();
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
	setChannelButton (newChannel % nrChannels);
}

void	RadioInterface::handle_prevChannelButton () {
int	nrChannels	= channelSelector -> count ();
	if (channelSelector -> currentIndex () == 0)
	   setChannelButton (nrChannels - 1);
	else
	   setChannelButton (channelSelector -> currentIndex () - 1);
}

void	RadioInterface::setChannelButton (int currentChannel) {
	if (!running. load ())
	   return;

	presetTimer. stop ();
	stopScanning	();
	stopChannel	();
	newChannelIndex (currentChannel);
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
	if (inputDevice_p -> isFileInput ()) {
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("Scanning not useful with file input"));
	   return;
	}
	stopChannel     ();
	theEnsembleHandler	-> setShowMode (SHOW_ENSEMBLE);
	presetButton		-> setText ("not in use");
	presetButton		-> setEnabled (false);
//	scanning and showing the techWindows does not make much sense
	techWindow_p 		-> hide ();	// until shown otherwise
        store (dabSettings_p, DAB_GENERAL, TECHDATA_VISIBLE, false);
	presetTimer. stop ();
	channelTimer. stop ();
	epgTimer. stop ();
	connect (theOFDMHandler. data (), &ofdmHandler::noSignalFound,
	         this, &RadioInterface::no_signal_found);

	if (theSCANHandler. scan_to_data ())
	   startScan_to_data ();
	else
	if (theSCANHandler. scan_single ())
	   startScan_single ();
	else
	   startScan_continuous ();
}

void	RadioInterface::startScan_to_data () {
//	when running scan to data, we look at all channels, whether
//	on the skiplist or not
	QString cs = theSCANHandler. getNextChannel (channelSelector -> currentText ());
	int cc = channelSelector -> findText (cs);
//	LOG scanning starts
	newChannelIndex (cc);
//	theSCANHandler. addText (" scanning channel " +
//	                            channelSelector -> currentText ());
	int switchDelay		=
	              configHandler_p -> switchDelayValue ();
	channelTimer. start (switchDelay);
	theOFDMHandler	-> setScanMode (true);
	startChannel    (channelSelector -> currentText ());
}

void	RadioInterface::startScan_single () {
basicPrint thePrinter;
	theScanlistHandler. clearScanList ();
	
	if (scanTable_p == nullptr) 
	   scanTable_p = new contentTable (this, dabSettings_p, "scan", 
	                                       thePrinter. scanWidth ());
	else					// should not happen
	   scanTable_p -> clearTable ();

	QString topLine = QString ("ensemble") + ";"  +
	                           "channelName" + ";" +
	                           "frequency (KHz)" + ";" +
	                           "Eid" + ";" +
	                           "time" + ";" +
	                           "SNR" + ";" +
	                           "nr services" + ";" ;
	 
	scanTable_p	-> addLine (topLine);
	scanTable_p	-> addLine ("\n");

	theOFDMHandler	-> setScanMode (true);
	QString fs	= theSCANHandler. getFirstChannel ();
	int k = channelSelector ->  findText (fs);
	if (k != -1)
	   newChannelIndex (k);
	theSCANHandler. addText (" scanning channel " +
	                            channelSelector -> currentText ());
	int switchDelay		=
	             configHandler_p -> switchDelayValue ();
	channelTimer. start (switchDelay);
	startChannel    (channelSelector -> currentText ());
}

void	RadioInterface::startScan_continuous () {
basicPrint thePrinter;
	if (scanTable_p == nullptr) 
	   scanTable_p = new contentTable (this, dabSettings_p, "scan", 
	                                              thePrinter.scanWidth ());
	else					// should not happen
	   scanTable_p -> clearTable ();

	QString topLine = QString ("ensemble") + ";"  +
	                           "channelName" + ";" +
	                           "frequency (KHz)" + ";" +
	                           "Eid" + ";" +
	                           "tii" + ";" +
	                           "time" + ";" +
	                           "SNR" + ";" +
	                           "nr services" + ";" +
	                           "transmitterName;" +
	                           "distance;" +
	                           "azimuth;" +
	                           "height";
	scanTable_p -> addLine (topLine);
	scanTable_p	-> addLine ("\n");

	theOFDMHandler	-> setScanMode (true);
//      To avoid reaction of the system on setting a different value:
	QString fs = theSCANHandler. getFirstChannel ();
	int k = channelSelector -> findText (fs);
	newChannelIndex (k);
	int switchDelay		=
	             configHandler_p -> switchDelayValue ();
	channelTimer. start (2 * switchDelay);
	startChannel    (channelSelector -> currentText ());
//	fprintf (stderr, "de scan start met  %s\n",
//	              channelSelector -> currentText (). toLatin1 (). data ());
}
//
//	stopScanning is called
//	1. when the scanbutton is touched during scanning
//	2. on user selection of a service or a channel select
//	3. on device selection
//	4. on handling a reset
void	RadioInterface::stopScanning	() {
	if (!theSCANHandler. active ())
	   return;
//	fprintf (stderr, "De scan wordt gestopt\n");
	disconnect (theOFDMHandler. data (), &ofdmHandler::noSignalFound,
	            this, &RadioInterface::no_signal_found);
	presetButton	-> setText ("favorites");
	presetButton	-> setEnabled (true);
//	LOG scanning stops
	channelTimer. stop ();

	if (theSCANHandler. scan_to_data ())
	   stopScan_to_data ();
	else
	if (theSCANHandler. scan_single ())
	   stopScan_single ();
	else
	   stopScan_continuous ();
	theSCANHandler. setStop ();
//	presetButton	-> setEnabled (true);
}

void	RadioInterface::stopScan_to_data () {
	theOFDMHandler	-> setScanMode (false);
	channelTimer. stop ();
}

void	RadioInterface::stopScan_single () {
	theOFDMHandler	-> setScanMode (false);
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
//	delete scanTable_p;
//	scanTable_p	= nullptr;
}

void	RadioInterface::stopScan_continuous () {
	theOFDMHandler	-> setScanMode (false);
	channelTimer. stop ();

	if (scanTable_p == nullptr)
	   return;		// should not happen

	FILE *scanDumper_p	= theSCANHandler. askFileName ();
	if (scanDumper_p != nullptr) {
	   scanTable_p -> dump (scanDumper_p);
	   fclose (scanDumper_p);
	   scanDumper_p = nullptr;
	}
//	delete scanTable_p;
//	scanTable_p	= nullptr;
}

//	If the ofdm processor has waited - without success -
//	for a period of N frames to get a start of a synchronization,
//	it sends a signal to the GUI handler
//	If "scanning" is "on" we hop to the next frequency on
//	the list.
//	Also called as a result of time out on channelTimer

void	RadioInterface::channel_timeOut () {
// fprintf (stderr, "Channel timeout\n");
	channelTimer. stop ();
	if (!theSCANHandler. active ())
	   return;

	if (theSCANHandler. scan_to_data ()) {
	   nextFor_scan_to_data ();
	}
	else	
	if (theSCANHandler. scan_single ()) {
	   if (channel. transmitters. size () > 0) {
	      QStringList ss;
	      for (auto &tr : channel. transmitters)
	         ss << tr. transmitterName;
	      theSCANHandler.
	                 addTransmitters (ss, channel. channelName);
	   }
	   nextFor_scan_single ();
	}
	else {
	   nextFor_scan_continuous ();
	}
}

void	RadioInterface::nextFor_scan_to_data () {
	if  (channel. nrServices > 0) {
	   stopScanning ();
	   return;
	}
	stopChannel ();
	QString ns	= theSCANHandler. getNextChannel ();
	int cc = channelSelector -> findText (ns);
	newChannelIndex (cc);
//
//	and restart for the next run
	theSCANHandler. addText ("scanning channel " +
	                             channelSelector -> currentText ());
	int switchDelay	= 
	         configHandler_p -> switchDelayValue ();
	channelTimer. start (switchDelay);
	startChannel (channelSelector -> currentText ());
}

void	RadioInterface::nextFor_scan_single () {
	if (channel. nrServices > 0) 
	   show_for_single_scan ();
	stopChannel ();
	try {
	   QString cs	= theSCANHandler. getNextChannel ();
	   int	cc	= channelSelector -> findText (cs);
	   newChannelIndex (cc);
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

void	RadioInterface::nextFor_scan_continuous () {
	if (channel. nrServices > 0)
	   show_for_continuous ();
	stopChannel ();

	QString cs	= theSCANHandler. getNextChannel ();
//	fprintf (stderr, "Going for channel %s\n", cs. toLatin1 (). data ());
	int cc	= channelSelector -> findText (cs);
	newChannelIndex (cc);
	int switchDelay	= 
	         configHandler_p -> switchDelayValue ();
	channelTimer. start (2 * switchDelay);
	startChannel (channelSelector -> currentText ());
}

////////////////////////////////////////////////////////////////////////////
QString RadioInterface::buildHeadLine () {
QString SNR 		= "SNR " + QString::number (channel. snr);
QString	tii;
QString	theName;
QString theDistance;
QString	theAzimuth;
QString	theHeight;

	if (channel. mainId != -1) 
	   tii		= idsToString (channel. mainId,
	                                     channel. subId);
	else
	   tii		= "???";
	if (channel. transmitterName != "")
	   theName	= channel. transmitterName;
	else
	   theName 	= " ";
	if (channel. distance > 0) {
	   theDistance	= QString::number (channel. distance, 'f', 1) + " km ";
	   theAzimuth	= QString::number (channel. azimuth, 'f', 1)
	                      + QString::fromLatin1 (" \xb0 ");
	   theHeight	= " (" + QString::number (channel. height, 'f', 1) +  "m)";
	}
	else {
	   theName	= "unknown";
	   theDistance	= "unknown";
	   theAzimuth	= "";
	   theHeight	= "";
	}
	   
	QString utcTime	= convertTime (UTC. year, UTC.month,
	                               UTC. day, UTC. hour, 
	                               UTC. minute);
	QString headLine = channel. ensembleName + ";" +
	                      channel. channelName  + ";" +
	                      QString::number (channel. tunedFrequency) + ";" +
	                      hextoString (channel. Eid) + ";" +
	                      tii + ";" +
	                      utcTime + ";" +
	                      SNR + ";" +
	                      QString::number (channel. nrServices) +";" +
	                      theName + ";" +
			      theDistance + ";" +
	                      theAzimuth + ";" + theHeight;
	return headLine;
}

QString RadioInterface::build_kop	() {
QString SNR 		= "SNR " + QString::number (channel. snr);
QString	theName;

	QString utcTime	= convertTime (UTC. year, UTC.month,
	                               UTC. day, UTC. hour, 
	                               UTC. minute);
	int	freeSpace	= theOFDMHandler -> freeSpace ();
	QString headLine = channel. ensembleName + ";" +
	                      channel. channelName  + ";" +
	                      QString::number (channel. tunedFrequency) + ";" +
	                      hextoString (channel. Eid) + ";" +
	                      utcTime + ";" +
	                      SNR + ";" +
	                      QString::number (channel. nrServices) +";"  +
	                      QString::number (freeSpace) + ", " +
	                      QString::number ((int) (freeSpace / 864.0 * 100)) + "%" + ";";
	return headLine;
}

QString	RadioInterface::build_transmitterLine (const transmitter &c) {
QString res	= "";
	res += ";";
	res += idsToString (c. mainId, c. subId) + ";";
	res += c. transmitterName + ";";
	res += QString::number (c. distance, 'f', 1) + "km;";
	res += QString::number (c. azimuth, 'f', 1) +
	               QString::fromLatin1 (" \xb0 ") + ";";
	res += QString::number (c. altitude, 'f', 1) + "m;";
	res += QString::number (c. height, 'f', 1) + "m;\n";
	return res;
}


void	RadioInterface::show_for_single_scan () {
QString	headLine = build_kop ();
	scanTable_p -> addLine (headLine);
	scanTable_p -> addLine ("\n");
	for (const auto &tr : channel. transmitters) {
	   QString transmitterLine = build_transmitterLine (tr);
           scanTable_p	-> addLine (transmitterLine);
        }
	basicPrint thePrinter;
	QStringList s = thePrinter. print (theOFDMHandler -> contentPrint ());
	for (const auto &l : s)
	   scanTable_p -> addLine (l);
	scanTable_p -> addLine ("\n;\n;\n");
	scanTable_p -> show ();
}

void	RadioInterface::show_for_continuous () {
	QString headLine = buildHeadLine ();
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

QString RadioInterface::build_cont_addLine (const transmitter &tr) {
QString tii;
QString theName;
QString theDistance;
QString theCorner;
QString theHeight;

	tii		= idsToString (tr. mainId, tr. subId) + ";" ;
	if (tr. transmitterName != "")
	   theName	= tr. transmitterName;
	else
	   theName 	= "";
	
	if (tr. distance > 0) {
	   theDistance	= QString::number (tr. distance, 'f', 1) + " km ";
	   theCorner	= QString::number (tr. azimuth, 'f', 1)
	                      + QString::fromLatin1 (" \xb0 ");
	   theHeight	= " (" + QString::number (tr. height, 'f', 1) +  "m)";
	}
	else {
	   theName	= "unknown";
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
	                  theName +";" +
	                  theDistance + ";" +
	                  theCorner + ";" + theHeight;
}

/////////////////////////////////////////////////////////////////////
//
//	Handling the Mute button
void	RadioInterface::handle_muteButton	() {
//	do not activate mute timer if volume == 0
//	but Volume is only defined with Qt_Audio
	if ((soundOut_p -> is_QtAudio () && audioVolume == 0) &&
	                           !muteTimer. isActive ())
	   return;
	if (muteTimer. isActive ()) {
	   stopMuting ();
	   return;
	}
	if (!channel. audioActive)
	   return;
	setSoundLabel (false);
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
	}
	else {
	   disconnect (&muteTimer, &QTimer::timeout,
	               this, &RadioInterface::muteButton_timeOut);
	   stillMuting	-> hide ();
	   if (channel. audioActive)
	      setSoundLabel (true);
	}
}

void	RadioInterface::stopMuting		() {
	if (!muteTimer. isActive ()) 
	   return;
	setSoundLabel (true);
	muteTimer. stop ();
	disconnect (&muteTimer, &QTimer::timeout,
	               this, &RadioInterface::muteButton_timeOut);
	stillMuting	-> hide ();
}
//
//	End of handling mute button

//	newChannelIndex is called whenever we are sure that
//	the channel selector is "connected", and we programamtically
//	change the setting, which obviously would lead to a signal
//	that we do not wwant right now

void	RadioInterface::newChannelIndex (int index) {
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

QString	tiiButton_color =
	   value_s (dabSettings_p, COLOR_SETTINGS, TII_BUTTON + "_color",
	                                              YELLOW);
QString tiiButton_font	=
	   value_s (dabSettings_p, COLOR_SETTINGS, TII_BUTTON + "_font",
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
	tiiButton	->
	              setStyleSheet (temp. arg (tiiButton_color,
	                                        tiiButton_font));
	prevServiceButton ->
	              setStyleSheet (temp. arg (prevServiceButton_color,
	                                        prevServiceButton_font));
	nextServiceButton ->
	              setStyleSheet (temp. arg (nextServiceButton_color, 
	                                        nextServiceButton_font));
}

void	RadioInterface::color_scanButton	() {
	setButtonColors (scanButton, SCAN_BUTTON);
}

void	RadioInterface::color_spectrumButton	()	{
	setButtonColors (spectrumButton, SPECTRUM_BUTTON);
}

void	RadioInterface::color_scanListButton	()	{
	setButtonColors (scanListButton, SCANLIST_BUTTON);
}
	                                        
void    RadioInterface::color_presetButton      ()      {
	setButtonColors (presetButton, PRESET_BUTTON);
}  

void	RadioInterface::color_prevServiceButton ()      {
	setButtonColors (prevServiceButton, PREVSERVICE_BUTTON);
}                                               

void	RadioInterface::color_nextServiceButton ()      {
	setButtonColors (nextServiceButton, NEXTSERVICE_BUTTON);
}                               
	   
void	RadioInterface::color_configButton	() 	{
	setButtonColors (configButton, CONFIG_BUTTON);
}

void	RadioInterface::color_httpButton	() 	{
	setButtonColors (httpButton, HTTP_BUTTON);
}

void	RadioInterface::color_tiiButton	() 	{
	setButtonColors (tiiButton, TII_BUTTON);
}

void	RadioInterface::setButtonColors	(QPushButton *b,
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

	QStringList selectables	= theEnsembleHandler -> getSelectables ();
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
	   scheduledAudioDumping ();
	   return;
	}

	if (s == "dlText") {
	   scheduledDLTextDumping ();
	   return;
	}

	if (s == "ficDump") {
	   scheduledFICDumping ();
	   return;
	}

	presetTimer. stop ();
	stopScanning ();
	scheduleSelect (s);
}

void	RadioInterface::scheduledFICDumping () {
	if (theOFDMHandler -> ficDumping_on ()) {
	   theOFDMHandler	-> stopFicDump ();
	   return;
	}

	QString ficDumpFileName =
	     theFilenameFinder. find_ficDump_file (channel. channelName);
	if (ficDumpFileName == "")
	   return;
	theOFDMHandler -> startFicDump (ficDumpFileName);
}

//------------------------------------------------------------------------
//
//	if configured, the interpreation of the EPG data starts automatically,
//	the servicenames of an SPI/EPG service may differ from one country
//	to another
void	RadioInterface::epgTimer_timeOut	() {
	epgTimer. stop ();
	
	if (theSCANHandler. active ())
	   return;
	if (value_i (dabSettings_p, CONFIG_HANDLER, "epgFlag", 0) != 1)
	   return;
	QStringList epgList = theEnsembleHandler -> getEpgServices ();
	for (auto &serv : epgList) {
	   int index = theOFDMHandler -> getServiceComp (serv);
	   if (index < 0)
	      continue;
	   if (theOFDMHandler -> serviceType (index) != PACKET_SERVICE)
	      continue;
	   packetdata pd;
	   theOFDMHandler -> packetData (index, pd);
	   if ((!pd. defined) ||
	            (pd.  DSCTy == 0) || (pd. bitRate == 0)) 
	      continue;
	   if (pd. DSCTy == 60) {
//	LOG hidden service starts
//	      fprintf (stderr, "Starting hidden service %s\n",
//	                                serv. toUtf8 (). data ());
	      start_epgService (pd);
	   }
	}
}

void	RadioInterface::handle_timeTable	() {
	if (myTimeTable. isVisible ()) {
	   myTimeTable. clear ();
	   myTimeTable. hide ();
	   return;
	}
	if (!channel. currentService. isValid ||
	                     !channel. currentService. isAudio)
	   return;

	myTimeTable. setUp (channel. theDate, channel. Eid,
	                     channel. currentService. SId,
	                     channel. currentService. serviceName);
	QPixmap p;
	if (get_serviceLogo (p, channel. currentService. SId)) // this may be
	   myTimeTable. addLogo (p);
}

//----------------------------------------------------------------------
//
void	RadioInterface::scheduledDLTextDumping () {
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
	if (inputDevice_p. isNull ())
	   return;
	int currentVisibility = inputDevice_p -> getVisibility ();
	inputDevice_p	-> setVisibility (!currentVisibility);
	store (dabSettings_p, DAB_GENERAL, DEVICE_WIDGET_VISIBLE,
	                                             !currentVisibility);
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
QString home	= QDir::homePath ();
	if (!home. endsWith ("/"))
	   home += "/";
	home += ".txdata.tii";
	home	= QDir::fromNativeSeparators (home);
	theTIIProcessor. reload (home);
}

void	RadioInterface::stopSourceDumping	() {
	if (!sourceDumping)
	   return;
	theLogger. log (logger::LOG_SOURCEDUMP_STOPS);
	theOFDMHandler	-> stopDumping();
	sourceDumping	= false;
	configHandler_p	-> mark_dumpButton (false);
}

void	RadioInterface::startSourceDumping () {
QString deviceName	= inputDevice_p -> deviceName	();
int	bitDepth	= inputDevice_p	-> bitDepth	();
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
	theOFDMHandler -> startDumping (rawDumpName,
	                                channel. tunedFrequency, 
	                                bitDepth,
	                                inputDevice_p -> deviceName ());
	sourceDumping = true;
}

void	RadioInterface::handle_sourcedumpButton () {
	if (!running. load () || theSCANHandler. active ())
	   return;
	if (sourceDumping)
	   stopSourceDumping ();
	else
	   startSourceDumping ();
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

void	RadioInterface::set_transmitters_local  (bool isChecked) {
	channel. targetPos	= position {0, 0};
	if ((isChecked) && (mapHandler != nullptr))
	   mapHandler -> putData (MAP_RESET, channel. targetPos);
}

void	RadioInterface::selectDecoder (int decoder) {
	if (decoder == DECODER_4)
	   QMessageBox::information (this, tr ("Warning"),
	                         tr ("Decoder is experimental"));
	if (!theOFDMHandler. isNull ())
	   theOFDMHandler	-> handleDecoderSelector (decoder);
}

void	RadioInterface:: set_streamSelector (int k) {
	if (!running. load ())
	   return;
	QString str = configHandler_p -> currentStream ();
	((audioSink *)(soundOut_p)) -> selectDevice (k, str);
	store (dabSettings_p, SOUND_HANDLING, AUDIO_STREAM_NAME, str);
}
//
//////////////////////////////////////////////////////////////////////////
//
//	Some, by far not all, ensembles are transmitted with a
//	specification of the number of services carried
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

	   mapHandler = new httpHandler (this,
	                                 mapPort,
	                                 browserAddress,
	                                 localPos,
	                                 QString (""),
	                                 configHandler_p -> localBrowserSelector_active (), dabSettings_p);
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
int h   = 3 * w / 4;
	if (channel. announcing)
	   return;
	pauzeTimer. stop ();
	pictureLabel	-> setAlignment(Qt::AlignCenter);
	if ((p. width () != 320) || (p. height () != 200))
	   pictureLabel ->
	       setPixmap (p. scaled (w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	else
	   pictureLabel -> setPixmap (p);
	pictureLabel -> setToolTip (t);
	pictureLabel -> show ();
}

void	RadioInterface::show_pauzeSlide () {
QPixmap p;
QString slideName	= ":res/radio-pictures/pauze-slide-%1.png";
	pauzeTimer. stop ();
//	int nr		= rand () % 11;
	slideName	= slideName. arg (pauzeSlideTeller);
	if (p. load (slideName, "png")) {
	QString tooltipText;
	   switch (pauzeSlideTeller) {
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
	pauzeSlideTeller = (pauzeSlideTeller + 1) % 11;
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
	if (channel. etiActive)
	   stop_etiHandler ();
	else
	   start_etiHandler ();
}

void	RadioInterface::stop_etiHandler () {
	if (!channel. etiActive) 
	   return;

	theLogger. log (logger::LOG_ETI_STOPS);
	theOFDMHandler -> stopEtiGenerator ();
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
	channel. etiActive = theOFDMHandler -> startEtiGenerator (etiFile);
	if (channel. etiActive) 
	   scanButton -> setText ("eti runs");
}

void	RadioInterface::handle_eti_activeSelector (int k) {
bool setting	= configHandler_p -> eti_active ();
	(void)k;
	if (setting) {
	   stopScanning ();
	   disconnect (scanButton, &QPushButton::clicked,
	               this, &RadioInterface::handle_scanButton);
	   connect (scanButton, &QPushButton::clicked,
	            this, &RadioInterface::handle_etiHandler);
	   scanButton -> setEnabled (true);
	   scanButton	-> setText ("eti");
//	   if (!inputDevice_p -> isFileInput ())// restore the button' visibility
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
std::vector<Complex> inBuffer (SAMPLERATE / 1000);
	(void)amount;
	if (theSpectrumBuffer. GetRingBufferReadAvailable () < SAMPLERATE / 1000)
	   return;
	theSpectrumBuffer. getDataFromBuffer (inBuffer. data (), SAMPLERATE / 1000);
	theSpectrumBuffer. FlushRingBuffer ();
	if (!theNewDisplay. isHidden () &&
	           (theNewDisplay. getTab () == SHOW_SPECTRUM))
	   theNewDisplay. showSpectrum (inBuffer, channel. tunedFrequency);
}

void	RadioInterface::handle_tiiThreshold	(int v) {
	store (dabSettings_p, CONFIG_HANDLER, TII_THRESHOLD, v);
	theOFDMHandler -> setTIIThreshold (v);
}

void	RadioInterface::show_tii_spectrum	() {
std::vector<Complex> inBuffer (SAMPLERATE / 1000);

	if (theTIIBuffer. GetRingBufferReadAvailable () < SAMPLERATE / 1000)
	   return;
	theTIIBuffer. getDataFromBuffer (inBuffer. data (), SAMPLERATE / 1000);
	theTIIBuffer. FlushRingBuffer ();
	if (!theNewDisplay. isHidden () &&
	           (theNewDisplay. getTab () == SHOW_TII)) {
	   if (channel. subId != 0)
	      theNewDisplay. showTII (inBuffer,
	                           channel. tunedFrequency, channel. subId);
	   else
	      theNewDisplay. showTII (inBuffer,
	                           channel. tunedFrequency, -1);
	}
}

void	RadioInterface::showCorrelation	(int s, int g, QVector<int> maxVals) {
std::vector<float> inBuffer (s);

	(void)g;
	theResponseBuffer. getDataFromBuffer (inBuffer. data (), s);
	theResponseBuffer. FlushRingBuffer ();
	if (!theNewDisplay. isHidden ()) {
	   if (theNewDisplay. getTab () == SHOW_CORRELATION)
	      theNewDisplay. showCorrelation (inBuffer,
	                                       maxVals,
	                                       g,
	                                       channel. transmitters);
	}
}

void	RadioInterface::show_null		(int amount, int startIndex) {
Complex	*inBuffer  = (Complex *)(alloca (amount * sizeof (Complex)));
	theNULLBuffer. getDataFromBuffer (inBuffer, amount);
	if (!theNewDisplay. isHidden ())
	   if (theNewDisplay. getTab () ==  SHOW_NULL)
	      theNewDisplay. showNULL (inBuffer, amount, startIndex);
}

void	RadioInterface::removeFromList (uint8_t mainId, uint8_t subId) {
	for (uint16_t i = 0; i < channel. transmitters. size (); i ++)
	   if ((mainId == channel. transmitters [i]. mainId) &&
	       (subId == channel. transmitters [i]. subId)) {
	          channel. transmitters. erase
	                         (channel. transmitters. begin () + i);
	         break;
	   }
}
//
transmitter *RadioInterface::inList (uint8_t mainId, uint8_t subId) {
	for (auto &tr: channel. transmitters) 
	   if ((tr. mainId == mainId) &&
	       (tr. subId ==  subId))
	     return &tr;
	return nullptr;
}

void	RadioInterface::show_tiiData	(QVector<tiiData> r, int ind) {

	(void)ind;
	if (r. size () == 0)
	   return;

	if (!dxMode) {
	   if (!theDXDisplay. isHidden ())
	      theDXDisplay. hide ();
	}

//	first step: are we going to process the data or not?
	if (!running. load ())	// shouldn't be
	   return;

	if ((localPos. latitude == 0) || (localPos. longitude == 0)) 
	   return;

	if (!theTIIProcessor. has_tiiFile ())	// should not happen
	   return;

	if (channel. Eid == 0)
	   return;
//	probably yes, get the country code
	if ((channel. countryName == "") && (channel. hasEcc)) {
	   QString country	= find_ITU_code (channel. eccByte,
	                                         (channel. Eid >> 12) &0xF);
	   channel. countryName	= country;
	   channel. transmitterName = "";
	   transmitter_country	-> setText (country);
	}

//	The data in the vector is sorted on signal strength

//	first step
//	see whether or not the data is already in the list
//	if it is already
	for (uint16_t i = 0; i < r. size (); i ++) {
	   if ((r [i]. mainId == 0) ||
	       (r [i]. mainId == 255))
	      continue;
//
//	It the new TII data make sense and there is already
//	an item in the transmitterList, then just check whether
//	or not that item as recognized
	   transmitter *to = inList (r [i]. mainId, r [i]. subId);
	   if (to != nullptr) {
	      if (to -> transmitterName == "not in database") {
	         removeFromList (to -> mainId, to -> subId);
	      }
 	      else {
	         to -> strength = r [i]. strength;
//	         need_to_print = false;
	         continue;
	      }
	   }

	   dbElement * tr =
	       channel. realChannel ?
	         theTIIProcessor. getTransmitter (channel. channelName,
	                                          channel. Eid,
	                                          r [i]. mainId,  r [i]. subId):
	         theTIIProcessor. getTransmitter (channel. Eid,
	                                          r [i]. mainId, r [i]. subId);
	   transmitter theTransmitter (tr);
//
	   if (!tr -> valid) {		// nothing found
	      if (!configHandler_p -> get_allTIISelector ())
	         continue;
	      theTransmitter. valid	= false;
	      theTransmitter. ensemble	= channel. ensembleName;
	      theTransmitter. mainId	= r [i]. mainId;
	      theTransmitter. subId	= r [i]. subId;
	      theTransmitter. pattern	= r [i]. pattern;
	      theTransmitter. phase	= r [i]. phase;
	      theTransmitter. norm	= r [i]. norm;
	      theTransmitter. transmitterName	= "not in database";
	      theTransmitter. distance	=  -1;
	      theTransmitter. strength	= r [i]. strength;
	      theTransmitter. isStrongest	= false;
//	      transmitterDesc t = {false, false, theTransmitter};
//	      channel. transmitters. push_back (t);	
	      channel. transmitters. push_back (theTransmitter);	
	   }
	   else {
//	first copy the db data, theTransmitter is properly initalized
//	with the db Calues, now the dynamics
	      theTransmitter. valid	= true;
	      position thePosition;
	      thePosition. latitude     = theTransmitter. latitude;
	      thePosition. longitude    = theTransmitter. longitude;
	      theTransmitter. distance  = distance   (thePosition, localPos);
	      theTransmitter. azimuth	= corner     (thePosition, localPos);
	      theTransmitter. strength	= r [i]. strength;
	      theTransmitter. phase	= r [i]. phase;
	      theTransmitter. norm	= r [i]. norm;
	      theTransmitter. collision	= r [i]. collision;
	      theTransmitter. pattern	= r [i]. pattern;
	      theTransmitter. isStrongest	= false;
	      channel. transmitters. push_back (theTransmitter);	
//	      transmitterDesc t = {true,  false, theTransmitter};
//	      channel. transmitters. push_back (t);	
	   }
	   if (dxMode)
	      addtoLogFile (&theTransmitter);
//	   need_to_print = true;
	}

//
	int	bestIndex = -1;
	float Strength	= -100;
//	Now the list is updated, see whether or not the strongest is ...
	int teller = 0;
	for (auto &transm : channel. transmitters) {
	   if (transm. valid &&
	                  (transm. strength > Strength)) {
	      bestIndex = teller;
	      Strength  = transm. strength;
	   }
	   transm. isStrongest = false;
	   teller ++;
	}


	if (bestIndex >= 0) {
	   channel. transmitters [bestIndex]. isStrongest = true;
	}
//
//	for content maps etc we need to have the data of the strongest
//	signal
	if (bestIndex >= 0) {
	   transmitter *ce = &channel. transmitters [bestIndex];	
	   channel. mainId		= ce -> mainId;
	   channel. subId		= ce -> subId;
	   channel. transmitterName	= ce -> transmitterName;
	   channel. height		= ce -> height;
	   channel. distance		= ce -> distance;
	   channel. azimuth		= ce -> azimuth; 
	}

//	if the list has somehow changed, rewrite it
	if (dxMode) {
	   theDXDisplay. cleanUp ();
	   theDXDisplay. show ();
	   theDXDisplay. setChannel (channel. channelName,
	                             channel. ensembleName);
	   int teller = 0;
	   for (auto &theTr : channel. transmitters) {
	      if (!configHandler_p -> get_allTIISelector ()) {
	         if (theTr. distance < 0) {
	            continue;
	         }
	      }
	      
	      if (bestIndex == teller) {
	         QString labelText = createTIILabel (theTr);
	         distanceLabel	-> setText (labelText);
	      }
	      theDXDisplay. addRow (theTr,
	                             bestIndex == teller);
	      teller ++;
	   }
	}
	else {	// just show on the main widget the strongest
	   for (auto &theTr: channel. transmitters) {
	      if (theTr. distance < 0)
	         continue;
	      if (theTr. isStrongest) {
	         QString labelText = createTIILabel (theTr);
	         distanceLabel	-> setText (labelText);
	         break;
	      }
	   }
	}
//
	if (mapHandler == nullptr)
	   return;
//
	for (auto &theTr : channel. transmitters) {
	   if (theTr. transmitterName == "not in database")
	      continue;

	   uint8_t key = configHandler_p -> showAll_Selector_active () ?
	                                      SHOW_ALL: SHOW_SINGLE;
	   
	   QDateTime theTime = 
	            configHandler_p -> utcSelector_active () ?
	                                     QDateTime::currentDateTimeUtc () :
	                                     QDateTime::currentDateTime ();

	   mapHandler -> putData (key,
	                          theTr,
	                          theTime. toString (Qt::TextDate));
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
	   theNewDisplay. showCorrector (h, l);
}

void	RadioInterface::show_stdDev	(int amount) {
std::vector<float>Values (amount);
	stdDevBuffer. getDataFromBuffer (Values. data (), amount);
	if (!theNewDisplay. isHidden ())
	   theNewDisplay. showStdDev (Values);
}

void	RadioInterface::show_snr		(float snr) {
QPixmap p;

	if (!theNewDisplay. isHidden ())
	   theNewDisplay. showSNR (snr);
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
	theNewDisplay. showQuality (q, sco, freqOffset); 
}
//
//	called from the MP4 decoder
void	RadioInterface::show_rsCorrections	(int c, int ec) {
	if (!running)
	   return;
	if (!techWindow_p -> isHidden ())
	   techWindow_p -> showRsCorrections (c, ec);
}
//
//	called from the ofdm handler
void	RadioInterface::show_clock_error	(int d) {
	if (!running. load ())
	   return;
	if (!theNewDisplay. isHidden ()) {
	   theNewDisplay. showClock_err (d);
	}
}

void	RadioInterface::show_channel	(int n) {
std::vector<Complex> v (n);
	theChannelBuffer. getDataFromBuffer (v. data (), n);
	theChannelBuffer. FlushRingBuffer ();
	if (!theNewDisplay. isHidden () &&
	           (theNewDisplay. getTab () == SHOW_CHANNEL))
	   theNewDisplay. showChannel (v);
}
	                                 
bool	RadioInterface::channelOn () {
	return (!theNewDisplay. isHidden () &&
	           (theNewDisplay. getTab () == SHOW_CHANNEL));
}

bool	RadioInterface::devScopeOn () {
	return !theNewDisplay. isHidden () &&
	           (theNewDisplay. getTab () == SHOW_STDDEV);
}

void	RadioInterface::handle_iqSelector () {
	theOFDMHandler -> handleIQSelector ();
}

void	RadioInterface::showPeakLevel (float iPeakLeft, float iPeakRight) {
	auto peak_avr = [](float iPeak, float & ioPeakAvr) -> void {
	   ioPeakAvr = (iPeak > ioPeakAvr ? iPeak : ioPeakAvr - 0.5f /*decay*/);
	};

	peak_avr (iPeakLeft,  peakLeftDamped);
	peak_avr (iPeakRight, peakRightDamped);

	leftAudio              -> setValue (peakLeftDamped);
	rightAudio             -> setValue (peakRightDamped);
}

void    RadioInterface::handle_presetButton     () {    
	int mode = theEnsembleHandler ->  getShowMode ();
	if (inputDevice_p -> isFileInput ()) {
	   mode = SHOW_ENSEMBLE;
	   presetButton -> setText ("favorites");
	   theEnsembleHandler -> setShowMode (mode);
	   return;
	}
	mode = mode == SHOW_ENSEMBLE ? SHOW_PRESETS : SHOW_ENSEMBLE;
	theEnsembleHandler -> setShowMode (mode);
	presetButton -> setText (mode == SHOW_ENSEMBLE ? "favorites" : "ensemble");
}     

void	RadioInterface::setSoundLabel  (bool f) {
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
void	RadioInterface::handle_backgroundTask (const QString &service) {
audiodata ad;
	int index = theOFDMHandler -> getServiceComp (service);
	if (index < 0)
	   return;
	if (theOFDMHandler -> serviceType (index) != AUDIO_SERVICE)
	   return;
	theOFDMHandler -> audioData (index, ad);
	if (!ad. defined)
	   return;
	
	int teller	= 0;
	for (auto &task: channel. runningTasks) {
	   if (task. serviceName == service) {
	      theOFDMHandler -> stopService (service, ad. subchId, BACK_GROUND);
	      if (task. fd != nullptr)
	         fclose (task. fd);
	      channel. runningTasks. erase
	                        (channel. runningTasks. begin () + teller);
	      return;
	   }
	   teller ++;
	}
	uint8_t audioType	= ad. ASCTy;
	FILE *f = theFilenameFinder. findFrameDump_fileName (service,
	                                                     audioType, true);
	if (f == nullptr)
	   return;

	(void)theOFDMHandler ->
	                   setAudioChannel (ad, &theAudioBuffer, f, BACK_GROUND);
	dabService s;
	s. channel	= ad. channel;
	s. serviceName	= ad. serviceName;
	s. SId		= ad. SId;
	s. subChId	= ad. subchId;
	s. ASCTy	= ad. ASCTy;
	s. fd		= f;
	s. runsBackground	= true;
	channel. runningTasks. push_back (s);
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
	theNewDisplay. showDCOffset (dcOffset);
}

void	RadioInterface::handle_techFrame_closed () {
	store (dabSettings_p, DAB_GENERAL, TECHDATA_VISIBLE, 0);
}

void	RadioInterface::handle_configFrame_closed () {
	store (dabSettings_p, DAB_GENERAL, CONFIG_WIDGET_VISIBLE, 0);
}

void	RadioInterface::handle_deviceFrame_closed () {
	store (dabSettings_p, DAB_GENERAL, DEVICE_WIDGET_VISIBLE, 0);
}

void	RadioInterface::handle_newDisplayFrame_closed () {
	store (dabSettings_p, DAB_GENERAL, NEW_DISPLAY_VISIBLE, 0);
}

void	RadioInterface::setVolume	(int n) {
	audioVolume	= n;
	if (n == 0) 
	   setSoundLabel (false);
	else {
	   if (muteTimer. isActive ()) {
              muteTimer. stop ();
              disconnect (&muteTimer, &QTimer::timeout,
	                  this, &RadioInterface::muteButton_timeOut);
	      stillMuting     -> hide ();
	   }
	   if (channel. audioActive)
	      setSoundLabel (true);
	   ((Qt_Audio *)soundOut_p) -> setVolume (n);
	   store (dabSettings_p, SOUND_HANDLING, QT_AUDIO_VOLUME, audioVolume);
	}
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
	store (dabSettings_p, CONFIG_HANDLER, S_CORRELATION_ORDER, b ? 1 : 0);
	theOFDMHandler -> setCorrelationOrder (b);
}

void	RadioInterface::channelSignal (const QString &channel) {
	stopChannel ();
	channelSelector	-> setEnabled (false);
	int k = channelSelector -> findText (channel);
	if (k != -1) 	
	   channelSelector -> setCurrentIndex (k);
	channelSelector	-> setEnabled (true);
	startChannel (channel);
}

void	RadioInterface::show_changeLabel (const QStringList notInOld,
	                                  const QStringList notInNew) {
	theLogger. log (logger::LOG_CONFIG_CHANGE);
	if (notInOld. size () > 0) {
	   fprintf (stderr, "New service:\n");
	   for (auto &s: notInOld) 
	      fprintf (stderr, "\t%s\n", s. toUtf8 (). data ());
	}
	if (notInNew. size () > 0) {
	   fprintf (stderr, "removed service:\n");
	   for (auto &s: notInNew)
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
	(void)system (x. c_str ());
#endif
}

const char *directionTable [] = {
	"N", "NE", "E", "SE", "S", "SW", "W", "NW", "N"};

QString	fromAzimuth_toDirection (float azimuth) {
int direction	= (azimuth) / 22.5;
	return directionTable [(direction + 1) / 2];
}

QString	RadioInterface::createTIILabel	(const transmitter &theTransmitter) {
	uint8_t mainId		= theTransmitter. mainId;
	uint8_t subId		= theTransmitter. subId;
	const QString & theTransmitterName
	                        = theTransmitter. transmitterName;
	float	theDistance	= theTransmitter. distance;
	float	theAzimuth	= theTransmitter. azimuth;
	QString direction	= fromAzimuth_toDirection (theAzimuth);
//	int	theAltitude	= theTransmitter. altitude;
//	int	theHeight	= theTransmitter. height;
//	float	thePower	= theTransmitter. power;

QString labelText = "(" + QString::number (mainId) + ","
	               + QString::number (subId) + ") ";
	labelText += theTransmitterName;
	labelText += QString ("  ")
	             + "(" + direction + ") "
	             + QString::number (theDistance, 'f', 1) + " km " 
	             + QString::number (theAzimuth, 'f', 1)
	             + QString::fromLatin1 (" \xb0 ");
//	             + "altitude " + QString::number (theAltitude) +  "m "
//	             + "transmitter height " + QString::number (theHeight) +  "m "
//	             + "transmitter power " + QString::number (thePower, 'f', 1) + "kW";
	return labelText;
}

void	RadioInterface::addtoLogFile (const transmitter &theTransmitter) {
FILE	*theFile = nullptr;
bool exists	= false;

	if ((theTransmitter. mainId == 0) ||
	                        (theTransmitter. mainId == 255))
	   return;

	QString fileName = path_for_files + "tii-files.csv";
	theFile = fopen (fileName. toLatin1 (). data (), "r");
	if (theFile != nullptr) {
	   exists = true;
	   fclose (theFile);
	}
	theFile	= fopen (fileName. toLatin1 (). data (), "a");
	if (theFile == nullptr) 
	   return;

	if (!exists) 
	   fprintf (theFile, "date; channel; ensemble; transmitter; coords; mainId; subId;distance (km);azimuth;Power;altitude;height;direction\n\n");

	QDateTime theTime;
	QString tod = theTime. currentDateTime (). toString ();	
//	QString symb	=  QString::fromLatin1 (" \xb0 ");
	QString symb	= "";

	fprintf (theFile, "%s;\"%s\";\"%s\";\"%s\"; \"(%f, %f)\";\"%d\";\"%d\";\"%.1f km\";\"%.1f %s\"  ;\"%.1f kW\";\"%d m\";\"%d m\";\"%s\"\n",
	         tod. toLatin1 (). data (),	// the time
	         theTransmitter. channel. toLatin1 (). data (),
	         theTransmitter. ensemble. toLatin1 (). data (), +
	         theTransmitter. transmitterName. toLatin1 (). data (),
	         theTransmitter. latitude,
	         theTransmitter. longitude,
	         theTransmitter. mainId,
	         theTransmitter. subId,
	         theTransmitter. distance,
	         theTransmitter. azimuth, symb. toLatin1 (). data (),
	         theTransmitter. power,
	         (int)(theTransmitter. altitude),
	         (int)(theTransmitter. height),
	         theTransmitter. direction. toLatin1 (). data ());
	fclose (theFile);
}

void	RadioInterface::handle_tiiButton () {
	dxMode	= !dxMode;
	store (dabSettings_p, CONFIG_HANDLER, S_DX_MODE, dxMode ? 1 : 0);
	theDXDisplay. cleanUp ();
	if (!dxMode) {
	   theDXDisplay. hide ();
	   tiiButton	-> setText ("dx display");
	}
	if (dxMode) {
	   distanceLabel	-> setText ("");
	   theDXDisplay. show ();
	   tiiButton	-> setText ("tii local");
	}
	theOFDMHandler -> setDXMode (dxMode);
}

void	RadioInterface::handle_tiiCollisions     (int b) {
	theOFDMHandler	-> setTIICollisions (b);
}

void	RadioInterface::handle_tiiFilter         (bool b) {
	theOFDMHandler	-> setTIIFilter	(b);
}

void	RadioInterface::process_tiiSelector	(bool b) {
	theOFDMHandler	-> selectTII (b ? 1 : 0);
}

void	RadioInterface::deviceListChanged	() {
#ifndef	TCP_STREAMER
QStringList streams	= ((Qt_Audio *)soundOut_p) -> streams ();
	configHandler_p -> fill_streamTable (streams);
	configHandler_p -> show_streamSelector (true);
#endif
}

//
/////////////////////////////////////////////////////////////////////////
//
//	Handling epg/spi
/////////////////////////////////////////////////////////////////////////

void	RadioInterface::extractServiceInformation (const QDomDocument &doc,
	                                            uint32_t Eid, bool fresh) {
QDomElement root = doc. firstChildElement ("serviceInformation");
	for (QDomElement theElement = root. firstChildElement ("");
	    !theElement. isNull ();
	    theElement = theElement. nextSiblingElement ("")) {
	   if (theElement. tagName () == "ensemble") {
	      bool ok = false;
	      QString Ident = theElement. attribute ("Eid");
	      uint32_t ensemble = Ident. toInt (&ok, 16);
	      if (Eid != ensemble)
	         continue;
	      if (process_ensemble (theElement, Eid) && fresh)
	         saveServiceInfo (doc, Eid);
	   }
	   else
	   if (theElement. tagName () == "service")
	      processService (theElement);
	}
}

void	RadioInterface::saveServiceInfo (const QDomDocument &doc,
	                                                uint32_t Eid) {
QString fileName = path_for_files;
	if (!fileName. endsWith ("/"))
	   fileName =  fileName + "/";
	fileName += QString::number (Eid, 16). toUpper () + "/list.xml";
	QFile file (QDir::toNativeSeparators (fileName));
	if (file. open (QIODevice::WriteOnly | QIODevice::Text)) { 
	   QTextStream stream (&file);
	   stream << doc. toString ();
	   file. close ();
	}
}

bool	RadioInterface::process_ensemble (const QDomElement &node,
	                                              uint32_t Eid) {
int picturesSeen = 0;
	QString Ident = node. attribute ("Eid");
	bool ok = false;
	uint32_t attrib = Ident. toInt (&ok, 16);
	if (!ok || (Eid != attrib))
	   return false;
	QDomElement nameNode = 
	           node. firstChildElement ("mediumName");
	QString ensembleName	= nameNode. text ();
	for (QDomElement service = node. firstChildElement ("service");
	     !service. isNull ();
	     service = service. nextSiblingElement ("service")) {
	    picturesSeen += processService (service);
	}
	return picturesSeen > 0;
}

bool	containsPicture (mmDescriptor &set, multimediaElement &m) {
	for (auto &mm : set. elements) {
	   if ((mm. url == m. url) && (mm. width == m. width))
	      return true;
	}
	return false;
}

int	RadioInterface::processService (const QDomElement &service) {
mmDescriptor pictures;
	uint32_t serviceId =
	             xmlHandler. serviceSid (service);
	pictures. serviceId = serviceId;
	QDomElement mediaDescription =
	            service. firstChildElement ("mediaDescription");
	while (!mediaDescription. isNull ()) {
	   QDomElement multimedia = 
	             mediaDescription. firstChildElement ("multimedia");
	   multimediaElement mE = xmlHandler. extract_multimedia (multimedia);
	   if (mE. valid) {
	      pictures. elements. push_back (mE);
	   }
	
	   mediaDescription = mediaDescription.
	                   nextSiblingElement ("mediaDescription");
	}
	for (auto &pictureElement: channel. servicePictures) {
	   uint32_t serviceId = pictureElement. serviceId;
	   if (pictures. serviceId != serviceId)
	      continue;
	   for (auto &me : pictures. elements) {
	      if (!containsPicture (pictureElement, me)) {
	         pictureElement. elements. push_back (me);
	         break;
	      }
	   }
	}
	channel. servicePictures. push_back (pictures);
	return pictures. elements. size ();
}
//
static
int	mcmp (const void *a, const void *b) {
multimediaElement *aa = (multimediaElement *)a;
multimediaElement *bb = (multimediaElement *)b;
	if (aa -> width < bb -> width)
	   return 1;
	if (aa -> width > bb -> width)
	   return -1;
	return 0;
}
//
//	we want the largest pictures, so we sort the list 
bool	RadioInterface::get_serviceLogo (QPixmap &p, uint32_t SId) {
bool res = false;
	for (auto &ss : channel. servicePictures) {
//	   fprintf (stderr, "comparing %X with %X\n",
//	                          ss. serviceId, SId);
	   if (ss. serviceId != SId)
	      continue;
	   QVector<multimediaElement> options;
	   for (auto &ff: ss. elements)
	      options. push_back (ff);
	   qsort (options. data (), options. size (),
	                 sizeof (multimediaElement), &mcmp);
	   for (auto &ff: options) {
	      QString pict  = path_for_files + QString::number (channel. Eid, 16). toUpper ()+ "/" + ff. url;
	      FILE *tt = fopen (pict. toLatin1 (). data (), "r + b");
	      if (tt == nullptr) 
	         continue;
	      fclose (tt);
	      bool res = p. load (pict, "png");
	      return res;
	   }
	}
	return res;
}
//
void	RadioInterface::read_pictureMappings (uint32_t Eid) {
	QString fileName = path_for_files;
	if (!fileName. endsWith ("/"))
	   fileName += "/";
	fileName += QString::number (Eid, 16). toUpper () + "/list.xml";
	QDomDocument pictureMappings;
	QFile f (fileName);
	if (!f. open (QIODevice::ReadOnly))
	   return;
	pictureMappings. setContent (&f);
	extractServiceInformation	(pictureMappings, Eid, false);
}

void    RadioInterface::lto_ecc (int lto, int ecc) {
	channel. eccByte = ecc; 
	channel. hasEcc = true;
	channel. lto	= lto;
}

void	RadioInterface::setFreqList	() {
	std::vector<int>  freqList =
	       theOFDMHandler -> getFrequency (channel.
	                                       currentService. serviceName);
	if (freqList. size () == 0)
	   return;
	channel. currentService. fmFrequencies = freqList;
	techWindow_p	-> updateFM (freqList);
}

void	RadioInterface::handle_dcRemoval	(bool b) {
	theOFDMHandler	-> set_dcRemoval (b);
	theNewDisplay. set_dcRemoval (b);
}
//
//	Experimental code for handling DL2 data

void	RadioInterface::show_dl2	(uint8_t ct, uint8_t IT,
	                                              const QString &s) {
	if (!configHandler_p -> get_saveTitles ())
	   return;
	dl2_handler theHandler (channel. channelName,
	                        channel. ensembleName,
	                        channel. currentService. serviceName,
	                        dabSettings_p);
	theHandler. show_dl2 (ct, IT, s);
}


/////////////////////////////////////////////////////////////////////////////
//	work in progress
//
void	RadioInterface::nrActiveServices	(int n) {
	configHandler_p -> set_activeServices (n);
}

void	RadioInterface::handle_activeServices () {
QList<contentType> serviceData = theOFDMHandler -> contentPrint ();
bool	serviceAvailable	= false;
	for (auto &ct : serviceData) {
	   if (theOFDMHandler -> serviceRuns (ct. SId, ct. subChId)) {
	      serviceAvailable = true;
//	      fprintf (stderr, "Service %s (%X, %d) runs\n",
//	                      ct. serviceName. toLatin1 (). data (),
//	                      ct. SId, ct. subChId);
	   }
	}
	(void)serviceAvailable;
}

//////////////////////////////////////////////////////////////////
//	handling journaline					//

void	RadioInterface::startJournaline		(int currentKey) {
	if (!journalineHandler. isNull ())
	   return;
	journalineHandler. reset (new journaline_dataHandler ());
	journalineKey		= currentKey;
}

void	RadioInterface::stopJournaline		(int currentKey) {
	if (journalineHandler. isNull ())
	   return;	
	if (journalineKey != currentKey)
	   fprintf (stderr, "What is happenng here %d %d\n",
	                                   journalineKey, currentKey);
	journalineHandler. reset ();
	journalineKey	= -1;
}

void	RadioInterface::journalineData		(QByteArray data,
	                                                 int currentKey) {
	if (journalineHandler. isNull ())
	   return;
	if (currentKey != journalineKey)
	   return;
	std::vector<uint8_t> theMscdata (data. size());
	for (int i = 0; i < data. size (); i ++)
	   theMscdata [i] = data [i];
	journalineHandler	-> add_mscDatagroup (theMscdata);
}


void	RadioInterface::focusInEvent (QFocusEvent *evt) {
	(void)evt;
}
//
//	This function is called whenever a key is touched
//	that is not the return key
//	as it turns out, our "beloved" windows does not let
//	the Qt user catch the functon keys, we settle for Ctrl Ii
bool	RadioInterface::handle_keyEvent (int theKey) {
	if (theKey != Qt::Key_I)
	   return false;

	if (theEnsembleHandler	-> hasFocus ()) {
	   this -> activateWindow ();
	   this -> setFocus ();
	   return true;
	}
	else
	if (this -> hasFocus ()) {
	   configHandler_p -> activateWindow ();
           configHandler_p -> setFocus ();
           return true;
	}
	else 
	if (configHandler_p -> hasFocus ()) {
	   techWindow_p -> activateWindow ();
	   techWindow_p -> setFocus ();
	   return true;
	}
	else
	if (techWindow_p -> hasFocus ()) {
	   theEnsembleHandler	-> activateWindow ();
	   theEnsembleHandler	-> setFocus ();
	   return true;
	}
	return false;
}

