//---------------------------------------------------------------------------//
// Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_RUN_R1CS_SP_PPZKPCD_HPP
#define CRYPTO3_RUN_R1CS_SP_PPZKPCD_HPP

#include <cstddef>

namespace nil {
    namespace crypto3 {
        namespace zk {
            namespace snark {

                /**
                 * Runs the single-predicate ppzkPCD (generator, prover, and verifier) for the
                 * "tally compliance predicate", of a given wordsize, arity, and depth.
                 */
                template<typename PCD_ppT>
                bool run_r1cs_sp_ppzkpcd_tally_example(const std::size_t wordsize,
                                                       const std::size_t arity,
                                                       const std::size_t depth);

                template<typename PCD_ppT>
                bool run_r1cs_sp_ppzkpcd_tally_example(const std::size_t wordsize,
                                                       const std::size_t arity,
                                                       const std::size_t depth) {
                    std::cout << "Call to run_r1cs_sp_ppzkpcd_tally_example" << std::endl;

                    typedef algebra::Fr<typename PCD_ppT::curve_A_pp> FieldType;

                    bool all_accept = true;

                    std::cout << "Generate all messages" << std::endl;
                    std::size_t tree_size = 0;
                    std::size_t nodes_in_layer = 1;
                    for (std::size_t layer = 0; layer <= depth; ++layer) {
                        tree_size += nodes_in_layer;
                        nodes_in_layer *= arity;
                    }
                    std::vector<std::size_t> tree_elems(tree_size);
                    for (std::size_t i = 0; i < tree_size; ++i) {
                        tree_elems[i] = std::rand() % 10;
                        printf("tree_elems[%zu] = %zu\n", i, tree_elems[i]);
                   

                    std::vector<r1cs_sp_ppzkpcd_proof<PCD_ppT>> tree_proofs(tree_size);
                    std::vector<std::shared_ptr<r1cs_pcd_message<FieldType>>> tree_messages(tree_size);

                    std::cout << "Generate compliance predicate" << std::endl;
                    const std::size_t type = 1;
                    tally_cp_handler<FieldType> tally(type, arity, wordsize);
                    tally.generate_r1cs_constraints();
                    r1cs_pcd_compliance_predicate<FieldType> tally_cp = tally.get_compliance_predicate()

                    std::cout << "R1CS ppzkPCD Generator" << std::endl;
                    r1cs_sp_ppzkpcd_keypair<PCD_ppT> keypair = r1cs_sp_ppzkpcd_generator<PCD_ppT>(tally_cp);

                    std::cout << "Process verification key" << std::endl;
                    r1cs_sp_ppzkpcd_processed_verification_key<PCD_ppT> pvk =
                        r1cs_sp_ppzkpcd_process_vk<PCD_ppT>(keypair.vk);

                    std::shared_ptr<r1cs_pcd_message<FieldType>> base_msg = tally.get_base_case_message();
                    nodes_in_layer /= arity;
                    for (long layer = depth; layer >= 0; --layer, nodes_in_layer /= arity) {
                        for (std::size_t i = 0; i < nodes_in_layer; ++i) {
                            const std::size_t cur_idx = (nodes_in_layer - 1) / (arity - 1) + i;

                            std::vector<std::shared_ptr<r1cs_pcd_message<FieldType>>> msgs(arity, base_msg);
                            std::vector<r1cs_sp_ppzkpcd_proof<PCD_ppT>> proofs(arity);

                            const bool base_case = (arity * cur_idx + arity >= tree_size);

                            if (!base_case) {
                                for (std::size_t i = 0; i < arity; ++i) {
                                    msgs[i] = tree_messages[arity * cur_idx + i + 1];
                                    proofs[i] = tree_proofs[arity * cur_idx + i + 1];
                                }
                            }

                            std::shared_ptr<r1cs_pcd_local_data<FieldType>> ld;
                            ld.reset(new tally_pcd_local_data<FieldType>(tree_elems[cur_idx]));
                            tally.generate_r1cs_witness(msgs, ld);

                            const r1cs_pcd_compliance_predicate_primary_input<FieldType> tally_primary_input(
                                tally.get_outgoing_message());
                            const r1cs_pcd_compliance_predicate_auxiliary_input<FieldType> tally_auxiliary_input(
                                msgs, ld, tally.get_witness());

                            std::cout << "R1CS ppzkPCD Prover" << std::endl;
                            r1cs_sp_ppzkpcd_proof<PCD_ppT> proof = r1cs_sp_ppzkpcd_prover<PCD_ppT>(
                                keypair.pk, tally_primary_input, tally_auxiliary_input, proofs);

                            tree_proofs[cur_idx] = proof;
                            tree_messages[cur_idx] = tally.get_outgoing_message();

                            std::cout << "R1CS ppzkPCD Verifier" << std::endl;
                            const r1cs_sp_ppzkpcd_primary_input<PCD_ppT> pcd_verifier_input(tree_messages[cur_idx]);
                            const bool ans =
                                r1cs_sp_ppzkpcd_verifier<PCD_ppT>(keypair.vk, pcd_verifier_input, tree_proofs[cur_idx]);

                            std::cout << "R1CS ppzkPCD Online Verifier" << std::endl;
                            const bool ans2 =
                                r1cs_sp_ppzkpcd_online_verifier<PCD_ppT>(pvk, pcd_verifier_input, tree_proofs[cur_idx]);
                            assert(ans == ans2);

                            all_accept = all_accept && ans;

                            printf("\n");
                            for (std::size_t i = 0; i < arity; ++i) {
                                printf("Message %zu was:\n", i);
                                msgs[i]->print();
                            }
                            printf("Summand at this node:\n%zu\n", tree_elems[cur_idx]);
                            printf("Outgoing message is:\n");
                            tree_messages[cur_idx]->print();
                            printf("\n");
                            printf("Current node = %zu. Current proof verifies = %s\n", cur_idx, ans ? "YES" : "NO");
                            printf(
                                "\n\n\n "
                                "================================================================================"
                                "\n\n\n");
                        }
                    }

                    return all_accept;
                }

            }    // namespace snark
        }        // namespace zk
    }            // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_RUN_R1CS_SP_PPZKPCD_HPP
