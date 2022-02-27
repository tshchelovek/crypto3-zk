//---------------------------------------------------------------------------//
// Copyright (c) 2020-2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2020-2021 Nikita Kaskov <nbering@nil.foundation>
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
// @file Declaration of interfaces for:
// - a variable (i.e., x_i),
// - a linear term (i.e., a_i * x_i), and
// - a linear combination (i.e., sum_i a_i * x_i).
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_ZK_PLONK_VARIABLE_HPP
#define CRYPTO3_ZK_PLONK_VARIABLE_HPP

#include <vector>

namespace nil {
    namespace crypto3 {
        namespace zk {
            namespace snark {

                /********************************* Variable **********************************/

                /**
                 * Forward declaration.
                 */
                template<typename VariableType>
                struct non_linear_term;

                /**
                 * Forward declaration.
                 */
                template<typename VariableType>
                struct non_linear_combination;

                /**
                 * A variable represents a formal expression of the form "w^{index}_{rotation}" and type type.
                 */
                template<typename FieldType>
                class plonk_variable {

                public:
                    using assignment_type = typename FieldType::value_type;

                    /**
                     * Mnemonic typedefs.
                     */
                    enum rotation_type { pre_previous = -2, previous, current, next, after_next } rotation;
                    enum column_type { witness, selector, public_input, constant } type;
                    std::size_t index;

                    constexpr plonk_variable(const std::size_t index, rotation_type rotation, column_type type) :
                        index(index), rotation(rotation), type(type) {};

                    non_linear_term<plonk_variable<FieldType>> operator^(const std::size_t power) const {
                        return non_linear_term<plonk_variable<FieldType>>(*this) ^ power;
                    }

                    non_linear_term<plonk_variable<FieldType>> operator*(const assignment_type &field_coeff) const {
                        return non_linear_term<plonk_variable<FieldType>>(*this) * field_coeff;
                    }

                    non_linear_term<plonk_variable<FieldType>> operator*(const plonk_variable &other) const {
                        return non_linear_term<plonk_variable<FieldType>>(*this) * other;
                    }

                    non_linear_combination<plonk_variable<FieldType>>
                        operator+(const non_linear_combination<plonk_variable<FieldType>> &other) const {
                        non_linear_combination<plonk_variable<FieldType>> result(other);

                        result.add_term(*this);

                        return result;
                    }

                    non_linear_combination<plonk_variable<FieldType>>
                        operator-(const non_linear_combination<plonk_variable<FieldType>> &other) const {
                        return (*this) + (-other);
                    }

                    non_linear_combination<plonk_variable<FieldType>>
                        operator-(const assignment_type &field_val) const {
                        return (*this) - non_linear_combination<plonk_variable<FieldType>>(field_val);
                    }

                    non_linear_term<plonk_variable<FieldType>> operator-() const {
                        return non_linear_term<plonk_variable<FieldType>>(*this) * (-assignment_type::one());
                    }

                    bool operator==(const plonk_variable &other) const {
                        return ((this->index == other.index) && (this->rotation == other.rotation));
                    }

                    bool operator<(const plonk_variable &other) const {
                        return ((this->index < other.index) ||
                                ((this->index == other.index) && (this->rotation < other.rotation)));
                    }
                };

                template<typename FieldType>
                non_linear_term<plonk_variable<FieldType>> operator*(const typename FieldType::value_type &field_coeff,
                                                                     const plonk_variable<FieldType> &var) {
                    return var * field_coeff;
                }

                template<typename FieldType>
                non_linear_combination<plonk_variable<FieldType>>
                    operator+(const typename FieldType::value_type &field_val, const plonk_variable<FieldType> &var) {
                    return var + field_val;
                }

                template<typename FieldType>
                non_linear_combination<plonk_variable<FieldType>>
                    operator-(const typename FieldType::value_type &field_val, const plonk_variable<FieldType> &var) {
                    return (-var) + field_val;
                }

            }    // namespace snark
        }        // namespace zk
    }            // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_ZK_PLONK_VARIABLE_HPP