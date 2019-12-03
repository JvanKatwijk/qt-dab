
#ifndef	SOAPY_CF32
#define	SOAPY_CF32

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Formats.hpp>
#include        <SoapySDR/Errors.hpp>
#include <stdio.h> //printf
#include <stdlib.h> //free
#include <complex.h>

#include	"soapy-worker.h";


class	soapy_CF32: public soapyWorker {
public:
	soapy_CF32	(SoapySDR::Device *);
	~soapy_CF32	(void);
int	Samples		(void);
int	getSamples	(std::complex<float> *, int);
void	run		(void);
private:
SoapySDR::Device	*theDevice;
SoapySDR::Stream	*stream;
RingBuffer<std::complex<float>> *theBuffer;
bool	running;
};

#endif

