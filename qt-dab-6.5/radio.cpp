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
#include	<unistd.h>
#include	<vector>
#include	"radio.h"
#include	"config-handler.h"
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
#include	"db-loader.h"
#include	"cacheElement.h"
#include	"distances.h"
#include	"position-handler.h"
#ifdef	TCP_STREAMER
#include	"tcp-streamer.h"
#elif	QT_AUDIO
#include	"Qt-audio.h"
#else
#include	"audiosink.h"
#endif
#include	"time-table.h"

#include	"device-exceptions.h"
#include	"settingNames.h"

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
	                                        the_dlCache (10),
	                                        tiiProcessor (Si),
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
	if (int_configValue ("onTop", 0) == 1) 
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
//	and create the workers
	set_position_and_size	(dabSettings_p, this, "mainWidget");
	configHandler_p		= new configHandler (this, dabSettings_p);
	the_ensembleHandler	= new ensembleHandler (this, dabSettings_p,
	                                                       presetFile);

//	we have the configuration handler and the ensemble handler,
//	connect some signals directly
	configHandler_p		-> set_connections ();
	connect (configHandler_p, SIGNAL (frameClosed ()),
	         this, SLOT (handle_configFrame_closed ()));
	connect (configHandler_p, SIGNAL (handle_fontSelect ()),
	         the_ensembleHandler, SLOT (handle_fontSelect ()));
	connect (configHandler_p, SIGNAL (handle_fontSizeSelect (int)),
	         the_ensembleHandler, SLOT (handle_fontSizeSelect (int)));
	connect (configHandler_p, SIGNAL (handle_fontColorSelect ()),
	         the_ensembleHandler, SLOT (handle_fontColorSelect ()));
	connect (configHandler_p, SIGNAL (set_serviceOrder (int)),
	         the_ensembleHandler, SLOT (set_serviceOrder (int)));

	connect (&newDisplay, SIGNAL (frameClosed ()),
	         this, SLOT (handle_newDisplayFrame_closed ()));
#ifdef HAVE_RTLSDR_V3
	SystemVersion	= QString ("5") + " with RTLSDR-V3";
#elif HAVE_RTLSDR_V4
	SystemVersion	= QString ("5") + " with RTLSDR-V4";
#else
	SystemVersion	= QString ("5");
#endif
	setWindowTitle ("Qt-DAB-6." +SystemVersion);

	ensembleWidget -> setWidget (the_ensembleHandler);
	connect (the_ensembleHandler, SIGNAL (selectService (const QString &,
	                                             const QString &)),
	         this, SLOT (localSelect (const QString &, const QString &)));
	connect (the_ensembleHandler,
	             SIGNAL (start_background_task (const QString &)),
	         this, SLOT (start_background_task (const QString &)));

	QStringList sl = chooseDevice. getDeviceList ();
	configHandler_p	-> setDeviceList (sl);
	   
	techWindow_p	= new techData (this, dabSettings_p, &theTechData);
	connect (techWindow_p, SIGNAL (frameClosed ()),
	         this, SLOT (handle_techFrame_closed ()));

	if (dabSettings_p -> value (NEW_DISPLAY_VISIBLE, 0). toInt () != 0)
	   newDisplay. show ();
	else
	   newDisplay. hide ();

	labelStyle	= dabSettings_p -> value (LABEL_COLOR,
	                                             LABEL_STYLE). toString ();
	nextService. valid	= false;
	channel. currentService. valid	= false;
	channel. serviceCount		= -1;

	channel. targetPos	= position {0, 0};
	localPos. latitude 		=
	             dabSettings_p -> value (HOME_LATITUDE, 0). toFloat ();
	localPos. longitude 		=
	             dabSettings_p -> value (HOME_LONGITUDE, 0). toFloat ();

	logFile			= nullptr;
	peakLeftDamped          = -100;
	peakRightDamped         = -100;

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
	configHandler_p	-> show_streamSelector (false);
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
	QStringList streams	= ((audioSink *)soundOut_p) -> streams ();
	configHandler_p	-> fill_streamTable (streams);
	configHandler_p	-> show_streamSelector (true);
	QString temp	=
	       dabSettings_p -> value (AUDIO_STREAM_NAME,
	                                      "default"). toString ();
	k	= configHandler_p -> init_streamTable (temp);
	if (k >= 0) {
	   bool err = !((audioSink *)soundOut_p) -> selectDevice (k);
	   if (err)
	      ((audioSink *)soundOut_p)	-> selectDefaultDevice();
	}
	configHandler_p	-> connect_streamTable	();
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

	connect (spectrumButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_spectrumButton ()));
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

	if (tiiProcessor. has_tiiFile ())
	   configHandler_p -> enable_loadLib ();
	else
	   httpButton	-> setEnabled (false);

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
	           dabSettings_p -> value (SELECTED_DEVICE,
	                                      "no device"). toString();
	bool b = configHandler_p -> findDevice (h);
	if (b) {
	   inputDevice_p = create_device (h);
	}
