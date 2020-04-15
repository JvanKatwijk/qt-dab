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
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
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
#include	"dab_tables.h"
#include	"audiosink.h"
#ifdef	HAVE_RTLSDR
#include	"rtlsdr-handler.h"
#endif
#ifdef	HAVE_SDRPLAY
#include	"sdrplay-handler.h"
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

	RadioInterface::RadioInterface (QSettings	*Si,
	                                const QString	&presetFile,
	                                QWidget		*parent):
	                                        QWidget (parent),
	                                        my_presetHandler (this),
	                                        theBand ("") {
int16_t	latency;
int16_t k;
QString h;
QString	presetName;

	dabSettings		= Si;
	running. 		store (false);
	scanning. 		store (false);
	isSynced		= false;
	runningServices. 	resize (0);
//
//	These buffers are not used here, but needed as parameter
//	in dabProcessor
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
	dataBuffer		=
	                  new RingBuffer<uint8_t>(32768);
	switchTime		=
	                  dabSettings -> value ("switchTime", 8000). toInt ();
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
//
//	as with many of the buffers, picturesPath is not used
//	but used in the dabProcessor
	picturesPath     = QDir::tempPath();

	serviceOrder	=
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

//	if a device was selected, we just start, otherwise
//	we wait until one is selected
	inputDevice	= findDevice ();
	if (inputDevice == nullptr) {
	   QMessageBox::warning (this, tr ("Warning"),
	                               tr ("Opening  input stream failed\n"));
	   exit (21);
	}
	qApp	-> installEventFilter (this);
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
//
//	It is time for some action
	my_dabProcessor = new dabProcessor   (this,
	                                      inputDevice,
	                                      1,
	                                      threshold,
	                                      diff_length,
	                                      tii_delay,
	                                      tii_depth,
	                                      echo_depth,
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

	h	= dabSettings -> value ("channel", "12C"). toString();
        k	= channelSelector -> findText (h);
        if (k != -1)
           channelSelector -> setCurrentIndex (k);

	startChannel (channelSelector -> currentText ());
	running. store (true);
}

QString RadioInterface::footText () {
	version			= QString (CURRENT_VERSION);
        QString versionText = "simpeRadio- : " + QString(CURRENT_VERSION);
        versionText += "Copyright J van Katwijk, J. vanKatwijk@gmail.com\n";
        versionText += "Rights of Qt, portaudio, libsamplerate gratefully acknowledged";
        versionText += "Rights of other contribuants gratefully acknowledged\n";
        versionText += " Build on: " + QString(__TIMESTAMP__) + QString (" ") + QString (GITHASH);
        return versionText;
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
	if (!running. load())
	   return;

	(void)SId;
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
	ensembleId      -> setAlignment(Qt::AlignCenter);
        ensembleId      -> setText (v + QString (":") + hextoString (id));
	my_dabProcessor	-> coarseCorrectorOff();
}

//////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////

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
	   if (runningServices. size () > 0) 
	      s = runningServices. at (runningServices. size () - 1);
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
	displayTimer. stop();
	signalTimer.  stop();
	presetTimer.  stop();

	my_presetHandler. savePresets (presetSelector);

	inputDevice		-> stopReader();	// might be concurrent
	my_dabProcessor		-> stop();		// definitely concurrent

	soundOut		-> stop();
//	everything should be halted by now
	dumpControlState (dabSettings);
	delete		soundOut;
	delete		inputDevice;
	fprintf (stderr, "going to delete dabProcessor\n");

	delete		my_dabProcessor;
	fprintf (stderr, "deleted dabProcessor\n");
	delete	iqBuffer;
	delete	spectrumBuffer;
	delete	responseBuffer;
	delete	tiiBuffer;
	delete	frameBuffer;
//	close();
	fprintf (stderr, ".. end the radio silences\n");
}

//
void	RadioInterface::updateTimeDisplay() {
	if (!running. load())
	   return;
//	numberofSeconds ++;
//	int16_t	numberHours	= numberofSeconds / 3600;
//	int16_t	numberMinutes	= (numberofSeconds / 60) % 60;
//	QString text = QString ("runtime ");
//	text. append (QString::number (numberHours));
//	text. append (" hr, ");
//	text. append (QString::number (numberMinutes));
//	text. append (" min");
//	runtimeDisplay	-> setText (text);
}
//
deviceHandler	*RadioInterface::findDevice () {
deviceHandler *inputDevice;
	gainSelector	-> hide ();
	lnaSelector	-> hide ();
	agcControl	-> hide ();
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


void	RadioInterface::showTime	(const QString &s) {
	timeDisplay	-> setText (s);
}

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
	(void)b;
}

