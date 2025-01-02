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

#include	"Qt-audiodevice.h"
#include	<QMediaDevices>

class	RadioInterface;

//
//	The format is known, everything mapped onto 2 channel 48000
	Qt_Audio::Qt_Audio (RadioInterface *mr,
	                    QSettings *settings) :
//	                                  audioPlayer (mr),
	                                  tempBuffer (16 * 32768) {
	this	-> mr		= mr;
	this	-> audioSettings	= settings;
	newDeviceIndex		= 0;
	
        m_settings. setSampleRate (48000);
        m_settings. setSampleFormat (QAudioFormat::Float);
        m_settings. setChannelCount (2);
        m_settings. setChannelConfig(QAudioFormat::ChannelConfigStereo);

	const QAudioDevice &defaultDevice =
                               QMediaDevices::defaultAudioOutput ();
        outputDevices. push_back (defaultDevice);

	QMediaDevices *m_devices = new QMediaDevices ();
        for (auto &deviceInfo : QMediaDevices::audioOutputs ()) {
           if ((deviceInfo != defaultDevice) &&
                   deviceInfo. isFormatSupported (m_settings)) {
              outputDevices. push_back (deviceInfo);
           }
        }
	connect (m_devices, &QMediaDevices::audioOutputsChanged,
                 this, &Qt_Audio::updateAudioDevices);

	m_audioSink	= nullptr;
	theIODevice	= new Qt_AudioDevice (mr, &tempBuffer);

//	currentVolume	= value_i (audioSettings, SOUND_HANDLING, 
//	                                      QT_AUDIO_VOLUME, 50);
}

	Qt_Audio::~Qt_Audio	() {
	if (m_audioSink != nullptr) {
	   m_audioSink -> stop ();
	   delete m_audioSink;
	}
	theIODevice	-> close ();
	delete theIODevice;
}

QStringList     Qt_Audio::streams       () {
QStringList nameList;
        for (auto & listEl: outputDevices)
           nameList << listEl. description ();
        return nameList; 
}

void	Qt_Audio::restart	() {
	if (newDeviceIndex < 0)
	   return;
	if (m_audioSink != nullptr) {
	   delete m_audioSink;
	   m_audioSink	= nullptr;
	   if (theIODevice != nullptr) {
	      theIODevice	-> close ();
	      delete theIODevice;
	   }
	}
	QAudioDevice currentDevice
	                = outputDevices. at (newDeviceIndex);
	m_audioSink	= new QAudioSink (currentDevice, m_settings);

	m_audioSink	-> setBufferSize (8 * 32768);
	connect (m_audioSink, &QAudioSink::stateChanged,
                 this, &Qt_Audio::state_changed);
//
//	and run off
	theIODevice	= new Qt_AudioDevice (mr, &tempBuffer);
	theIODevice	-> start ();
	m_audioSink	-> start (theIODevice);
	QtAudio::Error err = m_audioSink -> error ();
	fprintf (stderr, "Errorcode %d\n", (int)(err));
	int currentVolume	= value_i (audioSettings, SOUND_HANDLING,
	                                      QT_AUDIO_VOLUME, 50);
	qreal linearVolume =
                       QAudio::convertVolume (currentVolume / qreal (100),
                                              QAudio::LogarithmicVolumeScale,
                                              QAudio::LinearVolumeScale);
	m_audioSink	-> setVolume (linearVolume);
}

void	Qt_Audio::stop	() {
	if (m_audioSink == nullptr)
	   return;
	m_audioSink	-> stop ();
	theIODevice	-> close ();
}

void	Qt_Audio::suspend	() {
	if (m_audioSink == nullptr)
	   return;
	if (m_audioSink -> state () == QAudio::ActiveState) 
           m_audioSink -> suspend ();
}

void	Qt_Audio::resume	() {
	if (m_audioSink == nullptr)
	   return;
	if ((m_audioSink -> state () == QAudio::SuspendedState) ||
            (m_audioSink -> state () == QAudio::StoppedState)) 
           m_audioSink -> resume ();
}
//      Note that - by convention - all audio samples here
//      are in a rate 48000
void    Qt_Audio::audioOutput (float *fragment, int32_t size) {
	if (m_audioSink == 0)
	   return;
	tempBuffer. putDataIntoBuffer ((char *)fragment,
	                                       sizeof (float) * size);
}

void	Qt_Audio::state_changed (const QAudio::State newState) {
	switch (newState) {
	   case QAudio::ActiveState:
	      fprintf (stderr, "State: active\n");
	      break;
	   case QAudio::IdleState:
	      fprintf (stderr, "State: Idle\n");
	      if (m_audioSink -> error () != QAudio::NoError)
	         fprintf (stderr, "we found %d \n", (int)(m_audioSink -> error ()));
	      break;
	   case QAudio::StoppedState:
	      fprintf (stderr, "State: Stopped\n");
	      break;	
	   case QAudio::SuspendedState:
	      fprintf (stderr, "State: suspended\n");
	      break;
	}
}

bool	Qt_Audio::selectDevice (int16_t index, const QString &deviceName) {
	(void)deviceName;
	if (m_audioSink != nullptr) {
	   theIODevice -> stop ();
           m_audioSink -> stop ();
           m_audioSink -> disconnect (this);
	}
	newDeviceIndex = index;
	for (int i = 0; i < outputDevices. size (); i ++) {
	   if (outputDevices. at (i). description () == deviceName) {
	      newDeviceIndex = i;
	      break;
	   }
	}
	fprintf (stderr, "deviceIndex %d (%d)\n", newDeviceIndex, index);
	fprintf (stderr, "Going for a restart\n");
	restart ();
	return true;
}

void    Qt_Audio::setVolume (int value) {
        qreal linearVolume =
                       QAudio::convertVolume (value / qreal (100),
                                              QAudio::LogarithmicVolumeScale,
                                              QAudio::LinearVolumeScale);
	store (audioSettings, SOUND_HANDLING,
	                                QT_AUDIO_VOLUME, value);
	if (m_audioSink == nullptr)
	   return;
        m_audioSink -> setVolume (linearVolume);
}

void	Qt_Audio::updateAudioDevices () {
const QAudioDevice &defaultDevice =
                               QMediaDevices::defaultAudioOutput ();
	outputDevices. resize (0);
        outputDevices. push_back (defaultDevice);

        for (auto &deviceInfo : QMediaDevices::audioOutputs ()) {
           if ((deviceInfo != defaultDevice) &&
                   deviceInfo. isFormatSupported (m_settings)) {
              outputDevices. push_back (deviceInfo);
           }
	}
//	hier nog bij dat we de HUI moeten informren
}

bool	Qt_Audio::hasMissed	() {
	return true;
}

void	Qt_Audio::samplesMissed	(int &total, int & missed) {
	if (theIODevice != nullptr)
	   theIODevice -> samplesMissed (total, missed);
}

