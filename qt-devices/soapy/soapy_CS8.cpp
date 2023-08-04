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

#include	"soapy_CS8.h"

	soapy_CS8::soapy_CS8 (SoapySDR::Device *device):
	                           theBuffer (16 * 32768) {
	this	-> theDevice	= device;
	std::vector<size_t> xxx;
	stream		= device -> setupStream (SOAPY_SDR_RX,
	                                         "CS8", xxx,
	                                         SoapySDR::Kwargs ());
        theDevice  -> activateStream (stream);
	start ();
}

	soapy_CS8::~soapy_CS8	(void) {
	running	= false;
	while (isRunning ()) {
	   usleep (1000);
	}
}

int	soapy_CS8::Samples	(void) {
	return theBuffer. GetRingBufferReadAvailable () / 2;
}

int	soapy_CS8::getSamples	(std::complex<float> *v, int amount) {
int8_t temp [amount * 2];
int	realAmount;
	realAmount	= theBuffer. getDataFromBuffer (temp, amount * 2);
	for (int i = 0; i < realAmount / 2; i ++) 
	   v [i] = std::complex<float> (temp [2 * i] / 127.0,
	                                temp [2 * i + 1] / 127.0);
	return realAmount / 2;
}

void	soapy_CS8::run	(void) {
int     flag    = 0;
long long int timeNS;
int8_t buffer [2048 * 2];
void *const buffs [] = {buffer};
	running	= true;
	while (running) {
           theDevice -> readStream (stream, buffs, 2048, flag, timeNS, 10000);
	   theBuffer. putDataIntoBuffer (buffer, 2 * 2048);
	}
	theDevice	-> deactivateStream (stream);
}

