
#ifndef	__SOAPY_WORKER__
#define	__SOAPY_WORKER__
#include	<QThread>
#include	<complex>

class	soapyWorker: public QThread {
public:
		soapyWorker	(void);
virtual		~soapyWorker	(void);
virtual	int	Samples		(void);
virtual	int	getSamples	(std::complex<float> *, int);
};

#endif


