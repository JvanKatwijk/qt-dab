#
/*
 *    Copyright (C) 2014 .. 2025
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB
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

//
//	Output implementation using portaudio
//
#include	"audiosink.h"
#include	<cstdio>
#include	<QDebug>
#include	<QMessageBox>
#include	<QComboBox>

	audioSink::audioSink	(int16_t latency):
	                           _O_Buffer (16 * 32768) {
int32_t	i;
	this	-> latency	= latency;
	if (latency <= 0)
	   latency = 1;

	this	-> CardRate	= 48000;
	portAudio		= false;
	writerRunning		= false;
	if (Pa_Initialize() != paNoError) {
	   fprintf (stderr, "Initializing Pa for output failed\n");
	   return;
	}

	portAudio	= true;

	qDebug ("Hostapis: %d\n", Pa_GetHostApiCount());

	for (i = 0; i < Pa_GetHostApiCount(); i ++)
	   qDebug ("Api %d is %s\n", i, Pa_GetHostApiInfo (i) -> name);

	numofDevices	= Pa_GetDeviceCount();
	outTable. resize (numofDevices + 1);
	for (i = 0; i < numofDevices; i ++)
	   outTable [i] = -1;
	ostream		= nullptr;
	theMissed	= 0;
	totalSamples	= 1;
}

	audioSink::~audioSink () {
	if ((ostream != nullptr) && !Pa_IsStreamStopped (ostream)) {
	   paCallbackReturn = paAbort;
	   (void) Pa_AbortStream (ostream);
	   while (!Pa_IsStreamStopped (ostream))
	      Pa_Sleep (1);
	   writerRunning = false;
	}

	if (ostream != nullptr)
	   Pa_CloseStream (ostream);

	if (portAudio)
	   Pa_Terminate();
}

bool	audioSink::selectDevice (int16_t idx, const QString &dev) {
PaError err;
int16_t	outputDevice;

	(void)dev;
	if (idx	== 0)
	   return false;

	outputDevice	= outTable [idx];
	if (!isValidDevice (outputDevice)) {
	   fprintf (stderr, "invalid device (%d) selected\n", outputDevice);
	   return false;
	}

	if ((ostream != nullptr) && !Pa_IsStreamStopped (ostream)) {
	   paCallbackReturn = paAbort;
	   (void) Pa_AbortStream (ostream);
	   while (!Pa_IsStreamStopped (ostream))
	      Pa_Sleep (1);
	   writerRunning = false;
	}

	if (ostream != nullptr)
	   Pa_CloseStream (ostream);

	outputParameters. device		= outputDevice;
	outputParameters. channelCount		= 2;
	outputParameters. sampleFormat		= paFloat32;
	outputParameters. suggestedLatency	= 
	                          Pa_GetDeviceInfo (outputDevice) ->
	                                      defaultHighOutputLatency;
	bufSize	= (int)((float)outputParameters. suggestedLatency * latency);
//
//	A small buffer causes more callback invocations, sometimes
//	causing underflows and intermittent output.
	
//	bufSize	= latency * 512;

	outputParameters. hostApiSpecificStreamInfo = nullptr;
//
	fprintf (stderr, "Suggested size for outputbuffer = %d\n", bufSize);
	err = Pa_OpenStream (&ostream,
	                     nullptr,
	                     &outputParameters,
	                     CardRate,
	                     bufSize,
	                     0,
	                     this	-> paCallback_o,
	                     this
	      );

	if (err != paNoError) {
	   qDebug ("Open ostream error\n");
	   return false;
	}
	fprintf (stderr, "stream opened\n");
	paCallbackReturn = paContinue;
	err = Pa_StartStream (ostream);
	if (err != paNoError) {
	   qDebug ("Open startstream error\n");
	   return false;
	}
	fprintf (stderr, "stream started\n");
	writerRunning	= true;
	return true;
}

void	audioSink::restart () {
PaError err;

	if (!Pa_IsStreamStopped (ostream))
	   return;

	_O_Buffer. FlushRingBuffer ();
	totalSamples	= 1;
	paCallbackReturn = paContinue;
	err = Pa_StartStream (ostream);
	if (err == paNoError) 
	   writerRunning	= true;
}

void	audioSink::stop () {
	if (Pa_IsStreamStopped (ostream))
	   return;

//	paCallbackReturn	= paAbort;
	(void)Pa_StopStream	(ostream);
	while (!Pa_IsStreamStopped (ostream))
	   Pa_Sleep (1);
	writerRunning		= false;
	_O_Buffer. FlushRingBuffer ();
}
//
//	helper
bool	audioSink::OutputrateIsSupported (int16_t device, int32_t Rate) {
PaStreamParameters *outputParameters =
	           (PaStreamParameters *)alloca (sizeof (PaStreamParameters)); 

	outputParameters -> device		= device;
	outputParameters -> channelCount	= 2;	/* I and Q	*/
	outputParameters -> sampleFormat	= paFloat32;
	outputParameters -> suggestedLatency	= 0;
	outputParameters -> hostApiSpecificStreamInfo = nullptr;

	return Pa_IsFormatSupported (nullptr, outputParameters, Rate) ==
	                                          paFormatIsSupported;
}
/*
 * 	... and the callback
 */

