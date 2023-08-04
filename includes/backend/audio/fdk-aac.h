#
/*
 *    Copyright (C) 2020
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
 *
 *	Use the fdk-aac library.
 */
#ifdef	__WITH_FDK_AAC__
#ifndef	__FDK_AAC__
#define	__FDK_AAC__

#include	<QObject>
#include	<stdint.h>
#include	<aacdecoder_lib.h>
#include	"ringbuffer.h"


typedef struct {
        int     rfa;
        int     dacRate;
        int     sbrFlag;
        int     psFlag;
        int     aacChannelMode;
        int     mpegSurround;
        int     CoreChConfig;
        int     CoreSrIndex;
        int     ExtensionSrIndex;
} stream_parms;

class	RadioInterface;
//
/**
  *	fdkAAC is an interface to the fdk-aac library,
  *	using the LOAS protocol
  */
class	fdkAAC : public QObject {
Q_OBJECT
public:
		fdkAAC (RadioInterface *mr,
                        RingBuffer<int16_t> *buffer);
		~fdkAAC	();

int16_t		MP42PCM (stream_parms *sp,
                         uint8_t   packet [],
                         int16_t   packetLength);
private:
	RingBuffer<int16_t>	*audioBuffer;
	bool			working;
	HANDLE_AACDECODER	handle;
signals:
	void			newAudio	(int, int);
};

#endif
#endif
