#
/*
 *    Copyright (C) 2016 .. 2025
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

#pragma once

#include	"dab-constants.h"
#include	<QMainWindow>
#include	<QStringList>
#include	<QScrollArea>
#include	<QStandardItemModel>
#include	<QDomDocument>
#ifdef	_SEND_DATAGRAM_
#include	<QUdpSocket>
#endif
#include	<QVector>
#include	<QComboBox>
#include	<QByteArray>
#include	<QLabel>
#include	<QTimer>
#include	"ui_dabradio-6.h"
#include	"ringbuffer.h"
#include	"process-params.h"
#include	"converter_48000.h"
#include	"scanlist-handler.h"
#include	"dl-cache.h"
#include	"content-table.h"
#include	"dxDisplay.h"
#include	<memory>
#include	<mutex>
#ifdef	DATA_STREAMER
#include	"tcp-server.h"
#endif
#include	"scan-handler.h"
#include	"epg-compiler.h"
#include	"xml-extractor.h"

#include	"device-chooser.h"
#include	"device-handler.h"
#include	"display-widget.h"
#include	"snr-viewer.h"

#include	"time-table.h"
#include	"findfilenames.h"

#include	"scheduler.h"

#include	"http-handler.h"
#include	"tii-mapper.h"

#include	"ensemble-handler.h"
#include	"config-handler.h"
#include	"logger.h"
#include	"errorlog.h"

#include	"ofdm-handler.h"

#include	<QScopedPointer>
#include	"journaline-datahandler.h"

class	copyrightText;

typedef struct {
	uint32_t serviceId;
	std::vector<multimediaElement> elements;
} mmDescriptor;

class	QSettings;
class	ofdmHandler;
class	deviceHandler;
class	audioPlayer;
class	common_fft;
class	timeTableControl;
class	audioDisplay;
#ifdef	HAVE_PLUTO_RXTX
class	dabStreamer;
#endif

class techWindow;
//#include	"ui_config-helper.h"
class	configHandler;

#define DECODER_1    0100
#define DECODER_2    0200
#define DECODER_3    0300
#define DECODER_4    0400
#define DECODER_5    0500
#define DECODER_6    0600


#define SCAN_BUTTON		QString ("scanButton")
#define	SPECTRUM_BUTTON		QString ("spectrumButton")
#define	SCANLIST_BUTTON		QString ("scanListButton")
#define PRESET_BUTTON           QString ("presetButton")
#define PREVSERVICE_BUTTON	QString ("prevServiceButton")
#define NEXTSERVICE_BUTTON	QString ("nextServiceButton")
#define	CONFIG_BUTTON		QString ("configButton")
#define	HTTP_BUTTON		QString ("httpButton")
/*
 *	The main gui object. It inherits from
 *	QWidget and the generated form
 */

class dabService {
public:
	QString		channel;
	QString		serviceName;
	uint32_t	SId;
	int		subChId;
	bool		isValid;
	bool		isAudio;
	uint8_t		ASCTy;		// only for audio
	FILE		*fd;
	FILE		*frameDumper;
	bool		runsBackground;
	std::vector<int>	fmFrequencies;
	dabService () {
	   channel	= "";
	   serviceName	= "";
	   SId		= 0;
	   subChId	= 77;
	   isValid	= false;
	   isAudio	= false;
	   fd		= nullptr;
	   ASCTy	= 0;
	   frameDumper	= nullptr;
	   runsBackground	= false;
	}
	~dabService	() {}
};

struct	theTime {
	int	year;
	int	month;
	int	day;
	int	hour;
	int	minute;
	int	second;
};

//	The channel descriptor is supposed to
//	contain all data related to the currently selected channel

