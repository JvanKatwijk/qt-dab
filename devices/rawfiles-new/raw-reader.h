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
#ifndef	__RAW_READER__
#define	__RAW_READER__

#include	<QThread>
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	<atomic>
class	rawFiles;

class	rawReader:public QThread {
Q_OBJECT
public:
			rawReader	(rawFiles *,
	                                 FILE *, RingBuffer<uint8_t> *); 
			~rawReader();
	void		startReader();
	void		stopReader();
private:
virtual void		run();
	FILE		*filePointer;
	RingBuffer<uint8_t>	*theBuffer;
	uint64_t	period;
	std::atomic<bool>	running;
	uint8_t		*bi;
	rawFiles	*parent;
	int64_t		fileLength;
signals:
	void		setProgress	(int, float);
};

#endif

