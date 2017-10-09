
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
#ifndef	__SAMPLE_READER__
#define	__SAMPLE_READER__
/*
 *
 */
#include	"dab-constants.h"
#include	<QObject>
#include	"stdint.h"
#include	"virtual-input.h"
#include	"ringbuffer.h"
//

class	RadioInterface;
class	Reader : public QObject {
Q_OBJECT
public:
			Reader		(RadioInterface *mr,
	                         	virtualInput *theRig,
#ifdef	HAVE_SPECTRUM
	                         	,RingBuffer<DSPCOMPLEX> *spectrumBuffer
#endif
	       				);

			~Reader		(void);
		void	setRunning	(bool b);
		float	get_sLevel	(void);
		DSPCOMPLEX getSample	(int32_t);
	        void	getSamples	(DSPCOMPLEX *v,
	                                 int16_t n, int32_t phase);
private:
	int32_t		bufferSize;
	DSPCOMPLEX	*localBuffer;
	int32_t		localCounter;
	int32_t		localPhase;
	DSPCOMPLEX	*oscillatorTable;
	std::atomic<bool>	running;
	int32_t		bufferContent;
	float		sLevel;
	int32_t		sampleCount;

signals:
	void		show
	void		showSpectrum (int);
};