//
//	do we show controls?
	bool visible	=
	            dabSettings_p	-> value (CONFIG_WIDGET_VISIBLE,
	                                             0). toInt () != 0;
	if (visible) {
	   configHandler_p	-> show ();
	}

	connect (configButton, SIGNAL (clicked ()),
	         this, SLOT (handle_configButton ()));

	if (dabSettings_p -> value (SNR_WIDGET_VISIBLE, 0). toInt () != 0)
	   my_snrViewer. show ();
	else
	   my_snrViewer. hide ();
	if (dabSettings_p -> value (TECHDATA_VISIBLE, 0). toInt () == 1)
	   techWindow_p -> show ();

	dynamicLabel	-> setTextInteractionFlags(Qt::TextSelectableByMouse);
	dynamicLabel    -> setToolTip ("The text (or parts of it) of the dynamic label can be copied. Selecting the text with the mouse and clicking the right hand mouse button shows a small menu with which the text can be put into the clipboard");
//
//	Until the opposite is known to be true:
	nextService. channel = "";
	nextService. serviceName = "";
	nextService. valid = false;
//	if a device was selected, we just start, otherwise
//	we wait until one is selected
	connectGUI ();
	if (inputDevice_p != nullptr) {
	   doStart ();
	   qApp	-> installEventFilter (this);
	   return;
	}
	if (!visible) { 	// make it visible
	   dabSettings_p	-> setValue (CONFIG_WIDGET_VISIBLE, 1);
	   dabSettings_p	-> setValue (DEVICE_WIDGET_VISIBLE, 1);
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
	inputDevice_p	= create_device	(dev);
//	Some buttons should not be touched before we have a device
	if (inputDevice_p == nullptr) {
	   fprintf (stderr, "disconnecting\n");
	   return;
	}
	fprintf (stderr, "in do start 1\n");
	doStart ();
}
//
//	we (re)start a device, if it happens to be a regular
//	device, check for a preset name
void	RadioInterface::doStart	() {
	disconnect (channelSelector, SIGNAL (activated (const QString &)),
	            this, SLOT (handle_channelSelector (const QString &)));
	nextService	= checkPresets ();
	if (nextService. valid) {
	   int k = channelSelector -> findText (nextService. channel);
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
	fprintf (stderr, "connecting GUI\n");

	if (dabSettings_p -> value (DEVICE_WIDGET_VISIBLE, 0).  toInt () != 0)
	   inputDevice_p -> setVisibility (true);

//
//	Just to be sure we disconnect here.
//	It would have been helpful to have a function
//	testing whether or not a connection exists, we need a kind
//	of "reset"
	configHandler_p -> disconnectDevices ();
	configHandler_p	-> reconnectDevices ();
	
	connect (channelSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (handle_channelSelector (const QString &)));

	if (configHandler_p -> tii_detector_active ())
	   my_ofdmHandler -> set_tiiDetectorMode (true);

	if (nextService. valid) {
	   int switchDelay	= configHandler_p -> switchDelayValue ();
	   presetTimer. setSingleShot	(true);
	   presetTimer. setInterval 	(switchDelay);
	   presetTimer. start 		(switchDelay);
//	after the preset timer signals, the service will be started
	}
	startChannel (channelSelector -> currentText ());
	running. store (true);
}

	RadioInterface::~RadioInterface () {
	fprintf (stderr, "radioInterface is deleted\n");
}

dabService	RadioInterface::checkPresets () {
dabService	res;
	res. channel = "";
	res. serviceName = "";
	res. valid = false;
	if (inputDevice_p -> isFileInput ())
	   return res;
	QString preset = dabSettings_p -> value (PRESET_NAME, ""). toString ();
	if (preset == "")
	   return res;
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 2)
	QStringList list = preset.split (":", Qt::SkipEmptyParts);
#else   
	QStringList list = preset.split (":", QString::SkipEmptyParts);
#endif  
	if (list. size () != 2)
	   return res;
	res. channel	= list. at (0);
	res. serviceName = list. at (1);
	res. valid	= true;
	return res;
}
	
