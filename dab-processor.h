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
#include	"stdint.h"
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
class	common_fft;
class	dabParams;

class dabProcessor: public QThread {
Q_OBJECT
public:
		dabProcessor  	(RadioInterface *,
	                         virtualInput *,
	                         uint8_t,
	                         int16_t,
	                         RingBuffer<int16_t> *,
	                         RingBuffer<uint8_t> *,
	                         QString
#ifdef	HAVE_SPECTRUM
	                        ,RingBuffer<DSPCOMPLEX>	*,
	                         RingBuffer<DSPCOMPLEX>	*
#endif
	                        );
		~dabProcessor	(void);
	void		reset			(void);
	void		stop			(void);
	void		setOffset		(int32_t);
	void		coarseCorrectorOn	(void);
	void		coarseCorrectorOff	(void);
	void		startDumping		(SNDFILE *);
	void		stopDumping		(void);
	void		set_scanMode		(bool);
//
//	inheriting from our delegates
	void		set_tiiCoordinates	(void);
	void		setSelectedService      (QString &);
        uint8_t		kindofService           (QString &);
        void		dataforAudioService     (QString &, audiodata *);
        void		dataforAudioService     (int16_t,   audiodata *);
        void		dataforDataService      (QString &, packetdata *);
        void		dataforDataService      (int16_t,   packetdata *);
	void		set_audioChannel	(audiodata *);
	void		set_dataChannel		(packetdata *);
        uint8_t		get_ecc                 (void);
        int32_t		get_ensembleId          (void);
        QString		get_ensembleName        (void);
	void		clearEnsemble		(void);
private:
	virtualInput	*theRig;
	dabParams	params;
	sampleReader	myReader;
	RadioInterface	*myRadioInterface;
	ficHandler	my_ficHandler;
	mscHandler	my_mscHandler;
	TII_Detector	my_TII_Detector;
	bool		tiiCoordinates;

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
	DSPCOMPLEX	*dataBuffer;
	int32_t		FreqOffset;
	int16_t		fineCorrector;
	int32_t		coarseCorrector;

	bool		f2Correction;
	int32_t		tokenCount;
	DSPCOMPLEX	*ofdmBuffer;
	uint32_t	ofdmBufferIndex;
	uint32_t	ofdmSymbolCount;
	phaseReference	phaseSynchronizer;
	ofdmDecoder	my_ofdmDecoder;
virtual	void		run		(void);
	bool		isReset;
	int16_t		processBlock_0	(DSPCOMPLEX *);
	common_fft	*fft_handler;
	DSPCOMPLEX	*fft_buffer;
signals:
	void		setSynced		(char);
	void		No_Signal_Found		(void);
	void		setSyncLost		(void);
	void		showCoordinates		(float, float);
};
#endif

