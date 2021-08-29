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

#ifndef __RADIO__
#define __RADIO__

#include	"dab-constants.h"
#include	<QMainWindow>
#include	<QStringList>
#include	<QStandardItemModel>
#ifdef	_SEND_DATAGRAM_
#include	<QUdpSocket>
#endif
#include	<QComboBox>
#include	<QByteArray>
#include	<QLabel>
#include	<QTimer>
#include	<sndfile.h>
#include	"ui_dabradio.h"
#include	"dab-processor.h"
#include	"ringbuffer.h"
#include        "band-handler.h"
#include	"text-mapper.h"
#include	"process-params.h"
#include	"dl-cache.h"
#include	<memory>
#ifdef	DATA_STREAMER
#include	"tcp-server.h"
#endif
#include	"preset-handler.h"
#include	"scanner-table.h"
#ifdef	TRY_EPG
#include	"epgdec.h"
#include	"epg-decoder.h"
#endif

#include	"spectrum-viewer.h"
#include	"tii-viewer.h"
#include	"correlation-viewer.h"
#include	"snr-viewer.h"

#include	"findfilenames.h"

#include	"scheduler.h"
class	QSettings;
class	deviceHandler;
class	audioBase;
class	common_fft;
class	serviceDescriptor;
class	historyHandler;
class	timeTableHandler;
#ifdef	HAVE_PLUTO_RXTX
class	dabStreamer;
#endif

#include	"ui_technical_data.h"
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
	bool		valid;
	bool		is_audio;
};

struct	theTime {
	int	year;
	int	month;
	int	day;
	int	hour;
	int	minute;
	int	second;
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
	scannerTable		theTable;
	QFrame			dataDisplay;
	QFrame			configDisplay;
	dlCache			the_dlCache;
	findfileNames		filenameFinder;
	processParams		globals;
	QString			version;
	QString			theFont;
	int			fontSize;
	int			fmFrequency;
	Scheduler		theScheduler;
	QString			externalSchedule;
#ifdef	__LOGGING__
	FILE			*logFile;
#endif
	void			LOG		(const QString &,
	                                           const QString &);
	int			serviceOrder;
	bool			error_report;
	Ui_technical_data	techData;
//	QFrame			*dataDisplay;
	Ui_configWidget		configWidget;
//	QFrame			*configDisplay;
	QSettings		*dabSettings;
	dabService		currentService;
	dabService		nextService;

	QByteArray		transmitters;
	int16_t			tii_delay;
	int32_t			dataPort;
	serviceDescriptor	*currentServiceDescriptor;
	QLabel			*motSlides;
//	std::vector<int>	secondariesVector;
	bool			isSynced;
	bool			stereoSetting;
	std::atomic<bool>	running;
	std::atomic<bool>	scanning;
	deviceHandler		*inputDevice;
#ifdef	HAVE_PLUTO_RXTX
	dabStreamer		*streamerOut;
#endif
	textMapper		the_textMapper;
	dabProcessor		*my_dabProcessor;
	audioBase		*soundOut;
#ifdef	DATA_STREAMER
	tcpServer		*dataStreamer;
#endif
#ifdef	CLOCK_STREAMER
	tcpServer		*clockStreamer;
#endif
#ifdef	TRY_EPG
	CEPGDecoder		epgHandler;
	epgDecoder		epgProcessor;
	QString			epgPath;
	QTimer			epgTimer;
	uint32_t		extract_epg (QString,
                                             std::vector<serviceId> serviceList,
	                                     uint32_t);
#endif
	bool			saveSlides;
	QString			picturesPath;
	QString			filePath;
#ifdef	_SEND_DATAGRAM_
	QUdpSocket		dataOut_socket;
	QString			ipAddress;
	int32_t			port;
#endif
	SNDFILE                 *rawDumper;
        FILE                    *frameDumper;
        SNDFILE                 *audioDumper;
	FILE			*scanDumpFile;
	void			set_Colors		();
	void			set_channelButton	(int);
	QStandardItemModel	model;
	std::vector<serviceId>	serviceList;
	bool			isMember (std::vector<serviceId>,
	                                       serviceId);
	std::vector<serviceId>
	  	                insert   (std::vector<serviceId>,
	                                  serviceId, int);

	QStringList		soundChannels;
	QTimer			displayTimer;
	QTimer			channelTimer;
	QTimer			presetTimer;
	QTimer			startTimer;
	QTimer			muteTimer;
	int			muteDelay;
	int32_t			numberofSeconds;
	bool			muting;
	int16_t			ficBlocks;
	int16_t			ficSuccess;
	int			total_ficError;
	int			total_fics;
	void			connectGUI		();
	void			disconnectGUI		();

