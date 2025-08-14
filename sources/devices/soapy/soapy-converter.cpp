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

static int qualityTable [] = {
        SRC_SINC_BEST_QUALITY, SRC_SINC_MEDIUM_QUALITY,
        SRC_SINC_FASTEST, SRC_ZERO_ORDER_HOLD, SRC_LINEAR};

	soapyConverter::soapyConverter
	                        (RingBuffer<std::complex<float>> *outBuffer){
	this	-> outBuffer	= outBuffer;
}

	soapyConverter::~soapyConverter	() {}

void	soapyConverter::setup (int inputRate, int targetRate) {
	double ratio            = (double)2048000 / inputRate;
        inputLimit              = 4096;
        outputLimit             = inputLimit * ratio;
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

void	soapyConverter::add	(std::complex<float> *inBuf, int nSamples) {
std::complex<float> temp [targetRate / 1000];

	if (inputRate == targetRate) {
	   outBuffer -> putDataIntoBuffer (inBuf, nSamples);
	   return;
	}
//
//	alas, a real conversion is required
	for (int i = 0; i < nSamples; i ++) {
	   inBuffer [2 * inp ] = real (inBuf [i]);
	   inBuffer [2 * inp + 1] = imag (inBuf [i]);
	   inp ++;
	   if (inp < inputLimit)
	      continue;
	   src_data.       input_frames    = inp;
	   src_data.       output_frames   = outputLimit;
	   int res   = src_process (converter, &src_data);
	   if (res != 0) {
	      fprintf (stderr, "error %s\n", src_strerror (res));
	      return;
	   }
	   for (inp = 0;
	        inp < inputLimit - src_data. input_frames_used; inp ++)
	      inBuffer [inp] = inBuffer [src_data. input_frames_used + inp];
	   int framesOut       = src_data. output_frames_gen;
	   for (int i = 0; i < framesOut; i ++)
	      temp [i] = std::complex<float> (uitBuffer [2 * i],
	                                      uitBuffer [2 * i + 1]);
	   outBuffer -> putDataIntoBuffer (temp, framesOut);
	}
}



