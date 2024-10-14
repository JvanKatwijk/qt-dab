
#
/*
 *    Copyright (C) 2013 .. 2024
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
#include	<QString>
#include	<cstdio>
#include	<unistd.h>
#include	<cstdlib>
#include	<fcntl.h>
#include	<sys/time.h>
#include	<ctime>
#include	"newfiles.h"

#define	__BUFFERSIZE__	8 * 32768

	newFiles::newFiles (const QString &fileName):
	                               _I_Buffer (__BUFFERSIZE__),
	                               theReader (fileName) {
	setupUi (&myFrame);
	myFrame. show	();
	this -> fileName	= fileName;

	nameofFile		-> setText (fileName);
	fileProgress		-> setValue (0);
	currentTime		-> display (0);
	int64_t fileLength	= theReader. elementCount ();
	totalTime	-> display ((float)fileLength / INPUT_RATE);
	running. store (false);
}
//
//	Note that running == true <==> readerTask has value assigned

	newFiles::~newFiles	() {
	if (running. load()) {
	   readerTask	-> stopReader();
	   while (readerTask -> isRunning())
	      usleep (500);
	   delete readerTask;
	}
}

bool	newFiles::restartReader		(int32_t freq) {
	(void)freq;
	if (running. load())
           return true;
        readerTask      = new newReader (this, &theReader, &_I_Buffer);
        running. store (true);
        return true;
}

void	newFiles::stopReader	() {
       if (running. load()) {
           readerTask   -> stopReader();
           while (readerTask -> isRunning())
              usleep (100);
	   delete readerTask;
        }
        running. store (false);
}

//	size is in I/Q pairs
int32_t	newFiles::getSamples	(std::complex<float> *V, int32_t size) {
int32_t	amount;
	
	while (_I_Buffer. GetRingBufferReadAvailable() < size)
	      usleep (100);

	amount = _I_Buffer. getDataFromBuffer (V, size);
	
	return amount;
}

int32_t	newFiles::Samples() {
	return _I_Buffer. GetRingBufferReadAvailable();
}

void    newFiles::setProgress (int progress, float timelength) {
        fileProgress	-> setValue (progress);
	currentTime	-> display (QString ("%1").arg(timelength, 0, 'f', 1));
}

bool	newFiles::isFileInput	() {
	return true;
}

int	newFiles::getVFOFrequency	() {
	return theReader. getVFOFrequency ();
}

QString	newFiles::deviceName	() {
	return QString (".wav file: ") + fileName;
}


