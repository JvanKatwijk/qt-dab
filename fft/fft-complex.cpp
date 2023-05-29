/* 
 * Free FFT and convolution (C)
 * 
 * Copyright (c) 2020 Project Nayuki. (MIT License)
 * https://www.nayuki.io/page/free-small-fft-in-multiple-languages
 * 
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of
 * this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software
 * is furnished to do so, * subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise,
 *   arising from, out of or in connection with the Software
 *   or the use or other dealings in the Software.
 */

#include "fft-complex.h"
#define _USE_MATH_DEFINES
#include	<math.h>
#include	<string.h>


#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif

// Private function prototypes
static size_t reverse_bits (size_t val, int width);
static void *memdup (const void *src, size_t n);


bool	Fft_transform (std::complex<float> vec[], size_t n, bool inverse) {
	if (n == 0)
	   return true;
	else
	if ((n & (n - 1)) == 0)  // Is power of 2
	   return Fft_transformRadix2 (vec, n, inverse);
	else  // More complicated algorithm for arbitrary sizes
	   return Fft_transformBluestein (vec, n, inverse);
}

bool	Fft_transformRadix2 (std::complex<float> vec[],
	                                   size_t n, bool inverse) {
// Length variables
int levels = 0;  // Compute levels = floor(log2(n))

	for (size_t temp = n; temp > 1U; temp >>= 1)
	   levels++;

	if ((size_t)1U << levels != n)
	   return false;  // n is not a power of 2
	
	// Trigonometric tables
	if (SIZE_MAX / sizeof(std::complex<float>) < n / 2)
	   return false;

	std::complex<float> *exptable =
	 (std::complex<float> *)malloc ((n / 2) * sizeof (std::complex<float>));
	if (exptable == NULL)
	   return false;
	for (size_t i = 0; i < n / 2; i++)
	   exptable [i] = std::exp (
	         std::complex<float> (0, (inverse ? 2 : -2) * M_PI * i / n));
	
// Bit-reversed addressing permutation
	for (size_t i = 0; i < n; i++) {
	   size_t j = reverse_bits(i, levels);
	   if (j > i) {
	      std::complex<float> temp = vec[i];
	      vec [i] = vec [j];
	      vec [j] = temp;
	   }
	}
	
// Cooley-Tukey decimation-in-time radix-2 FFT
	for (size_t size = 2; size <= n; size *= 2) {
	   size_t halfsize = size / 2;
	   size_t tablestep = n / size;
	   for (size_t i = 0; i < n; i += size) {
	      for (size_t j = i, k = 0; j < i + halfsize; j++, k += tablestep) {
	         size_t l = j + halfsize;
	         std::complex<float> temp = vec [l] * exptable [k];
	         vec [l] = vec [j] - temp;
	         vec [j] += temp;
	      }
	   }

	   if (size == n)  // Prevent overflow in 'size *= 2'
	      break;
	}
	
	free (exptable);
	return true;
}


bool	Fft_transformBluestein (std::complex<float> vec[],
	                                   size_t n, bool inverse) {
bool status = false;
	
	// Find a power-of-2 convolution length m such that m >= n * 2 + 1
size_t m = 1;
	while (m / 2 <= n) {
	   if (m > SIZE_MAX / 2)
	      return false;
	   m *= 2;
	}
	
// Allocate memory
	if ((SIZE_MAX / sizeof (std::complex<float>) < n) ||
	               (SIZE_MAX / sizeof (std::complex<float>) < m))
	   return false;
	std::complex<float> *exptable =
	        (std::complex<float> *)malloc(n * sizeof (std::complex<float>));
	std::complex<float> *avec =
	     (std::complex<float> *)calloc (m, sizeof (std::complex<float>));
	std::complex<float> *bvec =
	     (std::complex<float> *)calloc (m, sizeof (std::complex<float>));
	std::complex<float> *cvec =
	     (std::complex<float> *) malloc (m * sizeof (std::complex<float>));
	if (exptable == NULL || avec == NULL || bvec == NULL || cvec == NULL)
		goto cleanup;
	
	// Trigonometric tables
	for (size_t i = 0; i < n; i++) {
	   uintmax_t temp = ((uintmax_t)i * i) % ((uintmax_t)n * 2);
	   double angle = (inverse ? M_PI : -M_PI) * temp / n;
	   exptable [i] = std::exp (std::complex<float> (0, angle));
	}
	
	// Temporary vectors and preprocessing
	for (size_t i = 0; i < n; i++)
	   avec [i] = vec [i] * exptable [i];
	bvec [0] = exptable [0];
	for (size_t i = 1; i < n; i++)
	   bvec [i] = bvec [m - i] = conj(exptable[i]);
	
	// Convolution
	if (!Fft_convolve (avec, bvec, cvec, m))
	   goto cleanup;
	
	// Postprocessing
	for (size_t i = 0; i < n; i++)
	   vec[i] = cvec[i] * exptable[i];
	status = true;
	
// Deallocation
cleanup:
	free (exptable);
	free (avec);
	free (bvec);
	free (cvec);
	return status;
}

bool Fft_convolve (const std::complex<float> xvec[],
	           const std::complex<float> yvec[],
	           std::complex<float> outvec [], size_t n) {
	
bool status = false;

	if (SIZE_MAX / sizeof (std::complex<float>) < n)
	   return false;
	std::complex<float> *xv =
	     (std::complex<float> *) memdup (xvec,
	                                     n * sizeof (std::complex<float>));
	std::complex<float> *yv =
	     (std::complex<float> *) memdup (yvec,
	                                     n * sizeof (std::complex<float>));
	if (xv == NULL || yv == NULL)
	   goto cleanup;
	
	if (!Fft_transform(xv, n, false))
	   goto cleanup;
	if (!Fft_transform(yv, n, false))
	   goto cleanup;
	for (size_t i = 0; i < n; i++)
	   xv[i] *= yv[i];
	if (!Fft_transform(xv, n, true))
	   goto cleanup;
// Scaling (because this FFT implementation omits it)
	for (size_t i = 0; i < n; i++)
	   outvec[i] =  std::complex<float> (real (xv[i]) / n, imag (xv [i]) / n);
	status = true;
	
cleanup:
	free(xv);
	free(yv);
	return status;
}

static size_t reverse_bits(size_t val, int width) {
size_t result = 0;
	for (int i = 0; i < width; i++, val >>= 1)
	   result = (result << 1) | (val & 1U);
	return result;
}

static void *memdup (const void *src, size_t n) {
void *dest = malloc (n);
	if (n > 0 && dest != NULL)
	   memcpy (dest, src, n);
	return dest;
}

