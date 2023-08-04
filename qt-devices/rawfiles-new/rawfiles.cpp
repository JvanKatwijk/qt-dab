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
 *
 * 	File reader:
 *	For the (former) files with 8 bit raw data from the
 *	dabsticks 
 */
#include	"rawfiles.h"
#include	<cstdio>
#include	<unistd.h>
#include	<cstdlib>
#include	<fcntl.h>
//
#include	<sys/time.h>
#include	<ctime>

#include	"raw-reader.h"

#define	INPUT_FRAMEBUFFERSIZE	8 * 32768
//
//
	rawFiles::rawFiles (QString f):
	   myFrame (nullptr),
	   _I_Buffer (INPUT_FRAMEBUFFERSIZE) {
	fileName	= f;
	setupUi	(&myFrame);
	myFrame. show	();
	filePointer	= fopen (f. toUtf8(). data(), "rb");
	if (filePointer == nullptr) {
	   fprintf (stderr, "file %s cannot open\n",
	                                   f. toUtf8(). data());
	   perror ("file ?");
	   throw (31);
	}
	nameofFile	-> setText (f);
	fseek (filePointer, 0, SEEK_END);
	int64_t fileLength      = ftell (filePointer);
        totalTime       -> display ((float)fileLength / (2048000 * 2));
	fseek (filePointer, 0, SEEK_SET);
	fileProgress    -> setValue (0);
        currentTime     -> display (0);

	running. store (false);
}

	rawFiles::~rawFiles() {
	if (running. load()) {
	   readerTask	-> stopReader();
	   while (readerTask -> isRunning())
	      usleep (100);
	   delete readerTask;
	}
	if (filePointer != nullptr)
	   fclose (filePointer);
}

bool	rawFiles::restartReader	(int32_t freq) {
	(void)freq;
	if (running. load())
	   return true;
	readerTask	= new rawReader (this, filePointer, &_I_Buffer);
	running. store (true);
	return true;
}

void	rawFiles::stopReader() {
	if (running. load()) {
	   readerTask	-> stopReader();
	   while (readerTask -> isRunning())
	      usleep (100);
	   delete readerTask;
	}
	running. store (false);
}

//	size is in I/Q pairs, file contains 8 bits values
int32_t	rawFiles::getSamples	(std::complex<float> *V, int32_t size) {
int32_t	amount;

	if (filePointer == nullptr)
	   return 0;

	while ((int32_t)(_I_Buffer. GetRingBufferReadAvailable ()) < size)
	   usleep (500);

	amount = _I_Buffer. getDataFromBuffer (V,  size);
	return amount;
}

int32_t	rawFiles::Samples() {
	return _I_Buffer. GetRingBufferReadAvailable ();
}

void	rawFiles::setProgress (int progress, float timelength) {
	fileProgress      -> setValue (progress);
	currentTime       -> display (timelength);
}

void	rawFiles::show		() {
	myFrame. show ();
}

void	rawFiles::hide		() {
	myFrame. hide ();
}

bool	rawFiles::isHidden	() {
	return myFrame. isHidden ();
}

bool	rawFiles::isFileInput	() {
	return true;
}

