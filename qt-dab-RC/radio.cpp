#
/*
 *    Copyright (C)  2015, 2023
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
#include	<QColorDialog>
#include	<fstream>
#include	"dab-constants.h"
#include	"mot-content-types.h"
#include	<iostream>
#include	<numeric>
#include	<unistd.h>
#include	<vector>
#include	"radio.h"
#include	"ofdm-handler.h"
#include	"rawfiles.h"
#include	"wavfiles.h"
#include	"xml-filereader.h"
#include	"schedule-selector.h"
#include	"element-selector.h"
#include        "skin-handler.h" 
#include	"dab-tables.h"
#include	"dab-params.h"
#include	"ITU_Region_1.h"
#include	"coordinates.h"
#include	"mapport.h"
#include	"upload.h"
#include	"techdata.h"
#include	"aboutdialog.h"
#ifdef	TCP_STREAMER
#include	"tcp-streamer.h"
#elif	QT_AUDIO
#include	"Qt-audio.h"
#else
#include	"audiosink.h"
#endif
#include	"time-table.h"

#include	"device-exceptions.h"

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
	total_time = std::accumulate (cpu_times. begin(),
	                            cpu_times. end(), (size_t)0);
	return true;
}
#endif

static const
char	LABEL_STYLE [] = "color:lightgreen";

static struct {
	QString	decoderName;
	int	decoderKey;
} decoders []  = {
{"fast decoder", FAST_DECODER},
{"alt1 decoder", ALT1_DECODER},
{"alt2_decoder", ALT2_DECODER},
{"", 0}
};

#define	RESET_BUTTON		QString ("resetButton")
#define SCAN_BUTTON		QString ("scanButton")
#define	SPECTRUM_BUTTON		QString ("spectrumButton")
#define	SNR_BUTTON		QString ("snrButton")
#define	DEVICEWIDGET_BUTTON	QString ("devicewidgetButton")
#define	SCANLIST_BUTTON		QString ("scanListButton")
#define PRESET_BUTTON           QString ("presetButton")
#define	DUMP_BUTTON		QString ("dumpButton")
#define PREVSERVICE_BUTTON	QString ("prevServiceButton")
#define NEXTSERVICE_BUTTON	QString ("nextServiceButton")
#define	DLTEXT_BUTTON		QString	("dlTextButton")
#define	CONFIG_BUTTON		QString ("configButton")
#define	HTTP_BUTTON		QString ("httpButton")
#define	SCHEDULE_BUTTON		QString ("scheduleButton")
#define	SET_COORDINATES_BUTTON	QString ("set_coordinatesButton")
#define	LOAD_TABLE_BUTTON	QString ("loadTableButton")
#define	SKIN_BUTTON		QString ("skinButton")
#define	FONT_BUTTON		QString ("fontButton")
#define	FONTCOLOR_BUTTON	QString ("fontColorButton")
#define	PORT_SELECTOR		QString ("portSelector")

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
	                                        spectrumBuffer (16 * 32768),
	                                        iqBuffer (2 * 1536),
	                                        tiiBuffer (32768),
	                                        nullBuffer (32768),
	                                        channelBuffer (4096),
	                                        snrBuffer (512),
	                                        responseBuffer (32768),
	                                        frameBuffer (2 * 32768),
		                                dataBuffer (32768),
	                                        audioBuffer (8 * 32768),
	                                        stdDevBuffer (2 * 1536),
	                                        newDisplay (this, Si),
	                                        my_snrViewer (this, Si),
	                                        configDisplay (nullptr),
	                                        the_dlCache (10),
	                                        tiiProcessor (),
	                                        filenameFinder (Si),
	                                        theScheduler (this, schedule),
	                                        theTechData (16 * 32768),
	                                        audioConverter (this),
	                                        my_scanListHandler (this,
	                                                        scanListFile),
	                                        chooseDevice (Si),
	                                        scanMonitor (this, Si, freqExtension) {
int16_t k;
QString h;

	dabSettings_p			= Si;
	this	-> error_report		= error_report;
	this	-> fmFrequency		= fmFrequency;
	this	-> dlTextFile		= nullptr;
	this	-> ficDumpPointer	= nullptr;
	this	-> the_aboutLabel	= nullptr;
	running. 		store (false);
	my_ofdmHandler		= nullptr;
	stereoSetting		= false;
	maxDistance		= -1;
	contentTable_p		= nullptr;
	scanTable_p		= nullptr;
	mapHandler		= nullptr;
//	"globals" is introduced to reduce the number of parameters
//	for the ofdmHandler
	globals. spectrumBuffer	= &spectrumBuffer;
	globals. iqBuffer	= &iqBuffer;
	globals. responseBuffer	= &responseBuffer;
	globals. tiiBuffer	= &tiiBuffer;
	globals. nullBuffer	= &nullBuffer;
	globals. channelBuffer	= &channelBuffer;
	globals. snrBuffer	= &snrBuffer;
	globals. frameBuffer	= &frameBuffer;
	globals. stdDevBuffer	= &stdDevBuffer;

	globals. dabMode         =
	          dabSettings_p   -> value ("dabMode", 1). toInt ();
	globals. threshold		=
	          dabSettings_p	-> value ("threshold", 3). toInt();
	globals. diff_length	=
	          dabSettings_p	-> value ("diff_length", DIFF_LENGTH). toInt();
	globals. tii_delay   =
	          dabSettings_p	-> value ("tii_delay", 5). toInt();
	if (globals. tii_delay < 2)
	   globals. tii_delay	= 2;
	globals. tii_depth      =
	          dabSettings_p -> value ("tii_depth", 4). toInt();
	globals. echo_depth     =
	          dabSettings_p -> value ("echo_depth", 1). toInt();

#ifdef	_SEND_DATAGRAM_
	ipAddress		= dabSettings_p -> value ("ipAddress", "127.0.0.1"). toString();
	port			= dabSettings_p -> value ("port", 8888). toInt();
#endif
//	set on top or not? checked at start up
	if (dabSettings_p -> value ("onTop", 0). toInt () == 1) 
	   setWindowFlags (windowFlags () | Qt::WindowStaysOnTopHint);

	for (int i = 0; i < 4; i ++) {
	   QPixmap p;
	   QString labelName	=
	        QString (":res/signal%1.png"). arg (i, 1, 10, QChar ('0'));
	   p. load (labelName, "png");
	   strengthLabels. push_back (p);
	}

//	The settings are done, now creation of the GUI parts
	setupUi (this);
//	and init the up and down button
	{  QPixmap p;
	   if (p. load (":res/up-arrow.png", "png"))
	      prevChannelButton -> setPixmap (p. scaled (30, 30, Qt::KeepAspectRatio));
	   if (p. load (":res/down-arrow.png", "png"))
	      nextChannelButton -> setPixmap (p. scaled (30, 30, Qt::KeepAspectRatio));
	   if (p. load (":res/details24.png", "png"))
	      serviceButton -> setPixmap (p. scaled (30, 30, Qt::KeepAspectRatio));
	   else
	      fprintf (stderr, "Loading details button failed\n");
	}
//
//	put the widgets in the right place
	set_position_and_size (this, "mainWidget");
	configWidget. setupUi (&configDisplay);

	set_position_and_size (&configDisplay, "configWidget");
	the_ensembleHandler	= new ensembleHandler (this, dabSettings_p,
	                                                       presetFile);

#ifdef HAVE_RTLSDR_V3
	SystemVersion	= QString ("5.Beta") + " with RTLSDR-V3";
#elif HAVE_RTLSDR_V4
	SystemVersion	= QString ("5.Beta") + " with RTLSDR-V4";
#else
	SystemVersion	= QString ("5.Beta");
#endif
	setWindowTitle ("Qt-DAB-6." +SystemVersion);

	ensembleWidget -> setWidget (the_ensembleHandler);
	connect (the_ensembleHandler, SIGNAL (selectService (const QString &,
	                                             const QString &)),
	         this, SLOT (localSelect (const QString &, const QString &)));
	connect (the_ensembleHandler,
	             SIGNAL (start_background_task (const QString &)),
	         this, SLOT (start_background_task (const QString &)));

	int fontSize	= 
	         dabSettings_p	-> value ("fontSize", 10). toInt ();
	configWidget. fontSizeSelector -> setValue (fontSize);

	QStringList sl = chooseDevice. getDeviceList ();
	for (auto &sle : sl) 
	   configWidget. deviceSelector -> addItem (sle);
	   
	techWindow_p	= new techData (this, dabSettings_p, &theTechData);
	
	init_configWidget ();

	if (dabSettings_p -> value ("newDisplay", 0). toInt () != 0)
	   newDisplay. show ();
	else
	   newDisplay. hide ();

	labelStyle	= dabSettings_p -> value ("labelStyle",
	                                             LABEL_STYLE). toString ();
	channel. currentService. valid	= false;
	channel. nextService. valid	= false;
	channel. serviceCount		= -1;
	if (dabSettings_p -> value ("has-presetName", 0). toInt () != 0) {
	   configWidget. saveServiceSelector -> setChecked (true);
	   QString presetName		=
	              dabSettings_p -> value ("presetname", ""). toString();
	   if (presetName != "") {
	      QStringList ss = presetName. split (":");
	      if (ss. size () == 2) {
	         channel. nextService. channel	= ss. at (0);
	         channel. nextService. serviceName = ss. at (1);
	         if (the_ensembleHandler -> hasFavorite (ss. at (1))) {
	            the_ensembleHandler -> set_showMode (SHOW_PRESETS);
	            presetButton -> setText ("ensemble");
//	            fprintf (stderr, "Set view to presets\n");
	         }
	      }
	      else {
	         channel. nextService. channel = "";
	         channel. nextService. serviceName = presetName;
	      }
	      channel. nextService. SId		= 0;
	      channel. nextService. SCIds	= 0;
	      channel. nextService. valid	= true;
	   }
	}

	channel. targetPos	= position {0, 0};
	channel. localPos. latitude 		=
	             dabSettings_p -> value ("latitude", 0). toFloat ();
	channel. localPos. longitude 		=
	             dabSettings_p -> value ("longitude", 0). toFloat ();

	logFile			= nullptr;
	peakLeftDamped          = -100;
	peakRightDamped         = -100;

	transmitterTags_local	= configWidget. transmitterTags -> isChecked ();
	techWindow_p 		-> hide ();	// until shown otherwise
	stillMuting		-> hide ();
/*
 */
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

//	Where do we leave the audio out?
	configWidget. streamoutSelector	-> hide();
#ifdef	TCP_STREAMER
	soundOut_p		= new tcpStreamer	(20040);
	techWindow_p		-> hide		();
#elif	QT_AUDIO
	soundOut_p		= new Qt_Audio();
	techWindow_p		-> hide		();
	(void)latency;
#else
//	just sound out
	int latency		= dabSettings_p -> value ("latency", 5). toInt();
	soundOut_p		= new audioSink		(latency);
	((audioSink *)soundOut_p)	-> setupChannels (configWidget. streamoutSelector);
//	QString temp;
	configWidget. streamoutSelector	-> show();
	h	= dabSettings_p -> value ("soundchannel",
	                                         "default"). toString();
//	temp	= "gevonden channel " + h + "\n";
	k	= configWidget. streamoutSelector -> findText (h);
//	temp	= temp + "de index in de combobox = " + QString::number (k);
	if (k != -1) {
	   configWidget. streamoutSelector -> setCurrentIndex (k);
	   bool err = !((audioSink *)soundOut_p) -> selectDevice (k);
	   if (err)
	      ((audioSink *)soundOut_p)	-> selectDefaultDevice();
	}
#endif

#ifndef	__MINGW32__
	path_for_pictures	= checkDir (QDir::tempPath ());
#else
	path_for_pictures	= checkDir (QDir::homePath ());
#endif
	path_for_pictures	+= "Qt-DAB-files/";
	path_for_pictures	= dabSettings_p -> value ("picturesPath",
	                                       path_for_pictures). toString ();
	path_for_pictures	= checkDir (path_for_pictures);

	path_for_files		= dabSettings_p -> value ("filePath", 
	                                       path_for_pictures). toString ();
	if (path_for_files != "")
	   path_for_files = checkDir (path_for_files);
//
#ifndef	__MINGW32__
	epgPath		= checkDir (QDir::tempPath ());
#else
	epgPath		= checkDir (QDir::homePath ());
#endif
	epgPath		+= "Qt-DAB-files/";
	epgPath		= dabSettings_p -> value ("epgPath", epgPath). toString ();
	epgPath		= checkDir (epgPath);
	connect (&epgProcessor,
	         SIGNAL (set_epgData (int, int,
	                              const QString &, const QString &)),
	         this, SLOT (set_epgData (int, int,
	                              const QString &, const QString &)));
//	timer for autostart epg service
	epgTimer. setSingleShot (true);
	connect (&epgTimer, SIGNAL (timeout ()),
	         this, SLOT (epgTimer_timeOut ()));

	my_timeTable		= new timeTableHandler (this);
	my_timeTable		-> hide ();

	connect (&my_scanListHandler,
	               SIGNAL (handle_scanListSelect (const QString &)),
	          this, SLOT (handle_scanListSelect (const QString &)));
	connect (this, SIGNAL (set_newChannel (int)),
	         channelSelector, SLOT (setCurrentIndex (int)));

//	extract the channelnames and fill the combobox
	QStringList res = scanMonitor. getChannelNames ();
	for (auto &s: res)
	  channelSelector -> addItem (s);

	QPalette p	= newDisplay. ficError_display -> palette();
	p. setColor (QPalette::Highlight, Qt::red);
	newDisplay. ficError_display	-> setPalette (p);
	p. setColor (QPalette::Highlight, Qt::green);
//
	audioDumper_p		= nullptr;
	rawDumper_p		= nullptr;
	ficBlocks		= 0;
	ficSuccess		= 0;
	total_ficError		= 0;
	total_fics		= 0;
      
	previous_idle_time	= 0;
	previous_total_time	= 0; 

//	Connect the buttons for the color_settings
	connect	(scanButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_scanButton ()));
	connect (scanListButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_scanListButton ()));
	connect (presetButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_presetButton ()));
	connect (configButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_configButton ()));
	connect (httpButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_httpButton ()));
	connect (prevServiceButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_prevServiceButton ()));
	connect (nextServiceButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_nextServiceButton ()));


	connect (configWidget. resetButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_resetButton ()));
	connect (spectrumButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_spectrumButton ()));
	connect (configWidget. snrButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_snrButton ()));
	connect (configWidget. devicewidgetButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_devicewidgetButton ()));
	connect (configWidget. dumpButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_sourcedumpButton ()));
	connect (configWidget. dlTextButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_dlTextButton ()));
	connect (configWidget. set_coordinatesButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_set_coordinatesButton ()));
	connect (configWidget. loadTableButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_loadTableButton ()));
	connect (configWidget. portSelector, SIGNAL (rightClicked ()),
	         this, SLOT (color_portSelector ()));
	connect (configWidget. skinButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_skinButton ()));
	connect (configWidget. fontButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_fontButton ()));
	connect (configWidget. fontColorButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_fontColorButton ()));
