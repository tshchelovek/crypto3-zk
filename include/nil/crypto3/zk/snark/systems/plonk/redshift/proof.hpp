//---------------------------------------------------------------------------//
// Copyright (c) 2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2021 Nikita Kaskov <nbering@nil.foundation>
// Copyright (c) 2022 Ilia Shirobokov <i.shirobokov@nil.foundation>
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

#ifndef CRYPTO3_ZK_PLONK_REDSHIFT_PROOF_HPP
#define CRYPTO3_ZK_PLONK_REDSHIFT_PROOF_HPP

namespace nil {
    namespace crypto3 {
        namespace zk {
            namespace snark {

                template<typename FieldType, typename CommitmentSchemeTypeWitness,
                    typename CommitmentSchemeTypePermutation,
                    typename CommitmentSchemeTypeQuotient>
                struct redshift_proof {

                    struct evaluation_proof {
                        std::vector<typename CommitmentSchemeTypeWitness::proof_type> witness;
                        std::vector<typename CommitmentSchemeTypePermutation::proof_type> permutation;
                        std::vector<typename CommitmentSchemeTypeQuotient::proof_type> quotient;
                    };

                    redshift_proof() {
                    }

                    std::vector<typename CommitmentSchemeTypeWitness::commitment_type> witness_commitments;

                    typename CommitmentSchemeTypePermutation::commitment_type v_perm_commitment;

                    std::vector<typename CommitmentSchemeTypeQuotient::commitment_type> T_commitments;

                    evaluation_proof eval_proof;

                    bool operator==(const redshift_proof &rhs) const {
                        return witness_commitments == rhs.witness_commitments && T_commitments == rhs.T_commitments &&
                               v_perm_commitment == rhs.v_perm_commitment &&
                               eval_proof = rhs.eval_proof;
                    }
                    bool operator!=(const redshift_proof &rhs) const {
                        return !(rhs == *this);
                    }
                };
            }    // namespace snark
        }        // namespace zk
    }            // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_ZK_PLONK_REDSHIFT_PROOF_HPP