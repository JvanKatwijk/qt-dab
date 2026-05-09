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
#include	<QDesktopServices>
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
#include	"dab-params.h"
#include	"ITU_tables.h"
#include	"coordinates.h"
#include	"mapport.h"
#include	"tech-window.h"
#include	"db-element.h"
#include	"distances.h"
#include	"timetable-control.h"
#include	"position-handler.h"
#include	"settings-handler.h"
#include	"selector.h"
#ifdef	TCP_STREAMER
#include	"tcp-streamer.h"
#else
#include	"Qt-audio.h"
#include	"audiosink.h"
#endif

#include	"device-exceptions.h"
#include	"settingNames.h"
#include	"uploader.h"

#include	"updatechecker.h"
#include	"appversion.h"
#include	"dump-display.h"
#include	<QScreen>
#include	<QDomElement>

#include	"dl2-handler.h"
#include	"basic-print.h"

#include	<filesystem>

#include	"copyrightLabel.h"
#include	"view-handler.h"
namespace	fs= std::filesystem;

#if defined (__MINGW32__) || defined (_WIN32)
#include <windows.h>
__int64 FileTimeToInt64 (FILETIME & ft) {
	ULARGE_INTEGER foo;

	foo.LowPart	= ft.dwLowDateTime;
	foo.HighPart	= ft.dwHighDateTime;
	return (foo.QuadPart);
}

static
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
static
std::vector<size_t> getCpuTimes() {
	std::ifstream proc_stat ("/proc/stat");
	proc_stat. ignore (5, ' ');    // Skip the 'cpu' prefix.
	std::vector<size_t> times;
	for (size_t time; proc_stat >> time; times. push_back (time));
	return times;
}

