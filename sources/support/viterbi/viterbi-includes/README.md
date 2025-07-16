# Copyright
Copyright 2004 by Phil Karn, KA9Q. 
It may be used under the terms of the GNU Lesser General Public License (LGPL). 

Modified 2023 by William Yang.

# Explanations
Refer to <code>viterbi_decoder_scalar.h</code> for an explanation of how the generalised Viterbi decoding algorithm works.
This includes an explanation of how the butterfly optimisation works.

This fork of Phil Karn's library for Viterbi decoding primarily aims to provide C++ templates that generalise across all constraint lengths and code rates.
Additionally it should also automatically generate vectorised implementations for CPUs which have SIMD instructions. 
The original source only provided support for specific combinations of constraint lengths and code rates.

Refer to <code>x86/viterbi_decoder_sse_u16.h</code> for an explanation of how the decoder implementation can be vectorised. 
This includes an explanation of how to mitigate problems with data reordering and how we fix it while maintaining high performance.

# Future work
- Support AVX-512 instruction set when I can test it on a supported CPU.
- Support SVE for ARM64 cpus if possible.