#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the dab-mini
 *
 *    dab-mini is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, version 2 of the License.
 *
 *    dab-mini is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dab-mini; if not, write to the Free Software
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
#include	"audiosink.h"
#include        "schedule-selector.h"
#include        "element-selector.h"
#ifdef	HAVE_RTLSDR
#include	"rtlsdr-handler.h"
#endif
#ifdef	HAVE_SDRPLAY
#include	"sdrplay-handler.h"
#endif
#ifdef	HAVE_SDRPLAY_V3
#include	"sdrplay-handler-v3.h"
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
#endif
//
//	approach: if a preset is called for, the "nextService"
//	is set,
//	if a service is running "currentService" is set
//	the "secondService" is a gadget, it allows for writing
//	the aac contents of another service

dabService	nextService;
dabService	currentService;
dabService	secondService;
//
//	Note:
//	The buffers, like spectrumBuffer etc,
//	are needed for the dabProcessor, there content is
//	not touched here
	RadioInterface::RadioInterface (QSettings	*Si,
	                                const QString	&presetFile,
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
	                                        my_presetHandler (this),
	                                        theBand ("", Si ),
	                                        filenameFinder (Si),
	                                        the_dlCache (10),
	                                        theScheduler (this) {
int16_t	latency;
int16_t k;
QString h;
QString	presetName;

	dabSettings		= Si;
	running. 		store (false);
	scanning. 		store (false);
	this	-> dlTextFile	= nullptr;
//
//	These buffers are not used here, but needed as parameter
//	in dabProcessor
//	"globals" is introduced to reduce the number of parameters
//	for the dabProcessor
	globals. spectrumBuffer		= &spectrumBuffer;
	globals. iqBuffer		= &iqBuffer;
	globals. responseBuffer		= &responseBuffer;
	globals. tiiBuffer		= &tiiBuffer;
	globals. snrBuffer		= &snrBuffer;
//
//	frameBuffer is used for the "secondService"
	globals. frameBuffer		= &frameBuffer;
	globals. dabMode		= 1;
	globals. threshold	=
	                  dabSettings -> value ("threshold", 3). toInt();
	globals. diff_length	=
	           dabSettings	-> value ("diff_length", DIFF_LENGTH). toInt();
	globals. tii_delay	=
	           dabSettings  -> value ("tii_delay", 5). toInt();
	if (globals. tii_delay < 5)
	   globals. tii_delay = 5;
	globals. tii_depth	=
	               dabSettings -> value ("tii_depth", 1). toInt();
	globals. echo_depth	=
	               dabSettings -> value ("echo_depth", 1). toInt();

	switchTime		=
	                  dabSettings -> value ("switchTime", 8000). toInt ();
	latency			=
	                  dabSettings -> value ("latency", 5). toInt();

	stereoSetting		= false;
	my_dabProcessor		= nullptr;
//
//	as with many of the buffers, picturesPath is not used
//	but needed as parameter in the dabProcessor
	picturesPath		= QDir::tempPath();

	serviceOrder		=
	                 dabSettings -> value ("serviceOrder", 0). toInt ();

//	The settings are done, now creation of the GUI parts
	setupUi (this);
//
	serviceList. clear ();
        model . clear ();
        ensembleDisplay         -> setModel (&model);

//	Where do we leave the audio out?
	streamoutSelector	-> hide();
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

	theBand. setupChannels  (channelSelector, BAND_III);

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
//	presetTimer
	presetTimer. setSingleShot (true);
	presetTimer. setInterval (switchTime);
	connect (&presetTimer, SIGNAL (timeout (void)),
	            this, SLOT (setPresetStation (void)));
//
//      timer for muting
        muteTimer. setSingleShot (true);
        muting          = false;
        muteDelay       = dabSettings -> value ("muteTime", 10). toInt ();
	connect (muteButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_muteButton ()));

//	if a device was selected, we just start, otherwise
//	we do not know what to do other than giving up
	inputDevice	= findDevice ();
	if (inputDevice == nullptr) {
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("Opening  input stream failed\n"));
	   exit (21);
	}

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
	qApp	-> installEventFilter (this);
	
	my_dabProcessor	= new dabProcessor (this, inputDevice, &globals);

	connect (presetSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (handle_presetSelector (const QString &)));
	connect (channelSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (selectChannel (const QString &)));
	connect (prev_serviceButton, SIGNAL (clicked ()),
                 this, SLOT (handle_prevServiceButton ()));
        connect (next_serviceButton, SIGNAL (clicked ()),
                 this, SLOT (handle_nextServiceButton ()));
	connect (ensembleDisplay, SIGNAL (clicked (QModelIndex)),
	         this, SLOT (selectService (QModelIndex)));
	connect (nextchannelButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_nextChannelButton (void)));
	connect	(prevchannelButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_prevChannelButton (void)));
	connect (dlTextButton, SIGNAL (clicked (void)),
                 this, SLOT (handle_dlTextButton (void)));
	connect (scheduleButton, SIGNAL (clicked ()),
	         this, SLOT (handle_scheduleButton ()));

