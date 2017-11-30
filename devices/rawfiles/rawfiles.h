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
#ifndef	__RAW_FILES__
#define	__RAW_FILES__

#include	<QThread>
#include	<QString>
#include	<QFrame>
#include	"dab-constants.h"
#include	"virtual-input.h"
#include	"ringbuffer.h"

#include	"ui_filereader-widget.h"

class	QLabel;
class	QSettings;
class	fileHulp;
/*
 */
class	rawFiles: public virtualInput,
	          public Ui_filereaderWidget, QThread {
public:

			rawFiles	(QString);
	       		~rawFiles	(void);
	int32_t		getSamples	(std::complex<float> *, int32_t);
	uint8_t		myIdentity	(void);
	int32_t		Samples		(void);
	bool		restartReader	(void);
	void		stopReader	(void);
private:
	QString		fileName;
virtual	void		run		(void);
	QFrame		*myFrame;
	int32_t		readBuffer	(uint8_t *, int32_t);
	RingBuffer<uint8_t>	*_I_Buffer;
	int32_t		bufferSize;
	FILE		*filePointer;
	bool		readerOK;
	bool		readerPausing;
	bool		ExitCondition;
	bool		ThreadFinished;
	int64_t		currPos;
};

#endif

