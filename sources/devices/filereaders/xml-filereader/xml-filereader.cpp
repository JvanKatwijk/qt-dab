#
/*
 *    Copyright (C) 2013 .. 2024
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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
//#include	<QFileDialog>
#include	<QString>
#include	"xml-filereader.h"
#include	<cstdio>
#include	<cstdlib>
#include	<fcntl.h>
#include	<ctime>

#include	"position-handler.h"
#include	"settingNames.h"

#ifdef	_WIN32
#else
#include	<unistd.h>
#include	<sys/time.h>
#endif

#include	"device-exceptions.h"
#include	"xml-descriptor.h"
#include	"xml-reader.h"

#define	INPUT_FRAMEBUFFERSIZE	8 * 32768
//
//
	xml_fileReader::xml_fileReader (QSettings *s,
	                                const QString &fileName):
	                                 _I_Buffer (INPUT_FRAMEBUFFERSIZE) {
	xmlFilesSettings	= s;
	this -> fileName	= fileName;
	if (fileName == nullptr) 	// should not happen
	   throw device_exception ("no file specified");
//
//	back to normal
	setupUi	(&myFrame);
	setPositionAndSize (s, &myFrame, XMLSETTINGS);
	myFrame. show	();

	theFile	= fopen (fileName. toUtf8 (). data (), "rb");
	if (theFile == nullptr) {
	   throw device_exception ("cannot open " + fileName. toStdString ());
	}
	filenameLabel	-> setText (fileName);
	
	bool	ok	= false;
	theDescriptor. reset (new xmlDescriptor (theFile, &ok));
	if (!ok) {
	   theDescriptor. reset ();
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
	deviceGainLabel		-> hide ();
	if (theDescriptor -> deviceGain > 0) {
	   deviceGainLabel	-> show ();
	   deviceGainLabel	-> setText ("Gain used " +
	                                       QString::number (theDescriptor -> deviceGain));
	}
	typeofUnitLabel		-> setText (theDescriptor -> blockList [0]. typeofUnit);
	modulationtypeLabel	-> setText (theDescriptor -> blockList [0]. modType);

	deviceVersion		-> setText (theDescriptor -> deviceName);
	deviceModel		-> setText (theDescriptor -> deviceModel);
	recorderName		-> setText (theDescriptor -> recorderName);
	recorderVersion		-> setText (theDescriptor -> recorderVersion);
	recordingTime		-> setText (theDescriptor -> recordingTime);
	QString res = QString::number (theDescriptor -> blockList [0]. nrElements);
	nrElementsDisplay	-> display (res);
#ifdef __MINGW32__
	fprintf (stderr, "nrElements = %lld\n",
#else
	fprintf (stderr, "nrElements = %ld\n",
#endif
	             theDescriptor -> blockList [0].nrElements);
	connect (continuousButton, &QPushButton::clicked,
	         this, &xml_fileReader::handle_continuousButton);
	running. store (false);
}

	xml_fileReader::~xml_fileReader	() {
	if (running. load()) {
	   theReader	-> stopReader();
	   while (theReader -> isRunning())
	      usleep (100);
	   theReader. reset ();
	}
	storeWidgetPosition (xmlFilesSettings, &myFrame, XMLSETTINGS);
	if (theFile != nullptr)		// cannot happen
	   fclose (theFile);

	theDescriptor. reset ();
}

bool	xml_fileReader::restartReader (int32_t freq, int skipped) {
	(void)freq;
	(void)skipped;
	if (running. load())
	   return true;
	theReader. reset (new xml_Reader (this, theFile,
	                                  theDescriptor. data (),
	                                  5000, &_I_Buffer));
	running. store (true);
	return true;
}

void	xml_fileReader::stopReader () {
	if (running. load()) {
	   theReader	-> stopReader();
	   while (theReader -> isRunning())
	      usleep (100);
	   theReader. reset ();
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
	currentTime	-> display (samplesRead / (float)SAMPLERATE);
	totalTime	-> display (samplesToRead / (float)SAMPLERATE);
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

QString	xml_fileReader::deviceName	() {
QString res = QString ("xml-file : ") + fileName;
	return res;
}

