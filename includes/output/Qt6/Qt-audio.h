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
#pragma once

//
//	Note: this class is parameterized on the Qt major version
//	In this implementatiom, the audio output drivers have to
//	implement the interface class "audioPlayer"
#include        <stdio.h>
#include        <QAudioOutput>
#include        <QStringList>
#include        "dab-constants.h"
#include        "audio-player.h"
#include        <vector>
#include        <atomic>
#include        "ringbuffer.h"

#include        <QIODevice>
#include        <QScopedPointer>
#include        <QAudioSink>
#include        <QComboBox>
#include        <QMediaDevices>
#include	"Qt-audiodevice.h"

class		QSettings;
class		RadioInterface;
//
//	The public interface is common to both the Qt5 and Qt6 version
class	Qt_Audio: public audioPlayer {
Q_OBJECT
public:
			Qt_Audio	(RadioInterface *, QSettings *);
			~Qt_Audio	();
	void		stop		();
	void		restart		();
	void		suspend		();
	void		resume		();
	void		audioOutput	(float *, int32_t);
	QStringList	streams		();
	bool		selectDevice	(int16_t, const QString &);

	bool		hasMissed	();
	void		samplesMissed	(int &, int &);
private:
	RingBuffer<char> tempBuffer;
	QSettings	*audioSettings;
	std::atomic<bool> working;
        int             newDeviceIndex;
	RadioInterface	*mr;
	Qt_AudioDevice	*theIODevice;
	QAudioFormat	m_settings;
	QList<QAudioDevice>     outputDevices;
	QAudioSink	*m_audioSink;
        int32_t         outputRate;
public slots:
	void		state_changed		(const QAudio::State);
	void		updateAudioDevices	();
public slots:
	void		setVolume	(int);
};
