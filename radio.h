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
#include	<QStringListModel>
#include	<QStandardItemModel>
#ifdef	_SEND_DATAGRAM_
#include	<QUdpSocket>
#endif
#include	<QComboBox>
#include	<QLabel>
#include	<QTimer>
#include	<QByteArray>
#include	<sndfile.h>
#include	"ui_dabradio.h"
#include	"dab-processor.h"
#include	"ringbuffer.h"
#include        "band-handler.h"
#include	"text-mapper.h"
#ifdef	DATA_STREAMER
#include	"tcp-server.h"
#endif
#include	"preset-handler.h"
class	QSettings;
class	virtualInput;
class	audioBase;
class	common_fft;
class	serviceDescriptor;
#include	"ui_technical_data.h"

class	spectrumViewer;
class	correlationViewer;
class	tiiViewer;
/*
 *	GThe main gui object. It inherits from
 *	QWidget and the generated form
 */
class RadioInterface: public QWidget, private Ui_dabradio {
Q_OBJECT
public:
		RadioInterface		(QSettings	*,
	                                 QString	,
	                                 int32_t	 dataPort,
	                                 QWidget	*parent = nullptr);
		~RadioInterface();

protected:
	bool			eventFilter (QObject *obj, QEvent *event);
private:
	presetHandler		my_presetHandler;
	QString			footText		();
	int			switchTime;
	QSettings		*dabSettings;
	Ui_technical_data	techData;
	QFrame			*dataDisplay;
	serviceDescriptor	*currentService;
	void			set_picturePath		();
	void			dumpControlState	(QSettings *);
	void			Yes_Signal_Found	();
	void			hideButtons		();
	void			showButtons		();
	virtualInput		*setDevice		(QString);

	void			connectGUI		();
	void			disconnectGUI		();
	RingBuffer<uint8_t>	*frameBuffer;
	RingBuffer<int16_t>	*audioBuffer;
	RingBuffer<uint8_t>	*dataBuffer;
        spectrumViewer		*my_spectrumViewer;
	RingBuffer<DSPCOMPLEX>  *spectrumBuffer;
	RingBuffer<std::complex<float>>  *iqBuffer;
	correlationViewer	*my_correlationViewer;
	RingBuffer<float>	*responseBuffer;
	tiiViewer		*my_tiiViewer;
	RingBuffer<DSPCOMPLEX>  *tiiBuffer;

	std::vector<QString>	activeServices;
	std::vector<int>	secondariesVector;
	bool			isSynced;
	bandHandler		theBand;
	std::atomic<bool>	running;
	std::atomic<bool>	scanning;
	virtualInput		*inputDevice;
	textMapper		the_textMapper;
	dabProcessor		*my_dabProcessor;
	audioBase		*soundOut;
#ifdef	DATA_STREAMER
	tcpServer		*dataStreamer;
#endif
	bool			saveSlides;
#ifdef	_SEND_DATAGRAM_
	QUdpSocket		dataOut_socket;
	QString			ipAddress;
	int32_t			port;
#endif
	SNDFILE			*rawDumper;
	FILE			*frameDumper;
	SNDFILE			*audioDumper;

	QStringList		soundChannels;
	QStandardItemModel	model;
	QStringList		Services;
	QTimer			displayTimer;
	QTimer			signalTimer;
	QTimer			presetTimer;
//	QTimer			startTimer;
	QString			presetName;
	bool			has_presetName;
	int32_t			numberofSeconds;
	int16_t			ficBlocks;
	int16_t			ficSuccess;
	QString			picturesPath;
	bool			channelActive		();
public slots:
	void			set_Scanning		();
	void			startScanning		();
	void			stopScanning		();
	void			set_CorrectorDisplay	(int);
	void			addtoEnsemble		(const QString &);
	void			nameofEnsemble		(int, const QString &);
	void			show_frameErrors	(int);
	void			show_rsErrors		(int);
	void			show_aacErrors		(int);
	void			show_ficSuccess		(bool);
	void			show_snr		(int);
	void			setSynced		(bool);
	void			showLabel		(QString);
	void			showMOT			(QByteArray,
	                                                 int, QString);
	void			sendDatagram		(int);
	void			handle_tdcdata		(int, int);
	void			changeinConfiguration();
	void			newAudio		(int, int);
//
	void			show_techData		(QString, audiodata *);
	void			setStereo		(bool);
	void			set_streamSelector	(int);
	void			No_Signal_Found();
	void			show_motHandling	(bool);
	void			setSyncLost();
	void			showCoordinates		(int);
//	void			showSecondaries		(int);
	void			showSecondaries		(QByteArray);
	void			showCorrelation		(int);
	void			showIndex		(int);
	void			showSpectrum		(int);
	void			showIQ			(int);
	void			showQuality		(float);
	void			show_tii		(int);
	void			closeEvent		(QCloseEvent *event);
	void			showTime		(const QString &);
	void			startAnnouncement	(const QString &, int);
	void			stopAnnouncement	(const QString &, int);
	void			newFrame		(int);
//	Somehow, these must be connected to the GUI
private slots:
	void			toggle_show_data	();
	void			doStart			(QString);
	void			doStart			();
	void			newDevice		(QString);
	void			TerminateProcess	();
	void			updateTimeDisplay	();
	void			handleReset		();

	void			set_audioDump		();
	void			start_audioDumping	();
	void			stop_audioDumping	();
	void			set_sourceDump		();
	void			start_sourceDumping	();
	void			stop_sourceDumping	();
	void			set_frameDump		();
	void			start_frameDumping	();
	void			stop_frameDumping	();
	void			showEnsembleData	();
	void			set_tiiSwitch		();
	void			set_correlationSwitch	();
	void			set_spectrumSwitch	();
	void			handle_showDeviceWidget	();
	void			handle_PresetSelector	(const QString &);

	void			selectService		(QModelIndex);
//	void			selectService		(QString);
	void			handle_setprevious	();
	void			handle_setnext		();
	void			startService		(const QString &);
	void			colorService		(QModelIndex, QColor, int);
	void			start_audioService	(const QString &);
	void			start_packetService	(const QString &);
	void			cleanScreen		();
	void			stopService		();
	void			stop_subServices	(const QString &);
	void			setPresetStation	();
	void			set_nextChannel		();
	void			set_prevChannel		();
	void			selectChannel		(const QString &);
	void			startChannel		(const QString &);
	void			stopChannel		();
};
#endif

