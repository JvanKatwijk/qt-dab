/* Copyright 2004-2014, Phil Karn, KA9Q
 * Phil Karn's github repository: https://github.com/ka9q/libfec 
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 * 
 * Modified by author, William Yang
 * 07/2023 - Consolidated core data structures used for decoding between various viterbi decoders into a single class
 * 07/2023 - Refactored these data structured into cleared individual components
 */
#pragma once
#include "./viterbi_branch_table.h"
#include "./viterbi_decoder_config.h"
#include "./alignment.h"

#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>
#include <vector>
#include <cstring>
#include <assert.h>

/// @brief Stores the error metrics for each state in a double buffer.
template<size_t constraint_length, typename error_t>
class ViterbiErrorMetrics 
{
public:
    static constexpr size_t K = constraint_length;
    static constexpr size_t TOTAL_STATE_BITS = K-1;
    static constexpr size_t NUMSTATES = 1 << TOTAL_STATE_BITS;
    static constexpr size_t SIZE_IN_BYTES = sizeof(error_t)*NUMSTATES;
    static constexpr size_t ALIGNMENT = get_platform_alignment(SIZE_IN_BYTES);
public:
    ViterbiErrorMetrics(): index(0) {
        static_assert(sizeof(metric_t) % ALIGNMENT == 0);
        assert(uintptr_t(get_old()) % ALIGNMENT == 0);
    }
    error_t* get_old() { return &old_new_metrics[index  ].metrics[0]; }
    error_t* get_new() { return &old_new_metrics[1-index].metrics[0]; }
    void swap() { index = 1-index; }
private:
    struct alignas(ALIGNMENT) metric_t { 
        error_t metrics[NUMSTATES];
    };
    alignas(ALIGNMENT) metric_t old_new_metrics[2];
    size_t index;
};

/// @brief Stores the leading bit of the previous state for each current state. 
///        The bits are packed into a primitive type, where the lowest order bit corresponds to the first current state.
template<size_t constraint_length, typename decision_bits_t>
class ViterbiDecisionBits 
{
public:
    using format_t = decision_bits_t;
private:
    template <typename T>
    static constexpr
    T get_max(T x0, T x1) { 
        return (x0 > x1) ? x0 : x1; 
    }
public:
    static constexpr size_t K = constraint_length;
    static constexpr size_t TOTAL_STATE_BITS = K-1;
    static constexpr size_t NUMSTATES = 1 << TOTAL_STATE_BITS;
    static constexpr size_t TOTAL_BITS_PER_BLOCK = sizeof(format_t)*8;
    static constexpr size_t TOTAL_BLOCKS = get_max(NUMSTATES/TOTAL_BITS_PER_BLOCK, size_t(1));
    static constexpr size_t SIZE_IN_BYTES = TOTAL_BLOCKS*sizeof(format_t);
public:
    ViterbiDecisionBits() {}
    void resize(const size_t length) { 
        buffer.resize(length); 
    }
    size_t size() const { 
        return buffer.size(); 
    }
    format_t* operator[](const size_t index) {
        return &buffer[index].blocks[0];
    }
private:
    struct blocks_t {
        format_t blocks[TOTAL_BLOCKS];
    };
    std::vector<blocks_t> buffer;
};

/// @brief A buffer that is used to shift in the current state of the viterbi decoder as it goes back through the trellis.
///        Also has some additional logic to account for tail termination bits and add padding so we always have 8bits to form a byte. 
template<size_t constraint_length, typename buffer_t = size_t>
class ViterbiTracebackBuffer 
{
public:
    static constexpr size_t K = constraint_length;
    ViterbiTracebackBuffer() {
        buffer = buffer_t(0);
    }

    buffer_t get_state() {
        return buffer >> layout.shift_state;
    }

    template <typename T>
    void set_state(T state) {
        buffer = buffer_t(state) << layout.shift_state;
    }

    uint8_t get_data() {
        return uint8_t((buffer >> layout.shift_tail) & buffer_t(0xFF));
    }

    template <typename T>
    void push_bit_in(T bit) {
        buffer = (buffer >> 1);
        buffer |= buffer_t(bit) << (layout.total_bits-1);
    }
private:
    struct Layout {
        size_t shift_state; // Number of bits to shift the state out
        size_t shift_tail;  // Number of bits to compensate for tail bit termination delay
        size_t total_bits;  // Number of bits in buffer
    };

    template <typename T>
    static constexpr
    T get_min(T x0, T x1) { 
        return (x0 < x1) ? x0 : x1; 
    }