//
///////////////////////////////////////////////////////////////////////////////
//	
//
//	might be called when scanning only
void	RadioInterface::no_signal_found () {
	channel_timeOut ();
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
	if (configHandler_p	-> utcSelector_active ())
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
	for (auto &ss : s)
	   contentTable_p -> addLine (ss);
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

	if ((int_configValue ("saveSlides", 0) != 0) &&
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
	int	serviceOrder	= configHandler_p -> get_serviceOrder ();
	std::vector<serviceId> serviceList	=
	          my_ofdmHandler -> get_services (serviceOrder);
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

	store_widget_position (dabSettings_p, this, "mainWidget");
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
//	delete	the_ensembleHandler;
	configHandler_p	-> hide ();
//	delete	configHandler_p;
	techWindow_p	-> hide ();
	delete techWindow_p;
	if (contentTable_p != nullptr) {
	   contentTable_p -> clearTable ();
	   contentTable_p -> hide ();
	   delete contentTable_p;
	}
//	just save a few checkbox settings that are not

	if (scanTable_p != nullptr) {
	   scanTable_p	-> clearTable ();
	   scanTable_p	-> hide ();
	   delete scanTable_p;
	}
	scanMonitor. hide ();

	stopFramedumping	();
	stop_sourcedumping	();
	stopAudiodumping	();
	theScheduler. hide	();
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
	   configHandler_p -> showLoad (utilization);
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

	   if (configHandler_p -> currentStream () != "") {
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
	connect (inputDevice, SIGNAL (frameClosed ()),
	         this, SLOT (handle_deviceFrame_closed ()));
	dabSettings_p	-> setValue (SELECTED_DEVICE, s);
	if (dabSettings_p -> value (DEVICE_WIDGET_VISIBLE, 1). toInt () != 0)
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
	if (inputDevice_p != nullptr) {
	   delete inputDevice_p;
	   inputDevice_p = nullptr;
	   fprintf (stderr, "device is deleted\n");
	}

	LOG ("selecting ", deviceName);
	inputDevice_p		= create_device (deviceName);
	fprintf (stderr, "na create_device met %s\n", deviceName. toLatin1 (). data ());
	if (inputDevice_p == nullptr) {
	   inputDevice_p = new deviceHandler ();
	   return;		// nothing will happen
	}
	doStart();		// will set running
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
	   dynamicLabel	-> setStyleSheet (labelStyle);
	   dynamicLabel	-> setText (s);
	}
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

static
QString	tiiNumber (int n) {
	if (n >= 10)
	   return QString::number (n);
	return QString ("0") + QString::number (n);
}

void	RadioInterface::handle_detailButton	() {
	if (!running. load ())
	   return;
	if (techWindow_p -> isHidden ())
	   techWindow_p -> show ();
	else
	   techWindow_p -> hide ();
	dabSettings_p -> setValue (TECHDATA_VISIBLE,
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
}

void	RadioInterface::hideButtons		() {
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

static inline
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
	dabSettings_p	-> setValue (NEW_DISPLAY_VISIBLE,
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

//	channelButton handled elsewhere
	connect	(prevChannelButton, SIGNAL (clicked ()),
	         this, SLOT (handle_prevChannelButton ()));
	connect (nextChannelButton, SIGNAL (clicked ()),
	         this, SLOT (handle_nextChannelButton ()));

	connect (scanListButton, SIGNAL (clicked ()),
	         this, SLOT (handle_scanListButton ()));
	               
	connect (presetButton, SIGNAL (clicked ()),
	         this, SLOT (handle_presetButton ()));

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
}

void	RadioInterface::disconnectGUI () {
}
//
#include <QCloseEvent>
void RadioInterface::closeEvent (QCloseEvent *event) {
	
	int x = configHandler_p -> closeDirect_active ();
	dabSettings_p -> setValue (CLOSE_DIRECT_SETTING, x);
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
	nextService. valid		= true;
	nextService. channel		= theChannel;
	nextService. serviceName        = serviceName;
	nextService. SId                = 0;
	nextService. SCIds              = 0;
	presetTimer. setSingleShot (true);
	int switchDelay			=
	             configHandler_p -> switchDelayValue ();
	presetTimer. setInterval (switchDelay);
	presetTimer. start (switchDelay);
	startChannel    (channelSelector -> currentText ());
}

////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////

void	RadioInterface::stopService	(dabService &s) {
	fprintf (stderr, "callto stopService with service valid %d\n",
	                                       s. valid);
	if (!s. valid)
	   return;
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
//
//	Only presets for real input devices
	   if (!inputDevice_p -> isFileInput ()) {
	      QString s = channel. channelName + ":" + serviceName;
	      dabSettings_p	-> setValue (PRESET_NAME, s);
	   }
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
	   dabSettings_p	-> setValue (PRESET_NAME, "");
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
	distanceLabel			-> setText ("");
	transmitter_country		-> setText ("");
	transmitter_coordinates		-> setText ("");
	newDisplay. ficError_display	-> setValue (0);
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
	fprintf (stderr, "entered preset time %s %s %d\n",
	                nextService. channel. toLatin1 (). data (),
	                nextService. serviceName. toLatin1 (). data (),
	                nextService. valid);
	presetTimer. stop ();
	stopScanning ();
	if (!nextService. valid)
	   return;

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
	my_ofdmHandler	-> get_parameters (presetName, &s. SId, &s. SCIds);
	if (s. SId == 0) {
	   dynamicLabel -> setText (QString ("not all data for ") +
	                            s. serviceName +
	                             " on board");
//	   QMessageBox::warning (this, tr ("Warning"),
//	                        tr ("insufficient data for this program\n"));
	   return;
	}
	nextService. valid = false;
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
	inputDevice_p		-> resetBuffer ();
	inputDevice_p		-> restartReader (tunedFrequency);

	channel. cleanChannel ();
	channel. channelName	= theChannel;
	channel. tunedFrequency	= tunedFrequency;
	channel. realChannel	= !inputDevice_p -> isFileInput ();
	dabSettings_p		-> setValue (CHANNEL_NAME, theChannel);
	distanceLabel		-> setText ("");
	newDisplay. show_transmitters (channel. transmitters);
	bool localTransmitters =
	             configHandler_p -> localTransmitterSelector_active ();
	if (localTransmitters  && (mapHandler != nullptr))
	   mapHandler -> putData (MAP_RESET, channel. targetPos);
	else
	if (mapHandler != nullptr)
	   mapHandler -> putData (MAP_FRAME, position {-1, -1});
	the_ensembleHandler -> reset ();
	my_ofdmHandler		-> start ();
	int	switchDelay	= configHandler_p -> switchDelayValue ();
	if (!scanMonitor. active ())
	   epgTimer. start (switchDelay);
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
	disconnect (ensembleId, SIGNAL (clicked ()),
	            this, SLOT (handle_contentButton ()));
	ensembleId	-> setText ("");
	stop_sourcedumping	();
	stop_etiHandler	();	// if ant
	LOG ("channel stops ", channel. channelName);
//
//	first, stop services in fore and background
	if (channel. currentService. valid)
	   stopService (channel. currentService);

	for (auto s : channel. backgroundServices) {
	   my_ofdmHandler -> stop_service (s. subChId, BACK_GROUND);
	   if (s. fd != nullptr)
	      fclose (s. fd);
	}
	channel. backgroundServices. clear ();
	soundOut_p	-> stop ();

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
	my_ofdmHandler		-> stop ();
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
//	when running scan to data, we look at all channels, whether
//	on the skiplist or not
	QString cs = scanMonitor. getNextChannel (channelSelector -> currentText ());
	int cc = channelSelector -> findText (cs);
	LOG ("scanning starts with ", cs);
	new_channelIndex (cc);
//	scanMonitor. addText (" scanning channel " +
//	                            channelSelector -> currentText ());
	int switchDelay		=
	              configHandler_p -> switchDelayValue ();
	channelTimer. start (switchDelay);
	my_ofdmHandler	-> set_scanMode (true);
	startChannel    (channelSelector -> currentText ());
}

void	RadioInterface::start_scan_single () {
	if (configHandler_p -> clearScan_Selector_active ())
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
	scanMonitor. addText (" scanning channel " +
	                            channelSelector -> currentText ());
	int switchDelay		=
	             configHandler_p -> switchDelayValue ();
	channelTimer. start (switchDelay);
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
	             configHandler_p -> switchDelayValue ();
	channelTimer. start (switchDelay);
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
	channelTimer. stop ();

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

void	RadioInterface::channel_timeOut () {
	channelTimer. stop ();
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
	scanMonitor. addText ("scanning channel " +
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
	   QString cs	= scanMonitor. getNextChannel ();
	   int	cc	= channelSelector -> findText (cs);
	   new_channelIndex (cc);
	} catch (...) {
	   stopScanning ();
	   return;
	}

	scanMonitor. addText ("scanning channel " +
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

	QString cs	= scanMonitor. getNextChannel ();
	int cc	= channelSelector -> findText (cs);
	new_channelIndex (cc);

	int switchDelay	= 
	         configHandler_p -> switchDelayValue ();
	channelTimer. start (switchDelay);
	startChannel (channelSelector -> currentText ());
}

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
	channelSelector -> setEnabled (false);
        channelSelector -> setCurrentIndex (index);
        channelSelector -> setEnabled (true);
}
//
/////////////////////////////////////////////////////////////////////////
//	merely as a gadget, for each button the color can be set
//	Lots of code, about 400 lines, just for a gadget
//	
void	RadioInterface::set_Colors () {
	dabSettings_p	-> beginGroup (COLOR_SETTINGS);
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
	dabSettings_p	-> endGroup ();

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

	dabSettings_p	-> beginGroup (COLOR_SETTINGS);
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
	     filenameFinder. find_ficDump_file (channel. channelName);
	   if (ficDumpPointer == nullptr)
	      return;
	   my_ofdmHandler -> start_ficDump (ficDumpPointer);
	   return;
	}
	my_ofdmHandler	-> stop_ficDump ();
	ficDumpPointer = nullptr;
}

//------------------------------------------------------------------------
//
//	if configured, the interpreation of the EPG data starts automatically,
//	the servicenames of an SPI/EPG service may differ from one country
//	to another
void	RadioInterface::epgTimer_timeOut	() {
	epgTimer. stop ();
	
	if (int_configValue ("epgFlag", 0) != 1)
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
	   configHandler_p	-> mark_dlTextButton (false);
	   return;
	}

	QString	fileName = filenameFinder. finddlText_fileName (false);
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
	   dabSettings_p	-> setValue (CONFIG_WIDGET_VISIBLE, 0);
	}
	else {
	   configHandler_p -> show ();
	   dabSettings_p	-> setValue (CONFIG_WIDGET_VISIBLE, 1);
	}
}

void	RadioInterface::handle_devicewidgetButton	() {
	if (inputDevice_p == nullptr)
	   return;
	inputDevice_p	-> setVisibility (!inputDevice_p -> getVisibility ());

	dabSettings_p -> setValue (DEVICE_WIDGET_VISIBLE,
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

	QString	fileName =filenameFinder. finddlText_fileName (true);
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

	if (my_snrViewer. isHidden ())
	   my_snrViewer. show ();
	else
	   my_snrViewer. hide ();
	dabSettings_p	-> setValue (SNR_WIDGET_VISIBLE,
	                          my_snrViewer. isHidden () ? 0 : 1);
}
//
//	called from the configHandler
void	RadioInterface::handle_set_coordinatesButton	() {
coordinates theCoordinator (dabSettings_p);
	(void)theCoordinator. QDialog::exec();
	localPos. latitude		=
	             dabSettings_p -> value (HOME_LATITUDE, 0). toFloat ();
	localPos. longitude		=
	             dabSettings_p -> value (HOME_LONGITUDE, 0). toFloat ();
}
//
//	called from the configHandler
void	RadioInterface::handle_loadTable	 () {
dbLoader theLoader (dabSettings_p);
	if (theLoader. load_db ()) {
	   QMessageBox::information (this, tr ("success"),
	                            tr ("Loading and installing database complete\n"));
	   tiiProcessor. reload ();
	}
	else {
	   QMessageBox::information (this, tr ("fail"),
	                            tr ("Loading database failed\n"));
	}
}

void	RadioInterface::stop_sourcedumping	() {
	if (rawDumper_p == nullptr) 
	   return;

	LOG ("source dump stops ", "");
	my_ofdmHandler	-> stop_dumping();
	sf_close (rawDumper_p);
	rawDumper_p	= nullptr;
	configHandler_p	-> mark_dumpButton (false);
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
	configHandler_p	-> mark_dumpButton (true);
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

void	RadioInterface::handle_LoggerButton (int s) {
	(void)s;
	if (configHandler_p -> logger_active ()) {
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

void	RadioInterface::set_tii_detectorMode (bool isChecked) {
	if (my_ofdmHandler != nullptr) 
	   my_ofdmHandler	-> set_tiiDetectorMode (isChecked);
}

void	RadioInterface::handle_dcRemovalSelector (bool b) {
	if (my_ofdmHandler != nullptr)
	   my_ofdmHandler	-> set_dcRemoval  (b);
	newDisplay. set_dcRemoval (b);
}

void	RadioInterface::set_transmitters_local  (bool isChecked) {
	maxDistance = -1;
	channel. targetPos	= position {0, 0};
	if ((isChecked) && (mapHandler != nullptr))
	   mapHandler -> putData (MAP_RESET, channel. targetPos);
}

void	RadioInterface::selectDecoder (int decoder) {
	my_ofdmHandler	-> handle_decoderSelector (decoder);
}

void	RadioInterface:: set_streamSelector (int k) {
	if (!running. load ())
	   return;
#if	not defined (TCP_STREAMER) &&  not defined (QT_AUDIO)
	((audioSink *)(soundOut_p)) -> selectDevice (k);
	dabSettings_p -> setValue (AUDIO_STREAM_NAME,
	                          configHandler_p -> currentStream ());
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
	if (configHandler_p -> utcSelector_active ())
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
	if (localPos. latitude == 0) {
	   QMessageBox::information (this, tr ("Warning"),
	                         tr ("Function not available, no coordinates were found"));
	   return;
	}

	if (mapHandler == nullptr)  {
	   QString browserAddress	=
	            dabSettings_p -> value (BROWSER_ADDRESS,
	                                    "http://localhost"). toString ();
	   QString mapPort		=
	            dabSettings_p -> value (MAP_PORT_SETTING,
	                                             8080). toString ();

	   QString mapFile;
	   if (configHandler_p -> transmitterNames_active ())
	      mapFile = filenameFinder. findMaps_fileName ();
	   else
	      mapFile = ""; 

	   mapHandler = new httpHandler (this,
	                                 mapPort,
	                                 browserAddress,
	                                 localPos,
	                                 mapFile,
	                                 configHandler_p -> localBrowserSelector_active ());
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
static int teller	= 0;
static const char *slideNames [] =	
   {":res/pauze-slide-1.png",
	":res/pauze-slide-2.png",
	":res/pauze-slide-3.png",
	":res/pauze-slide-4.png",
	":res/pauze-slide-5.png"};
	teller = (teller + 1) % 5;
	QString slideName	= slideNames [teller];
	if (p. load (slideName, "png"))
	   displaySlide (p);
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
bool setting	= configHandler_p -> eti_active ();
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
cacheElement	*theTransmitter	= nullptr;

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
	if ((localPos. latitude == 0) || (localPos. longitude == 0)) 
	   return;

	if (!tiiProcessor. has_tiiFile ())
	   return;

	if ((!tiiChange) && (channel. transmitterName != ""))
	   return;

	theTransmitter =
	      tiiProcessor. get_transmitter (channel. realChannel?
	                                         channel. channelName :
	                                         "any",
	                                     channel. Eid,
	                                     mainId, subId);
	if (theTransmitter == nullptr) 
	   return;

	channel. targetPos. latitude	= theTransmitter -> latitude;
	channel. targetPos. longitude	= theTransmitter -> longitude;
	if ( (channel. targetPos. latitude == 0) ||
	    (channel. targetPos. longitude == 0)) {
	   return;
	}
	
	QString theName 	= theTransmitter -> transmitterName;
	channel. transmitterName 	= theName;
	float power		= theTransmitter -> power;
	float height		= theTransmitter -> height;

//      if positions are known, we can compute distance and corner
	channel. distance = distance	(channel. targetPos, localPos);
	channel. corner	  = corner	(channel. targetPos, localPos);
	QString labelText = theName + " " +
	                    QString::number (channel. distance, 'f', 1) + " km " +
	                    QString::number (channel. corner, 'f', 1) + 
	                    QString::fromLatin1 (" \xb0 ") + 
	                    " (" +
	                    QString::number (height, 'f', 1) +  "m)";
	fprintf (stderr, "%s (%f)\n", labelText. toUtf8 (). data (), height);
	QFont f ("Arial", 9);
	distanceLabel		->  setFont (f);
	distanceLabel		-> setText (labelText);
//	see if we have a map
	if (mapHandler == nullptr) 
	   return;

	uint8_t key	= MAP_NORM_TRANS;	// default value
	bool localTransmitters	=
	            configHandler_p -> localTransmitterSelector_active ();
	if ((!localTransmitters) && (channel. distance > maxDistance)) { 
	   maxDistance = channel. distance;
	   key = MAP_MAX_TRANS;
	}
//
	QDateTime theTime = 
	   configHandler_p -> utcSelector_active () ?
	                  QDateTime::currentDateTimeUtc () :
	                  QDateTime::currentDateTime ();

	mapHandler -> putData (key,
	                       channel. targetPos, 
	                       channel. transmitterName,
	                       channel. channelName,
	                       theTime. toString (Qt::TextDate),
	                       channel. mainId * 100 + channel. subId,
	                       channel. distance,
	                       channel. corner, power, height);
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
	dabSettings_p	-> setValue (LABEL_COLOR, labelStyle);
}

int	RadioInterface::int_configValue	(const QString &key, int d) {
int val;
	val	= dabSettings_p -> value (key, d). toInt ();
	return val;
}

void	RadioInterface::show_dcOffset (float dcOffset) {
	newDisplay. show_dcOffset (dcOffset);
}

void	RadioInterface::handle_techFrame_closed () {
	dabSettings_p -> setValue (TECHDATA_VISIBLE, 0);
}

void	RadioInterface::handle_configFrame_closed () {
	dabSettings_p -> setValue (CONFIG_WIDGET_VISIBLE, 0);
}

void	RadioInterface::handle_deviceFrame_closed () {
	fprintf (stderr, "Geregistreerd\n");
	dabSettings_p	-> setValue (DEVICE_WIDGET_VISIBLE, 0);
}

void	RadioInterface::handle_newDisplayFrame_closed () {
	fprintf (stderr, "ook newDisplay clodes\n");
	dabSettings_p -> setValue (NEW_DISPLAY_VISIBLE, 0);
}

