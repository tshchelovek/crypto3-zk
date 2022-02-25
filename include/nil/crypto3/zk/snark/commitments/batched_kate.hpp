//---------------------------------------------------------------------------//
// Copyright (c) 2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2021 Nikita Kaskov <nbering@nil.foundation>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_ZK_BATCHED_KATE_COMMITMENT_SCHEME_HPP
#define CRYPTO3_ZK_BATCHED_KATE_COMMITMENT_SCHEME_HPP

#include <nil/crypto3/zk/snark/commitments/commitment.hpp>

namespace nil {
    namespace crypto3 {
        namespace zk {
            namespace snark {

                /**
                 * @brief Based on the Ploynomial Commitment description from \[Plonk].
                 *
                 * References:
                 * \[Plonk]:
                 * "PlonK: Permutations over Lagrange-bases for
                 * Oecumenical Noninteractive arguments of Knowledge",
                 * Ariel Gabizon, Zachary J. Williamson, Oana Ciobotaru,
                 * Aztec,
                 * <https://eprint.iacr.org/2019/953.pdf>
                 */
                class batched_kate_commitment_scheme : commitment_scheme<...> {
                    typedef TCommitment...;
                    typedef TDecommitmentInfo...;
                    typedef TSRS...;
                    typedef TData...;

                public:
                    virtual std::pair<TCommitment, TDecommitmentInfo> commit(TSRS PK, TData phi) {
                    }

                    virtual... open(TSRS PK, TCommitment C, TData phi, TDecommitmentInfo d) {
                    }

                    virtual bool verify(TSRS PK, TCommitment C, TData phi, TDecommitmentInfo d) {
                    }
                };

            }    // namespace snark
        }        // namespace zk
    }            // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_ZK_BATCHED_KATE_COMMITMENT_SCHEME_HPP