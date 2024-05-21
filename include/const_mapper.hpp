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

#include <array>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <type_traits>

namespace const_mapper {
template <class T>
struct Anyable;
}
namespace {

template <class Tuple, class T, std::size_t index = 0>
inline constexpr std::size_t tuple_index() {
  static_assert(index != std::tuple_size_v<Tuple>, "Tuple does not contain T.");

  if constexpr (std::is_same_v<T, std::tuple_element_t<index, Tuple>>) {
    return index;
  } else {
    return tuple_index<Tuple, T, index + 1>();
  }
}

template <class Tuple, class T>
inline constexpr auto tuple_index_v = tuple_index<Tuple, T>();

}  // namespace

namespace const_mapper {
template <class T>
struct Anyable {
  using type = T;

  constexpr Anyable(){};
  constexpr Anyable(T value) : value(value) {}

  constexpr bool operator==(const Anyable<T> &rhs) const {
    return (value && rhs.value) ? (value == rhs.value) : true;
  }

  constexpr bool operator==(const T &rhs) const {
    return (value) ? (value == rhs) : true;
  }

  std::optional<T> value;
};

template <std::size_t N, class... Args>
class ConstMapper {
  using Tuple = std::tuple<Args...>;

  template <std::size_t N_value>
  using Type = typename std::tuple_element<N_value, Tuple>::type;

 public:
  explicit constexpr ConstMapper(std::array<Tuple, N> list) : map_data_(list) {}

  template <std::size_t i_to, std::size_t i_from, class Key = Type<i_from>>
  constexpr Type<i_to> to(const Key &key) const {
    for (const auto &tuple : map_data_) {
      if (std::get<i_from>(tuple) == key) {
        return std::get<i_to>(tuple);
      }
    }
    throw std::out_of_range("key not found.");
  }

  template <class To, class From, class Key = From>
  constexpr To to(const Key &key) const {
    constexpr auto i_to = tuple_index_v<To, Tuple>;
    constexpr auto i_from = tuple_index_v<From, Tuple>;
    return to<i_to, i_from>(key);
  }

  template <class To, class... Keys>
  constexpr To pattern_to(const Keys &...keys) const {
    for (const auto &tuple : map_data_) {
      if (check_pattern(tuple, keys...)) {
        return std::get<To>(tuple);
      }
    }
    throw std::out_of_range("key not found.");
  }

 private:
  std::array<Tuple, N> map_data_;

  template <class Key, class... Keys>
  constexpr bool check_pattern(const Tuple &tuple, const Key &key,
                               const Keys &...keys) const {
    return (std::get<Key>(tuple) == key) && check_pattern(tuple, keys...);
  }

  template <class Key>
  constexpr bool check_pattern(const Tuple &tuple, const Key &key) const {
    return (std::get<Key>(tuple) == key);
  }
};
}  // namespace const_mapper