//
	h	= dabSettings -> value ("channel", "12C"). toString();
        k	= channelSelector -> findText (h);
        if (k != -1)
           channelSelector -> setCurrentIndex (k);

	if (nextService. valid) {
	   presetTimer. setSingleShot	(true);
	   presetTimer. setInterval 	(switchTime);
	   presetTimer. start 		(switchTime);
	}

	frameDumper	= nullptr;
	startChannel (channelSelector -> currentText ());
	ficSuccess	= 0;
	ficBlocks	= 0;
	running. store (true);
}

QString RadioInterface::footText () {
        QString versionText = "dabMini-1.0: ";
        versionText += "Copyright J van Katwijk, J. vanKatwijk@gmail.com\n";
        versionText += "Rights of Qt, fftw, portaudio, libsamplerate  libfaad gratefully acknowledged";
        versionText += "Rights of other contribuants gratefully acknowledged\n";
        versionText += " Build on: " + QString(__TIMESTAMP__) + QString (" ") + QString (GITHASH);
        return versionText;
}

	RadioInterface::~RadioInterface () {
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

	s	-> setValue ("channel",
	                      channelSelector -> currentText());
	s	-> setValue ("soundchannel",
	                      streamoutSelector -> currentText());
	s	-> sync();
}

//
///////////////////////////////////////////////////////////////////////////////
//	
//	a slot called by the ofdmprocessor
void	RadioInterface::set_CorrectorDisplay (int v) {
	(void)v;
}
//
//	might be called when scanning only
void	RadioInterface::signalTimer_out() {
}

