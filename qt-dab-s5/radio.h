#
/*
 *    Copyright (C) 2013, 2014, 2015, 2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB (formerly SDR-J, JSDR).
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
#include	<QStandardItemModel>
#ifdef	_SEND_DATAGRAM_
#include	<QUdpSocket>
#endif
#include	<QVector>
#include	<QComboBox>
#include	<QByteArray>
#include	<QLabel>
#include	<QTimer>
#include	<sndfile.h>
#include	"ui_dabradio-5.h"
#include	"ringbuffer.h"
#include        "band-handler.h"
#include	"process-params.h"
#include	"dl-cache.h"
#include	"tii-codes.h"
#include	"content-table.h"
#include	<memory>
#include	<mutex>
#ifdef	DATA_STREAMER
#include	"tcp-server.h"
#endif
#include	"preset-handler.h"
//#include	"scanner-table.h"
#include	"epgdec.h"
#include	"epg-decoder.h"

#include	"spectrum-viewer.h"
#include	"tii-viewer.h"
#include	"correlation-viewer.h"
#include	"snr-viewer.h"

#include	"findfilenames.h"

#include	"scheduler.h"

#include	"http-handler.h"

class	QSettings;
class	ofdmHandler;
class	deviceHandler;
class	audioBase;
class	common_fft;
class	scanListHandler;
class	timeTableHandler;
class	audioDisplay;
#ifdef	HAVE_PLUTO_RXTX
class	dabStreamer;
#endif

class techData;
#include	"ui_config-helper.h"

/*
 *	The main gui object. It inherits from
 *	QWidget and the generated form
 */

class dabService {
public:
	QString		channel;
	QString		serviceName;
	uint32_t	SId;
	int		SCIds;
	int		subChId;
	bool		valid;
	bool		is_audio;
	FILE		*fd;
	FILE		*frameDumper;
};

struct	theTime {
	int	year;
	int	month;
	int	day;
	int	hour;
	int	minute;
	int	second;
};

class	channelDescriptor {
public:
	channelDescriptor () {
}
	~channelDescriptor () {
}

	QString		channelName;
	bool		realChannel;
	bool		etiActive;
	int		serviceCount;
	int		frequency;
	QString		ensembleName;
	uint8_t		mainId;
	uint8_t		subId;
	std::vector<dabService> backgroundServices;
	dabService	currentService;
	dabService	nextService;
	uint32_t	Eid;
	bool		has_ecc;
	uint8_t		ecc_byte;
	bool		tiiFile;
	QString		transmitterName;
	QString		countryName;
	int		nrTransmitters;
	position	localPos;
	position	targetPos;
	int		snr;
	QByteArray	transmitters;

	void	cleanChannel () {
	realChannel	= true;
	serviceCount	= -1;
	frequency	= -1;
	ensembleName	=  "";
	nrTransmitters	= 0;
	countryName	= "";
	targetPos	= position {0, 0};
	mainId		= 0;
	subId		= 0;
	Eid		= 0;
	has_ecc		= false;
	snr		= 0;
	transmitters. resize (0);
	currentService. frameDumper	= nullptr;
	nextService. frameDumper	= nullptr;
}
};

