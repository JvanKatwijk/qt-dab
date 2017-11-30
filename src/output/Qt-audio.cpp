#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB 
 *    Many of the ideas as implemented in Qt-DAB are derived from
 *    other work, made available through the GNU general Public License.
 *    All copyrights of the original authors are acknowledged.
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
 */

#include	<stdio.h>
#include	"Qt-audiodevice.h"
#include	"Qt-audio.h"

	Qt_Audio::Qt_Audio (void) {
	Buffer		= new RingBuffer<float> (8 * 32768);
	outputRate	= 48000;	// default
	theAudioDevice	= new Qt_AudioDevice (Buffer, this);
	theAudioOutput	= NULL;
	setParams (outputRate);
}

	Qt_Audio::~Qt_Audio(void) {
	if (theAudioOutput != NULL)
	   delete	theAudioOutput;
	delete	theAudioDevice;
	delete	Buffer;
}
//
//	Note that audioBase functions have - if needed - the rate
//	converted.  This functions overrides the one in audioBase
void	Qt_Audio::audioOutput (float *fragment, int32_t size) {
	if (theAudioDevice != NULL) {
	   Buffer -> putDataIntoBuffer (fragment, 2 * size);
	}
}

void	Qt_Audio::setParams (int outputRate) {
	if (theAudioOutput != NULL) {
	   delete theAudioOutput;
	   theAudioOutput = NULL;
	}

	AudioFormat. setSampleRate	(outputRate);
	AudioFormat. setChannelCount	(2);
	AudioFormat. setSampleSize	(sizeof (float) * 8);
	AudioFormat. setCodec		("audio/pcm");
	AudioFormat. setByteOrder	(QAudioFormat::LittleEndian);
	AudioFormat. setSampleType	(QAudioFormat::Float);

	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice ());
	if (!info. isFormatSupported(AudioFormat)) {
	   fprintf (stderr, "Audio: Sorry, format cannot be handled");
	   return;
	}

	theAudioOutput = new QAudioOutput(AudioFormat, this);
	connect (theAudioOutput, SIGNAL (stateChanged (QAudio::State)),
	         this, SLOT (handleStateChanged (QAudio::State)));

	restart ();
	currentState = theAudioOutput -> state ();
}

void	Qt_Audio::stop (void) {
	if (theAudioDevice == NULL)
	   return;
	theAudioDevice	-> stop ();
	theAudioOutput	-> stop();
}

void	Qt_Audio::restart	(void) {
	if (theAudioDevice == NULL)
	   return;
	theAudioDevice	-> start ();
	theAudioOutput	-> start (theAudioDevice);
}

void	Qt_Audio::handleStateChanged (QAudio::State newState) {
	currentState = newState;
	switch (currentState) {
	   case QAudio::IdleState:
	      theAudioOutput -> stop();
	      break;

	   default:
	      break;
	}
}
