#
/*
 *    Copyright (C) 2015 .. 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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
#ifndef	__OFDM_HANDLER_H
#define	__OFDM_HANDLER_H
/*
 *	dabProcessor is the embodying of all functionality related
 *	to the actual DAB processing.
 */
#include	"dab-constants.h"
#include	<QThread>
#include	<QObject>
#include	<QByteArray>
#include	<QStringList>
#include	<vector>
#include	<cstdint>
#include	<sndfile.h>
#include	"sample-reader.h"
#include	"fic-handler.h"
#include	"msc-handler.h"
#include	"device-handler.h"
#include	"ringbuffer.h"
#include	"tii-detector.h"
#include	"eti-generator.h"
//

class	RadioInterface;
class	dabParams;
class	processParams;

class ofdmHandler: public QThread {
Q_OBJECT
public:
		ofdmHandler  	(RadioInterface *,
	                         deviceHandler *,
	                         processParams *);
		~ofdmHandler			();
	void		start			();
//	void		start			(int32_t);
	void		stop			();
	void		startDumping		(SNDFILE *);
	void		stopDumping		();
	bool		start_etiGenerator	(const QString &);
	void		stop_etiGenerator	();
	void		reset_etiGenerator	();
	void		set_scanMode		(bool);
	void		get_frameQuality	(int *, int*, int *);
//
//	inheriting from our delegates
//	for the ficHandler:
	QString		findService		(uint32_t, int);
	void		getParameters		(const QString &,
	                                         uint32_t *, int *);
	std::vector<serviceId>	getServices	(int);
	bool		is_audioService		(const QString &s);
	bool		is_packetService	(const QString &s);
        void		dataforAudioService     (const QString &,
	                                             audiodata *);
        void		dataforPacketService	(const QString &,
	                                             packetdata *, int16_t);
	int		getSubChId		(const QString &, uint32_t);
        uint8_t		get_ecc			();
        int32_t		get_ensembleId		();
        QString		get_ensembleName	();
	uint16_t	get_countryName		();
	void		set_epgData		(int32_t, int32_t,
	                                         const QString &,
	                                         const QString &);
	bool		has_timeTable		(uint32_t);
	std::vector<epgElement>	find_epgData		(uint32_t);
	uint32_t	julianDate		();
	QStringList	basicPrint		();
	int		scanWidth		();
	void		start_ficDump		(FILE *);
	void		stop_ficDump		();

//	for the mscHandler
	void		reset_Services		();
	void		stop_service		(descriptorType *, int);
	void		stop_service		(int, int);
	bool		set_audioChannel	(audiodata *,
	                                         RingBuffer<int16_t> *,
	                                         FILE *, int);
	bool		set_dataChannel		(packetdata *,
	                                         RingBuffer<uint8_t> *, int);
	void		set_tiiDetectorMode	(bool);
private:
	int		threshold;
	int		totalFrames;
	int		goodFrames;
	int		badFrames;
	bool		tiiSwitch;
	int16_t		tii_depth;
	int16_t		echo_depth;
	deviceHandler	*inputDevice;
	dabParams	params;
	RingBuffer<Complex > *tiiBuffer;
	RingBuffer<Complex > *nullBuffer;
	RingBuffer<float>	*snrBuffer;
#ifdef	__ESTIMATOR_
	RingBuffer<Complex>	*channelBuffer;
#endif
	int16_t		tii_delay;
	int16_t		tii_counter;
	bool		eti_on;
	sampleReader	myReader;
	RadioInterface	*myRadioInterface;
	processParams	*p;
	ficHandler	my_ficHandler;
	mscHandler	my_mscHandler;
	etiGenerator	my_etiGenerator;
	TII_Detector	my_TII_Detector;

	int16_t		attempts;
	bool		scanMode;
	int32_t		T_null;
	int32_t		T_u;
	int32_t		T_s;
	int32_t		T_g;
	int32_t		T_F;
	int32_t		nrBlocks;
	int32_t		carriers;
	int32_t		carrierDiff;
	int16_t		fineOffset;
	int32_t		coarseOffset;
	QByteArray	transmitters;
	bool		correctionNeeded;
	std::vector<Complex>	ofdmBuffer;
	bool		wasSecond		(int16_t, dabParams *);
virtual	void		run			();
signals:
	void		setSynced		(bool);
	void		No_Signal_Found		();
	void		setSyncLost		();
	void		show_tii		(int, int);
	void		show_tii_spectrum	();
	void		show_Spectrum		(int);
	void		show_snr		(int);
	void		show_snr		(float, float, float,
	                                                  float, float);
	void		show_clockErr		(int);
	void		show_null		(int);
#ifdef	__ESTIMATOR_
	void		show_channel		(int);
#endif
};
#endif

