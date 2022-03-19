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

#define BOOST_TEST_MODULE pedersen_test

#include <vector>
#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <nil/crypto3/algebra/curves/bls12.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/bls12.hpp>
#include <nil/crypto3/algebra/random_element.hpp>
#include <nil/crypto3/algebra/fields/bls12/base_field.hpp>
#include <nil/crypto3/algebra/fields/bls12/scalar_field.hpp>
#include <nil/crypto3/algebra/curves/params/multiexp/bls12.hpp>
#include <nil/crypto3/algebra/multiexp/multiexp.hpp>
#include <nil/crypto3/algebra/multiexp/policies.hpp>

#include <nil/crypto3/math/polynomial/polynomial.hpp>

#include <nil/crypto3/zk/commitments/polynomial/pedersen.hpp>

using namespace nil::crypto3;

BOOST_AUTO_TEST_SUITE(pedersen_test_suite)

BOOST_AUTO_TEST_CASE(pedersen_basic_test) {

    // setup
    using curve_type = algebra::curves::bls12<381>;
    using curve_group_type = curve_type::template g1_type<>;
    using field_type = typename curve_type::scalar_field_type;
    typedef typename algebra::policies::multiexp_method_BDLO12 multiexp_type;

    constexpr static const std::size_t n = 2;
    constexpr static const std::size_t k = 1;
    static curve_group_type::value_type g = algebra::random_element<curve_group_type>();
    static curve_group_type::value_type h = algebra::random_element<curve_group_type>();
    while (g == h) {
        h = algebra::random_element<curve_group_type>();
    }
    // std::cout << "g and h: " << g.X.data << ' ' << g.Y.data << ' ' << g.Z.data << ", " << h.X.data << ' ' << h.Y.data << ' ' << h.Z.data << '\n';

    typedef typename zk::commitments::pedersen<curve_type, multiexp_type> pedersen_type;

    typedef typename pedersen_type::proof_type proof_type;
    typedef typename pedersen_type::params_type params_type;

    params_type params;

    params.n = n;
    params.k = k;
    params.g = g;
    params.h = h;

    BOOST_CHECK(g != h);
    BOOST_CHECK(n >= k);
    BOOST_CHECK(k > 0);

    // commit
    constexpr static const field_type::value_type w = field_type::value_type(5);
    // std::cout << "secret message: " << w.data << ' ' << (g + g + g + g + g == w * g) << '\n';

    // eval
    proof_type proof = pedersen_type::proof_eval(params, w);
    // std::cout << "commitment value: " << proof.E_0.X.data << ' ' << proof.E_0.Y.data << ' ' << proof.E_0.Z.data << '\n';

    // verify
    // BOOST_CHECK(proof.E_0 == params.g * proof.pk[0].s + params.h * proof.pk[0].t);
    // BOOST_CHECK(proof.pk[0].s == w);
    BOOST_CHECK(pedersen_type::verify_eval(params, proof));
    
    std::vector<std::size_t> idx = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    BOOST_CHECK(idx.size() >= k);
    BOOST_CHECK(w == pedersen_type::message_eval(params, proof, idx));
}

BOOST_AUTO_TEST_SUITE_END()