//
//	
	connect (techWindow_p, SIGNAL (handle_timeTable ()),
	         this, SLOT (handle_timeTable ()));
	connect (&newDisplay, SIGNAL (mouseClick ()),
	         this, SLOT (handle_iqSelector ()));
//	display the version

	version		= "Qt-DAB-6." + SystemVersion;
	connect (aboutLabel, SIGNAL (clicked ()),
	         this, SLOT (handle_aboutLabel ()));

	connect (soundLabel, SIGNAL (clicked ()),
	         this, SLOT (handle_muteButton ()));

	QString tiiFileName = QDir::homePath () + "/.txdata.tii";
	tiiFileName = dabSettings_p -> value ("tiiFile", tiiFileName). toString ();
	channel. tiiFile	= false;
	if (tiiFileName != "") {
	   channel. tiiFile = tiiProcessor. tiiFile (tiiFileName);
	   if (!channel. tiiFile) {
	      httpButton -> hide ();
	   }
	}

	channel. etiActive	= false;

	show_pauzeSlide ();

//	and start the timer(s)
//	The displaytimer is there to show the number of
//	seconds running and handle - if available - the tii data
	displayTimer. setInterval (1000);
	connect (&displayTimer, SIGNAL (timeout ()),
	         this, SLOT (updateTimeDisplay ()));
	displayTimer. start (1000);
	numberofSeconds		= 0;

//	timer for scanning
	channelTimer. setSingleShot (true);
	channelTimer. setInterval (10000);
	connect (&channelTimer, SIGNAL (timeout ()),
	         this, SLOT (channel_timeOut ()));
//
//	presetTimer
	presetTimer. setSingleShot (true);
	connect (&presetTimer, SIGNAL (timeout ()),
	         this, SLOT (setPresetService ()));
//
//	timer for muting
	muteTimer. setSingleShot (true);
	set_Colors ();
//
//	do we habe a known device from previous invocations?
	inputDevice_p	= nullptr;
	h               =
	           dabSettings_p -> value ("device", "no device"). toString();
	k               = configWidget. deviceSelector -> findText (h);
	if (k != -1) {
	   configWidget. deviceSelector -> setCurrentIndex (k);
	   QString deviceName	= configWidget. deviceSelector -> currentText ();
	   inputDevice_p = create_device (configWidget. deviceSelector -> currentText());
	}
//
//	do we show controls?
	bool hidden	=
	            dabSettings_p	-> value ("hidden", 0). toInt () != 0;
	if (hidden) {
	   configButton	-> setText ("show controls");
	   configDisplay. hide ();	
	}
	else {
	   configButton -> setText ("hide controls");
	   configDisplay. show ();
	}

	connect (configButton, SIGNAL (clicked ()),
	         this, SLOT (handle_configButton ()));

	if (dabSettings_p -> value ("snrVisible", 0). toInt () != 0)
	   my_snrViewer. show ();
	else
	   my_snrViewer. hide ();
	if (dabSettings_p -> value ("techDataVisible", 0). toInt () == 1)
	   techWindow_p -> show ();

	dynamicLabel	-> setTextInteractionFlags(Qt::TextSelectableByMouse);
	dynamicLabel    -> setToolTip ("The text (or parts of it) of the dynamic label can be copied. Selecting the text with the mouse and clicking the right hand mouse button shows a small menu with which the text can be put into the clipboard");

//	if a device was selected, we just start, otherwise
//	we wait until one is selected

	if (inputDevice_p != nullptr) {
	   if (doStart ()) {
	      qApp	-> installEventFilter (this);
	      return;
	   }
	   else {
	      delete inputDevice_p;
	      inputDevice_p	= nullptr;
	   }
	}
	if (hidden) { 	// make it visible
	   configButton		-> setText ("hide controls");
	   dabSettings_p	-> setValue ("hidden", 0);
	   dabSettings_p	-> setValue ("deviceVisible", 1);
	}
	configDisplay. show ();
	connect (configWidget. deviceSelector,
	                               SIGNAL (activated (const QString &)),
	         this,  SLOT (doStart (const QString &)));
	qApp	-> installEventFilter (this);
}
//
//	doStart (QString) is called when - on startup - NO device
//	was registered to be used, and the user presses the
//	selectDevice comboBox
void	RadioInterface::doStart (const QString &dev) {
	(void)dev;
	inputDevice_p	= create_device	(dev);
//	Some buttons should not be touched before we have a device
	if (inputDevice_p == nullptr) {
	   fprintf (stderr, "disconnecting\n");
	   disconnectGUI ();
	   return;
	}
	doStart ();
}
//
//	when doStart is called, a device is available and selected
bool	RadioInterface::doStart	() {
	if (channel. nextService. channel != "") {
	   int k = channelSelector -> findText (channel. nextService. channel);
	   if (k != -1) 
	      channelSelector -> setCurrentIndex (k);
	}
	else
	   channelSelector -> setCurrentIndex (0);

	my_ofdmHandler	= new ofdmHandler  (this,
	                                    inputDevice_p, &globals, dabSettings_p);
	channel. cleanChannel ();

	the_ensembleHandler	-> reset	();
	the_ensembleHandler	-> setMode (!inputDevice_p -> isFileInput ());
	fprintf (stderr, "Connecting GUI\n");
	connectGUI ();

	if (dabSettings_p -> value ("showDeviceWidget", 0).  toInt () != 0)
	   inputDevice_p -> setVisibility (true);

	
	connect (channelSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (handle_channelSelector (const QString &)));
//
//	Just to be sure we disconnect here.
//	It would have been helpful to have a function
//	testing whether or not a connection exists, we need a kind
//	of "reset"
	disconnect (configWidget. deviceSelector,
	                               SIGNAL (activated (const QString &)),
	            this, SLOT (doStart (const QString &)));
	disconnect (configWidget. deviceSelector,
	                              SIGNAL (activated (const QString &)),
	            this,  SLOT (newDevice (const QString &)));
	connect (configWidget. deviceSelector,
	                              SIGNAL (activated (const QString &)),
	         this, SLOT (newDevice (const QString &)));
//
	if (channel. nextService. valid) {
	   int switchDelay	= configWidget. switchDelaySetting -> value ();
	   presetTimer. setSingleShot	(true);
	   presetTimer. setInterval 	(switchDelay * 1000);
	   presetTimer. start 		(switchDelay * 1000);
	}

	if (configWidget. tii_detectorMode -> isChecked ())
	   my_ofdmHandler -> set_tiiDetectorMode (true);
//
//	after the preset timer signals, the service will be started
	startChannel (channelSelector -> currentText ());
	running. store (true);
	return true;
}

	RadioInterface::~RadioInterface () {
	fprintf (stderr, "radioInterface is deleted\n");
}
//
///////////////////////////////////////////////////////////////////////////////
//	
//
//	might be called when scanning only
void	RadioInterface::channel_timeOut () {
	no_signal_found ();
}

