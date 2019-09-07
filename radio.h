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
#ifdef	_SEND_DATAGRAM_
#include	<QUdpSocket>
#endif
#include	<QComboBox>
#include	<QLabel>
#include	<QTimer>
#include	<sndfile.h>
#include	"ui_dabradio.h"
#include	"dab-processor.h"
#include	"ringbuffer.h"
#include        "band-handler.h"
#include	"text-mapper.h"
#ifdef	DATA_STREAMER
#include	"tcp-server.h"
#endif
class	QSettings;
class	virtualInput;
class	audioBase;
class	common_fft;
class	serviceDescriptor;
#include	"ui_technical_data.h"

class	spectrumViewer;
class	impulseViewer;
class	tiiViewer;
/*
 *	GThe main gui object. It inherits from
 *	QDialog and the generated form
 */
class RadioInterface: public QWidget, private Ui_dabradio {
Q_OBJECT
public:
		RadioInterface		(QSettings	*,
	                                 int32_t	 dataPort,
	                                 QWidget	*parent = nullptr);
		~RadioInterface();

protected:
	bool	eventFilter (QObject *obj, QEvent *event);
private:
	QSettings	*dabSettings;
	int16_t		tii_delay;
	int32_t         dataPort;
	bool		ensembleAvailable;
	QString         deviceName;
	Ui_technical_data	techData;
	QFrame		*dataDisplay;
	bool		show_data;
	serviceDescriptor	*currentService;
	void		clear_showElements();
	void		set_picturePath();
//const	char		*get_programm_type_string (int16_t);
//const	char		*get_programm_language_string (int16_t);
	void		dumpControlState	(QSettings *);
	void		Yes_Signal_Found();
	void		Increment_Channel();
	uint8_t		convert			(QString);
	void		hideButtons();
	void		showButtons();
	virtualInput	*setDevice		(QString);

	std::vector<int> secondariesVector;
	QString		dabMode;
	uint8_t		dabBand;
	uint8_t		isSynced;
	int16_t		threshold;
	int16_t		diff_length;
	bandHandler     theBand;
	std::atomic<bool>	running;
	bool		scanning;
	bool		tiiSwitch;
	virtualInput	*inputDevice;
	textMapper	the_textMapper;
	dabProcessor	*my_dabProcessor;
	audioBase	*soundOut;
#ifdef	DATA_STREAMER
	tcpServer	*dataStreamer;
#endif
	RingBuffer<int16_t>	*audioBuffer;
	RingBuffer<uint8_t>	*dataBuffer;
	bool		autoCorrector;
	QLabel		*pictureLabel;
	bool		saveSlides;
	bool		showSlides;
#ifdef	_SEND_DATAGRAM_
	QUdpSocket	dataOut_socket;
	QString		ipAddress;
	int32_t		port;
#endif

	void		start_sourceDumping();
	void		stop_sourceDumping();
	bool		sourceDumping;
	SNDFILE		*dumpfilePointer;
	bool		audioDumping;
	SNDFILE		*audiofilePointer;
	QStringList	soundChannels;
	QStringListModel	ensemble;
	QStringList	Services;
	QString		ensembleLabel;
	QTimer		displayTimer;
	QTimer		signalTimer;
	QTimer		presetTimer;
	QTimer		startTimer;
	QString		presetName;
	QString		currentName;
	bool		has_presetName;
	int32_t		numberofSeconds;
	int16_t		ficBlocks;
	int16_t		ficSuccess;
	void		connectGUI();
	void		disconnectGUI();
        spectrumViewer         *my_spectrumViewer;
	RingBuffer<std::complex<float>>  *spectrumBuffer;
	RingBuffer<std::complex<float>>  *iqBuffer;
	impulseViewer		*my_impulseViewer;
	RingBuffer<float>	*responseBuffer;
	tiiViewer		*my_tiiViewer;
	RingBuffer<std::complex<float>>  *tiiBuffer;

	QString		picturesPath;
public slots:
	void		set_Scanning();
	void		set_CorrectorDisplay	(int);
	void		clearEnsemble();
	void		addtoEnsemble		(const QString &);
	void		nameofEnsemble		(int, const QString &);
	void		show_frameErrors	(int);
	void		show_rsErrors		(int);
	void		show_aacErrors		(int);
	void		show_ficSuccess		(bool);
	void		show_snr		(int);
	void		setSynced		(char);
	void		showLabel		(QString);
	void		showMOT			(QByteArray, int, QString);
	void		sendDatagram		(int);
	void		handle_tdcdata		(int, int);
	void		changeinConfiguration();
	void		newAudio		(int, int);
//
	void		show_techData		(QString,
	                                         QString,
	                                         float,
	                                         audiodata *);

	void		setStereo		(bool);
	void		set_streamSelector	(int);
	void		No_Signal_Found();
	void		show_motHandling	(bool);
	void		setSyncLost();
	void		showCoordinates		(int);
	void		showSecondaries		(int);
	void		showImpulse		(int);
	void		showIndex		(int);
	void		showSpectrum		(int);
	void		showIQ			(int);
	void		showQuality		(float);
	void		show_tii		(int);
	void		closeEvent		(QCloseEvent *event);
	void		showTime		(const QString &);
	void		startAnnouncement	(const QString &, int);
	void		stopAnnouncement	(const QString &, int);
//	Somehow, these must be connected to the GUI
private slots:
	void		set_nextChannel();
	void		toggle_show_data();
	void		doStart			(QString);
	void		doStart();
	void		TerminateProcess();
	void		selectChannel		(QString);
	void		updateTimeDisplay();
	void		signalTimer_out();
	void		autoCorrector_on();

	void		newDevice		(QString);

	void		selectService		(QModelIndex);
	void		selectService		(QString);
	void		set_audioDump();
	void		set_sourceDump();
	void		showEnsembleData();
	void		setPresetStation();
	void		set_tiiSwitch();
	void		set_irSwitch();
	void		set_spectrumSwitch();
};
#endif

