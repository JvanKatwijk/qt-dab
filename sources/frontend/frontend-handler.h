#
/*
 *    Copyright (C) 2024
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
#
#pragma once
/*
 *	Based on user requests (Peter L.G) a secondary frontend
 *	path for handling  eti files for input is added.
 *	The original ofdm handler is - together with an (almost) copy
 *	of the front end handling of the eti backend - made into a
 *	derived version of a "frontend-handler"
 *	Note that the eti frontend hamdling here is much, much simpler and
 *	GUI buttons  and elements related to raw input signals are 
 *	superfluous here
 */
#include	"dab-constants.h"
#include	<QThread>
#include	<QObject>
#include	<QByteArray>
#include	<QStringList>
#include	<QSettings>
#include	<vector>
#include	<cstdint>
#include	"dab-constants.h"
#include	"ringbuffer.h"

class	RadioInterface;
class	processParams;
class	logger;

//	Lots of functions only give results when having selected the
//	regular processing path
class frontendHandler: public QThread {
Q_OBJECT
public:
		frontendHandler 		();
virtual		~frontendHandler		();
//
//	Functions for both processing paths
virtual	void		start			();
virtual	void		stop			();

virtual	bool		isPrimaryService	(uint32_t, uint8_t);
virtual	bool		isAudioService		(uint32_t, uint8_t);
virtual	bool		isPacketService		(uint32_t, uint8_t);
virtual	bool		is_SPI			(uint32_t);
virtual	std::vector<uint8_t>
	                get_secondaryServices		(uint32_t SId);
virtual	int		FIG07_value		();
virtual	void		getFreqs		(uint32_t,
	                                              std::vector<uint32_t> &);
virtual	void		getServiceName		(QString &, QString &,
	                                         uint32_t, uint8_t);
virtual	void		mapNameToId		(const QString &,
	                                                uint32_t &, uint8_t &);
virtual void		audioData		(uint32_t, uint8_t,
	                                                   audiodata &);
virtual void		packetData		(uint32_t, uint8_t,
	                                                   packetdata &);
virtual	uint32_t	julianDate		();
virtual	QList<contentType>
	                contentPrint		();
virtual	void		set_dataTracer		(bool);
virtual	bool		serviceRuns		(uint32_t SId, uint8_t SCIds);
virtual	void		stopService		(uint32_t, uint8_t);
virtual	bool		setAudioChannel		(audiodata &,
	                                         RingBuffer<std::complex<int16_t>> *,
	                                         FILE *, int);
virtual	bool		setDataChannel		(packetdata &,
	                                         RingBuffer<uint8_t> *, int);
virtual	std::vector<basicService>
	                getServices		();
	      
//
//	Functions only useful in the regular processing path
virtual	void		startDumping		(const QString &, int, 
	                                         int, const QString);
virtual	void		stopDumping		();
virtual	bool		isDumping		();
virtual	bool		startEtiGenerator	(const QString &);
virtual	void		stopEtiGenerator	();
virtual	void		resetEtiGenerator	();
virtual	void		setScanMode		(bool);
virtual	void		viewCarriers		(uint8_t);
virtual	void		setTIIThreshold		(int16_t);
virtual	void		setTIICollisions	(int);
virtual	int		freeSpace		();
virtual	void		startFicDump		(const QString &);
virtual	bool		ficDumping_on		();
virtual	void		stopFicDump		();
//virtual	void	setSpeedUp		(bool);
virtual	void		setFreqCorrelator	(uint8_t);
//	for the mscHandler
virtual	void		handleIQSelector	();
virtual	void		handleDecoderSelector	(int);
virtual	void		setCorrelationOrder	(const QString &);
virtual	void		setDXMode		(bool);
virtual	void		set_dcRemoval		(bool);
virtual	void		set_iqBalance		(bool);

signals:
//
//	for both processing paths
	void            setSynced               (bool);
        void            noSignalFound           ();
        void            setSyncLost             ();
//
//	for rthe regular processing path
	void            showTIIData             (QVector<tiiData>, int);
        void            showTIIspectrum         ();
        void            showSpectrum            (int);
        void            showSnr                 (float);
        void            showSnr                 (float, float, float,
                                                          float, float);
        void            showClockError          (int);
        void            showNull                (int, int); 
        void            showChannel             (int);
        void            showCorrector           (int, float);
 
        void            show_carriers           (int, int);
};

