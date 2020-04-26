#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the sdrplayDab program
 *
 *    sdrplayDab is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    sdrplayDab is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with sdrplayDab; if not, write to the Free Software
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
#include	<QObject>
#include	<vector>
#include	"stdint.h"
#include	<sndfile.h>
#include	"phasereference.h"
#include	"ofdm-decoder.h"
#include	"fic-handler.h"
#include	"msc-handler.h"
#include	"ringbuffer.h"
#include	"tii_detector.h"
//

#define	DUMPSIZE	4096
class	RadioInterface;
class	dabParams;
class	QSettings;

#define	START		0000
#define	INIT		0001
#define	LOOKING_FOR_DIP	0002
#define	DIP_FOUND	0003
#define	END_OF_DIP	0004
#define	GO_FOR_BLOCK_0	0005
#define	SYNC_COMPLETE	0006
#define	BLOCK_READING	0007
#define	END_OF_FRAME	0010
#define	PREPARE_FOR_SKIP_NULL_PERIOD	0011
#define	SKIP_NULL_PERIOD		0012
#define	TO_NEXT_FRAME	0013

//
//	return values for addSymbol
#define	GO_ON			0
//#define	INITIAL_STRENGTH	1
#define	DEVICE_UPDATE		2

class dabProcessor: public QObject {
Q_OBJECT
public:
		dabProcessor  	(RadioInterface *,
	                         uint8_t,
	                         int16_t,
	                         int16_t,
	                         int16_t,
	                         int16_t,
	                         int16_t,
	                         RingBuffer<float> *,
	                         RingBuffer<std::complex <float>> *,
	                         RingBuffer<std::complex <float>> *,
	                         RingBuffer<std::complex <float>> *,
	                         RingBuffer<uint8_t> *
	                        );
		~dabProcessor	(void);
	int		addSymbol	(std::complex<float> *, int);
	void		reset		(void);
	void		stop		(void);
	void		start		();
	void		setOffset	(int32_t);
	void		coarseCorrectorOn	(void);
	void		coarseCorrectorOff	(void);
	void		startDumping	(SNDFILE *);
	void		stopDumping	();
	void		set_scanMode	(bool);
	void		update_data	(int *, float *, float *);
	float		initialSignal	(void);
//
//	inheriting from our delegates
	void		set_tiiCoordinates	(void);

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
	void		reset_msc();
	void		set_audioChannel	(audiodata *,
	                                             RingBuffer<int16_t> *);
	void		set_dataChannel		(packetdata *,
	                                             RingBuffer<uint8_t> *);
        uint8_t		get_ecc();
        int32_t		get_ensembleId();
        QString		get_ensembleName();
private:
	bool		phaseComputing;
	void		dump			(std::complex<float>);
	int16_t         dumpIndex;
	int16_t         dumpScale;
	int16_t         dumpBuffer [DUMPSIZE];
	std::atomic<SNDFILE *>  dumpfilePointer;

	bool		tiiSwitch;
	dabParams	params;
	uint8_t		dabMode;
	RadioInterface	*myRadioInterface;
	ficHandler	my_ficHandler;
	mscHandler	my_mscHandler;
	phaseReference	phaseSynchronizer;
	TII_Detector	my_TII_Detector;
	ofdmDecoder	my_ofdmDecoder;
	RingBuffer<std::complex<float>> *spectrumBuffer;
	RingBuffer<std::complex<float>> *tiiBuffer;
	std::vector<std::complex<float>> localBuffer;
	int32_t         localCounter;
	int32_t         bufferSize;

	int16_t		attempts;
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
	int32_t		totalOffset;
	int32_t		sampleCount;
	int32_t		nullCount;
	uint8_t		processorMode;
	bool		correctionNeeded;
	std::vector<std::complex<float>	>ofdmBuffer;
	std::vector<float>dataBuffer;
	int		bufferP;
	int		ofdmBufferIndex;
	float		avgSignalValue;
	float		avgLocalValue;
	int		counter;
	float		dipValue;
	int		dipCnt;

	std::complex<float>		FreqCorr;
	int		ofdmSymbolCount;
	std::vector<int16_t>		ibits;
	bool            wasSecond               (int16_t, dabParams *);
	void		handle_tii_detection	(std::vector<std::complex<float>>);

signals:
	void		setSynced		(bool);
	void		No_Signal_Found		(void);
	void		setSyncLost		(void);
	void		show_Spectrum		(int);
	void		set_freqOffset		(int);
	void		show_tii		(QByteArray);
	void		show_snr		(float, float);
};
#endif

