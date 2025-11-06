#
/*
 *    Copyright (C) 2016 .. 2024
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

typedef struct {
	uint32_t serviceId;
	std::vector<multimediaElement> elements;
} mmDescriptor;

class	QSettings;
class	ofdmHandler;
class	deviceHandler;
class	audioPlayer;
class	common_fft;
class	timeTableHandler;
class	audioDisplay;
#ifdef	HAVE_PLUTO_RXTX
class	dabStreamer;
#endif

class techData;
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
#define	TII_BUTTON		QString ("tiiButton")
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

typedef  struct {
	QString serviceName;
	uint32_t SId;
} SPI_service;

struct	theTime {
	int	year;
	int	month;
	int	day;
	int	hour;
	int	minute;
	int	second;
};

//typedef struct {
//	uint32_t serviceId;
//	std::vector<multimediaElement> elements;
//} mmDescriptor;
//
//	Pretty important, the channel descriptor is supposed to
//	contain all data related to the currently selected class

class	channelDescriptor {
public:
	channelDescriptor () {
	cleanChannel ();
}
	~channelDescriptor () {
}

	QString		channelName;
	int		tunedFrequency;
	bool		realChannel;
	bool		etiActive;
	QString		ensembleName;
	uint32_t	Eid;
	int		nrServices;	// measured
	int		serviceCount;	// from FIC or nothing
	dabService	currentService;
	std::vector<SPI_service> SPI_services;
	std::vector<dabService> runningTasks;
	bool		hasEcc;
	uint8_t		eccByte;
	int		lto;
	QString		countryName;
	int		nrTransmitters;
	std::vector<transmitter>	transmitters;
	QString		transmitterName;
	int		snr;
	bool		announcing;
	position	targetPos;
	QDate		theDate;
	int8_t		mainId;
	int8_t		subId;
	float		height;
	float		distance;
	float		azimuth;
	bool		audioActive;
	std::vector<mmDescriptor>servicePictures;
	void	cleanChannel () {
	   transmitters. resize (0);
	   servicePictures. resize (0);
	   SPI_services. resize (0);
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
	   transmitterName	= "";
	   snr			= 0;
	   announcing		= false;
	   height		= -1;
	   distance		= -1;
	   audioActive	 	= false;
	   currentService. isValid	= false;
	   currentService. frameDumper	= nullptr;
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
	deviceChooser		theDeviceChoser;
	dxDisplay		theDXDisplay;
	logger			theLogger;
	scanHandler		theSCANHandler;
	tiiMapper		theTIIProcessor;
	timeTableHandler	myTimeTable;
	xmlExtractor		xmlHandler;
	epgCompiler		epgVertaler;
//	end of variables that are initalized

	QScopedPointer<configHandler>	configHandler_p;
	QScopedPointer<ensembleHandler> theEnsembleHandler;
	QScopedPointer<ofdmHandler>	theOFDMHandler;
	QScopedPointer<deviceHandler>	inputDevice_p;
	bool			autoStart_http		();
	bool			dxMode;
	QScopedPointer<journaline_dataHandler>	journalineHandler;
	int			journalineKey;
//
//	Since the local position does not depend on the channel selected
//	the local position is not stored in the channel data
	position		localPos;
	dabService		nextService;
	std::vector<QPixmap>	strengthLabels;
	httpHandler		*mapHandler;
	processParams		globals;
	QString			SystemVersion;
	QString			version;
	int			fmFrequency;
	contentTable		*contentTable_p;
	contentTable		*scanTable_p;
	channelDescriptor	channel;
	QDialog			*the_aboutLabel;
	bool			error_report;
	QScopedPointer<techData> techWindow_p;
	QSettings		*dabSettings_p;
	int16_t			tii_delay;
	int32_t			dataPort;
	bool			stereoSetting;
	std::atomic<bool>	running;
//
	QString			labelStyle;
#ifdef	HAVE_PLUTO_RXTX
	dabStreamer		*streamerOut_p;
#endif
	audioPlayer		*soundOut_p;
#ifdef	DATA_STREAMER
	tcpServer		*dataStreamer_p;
#endif
#ifdef	CLOCK_STREAMER
	tcpServer		*clockStreamer_p;
#endif
	QTimer			epgTimer;
	QTimer			pauzeTimer;
	QTimer			stressTimer;
	QString			path_for_files;
#ifdef	_SEND_DATAGRAM_
	QUdpSocket		dataOut_socket;
	QString			ipAddress;
	int32_t			port;
#endif
	bool			sourceDumping;
	bool			audioDumping;
	void			set_Colors		();
	void			setChannelButton	(int);

	void			displaySlide	(const QPixmap &,	
	                                              const QString &t = "");
	QTimer			displayTimer;
	QTimer			channelTimer;
	QTimer			presetTimer;
	void			write_servicePictures	(uint32_t);
	QTimer			muteTimer;
	int			muteDelay;
	int32_t			numberofSeconds;
	void			connectGUI		();
	void			disconnectGUI		();

	struct theTime		localTime;
	struct theTime		UTC;

	int			audioVolume;
	size_t			previous_idle_time;
	size_t			previous_total_time;

	float			peakLeftDamped;
	float			peakRightDamped;
	int			audioTeller;
	int			pauzeSlideTeller;
	QPixmap			fetchAnnouncement (int id);

	QString			convertTime		(int, int, int, int, int);
	QString			convertTime		(struct theTime &);
	void			setButtonColors		(QPushButton *,
	                                                 const QString &);
	QString			footText		();
	QString			presetText		();
	void			cleanScreen		();
	void			hideButtons		();
	void			showButtons		();
	deviceHandler		*createDevice		(const QString &,
	                                                             logger *);

	void			start_etiHandler	();
	void			stop_etiHandler		();
	QString			checkDir		(const QString &);
//
	void			startAudioservice	(audiodata &);
	void			startPacketservice	(packetdata &);
	void			startAudioDumping	();
	void			stopAudioDumping	();
	void			scheduledAudioDumping	();
	void			scheduledDLTextDumping	();
	void			scheduledFICDumping	();
	FILE			*ficDumpPointer;

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
	void			localSelect		(const QString &c,
	                                                 const QString &s);
	void			scheduleSelect		(const QString &s);

	QString			buildHeadLine		();
	QString			build_kop		();
	QString			build_cont_addLine	(const transmitter &);
	QString		        build_transmitterLine	(const transmitter &);

	void			show_for_single_scan	();
	void			show_for_continuous	();

	void			startDirect		();
	void			saveMOTObject		(QByteArray &,
	                                                 QString &);

	void			saveMOTtext		(QByteArray &, int,
	                                                 const QString &);
	void			showMOTlabel		(QByteArray &, int,
	                                                 const QString &,
	                                                 int, bool);
	void			stopMuting		();
//	short hands
	void                    newChannelIndex        (int);
	std::mutex		locker;
	void			setSoundLabel		(bool);
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

	void			setPeakLevel	(const std::vector<float> &);
	QString			createTIILabel	(const transmitter &);
	void			addtoLogFile	(const transmitter &);
	void			removeFromList	(uint8_t, uint8_t);
	transmitter		*inList		(uint8_t, uint8_t);

//	EPG extraction
	void			extractServiceInformation (const QDomDocument &,
	                                                      uint32_t, bool);
	void			saveServiceInfo	(const QDomDocument &, 
	                                                      uint32_t);
	bool			process_ensemble (const QDomElement &, uint32_t);
	int			processService	(const QDomElement &);

	void			read_pictureMappings	(uint32_t);
	bool			get_serviceLogo		(QPixmap &, uint32_t);
	QString			extractName	(const QString &);

//
//	announcements
	void			announcement_start	(uint16_t, uint16_t);	
	void			announcement_stop	();
//
//	key events for setting focus to windows
	bool			handle_keyEvent		(int);
//
signals:
	void			select_ensemble_font	();
	void			select_ensemble_fontSize	();
	void			select_ensemble_fontColor	();

	void			call_scanButton		();
public slots:
	void			lto_ecc			(int, int);
	void			setFreqList		();
	void			channelSignal		(const QString &);
	void			show_dcOffset		(float);
	void			startScanning		();
	void			stopScanning		();
	void			show_quality		(float, float, float);
	void			show_rsCorrections	(int, int);
	void			show_clock_error	(int);

	void			show_Corrector		(int, float);
	void			addToEnsemble		(const QString &,
	                                                          int, int);
	void			ensembleName		(int, const QString &);
	void			nrServices		(int);
	void			show_frameErrors	(int);
	void			show_rsErrors		(int);
	void			show_aacErrors		(int);
	void			show_ficQuality		(int, int);
	void			show_ficBER		(float);
	void			set_synced		(bool);
	void			showLabel		(const QString &, int);
	void			handle_motObject	(QByteArray,
	                                                 QString,
	                                                 int, bool, bool);
	void			sendDatagram		(int);
	void			handle_tdcdata		(int, int);
	void			changeinConfiguration	();
	void			newAudio		(int, int, bool, bool);

	void			localSelect_SS		(const QString &,
	                                                 const QString &);
	
	void			setStereo		(bool);
	void			set_streamSelector	(int);
	void			no_signal_found		();
	void			show_mothandling	(bool);
	void			set_sync_lost		();
	void			closeEvent		(QCloseEvent *event);
	void			clockTime		(int, int, int,
	                                                 int, int,
	                                                 int, int, int, int);
	void			announcement		(int, int);
	void			newFrame		(int);

	void			epgTimer_timeOut	();
	void			handle_presetSelect	(const QString &,
	                                                 const QString &);
	void			handle_contentSelector	(const QString &);
	
	void			http_terminate		();
	void			show_channel		(int);

	void			handle_iqSelector	();

	void			show_spectrum		(int);
	void			handle_tiiThreshold	(int);
	void			show_tiiData		(QVector<tiiData>,
	                                                 int);
	void			show_tii_spectrum       ();
	void			show_snr		(float);
	void			show_null		(int, int);
	void			showIQ			(int);
	void			showCorrelation		(int, int,
	                                                 QVector<int>);
	void			show_stdDev		(int);
	void			showPeakLevel		(float, float);

	void			handle_techFrame_closed		();
	void			handle_configFrame_closed	();
	void			handle_deviceFrame_closed	();
	void			handle_newDisplayFrame_closed	();

	void			doStart			(const QString &);
	void			newDevice		(const QString &);
	void			handle_scheduleButton	();
	void			handle_devicewidgetButton	();
	void			handle_resetButton	();
	void			handle_dlTextButton		();
	void			handle_snrButton	();
	void			handle_sourcedumpButton	();
	void			scheduler_timeOut	(const QString &);
	void			show_changeLabel (const QStringList notInOld,
                                          	  const QStringList notInNew);

	void			process_tiiSelector	(bool);
//
//	triggered from the techdata
	void			handleFramedumpButton	();
	void			handleAudiodumpButton 	();

	void			startJournaline			(int);
	void			stopJournaline			(int);
	void			journalineData			(QByteArray,
	                                                         int);
//	Somehow, these must be connected to the GUI
private slots:
	void			handle_aboutLabel	();
	void			show_pauzeSlide		();
	void			handle_timeTable	();
	void			handle_contentButton	();
	void			handle_detailButton	();
	void			handle_scanButton	();
	void			handle_etiHandler	();

	void			handle_spectrumButton	();

	void			handle_scanListButton	();
	void			handle_presetButton	();
	void			handle_prevServiceButton        ();
	void			handle_nextServiceButton        ();

	void			handle_channelSelector		(const QString &);
	void			handle_nextChannelButton	();
	void			handle_prevChannelButton	();

	void			handle_muteButton		();
	void			handle_folderButton		();

	void			handleScanListSelect	(const QString &);
	void			TerminateProcess	();
	void			updateTimeDisplay	();
	void			channel_timeOut		();

	void			handle_backgroundTask	(const QString &);

	void			setPresetService	();
	void			muteButton_timeOut	();

	void			handle_configButton	();
	void			handle_httpButton	();
	void			setVolume		(int);
	void			handle_snrLabel		();
	void			handle_tiiButton	();
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
	void			color_tiiButton		();

	void			devSL_visibility	();

//
//	config handlers
public slots:
	void			selectDecoder			(int);
	void			set_transmitters_local		(bool);
	void			handle_correlationSelector	(int);
	void			handle_LoggerButton		(int);
	void			handle_set_coordinatesButton	();
//	void			handle_transmSelector		(int);
	void			handle_eti_activeSelector	(int);
	void			handle_dcRemoval		(bool);
	void			handle_loadTable		();
	void			handle_tiiCollisions		(int);
	void			handle_tiiFilter		(bool);

	void			deviceListChanged		();
	void			show_dl2			(uint8_t,
	                                                         uint8_t,
	                                                         const QString &);
	void			nrActiveServices		(int);
	void			handle_activeServices		();

};
