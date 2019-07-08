#include <SoapySDR/Device.hpp>
#include <SoapySDR/Formats.hpp>
#include        <SoapySDR/Errors.hpp>
#include <stdio.h> //printf
#include <stdlib.h> //free
#include <complex.h>
#include	"ringbuffer.h"

#include	"soapy_CS16.h"

	soapy_CS16::soapy_CS16 (SoapySDR::Device *device) {
	this	-> theDevice	= device;
	theBuffer		= new RingBuffer<int16_t> (16 * 32768);
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
	delete theBuffer;
}

int	soapy_CS16::Samples	(void) {
	return theBuffer	-> GetRingBufferReadAvailable() / 2;
}

int	soapy_CS16::getSamples	(std::complex<float> *v, int amount) {
int16_t temp [amount * 2];
int	realAmount;
	realAmount	= theBuffer -> getDataFromBuffer (temp, amount * 2);
	for (int i = 0; i < realAmount / 2; i ++) 
	   v [i] = std::complex<float> (temp [2 * i] / 8191.0,
	                                temp [2 * i + 1] / 8191.0);
	return realAmount / 2;
}

void	soapy_CS16::run	(void) {
int     flag    = 0;
long long int timeNS;
int buffer [2048 * 2];
void *const buffs [] = {buffer};
	running	= true;
int	cnt	= 0;
	while (running) {
           theDevice -> readStream (stream, buffs, 2048, flag, timeNS, 10000);
	   theBuffer -> putDataIntoBuffer (buffer, 2 * 2048);
	}
}