class RadioInterface: public QWidget, private Ui_dabradio {
Q_OBJECT
public:
		RadioInterface		(QSettings	*,
	                                 const QString	&,
	                                 const QString	&,
	                                 const QString	&,
	                                 bool,
	                                 int32_t	dataPort,
	                                 int32_t	clockPort,
	                                 int,
	                                 QWidget	*parent = nullptr);
		~RadioInterface		();

protected:
	bool			eventFilter (QObject *obj, QEvent *event);
private:
	FILE			*dlTextFile;
	RingBuffer<std::complex<float>>  spectrumBuffer;
	RingBuffer<std::complex<float>>  iqBuffer;
	RingBuffer<std::complex<float>>  tiiBuffer;
	RingBuffer<std::complex<float>>  nullBuffer;
	RingBuffer<float>	snrBuffer;
	RingBuffer<float>	responseBuffer;
	RingBuffer<uint8_t>	frameBuffer;
	RingBuffer<uint8_t>	dataBuffer;
	RingBuffer<int16_t>	audioBuffer;
	spectrumViewer		my_spectrumViewer;
	correlationViewer	my_correlationViewer;
	tiiViewer		my_tiiViewer;
	snrViewer		my_snrViewer;
	presetHandler		my_presetHandler;
	bandHandler		theBand;
	QFrame			dataDisplay;
	QFrame			configDisplay;
	dlCache			the_dlCache;
	tiiHandler		tiiProcessor;
	findfileNames		filenameFinder;
	Scheduler		theScheduler;
	RingBuffer<int16_t>	theTechData;
	httpHandler		*mapHandler;
	processParams		globals;
	QString			version;
	QString			theFont;
	int			fontSize;
	int			fmFrequency;
	contentTable		*my_contentTable;
	contentTable		*my_scanTable;
	FILE			*logFile;
	channelDescriptor	channel;
	int			maxDistance;
	void			LOG		(const QString &,
	                                         const QString &);
	bool			error_report;
	techData		*theTechWindow;
	Ui_configWidget		configWidget;
	QSettings		*dabSettings;
	int16_t			tii_delay;
	int32_t			dataPort;
	bool			stereoSetting;
	std::atomic<bool>	running;
	std::atomic<bool>	scanning;
	deviceHandler		*inputDevice;
#ifdef	HAVE_PLUTO_RXTX
	dabStreamer		*streamerOut;
#endif
	ofdmHandler		*my_ofdmHandler;
	audioBase		*soundOut;
#ifdef	DATA_STREAMER
	tcpServer		*dataStreamer;
#endif
#ifdef	CLOCK_STREAMER
	tcpServer		*clockStreamer;
#endif
	CEPGDecoder		epgHandler;
	epgDecoder		epgProcessor;
	QString			epgPath;
	QTimer			epgTimer;
	uint32_t		extract_epg (QString,
	                                     std::vector<serviceId> &serviceList,
	                                     uint32_t);
	bool			saveSlides;
	QString			picturesPath;
	QString			filePath;
#ifdef	_SEND_DATAGRAM_
	QUdpSocket		dataOut_socket;
	QString			ipAddress;
	int32_t			port;
#endif
	SNDFILE                 *rawDumper;
	SNDFILE                 *audioDumper;
	FILE			*scanDumpFile;
	void			set_Colors		();
	void			set_channelButton	(int);
	QStandardItemModel	model;
	std::vector<serviceId>	serviceList;
	bool			isMember (const std::vector<serviceId> &, 
	                                       serviceId);
	std::vector<serviceId>
	  	                insert   (std::vector<serviceId> &,
	                                  serviceId, int);

	void			show_pauzeSlide	();
	QTimer			displayTimer;
	QTimer			channelTimer;
	QTimer			presetTimer;
	QTimer			muteTimer;
	int			muteDelay;
	int32_t			numberofSeconds;
	int16_t			ficBlocks;
	int16_t			ficSuccess;
	int			total_ficError;
	int			total_fics;
	void			connectGUI		();
	void			disconnectGUI		();

	int			serviceCount;
	struct theTime		localTime;
	struct theTime		UTC;
	QString			convertTime		(int, int, int, int, int);
	void			set_buttonColors	(QPushButton *,
	                                                 const QString &);
	QString			footText		();
	QString			presetText		();
	void			cleanScreen		();
	void			hideButtons		();
	void			showButtons		();
	deviceHandler		*setDevice		(const QString &);
	scanListHandler		*my_scanList;
	scanListHandler		*my_presets;
	timeTableHandler	*my_timeTable;

	void			start_etiHandler	();
	void			stop_etiHandler		();
	QString			checkDir		(const QString);
//
	void			startAudioservice	(audiodata *);
	void			startPacketservice	(const QString &);
	void			startScanning		();
	void			stopScanning		(bool);
	void			startAudiodumping	();
	void			stopAudiodumping	();
	void			scheduled_audioDumping	();
	void			scheduled_dlTextDumping ();
	void			scheduled_ficDumping ();
	FILE			*ficDumpPointer;

	void			startSourcedumping     ();
	void			stopSourcedumping      ();
	void			startFramedumping      ();
	void			stopFramedumping       ();
	void			scheduled_frameDumping	(const QString &);
	void			startChannel		(const QString &);
	void			stopChannel		();
	void			stopService		(dabService &);
	void			startService		(dabService &);
	void			colorService		(QModelIndex ind,
	                                                 QColor c, int pt,
	                                                 bool italic = false);
	void			localSelect		(const QString &s);
	void			scheduleSelect		(const QString &s);
	void			localSelect		(const QString &,
	                                                 const QString &);
	void			showServices		();

	bool			doStart			();
	void			save_MOTObject		(QByteArray &,
	                                                          QString);

	void			save_MOTtext		(QByteArray &, int,
	                                                         QString);
	void			show_MOTlabel		(QByteArray &, int,
	                                                 QString, int, bool);
	void			stop_muting		();


	enum direction {FORWARD, BACKWARDS};

	void			handle_serviceButton	(direction);
	void			hide_for_safety		();
	void			show_for_safety		();
//
//	short hands
	void                    new_presetIndex         (int);
	void                    new_channelIndex        (int);

