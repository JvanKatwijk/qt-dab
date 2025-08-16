#
/*
 *    Copyright (C) 2016 .. 2025
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

#pragma once

#include	<QThread>
#include	<QMessageBox>
#include	<stdio.h>
#include	"ringbuffer.h"
#include	<stdint.h>
#include	<complex>
#include	<vector>
#include	<atomic>

#include	"device-handler.h"

class	xml_fileReader;
class	xmlDescriptor;

class	xml_Reader:public QThread {
Q_OBJECT
public:
			xml_Reader (xml_fileReader	*mr,
	                            FILE		*f,
	                            xmlDescriptor	*fd,
	                            uint64_t		filePointer,
	                            RingBuffer<std::complex<float>> *b);
			~xml_Reader	();
	void		stopReader	();
	bool		handle_continuousButton	();
private:
	union bitsToFloat {
	   uint32_t bitValue;
	   float    floatValue;
	};
	std::atomic<bool>	continuous;
	FILE		*file;
	xmlDescriptor	*fd;
	uint64_t	filePointer;
	RingBuffer<std::complex<float>> *sampleBuffer;
	xml_fileReader	*parent;
	uint64_t	nrElements;
	uint64_t	samplesToRead;
	std::atomic<bool> running;
	void		run ();
	uint64_t	compute_nrSamples 	(FILE *f, int blockNumber);
	uint64_t	readSamples		(FILE *f, 
	                                       void(xml_Reader::*)(FILE *,
	                                          std::complex<float> *, int));
	void		readElements_IQ		(FILE *f,
	                                         std::complex<float> *, int amount);
	void		readElements_QI		(FILE *f, 
	                                         std::complex<float> *, int amount);
	void		readElements_I		(FILE *f, 
	                                         std::complex<float> *, int amount);
	void		readElements_Q		(FILE *f, 
	                                         std::complex<float> *, int amount);
//
//	for the conversion - if any
	int16_t         convBufferSize;
        int16_t         convIndex;
        std::vector <std::complex<float> >   convBuffer;
        int16_t         mapTable_int   [SAMPLERATE / 1000];
        float           mapTable_float [SAMPLERATE / 1000];

signals:
	void		setProgress		(int, int);
};
