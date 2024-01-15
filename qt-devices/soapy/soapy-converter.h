
#pragma once

#include	"ringbuffer.h"
#include	<complex>
#include	<vector>
#include	<samplerate.h>

class soapyConverter {
public:
		soapyConverter (RingBuffer<std::complex<float>> *outBuffer);
		~soapyConverter	();
	void	setup		(int, int);
	void	add		(std::complex<float> *, int size);
private:
	int		inputRate;
	int		targetRate;
	SRC_STATE       *converter;
        SRC_DATA        src_data;
        int             inputLimit;
        int             outputLimit;
	int		inp;
        std::vector<float> inBuffer;
        std::vector<float> uitBuffer;
	RingBuffer<std::complex<float>> *outBuffer;
};

