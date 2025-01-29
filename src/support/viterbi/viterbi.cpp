#
/*
 *    Copyright (C) 2014 .. 2024
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB 
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	This version of a Viterbi decoder is based on the viterbi decoder
 *	applied in DAB-Radio
 *	which can be found in
 *	https://github.com/williamyang98/DAB-Radio
 *	All rights graefully acknowledged
 */

#include	"./viterbi.h"
#include	<assert.h>
#include	<stddef.h>
#include	<stdint.h>
#include	<stdio.h>
#include	<limits>
#include	"dab-constants.h"
#include	<memory>

#include <emmintrin.h>
#include <xmmintrin.h>
#include <mmintrin.h>

#include	"viterbi-includes/convolutional_encoder_lookup.h"
#include	"viterbi-includes/viterbi_branch_table.h"
#include	"viterbi-includes/viterbi_decoder_config.h"
#include	"viterbi-includes/viterbi_decoder_core.h"

constexpr size_t K = viterbi::m_constraint_length;
constexpr size_t R = viterbi::m_code_rate;
const uint8_t code_polynomial [4] = {109, 79, 83, 109};
constexpr int16_t soft_decision_low	= -127;
constexpr int16_t soft_decision_high	= +127;

// in DAB we use same configuration for all decoders
static ViterbiDecoder_Config <uint16_t> create_decoder_config () {
	const uint16_t max_error =
	               uint16_t (soft_decision_high-soft_decision_low) *
	                       uint16_t(viterbi::m_code_rate);
	const uint16_t error_margin =
	               max_error * uint16_t(5u);
	ViterbiDecoder_Config<uint16_t> config;
	config. soft_decision_max_error = max_error;
	config. initial_start_error =
	               std::numeric_limits<uint16_t>::min ();
	config. initial_non_start_error =
	               config.initial_start_error + error_margin;
	config. renormalisation_threshold =
	               std::numeric_limits<uint16_t>::max() - error_margin;
	return config; 
}

static const auto decoder_config = create_decoder_config();

//	Share the branch table for all decoders
static const auto decoder_branch_table =
	                ViterbiBranchTable<K,R,int16_t>(
	                                code_polynomial,
	                                soft_decision_high,
	                                soft_decision_low
);

// Wrap compile time selected decoder for forward declaration
	#if defined(__ARCH_X86__)
	      #include "viterbi-includes/x86/viterbi_decoder_avx_u16.h"
	      using Decoder_avx = ViterbiDecoder_AVX_u16<K,R>;
	      #include "viterbi-includes/x86/viterbi_decoder_avx_u16.h"
	      using Decoder_sse = ViterbiDecoder_AVX_u16<K,R>;
	      #include "viterbi-includes/viterbi_decoder_scalar.h"
	      using Decoder_scalar = ViterbiDecoder_Scalar<K,R,uint16_t,int16_t>;
//	   #if defined(__AVX2__)
//	      #pragma message("DAB_VITERBI_DECODER using x86 AVX2")
//	      #include "viterbi-includes/x86/viterbi_decoder_avx_u16.h"
//	      using Decoder = ViterbiDecoder_AVX_u16<K,R>;
//	   #elif defined(__SSE4_1__)
//	      #pragma message("DAB_VITERBI_DECODER using x86 SSE4.1")
//	      #include	<emmintrin.h>
//	      #include "viterbi-includes/x86/viterbi_decoder_sse_u16.h"
//	      using Decoder = ViterbiDecoder_SSE_u16<K,R>;
//	   #else
//	      #pragma message("DAB_VITERBI_DECODER using x86 SCALAR")
//	      #include "viterbi-includes/viterbi_decoder_scalar.h"
//	      using Decoder = ViterbiDecoder_Scalar<K,R,uint16_t,int16_t>;
//	   #endif
	#elif defined(__ARCH_AARCH64__)
	   #pragma message("DAB_VITERBI_DECODER using ARM AARCH64 NEON")
	   #include "viterbi-includes/arm/viterbi_decoder_neon_u16.h"
	   using Decoder = ViterbiDecoder_NEON_u16<K,R>;
	#else
	   #pragma message("DAB_VITERBI_DECODER using crossplatform SCALAR")
	   #include "viterbi-includes/viterbi_decoder_scalar.h"
	   using Decoder_scalar = ViterbiDecoder_Scalar<K,R,uint16_t,int16_t>;
	#endif

	using Core = ViterbiDecoder_Core<K,R,uint16_t,int16_t>;

