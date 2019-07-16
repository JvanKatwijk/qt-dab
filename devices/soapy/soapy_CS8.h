
#ifndef	SOAPY_CS8
#define	SOAPY_CS8

#include	<SoapySDR/Device.hpp>
#include	<SoapySDR/Formats.hpp>
#include        <SoapySDR/Errors.hpp>
#include	<stdio.h> //printf
#include	<stdlib.h> //free
#include	<complex.h>

#include	"soapy-worker.h";


class	soapy_CS8: public soapyWorker {
public:
	soapy_CS8	(SoapySDR::Device *);
	~soapy_CS8	(void);
int	Samples		(void);
int	getSamples	(std::complex<float> *, int);
void	run		(void);
private:
SoapySDR::Device	*theDevice;
SoapySDR::Stream	*stream;
RingBuffer<int8_t> *theBuffer;
bool	running;
};

#endif

