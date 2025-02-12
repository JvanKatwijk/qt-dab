/* Copyright 2004-2014, Phil Karn, KA9Q
 * Phil Karn's github repository: https://github.com/ka9q/libfec 
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 * 
 * Modified by author, William Yang
 * 07/2023 - Generalised decoder using SSE4.1 instructions for 16bit types giving 8 way speedup.
 */
#pragma once
#include "../viterbi_decoder_core.h"
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <vector>
#include <immintrin.h>

/// @brief Vectorisation using SSE4.1
///        16bit integers for errors, soft-decision values.
///        8 way vectorisation from 128bits/16bits.
template <size_t constraint_length, size_t code_rate>
class ViterbiDecoder_SSE_u16
{
private:
    using Base = ViterbiDecoder_Core<constraint_length,code_rate,uint16_t,int16_t>;
    using decision_bits_t = typename Base::Decisions::format_t;
private:
    // Calculate the minimum constraint length for vectorisation
    // We require: stride(metric)/2 = stride(branch_table) = stride(decision)
    // total_states = 2^(K-1)
    //
    // sizeof(metric)       = total_states   * sizeof(u16) = 2^(K  )
    // sizeof(branch_table) = total_states/2 * sizeof(s16) = 2^(K-1)
    //
    // sizeof(__m128i)      = 16 = 2^4
    // stride(metric)       = sizeof(metric)       / sizeof(__m128i) = 2^(K-4)
    // stride(branch_table) = sizeof(branch_table) / sizeof(__m128i) = 2^(K-5)
    //
    // For stride(...) >= 1, then K >= 5
    static constexpr size_t SIMD_ALIGN = sizeof(__m128i);
    static constexpr size_t v_stride_metric = Base::Metrics::SIZE_IN_BYTES/SIMD_ALIGN;
    static constexpr size_t v_stride_branch_table = Base::BranchTable::SIZE_IN_BYTES/SIMD_ALIGN;
    static constexpr size_t K_min = 5;
public:
    static constexpr bool is_valid = Base::K >= K_min;

    template <typename sum_error_t>
    static sum_error_t update(Base& base, const int16_t* symbols, const size_t N) {
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
            auto* old_metrics = base.m_metrics.get_old();
            auto* new_metrics = base.m_metrics.get_new();
            bfly(base, &symbols[s], decision, old_metrics, new_metrics);
            if (new_metrics[0] >= base.m_config.renormalisation_threshold) {
                total_error += sum_error_t(renormalise(new_metrics));
            }
            base.m_metrics.swap();
            base.m_current_decoded_bit++;
        }
        return total_error;
    }
