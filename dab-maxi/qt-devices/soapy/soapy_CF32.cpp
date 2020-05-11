#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation recorder 2 of the License.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	<SoapySDR/Device.hpp>
#include	<SoapySDR/Formats.hpp>
#include        <SoapySDR/Errors.hpp>
#include	<stdio.h> //printf
#include	<stdlib.h> //free
#include	<complex.h>
#include	"ringbuffer.h"

#include	"soapy_CF32.h"

	soapy_CF32::soapy_CF32 (SoapySDR::Device *device) {
	this	-> theDevice	= device;
	theBuffer		= new RingBuffer<std::complex<float>> (32 * 32768);
	std::vector<size_t> xxx;
	stream		= device -> setupStream (SOAPY_SDR_RX,
	                                         "CF32", xxx,
	                                         SoapySDR::Kwargs());
	start();
}

	soapy_CF32::~soapy_CF32	(void) {
	running	= false;
	while (isRunning()) {
	   usleep (1000);
	}
	theDevice	-> deactivateStream (stream);
	delete theBuffer;
}

int	soapy_CF32::Samples	(void) {
	return theBuffer	-> GetRingBufferReadAvailable();
}

int	soapy_CF32::getSamples	(std::complex<float> *v, int amount) {
int	realAmount;
	realAmount	= theBuffer -> getDataFromBuffer (v, amount);
	return realAmount;
}

void	soapy_CF32::run	(void) {
int     flag    = 0;
long long int timeNS;
std::complex<float> buffer [4096];
void *const buffs [] = {buffer};
	running	= true;

        theDevice  -> activateStream (stream);
	while (running) {
           int aa = theDevice -> readStream (stream, buffs,
	                                       4096, flag, timeNS, 10000);
	   if (aa > 0)
	      theBuffer -> putDataIntoBuffer (buffer, aa);
	}
}