static
bool getCpuTimes (size_t &idle_time, size_t &total_time) {
	const std::vector <size_t> cpu_times = getCpuTimes();
	if (cpu_times. size() < 4)
	   return false;
	idle_time  = cpu_times [3];
	total_time = std::accumulate (cpu_times. begin(),
	                            cpu_times. end(), static_cast<size_t>(0));
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
	                                const QString	&dbFile,
	                                const QString	&freqExtension,
	                                const QString	&schedule,
	                                const QString	&tiiFile,
	                                bool		is_error_report,
	                                int32_t		dataPort,
	                                int32_t		clockPort,
	                                int		fmFreq,
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
	                                      theErrorLogger (Si),
	                                      theDeviceChooser (&theErrorLogger, Si),
	                                      theDXDisplay (this, Si),
	                                      theLogger	(Si),
	                                      theSCANHandler (this, Si,
	                                                      freqExtension),
	                                      theTIIProcessor (),
	                                      theEpgCompiler (&theErrorLogger) {
int16_t k;
QString h;

	theQSettings			= Si;
	this	-> error_report		= is_error_report;
	this	-> fmFrequency		= fmFreq;
	this	-> dlTextFile		= nullptr;
	this	-> thecopyrightLabel	= nullptr;
	running. 		store (false);
	stereoSetting			= false;
	theContentTable			= nullptr;
	theScanTable			= nullptr;
	mapViewer			= nullptr;
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
	          value_i (theQSettings, DAB_GENERAL, "dabMode", 1);
	globals. threshold		=
	          value_i (theQSettings, DAB_GENERAL, "threshold", 3);
	globals. diff_length	=
	          value_i (theQSettings, DAB_GENERAL, "diff_length", DIFF_LENGTH);
	globals. tii_delay   =
	          value_i (theQSettings, DAB_GENERAL, "tii_delay", 3);
	if (globals. tii_delay < 3)
	   globals. tii_delay	= 3;
	globals. tii_depth      =
	          value_i (theQSettings, DAB_GENERAL, "tii_depth", 4);
	globals. echo_depth     =
	          value_i (theQSettings, DAB_GENERAL, "echo_depth", 1);

#ifdef	_SEND_DATAGRAM_
	ipAddress	=  value_s (theQSettings, "DATAGRAM",
	                              "ipAddress", "127.0.0.1");
	port		=  value_i (theQSettings, "DATAGRAM",
	                              "port" 8888);
#endif
//	set on top or not? checked at start up
	if (value_i (theQSettings, DAB_GENERAL, "onTop", 0) == 1) 
	   setWindowFlags (windowFlags () | Qt::WindowStaysOnTopHint);
//
//	The settings are done, now creation of the GUI parts
	setupUi (this);

//	and init the up and down button, the select for details button ans
//	the button to show the directory with files 
//	signal strength icons
	QPixmap p;
	for (int i = 0; i < 4; i ++) {
	   QString labelName	=
	        QString (":res/radio-pictures/signal%1.png"). arg (i, 1, 10, QChar ('0'));
	   p. load (labelName, "png");
	   strengthLabels. push_back (p);
	}
	if (p. load (":res/radio-pictures/resetButton.png", "png")) {
	   resetSelectorLabel	-> setPixmap (p. scaled (30, 30,
	                                         Qt::KeepAspectRatio));
	   resetSelectorLabel	-> setToolTip ("touching this will reset the system");
	}
	if (p. load (":res/radio-pictures/details24.png", "png")) {
	   serviceButton -> setPixmap (p. scaled (30, 30,
	                                       Qt::KeepAspectRatio));
	   serviceButton -> setToolTip ("touching this controls the visibility of the technical details window");
	}
	if (p. load (":res/radio-pictures/folder_button.png", "png")) {
	   folder_shower -> setPixmap (p. scaled (30,
	                                         30, Qt::KeepAspectRatio));
	   folder_shower -> setToolTip ("touhcing this tells the software to start a shell with the Qt-DAB-files directory");
	}
	if (p. load (":res/radio-pictures/epgLabel.png", "png")) {
	   epgLabel	-> setPixmap (p. scaled (30, 30,
	                                         Qt::KeepAspectRatio));
	   epgLabel	-> setToolTip ("this icon is visible when the EPG processor is active,  the service will always run in the background");
	}
	if (p. load (":res/radio-pictures/deviceSelection.png", "png")) {
	   deviceSelectorLabel	-> setPixmap (p. scaled (30, 30,
	                                         Qt::KeepAspectRatio));
	   deviceSelectorLabel	-> setToolTip ("this icon controls the visibility of the device selection list");
	}

	connect (folder_shower, &clickablelabel::clicked_left,
	         this, &RadioInterface::handle_folderButton);
	dxMode     = value_i (theQSettings, CONFIG_HANDLER, S_DX_MODE, 0) != 0;
	connect (distanceLabel, &clickablelabel::clicked_left,
	         this, &RadioInterface::handle_distanceLabel);

	labelStyle	= value_s (theQSettings, DAB_GENERAL, LABEL_COLOR,
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
	stillMuting		-> hide ();
	volumeSlider		-> hide ();
	pauzeSlideTeller	= 0; // counting pause slides

	connect (deviceSelectorLabel, &clickablelabel::clicked_left,
	         this, &RadioInterface::devSL_visibility);
	connect (resetSelectorLabel, &clickablelabel::clicked_left,
	         this, &RadioInterface::handle_resetButton);

	aboutLabel -> setText (QString ("©") + VERSION);
	aboutLabel -> setToolTip ("Click to see the acknowledgements");
	connect (aboutLabel, &clickablelabel::clicked_left,
	         this, &RadioInterface::handle_copyrightLabel);

	pauzeTimer. setSingleShot (true);
	connect (&pauzeTimer, &QTimer::timeout,
	         this, &RadioInterface::show_pauzeSlide);

	QPixmap epgP;
	connect (epgLabel, &clickablelabel::clicked_left,
	         this, &RadioInterface::handle_startTimeTable);
	epgLabel	-> hide ();
#if QT_VERSION > QT_VERSION_CHECK (6, 0, 0)
	version		= QString ("Qt6-DAB-") + VERSION ;
#else
	version		= QString ("Qt5-DAB-") + VERSION;
#endif
	if (!QString (GITHASH). contains ("----"))
	   version = version + " (" + QString (GITHASH) + ")";
	setWindowTitle (version);
//	put the widgets in the right place and create the workers
	setPositionAndSize	(theQSettings, this, S_MAIN_WIDGET);

	theConfigHandler		= new configHandler (this,
	                                                 theQSettings);
//	we have the configuration handler and the ensemble handler,
//	connect some signals directly
	theConfigHandler		-> set_activeServices (0);
	theConfigHandler		-> set_connections ();

	connect (theConfigHandler, &configHandler::frameClosed,
	         this, &RadioInterface::handle_configFrame_closed);
	connect (theConfigHandler, &configHandler::handle_fontSelect,
	         this, &RadioInterface::handleFontSelect);
	connect (theConfigHandler, &configHandler::handle_fontSizeSelect,
	         this, &RadioInterface::handleFontSizeSelect);
	connect (theConfigHandler, &configHandler::handle_fontColorSelect,
	         this, &RadioInterface::handleFontColorSelect);
	connect (theConfigHandler, &configHandler::signal_dataTracer,
	         this, &RadioInterface::signal_dataTracer);

	connect (&theNewDisplay, &displayWidget::frameClosed,
	         this, &RadioInterface::handle_newDisplayFrame_closed);
	{  QPalette p	= theNewDisplay. ficError_display -> palette();
	   p. setColor (QPalette::Highlight, Qt::red);
	   theNewDisplay. ficError_display	-> setPalette (p);
	   p. setColor (QPalette::Highlight, Qt::green);
	}
	connect (&theNewDisplay, &displayWidget::mouseClick,
	         this, &RadioInterface::handle_iqSelector);
//
//	we try to load the db from the user's home dir
	if (std::filesystem::exists (tiiFile. toLatin1 (). data ())) {
	   theConfigHandler -> enable_loadLib ();
	   theTIIProcessor. reload (tiiFile);
	}
//	If no database in the homedirectory can be found,
//	we load the "default" version
	if (!theTIIProcessor. has_tiiFile ()) {
	   theTIIProcessor. reload (":res/txdata.tii");
	}

	if (!theTIIProcessor. has_tiiFile ())	// should not happen
	   httpButton	-> setEnabled (false);

///////////////////////////////////////////////////////////////////////
	theTechWindow	= new techWindow (this, theQSettings, &theTechData);
	connect (theTechWindow, &techWindow::frameClosed,
	         this, &RadioInterface::handle_techFrame_closed);
	theTechWindow 		-> hide ();	// until shown otherwise

	if (value_i (theQSettings, DAB_GENERAL, NEW_DISPLAY_VISIBLE, 0) != 0)
	   theNewDisplay. show ();
	else
	   theNewDisplay. hide ();

	channel. etiActive	= false;
	channel. cleanChannel ();
	localPos. latitude 		=
	             value_f (theQSettings, MAP_HANDLING, HOME_LATITUDE, 0.0);
	localPos. longitude 		=
	             value_f (theQSettings, MAP_HANDLING, HOME_LONGITUDE, 0.0);
	if (localPos. latitude == 0)
	   localPos. latitude = 52.22;
	if (localPos. longitude == 0)
	   localPos. longitude = 4.54;

#ifdef	DATA_STREAMER
	theDataStreamer		= new tcpServer (dataPort);
#else
	(void)dataPort;
#endif
#ifdef	CLOCK_STREAMER
	theClockStreamer		= new tcpServer (clockPort);
#else
	(void)clockPort;
#endif
//	Where do we leave the audio out?
	theConfigHandler	-> show_streamSelector (false);
	int latency	= value_i (theQSettings, SOUND_HANDLING, "latency", 5);
	theAudioPlayer		= nullptr;
//
//	If we do not have a TCP streamer, we go for either the
//	portaudio and the Qt_audio alternatives.
//	Default - and if Qt_Audio fails, we go for Portaudio
#ifndef	TCP_STREAMER
	QStringList streams;
	QString	temp;
//
	QString sound = value_s (theQSettings, SOUND_HANDLING,
	                                       SOUND_HANDLER, S_QT_AUDIO);
	if (sound != S_PORT_AUDIO) {	// try Qt_Audio
	   try {
	      theAudioPlayer	= new Qt_Audio (this, theQSettings);
	      streams		= static_cast<Qt_Audio *>(theAudioPlayer) -> streams ();
	      temp		=
	          value_s (theQSettings, SOUND_HANDLING,
	                                  AUDIO_STREAM_NAME, "default");
	      volumeSlider	-> show ();
	      audioVolume	=
	          value_i (theQSettings, SOUND_HANDLING, QT_AUDIO_VOLUME, 50);
	      volumeSlider		-> setValue (audioVolume);
	      static_cast<Qt_Audio *>(theAudioPlayer)	-> setVolume (audioVolume);
	      connect (volumeSlider, &QSlider::valueChanged,
	               this, &RadioInterface::setVolume);
	   } catch (...) {
	      theAudioPlayer = nullptr;
	   }
	}

//	we end up here if selection was PORT_AUDIO or using Qt_Audio failed
//	as it does on U20
	if (theAudioPlayer == nullptr) {
	   theAudioPlayer	= new audioSink	(latency);
	   streams	= static_cast<audioSink *>(theAudioPlayer) -> streams ();
	   temp		=
	          value_s (theQSettings, SOUND_HANDLING,
	                                 AUDIO_STREAM_NAME, "default");
	}

	if (streams. size () > 0) {
	   theConfigHandler -> fill_streamTable (streams);
	   theConfigHandler -> show_streamSelector (true);
	   k	= theConfigHandler -> init_streamTable (temp);
	   if (k >= 0) {
	      QString str = theConfigHandler -> currentStream ();
	      theAudioPlayer -> selectDevice (k, str);
	   }
	   theConfigHandler	-> connect_streamTable	();
	}
	else {
	   delete theAudioPlayer;
	   theAudioPlayer = new audioPlayer ();
	}

	if (!theAudioPlayer -> hasMissed ())
	   theTechWindow -> hideMissed ();
#else
	theAudioPlayer		= new tcpStreamer	(20040);
	theTechWindow		-> hide		();
#endif
//
//	some MOT, text and other data is stored in the Qt-DAB-files directory
	path_for_files	= theFilenameFinder. basicPath ();	
	path_for_files	= checkDir (path_for_files);
	path_for_files	= QDir::toNativeSeparators (path_for_files);
	path_for_serviceLists = path_for_files + "/serviceLists/";
	path_for_serviceLists = checkDir (path_for_serviceLists);
	path_for_serviceLists = QDir::toNativeSeparators (path_for_serviceLists);
	path_for_epg	= path_for_files + "EpgData/";
	path_for_epg	= checkDir (path_for_epg);
	path_for_epg	= QDir::toNativeSeparators (path_for_epg);

//	extract the channelnames and fill the combobox
	QStringList channelNames	= theSCANHandler. getChannelNames ();
	this -> newServices	= new serviceViewer (dbFile, this,
	                                             channelNames,
	                                             theQSettings,
	                                                the_newFrame);
	connect (theConfigHandler, &configHandler::set_serviceOrder,
	         newServices, &serviceViewer::setServiceOrder);

	audioDumping		= false;
	theControl		= nullptr;
	journalineHandler 	= nullptr;
	peakLeftDamped          = -100;
	peakRightDamped         = -100;
	audioTeller		= 0;	// counting audio frames
	dumpDisplay_p		= nullptr;
	previous_idle_time	= 0;
	previous_total_time	= 0; 
	numberofSeconds		= 0;
	crcLabel	-> setStyleSheet ("color : green");

//	Connect the buttons for the color_settings
	connect (configButton, &smallPushButton::rightClicked,
	         this, &RadioInterface::color_configButton);
	connect (spectrumButton, &smallPushButton::rightClicked,
	         this, &RadioInterface::color_spectrumButton);
	connect (serviceButton, &clickablelabel::clicked_left,
	         this, &RadioInterface::handle_detailButton);
	connect (httpButton, &smallPushButton::rightClicked,
	         this, &RadioInterface::color_httpButton);
	connect	(scanButton, &smallPushButton::rightClicked,
	         this, &RadioInterface::color_scanButton);
//	
	connect (soundLabel, &clickablelabel::clicked_left,
	         this, &RadioInterface::handle_muteButton);
	connect (snrLabel, &clickablelabel::clicked_left,
	         this, &RadioInterface::handle_snrLabel);

	show_pauzeSlide ();

//	and start the timer(s)
//	The displaytimer is there to show the number of
//	seconds running and handle - if available - the tii data
	displayTimer. setInterval (1000);
	connect (&displayTimer, &QTimer::timeout,
	         this, &RadioInterface::updateTimeDisplay);
	displayTimer. start (1000);

//	timer for scanning
	channelTimer. setSingleShot (true);
	channelTimer. setInterval (10000);
	connect (&channelTimer, &QTimer::timeout,
	         this, &RadioInterface::channel_timeOut);

//	presetTimer
	presetTimer. setSingleShot (true);
	connect (&presetTimer, &QTimer::timeout,
	         this, &RadioInterface::setPresetService);
//
//	timer for muting
	muteTimer. setSingleShot (true);
	set_Colors ();
//
	leftAudio	-> setBorderWidth	(0);
	leftAudio	-> setScalePosition	(QwtThermo::NoScale);
	rightAudio	-> setBorderWidth	(0);
	rightAudio	-> setScalePosition	(QwtThermo::NoScale);
	leftAudio	-> setValue (-35);
	rightAudio	-> setValue (-35);
	QwtLinearColorMap * mapLeft = new QwtLinearColorMap ();
	mapLeft	-> setColorInterval (QColor (0, 50, 200), QColor(255, 0, 0));
	mapLeft -> addColorStop (0.75, QColor (100, 50, 0)); 
	mapLeft -> addColorStop (0.4, QColor (0, 200, 50)); 
	leftAudio -> setColorMap (mapLeft);
	QwtLinearColorMap * mapRight = new QwtLinearColorMap ();
	mapRight -> setColorInterval (QColor(0, 50, 200), QColor(255, 0, 0));
	mapRight -> addColorStop (0.75, QColor (100, 50, 0)); 
	mapRight -> addColorStop (0.4, QColor (0, 200, 50)); 
	rightAudio -> setColorMap (mapRight);
//
	audiorateLabel	-> setStyleSheet ("color:cyan");
	psLabel		-> setStyleSheet ("color:cyan"); 
	sbrLabel	-> setStyleSheet ("color:cyan");

	journalineKey		= -1;
//	do we show controls?
	bool visible	=
	            value_i (theQSettings, DAB_GENERAL, 
	                                     CONFIG_WIDGET_VISIBLE, 0) != 0;
	if (visible) {
	   theConfigHandler	-> show ();
	}
	connect (configButton, &QPushButton::clicked,
	         this, &RadioInterface::handle_configButton);

	if (value_i (theQSettings, DAB_GENERAL, SNR_WIDGET_VISIBLE, 0) != 0)
	   theSNRViewer. show ();
	else
	   theSNRViewer. hide ();
	if (value_i (theQSettings, DAB_GENERAL, TECHDATA_VISIBLE, 0) != 0)
	   theTechWindow -> show ();

	dynamicLabel	-> setFont (QFont ("Times", 12, -1, false));
	dynamicLabel	-> setAlignment(Qt::AlignCenter);
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
	           value_i (theQSettings, DAB_GENERAL, "EXISTS", 0) != 0;
	if (!iniExists) {
	   store (theQSettings, DAB_GENERAL, "EXISTS", 1);
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("The ini file is new and no home location is known yet"));
	}

	theDeviceHandler	= nullptr;
//	do we have a known device from previous invocations?
	h               =
	           value_s (theQSettings, DAB_GENERAL, 
	                                      SELECTED_DEVICE, "no device");

	if (theDeviceChooser. getDeviceIndex (h) >= 0)
	   theDeviceHandler	= createDevice (h, &theLogger);
	
	if (theDeviceHandler != nullptr) {
	   connect (&theDeviceChooser, &deviceChooser::deviceSelected,
	            this, &RadioInterface::newDevice);
	   theDeviceChooser. hide ();
	   startDirect ();
	   qApp	-> installEventFilter (this);
	   return;
	}

//	What we want here is that we start up after a device
//	is selected
	connect (&theDeviceChooser, &deviceChooser::deviceSelected,
	         this, &RadioInterface::doStart);
	theDeviceChooser. show ();
	qApp	-> installEventFilter (this);
}
//
//	doStart (QString) is called when - on startup - NO device
//	was registered to be used, and the user presses the
//	selectDevice comboBox
void	RadioInterface::doStart (const QString &dev) {
	if (theDeviceHandler != nullptr)
	   delete theDeviceHandler;
	theDeviceHandler	= createDevice	(dev, &theLogger);
//	Some buttons should not be touched before we have a device
	if (theDeviceHandler == nullptr) {
	   return;
	}

	disconnect (&theDeviceChooser, &deviceChooser::deviceSelected,
	            this, &RadioInterface::doStart);
	connect (&theDeviceChooser, &deviceChooser::deviceSelected,
	         this, &RadioInterface::newDevice);
	theDeviceChooser. hide ();
	startDirect ();
}
//
//	we (re)start a device, if it happens to be a regular
////	device, check for a preset name
void	RadioInterface::startDirect	() {

	cpuLabel -> setText (VITERBI);

	theOfdmHandler	= new ofdmHandler  (this,
	                                    theDeviceHandler,
	                                    &globals, theQSettings,
	                                    &theLogger, this -> cpuSupport);
	if (theOfdmHandler == nullptr) {
	   QMessageBox::warning (this, tr ("Warning"),
                                       tr ("Fatal error, call expert 11"));
	   abort ();
	}

	theOfdmHandler	-> set_dcRemoval (true);
	theNewDisplay. set_dcRemoval (true);
	channel. cleanChannel ();
//
	if (theDeviceHandler -> isFileInput ())
	   newServices	-> startMode (FILEINPUT, 
	                                   get_serviceOrder ());
	else 
	if (!theConfigHandler -> get_loadSelection ()) 
	   newServices -> startMode (ENSEMBLEVIEW, get_serviceOrder ());
	else {
	   bool resetFlag = false;
//	Basically, the choices here are
//	1. open an existing file and use that'
//	2. create a new file and name it;
//	3. start with a fresh default input file
//	4. this was a mistake, just start with the default file
	   startList	(resetFlag);	// he does the querying
	   if (resetFlag)
	      theConfigHandler -> reset_loadSelection ();
	}
//
	startChannel (newServices -> currentChannel ());
	int auto_http	= value_i (theQSettings, CONFIG_HANDLER, AUTO_HTTP, 0);
	if ((auto_http != 0) && (localPos. latitude != 0)) {
	   bool succ = autoStart_http ();
	   if (succ)
	      httpButton	-> setText ("http-on");
	}

	bool do_updateCheck	=
	       value_i (theQSettings, CONFIG_HANDLER, DO_UPDATECHECK, 1) != 0;
	if (do_updateCheck) {
	   updateCheck_timer. setSingleShot (true);
	   connect (&updateCheck_timer, &QTimer::timeout,
	            this, &RadioInterface::check_newVersion);
	   updateCheck_timer. start (10000);
	}
	running. store (true);
}
//
//	It took a while but now we are running
	RadioInterface::~RadioInterface () {
	fprintf (stderr, "radioInterface is deleted\n");
}
//	
//	no_signal_found is called when scanning is on by the
//	ofdm handler
void	RadioInterface::no_signal_found () {
	channel_timeOut ();
}
//
//
//	There are three FIG types that are used to define 
//	servicelabels, we create an entryfor each of them
//	FIG1/1 is for 16 bit primary services
void	RadioInterface::handle_FIG11 (const QString &serviceName,
	                                              uint32_t SId) {
	(void)serviceName;
	if (!running. load())
	   return;

	audiodata ad;
	theOfdmHandler	-> audioData (SId, 0, ad);
	if (!ad. defined)
	   return;		// should not happen

	newServices	-> addService (ad);

	if (theSCANHandler. active ())
	   return;
	
	channel. nrServices ++;
	int servCount	= theOfdmHandler -> FIG07_value ();
	if ((servCount > 0) && (channel. nrServices >= servCount))
	   setPresetService ();
}
//
//	FIG1/5 is for labels for primary data services
void	RadioInterface::handle_FIG15 (const QString &serviceName,
	                                             uint32_t SId) {
	(void)serviceName;
	if (!running. load())
	   return;

	packetdata pd;
	theOfdmHandler	-> packetData (SId, 0, pd);
	if (!pd. defined)
	   return;		// should not happen

	if (!theConfigHandler -> get_audioServices_only ())
	   newServices	-> addService (pd);
	   
	if ((!theSCANHandler. active ()) &&
	    (theOfdmHandler	-> is_SPI (SId))) 
	   start_epgService (pd);

	channel. nrServices ++;
	int servCount	= theOfdmHandler -> FIG07_value ();
	if ((servCount > 0) && (channel. nrServices >= servCount))
	   setPresetService ();
}
//
//	FIG1/4 is for labels for secondary components,
//	we assume here data components
//
void	RadioInterface::handle_FIG14 (const QString &serviceName,
	                                        uint32_t SId, uint8_t SCIds) {
	(void)serviceName;
	if (!running. load())
	   return;

	packetdata pd;
	theOfdmHandler	-> packetData (SId, SCIds, pd);
	if (!pd. defined)
	   return;		// should not happen

	if (!theConfigHandler -> get_audioServices_only ())
	   newServices	-> addService (pd);
	   
	channel. nrServices ++;
	int servCount	= theOfdmHandler -> FIG07_value ();
	if ((servCount > 0) && (channel. nrServices >= servCount))
	   setPresetService ();
}
//	Some, by far not all, ensembles are transmitted with a
//	a slot, called by the fib processor
//	on recognition of an ensemblename
void	RadioInterface::handle_FIG10 (const QString &v, uint16_t EId) {
	if (!running. load())
	   return;
	newServices		-> set_ensembleId (v, EId);
	channel. ensembleName	= v;
	channel. Eid		= EId;
	dynamicLabel		-> setText ("");
//
	if (channel. hasEcc) {
	   QString country      = get_ITU_Code (channel. internatTable,
                                                channel. eccByte,
                                                (channel. Eid >> 12) &0xF);
           channel. countryName = country;
           newServices          -> set_countryName (country);
	}

        if (!theSCANHandler. active ()) 
           read_pictureMappings (static_cast<uint32_t>(EId));
	else
	if (theSCANHandler. active ()) {
	   if (theSCANHandler. scan_to_data ()) {
	      stopScanning ();
	      return;
	   }
	   theSCANHandler.
	                 addEnsemble (newServices -> currentChannel (), v);
	   channelTimer. stop ();
	   int switchStay		= 
	           theSCANHandler. switchStayValue ();
	   channelTimer. start (switchStay);
	}
}
//
///////////////////////////////////////////////////////////////////////////

void	RadioInterface::handle_contentButton	() {

	if (theContentTable != nullptr) {
	   theContentTable -> hide ();
	   delete theContentTable;
	   theContentTable = nullptr;
	   return;
	}

	basicPrint thePrinter (channel. internatTable,
	                                 channel. Eid, channel. eccByte);
	QStringList contentList	=
	            thePrinter. print (theOfdmHandler -> contentPrint ());
	QString headLine	= build_kop ();
	theContentTable		= new contentTable (this, theQSettings,
	                                            channel. channelName,
	                                            thePrinter. scanWidth ());
	connect (theContentTable, &contentTable::goService,
	         this, &RadioInterface::handle_contentSelector);

	theContentTable		-> addLine (headLine);
	for (auto &tr : channel. transmitters) {
	   QString transmitterLine = build_transmitterLine (tr);
	   theContentTable	-> addLine (transmitterLine);
	}
  
	for (auto &line : contentList)
	   theContentTable -> addLine (line);
	theContentTable -> show ();

	if (theConfigHandler -> upload_selector_active ()) {
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
	                               theContentTable -> upload ());
	   } catch (...) {}
	}
}

