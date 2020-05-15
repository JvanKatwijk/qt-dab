#
/*
 *    Copyright (C) 2013 .. 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of dab-2
 *
 *    dab-2 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    dab-2 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dab-2; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef	__RAW_FILES__
#define	__RAW_FILES__

#include	<QThread>
#include	<QString>
#include	<QFrame>
#include	<atomic>
#include	"dab-constants.h"
#include	"device-handler.h"
#include	"ringbuffer.h"

#include	"ui_filereader-widget.h"

class	dabProcessor;
/*
 */
class	rawFiles: public deviceHandler, public Ui_filereaderWidget {
Q_OBJECT
public:
			rawFiles	(QString, dabProcessor *);
 	               ~rawFiles();
	int32_t		getSamples	(std::complex<float> *, int32_t);
	bool		restartReader	(int32_t);
	void		stopReader	(void);
	void		show		();
	void		hide		();
	bool		isHidden	();
private:
	QString		fileName;
	dabProcessor	*base;
	int		tester;
        QFrame          *myFrame;
        std::complex<float> *oscillatorTable;
        int             currentPhase;
        int		totalOffset;
        void            handle_Value    (int, float, float);
virtual void            run             (void);
        int32_t         readBuffer      (std::complex<float> *, int32_t);
        RingBuffer<std::complex<float>> *_I_Buffer;
        int32_t         bufferSize;
        FILE		*filePointer;
        bool            readerOK;
	bool		readerPausing;
        bool            ExitCondition;
        int64_t		currPos;
};

#endif

