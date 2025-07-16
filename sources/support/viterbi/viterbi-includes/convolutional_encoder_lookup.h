/* Copyright (c) 2023 William Yang. All rights reserved.
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see https://opensource.org/licenses/MIT.
 */
#pragma once
#include "./convolutional_encoder.h"
#include "./parity_table.h"
#include <vector>
#include <assert.h>

/// @brief Convolutional encoder that uses a lookup table to process per byte.
///        For codes such as Cassini (K=15) which have large constraint lengths.
///        The generated lookup table is extremely large and would perform worse due to cache invalidation.
///        For these types of codes a lookup table based implementation is entirely inappropriate.
class ConvolutionalEncoder_Lookup: public ConvolutionalEncoder
{
private:
    const size_t TOTAL_BITS_STATE;
    const size_t TOTAL_BITS_INPUT;
    const size_t TOTAL_BITS_LOOKUP;
    const size_t LOOKUP_TABLE_SIZE;
    const size_t LOOKUP_BITMASK;
    const size_t ENCODE_BITMASK;
private:
    size_t reg = 0;
    std::vector<uint8_t> table;
public:
    template <typename code_t>
    ConvolutionalEncoder_Lookup(const size_t constraint_length, const size_t code_rate, const code_t* G) 
    :   ConvolutionalEncoder(constraint_length, code_rate),
        TOTAL_BITS_STATE(K-1),
        TOTAL_BITS_INPUT(8u),
        TOTAL_BITS_LOOKUP(TOTAL_BITS_STATE + TOTAL_BITS_INPUT),
        LOOKUP_TABLE_SIZE(std::size_t(1) << TOTAL_BITS_LOOKUP),
        LOOKUP_BITMASK((std::size_t(1) << TOTAL_BITS_LOOKUP) - 1u),
        ENCODE_BITMASK((std::size_t(1) << K) - 1u)
    {
        assert(K > 1u);
        assert(R > 1u);
        assert(K <= (sizeof(size_t)*8u));       // number of constraint bits must fit inside size_t
        table.resize(LOOKUP_TABLE_SIZE * R); 
        generate_table(G, R);
        reset();
    }

    void reset() override { 
        reg = 0u; 
    }

    // Output R bytes for each input byte
    void consume_byte(const uint8_t x, uint8_t* y) override {
        reg = (reg << TOTAL_BITS_INPUT) | (size_t)x;
        const size_t i = reg & LOOKUP_BITMASK;
        uint8_t* v = &table[i*R];
        for (size_t j = 0; j < R; j++) {
            y[j] = v[j];
        }
    }
private:
    template <typename code_t>
    void generate_table(const code_t G, const size_t R) {
        auto& parity_table = ParityTable::get();

        for (size_t i = 0u; i < LOOKUP_TABLE_SIZE; i++) {
            uint8_t* y = &table[i*R];
            size_t x = i;
            size_t curr_bit = 0u;
            for (int j = 0; j < 8; j++) {
                x = x << 1;
                const size_t state = (x >> TOTAL_BITS_INPUT) & ENCODE_BITMASK;
                for (size_t k = 0; k < R; k++) {
                    const size_t code_out = state & size_t(G[k]);
                    const uint8_t bit_out = parity_table.parse(code_out);
                    const size_t curr_out_index = curr_bit / 8;
                    const size_t curr_out_bit   = curr_bit % 8;
                    y[curr_out_index] |= (bit_out << curr_out_bit);
                    curr_bit++;
                }
            }
        }
    }
};