#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB.
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
#ifndef	__MP4PROCESSOR__
#define	__MP4PROCESSOR__
/*
 * 	Handling superframes for DAB+ and delivering
 * 	frames into the ffmpeg or faad decoding library
 */
//
#include	"dab-constants.h"
#include	<cstdio>
#include	<cstdint>
#include	<vector>
#include	"audio-base.h"
#include	"frame-processor.h"
#include	"firecode-checker.h"
#include	"reed-solomon.h"
#include	<QObject>
#include	"pad-handler.h"

#ifdef	__WITH_FDK_AAC__
#include	"fdk-aac.h"
#else
#include	"faad-decoder.h"
#endif

class	RadioInterface;

class	mp4Processor : public QObject, public frameProcessor {
Q_OBJECT
public:
			mp4Processor	(RadioInterface *,
	                                 int16_t,
	                                 RingBuffer<int16_t> *,
	                                 RingBuffer<uint8_t> *,
	                                 FILE *);
			~mp4Processor();
	void		addtoFrame	(std::vector<uint8_t>);
private:
	RadioInterface	*myRadioInterface;
	padHandler	my_padhandler;
	bool		processSuperframe (uint8_t [], int16_t);
	int		build_aacFile (int16_t aac_frame_len,
                                     stream_parms *sp,
                                     uint8_t	*data,
                                     std::vector<uint8_t> &fileBuffer);

	FILE		*dump;
	uint8_t		procMode;
	int16_t		superFramesize;
	int16_t		blockFillIndex;
	int16_t		blocksInBuffer;
	int16_t         frameCount;
        int16_t         frameErrors;
        int16_t         rsErrors;
        int16_t		crcErrors;
        int16_t         aacErrors;
        int16_t         aacFrames;
        int16_t         successFrames;
        int16_t         charSet;
	int		goodFrames;
	int		totalCorrections;
	int16_t		bitRate;
	RingBuffer<uint8_t>	*frameBuffer;
	std::vector<uint8_t> frameBytes;
	std::vector<uint8_t> outVector;
	int16_t		RSDims;
	int16_t		au_start	[10];
	firecode_checker	fc;
	reedSolomon	my_rsDecoder;
//	and for the aac decoder
#ifdef	__WITH_FDK_AAC__
	fdkAAC		*aacDecoder;
#else
	faadDecoder	*aacDecoder;
#endif
signals:
	void		show_frameErrors		(int);
	void		show_rsErrors			(int);
	void		show_aacErrors			(int);
	void		isStereo			(bool);
	void		newFrame			(int);
	void		show_rsCorrections		(int, int);
};

#endif


