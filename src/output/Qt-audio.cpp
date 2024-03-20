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

	Qt_Audio::Qt_Audio (QSettings *settings):
	                 tempBuffer (8 * 32768) { 
	audioSettings		= settings;
	outputRate		= 48000;	// default
	working. store		(false);
	isInitialized. store	(false);
	newDeviceIndex		= -1;
	initialize_deviceList	();
	initializeAudio (QAudioDeviceInfo::defaultOutputDevice());
}

void	Qt_Audio::initialize_deviceList () {	
	const QAudioDeviceInfo &defaultDeviceInfo =
	                QAudioDeviceInfo::defaultOutputDevice ();
	theList. push_back (defaultDeviceInfo);
	for (auto &deviceInfo:
	       QAudioDeviceInfo::availableDevices(QAudio::AudioOutput)) {
	   if (deviceInfo != defaultDeviceInfo) {
	      theList. push_back (deviceInfo);
	   }
	}
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
//	Note that AudioBase functions have - if needed - the rate
//	converted.  This functions overrides the one in AudioBase
void	Qt_Audio::audioOutput (float *fragment, int32_t size) {
	if (!working. load ())
	   return;
	int aa = tempBuffer. GetRingBufferWriteAvailable ();
	aa	= std::min ((int)(size * sizeof (float)), aa);
	aa	&= ~03;
	tempBuffer. putDataIntoBuffer ((char *)fragment, aa);
	int periodSize = m_audioOutput -> periodSize ();
	char buffer [periodSize];
	while ((m_audioOutput -> bytesFree () >= periodSize) &&
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
	   m_audioOutput. reset (new QAudioOutput (audioFormat));
	   if (m_audioOutput -> error () == QAudio::NoError) {
	      isInitialized. store (true);
//	      fprintf (stderr, "Initialization wens OK\n");
	   }
//	   else
//	     fprintf (stderr, "Audio device gives error\n");
	}
}

void	Qt_Audio::stop () {
	m_audioOutput	-> stop ();
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
	theWorker	= m_audioOutput	-> start ();
	if (m_audioOutput -> error () == QAudio::NoError) {
	   working. store (true);
//	   fprintf (stderr, "Device reports: no error\n");
	}
	else
	   fprintf (stderr, "restart gaat niet door\n");
	int vol		= audioSettings -> value (QT_AUDIO_VOLUME, 50). toInt ();
	m_audioOutput	-> setVolume ((float)vol / 100);
	
}

bool	Qt_Audio::selectDevice	(int16_t index) {
	newDeviceIndex	= index;
	stop ();
	restart ();
	return working. load ();
}

void	Qt_Audio::suspend	() {
	if (!working. load ())
	   return;
	m_audioOutput	-> suspend ();
}

void	Qt_Audio::resume	() {
	if (!working. load ())
	   return;
	m_audioOutput	-> resume ();
}

void	Qt_Audio::setVolume	(int v) {
	audioSettings	-> setValue (QT_AUDIO_VOLUME, v);
	m_audioOutput	-> setVolume ((float)v / 100);
}

