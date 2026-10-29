
#include	"coherent-demodulator.h"
#include	"dab-params.h"
#include	"phasetable.h"

//
//	This file is copied - with asjustments for the local style and
//	conventions - from github.com/AlbrechtL/welle.io.git
//	All rights acknowledged
//
//	CoherentDemodulator: coherent demodulation of the DQPSK carriers, see
//	ofdm-decoder.h.

//
//	Note:
//	Since the class is instantiated in a thread different from
//	the thread calling for the demodulation, the
//	fft handlers cannot be on the class level

//	Weight of the newest PRS in the average of the channel estimate. The
//	first frames are averaged with equal weights.
static const float channelAlpha = 0.15f;

//	Weight of the newest PRS in the average of the noise power
static const float noiseAlpha = 0.2f;

//	The noise power of a carrier is averaged over this many
//	neighbours on each side
static const int noiseSmoothing = 8;

//	Taps of the impulse response that are kept:
//	above this many times the median tap power (noise),
//	and margin taps before and after.
//	A lower threshold lets single noise taps far
//	from the paths widen the window
//	(7D recording at 3 dB SNR: 15 % less audio with 16 than with 64).
static const float tapThreshold = 128.0f;
//static const float tapThreshold = 64.0f;
static const int tapMargin = 16;

//	A frame whose PRS power differs from the average by more than this
//	factor, or whose correlation with the average is below minCorrelation,
//	starts a new average (new channel, gain changed)
static const float maxPowerChange = 4.0f;
static const float minCorrelation = 0.25f;

//	Distances of the carriers whose phase differences give the
//	timing drift from symbol to symbol (precise, and without ambiguity)
static const int trackingDistance = 256;
static const int coarseTrackingDistance = 32;

//	Weight of a timing measurement in the timing and in the timing rate, at
//	high SNR.
//	The measurement is noisy at low SNR: the weights are multiplied
//	by SNR / (SNR + 2) (linear SNR, e.g. 0.5 at 3 dB).

static const float timingGain = 0.5f;
static const float rateGain = 0.03f;

//	Weight of a symbol in the average of the noise of the data symbols
static const float dataNoiseAlpha = 0.05f;

//	Limit of the log-likelihoods of the phases of a carrier
static const float maxLogLikelihood = 15.0f;
static const float minProbability = 1e-15f;

//	Largest timing rate: 0.33 samples per symbol (130 ppm)
static const float maxTimingRate = 0.001f;

//	an alternative for std::polar (with 1 as amplitude);
static
Complex makeComplex (DABFLOAT phase) {
//      Minimax polynomial for sin(x) and cos(x) on [-pi/4, pi/4]
//      Coefficients via Remez algorithm (Sollya)
//      Max |error| < 1.5e-4 for sinus, < 6e-4 for cosinus

        const DABFLOAT x2 =  phase * phase;
        const DABFLOAT s1 =  0.99903142452239990234375f;
        const DABFLOAT s2 = -0.16034401953220367431640625;
        const DABFLOAT sine = phase * (x2 * s2 + s1);

        const DABFLOAT c1 =  0.9994032382965087890625f;
        const DABFLOAT c2 = -0.495580852031707763671875;
        const DABFLOAT c3 =  3.679168224334716796875e-2;
        const DABFLOAT cosine = c1 + x2 * (x2 * c3 + c2);
        return Complex (cosine, sine);
}

	CoherentDemodulator::CoherentDemodulator () {
	frames		= 0;
	snr		= 0;
	commonPhase	= 0;
	timingPhase	= 0;
	timingRate	= 0;
	dataNoise	= 1;
	guardSum	= 0;
	carriers	= get_carriers	();	        
	T_u		= get_T_u	();
	T_s		= get_T_s	();
	hRaw.		resize (carriers);
	hAvg.		resize (carriers);
	hSmooth.	resize (carriers);
	reference.	resize (carriers);
	noiseVar.	resize (carriers);
	weight. 	resize (carriers);
	cross.		resize (carriers);
	previous.	resize (carriers);
	symbol.		resize (carriers);
	residual.	resize (carriers);
	rotations.	resize (carriers);
	llrRe.		resize (T_u);
	llrIm.		resize (T_u);
	decisions.	resize (T_u);

	phaseTable table;
	for (int k = -carriers / 2; k <= carriers / 2; k++) {
	   if (k == 0)
	      continue;
           carrier. push_back (k);
           bin. push_back ((k + T_u) % T_u);
//	   prs. push_back (makeComplex (float (table. get_Phi (k))));
	   prs. push_back (std::polar (1.0f, float (table. get_Phi (k))));
	}
}