void	RadioInterface::show_motHandling (bool b) {
	(void)b;
}

//	called from the ofdmDecoder, it is computed for each frame
void	RadioInterface::show_snr (int s) {
	(void)s;
}

void	RadioInterface::setSynced	(bool b) {
	(void)b;
}

void	RadioInterface::showLabel	(QString s) {
	if (running. load ()) {
	   dynamicLabel	-> setText (s);
	   dynamicLabel -> setWordWrap (true);
	}
}

void	RadioInterface::setStereo	(bool s) {
	(void)s;
}

void	RadioInterface::show_tii (QByteArray data) {
	(void)data;
}

void	RadioInterface::showSpectrum	(int32_t amount) {
	(void)amount;
}

void	RadioInterface::showIQ	(int amount) {
	(void)amount;
}

void	RadioInterface::showQuality	(float q) {
	(void)q;
}

void	RadioInterface::showCorrelation	(int amount, int marker) {
	(void)amount; (void)marker;
}

void	RadioInterface::showIndex (int ind) {
	(void)ind;
}

void	RadioInterface::newFrame	(int amount) {
	(void)amount;
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
//
//	to avoid multiple events from a keystroke we
//	test whether or not the current service is already selected
	      if ((runningServices. size () > 0)  &&
	             (runningServices. at (runningServices. size () - 1). serviceName ==
	                          serviceLabel -> text ())) {
	         selectService (ensembleDisplay -> currentIndex ());
              }
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
//
//	preset selection, either from presets or from history
////////////////////////////////////////////////////////////////////////

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
//	my_dabProcessor -> getParameters (service, &serv. SId, &serv. SCIds);
//	if (serv. SId == 0) {
//	   QMessageBox::warning (this, tr ("Warning"),
//	                         tr ("insufficient data for this program\n"));
//           return;
//	}

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
	disconnect (prev_serviceButton, SIGNAL (clicked ()),
	            this, SLOT (handle_prevServiceButton ()));
	QString currentService = serviceLabel -> text ();
	presetTimer. stop ();
	stopService  ();
	if ((serviceList. size () != 0) && (currentService != "")) {
	   for (int i = 0; i < (int)(serviceList. size ()); i ++) {
	      if (serviceList. at (i). name == currentService) {
	         colorService (model. index (i, 0), Qt::black, 11);
	         i = i - 1;
	         if (i < 0)
	            i = serviceList. size () - 1;
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
	         runningServices. push_back (s);
	         startService (&s);
	         break;
	      }
	   }
	}
	connect (prev_serviceButton, SIGNAL (clicked ()),
	         this, SLOT (handle_prevServiceButton ()));
}

void	RadioInterface::handle_nextServiceButton	() {
	disconnect (next_serviceButton, SIGNAL (clicked ()),
	            this, SLOT (handle_nextServiceButton ()));
	QString currentService = serviceLabel -> text ();
	presetTimer. stop ();
	stopService ();
	if ((serviceList. size () != 0) && (currentService != "")) {
	   for (int i = 0; i < (int)(serviceList. size ()); i ++) {
	      if (serviceList. at (i). name == currentService) {
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
	connect (next_serviceButton, SIGNAL (clicked ()),
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

	if (runningServices. size () == 0)
	   return;

	QString presetName	= runningServices. back (). serviceName;
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
	runningServices. pop_back ();
	fprintf (stderr, "presetName %s not found\n", presetName. toLatin1 (). data ());
}

///////////////////////////////////////////////////////////////////////////
//
//	Channel basics
//	Precondition: no channel should be activated
//	
void	RadioInterface::startChannel (const QString &channel) {
int	tunedFrequency	=
	         theBand. Frequency (channel);
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
	my_dabProcessor		-> stop ();
//	the visual elements
	setSynced	(false);
	serviceList. clear ();
	model. clear ();
	ensembleDisplay		-> setModel (&model);
}

void    RadioInterface::selectChannel (const QString &channel) {
	presetTimer. stop ();
	stopService ();
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
	             ((baseN < n. SId) && (n. SId <= serv. SId)):
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

