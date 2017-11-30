#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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

#include	<QThread>
#include	<QSettings>
#include	<QHBoxLayout>
#include	<QLabel>
#include	<QMessageBox>
#include	<QDir>

#include	"elad-handler.h"	// our header
#include	"elad-worker.h"		// the thread, reading in the data
#include	"elad-loader.h"		// function loader
#include	<stdio.h>

#define	RATE	3072000
//	Currently, we do not have lots of settings,
//	it just might change suddenly, but not today
		eladHandler::eladHandler (QSettings *s) {
int16_t	success;

	this	-> eladSettings	= s;
	this	-> myFrame	= new QFrame (NULL);
	setupUi (myFrame);
	myFrame	-> show ();
	_I_Buffer		= NULL;
	theLoader		= NULL;
	theWorker		= NULL;
	vfoOffset			=
	             eladSettings -> value ("elad-offset", 0). toInt ();

//
//	sometimes problems with dynamic linkage of libusb, it is
//	loaded indirectly through the dll
	if (libusb_init (NULL) < 0) {
	   fprintf (stderr, "libusb problem\n");	// should not happen
	   throw (21);
	}

	libusb_exit (NULL);
	theLoader	= new eladLoader (RATE, &success);
	if (success != 0) {
	   if (success == -1)
	   QMessageBox::warning (myFrame, tr ("sdr"),
	                         tr ("No success in loading libs\n"));
	   else
	   if (success == -2)
	   QMessageBox::warning (myFrame, tr ("sdr"),
	                         tr ("No success in setting up USB\n"));
	   else
	   if (success == -3)
	   QMessageBox::warning (myFrame, tr ("sdr"),
	                         tr ("No success in FPGA init\n"));
	   else
	   if (success == -4)
	   QMessageBox::warning (myFrame, tr ("sdr"),
	                         tr ("No success in hardware init\n"));
	
	   statusLabel -> setText ("not functioning");
	   delete theLoader;
	   theLoader	= NULL;
	   delete myFrame;
	   throw (21);
	}
//
//	Note (10.10.2014: 
//	It turns out that the elad provides for 32 bit samples
//	packed as bytes
	statusLabel	-> setText ("Loaded");
//
//	buffersize app 0.5 seconds of data
	_I_Buffer	= new RingBuffer<std::complex<float>>(32 * 32768);
	vfoFrequency	= Khz (220000);
//
//	since localFilter and gainReduced are also used as
//	parameter for the API functions, they are int's rather
//	than bool.
	localFilter	= 0;
	filterText	-> setText ("no filter");
	gainReduced	= 0;
	gainLabel	-> setText ("0");
	connect (hzOffset, SIGNAL (valueChanged (int)),
	         this, SLOT (setOffset (int)));
	connect (gainReduction, SIGNAL (clicked (void)),
	         this, SLOT (setGainReduction (void)));
	connect (filter, SIGNAL (clicked (void)),
	         this, SLOT (setFilter (void)));
}
//
	eladHandler::~eladHandler	(void) {
	eladSettings -> setValue ("elad-offset", vfoOffset);
	stopReader ();
	if (_I_Buffer != NULL)
	   delete _I_Buffer;
	if (theLoader != NULL)
	   delete theLoader;
	if (theWorker != NULL)
	   delete theWorker;
}

int32_t	eladHandler::defaultFrequency	(void) {
	return Khz (220000);
}

void	eladHandler::setVFOFrequency	(int32_t newFrequency) {
int32_t	realFreq = newFrequency - vfoOffset;

	if (theWorker == NULL) {
	   vfoFrequency = newFrequency - vfoOffset;
	   return;
	}

	theWorker -> setVFOFrequency (realFreq);
	vfoFrequency = theWorker -> getVFOFrequency ();
}

int32_t	eladHandler::getVFOFrequency	(void) {
	return vfoFrequency + vfoOffset;
}

bool	eladHandler::restartReader	(void) {
bool	success;

	if (theWorker != NULL) 
	   return true;

	_I_Buffer	-> FlushRingBuffer ();
	theWorker	= new eladWorker (vfoFrequency,
	                                  theLoader,
	                                  this,
	                                  _I_Buffer,
	                                  &success);
	fprintf (stderr, "worker started, success = %d\n", success);
	return success;
}

void	eladHandler::stopReader	(void) {
	if (theWorker == NULL) 
	   return;

	theWorker	-> stop ();
	while (theWorker -> isRunning ())
	   usleep (100);
	delete theWorker;
	theWorker = NULL;
}

//	we are - in this context - certain that whenever getSamples
//	is called, there are sufficient samples available.
int32_t	eladHandler::getSamples (std::complex<float> *V, int32_t size) { 
	return _I_Buffer	-> getDataFromBuffer (V, size);
}

int32_t	eladHandler::Samples	(void) {
	return _I_Buffer	-> GetRingBufferReadAvailable () / 2;
}

void	eladHandler::resetBuffer	(void) {
	_I_Buffer	-> FlushRingBuffer ();
}

//
//	Although we are getting 30-more bits in, the adc in the
//	elad gives us 14 bits. That + 20 db gain results in app 105 db
//	plus a marge it is app 120 a 130 db, so the bit depth for the scope
//	is 21
int16_t	eladHandler::bitDepth	(void) {
	return 21;
}

//
void	eladHandler::setOffset	(int k) {
	vfoOffset	= k;
}

void	eladHandler::setGainReduction	(void) {
	gainReduced = gainReduced == 1 ? 0 : 1;
	theLoader -> set_en_ext_io_ATT20 (theLoader -> getHandle (),
	                                     &gainReduced);
	gainLabel -> setText (gainReduced == 1 ? "-20" : "0");
}

void	eladHandler::setFilter	(void) {
	localFilter = localFilter == 1 ? 0 : 1;
	theLoader -> set_en_ext_io_LP30 (theLoader -> getHandle (),
	                                     &localFilter);
	filterText	-> setText (localFilter == 1 ? "30 Mhz" : "no filter");
}

void	eladHandler::show_eladFrequency	(int f) {
	eladFrequency	-> display (f);
}

void	eladHandler::show_iqSwitch	(bool b) {
	if (b)
	   elad_iqSwitch -> setText ("reversed IQ");
	else
	   elad_iqSwitch -> setText ("normal IQ");
}

