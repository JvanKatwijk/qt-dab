
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computinmg
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
#include	"dab-constants.h"	// some general definitions
#include	"elad-worker.h"		// our header
#include	"elad-handler.h"
#include	"elad-loader.h"		// the API definition
#include	"ringbuffer.h"		// the buffer

//	The elad-worker is a simple wrapper around the elad
//	interface. It is a pretty simple thread performing the
//	basic functions, it reads the bytes, converts them to
//	samples and converts the rate to 2048000
//	
	eladWorker::eladWorker (int32_t		eladFrequency,
	                        eladLoader	*f,
	                        eladHandler	*h,
	                        RingBuffer<uint8_t> *_O_Buffer,
	                        bool	*OK) {
	fprintf (stderr, "creating a worker\n");
	(void)h;
	this	-> eladFrequency	= eladFrequency;
	this	-> functions		= f;
	this	-> _O_Buffer		= _O_Buffer;
	*OK				= false;	// just the default
	functions	-> StartFIFO (functions -> getHandle ());
	start ();
	*OK			= true;
}

//	As usual, killing objects containing a thread need to
//	be done carefully.
void	eladWorker::stop	(void) {
	if (running. load ())
	   functions	-> StopFIFO (functions -> getHandle ());
	running. store (false);
}

	eladWorker::~eladWorker	(void) {
	stop ();
	while (isRunning ())
	   msleep (1);
}

#define	BUFFER_SIZE	(8 * 8192)
uint8_t	buffer [BUFFER_SIZE];
//
//	The libusb function is synchronous, meaning that
//	we do not have time to do the conversion from 3072
//	to 2048 here.
//	So here we convert the samplestream to a stream of
//	complex IQ samples, and pass them on
void	eladWorker:: run (void) {
int32_t	amount;
int	rc;

	running. store (false);
//	when (re)starting, clean up first
	fprintf (stderr, "worker thread started\n");

	bool result = functions -> SetHWLO (functions -> getHandle (),
	                                                 &eladFrequency);
	fprintf (stderr, "setting frequency is %s\n",
	                               result ? "good" : "erroneous");

	running. store (true);
	while (running. load ()) {
	   rc = libusb_bulk_transfer (functions -> getHandle (),
	                              (6 | LIBUSB_ENDPOINT_IN),
	                              (uint8_t *)buffer,
	                              BUFFER_SIZE * sizeof (uint8_t),
	                              &amount,
	                              2000);
	   if (rc) {
              fprintf (stderr,
	               "Error in libusb_bulk_transfer: [%d] %s\n",
	               rc,
	               libusb_error_name (rc));
	      if (rc != 7)
	         break;
	   }

	   _O_Buffer -> putDataIntoBuffer (buffer, amount);
	}
	fprintf (stderr, "eladWorker now stopped\n");
}

