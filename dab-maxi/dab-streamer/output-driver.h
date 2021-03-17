

#ifndef	__OUTPUT_DRIVER__
#define	__OUTPUT_DRIVER__

#include	<stdint.h>
#include	<complex>

class	outputDriver {
public:
		outputDriver	(void);
virtual		~outputDriver	(void);
virtual void    sendSample	(std::complex<float>);
};

#endif