class	channelDescriptor {
public:
	channelDescriptor () {
	cleanChannel ();
}
	~channelDescriptor () {
}
//
	QDate		theDate;
//	static channel info
	QString		channelName;
	uint32_t	Eid;
	QString		ensembleName;
	QString		countryName;
	int		tunedFrequency;
	int		nrServices;	// measured
	int		serviceCount;	// from FIC or nothing
	bool		realChannel;	// false if file input
	bool		hasEcc;
	uint8_t		eccByte;
	int		lto;
//	info on the transmitters seen
	int		nrTransmitters;
	std::vector<transmitter>	transmitters;
	QString		strongestTransmitter;
	position	targetPos;
	int8_t		mainId;
	int8_t		subId;
	float		height;
	float		distance;
	float		azimuth;
	int		snr;
//	info on services
	dabService	currentService;
	bool		audioActive;
	bool		etiActive;	//
	bool		announcing;
	std::vector<dabService> runningTasks;
	std::vector<mmDescriptor>servicePictures;
	void	cleanChannel () {
	   transmitters. resize (0);
	   servicePictures. resize (0);
	   runningTasks. resize (0);
	   realChannel		= true;
	   serviceCount		= -1;
	   nrServices		= 0;
	   tunedFrequency	= -1;
	   ensembleName		=  "";
	   nrTransmitters	= 0;
	   countryName		= "";
	   hasEcc		= false;
	   Eid			= 0;
	   targetPos		= position {0, 0};
	   mainId		= -1;
	   subId		= -1;
	   strongestTransmitter	= "";
	   snr			= 0;
	   announcing		= false;
	   height		= -1;
	   distance		= -1;
	   audioActive	 	= false;
	}
};

class RadioInterface: public QWidget, private Ui_dabradio {
Q_OBJECT
public:
		RadioInterface		(QSettings	*,
	                                 const QString	&,	//scanlist
	                                 const QString	&,	//presets
	                                 const QString	&,	//freqExt
	                                 const QString	&,	//schedule
	                                 const QString	&,	//tiiFile
	                                 bool,			// errorreport
	                                 int32_t	dataPort,
	                                 int32_t	clockPort,
	                                 int,
	                                 QWidget	*parent = nullptr);
		~RadioInterface		();

	bool	channelOn		();
	bool	devScopeOn		();
protected:
	bool			eventFilter (QObject *obj, QEvent *event);
	void			focusInEvent (QFocusEvent *);
private:
	FILE			*dlTextFile;
	RingBuffer<Complex>	theSpectrumBuffer;
	RingBuffer<Complex>	theIQBuffer;
	RingBuffer<Complex>	theTIIBuffer;
	RingBuffer<Complex>	theNULLBuffer;
	RingBuffer<Complex>	theChannelBuffer;
	RingBuffer<float>	theSNRBuffer;
	RingBuffer<float>	theResponseBuffer;
	RingBuffer<uint8_t>	theFrameBuffer;
	RingBuffer<uint8_t>	theDataBuffer;
	RingBuffer<std::complex<int16_t>>	theAudioBuffer;
	RingBuffer<float>	stdDevBuffer;
	uint8_t			cpuSupport;
	displayWidget		theNewDisplay;
	snrViewer		theSNRViewer;
	dlCache			theDLCache;
	findfileNames		theFilenameFinder;
	Scheduler		theScheduler;
	RingBuffer<std::complex<int16_t>>	theTechData;
	converter_48000		theAudioConverter;

	scanListHandler		theScanlistHandler;
	errorLogger		theErrorLogger;
	deviceChooser		theDeviceChooser;
	dxDisplay		theDXDisplay;
	logger			theLogger;
	scanHandler		theSCANHandler;
	tiiMapper		theTIIProcessor;
	xmlExtractor		theXmlExtractor;
	epgCompiler		theEpgCompiler;
//	end of variables that are initalized

