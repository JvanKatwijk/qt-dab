#
/*
 *    Copyright (C) 2014 .. 2017
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

#include	"lime-reader.h"
#include	"lime-handler.h"
#include	<stdint.h>

	limeReader::limeReader (lms_device_t *device, 
	                        RingBuffer<std::complex<float>> *buffer,
	                        limeHandler	*theBoss) {
int	res;

	this	-> device	= device;
	this	-> buffer	= buffer;
	this	-> theBoss	= theBoss;
	stream. isTx		= false;
	stream. channel		= 0;
	stream. fifoSize	= 4096;
	stream. throughputVsLatency	= 0.5;	// ???
	stream. dataFmt		= lms_stream_t::LMS_FMT_I12;	// 12 bit ints
	
	running. store (false);
	start	();
}

	limeReader::~limeReader	(void) {
	while (isRunning ()) {
	   running. store (false);
	   usleep (1000);
	}
}

#define	FIFO_SIZE	4096
static
int16_t localBuffer [4 * FIFO_SIZE];
static
std::complex <float> dataBuffer [2 * FIFO_SIZE];

void	limeReader::run	(void) {
int	res;
lms_stream_status_t streamStatus;
int	underruns	= 0;
int	overruns	= 0;
int	dropped		= 0;
int	amountRead	= 0;

	res	= theBoss -> LMS_SetupStream (device, &stream);
	if (res < 0)
	   return;
	res	= theBoss -> LMS_StartStream (&stream);
	if (res < 0) 
	   return;

	running. store (true);
	while (running. load ()) {
	   res = theBoss -> LMS_RecvStream (&stream, localBuffer,
	                                     FIFO_SIZE,  &meta, 10000);
	   if (res > 0) {
	      for (int i = 0; i < res; i ++)
	         dataBuffer [i] =
	              std::complex<float> (localBuffer [2 * i] / 1024.0,
	                                   localBuffer [2 * i + 1] / 1024.0);
	      buffer -> putDataIntoBuffer (dataBuffer, res);
	      amountRead	+= res;
	   }
	   res	= theBoss -> LMS_GetStreamStatus (&stream, &streamStatus);
	   underruns	+= streamStatus. underrun;
	   overruns	+= streamStatus. overrun;
	   if (amountRead > 4 * 2048000) {
	      amountRead = 0;
	      showErrors (underruns, overruns);
	      underruns	= 0;
	      overruns	= 0;
	   }
	}
	theBoss -> LMS_StopStream	(&stream);	
	theBoss	-> LMS_DestroyStream	(device, &stream);
}

