#
/*
 *    Copyright (C) 2013, 2014, 2015, 2016, 2017, 2018, 2019, 2020
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
#include	"ui_config.h"
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
	                                bool		error_report,
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
	                                        theBand (freqExtension),
	                                        theTable (this) {
int16_t	latency;
int16_t k;
QString h;
QString	presetName;
uint8_t	dabBand;

	dabSettings		= Si;
	this	-> error_report	= error_report;
	running. 		store (false);
	scanning. 		store (false);
	my_dabProcessor		= nullptr;
	isSynced		= false;
	stereoSetting		= false;
//
//	"globals" is introduced to reduce the number of parameters
//	for the dabProcessor
	globals. spectrumBuffer          = &spectrumBuffer;
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
	      nextService. serviceName = presetName;
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
	normalScan	=
	           dabSettings -> value ("normalScan", 0). toInt () == 1;

//	The settings are done, now creation of the GUI parts
	setupUi (this);
//
	dataDisplay	= new QFrame (nullptr);
	techData. setupUi (dataDisplay);

	configDisplay	= new QFrame (nullptr);
	configWidget. setupUi (configDisplay);

	int x = dabSettings -> value ("switchTime", 8). toInt ();
	configWidget. switchTimeSetting -> setValue (x);

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
	bool	b	=
	            dabSettings -> value ("motSlides", 0). toInt () == 1;
	motSlides	= b ? new QLabel () : nullptr;
	if (motSlides != nullptr)
	   motSlides		-> hide ();
	dataDisplay		-> hide ();
	stillMuting		-> hide ();
	serviceList. clear ();
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
	QString historyFile     = QDir::homePath () + "/.qt-history.xml";
        historyFile             =
	             dabSettings -> value ("history", historyFile). toString ();
        historyFile             = QDir::toNativeSeparators (historyFile);
        my_history              = new historyHandler (this, historyFile);

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
        connect (devicewidgetButton, SIGNAL (rightClicked ()),
                 this, SLOT (color_devicewidgetButton ()));
        connect (historyButton, SIGNAL (rightClicked ()),
                 this, SLOT (color_historyButton ()));
	connect (dumpButton, SIGNAL (rightClicked (void)),
	         this, SLOT (color_sourcedumpButton (void)));
	connect (muteButton, SIGNAL (rightClicked (void)),
	         this, SLOT (color_muteButton (void)));

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

	QPalette *lcdPalette	= new QPalette;
	lcdPalette	-> setColor (QPalette::Background, Qt::white);
	lcdPalette	-> setColor (QPalette::Base, Qt::black);
	snrDisplay	-> setPalette (*lcdPalette);
	snrDisplay	-> setAutoFillBackground (true);
	frequencyDisplay	-> setPalette (*lcdPalette);
	frequencyDisplay	-> setAutoFillBackground (true);
	cpuMonitor	-> setPalette (*lcdPalette);
	cpuMonitor	-> setAutoFillBackground (true);
	correctorDisplay	-> setPalette (*lcdPalette);
	correctorDisplay	-> setAutoFillBackground (true);

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
#ifdef	HAVE_PLUTO
	deviceSelector	-> addItem ("pluto");
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
//	fprintf (stderr, "%d %s\n", k, h. toUtf8(). data());
	if (k != -1) {
	   deviceSelector       -> setCurrentIndex (k);
	   inputDevice	= setDevice (deviceSelector -> currentText());
	}

	set_Colors ();
	localTimeDisplay -> setStyleSheet ("QLabel {background-color : gray; color: white}");
	runtimeDisplay	-> setStyleSheet ("QLabel {background-color : gray; color: white}");

	if (dabSettings	-> value ("spectrumVisible", 0). toInt () == 1) 
	   my_spectrumViewer. show ();
	if (dabSettings -> value ("tiiVisible", 0). toInt () == 1) 
	   my_tiiViewer. show ();
	if (dabSettings -> value ("correlationVisible", 0). toInt () == 1)
	   my_correlationViewer. show ();

//	if a device was selected, we just start, otherwise
//	we wait until one is selected
	currentServiceDescriptor	= nullptr;

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
//	doStart (QString) is called when - on startup - no device
//	was registered to be used, and the user presses the
//	selectDevice comboBox
void	RadioInterface::doStart (const QString &dev) {
	(void)dev;
	inputDevice = setDevice	(dev);
	if (inputDevice == nullptr) {
//	just in case someone wants to push all those nice buttons that
//	are now connected to erroneous constructs
//	Some buttons should not be touched before we have a device
	   disconnectGUI ();
	   return;
	}
	doStart ();
}
//
//	when doStart is called, a device is available and selected
bool	RadioInterface::doStart	() {
int	switchTime;
	QString h       = dabSettings -> value ("channel", "12C"). toString();
	int k           = channelSelector -> findText (h);
	if (k != -1) 
	   channelSelector -> setCurrentIndex (k);

	my_dabProcessor	= new dabProcessor  (this, inputDevice, &globals);

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
	            this,  SLOT (newDevice (const QString &)));

	connect (deviceSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (newDevice (const QString &)));
//
	secondariesVector. resize (0);	
	if (nextService. valid) {
	   switchTime		=
	                  dabSettings -> value ("switchTime", 8). toInt ();
	   presetTimer. setSingleShot	(true);
	   presetTimer. setInterval 	(switchTime * 1000);
	   presetTimer. start 		(switchTime * 1000);
	}

	startChannel (channelSelector -> currentText ());
	running. store (true);
	return true;
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

	if (currentService. valid)
	   s	-> setValue ("presetname", currentService. serviceName);
	else
	   s	-> setValue ("presetname", "");

	s	-> setValue ("channel", channelSelector -> currentText ());
	s	-> setValue ("device",
	                      deviceSelector -> currentText());
	s	-> setValue ("soundchannel",
	                               streamoutSelector -> currentText());
	if (inputDevice != nullptr)
           s    -> setValue ("devicewidgetButton",
                                  inputDevice -> isHidden () != 0);
	s	-> setValue ("spectrumVisible",
	                          my_spectrumViewer. isHidden () ? 0 : 1);
	s	-> setValue ("tiiVisible",
	                          my_tiiViewer. isHidden () ? 0 : 1);
	s	-> setValue ("correlationVisible",
	                          my_correlationViewer. isHidden () ? 0 : 1);
	s	-> sync();
}

//
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

	serviceOrder	=
	    dabSettings -> value ("serviceOrder", ALPHA_BASED). toInt ();
	if (serviceOrder	== SUBCH_BASED) {
	   audiodata ad;
	   packetdata pd;
	   my_dabProcessor	-> dataforAudioService (serviceName, &ad);
	   if (ad. defined)
	      ed. subChId	= ad. subchId;
	   else {
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
	if (normalScan) {
	   stopScanning (false);
	}
}

//////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////

static inline
bool	isValid (QChar c) {
	return c. isLetter () || c. isDigit () || (c == '-');
}

void	RadioInterface::handle_contentButton	() {
ensemblePrinter	my_Printer;
QString suggestedFileName;
QString currentChannel	= channelSelector -> currentText();
int32_t	frequency	= inputDevice -> getVFOFrequency();
QString	saveDir		= dabSettings -> value ("contentDir",
	                                        QDir::homePath ()). toString ();

	if (!running. load() || (ensembleId -> text () == QString ("")))
	   return;
	if (scanning. load ())
	   return;

	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	QString theTime	= localTimeDisplay -> text ();
	for (int i = 0; i < theTime. length (); i ++)
	   if (!isValid (theTime. at (i)))
	      theTime. replace (i, 1, '-');
	suggestedFileName = saveDir + "Qt-DAB-" + currentChannel +
	                    "-" + theTime;
	
	fprintf (stderr, "suggested filename %s\n",
	                         suggestedFileName. toLatin1 (). data ());
	QString fileName = QFileDialog::getSaveFileName (this,
	                                        tr ("Save file ..."),
	                                        suggestedFileName + ".txt",
	                                        tr ("Text (*.txt)"));
	if (fileName == "")
	   return;

	fileName	= QDir::toNativeSeparators (fileName);
	FILE *file_P	= fopen (fileName. toUtf8(). data(), "w");

	if (file_P == nullptr) {
	   fprintf (stderr, "Could not open file %s\n",
	                              fileName. toUtf8(). data());
	   return;
	}

	QString	dumper	= QDir::fromNativeSeparators (fileName);
        int x           = dumper. lastIndexOf ("/");
        saveDir         = dumper. remove (x, dumper. count () - x);
        dabSettings     -> setValue ("contentDir", saveDir);
//
//	we asserted that my_dabProcessor exists
	my_Printer. showEnsembleData (currentChannel,
	                              frequency,
	                              localTimeDisplay -> text (),
	                              serviceList,
	                              my_dabProcessor, file_P);
	fclose (file_P);
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
static int fileNumber	= 0;
static int propNumber	= 0;

void	RadioInterface::handle_motObject (QByteArray result,
	                                  QString name,
	                                  int contentType, bool dirElement) {
QString realName;

	fprintf (stderr, "handle_MOT: type %x (%x), name %s dir = %d\n",
	                           contentType,
	                           getContentBaseType ((MOTContentType)contentType),
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
	      fprintf (stderr, "epg\n");
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
//	         QString fn = "/tmp/epgfile" + QString::number (fileNumber ++) + ".xxx";
//	         fprintf (stderr, "going to write %s (%d bytes)\n",
//	                                      fn. toLatin1 (). data (),
//	                                      (int)(epgData. size ()));
//	         FILE *f = fopen (fn. toLatin1 (). data (), "w+b");
//	         fwrite (epgData. data (), 1, epgData. size (), f);
//	         fclose (f);
//	         epgHandler. decode (epgData, realName);
	      }
//	      fprintf (stderr, "epg file %s\n",
//	                            realName. toLatin1 (). data ());
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
	
	if (!currentService. is_audio)
	   return;

	if (motSlides == nullptr) {
	   int w   = techData. pictureLabel -> width ();
           int h   = 2 * w / 3;
           techData. pictureLabel ->
                  setPixmap (p. scaled (w, h, Qt::KeepAspectRatio));
           techData. pictureLabel -> show ();
	}
	else {
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
	displayTimer. stop	();
	channelTimer.  stop	();
	presetTimer.  stop	();
	soundOut	-> stop ();
	if (my_dabProcessor != nullptr)
	   my_dabProcessor -> stop ();
	my_presetHandler. savePresets (presetSelector);
	stop_frameDumping	();
	stop_sourceDumping	();
	stop_audioDumping	();
	theTable. hide		();
	dataDisplay	->  hide();
	if (motSlides != nullptr)
	   motSlides	-> hide ();
	usleep (1000);		// pending signals
//	everything should be halted by now

	my_spectrumViewer. hide ();
	my_correlationViewer. hide ();
	my_tiiViewer. hide ();
	if (my_dabProcessor != nullptr)
	   delete	my_dabProcessor;
	if (inputDevice != nullptr)
	   delete	inputDevice;

	delete		soundOut;
	if (motSlides != nullptr)
	   delete	motSlides;
	if (currentServiceDescriptor != nullptr)
	   delete currentServiceDescriptor;
	delete	dataDisplay;
	delete	configDisplay;
	delete	my_history;
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
#ifdef	HAVE_PLUTO
	if (s == "pluto") {
	   try {
	      inputDevice = new plutoHandler (dabSettings, version);
	      showButtons();
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("Warning"),
	                                  tr ("no pluto device found\n"));
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

	fprintf (stderr, "op naar een nieuw device\n");
//	Part I : stopping all activities
	running. store (false);
	stopChannel	();
	disconnectGUI();
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

///////////////////////////////////////////////////////////////////////////
//	signals, received from ofdm_decoder that data is
//	to be displayed
///////////////////////////////////////////////////////////////////////////


void	RadioInterface::showTime	(const QString &s) {
	localTimeDisplay -> setText (s);
}

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

void	RadioInterface::show_rsErrors (int s) {
	if (!running. load())		// should not happen
	   return;
	QPalette p      = techData. rsError_display -> palette();
	if (100 - 4 * s < 80)
           p. setColor (QPalette::Highlight, Qt::red);
	else
           p. setColor (QPalette::Highlight, Qt::green);
	techData. rsError_display	-> setPalette (p);
	techData. rsError_display	-> setValue (100 - 4 * s);
}
	
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
	   ficError_display                -> setPalette (p);
	   ficError_display	-> setValue (ficSuccess);
	   total_ficError	+= 100 - ficSuccess;
	   total_fics		+= 100;
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
	syncedLabel	-> setStyleSheet (b ?
	   	              "QLabel {background-color: green; color : white}":
	   	              "QLabel {background-color: red; color : white}");
}

void	RadioInterface::showLabel	(QString s) {
	if (running. load())
	   dynamicLabel	-> setText (s);
}

void	RadioInterface::setStereo	(bool s) {
	if (!running. load ())
	   return;
	if (stereoSetting == s)
	   return;
	
	techData. stereoLabel	-> setStyleSheet (s ?
	   	         "QLabel {background-color: green; color : white}":
	   	         "QLabel {background-color: red; color : white}");
	techData. stereoLabel	-> setText (s ? "stereo" : "mono");
	stereoSetting = s;
}

void	RadioInterface::show_tii	(QByteArray data) {
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
	   my_tiiViewer. showSecondaries (data);
        }
	my_tiiViewer. showSpectrum (1);

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

void	RadioInterface::show_rsCorrections	(int c) {
	if (!running)
	   return;
	techData. rsCorrections	-> display (c);
}

void	RadioInterface::show_clockError	(int e) {
	if (!running. load ())
	   return;
	my_spectrumViewer. show_clockErr (e);
}

void	RadioInterface::showCorrelation	(int amount, int marker) {
	if (!running. load())
	   return;
	my_correlationViewer. showCorrelation (amount, marker);
}

void	RadioInterface::showIndex	(int ind) {
	if (!running. load())
	   return;

	my_correlationViewer. showIndex (ind);
}

//
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

void	RadioInterface::handle_detailButton	() {
	if (!running. load ())
	   return;

	if (dataDisplay -> isHidden())
	   dataDisplay -> show();
	else
	   dataDisplay -> hide();
}

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

void	RadioInterface::stop_sourceDumping	() {
	if (rawDumper == nullptr) 
	   return;

	my_dabProcessor	-> stopDumping();
	sf_close (rawDumper);
	rawDumper	= nullptr;
	QFont font      = dumpButton -> font ();
        font. setPointSize (10);
        dumpButton -> setFont (font);
	dumpButton	-> setText ("Raw dump");
	dumpButton	-> update ();
}
//
//	This is a response on a GUI event, so the existence
//	of dabProcessor was already asserted
void	RadioInterface::start_sourceDumping () {
SF_INFO *sf_info        = (SF_INFO *)alloca (sizeof (SF_INFO));
QString deviceName	= inputDevice -> deviceName ();
QString channelName	= channelSelector -> currentText ();
QString suggestedFileName;
QString	saveDir		= dabSettings -> value ("saveDir_rawDump",
	                                         QDir::homePath ()). toString ();
	if (scanning. load ())
	   return;

	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';
	
	if (deviceName == "") {
	   fprintf (stderr, "No dumping possible from selected reader\n");
	   return;
	}

	QString theTime	= localTimeDisplay -> text ();
	for (int i = 0; i < theTime. length (); i ++)
	   if (!isValid (theTime. at (i)))
	      theTime. replace (i, 1, '-');

	suggestedFileName = saveDir + deviceName + "-" + channelName +
	                                                    "-" + theTime;
	QString file = QFileDialog::getSaveFileName (this,
	                                     tr ("Save file ..."),
	                                     suggestedFileName + ".sdr",
	                                     tr ("raw data (*.sdr)"));

	if (file == QString (""))       // apparently cancelled
	   return;
	if (!file.endsWith (".sdr", Qt::CaseInsensitive))
	   file.append (".sdr");
	file	= QDir::toNativeSeparators (file);
	sf_info -> samplerate   = INPUT_RATE;
	sf_info -> channels     = 2;
	sf_info -> format       = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	rawDumper = sf_open (file. toUtf8(). data(),
	                                   SFM_WRITE, sf_info);
	if (rawDumper == nullptr) {
	   qDebug() << "cannot open " << file. toUtf8(). data();
	   return;
	}

	QString dumper	= QDir::fromNativeSeparators (file);
	int x		= dumper. lastIndexOf ("/");
	saveDir		= dumper. remove (x, dumper. count () - x);
	dabSettings	-> setValue ("saveDir_rawDump", saveDir);

	QFont font      = dumpButton -> font ();
        font. setPointSize (12);
        dumpButton -> setFont (font);
	dumpButton	-> setText ("writing");
	dumpButton	-> update ();
	my_dabProcessor -> startDumping (rawDumper);
}

void	RadioInterface::handle_sourcedumpButton () {
	if (!running. load ())
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
	audioDumper = nullptr;
	QString audiodumpButton_color =
           dabSettings -> value (AUDIODUMP_BUTTON + "_color",
	                                              "white"). toString ();
	QString audiodumpButton_font =
           dabSettings -> value (AUDIODUMP_BUTTON + "_font",
	                                               "black"). toString ();

	QString temp = "QPushButton {background-color: %1; color: %2}";
	techData. audiodumpButton
	                -> setStyleSheet (temp. arg (audiodumpButton_color,
	                                             audiodumpButton_font));
	QFont font      = techData. audiodumpButton -> font ();
        font. setPointSize (10);
        dumpButton -> setFont (font);
	techData. audiodumpButton	-> setText ("audio dump");
	techData. audiodumpButton	-> update ();
}

void	RadioInterface::start_audioDumping () {
SF_INFO	*sf_info	= (SF_INFO *)alloca (sizeof (SF_INFO));
QString	saveDir	 = dabSettings -> value ("saveDir_audioDump",
	                                 QDir::homePath ()).  toString ();
	if (scanning. load ())
	   return;

	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	QString tailS = serviceLabel -> text () + "-" + 
	                                      localTimeDisplay -> text ();
	for (int i = 0; i < tailS. length (); i ++)
	   if (!isValid (tailS. at (i))) 
	      tailS. replace (i, 1, '-');
	QString suggestedFileName = saveDir + tailS;
	QString file = QFileDialog::getSaveFileName (this,
	                                        tr ("Save file ..."),
	                                        suggestedFileName + ".wav",
	                                        tr ("PCM wave file (*.wav)"));
	if (file == QString (""))
	   return;
	if (!file.endsWith (".wav", Qt::CaseInsensitive))
	   file.append (".wav");
	file		= QDir::toNativeSeparators (file);
	sf_info		-> samplerate	= 48000;
	sf_info		-> channels	= 2;
	sf_info		-> format	= SF_FORMAT_WAV | SF_FORMAT_PCM_16;

	audioDumper	= sf_open (file. toUtf8(). data(),
	                                   SFM_WRITE, sf_info);
	if (audioDumper == nullptr) {
	   qDebug() << "Cannot open " << file. toUtf8(). data();
	   return;
	}

	QString	dumper	= QDir::fromNativeSeparators (file);
	int x		= dumper. lastIndexOf ("/");
	saveDir		= dumper. remove (x, dumper. count () - x);
	dabSettings	-> setValue ("saveDir_audioDump", saveDir);

	QFont font	= techData. audiodumpButton -> font ();
        font. setPointSize (12);
        techData. audiodumpButton -> setFont (font);
	techData. audiodumpButton	-> setText ("writing");
	techData. audiodumpButton	-> update ();
	soundOut		-> startDumping (audioDumper);
}

void	RadioInterface::handle_audiodumpButton () {
	if (!running. load ())
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

	QFont font      = techData. framedumpButton -> font ();
        font. setPointSize (10);
        techData. framedumpButton -> setFont (font);
	techData. framedumpButton	-> setText ("frame dump");
	techData. framedumpButton	-> update ();
	frameDumper	= nullptr;
}

void	RadioInterface::start_frameDumping () {
QString	saveDir	= dabSettings -> value ("saveDir_frameDump",
	                                QDir::homePath ()).  toString ();
	if ((saveDir != "") && (!saveDir. endsWith ('/')))
	   saveDir = saveDir + '/';

	if (scanning. load ())
	   return;

	QString tailS	= serviceLabel -> text () + "-" +
	                                 localTimeDisplay -> text ();
	for (int i = 0; i < tailS. length (); i ++)
	   if (!isValid (tailS. at (i)))
	      tailS. replace (i,1, '-');
	QString suggestedFileName = saveDir + tailS;
	QString file = QFileDialog::getSaveFileName (this,
	                                     tr ("Save file ..."),
	                                     suggestedFileName + ".aac",
	                                     tr ("aac data (*.aac)"));
	if (file == QString (""))       // apparently cancelled
	   return;

	file		= QDir::toNativeSeparators (file);
	frameDumper = fopen (file. toLatin1 (). data (), "w+b");
	if (frameDumper == nullptr) {
	   QString s = QString ("cannot open ") + file;
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr (s. toLatin1 (). data ()));
	   return;
	}

	QString dumper	= QDir::fromNativeSeparators (file);
	int x		= dumper. lastIndexOf ("/");
	saveDir		= dumper. remove (x, dumper. count () - x);
	dabSettings	-> setValue ("saveDir_frameDump", saveDir);

	QFont font	= techData. framedumpButton -> font ();
        font. setPointSize (12);
        techData. framedumpButton	-> setFont (font);
	techData. framedumpButton	-> setText ("recording");
	techData. framedumpButton	-> update ();
}

void	RadioInterface::handle_framedumpButton () {
	if (!running. load ())
	   return;

	if (frameDumper != nullptr) 
	   stop_frameDumping ();
	else
	   start_frameDumping ();
}

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
	   my_spectrumViewer. show();
	else
	   my_spectrumViewer. hide();
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
        connect (devicewidgetButton, SIGNAL (clicked ()),
                 this, SLOT (handle_devicewidgetButton ()));
        connect (historyButton, SIGNAL (clicked ()),
                 this, SLOT (handle_historyButton ()));
	connect (dumpButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_sourcedumpButton (void)));
	connect (muteButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_muteButton (void)));

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

	connect (ensembleDisplay, SIGNAL (clicked (QModelIndex)),
	         this, SLOT (selectService (QModelIndex)));

	connect (configButton, SIGNAL (clicked ()),
	         this, SLOT (handle_configSetting ()));
	connect (configWidget. muteTimeSetting, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_muteTimeSetting (int)));
	connect (configWidget. switchTimeSetting,
	                                 SIGNAL (valueChanged (int)),
	         this, SLOT (handle_switchTimeSetting (int)));
	connect (configWidget. orderAlfabetical, SIGNAL (clicked ()),
	         this, SLOT (handle_orderAlfabetical ()));
	connect (configWidget. orderServiceIds, SIGNAL (clicked ()),
	         this, SLOT (handle_orderServiceIds ()));
	connect (configWidget. ordersubChannelIds, SIGNAL (clicked ()),
	         this, SLOT (handle_ordersubChannelIds ()));
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
        disconnect (devicewidgetButton, SIGNAL (clicked ()),
                 this, SLOT (handle_devicewidgetButton ()));
        disconnect (historyButton, SIGNAL (clicked ()),
                 this, SLOT (handle_historyButton ()));
	disconnect (dumpButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_sourcedumpButton (void)));
	disconnect (muteButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_muteButton (void)));

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

	disconnect (ensembleDisplay, SIGNAL (clicked (QModelIndex)),
	         this, SLOT (selectService (QModelIndex)));

	disconnect (configButton, SIGNAL (clicked ()),
	            this, SLOT (handle_configSetting ()));
	disconnect (configWidget. muteTimeSetting,
	                                    SIGNAL (valueChanged (int)),
	            this, SLOT (handle_muteTimeSetting (int)));
	disconnect (configWidget. switchTimeSetting,
	                                    SIGNAL (valueChanged (int)),
	            this, SLOT (handle_switchTimeSetting (int)));
	disconnect (configWidget. orderAlfabetical, SIGNAL (clicked ()),
	            this, SLOT (handle_orderAlfabetical ()));
	disconnect (configWidget. orderServiceIds, SIGNAL (clicked ()),
	            this, SLOT (handle_orderServiceIds ()));
	disconnect (configWidget. ordersubChannelIds, SIGNAL (clicked ()),
	            this, SLOT (handle_ordersubChannelIds ()));
}
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
	if (!running. load ())
	   return QWidget::eventFilter (obj, event);
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
	                  currentService. serviceName. toLatin1 (). data (),
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
	      if (my_dabProcessor == nullptr) {
	         fprintf (stderr, "expert error 5\n");
	         return true;
	      }
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
	                             name. toLatin1 (). data (), subChId);
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
	                              name. toLatin1 (). data ());
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
//	presetSelector	-> setCurrentIndex (0);
        localSelect (s);
}

void    RadioInterface::handle_presetSelector (const QString &s) {
	if (!running. load ())
	   return;

        presetTimer. stop ();
        if ((s == "Presets") || (presetSelector -> currentIndex () == 0))
           return;
        localSelect (s);
}

void	RadioInterface::localSelect (const QString &s) {
int	switchTime;
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
	switchTime			=
	                  dabSettings -> value ("switchTime", 8). toInt ();
        presetTimer. setInterval (switchTime * 1000);
        presetTimer. start (switchTime * 1000);
        startChannel    (channelSelector -> currentText ());
}

////////////////////////////////////////////////////////////////////////////
//
//	handling services: stop and start
///////////////////////////////////////////////////////////////////////////

void	RadioInterface::stopService	() {
	presetTimer. stop ();
//	presetSelector	-> setCurrentIndex (0);
	channelTimer. stop ();
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
	         colorService (model. index (i, 0), Qt::black, 11);
	         break;
	      }
	   }
	   currentService. valid = false;
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

	presetTimer.	stop();
	channelTimer.	stop ();
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

	if (motSlides != nullptr)
	   motSlides	-> hide ();

        techData. pictureLabel -> hide ();

	int rowCount	= model. rowCount ();
	for (int i = 0; i < rowCount; i ++) {
	   QString itemText =
	           model. index (i, 0). data (Qt::DisplayRole). toString ();
	   if (itemText == serviceName) {
	      colorService (model. index (i, 0), Qt::red, 15);
	      serviceLabel	-> setStyleSheet ("QLabel {color : black}");
	      serviceLabel	-> setText (serviceName);
	      audiodata ad;
	      
	      my_dabProcessor -> dataforAudioService (serviceName, &ad);
	      if (ad. defined) {
	         currentService. valid = true;
	         currentService. is_audio	= true;
	         start_audioService (&ad);
	      }
	      else
	      if (my_dabProcessor -> is_packetService (serviceName)) {
	         currentService. valid = true;
	         currentService. is_audio	= false;
	         start_packetService (serviceName);
	      }
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

	serviceLabel -> setAlignment(Qt::AlignCenter);
	serviceLabel -> setText (ad -> serviceName);

	my_dabProcessor -> set_audioChannel (ad, &audioBuffer);
	for (int i = 1; i < 10; i ++) {
	   packetdata pd;
	   my_dabProcessor -> dataforPacketService (ad -> serviceName, &pd, i);
	   if (pd. defined) {
	      my_dabProcessor -> set_dataChannel (&pd, &dataBuffer);
	      fprintf (stderr, "adding %s (%d) as subservice\n",
	                            pd. serviceName. toLatin1 (). data (),
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
	         colorService (model. index (i, 0), Qt::black, 11);
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
	nextService. valid = false;
//
//	not found, no service selected
	fprintf (stderr, "presetName %s not found\n", presetName. toLatin1 (). data ());
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
	inputDevice		-> restartReader (tunedFrequency);
	my_dabProcessor		-> start (tunedFrequency);
	show_for_safety ();
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
//	note framedumping - if any - was already stopped
	presetTimer. stop ();
        channelTimer. stop ();
//
//	The services - if any - need to be stopped
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

	presetTimer. stop ();
	presetSelector		-> setCurrentIndex (0);
	stopScanning	(false);
	stopChannel	();
	startChannel	(channel);
}

void	RadioInterface::handle_nextChannelButton () {
	set_channelButton (channelSelector -> currentIndex () + 1);
}

void	RadioInterface::handle_prevChannelButton () {
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
	if (currentChannel < 0)
	   currentChannel = channelSelector -> count () - 1;
	if (currentChannel >= channelSelector -> count ())
	   currentChannel = 0;
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
int	switchTime;
	presetTimer. stop ();
	channelTimer. stop ();

	connect (my_dabProcessor, SIGNAL (No_Signal_Found ()),
	         this, SLOT (No_Signal_Found ()));
	new_presetIndex (0);
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
	if (!normalScan)
	   scanDumpFile	= findScanDump_FileName ();
	else
	   scanDumpFile = nullptr;
	my_dabProcessor	-> set_scanMode (true);
//      To avoid reaction of the system on setting a different value:
	new_channelIndex (cc);
	dynamicLabel	-> setText ("scanning channel " +
	                                     channelSelector -> currentText ());
        scanButton      -> setText ("scanning");
	switchTime		=
	                  dabSettings -> value ("switchTime", 8). toInt ();
        channelTimer. start (switchTime * 1000);

        startChannel    (channelSelector -> currentText ());
	if (!normalScan) {
	   theTable. clear ();
	   theTable. show ();
	}
}

void	RadioInterface::stopScanning	(bool dump) {
	disconnect (my_dabProcessor, SIGNAL (No_Signal_Found ()),
	            this, SLOT (No_Signal_Found ()));
	my_dabProcessor	-> set_scanMode (false);
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

//	theTable. hide ();
	dynamicLabel	-> setText ("Scan ended");
        scanButton      -> setText ("scan");
}

//	If the ofdm processor has waited - without success -
//	for a period of N frames to get a start of a synchronization,
//	it sends a signal to the GUI handler
//	If "scanning" is "on" we hop to the next frequency on
//	the list.
//	Also called as a result of time out on channelTimer

void	RadioInterface::No_Signal_Found () {
int	switchTime;
	disconnect (my_dabProcessor, SIGNAL (No_Signal_Found (void)),
	            this, SLOT (No_Signal_Found (void)));
        disconnect (&channelTimer, SIGNAL (timeout (void)),
                    this, SLOT (channel_timeOut (void)));

	channelTimer. stop ();
	if (running. load () && scanning. load ()) {
	   int	cc	= channelSelector -> currentIndex ();
	   if ((!normalScan) && (serviceList. size () > 0))
	      showServices ();
	   stopChannel ();
	   cc ++;
	   if ((cc >= channelSelector -> count ()) && !normalScan) {
	      stopScanning (true);
	   }
	   else {  // we just continue
	      if (cc >= channelSelector -> count ())
	         cc = 0;
//	To avoid reaction of the system on setting a different value:
	      new_channelIndex (cc);

	      connect (my_dabProcessor, SIGNAL (No_Signal_Found (void)),
	               this, SLOT (No_Signal_Found (void)));
	      connect (&channelTimer, SIGNAL (timeout (void)),
                       this, SLOT (channel_timeOut (void)));

	      dynamicLabel -> setText ("scanning channel " +
	                                  channelSelector -> currentText ());
	      switchTime	= 
	                  dabSettings -> value ("switchTime", 8). toInt ();
	      startChannel (channelSelector -> currentText ());
	      channelTimer. start (switchTime);
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
QString SNR = "SNR " + QString::number (snrDisplay -> value ());
QString ensembleId	= hextoString (my_dabProcessor -> get_ensembleId ());
	if (my_dabProcessor == nullptr) {
	   fprintf (stderr, "Expert error 26\n");
	   return;
	}
	theTable. newEnsemble (" ",
	                       channelSelector -> currentText (),
	                       my_dabProcessor	-> get_ensembleName (),
	                       ensembleId,
	                       SNR,
	                       transmitter_coordinates -> text ());
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
	if (scanDumpFile != nullptr)
	   my_Printer. showEnsembleData (channelSelector -> currentText (),
	                                 inputDevice -> getVFOFrequency (),
		                         localTimeDisplay -> text (),
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
           muteButton   -> setText ("mute");
           muteButton   -> update ();
	   stillMuting	-> hide ();
           muting = false;
	}
}

void	RadioInterface::handle_muteButton	() {
	if (muting) {
	   muteTimer. stop ();
	   disconnect (&muteTimer, SIGNAL (timeout ()),
	               this, SLOT (muteButton_timeOut ()));
	   muteButton	-> setText ("mute");
	   muteButton	-> update ();
	   muting = false;
	   stillMuting	-> hide ();
	   return;
	}

	connect (&muteTimer, SIGNAL (timeout (void)),
                 this, SLOT (muteButton_timeOut (void)));
	muteDelay	= dabSettings -> value ("muteTime", 2). toInt ();
	muteDelay	*= 60;
        muteTimer. start (1000);
	muteButton	-> setText ("MUTING");
	stillMuting	-> show ();
	stillMuting	-> display (muteDelay);
	muteButton	-> update ();
	muting = true;
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
	set_buttonColors (dumpButton, DUMP_BUTTON);
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

void	RadioInterface::handle_switchTimeSetting	(int newV) {
	dabSettings	-> setValue ("switchTime", newV);
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


