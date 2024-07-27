#
/*
 *    Copyright (C) 2016 .. 2023
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
 *	Reading the samples from the input device. Since it has its own
 *	"state", we embed it into its own class
 */
#include	"dab-constants.h"
#include	<QObject>
#include	<sndfile.h>
#include	<cstdint>
#include	<atomic>
#include	<vector>
#include	"device-handler.h"
#include	"ringbuffer.h"
#include	"riffWriter.h"
//
//      Note:
//      It was found that enlarging the buffersize to e.g. 8192
//      cannot be handled properly by the underlying system.
#define DUMPSIZE                4096

class	RadioInterface;
class	sampleReader : public QObject {
Q_OBJECT
public:
	      	sampleReader	(RadioInterface *mr,
	                         	deviceHandler *theRig,
	                         	RingBuffer<Complex> *spectrumBuffer = nullptr);

	      	~sampleReader		();
	      void	setRunning	(bool b);
	      float	get_sLevel	();
	      Complex	get_sample	(float);
	      void	get_samples	(std::vector<Complex> &v,
	                                 int index,
	                                 int32_t n, int32_t phase, bool saving);
	      void	start_dumping	(const QString &, int);
	      void	stop_dumping();
	      void	set_dcRemoval	(bool);
private:
	      riffWriter	sourceDumper;
	      RadioInterface	*myRadioInterface;
	      deviceHandler	*theRig;
	      RingBuffer<Complex> *spectrumBuffer;
	      std::vector<Complex> localBuffer;
	      int32_t		localCounter;
	      int32_t		bufferSize;
	      int32_t		currentPhase;
	      std::atomic<bool>	running;
	      int32_t		bufferContent;
	      float		sLevel;
	      int32_t		sampleCount;
	      int32_t		corrector;
	      bool		dumping;
	      int16_t		dumpIndex;
	      int16_t		dumpScale;
	      int16_t		dumpBuffer [DUMPSIZE];
//	      std::atomic<SNDFILE *>	dumpfilePointer;
	      int		repetitionCounter;

	      bool		dcRemoval;
	      DABFLOAT		dcReal;
	      DABFLOAT		dcImag;

signals:
	      void		show_spectrum	(int);
	      void		show_dcOffset	(float);
};