private:
    static void bfly(Base& base, const int16_t* symbols, decision_bits_t* decision, uint16_t* old_metrics, uint16_t* new_metrics) {
        const __m128i* v_branch_table = reinterpret_cast<const __m128i*>(base.m_branch_table.data());
        __m128i* v_old_metrics = reinterpret_cast<__m128i*>(old_metrics);
        __m128i* v_new_metrics = reinterpret_cast<__m128i*>(new_metrics);
        uint16_t* v_decision = reinterpret_cast<uint16_t*>(decision);

        assert(uintptr_t(v_branch_table) % SIMD_ALIGN == 0);
        assert(uintptr_t(v_old_metrics)  % SIMD_ALIGN == 0);
        assert(uintptr_t(v_new_metrics)  % SIMD_ALIGN == 0);

        __m128i v_symbols[Base::R];

        // Vectorise constants
        for (size_t i = 0; i < Base::R; i++) {
            v_symbols[i] = _mm_set1_epi16(symbols[i]);
        }
        const __m128i max_error = _mm_set1_epi16(base.m_config.soft_decision_max_error);

        for (size_t curr_state = 0u; curr_state < v_stride_branch_table; curr_state++) {
            // Total errors across R symbols
            __m128i total_error = _mm_set1_epi16(0);
            for (size_t i = 0u; i < Base::R; i++) {
                __m128i error = _mm_subs_epi16(v_branch_table[i*v_stride_branch_table+curr_state], v_symbols[i]);
                error = _mm_abs_epi16(error);
                total_error = _mm_adds_epu16(total_error, error);
            }

            // Butterfly algorithm
            // This is the same as the scalar version except we have some state reshuffling to do as we see later on...
            const size_t curr_state_0 = curr_state;
            const size_t curr_state_1 = curr_state + v_stride_metric/2;
            const size_t next_state_0 = (curr_state << 1) | 0;
            const size_t next_state_1 = (curr_state << 1) | 1;

            const __m128i inverse_error = _mm_subs_epu16(max_error, total_error);
            const __m128i next_error_0_0 = _mm_adds_epu16(v_old_metrics[curr_state_0],   total_error);
            const __m128i next_error_1_0 = _mm_adds_epu16(v_old_metrics[curr_state_1], inverse_error);
            const __m128i next_error_0_1 = _mm_adds_epu16(v_old_metrics[curr_state_0], inverse_error);
            const __m128i next_error_1_1 = _mm_adds_epu16(v_old_metrics[curr_state_1],   total_error);

            const __m128i min_next_error_0 = _mm_min_epu16(next_error_0_0, next_error_1_0);
            const __m128i min_next_error_1 = _mm_min_epu16(next_error_0_1, next_error_1_1);
            const __m128i decision_0 = _mm_cmpeq_epi16(min_next_error_0, next_error_1_0);
            const __m128i decision_1 = _mm_cmpeq_epi16(min_next_error_1, next_error_1_1);

            // Update metrics
            //
            // We have to repack the next state error metrics so that they correspond to the correct state
            // We are iterating over in strides of L, so we have the following mapping
            //                stride =>    correct_position |  just_calculated_states
            // curr_state_0 =  i     =>      i*L + {0..L-1} |  i*L + {0..L-1}
            // next_state_0 = 2i     =>     2i*L + {0..L-1} | 2i*L + {0,2..2*(L-1)}   = 2i*L + {0,2,4..L-2..2*L-2}
            // next_state_1 = 2i + 1 => (2i+1)*L + {0..L-1} | 2i*L + {1,3..2*(L-1)+1} = 2i*L + {1,3,5..L-1..2*L-1}
            //
            // We want to store our just calculated states into their correct position
            // unpacklo: {0,2,4..L-2..2*L-2},{1,3,5..L-1..2*L-1} =>                       {0,1,2,3..L-2,L-1}
            // unpackhi: {0,2,4..L-2..2*L-2},{1,3,5..L-1..2*L-1} => {L,L+1,..2*L-1} = L + {0,1,2,3..L-2,L-1}
            // Therefore we can use this to correctly repack our metrics into their corresponding states
            v_new_metrics[next_state_0] = _mm_unpacklo_epi16(min_next_error_0, min_next_error_1);
            v_new_metrics[next_state_1] = _mm_unpackhi_epi16(min_next_error_0, min_next_error_1);

            // Pack each set of decisions into 8 8-bit bytes, then interleave them and compress into 16 bits
            // 
            // Note that the decision bits are packed so that they store 2L states 
            // But we stride L states at a time for our branch table
            // So to store the decision bits for (2i)*L and (2i+1)*L we store them in index j 
            // Where the states map as: 
            // i = curr_state,      i: 2i*L + {0..L-1} | (2i+1)*L + {0..L-1} = 2i*L + {0..2L-1}
            // j = decision_index,  j: j*2L + {0..2L-1}
            // Therefore we index using j = i = curr_state
            //
            // We still have to do the repacking like for the new metrics, which is done using the unpacklo
            // packs: {d..},{0..} => {d..d,0..0} => 16bit elements get saturated into 8bit elements and concatentated
            // unpacklo: {d0..d0,0..0},{d1..d1,0..0} => {d0,d1,d0,d1..d0,d1}
            // movemask: Gets the most significant bit and compacts it into a bitfield
            v_decision[curr_state] = _mm_movemask_epi8(_mm_unpacklo_epi8(
                _mm_packs_epi16(decision_0, _mm_setzero_si128()), 
                _mm_packs_epi16(decision_1, _mm_setzero_si128())));
        }
    }

    static uint16_t renormalise(uint16_t* metric) {
        assert(uintptr_t(metric) % SIMD_ALIGN == 0);
        __m128i* v_metric = reinterpret_cast<__m128i*>(metric);

        // Find minimum 
        __m128i adjustv = v_metric[0];
        for (size_t i = 1u; i < v_stride_metric; i++) {
            adjustv = _mm_min_epu16(adjustv, v_metric[i]);
        }
        // Shift half of the array onto the other half and get the minimum between them
        // Repeat this until we get the minimum value of all 16bit values
        // NOTE: srli performs shift on 128bit lanes
        adjustv = _mm_min_epu16(adjustv, _mm_srli_si128(adjustv, 8));
        adjustv = _mm_min_epu16(adjustv, _mm_srli_si128(adjustv, 4));
        adjustv = _mm_min_epu16(adjustv, _mm_srli_si128(adjustv, 2));
        const uint16_t* reduce_buffer = reinterpret_cast<uint16_t*>(&adjustv);
        const uint16_t min = reduce_buffer[0];

        // Normalise to minimum
        const __m128i vmin = _mm_set1_epi16(min);
        for (size_t i = 0u; i < v_stride_metric; i++) {
            v_metric[i] = _mm_subs_epu16(v_metric[i], vmin);
        }

        return min;
    }
};