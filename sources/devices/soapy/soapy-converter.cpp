#
/*
 *    Copyright (C) 2014 .. 2023
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

#include	"soapy-converter.h"
#include	"dab-constants.h"

static int qualityTable [] = {
        SRC_SINC_BEST_QUALITY, SRC_SINC_MEDIUM_QUALITY,
        SRC_SINC_FASTEST, SRC_ZERO_ORDER_HOLD, SRC_LINEAR};

	soapyConverter::soapyConverter
	                        (RingBuffer<std::complex<float>> *outBuffer){
	this	-> outBuffer	= outBuffer;
}

	soapyConverter::~soapyConverter	() {}

void	soapyConverter::setup (int inputRate, int targetRate) {
	this	-> inputRate	= inputRate;
	this	-> targetRate	= targetRate;
	double ratio            = (double)2048000 / inputRate;
        inputLimit		= 1024;
        outputLimit             = (int)(inputLimit * ratio) + 10;
        int err;
	converter               = src_new (SRC_SINC_FASTEST, 2, &err);
        inBuffer. resize (2 * inputLimit + 20);
        uitBuffer. resize (2 * outputLimit + 20);
        src_data. data_in       = inBuffer. data ();
        src_data. data_out      = uitBuffer. data ();
        src_data. src_ratio     = ratio;
        src_data. end_of_input  = 0;
        inp                     = 0;
}

void	soapyConverter::reset () {
// Flush any accumulated samples before resetting
	if (converter == nullptr)
	   return;
	if (inp > 0 && inputRate != targetRate) {
	   // Process remaining samples with end_of_input flag
	   src_data. input_frames = inp;
	   src_data. output_frames = outputLimit;
	   src_data. end_of_input = 1;  // Signal end of input
	   std::complex<float> *temp = dynVec (std::complex<float>, outputLimit);
	   int res = src_process (converter, &src_data);
	   if (res == 0) {
	      int framesOut = src_data. output_frames_gen;
	      for (int i = 0; i < framesOut; i ++)
	         temp [i] = std::complex<float> (uitBuffer [2 * i],
	                                         uitBuffer [2 * i + 1]);
	      if (framesOut > 0)
	         outBuffer -> putDataIntoBuffer (temp, framesOut);
	   }
	   src_data. end_of_input = 0;
	}
	
	if (converter != nullptr) {
	   src_reset (converter);
	}
	inp = 0;
	src_data. input_frames = 0;
	src_data. output_frames = 0;
	src_data. input_frames_used = 0;
	src_data. output_frames_gen = 0;
	src_data. end_of_input = 0;
}

void	soapyConverter::add	(std::complex<float> *inBuf,
	                                          uint32_t nSamples) {
	if (inputRate == targetRate) {
	   copyDirect (inBuf, nSamples);
	   return;
	}
	for (uint32_t i = 0; i < inputLimit; i ++) {
	   inBuffer [2 * inp] = real (inBuf [i]);
	   inBuffer [2 * inp + 1] = imag (inBuf [i]);
	   inp ++;
	   if (inp >= inputLimit) {
	      convert ();
	      inp = 0;
	   }
	}
}

//	Direct pass-through - check buffer space first
void	soapyConverter::copyDirect (std::complex<float> *inBuf, int nSamples) {
static int dropCount = 0;
	uint32_t available = outBuffer -> GetRingBufferWriteAvailable();
	if (available < (uint32_t)nSamples) {
// Buffer is getting full - drop samples to prevent blocking
	   if ((++dropCount % 100) == 0) {
	      int32_t dropped = nSamples - available;
	      QString report = QString ("Buffer overload, dropped %1").
	                                        arg (QString::number (dropped));
	      reportStatus (report);
	   }
// Write what we can
	   if (available > 0) {
	      outBuffer -> putDataIntoBuffer (inBuf, available);
	   }
	   return;
	}
//	enough space
	outBuffer -> putDataIntoBuffer (inBuf, nSamples);
}

//
void	soapyConverter::convert () {
std::complex<float> *temp  = dynVec (std::complex<float>, outputLimit);
static int dropCount = 0;
	src_data.       input_frames    = inp;
	src_data.       output_frames   = outputLimit;
	int res   = src_process (converter, &src_data);
	if (res != 0) {
	   QString report = QString ("conversion error %1").
	                                  arg (QString (src_strerror (res)));
	   reportStatus (report);
	   return;
	}
	uint32_t framesOut       = src_data. output_frames_gen;

//	Check buffer space before writing converted samples
	uint32_t available = outBuffer -> GetRingBufferWriteAvailable();
	if (available < framesOut) {
	   if ((++dropCount % 100) == 0) {
	      int32_t dropped = framesOut - available;
	      QString report = QString ("Buffer overload, dropped %1").
	                                        arg (QString::number (dropped));
	      reportStatus (report);
	   }
	   if (available > 0) {
	      outBuffer -> putDataIntoBuffer (temp, available);
           }
	   return;
	}
	outBuffer -> putDataIntoBuffer (temp, framesOut);
}
