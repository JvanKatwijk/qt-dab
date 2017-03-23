#
/*
 *    Copyright (C) 2013, 2014, 2015, 2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the Qt-DAB (formerly SDR-J, JSDR).
 *    Many of the ideas as implemented in Qt-DAB are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are acknowledged.
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

#ifndef __RADIO
#define __RADIO

#include	"dab-constants.h"
#include	<QMainWindow>
#include	<QStringList>
#include	<QStringListModel>
#include	<QUdpSocket>
#include	<QComboBox>
#include	<QLabel>
#include	<QTimer>
#include	<sndfile.h>
#include	"ui_dabradio.h"
#include	"fic-handler.h"
#include	"ofdm-processor.h"
#include	"ringbuffer.h"

class	QSettings;
class	virtualInput;
class	audioBase;
class	mscHandler;
class	common_fft;
class	dabParams;

#ifdef	TCP_STREAMER
class	tcpStreamer;
#endif

#ifdef	TECHNICAL_DATA
#include	"ui_technical_data.h"
#endif

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
	                                 uint8_t	freqsyncMethod,
	                                 bool		tracing,
	                                 QWidget *parent = NULL);
		~RadioInterface		();

private:
#ifdef	TECHNICAL_DATA
	Ui_technical_data	techData;
	QFrame		*dataDisplay;
	bool		show_data;
private slots:
	void		toggle_show_data	(void);
private:
#endif
	QSettings	*dabSettings;
	uint8_t		freqsyncMethod;
	bool		tracing;
	bool		autoStart;
	int16_t		threshold;
	void		setupChannels		(QComboBox *s, uint8_t band);
	void		setModeParameters	(uint8_t);
	void		clear_showElements	(void);
	dabParams	*dabModeParameters;
	uint8_t		isSynced;
	uint8_t		dabBand;
	bool		running;
	bool		scanning;

	virtualInput	*inputDevice;
	int32_t		tunedFrequency;

	ofdmProcessor	*my_ofdmProcessor;
	ficHandler	my_ficHandler;
	mscHandler	*my_mscHandler;
	audioBase	*soundOut;
	RingBuffer<int16_t>	*audioBuffer;
	bool		autoCorrector;
const	char		*get_programm_type_string (int16_t);
const	char		*get_programm_language_string (int16_t);
	QLabel		*pictureLabel;
	bool		saveSlide;
	QUdpSocket	DSCTy_59_socket;
	QString		ipAddress;
	int32_t		port;
	bool		show_crcErrors;
	void		init_your_gui		(void);
	void		dumpControlState	(QSettings *);
	FILE		*crcErrors_File;
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
	int32_t		numberofSeconds;
	void		resetSelector		(void);
	int16_t		ficBlocks;
	int16_t		ficSuccess;
	void		Yes_Signal_Found	(void);
	void		Increment_Channel	(void);
#ifdef	HAVE_SPECTRUM
        spectrumhandler         *spectrumHandler;
	RingBuffer<DSPCOMPLEX>  *spectrumBuffer;
	RingBuffer<DSPCOMPLEX>	*iqBuffer;
#endif


public slots:
	void		set_Scanning		(void);
	void		set_fineCorrectorDisplay	(int);
	void		set_coarseCorrectorDisplay	(int);
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
	void		sendDatagram		(char *, int);
	void		changeinConfiguration	(void);
	void		newAudio		(int);
//
	void		show_mscErrors		(int);
	void		show_ipErrors		(int);
	void		setStereo		(bool);
	void		set_streamSelector	(int);
	void		No_Signal_Found		(void);

#ifdef	HAVE_SPECTRUM
	void		showSpectrum		(int);
	void		showIQ			(int);
#ifdef	__QUALITY
	void		showQuality		(float);
#endif
#endif
private slots:
//
//	Somehow, these must be connected to the GUI
	void	setStart		(void);
	void	TerminateProcess	(void);
	void	set_channelSelect	(QString);
	void	updateTimeDisplay	(void);
	void	signalTimer_out		(void);

	void	autoCorrector_on	(void);

	void	set_modeSelect		(const QString &);
	void	set_bandSelect		(QString);
	void	setDevice		(QString);
	void	selectService		(QModelIndex);
	void	set_dumping		(void);
	void	set_audioDump		(void);
};
#endif

