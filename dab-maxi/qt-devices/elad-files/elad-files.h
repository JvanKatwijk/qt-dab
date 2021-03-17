#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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
#ifndef	__ELAD_FILES__
#define	__ELAD_FILES__

#include	<QString>
#include	<QFrame>
#include	<sndfile.h>
#include	<atomic>
#include	"dab-constants.h"
#include	"device-handler.h"
#include	"ringbuffer.h"

#include	"ui_elad-files.h"
#include	"elad-reader.h"

#define	ELAD_RATE	3072000
#define	DAB_RATE	2048000
class	eladFiles: public deviceHandler, public Ui_eladreaderWidget {
Q_OBJECT
public:
			eladFiles	(QString);
	       		~eladFiles	();
	int32_t		getSamples	(std::complex<float> *, int32_t);
	int32_t		Samples		();
	bool		restartReader	(int32_t);
	void		stopReader	();
	void		show		();	
	void		hide		();
	bool		isHidden	();
private:
	QFrame		myFrame;
	QString		fileName;
	RingBuffer<uint8_t>	_I_Buffer;
	RingBuffer<std::complex<float>> _O_Buffer;
	int32_t		bufferSize;
	FILE		*filePointer;
	eladReader	*readerTask;
	std::atomic<bool>	running;

	int             iqSize;
        std::complex<float> convBuffer  [ELAD_RATE / 1000 + 1];
        int             mapTable_int    [DAB_RATE / 1000];
        float           mapTable_float  [DAB_RATE / 1000];
        int             convIndex;
	std::atomic<bool> iqSwitch;

public slots:
	void		setProgress	(int);
	void		handle_iqButton	();
};

#endif

