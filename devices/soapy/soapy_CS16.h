
#ifndef	SOAPY_CS16
#define	SOAPY_CS16

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Formats.hpp>
#include        <SoapySDR/Errors.hpp>
#include <stdio.h> //printf
#include <stdlib.h> //free
#include <complex.h>

#include	"soapy-worker.h";


class	soapy_CS16: public soapyWorker {
public:
	soapy_CS16	(SoapySDR::Device *);
	~soapy_CS16	(void);
int	Samples		(void);
int	getSamples	(std::complex<float> *, int);
void	run		(void);
private:
SoapySDR::Device	*theDevice;
SoapySDR::Stream	*stream;
RingBuffer<int16_t> *theBuffer;
bool	running;
};

#endif