	struct theTime		localTime;
	struct theTime		UTC;
	QString			convertTime		(int, int, int, int, int);
	void			set_buttonColors	(QPushButton *,
	                                                 const QString &);
	QString			footText		();
	void			cleanScreen		();
	void			set_picturePath		();
	void			dumpControlState	(QSettings *);
	void			hideButtons		();
	void			showButtons		();
	deviceHandler		*setDevice		(const QString &);
	historyHandler		*my_history;
	historyHandler		*my_presets;
	timeTableHandler	*my_timeTable;
//
	void			start_audioService	(audiodata *);
	void			start_packetService	(const QString &);
	void			startScanning		();
	void			stopScanning		(bool);
        void			start_audioDumping      ();
        void			stop_audioDumping       ();
        void			scheduled_audioDumping	();
        void			start_sourceDumping     ();
        void			stop_sourceDumping      ();
        void			start_frameDumping      ();
        void			stop_frameDumping       ();
	void			scheduled_frameDumping	(const QString &);
	void			startChannel		(const QString &);
	void			stopChannel		();
	void			stopService		();
	void			startService		(dabService *);
	void			colorService		(QModelIndex ind,
	                                                   QColor c, int pt);
	void			localSelect		(const QString &s);
	void			scheduleSelect		(const QString &s);
	void			localSelect		(const QString &,
	                                                 const QString &);
	void			showServices		();

	bool			doStart			();
	void			save_MOTtext		(QByteArray, int,
                                                                 QString);
	void			show_MOTlabel		(QByteArray, int,
                                                                  QString, int);
	void			stop_muting		();
enum direction {FORWARD, BACKWARDS};

	void			handle_serviceButton	(direction);
	void			hide_for_safety		();
	void			show_for_safety		();
//
//	short hands
        void                    new_presetIndex         (int);
        void                    new_channelIndex        (int);

signals:
	void                    set_newChannel		(int);
        void                    set_newPresetIndex      (int);

public slots:
	void			set_CorrectorDisplay	(int);
	void			addtoEnsemble		(const QString &, int);
	void			nameofEnsemble		(int, const QString &);
	void			show_frameErrors	(int);
	void			show_rsErrors		(int);
	void			show_aacErrors		(int);
	void			show_ficSuccess		(bool);
	void			show_snr		(int);
	void			setSynced		(bool);
	void			showLabel		(QString);
	void			handle_motObject	(QByteArray, QString,
	                                                      int, bool);
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
	void			showCorrelation		(int, int);
	void			showSpectrum		(int);
	void			showIQ			(int);
	void			showQuality		(float);
	void			show_rsCorrections	(int);
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
#ifdef	TRY_EPG
	void			set_epgData		(int,
	                                                 int, const QString &);
	void			epgTimer_timeOut	();
#endif
	void			switchVisibility	(QWidget *);

//	Somehow, these must be connected to the GUI
private slots:
	void			handle_timeTable	();
	void			handle_contentButton	();
	void			handle_detailButton	();
	void			handle_resetButton	();
	void			handle_scanButton	();

	void			handle_tiiButton	();
	void			handle_snrButton	();
	void			handle_correlationButton	();
	void			handle_spectrumButton	();
	void			handle_devicewidgetButton	();

	void			doStart			(const QString &);
	void			newDevice		(const QString &);

	void			handle_historyButton	();
        void			handle_sourcedumpButton	();
        void			handle_framedumpButton	();
	void			handle_audiodumpButton 	();

	void			handle_presetSelector	(const QString &);
	void			handle_prevServiceButton	();
        void			handle_nextServiceButton	();
	void			selectChannel		(const QString &);
	void			handle_nextChannelButton();
	void			handle_prevChannelButton();

	void			handle_historySelect	(const QString &);
	void			TerminateProcess	();
	void			updateTimeDisplay	();
	void			channel_timeOut		();

	void			selectService		(QModelIndex);
	void			setPresetStation	();
	void			handle_muteButton	();
	void			muteButton_timeOut	();
	void			scheduler_timeOut	(const QString &);
	void			handle_dlTextButton     ();

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
	void			color_historyButton	();
	void			color_sourcedumpButton	();
	void			color_muteButton	();
	void			color_prevChannelButton	();
	void			color_nextChannelButton	();
	void			color_prevServiceButton	();
	void			color_nextServiceButton	();
	void			color_framedumpButton	();
	void			color_audiodumpButton	();
	void			color_configButton	();
	void			color_dlTextButton	();
//
//	config handlers
	void			handle_configSetting		();
	void			handle_muteTimeSetting		(int);
	void			handle_switchDelaySetting 	(int);
	void			handle_orderAlfabetical		();
	void			handle_orderServiceIds		();
	void			handle_ordersubChannelIds	();
	void			handle_scheduleSelector		();
	void			handle_plotLengthSetting	(int);
	void			handle_scanmodeSelector		(int);
	void			handle_motslideSelector		(int);
	void			handle_saveServiceSelector	(int);
	void			handle_snrHeightSelector	(int);
	void			handle_snrLengthSelector	(int);
	void			handle_skipList_button		();
	void			handle_skipFile_button		();
	void			handle_snrDelaySetting		(int);
	void			handle_tii_detectorMode		(int);

};
#endif
