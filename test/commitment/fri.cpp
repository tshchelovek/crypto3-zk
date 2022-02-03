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

#define BOOST_TEST_MODULE fri_test

#include <string>

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <nil/crypto3/algebra/curves/mnt4.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/mnt4.hpp>
#include <nil/crypto3/algebra/random_element.hpp>

#include <nil/crypto3/math/polynomial/polynomial.hpp>
#include <nil/crypto3/math/polynomial/lagrange_interpolation.hpp>
#include <nil/crypto3/math/algorithms/unity_root.hpp>
#include <nil/crypto3/math/domains/evaluation_domain.hpp>
#include <nil/crypto3/math/algorithms/make_evaluation_domain.hpp>

#include <nil/crypto3/merkle/tree.hpp> // until fri inclusion

#include <nil/crypto3/zk/snark/transcript/fiat_shamir.hpp>
#include <nil/crypto3/zk/snark/commitments/fri_commitment.hpp>

using namespace nil::crypto3;

template<typename FieldType> 
std::shared_ptr<math::evaluation_domain<FieldType>> prepare_domain(const std::size_t d) {
    return math::make_evaluation_domain<FieldType>(d);                        
}

BOOST_AUTO_TEST_SUITE(fri_test_suite)

BOOST_AUTO_TEST_CASE(fri_basic_test) {

    // fri params
    using curve_type = algebra::curves::mnt4<298>;
    using FieldType = typename curve_type::base_field_type;

    typedef hashes::sha2<256> merkle_hash_type;
    typedef hashes::sha2<256> transcript_hash_type;

    typedef typename containers::merkle_tree<merkle_hash_type, 2> merkle_tree_type;

    constexpr static const std::size_t d = 16;

    constexpr static const std::size_t r = boost::static_log2<d>::value;
    constexpr static const std::size_t m = 2;

    typedef zk::snark::fri_commitment_scheme<FieldType, merkle_hash_type, m> fri_type;
    typedef typename fri_type::proof_type proof_type;
    typedef typename fri_type::params_type params_type;

    params_type params;
    math::polynomial::polynomial<typename FieldType::value_type> f = {1, 3, 4, 1,
                                                                        5, 6, 7, 2,
                                                                        8, 7, 5, 6,
                                                                        1, 2, 1, 1};

    // create domain D_0

    std::vector<std::shared_ptr<math::evaluation_domain<FieldType>>> D;
    constexpr static const std::size_t d_extended = d * 16;
    std::size_t extended_log = boost::static_log2<d_extended>::value;
    for (std::size_t i = 0; i < r; i++) {
        std::shared_ptr<math::evaluation_domain<FieldType>> domain = prepare_domain<FieldType>(d - i);
        D.push_back(domain);
    }

    params.r = r + 1;
    params.D = D;
    params.q = f;

    merkle_tree_type commit_merkle = fri_type::commit(f, D[0]);
    // std::array<typename FieldType::value_type, 1> evaluation_points = {omega.pow(5)};

    //proof_type proof = fri_type::proof_eval(f, f, T, transcript, params);
    // BOOST_CHECK(fry_type::verify_eval(evaluation_points, T, proof, D_0))
}