//
//	to avoid confusion, each MOT object arriving here carries
//	the SId of the service it belongs to.
//	Data for background services other than the SPI servides,
//	is basically ignored.
void	RadioInterface::handle_motObject (QByteArray	result,
	                                  QString	objectName,
	                                  int		contentType,
	                                  bool		dirElement,
	                                  uint32_t	SId) {

	if (!theOfdmHandler	-> is_SPI (SId) &&
	   (channel. currentService. SId != SId))
	   return;

	switch (getContentBaseType (static_cast<MOTContentType>(contentType))) {
	   default:		// should not happen
	   case MOTBaseTypeGeneralData:
	      break;

	   case MOTBaseTypeText:
	      saveMOTtext (result, contentType, objectName);
	      break;

	   case MOTBaseTypeImage: 
	      showMOTlabel (result, contentType,
	                       objectName, dirElement, SId);
	      break;

	   case MOTBaseTypeAudio:
	      break;

	   case MOTBaseTypeVideo:
	      break;

	   case MOTBaseTypeTransport:
	      saveMOTObject (result, contentType, objectName);
	      break;

	   case MOTBaseTypeSystem:
	      break;

	   case  MOTBaseTypeApplication: { 	// epg data
	      if (theSCANHandler. active ())
	         return;
	      if (objectName == QString (""))
	         objectName	= "epgFile";	// should not happen
	      process_epgData (objectName, result);
	   }
	   return;

	   case MOTBaseTypeProprietary:
	      break;
	}
}