// exp(-j (phase + slope * k)) for every carrier k, by multiplying
// (std::polar for every carrier is too slow)
void	CoherentDemodulator::rotation (float phase,
	                               float slope,
	                               std::vector<Complex>& out) const {
const Complex step = std::polar(1.0f, -slope);
Complex r = std::polar(1.0f, -(phase + slope * carrier[0]));

	out [0] = r;
	for (int c = 1; c < carriers; c++) {
	   for (int k = carrier [c - 1]; k < carrier [c]; k++)
            r *= step;
	   out [c] = r;
	}
}

void	CoherentDemodulator::reset () {
	frames		= 0;
	timingRate	= 0;
	dataNoise	= 1;
}

// exp(-j (phase + slope * k)) for every carrier k, by multiplying
// (std::polar for every carrier is too slow)

void	CoherentDemodulator::processPRS (const Complex* spectrum) {
float rawPower = 0;
float avgPower = 0;

	for (int c = 0; c < carriers; c++) {
	   hRaw [c] = spectrum [bin [c]] * std::conj (prs [c]);
	   rawPower += std::norm (hRaw [c]);
	   avgPower += std::norm (hAvg [c]);
	}

	if ((frames > 0) && 
	    ((rawPower > maxPowerChange * avgPower) ||
                       (maxPowerChange * rawPower < avgPower))) 
        frames = 0;

	if (frames == 0) {
	   hAvg = hRaw;
	   frames = 1;
	   smoothChannel ();
	   return;
	}

//	Turn the average to the timing and the phase of this frame: a
//	different FFT window position gives a phase that grows linearly with
//	the carrier number, the oscillator a common phase
	const float phasePerCarrier = timingRotation ();
	Complex common (0, 0);
	for (int c = 0; c < carriers; c++) {
	   hAvg [c] *= std::polar (1.0f, phasePerCarrier * carrier [c]);
	   common += hRaw [c] * std::conj(hAvg [c]);
	}

	const Complex commonRotation = std::polar (1.0f, std::arg (common));

//	A different channel (other ensemble, other antenna) most likely
//	hardly correlates with the average;
//	at 0 dB SNR the correlation is still about 0.7
	if (std::abs (common) <
	           minCorrelation * std::sqrt (rawPower * avgPower)) {
	   hAvg		= hRaw;
	   frames	= 1;
	   smoothChannel ();
	   return;
	}

//	Equal weights for the first frames, then exponential
	const float alpha = std::max (channelAlpha, 1.0f / (frames + 1));

//	The difference between the new and the averaged estimate contains the
//	noise of both: the average of n frames still has 1/n of the noise
//	power, the exponential average alpha / (2 - alpha)
	const float remaining = std::max (channelAlpha / (2 - channelAlpha), 1.0f / frames);
	const float noiseScale = 1.0f / (1.0f + remaining);
	const float beta = std::max (noiseAlpha, 1.0f / frames);
	for (int c = 0; c < carriers; c++) {
	   hAvg[c] *= commonRotation;
	   const float e = std::norm(hRaw[c] - hAvg[c]) * noiseScale;
	   noiseVar [c] = frames == 1 ? e : (1 - beta) * noiseVar[c] + beta * e;
	   hAvg [c] = (1 - alpha) * hAvg[c] + alpha * hRaw[c];
	} 
	frames++;

	smoothChannel ();

//	Noise power, averaged over neighbouring carriers. The soft bits of a
//	carrier are weighted with 2 / noise power.
	float signalSum = 0;
	float noiseSum = 0;
	for (int c = 0; c < carriers; c++) {
	   const int first = std::max (0, c - noiseSmoothing);
	   const int last = std::min (carriers - 1, c + noiseSmoothing);
	   float sum = 0;
	   for (int j = first; j <= last; j++)
	      sum += noiseVar [j];
	   const float n = sum / (last - first + 1);
	   weight [c] = n > 0 ? 2.0f / n : 0;
	   signalSum += std::norm (hSmooth [c]);
	   noiseSum += n;
	}
	snr = noiseSum > 0 ? signalSum / noiseSum : 0;
}

