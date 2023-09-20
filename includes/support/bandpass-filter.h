
#pragma once
/*
 *	The bandfilter is for the complex domain. 
 *	We create a lowpass filter, which stretches over the
 *	positive and negative half, then shift this filter
 *	to the right position to form a nice bandfilter.
 *	For the real domain, we use the Simple BandPass version.
 */

#include	"dab-constants.h"

class	BandPassFIR {
public:
		BandPassFIR	(int16_t filterSize,
	                          int32_t low, int32_t high,
	                          int32_t sampleRate);
		~BandPassFIR	();
std::complex<float>	Pass	(std::complex<float>);
float		Pass		(float);
private:

	int	filterSize;
	int	sampleRate;
	int	ip;
	std::complex<float> *kernel;
	std::complex<float> *buffer;
};