	QScopedPointer<ensembleHandler> theEnsembleHandler;
	configHandler			*theConfigHandler;
	techWindow			*theTechWindow;
	ofdmHandler			*theOfdmHandler;
	deviceHandler			*theDeviceHandler;
	journaline_dataHandler		*journalineHandler;
	httpHandler			*mapViewer;
	contentTable			*theContentTable;
	contentTable			*theScanTable;
	copyrightText			*thecopyrightLabel;
	QSettings			*theQSettings;
#ifdef	HAVE_PLUTO_RXTX
	dabStreamer			*theDabStreamer;
#endif
	audioPlayer			*theAudioPlayer;
#ifdef	DATA_STREAMER
	tcpServer			*theDataStreamer;
#endif
#ifdef	CLOCK_STREAMER
	tcpServer			*theClockStreamer;
#endif
//
	void			connectGUI		();
	void			disconnectGUI		();
	void			cleanScreen		();
	void			hideButtons		();
	void			showButtons		();
	void			setChannelButton	(int);
	void			set_Colors		();
	void			setButtonColors		(QPushButton *,
	                                                 const QString &);
	void			displaySlide		(const QPixmap &,	
	                                                 const QString &t = "");
	deviceHandler		*createDevice		(const QString &,
	                                                             logger *);

	QString			convertTime		(int, int, int,
	                                                      int, int);
	QString			convertTime		(struct theTime &);

//
	bool			autoStart_http		();
	void			start_etiHandler	();
	void			stop_etiHandler		();
	void			startAudioservice	(audiodata &);
	void			startPacketservice	(packetdata &);
	void			startAudioDumping	();
	void			stopAudioDumping	();
	void			scheduledAudioDumping	();
	void			scheduledDLTextDumping	();
	void			scheduledFICDumping	();
	void			startSourceDumping	();
	void			stopSourceDumping	();
	void			startFrameDumping	();
	void			stopFrameDumping	();
	void			scheduled_frameDumping	(const QString &);
	void			startChannel		(const QString &,
	                                                 const QString firstService = "");
	void			stopChannel		();
	void			stopService		(dabService &);
	void			startService		(dabService &, int);
	void			start_epgService	(packetdata &);
	void			scheduleSelect		(const QString &s);


	QString			checkDir		(const QString &);
	void			saveMOTObject		(QByteArray &,
	                                                 int,
	                                                 QString &);
	void			saveMOTtext		(QByteArray &, int,
	                                                 const QString &);
	void			showMOTlabel		(QByteArray &, int,
	                                                 const QString &,
	                                                 int,
	                                                 uint32_t);
	QString			slidePath		(bool, uint32_t,
	                                                   const QString &);
	void			stopMuting		();
	void			setPeakLevel	(const std::vector<float> &);
	QString			createTIILabel	(const transmitter &);
	void			addtoLogFile	(const transmitter &);
	void			removeFromList	(uint8_t, uint8_t);
	transmitter		*inList		(uint8_t, uint8_t);

//	short hands
	void                    newChannelIndex        (int);

//	scan functions
	void			startScan_to_data	();
	void			startScan_single	();
	void			startScan_continuous	();
	void			nextFor_scan_to_data	();
	void			nextFor_scan_single	();
	void			nextFor_scan_continuous	();
	void			stopScan_to_data	();
	void			stopScan_single	();
	void			stopScan_continuous	();
	QString			buildHeadLine		();
	QString			build_kop		();
	QString			build_cont_addLine	(const transmitter &);
	QString		        build_transmitterLine	(const transmitter &);

	void			show_for_single_scan	();
	void			show_for_continuous	();

//	EPG extraction
	void			process_epgData 	(const QString &,
                                                 	const QByteArray &);

	void			extractServiceInformation (const QDomDocument &,
	                                                      uint32_t, bool);
	void			saveServiceInfo		(const QDomDocument &, 
	                                                      uint32_t);
	bool			process_ensemble 	(const QDomElement &,	                                                              uint32_t);
	int			processService		(const QDomElement &);

	void			read_pictureMappings	(uint32_t);
	bool			get_serviceLogo		(QPixmap &, uint32_t);

	QString			extractName		(const QString &);

//
//	announcements
	QPixmap			fetchAnnouncement 	(int id);
	void			announcement_start	(uint16_t, uint16_t);	
	void			announcement_stop	(uint16_t);
//
//	key events for setting focus to windows
	bool			handle_keyEvent		(int);

	int			journalineKey;

	timeTableControl	*theControl;