///////////////////////////////////////////////////////////////////////////
//
//	a slot, called by the fic/fib handlers
void	RadioInterface::add_to_ensemble (const QString &serviceName,
	                                             int32_t SId) {
	if (!running. load())
	   return;

	int subChId = my_ofdmHandler -> get_subCh_id (serviceName, SId);
	if (subChId < 0)
	   return;

	serviceId ed;
	ed. name	= serviceName;
	ed. SId		= SId;
	ed. subChId	= subChId;
	ed. channel	= channel. channelName;

	bool added	= the_ensembleHandler -> add_to_ensemble (ed);
	if (added) {
	   channel. nrServices ++;
	   if (scanMonitor. active ())
	      scanMonitor. addService (channel. channelName);
	   if (scanMonitor. active () && !scanMonitor. scan_to_data ()) {
	      my_scanListHandler. addElement (channel. channelName,
	                                              serviceName);
	   }
	}
	if ((channel. serviceCount == channel. nrServices)&& 
	                     !scanMonitor. active ()) {
	   presetTimer. stop ();
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

	QFont font	= ensembleId -> font ();
	font. setPointSize (14);
	
	ensembleId	-> setFont (font);
	ensembleId	-> setText (v + QString ("(") + hextoString (id) + QString (")"));

	channel. ensembleName	= v;
	channel. Eid		= id;
//
//	id we are scanning "to data", we reached the end
	if (scanMonitor. scan_to_data ())
	   stopScanning ();
	else
	if (scanMonitor. active () && scanMonitor. scan_single ())
	   scanMonitor. addEnsemble (channelSelector -> currentText (), v);
	                           
//	... and is we are not scanning, clicking the ensembleName
//	has effect
	if (!scanMonitor. active ())
	   connect (ensembleId, SIGNAL (clicked ()),
	            this, SLOT (handle_contentButton ()));
}
//
///////////////////////////////////////////////////////////////////////////

void	RadioInterface::handle_contentButton	() {
QStringList s	= my_ofdmHandler -> basicPrint ();

	if (contentTable_p != nullptr) {
	   contentTable_p -> hide ();
	   delete contentTable_p;
	   contentTable_p = nullptr;
	   return;
	}
	QString theTime;
	QString SNR	= "SNR " + QString::number (channel. snr);
	if (configWidget. utcSelector -> isChecked ())
	   theTime	= convertTime (UTC);
	else
	   theTime	= convertTime (localTime);

	QString header		= channel. ensembleName + ";" +
	                          channel. channelName  + ";" +
	                          QString::number (channel. tunedFrequency) + ";" +
	                          hextoString (channel. Eid) + " " + ";" +
	                          transmitter_coordinates -> text () + " " + ";" +
	                          theTime  + ";" +
	                          SNR  + ";" +
	                          QString::number (channel. nrServices) + ";" +
	                          distanceLabel -> text () + "\n";

	contentTable_p		= new contentTable (this, dabSettings_p,
	                                            channel. channelName,
	                                            my_ofdmHandler -> scanWidth ());
	connect (contentTable_p, SIGNAL (goService (const QString &)),
	         this, SLOT (handle_contentSelector (const QString &)));

	contentTable_p		-> addLine (header);
//	contentTable_p		-> addLine ("\n");
	for (int i = 0; i < s. size (); i ++) 
	   contentTable_p	-> addLine (s. at (i));
	contentTable_p -> show ();
}

QString	RadioInterface::checkDir (const QString s) {
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

	      {
	         QString temp = objectName;
	         temp = temp. left (temp. lastIndexOf (QChar ('/')));
	         if (!QDir (temp). exists ())
	            QDir (). mkpath (temp);	

	         std::vector<uint8_t> epgData (result. begin(),
	                                                  result. end());
//	         uint32_t ensembleId =
//	                     my_ofdmHandler -> get_ensembleId ();
	         uint32_t currentSId =
	                     the_ensembleHandler -> extract_SId  (objectName);
	         uint32_t julianDate	=
	                     my_ofdmHandler -> julianDate ();
	         int subType = 
	                  getContentSubType ((MOTContentType)contentType);
	         epgProcessor. process_epg (epgData. data (), 
	                                    epgData. size (), currentSId,
	                                    subType,
	                                    julianDate);
	         if (configWidget. epg2xmlSelector -> isChecked ()) {
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
	   fprintf (stderr, "going to write file %s\n",
	                            textName. toUtf8(). data());
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

	if (configWidget. saveSlides  -> isChecked () 
	                               && (path_for_pictures != "")) {
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
	      fprintf (stderr, "going to write file %s\n",
	                            pict. toUtf8(). data());
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
uint8_t localBuffer [length];

	if (dataBuffer. GetRingBufferReadAvailable() < length) {
	   fprintf (stderr, "Something went wrong\n");
	   return;
	}

	dataBuffer. getDataFromBuffer (localBuffer, length);
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
	   dataStreamer_p -> sendData (localBuffer, length + 8);
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
	if (!running. load () || my_ofdmHandler == nullptr)
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
	   my_ofdmHandler -> reset_etiGenerator ();
	for (uint16_t i = 0; i < channel. backgroundServices. size (); i ++)
	   my_ofdmHandler -> stop_service (channel. backgroundServices. at (i). subChId,
	                                   BACK_GROUND);

	fprintf (stderr, "change will be effected\n");
//	we rebuild the services list from the fib and
//	then we (try to) restart the service
	std::vector<serviceId> serviceList	=
	          my_ofdmHandler -> get_services (SUBCH_BASED);
	the_ensembleHandler -> reset	();
	channel. nrServices = 0;
	for (auto &serv: serviceList) {
	   serv . channel = channel. channelName;
	   if (the_ensembleHandler -> add_to_ensemble (serv))
	      channel. nrServices  ++;
	}

//	Of course, the (sub)service may have disappeared
	if (s. valid) {
	   QString ss = my_ofdmHandler -> find_service (s. SId, s. SCIds);
	   if (ss != "") {
	      startService (s);
	      return;
	   }
//
//	The service is gone, it may be the subservice of another one
	   s. SCIds = 0;
	   s. serviceName =
	               my_ofdmHandler -> find_service (s. SId, s. SCIds);
	   if (s. serviceName != "") {
	      startService (s);
	   }
	}
//
//	we also have to restart all background services,
	for (uint16_t i = 0; i < channel. backgroundServices. size (); i ++) {
	   QString ss = my_ofdmHandler -> find_service (s. SId, s. SCIds);
	   if (ss == "") {	// it is gone, close the file if any
	      if (channel. backgroundServices. at (i). fd != nullptr)
	         fclose (channel. backgroundServices. at (i). fd);
	      channel. backgroundServices. erase
	                        (channel. backgroundServices. begin () + i);
	   }
	   else {	// (re)start the service
	      if (my_ofdmHandler -> is_audioservice (ss)) {
	         audiodata ad;
	         FILE *f = channel. backgroundServices. at (i). fd;
	         my_ofdmHandler -> data_for_audioservice (ss, ad);
	         my_ofdmHandler -> 
	                   set_audioChannel (ad, &audioBuffer, f, BACK_GROUND);	       
	         channel. backgroundServices. at (i). subChId  = ad. subchId;
	      }
	      else {
	         packetdata pd;
	         my_ofdmHandler -> data_for_packetservice (ss, &pd, 0);
	         my_ofdmHandler -> 
	                   set_dataChannel (pd, &dataBuffer, BACK_GROUND);	       
	         channel. backgroundServices. at (i). subChId     = pd. subchId;
	      }

	   }
	}
}
//
//	In order to not overload with an enormous amount of
//	signals, we trigger this function at most 10 times a second
//
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
	while (audioBuffer. GetRingBufferReadAvailable () > amount) {
	   audioBuffer. getDataFromBuffer (vec, amount);
#ifdef	HAVE_PLUTO_RXTX
	   if (streamerOut_p != nullptr)
	      streamerOut_p	-> audioOut (vec, amount, rate);
#endif
//
	   std::vector<float> tmpBuffer;
	   int size = audioConverter. convert (vec, amount, rate, tmpBuffer);
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
	scanMonitor. hide ();
	stopScanning ();
	while (scanMonitor. active ())
	   usleep (1000);
	hideButtons	();

	store_widget_position (this, "mainWidget");
	store_widget_position (&configDisplay, "configWidget");

	newDisplay. hide ();
//
#ifdef	DATA_STREAMER
	fprintf (stderr, "going to close the dataStreamer\n");
	delete		dataStreamer_p;
#endif
#ifdef	CLOCK_STREAMER
	fprintf (stderr, "going to close the clockstreamer\n");
	delete	clockStreamer_p;
#endif
	if (mapHandler != nullptr)
	   mapHandler ->  stop ();
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
	if (my_ofdmHandler != nullptr)
	   my_ofdmHandler -> stop ();
	the_ensembleHandler	-> hide ();
	delete	the_ensembleHandler;
	techWindow_p	-> hide ();
	delete techWindow_p;
	if (contentTable_p != nullptr) {
	   contentTable_p -> clearTable ();
	   contentTable_p -> hide ();
	   delete contentTable_p;
	}
//	just save a few checkbox settings that are not
//	bound by signal/slots, but just read if needed
	dabSettings_p	-> setValue ("utcSelector",
	                          configWidget. utcSelector -> isChecked () ? 1 : 0);
	dabSettings_p	-> setValue ("epg2xml",
	                          configWidget. epg2xmlSelector -> isChecked () ? 1 : 0);

	if (scanTable_p != nullptr) {
	   scanTable_p	-> clearTable ();
	   scanTable_p	-> hide ();
	   delete scanTable_p;
	}
	scanMonitor. hide ();

//	theBand. saveSettings	();
	stopFramedumping	();
	stop_sourcedumping	();
	stopAudiodumping	();
//	theTable. hide		();
//	theBand. hide		();
	theScheduler. hide	();
	configDisplay. hide	();
	LOG ("terminating ", "");
	usleep (1000);		// pending signals
	if (logFile != nullptr)
	   fclose (logFile);
	logFile	= nullptr;
//	everything should be halted by now
	dabSettings_p	-> sync ();
	my_snrViewer. hide ();
	if (my_ofdmHandler != nullptr)
	   delete	my_ofdmHandler;
	if (inputDevice_p != nullptr)
	   delete	inputDevice_p;
	delete		soundOut_p;
	my_scanListHandler. hide ();
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
	   size_t idle_time, total_time;
	   get_cpu_times (idle_time, total_time);
	   const float idle_time_delta =
	                 static_cast<float>(idle_time - previous_idle_time);
	   const float total_time_delta =
	                 static_cast<float> (total_time - previous_total_time);
	   const float utilization = 100.0 * (1.0 - idle_time_delta / total_time_delta);
//	   cpuMonitor -> display (utilization);
//	   cpuMonitor -> display (QString("%1").arg(utilization, 0, 'f', 2));
	   previous_idle_time = idle_time;
	   previous_total_time = total_time;
	}
//
//	The timer runs autonomously, so it might happen
//	that it rings when there is no processor running
	if (my_ofdmHandler == nullptr)
	   return;
#if !defined (TCP_STREAMER) && !defined (QT_AUDIO)
	if (!techWindow_p -> isHidden ()) {
	   if (soundOut_p -> hasMissed ()) {
	      int xxx = ((audioSink *)soundOut_p) -> missed ();
	      techWindow_p -> showMissed (xxx);
	   }
	}
#endif
	if (error_report && (numberofSeconds % 10) == 0) {
	   int	totalFrames;
	   int	goodFrames;
	   int	badFrames;
	   my_ofdmHandler	-> get_frameQuality (&totalFrames,
	                                             &goodFrames,
	                                             &badFrames);
	   fprintf (stderr, "total %d, good %d bad %d ficRatio %f\n",
	                     totalFrames, goodFrames, badFrames,
	                                            total_ficError * 100.0 / total_fics);
	   total_ficError	= 0;
	   total_fics		= 0;
#ifndef TCP_STREAMER 
#ifndef	QT_AUDIO
	   if (configWidget. streamoutSelector -> isVisible ()) {
	      int xxx = ((audioSink *)soundOut_p)	-> missed();
	      fprintf (stderr, "missed %d\n", xxx);
	   }
#endif
#endif
	}
}
//
//	precondition: everything is quiet
deviceHandler	*RadioInterface::create_device (const QString &s) {
deviceHandler	*inputDevice = chooseDevice. createDevice  (s, version);

	if (inputDevice	== nullptr)
	   return nullptr;

	channel. realChannel	= !inputDevice -> isFileInput ();
	if (channel. realChannel)
	   showButtons ();
	else 
	   hideButtons ();
	the_ensembleHandler -> setMode (channel. realChannel);

	dabSettings_p	-> setValue ("device", s);
	if (dabSettings_p -> value ("deviceVisible", 1). toInt () != 0)
	   inputDevice -> setVisibility (true);
	else
	   inputDevice -> setVisibility (false);
	newDisplay. set_bitDepth (inputDevice -> bitDepth ());
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
	disconnectGUI	();
	if (inputDevice_p != nullptr) {
	   delete inputDevice_p;
	   inputDevice_p = nullptr;
	   fprintf (stderr, "device is deleted\n");
	}

	LOG ("selecting ", deviceName);
	inputDevice_p		= create_device (deviceName);
	if (inputDevice_p == nullptr) {
	   inputDevice_p = new deviceHandler ();
	   return;		// nothing will happen
	}
	doStart();		// will set running
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
	   clockStreamer_p -> sendData (localBuffer, 10);
#endif
	this	-> UTC. year		= year;
	this	-> UTC. month		= month;
	this	-> UTC. day		= d2;
	this	-> UTC. hour		= h2;
	this	-> UTC. minute		= m2;
	QString result;
	if (configWidget. utcSelector -> isChecked ())
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
//	called from the aac decoder
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
	   QPalette p      = newDisplay. ficError_display -> palette();
	   if (ficSuccess < 85)
	      p. setColor (QPalette::Highlight, Qt::red);
	   else
	      p. setColor (QPalette::Highlight, Qt::green);

	   newDisplay. ficError_display	-> setPalette (p);
	   newDisplay. ficError_display	-> setValue (ficSuccess);
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
	newDisplay. set_syncLabel (b);
}
//
//	called from the PAD handler

void	RadioInterface::show_label	(const QString &s) {
#ifdef	HAVE_PLUTO_RXTX
	if ((streamerOut_p != nullptr) && (s != ""))
	   streamerOut_p -> addRds (std::string (s. toUtf8 (). data ()));
#endif
	if (running. load()) {
//	   dynamicLabel -> setWordWrap (true);
	   dynamicLabel	-> setStyleSheet (labelStyle);
	   dynamicLabel	-> setText (s);
	}
//	if we have dtText is ON, some work is still to be done
	if ((s == "") || (dlTextFile == nullptr) ||
	                                (the_dlCache. addifNew (s)))
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
	if (!running. load ())
	   return;
	if (stereoSetting == b)
	   return;
	if (b) {
	   stereoLabel	-> setStyleSheet (labelStyle);
	   stereoLabel	-> setText ("<i>stereo</i>");
	}
	else
	   stereoLabel	-> setText ("      ");
	   
	stereoSetting = b;
}
//
static
QString	tiiNumber (int n) {
	if (n >= 10)
	   return QString::number (n);
	return QString ("0") + QString::number (n);
}

//
////////////////////////////////////////////////////////////////////////////


void	RadioInterface::switchVisibility (QWidget *w) {
	if (w -> isHidden ())
	   w  -> show ();
	else
	   w -> hide ();
}

void	RadioInterface::handle_detailButton	() {
	if (!running. load ())
	   return;
	if (techWindow_p -> isHidden ())
	   techWindow_p -> show ();
	else
	   techWindow_p -> hide ();
	dabSettings_p -> setValue ("techDataVisible",
	                     techWindow_p -> isHidden () ? 0 : 1);
}
//
//	Whenever the input device is a file, some functions,
//	e.g. selecting a channel, setting an alarm, are not
//	meaningful
void	RadioInterface::showButtons		() {
	configWidget. dumpButton	-> setEnabled (true);
	scanButton		-> setEnabled (true);;
	channelSelector		-> setEnabled (true);
	nextChannelButton	-> setEnabled (true);
	prevChannelButton	-> setEnabled (true);
}

void	RadioInterface::hideButtons		() {
	configWidget. dumpButton	-> setEnabled (false);
	scanButton		-> setEnabled (false);
	channelSelector		-> setEnabled (false);
	nextChannelButton	-> setEnabled (false);
	prevChannelButton	-> setEnabled (false);
}

void	RadioInterface::set_sync_lost	() {
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

void	RadioInterface::handle_audiodumpButton () {
	if (!running. load () || scanMonitor. active ())
	   return;

	if (audioDumper_p != nullptr) 
	   stopAudiodumping ();	
	else
	   startAudiodumping ();
}

void	RadioInterface::stopAudiodumping	() {
	if (audioDumper_p == nullptr)
	   return;

	LOG ("audiodump stops", "");
	audioConverter. stop_audioDump ();
	sf_close (audioDumper_p);
	audioDumper_p	= nullptr;
	techWindow_p	-> audiodumpButton_text ("audio dump", 10);
}

void	RadioInterface::startAudiodumping () {
	audioDumper_p	=
	      filenameFinder.
	           findAudioDump_fileName  (channel. currentService. serviceName, true);
	if (audioDumper_p == nullptr)
	   return;

	LOG ("audiodump starts ", serviceLabel -> text ());
	techWindow_p	-> audiodumpButton_text ("writing", 12);
	audioConverter. start_audioDump (audioDumper_p);
}

void	RadioInterface::scheduled_audioDumping () {
	if (audioDumper_p != nullptr) {
	   audioConverter. stop_audioDump	();
	   sf_close (audioDumper_p);
	   audioDumper_p	= nullptr;
	   LOG ("scheduled audio dump stops ", serviceLabel -> text ());
	   techWindow_p	-> audiodumpButton_text ("audio dump", 10);
	   return;
	}

	audioDumper_p	=
	      filenameFinder.
	            findAudioDump_fileName  (serviceLabel -> text (), false);
	if (audioDumper_p == nullptr)
	   return;

	LOG ("scheduled audio dump starts ", serviceLabel -> text ());
	techWindow_p	-> audiodumpButton_text ("writing", 12);
	audioConverter. start_audioDump (audioDumper_p);
}

void	RadioInterface::handle_framedumpButton () {
	if (!running. load () || scanMonitor. active ())
	   return;

	if (channel. currentService. frameDumper != nullptr) 
	   stopFramedumping ();
	else
	   startFramedumping ();
}

void	RadioInterface::stopFramedumping () {
	if (channel. currentService. frameDumper == nullptr)
	   return;

	fclose (channel. currentService. frameDumper);
	techWindow_p ->  framedumpButton_text ("frame dump", 10);
	channel. currentService. frameDumper	= nullptr;
}

void	RadioInterface::startFramedumping () {
	channel. currentService. frameDumper	=
	     filenameFinder. findFrameDump_fileName (channel. currentService. serviceName,
	                                                              true);
	if (channel. currentService. frameDumper == nullptr)
	   return;
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
	     filenameFinder. findFrameDump_fileName (s, false);
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
uint8_t buffer [amount];
	if (!running. load ())
	   return;

	if (channel. currentService. frameDumper == nullptr) 
	   frameBuffer. FlushRingBuffer ();
	else
	while (frameBuffer. GetRingBufferReadAvailable () >= amount) {
	   frameBuffer. getDataFromBuffer (buffer, amount);
	   if (channel. currentService. frameDumper != nullptr)
	      fwrite (buffer, amount, 1, channel. currentService. frameDumper);
	}
}

void	RadioInterface::handle_spectrumButton	() {
	if (!running. load ())
	   return;
	if (newDisplay. isHidden ())
	   newDisplay. show ();
	else
	   newDisplay. hide ();
	dabSettings_p	-> setValue ("newDisplay", 
	                             newDisplay. isHidden () ? 0 : 1);
}


void	RadioInterface::handle_scanListButton    () {
	if (!running. load ())
	   return;

	if (my_scanListHandler. isHidden ())
	   my_scanListHandler. show ();
	else
	   my_scanListHandler. hide ();
}
//
//	When changing (or setting) a device, we do not want anybody
//	to have the buttons on the GUI touched, so
//	we just disconnet them and (re)connect them as soon as
//	a device is operational
void	RadioInterface::connectGUI	() {
//	connections from the main widget
	connect (prevServiceButton, SIGNAL (clicked ()), 
	         this, SLOT (handle_prevServiceButton ()));
	connect (nextServiceButton, SIGNAL (clicked ()), 
	         this, SLOT (handle_nextServiceButton ()));

//	channelButton handled elsewjere
	connect	(prevChannelButton, SIGNAL (clicked ()),
	         this, SLOT (handle_prevChannelButton ()));
	connect (nextChannelButton, SIGNAL (clicked ()),
	         this, SLOT (handle_nextChannelButton ()));

	connect (scanListButton, SIGNAL (clicked ()),
	         this, SLOT (handle_scanListButton ()));
	               
	connect (presetButton, SIGNAL (clicked ()),
	         this, SLOT (handle_presetButton ()));

	connect (configWidget. scheduleButton, SIGNAL (clicked ()),
	         this, SLOT (handle_scheduleButton ()));
//
//
//	The main control part
//	Handling the visibility of the control widget
//	is handled separately
	connect (spectrumButton, SIGNAL (clicked ()),
	         this, SLOT (handle_spectrumButton ()));
	connect (serviceLabel, SIGNAL (clicked ()),
	         this, SLOT (handle_labelColor ()));
	connect (serviceButton, SIGNAL (clicked ()),
	         this, SLOT (handle_detailButton ()));
//
	connect (httpButton, SIGNAL (clicked ()),
	         this, SLOT (handle_httpButton ()));
//	connect (ensembleId, SIGNAL (clicked ()),
//	         this, SLOT (handle_contentButton ()));
	connect (scanButton, SIGNAL (clicked ()),
	         this, SLOT (handle_scanButton ()));
//
//	and for the techWindow
	connect (techWindow_p, SIGNAL (handle_audioDumping ()),
	         this, SLOT (handle_audiodumpButton ()));
	connect (techWindow_p, SIGNAL (handle_frameDumping ()),
	         this, SLOT (handle_framedumpButton ()));
//
//	and the configuration window
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
//
	connect (configWidget. fontButton, SIGNAL (clicked ()),
	         the_ensembleHandler, SLOT (handle_fontSelect ()));
	connect (configWidget. fontColorButton, SIGNAL (clicked ()),
	         the_ensembleHandler, SLOT (handle_fontColorSelect ()));

	connect (configWidget. fontSizeSelector, SIGNAL (valueChanged (int)),
	         the_ensembleHandler, SLOT (handle_fontSizeSelect (int)));
//
//	Now the two rows with buttons
//
	connect (configWidget. devicewidgetButton, SIGNAL (clicked ()),
	         this, SLOT (handle_devicewidgetButton ()));
	connect (configWidget. portSelector, SIGNAL (clicked ()),
	         this, SLOT (handle_portSelector ()));
	connect (configWidget. dlTextButton, SIGNAL (clicked ()),
	         this, SLOT (handle_dlTextButton ()));
	connect (configWidget. resetButton, SIGNAL (clicked ()),
	         this, SLOT (handle_resetButton ()));
//
//	second row
	connect (configWidget. snrButton, SIGNAL (clicked ()),
	         this, SLOT (handle_snrButton ()));
	connect (configWidget. set_coordinatesButton, SIGNAL (clicked ()),
	         this, SLOT (handle_set_coordinatesButton ()));
	connect (configWidget. loadTableButton, SIGNAL (clicked ()),
	         this, SLOT (handle_loadTable ()));
	connect (configWidget. dumpButton, SIGNAL (clicked ()),
	         this, SLOT (handle_sourcedumpButton ()));
	connect (configWidget. skinButton, SIGNAL (clicked ()),
	         this, SLOT (handle_skinSelector ()));
//
//	Now the checkboxes
//	top line
	connect (configWidget. saveServiceSelector,
	                                  SIGNAL (stateChanged (int)),
	         this, SLOT (handle_saveServiceSelector (int)));
	connect (configWidget. loggerButton, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_LoggerButton (int)));
//	the epg2xmlSelector is kust polled, no need to react on an event

//	second line
	connect (configWidget. tii_detectorMode, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_tii_detectorMode (int)));
//
//	utcSelector is just polled, no need to react on an event here
	connect (configWidget. onTop, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_onTop (int)));
//
//	third line
//	here we expect the close without asking

	connect (configWidget. epgSelector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_epgSelector (int)));
	connect (configWidget. autoBrowser, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_autoBrowser      (int)));
