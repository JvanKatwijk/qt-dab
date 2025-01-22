/* Copyright 2004-2014, Phil Karn, KA9Q
 * Phil Karn's github repository: https://github.com/ka9q/libfec 
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 * 
 * Modified by author, William Yang
 * 07/2023 - Generalised decoder using AVX2 instructions for 16bit types giving 16 way speedup.
 */
#pragma once
#include "../viterbi_decoder_core.h"
#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>
#include <assert.h>
#include <vector>
#include <immintrin.h>

/// @brief Vectorisation using AVX2.
///        16bit integers for errors, soft-decision values.
///        16 way vectorisation from 256bits/16bits.
template <size_t constraint_length, size_t code_rate>
class ViterbiDecoder_AVX_u16
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
    // sizeof(__m256i)      = 32 = 2^5
    // stride(metric)       = sizeof(metric)       / sizeof(__m128i) = 2^(K-5)
    // stride(branch_table) = sizeof(branch_table) / sizeof(__m128i) = 2^(K-6)
    //
    // For stride(...) >= 1, then K >= 6
    static constexpr size_t SIMD_ALIGN = sizeof(__m256i);
    static constexpr size_t v_stride_metric = Base::NUMSTATES/SIMD_ALIGN*2u;
    static constexpr size_t v_stride_branch_table = Base::NUMSTATES/SIMD_ALIGN;
    static constexpr size_t K_min = 6;
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
    static void bfly(Base& base, const int16_t* symbols, decision_bits_t* decision, uint16_t* old_metric, uint16_t* new_metric) {
        const __m256i* v_branch_table = reinterpret_cast<const __m256i*>(base.m_branch_table.data());
        __m256i* v_old_metrics = reinterpret_cast<__m256i*>(old_metric);
        __m256i* v_new_metrics = reinterpret_cast<__m256i*>(new_metric);
        uint32_t* v_decision = reinterpret_cast<uint32_t*>(decision);

        assert(uintptr_t(v_branch_table) % SIMD_ALIGN == 0);
        assert(uintptr_t(v_old_metrics)  % SIMD_ALIGN == 0);
        assert(uintptr_t(v_new_metrics)  % SIMD_ALIGN == 0);

        __m256i v_symbols[Base::R];

        // Vectorise constants
        for (size_t i = 0; i < Base::R; i++) {
            v_symbols[i] = _mm256_set1_epi16(symbols[i]);
        }
        const __m256i max_error = _mm256_set1_epi16(base.m_config.soft_decision_max_error);

        for (size_t curr_state = 0u; curr_state < v_stride_branch_table; curr_state++) {
            // Total errors across R symbols
            __m256i total_error = _mm256_set1_epi16(0);
            for (size_t i = 0u; i < Base::R; i++) {
                __m256i error = _mm256_subs_epi16(v_branch_table[i*v_stride_branch_table+curr_state], v_symbols[i]);
                error = _mm256_abs_epi16(error);
                total_error = _mm256_adds_epu16(total_error, error);
            }

            // Butterfly algorithm
            const size_t curr_state_0 = curr_state;
            const size_t curr_state_1 = curr_state + v_stride_metric/2;
            const size_t next_state_0 = (curr_state << 1) | 0;
            const size_t next_state_1 = (curr_state << 1) | 1;

            const __m256i inverse_error = _mm256_subs_epu16(max_error, total_error);
            const __m256i next_error_0_0 = _mm256_adds_epu16(v_old_metrics[curr_state_0],   total_error);
            const __m256i next_error_1_0 = _mm256_adds_epu16(v_old_metrics[curr_state_1], inverse_error);
            const __m256i next_error_0_1 = _mm256_adds_epu16(v_old_metrics[curr_state_0], inverse_error);
            const __m256i next_error_1_1 = _mm256_adds_epu16(v_old_metrics[curr_state_1],   total_error);

            const __m256i min_next_error_0 = _mm256_min_epu16(next_error_0_0, next_error_1_0);
            const __m256i min_next_error_1 = _mm256_min_epu16(next_error_0_1, next_error_1_1);
            const __m256i decision_0 = _mm256_cmpeq_epi16(min_next_error_0, next_error_1_0);
            const __m256i decision_1 = _mm256_cmpeq_epi16(min_next_error_1, next_error_1_1);

            // Update metrics
            const __m256i new_metric_lo = _mm256_unpacklo_epi16(min_next_error_0, min_next_error_1);
            const __m256i new_metric_hi = _mm256_unpackhi_epi16(min_next_error_0, min_next_error_1);
            // Reshuffle into correct order along 128bit boundaries
            v_new_metrics[next_state_0] = _mm256_permute2x128_si256(new_metric_lo, new_metric_hi, 0b0010'0000);
            v_new_metrics[next_state_1] = _mm256_permute2x128_si256(new_metric_lo, new_metric_hi, 0b0011'0001);

            // Pack each set of decisions into 8 8-bit bytes, then interleave them and compress into 16 bits
            // 256bit packs works with 128bit segments
            // 256bit unpack works with 128bit segments
            // | = 128bit boundary
            // packs_16  : d0 .... 0 .... | d1 .... 0 ....
            // packs_16  : d2 .... 0 .... | d3 .... 0 ....
            // unpacklo_8: d0 d2 d0 d2 .. | d1 d3 d1 d3 ..
            // movemask_8: b0 b2 b0 b2 .. | b1 b3 b1 b3 ..
            v_decision[curr_state] = _mm256_movemask_epi8(_mm256_unpacklo_epi8(
                _mm256_packs_epi16(decision_0, _mm256_setzero_si256()), 
                _mm256_packs_epi16(decision_1, _mm256_setzero_si256())));
        }
    }

    static uint16_t renormalise(uint16_t* metric) {
        assert(uintptr_t(metric) % SIMD_ALIGN == 0);
        __m256i* v_metric = reinterpret_cast<__m256i*>(metric);

        union alignas(SIMD_ALIGN) {
            __m256i m256;
            __m128i m128[2];
            uint16_t u16[16]; 
        } reduce_buffer;

        // Find minimum 
        reduce_buffer.m256 = v_metric[0];
        for (size_t i = 1u; i < v_stride_metric; i++) {
            reduce_buffer.m256 = _mm256_min_epu16(reduce_buffer.m256, v_metric[i]);
        }
        // Shift half of the array onto the other half and get the minimum between them
        // Repeat this until we get the minimum value of all 16bit values
        // NOTE: srli performs shift on 128bit lanes
        __m128i adjustv = _mm_min_epu16(reduce_buffer.m128[0], reduce_buffer.m128[1]);
        adjustv = _mm_min_epu16(adjustv, _mm_srli_si128(adjustv, 8));
        adjustv = _mm_min_epu16(adjustv, _mm_srli_si128(adjustv, 4));
        adjustv = _mm_min_epu16(adjustv, _mm_srli_si128(adjustv, 2));
        reduce_buffer.m128[0] = adjustv;
        const uint16_t min = reduce_buffer.u16[0];

        // Normalise to minimum
        const __m256i vmin = _mm256_set1_epi16(min);
        for (size_t i = 0u; i < v_stride_metric; i++) {
            v_metric[i] = _mm256_subs_epu16(v_metric[i], vmin);
        }

        return min;
    }
};