	std::atomic<bool>	running;
//	Since the local position does not depend on the channel selected
//	the local position is not stored in the channel data
	position		localPos;
	dabService		nextService;
	std::vector<QPixmap>	strengthLabels;
	processParams		globals;
	QString			SystemVersion;
	QString			version;
	bool			error_report;
	int			fmFrequency;
	channelDescriptor	channel;
	int16_t			tii_delay;
	int32_t			dataPort;
	bool			stereoSetting;
	bool			dxMode;
//
	QString			labelStyle;
	QTimer			pauzeTimer;
	QTimer			theTimer;
	QTimer			startTimer;
	bool			stillWaiting;
	QString			path_for_files;
#ifdef	_SEND_DATAGRAM_
	QUdpSocket		dataOut_socket;
	QString			ipAddress;
	int32_t			port;
#endif
	bool			sourceDumping;
	bool			audioDumping;

	QTimer			displayTimer;
	QTimer			channelTimer;
	QTimer			presetTimer;
	QTimer			muteTimer;
//	void			write_servicePictures	(uint32_t);
	int			muteDelay;
	int32_t			numberofSeconds;

	struct theTime		localTime;
	struct theTime		UTC;

	int			audioVolume;
	size_t			previous_idle_time;
	size_t			previous_total_time;

	float			peakLeftDamped;
	float			peakRightDamped;
	int			audioTeller;
	int			pauzeSlideTeller;

	FILE			*ficDumpPointer;

	std::mutex		locker;
	std::mutex		mapHandler_locker;
	void			setSoundLabel		(bool);

	void			localSelect_SS		(const QString &,
	                                                 const QString &);
//
signals:
	void			select_ensemble_font		();
	void			select_ensemble_fontSize	();
	void			select_ensemble_fontColor	();
	void			call_scanButton		();

public slots:
//	signals from the configuration window
	void			startDirect		();

