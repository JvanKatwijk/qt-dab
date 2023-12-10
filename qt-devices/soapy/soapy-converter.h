
#pragma once

#include	"ringbuffer.h"
#include	<complex>
#include	<vector>


class soapyConverter {
public:
		soapyConverter (RingBuffer<std::complex<float>> *outBuffer);
		~soapyConverter	();
	void	setup		(int, int);
	void	add		(std::complex<float> *, int size);
private:
	int	inputRate;
	int	targetRate;
	RingBuffer<std::complex<float>> *outBuffer;
	std::vector<std::complex<float>> convBuffer;
	int 	convBufferSize;
	int	convIndex;
	std::vector<int>	mapTable_int;
	std::vector<float>	mapTable_float;
};