//
//	fourth line
	connect (configWidget. dcRemovalSelector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_dcRemovalSelector (int)));
//	

	connect (configWidget. eti_activeSelector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_eti_activeSelector (int)));
//
	connect (configWidget. transmSelector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_transmSelector (int)));
//
//	fifh line
	connect (configWidget. clearScan_Selector,
	                             SIGNAL (stateChanged (int)),
	         this, SLOT (handle_clearScan_Selector (int)));

	connect (configWidget. saveSlides, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_saveSlides (int)));
//
	connect (configWidget. transmitterTags, SIGNAL (stateChanged (int)),	
	         this, SLOT (handle_transmitterTags  (int)));
//
//	botton row
	connect (configWidget. decoderSelector,
	                            SIGNAL (activated (const QString &)),
	         this, SLOT (handle_decoderSelector (const QString &)));
	connect (configWidget. streamoutSelector, SIGNAL (activated (int)),
	         this,  SLOT (set_streamSelector (int)));
}

void	RadioInterface::disconnectGUI () {
//	disconnections from the main widget
	disconnect (prevServiceButton, SIGNAL (clicked ()),
	            this, SLOT (handle_prevServiceButton ()));
	disconnect (nextServiceButton, SIGNAL (clicked ()),
	            this, SLOT (handle_nextServiceButton ()));
//	channelButton handled elsewjere
	disconnect (prevChannelButton, SIGNAL (clicked ()),
	         this, SLOT (handle_prevChannelButton ()));
	disconnect (nextChannelButton, SIGNAL (clicked ()),
	         this, SLOT (handle_nextChannelButton ()));

	disconnect (scanListButton, SIGNAL (clicked ()),
	         this, SLOT (handle_scanListButton ()));
	disconnect (presetButton, SIGNAL (clicked ()),
	         this, SLOT (handle_presetButton ()));

//
//	The main control part
//	Handling the visibility of the control widget
//	is handled separately
	disconnect (spectrumButton, SIGNAL (clicked ()),
	         this, SLOT (handle_spectrumButton ()));
	disconnect (serviceLabel, SIGNAL (clicked ()),
	         this, SLOT (handle_detailButton ()));
//
	disconnect (httpButton, SIGNAL (clicked ()),
	         this, SLOT (handle_httpButton ()));
	disconnect (ensembleId, SIGNAL (clicked ()),
	         this, SLOT (handle_contentButton ()));
	disconnect (scanButton, SIGNAL (clicked ()),
	         this, SLOT (handle_scanButton ()));
//
//	and for the techWindow
	disconnect (techWindow_p, SIGNAL (handle_audioDumping ()),
	         this, SLOT (handle_audiodumpButton ()));
	disconnect (techWindow_p, SIGNAL (handle_frameDumping ()),
	         this, SLOT (handle_framedumpButton ()));
//
//	and the configuration window
	disconnect (configWidget. muteTimeSetting, SIGNAL (valueChanged (int)),
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
//
	disconnect (configWidget. fontButton, SIGNAL (clicked ()),
	            the_ensembleHandler, SLOT (handle_fontSelect ()));

	disconnect (configWidget. fontColorButton, SIGNAL (clicked ()),
	            the_ensembleHandler, SLOT (handle_fontColorSelect ()));
//
//	Now the two rows with buttons
//
	disconnect (configWidget. devicewidgetButton, SIGNAL (clicked ()),
	            this, SLOT (handle_devicewidgetButton ()));
	disconnect (configWidget. portSelector, SIGNAL (clicked ()),
	         this, SLOT (handle_portSelector ()));
	disconnect (configWidget. dlTextButton, SIGNAL (clicked ()),
	         this, SLOT (handle_dlTextButton ()));
	disconnect (configWidget. resetButton, SIGNAL (clicked ()),
	         this, SLOT (handle_resetButton ()));

//	second row
	disconnect (configWidget. snrButton, SIGNAL (clicked ()),
	         this, SLOT (handle_snrButton ()));
	disconnect (configWidget. set_coordinatesButton, SIGNAL (clicked ()),
	         this, SLOT (handle_set_coordinatesButton ()));
	disconnect (configWidget. loadTableButton, SIGNAL (clicked ()),
	         this, SLOT (handle_loadTable ()));
	disconnect (configWidget. dumpButton, SIGNAL (clicked ()),
	         this, SLOT (handle_sourcedumpButton ()));
	disconnect (configWidget. skinButton, SIGNAL (clicked ()),
	         this, SLOT (handle_skinSelector ()));
//
//	Now the checkboxes
//	top line
	disconnect (configWidget. saveServiceSelector,
	                                  SIGNAL (stateChanged (int)),
	         this, SLOT (handle_saveServiceSelector (int)));
	disconnect (configWidget. loggerButton, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_LoggerButton (int)));
//	the epg2xmlSelector is kust polled, no need to react on an event

//	second line
	disconnect (configWidget. tii_detectorMode, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_tii_detectorMode (int)));
//
//	utcSelector is just polled, no need to react on an event here
	disconnect (configWidget. onTop, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_onTop (int)));
//
//	third line
//	here we expect the close without asking

	disconnect (configWidget. epgSelector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_epgSelector (int)));
	disconnect (configWidget. autoBrowser, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_autoBrowser      (int)));
//
//	fourth line
	disconnect (configWidget. dcRemovalSelector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_dcRemovalSelector (int)));
//	

	disconnect (configWidget. eti_activeSelector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_eti_activeSelector (int)));
//
	disconnect (configWidget. transmSelector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_transmSelector (int)));
//
//	fifh line
	disconnect (configWidget. clearScan_Selector,
	                             SIGNAL (stateChanged (int)),
	         this, SLOT (handle_clearScan_Selector (int)));

	disconnect (configWidget. saveSlides, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_saveSlides (int)));
//
	disconnect (configWidget. transmitterTags, SIGNAL (stateChanged (int)),	
	         this, SLOT (handle_transmitterTags  (int)));
//
//	botton row
	disconnect (configWidget. decoderSelector,
	                            SIGNAL (activated (const QString &)),
	         this, SLOT (handle_decoderSelector (const QString &)));
	disconnect (configWidget. streamoutSelector, SIGNAL (activated (int)),
	         this,  SLOT (set_streamSelector (int)));
}
//
#include <QCloseEvent>
void RadioInterface::closeEvent (QCloseEvent *event) {
	int x = configWidget. closeDirect -> isChecked () ? 1 : 0;
	dabSettings_p -> setValue ("closeDirect", x);
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

	if (my_ofdmHandler == nullptr) {
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
	if ((obj == this -> my_scanListHandler. viewport ()) &&
	    (event -> type () == QEvent::MouseButtonPress)) {
	   QMouseEvent *ev = static_cast<QMouseEvent *>(event);
	   if (ev -> buttons () & Qt::RightButton) {
	      QString service =
	           this -> my_scanListHandler. indexAt (ev -> pos()). data ().toString();
	     the_ensembleHandler -> add_favorite_from_scanList (service);
	   }
	}

	return QWidget::eventFilter (obj, event);
}

QPixmap RadioInterface::fetch_announcement (int id) {
QPixmap p;
QString pictureName	= QString (":res/announcement%1.png"). arg (id, 2, 10, QChar ('0'));
	if (!p.load (pictureName, "png"))
	   p. load (":res/announcement-d.png", "png");
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

////////////////////////////////////////////////////////////////////////
//
//	selection, either direct, from presets,  from scanlist or schedule
////////////////////////////////////////////////////////////////////////

//
//	selecting from the preset list
void	RadioInterface::handle_presetSelect (const QString &channel,
	                                     const QString &service) {
	if (!inputDevice_p -> isFileInput ())
	   localSelect (service, channel);
	else
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("Selection not possible"));
}
//
//	selecting from the scan list, which is essential
//	the same as handling form the preset list
void	RadioInterface::handle_scanListSelect (const QString &s) {
	if (!inputDevice_p -> isFileInput ())
	   localSelect (s);
	else
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("Selection not possible"));
}
//
//	selecting from a content description
void	RadioInterface::handle_contentSelector (const QString &s) {
	localSelect (s, channel. channelName);
}
//
//	From a predefined schedule list, the service names most
//	likely are less than 16 characters
//
void	RadioInterface::scheduleSelect (const QString &s) {
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 2)
	QStringList list = s.split (":", Qt::SkipEmptyParts);
#else   
	QStringList list = s.split (":", QString::SkipEmptyParts);
#endif  
	if (list. length () != 2)
	   return;
	fprintf (stderr, "we found %s %s\n",
	                   list. at (1). toLatin1 (). data (),
	                   list. at (0). toLatin1 (). data ());
	localSelect (list. at (1), list. at (0));
}
//
void	RadioInterface::localSelect (const QString &s) {
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 2)
	QStringList list = s.split (":", Qt::SkipEmptyParts);
#else
	QStringList list = s.split (":", QString::SkipEmptyParts);
#endif
	if (list. length () != 2)
	   return;
	localSelect (list. at (1), list. at (0));
}

void	RadioInterface::localSelect (const QString &service,
	                             const QString &theChannel) {
int	switchDelay;
QString serviceName	= service;

	if (my_ofdmHandler == nullptr)	// should not happen
	   return;

	stopService (channel. currentService);

	for (int i = service. size (); i < 16; i ++)
	   serviceName. push_back (' ');

	if (theChannel == channel. channelName) {
	   channel. currentService. valid = false;
	   dabService s;
	   my_ofdmHandler -> get_parameters (serviceName, &s. SId, &s. SCIds);
	   if (s. SId == 0) {
	      dynamicLabel -> setText ("cannot run " +
	                       s. serviceName + " yet");
	      return;
	   }
	   s. serviceName = serviceName;
	   startService (s);
	   return;
	}
//
//	The hard part is stopping the current service,
//	quitting the current channel,
//      selecting a new channel, and waiting a while
	stopChannel ();
//      and start the new channel first

	int k           = channelSelector -> findText (theChannel);
	if (k != -1) {
	   new_channelIndex (k);
	}
	else {
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("Incorrect preset\n"));
	   return;
	}
//
//	prepare the service, start the new channel and wait
	channel. nextService. valid		= true;
	channel. nextService. channel		= theChannel;
	channel. nextService. serviceName        = serviceName;
	channel. nextService. SId                = 0;
	channel. nextService. SCIds              = 0;
	presetTimer. setSingleShot (true);
	switchDelay			=
	           configWidget. switchDelaySetting -> value ();
	presetTimer. setInterval (switchDelay * 1000);
	presetTimer. start (switchDelay * 1000);
	startChannel    (channelSelector -> currentText ());
}

////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////

