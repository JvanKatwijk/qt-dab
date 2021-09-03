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

#ifndef __SIMPLE_RADIO__
#define __SIMPLE_RADIO__

#include	"dab-constants.h"
#include	<QMainWindow>
#include	<QStringList>
#include	<QStandardItemModel>
#include	<QComboBox>
#include	<QByteArray>
#include	<QLabel>
#include	<QTimer>
#include	<sndfile.h>
#include	"ui_dab-mini.h"
#include	"dab-processor.h"
#include	"ringbuffer.h"
#include        "band-handler.h"
#include	"text-mapper.h"
#include	"process-params.h"
#include	"dl-cache.h"
#include	"preset-handler.h"

#include	"findfilenames.h"
#include	"scheduler.h"

class	QSettings;
class	virtualInput;
class	audioBase;
class	common_fft;

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

class RadioInterface: public QWidget, private Ui_dab_mini {
Q_OBJECT
public:
		RadioInterface		(QSettings	*,
	                                 const QString	&,
	                                 QWidget	*parent = nullptr);
		~RadioInterface		();

protected:
	bool			eventFilter (QObject *obj, QEvent *event);
private:
	FILE			*dlTextFile;
	RingBuffer<std::complex<float>>  spectrumBuffer;
        RingBuffer<std::complex<float>>  iqBuffer;
        RingBuffer<std::complex<float>>  tiiBuffer;
        RingBuffer<float>		snrBuffer;
        RingBuffer<float>       responseBuffer;
        RingBuffer<uint8_t>     frameBuffer;
        RingBuffer<uint8_t>     dataBuffer;
        RingBuffer<int16_t>     audioBuffer;

	void			start_secondService	(const QString &);
	void			stop_secondService	();
	FILE			*frameDumper;
	processParams		globals;
	int			serviceOrder;
	QString			version;
	presetHandler		my_presetHandler;
	bandHandler		theBand;
	QSettings		*dabSettings;
//	std::vector<dabService>	runningServices;
	dlCache                 the_dlCache;
        findfileNames           filenameFinder;
	Scheduler		theScheduler;

	int16_t			tii_delay;
	int32_t			dataPort;
	bool			isSynced;
	bool			stereoSetting;
	std::atomic<bool>	running;
	std::atomic<bool>	scanning;
	deviceHandler		*inputDevice;
	deviceHandler		*findDevice ();
	textMapper		the_textMapper;
	dabProcessor		*my_dabProcessor;
	audioBase		*soundOut;
	QString			picturesPath;
	int			switchTime;

	QStandardItemModel	model;
	std::vector<serviceId>	serviceList;
	bool			isMember (std::vector<serviceId>,
	                                       serviceId);
	std::vector<serviceId>
	  	                insert   (std::vector<serviceId>,
	                                  serviceId, int);
	QStringList		soundChannels;
	QTimer			displayTimer;
	QTimer			signalTimer;
	QTimer			presetTimer;
	QTimer			startTimer;
	QTimer			muteTimer;
	int32_t			numberofSeconds;
	int32_t			muteDelay;
	bool			muting;
	int16_t			ficBlocks;
	int16_t			ficSuccess;
	int			total_ficError;
	int			total_fics;
	QString			footText		();
	void			cleanScreen		();
	void			dumpControlState	(QSettings *);
	deviceHandler		*setDevice		(const QString &);
//
	void			start_audioService	(const QString &);
	void			startChannel		(const QString &);
	void			stopChannel		();
	void			stopService		();
	void			startService		(dabService *);
	void			colorService		(QModelIndex ind,
	                                                   QColor c, int pt);
	void			localSelect		(const QString &s);
	void			scheduleSelect		(const QString &);
	void			localSelect		(const QString &,
	                                                 const QString &);
	QString			filenameSuggestion 	(QString);
	bool			doStart			();
	void			hide_for_safety		();
	void			show_for_safety		();
	void			scheduled_dlTextDumping	();

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
	void			showIndex		(int);
	void			showSpectrum		(int);
	void			showIQ			(int);
	void			showQuality		(float);
	void			show_rsCorrections	(int);
	void			show_tii		(int, int);
	void			show_tii_spectrum	();
	void			show_snr		(int, float, float);
	void			closeEvent		(QCloseEvent *event);
	void			clockTime		(int, int, int,
	                                                 int, int,
	                                                 int, int, int, int);
//	void			showTime		(const QString &);
	void			startAnnouncement	(const QString &, int);
	void			stopAnnouncement	(const QString &, int);
	void			newFrame		(int);
	void			show_clockError		(int);
	void			scheduler_timeOut	(const QString &);

//	Somehow, these must be connected to the GUI
private slots:
	void			handle_nextChannelButton();
	void			handle_prevChannelButton();
	void			handle_prevServiceButton	();
        void			handle_nextServiceButton	();
	void			handle_presetSelector	(const QString &);
	void			TerminateProcess	();
	void			updateTimeDisplay	();
	void			signalTimer_out		();
	void			selectChannel		(const QString &);
	void			selectService		(QModelIndex);
	void			setPresetStation	();
	void			handle_muteButton	();
	void			handle_scheduleButton	();
	void			handle_dlTextButton	();
};
#endif