int	audioSink::paCallback_o (
		const void*			inputBuffer,
                void*				outputBuffer,
		unsigned long			framesPerBuffer,
		const PaStreamCallbackTimeInfo	*timeInfo,
	        PaStreamCallbackFlags		statusFlags,
	        void				*userData) {
RingBuffer<float>	*outB;
float	*outp		= (float *)outputBuffer;
audioSink *ud		= reinterpret_cast <audioSink *>(userData);
uint32_t	actualSize;
uint32_t	i;
	(void)statusFlags;
	(void)inputBuffer;
	(void)timeInfo;
	if (ud -> paCallbackReturn == paContinue) {
	   outB = &((reinterpret_cast < audioSink *> (userData)) -> _O_Buffer);
	   actualSize = outB -> getDataFromBuffer (outp, 2 * framesPerBuffer);
	   ud -> theMissed	+= 2 * framesPerBuffer - actualSize;
	   ud -> totalSamples	+= 2 * framesPerBuffer;
	   for (i = actualSize; i < 2 * framesPerBuffer; i ++)
	      outp [i] = 0;
	}

	return ud -> paCallbackReturn;
}

bool	audioSink::hasMissed	() {
	return true;
}

void	audioSink::samplesMissed	(int &total, int &missed) {
	total		= totalSamples;
	missed		= theMissed;
	theMissed	= 0;
	totalSamples	= 1;
}
//
//	we call this with the amount of floats!!
void	audioSink::audioOutput	(float *b, int32_t amount) {
//	if (_O_Buffer. GetRingBufferWriteAvailable () < amount) {
//	   fprintf (stderr, "%d\n",
//	        2 * amount - _O_Buffer. GetRingBufferReadAvailable ());
//	 }
	if (_O_Buffer. GetRingBufferWriteAvailable () < amount) 
	   amount = _O_Buffer. GetRingBufferWriteAvailable () & ~01;
	_O_Buffer. putDataIntoBuffer (b, amount);
}

QString audioSink::outputChannelwithRate (int16_t ch, int32_t rate) {
const PaDeviceInfo *deviceInfo;
QString name = QString ("");

	if ((ch < 0) || (ch >= numofDevices))
	   return name;

	deviceInfo = Pa_GetDeviceInfo (ch);
	if (deviceInfo == nullptr)
	   return name;
	if (deviceInfo -> maxOutputChannels <= 0)
	   return name;

	if (OutputrateIsSupported (ch, rate))
	   name = QString (deviceInfo -> name);
	return name;
}

int16_t	audioSink::invalidDevice() {
	return numofDevices + 128;
}

bool	audioSink::isValidDevice (int16_t dev) {
	return 0 <= dev && dev < numofDevices;
}

bool	audioSink::selectDefaultDevice () {
	QString str = "default";
	return selectDevice (Pa_GetDefaultOutputDevice (), str);
}

int32_t	audioSink::cardRate() {
	return 48000;
}

bool	audioSink::setupChannels (QComboBox *streamOutSelector) {
uint16_t	ocnt	= 0;
uint16_t	i;

	for (i = 0; i <  numofDevices; i ++) {
	   const QString so = 
	             outputChannelwithRate (i, 48000);
	   qDebug ("Investigating Device %d\n", i);
	
	   if (so != QString ("")) {
	      streamOutSelector -> insertItem (ocnt, so, QVariant (i));
	      outTable [ocnt] = i;
	      qDebug (" (output):item %d wordt stream %d (%s)\n", ocnt , i,
	                      so. toUtf8().data());
	      ocnt ++;
	   }
	}

	qDebug() << "added items to combobox";
	return ocnt > 0;
}

QStringList	audioSink::streams () {
uint16_t	ocnt	= 0;
QStringList res;

	for (int i = 0; i <  numofDevices; i ++) {
	   const QString so = 
	             outputChannelwithRate (i, 48000);
	   qDebug ("Investigating Device %d\n", i);

	   if (so != QString ("")) {
	      res << so;
	      outTable [ocnt] = i;
	      qDebug (" (output):item %d wordt stream %d (%s)\n", ocnt , i,
	                      so. toUtf8().data());
	      ocnt ++;
	   }
	}

	return res;
}

//
int16_t	audioSink::numberofDevices() {
	return numofDevices;
}

