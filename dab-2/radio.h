#
/*
 *    Copyright (C) 2013, 2018 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of dab-2
 *
 *    dab-2 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    dab-2 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dab-2; if not, write to the Free Software
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
#ifdef	DATA_STREAMER
#include	"tcp-server.h"
#endif
#include        "spectrum-viewer.h"
#include        "correlation-viewer.h"
#include        "tii-viewer.h"
#include	"preset-handler.h"
#include	"scanner-table.h"
#ifdef  TRY_EPG
#include        "epgdec.h"
#endif

class	QSettings;
class	deviceHandler;
class	audioBase;
class	common_fft;
class	serviceDescriptor;
class	historyHandler;

#include	"ui_technical_data.h"

/*
 *	The main gui object. It inherits from
 *	QWidget and the generated form
 */

class dabService {
public:
	QString		serviceName;
	uint32_t	SId;
	int		SCIds;
	bool		valid;
};
	
class RadioInterface: public QWidget, private Ui_dabradio {
Q_OBJECT
public:
		RadioInterface		(QSettings	*,
	                                 const QString	&,
	                                 const QString	&,
	                                 int32_t	 dataPort,
	                                 QWidget	*parent = nullptr);
		~RadioInterface		();

protected:
	bool			eventFilter (QObject *obj, QEvent *event);
private:
	RingBuffer<std::complex<float>>  spectrumBuffer;
	RingBuffer<std::complex<float>>  iqBuffer;
	RingBuffer<std::complex<float>>  tiiBuffer;
	RingBuffer<float>	responseBuffer;
	RingBuffer<uint8_t>	frameBuffer;
	RingBuffer<uint8_t>	dataBuffer;
	RingBuffer<int16_t>	audioBuffer;
        spectrumViewer		my_spectrumViewer;
	correlationViewer	my_correlationViewer;
	tiiViewer		my_tiiViewer;
	presetHandler		my_presetHandler;
	processParams		globals;
	QString			version;
	int			serviceOrder;
	bandHandler		theBand;
	scannerTable		theTable;
	Ui_technical_data	techData;
	QFrame			*dataDisplay;
	QSettings		*dabSettings;
	dabService		currentService;
	dabService		nextService;

	bool			normalScan;
	int16_t			tii_delay;
	int32_t			dataPort;
	serviceDescriptor	*currentServiceDescriptor;
	QLabel			*motSlides;
	std::vector<int>	secondariesVector;
	bool			isSynced;
	std::atomic<bool>	running;
	std::atomic<bool>	scanning;
	deviceHandler		*inputDevice;
	textMapper		the_textMapper;
	dabProcessor		*my_dabProcessor;
	audioBase		*soundOut;
#ifdef	DATA_STREAMER
	tcpServer		*dataStreamer;
#endif
#ifdef	TRY_EPG
	CEPGDecoder		epgHandler;
#endif
	bool			saveSlides;
	QString			picturesPath;
	QString			epgPath;
	QString			filePath;
	int			switchTime;
#ifdef	_SEND_DATAGRAM_
	QUdpSocket		dataOut_socket;
	QString			ipAddress;
	int32_t			port;
#endif
        FILE                    *frameDumper;
        SNDFILE                 *audioDumper;

	QStandardItemModel	model;
	std::vector<serviceId>	serviceList;
	bool			isMember (std::vector<serviceId>,
                                               serviceId);
	std::vector<serviceId>	insert   (std::vector<serviceId>,
                                                      serviceId, int);
	QStringList		soundChannels;
	QTimer			displayTimer;
	QTimer			signalTimer;
	QTimer			presetTimer;
	QTimer			startTimer;
	int32_t			numberofSeconds;
	int16_t			ficBlocks;
	int16_t			ficSuccess;
	void			connectGUI		();
	void			disconnectGUI		();

	QString			footText		();
	void			cleanScreen		();
	void			set_picturePath		();
	void			dumpControlState	(QSettings *);
	void			hideButtons		();
	void			showButtons		();
	deviceHandler		*setDevice		(const QString &);
	historyHandler		*my_history;
//
	void			start_audioService	(const QString &);
	void			start_packetService	(const QString &);
	void			startScanning		();
	void			stopScanning		(bool);
        void			start_audioDumping      ();
        void			stop_audioDumping       ();
        void			start_frameDumping      ();
        void			stop_frameDumping       ();
	void			startChannel		(const QString &);
	void			stopChannel		();
	void			stopService		();
	void			startService		(dabService *);
	void			colorService		(QModelIndex ind,
	                                                   QColor c, int pt);
	void			localSelect		(const QString &s);
	QString			filenameSuggestion 	(QString);
	void			showServices		();
	bool			doStart			();
	void			save_MOTtext		(QByteArray, int,
	                                                 QString);
        void                    show_MOTlabel           (QByteArray, int,
                                                                  QString);
public slots:
	void			set_CorrectorDisplay	(int);
	void			addtoEnsemble		(const QString &, int);
	void			nameofEnsemble		(int, const QString &);
	void			show_frameErrors	(int);
	void			show_rsErrors		(int);
	void			show_aacErrors		(int);
	void			show_ficSuccess		(bool);
	void			show_snr		(float);
	void			setSynced		(bool);
	void			showLabel		(QString);
	void			handle_motObject	(QByteArray, QString,
	                                                  int, bool);
	void			sendDatagram		(int);
	void			handle_tdcdata		(int, int);
	void			changeinConfiguration	();
	void			newAudio		(int, int);
//
	void			setStereo		(int);
	void			set_streamSelector	(int);
	void			No_Signal_Found		();
	void			show_motHandling	(bool);
	void			setSyncLost		();
	void			showCorrelation		(int, int);
	void			showIndex		(int);
	void			showSpectrum		(int);
	void			showIQ			(int);
	void			showQuality		(float);
	void			show_tii		(QByteArray);
	void			closeEvent		(QCloseEvent *event);
	void			showTime		(const QString &);
	void			startAnnouncement	(const QString &, int);
	void			stopAnnouncement	(const QString &, int);
	void			newFrame		(int);
//	Somehow, these must be connected to the GUI
private slots:
	void			handle_scanButton	();
	void			handle_audiodumpButton 	();
        void			handle_framedumpButton	();
        void			handle_rawdumpButton	();
	void			handle_nextChannelButton();
	void			handle_prevChannelButton();
	void			handle_prevServiceButton	();
        void			handle_nextServiceButton	();
	void			handle_tiiButton	();
	void			handle_correlationButton	();
	void			handle_spectrumButton	();
	void			handle_presetSelector	(const QString &);
	void			handle_historySelect	(const QString &);
	void			handle_contentButton	();
	void			handle_detailButton	();
	void			handle_devicewidgetButton	();

	void			doStart			(const QString &);
	void			newDevice		(const QString &);

	void			handle_resetButton	();
	void			handle_historyButton	();
	void			TerminateProcess	();
	void			updateTimeDisplay	();
	void			signalTimer_out		();
	void			selectChannel		(const QString &);
	void			selectService		(QModelIndex);
	void			setPresetStation	();
};
#endif

