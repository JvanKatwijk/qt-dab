#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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
#ifndef	__DAB_PROCESSOR__
#define	__DAB_PROCESSOR__
/*
 *	dabProcessor is the embodying of all functionality related
 *	to the actal DAB processing.
 */
#include	"dab-constants.h"
#include	<QThread>
#include	<QObject>
#include	<vector>
#include	<atomic>
#include	<cstdint>
#include	<sndfile.h>
#include	"sample-reader.h"
#include	"phasereference.h"
#include	"ofdm-decoder.h"
#include	"fic-handler.h"
#include	"msc-handler.h"
#include	"virtual-input.h"
#include	"ringbuffer.h"
#include	"tii_detector.h"
//

class	RadioInterface;
class	dabParams;

class dabProcessor: public QThread {
Q_OBJECT
public:
		dabProcessor  	(RadioInterface *,
	                         virtualInput *,
	                         uint8_t,
	                         int16_t,
	                         int16_t,
	                         int16_t,
	                         int16_t,
	                         int16_t,	// tii_depth
	                         int16_t,	// echo_depth
	                         QString,
	                         RingBuffer<float> *,
	                         RingBuffer<DSPCOMPLEX>	*,
	                         RingBuffer<std::complex<float>>	*,
	                         RingBuffer<DSPCOMPLEX>	*,
	                         RingBuffer<uint8_t> *
	                        );
		~dabProcessor();
    void		reset();
	void		stop();
	void		setOffset		(int32_t);
	void		coarseCorrectorOn();
	void		coarseCorrectorOff();
	void		startDumping		(SNDFILE *);
    void		stopDumping();
	void		set_scanMode		(bool);
//
//	inheriting from our delegates
	void		set_tiiCoordinates();
	void		setSelectedService      (QString &);
	bool		is_audioService		(const QString &s);
	bool		is_packetService	(const QString &s);
        void		dataforAudioService     (const QString &,
	                                             audiodata *, int16_t);
        void		dataforPacketService	(const QString &,
	                                             packetdata *, int16_t);
	void		unset_Channel		(const QString &);
	void		reset_msc();
	void		set_audioChannel	(audiodata *,
	                                             RingBuffer<int16_t> *);
	void		set_dataChannel		(packetdata *,
	                                             RingBuffer<uint8_t> *);
        uint8_t		get_ecc();
        int32_t		get_ensembleId();
        QString		get_ensembleName();
	void		print_Overview();
	void		clearEnsemble();
private:
	int		threshold_1;
	int		threshold_2;
	bool		tiiSwitch;
	int16_t		tii_depth;
	int16_t		echo_depth;
	virtualInput	*theRig;
	dabParams	params;
	RingBuffer<DSPCOMPLEX> *tiiBuffer;
	int16_t		tii_delay;
	int16_t		tii_counter;

	sampleReader	myReader;
	RadioInterface	*myRadioInterface;
	ficHandler	my_ficHandler;
	mscHandler	my_mscHandler;
	phaseReference	phaseSynchronizer;
	TII_Detector	my_TII_Detector;
	ofdmDecoder	my_ofdmDecoder;

	bool		scanMode;
	int		false_dipStarts;
	int		false_dipEnds;
	int		false_frameStarts;
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
	std::atomic<bool>	running;
	bool		correctionNeeded;
	std::vector<DSPCOMPLEX>ofdmBuffer;
	bool		wasSecond		(int16_t, dabParams *);
virtual	void		run();
signals:
	void		setSynced		(bool);
	void		No_Signal_Found();
	void		setSyncLost();
	void		showCoordinates		(int);
	void		showSecondaries		(int);
	void		show_Spectrum		(int);
	void		show_tii		(int);
	void		show_snr		(int);
};
#endif

