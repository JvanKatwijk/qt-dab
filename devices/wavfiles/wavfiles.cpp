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
#include	<cstdio>
#include	<unistd.h>
#include	<cstdlib>
#include	<fcntl.h>
#include	<sys/time.h>
#include	<ctime>
#include	<QString>
#include	"wavfiles.h"

#define	__BUFFERSIZE	8 * 32768

	wavFiles::wavFiles (QString f) {
SF_INFO *sf_info;

	fileName	= f;
	myFrame		= new QFrame;
	setupUi (myFrame);
	myFrame		-> show();

	_I_Buffer	= new RingBuffer<std::complex<float>> (__BUFFERSIZE);

	sf_info		= (SF_INFO *)alloca (sizeof (SF_INFO));
	sf_info	-> format	= 0;
	filePointer	= sf_open (f. toUtf8(). data(), SFM_READ, sf_info);
	if (filePointer == nullptr) {
	   fprintf (stderr, "file %s no legitimate sound file\n", 
	                                f. toUtf8().data());
	   delete myFrame;
	   throw (24);
	}
	if ((sf_info -> samplerate != 2048000) ||
	    (sf_info -> channels != 2)) {
	   fprintf (stderr, "This is not a recorded dab file, sorry\n");
	   sf_close (filePointer);
	   delete myFrame;
	   throw (25);
	}
	nameofFile	-> setText (f);
	fileProgress	-> setValue (0);
	currentTime	-> display (0);
	int64_t fileLength	= sf_seek (filePointer, 0, SEEK_END);
	totalTime	-> display ((float)fileLength / 2048000);
	running. store (false);
}
//
//	Note that running == true <==> readerTask has value assigned

	wavFiles::~wavFiles() {
	if (running. load()) {
	   readerTask	-> stopReader();
	   while (readerTask -> isRunning())
	      usleep (500);
	   delete readerTask;
	}
	if (filePointer != nullptr)
	   sf_close (filePointer);
	delete _I_Buffer;
	delete	myFrame;
}

bool	wavFiles::restartReader() {
	if (running. load())
           return true;
        readerTask      = new wavReader (this, filePointer, _I_Buffer);
        running. store (true);
        return true;
}

void	wavFiles::stopReader() {
       if (running. load()) {
           readerTask   -> stopReader();
           while (readerTask -> isRunning())
              usleep (100);
	   delete readerTask;
        }
        running. store (false);
}

//	size is in I/Q pairs
int32_t	wavFiles::getSamples	(std::complex<float> *V, int32_t size) {
int32_t	amount;
	
	if (filePointer == nullptr)
	   return 0;

	while (_I_Buffer -> GetRingBufferReadAvailable() < size)
	      usleep (100);

	amount = _I_Buffer	-> getDataFromBuffer (V, size);
	
	return amount;
}

int32_t	wavFiles::Samples() {
	return _I_Buffer -> GetRingBufferReadAvailable();
}

void    wavFiles::setProgress (int progress, float timelength) {
        fileProgress      -> setValue (progress);
        currentTime       -> display (timelength);
}



