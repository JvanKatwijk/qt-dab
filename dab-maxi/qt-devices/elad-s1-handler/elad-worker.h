#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB
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

#ifndef __ELAD_WORKER__
#define	__ELAD_WORKER__

#include	<stdlib.h>
#include	<stdio.h>
#include	<math.h>
#include	<string.h>
#include	<unistd.h>
#include	<atomic>
#include	<stdint.h>
#include	<QThread>
#include	"dab-constants.h"
#include	"ringbuffer.h"

class	eladLoader;
class	eladHandler;

class	eladWorker: public QThread {
Q_OBJECT
public:
			eladWorker	(int32_t,	// selected frequency
	                                 int32_t,	// NyquistWidth (KHz)
	                                 int32_t,	// Offset (in MHz)
	                                 eladLoader *,
	                                 eladHandler *,
	                                 RingBuffer<std::complex<float>> *,
	                                 bool *);
			~eladWorker		();
	int32_t		getVFOFrequency		();
	void		toggle_IQSwitch		();
	void		stop			();
private:
	void		run			();
	eladLoader		*functions;	// 
	RingBuffer<uint8_t>	_I_Buffer;
	RingBuffer<std::complex<float>>	*theBuffer;
	int32_t			externalFrequency;
	long int		eladFrequency;
	int32_t			NyquistWidth;
	int32_t			Offset;
	std::atomic<bool>	iqSwitch;
	std::atomic<bool>	running;
	int32_t			theRate;
	int			mapTable_int	[2048];
	float			mapTable_float	[2048];
	int			conversionNumber;
//      we convert to complexes directly
        int			iqSize;
        int			convBufferSize;
        int			convIndex;
        std::complex<float>	*convBuffer;
signals:
	void			show_eladFrequency	(int);
	void			show_iqSwitch	(bool);
};
#endif

