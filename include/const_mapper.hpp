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
#include <stdexcept>
#include <tuple>
#include <type_traits>

namespace {
// template magic for get index of tuple types
template <typename T, typename Tuple>
struct tuple_index;

template <typename T, typename... Types>
struct tuple_index<T, std::tuple<T, Types...>> {
  static constexpr std::size_t value = 0;
};

template <typename T, typename U, typename... Types>
struct tuple_index<T, std::tuple<U, Types...>> {
  static constexpr std::size_t value =
      1 + tuple_index<T, std::tuple<Types...>>::value;
};

template <typename T, typename... Types>
inline constexpr auto tuple_index_v = tuple_index<T, Types...>::value;

}  // namespace

namespace const_mapper {
template <std::size_t N, class... Args>
class ConstMapper {
  using Tuple = std::tuple<Args...>;

  template <std::size_t N_value>
  using Type = typename std::tuple_element<N_value, Tuple>::type;

 public:
  explicit constexpr ConstMapper(std::array<Tuple, N> list) : map_data_(list) {}

  template <std::size_t i_to, std::size_t i_from>
  constexpr Type<i_to> to(const Type<i_from> &key) const {
    for (const auto &tuple : map_data_) {
      if (std::get<i_from>(tuple) == key) {
        return std::get<i_to>(tuple);
      }
    }
    throw std::out_of_range("key not found.");
  }

  template <class To, class From>
  constexpr To to(const From &key) const {
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
