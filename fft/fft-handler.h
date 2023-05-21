
#ifndef	__FFT_HANDLER__
#define	__FFT_HANDLER__

#include	<complex>
#include	<vector>
#include	"kiss_fft.h"

class	fftHandler {
public:
			fftHandler	(int size, bool);
			~fftHandler	();
	void		fft (std::vector<std::complex<float>> &);
	void		fft (std::complex<float> *);
private:
	int		size;
	kiss_fft_cfg	plan;
	kiss_fft_cpx	*fftVector_in;
	kiss_fft_cpx	*fftVector_out;
};
#endif
