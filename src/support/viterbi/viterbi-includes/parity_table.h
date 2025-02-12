/* Copyright (c) 2023 William Yang. All rights reserved.
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see https://opensource.org/licenses/MIT.
 */
#pragma once

#include <stdint.h>

/// @brief Utility class for getting the parity of a primitive's bits
///        Odd bits = 1, Even bits = 0
class ParityTable 
{
private:
    uint8_t* table;
    ParityTable() {
        constexpr size_t N = 256;
        table = new uint8_t[N];
        for (size_t i = 0u; i < N; i++) {
            uint8_t parity = 0u;
            uint8_t b = static_cast<uint8_t>(i);
            for (size_t j = 0u; j < 8u; j++) {
                parity ^= (b & 0b1);
                b = b >> 1u;
            }
            table[i] = parity;
        }
    }
    ~ParityTable() {
        delete [] table;
    }
    ParityTable(const ParityTable&) = delete;
    ParityTable(ParityTable&&) = delete;
    ParityTable& operator=(const ParityTable&) = delete;
    ParityTable& operator=(ParityTable&&) = delete;
public:
    static 
    ParityTable& get() {
        static auto parity_table = ParityTable();
        return parity_table;
    }

    uint8_t parse(uint8_t x) {
        return table[x];
    }

    template <typename T>
    uint8_t parse(T x) {
        constexpr size_t N = sizeof(x);
        size_t TOTAL_BITS = N*8;
        while (TOTAL_BITS > 8u) {
            TOTAL_BITS = TOTAL_BITS >> 1;
            x = x ^ (x >> TOTAL_BITS);
        }
        return table[uint8_t(x & T(0xFF))];
    }
};