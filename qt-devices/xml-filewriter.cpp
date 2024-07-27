#
/*
 *    Copyright (C) 2014 .. 2019
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
#include	"xml-filewriter.h"
#include	<stdio.h>
#include	<time.h>

struct kort_woord {
	uint8_t byte_1;
	uint8_t byte_2;
};

	xml_fileWriter::xml_fileWriter (FILE *f,
	                                int	nrBits,
	                                QString	container,
	                                int	sampleRate,
	                                int	frequency,	
	                                int	deviceGain,
	                                QString	deviceName,
	                                QString	deviceModel,
	                                QString	recorderVersion) {
uint8_t t	= 0;
	xmlFile		= f;
	this	-> nrBits	= nrBits;
	this	-> container	= container;
	this	-> sampleRate	= sampleRate;
	this	-> frequency	= frequency;
	this	-> deviceName	= deviceName;
	this	-> deviceGain	= deviceGain;
	this	-> deviceModel	= deviceModel;
	this	-> recorderVersion	= recorderVersion;

	for (int i = 0; i < 5000; i ++)
	   fwrite (&t, 1, 1, f);
	int16_t testWord	= 0xFF;

	struct kort_woord *p	= (struct kort_woord *)(&testWord);
	if (p -> byte_1  == 0xFF)
	   byteOrder	= "LSB";
	else
	   byteOrder	= "MSB";
	nrElements	= 0;
	timeString	= QDateTime::currentDateTimeUtc ().toString ("yyyy-MM-dd hh:mm:ss");
}

	xml_fileWriter::~xml_fileWriter	() {
}

void	xml_fileWriter::computeHeader	() {
QString s;
QString	topLine = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	if (xmlFile == nullptr)
	   return;
	s = create_xmltree	();
	fseek (xmlFile, 0, SEEK_SET);
	fprintf (xmlFile, "%s", topLine. toLatin1 (). data ());
	fprintf (xmlFile, "%s", s. toLatin1 (). data ());
//	char * cs = s. toLatin1 (). data ();
//	int len = strlen (cs);
//	fwrite (s. toLatin1 (). data (), 1, len, xmlFile);
}

#define	BLOCK_SIZE	8192
static int16_t buffer_int16 [BLOCK_SIZE];
static int bufferP_int16	= 0;
void	xml_fileWriter::add	(std::complex<int16_t> * data, int count) {
//	nrElements	+= 2 * count;
	for (int i = 0; i < count; i ++) {
	   buffer_int16 [bufferP_int16 ++] = real (data [i]);
	   buffer_int16 [bufferP_int16 ++] = imag (data [i]);
	   if (bufferP_int16 >= BLOCK_SIZE) {
	      fwrite (buffer_int16, sizeof (int16_t), BLOCK_SIZE, xmlFile);
	      bufferP_int16 = 0;
	      nrElements += BLOCK_SIZE;
	   }
	}
}

static uint8_t buffer_uint8 [BLOCK_SIZE];
static int bufferP_uint8	= 0;
void	xml_fileWriter::add	(std::complex<uint8_t> * data, int count) {
//	nrElements	+= 2 * count;
	for (int i = 0; i < count; i ++) {
	   buffer_uint8 [bufferP_uint8 ++] = real (data [i]);
	   buffer_uint8 [bufferP_uint8 ++] = imag (data [i]);
	   if (bufferP_uint8 >= BLOCK_SIZE) {
	      fwrite (buffer_uint8, sizeof (uint8_t), BLOCK_SIZE, xmlFile);
	      bufferP_uint8 = 0;
	      nrElements += BLOCK_SIZE;
	   }
	}
}

static int8_t buffer_int8 [BLOCK_SIZE];
static int bufferP_int8	= 0;
void	xml_fileWriter::add	(std::complex<int8_t> * data, int count) {
//	nrElements	+= 2 * count;
	for (int i = 0; i < count; i ++) {
	   buffer_int8 [bufferP_int8 ++] = real (data [i]);
	   buffer_int8 [bufferP_int8 ++] = imag (data [i]);
	   if (bufferP_int8 >= BLOCK_SIZE) {
	      fwrite (buffer_int8, sizeof (int8_t), BLOCK_SIZE, xmlFile);
	      bufferP_int8 = 0;
	      nrElements += BLOCK_SIZE;
	   }
	}
}

QString	xml_fileWriter::create_xmltree () {
QDomDocument theTree;
QDomElement root	= theTree. createElement ("SDR");

	theTree. appendChild (root);
	QDomElement theRecorder = theTree. createElement ("Recorder");
	theRecorder. setAttribute ("Name", "Qt-DAB");
	theRecorder. setAttribute ("Version", recorderVersion);
	root. appendChild (theRecorder);

	QDomElement theDeviceGain	= theTree. createElement ("deviceGain");
	theDeviceGain. setAttribute ("Value",
	                              QString::number (this -> deviceGain));
	root. appendChild (theDeviceGain);

	QDomElement theDevice = theTree. createElement ("Device");
	theDevice. setAttribute ("Name", deviceName);
	theDevice. setAttribute ("Model", deviceModel);
	root. appendChild (theDevice);
	QDomElement theTime = theTree. createElement ("Time");
	theTime. setAttribute ("Unit", "UTC");
	theTime. setAttribute ("Value", timeString);
//	theTime. setAttribute ("Value", QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss"));
	root. appendChild (theTime);
	QDomElement theSample = theTree. createElement ("Sample");
	QDomElement theRate   = theTree. createElement ("Samplerate");
	theRate. setAttribute ("Unit", "Hz");
	theRate. setAttribute ("Value", QString::number (sampleRate));
	theSample. appendChild (theRate);
	QDomElement theChannels = theTree. createElement ("Channels");
	theChannels. setAttribute ("Bits", QString::number (nrBits));
	theChannels. setAttribute ("Container", container);
	theChannels. setAttribute ("Ordering", byteOrder);
	QDomElement I_Channel = theTree. createElement ("Channel");
	I_Channel. setAttribute ("Value", "I");
	theChannels. appendChild (I_Channel);
	QDomElement Q_Channel = theTree. createElement ("Channel");
	Q_Channel. setAttribute ("Value", "Q");
	theChannels. appendChild (Q_Channel);
	theSample. appendChild (theChannels);
	root. appendChild (theSample);

	QDomElement theDataBlocks	= theTree. createElement ("Datablocks");
	QDomElement theDataBlock	= theTree. createElement ("Datablock");
	theDataBlock. setAttribute ("Count", QString::number (nrElements));
	theDataBlock. setAttribute ("Number", "1");
	theDataBlock. setAttribute ("Unit",  "Channel");
	QDomElement theFrequency	= theTree. createElement ("Frequency");	
	theFrequency. setAttribute ("Value", 
	                                 QString::number (frequency / 1000));
	theFrequency. setAttribute ("Unit", "kHz");
	theDataBlock. appendChild (theFrequency);
	QDomElement theModulation	= theTree. createElement ("Modulation");
	theModulation. setAttribute ("Value", "DAB");
	theDataBlock. appendChild (theModulation);
	theDataBlocks. appendChild (theDataBlock);
	root. appendChild (theDataBlocks);


	return theTree. toString ();
}

