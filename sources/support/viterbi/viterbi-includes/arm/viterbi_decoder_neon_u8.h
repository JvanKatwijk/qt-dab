/* Copyright 2004-2014, Phil Karn, KA9Q
 * Phil Karn's github repository: https://github.com/ka9q/libfec 
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 * 
 * Modified by author, William Yang
 * 07/2023 - Generalised decoder using NEON instructions for 8bit types giving 16 way speedup.
 */
#pragma once
#include "../viterbi_decoder_core.h"
#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>
#include <assert.h>
#include <vector>
#include "arm_neon.h"

/// @brief Vectorisation using 128bit ARM.
///        8bit integers for errors, soft-decision values.
///        16 way vectorisation from 128bits/16bits.
template <size_t constraint_length, size_t code_rate>
class ViterbiDecoder_NEON_u8
{
private:
    using Base = ViterbiDecoder_Core<constraint_length,code_rate,uint8_t,int8_t>;
    using decision_bits_t = typename Base::Decisions::format_t;
private:
    // Calculate the minimum constraint length for vectorisation
    // We require: stride(metric)/2 = stride(branch_table) = stride(decision)
    // total_states = 2^(K-1)
    //
    // sizeof(metric)       = total_states   * sizeof(u8)  = 2^(K-1)
    // sizeof(branch_table) = total_states/2 * sizeof(s8)  = 2^(K-2)
    //
    // sizeof(__m128i)      = 16 = 2^4
    // stride(metric)       = sizeof(metric)       / sizeof(__m128i) = 2^(K-5)
    // stride(branch_table) = sizeof(branch_table) / sizeof(__m128i) = 2^(K-6)
    //
    // For stride(...) >= 1, then K >= 6
    static constexpr size_t SIMD_ALIGN = sizeof(uint16x8_t);
    static constexpr size_t v_stride_metric = Base::Metrics::SIZE_IN_BYTES/SIMD_ALIGN;
    static constexpr size_t v_stride_branch_table = Base::BranchTable::SIZE_IN_BYTES/SIMD_ALIGN;
    static constexpr size_t K_min = 6;
public:
    static constexpr bool is_valid = Base::K >= K_min;

    template <typename sum_error_t>
    static sum_error_t update(Base& base, const int8_t* symbols, const size_t N) {
        static_assert(is_valid, "Insufficient constraint length for vectorisation");
        static_assert(Base::Metrics::ALIGNMENT % SIMD_ALIGN == 0);
        static_assert(Base::BranchTable::ALIGNMENT % SIMD_ALIGN == 0);

        // number of symbols must be a multiple of the code rate
        assert(N % Base::R == 0);
        const size_t total_decoded_bits = N / Base::R;
        const size_t max_decoded_bits = base.get_traceback_length() + Base::TOTAL_STATE_BITS;
        assert((total_decoded_bits + base.m_current_decoded_bit) <= max_decoded_bits);

        sum_error_t total_error = 0;
        for (size_t s = 0; s < N; s+=Base::R) {
            auto* decision = base.m_decisions[base.m_current_decoded_bit];
            auto* old_metric = base.m_metrics.get_old();
            auto* new_metric = base.m_metrics.get_new();
            bfly(base, &symbols[s], decision, old_metric, new_metric);
            if (new_metric[0] >= base.m_config.renormalisation_threshold) {
                total_error += sum_error_t(renormalise(new_metric));
            }
            base.m_metrics.swap();
            base.m_current_decoded_bit++;
        }
        return total_error;
    }
private:
    static void bfly(Base& base, const int8_t* symbols, decision_bits_t* decision, uint8_t* old_metric, uint8_t* new_metric) {
        const int8x16_t* v_branch_table = reinterpret_cast<const int8x16_t*>(base.m_branch_table.data());
        uint8x16_t* v_old_metrics = reinterpret_cast<uint8x16_t*>(old_metric);
        uint8x16_t* v_new_metrics = reinterpret_cast<uint8x16_t*>(new_metric);
        uint32_t* v_decision = reinterpret_cast<uint32_t*>(decision);

        assert(uintptr_t(v_branch_table) % SIMD_ALIGN == 0);
        assert(uintptr_t(v_old_metrics)  % SIMD_ALIGN == 0);
        assert(uintptr_t(v_new_metrics)  % SIMD_ALIGN == 0);

        int8x16_t v_symbols[Base::R];

        // Vectorise constants
        for (size_t i = 0; i < Base::R; i++) {
            v_symbols[i] = vmovq_n_s8(symbols[i]);
        }
        const uint8x16_t max_error = vmovq_n_u8(base.m_config.soft_decision_max_error);

        for (size_t curr_state = 0u; curr_state < v_stride_branch_table; curr_state++) {
            // Total errors across R symbols
            uint8x16_t total_error = vmovq_n_u8(0);
            for (size_t i = 0u; i < Base::R; i++) {
                int8x16_t error = vabdq_s8(
                    v_branch_table[i*v_stride_branch_table+curr_state], 
                    v_symbols[i]
                );
                total_error = vqaddq_u8(total_error, vreinterpretq_u8_s8(error));
            }

            // Butterfly algorithm
            const size_t curr_state_0 = curr_state;
            const size_t curr_state_1 = curr_state + v_stride_metric/2;
            const size_t next_state_0 = (curr_state << 1) | 0;
            const size_t next_state_1 = (curr_state << 1) | 1;

            const uint8x16_t inverse_error = vqsubq_u8(max_error, total_error);
            const uint8x16_t next_error_0_0 = vqaddq_u8(v_old_metrics[curr_state_0],   total_error);
            const uint8x16_t next_error_1_0 = vqaddq_u8(v_old_metrics[curr_state_1], inverse_error);
            const uint8x16_t next_error_0_1 = vqaddq_u8(v_old_metrics[curr_state_0], inverse_error);
            const uint8x16_t next_error_1_1 = vqaddq_u8(v_old_metrics[curr_state_1],   total_error);

            const uint8x16_t min_next_error_0 = vminq_u8(next_error_0_0, next_error_1_0);
            const uint8x16_t min_next_error_1 = vminq_u8(next_error_0_1, next_error_1_1);
            const uint8x16_t decision_0 = vceqq_u8(min_next_error_0, next_error_1_0);
            const uint8x16_t decision_1 = vceqq_u8(min_next_error_1, next_error_1_1);

            // Update metrics
            v_new_metrics[next_state_0] = vzip1q_u8(min_next_error_0, min_next_error_1);
            v_new_metrics[next_state_1] = vzip2q_u8(min_next_error_0, min_next_error_1);

            // Pack decision bits
            v_decision[curr_state] = pack_decision_bits(decision_0, decision_1);
        }
    }

