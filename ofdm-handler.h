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
#include	<sndfile.h>
#include	"sample-reader.h"
#include	"fic-handler.h"
#include	"msc-handler.h"
#include	"ofdm-decoder.h"
#include	"device-handler.h"
#include	"ringbuffer.h"
#include	"tii-detector.h"
#include	"eti-generator.h"

class	RadioInterface;
class	dabParams;
class	processParams;

class ofdmHandler: public QThread {
Q_OBJECT
public:
		ofdmHandler  	(RadioInterface *,
	                         deviceHandler *,
	                         processParams *,
	                         QSettings	*);
		~ofdmHandler			();
	void		start			();
//	void		start			(int32_t);
	void		stop			();
	void		start_dumping		(SNDFILE *);
	void		stop_dumping		();
	bool		start_etiGenerator	(const QString &);
	void		stop_etiGenerator	();
	void		reset_etiGenerator	();
	void		set_scanMode		(bool);
	void		get_frameQuality	(int *, int*, int *);
//	servicing our subordinates
//	for the ficHandler:
	QString		find_service		(uint32_t, int);
	void		get_parameters		(const QString &,
	                                         uint32_t *, int *);
	std::vector<serviceId>	get_services	(int);
	bool		is_audioservice		(const QString &s);
	bool		is_packetservice	(const QString &s);
        void		data_for_audioservice	(const QString &,
	                                             audiodata &);
        void		data_for_packetservice	(const QString &,
	                                             packetdata *, int16_t);
	int		get_subCh_id		(const QString &, uint32_t);
	int		get_nrComps		(uint32_t);
	int		get_SCIds		(const QString &);
        uint8_t		get_ecc			();
        int32_t		get_ensembleId		();
        QString		get_ensembleName	();
	uint16_t	get_countryName		();
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

//	for the mscHandler
	void		reset_services		();
	void		stop_service		(descriptorType *, int);
	void		stop_service		(int, int);
	bool		set_audioChannel	(audiodata &,
	                                         RingBuffer<std::complex<int16_t>> *,
	                                         FILE *, int);
	bool		set_dataChannel		(packetdata &,
	                                         RingBuffer<uint8_t> *, int);
	void		set_tiiDetectorMode	(bool);
	void		handle_iqSelector	();
	void		set_dcRemoval		(bool);
	void		handle_decoderSelector	(int);
private:
	RadioInterface		*radioInterface_p;
	processParams		*p;
//	deviceHandler		*device_p;
	dabParams		params;
	QSettings		*settings_p;
	sampleReader		theReader;
	ficHandler		theFicHandler;
	etiGenerator		theEtiGenerator;
	TII_Detector		theTIIDetector;
	ofdmDecoder		theOfdmDecoder;
	mscHandler		theMscHandler;

//	deviceHandler		*inputDevice;
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
	bool			isEvenFrame	(int16_t, dabParams *);
virtual	void			run		();
signals:
	void		set_synced		(bool);
	void		no_signal_found		();
	void		set_sync_lost		();
	void		show_tii		(int, int);
	void		show_tii_spectrum	();
	void		show_spectrum		(int);
	void		show_snr		(float);
	void		show_snr		(float, float, float,
	                                                  float, float);
	void		show_clock_error	(int);
	void		show_null		(int);
	void		show_channel		(int);
	void		show_Corrector		(int, float);
};

