#
#pragma once

#include	"dab-constants.h"
#include	"fft-handler.h"

//
//	This file is copied (and adkusted to the local styling
//	conventions from github.com/AlbrechtL/welle.io.git
//
//	All rights acknowledged
//
// Coherent demodulation of the DQPSK carriers, an alternative to the
// differential demodulation in OfdmDecoder for weak, stationary signals.
//
// The channel (amplitude and phase of every carrier) is estimated from the
// phase reference symbol (PRS), averaged over frames and smoothed in the time
// domain (the impulse response is shorter than the guard interval). Before
// averaging, the estimate of the previous frames is turned to the timing and
// the phase of the current frame. Within the frame, the phase drift
// (frequency offset) and the timing drift (sample clock) are tracked from
// symbol to symbol. The data symbols are compared with this almost
// noise-free reference instead of with the noisy previous symbol, and the
// soft bits of the phase difference are computed from the probabilities of
// the absolute phases of both symbols. The FIC and the audio need up to
// 2 dB less SNR than with the differential demodulation (more with echoes). A
// channel that changes within a frame (mobile reception) makes it worse.

class	CoherentDemodulator {
public:
	CoherentDemodulator ();
//	Reset the channel estimate (new channel, sync lost, mode switched)
	void	reset();

//	spectrum: the PRS, T_u values in FFT order (carrier k at index
//	k mod T_u). Updates the channel estimate for the following data
//	symbols of this frame.
	void	processPRS (const Complex* spectrum);

//	True once the channel estimate and the noise are known
	bool	ready () const { return frames >= 2; }

    // spectrum: data symbol sym (1 .. L-1) of the frame of the last PRS,
    // guardCorrelation: sum of x[n + T_u] * conj(x[n]) over the guard
    // interval of this symbol (n = 0 .. T_g-1), for the frequency offset.
    // Afterwards llrReal(bin) and llrImag(bin) are the log-likelihood ratios
    // of the two bits of the carrier in FFT bin bin (> 0: bit 0), and
    // decision(bin) its phase difference to the previous symbol (for the
    // constellation diagram).
	void demodulate (const Complex* spectrum, int sym,
	                               Complex guardCorrelation);

	float llrReal (int bin) const { return llrRe[bin]; }
	float llrImag (int bin) const { return llrIm[bin]; }
	Complex decision (int bin) const { return decisions [bin]; }

private:
	void	smoothChannel	();
	void	rotation	(float phase, float slope,
	                        std::vector<Complex>& out) const;
    // Phase per carrier between the new and the averaged estimate
	float	timingRotation	();

	int carriers;
	int T_u;
	int T_s;

    // Carrier number k and FFT bin of carrier c = 0 .. K-1
	std::vector<int> carrier;
	std::vector<int> bin;
	std::vector<Complex> prs;		// PRS of every carrier

	int frames;
	std::vector<Complex> hRaw;	// estimate of the last PRS only
	std::vector<Complex> hAvg;	// averaged over frames
	std::vector<Complex> hSmooth;	// hAvg smoothed in the time domain
	std::vector<Complex> reference;	// conj(hSmooth * prs)
	std::vector<float> noiseVar;	// noise power of every carrier
	std::vector<float> weight;	// 2 / noiseVar, smoothed
	std::vector<Complex> cross;     // hRaw * conj(hAvg)
	float snr;			// mean SNR of the PRS, linear


//	Per carrier: probabilities of the four absolute phases of the
//	previous symbol
	std::vector<std::array<float, 4>> previous;
//	Phase drift since the PRS: common to all carriers, and per carrier
//	number (timing)
	float	commonPhase;
	float	timingPhase;
//	Change of the timing per symbol
	float timingRate;
//	Noise of the data symbols relative to the noise of the PRS
	float dataNoise;
	Complex guardSum;		// guard correlations of this frame
	std::vector<Complex> symbol;	// carriers without channel and PRS
	std::vector<Complex> residual;	// symbol without the decided phases
	std::vector<Complex> rotations;	// see rotation()

	std::vector<float> llrRe;
	std::vector<float> llrIm;
	std::vector<Complex> decisions;
};