void	RadioInterface::process_epgData (const QString &objectName,	
	                                 const QByteArray &result) {
std::vector<uint8_t> epgData (result. begin(), result. end());
QDomDocument epgDocument;
uint8_t docType	= theEpgCompiler. process_epg (epgDocument,
	                                        epgData, channel.lto);
	if (docType == noType)		// should not happen
	   return;

	if (docType == serviceInformationType) {
	   extractServiceInformation (epgDocument, channel. Eid, true);
	   return;
	}

	QString theName = extractName (objectName);
	if (theName == "") 
	   return;
	
	QString temp = path_for_epg +
	                   QString::number (channel. Eid, 16). toUpper () + "/";
	temp = QDir::toNativeSeparators (temp);
	theName  = temp + theName;
	if (!QDir (temp). exists ())
	   QDir (). mkpath (temp);	

	QFile file (QDir::toNativeSeparators (theName));
	if (file. open (QIODevice::WriteOnly | QIODevice::Text)) { 
	   QTextStream stream (&file);
	   stream << epgDocument. toString ();
	   file. close ();
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
	   uint32_t sid = sidString. toUInt (&ok, 16);
	   if (ok) {
	      QString name = newServices -> extractName (sid);
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
	QString path = path_for_epg +
	                        QString::number (channel. Eid, 16). toUpper ();
	path	+= "/";
	path	= QDir::toNativeSeparators (path);
	path	= checkDir (path);
	QString textName = path + name;

	FILE *x = fopen (textName. toUtf8 (). data (), "w+b");
	if (x == nullptr) {
	   QString t = QString ("problem to open file ") + textName;
	   theErrorLogger. add ("main", t);
	}
	else {
	   (void)fwrite (result. data (), 1,
	                        static_cast<size_t>(result.length ()), x);
	   fclose (x);
	}
}

void	RadioInterface::saveMOTObject (QByteArray  &result,
	                               int contentType,
	                               QString  &name) {
	if (name == "") {	// should not happen
	   static int counter	= 0;
	   name = "motObject_" + QString::number (counter);
	   counter ++;
	}
	saveMOTtext (result, contentType, name);
}

//	MOT slide, to show
void	RadioInterface::showMOTlabel	(QByteArray	&motData,
	                                 int		contentType,
	                                 const QString  &pictureName,
	                                 int		dirs,
	                                 uint32_t	SId) {
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

	if (theOfdmHandler	-> is_SPI (SId)) {
	   QString path		= slidePath (true, SId, pictureName);
	   FILE *x = fopen (path. toUtf8 (). data (), "w+b");
	   if (x != nullptr) {
	      theLogger. log (logger::LOG_SLIDE_WRITTEN, pictureName);
	      (void)fwrite (motData. data (), 1,
	                          static_cast<size_t>(motData.length ()), x);
	      fclose (x);
	   }
	   return;
	}

	if (channel. currentService. SId != SId)  // current service
	   return;		// cannot happen
//
//	handle slides for current service
	if (dirs || ((value_i (theQSettings, CONFIG_HANDLER,
	                           SAVE_SLIDES_SETTING, 0) != 0) &&
	                                         (path_for_files != ""))) {
	   QString thePath	= slidePath (dirs, SId, pictureName);
	   QString pict		= thePath + pictureName;
	   FILE *x = fopen (pict. toUtf8 (). data (), "w+b");
	   if (x != nullptr) {
	      theLogger. log (logger::LOG_SLIDE_WRITTEN, pict);
	      (void)fwrite (motData. data(), 1,
	                          static_cast<size_t>(motData.length ()), x);
	      fclose (x);
	   }
	   else {
	      QString t = QString ("Problem opening picture file (writing) ") +
	                                    pict;
	      theErrorLogger. add ("main", t);
	   }
	}

	if (dirs)
	   return;

	QPixmap p;
	if (p. loadFromData (motData, type))
	   displaySlide (p);
}
//
//	SPI files are stored in a directory, with as name the EId
//	of the ensemble. Regular slides are saved (obviously if
//	the saving is set in the configuration window) in a
//	directory with the name of the service, that is contained
//	in a directory with the word "slides-" followed by the Eid
QString	RadioInterface::slidePath	(bool kort, uint32_t SId,
	                                       const QString &pictureName) {
QString thePath;

	(void)SId;
	QString theEId = QString::number (channel. Eid, 16). toUpper ();
	
	if (kort)
	   thePath = path_for_epg + theEId + "/";
	else {
	   thePath = path_for_files + "slides-" +"(" + theEId + ")" +  "/";
	   thePath +=  channel. currentService. serviceName. trimmed () + "/";
	}
	thePath	= QDir::toNativeSeparators (thePath);
	if (!QDir (thePath). exists ())
	   QDir (). mkpath (thePath);	
	return thePath + pictureName;
}
//
//	sendDatagram is triggered by the ip handler,
void	RadioInterface::sendDatagram	(uint32_t length) {
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
void	RadioInterface::handle_tdcdata (int frametype, uint32_t length) {
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
	   theDataStreamer -> sendData (localBuffer, length + 8);
#endif
}
/**
  *	Response to a signal, so we presume that the signaling body exists
  *	signal may be pending though
  *	we copy the tasklist and stop all services,
  *	then, using the data in the copy we (try to) start
  *	all services again (including the secondary services);
  */

void	RadioInterface::handle_FIG00 () {
std::vector<dabService> taskCopy = channel. runningTasks;

	if (theSCANHandler. active ()) {
	   stopScanning ();
	   return;
	}
	channelTimer. stop ();
	stopMuting	();
	setSoundLabel (false);
	if (channel. etiActive)
	   theOfdmHandler -> resetEtiGenerator ();
//	and stop the service
	for (auto &serv :channel. runningTasks) 
	   stopService (serv);
	fprintf (stderr, "All services are halted, now start rebuilding\n");
	theTechWindow	-> cleanUp ();
	for (auto &serv : taskCopy) {
	   dabService s;
	   theOfdmHandler -> mapNameToId (serv. serviceName, s. SId, s. SCIds);
	   if (s. SId == 0) {
	      newServices -> remove (channel. channelName,
	                                         serv. serviceName);
	   }
	}
}
//
//	In order to not overload with an enormous amount of
//	signals, we trigger this function at most 10 times a second
void	RadioInterface::newAudio	(uint32_t amount, int rate,
	                                          bool ps, bool sbr) {
	if (!running. load () || !channel. audioActive)
	   return;
	audioTeller ++;
	if (audioTeller > 20) {
	   audioTeller = 0;
	   if (!theTechWindow -> isHidden ())
	      theTechWindow	-> showRate (rate, ps, sbr);
	   audiorateLabel	-> setText (QString::number (rate));
	   psLabel	-> setText (ps ? "ps" : " ");
	   sbrLabel	-> setText (sbr ? "sbr" : " ");
	}
//
//	processing the audio
	std::complex<int16_t> *vec = dynVec (std::complex<int16_t>, amount);
	while (theAudioBuffer. GetRingBufferReadAvailable () >= amount) {
	   theAudioBuffer. getDataFromBuffer (vec, amount);
	   if (!theTechWindow -> isHidden ()) {
	      theTechData. putDataIntoBuffer (vec, amount);
	      theTechWindow	-> audioDataAvailable (amount, rate);
	   }
#ifdef	HAVE_PLUTO_RXTX
	   if (theDabStreamer != nullptr)
	      theDabStreamer	-> audioOut (vec, amount, rate);
#endif
	   std::vector<float> tmpBuffer;
	   int size = theAudioConverter. convert (vec, amount,
	                                               rate, tmpBuffer);
	   if (!muteTimer. isActive ())
	      theAudioPlayer -> audioOutput (tmpBuffer. data (), size);
	   setPeakLevel (tmpBuffer);
	}
}

void	RadioInterface::setPeakLevel (const std::vector<float> &samples) {
float	absPeakLeft	= 0;
float	absPeakRight	= 0;
static int teller = 0;
	if (++teller < 4)
	   return;
	teller = 0;
	for (uint32_t i = 0; i < samples. size () / 2; i ++) {
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

void	RadioInterface::showPeakLevel (float iPeakLeft, float iPeakRight) {
	auto peak_avr = [](float iPeak, float & ioPeakAvr) -> void {
	   ioPeakAvr = (iPeak > ioPeakAvr ? iPeak : ioPeakAvr - 0.5f /*decay*/);
	};

	peak_avr (iPeakLeft,  peakLeftDamped);
	peak_avr (iPeakRight, peakRightDamped);

	leftAudio	-> setValue (peakLeftDamped);
	rightAudio	-> setValue (peakRightDamped);
}
//
/////////////////////////////////////////////////////////////////////////////
//	
/**
  *	\brief TerminateProcess
  *	Pretty critical, since there are many threads involved
  *	A clean termination is what is needed, regardless of the GUI
  *	the attempt is first to stop various activities and then
  *	delete the different objects
  */
void	RadioInterface::TerminateProcess () {
	running. store	(false);
	stopChannel	();
	
//	stop timers apart from scanner
	displayTimer.	stop	();
	presetTimer.	stop	();
	pauzeTimer.	stop	();
	muteTimer.	stop	();
	updateCheck_timer. stop (); 
//	stop activity using a timer
	stopScanning ();
	while (theSCANHandler. active ())
	   usleep (1000);
	mapHandler_locker. lock ();
	if (mapViewer != nullptr) {
	   delete mapViewer;
	   mapViewer = nullptr;
	}
	if (newServices != nullptr)
	   delete newServices;

	if (theControl != nullptr)
	   delete theControl;
	theControl		= nullptr;
	mapHandler_locker. unlock ();
	theSCANHandler. hide ();
	channelTimer.	stop	();
//
//	finish all dumping activities
	stopFrameDumping	();
	stopSourceDumping	();
	stopAudioDumping	();
	if (channel. etiActive)
	   stop_etiHandler      ();     // probably done by stopChannel
//
//	after a scan is stopped, the scanTable is of no further use
	if (theScanTable != nullptr) {
	   theScanTable	-> clearTable ();
	   theScanTable	-> hide ();
	   delete theScanTable;
	}
//
//	handling the aboutLabel
	if (thecopyrightLabel != nullptr) {
	   thecopyrightLabel -> hide ();
	   delete thecopyrightLabel;
	}
//
	if (mapViewer != nullptr) {
//	   mapViewer ->  stop ();
	   delete mapViewer;
	}

//	should be hidden  to ensure the parent can be killed
	theSNRViewer.	hide ();	
	theScheduler.	hide ();
//
//	store positions of the relevant windows
	storeWidgetPosition	(theQSettings, this, S_MAIN_WIDGET);
	theNewDisplay.		storePosition ();
	theNewDisplay. hide ();
	theDXDisplay.		storePosition ();
	theDXDisplay. hide ();
	theConfigHandler ->	storePosition ();
	theConfigHandler	->	hide ();
	theTechWindow	->	storePosition ();	
	theTechWindow 	->	 hide ();
//
#ifdef	DATA_STREAMER
	fprintf (stderr, "going to close the dataStreamer\n");
	delete		theDataStreamer;
#endif
#ifdef	CLOCK_STREAMER
	fprintf (stderr, "going to close the clockstreamer\n");
	delete	theClockStreamer;
#endif
	if (dlTextFile != nullptr)
	   fclose (dlTextFile);
#ifdef	HAVE_PLUTO_RXTX
	if (theDabStreamer != nullptr)
	   theDabStreamer	-> stop ();
#endif
	if (theOfdmHandler != nullptr)	// hould always happen
	   theOfdmHandler		-> stop ();
	if (theDeviceHandler != nullptr)
	   delete theDeviceHandler;
	if (theAudioPlayer != nullptr)
	   delete theAudioPlayer;
//	journaline could have been stopped by stopChannel,
	if (journalineHandler != nullptr) {
	   delete journalineHandler;
	   journalineHandler = nullptr;
	}
	theLogger. log (logger::LOG_RADIO_STOPS);
	theQSettings	-> sync ();
	usleep (1000);		// pending signals
//	everything should be halted by now, start deleting
	delete		theTechWindow;
	delete		theConfigHandler;
	delete		theOfdmHandler;
	hide ();
	close ();
	fprintf (stderr, ".. end the radio silences\n");
}
//
void	RadioInterface::updateTimeDisplay() {
	if (!running. load())
	   return;
	numberofSeconds ++;
	if ((numberofSeconds % 2) == 0) {
	   size_t idle_time	= 0, total_time = 0;
	   getCpuTimes (idle_time, total_time);
	   const float idle_time_delta =
	                 static_cast<float>(idle_time - previous_idle_time);
	   const float total_time_delta =
	                 static_cast<float> (total_time - previous_total_time);
	   const float utilization = 100.0 * (1.0 - idle_time_delta / total_time_delta);
	   theConfigHandler -> showLoad (utilization);
	   previous_idle_time = idle_time;
	   previous_total_time = total_time;
	}
//
//	The timer runs autonomously, so it might happen
//	that it rings when there is no processor running
	if (!running. load ())
	   return;

	if (!theTechWindow -> isHidden () &&
	                          theAudioPlayer -> hasMissed ())  {
	   int totalSamples	= 0;
	   int totalMissed	= 0;
	   theAudioPlayer -> samplesMissed (totalSamples, totalMissed);
	   if (totalSamples != 0) {
	      float correct = (totalSamples - totalMissed) * 100 / totalSamples;
	      theTechWindow -> showMissed (correct);
	   }
	}
}
//
////////////////////////////////////////////////////////////////////////
//	selecting and allocating a device
////////////////////////////////////////////////////////////////////////

deviceHandler	*RadioInterface::createDevice (const QString &s,
	                                        logger *theLoggerParam) {
deviceHandler	*inputDevice = theDeviceChooser.
	                               createDevice  (s, version);
	
	(void)theLoggerParam;		// for now
	if (inputDevice	== nullptr)
	   return nullptr;

	channel. realChannel	= !inputDevice -> isFileInput ();
	scanButton	-> setEnabled (channel. realChannel);
	connect (inputDevice, &deviceHandler::frameClosed,
	         this, &RadioInterface::handle_deviceFrame_closed);
	QString ss = s;
	store (theQSettings, DAB_GENERAL, SELECTED_DEVICE, ss);
	inputDevice -> setVisibility (true);
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
	theAudioPlayer	-> stop ();
	if (theDeviceHandler != nullptr) {
	   theDeviceHandler	-> stopReader	();
	   theDeviceHandler	-> stopDump	();
	   delete theDeviceHandler;
	   theDeviceHandler = nullptr;
	}

	theDeviceHandler =  createDevice (deviceName, &theLogger);
	if (theDeviceHandler == nullptr) {
	   theDeviceHandler	= new deviceHandler ();	// the dummy one
	   return;		// nothing will happen
	}

	theDeviceChooser. hide ();
	theAudioPlayer	-> restart ();
	startDirect ();		// will set running
}

///////////////////////////////////////////////////////////////////////
//	handling time
////////////////////////////////////////////////////////////////////////
static
const char *monthTable [] = {
	"jan", "feb", "mar", "apr", "may", "jun",
	"jul", "aug", "sep", "oct", "nov", "dec"
};
//
//	called from the fibDecoder
void	RadioInterface::handle_FIG010 (int year, int month, int day,
	                               int hours, int minutes,
	                               int d2, int h2, int m2, int seconds){
	if (!running. load ())
	   return;
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
	   theClockStreamer -> sendData (localBuffer, 10);
#endif
	this	-> UTC. year		= year;
	this	-> UTC. month		= month;
	this	-> UTC. day		= d2;
	this	-> UTC. hour		= h2;
	this	-> UTC. minute		= m2;
	QString result;
	if (theConfigHandler -> utcSelector_active ())
	   result	= convertTime (year, month, day, h2, m2);
	else
	   result	= convertTime (year, month, day,
	                                     hours, minutes);
	QDate theDate (year, month, day);
	channel. theDate = theDate;

	QFont font		= serviceLabel -> font ();
	font. setBold (true);
        font. setPointSize (14);
	localTimeDisplay	-> setStyleSheet (labelStyle);
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
////////////////////////////////////////////////////////////////////////////
//	reporting errors
//	called from the MP4 decoder
void	RadioInterface::show_frameErrors (int s) {
	if (!running. load ()) 
	   return;
	if (!theTechWindow -> isHidden ())
	   theTechWindow -> showFrameErrors (s);
}
//
//	called from the MP4 decoder
void	RadioInterface::show_rsErrors (int s) {
	if (!running. load ())		// should not happen
	   return;
	if (!theTechWindow -> isHidden ())
	   theTechWindow	-> showRsErrors (s);
}
//
//	called from the MP4 decoder
void	RadioInterface::show_aacErrors (int s) {
	if (!running. load ())
	   return;
	if (!theTechWindow -> isHidden ())
	   theTechWindow	-> showAacErrors (s);
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

//////////////////////////////////////////////////////////////////////
//	called from the PAD handler, set the "mot" label
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

static inline
bool	isDigit (QChar x) {
	return ('0' <= x) && (x <= '9');
}

static inline
bool	isLetter (QChar x) {
	return (('a' <= x) && (x <= 'z')) ||
	       (('A' <= x) && (x <= 'Z'));
}

//	called from the PAD handler
void	RadioInterface::showLabel	(const QString &s, int charset) {
	(void)charset;
#ifdef	HAVE_PLUTO_RXTX
	if ((theDabStreamer != nullptr) && (s != ""))
	   theDabStreamer -> addRds (std::string (s. toUtf8 (). data ()));
#endif
	
	if (running. load()) {
	   int index = s. indexOf ("www.");
	   if (index >= 0) {
	      QString res;
	      for (int i = index; i < s. size (); i ++) {
	         if (isDigit (s [i]) || isLetter (s [i]) || (s [i] == "."))
	            res. push_back (s [i]);
//	         fprintf (stderr, "%s\n", res. toUtf8 (). data ());
	      }
	   }
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
//
//	setting the "stereo" label
void	RadioInterface::setStereo	(bool b) {
	if (!running. load () || (stereoSetting == b))
	   return;
	if (b) {
	   stereoLabel	-> setStyleSheet (labelStyle);
	   stereoLabel	-> setText ("<i>stereo</i>");
	}
	else
	   stereoLabel	-> setText ("      ");
	theTechWindow	-> showStereo (b);
	stereoSetting = b;
}
//
//	setting the techwindow
void	RadioInterface::handle_detailButton	() {
	if (!running. load ())
	   return;
	if (theTechWindow -> isHidden ())
	   theTechWindow -> show ();
	else
	   theTechWindow -> hide ();
	store (theQSettings, DAB_GENERAL, TECHDATA_VISIBLE,
	                            theTechWindow -> isHidden () ? 0 : 1);
}
//
void	RadioInterface::set_sync_lost	() {
}
//
/////////////////////////////////////////////////////////////////////////
//	dump handling

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
	theTechWindow	-> audiodumpButton_text ("audio dump", 10);
}

void	RadioInterface::startAudioDumping () {
	if (audioDumping)	// should not happen
	   return;

	QString audioDumpName	=
	      theFilenameFinder.
	           find_audioDump_fileName  (channel. currentService. serviceName, true);
	if (audioDumpName == "")
	   return;

	theTechWindow	-> audiodumpButton_text ("writing", 12);
	theAudioConverter. start_audioDump (audioDumpName);
	audioDumping	= true;
}

//
//	called from the mp4 handler, using a signal
void	RadioInterface::newFrame        (uint32_t amount) {
uint8_t	*buffer = dynVec (uint8_t, amount);

	if (!running. load ())
	   return;

	if (channel. currentService. frameDumper == nullptr) 
	   theFrameBuffer. FlushRingBuffer ();
	else
	while (theFrameBuffer. GetRingBufferReadAvailable () >= amount) {
	   theFrameBuffer. getDataFromBuffer (buffer, amount);
	   if (channel. currentService. frameDumper != nullptr)
	      fwrite (buffer, amount, 1,
	                        channel. currentService. frameDumper);
	}
}

//	on sourcedumping
void	RadioInterface::handle_dump () {
	if (dumpDisplay_p == nullptr)
	   startSourceDumping ();
	else
	   stopSourceDumping ();
}

void	RadioInterface::startSourceDumping () {
QString deviceName	= theDeviceHandler	-> deviceName	();
int	bitDepth	= theDeviceHandler	-> bitDepth	();
QString channelName	= channel. channelName;
QString	dumpName;

	if ((newServices -> getMode () != ENSEMBLEVIEW) ||
	                                  (theSCANHandler. active ()))
	   return;

	if (!theConfigHandler -> dumpmode_set () ||
	          !theDeviceHandler	-> providesDump ()) {
	   dumpName	=
	         theFilenameFinder. find_rawDump_fileName (deviceName, channelName);
	   if (dumpName == "")
	      return;
	   theLogger. log (logger::LOG_SOURCEDUMP_STARTS,
	                                     deviceName, channelName);

	   theOfdmHandler -> startDumping (dumpName,
                                           channel. tunedFrequency,
                                           bitDepth,
                                           deviceName);
	   dumpDisplay_p	= new dumpDisplay ("dumping in a \"wav\" file",
	                                                    dumpName);
	   return;
	}

	if (!theDeviceHandler -> providesDump ()) 
	   return;

	try {
	   dumpName     =
                  theFilenameFinder.
	              find_xmlName (deviceName,  channelName, false);
	} catch (...) {
	   return;
	}

	if (dumpName == "")
	   return;

	theLogger. log (logger::LOG_SOURCEDUMP_STARTS,
	                                     deviceName, channelName);
	theDeviceHandler -> startDump (dumpName, 0);
	dumpDisplay_p	=
	             new dumpDisplay ("dumping in an \"uff\" (i.e. xml) file",
	                                                  dumpName);
}

void	RadioInterface::stopSourceDumping	() {
	if (dumpDisplay_p == nullptr)
	   return;
	delete dumpDisplay_p;
	dumpDisplay_p	= nullptr;
	theLogger. log (logger::LOG_SOURCEDUMP_STOPS);
	if (!theDeviceHandler	-> providesDump ())
	   theOfdmHandler	-> stopDumping ();
	else
	   theDeviceHandler	-> stopDump ();
}

//	End of section on dumping
///////////////////////////////////////////////////////////////////////

void	RadioInterface::handle_spectrumButton	() {
	if (!running. load ())
	   return;
	if (theNewDisplay. isHidden ())
	   theNewDisplay. show ();
	else
	   theNewDisplay. hide ();
	store (theQSettings, DAB_GENERAL, NEW_DISPLAY_VISIBLE,
	                              theNewDisplay. isHidden () ? 0 : 1);
}

void	RadioInterface::connectGUI	() {
//	connections from the main widget
	connect (spectrumButton, &QPushButton::clicked,
	         this, &RadioInterface::handle_spectrumButton);
	connect (serviceLabel, &clickablelabel::clicked_left,
	         this, &RadioInterface::handle_labelColor);
	connect (serviceLabel, &clickablelabel::clicked_right,
	         this, &RadioInterface::handle_detailButton);
//
	connect (httpButton, &QPushButton::clicked,
	         this, &RadioInterface::handle_httpButton);
	connect (scanButton, &QPushButton::clicked,
	         this, &RadioInterface::handle_scanButton);
//	connect (this, &RadioInterface::call_scanButton,
//	         this, &RadioInterface::handle_scanButton);
}

void	RadioInterface::disconnectGUI () {
}
//
#include <QCloseEvent>
void RadioInterface::closeEvent (QCloseEvent *event) {
int x = theConfigHandler -> closeDirect_active ();

	store (theQSettings, CONFIG_HANDLER, CLOSE_DIRECT_SETTING, x);
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

	if (obj == newServices -> theTable -> viewport ()) {
	    if (event -> type () == QEvent::MouseButtonPress) {
	      QMouseEvent *ev = static_cast<QMouseEvent *>(event);
	      if (ev -> buttons () & Qt::RightButton) {
//	         QTableWidgetItem *x =
//	              newServices -> theTable -> itemAt (ev -> pos ());
//	         if (x != nullptr)
//	            newServices -> handleRightMouseClick (x -> text ());
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
//	      theEnsembleHandler -> selectCurrentItem ();
	      return true;
	   }
	   else		// handling function keys 
	   if (QApplication::keyboardModifiers ().
	                          testFlag (Qt::ControlModifier)) {
              return handle_keyEvent (keyEvent -> key ());
	   }
	}
	return QWidget::eventFilter (obj, event);
}
//
//	Announcements
QPixmap RadioInterface::fetchAnnouncement (int id) {
QPixmap p;
QString pictureName	= QString (":res/radio-pictures/announcement%1.png").
	                                      arg (id, 2, 10, QChar ('0'));
	if (!p.load (pictureName, "png"))
	   p. load (":res/radio-pictures/announcement-d.png", "png");
	return p;
}
//
//	dispatch the signal
void	RadioInterface::handle_FIG019	(uint16_t SId, int flags) {
	if (!running. load ())
	   return;

	if (channel. currentService. SId == static_cast<uint32_t>(SId)) {
	   if (flags != 0)
	      announcement_start (SId, flags);
	   else
	      announcement_stop (SId);
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
	if (channel. currentService. SId != SId)
	   return;
	serviceLabel	-> setStyleSheet ("QLabel {color : red}");
	int pictureId	= bits (flags);
	QPixmap p = fetchAnnouncement (pictureId);
	displaySlide (p);
	channel. announcing = true;
}

void	RadioInterface::announcement_stop (uint16_t SId) {
	if (channel. currentService. SId != SId)
	   return;
	
	serviceLabel	-> setStyleSheet (labelStyle);
	channel. announcing = false;
	show_pauzeSlide ();
}
//
////////////////////////////////////////////////////////////////////////
//	selection, either direct, from presets,  from scanlist or schedule
//
//	called from the viewhandler
void	RadioInterface::localSelect (const QString &service,
	                                    const QString &theChannel) {
	localSelect_SS (service, theChannel);
}
//
//	selecting from a content description
void	RadioInterface::handle_contentSelector (const QString &s) {
	newServices	-> reportService (s);
	localSelect_SS (s, channel. channelName);
}
//
void	RadioInterface::scheduleSelect (const QString &s) {
QStringList list	= splitter (s);
	if (list. length () != 2)
	   return;
	presetTimer. stop ();
	newServices	-> reportService (list. at (0), list. at (1));
	localSelect_SS (list. at (1), list. at (0));
}
//
void	RadioInterface::localSelect_SS (const QString &service,
	                                const QString &theChannel) {

QString serviceName	= service;
	stopService (channel. currentService);
	for (int i = service. size (); i < 16; i ++)
	   serviceName. push_back (' ');

//	if not local, first stop channel
	if (!theDeviceHandler -> isFileInput () && 
	    (theChannel  != channel. channelName)) {
	   stopChannel ();
	   startChannel  (theChannel, serviceName);
	   return;
	}
//
//	if the service is from the current channel:
	channel. currentService. isValid = false;
	dabService s;
	startService (service);
}

///////////////////////////////////////////////////////////////////////////

void	RadioInterface::stopService	(dabService &s) {
	if (!s. isValid)
	   return;
	presetTimer. stop ();
	channelTimer. stop ();
	stopMuting	();
	setSoundLabel (false);
	channel. audioActive	= false;

	if (s. isAudio) {
	   theAudioPlayer -> suspend ();
	   stopAudioDumping ();
	   stopFrameDumping ();

//	and clean up the technical widget
	   theTechWindow	-> cleanUp ();
	}

//	all running backends are described in the runningtasks lit
	   for (uint32_t i =  channel. runningTasks. size () - 1;
	                           i > 0; i --) {
	      dabService g = channel. runningTasks [i];
	      if (g. SId == s. SId) {
	         theOfdmHandler -> stopService (g. SId, g. SCIds);
	         channel. runningTasks. erase
	                        (channel. runningTasks. begin () + i);
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
	theOfdmHandler -> setDataChannel (pd, &theDataBuffer, BACK_GROUND);
	dabService s;
	s. channel     = pd. channel;
	s. serviceName = pd. serviceName;
	s. SId         = static_cast<uint32_t>(pd. SId);
	s. subChId     = pd. subchId;
	s. fd          = nullptr;
	s. runsBackground = true;
	channel. runningTasks. push_back (s);
	epgLabel	-> show ();
}
//
void	RadioInterface::startService (const QString &serviceName) {
dabService	s;
	s. serviceName	= serviceName;
	theOfdmHandler -> mapNameToId (serviceName, s. SId, s. SCIds);
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
	if (theOfdmHandler -> isAudioService (s. SId, s. SCIds)) {
	   audiodata ad;
	   theOfdmHandler -> audioData (s. SId, s. SCIds, ad);
	   ad. channel = channel. channelName;
	   if (ad. defined) {
	      channel. currentService. isValid	= true;
	      channel. currentService. isAudio	= true;
	      channel. currentService. ASCTy	= ad. ASCTy;
	      channel. currentService. subChId	= ad. subchId;
	      startAudioservice (ad);

	      serviceLabel	-> setText (serviceName);
	      QPixmap p;
	      if (get_serviceLogo (p, channel. currentService. SId)) {
	         int height = 60;
	         int width =
	             static_cast<float>(p. width ()) / p. height () * height;
	         iconLabel ->
	            setPixmap (p. scaled (width, height));
	      }
	      else
	         iconLabel -> setText (ad. shortName);
	   }
	   theTechWindow	-> isDABPlus  (ad. ASCTy == DAB_PLUS);
	}
	else  {
	   packetdata pd;
	   theOfdmHandler -> packetData (s. SId, s. SCIds, pd);
	   if (!pd. defined) {
	      QMessageBox::warning (this, tr ("Warning"),
 	                           tr ("insufficient data for this program\n"));
	      QString s2 = "";
	      store (theQSettings, DAB_GENERAL, PRESET_NAME, s2);
	      return;;
	   }
	   pd. channel = channel. channelName;
	   serviceLabel	-> setText (pd. serviceName);
	   channel. currentService. isValid	= true;
	   channel. currentService. isAudio	= false;
	   channel. currentService. subChId	= pd. subchId;
	   startPacketservice (pd);
	}
}
//
void	RadioInterface::startAudioservice (audiodata &ad) {
	(void)theOfdmHandler -> setAudioChannel (ad, &theAudioBuffer,
	                                            nullptr, FORE_GROUND);
	dabService s;
	s. channel	= ad. channel;
	s. serviceName	= ad. serviceName;
	s. SId		= ad. SId;
	s. SCIds	= ad. SCIds;
	s. subChId	= ad. subchId;
	s. fd		= nullptr;
	s. runsBackground	= false;
	channel. runningTasks. push_back (s);
	if (!theDeviceHandler -> isFileInput ())
	   store (theQSettings, "channelPresets", s. channel, s. serviceName);
//
//	check the other components for this service (if any)
	if (theOfdmHandler -> isPrimaryService (ad. SId, ad. SCIds)) {
	   std::vector<uint8_t> f =
                         theOfdmHandler -> get_secondaryServices (ad. SId);
	   for (auto &sc : f) {
	      packetdata pd;
	      theOfdmHandler -> packetData (ad. SId, sc, pd);
	      if (pd. defined) {
	         dabService s2;
	         s2. channel      = pd. channel;
	         s2. serviceName  = pd. serviceName;
	         s2. SId          = pd. SId;
	         s2. SCIds        = pd. SCIds;
	         s2. subChId      = pd. subchId;
	         s2. runsBackground       = true;
	         channel. runningTasks. push_back (s2);
	         theOfdmHandler -> setDataChannel (pd, &theDataBuffer,
	                                                BACK_GROUND);
	      }
	   }
	}
//	activate sound and update the various signs
	theAudioPlayer		-> resume ();
	channel. audioActive	= true;
	setSoundLabel (true);
	programTypeLabel	-> setText (getProgramType (channel. internatTable, ad. programType));
	rateLabel		-> setStyleSheet ("color:magenta");
	rateLabel		-> setText (QString::number (ad. bitRate) +
	                                                        "kbit");
	QString protL		= getProtectionLevel (ad. shortForm,
	                                                    ad. protLevel);
	QString crL		= getCodeRate (ad. shortForm, ad. protLevel);
	protectionLabel		-> setStyleSheet ("color:red");
	QFont font		= protectionLabel -> font ();
	font. setPointSize (9);
	protectionLabel		-> setFont (font);
	protectionLabel		-> setText (protL+ " " + crL);

	theOfdmHandler -> getFreqs (ad. SId, ad. fmFrequencies);
	                                      
//	show service related data
	theTechWindow	-> showServiceData 	(channel. internatTable, &ad);
}

void	RadioInterface::startPacketservice (packetdata &pd) {
	if ((pd.  DSCTy == 0) || (pd. bitRate == 0)) {
	   QMessageBox::warning (this, tr ("sdr"),
 	                         tr ("still insufficient data for this service\n"));
	   return;
	}

	if (!theOfdmHandler -> setDataChannel (pd, &theDataBuffer,
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
	protectionLabel			-> setText ("");

	stereoSetting			= false;
	setStereo	(false);
	theTechWindow			-> cleanUp ();
	motLabel			-> setStyleSheet ("QLabel {color : red}");
	distanceLabel			-> setText ("");
	newServices			-> set_countryName ("");
	theNewDisplay. ficError_display	-> setValue (0);
}

//
//	The user(s)
///////////////////////////////////////////////////////////////////////////
//	setPresetService () is called after a time out to 
//	actually start the service that we were waiting for
//	The relevant data is in "nextService"
void	RadioInterface::setPresetService () {
	presetTimer. stop ();
	if (!running. load () ||
	     theSCANHandler. active () ||
	     theDeviceHandler -> isFileInput ())
	   return;

	if (nextService. channel != channel. channelName)
	   return;
	
	if (channel. Eid == 0) {
	   dynamicLabel -> setText ("ensemblename not yet found\n");
	   return;
	}
	if (newServices != nullptr)
	   newServices -> reportService (channel. channelName, 
	                                   nextService. serviceName);
	QString presetName	= nextService. serviceName;

	dabService s = nextService;
	startService (presetName);
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
	theNewDisplay. showFrequency (theChannel, tunedFrequency);
	presetTimer. stop	();		// should not run
	theDeviceHandler		-> restartReader (tunedFrequency, 
	                                                 SAMPLERATE / 10);
	channel. cleanChannel	();
	channel. channelName	= theChannel;
	channel. tunedFrequency	= tunedFrequency;
	theLogger. log (logger::LOG_NEW_CHANNEL, theChannel, channel. snr);
	channel. realChannel	= !theDeviceHandler -> isFileInput ();
	if (channel. realChannel) {
	   store (theQSettings, DAB_GENERAL, CHANNEL_NAME, theChannel);
	}

//	The ".sdr" and ".uff" files - when built by us - carry
//	the channel frequency in their data
	if (theDeviceHandler -> isFileInput ()) {
	   int freq		= theDeviceHandler -> getVFOFrequency ();
	   QString realChannel	= theSCANHandler. getChannel (freq);
	   if (realChannel != "") {
	      newServices	-> set_channelIndex (realChannel);
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
	mapHandler_locker. lock ();
	if (mapViewer != nullptr) 
	   mapViewer -> putData (MAP_FRAME);
	mapHandler_locker. unlock ();
	if (theSCANHandler. active ()) {
	   theOfdmHandler	-> start ();
	   return;
	}

//	If we are scanning, we do not do delayed service start
	int switchDelay		=
	             theConfigHandler -> switchDelayValue ();
//	if no preset is started, we look in the tables what the servicename
//	was the last time the channel was active
	if (!theDeviceHandler -> isFileInput () &&
	                   !theSCANHandler. active ()) {
	   if (firstService == "") 	// no preset specified
	      firstService =
	            value_s (theQSettings, "channelPresets", theChannel, "");
//	at this point we do not know whether or not a preset is 
//	set, so if this mode is "on" and there is a service name
//	associated with the channel, we set the preset handling on
//	but only if ....
	   if (firstService != "") {
	      presetTimer. stop ();	// should not run here
	      nextService. channel	= theChannel;
	      nextService. serviceName	= firstService;
	      nextService. SId		= 0;
	      presetTimer. setInterval	(switchDelay);
	      presetTimer. start	(switchDelay);
	   }
	}
//	all set, go for it
	theOfdmHandler		-> start ();
}
//
//	apart from stopping the reader, a lot of administration
//	is to be done.
void	RadioInterface::stopChannel	() {
	epgLabel	-> hide ();
	presetTimer. stop 	();		// if running
	channelTimer. stop	();		// if running
	if (theControl != nullptr)
	   delete theControl;
	theControl = nullptr;

	theDeviceHandler		-> stopReader	();
	theDeviceHandler		-> stopDump	();
	newServices			-> clear_ensembleId ();
	stopSourceDumping	();
	if (channel. etiActive)
	   stop_etiHandler	();	// 
	theLogger. log (logger::LOG_CHANNEL_STOPS, channel. channelName);
	theNewDisplay. setSilent	();
//
	for (auto &serv : channel. runningTasks) {
	      theOfdmHandler -> stopService (serv. SId, serv. SCIds);
	   if (serv. fd != nullptr)
	      fclose (serv. fd);
	}
	channel. runningTasks. resize (0);

	if (theContentTable != nullptr) {
	   theContentTable -> hide ();
	   delete theContentTable;
	   theContentTable = nullptr;
	}

//	note framedumping - if any - was already stopped
//	ficDumping - if on - is stopped here
	theOfdmHandler -> stopFicDump ();	// just in case ...
	theOfdmHandler		-> stop ();
	theDXDisplay. cleanUp ();
	usleep (1000);
	theTechWindow	-> cleanUp ();

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
/////////////////////////////////////////////////////////////////////////
//
//	called by the serviceViewer to process channel changes
void	RadioInterface::handle_channelSelector (const QString &channelParam) {
	if (!running. load ())
	   return;

//	LOG select channel
	presetTimer. stop ();
	stopScanning	();
	stopChannel	();
	startChannel	(channelParam);
}

//	scanning
//	The scan function covers three scan strategies. In order to make things
//	manageable, we implement the streams  in different functions and procedures
void	RadioInterface::handle_scanButton () {
	if (!running. load () ||  (newServices -> getMode () != ENSEMBLEVIEW))
	   return;
	
	if (theSCANHandler. isVisible ())
	   theSCANHandler. hide ();
	else
	   theSCANHandler. show ();
}

void	RadioInterface::startScanning	() {
	if (theDeviceHandler -> isFileInput ()) {
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("Scanning not useful with file input"));
	   return;
	}
	stopChannel     ();
//	scanning and showing the techWindows does not make much sense
	theTechWindow	-> hide ();	// until shown otherwise
        store (theQSettings, DAB_GENERAL, TECHDATA_VISIBLE, false);
	presetTimer. stop ();
	channelTimer. stop ();
	connect (theOfdmHandler, &ofdmHandler::noSignalFound,
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
	QString currentChannel	= newServices -> currentChannel ();
	QString cs = theSCANHandler. getNextChannel (currentChannel);
	newServices	-> set_channelIndex (cs);
	
	theSCANHandler. addText (" scanning channel " + cs);
	int switchDelay		=
	              theConfigHandler -> switchDelayValue ();
	channelTimer. start (switchDelay);
	theOfdmHandler	-> setScanMode (true);
	startChannel    (cs);
}

void	RadioInterface::startScan_single () {
basicPrint thePrinter (channel. internatTable, channel. Eid, channel. eccByte);
//	theScanlistHandler. clearScanList ();
	if (theScanTable == nullptr) 
	   theScanTable = new contentTable (this, theQSettings, "scan", 
	                                       thePrinter. scanWidth ());
	else					// should not happen
	   theScanTable -> clearTable ();

	QString topLine = QString ("ensemble") + ";"  +
	                           "channelName" + ";" +
	                           "frequency (KHz)" + ";" +
	                           "Eid" + ";" +
	                           "time" + ";" +
	                           "SNR" + ";" +
	                           "nr services" + ";" ;
	 
	theScanTable	-> addLine (topLine);
	theScanTable	-> addLine ("\n");
	stopChannel ();		// was done already
	theOfdmHandler		-> setScanMode (true);
	QString fs		= theSCANHandler. getFirstChannel ();
	newServices		-> set_channelIndex (fs);
	newServices		-> clearAll ();
	theSCANHandler. addText (" scanning channel " + fs);
	int switchDelay		=
	             theConfigHandler -> switchDelayValue ();
	channelTimer. start (switchDelay);
	startChannel    (fs);
}

void	RadioInterface::startScan_continuous () {
basicPrint thePrinter (channel. internatTable, channel. Eid, channel. eccByte);
	if (theScanTable == nullptr) 
	   theScanTable = new contentTable (this, theQSettings, "scan", 
	                                              thePrinter.scanWidth ());
	else					// should not happen
	   theScanTable -> clearTable ();

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
	theScanTable -> addLine (topLine);
	theScanTable	-> addLine ("\n");

	theOfdmHandler	-> setScanMode (true);
//      To avoid reaction of the system on setting a different value:
	QString fs = theSCANHandler. getFirstChannel ();
	int switchDelay	= 
	         theConfigHandler -> switchDelayValue ();
	newServices	-> set_channelIndex (fs);
	newServices	-> clearAll	();
	channelTimer. start (2 * switchDelay);
	startChannel    (fs);
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
	disconnect (theOfdmHandler, &ofdmHandler::noSignalFound,
	            this, &RadioInterface::no_signal_found);
	channelTimer. stop ();

	if (theSCANHandler. scan_to_data ())
	   stopScan_to_data ();
	else
	if (theSCANHandler. scan_single ())
	   stopScan_single ();
	else
	   stopScan_continuous ();
	theSCANHandler. setStop ();
}

void	RadioInterface::stopScan_to_data () {
	theOfdmHandler	-> setScanMode (false);
	channelTimer. stop ();
}

void	RadioInterface::stopScan_single () {
	theOfdmHandler	-> setScanMode (false);
	channelTimer. stop ();

	if (theScanTable == nullptr)
	   return;		// should not happen

	if (theConfigHandler -> upload_selector_active ()) {
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
	                               theScanTable -> upload ());
	      }
	   } catch (...) {}
	}

	FILE *scanDumper_p	= theSCANHandler. askFileName ();
	if (scanDumper_p != nullptr) {
	   theScanTable -> dump (scanDumper_p);
	   fclose (scanDumper_p);
	   scanDumper_p = nullptr;
	}
//	delete theScanTable;
//	theScanTable	= nullptr;
}

void	RadioInterface::stopScan_continuous () {
	theOfdmHandler	-> setScanMode (false);
	channelTimer. stop ();

	if (theScanTable == nullptr)
	   return;		// should not happen

	FILE *scanDumper_p	= theSCANHandler. askFileName ();
	if (scanDumper_p != nullptr) {
	   theScanTable -> dump (scanDumper_p);
	   fclose (scanDumper_p);
	   scanDumper_p = nullptr;
	}
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
	newServices	-> set_channelIndex (ns);
//	and restart for the next run
	theSCANHandler. addText ("scanning channel " + ns);
	int switchDelay	= 
	         theConfigHandler -> switchDelayValue ();
	channelTimer. start (switchDelay);
	startChannel (ns);
}

void	RadioInterface::nextFor_scan_single () {
	if (channel. nrServices > 0) 
	   show_for_single_scan ();
	stopChannel ();
	QString cs;
	try {
	   cs	= theSCANHandler. getNextChannel ();
	   newServices -> set_channelIndex (cs);
	} catch (...) {
	   stopScanning ();
	   return;
	}
	theSCANHandler. addText ("scanning channel " + cs);
	int switchDelay	= 
	         theConfigHandler -> switchDelayValue ();
	channelTimer. start (switchDelay);
	startChannel (cs);
}

void	RadioInterface::nextFor_scan_continuous () {
	if (channel. nrServices > 0)
	   show_for_continuous ();
	stopChannel ();

	QString cs	= theSCANHandler. getNextChannel ();
	newServices	-> set_channelIndex (cs);
	int switchDelay	= 
	         theConfigHandler -> switchDelayValue ();
	channelTimer. start (2 * switchDelay);
	startChannel (cs);
}

//	emd of scanning
////////////////////////////////////////////////////////////////////////////
//	preparing texts for csv file
///////////////////////////////////////////////////////////////////////////
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
	if (channel. strongestTransmitter != "")
	   theName	= channel. strongestTransmitter;
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
	                      QString::number (channel. Eid, 16) + ";" +
	                      getCountry (channel. internatTable,
	                                  channel. eccByte,
                                            (channel. Eid >> 12) &0xF) + ";" +
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
	int	freeSpace	= theOfdmHandler -> freeSpace ();
	QString headLine = channel. ensembleName + ";" +
	                      channel. channelName  + ";" +
	                      getCountry (channel. internatTable,
	                                  channel. eccByte,
	                                  (channel. Eid >> 12) & 0xF) + ";" +
	                      QString::number (channel. tunedFrequency) + ";" +
	                      QString::number (channel. Eid, 16) + ";" +
	                      utcTime + ";" +
	                      SNR + ";" +
	                      QString::number (channel. nrServices) +";"  +
	                      QString::number (freeSpace) + ", " +
	                      QString::number (static_cast<int> (freeSpace / 864.0 * 100)) + "%" + ";";
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
	theScanTable -> addLine (headLine);
	theScanTable -> addLine ("\n");
	for (const auto &tr : channel. transmitters) {
	   QString transmitterLine = build_transmitterLine (tr);
           theScanTable	-> addLine (transmitterLine);
        }
	basicPrint thePrinter (channel. internatTable,
	                       channel. Eid, channel. eccByte);
	QStringList s = thePrinter. print (theOfdmHandler -> contentPrint ());
	for (const auto &l : s)
	   theScanTable -> addLine (l);
	theScanTable -> addLine ("\n;\n;\n");
	theScanTable -> show ();
}

void	RadioInterface::show_for_continuous () {
	QString headLine = buildHeadLine ();
	theScanTable -> addLine (headLine);

	for (auto &tr: channel. transmitters) {
	   if (!tr. isStrongest) {
	      QString line = build_cont_addLine (tr);
	      if (line == "")
	         continue;
	      theScanTable -> addLine (line);
	   }
	}
	theScanTable -> show ();
}

QString RadioInterface::build_cont_addLine (const transmitter &tr) {
QString tii;
QString theName;
QString theDistance;
QString theCorner;
QString theHeight;

	tii	= idsToString (tr. mainId, tr. subId) + ";" ;
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
	if ((theAudioPlayer -> is_QtAudio () && audioVolume == 0) &&
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
	muteDelay	= theConfigHandler -> muteValue ();
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
//	End of handling mute button
//////////////////////////////////////////////////////////////////////////
//	
//	merely as a gadget, for each button the color can be set
//	Lots of code, about 400 lines, just for a gadget
//	
void	RadioInterface::set_Colors () {
QString scanButton_color =
	   value_s (theQSettings, COLOR_SETTINGS, SCAN_BUTTON + "_color",
	                                             GREEN);
QString scanButton_font =
	   value_s (theQSettings, COLOR_SETTINGS, SCAN_BUTTON + "_font",
	                                              BLACK);

QString spectrumButton_color =
	   value_s (theQSettings, COLOR_SETTINGS, SPECTRUM_BUTTON + "_color",
	                                              BLUE);
QString spectrumButton_font =
	   value_s (theQSettings, COLOR_SETTINGS, SPECTRUM_BUTTON + "_font",
	                                              BLACK);
QString	configButton_color =
	   value_s (theQSettings, COLOR_SETTINGS, CONFIG_BUTTON + "_color",
	                                              YELLOW);
QString configButton_font	=
	   value_s (theQSettings, COLOR_SETTINGS, CONFIG_BUTTON + "_font",
	                                              BLACK);
QString	httpButton_color =
	   value_s (theQSettings, COLOR_SETTINGS, HTTP_BUTTON + "_color",
	                                              YELLOW);
QString httpButton_font	=
	   value_s (theQSettings, COLOR_SETTINGS, HTTP_BUTTON + "_font",
	                                              BLACK);

	QString temp = "QPushButton {background-color: %1; color: %2}";
	spectrumButton ->
	              setStyleSheet (temp. arg (spectrumButton_color,
	                                        spectrumButton_font));
	scanButton	->
	              setStyleSheet (temp. arg (scanButton_color,
	                                        scanButton_font));
	configButton	->
	              setStyleSheet (temp. arg (configButton_color,
	                                        configButton_font));
	httpButton	->
	              setStyleSheet (temp. arg (httpButton_color,
	                                        httpButton_font));
}

void	RadioInterface::color_scanButton	() {
	setButtonColors (scanButton, SCAN_BUTTON);
}

void	RadioInterface::color_spectrumButton	()	{
	setButtonColors (spectrumButton, SPECTRUM_BUTTON);
}

void	RadioInterface::color_configButton	() 	{
	setButtonColors (configButton, CONFIG_BUTTON);
}

void	RadioInterface::color_httpButton	() 	{
	setButtonColors (httpButton, HTTP_BUTTON);
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
	store (theQSettings, COLOR_SETTINGS, buttonColor, baseColor_name);
	store (theQSettings, COLOR_SETTINGS, buttonFont, textColor_name);
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

	QStringList selectables	= newServices -> getSelectables ();
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
	   theConfigHandler -> set_closeDirect (true);
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

void	RadioInterface::scheduledAudioDumping () {
	if (audioDumping) {
	   theAudioConverter. stop_audioDump	();
	   audioDumping		= false;
	   theTechWindow	-> audiodumpButton_text ("audio dump", 10);
	   return;
	}

	QString audioDumpName	=
	      theFilenameFinder.
	            find_audioDump_fileName  (serviceLabel -> text (), false);
	if (audioDumpName == "")
	   return;

	theTechWindow	-> audiodumpButton_text ("writing", 12);
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
	theTechWindow ->  framedumpButton_text ("save AAC/MP2", 10);
	channel. currentService. frameDumper	= nullptr;
}

void	RadioInterface::startFrameDumping () {
	if (!channel. currentService. isAudio)
	   return;
	channel. currentService. frameDumper	=
	     theFilenameFinder.
	      find_frameDump_fileName (channel. currentService. serviceName,
	                              channel. currentService. ASCTy, true);
	if (channel. currentService. frameDumper == nullptr)
	   return;
	theLogger. log (logger::LOG_FRAMEDUMP_STARTS, 
	                                         channel. channelName,
	                                         channel. currentService. serviceName);
	QString mode = channel. currentService. ASCTy == DAB_PLUS ?
	                                       "recording aac" : "recording mp2";
	theTechWindow ->  framedumpButton_text (mode, 12);
}

void	RadioInterface::scheduled_frameDumping (const QString &s) {
	if (channel. currentService. frameDumper != nullptr) {
	   fclose (channel. currentService. frameDumper);
	   theTechWindow ->  framedumpButton_text ("frame dump", 10);
	   channel. currentService. frameDumper	= nullptr;
	   return;
	}
	audiodata ad;
	uint32_t SId;
	uint8_t SCIds;
	theOfdmHandler -> mapNameToId (s, SId, SCIds);
	theOfdmHandler -> audioData (SId, SCIds, ad);
	if (!ad. defined)
	   return;
	
	channel. currentService. frameDumper	=
	     theFilenameFinder. find_frameDump_fileName (s, ad. ASCTy, false);
	if (channel. currentService. frameDumper == nullptr)
	   return;
	theTechWindow ->  framedumpButton_text ("recording", 12);
}

void	RadioInterface::scheduledFICDumping () {
	if (theOfdmHandler -> ficDumping_on ()) {
	   theOfdmHandler	-> stopFicDump ();
	   return;
	}

	QString ficDumpFileName =
	     theFilenameFinder. find_ficDump_file (channel. channelName);
	if (ficDumpFileName == "")
	   return;
	theOfdmHandler -> startFicDump (ficDumpFileName);
}

void	RadioInterface::scheduledDLTextDumping () {
	if (dlTextFile != nullptr) {
	   fclose (dlTextFile);
	   dlTextFile = nullptr;
	   theConfigHandler	-> mark_dlTextButton (false);
	   return;
	}

	QString	fileName = theFilenameFinder. finddlText_fileName (false);
	dlTextFile	= fopen (fileName. toUtf8 (). data (), "w+");
	if (dlTextFile == nullptr)
	   return;
	theConfigHandler -> mark_dlTextButton (true);
}
//
//////////////////////////////////////////////////////////////////////////
//
void	RadioInterface::handle_configButton	() {
	if (!theConfigHandler -> isHidden ()) {
	   theConfigHandler ->  hide ();	
	   store (theQSettings, DAB_GENERAL, CONFIG_WIDGET_VISIBLE, 0);
	}
	else {
	   theConfigHandler -> show ();
	   store (theQSettings, DAB_GENERAL, CONFIG_WIDGET_VISIBLE, 1);
	}
}

void	RadioInterface::handle_devicewidgetButton	() {
	if (theDeviceHandler == nullptr)
	   return;
	int currentVisibility = theDeviceHandler -> getVisibility ();
	theDeviceHandler	-> setVisibility (!currentVisibility);
	store (theQSettings, DAB_GENERAL, DEVICE_WIDGET_VISIBLE,
	                                             !currentVisibility);
}
//
//	called from the configHandler
void	RadioInterface::handle_dlTextButton	() {
	if (dlTextFile != nullptr) {
	   fclose (dlTextFile);
	   dlTextFile = nullptr;
	   theConfigHandler	-> mark_dlTextButton (false);
	   return;
	}

	QString	fileName =theFilenameFinder. finddlText_fileName (true);
	dlTextFile	= fopen (fileName. toUtf8 (). data (), "w+");
	if (dlTextFile	== nullptr)
	   return;
	theConfigHandler	-> mark_dlTextButton (true);
}
//
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
	
	store (theQSettings, DAB_GENERAL, SNR_WIDGET_VISIBLE,
	                          theSNRViewer. isHidden () ? 0 : 1);
}
//
//	called from the configHandler
void	RadioInterface::handle_set_coordinatesButton	() {
coordinates theCoordinator (theQSettings);
	(void)theCoordinator. QDialog::exec();
	localPos. latitude		=
	             value_f (theQSettings, MAP_HANDLING, HOME_LATITUDE, 0);
	localPos. longitude		=
	             value_f (theQSettings, MAP_HANDLING, HOME_LONGITUDE, 0);
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
//
void	RadioInterface::handle_LoggerButton (int s) {
	(void)s;
	if (theConfigHandler -> logger_active ()) {
	   theLogger. logging_starts ();
	   store (theQSettings, DAB_GENERAL, LOG_MODE, 1);
	}
	else {
	   theLogger. logging_stops ();
	   store (theQSettings, DAB_GENERAL, LOG_MODE, 0);
	}
}

void	RadioInterface::set_transmitters_local  (bool isChecked) {
	channel. targetPos	= position {0, 0};
	if (isChecked) {
	   mapHandler_locker. lock ();
	   if (mapViewer != nullptr)
	      mapViewer -> putData (MAP_RESET);
	   mapHandler_locker. unlock ();
	}
}

void	RadioInterface::selectDecoder (int decoder) {
	if (theOfdmHandler != nullptr)
	   theOfdmHandler	-> handleDecoderSelector (decoder);
}

void	RadioInterface:: set_streamSelector (int k) {
	if (!running. load ())
	   return;
	QString str = theConfigHandler -> currentStream ();
	reinterpret_cast<audioSink *>(theAudioPlayer) -> selectDevice (k, str);
	store (theQSettings, SOUND_HANDLING, AUDIO_STREAM_NAME, str);
}
//
//////////////////////////////////////////////////////////////////////////
//
//	http handling
//
bool	RadioInterface::autoStart_http () {
	if (localPos. latitude == 0) 	// cannot happen
	   return false;
	if (mapViewer != nullptr)  
	   return false;

	try {
	   mapViewer = new httpHandler (this,
	                                ":res/qt-map.html",
	                                localPos,
	                                theConfigHandler -> localBrowserSelector_active (),
	       	                        theConfigHandler -> get_close_mapSelector (),	
	                                "",
	                                theQSettings);
	} catch (int e) {}
	return mapViewer != nullptr;
}
//
//	handling the httpButton has some interesting aspects
//	while it is obvious to start not only a server, but also
//	a browser with the web, killing both is less obvious.
//	If the "user" kills the broweser containing the map,
//	a simple timeout is sufficient to handle the killing of
//	the server.
//	If, however, the user touches the httpButton to stop the
//	webbrowsing, a special "token" is sent to the browser which
//	is understood by the javascript code (we hope).
//	The response is - at least in most cases - an error, caught
//	by the httphandler, telling to inform the "radio" to stop
//	the http server. It turned out that just killing the http server
//	from within the signal code caused - sometimes - a crash
//	That is why an indirection is added, the radio code will wait 
//	for a number of seconds for a signal to arrive to kill the http handler
static
int delay_teller = 0;
//	ensure that we only get a handler if we have a start location
void	RadioInterface::handle_httpButton	() {
	if (localPos. latitude == 0) {
	   QMessageBox::information (this, tr ("Warning"),
	                         tr ("Function not available, no coordinates were found"));
	   return;
	}

	if (mapViewer == nullptr)  {
	   try {
	      bool sf = value_i (theQSettings, CONFIG_HANDLER, SAVE_HTTP, 0);
	      QString saveName;
	      if (sf)
	         saveName =
	               theFilenameFinder.
	                   find_mapdumpName (theDeviceHandler -> deviceName ());
	      mapViewer = new httpHandler (this,
	                                    ":res/qt-map.html",
	                                    localPos,
	                                    theConfigHandler -> localBrowserSelector_active (),
	       	                            theConfigHandler -> get_close_mapSelector (),
	                                    saveName,
	                                    theQSettings);
	   } catch (int e) {}
	   if (mapViewer != nullptr)
	      httpButton -> setText ("http-on");
	}
	else {		// forced stop
//	Two options. If the server is running, issue a mapClose
//	request. If the server is running without a map just delete
	   if (mapViewer == nullptr) 	// nothing to do
	      return;
	   int auto_http   = value_i (theQSettings, CONFIG_HANDLER,
                                               AUTO_HTTP, 0);
	   if ((auto_http != 0) ||
	       !theConfigHandler -> get_close_mapSelector () ||
	       !mapViewer -> isConnected ()) {
	      cleanUp_mapHandler ();
	      return;
	   }
//	handler is running and we want the map closed
//	we send a signal, and "poll" for the result
	   mapHandler_locker. lock ();
	   mapViewer -> putData (MAP_CLOSE);
	   mapHandler_locker. unlock ();
	   delay_teller	= 0;
	   stillWaiting	= true;
	   connect (&theTimer, &QTimer::timeout,
	            this, &RadioInterface::waitingToDelete);
	   theTimer. start (1000);
	}
}

void	RadioInterface::waitingToDelete () {
	delay_teller ++;
	if ((delay_teller < 10) && stillWaiting) {
	   theTimer. start (1000);
	   return;
	}
	disconnect (&theTimer, &QTimer::timeout,
                    this, &RadioInterface::waitingToDelete);
	cleanUp_mapHandler ();
}
//
//	On the map we can select a channel
void	RadioInterface::channelSignal (const QString &channelParam) {
	stopChannel ();
	newServices	-> set_channelIndex (channelParam);
	startChannel (channelParam);
}

void	RadioInterface::cleanUp_mapHandler () {
	disconnect (mapViewer, &httpHandler::mapClose_processed,
	            this, &RadioInterface::http_terminate);
//	fprintf (stderr, "Going to delete mapserver\n");
	locker. lock ();
	if (mapViewer != nullptr) {
	   mapViewer -> close ();
	   delete mapViewer;
	   mapViewer	= nullptr;
	}
	locker. unlock ();
	httpButton -> setText ("http");
}
//
//	
void	RadioInterface::http_terminate	() {
	stillWaiting = false;
}
//
///////////////////////////////////////////////////////////////////////////

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
	slideName	= slideName. arg (pauzeSlideTeller);
	if (p. load (slideName, "png")) {
	QString tooltipText = "";
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
	theOfdmHandler -> stopEtiGenerator ();
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
	                     theDeviceHandler -> deviceName (),
	                     channel. channelName);
	channel. etiActive = theOfdmHandler -> startEtiGenerator (etiFile);
	if (channel. etiActive) 
	   scanButton -> setText ("eti runs");
}

void	RadioInterface::handle_eti_activeSelector (int k) {
bool setting	= theConfigHandler -> eti_active ();
	(void)k;
          
        if (newServices -> getMode () == FAVORITEVIEW)
	   return;

	if (setting) {
	   stopScanning ();
	   disconnect (scanButton, &QPushButton::clicked,
	               this, &RadioInterface::handle_scanButton);
	   connect (scanButton, &QPushButton::clicked,
	            this, &RadioInterface::handle_etiHandler);
	   scanButton -> setEnabled (true);
	   scanButton	-> setText ("eti");
//	   if (!theDeviceHandler -> isFileInput ())// restore the button' visibility
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
        if (newServices -> getMode () != ENSEMBLEVIEW)
	   scanButton -> hide ();
}

/////////////////////////////////////////////////////////////////////////
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
	store (theQSettings, CONFIG_HANDLER, TII_THRESHOLD, v);
	theOfdmHandler -> setTIIThreshold (v);
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
	      theNewDisplay. showTII (inBuffer, channel. tunedFrequency, -1);
	}
}

void	RadioInterface::showCorrelation	(uint32_t s, int g,
	                                        QVector<int> maxVals) {
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

void	RadioInterface::show_null		(uint32_t amount,
	                                                 int startIndex) {
Complex	*inBuffer = dynVec (Complex, amount);
	theNULLBuffer. getDataFromBuffer (inBuffer, amount);
	if (!theNewDisplay. isHidden ())
	   if (theNewDisplay. getTab () ==  SHOW_NULL)
	      theNewDisplay. showNULL (inBuffer, amount, startIndex);
}

void	RadioInterface::showIQ			(uint32_t amount) {
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

void	RadioInterface::show_stdDev	(uint32_t amount) {
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

	uint32_t amount =  theSNRBuffer. GetRingBufferReadAvailable ();
	if (amount == 0)
	   return;
	float *ss = dynVec (float, amount);
	theSNRBuffer. getDataFromBuffer (ss, amount);
	for (uint32_t i = 0; i < amount; i ++) {
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
	if (!theTechWindow -> isHidden ())
	   theTechWindow -> showRsCorrections (c, ec);
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

void	RadioInterface::show_channel	(uint32_t n) {
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
	theOfdmHandler -> handleIQSelector ();
}
//
////////////////////////////////////////////////////////////////////////////
//
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
	
void	RadioInterface::handle_copyrightLabel   () { 
	if (thecopyrightLabel == nullptr) {
	   thecopyrightLabel	= new copyrightText (this, VERSION);
	   thecopyrightLabel -> show ();
	   return;
	}
	copyrightText_closed ();
}
//
//	killing the window generates a signal
void	RadioInterface::copyrightText_closed	() {
	delete thecopyrightLabel;
	thecopyrightLabel	= nullptr;
}
///////////////////////////////////////////////////////////////////////
//
void	RadioInterface::show_dcOffset (float dcOffset) {
	theNewDisplay. showDCOffset (dcOffset);
}

void	RadioInterface::handle_dcRemoval	(bool b) {
	theOfdmHandler	-> set_dcRemoval (b);
	theNewDisplay. set_dcRemoval (b);
}
///////////////////////////////////////////////////////////////////////
//	processing TII data
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
//
//	showing tii data, and preparing for the map
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
	   QString country	= get_ITU_Code (channel. internatTable,
	                                         channel. eccByte,
	                                         (channel. Eid >> 12) &0xF);
	   channel. countryName	= country;
	   channel. strongestTransmitter = "";
	   newServices		-> set_countryName (country);
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
	      if (!theConfigHandler -> get_allTIISelector ())
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
	      theTransmitter. valid	= true;
	      channel. transmitters. push_back (theTransmitter);	
	   }
	   if (dxMode)
	      addtoLogFile (&theTransmitter);
	}
//
	int	bestIndex = -1;
	float Strength	= -100;
//	Now the list is updated, see whether or not the strongest is ...
	int localTeller = 0;
	for (auto &transm : channel. transmitters) {
	   if (transm. valid &&
	                  (transm. strength > Strength)) {
	      bestIndex = localTeller;
	      Strength  = transm. strength;
	   }
	   transm. isStrongest = false;
	   localTeller ++;
	}

	if (bestIndex >= 0) {
	   channel. transmitters [static_cast<uint32_t>(bestIndex)]. isStrongest = true;
	}
//
//	for content maps etc we need to have the data of the strongest
//	signal
	if (bestIndex >= 0) {
	   transmitter *ce = &channel. transmitters [static_cast<uint32_t>(bestIndex)];	
	   channel. mainId		= static_cast<int8_t>(ce -> mainId);
	   channel. subId		= static_cast<int8_t>(ce -> subId);
	   channel. strongestTransmitter	= ce -> transmitterName;
	   channel. height		= ce -> height;
	   channel. distance		= ce -> distance;
	   channel. azimuth		= ce -> azimuth; 
	}

	// just show on the main widget the strongest
	for (auto &theTr: channel. transmitters) {
	   if (theTr. distance < 0)
	      continue;
	   if (theTr. isStrongest) {
	      QString labelText = createTIILabel (theTr);
	         
	      QFont font	= distanceLabel -> font ();
	      font. setPointSize (9);
	      distanceLabel	-> setFont (font);
	      distanceLabel	-> setText (labelText);
	      break;
	   }
	}

//	if the list has somehow changed, rewrite it
	if (dxMode) {
	   theDXDisplay. cleanUp ();
	   theDXDisplay. show ();
	   theDXDisplay. setChannel (channel. channelName,
	                             channel. ensembleName);
	   int teller_2 = 0;
	   for (auto &theTr : channel. transmitters) {
	      if (!theConfigHandler -> get_allTIISelector ()) {
	         if (theTr. distance < 0) {
	            continue;
	         }
	      }
	      
	      if (bestIndex == localTeller) {
	         QString labelText = createTIILabel (theTr);
	         distanceLabel	-> setText (labelText);
	      }
	      theDXDisplay. addRow (theTr, bestIndex == teller_2);
	      teller_2 ++;
	   }
	}
//
	if (mapViewer == nullptr)
	   return;
//
	for (auto &theTr : channel. transmitters) {
	   if (theTr. transmitterName == "not in database")
	      continue;

	   uint8_t key = theConfigHandler -> showAll_Selector_active () ?
	                                      SHOW_ALL: SHOW_SINGLE;
	   
	   bool utc	= theConfigHandler -> utcSelector_active ();
	   mapHandler_locker. lock ();
	   if (mapViewer != nullptr)
	      mapViewer -> putData (key, theTr, utc);
	   mapHandler_locker. unlock ();
	}
}

const char *directionTable [] = {
	"N", "NE", "E", "SE", "S", "SW", "W", "NW", "N"};

static
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
	labelText		+= theTransmitterName;
	labelText		+= QString ("  ")
	             + "(" + direction + ") "
	             + QString::number (theDistance, 'f', 1) + " km " 
	             + QString::number (theAzimuth, 'f', 1)
	             + QString::fromLatin1 (" \xb0 ");
//	             + "altitude " + QString::number (theAltitude) +  "m "
//	             + "transmitter height " + QString::number (theHeight) +  "m "
//	             + "transmitter power " + QString::number (thePower, 'f', 1) + "kW";
	return labelText;
}

/////////////////////////////////////////////////////////////////////////////
//	Starting a background task is by clicking with the right mouse button
//	on the servicename, swrvice is known to be in current ensemble
void	RadioInterface::handle_backgroundTask (const QString &service) {
	(void)service;
}

void	RadioInterface::handle_labelColor () {
QColor	labelColor;
	QColor color	= QColorDialog::getColor (labelColor,
	                                          nullptr, "labelColor");
	if (!color. isValid ())
	   return;
	labelStyle		= "color:" + color. name ();
	store (theQSettings, COLOR_SETTINGS, LABEL_COLOR, labelStyle);
	QFont font	= serviceLabel -> font ();
	font. setPointSize (16);
	font. setBold (true);
	serviceLabel	-> setStyleSheet (labelStyle);
	serviceLabel	-> setFont (font);
	programTypeLabel	-> setStyleSheet (labelStyle);
}

void	RadioInterface::handle_techFrame_closed () {
	store (theQSettings, DAB_GENERAL, TECHDATA_VISIBLE, 0);
}

void	RadioInterface::handle_configFrame_closed () {
	store (theQSettings, DAB_GENERAL, CONFIG_WIDGET_VISIBLE, 0);
}

void	RadioInterface::handle_deviceFrame_closed () {
	store (theQSettings, DAB_GENERAL, DEVICE_WIDGET_VISIBLE, 0);
}

void	RadioInterface::handle_newDisplayFrame_closed () {
	store (theQSettings, DAB_GENERAL, NEW_DISPLAY_VISIBLE, 0);
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
	   reinterpret_cast<Qt_Audio *>(theAudioPlayer) -> setVolume (n);
	   store (theQSettings, SOUND_HANDLING, QT_AUDIO_VOLUME, audioVolume);
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
	bool b =  theConfigHandler -> get_correlationSelector ();
	store (theQSettings, CONFIG_HANDLER, S_CORRELATION_ORDER, b ? 1 : 0);
	theOfdmHandler -> setCorrelationOrder (b);
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
QString Qt_files	= theFilenameFinder. basicPath ();
	Qt_files	= checkDir (Qt_files);
//
//	we never know whether or not all spaces are removed
	QDesktopServices::openUrl (QUrl (Qt_files, QUrl::TolerantMode));
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
	         static_cast<int>(theTransmitter. altitude),
	         static_cast<int>(theTransmitter. height),
	         theTransmitter. direction. toLatin1 (). data ());
	fclose (theFile);
}

void	RadioInterface::handle_distanceLabel () {
	dxMode	= !dxMode;
	store (theQSettings, CONFIG_HANDLER, S_DX_MODE, dxMode ? 1 : 0);
	theDXDisplay. cleanUp ();
	if (!dxMode) {
	   theDXDisplay. hide ();
	}
	if (dxMode) {
	   theDXDisplay. show ();
	}
	theOfdmHandler -> setDXMode (dxMode);
}

void	RadioInterface::handle_tiiCollisions     (int b) {
	theOfdmHandler	-> setTIICollisions (b);
}

void	RadioInterface::deviceListChanged	() {
#ifndef	TCP_STREAMER
QStringList streams	= reinterpret_cast<Qt_Audio *>(theAudioPlayer) -> streams ();
	theConfigHandler -> fill_streamTable (streams);
	theConfigHandler -> show_streamSelector (true);
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
	      uint32_t ensemble = Ident. toUInt (&ok, 16);
	      if (Eid != ensemble)
	         continue;
//	      if (process_ensemble (theElement, Eid) && true)
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
QString fileName = path_for_epg;
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
	uint32_t attrib = Ident. toUInt (&ok, 16);
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

static
bool	containsPicture (mmDescriptor &set, multimediaElement &m) {
	for (auto &mm : set. elements) {
	   if ((mm. url == m. url) && (mm. width == m. width))
	      return true;
	}
	return false;
}

int	RadioInterface::processService (const QDomElement &service) {
mmDescriptor pictures;
	uint32_t theServiceId =
	             theXmlExtractor. serviceSid (service);
	pictures. serviceId = theServiceId;
	QDomElement mediaDescription =
	            service. firstChildElement ("mediaDescription");
	while (!mediaDescription. isNull ()) {
	   QDomElement multimedia = 
	             mediaDescription. firstChildElement ("multimedia");
	   multimediaElement mE = theXmlExtractor. extract_multimedia (multimedia);
	   if (mE. valid) {
	      pictures. elements. push_back (mE);
	   }
	
	   mediaDescription = mediaDescription.
	                   nextSiblingElement ("mediaDescription");
	}
	for (auto &pictureElement: channel. servicePictures) {
	   uint32_t thisServiceId = pictureElement. serviceId;
	   if (pictures. serviceId != thisServiceId)
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
//	We prefer square pictures, if no one found,
//	then the largest
//	All pictures are stored in groups  (SId is key) in
//	the channel description
QString RadioInterface::findPicture (uint32_t SId, const QString &url) {
	QString pict  = path_for_epg +
	                QString::number (channel. Eid, 16). toUpper ()+
	                "/" + url;
	if (fs::exists (pict. toUtf8 (). data ()))
	   return pict;
	pict  = path_for_epg +
	        QString::number (channel. Eid, 16). toUpper () +
	        "/" + QString::number (SId,16) + "_" + url;
	if (fs::exists (pict. toUtf8 (). data ()))
	   return pict;
	return "";
}

bool	RadioInterface::get_serviceLogo (QPixmap &p, uint32_t SId) {
bool pictureFound	= false;
	for (auto &ss : channel. servicePictures) {
	   if (ss. serviceId != SId)
	      continue;
	   int max		= 0;
	   int sq_max		= 0;
	   for (auto &ff: ss. elements) {
	      QPixmap candidate;
	      QString pict	= findPicture (SId, ff. url);
	      if (pict == "")
	         continue;
	      bool res = candidate. load (pict, "png");
	      if (!res)
	         continue;
	      if ((candidate. height () == candidate. width ()) &&
	                                     (candidate. height () > sq_max)) {
	         p = candidate;
	         pictureFound = true;
	         sq_max = candidate. height ();
	         max	= sq_max;
	      }
	      else
	      if (!pictureFound && (candidate. height () > max)) {
	         max = candidate. height ();
	         p = candidate;
	         pictureFound = true;
	      }
	   }
	}
	return pictureFound;
}
//
void	RadioInterface::read_pictureMappings (uint32_t Eid) {
	QString fileName = path_for_epg;
	fileName += QString::number (Eid, 16). toUpper () + "/list.xml";
	fprintf (stderr, "Looking for %s\n", fileName. toLatin1 (). data ());
	QDomDocument pictureMappings;
	QFile f (fileName);
	if (!f. open (QIODevice::ReadOnly))
	   return;
	pictureMappings. setContent (&f);
	extractServiceInformation	(pictureMappings, Eid, false);
}
//
///////////////////////////////////////////////////////////////////////

void    RadioInterface::handle_FIG09 (int lto,
	                              uint8_t ecc, uint8_t internatTable) {
	if (!channel. hasEcc) {
	   channel. eccByte	= ecc; 
	   channel. hasEcc	= true;
	   channel. lto		= lto;
	   channel. internatTable = internatTable;
	   QString country      = get_ITU_Code (channel. internatTable,
                                                 channel. eccByte,
                                                 (channel. Eid >> 12) &0xF);
           channel. countryName = country;
           channel. strongestTransmitter = "";
           newServices         -> set_countryName (country);
	}

}

void	RadioInterface::handle_FIG021	(uint16_t SId, uint32_t freq) {

	(void)freq;
	if (SId != channel. currentService. SId)
	   return;
	std::vector<uint32_t>  freqList;
	theOfdmHandler -> getFreqs (SId, freqList);
	if (freqList. size () == 0)
	   return;
	channel. currentService. fmFrequencies = freqList;
	theTechWindow	-> updateFM (freqList);
}
//
//	Experimental code for handling DL2 data

void	RadioInterface::show_dl2	(uint8_t ct, uint8_t IT,
	                                              const QString &s) {
	if (!theConfigHandler -> get_saveTitles ())
	   return;
	dl2_handler theHandler (channel. channelName,
	                        channel. ensembleName,
	                        channel. currentService. serviceName,
	                        theQSettings);
	theHandler. show_dl2 (ct, IT, s);
}

/////////////////////////////////////////////////////////////////////////////
//	work in progress
//
void	RadioInterface::nrActiveServices	(int n) {
	theConfigHandler -> set_activeServices (n);
}

void	RadioInterface::handle_activeServices () {
QList<contentType> serviceData = theOfdmHandler -> contentPrint ();
bool	serviceAvailable	= false;
	for (auto &ct : serviceData) {
	   if (theOfdmHandler -> serviceRuns (ct. SId, ct. subChId)) {
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
//	support functions
void	RadioInterface::startJournaline		(int currentKey) {
	if (journalineHandler != nullptr)
	   return;
	journalineHandler	= new journaline_dataHandler ();
	journalineKey		= currentKey;
}

void	RadioInterface::stopJournaline		(int currentKey) {
	if (journalineHandler == nullptr)
	   return;	
	if (journalineKey != currentKey)
	   fprintf (stderr, "What is happenng here %d %d\n",
	                                   journalineKey, currentKey);
	delete 	journalineHandler;
	journalineHandler	= nullptr;
	journalineKey	= -1;
}

void	RadioInterface::journalineData		(QByteArray J_data,
	                                                 int currentKey) {
	if (journalineHandler == nullptr)
	   return;
	if (currentKey != journalineKey)
	   return;
	std::vector<uint8_t> theMscdata (static_cast<uint32_t>(J_data. size ()));
	for (int32_t i = 0; i < J_data. size (); i ++)
	   theMscdata [i] = static_cast<uchar>(J_data [i]);
	journalineHandler	-> add_mscDatagroup (theMscdata);
}

///////////////////////////////////////////////////////////////////////////
//	focus handling
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
	else
	if (this -> hasFocus ()) {
	   theConfigHandler -> activateWindow ();
           theConfigHandler -> setFocus ();
           return true;
	}
	else 
	if (theConfigHandler -> hasFocus ()) {
	   theTechWindow -> activateWindow ();
	   theTechWindow -> setFocus ();
	   return true;
	}
	else
	if (theTechWindow -> hasFocus ()) {
//	   theEnsembleHandler	-> activateWindow ();
//	   theEnsembleHandler	-> setFocus ();
	   return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
//
void	RadioInterface::devSL_visibility	() {
	if (theDeviceChooser. isVisible ())
	   theDeviceChooser. hide ();
	else
	   theDeviceChooser. show ();
}

void	RadioInterface::tell_programType	(uint32_t SId,
	                                                int programType) {
	if (channel. currentService. SId == SId)
	   programTypeLabel	-> setText (getProgramType (channel. internatTable, programType));
}

void	RadioInterface::signal_dataTracer	(bool b) {
	if (theOfdmHandler != nullptr)
	   theOfdmHandler -> set_dataTracer (b);
}

//////////////////////////////////////////////////////////////////////////
//	timetable is triggered by the EPG button
//////////////////////////////////////////////////////////////////////////
void	RadioInterface::handle_startTimeTable	() {
	if (theControl != nullptr) {
	   delete theControl;
	   theControl	= nullptr;
	   return;
	}
	   
	std::vector<basicService> theServices = 
	             theOfdmHandler	-> getServices ();
	fprintf (stderr, "New timetable\n");
	theControl = new timeTableControl (channel. ensembleName,
	                                   channel. Eid,
	                                   theServices,
	                                   channel. theDate,
	                                   theQSettings);
	connect (theControl, &superFrame::frameClosed,
	         this, &RadioInterface::timeTableFrame_closed);
}

void	RadioInterface::timeTableFrame_closed	(){
	if (theControl != nullptr) {	// it better be
	   delete theControl;
	   theControl	= nullptr;
	}
}
//
/////////////experimental////////////////////////////////////////////
//	checking for update

const char *theUrl ="https://api.github.com/repos/jvankatwijk/Qt-DAB/releases/latest";

UpdateChecker *updateChecker = nullptr;
void	RadioInterface::check_newVersion	() {
	connect (&updateCheck_timer, &QTimer::timeout,
                 this, &RadioInterface::check_newVersion);
	fprintf (stderr, "going to check for a new update\n");
	updateChecker = new UpdateChecker (theUrl);
	connect (updateChecker, &UpdateChecker::done,
	         this, &RadioInterface::process_updateCheck);
	updateChecker -> check ();
}

void	RadioInterface::process_updateCheck (bool res) {
	if (!res) {
	   delete updateChecker;
	   updateChecker	= nullptr;
	   return;
	}
	const versionNumber theNewOne (updateChecker -> version ());
	const versionNumber theCurrentOne (VERSION);

	if (!(theNewOne. isValid () && theCurrentOne. isValid ()))
	   return;

	if (theNewOne. value () > theCurrentOne. value ())  {
	   QString message = "Version " + updateChecker -> version () +
	                                               "is now available";
	   QMessageBox::warning (this, tr ("Warning"), message);
	}

	delete updateChecker;
	updateChecker	= nullptr;
}

//////////////////////////////////////////////////////////////////////////
//	support functions
QString	RadioInterface::checkDir (const QString &s) {
QString	dir = s;

	if (!dir. endsWith (QChar ('/')))
	   dir += QChar ('/');

	if (QDir (dir). exists())
	   return dir;
	QDir (). mkpath (dir);
	return dir;
}

void	RadioInterface::reduceButtons	(bool flag) {
	scanButton	-> setEnabled (!flag);
}

void	RadioInterface::handleFontSelect	() {
	if (newServices != nullptr)
	   newServices -> handleFontSelect ();
}

void	RadioInterface::handleFontColorSelect	() {
	if (newServices != nullptr)
	   newServices -> handleFontColorSelect ();
}

void	RadioInterface::handleFontSizeSelect	(int v) {
	if (newServices != nullptr)
	   newServices -> handleFontSizeSelect (v);
}

int	RadioInterface::get_serviceOrder	() {
	return theConfigHandler -> get_serviceOrder ();
}

void	RadioInterface::crc_error (bool b) {
	crcLabel	-> setStyleSheet (b ? "color : red" :  "color : green");
}
//
//	This function is called in the set-up whenever the
//	getLoadSelection selector is set

void	RadioInterface::startList (bool &resetFlag) {
QString fileName;
selector ListSelector ("select option");
	ListSelector. addtoList ("Open existing file");
	ListSelector. addtoList ("Create new servicelist");
	ListSelector. addtoList ("Clear default servicelist");
	ListSelector. addtoList ("Forget it");
	int option = ListSelector. QDialog::exec ();
	       
	switch (option) {
	   case 0: {	// open existing file
	      fileName =            
	             QFileDialog::getOpenFileName (nullptr,
                                                   "Open file ...",
	                                           path_for_serviceLists,
	                                           "*.xml");
	      if (fileName != "")
	         newServices -> startMode (ENSEMBLEVIEW,
	                                 fileName, get_serviceOrder ());
	      else
	         newServices -> startMode (ENSEMBLEVIEW,
	                                         get_serviceOrder ());
	      break;
	   }
	   case 1: {	// create a new file
	      fileName =
	             QFileDialog::getSaveFileName (nullptr,
                                                   "Open file ...",
	                                           path_for_serviceLists,
	                                           "*.xml");
	      if (fileName != "") 
	         newServices -> startMode (ENSEMBLEVIEW,
	                                      fileName,
	                                         get_serviceOrder ());
	      else
	         newServices -> startMode (ENSEMBLEVIEW,
	                                               get_serviceOrder ());
	      break;
	   }
	   case 2:	// empty current default
	      newServices -> startMode (ENSEMBLEVIEW, 
	                                      "", get_serviceOrder ());
	      resetFlag	= true;
	      break;
	   default:
	   case 3:	// selecting this was a mostake
	      newServices -> startMode (ENSEMBLEVIEW,
	                                             get_serviceOrder ());
	      resetFlag	= true;
	}
}

