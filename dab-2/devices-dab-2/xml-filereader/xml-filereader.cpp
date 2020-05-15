#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of dab-2
 *
 *    dab-2 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    dab-2 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dab-2; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"xml-filereader.h"
#include	<cstdio>
#include	<unistd.h>
#include	<cstdlib>
#include	<fcntl.h>
#include	<sys/time.h>
#include	<ctime>

#include	"xml-descriptor.h"
#include	"xml-reader.h"
#include	"radio.h"
#include	"dab-processor.h"

#define	INPUT_FRAMEBUFFERSIZE	8 * 32768
//
//
	xml_fileReader::xml_fileReader (RadioInterface	*mr,
	                                dabProcessor	*base,
	                                QString		&f) {
	(void)mr;
	this	-> base	= base;
	fileName	= f;
	myFrame		= new QFrame;
	setupUi	(myFrame);
	theFile	= fopen (f. toUtf8 (). data(), "rb");
	if (theFile == nullptr) {
	   fprintf (stderr, "file %s cannot open\n",
	                                   f. toUtf8(). data());
	   perror ("file ?");
	   delete myFrame;
	   throw (31);
	}
	
	bool	ok	= false;
	filenameLabel	-> setText (f);
	theDescriptor	= new xmlDescriptor (theFile, &ok);
	if (!ok) {
	   fprintf (stderr, "%s probably not an xml file\n",
	                               f. toUtf8 (). data ());
	   delete myFrame;
	   throw (32);
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
	connect (continuousButton, SIGNAL (clicked ()),
	         this, SLOT (handle_continuousButton ()));
	running. store (false);
	totalOffset	= 0;
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

	delete	myFrame;
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
	                                 base);
	running. store (true);
	totalOffset	= 0;
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

void	xml_fileReader::handle_Value	(int offset,
	                                 float lowVal, float highVal) {
	totalOffset	+= offset;
	theReader	-> setOffset (totalOffset);
	offsetDisplay	-> display (totalOffset);
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
	theReader -> handle_continuousButton ();
}

int16_t	xml_fileReader::bitDepth	() {
	return theDescriptor	-> bitsperChannel;
}

void	xml_fileReader::hide	() {
	myFrame	-> hide ();
}

void	xml_fileReader::show	() {
	myFrame	-> show ();
}

bool	xml_fileReader::isHidden	() {
	return myFrame -> isHidden ();
}

