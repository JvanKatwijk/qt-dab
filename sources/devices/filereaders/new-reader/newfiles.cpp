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
#include	"position-handler.h"
#include	"settingNames.h"

#define	__BUFFERSIZE__	8 * 32768

	newFiles::newFiles (QSettings *s,
	                    const QString &fileName):
	                               _I_Buffer (__BUFFERSIZE__),
	                               theReader (fileName) {
	newFilesSettings	= s;
	setupUi (&myFrame);
	setPositionAndSize (s, &myFrame, WAVSETTINGS);
	myFrame. setWindowTitle ("BW64/RIFF reader");
	myFrame. show	();
	this -> fileName	= fileName;
	nameofFile		-> setText (fileName);
	progressSlider		-> setValue (0);
	sliderFree. store (true);
        connect (progressSlider, &QSlider::sliderPressed,
                 this, &newFiles::handle_sliderPressed);
        connect (progressSlider, &QSlider::sliderMoved, 
                 this, &newFiles::handle_sliderMoved);
        connect (progressSlider, &QSlider::sliderReleased,  
                 this, &newFiles::handle_sliderReleased);

	currentTime		-> display (0);
//
//	The reader knows it all
	int64_t fileLength	= theReader. elementCount ();
	QString	fileType	= theReader. fileType ();
	QString deviceName	= theReader. getDevice ();
	totalTime	-> display ((float)fileLength / SAMPLERATE);
	if (deviceName != "") {
	   deviceLabel	-> setText ("Generating device: ");
	   theDevice	-> setText (deviceName);
	}
	typeOfFile	-> setText (fileType);
	sampleCount	-> setText (QString::number (fileLength));
	int32_t	Freq	= theReader. getVFOFrequency ();
	if (Freq > 0)
	   frequencyLabel -> setText (" Freq:" + QString::number (Freq / 1000));
	readerTask. reset ();
}
//
//	Note that running == true <==> readerTask has value assigned

	newFiles::~newFiles	() {
	if (!readerTask. isNull ()) {
	   readerTask	-> stopReader();
	   while (readerTask -> isRunning())
	      usleep (500);
	   storeWidgetPosition (newFilesSettings, &myFrame, WAVSETTINGS);
	   readerTask. reset ();
	}
}

bool	newFiles::restartReader		(int32_t freq, int skipped) {
	(void)freq;
	(void)skipped;
        readerTask. reset (new newReader (this, &theReader, &_I_Buffer));
        return true;
}

void	newFiles::stopReader	() {
	if (!readerTask. isNull ()) {
           readerTask   -> stopReader();
           while (readerTask -> isRunning())
              usleep (100);
	   readerTask. reset ();
        }
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
        if (sliderFree. load ())
	   progressSlider -> setValue (progress);
//	fileProgress	-> setValue (progress);
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


void	newFiles::handle_sliderPressed    () {
        sliderFree. store (false);
}       
  
void	newFiles::handle_sliderMoved      (int value) {
        if (readerTask == nullptr)
           return;
//	fprintf (stderr, "set to %d\n", value);
        readerTask -> handle_progressSlider (value);
}       
        
void	newFiles::handle_sliderReleased   () {
        sliderFree. store (true);
}

	
