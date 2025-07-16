/* Copyright (c) 2023 William Yang. All rights reserved.
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see https://opensource.org/licenses/MIT.
 */
#pragma once
#include "./convolutional_encoder.h"
#include "./parity_table.h"
#include <vector>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

/// @brief Convolutional encoder that uses a shift register
template <typename reg_t = uint16_t>
class ConvolutionalEncoder_ShiftRegister: public ConvolutionalEncoder
{
private:
    const reg_t CONSTRAINT_MASK;
    std::vector<reg_t> G;
    reg_t reg = 0;
public:
    template <typename code_t>
    ConvolutionalEncoder_ShiftRegister(const size_t constraint_length, const size_t code_rate, const code_t* _G)
    :   ConvolutionalEncoder(constraint_length, code_rate),
        CONSTRAINT_MASK((reg_t(1) << K) - reg_t(1)),
        G(R)
    {
        static_assert(sizeof(code_t) <= sizeof(reg_t), "Code type is being truncated by size of register");
        assert(K <= (sizeof(reg_t)*8u));            // number of constraint bits must fit inside size_t
        assert(CONSTRAINT_MASK != 0);

        for (size_t i = 0u; i < R; i++) {
            G[i] = reg_t(_G[i]) & CONSTRAINT_MASK;
        }
    }

    void reset() override { 
        reg = 0u; 
    }

    // Output R bytes for each input byte
    void consume_byte(const uint8_t x, uint8_t* y) override {
        auto& parity_table = ParityTable::get();

        for (size_t i = 0u; i < R; i++) {
            y[i] = 0x00;
        }

        size_t curr_bit = 0u;
        uint8_t input = x;
        for (size_t i = 0u; i < 8u; i++) {
            reg = (reg << 1u) | ((input >> (8u-1u-i)) & reg_t(0b1));

            for (size_t j = 0u; j < R; j++) {
                const size_t curr_out_index = curr_bit / 8;
                const size_t curr_out_bit   = curr_bit % 8;
                const uint8_t bit_out = parity_table.parse<reg_t>(G[j] & reg);
                y[curr_out_index] |= (bit_out << curr_out_bit);
                curr_bit++;
            }
        }
    }
};