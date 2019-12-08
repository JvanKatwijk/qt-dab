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
 *
 * 	xml-file handler
 *	dabsticks 
 */
#include	"xmlfile-handler.h"
#include	<cstdio>
#include	<unistd.h>
#include	<cstdlib>
#include	<fcntl.h>
#include	<sys/time.h>
#include	<ctime>

#include	"xml-descriptor.h"
#include	"xmlfile-reader.h"

#define	INPUT_FRAMEBUFFERSIZE	8 * 32768
//
//
	xmlfileHandler::xmlfileHandler (QString f) {
	fileName	= f;
	myFrame		= new QFrame;
	setupUi	(myFrame);
	_I_Buffer	= new RingBuffer<std::complex<float>>(INPUT_FRAMEBUFFERSIZE);
	theFile	= fopen (f. toUtf8 (). data(), "rb");
	if (theFile == nullptr) {
	   fprintf (stderr, "file %s cannot open\n",
	                                   f. toUtf8(). data());
	   perror ("file ?");
	   delete myFrame;
	   delete _I_Buffer;
	   throw (31);
	}
	
	bool	ok	= false;
	nameofFile	-> setText (f);
	theDescriptor	= new xmlDescriptor (theFile, &filePointer, &ok);
	if (!ok) {
	   fprintf (stderr, "%s probably not an xml file\n",
	                               f. toUtf8 (). data ());
	   delete myFrame;
	   delete _I_Buffer;
	   throw (32);
	}

	fileProgress    -> setValue (0);
        currentTime     -> display (0);
	running. store (false);
}

	xmlfileHandler::~xmlfileHandler	() {
	if (running. load()) {
	   theReader	-> stopReader();
	   while (theReader -> isRunning())
	      usleep (100);
	   delete theReader;
	}
	if (theFile != nullptr)
	   fclose (theFile);

	delete _I_Buffer;
	delete	myFrame;
	delete	theDescriptor;
}

bool	xmlfileHandler::restartReader (int32_t freq) {
	(void)freq;
	if (running. load())
	   return true;
	theReader	= new xmlfileReader (this,
	                                     theFile,
	                                     theDescriptor,
	                                     filePointer,
	                                     _I_Buffer);
	running. store (true);
	return true;
}

void	xmlfileHandler::stopReader () {
	if (running. load()) {
	   theReader	-> stopReader();
	   while (theReader -> isRunning())
	      usleep (100);
	   delete theReader;
	}
	running. store (false);
}

//	size is in "samples"
int32_t	xmlfileHandler::getSamples	(std::complex<float> *V,
	                                 int32_t size) {
int32_t	amount, i;

	if (theFile == nullptr)		// should not happen
	   return 0;

	while ((int32_t)(_I_Buffer -> GetRingBufferReadAvailable()) < size)
	   usleep (500);

	amount = _I_Buffer	-> getDataFromBuffer (V, size);
	return amount;
}

int32_t	xmlfileHandler::Samples	() {
	if (theFile == nullptr)
	   return 0;
	return _I_Buffer -> GetRingBufferReadAvailable();
}

void	xmlfileHandler::setProgress (int samplesRead, int samplesToRead) {
	fileProgress	-> setValue ((float)samplesRead / samplesToRead * 100);
	currentTime	-> display (samplesRead / 2048000.0);
	totalTime	-> display (samplesToRead / 2048000.0);
}

