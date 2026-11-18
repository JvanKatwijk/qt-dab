#
/*
 *    Copyright (C) 2016 .. 2024
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
 *	For eti files
 *	dabsticks 
 */

#include	<QFileDialog>
#include	"eti-files.h"
#include	<cstdio>
#include	<unistd.h>
#include	<cstdlib>
#include	<fcntl.h>
//
#include	<sys/time.h>
#include	<ctime>
#include	"device-exceptions.h"
#include	"eti-reader.h"
#include	"position-handler.h"
#include	"settingNames.h"
#define	INPUT_FRAMEBUFFERSIZE	6144
//
//
	etiFiles::etiFiles (QSettings *s,
	                    const QString &fileName):
	                       _I_Buffer (10 * INPUT_FRAMEBUFFERSIZE) {
	etiFilesSettings	= s;
	this -> fileName	= fileName;
	setupUi	(&myFrame);
	setPositionAndSize (s, &myFrame, ETI_SETTINGS);
	myFrame. setWindowTitle ("Reader for eti (8 bit) files");
	myFrame. show	();

	filePointer	= fopen (fileName. toUtf8 (). data (), "rb");
	if (filePointer == nullptr) {
	   const QString str = QString ("Cannot open file '%1'").arg (fileName);
	   throw device_exception (str. toStdString ());
	}

	nameofFile	-> setText (fileName);
	fseek (filePointer, 0, SEEK_END);
	fileLength      = ftell (filePointer);
        totalTime       -> display ((float)fileLength / 6144 * 0.024);
	fseek (filePointer, 0, SEEK_SET);
	progressSlider	-> setValue (0);
	sliderFree. store (true);
        currentTime     -> display (0);
	typeOfFile	-> setText ("eti file");
	sampleCount	-> setText (QString::number (fileLength));
	running. store (false);
}

	etiFiles::~etiFiles() {
	if (running. load()) {
	   readerTask	-> stopReader();
	   while (readerTask -> isRunning())
	      usleep (100);
	   readerTask. reset ();
	}

	storeWidgetPosition (etiFilesSettings, &myFrame, ETI_SETTINGS);
	if (filePointer != nullptr)
	   fclose (filePointer);
}

bool	etiFiles::restartReader	(int32_t freq, int skipped) {
	(void)freq;
	(void)skipped;
	if (running. load())
	   return true;
	readerTask. reset (new etiReader (this, filePointer, &_I_Buffer));
	running. store (true);
	return true;
}

void	etiFiles::stopReader	() {
	if (running. load()) {
	   readerTask	-> stopReader();
	   while (readerTask -> isRunning())
	      usleep (100);
	   readerTask. reset ();
	   running. store (false);
	}
}

//	size is in I/Q pairs, file contains 8 bits values
int32_t	etiFiles::getSamples	(uint8_t *V, int32_t size) {
int32_t	amount;

	if (filePointer == nullptr)
	   return 0;

	while ((int32_t)(_I_Buffer. GetRingBufferReadAvailable ()) < size)
	   usleep (500);

	amount = _I_Buffer. getDataFromBuffer (V,  size);
	return amount;
}

int32_t	etiFiles::Samples() {
	return _I_Buffer. GetRingBufferReadAvailable ();
}

void	etiFiles::setProgress (int progress, float timelength) {
	progressSlider	-> setValue (progress);
	float xx = (float)fileLength / 6144 * 0.024;
	currentTime	-> display ((float)progress / 100 * xx);
//	currentTime	-> display (timelength / 1000);
}

bool	etiFiles::isFileInput	() {
	return true;
}

uint8_t	etiFiles::deviceType	() {
	return 1;
}

QString	etiFiles::deviceName	() {
QString res	= "eti file: " + fileName;
	return res;
}