void	RadioInterface::stopService	(dabService &s) {
	presetTimer. stop ();
	channelTimer. stop ();
	stop_muting	();
	set_soundLabel (false);
	channel. audioActive	= false;
	if (my_ofdmHandler == nullptr) {
	   fprintf (stderr, "Expert error 22\n");
	   return;
	}

//	stop "dumpers"
	if (channel. currentService. frameDumper != nullptr) {
	   stopFramedumping ();
	   channel. currentService. frameDumper = nullptr;
	}

	if (audioDumper_p != nullptr) {
	   stopAudiodumping ();
	}

//	and clean up the technical widget
	techWindow_p	-> cleanUp ();

//	stop "secondary services" - if any - as well
	if (s. valid) {
	   my_ofdmHandler -> stop_service (s. subChId, FORE_GROUND);
	   if (s. is_audio) {
	      soundOut_p -> stop ();
	      for (int i = 0; i < 5; i ++) {
	         packetdata pd;
	         my_ofdmHandler -> data_for_packetservice (s. serviceName,
	                                                        &pd, i);
	         if (pd. defined) {
	            my_ofdmHandler -> stop_service (pd. subchId, FORE_GROUND);
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
	LOG ("start service ", serviceName. toUtf8 (). data ());
	LOG ("service has SNR ", QString::number (channel. snr));
//
//	mark the selected service in the service list
//
//	and display the servicename on the serviceLabel
	QFont font = serviceLabel -> font ();
	font. setPointSize (16);
	font. setBold (true);
	serviceLabel	-> setStyleSheet (labelStyle);
	serviceLabel	-> setFont (font);
	serviceLabel	-> setText (serviceName);
	dynamicLabel	-> setText ("");
	the_ensembleHandler -> reportStart (serviceName);
	audiodata ad;
	my_ofdmHandler -> data_for_audioservice (serviceName, ad);
	if (ad. defined) {
	   channel. currentService. valid	= true;
	   channel. currentService. is_audio	= true;
	   channel. currentService. subChId	= ad. subchId;
	   if (my_ofdmHandler -> has_timeTable (ad. SId))
	      techWindow_p -> show_timetableButton (true);

	   startAudioservice (ad);
	   if (dabSettings_p -> value ("has-presetName", 0).
	                                                   toInt () == 1) {
	      QString s = channel. channelName + ":" + serviceName;
	      dabSettings_p	-> setValue ("presetname", s);
	   }
	   else 
	      dabSettings_p	-> setValue ("presetname", "");
#ifdef	HAVE_PLUTO_RXTX
	   if (streamerOut_p != nullptr)
	      streamerOut_p -> addRds (std::string (serviceName. toUtf8 (). data ()));
#endif
	}
	else
	if (my_ofdmHandler -> is_packetservice (serviceName)) {
	   packetdata pd;
	   my_ofdmHandler -> data_for_packetservice (serviceName, &pd, 0);
	   channel. currentService. valid		= true;
	   channel. currentService. is_audio	= false;
	   channel. currentService. subChId	= pd. subchId;
	   startPacketservice (serviceName);
	}
	else {
	   QMessageBox::warning (this, tr ("Warning"),
 	                         tr ("insufficient data for this program\n"));
	   dabSettings_p	-> setValue ("presetname", "");
	}
}

//
void	RadioInterface::startAudioservice (audiodata &ad) {
	channel. currentService. valid	= true;
	(void)my_ofdmHandler -> set_audioChannel (ad, &audioBuffer,
	                                            nullptr, FORE_GROUND);
	for (int i = 1; i < 10; i ++) {
	   packetdata pd;
	   my_ofdmHandler -> data_for_packetservice (ad. serviceName, &pd, i);
	   if (pd. defined) {
	      my_ofdmHandler -> set_dataChannel (pd, &dataBuffer, FORE_GROUND);
	      fprintf (stderr, "adding %s (%d) as subservice\n",
	                            pd. serviceName. toUtf8 (). data (),
	                            pd. subchId);
	      break;
	   }
	}
//	activate sound
	soundOut_p -> restart ();
	channel. audioActive	= true;
	set_soundLabel (true);
	programTypeLabel	-> setStyleSheet (labelStyle);
	programTypeLabel	-> setText (getProgramType (ad. programType));
	rateLabel		-> setStyleSheet (labelStyle);
	rateLabel		-> setText (QString::number (ad. bitRate) + "kbit");
//	show service related data
	techWindow_p	-> show_serviceData 	(&ad);
}

void	RadioInterface::startPacketservice (const QString &s) {
packetdata pd;

	my_ofdmHandler -> data_for_packetservice (s, &pd, 0);
	if ((!pd. defined) ||
	            (pd.  DSCTy == 0) || (pd. bitRate == 0)) {
	   QMessageBox::warning (this, tr ("sdr"),
 	                         tr ("still insufficient data for this service\n"));
	   return;
	}

	if (!my_ofdmHandler -> set_dataChannel (pd,
	                                         &dataBuffer, FORE_GROUND)) {
	   QMessageBox::warning (this, tr ("sdr"),
 	                         tr ("could not start this service\n"));
	   return;
	}

	switch (pd. DSCTy) {
	   default:
	      show_label (QString ("unimplemented Data"));
	      break;
	   case 5:
	      fprintf (stderr, "selected apptype %d\n", pd. appType);
	      show_label (QString ("Transp. Channel not implemented"));
	      break;
	   case 60:
	      show_label (QString ("MOT partially implemented"));
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
}

////////////////////////////////////////////////////////////////////////////
//
void	RadioInterface::handle_prevServiceButton        () {
	the_ensembleHandler -> selectPrevService ();
}

void	RadioInterface::handle_nextServiceButton        () {
	the_ensembleHandler -> selectNextService ();
}

////////////////////////////////////////////////////////////////////////////
//
//	The user(s)
///////////////////////////////////////////////////////////////////////////
//
//	setPresetService () is called after a time out to 
//	actually start the service that we were waiting for
//	Assumption is that the channel is set, and the servicename
//	is to be found in "nextService"
void	RadioInterface::setPresetService () {
	if (!running. load ())
	   return;

	presetTimer. stop ();
	stopScanning ();
	if (!channel. nextService. valid)
	   return;

	if (channel. nextService. channel != channel. channelName)
	   return;

	if (channel. Eid == 0) {
	   dynamicLabel -> setText ("ensemblename not yet found\n");
	   return;
	}

	QString presetName	= channel. nextService. serviceName;
	for (int i = presetName. length (); i < 16; i ++)
	   presetName. push_back (' ');

	dabService s;
	s. serviceName	= presetName;
	my_ofdmHandler	-> get_parameters (presetName, &s. SId, &s. SCIds);
	if (s. SId == 0) {
	   dynamicLabel -> setText (QString ("not all data for ") +
	                            s. serviceName +
	                             " on board");
//	   QMessageBox::warning (this, tr ("Warning"),
//	                        tr ("insufficient data for this program\n"));
	   return;
	}
	channel. nextService. valid = false;
	fprintf (stderr, "gaan we de scheduled item starten %s?\n",
	                                        s. serviceName. toLatin1 (). data ());
	startService (s);
}

///////////////////////////////////////////////////////////////////////////
//
//	Channel basics
///////////////////////////////////////////////////////////////////////////
//	Precondition: no channel should be active
//	
void	RadioInterface::startChannel (const QString &theChannel) {
int	tunedFrequency	=
	         scanMonitor. Frequency (theChannel);
	LOG ("channel starts ", theChannel);
	newDisplay. showFrequency (tunedFrequency);
	dabSettings_p		-> setValue ("channel", theChannel);
	inputDevice_p		-> resetBuffer ();
//	serviceList. clear ();
	inputDevice_p		-> restartReader (tunedFrequency);

	channel. cleanChannel ();
	channel. channelName	= theChannel;
	channel. tunedFrequency	= tunedFrequency;
	channel. realChannel	= !inputDevice_p -> isFileInput ();
	channel. localPos. latitude             =
                     dabSettings_p -> value ("latitude", 0). toFloat (); 
        channel. localPos. longitude            =
                     dabSettings_p -> value ("longitude", 0). toFloat ();

	dabSettings_p		-> setValue ("channel", theChannel);
//	channel. frequency	= tunedFrequency / 1000;
	distanceLabel		-> setText ("");
	newDisplay. show_transmitters (channel. transmitters);
	if (transmitterTags_local  && (mapHandler != nullptr))
	   mapHandler -> putData (MAP_RESET,channel. targetPos, "", "", "", 0, 0, 0, 0);
	else
	if (mapHandler != nullptr)
	   mapHandler -> putData (MAP_FRAME, position {-1, -1}, "", "", "", 0, 0, 0, 0);
	the_ensembleHandler -> reset ();
	my_ofdmHandler		-> start ();
	int	switchDelay	=
	             configWidget. switchDelaySetting -> value ();
	if (!scanMonitor. active ())
	   epgTimer. start (switchDelay * 1000);
}
//
//	apart from stopping the reader, a lot of administration
//	is to be done.
void	RadioInterface::stopChannel	() {
	if (inputDevice_p == nullptr)		// should not happen
	   return;

	disconnect (ensembleId, SIGNAL (clicked ()),
	            this, SLOT (handle_contentButton ()));
	stop_etiHandler	();
	LOG ("channel stops ", channel. channelName);
//
//	first, stop services in fore and background
	if (channel. currentService. valid)
	   stopService (channel. currentService);

	for (uint16_t i = 0; i < channel. backgroundServices. size (); i ++) {
	   dabService s =  channel. backgroundServices. at (i);
	   my_ofdmHandler -> stop_service (s. subChId, BACK_GROUND);
	   if (s. fd != nullptr)
	      fclose (s. fd);
	}
	channel. backgroundServices. clear ();

	distanceLabel	-> setText ("");
	stop_sourcedumping	();
	soundOut_p	-> stop ();
//
//	newDisplay. EPGLabel	-> hide ();
	if (contentTable_p != nullptr) {
	   contentTable_p -> hide ();
	   delete contentTable_p;
	   contentTable_p = nullptr;
	}
//	note framedumping - if any - was already stopped
//	ficDumping - if on - is stopped here
	if (ficDumpPointer != nullptr) {
	   my_ofdmHandler -> stop_ficDump ();
	   ficDumpPointer = nullptr;
	}
	epgTimer. stop		();
	inputDevice_p		-> stopReader ();
	my_ofdmHandler		-> stop ();
	usleep (1000);
	techWindow_p	-> cleanUp ();

	show_pauzeSlide ();
	presetTimer. stop 	();
	channelTimer. stop	();
	channel. cleanChannel	();
	channel. targetPos. latitude	= 0;
	channel. targetPos. longitude	= 0;
	if (transmitterTags_local && (mapHandler != nullptr))
	   mapHandler -> putData (MAP_RESET, channel. targetPos, "", "", "", 0, 0, 0, 0);
	transmitter_country     -> setText ("");
	transmitter_coordinates -> setText ("");
//
	QCoreApplication::processEvents ();
//
//	no processing left at this time
	usleep (1000);		// may be handling pending signals?
	channel. currentService. valid	= false;
	channel. nextService. valid	= false;

//	all stopped, now look at the GUI elements
	newDisplay. ficError_display	-> setValue (0);
//	the visual elements related to service and channel
	set_synced	(false);
	ensembleId	-> setText ("");
	transmitter_coordinates	-> setText (" ");

	cleanScreen	();
//	newDisplay. EPGLabel	-> hide ();
	distanceLabel	-> setText ("");
}

//
/////////////////////////////////////////////////////////////////////////
//
//	next- and previous channel buttons
/////////////////////////////////////////////////////////////////////////

void	RadioInterface::handle_channelSelector (const QString &channel) {
	if (!running. load ())
	   return;

	LOG ("select channel ", channel. toUtf8 (). data ());
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
	if (my_ofdmHandler == nullptr) {
	   fprintf (stderr, "Expert error 23\n");
	   abort ();
	}

	stopScanning	();
	stopChannel	();
	new_channelIndex (currentChannel);
	startChannel (channelSelector -> currentText ());
}

////////////////////////////////////////////////////////////////////////
//
//	scanning
/////////////////////////////////////////////////////////////////////////
//
//	The scan function covers three scan strategies. In order to make things
//	manageable, we implement the streams  in different functions and procedures
void	RadioInterface::handle_scanButton () {
	if (!running. load ())
	   return;
	if (scanMonitor. isVisible ())
	   scanMonitor. hide ();
	else
	   scanMonitor. show ();
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
	connect (my_ofdmHandler, SIGNAL (no_signal_found ()),
	         this, SLOT (no_signal_found ()));

	if (scanMonitor. scan_to_data ())
	   start_scan_to_data ();
	else
	if (scanMonitor. scan_single ())
	   start_scan_single ();
	else
	   start_scan_continuous ();
}

void	RadioInterface::start_scan_to_data () {
//
//	when running scan to data, we look at all channels, whether
//	on the skiplist or not
	QString cs = scanMonitor. getNextChannel (channelSelector -> currentText ());
	int cc = channelSelector -> findText (cs);
	LOG ("scanning starts with ", cs);
	new_channelIndex (cc);
//	scanMonitor. addText (" scanning channel " +
//	                            channelSelector -> currentText ());
	int switchDelay		=
	             configWidget. switchDelaySetting -> value ();
	channelTimer. start (switchDelay * 1000);
	my_ofdmHandler	-> set_scanMode (true);
	startChannel    (channelSelector -> currentText ());
}

void	RadioInterface::start_scan_single () {
	if (configWidget. clearScan_Selector -> isChecked ())
	   my_scanListHandler. clear_scanList ();

	if (scanTable_p == nullptr) 
	   scanTable_p = new contentTable (this, dabSettings_p, "scan", 
	                                       my_ofdmHandler -> scanWidth ());
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

	my_ofdmHandler	-> set_scanMode (true);
	QString fs	= scanMonitor. getFirstChannel ();
	int k = channelSelector ->  findText (fs);
	if (k != -1)
	   new_channelIndex (k);
//	scanMonitor. addText (" scanning channel " +
//	                            channelSelector -> currentText ());
	int switchDelay		=
	             configWidget. switchDelaySetting -> value ();
	channelTimer. start (switchDelay * 1000);
	startChannel    (channelSelector -> currentText ());
}

void	RadioInterface::start_scan_continuous () {
	if (scanTable_p == nullptr) 
	   scanTable_p = new contentTable (this, dabSettings_p, "scan", 
	                                     my_ofdmHandler -> scanWidth ());
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

	my_ofdmHandler	-> set_scanMode (true);
//      To avoid reaction of the system on setting a different value:
	QString fs = scanMonitor. getFirstChannel ();
	int k = channelSelector -> findText (fs);
	new_channelIndex (k);
//	scanMonitor. addText (" scanning channel " +
//	                            channelSelector -> currentText ());
	int switchDelay		=
	             configWidget. switchDelaySetting -> value ();
	channelTimer. start (switchDelay * 1000);
	startChannel    (channelSelector -> currentText ());
}
//
//	stop_scanning is called
//	1. when the scanbutton is touched during scanning
//	2. on user selection of a service or a channel select
//	3. on device selection
//	4. on handling a reset
void	RadioInterface::stopScanning	() {
	disconnect (my_ofdmHandler, SIGNAL (no_signal_found ()),
	            this, SLOT (no_signal_found ()));
	if (!scanMonitor. active ())
	   return;
	presetButton	-> setText ("favorites");
	presetButton	-> setEnabled (true);
	LOG ("scanning stops ", "");
	if (scanMonitor. scan_to_data ())
	   stop_scan_to_data ();
	else
	if (scanMonitor. scan_single ())
	   stop_scan_single ();
	else
	   stop_scan_continuous ();
	scanMonitor. setStop ();
//	presetButton	-> setEnabled (true);
}

void	RadioInterface::stop_scan_to_data () {
	my_ofdmHandler	-> set_scanMode (false);
	channelTimer. stop ();
}

void	RadioInterface::stop_scan_single () {
	my_ofdmHandler	-> set_scanMode (false);
	channelTimer. stop ();

	if (scanTable_p == nullptr)
	   return;		// should not happen
	FILE *scanDumper_p	= scanMonitor. askFileName ();
	if (scanDumper_p != nullptr) {
	   scanTable_p -> dump (scanDumper_p);
	   fclose (scanDumper_p);
	   scanDumper_p = nullptr;
	}
	delete scanTable_p;
	scanTable_p	= nullptr;
}

void	RadioInterface::stop_scan_continuous () {
	my_ofdmHandler	-> set_scanMode (false);
	channelTimer. stop ();

	if (scanTable_p == nullptr)
	   return;		// should not happen

	FILE *scanDumper_p	= scanMonitor. askFileName ();
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

void	RadioInterface::no_signal_found () {
	disconnect (my_ofdmHandler, SIGNAL (no_signal_found ()),
	            this, SLOT (no_signal_found ()));
	channelTimer. stop ();
	disconnect (&channelTimer, SIGNAL (timeout ()),
	            this, SLOT (channel_timeOut ()));
	if (!scanMonitor. active ())
	   return;

	if (scanMonitor. scan_to_data ())
	   next_for_scan_to_data ();
	else	
	if (scanMonitor. scan_single ())
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
	QString ns	= scanMonitor. getNextChannel ();
	int cc = channelSelector -> findText (ns);
	new_channelIndex (cc);
//
//	and restart for the next run
//	scanMonitor. addText ("scanning channel " +
//	                             channelSelector -> currentText ());
	connect (my_ofdmHandler, SIGNAL (no_signal_found ()),
	         this, SLOT (no_signal_found ()));
	connect (&channelTimer, SIGNAL (timeout ()),
	         this, SLOT (channel_timeOut ()));

	int switchDelay	= 
	               configWidget. switchDelaySetting -> value ();
	channelTimer. start (switchDelay * 1000);
	startChannel (channelSelector -> currentText ());
}

void	RadioInterface::next_for_scan_single () {
	if (channel. nrServices > 0)
	   show_for_single_scan ();
	stopChannel ();
	try {
	   QString cs	= scanMonitor. getNextChannel ();
	   int	cc	= channelSelector -> findText (cs);
	   new_channelIndex (cc);
	} catch (...) {
	   stopScanning ();
	   return;
	}

//	scanMonitor. addText ("scanning channel " +
//	                             channelSelector -> currentText ());
	connect (my_ofdmHandler, SIGNAL (no_signal_found ()),
	         this, SLOT (no_signal_found ()));
	connect (&channelTimer, SIGNAL (timeout ()),
	         this, SLOT (channel_timeOut ()));

	int switchDelay	= 
	               configWidget. switchDelaySetting -> value ();
	channelTimer. start (switchDelay * 1000);
	startChannel (channelSelector -> currentText ());
}

void	RadioInterface::next_for_scan_continuous () {
	if (channel. nrServices > 0)
	   show_for_continuous ();
	stopChannel ();

	QString cs	= scanMonitor. getNextChannel ();
	int cc	= channelSelector -> findText (cs);
	new_channelIndex (cc);

	connect (my_ofdmHandler, SIGNAL (no_signal_found ()),
	         this, SLOT (no_signal_found ()));
	connect (&channelTimer, SIGNAL (timeout ()),
	         this, SLOT (channel_timeOut ()));

//	scanMonitor. addText ("scanning channel " +
//	                             channelSelector -> currentText ());
	int switchDelay	= 
	               configWidget. switchDelaySetting -> value ();
	channelTimer. start (switchDelay * 1000);
	startChannel (channelSelector -> currentText ());
}

////////////////////////////////////////////////////////////////////////////
//
// showServices
////////////////////////////////////////////////////////////////////////////
	
void	RadioInterface::show_for_single_scan () {
QString SNR 		= "SNR " + QString::number (channel. snr);

	if (my_ofdmHandler == nullptr) {	// cannot happen
	   fprintf (stderr, "Expert error 26\n");
	   return;
	}

	QString utcTime	= convertTime (UTC. year, UTC.month,
	                               UTC. day, UTC. hour, 
	                               UTC. minute);
	QString headLine = channel. ensembleName + ";" +
	                      channel. channelName + ";" +
	                      QString::number (channel. tunedFrequency) + ";" +
	                      hextoString (channel. Eid) + " " + ";" +
	                      transmitter_coordinates -> text () + " " + ";" +
	                      utcTime  + ";" +
	                      SNR  + ";" +
	                      QString::number (channel. nrServices) + ";" +
	                      distanceLabel -> text ();
	QStringList s = my_ofdmHandler -> basicPrint ();
	scanTable_p -> addLine (headLine);
	scanTable_p -> addLine ("\n;\n");
	for (const auto &l : s)
	   scanTable_p -> addLine (l);
	scanTable_p -> addLine ("\n;\n;\n");
	scanTable_p -> show ();
}

void	RadioInterface::show_for_continuous () {
QString SNR 		= "SNR " + QString::number (channel. snr);

	if (my_ofdmHandler == nullptr) {	// cannot happen
	   fprintf (stderr, "Expert error 26\n");
	   return;
	}

	QString utcTime	= convertTime (UTC. year, UTC.month,
	                               UTC. day, UTC. hour, 
	                               UTC. minute);
	QString headLine = channel. ensembleName + ";" +
	                      channel. channelName  + ";" +
	                      QString::number (channel. tunedFrequency) + ";" +
	                      hextoString (channel. Eid) + ";" +
	                      transmitter_coordinates -> text () + ";" +
	                      utcTime + ";" +
	                      SNR + ";" +
	                      QString::number (channel. nrServices) + ";" +
	                      distanceLabel -> text ();

	scanTable_p -> addLine (headLine);
	scanTable_p	-> show ();
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
	connect (&muteTimer, SIGNAL (timeout ()),
	         this, SLOT (muteButton_timeOut ()));
	muteDelay	= dabSettings_p -> value ("muteTime", 2). toInt ();
	muteDelay	*= 60;
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
	   disconnect (&muteTimer, SIGNAL (timeout ()),
	               this, SLOT (muteButton_timeOut ()));
	   stillMuting	-> hide ();
	   if (!channel. audioActive)
	      set_soundLabel (true);
	}
}

void	RadioInterface::stop_muting		() {
	if (!muteTimer. isActive ()) 
	   return;
	set_soundLabel (true);
	muteTimer. stop ();
	disconnect (&muteTimer, SIGNAL (timeout ()),
	               this, SLOT (muteButton_timeOut ()));
//	setButtonFont (muteButton, "mute", 10);
	stillMuting	-> hide ();
}
//
//	End of handling mute button
//

void	RadioInterface::new_channelIndex (int index) {
	if (channelSelector -> currentIndex () == index)
	   return;
	disconnect (channelSelector, SIGNAL (activated (const QString &)),
	            this, SLOT (handle_channelSelector (const QString &)));
	channelSelector	-> blockSignals (true);
	set_newChannel (index);
	while (channelSelector -> currentIndex () != index)
	   usleep (2000);
	channelSelector	-> blockSignals (false);
	connect (channelSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (handle_channelSelector (const QString &)));
}
//
/////////////////////////////////////////////////////////////////////////
//	merely as a gadget, for each button the color can be set
//	Lots of code, about 400 lines, just for a gadget
//	
void	RadioInterface::set_Colors () {
	dabSettings_p	-> beginGroup ("colorSettings");
QString resetButton_color =
	   dabSettings_p -> value (RESET_BUTTON + "_color",
	                                              "white"). toString ();
QString resetButton_font =
	   dabSettings_p -> value (RESET_BUTTON + "_font",
	                                              "black"). toString ();
QString scanButton_color =
	   dabSettings_p -> value (SCAN_BUTTON + "_color",
	                                              "white"). toString ();
QString scanButton_font =
	   dabSettings_p -> value (SCAN_BUTTON + "_font",
	                                              "black"). toString ();

QString spectrumButton_color =
	   dabSettings_p -> value (SPECTRUM_BUTTON + "_color",
	                                              "white"). toString ();
QString spectrumButton_font =
	   dabSettings_p -> value (SPECTRUM_BUTTON + "_font",
	                                              "black"). toString ();
QString snrButton_color =
	   dabSettings_p -> value (SNR_BUTTON + "_color",
	                                              "white"). toString ();
QString snrButton_font =
	   dabSettings_p -> value (SNR_BUTTON + "_font",
	                                              "black"). toString ();
QString devicewidgetButton_color =
	   dabSettings_p -> value (DEVICEWIDGET_BUTTON + "_color",
	                                              "white"). toString ();
QString devicewidgetButton_font =
	   dabSettings_p -> value (DEVICEWIDGET_BUTTON + "_font",
	                                              "black"). toString ();

QString scanListButton_color =
	   dabSettings_p -> value (SCANLIST_BUTTON + "_color",
	                                              "white"). toString ();
QString scanListButton_font =
	   dabSettings_p -> value (SCANLIST_BUTTON + "_font",
	                                              "black"). toString ();
QString presetButton_color =
	   dabSettings_p -> value (PRESET_BUTTON + "_color",
	                                              "white"). toString ();
QString presetButton_font =
	   dabSettings_p -> value (PRESET_BUTTON + "_font",
	                                              "black"). toString ();

QString dumpButton_color =
	   dabSettings_p -> value (DUMP_BUTTON + "_color",
	                                              "white"). toString ();
QString dumpButton_font =
	   dabSettings_p -> value (DUMP_BUTTON + "_font",
	                                              "black"). toString ();

QString prevServiceButton_color =
	   dabSettings_p -> value (PREVSERVICE_BUTTON + "_color",
	                                              "blaCK"). toString ();
QString prevServiceButton_font =
	   dabSettings_p -> value (PREVSERVICE_BUTTON + "_font",
	                                              "white"). toString ();
QString nextServiceButton_color =
	   dabSettings_p -> value (NEXTSERVICE_BUTTON + "_color",
	                                              "black"). toString ();
QString nextServiceButton_font =
	   dabSettings_p -> value (NEXTSERVICE_BUTTON + "_font",
	                                              "white"). toString ();

QString	dlTextButton_color =
	   dabSettings_p -> value (DLTEXT_BUTTON + "_color",
	                                              "black"). toString ();

QString dlTextButton_font	=
	   dabSettings_p -> value (DLTEXT_BUTTON + "_font",
	                                              "white"). toString ();
QString	configButton_color =
	   dabSettings_p -> value (CONFIG_BUTTON + "_color",
	                                              "black"). toString ();
QString configButton_font	=
	   dabSettings_p -> value (CONFIG_BUTTON + "_font",
	                                              "white"). toString ();
QString	httpButton_color =
	   dabSettings_p -> value (HTTP_BUTTON + "_color",
	                                              "black"). toString ();
QString httpButton_font	=
	   dabSettings_p -> value (HTTP_BUTTON + "_font",
	                                              "white"). toString ();
QString	scheduleButton_color =
	   dabSettings_p -> value (SCHEDULE_BUTTON + "_color",
	                                              "black"). toString ();
QString scheduleButton_font	=
	   dabSettings_p -> value (SCHEDULE_BUTTON + "_font",
	                                              "white"). toString ();
QString	set_coordinatesButton_color =
	   dabSettings_p -> value (SET_COORDINATES_BUTTON + "_color",
	                                              "white"). toString ();
QString set_coordinatesButton_font	=
	   dabSettings_p -> value (SET_COORDINATES_BUTTON + "_font",
	                                              "black"). toString ();
	
QString skinButton_font	=
	   dabSettings_p -> value (SKIN_BUTTON + "_font",
	                                              "white"). toString ();
QString	skinButton_color =
	   dabSettings_p -> value (SKIN_BUTTON + "_color",
	                                              "black"). toString ();
QString fontButton_font	=
	   dabSettings_p -> value (FONT_BUTTON + "_font",
	                                              "white"). toString ();
QString	fontButton_color =
	   dabSettings_p -> value (FONT_BUTTON + "_color",
	                                              "black"). toString ();
QString fontColorButton_font	=
	   dabSettings_p -> value (FONTCOLOR_BUTTON + "_font",
	                                              "white"). toString ();
QString	fontColorButton_color =
	   dabSettings_p -> value (FONTCOLOR_BUTTON + "_color",
	                                              "black"). toString ();
QString portSelector_font	=
	   dabSettings_p -> value (PORT_SELECTOR + "_font",
	                                              "white"). toString ();
QString	portSelector_color =
	   dabSettings_p -> value (PORT_SELECTOR + "_color",
	                                              "black"). toString ();
QString	loadTableButton_color =
	   dabSettings_p -> value (LOAD_TABLE_BUTTON + "_color",
	                                              "white"). toString ();
QString loadTableButton_font	=
	   dabSettings_p -> value (LOAD_TABLE_BUTTON + "_font",
	                                              "black"). toString ();
	dabSettings_p	-> endGroup ();

	QString temp = "QPushButton {background-color: %1; color: %2}";

	configWidget.  resetButton ->
	              setStyleSheet (temp. arg (resetButton_color,	
	                                        resetButton_font));

	spectrumButton ->
	              setStyleSheet (temp. arg (spectrumButton_color,
	                                        spectrumButton_font));

	configWidget. snrButton	->
	              setStyleSheet (temp. arg (snrButton_color,
	                                        snrButton_font));

	configWidget. devicewidgetButton ->
	              setStyleSheet (temp. arg (devicewidgetButton_color,
	                                        devicewidgetButton_font));

	configWidget. dlTextButton ->
	              setStyleSheet (temp. arg (dlTextButton_color,
	                                        dlTextButton_font));

	configWidget. dumpButton ->
	              setStyleSheet (temp. arg (dumpButton_color,
	                                        dumpButton_font));
	configWidget. set_coordinatesButton ->
	              setStyleSheet (temp. arg (set_coordinatesButton_color,
	                                        set_coordinatesButton_font));

	configWidget. loadTableButton ->
	              setStyleSheet (temp. arg (loadTableButton_color,
	                                        loadTableButton_font));
	configWidget. skinButton ->
	              setStyleSheet (temp. arg (skinButton_color,
	                                        skinButton_font));

	configWidget. fontButton ->
	              setStyleSheet (temp. arg (fontButton_color,
	                                        fontButton_font));

	configWidget. fontColorButton ->
	              setStyleSheet (temp. arg (fontColorButton_color,
	                                        fontColorButton_font));

	configWidget. portSelector ->
	              setStyleSheet (temp. arg (portSelector_color,
	                                        portSelector_font));

	scanListButton	->
	              setStyleSheet (temp. arg (scanListButton_color,
	                                        scanListButton_font));
	presetButton    -> 
	              setStyleSheet (temp. arg (presetButton_color,
	                                        presetButton_font));

	configWidget. scheduleButton ->
	              setStyleSheet (temp. arg (scheduleButton_color,
	                                        scheduleButton_font));

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

void	RadioInterface::color_resetButton	() {
	set_buttonColors (configWidget. resetButton, RESET_BUTTON);
}

void	RadioInterface::color_scanButton	() {
	set_buttonColors (scanButton, SCAN_BUTTON);
}

void	RadioInterface::color_spectrumButton	()	{
	set_buttonColors (spectrumButton, SPECTRUM_BUTTON);
}

void	RadioInterface::color_snrButton		() {
	set_buttonColors (configWidget. snrButton, SNR_BUTTON);
}

void	RadioInterface::color_devicewidgetButton	() {
	set_buttonColors (configWidget. devicewidgetButton, DEVICEWIDGET_BUTTON);
}

void	RadioInterface::color_scanListButton	()	{
	set_buttonColors (scanListButton, SCANLIST_BUTTON);
}
	                                        
void    RadioInterface::color_presetButton      ()      {
	set_buttonColors (presetButton, PRESET_BUTTON);
}  

void	RadioInterface::color_sourcedumpButton	()	{
	set_buttonColors (configWidget. dumpButton, DUMP_BUTTON);
}

void	RadioInterface::color_dlTextButton	()	{
	set_buttonColors (configWidget. dlTextButton, DLTEXT_BUTTON);
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

void	RadioInterface::color_scheduleButton	() 	{
	set_buttonColors (configWidget. scheduleButton, SCHEDULE_BUTTON);
}

void	RadioInterface::color_set_coordinatesButton	() 	{
	set_buttonColors (configWidget. set_coordinatesButton,
	                                          SET_COORDINATES_BUTTON);
}

void	RadioInterface::color_loadTableButton	() 	{
	set_buttonColors (configWidget. loadTableButton, LOAD_TABLE_BUTTON);
}

void	RadioInterface::color_skinButton	() 	{
	set_buttonColors (configWidget. skinButton, SKIN_BUTTON);
}

void	RadioInterface::color_fontButton	() 	{
	set_buttonColors (configWidget. fontButton, FONT_BUTTON);
}

void	RadioInterface::color_fontColorButton	() 	{
	set_buttonColors (configWidget. fontColorButton, FONTCOLOR_BUTTON);
}

void	RadioInterface::color_portSelector	() 	{
	set_buttonColors (configWidget. portSelector, PORT_SELECTOR);
}

void	RadioInterface::set_buttonColors	(QPushButton *b,
	                                         const QString &buttonName) {
QColor	baseColor;
QColor	textColor;
QColor	color;

	color = QColorDialog::getColor (baseColor, nullptr, "baseColor");
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

	dabSettings_p	-> beginGroup ("colorSettings");
	dabSettings_p	-> setValue (buttonColor, baseColor. name ());
	dabSettings_p	-> setValue (buttonFont, textColor. name ());
	dabSettings_p	-> endGroup ();
}

///////////////////////////////////////////////////////////////////////////
//	Handling schedule

void	RadioInterface::handle_scheduleButton	() {
QStringList candidates;
scheduleSelector theSelector;
QString		scheduleService;

	theSelector. addtoList ("nothing");
	theSelector. addtoList ("exit");
	theSelector. addtoList ("framedump");
	theSelector. addtoList ("audiodump");
	theSelector. addtoList ("dlText");
	theSelector. addtoList ("ficDump");
	candidates	+= "nothing";
	candidates	+= "exit";
	candidates	+= "framedump";
	candidates	+= "audiodump";
	candidates	+= "dlText";
	candidates	+= "ficDump";

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
	   configWidget. closeDirect -> setChecked (true);
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
	     filenameFinder. find_ficDump_file (channel. channelName);
	   if (ficDumpPointer == nullptr)
	      return;
	   my_ofdmHandler -> start_ficDump (ficDumpPointer);
	   return;
	}
	my_ofdmHandler	-> stop_ficDump ();
	ficDumpPointer = nullptr;
}

//-------------------------------------------------------------------------
//------------------------------------------------------------------------
//
//	if configured, the interpreation of the EPG data starts automatically,
//	the servicenames of an SPI/EPG service may differ from one country
//	to another
void	RadioInterface::epgTimer_timeOut	() {
	epgTimer. stop ();
	
	if (dabSettings_p   -> value ("epgFlag", 0). toInt () != 1)
	   return;
	if (scanMonitor. active ())
	   return;
	QStringList epgList = the_ensembleHandler -> get_epgServices ();
	for (auto serv : epgList) {
	   packetdata pd;
	   my_ofdmHandler -> data_for_packetservice (serv, &pd, 0);
	   if ((!pd. defined) ||
	            (pd.  DSCTy == 0) || (pd. bitRate == 0)) 
	      continue;
	   if (pd. DSCTy == 60) {
	      LOG ("hidden service started ", serv);
	      fprintf (stderr, "Starting hidden service %s\n",
	                                serv. toUtf8 (). data ());
	      my_ofdmHandler -> set_dataChannel (pd, &dataBuffer, BACK_GROUND);
	      dabService s;
	      s. channel     = pd. channel;
	      s. serviceName = pd. serviceName;
	      s. SId         = pd. SId;
	      s. SCIds       = pd. SCIds;
	      s. subChId     = pd. subchId;
	      s. fd          = nullptr;
	      channel. backgroundServices. push_back (s);
	   }
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------

void	RadioInterface::set_epgData (int SId, int theTime,
	                             const QString &theText,
	                             const QString &theDescr) {
	if (my_ofdmHandler != nullptr)
	   my_ofdmHandler -> set_epgData (SId, theTime,
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
	epgWidth	= dabSettings_p -> value ("epgWidth", 70). toInt ();
	if (epgWidth < 50)
	   epgWidth = 50;
	std::vector<epgElement> res =
	           my_ofdmHandler -> find_epgData (channel. currentService. SId);
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
	   configWidget. dlTextButton	-> setText ("dlText");
	   return;
	}

	QString	fileName = filenameFinder. finddlText_fileName (false);
	dlTextFile	= fopen (fileName. toUtf8 (). data (), "w+");
	if (dlTextFile == nullptr)
	   return;
	configWidget. dlTextButton	-> setText ("writing");
}
//
//---------------------------------------------------------------------
//
void	RadioInterface::handle_configButton	() {
	if (!configDisplay. isHidden ()) {
	   configButton	-> setText ("show controls");
	   configDisplay. hide ();	
	   dabSettings_p	-> setValue ("hidden", 1);
	}
	else {
	   configButton	-> setText ("hide controls");
	   configDisplay. show ();
	   dabSettings_p	-> setValue ("hidden", 0);
	}
}

void	RadioInterface::handle_muteTimeSetting	(int newV) {
	dabSettings_p	-> setValue ("muteTime", newV);
}

void	RadioInterface::handle_switchDelaySetting	(int newV) {
	dabSettings_p	-> setValue ("switchDelay", newV);
}

void	RadioInterface::handle_orderAlfabetical		() {
	dabSettings_p -> setValue ("serviceOrder", ALPHA_BASED);
}

void	RadioInterface::handle_orderServiceIds		() {
	dabSettings_p -> setValue ("serviceOrder", ID_BASED);
}

void	RadioInterface::handle_ordersubChannelIds	() {
	dabSettings_p -> setValue ("serviceOrder", SUBCH_BASED);
}

void	RadioInterface::handle_devicewidgetButton	() {
	if (inputDevice_p == nullptr)
	   return;
	inputDevice_p	-> setVisibility (!inputDevice_p -> getVisibility ());

	dabSettings_p -> setValue ("deviceVisible",
	                      inputDevice_p -> getVisibility () ? 1 : 0);
}

void	RadioInterface::handle_portSelector () {
mapPortHandler theHandler (dabSettings_p);
	(void)theHandler. QDialog::exec();
}

void	RadioInterface::handle_dlTextButton	() {
	if (dlTextFile != nullptr) {
	   fclose (dlTextFile);
	   dlTextFile = nullptr;
	   configWidget. dlTextButton	-> setText ("dlText");
	   return;
	}

	QString	fileName =filenameFinder. finddlText_fileName (true);
	dlTextFile	= fopen (fileName. toUtf8 (). data (), "w+");
	if (dlTextFile	== nullptr)
	   return;
	configWidget. dlTextButton		-> setText ("writing");
}

void	RadioInterface::handle_resetButton	() {
	if (!running. load())
	   return;
	QString	channelName	= channel. channelName;
	stopScanning ();
	stopChannel ();
	startChannel	(channelName);
}

void	RadioInterface::handle_snrButton	() {
	if (!running. load ())
	   return;

	if (my_snrViewer. isHidden ())
	   my_snrViewer. show ();
	else
	   my_snrViewer. hide ();
	dabSettings_p	-> setValue ("snrVisible",
	                          my_snrViewer. isHidden () ? 0 : 1);
}

void	RadioInterface::handle_set_coordinatesButton	() {
coordinates theCoordinator (dabSettings_p);
	(void)theCoordinator. QDialog::exec();
	channel. localPos. latitude		=
	             dabSettings_p -> value ("latitude", 0). toFloat ();
	channel. localPos. longitude		=
	             dabSettings_p -> value ("longitude", 0). toFloat ();
}

void	RadioInterface::handle_loadTable	 () {
QString defaultName	= QDir::homePath () + "/.txdata.tii";
QString	tableFile	=
	dabSettings_p -> value ("tiiFile", defaultName). toString ();

	if (tiiProcessor. loadTable (tableFile)) {
	   QMessageBox::information (this, tr ("success"),
	                            tr ("Loading and installing database complete\n"));
	   channel. tiiFile	= tiiProcessor. tiiFile (tableFile);
	}
	else {
	   QMessageBox::information (this, tr ("fail"),
	                            tr ("Loading database failed\n"));
	   channel. tiiFile = false;
	}
}

void	RadioInterface::stop_sourcedumping	() {
	if (rawDumper_p == nullptr) 
	   return;

	LOG ("source dump stops ", "");
	my_ofdmHandler	-> stop_dumping();
	sf_close (rawDumper_p);
	rawDumper_p	= nullptr;
	setButtonFont (configWidget. dumpButton, "Raw dump", 10);
}

void	RadioInterface::start_sourcedumping () {
QString deviceName	= inputDevice_p -> deviceName ();
QString channelName	= channel. channelName;
	
	if (scanMonitor. active ())
	   return;

	rawDumper_p	=
	         filenameFinder. findRawDump_fileName (deviceName, channelName);
	if (rawDumper_p == nullptr)
	   return;

	LOG ("source dump starts ", channelName);
	setButtonFont (configWidget. dumpButton, "writing", 12);
	my_ofdmHandler -> start_dumping (rawDumper_p);
}

void	RadioInterface::handle_sourcedumpButton () {
	if (!running. load () || scanMonitor. active ())
	   return;

	if (rawDumper_p != nullptr)
	   stop_sourcedumping ();
	else
	   start_sourcedumping ();
}


void	RadioInterface::handle_skinSelector     () {
skinHandler theSkins;
	int skinIndex = theSkins. QDialog::exec ();
	QString skinName = theSkins. skins. at (skinIndex);
	fprintf (stderr, "skin select %s\n", skinName. toLatin1 (). data ());
	dabSettings_p -> setValue ("skin", skinName); 
}


void	RadioInterface::handle_LoggerButton (int s) {
	(void)s;
	if (configWidget. loggerButton -> isChecked ()) {
	   if (logFile != nullptr) {
	      fprintf (stderr, "should not happen (logfile)\n");
	      return;
	   }
	   logFile = filenameFinder. findLogFileName ();
	   if (logFile != nullptr)
	      LOG ("Log started with ", inputDevice_p -> deviceName ());
	}
	else
	if (logFile != nullptr) {
	   fclose (logFile);
	   logFile = nullptr;
	}
}

void	RadioInterface::handle_tii_detectorMode (int d) {
bool	b = configWidget. tii_detectorMode -> isChecked ();
	(void)d;
	if (my_ofdmHandler != nullptr) {
	   my_ofdmHandler	-> set_tiiDetectorMode (b);
	   dabSettings_p	-> setValue ("tii_detector", b ? 1 : 0);
	}
}

void	RadioInterface::handle_onTop	(int d) {
bool onTop = false;
	(void)d;
	if (configWidget. onTop -> isChecked ())
	   onTop = true;
	dabSettings_p -> setValue ("onTop", onTop ? 1 : 0);
}

void	RadioInterface::handle_epgSelector	(int x) {
	(void)x;
	dabSettings_p -> setValue ("epgFlag", 
	                         configWidget. epgSelector -> isChecked () ? 1 : 0);
}

void	RadioInterface::handle_autoBrowser	(int d) {
	(void)d;
	dabSettings_p -> setValue ("autoBrowser", 
	               configWidget. autoBrowser -> isChecked () ? 1 : 0);
}

void	RadioInterface::handle_dcRemovalSelector (int s) {
	(void)s;
	if (my_ofdmHandler != nullptr)
	   my_ofdmHandler -> handle_dcRemovalSelector (configWidget. dcRemovalSelector -> isChecked ());
}

//	eti handler is elsewehere

void	RadioInterface::handle_clearScan_Selector (int c) {
	(void)c;
	dabSettings_p -> setValue ("clearScanResult",
	               configWidget. clearScan_Selector -> isChecked () ? 1 : 0);
}

void	RadioInterface::handle_saveServiceSelector	(int d) {
	(void)d;
	dabSettings_p	-> setValue ("has-presetName",
	                             configWidget. saveServiceSelector -> isChecked () ? 1 : 0);
}

void	RadioInterface::handle_saveSlides	(int x) {
	(void)x;
	dabSettings_p -> setValue ("saveSlides", 
	                         configWidget. saveSlides -> isChecked () ? 1 : 0);
}

void	RadioInterface::handle_transmitterTags  (int d) {
	(void)d;
	maxDistance = -1;
	transmitterTags_local = configWidget. transmitterTags -> isChecked ();
	dabSettings_p -> setValue ("transmitterTags", transmitterTags_local  ? 1 : 0);
	channel. targetPos	= position {0, 0};
	if ((transmitterTags_local) && (mapHandler != nullptr))
	   mapHandler -> putData (MAP_RESET, channel. targetPos, "", "", "", 0, 0, 0,0);
}

void	RadioInterface::handle_decoderSelector	(const QString &s) {
int	decoder	= 0100;
	for (int i = 0; decoders [i]. decoderName != ""; i ++)
	   if (decoders [i]. decoderName == s)
	      decoder = decoders [i]. decoderKey;
	my_ofdmHandler	-> handle_decoderSelector (decoder);
}

void	RadioInterface:: set_streamSelector (int k) {
	if (!running. load ())
	   return;
#if	not defined (TCP_STREAMER) &&  not defined (QT_AUDIO)
	((audioSink *)(soundOut_p)) -> selectDevice (k);
	dabSettings_p -> setValue ("soundchannel",
	                          configWidget. streamoutSelector -> currentText());
#else
	(void)k;
#endif
}
//
//////////////////////////////////////////////////////////////////////////

void	RadioInterface::nrServices	(int n) {
	channel. serviceCount = n;
}

void	RadioInterface::LOG	(const QString &a1, const QString &a2) {
QString theTime;
	if (logFile == nullptr)
	   return;
	if (configWidget. utcSelector -> isChecked ())
	   theTime  = convertTime (UTC. year, UTC. month, UTC. day,
	                                  UTC. hour, UTC. minute);
	else
	   theTime = QDateTime::currentDateTime (). toString ();

	fprintf (logFile, "at %s: %s %s\n",
	              theTime. toUtf8 (). data (),
	              a1. toUtf8 (). data (), a2. toUtf8 (). data ());
}

//
//	ensure that we only get a handler if we have a start location
void	RadioInterface::handle_httpButton	() {
	if (channel. localPos. latitude == 0) {
	   QMessageBox::information (this, tr ("Warning"),
	                         tr ("Function not available, no coordinates were found"));
	   return;
	}

	if (mapHandler == nullptr)  {
	   QString browserAddress	=
	                  dabSettings_p -> value ("browserAddress",
	                                "http://localhost"). toString ();
	   QString mapPort		=
	                  dabSettings_p -> value ("mapPort", 8080). toString ();

	   QString mapFile;
	   if (dabSettings_p -> value ("saveLocations", 0). toInt () == 1)
	      mapFile = filenameFinder. findMaps_fileName ();
	   else
	      mapFile = "";
	   mapHandler = new httpHandler (this,
	                                 mapPort,
	                                 browserAddress,
	                                 channel. localPos,
	                                 mapFile,
	                                 dabSettings_p -> value ("autoBrowser", 1). toInt () == 1);
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

void	RadioInterface::displaySlide	(const QPixmap &p) {
int w   = 360;
int h   = 2 * w / 3;
	pictureLabel	-> setAlignment(Qt::AlignCenter);
	pictureLabel ->
	       setPixmap (p. scaled (w, h, Qt::KeepAspectRatio));
	pictureLabel -> show ();
}

void	RadioInterface::show_pauzeSlide () {
QPixmap p;
QByteArray theSlide;

	if (p. load (":res/pauze-slide.png", "png"))
	   displaySlide (p);
}

void	RadioInterface::handle_transmSelector	(int x) {
	(void)x;
	dabSettings_p -> setValue ("saveLocations",
	                         configWidget. transmSelector -> isChecked () ? 1 : 0);
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
	if (my_ofdmHandler == nullptr)	// should not happen
	   return;

	if (channel. etiActive)
	   stop_etiHandler ();
	else
	   start_etiHandler ();
}

void	RadioInterface::stop_etiHandler () {
	if (!channel. etiActive) 
	   return;

	my_ofdmHandler -> stop_etiGenerator ();
	channel. etiActive = false;
	scanButton	-> setText ("eti");
}

void	RadioInterface::start_etiHandler () {
	if (channel. etiActive)
	   return;

	QString etiFile		=  filenameFinder.
	                                find_eti_fileName (channel. ensembleName, channel. channelName);
	if (etiFile == QString (""))
	   return;
	LOG ("etiHandler started", etiFile);
	channel. etiActive = my_ofdmHandler -> start_etiGenerator (etiFile);
	if (channel. etiActive) 
	   scanButton -> setText ("eti runs");
}

void	RadioInterface::handle_eti_activeSelector (int k) {
bool setting	= configWidget. eti_activeSelector	-> isChecked ();
	(void)k;
	if (inputDevice_p == nullptr)
	   return;

	if (setting) {
	   stopScanning ();	
	   disconnect (scanButton, SIGNAL (clicked ()),
	               this, SLOT (handle_scanButton ()));
	   connect (scanButton, SIGNAL (clicked ()),
	            this, SLOT (handle_etiHandler ()));
	   scanButton	-> setText ("eti");
	   if (!inputDevice_p -> isFileInput ())// restore the button' visibility
	      scanButton -> show ();
	   return;
	}
//	otherwise, disconnect the eti handling and reconnect scan
//	be careful, an ETI session may be going on
	stop_etiHandler ();		// just in case
	disconnect (scanButton, SIGNAL (clicked ()),
	            this, SLOT (handle_etiHandler ()));
	connect (scanButton, SIGNAL (clicked ()),
	         this, SLOT (handle_scanButton ()));
	scanButton      -> setText ("scan");
	if (inputDevice_p -> isFileInput ())	// hide the button now
	   scanButton -> hide ();
}

//
//	access functions to the display widget
void	RadioInterface::show_spectrum            (int amount) {
std::vector<Complex> inBuffer (2048);
	(void)amount;
	if (spectrumBuffer. GetRingBufferReadAvailable () < 2048)
	   return;
	spectrumBuffer. getDataFromBuffer (inBuffer. data (), 2048);
	spectrumBuffer. FlushRingBuffer ();
	if (!newDisplay. isHidden () &&
	           (newDisplay. get_tab () == SHOW_SPECTRUM))
	   newDisplay. show_spectrum (inBuffer, channel. tunedFrequency);
}

void	RadioInterface::show_tii_spectrum	() {
std::vector<Complex> inBuffer (2048);

	if (tiiBuffer. GetRingBufferReadAvailable () < 2048)
	   return;
	tiiBuffer. getDataFromBuffer (inBuffer. data (), 2048);
	tiiBuffer. FlushRingBuffer ();
	if (!newDisplay. isHidden () &&
	           (newDisplay. get_tab () == SHOW_TII))
	   newDisplay. show_tii (inBuffer, channel. tunedFrequency);
}

void	RadioInterface::show_correlation	(int s, int g, QVector<int> r) {
std::vector<float> inBuffer;

	(void)g;
	inBuffer. resize (s);
	responseBuffer. getDataFromBuffer (inBuffer. data (), s);
	responseBuffer. FlushRingBuffer ();
	if (!newDisplay. isHidden ()) {
	   if (newDisplay. get_tab () == SHOW_CORRELATION)
	      newDisplay. show_correlation (inBuffer, g, r, channel. distance);
	}
}
	      
void	RadioInterface::show_null		(int amount) {
Complex inBuffer [amount];
	nullBuffer. getDataFromBuffer (inBuffer, amount);
	if (!newDisplay. isHidden ())
	   if (newDisplay. get_tab () ==  SHOW_NULL)
	      newDisplay. show_null (inBuffer, amount);
}

void	RadioInterface::show_tii	(int mainId, int subId) {
QString	country	= "";
bool	tiiChange	= false;

	if (!running. load () ||(mainId == 0xFF))	// shouldn't be
	   return;

	{  bool inList = false;
	   for (int i = 0; i < channel. transmitters. size () / 2; i ++)
	      if ((channel. transmitters. at (2 * i) == mainId) &&
	          (channel. transmitters. at (2 * i + 1) == subId))
	         inList = true;
	   if (!inList) {
	      channel. transmitters. push_back (mainId);
	      channel. transmitters. push_back (subId);
	   }
	}

	if ((mainId != channel. mainId) ||
	    (subId != channel. subId)) {
	   tiiChange = true;
	   channel. mainId	= mainId;
	   channel. subId	= subId;

	   QString a = "(" + tiiNumber (mainId) + " " +
	                                    tiiNumber (subId) + ")";

	   transmitter_coordinates -> setAlignment (Qt::AlignRight);
	   transmitter_coordinates -> setText (a);
//	   transmitter_coordinates	-> hide ();
	}
//
//	display the transmitters on the scope widget
	if (!(newDisplay. isHidden () &&
	               (newDisplay. get_tab () == SHOW_TII))) 
	   newDisplay. show_transmitters (channel. transmitters);

	if (!channel. has_ecc && (my_ofdmHandler -> get_ecc () != 0)) {
	   channel. ecc_byte	= my_ofdmHandler -> get_ecc ();
	   country		= find_ITU_code (channel. ecc_byte,
	                                         (channel. Eid >> 12) &0xF);
	   channel. has_ecc	= true;
	   channel. transmitterName = "";
	}

	if ((country != "") && (country != channel. countryName)) {
	   transmitter_country	-> setText (country);
	   channel. countryName	= country;
	}
//
//	Now looking for a name
	if (!channel. tiiFile) 
	   return;

	if ((!tiiChange) && (channel. transmitterName != ""))
	   return;

	QString theName =
	      tiiProcessor. get_transmitterName (channel. realChannel?
	                                               channel. channelName :
	                                               "any",
	                                         channel. Eid,
	                                         mainId, subId);
	if (theName == "") {	// can happen
	   return;
	}
//	we have a name
//
	channel. transmitterName = theName;
//	being here, we know that we can estimate the distances of
	float power;
	tiiProcessor. get_coordinates (channel. targetPos, power,
	                               channel. realChannel ?
	                                  channel. channelName :
	                                  "any",
	                               theName);
//
//	
	if ((channel. localPos. latitude == 0) ||
	    (channel. localPos. longitude == 0) ||
	    (channel. targetPos. latitude == 0) ||
	    (channel. targetPos. longitude == 0)) {
	   return;
	}

//      if positions are known, we can compute distance and corner
	channel. distance = tiiProcessor. distance (channel. targetPos,
	                                            channel. localPos);
	channel. corner	  = tiiProcessor. corner (channel. targetPos,
	                                          channel. localPos);
	QString labelText = theName + " " +
	                    QString::number (channel. distance, 'f', 1) + " km " +
	                    QString::number (channel. corner, 'f', 1) +
	                    QString::fromLatin1 (" \xb0 ");
	fprintf (stderr, "%s\n", labelText. toUtf8 (). data ());
	QFont f ("Arial", 9);
	distanceLabel	->  setFont (f);
	distanceLabel -> setText (labelText);

//	see if we have a map
	if (mapHandler == nullptr) 
	   return;

	uint8_t key = MAP_NORM_TRANS;
	if ((!transmitterTags_local) && (channel. distance > maxDistance)) { 
	   maxDistance = channel. distance;
	   key = MAP_MAX_TRANS;
	}
//
	QDateTime theTime = 
	   configWidget.  utcSelector -> isChecked () ?
	                  QDateTime::currentDateTimeUtc () :
	                  QDateTime::currentDateTime ();

	mapHandler -> putData (key,
	                       channel. targetPos, 
	                       channel. transmitterName,
	                       channel. channelName,
	                       theTime. toString (Qt::TextDate),
	                       channel. mainId * 100 + channel. subId,
	                       channel. distance,
	                       channel. corner, power);
}

void	RadioInterface::showIQ			(int amount) {
std::vector<Complex> Values (amount);
	iqBuffer. getDataFromBuffer (Values. data (), amount);
	iqBuffer. FlushRingBuffer ();
	if (!newDisplay. isHidden ())
	   newDisplay. showIQ (Values);
}

void	RadioInterface::show_Corrector (int h, float l) {
	if (!newDisplay. isHidden ())
	   newDisplay. show_corrector (h, l);
}

void	RadioInterface::show_stdDev	(int amount) {
std::vector<float>Values (amount);
	stdDevBuffer. getDataFromBuffer (Values. data (), amount);
	if (!newDisplay. isHidden ())
	   newDisplay. show_stdDev (Values);
}

void	RadioInterface::show_snr		(float snr) {
QPixmap p;

	if (!newDisplay. isHidden ())
	   newDisplay. show_snr (snr);
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

void	RadioInterface::show_quality	(float q, 
	                                 float sco, float freqOffset) {
	if (!running. load () || newDisplay. isHidden ())
	   return;
	newDisplay. show_quality (q, sco, freqOffset); 
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
//	called from the DAB processor
void	RadioInterface::show_clock_error	(int d) {
	if (!running. load ())
	   return;
	if (!newDisplay. isHidden ()) {
	   newDisplay. show_clock_err (d);
	}
}

void	RadioInterface::init_configWidget () {
//
//	inits of checkboxes etc in the configuration widget,
//	note that only the GUI is set, values are not used
	int x = dabSettings_p -> value ("muteTime", 2). toInt ();
	configWidget. muteTimeSetting -> setValue (x);

	x = dabSettings_p -> value ("switchDelay",
	                               DEFAULT_SWITCHVALUE). toInt ();
	configWidget. switchDelaySetting -> setValue (x);
	x = dabSettings_p -> value ("serviceOrder", ALPHA_BASED). toInt ();
	if (x == ALPHA_BASED)
	   configWidget. orderAlfabetical -> setChecked (true);
	else
	if (x == ID_BASED)
	   configWidget. orderServiceIds -> setChecked (true);
	else
	   configWidget. ordersubChannelIds -> setChecked (true);

//	first row of checkboxes
//	saveServiceSelector is set elsewhere in the process
//	logger is set per process, not kept
	x = dabSettings_p -> value ("epg2xml", 0). toInt ();
	if (x != 0)
	   configWidget. epg2xmlSelector -> setChecked (true);
//
//	second row of checkboxes
	bool dm = dabSettings_p -> value ("tii_detector", 0). toInt () == 1;
	configWidget. tii_detectorMode -> setChecked (dm);

	bool b	= dabSettings_p	-> value ("utcSelector", 0). toInt () == 1;
	configWidget.  utcSelector -> setChecked (b);
	if (dabSettings_p -> value ("onTop", 0). toInt () == 1) 
	   configWidget.  onTop -> setChecked (true);
//
//	third row of checkboxes
	x = dabSettings_p -> value ("closeDirect", 0). toInt ();
	if (x != 0)
	   configWidget. closeDirect -> setChecked (true);
	if (dabSettings_p -> value ("epgFlag", 0). toInt () == 1)
	   configWidget. epgSelector -> setChecked (true);
	if (dabSettings_p -> value ("autoBrowser", 1). toInt () == 1)
	   configWidget. autoBrowser -> setChecked (true);
//
//	fourth row of checknoxes
//	deRemoval	not connected yet
//	eti activated	set per process
	if (dabSettings_p -> value ("transmitterTags", 1). toInt () == 1)
	   configWidget. transmitterTags -> setChecked (true);
//
//	fifth row of checkboxes
	if (dabSettings_p -> value ("clearScanResult", 1). toInt () == 1)
	   configWidget. clearScan_Selector -> setChecked (true);
	if (dabSettings_p -> value ("saveSlides", 0). toInt () == 1)
	   configWidget. saveSlides -> setChecked (true);
	if (dabSettings_p -> value ("saveLocations", 0). toInt () == 1)
	   configWidget. transmSelector -> setChecked (true);
//
#ifndef	__MSC_THREAD__
	for (int i = 0; decoders [i]. decoderName != ""; i ++) 
	   configWidget. decoderSelector -> addItem (decoders [i]. decoderName);
#else
	configWidget. decoderSelector -> setEnabled (false);
#endif
}

void	RadioInterface::show_channel	(int n) {
std::vector<Complex> v (n);
	channelBuffer. getDataFromBuffer (v. data (), n);
	channelBuffer. FlushRingBuffer ();
	if (!newDisplay. isHidden () &&
	           (newDisplay. get_tab () == SHOW_CHANNEL))
	   newDisplay. show_channel (v);
}

bool	RadioInterface::channelOn () {
	return (!newDisplay. isHidden () &&
	           (newDisplay. get_tab () == SHOW_CHANNEL));
}

bool	RadioInterface::devScopeOn () {
	return !newDisplay. isHidden () &&
	           (newDisplay. get_tab () == SHOW_STDDEV);
}

void	RadioInterface::handle_iqSelector () {
	if (my_ofdmHandler != nullptr)
	   my_ofdmHandler -> handle_iqSelector ();
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
	if (mode == SHOW_ENSEMBLE)
	   mode = SHOW_PRESETS;
	else
	   mode = SHOW_ENSEMBLE;
	the_ensembleHandler -> set_showMode (mode);
	if (mode == SHOW_ENSEMBLE)
	   presetButton -> setText ("favorites");
	else
	   presetButton -> setText ("ensemble");
}     

void	RadioInterface::set_soundLabel  (bool f) {
QPixmap p;

	if (f) 
	    p. load (":res/volume_on.png", "png");
	else
	    p. load (":res/volume_off.png", "png");
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

	my_ofdmHandler -> data_for_audioservice (service, ad);
	if ((!ad. defined) || (ad. ASCTy != 077))
	   return;

	for (uint16_t i = 0;
	     i < channel. backgroundServices. size (); i ++) {
	   if (channel. backgroundServices. at (i). serviceName ==
	                                                      service) {
	      my_ofdmHandler -> stop_service (ad. subchId, BACK_GROUND);
	      if (channel. backgroundServices. at (i). fd != nullptr)
	         fclose (channel. backgroundServices. at (i). fd);
	      channel. backgroundServices. erase
	                        (channel. backgroundServices. begin () + i);
	      return;
	   }
	}

	FILE *f = filenameFinder. findFrameDump_fileName (service, true);
	if (f == nullptr)
	   return;

	fprintf (stderr, "starting a background job %s\n",
	                             ad. serviceName. toLatin1 (). data ());
	(void)my_ofdmHandler ->
	                   set_audioChannel (ad, &audioBuffer, f, BACK_GROUND);
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
	dabSettings_p	-> setValue ("labelStyle", labelStyle);
}


void	RadioInterface::set_position_and_size (QWidget *w,
	                                       const QString &key) {
	int x	= dabSettings_p -> value (key + "-x", 100). toInt ();
	int y	= dabSettings_p -> value (key + "-y", 100). toInt ();
	int wi	= dabSettings_p -> value (key + "-w", 300). toInt ();
	int he	= dabSettings_p -> value (key + "-h", 200). toInt ();
	w 	-> resize (QSize (wi, he));
	w	-> move (QPoint (x, y));
}

void	RadioInterface::store_widget_position (QWidget *w, 
	                                       const QString &key) {
	dabSettings_p	-> setValue (key + "-x", w -> pos (). x ());
	dabSettings_p	-> setValue (key + "-y", w -> pos (). y ());
	dabSettings_p	-> setValue (key + "-w", w -> size (). width ());
	dabSettings_p	-> setValue (key + "-h", w -> size (). height ());
}