///////////////////////////////////////////////////////////////////////////
//
//	a slot, called by the fic/fib handlers
void	RadioInterface::addtoEnsemble (const QString &serviceName,
	                                             int32_t SId) {
	if (!running. load ())
	   return;

	serviceId ed;
	ed. name = serviceName;
	ed. SId	= SId;
	if (!my_dabProcessor -> is_audioService (serviceName))
	   return;
	if (isMember (serviceList, ed))
	   return;
	serviceList = insert (serviceList, ed, serviceOrder);

	model. clear ();
	for (const auto serv : serviceList)
	   model. appendRow (new QStandardItem (serv. name));
        int row = model. rowCount ();
        for (int i = 0; i < row; i ++) {
           model. setData (model. index (i, 0),
                      QFont ("Cantarell", 10), Qt::FontRole);
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
	ensembleId      -> setAlignment(Qt::AlignLeft);
        ensembleId      -> setText (v + QString (":") + hextoString (id));
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

void	RadioInterface::handle_motObject (QByteArray result, QString name,
	                                  int contentType, bool dirName) {
	(void)result;
	(void)name;
	(void)contentType;
	(void)dirName;
}
//
//	sendDatagram is triggered by the ip handler,
void	RadioInterface::sendDatagram	(int length) {
	(void)length;
}
//
//	tdcData is triggered by the backend.
void	RadioInterface::handle_tdcdata (int frametype, int length) {
	(void)frametype; (void)length;
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
	   if (currentService. valid) { 
	      s = currentService;
	      stopService     ();
	   }
	   stop_secondService ();
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
	      QFont ("Cantarell", 10), Qt::FontRole);
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
	dumpControlState (dabSettings);
	stopChannel ();
	soundOut		-> stop();
	running. store (false);
	displayTimer. stop();
	signalTimer.  stop();
	presetTimer.  stop();

	my_presetHandler. savePresets (presetSelector);

	my_dabProcessor		-> stop();		// definitely concurrent
	if (dlTextFile != nullptr)
	   fclose (dlTextFile);
	usleep (1000);		// give space to clean up pending signals
//	everything should be halted by now
	delete		soundOut;
	delete		inputDevice;
	delete		my_dabProcessor;
	usleep (1000);
//	close();
	fprintf (stderr, ".. end the radio silences\n");
}

//
void	RadioInterface::updateTimeDisplay () {
}
//
deviceHandler	*RadioInterface::findDevice () {
deviceHandler *inputDevice;
	gainSelector	-> hide ();
	lnaSelector	-> hide ();
	agcControl	-> hide ();
#ifdef	HAVE_SDRPLAY_V3
	try {
	   inputDevice	= new sdrplayHandler_v3 (dabSettings,
	                                         gainSelector,
	                                         lnaSelector,
	                                         agcControl);
	   gainSelector	-> show ();
	   lnaSelector	-> show ();
	   agcControl -> show ();
	   return inputDevice;
	} catch (int e) {}
#endif
#ifdef	HAVE_SDRPLAY
	try {
	   inputDevice	= new sdrplayHandler (dabSettings,
	                                      gainSelector,
	                                      lnaSelector,
	                                      agcControl);
	   gainSelector	-> show ();
	   lnaSelector	-> show ();
	   agcControl -> show ();
	   return inputDevice;
	} catch (int e) {}
#endif
#ifdef	HAVE_RTLSDR
	try {
	   inputDevice	= new rtlsdrHandler (dabSettings,
	                                     gainSelector,
	                                     agcControl);
	   gainSelector	-> show ();
	   agcControl	-> show ();
	   return inputDevice;
	} catch (int e) {}
#endif
#ifdef	HAVE_AIRSPY
	try {
	   inputDevice	= new airspyHandler (dabSettings,
	                                     gainSelector,
	                                     agcControl);
	   gainSelector	-> show ();
	   agcControl	-> show ();
	   return inputDevice;
	} catch (int e) {
	}
#endif
#ifdef	HAVE_LIME
	try {
	   inputDevice	= new limeHandler (dabSettings,
	                                   gainSelector,
	                                   lnaSelector,
	                                   agcControl
);
	   gainSelector	-> show ();
	   return inputDevice;
	} catch (int e) {}
#endif
#ifdef	HAVE_PLUTO
	try {
	   inputDevice = new plutoHandler (dabSettings,
	                                   gainSelector,
	                                   agcControl);
	   gainSelector	-> show ();
	   agcControl	-> show ();
	   return inputDevice;
	} catch (int e) {}
#endif
#ifdef	HAVE_HACKRF
	try {
	   inputDevice	= new hackrfHandler (dabSettings,
	                                     gainSelector,
	                                     lnaSelector);
	   gainSelector	-> show ();
	   lnaSelector	-> show ();
	   return inputDevice;
	} catch (int e) {}
#endif
	return nullptr;
}
//

///////////////////////////////////////////////////////////////////////////
//	signals, received from ofdm_decoder that data is
//	to be displayed
///////////////////////////////////////////////////////////////////////////

static
const char *monthTable [] = {
	"jan",
	"feb",
	"mar",
	"apr",
	"may",
	"jun",
	"jul",
	"aug",
	"sep",
	"oct",
	"nov",
	"dec"
};

void	RadioInterface::clockTime (int year, int month, int day,
	                                    int hours, int minutes,
	                                    int d2, int h2, int m2, int sec){
char dayString [3];
char hourString [3];
char minuteString [3];
//	currentHour	= hours;
//	currentMinute	= minutes;
	sprintf (dayString, "%.2d", day);
	sprintf (hourString, "%.2d", hours);
	sprintf (minuteString, "%.2d", minutes);
	QString result = QString::number (year) + "-" +
	                       monthTable [month - 1] + "-" +
	                       QString (dayString) + "  " +
	                       QString (hourString) + ":" +
	                       QString (minuteString);
	timeDisplay -> setText (result);
}

//void	RadioInterface::showTime	(const QString &s) {
//	timeDisplay	-> setText (s);
//}

void	RadioInterface::show_frameErrors (int s) {
	(void)s;
}

void	RadioInterface::show_rsErrors (int s) {
	(void)s;
}

void	RadioInterface::show_aacErrors (int s) {
	(void)s;
}

void	RadioInterface::show_ficSuccess (bool b) {
	if (b) 
	   ficSuccess ++;
	if (++ficBlocks >= 25){
	   if (ficSuccess > 24)
	      ficLabel ->
	          setStyleSheet ("QLabel {background-color : green; color: white}");
	   else
	      ficLabel ->
	          setStyleSheet ("QLabel {background-color : red; color: white}");
	   ficBlocks	= 0;
	   ficSuccess	= 0;
	}
}

void	RadioInterface::show_motHandling (bool b) {
	(void)b;
}

//	called from the ofdmDecoder, it is computed for each frame
void	RadioInterface::show_snr (int s) {
	snrBuffer. FlushRingBuffer ();
	(void)s;
}

void	RadioInterface::setSynced	(bool b) {
	(void)b;
}

void	RadioInterface::showLabel	(QString s) {
	if (!running. load ()) 
	   return;
	dynamicLabel	-> setText (s);
	dynamicLabel -> setWordWrap (true);

	if (dlTextFile == nullptr)
	   return;
	if (the_dlCache. addifNew (s))
	   return;
        QString currentChannel = channelSelector -> currentText ();
        QDateTime theDateTime   = QDateTime::currentDateTime ();
        fprintf (dlTextFile, "%s.%s %s: %s\n",
                                  currentChannel. toUtf8 (). data (),
                                  currentService. serviceName.
                                                  toUtf8 (). data (),
	                          theDateTime.
	                                toString ("yy-mm-dd hh:mm:ss").
	                                      toLatin1 (). data (),
	                          s. toLatin1 (). data ());
	
}

void	RadioInterface::setStereo	(bool s) {
	if (!running. load ())
           return;
        if (stereoSetting == s)
           return;

        stereoLabel   -> setStyleSheet (s ?
                         "QLabel {background-color: green; color : white}":
                         "QLabel {background-color: red; color : white}");
        stereoLabel   -> setText (s ? "stereo" : "mono");
        stereoSetting = s;

}

void	RadioInterface::show_tii_spectrum () {
}

void	RadioInterface::show_tii	(int s1, int s2) {
	(void)s1; (void)s2;
}

void	RadioInterface::show_snr	(int s1, float f1, float f2) {
	(void)s1; (void)f1; (void)f2;
}

void	RadioInterface::showSpectrum	(int32_t amount) {
	(void)amount;
}

void	RadioInterface::showIQ		(int amount) {
	(void)amount;
}

void	RadioInterface::showQuality	(float q) {
	(void)q;
}

void	RadioInterface::show_rsCorrections	(int c) {
	(void)c;
}

void	RadioInterface::show_clockError	(int e) {
	(void)e;
}

void	RadioInterface::showCorrelation	(int amount, int marker) {
	(void)amount; (void)marker;
}

void	RadioInterface::showIndex 	(int ind) {
	(void)ind;
}
//
////////////////////////////////////////////////////////////////////////////

void	RadioInterface:: set_streamSelector (int k) {
	((audioSink *)(soundOut)) -> selectDevice (k);
}

void	RadioInterface::setSyncLost() {
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
	   event -> ignore ();
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
//	         fprintf (stderr, "currentservice = %s (%d)\n",
//	                       currentService. serviceName. toLatin1 (). data (),
//	                                    currentService. valid);
	         stopService ();	// basically redundant
	         selectService (ensembleDisplay -> currentIndex ());
	      }
           }
        }
        else
	if ((obj == this -> ensembleDisplay -> viewport()) &&
	    (event -> type () == QEvent::MouseButtonPress )) {
	   QMouseEvent *ev = static_cast<QMouseEvent *>(event);
	   if (ev -> buttons() & Qt::RightButton) {
	      audiodata ad;
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
	      else {
	         if (secondService. valid) {
	            stop_secondService ();
	            secondService. valid = false;
	         }
	         if (secondService. serviceName != serviceName) {
	            audiodata ad;
	            my_dabProcessor	-> dataforAudioService (serviceName,
	                                                          &ad);
	            if (ad. defined && (ad. ASCTy == 077))
	               start_secondService (serviceName);
	         }
	      }
	   }
	}
	return QWidget::eventFilter (obj, event);
}