    static uint8_t renormalise(uint8_t* metric) {
        assert(uintptr_t(metric) % SIMD_ALIGN == 0);
        uint8x16_t* v_metric = reinterpret_cast<uint8x16_t*>(metric);

        // Find minimum 
        uint8x16_t adjustv = v_metric[0];
        for (size_t i = 1u; i < v_stride_metric; i++) {
            adjustv = vminq_u8(adjustv, v_metric[i]);
        }
        const uint8_t min = vminvq_u8(adjustv);

        // Normalise to minimum
        const uint8x16_t vmin = vmovq_n_u8(min);
        for (size_t i = 0u; i < v_stride_metric; i++) {
            v_metric[i] = vqsubq_u8(v_metric[i], vmin);
        }

        return min;
    }

    static uint32_t pack_decision_bits(uint8x16_t decision_0, uint8x16_t decision_1) {
        alignas(SIMD_ALIGN) constexpr uint8_t _d0_mask[16] = {
            1<<0, 1<<2, 1<<4, 1<<6,
            1<<0, 1<<2, 1<<4, 1<<6,
            1<<0, 1<<2, 1<<4, 1<<6,
            1<<0, 1<<2, 1<<4, 1<<6,
        };

        alignas(SIMD_ALIGN) constexpr uint8_t _d1_mask[16] = {
            1<<1, 1<<3, 1<<5, 1<<7,
            1<<1, 1<<3, 1<<5, 1<<7,
            1<<1, 1<<3, 1<<5, 1<<7,
            1<<1, 1<<3, 1<<5, 1<<7,
        };

        alignas(SIMD_ALIGN) constexpr int32_t _shift_mask[4] = {
            0, 8, 16, 24
        };

        uint8x16_t d0_mask = vld1q_u8(_d0_mask);
        uint8x16_t d1_mask = vld1q_u8(_d1_mask);
        int32x4_t shift_mask = vld1q_s32(_shift_mask);

        uint8x16_t m0 = vorrq_u8(vandq_u8(decision_0, d0_mask), vandq_u8(decision_1, d1_mask));
        uint16x8_t m1 = vpaddlq_u8(m0);
        uint32x4_t m2 = vpaddlq_u16(m1);

        uint32x4_t m3 = vshlq_u32(m2, shift_mask);
        uint32_t v = vaddvq_u32(m3);
        return v;
    }
};