    // Determine layout of our decode buffer
    constexpr static 
    Layout get_layout() {
        // We want to store enough bits in our decode buffer so that we can retrieve at least 1 byte
        // | State bits | Padding byte |
        // | Output byte   | Tail bits |
        constexpr size_t total_state_bits = K-1;
        constexpr size_t total_bits_in_byte = 8;
        constexpr size_t total_tail_bits = total_state_bits;

        // We can reduce the amount of unnecessary padding
        constexpr size_t total_bits_ignore = get_min(total_bits_in_byte, total_tail_bits);

        constexpr size_t shift_state = total_bits_in_byte - total_bits_ignore;
        constexpr size_t shift_tail = total_tail_bits - total_bits_ignore;

        // Make sure that we can fit all of our bits into the decode buffer
        constexpr size_t total_bits_in_decode_buffer = total_state_bits + shift_state;
        constexpr size_t total_bits_in_type = sizeof(size_t)*8;
        static_assert(total_bits_in_type >= total_bits_in_decode_buffer, "Too many bits to store in decode buffer");

        return Layout { shift_state, shift_tail, total_bits_in_decode_buffer };
    };

    static constexpr Layout layout = get_layout();
    buffer_t buffer;
};

/// @brief Core data structures for viterbi decoder.
///        Traceback technique is the same for all types of viterbi decoders.
template <size_t constraint_length, size_t code_rate, typename error_t, typename soft_t>
class ViterbiDecoder_Core
{
public:
    static constexpr size_t K = constraint_length;
    static constexpr size_t R = code_rate;
    static constexpr size_t TOTAL_STATE_BITS = K-1;
    static constexpr size_t NUMSTATES = 1 << TOTAL_STATE_BITS;
    using BranchTable = ViterbiBranchTable<K,R,soft_t>;
    using Config = ViterbiDecoder_Config<error_t>;
    using Metrics = ViterbiErrorMetrics<K,error_t>;
    using Decisions = ViterbiDecisionBits<K,uintptr_t>;
public:
    ViterbiDecoder_Core(const BranchTable& _branch_table, const Config& _config)
    :   m_branch_table(_branch_table), m_config(_config), m_decisions()
    {
        static_assert(K >= 2u);       
        static_assert(R >= 1u);
        reset();
        set_traceback_length(0);
    }

    /// @brief Set the number of output decoded bits to store. This doesn't include the tail termination bits.
    void set_traceback_length(const size_t traceback_length) {
        const size_t new_length = traceback_length + TOTAL_STATE_BITS;
        m_decisions.resize(new_length);
        if (m_current_decoded_bit > new_length) {
            m_current_decoded_bit = new_length;
        }
    }

    /// @brief Returns the number of output decoded bits we are storing
    size_t get_traceback_length() const {
        const size_t N = m_decisions.size();
        return N - TOTAL_STATE_BITS;
    }

    /// @brief Get the normalised error at a specified end state
    error_t get_error(const size_t end_state = 0u) {
        assert(end_state < Metrics::NUMSTATES);
        auto* old_metrics = m_metrics.get_old();
        return old_metrics[end_state];
    }

    /// @brief Prime the error metrics for a clean decode run
    void reset(const size_t starting_state = 0u) {
        m_current_decoded_bit = 0u;

        auto* old_metrics = m_metrics.get_old();
        for (size_t i = 0; i < Metrics::NUMSTATES; i++) {
            old_metrics[i] = m_config.initial_non_start_error;
        }
        constexpr size_t STATE_MASK = Metrics::NUMSTATES-1;
        old_metrics[starting_state & STATE_MASK] = m_config.initial_start_error;
    }

    /// @brief Writes the decoded bytes into the given array
    void chainback(uint8_t* bytes_out, const size_t total_bits, const size_t end_state = 0u) {
        const size_t traceback_length = get_traceback_length();
        assert(traceback_length >= total_bits);
        assert((m_current_decoded_bit - TOTAL_STATE_BITS) >= total_bits);
        assert(end_state < NUMSTATES);

        ViterbiTracebackBuffer<K> decode_buffer;
        decode_buffer.set_state(end_state);

        for (size_t i = 0u; i < total_bits; i++) {
            const size_t j = (total_bits-1)-i;
            const size_t curr_decoded_byte = j/8;
            const size_t curr_decision = j + TOTAL_STATE_BITS;
            auto* decision_bits = m_decisions[curr_decision];

            const size_t state = decode_buffer.get_state();
            const size_t curr_block_index = state / m_decisions.TOTAL_BITS_PER_BLOCK;
            const size_t curr_block_bit   = state % m_decisions.TOTAL_BITS_PER_BLOCK;
            const size_t input_bit = (decision_bits[curr_block_index] >> curr_block_bit) & 0b1;
            decode_buffer.push_bit_in(input_bit);
            bytes_out[curr_decoded_byte] = decode_buffer.get_data();
        }
    }
public:
    const BranchTable& m_branch_table;
    const Config m_config;
    Metrics m_metrics;
    Decisions m_decisions;
    size_t m_current_decoded_bit;
};