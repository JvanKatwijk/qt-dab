#
/*
 *    Copyright (C) 2015 .. 2025
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
 *	the ofdmHandler is the embodying of all functionality related
 *	to the ofdm processing and preparation for further decoding
 */
#include	"dab-constants.h"
#include	<QThread>
#include	<QObject>
#include	<QByteArray>
#include	<QStringList>
#include	<QSettings>
#include	<vector>
#include	<cstdint>
#include	"sample-reader.h"
#include	"fic-handler.h"
#include	"msc-handler.h"
#include	"ofdm-decoder.h"
#include	"device-handler.h"
#include	"ringbuffer.h"
#include	"eti-generator.h"

class	RadioInterface;
class	dabParams;
class	processParams;
class	logger;

class ofdmHandler: public QThread {
Q_OBJECT
public:
		ofdmHandler  	(RadioInterface *,
	                         deviceHandler *,
	                         processParams *,
	                         QSettings	*,
	                         logger		*,
	                         uint8_t);
		~ofdmHandler			();
	void		start			();
//	void		start			(int32_t);
	void		stop			();

	void		selectTII		(uint8_t);
	void		startDumping		(const QString &, int, 
	                                         int, const QString);
	void		stopDumping		();
	bool		startEtiGenerator	(const QString &);
	void		stopEtiGenerator	();
	void		resetEtiGenerator	();
	void		setScanMode		(bool);
	void		getFrameQuality		(int *, int*, int *);
//
//	just a gadget
	bool		serviceRuns		(uint32_t SId, uint16_t SCIds);

//
//	for the tii settings
	void		setTIIThreshold		(int16_t);
	void		setTIICollisions	(bool);
	void		setTIIFilter		(bool);
//	servicing our subordinates
//	for the ficHandler:
	int		getServiceComp		(const QString &);
	int		getServiceComp		(uint32_t, int);
//	int		getServiceComp_SCIds	(uint32_t SId, int SCIds);
	bool		isPrimary		(const QString &);

	uint16_t	getAnnouncing		(uint16_t);
	uint32_t	getSId			(int);
	uint8_t		serviceType		(int);
        void		audioData		(int, audiodata &);
        void		packetData		(int, packetdata &);
	int		getNrComps		(uint32_t);
	uint32_t	julianDate		();
	std::vector<int>	getFrequency	(const QString &);
	QList<contentType>	contentPrint	();
	int		freeSpace		();
	void		startFicDump		(const QString &);
	bool		ficDumping_on		();
	void		stopFicDump		();

	void		setSpeedUp		(bool);
	void		setFreqCorrelator	(uint8_t);
//	for the mscHandler
//	void		resetServices		();
//	void		stopService		(descriptorType *, int);
	void		stopService		(const QString &,
	                                         int, int);
	bool		setAudioChannel		(audiodata &,
	                                         RingBuffer<std::complex<int16_t>> *,
	                                         FILE *, int);
	bool		setDataChannel		(packetdata &,
	                                         RingBuffer<uint8_t> *, int);
	void		handleIQSelector	();
	void		handleDecoderSelector	(int);
	void		setCorrelationOrder	(bool);
	void		setDXMode		(bool);
	void		set_dcRemoval		(bool);
private:
	RadioInterface		*radioInterface_p;
	processParams		*p;
	dabParams		params;
	QSettings		*settings_p;
	logger			*theLogger;
	uint8_t			cpuSupport;
	sampleReader		theReader;
	ficHandler		theFicHandler;
	etiGenerator		theEtiGenerator;
	ofdmDecoder		theOfdmDecoder;
	mscHandler		theMscHandler;
	phaseTable		theTable;


	uint8_t			selectedTII;
	DABFLOAT		snr;
	int16_t			tiiThreshold;
	bool			tiiCollisions_active;
	bool			tiiFilter_active;

	int			decoder;
	int			thresHold;
	int			totalFrames;
	int			goodFrames;
	int			badFrames;
	bool			tiiSwitch;
	int16_t			tiiDepth;
	int16_t			echoEepth;
	RingBuffer<Complex >	*tiiBuffer_p;
	RingBuffer<Complex >	*nullBuffer_p;
	RingBuffer<float>	*snrBuffer_p;
	RingBuffer<Complex>	*channelBuffer_p;
	int16_t			tiiDelay;
	int16_t			tiiCounter;
	bool			etiOn;
	int16_t			attempts;
	bool			scanMode;
	int32_t			T_null;
	int32_t			T_u;
	int32_t			T_s;
	int32_t			T_g;
	int32_t			T_F;
	int32_t			nrBlocks;
	int32_t			carriers;
	int32_t			carrierDiff;
	int16_t			fineOffset;
	int32_t			coarseOffset;
//	QByteArray		transmitters;
	bool			correctionNeeded;
	std::vector<Complex>	ofdmBuffer;
	bool			correlationOrder;
	bool			dxMode;
	bool			freqSpeedUp;
	uint8_t			freqCorrelator;
virtual	void			run		();
	void			generate_CI	(const std::vector<Complex> &,
	                                         int);

signals:
	void		setSynced		(bool);
	void		noSignalFound		();
	void		setSyncLost		();
	void		showTIIData		(QVector<tiiData>, int);
	void		showTIIspectrum		();
	void		showSpectrum		(int);
	void		showSnr			(float);
	void		showSnr			(float, float, float,
	                                                  float, float);
	void		showClockError		(int);
	void		showNull		(int, int);
	void		showChannel		(int);
	void		showCorrector		(int, float);
};

