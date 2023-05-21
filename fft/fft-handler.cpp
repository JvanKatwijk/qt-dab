

#include	"fft-handler.h"


	fftHandler::fftHandler	(int size, bool dir) {
	this	-> size		= size;
	fftVector_in            = new kiss_fft_cpx [size];
        fftVector_out           = new kiss_fft_cpx [size];
        plan			= kiss_fft_alloc (size, dir, 0, 0);
}

	fftHandler::~fftHandler	() {
	delete fftVector_in;
	delete fftVector_out;
}

void	fftHandler::fft		(std::vector<std::complex<float>> &v) {
	for (int i = 0; i < size; i ++) {
	   fftVector_in [i]. r = real (v [i]);
	   fftVector_in [i]. i = imag (v [i]);
	}
	kiss_fft (plan, fftVector_in, fftVector_out);
	for (int i = 0; i < size; i ++) {
	   v [i] = std::complex<float> (fftVector_out [i]. r,
	                                fftVector_out [i]. i);
	}
}

void	fftHandler::fft		(std::complex<float>  *v) {
	for (int i = 0; i < size; i ++) {
	   fftVector_in [i]. r = real (v [i]);
	   fftVector_in [i]. i = imag (v [i]);
	}
	kiss_fft (plan, fftVector_in, fftVector_out);
	for (int i = 0; i < size; i ++) {
	   v [i] = std::complex<float> (fftVector_out [i]. r,
	                                fftVector_out [i]. i);
	}
}

