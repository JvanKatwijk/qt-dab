#
#pragma once

#include	<stdint.h>
#include	<complex>
#include	"ringbuffer.h"

class	processParams {
public:
	uint8_t	dabMode;
	int16_t	threshold;
	int16_t	diff_length;
	int16_t	tii_delay;
	int16_t	tii_depth;
	int16_t	echo_depth;
	int16_t	bitDepth;
	RingBuffer<float>	* responseBuffer;
	RingBuffer<std::complex<float>> * spectrumBuffer;
	RingBuffer<std::complex<float>> * iqBuffer;
	RingBuffer<std::complex<float>> * tiiBuffer;
	RingBuffer<std::complex<float>> * nullBuffer;
	RingBuffer<float>		* snrBuffer;
	RingBuffer<uint8_t> *frameBuffer;
};


