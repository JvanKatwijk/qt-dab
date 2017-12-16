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
 *
 *	Main program
 */

#ifndef __RADIO__
#define __RADIO__

#include	"dab-constants.h"
#include	<QMainWindow>
#include	<QStringList>
#include	<QStringListModel>
#include	<QUdpSocket>
#include	<QTcpSocket>
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

#include	"ui_technical_data.h"

#ifdef	HAVE_SPECTRUM
class	spectrumhandler;
#endif

/*
 *	GThe main gui object. It inherits from
 *	QDialog and the generated form
 */
class RadioInterface: public QMainWindow,
		      private Ui_dabradio {
Q_OBJECT
public:
		RadioInterface		(QSettings	*,
	                                 int32_t	 dataPort,
	                                 QWidget	*parent = NULL);
		~RadioInterface		(void);

private:
	QSettings	*dabSettings;
	int16_t		tii_delay;
	int32_t         dataPort;
	QString         deviceName;
	uint8_t		dabMode;
	Ui_technical_data	techData;
	QFrame		*dataDisplay;
	bool		show_data;
private:
	void		clear_showElements	(void);
	void		set_picturePath		(void);
const	char		*get_programm_type_string (int16_t);
const	char		*get_programm_language_string (int16_t);
	void		dumpControlState	(QSettings *);
	void		Yes_Signal_Found	(void);
	void		Increment_Channel	(void);
	uint8_t		convert			(QString);
	void		hideButtons		(void);
	void		showButtons		(void);
	virtualInput	*setDevice		(QString);

	uint8_t		dabBand;
	bool		thereisSound;
	uint8_t		isSynced;
	int16_t		threshold;
	int16_t		diff_length;
	bandHandler     theBand;
	std::atomic<bool>	running;
	bool		scanning;
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
	QUdpSocket	dataOut_socket;
	QString		ipAddress;
	int32_t		port;

	void		start_sourceDumping	(void);
	void		stop_sourceDumping	(void);
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
#ifdef	HAVE_SPECTRUM
        spectrumhandler         *spectrumHandler;
	RingBuffer<std::complex<float>>  *spectrumBuffer;
	RingBuffer<std::complex<float>>	*iqBuffer;
#endif

	QString		picturesPath;
public slots:
	void		set_Scanning		(void);
	void		set_CorrectorDisplay	(int);
	void		clearEnsemble		(void);
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
	void		changeinConfiguration	(void);
	void		newAudio		(int, int);
//
	void		setStereo		(bool);
	void		set_streamSelector	(int);
	void		No_Signal_Found		(void);
	void		show_motHandling	(bool);
	void		setSyncLost		(void);
	void		showCoordinates		(float, float);
	void		show_tiiLabel		(int);

#ifdef	HAVE_SPECTRUM
	void		showSpectrum		(int);
	void		showIQ			(int);
#ifdef	__QUALITY
	void		showQuality		(float);
#endif
#endif
//
//	Somehow, these must be connected to the GUI
private slots:
	void		set_nextChannel		(void);
	void		toggle_show_data	(void);
	void		doStart			(QString);
	void		doStart			(void);
	void		TerminateProcess	(void);
	void		selectChannel		(QString);
	void		updateTimeDisplay	(void);
	void		signalTimer_out		(void);
	void		autoCorrector_on	(void);

	void		newDevice		(QString);
	void		set_modeSelect		(const QString &);
	void		set_bandSelect		(QString s);

	void		selectService		(QModelIndex);
	void		selectService		(QString);
	void		set_audioDump		(void);
	void		set_sourceDump		(void);
	void		showEnsembleData	(void);
	void		setPresetStation	(void);
};
#endif