float	CoherentDemodulator::timingRotation () {
//	Cross-spectrum of the new and the averaged estimate.
//	A timing difference of d samples makes it turn
//	by a = 2 pi d / T_u per carrier.
//	The phase differences of neighbouring carriers would be
//	biased by echoes whose strength changes a little from frame to frame;
//	the maximum of the cross-correlation is not.
fftHandler fftBackwards (T_u, true);
	for (int c = 0; c < carriers; c++)
	   cross [c] = hRaw [c] * std::conj (hAvg [c]);

	Complex *time = dynVec (Complex, T_u);
	for (int i = 0; i < T_u; i ++)
	   time [i] = Complex (0, 0);
	for (int c = 0; c < carriers; c++)
	   time [bin [c]] = cross[c];
	fftBackwards. fft (time);
	int peak = 0;
	for (int n = 1; n < T_u; n++) 
	   if (std::norm (time [n]) > std::norm (time [peak]))
	      peak = n;

//	The IFFT has its maximum at n = -a T_u / (2 pi)
	const int lag = peak < T_u / 2 ? peak : peak - T_u;
	const float step = float (2 * M_PI / T_u);

//	Fraction of a sample: ternary search for the maximum of the
//	correlation within the main lobe
	auto correlation = [&](float a) {
	   rotation (0, a, rotations);
	   Complex sum (0, 0);
	   for (int c = 0; c < carriers; c++)
	      sum += cross [c] * rotations [c];
	   return std::norm (sum);
	};

	float low = (-lag - 1) * step;
	float high = (-lag + 1) * step;
	for (int i = 0; i < 16; i++) {
	   const float m1 = low + (high - low) / 3;
	   const float m2 = high - (high - low) / 3;
	   if (correlation (m1) < correlation (m2))
	      low = m1;
	   else
	      high = m2;
	}
	return (low + high) / 2;
}

//	The impulse response is short compared to T_u: keep only its taps,
//	which removes most of the noise of the estimate
void	CoherentDemodulator::smoothChannel () {
Complex *time  	= dynVec (Complex, T_u);
fftHandler fftBackwards (T_u, true);
fftHandler fftForward (T_u, false);
	for (int i = 0; i < T_u; i ++)
	   time [i] = Complex (0, 0);
	for (int c = 0; c < carriers; c++)
	   time [bin [c]] = hAvg [c];
	fftBackwards. fft (time);

	std::vector<float> power(T_u);
	for (int n = 0; n < T_u; n ++)
	   power [n] = std::norm (time [n]);
	const int peak =
	    std::max_element (power. begin (), power. end()) - power. begin ();

	std::vector<float> sorted = power;
	std::nth_element (sorted. begin (),
	                  sorted. begin () + T_u / 2, sorted. end ());
	const float threshold = tapThreshold * sorted [T_u / 2];

//	Taps above the noise within +-T_u/4 of the strongest one
	int first = 0;
	int last = 0;
	for (int d = -T_u / 4; d < T_u / 4; d++) {
	   if (power[(peak + d + T_u) % T_u] > threshold) {
	      first = std::min (first, d);
	      last = std::max(last, d);
	   }
	}

	first -= tapMargin;
	last += tapMargin;

	std::vector<Complex> kept (T_u, Complex (0, 0));
	std::vector<bool> inWindow (T_u, false);
	for (int d = first; d <= last; d++) {
	   const int n	= (peak + d + T_u) % T_u;
	   kept [n]	= time [n];
	   inWindow [n] = true;
	}

	Complex *freq = dynVec (Complex, T_u);
	std::copy (kept. begin (), kept. end (), freq);
	fftForward. fft (freq);
	for (int c = 0; c < carriers; c++)
	   hSmooth [c] = freq [bin [c]];

//	The window also smooths the edges of the band and the unused centre
//	carrier into their empty neighbours. Divide by the response of a flat
//	channel to the same window.
	for (int i = 0; i < T_u; i ++)
	   time [i] = Complex (0, 0);
	for (int c = 0; c < carriers; c++)
	   time [bin [c]] = 1;
	fftBackwards. fft (time);

	for (int n = 0; n < T_u; n ++)
	   freq [n] = inWindow [n] ? time [n] : Complex (0, 0);
	fftForward. fft (freq);

	for (int c = 0; c < carriers; c++) {
	   const float flat = std::real (freq [bin [c]]);
           if (flat > 0.1f)
	      hSmooth[c] /= flat;
	   reference [c] = std::conj (hSmooth [c] * prs [c]);
	}
}