	std::mutex		locker;
	bool			transmitterTags_local;
	void			colorServiceName (const QString &s,
	                                          QColor color, int fS, bool);

signals:
	void                    set_newChannel		(int);
	void                    set_newPresetIndex      (int);

public slots:
//	void			set_CorrectorDisplay	(int);
	void			show_Corrector		(int, float);
	void			addtoEnsemble		(const QString &, int);
	void			nameofEnsemble		(int, const QString &);
	void			show_frameErrors	(int);
	void			show_rsErrors		(int);
	void			show_aacErrors		(int);
	void			show_ficSuccess		(bool);
	void			show_snr		(float);
	void			setSynced		(bool);
	void			showLabel		(const QString &);
	void			handle_motObject	(QByteArray, QString,
	                                                 int, bool, bool);
	void			sendDatagram		(int);
	void			handle_tdcdata		(int, int);
	void			changeinConfiguration	();
	void			newAudio		(int, int);
//
	void			setStereo		(bool);
	void			set_streamSelector	(int);
	void			No_Signal_Found		();
	void			show_motHandling	(bool);
	void			setSyncLost		();
	void			showCorrelation		(int, int, 
	                                                      QVector<int>);
	void			showSpectrum		(int);
	void			showIQ			(int);
	void			showQuality		(float, float, float);

	void			show_rsCorrections	(int, int);
	void			show_tii		(int, int);
	void			show_tii_spectrum	();
	void			closeEvent		(QCloseEvent *event);
	void			clockTime		(int, int, int,
	                                                 int, int,
	                                                 int, int, int, int);
	void			startAnnouncement	(const QString &, int);
	void			stopAnnouncement	(const QString &, int);
	void			newFrame		(int);

	void			show_clockError		(int);
	void			show_null		(int);
	void			set_epgData		(int, int,
	                                                 const QString &,
	                                                 const QString &);
	void			epgTimer_timeOut	();
	void			switchVisibility	(QWidget *);
	void			nrServices		(int);

	void			handle_presetSelector	(const QString &);
	void			handle_contentSelector	(const QString &);
	
	void			http_terminate		();


//	Somehow, these must be connected to the GUI
private slots:
	void			handle_timeTable	();
	void			handle_contentButton	();
	void			handle_detailButton	();
	void			handle_resetButton	();
	void			handle_scanButton	();
	void			handle_etiHandler	();

	void			handle_tiiButton	();
	void			handle_snrButton	();
	void			handle_correlationButton	();
	void			handle_spectrumButton	();
	void			handle_devicewidgetButton	();
	void			handle_clearScan_Selector	(int);

	void			handle_scanListButton	();
	void			handle_sourcedumpButton	();
	void			handle_framedumpButton	();
	void			handle_audiodumpButton 	();

	void			handle_prevServiceButton	();
	void			handle_nextServiceButton	();
	void			handle_channelSelector		(const QString &);
	void			handle_nextChannelButton	();
	void			handle_prevChannelButton	();

	void			handle_scanListSelect	(const QString &);
	void			TerminateProcess	();
	void			updateTimeDisplay	();
	void			channel_timeOut		();

	void			doStart			(const QString &);
	void			newDevice		(const QString &);

	void			selectService		(QModelIndex);
	void			setPresetService	();
	void			handle_muteButton	();
	void			muteButton_timeOut	();
	void			scheduler_timeOut	(const QString &);
	void			handle_dlTextButton     ();

	void			handle_configButton	();
	void			handle_scheduleButton	();
	void			handle_httpButton	();
	void			handle_onTop		(int);
	void			handle_autoBrowser	(int);
	void			handle_transmitterTags	(int);

//
//	color handlers
	void			color_contentButton	();
	void			color_detailButton	();
	void			color_resetButton	();
	void			color_scanButton	();
	void			color_tiiButton		();
	void			color_correlationButton	();
	void			color_spectrumButton	();
	void			color_snrButton		();
	void			color_devicewidgetButton	();
	void			color_scanListButton	();
	void			color_sourcedumpButton	();
	void			color_muteButton	();
	void			color_prevChannelButton	();
	void			color_nextChannelButton	();
	void			color_prevServiceButton	();
	void			color_nextServiceButton	();
	void			color_dlTextButton	();
	void			color_scheduleButton	();
	void			color_configButton	();
	void			color_httpButton	();
	void			color_set_coordinatesButton     ();
	void			color_loadTableButton   ();
//
//	config handlers
	void			handle_muteTimeSetting		(int);
	void			handle_switchDelaySetting 	(int);
	void			handle_orderAlfabetical		();
	void			handle_orderServiceIds		();
	void			handle_ordersubChannelIds	();
	void			handle_scanmodeSelector		(int);
	void			handle_saveServiceSelector	(int);
	void			handle_skipList_button		();
	void			handle_skipFile_button		();
	void			handle_tii_detectorMode		(int);
	void			handle_LoggerButton		(int);
	void			handle_set_coordinatesButton	();
	void			handle_portSelector		();
	void			handle_epgSelector		(int);
	void			handle_transmSelector		(int);
	void			handle_eti_activeSelector	(int);
	void			handle_saveSlides		(int);
	void			handle_skinSelector		();
	void			loadTable			();
};
