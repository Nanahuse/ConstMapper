/**
 * MIT License
 *
 * Copyright (c) 2024 Nanahuse
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once

#include <array>
#include <stdexcept>
#include <tuple>
#include <type_traits>

namespace const_mapper {
template <std::size_t N, class... Args>
class ConstMapper {
  template <std::size_t N_value>
  using Type = typename std::tuple_element<N_value, std::tuple<Args...> >::type;

 public:
  explicit constexpr ConstMapper(std::array<std::tuple<Args...>, N> list)
      : map_data_(list) {}

  template <class To, class From>
  constexpr To to(const From &key) const {
    for (const auto &tuple : map_data_) {
      if (std::get<From>(tuple) == key) {
        return std::get<To>(tuple);
      }
    }
    throw std::out_of_range("key not found.");
  }

  template <std::size_t n_to, std::size_t n_from>
  constexpr Type<n_to> to(const Type<n_from> &key) const {
    for (const auto &tuple : map_data_) {
      if (std::get<n_from>(tuple) == key) {
        return std::get<n_to>(tuple);
      }
    }
    throw std::out_of_range("key not found.");
  }

 private:
  std::array<std::tuple<Args...>, N> map_data_;
};
}  // namespace const_mapper
