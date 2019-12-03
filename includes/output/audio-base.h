#
/*
 *    Copyright (C)  2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB.
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

#ifndef __AUDIO_BASE__
#define	__AUDIO_BASE__
#include	"dab-constants.h"
#include	<cstdio>
#include	<samplerate.h>
#include	<sndfile.h>
#include	<QMutex>
#include	<QObject>
#include	"newconverter.h"
#include	"ringbuffer.h"


class	audioBase: public QObject {
Q_OBJECT
public:
			audioBase();
virtual			~audioBase();
virtual	void		stop			(void);
virtual	void		restart();
//
	void		audioOut		(int16_t *, int32_t, int);
	void		startDumping		(SNDFILE *);
	void		stopDumping();
private:
	void		audioOut_16000		(int16_t *, int32_t);
	void		audioOut_24000		(int16_t *, int32_t);
	void		audioOut_32000		(int16_t *, int32_t);
	void		audioOut_48000		(int16_t *, int32_t);
	newConverter	converter_16;
	newConverter	converter_24;
	newConverter	converter_32;
	SNDFILE		*dumpFile;
	QMutex		myLocker;
protected:
virtual	void		audioOutput		(float *, int32_t);
};
#endif

