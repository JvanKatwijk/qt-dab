/* Copyright 2004-2014, Phil Karn, KA9Q
 * Phil Karn's github repository: https://github.com/ka9q/libfec 
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 * 
 * Modified by author, William Yang
 * 07/2023 - Refactored branch table into separate class
 */
#pragma once

#include "./parity_table.h"
#include "./alignment.h"
#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>
#include <assert.h>

// Store a table of the soft decision values of the encoded symbol for each possible state and input
// If the same parameters are used for the viterbi decoder we can reuse the branch table for better memory usage
template <size_t constraint_length, size_t code_rate, typename soft_t>
class ViterbiBranchTable 
{
public:
    static constexpr size_t K = constraint_length;
    static constexpr size_t R = code_rate;
    static constexpr size_t TOTAL_STATE_BITS = K-1;
    // We are using the butterfly optimisation trick so we only need to compute half the states
    static constexpr size_t NUMSTATES = (size_t(1) << TOTAL_STATE_BITS)/2;
    static constexpr size_t SIZE_IN_BYTES = sizeof(soft_t)*NUMSTATES;
    static constexpr size_t ALIGNMENT = get_platform_alignment(SIZE_IN_BYTES);
public:
    // NOTE: Polynomials (G) should be in binary form with least signficant bit corresponding to the input bit
    //       The number of polynomial coefficients should be equal to the code rate
    template <typename code_t>
    ViterbiBranchTable(const code_t* G, const soft_t _soft_decision_high, const soft_t _soft_decision_low)
    :   soft_decision_high(_soft_decision_high), soft_decision_low(_soft_decision_low)
    {
        static_assert(K > 1u);       
        static_assert(R > 1u);
        static_assert(sizeof(branch_t) % ALIGNMENT == 0);
        assert(uintptr_t(this->data()) % ALIGNMENT == 0);
        assert(soft_decision_high > soft_decision_low);

        // calculate branch table
        auto& parity_table = ParityTable::get();
        for (size_t state = 0u; state < NUMSTATES; state++) {
            for (size_t i = 0u; i < R; i++) {
                // Here state is only K-2 bits long
                // So the symbol we calculate takes the value
                // y = P{(0|X|0)^code}, where P{x} calculates the parity bit
                const size_t value = (state << 1) & size_t(G[i]);
                const uint8_t parity = parity_table.parse(value);
                branch_table[i].buf[state] = parity ? soft_decision_high : soft_decision_low;
            }
        }
    }

    /// @brief Return the symbol values for each state at the specified rate index.
    ///        This allows for 2D indexing: branch_table[current_rate][current_state].
    const soft_t* operator[](size_t index) const { 
        assert(index < R);
        return &branch_table[index].buf[0];
    }

    const soft_t* data() const { 
        return &branch_table[0].buf[0]; 
    }
private:
    const soft_t soft_decision_high;
    const soft_t soft_decision_low;

    struct alignas(ALIGNMENT) branch_t {
        soft_t buf[NUMSTATES];
    };
    alignas(ALIGNMENT) branch_t branch_table[R];
};