	void			copyrightText_closed		();
	void			handle_tiiThreshold	(int);
	void			handle_tiiCollisions		(int);
	void			handle_activeServices		();
	void			handle_dcRemoval		(bool);
	void			selectDecoder			(int);
	void			set_transmitters_local		(bool);
	void			handle_scheduleButton	();
	void			handle_devicewidgetButton	();
	void			handle_dlTextButton		();
	void			handle_resetButton	();
	void			handle_snrButton	();
	void			handle_set_coordinatesButton	();
	void			handle_loadTable		();
	void			handle_sourcedumpButton	();
	void			handle_correlationSelector	(int);
	void			handle_LoggerButton		(int);
	void			handle_eti_activeSelector	(int);
	void			set_streamSelector		(int);
//	connected in the Radio, coming from the config handler
	void			handle_configFrame_closed	();
	void			signal_dataTracer       (bool);
	void			timeTableFrame_closed	();

//	signals from ensemblehandler
	void			localSelect		(const QString &c,
	                                                 const QString &s);
	void			handle_backgroundTask	(const QString &);
//
//	signals from ofdmHandler
	void			set_synced		(bool);
	void			set_sync_lost		();
	void			show_tiiData		(QVector<tiiData>,
	                                                 int);
	void			show_tii_spectrum       ();
	void			show_snr		(float);
	void			show_clock_error	(int);
	void			show_null		(int, int);
	void			show_channel		(int);
	void			show_Corrector		(int, float);
//
//	signals from ofdmDecoder
	void			showIQ			(int);
	void			show_quality		(float, float, float);
	void			show_stdDev		(int);
//
//	signals from fib-config
	void			announcement		(int, int);

//	signals from fic-handler
	void			show_ficQuality		(int, int);
	void			show_ficBER		(float);

//	signals from fib-decoder
	void			ensembleName		(int, const QString &);
	void			clockTime		(int, int, int,
	                                                 int, int,
	                                                 int, int, int, int);
	void			changeinConfiguration	();
	void			nrServices		(int);
	void			lto_ecc			(int, int);
	void			setFreqList		();
	void			tell_programType		(uint32_t, int);
	void			addToEnsemble		(const QString &,
	                                                          int, int);

//	signals from the techWindow
	void			handleAudiodumpButton 	();
	void			handleFramedumpButton	();
	void			handle_techFrame_closed		();

//	signals from theNewDisplay
	void			handle_iqSelector	();
	void			handle_newDisplayFrame_closed	();
	
//	signals from correlator
	void			showCorrelation		(int, int,
	                                                 QVector<int>);
//
//	signals from samplereader
	void			show_spectrum		(int);
	void			show_dcOffset		(float);

//	signals	from mscHandler
	void			nrActiveServices		(int);

//	signals from faad decoder/fdk-aac/mp2Processor
	void			newAudio		(int, int, bool, bool);

//	signals from mp2Processor/ mp4 processor
	void			show_frameErrors	(int);
	void			setStereo		(bool);
	void			newFrame		(int);

//	signals from mp4Processor
	void			show_rsErrors		(int);
	void			show_aacErrors		(int);
	void			show_rsCorrections	(int, int);

//	signals from data-processor

//	signals from ip-datahandler
	void			sendDatagram		(int);

//	signals from journaline-controller
	void			startJournaline		(int);
	void			stopJournaline		(int);
	void			journalineData		(QByteArray, int);

//	signals from padHandler
	void			showLabel		(const QString &, int);
	void			show_mothandling	(bool);
	void			show_dl2			(uint8_t,
	                                                         uint8_t,
	                                                         const QString &);

//	signals from MOTObject
	void			handle_motObject	(QByteArray,
	                                                 QString,
	                                                 int, bool,
	                                                 uint32_t);

//	signals from tdc-dataHandler
	void			handle_tdcdata		(int, int);

//	signals from httpHandler
	void			channelSignal		(const QString &);
	void			cleanUp_mapHandler	();
	void			http_terminate		();

//
//	signals from contentTable
	void			handle_contentSelector	(const QString &);

//	signals from Qt_Audio
	void			deviceListChanged		();

//	signals	from scanHandler
	void			startScanning		();
	void			stopScanning		();

//	signals from scanListHandler
	void			handleScanListSelect	(const QString &);

//	signals	from scheduler
	void			scheduler_timeOut	(const QString &);

//	Local signals

	
	void			no_signal_found		();
	void			closeEvent		(QCloseEvent *event);

	void			handle_presetSelect	(const QString &,
	                                                 const QString &);
	

	void			showPeakLevel		(float, float);

	void			handle_deviceFrame_closed	();

	void			doStart			(const QString &);
	void			newDevice		(const QString &);
	void			show_changeLabel (const QStringList notInOld,
                                          	  const QStringList notInNew);
//
private slots:
//	button and selectorhandlers
	void			handle_configButton	();
	void			handle_httpButton	();
	void			handle_copyrightLabel	();
	void			handle_contentButton	();
	void			handle_detailButton	();
	void			handle_scanButton	();
	void			handle_etiHandler	();
	void			handle_spectrumButton	();
	void			handle_scanListButton	();
	void			handle_presetButton	();
	void			handle_prevServiceButton        ();
	void			handle_nextServiceButton        ();
	void			handle_nextChannelButton	();
	void			handle_prevChannelButton	();
	void			handle_muteButton	();
	void			handle_folderButton	();
	void			devSL_visibility	();

	void			handle_channelSelector	(const QString &);
	void			setVolume		(int);
	void			handle_snrLabel		();
	void			handle_distanceLabel	();

	void			handle_startTimeTable	();
//
//
	void			TerminateProcess	();
	void			updateTimeDisplay	();
	void			channel_timeOut		();
	void			setPresetService	();
	void			muteButton_timeOut	();
	void			waitingToDelete		();
	void			show_pauzeSlide		();
//
//	color handlers
	void			handle_labelColor	();
	void			color_scanButton	();
	void			color_presetButton	();
	void			color_spectrumButton	();
	void			color_scanListButton	();
	void			color_prevServiceButton ();     
	void			color_nextServiceButton ();
	void			color_configButton	();
	void			color_httpButton	();

};