BOOST_AUTO_TEST_CASE(fri_fold_test) {

    // fri params
    using curve_type = algebra::curves::mnt4<298>;
    using FieldType = typename curve_type::base_field_type;

    typedef hashes::sha2<256> merkle_hash_type;
    typedef hashes::sha2<256> transcript_hash_type;

    typedef typename containers::merkle_tree<merkle_hash_type, 2> merkle_tree_type;

    constexpr static const std::size_t d = 4;
    constexpr static const std::size_t r = boost::static_log2<d>::value;
    constexpr static const std::size_t m = 2;

    typedef zk::snark::fri_commitment_scheme<FieldType, merkle_hash_type, m> fri_type;
    typedef typename fri_type::proof_type proof_type;
    typedef typename fri_type::params_type params_type;

    params_type params;
    math::polynomial::polynomial<typename FieldType::value_type> q = {0, 0, 1};

    std::shared_ptr<math::evaluation_domain<FieldType>> domain = prepare_domain<FieldType>(d);
    std::vector<std::shared_ptr<math::evaluation_domain<FieldType>>> D = {domain};


    params.r = r;
    params.D = D;
    params.q = q;

    math::polynomial::polynomial<typename FieldType::value_type> f = {1, 3, 4, 3};

    typename FieldType::value_type omega = domain->get_domain_element(0);

    typename FieldType::value_type x_next = params.q.evaluate(omega);
    typename FieldType::value_type alpha = algebra::random_element<FieldType>();
    math::polynomial::polynomial<typename FieldType::value_type> f_next = 
        fri_type::fold_polynomial(f, alpha);

    std::vector<std::pair<typename FieldType::value_type, typename FieldType::value_type>> points {
        std::make_pair(omega, f.evaluate(omega)),
        std::make_pair(-omega, f.evaluate(-omega)),
    };

    // TODO: Fix it with a proper interpolation
    math::polynomial::polynomial<typename FieldType::value_type> interpolant = {f[0] + f[2] * x_next, f[1] + f[3] * x_next};
    typename FieldType::value_type x1 = interpolant.evaluate(alpha);
    typename FieldType::value_type x2 = f_next.evaluate(x_next);
    BOOST_CHECK(x1 == x2);
    // BOOST_CHECK_EQUAL(interpolant.eval(alpha), f_next.eval(x_next))
}

BOOST_AUTO_TEST_CASE(fri_coset_test) {

    // fri params
    using curve_type = algebra::curves::mnt4<298>;
    using FieldType = typename curve_type::base_field_type;

    // typedef fri_commitment_scheme<FieldType, merkle_hash_type, lambda, k, r, m> fri_type;

    math::polynomial::polynomial<typename FieldType::value_type> f = {1, 3, 4, 25};

    // create domain D_0
    // Get omega from D_0

    // delegate q = fry_type::q()
    // x_next = q(omega)

    // vector s = fri_type::get_coset(x_next)
    //for (std::size_t i = 0; i < s.size(); i++) {
    //    BOOST_CHECK_EQUAL(x_next, s[i]^2);
    //}
}

BOOST_AUTO_TEST_CASE(fri_steps_count_test) {

    // fri params
    using curve_type = algebra::curves::mnt4<298>;
    using FieldType = typename curve_type::base_field_type;

    typedef hashes::sha2<256> merkle_hash_type;
    typedef hashes::sha2<256> transcript_hash_type;

    typedef typename containers::merkle_tree<merkle_hash_type, 2> merkle_tree_type;

    constexpr static const std::size_t d = 16;

    constexpr static const std::size_t r = boost::static_log2<d>::value;
    constexpr static const std::size_t m = 2;

    typedef zk::snark::fri_commitment_scheme<FieldType, merkle_hash_type, m> fri_type;
    typedef typename fri_type::proof_type proof_type;
    typedef typename fri_type::params_type params_type;

    params_type params;
    math::polynomial::polynomial<typename FieldType::value_type> f = {1, 3, 4, 1,
                                                                        5, 6, 7, 2,
                                                                        8, 7, 5, 6,
                                                                        1, 2, 1, 1};

    // create domain D_0

    std::vector<std::shared_ptr<math::evaluation_domain<FieldType>>> D;
    constexpr static const std::size_t d_extended = d * 16;
    std::size_t extended_log = boost::static_log2<d_extended>::value;
    for (std::size_t i = 0; i < r; i++) {
        std::shared_ptr<math::evaluation_domain<FieldType>> domain = prepare_domain<FieldType>(d - i);
        D.push_back(domain);
    }

    params.r = r + 1;
    params.D = D;
    params.q = f;

    merkle_tree_type commit_merkle = fri_type::commit(f, D[0]);

    // std::array<typename FieldType::value_type, 1> evaluation_points = {omega.pow(5)};

    //proof_type proof = fri_type::proof_eval(f, f, T, transcript, params);

    // proof_type proof = fri_type::proof_eval(evaluation_points, T, f, D_0)
    // math::polynomial::polynomial<typename FieldType::value_type> f_res = proof.last_round.f

    // int expected_deg = def(f) - 2^r

    // BOOST_CHECK_EQUAL(f_res.deg(), expected_deg)
}

BOOST_AUTO_TEST_SUITE_END()