void	RadioInterface::startAnnouncement (const QString &name, int subChId) {
	(void)name; (void)subChId;
}

void	RadioInterface::stopAnnouncement (const QString &name, int subChId) {
	(void)name; (void)subChId;
}

////////////////////////////////////////////////////////////////////////
//	preset selection,
////////////////////////////////////////////////////////////////////////
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
	localSelect (list. at (0), list. at (1));
}
//
//      From a predefined schedule list, the service names most
//      likely are less than 16 characters
//
void    RadioInterface::scheduleSelect (const QString &s) {
        QStringList list = s.split (":", QString::SkipEmptyParts);
        if (list. length () != 2)
           return;
        QString channel = list. at (0);
        QString service = list. at (1);
        for (int i = service. size (); i < 16; i ++)
           service. append (' ');
	fprintf (stderr, "scheduling %s %s\n", channel. toLatin1 (). data (),
	                                       service. toLatin1 (). data ());
        localSelect (channel, service);
}

void	RadioInterface::localSelect (const QString &channel,
	                             const QString &service) {
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

	stopChannel ();		// this will handle stopping the service
	nextService. valid = true;
	nextService. serviceName	= service;
	nextService. SId		= 0;
	nextService. SCIds		= 0;
	presetTimer. setSingleShot (true);
	presetTimer. setInterval (switchTime);
	presetTimer. start (switchTime);
	startChannel	(channelSelector -> currentText ());
}

