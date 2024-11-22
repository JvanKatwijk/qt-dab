#
/*
 *    Copyright (C)  2014 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB.
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

#include	<QString>
#include	<QStringList>
#include	<vector>
#include	"dab-constants.h"
#include	<portaudio.h>
#include	<cstdio>
#include	"audio-player.h"
#include	"ringbuffer.h"

class	QComboBox;

class	audioSink : public audioPlayer {
public:
	                audioSink		(int16_t);
			~audioSink		();
	bool		setupChannels		(QComboBox *);
	QStringList	streams			();
	void		stop			();
	void		restart			();
	bool		selectDevice		(int16_t, const QString &);
	bool		selectDefaultDevice	();
	void		audioOutput		(float *, int32_t);
	void		samplesMissed		(int &, int &);
	bool		hasMissed		();
private:
	int16_t		numberofDevices		();
	QString		outputChannelwithRate	(int16_t, int32_t);
	int16_t		invalidDevice		();
	bool		isValidDevice		(int16_t);
	int32_t		cardRate		();

	bool		OutputrateIsSupported	(int16_t, int32_t);
	int32_t		CardRate;
	int16_t		latency;
	int32_t		size;
	bool		portAudio;
	bool		writerRunning;
	int16_t		numofDevices;
	int		paCallbackReturn;
	int16_t		bufSize;
	PaStream	*ostream;
	RingBuffer<float>	_O_Buffer;
	PaStreamParameters	outputParameters;

	int		theMissed;
	int		totalSamples;
	std::vector<int16_t>	outTable;
	QStringList	*InterfaceList;
protected:
static	int		paCallback_o	(const void	*input,
	                                 void		*output,
	                                 unsigned long	framesperBuffer,
	                                 const PaStreamCallbackTimeInfo *timeInfo,
					 PaStreamCallbackFlags statusFlags,
	                                 void		*userData);
};


