#
/*
 *    Copyright (C) 2014 .. 2023
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

#include	<stdio.h>
#include	"Qt-audio.h"
#include	<QSettings>
#include	"settingNames.h"
#include	"settings-handler.h"

class	RadioInterface;

	Qt_Audio::Qt_Audio (RadioInterface *mr, 
	                       QSettings *settings):
	                      tempBuffer (8 * 32768) { 
	(void)mr;
	audioSettings		= settings;
	outputRate		= 48000;	// default
	working. store		(false);
	isInitialized. store	(false);
	newDeviceIndex		= -1;
//
//	allDevices		= new QMediaDevices (nullptr);
//	deviceList	 	= allDevices -> audioOutputs ();
//	for (auto &deviceInfo : deviceList)
//	      theList. push_back (deviceInfo. description (),
//	                          QVariant:: fromValue (deviceInfo));
	initialize_deviceList	();
	initializeAudio (QAudioDeviceInfo::defaultOutputDevice());
}

bool	Qt_Audio::is_QtAudio		() {
	return true;
}

void	Qt_Audio::initialize_deviceList () {	
	QAudioFormat audioFormat;
	audioFormat. setSampleRate      (outputRate);
        audioFormat. setChannelCount    (2);
        audioFormat. setSampleSize      (sizeof (float) * 8);
        audioFormat. setCodec           ("audio/pcm");
        audioFormat. setByteOrder       (QAudioFormat::LittleEndian);
        audioFormat. setSampleType      (QAudioFormat::Float);

	const QAudioDeviceInfo &defaultDeviceInfo =
	                QAudioDeviceInfo::defaultOutputDevice ();
	if (!defaultDeviceInfo. isFormatSupported (audioFormat))
	   fprintf (stderr, "Default device does not support 48000\n");
	else {
	   fprintf (stderr, "Default device does support 48000\n");
	   theList. push_back (defaultDeviceInfo);
	}
	for (auto &deviceInfo:
	       QAudioDeviceInfo::availableDevices (QAudio::AudioOutput)) {
	   if (deviceInfo != defaultDeviceInfo) {
	      if (deviceInfo. isFormatSupported (audioFormat))
	         theList. push_back (deviceInfo);
	   }
	}

//	fprintf (stderr, "The devicelist \n");
//	for (auto & listEl: theList)
//	   fprintf (stderr, "%s\n",  listEl. deviceName (). toLatin1 (). data ());;

//	fprintf (stderr, "Length of deviceList %d\n",  theList. size ());
	if (theList. size () == 0)
	   throw (22);
}

	Qt_Audio::~Qt_Audio () {
}

QStringList	Qt_Audio::streams	() {
QStringList nameList;
	for (auto & listEl: theList)
	   nameList << listEl. deviceName ();
	return nameList;
}
//
//	Note that - by convention - all audio samples here
//	are in a rate 48000
void	Qt_Audio::audioOutput (float *fragment, int32_t size) {
	if (!working. load ())
	   return;
	int aa = tempBuffer. GetRingBufferWriteAvailable ();
	aa	= std::min ((int)(size * sizeof (float)), aa);
	aa	&= ~03;
	tempBuffer. putDataIntoBuffer ((char *)fragment, aa);
	int periodSize = m_audioSink -> periodSize ();
	char buffer [periodSize];
	while ((m_audioSink -> bytesFree () >= periodSize) &&
	       (tempBuffer. GetRingBufferReadAvailable () >= periodSize)) {
	   tempBuffer. getDataFromBuffer (buffer, periodSize);
	   theWorker	-> write (buffer, periodSize);
	}
}

void	Qt_Audio::initializeAudio(const QAudioDeviceInfo &deviceInfo) {
	audioFormat. setSampleRate	(outputRate);
	audioFormat. setChannelCount	(2);
	audioFormat. setSampleSize	(sizeof (float) * 8);
	audioFormat. setCodec		("audio/pcm");
	audioFormat. setByteOrder	(QAudioFormat::LittleEndian);
	audioFormat. setSampleType	(QAudioFormat::Float);

	if (!deviceInfo. isFormatSupported (audioFormat)) {
           audioFormat = deviceInfo.nearestFormat (audioFormat);
	}
	isInitialized. store (false);
	if (deviceInfo. isFormatSupported (audioFormat)) {
	   m_audioSink. reset (new QAudioOutput (audioFormat));
	   if (m_audioSink -> error () == QAudio::NoError) {
	      isInitialized. store (true);
//	      fprintf (stderr, "Initialization went OK\n");
	   }
//	   else
//	     fprintf (stderr, "Audio device gives error\n");
	}
}

void	Qt_Audio::stop () {
	m_audioSink	-> stop ();
	working. store (false);
	isInitialized. store (false);
}

void	Qt_Audio::restart	() {
//	fprintf (stderr, "Going to restart with %d\n", newDeviceIndex);
	if (newDeviceIndex < 0)
	   return;
	initializeAudio (theList. at (newDeviceIndex));
	if (!isInitialized. load ()) {
	   fprintf (stderr, "Init failed for device %d\n", newDeviceIndex);
	   return;
	}
	theWorker	= m_audioSink	-> start ();
	if (m_audioSink -> error () == QAudio::NoError) {
	   working. store (true);
	   fprintf (stderr, "Device reports: no error\n");
	}
	else
	   fprintf (stderr, "restart gaat niet door\n");
	int vol		= value_i (audioSettings, SOUND_HANDLING,
	                                  QT_AUDIO_VOLUME, 50);
	m_audioSink	-> setVolume ((float)vol / 100);
}

bool	Qt_Audio::selectDevice	(int16_t index, const QString &s) {
	(void)index;
	for (int i = 0; i < (int)(theList. size ()); i ++)
	   if (theList [i]. deviceName () == s)
	      newDeviceIndex = i;
	stop ();
	restart ();
	return working. load ();
}

void	Qt_Audio::suspend	() {
	if (!working. load ())
	   return;
	tempBuffer. FlushRingBuffer ();
	m_audioSink	-> suspend ();
}

void	Qt_Audio::resume	() {
	if (!working. load ())
	   return;
	tempBuffer. FlushRingBuffer ();
	m_audioSink	-> resume ();
}

void	Qt_Audio::setVolume	(int v) {
	store (audioSettings, SOUND_HANDLING, QT_AUDIO_VOLUME, v);
	m_audioSink	-> setVolume ((float)v / 100);
}

bool	Qt_Audio::hasMissed	() {
	return false;
}

void	Qt_Audio::samplesMissed	(int &total, int & missed) {
	total	= 1;
	missed	= 0;
}