////////////////////////////////////////////////////////////////////////////
//
//	handling services: stop, select and start
///////////////////////////////////////////////////////////////////////////

void	RadioInterface::stopService	() {
	presetTimer. stop ();
	presetSelector -> setCurrentIndex (0);
	signalTimer. stop ();
	if (currentService. valid) {
	   fprintf (stderr, "stopping service\n");
	   audiodata ad;
	   my_dabProcessor -> dataforAudioService (currentService. serviceName,
	                                                  &ad);
	   my_dabProcessor -> stopService (&ad);
	   usleep (1000);
	   soundOut	-> stop ();
	   QString serviceName = currentService. serviceName;
	   for (int i = 0; i < model. rowCount (); i ++) {
	      QString itemText =
	          model. index (i, 0). data (Qt::DisplayRole). toString ();
	      if (itemText == serviceName) {
	         colorService (model. index (i, 0), Qt::black, 10);
	         break;
	      }
	   }
	}
	currentService. valid	= false;
	cleanScreen	();
}
//
void	RadioInterface::selectService (QModelIndex ind) {
QString	currentProgram = ind. data (Qt::DisplayRole). toString();
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
	   fprintf (stderr, "Niet verwacht, service %s is still valid\n",
	                    currentService. serviceName. toUtf8 (). data ());
	   stopService ();
	}

	ficBlocks		= 0;
	ficSuccess		= 0;
	currentService		= *s;
	currentService. valid	= false;
	int rowCount		= model. rowCount ();
	for (int i = 0; i < rowCount; i ++) {
	   QString itemText =
	           model. index (i, 0). data (Qt::DisplayRole). toString ();
	   if (itemText == serviceName) {
	      colorService (model. index (i, 0), Qt::red, 13);
	      serviceLabel	-> setStyleSheet ("QLabel {color : black}");
	      serviceLabel	-> setText (serviceName);
	      if (my_dabProcessor -> is_audioService (serviceName)) {
	         start_audioService (serviceName);
	         currentService. valid = true;
	         currentService. serviceName = serviceName;
	      }
	      else
	         fprintf (stderr, "%s not supported\n",
	                            serviceName. toUtf8 (). data ());
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
	serviceLabel		-> setText ("");
	dynamicLabel		-> setText ("");
	presetSelector		-> setCurrentIndex (0);
	stereoLabel	-> setStyleSheet (
                         "QLabel {background-color: white; color : black}");
        stereoLabel	-> setText ("");
	stereoSetting		= false;
}

void	RadioInterface::start_audioService (const QString &serviceName) {
audiodata ad;

	my_dabProcessor -> dataforAudioService (serviceName, &ad);
	if (!ad. defined) {
	   QMessageBox::warning (this, tr ("Warning"),
 	                         tr ("insufficient data for this program\n"));
	   return;
	}
	serviceLabel -> setText (serviceName);
	serviceLabel -> setAlignment(Qt::AlignLeft);

	ad. procMode	= __ONLY_SOUND;
	my_dabProcessor -> set_audioChannel (&ad, &audioBuffer);
//	activate sound
	soundOut -> restart ();
}

////////////////////////////////////////////////////////////////////////////
//
//	next and previous service selection
////////////////////////////////////////////////////////////////////////////

//
//	Previous and next services. trivial implementation
void	RadioInterface::handle_prevServiceButton	() {
	presetTimer. stop ();
	presetSelector -> setCurrentIndex (0);
	nextService. valid	= false;

	if (!currentService. valid)
	   return;

	QString oldService	= currentService. serviceName;
	disconnect (prev_serviceButton, SIGNAL (clicked ()),
	            this, SLOT (handle_prevServiceButton ()));
	stopService  ();
	if ((serviceList. size () != 0) &&
	                    (oldService != "")) {
	   for (int i = 0; i < (int)(serviceList. size ()); i ++) {
	      if (serviceList. at (i). name == oldService) {
	         colorService (model. index (i, 0), Qt::black, 10);
	         i = i - 1;
	         if (i < 0)
	            i = serviceList. size () - 1;
	         dabService s;
	         s. serviceName = serviceList. at (i). name;
	         my_dabProcessor ->
	                  getParameters (s. serviceName, &s. SId, &s. SCIds);
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
	connect (prev_serviceButton, SIGNAL (clicked ()),
	         this, SLOT (handle_prevServiceButton ()));
}

void	RadioInterface::handle_nextServiceButton	() {
	presetTimer. stop ();
	presetSelector -> setCurrentIndex (0);
	nextService. valid	= false;

	if (!currentService. valid)
	   return;

	QString oldService = currentService. serviceName;
	disconnect (next_serviceButton, SIGNAL (clicked ()),
	            this, SLOT (handle_nextServiceButton ()));
	stopService ();
	if ((serviceList. size () != 0) && (oldService != "")) {
	   for (int i = 0; i < (int)(serviceList. size ()); i ++) {
	      if (serviceList. at (i). name == oldService) {
	         colorService (model. index (i, 0), Qt::black, 10);
	         i = i + 1;
	         if (i >= (int)(serviceList. size ()))
	            i = 0;
	         dabService s;
	         s. serviceName = serviceList. at (i). name;
	         my_dabProcessor ->
	                 getParameters (s. serviceName, &s. SId, &s. SCIds);
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
	connect (next_serviceButton, SIGNAL (clicked ()),
	         this, SLOT (handle_nextServiceButton ()));
}

////////////////////////////////////////////////////////////////////////////
//
//	The user(s)
///////////////////////////////////////////////////////////////////////////
//
//	Called from a timer
void	RadioInterface::setPresetStation () {
	if (ensembleId -> text () == QString ("")) {
	   QMessageBox::warning (this, tr ("Warning"),
	                          tr ("Oops, ensemble not yet recognized\nselect service manually\n"));
	   return;
	}

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
	fprintf (stderr, "presetName %s not found\n",
	                      presetName. toUtf8 (). data ());
}

///////////////////////////////////////////////////////////////////////////
//
//	Channel basics
//	Precondition: no channel should be active
//	
void	RadioInterface::startChannel (const QString &channel) {
int	tunedFrequency	=
	         theBand. Frequency (channel);
	inputDevice		-> restartReader (tunedFrequency);
	my_dabProcessor		-> start (tunedFrequency);
	show_for_safety	();
}
//
void	RadioInterface::stopChannel	() {
	if ((inputDevice == nullptr) || (my_dabProcessor == nullptr))
	   return;
	soundOut		-> stop ();
	ficSuccess		= 0;
	ficBlocks		= 0;
	presetTimer. stop ();
	presetSelector		-> setCurrentIndex (0);
	muteTimer. stop ();
	muting			= false;
	signalTimer. stop ();
	hide_for_safety		();
	stop_secondService ();	// just in case ...
//
//	The service(s) - if any - is stopped by halting the dabProcessor
	my_dabProcessor		-> stop ();
	inputDevice		-> stopReader ();
	usleep (1000);
	currentService. valid	= false;
	nextService. valid	= false;
//	the visual elements
	setSynced	(false);
	serviceList. clear ();
	model. clear ();
	ensembleDisplay		-> setModel (&model);
	cleanScreen	();
}

//
/////////////////////////////////////////////////////////////////////////
//
//	select, next- and previous channel buttons
/////////////////////////////////////////////////////////////////////////

void    RadioInterface::selectChannel (const QString &channel) {
	stopChannel ();
	startChannel (channel);
}


void	RadioInterface::handle_nextChannelButton () {
int     currentChannel  = channelSelector -> currentIndex ();
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
//	scanning: not supported in dabMini
/////////////////////////////////////////////////////////////////////////

void	RadioInterface::No_Signal_Found () {
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
	int 	baseN		= 0;
	QString baseS		= "";
	bool	inserted	= false;
	for (const auto serv : l) {
	   if (!inserted &&
	         (order == ID_BASED ?
	             ((baseN < (int)n. SId) && (n. SId <= serv. SId)):
	             ((baseS < n. name) && (n. name < serv. name)))) {
	      k. push_back (n);
	      inserted = true;
	   }
	   baseS	= serv. name;
	   baseN	= serv. SId;
	   k. push_back (serv);
	}
	if (!inserted)
	   k. push_back (n);
	return k;
}

void	RadioInterface::hide_for_safety () {
	prev_serviceButton	->	hide ();
        next_serviceButton	->	hide ();
}

void	RadioInterface::show_for_safety () {
	prev_serviceButton	->	show ();
        next_serviceButton	->	show ();
}

static inline
bool    isValid (QChar c) {
        return c. isLetter () || c. isDigit () || (c == '-');
}

void	RadioInterface::start_secondService	(const QString &s) {
audiodata ad;

	my_dabProcessor	-> dataforAudioService (s, &ad);
	if (!ad. defined)
	   return;
	QString saveDir		= QDir::homePath ();
	if ((saveDir != "") && (!saveDir. endsWith ('/')))
           saveDir = saveDir + '/';

        QString theTime = timeDisplay -> text ();
        QString suggestedFileName =
	                    secondService. serviceName + 
	                    channelSelector -> currentText () +
                            "-" + theTime;
        for (int i = 0; i < suggestedFileName. length (); i ++)
           if (!isValid (suggestedFileName. at (i)))
              suggestedFileName. replace (i, 1, '-');
	suggestedFileName = saveDir + suggestedFileName;
        fprintf (stderr, "suggested filename %s\n",
                                 suggestedFileName. toUtf8 (). data ());
        QString fileName = QFileDialog::getSaveFileName (this,
                                                tr ("Save file ..."),
                                                suggestedFileName + ".aac",
                                                tr ("aac (*.aac)"));
        if (fileName == "")
           return;

	fileName        = QDir::toNativeSeparators (fileName);
	frameDumper	= fopen (fileName. toUtf8(). data(), "w");

        if (frameDumper == nullptr) {
           fprintf (stderr, "Could not open file %s\n",
                                      fileName. toUtf8(). data());
           return;
        }

	secondService. serviceName = s;
	secondService. valid	= true;
	ad. procMode	= __ONLY_DATA;
	frameBuffer. FlushRingBuffer ();
	my_dabProcessor -> set_audioChannel (&ad, &audioBuffer);
}

void	RadioInterface::stop_secondService () {
	if (frameDumper == nullptr)
	   return;
	if (!secondService. valid)
	   return;
	fprintf (stderr, "stopping second service %s\n",
	                      secondService. serviceName. toUtf8 (). data ());
	audiodata ad;
	my_dabProcessor	-> dataforAudioService (secondService. serviceName,
	                                                      &ad);
	my_dabProcessor -> stopService (&ad);
	fclose (frameDumper);
	frameDumper = nullptr;
}

void	RadioInterface::newFrame	(int amount) {
uint8_t buffer [amount];

	if (frameDumper == nullptr) {
	   frameBuffer. FlushRingBuffer ();
	   return;
	}

	while (frameBuffer. GetRingBufferReadAvailable () > amount) {
	   frameBuffer. getDataFromBuffer (buffer, amount);
	   if (frameDumper != nullptr)
	      fwrite (buffer, amount, 1, frameDumper);
	}
}

void    setButtonFont (QPushButton *b, QString text, int size) {
        QFont font      = b -> font ();
        font. setPointSize (size);
        b               -> setFont (font);
        b               -> setText (text);
        b               -> update ();
}

void    RadioInterface::handle_muteButton       () {
        if (muting) {
           muteTimer. stop ();
           disconnect (&muteTimer, SIGNAL (timeout ()),
                       this, SLOT (handle_muteButton ()));
	   setButtonFont (muteButton, "mute", 10);
           muting = false;
           return;
        }

        connect (&muteTimer, SIGNAL (timeout (void)),
                 this, SLOT (handle_muteButton (void)));
        muteTimer. start (muteDelay * 1000);
        setButtonFont (muteButton, "muting", 11);
        muting = true;
}

///////////////////////////////////////////////////////////////////////////
//	Handling schedule

void	RadioInterface::handle_scheduleButton	() {
QStringList candidates;
scheduleSelector theSelector;
QString		scheduleService;

	theSelector. addtoList ("nothing");
	theSelector. addtoList ("exit");
	theSelector. addtoList ("dlText");
	candidates	+= "nothing";
	candidates	+= "exit";
	candidates	+= "dlText";
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
	fprintf (stderr, "selected %d %s\n", selected, 
	                            scheduleService. toLatin1 (). data ());
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

	if (s == "nothing")
	   return;

	if (s == "exit") {
	   QWidget::close ();
	   return;
	}

	if (s == "dlText") {
	   scheduled_dlTextDumping ();
	   return;
	}

	presetTimer. stop ();
	scheduleSelect (s);
}

void	RadioInterface::handle_dlTextButton	(){
	if (dlTextFile != nullptr) {
           fclose (dlTextFile);
           dlTextFile = nullptr;
           dlTextButton -> setText ("dlText");
           return;
        }

        QString fileName =filenameFinder. finddlText_fileName (true);
        dlTextFile      = fopen (fileName. toUtf8 (). data (), "w+");
        if (dlTextFile  == nullptr)
           return;
        dlTextButton            -> setText ("writing");

}

void	RadioInterface::scheduled_dlTextDumping () {
	if (dlTextFile != nullptr) {
	   fclose (dlTextFile);
	   dlTextFile = nullptr;
	   dlTextButton	-> setText ("dlText");
	   return;
	}

	QString	fileName = filenameFinder. finddlText_fileName (false);
	dlTextFile	= fopen (fileName. toUtf8 (). data (), "w+");
	if (dlTextFile == nullptr)
	   return;
	dlTextButton		-> setText ("writing");
}

void	RadioInterface::nrServices	(int nr) {
	(void)nr;	// todo
}

void	RadioInterface::showCorrelation (int x1, int x2, QVector<int> x3) {
	(void)x1; (void)x2; (void)x3;
}


