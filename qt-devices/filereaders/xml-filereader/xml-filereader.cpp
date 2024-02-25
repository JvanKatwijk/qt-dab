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
 */
#include	<QFileDialog>
#include	"xml-filereader.h"
#include	<cstdio>
#include	<unistd.h>
#include	<cstdlib>
#include	<fcntl.h>
#include	<sys/time.h>
#include	<ctime>

#include	"device-exceptions.h"
#include	"xml-descriptor.h"
#include	"xml-reader.h"

#define	INPUT_FRAMEBUFFERSIZE	8 * 32768
//
//
	xml_fileReader::xml_fileReader (const QString &fileName):
	                      _I_Buffer (INPUT_FRAMEBUFFERSIZE) {
	setupUi	(&myFrame);
	myFrame. show	();

	this -> fileName	= fileName;
	if (fileName == nullptr) 
	   throw device_exception ("no file specified");

	theFile	= fopen (fileName. toUtf8 (). data (), "rb");
	if (theFile == nullptr) {
	   throw device_exception ("cannot open " + fileName. toStdString ());
	}
	
	bool	ok	= false;
	filenameLabel	-> setText (fileName);
	theDescriptor	= new xmlDescriptor (theFile, &ok);
	if (!ok) {
	   throw device_exception (fileName. toStdString () + "no xml file");
	}

	fileProgress		-> setValue (0);
        currentTime		-> display (0);
	samplerateDisplay	-> display (theDescriptor -> sampleRate);
	nrBitsDisplay		-> display (theDescriptor -> bitsperChannel);
	containerLabel		-> setText (theDescriptor -> container);
	iqOrderLabel		-> setText (theDescriptor -> iqOrder);
	byteOrderLabel		-> setText (theDescriptor -> byteOrder);
	frequencyDisplay	-> display (theDescriptor -> blockList [0]. frequency / 1000.0);
	typeofUnitLabel		-> setText (theDescriptor -> blockList [0]. typeofUnit);
	modulationtypeLabel	-> setText (theDescriptor -> blockList [0]. modType);

	deviceVersion		-> setText (theDescriptor -> deviceName);
	deviceModel		-> setText (theDescriptor -> deviceModel);
	recorderName		-> setText (theDescriptor -> recorderName);
	recorderVersion		-> setText (theDescriptor -> recorderVersion);
	recordingTime		-> setText (theDescriptor -> recordingTime);

	nrElementsDisplay	-> display (theDescriptor -> blockList [0]. nrElements);
	fprintf (stderr, "nrElements = %d\n",
	             theDescriptor -> blockList [0].nrElements);
//	connect (continuousButton, SIGNAL (clicked ()),
//	         this, SLOT (handle_continuousButton ()));
	running. store (false);
}

	xml_fileReader::~xml_fileReader	() {
	if (running. load()) {
	   theReader	-> stopReader();
	   while (theReader -> isRunning())
	      usleep (100);
	   delete theReader;
	}
	if (theFile != nullptr)
	   fclose (theFile);

	delete	theDescriptor;
}

bool	xml_fileReader::restartReader (int32_t freq) {
	(void)freq;
	if (running. load())
	   return true;
	theReader	= new xml_Reader (this,
	                                 theFile,
	                                 theDescriptor,
	                                 5000,
	                                 &_I_Buffer);
	running. store (true);
	return true;
}

void	xml_fileReader::stopReader () {
	if (running. load()) {
	   theReader	-> stopReader();
	   while (theReader -> isRunning())
	      usleep (100);
	   delete theReader;
	   theReader = nullptr;
	}
	running. store (false);
}

//	size is in "samples"
int32_t	xml_fileReader::getSamples	(std::complex<float> *V,
	                                 int32_t size) {

	if (theFile == nullptr)		// should not happen
	   return 0;

	while ((int32_t)(_I_Buffer. GetRingBufferReadAvailable()) < size)
	   usleep (1000);

	return _I_Buffer. getDataFromBuffer (V, size);
}

int32_t	xml_fileReader::Samples	() {
	if (theFile == nullptr)
	   return 0;
	return _I_Buffer. GetRingBufferReadAvailable();
}

void	xml_fileReader::setProgress (int samplesRead, int samplesToRead) {
	fileProgress	-> setValue ((float)samplesRead / samplesToRead * 100);
	currentTime	-> display (samplesRead / 2048000.0);
	totalTime	-> display (samplesToRead / 2048000.0);
}

int	xml_fileReader::getVFOFrequency	() {
	return theDescriptor -> blockList [0]. frequency;
}

void	xml_fileReader::handle_continuousButton () {
	if (theReader == nullptr)
	   return;
	bool cont = theReader -> handle_continuousButton ();
	continuousButton -> setText (cont ? "continuous ON" :
	                                    "continuous off");
}

bool	xml_fileReader::isFileInput	() {
	return true;
}

