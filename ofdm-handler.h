#
/*
 *    Copyright (C) 2015 .. 2023
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
//#include	"tii-detector-1.h"
//#include	"tii-detector-2.h"
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

	void		select_TII		(uint8_t);
	void		start_dumping		(const QString &, int, int);
	void		stop_dumping		();
	bool		start_etiGenerator	(const QString &);
	void		stop_etiGenerator	();
	void		reset_etiGenerator	();
	void		set_scanMode		(bool);
	void		get_frameQuality	(int *, int*, int *);
//
//	for the tii settings
	void		set_tiiThreshold	(int16_t);
	void		set_tiiCollisions	(bool);
	void		set_tiiFilter		(bool);
//	servicing our subordinates
//	for the ficHandler:
	QString		find_service		(uint32_t, int);
	void		get_parameters		(const QString &,
	                                         uint32_t *, int *);
	std::vector<serviceId>	get_services	(int);
        void		data_for_audioservice	(const QString &,
	                                             audiodata &);
        void		data_for_packetservice	(const QString &,
	                                             packetdata &, int16_t);
	int		get_nrComps		(uint32_t);
        uint8_t		get_ecc			();
        int32_t		get_ensembleId		();
	QString		get_ensembleName	();
	void		set_epgData		(int32_t, int32_t,
	                                         const QString &,
	                                         const QString &);
	bool		has_timeTable		(uint32_t);
	std::vector<epgElement>	find_epgData	(uint32_t);
	uint32_t	julianDate		();
	QStringList	basicPrint		();
	int		scanWidth		();
	void		start_ficDump		(FILE *);
	void		stop_ficDump		();
//
//	for the mscHandler
//	void		resetServices		();
//	void		stopService		(descriptorType *, int);
	void		stopService		(int, int);
	bool		setAudioChannel		(audiodata &,
	                                         RingBuffer<std::complex<int16_t>> *,
	                                         FILE *, int);
	bool		setDataChannel		(packetdata &,
	                                         RingBuffer<uint8_t> *, int);
	void		handleIQSelector	();
	void		handleDecoderSelector	(int);
	void		setCorrelationOrder	(bool);
	void		setDXMode		(bool);
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

	uint8_t			selected_TII;

	int16_t			tiiThreshold;
	bool			tiiCollisions_active;
	bool			tiiFilter_active;

	int			decoder;
	int			threshold;
	int			totalFrames;
	int			goodFrames;
	int			badFrames;
	bool			tiiSwitch;
	int16_t			tii_depth;
	int16_t			echo_depth;
	RingBuffer<Complex >	*tiiBuffer_p;
	RingBuffer<Complex >	*nullBuffer_p;
	RingBuffer<float>	*snrBuffer_p;
	RingBuffer<Complex>	*channelBuffer_p;
	int16_t			tii_delay;
	int16_t			tii_counter;
	bool			eti_on;
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
	QByteArray		transmitters;
	bool			correctionNeeded;
	std::vector<Complex>	ofdmBuffer;
	bool			correlationOrder;
	bool			dxMode;
virtual	void			run		();
signals:
	void		set_synced		(bool);
	void		no_signal_found		();
	void		set_sync_lost		();
	void		show_tiiData		(QVector<tiiData>, int);
	void		show_tii_spectrum	();
	void		show_spectrum		(int);
	void		show_snr		(float);
	void		show_snr		(float, float, float,
	                                                  float, float);
	void		show_clock_error	(int);
	void		show_null		(int, int);
	void		show_channel		(int);
	void		show_Corrector		(int, float);
};

