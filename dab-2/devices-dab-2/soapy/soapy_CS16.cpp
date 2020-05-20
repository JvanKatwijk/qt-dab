#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of dab-2
 *
 *    dab-2 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation recorder 2 of the License.
 *
 *    dab-2 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dab-2 if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	<SoapySDR/Device.hpp>
#include	<SoapySDR/Formats.hpp>
#include        <SoapySDR/Errors.hpp>
#include	<stdio.h> //printf
#include	<stdlib.h> //free
#include	<complex.h>
#include	"ringbuffer.h"
#include	"dab-processor.h"
#include	"soapy_CS16.h"

	soapy_CS16::soapy_CS16 (SoapySDR::Device *device,
	                           dabProcessor *base) {
	this	-> theDevice	= device;
	this	-> base		= base;
	std::vector<size_t> xxx;
	stream		= device -> setupStream (SOAPY_SDR_RX,
	                                         "CS16", xxx,
	                                         SoapySDR::Kwargs());
        theDevice  -> activateStream (stream);
	start();
}

	soapy_CS16::~soapy_CS16	(void) {
	running	= false;
	while (isRunning()) {
	   usleep (1000);
	}
	theDevice	-> deactivateStream (stream);
}

void	soapy_CS16::run	(void) {
int     flag    = 0;
long long int timeNS;
int buffer [2048 * 2];
void *const buffs [] = {buffer};
std::complex<float> bufOut [2048];
int	cnt	= 0;

	running	= true;
	while (running) {
           int aa = theDevice -> readStream (stream, buffs, 2048,
	                                       flag, timeNS, 10000);
	   if (aa <= 0)
	      continue;
	   for (int i = 0; i < aa; i ++)
	      bufOut [i] = std::complex<float> (buffer [2 * i] / 8192.0,
	                                        buffer [2 * i + 1] / 8192.0);
	   base -> addSymbol (bufOut, aa);
	}
}