class DAB_Viterbi_Decoder_Internal: public Core {
public:
    template <typename ... U>
    DAB_Viterbi_Decoder_Internal(U&& ... args): Core(std::forward<U>(args)...) {}
};

	viterbi::viterbi (int frameBits, bool f, uint8_t cpuSupport):
	                              m_accumulated_error(0) {
	(void)f;
	this	-> cpuSupport = cpuSupport;
	m_decoder =
	          std::make_unique<DAB_Viterbi_Decoder_Internal>(
	                                           decoder_branch_table,
	                                           decoder_config
	          );
	this	-> frameBits	= frameBits;
	this	-> nrInputValues	= (frameBits + K - 1) * R;
	m_decoder	-> set_traceback_length ((frameBits + K - 1) * 8);
}

	viterbi::~viterbi	() { }

static 
const uint8_t bits [] =
	{0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};


void	viterbi::deconvolve (const int16_t *inputValues, uint8_t *outVector) {
uint8_t dataOut [(frameBits + K - 1) / 8 + 1];

	m_decoder -> reset ();
//	fprintf (stderr, "nrInputValues %d\n", nrInputValues);

#if defined(__ARCH_X86__)
	if (cpuSupport & AVX_SUPPORT) {
	   m_accumulated_error +=
	              Decoder_avx::update<uint64_t> (*m_decoder.get (),
	                                         inputValues, nrInputValues);
	}
	else
	if (cpuSupport & SSE_SUPPORT) {
	   m_accumulated_error +=
	              Decoder_sse::update<uint64_t> (*m_decoder.get (),
	                                         inputValues, nrInputValues);
	}
	else
	   m_accumulated_error +=
	              Decoder_scalar::update<uint64_t> (*m_decoder.get (),
	                                         inputValues, nrInputValues);
#elif defined(__ARCH_AARCH64__)
	   m_accumulated_error +=
	              Decoder::update<uint64_t> (*m_decoder.get (),
	                                         inputValues, nrInputValues);
#else
	   m_accumulated_error +=
	              Decoder_scalar::update<uint64_t> (*m_decoder.get (),
	                                         inputValues, nrInputValues);
#endif
	m_decoder -> chainback (dataOut, frameBits, 0);
	const uint64_t error =
	          m_accumulated_error + uint64_t(m_decoder -> get_error ());
	for (int i = 0; i < this -> frameBits; i ++)
	   outVector [i] = (dataOut [i >> 3] & bits [i & 07]) != 0 ? 1 : 0;
}


void	viterbi::convolve (uint8_t *input, uint8_t *out, int blockLength) {
uint8_t a0, a1 = 0,
	    a2 = 0, 
	    a3 = 0,
	    a4 = 0,
	    a5 = 0,
	    a6 = 0;

//#define POLYS { 0155, 0117, 0123, 0155}
	for (int i = 0; i < blockLength; i ++) {
	   a0 = input [i];
	   out [4 * i + 0] = a0 ^ a2 ^ a3 ^ a5 ^ a6;
           out [4 * i + 1] = a0 ^ a1 ^ a2 ^ a3 ^ a6;
           out [4 * i + 2] = a0 ^ a1 ^ a4 ^ a6;
           out [4 * i + 3] = out [4 * i + 0];

//	now shift
           a6 = a5; a5 = a4; a4 = a3; a3 = a2; a2 = a1; a1 = a0;
        }
//
//      Now the residu bits. Empty the registers by shifting in
//      zeros
        for (int i = blockLength; i < blockLength + 6; i ++) {
           a0 = 0;
	   out [4 * i + 0] = a0 ^ a2 ^ a3 ^ a5 ^ a6;
           out [4 * i + 1] = a0 ^ a1 ^ a2 ^ a3 ^ a6;
           out [4 * i + 2] = a0 ^ a1 ^ a4 ^ a6;
           out [4 * i + 3] = out [4 * i + 0];
//	now shift
           a6 = a5; a5 = a4; a4 = a3; a3 = a2; a2 = a1; a1 = a0;
        }
}
