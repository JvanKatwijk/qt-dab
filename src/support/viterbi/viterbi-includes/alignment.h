/* Copyright (c) 2023 William Yang. All rights reserved.
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see https://opensource.org/licenses/MIT.
 */
#pragma once
#include <stddef.h>

/// @brief Returns the best possible alignment for a given size, both are in bytes.
static constexpr 
size_t get_platform_alignment(const size_t x) {
    if      (x % 64 == 0) { return 64; } // AVX-512: 512
    else if (x % 32 == 0) { return 32; } // AVX-2:   256
    else if (x % 16 == 0) { return 16; } // SSE:     128
    else                  { return x;  } // Fallback
}