void	CoherentDemodulator::demodulate (const Complex* spectrum,
	                                 int sym,
	                                 Complex guardCorrelation) {
	if (sym == 1) {
//	The previous symbol is the PRS, we know that one
	   for (auto& p : previous)
	      p = {1, 0, 0, 0};
           commonPhase = 0;
           timingPhase = 0;
           guardSum = 0;
	}

//	Symbol sym has the phases of the PRS plus sym * pi/4 plus a
//	multiple of pi/2.
//	Remove the channel, the PRS and sym * pi/4, then the four
//	possible phases are 0, pi/2, pi, 3 pi/2.
//
//	The channel estimate is from the PRS.
//	Until the end of the frame a residual frequency	offset
//	turns all carriers (with a common phase);

//	The error of the sample clock moves the symbols in time
//	(a phase growing linearly with the carrier number).
//	Both are predicted, then the nearest phases are
//	computed and what is left is measured.
//
//	The decisions can't tell a phase rate from the same rate plus pi/2
//	per symbol (about 200 Hz),
//	but then every phase difference would be wrong.
//	So the phase rate comes from the correlation of the guard
//	intervals of this frame, which has no such ambiguity. The timing
//	rate (sample clock) is the same in every frame and is averaged.

	guardSum	+= guardCorrelation;
	commonPhase	+= std::arg (guardSum) * T_s / T_u;
	timingPhase	+= timingRate;
	rotation (commonPhase + float(sym * M_PI / 4), timingPhase, rotations);
	auto& u = symbol;

	for (int c = 0; c < carriers; c++) {
	   u [c] = spectrum [bin [c]] * reference [c] * rotations [c];
//	Remove the computed phase, weighted with the reliability
	   const Complex v = u [c];
           Complex r;
	   if (std::abs (real (v)) >= std::abs (imag (v)))
	      r = real (v) > 0 ? v : -v;
	   else
	      r = imag (v) > 0 ?
	               Complex (imag (v), -real (v)) :
	               Complex (-imag (v), real (v));
	   residual [c] = r * weight [c];
	}

//	Timing: phase differences of carriers far apart are precise,
//	but the decisions fold every phase into +-pi/4,
//	so a slope of pi/2 over the distance looks like none.
//	Carriers closer together resolve this.
	auto slope = [&](int distance) {
	   Complex sum (0, 0);
	   for (int c = 0; c + distance < carriers; c++) {
	      const int c2 = c + distance;
	      if (carrier [c2] - carrier [c] == distance)
	         sum += residual [c2] * std::conj (residual [c]);
	   }
	   return std::arg(sum) / distance;
	};

	const float coarse	= slope (coarseTrackingDistance);
	const float step	= float (M_PI / 2) / trackingDistance;
	float measured		= slope (trackingDistance);
	measured	+= step * std::round((coarse - measured) / step);

//	The timing changes slowly, its measurement is noisy at low SNR:
//	follow it slowly, otherwise the noise adds up from symbol to symbol
	const float dataSnr = snr / std::max (1.0f, dataNoise);
	const float reliability = dataSnr / (dataSnr + 2);
	const float a = reliability * timingGain * measured;
	rotation (0, a, rotations);
	Complex common(0, 0);
	for (int c = 0; c < carriers; c++)
	   common += residual [c] * rotations [c];
	const float phase = std::arg(common);

	timingPhase += a;
	commonPhase += phase;
	timingRate += reliability * rateGain * measured;
	timingRate = std::max (-maxTimingRate,
	                       std::min (maxTimingRate, timingRate));

	rotation (phase, a, rotations);
//	The noise of the PRS doesn't contain the interference between
//	symbols (echoes longer than the guard interval):
//	the PRS follows the almost empty null symbol.
//	Measure the noise of the data symbols as the distance to
//	the nearest phases, relative to the noise of the PRS,
//	and use the larger of both.
	const float noiseFactor = std::max (1.0f, dataNoise);
	float relativeError = 0;
	int counted = 0;
//
//	we go for the main decoding loop
	for (int c = 0; c < carriers; c++) {
	   const Complex v = u [c] * rotations [c];
	   const float w = weight [c] / noiseFactor;

	   const float h2 = std::norm (hSmooth [c]);
	   if ((h2 > 0) && (weight[c] > 0)) {
	      const Complex nearest =
	           std::abs (v. real ()) >= std::abs (v. imag ()) ?
                      Complex (v. real () > 0 ? h2 : -h2, 0) :
	              Complex (0, v.imag() > 0 ? h2 : -h2);
// |H|^2 times the noise power of the PRS
	      relativeError += std::norm (v - nearest) * weight[c] / (2 * h2);
	      counted++;
	   }

//	Probabilities of the absolute phases 0, pi/2, pi, 3 pi/2, from
//	their log-likelihoods w * Re(v), w * Im(v), -w * Re(v), -w * Im(v).
//	Both scaled down if necessary, so that exp () doesn't overflow and
//	the products below stay normal floats (denormal ones are very slow);
//	log-likelihood ratios of 30 are far above what the soft
//	bits can hold.
           float x = w * v.real();
           float y = w * v.imag();
           const float largest = std::max(std::abs(x), std::abs(y));
           if (largest > maxLogLikelihood) {
               x *= maxLogLikelihood / largest;
               y *= maxLogLikelihood / largest;
           }
           const float ex = std::exp (x);
           const float ey = std::exp (y);
           std::array<float, 4> p = {ex, ey, 1 / ex, 1 / ey};
	   const float norm = 1 / (p[0] + p[1] + p[2] + p[3]);
	   for (auto& pm : p)
	      pm = std::max(pm * norm, minProbability);

//	Phase difference n * pi/2 + pi/4 to the previous symbol
	   const auto& q = previous [c];
	   float pd [4];
	   for (int n = 0; n < 4; n++) {
	      pd [n] = 0;
	      for (int m = 0; m < 4; m++)
	         pd [n] += q [m] * p [(m + n) & 3];
	   }

//	The differences pi/4 and 7 pi/4 have a positive real part (bit 0),
//	pi/4 and 3 pi/4 a positive imaginary part
	   const float tiny = 1e-30f;
	   const float lre = std::log ((pd [0] + pd [3] + tiny) /
	                                           (pd [1] + pd [2] + tiny));
	   const float lim = std::log ((pd [0] + pd [1] + tiny) /
	                                           (pd [2] + pd [3] + tiny));
	   const int b = bin [c];
	   llrRe [b] = lre;
	   llrIm [b] = lim;

// exp(j (pi/4 - m pi/2)) for the most probable previous phase m
	   static const Complex toDifference [4] = {
            {float (M_SQRT1_2), float(M_SQRT1_2)},
	    {float(M_SQRT1_2), -float(M_SQRT1_2)},
            {-float(M_SQRT1_2), -float(M_SQRT1_2)},
	    {-float(M_SQRT1_2), float(M_SQRT1_2)}
	   };
	   const int mPrev = std::max_element (q.begin(), q.end()) - q.begin();
	   decisions [b] = v * toDifference [mPrev];
	   previous[c] = p;
	}

	if (counted > 0)
	   dataNoise = (1 - dataNoiseAlpha) * dataNoise +
	               dataNoiseAlpha * relativeError / counted;
}
